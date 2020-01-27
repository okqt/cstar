// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_INLINER_HPP__
#include "ato_inliner.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

hFInt32 ATOInliner::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATOInliner::AnatropDo(IRFunction*)", func_);
  hFUInt32 retVal(0);
  //! \todo H Design: Should we add an option for number of parameters?
  if (func_->GetInlineFlag() == IFNotDecided) {
    if (pred.pMayDirectRecurse(func_) == true) {
      func_->SetInlineFlag(IFNotToBeInlined);
    } else if (pred.pIsStructUnion(extr.eGetRetType(func_)) == true) {
      func_->SetInlineFlag(IFNotToBeInlined);
    } else if (pred.pHasAnyLoop(func_) == true) {
      func_->SetInlineFlag(IFNotToBeInlined);
    } else {
      StmtCollector2<IRSSwitch, IRSDynJump, IRFunction> lNotHandledStmts(func_);
      if (!lNotHandledStmts.empty()) {
        func_->SetInlineFlag(IFNotToBeInlined);
      } else {
        func_->SetInlineFlag(IFToBeInlined);
      } // if
    } // if
    if (func_->GetInlineFlag() != IFNotDecided) {
      ++ retVal;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOInliner::AnatropDo

hFInt32 ATOInliner::
AnatropDo(IRModule* module_) {
  BMDEBUG2("ATOInliner::AnatropDo(IRModule*)", module_);
  hFUInt32 retVal(0);
  // First mark all functions
  ListIterator<IRFunction*> lFuncIter(module_->GetFuncIter());
  for (lFuncIter.First(); !lFuncIter.IsDone(); lFuncIter.Next()) {
    retVal += AnatropDo(*lFuncIter);
  } // for
  // Then inline the calls to the to be inlined functions.
  for (lFuncIter.First(); !lFuncIter.IsDone(); lFuncIter.Next()) {
    retVal += inlineCallsOf(*lFuncIter);
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOInliner::AnatropDo

hFInt32 ATOInliner::
inlineCallsOf(IRFunction* func_) {
  BMDEBUG2("ATOInliner::inlineCallsOf", func_);
  hFUInt32 retVal(0);
  StmtCollector<IRSCall, IRFunction> lCallStmts(func_);
  for (hFUInt32 c(0); c < lCallStmts.size(); ++ c) {
    if (pred.pIsDirect(lCallStmts[c]) == true) {
      IRFunction* lCalledFunc(extr.eGetCalledFunc(lCallStmts[c]));
      if (lCalledFunc != 0 && lCalledFunc->GetInlineFlag() == IFToBeInlined) {
        if (extr.eGetNumOfArgs(lCallStmts[c]) == extr.eGetNumOfParams(lCalledFunc)) {
          inlineCall(lCallStmts[c], lCalledFunc);
          STATRECORD("inliner: call inlined");
        } // if
      } // if
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOInliner::inlineCallsOf

//! \todo M Design: If any inline happened we can reset the flags as
//!       undecided since we may have created more opportunities?
void ATOInliner::
inlineCall(IRSCall* call_, IRFunction* func_) {
  BMDEBUG3("ATOInliner::inlineCall", call_, func_);
  // We need to handle:
  // - Creation of locals for func_ params, locals, return value if present.
  // - Create same statements of func_ with replaced local and param objects,
  //   CFG must be up-to-date after statement copying.
  map<IRObject*, IRObject*> lObjMap;
  IRBuilder lIRBuilder(extr.eGetFunc(call_), consts.cIRBNoOptims);
  IRBuilderHelper& lIRHBuilder(*lIRBuilder.GetHelper());
  { // First create the object map, from func_ objects to the newly created objects.
    { // Process params
      const vector<IROParameter*>& lParams(extr.eGetParams(func_));
      for (hFUInt32 c(0); c < lParams.size(); ++ c) {
        lObjMap[lParams[c]] = lIRBuilder.irboTmp(lParams[c]->GetType(), "inlprm");
      } // for
    } // block
    { // Process locals
      const vector<IROLocal*> lLocals(extr.eGetLocals(func_));
      for (hFUInt32 c(0); c < lLocals.size(); ++ c) {
        lObjMap[lLocals[c]] = lIRBuilder.irboTmp(lLocals[c]->GetType(), "inllcl");
      } // for
    } // block
  } // block
  { // In order to copy the cfg and statements, first copy all non-cf
    // statements in to a new block then insert the cf statements.
    lIRBuilder.irbSplitBB(ICStmt(call_, ICBefore));
    IRBB* lNewBB(lIRBuilder.irbInsertBB(ICBB(call_->GetBB(), ICBefore)));
    IRObject* lRetValObj(0);
    // Create return value object. 
    if (pred.pIsFCall(call_) == true) {
      lRetValObj = lIRBuilder.irboTmp(extr.eGetRetType(func_), "inlrtv");
      /* LIE */ lIRHBuilder.irbsAssign(call_->GetReturnIn(), lIRHBuilder.irbeUseOf(lRetValObj), 
        ICStmt(call_, ICAfter), extr.eGetSrcLoc(call_));
    } // if
    PDEBUG(GetName(), "detail", " copying arguments to param objects: ");
    { // Copy arguments to param objects.
      const vector<IROParameter*>& lParams(extr.eGetParams(func_));
      vector<IRExpr*> lArgs;
      call_->GetArgs()->GetChildren(lArgs);
      for (hFUInt32 c(0); c < lArgs.size(); ++ c) {
        /* LIE */ lIRHBuilder.irbsAssign(lObjMap[lParams[c]], 
          lIRBuilder.irbeCast(lParams[c]->GetType(), lArgs[c]->GetRecCopyExpr()), 
          ICBB(lNewBB, ICBegOfBB), extr.eGetSrcLoc(call_));
      } // for
    } // block
    ListIterator<IRStmt*> lStmtIter(extr.eGetStmtIter(func_));
    ICStmt lIC(lNewBB->GetLastStmt(), ICBefore);
    // Control flow insert contexts, from ics to func_ cf statements
    map<ICStmt*, IRStmt*> lCFICPoints;
    // Keep the mapping from old BB to new stmts that marks the begining of
    // corresponding new BBs.
    map<IRBB*, IRStmt*> lOldBB2NewStmt;
    PDEBUG(GetName(), "detail", " insertion of non-control flow statements: ");
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      IRStmt* lNewStmt(0);
      PDEBUG(GetName(), "detail", " non-cf stmt= " << progcxt(*lStmtIter));
      if (IRSReturn* lRetStmt = dynamic_cast<IRSReturn*>(*lStmtIter)) {
        if (pred.pIsFCall(call_) == true) {
          lNewStmt = lIRHBuilder.irbsAssign(lRetValObj, 
            lIRBuilder.irbeCast(lRetValObj->GetType(), lRetStmt->GetExpr()->GetRecCopyExpr()), 
              lIC, extr.eGetSrcLoc(lRetStmt));
          lIRBuilder.irbRemapObjs(lNewStmt, lObjMap);
        } else {
          lNewStmt = lIRBuilder.irbsNull(lIC, SrcLoc());
          lIRBuilder.irbToBeRemoved(lNewStmt);
        } // if
        IRSJump* lJump(lIRBuilder.irbsJump(func_->GetCFG()->GetExitBB(), ICNull(), SrcLoc()));
        lCFICPoints[new ICStmt(lNewStmt, ICAfter)] = lJump;
        // Note that there is no need to mark the jump as to be removed, it has no impact on ir.
      } else if (pred.pIsPrologOrEpilog(*lStmtIter) == true) {
        lNewStmt = lIRBuilder.irbsNull(lIC, SrcLoc());
        lIRBuilder.irbToBeRemoved(lNewStmt);
      } else if (pred.pIsCFStmt(*lStmtIter) == true) {
        lNewStmt = lIRBuilder.irbsNull(lIC, SrcLoc());
        lIRBuilder.irbToBeRemoved(lNewStmt);
        // Record the cf stmts for later insertion.
        lCFICPoints[new ICStmt(lNewStmt, ICAfter)] = *lStmtIter;
      } else {
        lNewStmt = lStmtIter->GetRecCopyStmt();
        lIRBuilder.irbInsertStmt(lNewStmt, lIC);
        lIRBuilder.irbRemapObjs(lNewStmt, lObjMap);
      } // if
      if (pred.pIsFirstStmt(*lStmtIter) == true) {
        // Record the CF targets.
        lOldBB2NewStmt[lStmtIter->GetBB()] = lNewStmt;
      } // if
    } // for
    PDEBUG(GetName(), "detail-insertcfs", " insertion of control flow statements: ");
    { // Insert the cf statements.
      map<ICStmt*, IRStmt*>::iterator lIt(lCFICPoints.begin());
      while (lIt != lCFICPoints.end()) {
        IRStmt* lNewStmt(0);
        if (IRSIf* lIf = dynamic_cast<IRSIf*>(lIt->second)) {
          IRStmt* lTrueCase(lOldBB2NewStmt[lIf->GetTrueCaseBB()]);
          IRStmt* lFalseCase(lOldBB2NewStmt[lIf->GetFalseCaseBB()]);
          lNewStmt = lIRHBuilder.irbsIf(lIf->GetExpr()->GetRecCopyExpr(), lTrueCase, lFalseCase, *lIt->first, extr.eGetSrcLoc(lIf));
          lIRBuilder.irbRemapObjs(lNewStmt, lObjMap);
        } else if (IRSJump* lJump = dynamic_cast<IRSJump*>(lIt->second)) {
          IRStmt* lTarget(lOldBB2NewStmt[lJump->GetTargetBB()]);
          lNewStmt = lIRHBuilder.irbsJump(lTarget, *lIt->first, extr.eGetSrcLoc(lJump));
        } else {
          ASSERTMSG(0, ieStrPCShouldNotReachHere << 
            " unhandled control flow statement type, function should not have been marked as to be inlined");
        } // if
        ++ lIt;
      } // while
    } // block
  } // block
  // Finally remove the call.
  lIRBuilder.irbRemoveStmt(call_);
  // Remove the null statements inserted during the process.
  lIRBuilder.irbRemoveToBeRemovedStmts();
  EMDEBUG0();
  return;
} // ATOInliner::inlineCall


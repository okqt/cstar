// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_LOOPINVARIANT_HPP__
#include "ato_loopinvariant.hpp"
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
#ifndef __DFA_HPP__
#include "dfa.hpp"
#endif

void ATOLoopInvariant::
InitOptions() {
  BMDEBUG1("ATOLoopInvariant::InitOptions");
  addBoolOption("only_analyse", consts.cDefaultIsFalse);
  EMDEBUG0();
  return;
} // ATOLoopInvariant::InitOptions

hFInt32 ATOLoopInvariant::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATOLoopInvariant::AnatropDo(IRFunction*)", func_);
  hFInt32 retVal(0);
  //! \todo M Design: Can we make use of new OnStart here?
  mIRBuilder = new IRBuilder(func_, consts.cIRBDoOptims);
  mIRHBuilder = mIRBuilder->GetHelper();
  mGrpNo2Expr.clear();
  const vector<IRLoop*>& lLoops(extr.eGetLoops(func_->GetCFG()));
  for (hFUInt32 c(0); c < lLoops.size(); ++ c) {
    retVal += AnatropDo(lLoops[c]);
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOLoopInvariant::AnatropDo

void ATOLoopInvariant::
OnOptionsSet() {
  BMDEBUG1("ATOLoopInvariant::OnOptionsSet");
  mOnlyAnalyse = GetOptions().obGet("only_analyse");
  EMDEBUG0();
  return;
} // ATOLoopInvariant::OnOptionsSet

hFInt32 ATOLoopInvariant::
AnatropDo(IRLoop* loop_) {
  BMDEBUG2("ATOLoopInvariant::AnatropDo(IRLoop*)", loop_);
  hFInt32 retVal(0);
  CFG* lCFG(loop_->GetCFG());
  // Collect address of expressions which makes it easy to reach nodes of loop invariant expressions.
  // Note that expression collector will mark the constant and address of expressions.
  //! \todo H Design: Do forward substitution on the loop bbs.
  // Note that there is no need to call markExprTree for the collected expressions.
  ExprCollector2<IREConst, IREAddrOf, IRLoop> lExprCollector(EVOTPost, loop_, ExprVisitorOptions().SetMark2False(eIRTMLoopInvariant).SetLHSNo());
  if (!lExprCollector.empty()) {
    mDUUDChains = loop_->GetCFG()->GetDUUDChains();
  } // if
  for (hFUInt32 d(0); d < lExprCollector.mCollected1.size(); ++ d) {
    IREAddrOf* lAddrOf(lExprCollector.mCollected1[d]);
    IRObject* lObj(extr.eGetObjOfAddrOf(lAddrOf));
    // Address of objects are loop invariants.
    if (!lObj) {
      lAddrOf->ResetMark(eIRTMLoopInvariant);
    } else {
      // Check the use of this object, if all the definitions are outside the
      // loop mark them also as loop invariant.
      if (IREDeref* lUseOfObj = dynamic_cast<IREDeref*>(extr.eGetParentExpr(lAddrOf))) {
        bool lDefInLoopBody(false);
        const set<IRStmt*>& lDefsOfObj(extr.eGetAllDefsOfObj(lCFG, lObj));
        for (set<IRStmt*>::const_iterator lIt(lDefsOfObj.begin()); lIt != lDefsOfObj.end(); ++ lIt) {
          IRStmt* lStmt(*lIt);
          if (loop_->IsInLoopBody(lStmt->GetBB())) {
            lDefInLoopBody = true;
            break;
          } // if
        } // for
        if (!lDefInLoopBody) {
          markExprTree(lUseOfObj, loop_);
        } // if
      } // if
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOLoopInvariant::AnatropDo

void ATOLoopInvariant::
markExprTree(IRExpr* expr_, IRLoop* loop_) {
  BMDEBUG3("ATOLoopInvariant::markExprTree", expr_, loop_);
  bool lExprHandled(false);
  if (mOnlyAnalyse == false && pred.pIsParentExpr(expr_) == true && expr_->GetParentExpr()->GetAssocGroupNo() != 0) {
    map<hFUInt32, IRExpr*>::iterator lPrevExpr(mGrpNo2Expr.find(expr_->GetParentExpr()->GetAssocGroupNo()));
    if (lPrevExpr != mGrpNo2Expr.end()) {
      // Move the expressions out of the loop.
      ASSERTMSG(pred.pIsBinary(expr_->GetParentExpr()), ieStrInconsistentInternalStructure);
      IRExpr* lTmpExpr(mIRBuilder->irbeFromBinary(static_cast<IREBinary*>(expr_->GetParentExpr()), 
        expr_->GetRecCopyExpr(), lPrevExpr->second->GetRecCopyExpr()));
      IROTmp* lTmp(mIRHBuilder->irbNewDef("licm", lTmpExpr, ICStmt(extr.eGetPreHeader(loop_)->GetLastStmt(), ICBefore), SrcLoc()));
      // Note that identity is not marked as loop invariant.
      expr_->ReplaceWith(mIRBuilder->irbeIdentity(expr_->GetParentExpr()));
      lPrevExpr->second->ReplaceWith(mIRHBuilder->irbeUseOf(lTmp))->SetMark(eIRTMLoopInvariant);
      lPrevExpr->second->ResetMark(eIRTMLoopInvariant);
      mGrpNo2Expr.erase(lPrevExpr);
      lExprHandled = true;
      STATRECORD("loopinvariant: expr moved");
    } // if
    if (lExprHandled == false) {
      mGrpNo2Expr[expr_->GetParentExpr()->GetAssocGroupNo()] = expr_;
    } // if
  } // if
  if (lExprHandled == true) {
    /* LIE */
  } else if (!pred.pIsParentExpr(expr_)) {
    expr_->SetMark(eIRTMLoopInvariant);
    // The whole statement might be loop invariant. Only the assign and eval
    // is important. If statement may also be important if we do the loop
    // unswitching.
    //! \todo M Design: Check loop unswitching and this case.
    IRStmt* lStmt(expr_->GetParentStmt());
    //! \todo M Design: Consider the eval statements also.
    if (IRSAssign* lAssign = dynamic_cast<IRSAssign*>(lStmt)) {
      // We can move the assignment out of loop if it is the only definition
      // of an object and this definition comes before all uses of it in the loop.
      set<IRObject*> lDefs;
      extr.eGetDefsOfStmt(lAssign, lDefs);
      //! \todo M Design: create a loop guard or do more analysis for moving
      //!       globals out of the loop.
      if (lDefs.size() == 1 && !pred.pIsGlobal(*lDefs.begin())) {
        IRObject* lObj(*lDefs.begin());
        const set<IRStmt*>& lDefPoints(extr.eGetAllDefsOfObj(mDUUDChains, lObj));
        bool lNoUseBeforeDef(true);
        if (lDefPoints.size() == 1) {
          // We need to check if this definition precedes all uses.
          set<IRBB*> lPredSet;
          extr.eGetPredsSet(lAssign->GetBB(), loop_->GetHeader(), lPredSet);
          // Remove current bb, we will do a local check on it.
          lPredSet.erase(lAssign->GetBB());
          const set<IRStmt*>& lUses(extr.eGetAllUsesOfObj(mDUUDChains, lObj));
          if (!lUses.empty()) {
            for (set<IRStmt*>::const_iterator lIt(lUses.begin()); lIt != lUses.end(); ++ lIt) {
              IRStmt* lUseStmt(*lIt);
              if (lPredSet.find(lUseStmt->GetBB()) != lPredSet.end()) {
                lNoUseBeforeDef = false;
                break;
              } // if
            } // for
          } // if
          if (lNoUseBeforeDef == true) {
            // Do a local scan for a use in defs bb.
            for (IRStmt* lCurrStmt(extr.eGetPrevStmt(lAssign)); lCurrStmt != 0; lCurrStmt = extr.eGetPrevStmt(lCurrStmt)) {
              set<IRObject*> lUses;
              extr.eGetUsesOfStmt(lCurrStmt, lUses);
              if (lUses.find(lObj) != lUses.end()) {
                lNoUseBeforeDef = false;
                break;
              } // if
            } // for
          } // if
          if (mOnlyAnalyse == false && lNoUseBeforeDef == true) {
            // lAssign has a single def and there is no use before this, so we
            // can move lAssign to the end of preheader.
            mIRHBuilder->irbMoveStmt(lAssign, ICStmt(extr.eGetPreHeader(loop_)->GetLastStmt(), ICBefore));
            STATRECORD("loopinvariant: stmt moved");
          } // if
        } // if
      } // if
    } // if
  } else {
    if (pred.pIsNegOrNot(expr_->GetParentExpr()) == true) {
      markExprTree(expr_->GetParentExpr(), loop_);
    } else if (IREBinary* lBinExpr = dynamic_cast<IREBinary*>(expr_->GetParentExpr())) {
      // Check the other operand of parent, if the operand is also loop
      // invariant mark the parent as loop invariant.
      IRExpr* lOp(extr.eGetBinOtherOp(expr_));
      if (lOp->HasMarked(eIRTMLoopInvariant) == true && lOp->IsMarkTrue(eIRTMLoopInvariant) == true) {
        // We can move the lBinExpr out of loop, we need to create a temporary.
        // Note that DIV and MOD needs extra care, if b is zero in 'a /% b' 
        // we may not move 'a /% b' out of loop without imposing the loop condition.
        if (mOnlyAnalyse == true) {
          // LIE: do nothing.
        } else if (!pred.pIsModOrDiv(lBinExpr)) {
          STATRECORD("loopinvariant: expr moved");
          IROTmp* lTmp(mIRHBuilder->irbNewDef("licm", lBinExpr->GetRecCopyExpr(), 
            ICStmt(extr.eGetPreHeader(loop_)->GetLastStmt(), ICBefore), SrcLoc()));
          IRExpr* lExpr(lBinExpr->ReplaceWith(mIRHBuilder->irbeUseOf(lTmp)));
          markExprTree(lExpr, loop_);
        } else {
          STATRECORD("loopinvariant: div/mod");
        } // if
      } // if
    } // if
  } // if
  EMDEBUG0();
  return;
} // ATOLoopInvariant::markExprTree



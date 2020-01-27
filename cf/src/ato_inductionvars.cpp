// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_INDUCTIONVARS_HPP__
#include "ato_inductionvars.hpp"
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
#ifndef __TEMPLATE_AVAILEXPR_HPP__
#include "template_availexpr.hpp"
#endif

hFInt32 ATOInductionVars::
AnatropDo(IRLoop* loop_) {
  BMDEBUG2("ATOInductionVars::AnatropDo(IRLoop*)", loop_);
  hFUInt32 retVal(0);
  //! \todo M Design: add a stat when member/subscript is seen, licm is most
  //!       effective when these are lowered.
  // Mark loop invariant expressions and statements.
//  anatropManager->Execute("LFwdSubst", loop_, GenericOptions().obSet("execute_triggers", false));
  anatropManager->Execute("LoopInvariant", loop_, GenericOptions().obSet("only_analyse", true).obSet("execute_triggers", false));
  // Find basic induction variables, i = i +/- const and add them
  // immediately. Also add assignments that can lead to non-basic induction variables.
  if (!loop_->mBasicIndVars) {
    loop_->mBasicIndVars = new vector<IRInductionVar*>;
    loop_->mDependentIVs = new vector<IRInductionVar*>;
  } // if
  ListIterator<IRStmt*> lStmtIter(extr.eGetStmtIter(loop_));
  vector<pair<IRInductionVar*, bool /*stillcandid*/> > lCandidIVs;
  map<IRObject*, IRInductionVar*> obj2IVs;
  for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
    if (pred.pIsObjAssign(*lStmtIter) == true) {
      IRSAssign* lAssign(static_cast<IRSAssign*>(*lStmtIter));
      IRInductionVar* lCandidIV(GetCandidInductionVar(loop_, lAssign));
      if (lCandidIV != 0) {
        if (lCandidIV->IsBasic() == true) {
          loop_->mBasicIndVars->push_back(lCandidIV);
          lCandidIV->mBasisIV = lCandidIV;
          obj2IVs[lCandidIV->GetIVObj()] = lCandidIV;
          PDEBUG(GetName(), "detail", " basic IV is added " << progcxt(lCandidIV));
        } else {
          PDEBUG(GetName(), "detail", " candid IV: " << progcxt(lCandidIV));
          lCandidIVs.push_back(make_pair(lCandidIV, true));
        } // if
      } // if
    } // if
  } // for
  // All basic ivs are added to the loop. Now add the dependent ones.
  // First add the IVs with basic basis IVs.
  for (hFUInt32 c(0); c < lCandidIVs.size(); ++ c) {
    IRInductionVar* lCandidIV(lCandidIVs[c].first);
    map<IRObject*, IRInductionVar*>::iterator lIt(obj2IVs.find(lCandidIV->GetBasisIVObj()));
    if (lIt != obj2IVs.end() && lIt->second->IsBasic() == true) {
      loop_->mDependentIVs->push_back(lCandidIV);
      obj2IVs[lCandidIV->GetIVObj()] = lCandidIV;
      lCandidIVs[c].second = false; // still candid => false
      lCandidIV->mBasisIV = lIt->second->mBasisIV;
      PDEBUG(GetName(), "detail", " dependent IV having basic basis is added " << progcxt(lCandidIV));
    } else {
      PDEBUG(GetName(), "detail", " dependent IV having non basic basis is skipped " << progcxt(lCandidIV));
    } // if
  } // for
  // Process IV candidates that are based on non basic ivs.
  for (hFUInt32 c(0); c < lCandidIVs.size(); ++ c) {
    IRInductionVar* lCandidIV(lCandidIVs[c].first);
    map<IRObject*, IRInductionVar*>::iterator lIt(obj2IVs.find(lCandidIV->GetBasisIVObj()));
    if (lCandidIVs[c].second == true && lIt != obj2IVs.end()) {
      if (lIt->second->mBasisIV == 0) {
        PDEBUG(GetName(), "detail", " dependent IV having non proven IV basis is skipped " << progcxt(lCandidIV));
        continue; // Skip this IV basis it is not yet proven to be IV.
      } // if
      lCandidIVs[c].second = false; // still candid => false
      bool lIVOk(false);
      // For a dependent iv 'k' on a non-basic iv we need two more checks:
      // 'i' is basic iv, j = m*i + c; and assume we catched k = n*j + d =>
      // 1. no def of 'j' outside of the loop reaches 'k',
      // 2. no assignment to 'i' between assignment to 'j' in loop and assignment to 'k' is present.
      { // Case 1
        const set<IRStmt*>& lDefs(extr.eGetDefs(loop_->GetCFG()->GetDUUDChains(), 
              lIt->second->GetBasisIV()->GetDefStmt(), lCandidIV->GetBasisIVObj()));
        for (set<IRStmt*>::iterator lIt(lDefs.begin()); lIt != lDefs.end(); ++ lIt) {
          IRStmt* lStmt(*lIt);
          if (pred.pIsInLoop(loop_, lStmt) == false) {
            PDEBUG(GetName(), "detail", " dependent IV reaching def of basis outside of loop " << progcxt(lCandidIV));
            lIVOk = false;
            break;
          } // if
        } // for
      } // block
      { // Case 2 
        AvailableExpressions<bool> lBasisAE;
        lBasisAE.Add(irhBuilder->irbeUseOf(lIt->second->GetBasisIV()->GetBasisIVObj()), hFSInt32_ignore);
        lIVOk = lIVOk == true && !lBasisAE.pAnyKilled(lIt->second->GetBasisIV()->GetDefStmt(), lCandidIV->GetDefStmt());
      } // block
      if (lIVOk == true) {
        loop_->mDependentIVs->push_back(lCandidIV);
        obj2IVs[lCandidIV->GetIVObj()] = lCandidIV;
        // Now we need to update basis and mul and add factors.
        lCandidIV->mAddFactor = irBuilder->irbeAdd(
            irBuilder->irbeMul(lCandidIV->mMulFactor->GetRecCopyExpr(), lIt->second->mAddFactor->GetRecCopyExpr()), 
            lCandidIV->mAddFactor->GetRecCopyExpr());
        lCandidIV->mMulFactor = irBuilder->irbeMul(lCandidIV->mMulFactor->GetRecCopyExpr(), 
            lIt->second->mMulFactor->GetRecCopyExpr());
        lCandidIV->mBasisIV = lIt->second->mBasisIV;
        lCandidIV->mBasisVar = lIt->second->mBasisVar;
        PDEBUG(GetName(), "detail", " dependent IV is added " << progcxt(lCandidIV));
        c = 0; // Restart the check.
      } else {
        PDEBUG(GetName(), "detail", " dependent IV is not added " << progcxt(lCandidIV));
      } // if
    } // if
  } // for
  retVal = loop_->mDependentIVs->size() + loop_->mBasicIndVars->size();
  EMDEBUG1(retVal);
  return retVal;
} // ATOInductionVars::AnatropDo

bool ATOInductionVars::
IsLoopInvariant(IRExpr* expr_) const {
  BMDEBUG2("ATOInductionVars::IsLoopInvariant", expr_);
  REQUIREDMSG(expr_ != 0, ieStrParamValuesDBCViolation);
  bool retVal(expr_->HasMarked(eIRTMLoopInvariant) == true && expr_->IsMarkTrue(eIRTMLoopInvariant) == true);
  EMDEBUG1(retVal);
  return retVal;
} // ATOInductionVars::IsLoopInvariant

//! \todo H Design: d = o + 12; o = i + 12; => d should not become iv, do reaching defs and duud chains.
//! \todo H Design: Use code generator stmt match interface for iv patterns.
IRInductionVar* ATOInductionVars::
GetCandidInductionVar(IRLoop* loop_, IRSAssign* assign_) {
  BMDEBUG3("ATOInductionVars::GetCandidInductionVar", loop_, assign_);
  static IRTInt* lIntType(irBuilder->irbtGetInt(*extr.eGetTarget(loop_)->GetFastIntType(INBITS(8), IRSSigned)));
  IRInductionVar* retVal(0);
  IRObject* lUseObj;
  IRExpr* lLoopInv;
  IRObject* lIndVar(extr.eGetObjOfAddrOf(assign_->GetLHS()));
  // Note that loop invariant expressions may not involve induction variables.
  if (pred.pIsAddOrSub(assign_->GetRHS()) == true) {
    if (IsBinObjUseAndLoopInvariant(static_cast<IREBinary*>(assign_->GetRHS()), lUseObj, lLoopInv) == true) {
      if (pred.pIsSub(assign_->GetRHS()) == true) {
        if (lLoopInv == extr.eGetRightExpr(assign_->GetRHS())) {
          lLoopInv = irBuilder->irbeANeg(lLoopInv);
          if (lIndVar == lUseObj) {
            retVal = new IRInductionVar(loop_, assign_, lUseObj, irBuilder->irbeIntConst(lIntType, 1), lLoopInv);
          } else {
            retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, irBuilder->irbeIntConst(lIntType, 1), lLoopInv);
          } // if
        } else {
          if (lIndVar == lUseObj) {
            // LIE: this is not a basic induction variable: i = 1 - i like case.
            PDEBUG(GetName(), "detail", " not an iv, use=" << progcxt(lUseObj) << " iv=" << progcxt(lIndVar));
          } else {
            retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, irBuilder->irbeIntConst(lIntType, -1), lLoopInv);
          } // if
        } // if
      } else {
        if (lIndVar == lUseObj) {
          retVal = new IRInductionVar(loop_, assign_, lUseObj, irBuilder->irbeIntConst(lIntType, 1), lLoopInv);
        } else {
          retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, irBuilder->irbeIntConst(lIntType, 1), lLoopInv);
        } // if
      } // if
    } else {
      IRExpr* lAdditiveRight(extr.eGetRightExpr(assign_->GetRHS()));
      IRExpr* lAdditiveLeft(extr.eGetLeftExpr(assign_->GetRHS()));
      if (IsLoopInvariant(lAdditiveRight) == true && pred.pIsMul(lAdditiveLeft) == true) {
        if (IsBinObjUseAndLoopInvariant(static_cast<IREBinary*>(lAdditiveLeft), lUseObj, lLoopInv) == true) {
          // case j = m*i +/- li
          if (lUseObj == lIndVar) {
            // LIE: may not be a basic induction variable with mul factor.
            PDEBUG(GetName(), "detail", " not an iv, use=" << progcxt(lUseObj) << " iv=" << progcxt(lIndVar));
          } else {
            if (pred.pIsSub(assign_->GetRHS()) == true) {
              lAdditiveRight = irBuilder->irbeANeg(lAdditiveRight);
              retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, lLoopInv, lAdditiveRight);
            } else {
              retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, lLoopInv, lAdditiveRight);
            } // if
          } // if
        } // if
      } else if (IsLoopInvariant(lAdditiveLeft) == true && pred.pIsMul(lAdditiveRight) == true) {
        if (IsBinObjUseAndLoopInvariant(static_cast<IREBinary*>(lAdditiveRight), lUseObj, lLoopInv) == true) {
          // case j = li +/- m*i
          if (lUseObj == lIndVar) {
            // LIE: may not be a basic induction variable with mul factor.
            PDEBUG(GetName(), "detail", " not an iv, use=" << progcxt(lUseObj) << " iv=" << progcxt(lIndVar));
          } else {
            if (pred.pIsSub(assign_->GetRHS()) == true) {
              lLoopInv = irBuilder->irbeANeg(lLoopInv);
              retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, lLoopInv, lAdditiveLeft);
            } else {
              retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, lLoopInv, lAdditiveLeft);
            } // if
          } // if
        } // if
      } // if
    } // if
  } else if (IREANeg* lANeg = dynamic_cast<IREANeg*>(assign_->GetRHS())) {
    if (pred.pIsUseOfObj(lANeg->GetExpr()) == true) {
      // j = -i
      if (lIndVar != extr.eGetObjOfUse(lANeg->GetExpr())) {
        retVal = new IRInductionVar(loop_, assign_, lIndVar, extr.eGetObjOfUse(lANeg->GetExpr()), 
          irBuilder->irbeIntConst(lIntType, -1), irBuilder->irbeZero(lIntType));
      } // if
    } // if
  } else if (pred.pIsMul(assign_->GetRHS()) == true) {
    if (IsBinObjUseAndLoopInvariant(static_cast<IREBinary*>(assign_->GetRHS()), lUseObj, lLoopInv) == true) {
      if (lIndVar != lUseObj) {
        retVal = new IRInductionVar(loop_, assign_, lIndVar, lUseObj, lLoopInv, irBuilder->irbeZero(lIntType));
      } // if
    } // if
  } // if
  //! \todo M Design: Single use is unnecessarily restrictive we can split the
  //!       basic induction variables for the same variable.
  if (retVal != 0 && pred.pHasSingleDefInLoop(loop_, retVal->GetIVObj()) == false) {
    retVal = 0;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOInductionVars::GetCandidInductionVar

bool ATOInductionVars::
IsBinObjUseAndLoopInvariant(const IREBinary* expr_, IRObject*& objUse_, IRExpr*& loopInv_) {
  BMDEBUG2("ATOInductionVars::IsBinObjUseAndLoopInvariant", expr_);
  tristate retVal;
  if (pred.pIsUseOfObj(extr.eGetLeftExpr(expr_)) == true) {
    if (IsLoopInvariant(extr.eGetRightExpr(expr_)) == true) {
      objUse_ = extr.eGetObjOfUse(extr.eGetLeftExpr(expr_));
      loopInv_ = extr.eGetRightExpr(expr_);
      retVal = true;
    } else {
      retVal = false;
    } // if
  } else if (pred.pIsUseOfObj(extr.eGetRightExpr(expr_)) == true) {
    if (IsLoopInvariant(extr.eGetLeftExpr(expr_)) == true) {
      objUse_ = extr.eGetObjOfUse(extr.eGetRightExpr(expr_));
      loopInv_ = extr.eGetLeftExpr(expr_);
      retVal = true;
    } else {
      retVal = false;
    } // if
  } else {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOInductionVars::IsBinObjUseAndLoopInvariant




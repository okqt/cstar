// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_GCSE_HPP__
#include "ato_gcse.hpp"
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

bool ATOGCSE::
shouldDoExpr(const IRExpr* expr_) {
  BDEBUG2("ATOGCSE::shouldDoExpr", expr_);
  bool retVal(true);
  //! \todo H Design: Add options to check these.
  if (pred.pIsCmp(expr_) == true) {
    retVal = false;
  } else if (pred.pIsCast(expr_) == true) {
    retVal = false;
  } else if (pred.pIsConst(expr_) == true) {
    retVal = false;
  } else if (pred.pIsUse(expr_) == true || pred.pIsAddrOf(expr_) == true) {
    retVal = false;
  } // if
  PDEBUGBLOCK() {
    if (retVal == true) {
      PDEBUG("ATOGCSE", "detail", "should do expression: " << progcxt(expr_));
    } // if
  } // PDEBUGBLOCK
  EDEBUG1(retVal);
  return retVal;
} // ATOGCSE::shouldDoExpr

hFInt32 ATOGCSE::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATOGCSE::AnatropDo(IRFunction*)", func_);
  hFInt32 retVal(0);
  DFAIRAvailExpressions lDFAAE(func_, shouldDoExpr);
  lDFAAE.Do();
  ListIterator<IRBB*> lBBIter(func_->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    DFAIRAvailExpressions::TopBitVector lAEIn;
    lDFAAE.GetAvailExprsInSet(*lBBIter, lAEIn);
    AvailableExpressions<IROTmp*> lAvailExprs;
    for (hFUInt32 c(0); c < lAEIn.size(); ++ c) {
      if (shouldDoExpr(lAEIn[c]->GetData()) == true) {
        lAvailExprs.Add(lAEIn[c]->GetData(), hFSInt32_ignore);
      } // if
    } // for
    ListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtIter());
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      ExprCollector<IRExpr> lExprCollector(EVOTPost, *lStmtIter, ExprVisitorOptions().SetLHSNo());
      for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
        hFSInt32 lPrevIndex;
        IRExpr* lExpr(lExprCollector[c]);
        if (!shouldDoExpr(lExpr) || pred.pIsDetached(lExpr) == true) {
          // LIE: Skip the expression.
        } else if (lAvailExprs.Add(lExpr, lPrevIndex) == true && (lPrevIndex != -1 && lPrevIndex < lAEIn.size())) {
          // We found the first expression that matches to an available expression.
          STATRECORDMSG("gcse: expr found", progcxt(lExpr));
          vector<IRExpr*> lSources;
          lBBIter->GetCFG()->ResetVisits();
          findSources(lExpr, *lBBIter, lSources);
          ASSERTMSG(!lSources.empty(), ieStrInconsistentInternalStructure);
          IRBuilder lIRBuilder(func_, consts.cIRBNoOptims);
          IROTmp* lTmp(lIRBuilder.irboTmp(lExpr->GetType(), "gcse"));
          for (hFUInt32 d(0); d < lSources.size(); ++ d) {
            PDEBUG(GetName(), "detail", "source= " << refcxt(lSources[d]) << ":" << progcxt(lSources[d]));
            ASSERTMSG(lExpr != lSources[d], ieStrInconsistentInternalStructure);
            /* LIE */ lIRBuilder.GetHelper()->irbsAssign(lTmp, lSources[d]->GetRecCopyExpr(), 
              ICStmt(extr.eGetStmt(lSources[d]), ICBefore), SrcLoc());
            /* LIE */ lSources[d]->ReplaceWith(lIRBuilder.GetHelper()->irbeUseOf(lTmp));
            retVal ++;
            STATRECORDMSG("gcse: expr replaced", refcxt(lSources[d]) << ":" << progcxt(lSources[d]));
          } // for
          /* LIE */ lExpr->ReplaceWith(lIRBuilder.GetHelper()->irbeUseOf(lTmp));
          // Do not process the same expression again
          lAvailExprs.GetAvail(lPrevIndex) = false;
        } // if
      } // for
      lAvailExprs.Remove(*lStmtIter);
    } // for
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOGCSE::AnatropDo

void ATOGCSE::
findSources(IRExpr* expr_, IRBB* bb_, vector<IRExpr*>& sources_) {
  BMDEBUG3("ATOGCSE::findSources", expr_, bb_);
  IRBB* lBB(extr.eGetStmt(expr_)->GetBB());
  if (bb_ != lBB && bb_->IsVisited() == false) {
    RevListIterator<IRStmt*> lStmtIter(bb_->GetStmtRevIter());
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      ExprCollector<IRExpr> lExprCollector(EVOTPost, *lStmtIter, ExprVisitorOptions().SetLHSNo());
      bool lMatchFound(false);
      for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
        PDEBUG(GetName(), "detail", "comparing: " << progcxt(expr_) << " <-> " << progcxt(lExprCollector[c]));
        if (AvailableExpressions<bool>::smSameExpr(expr_, lExprCollector[c]) == true) {
          PDEBUG(GetName(), "detail", "a source found: " << refcxt(lExprCollector[c]));
          sources_.push_back(lExprCollector[c]);
          lMatchFound = true;
          break;
        } // if
      } // for
      if (lMatchFound == true) {
        break;
      } // if
    } // for
  } // if
  if (bb_->IsVisited() == false) {
    bb_->SetVisited();
    vector<IRBB*> lPreds;
    extr.eGetPredsOfBB(bb_, lPreds);
    for (hFUInt32 c(0); c < lPreds.size(); ++ c) {
      if (!pred.pIsEntryBB(lPreds[c])) {
        findSources(expr_, lPreds[c], sources_);
      } // if
    } // for
  } // if
  EMDEBUG1(sources_.size());
} // ATOGCSE::findSources



// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_LCOPYPROP_HPP__
#include "ato_lcopyprop.hpp"
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

hFInt32 ATOLCopyProp::
AnatropDo(IRBB* bb_) {
  BMDEBUG2("ATOLCopyProp::AnatropDo(IRBB*)", bb_);
  hFInt32 retVal(0);
  FwdListIterator<IRStmt*> lStmtIter(bb_->GetStmtIter());
  // <lhsobj, uselhs, userhs, iskilled>
  typedef tuple4<IRObject*, IRExpr*, IRExpr*, bool> TupleCopyPairs;
  vector<TupleCopyPairs> lCopyPairs;
  // <lhsobj> to the index in lCopyPairs.
  map<IRObject*, hFUInt32> lLHSObj2Copies;
  // Both lhs and rhs object should not be killed between the use of lhs and assignmet.
  for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
    // Replace the lhs obj uses with rhs expressions.
    if (!lLHSObj2Copies.empty()) {
      PDEBUG(GetName(), "detail", "stmt in consideration " << progcxt(*lStmtIter));
      ExprCollector<IREDeref> lExprCollector(EVOTPost, *lStmtIter, ExprVisitorOptions().SetLHSYes());
      for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
        if (pred.pIsUse(lExprCollector[c]) == true) {
          IRObject* lObj(extr.eGetObjOfUse(lExprCollector[c]));
          map<IRObject*, hFUInt32>::iterator lIt(lLHSObj2Copies.find(lObj));
          if (lIt != lLHSObj2Copies.end() && lCopyPairs[lIt->second].mT3 == consts.cIsKilledNo) {
            IRExpr* lRHSExpr(lCopyPairs[lIt->second].mT2);
            /* LIE */ lExprCollector[c]->ReplaceWith(lRHSExpr->GetRecCopyExpr());
            //! \todo H Design: Can't we do this auto in irbuilder?
            if (pred.pIsConst(lRHSExpr) == true && pred.pIsIf(*lStmtIter) == true) {
              anatropManager->Trigger("IfSimplifications", *lStmtIter);
            } // if
            STATRECORDMSG("lcopyprop: expr replaced", progcxt(lExprCollector[c]));
            ++ retVal;
          } // if
        } // if
      } // for
    } // if
    // Kill the pairs
    for (hFUInt32 c(0); c < lCopyPairs.size(); ++ c) {
      if (lCopyPairs[c].mT3 == consts.cIsKilledNo) {
        if (pred.pCanKill(*lStmtIter, lCopyPairs[c].mT1) == true) {
          lCopyPairs[c].mT3 = consts.cIsKilledYes;
        } else if (pred.pCanKill(*lStmtIter, lCopyPairs[c].mT2) == true) {
          lCopyPairs[c].mT3 = consts.cIsKilledYes;
        } // if
      } // if
    } // for
    // Populate the copy pairs.
    if (pred.pIsCopyAssign(*lStmtIter) == true || pred.pIsConstAssign(*lStmtIter) == true) {
      PDEBUG(GetName(), "detail", " copy/const assign stmt seen " << progcxt(*lStmtIter));
      IRSAssign* lAssign(static_cast<IRSAssign*>(*lStmtIter));
      IRObject* lLHSObj(extr.eGetObjOfAddrOf(lAssign->GetLHS()));
      lLHSObj2Copies[lLHSObj] = lCopyPairs.size();
      lCopyPairs.push_back(TupleCopyPairs(lLHSObj, irhBuilder->irbeUseOf(lLHSObj), lAssign->GetRHS(), consts.cIsKilledNo));
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOLCopyProp::AnatropDo



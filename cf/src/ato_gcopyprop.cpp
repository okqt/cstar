// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_GCOPYPROP_HPP__
#include "ato_gcopyprop.hpp"
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

hFInt32 ATOGCopyProp::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATOGCopyProp::AnatropDo(IRFunction*)", func_);
  hFInt32 retVal(0);
  DFAIRCopyAssignments lDFACopyAssign(func_);
  lDFACopyAssign.Do();
  ListIterator<IRBB*> lBBIter(func_->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    DFAIRCopyAssignments::TopBitVector lCopyPairs;
    lDFACopyAssign.GetAvailCopiesInSet(*lBBIter, lCopyPairs);
    map<IRObject*, hFUInt32> lLHSObj2Copies;
    for (hFUInt32 c(0); c < lCopyPairs.size(); ++ c) {
      lLHSObj2Copies[lCopyPairs[c]->GetData()->mT1] = c;
      // We should reset iskilled to false for each basic block.
      lCopyPairs[c]->GetData()->mT4 = consts.cIsKilledNo;
    } // for
    ListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtIter());
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      { // Replace the lhs obj uses with rhs expressions.
        ExprCollector<IREDeref> lExprCollector(EVOTPost, *lStmtIter, ExprVisitorOptions().SetLHSYes());
        for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
          if (pred.pIsUse(lExprCollector[c]) == true) {
            IRObject* lObj(extr.eGetObjOfUse(lExprCollector[c]));
            map<IRObject*, hFUInt32>::iterator lIt(lLHSObj2Copies.find(lObj));
            if (lIt != lLHSObj2Copies.end() && lCopyPairs[lIt->second]->GetData()->mT4 == consts.cIsKilledNo) {
              IRExpr* lRHSExpr(lCopyPairs[lIt->second]->GetData()->mT3);
              STATRECORDMSG("gcopyprop: expr replaced", progcxt(*lStmtIter) << endl << 
                " expr:" << progcxt(lExprCollector[c]) << " new expr:" << progcxt(lRHSExpr));
              //! \todo H Design: set a trigger container in irbuilder and add
              //!         the triggers in optimizing irbuilder when cases arises like below.
              /* LIE */ lExprCollector[c]->ReplaceWith(lRHSExpr->GetRecCopyExpr());
              if (pred.pIsConst(lRHSExpr) == true && pred.pIsIf(*lStmtIter) == true) {
                anatropManager->Trigger("IfSimplifications", *lStmtIter);
              } // if
              ++ retVal;
            } // if
          } // if
        } // for
      } // block
      // Kill the pairs
      for (hFUInt32 c(0); c < lCopyPairs.size(); ++ c) {
        DFAIRCopyAssignData* lData(lCopyPairs[c]->GetData());
        if (lData->mT4 == consts.cIsKilledNo) {
          PDEBUG(GetName(), "detail", "can kill? stmt=" << progcxt(*lStmtIter) << endl << 
            " expr1=" << progcxt(lData->mT2) << endl << " expr2=" << progcxt(lData->mT3));
          if (pred.pCanKill(*lStmtIter, lData->mT2) == true) {
            lData->mT4 = consts.cIsKilledYes;
          } else if (pred.pCanKill(*lStmtIter, lData->mT3) == true) {
            lData->mT4 = consts.cIsKilledYes;
          } // if
        } // if
      } // for
    } // for
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOGCopyProp::AnatropDo



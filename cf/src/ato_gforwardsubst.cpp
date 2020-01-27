// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_GFORWARDSUBST_HPP__
#include "ato_gforwardsubst.hpp"
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
#ifndef __TEMPLATE_AVAILEXPR_HPP__
#include "template_availexpr.hpp"
#endif

hFInt32 ATOGForwardSubstitution::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATOGForwardSubstitution::AnatropDo(IRFunction*)", func_);
  hFInt32 retVal(0);
  if (!mDUUDChains) {
    mDUUDChains = func_->GetCFG()->GetDUUDChains();
  } // if
  ListIterator<IRStmt*> lStmtIter(extr.eGetStmtIter(func_));
  for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
    // Check for assignment candidates
    if (IRSAssign* lAssign = dynamic_cast<IRSAssign*>(*lStmtIter)) {
      if (pred.pIsConstAssign(lAssign) == true || pred.pIsCopyAssign(lAssign) == true) {
        static bool lTriggered(false);
        if (!lTriggered) {
          anatropManager->Trigger("GCopyProp", func_);
          lTriggered = true;
        } // if
      } else if (pred.pIsAddrOfObj(lAssign->GetLHS()) == true) {
        IRObject* lLHSObj(extr.eGetObjOfAddrOf(lAssign->GetLHS()));
        // The lhs object must have a single definition.
        if (!pred.pIsGlobal(lLHSObj) && !pred.pIsAddrTaken(lLHSObj) && extr.eGetAllDefsOfObj(mDUUDChains, lLHSObj).size() == 1) {
          IRExpr* lLHSUse(irhBuilder->irbeUseOf(lLHSObj));
          PDEBUG(GetName(), "detail", " forward subst assignment found = " << progcxt(lAssign));
          AvailableExpressions<bool> lRHSAExprs;
          { // Construct the available expressions object for RHS.
            vector<IREDeref*> lDerefs;
            extr.eGetDerefOfObjs(lAssign->GetRHS(), lDerefs);
            for (hFUInt32 c(0); c < lDerefs.size(); ++ c) {
              hFSInt32 lDummyPrev;
              lRHSAExprs.Add(lDerefs[c], lDummyPrev);
            } // for
          } // block
          // Get all uses that this definition reaches.
          const set<IRStmt*>& lUses(extr.eGetUses(mDUUDChains, lAssign, lLHSObj));
          for (set<IRStmt*>::const_iterator lIt(lUses.begin()); lIt != lUses.end(); ++ lIt) {
            IRStmt* lUseStmt(*lIt);
            set<IRBB*> lBBsInPath;
            extr.eGetBBsInPath(lAssign->GetBB(), lUseStmt->GetBB(), lBBsInPath);
            if (!lBBsInPath.empty()) {
              // Remove assign bb and use bb from the list. For assign bb we
              // will invoke local fwdsubst and for use bb we will handle below.
              anatropManager->Execute("LFwdSubst", lAssign, GenericOptions().obSet("execute_triggers", false));
              lBBsInPath.erase(lAssign->GetBB());
              lBBsInPath.erase(lUseStmt->GetBB());
              if (!lBBsInPath.empty() && lRHSAExprs.pAnyKilled(lBBsInPath) == false) {
                // Scan thru all stmts till the use in use bb.
                FwdListIterator<IRStmt*> lBBStmtIter(lUseStmt->GetBB()->GetStmtIter());
                for (lBBStmtIter.First(); !lBBStmtIter.IsDone(); lBBStmtIter.Next()) {
                  if (*lBBStmtIter == lUseStmt) {
                    // Check the uses of the lhs object.
                    ExprCollector<IREDeref> lExprCollector(EVOTPost, *lBBStmtIter, ExprVisitorOptions().SetLHSYes());
                    for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
                      if (pred.pIsUse(lExprCollector[c]) == true && extr.eGetObjOfUse(lExprCollector[c]) == lLHSObj) {
                        STATRECORDMSG("gfwdsubst: expr replaced", progcxt(lAssign) << endl << 
                            progcxt(*lBBStmtIter) << endl << " expr: " << progcxt(lExprCollector[c]));
                        /* LIE */ lExprCollector[c]->ReplaceWith(lAssign->GetRHS()->GetRecCopyExpr());
                        ++ retVal;
                      } // if
                    } // for
                    break;
                  } // if
                  // Stop if LHS obj or RHS expression is killed.
                  if (lRHSAExprs.Remove(*lBBStmtIter) == true) {
                    break;
                  } // if
                } // for
              } // if
            } // if
          } // for
        } // if
      } // if
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOGForwardSubstitution::AnatropDo




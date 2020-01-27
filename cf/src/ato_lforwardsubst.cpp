// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_LFORWARDSUBST_HPP__
#include "ato_lforwardsubst.hpp"
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

void ATOLForwardSubstitution::
OnStart(IRStmt* stmt_) {
  BMDEBUG2("ATOLForwardSubstitution::OnStart", stmt_);
  if (!mDUUDChains) {
    mDUUDChains = extr.eGetCFG(stmt_)->GetDUUDChains();
  } // if
  EMDEBUG0();
  return;
} // ATOLForwardSubstitution::OnStart

hFInt32 ATOLForwardSubstitution::
AnatropDo(IRStmt* stmt_) {
  BMDEBUG2("ATOLForwardSubstitution::AnatropDo(IRStmt*)", stmt_);
  hFInt32 retVal(0);
  // Check for assignment candidates
  if (IRSAssign* lAssign = dynamic_cast<IRSAssign*>(stmt_)) {
    if (pred.pIsConstAssign(lAssign) == true || pred.pIsCopyAssign(lAssign) == true) {
      anatropManager->Trigger("LCopyProp", stmt_->GetBB());
    } else if (pred.pIsAddrOfObj(lAssign->GetLHS()) == true) {
      IRObject* lLHSObj(extr.eGetObjOfAddrOf(lAssign->GetLHS()));
      // The lhs object must have a single definition.
      if (!pred.pIsGlobal(lLHSObj) && !pred.pIsAddrTaken(lLHSObj) && extr.eGetAllDefsOfObj(mDUUDChains, lLHSObj).size() == 1) {
        IRExpr* lLHSUse(irhBuilder->irbeUseOf(lLHSObj));
        PDEBUG(GetName(), "detail", " forward subst assignment found = " << progcxt(lAssign));
        AvailableExpressions<bool> lRHSAExprs;
        { // Construct the available expressions object for RHS.
          hFSInt32 lDummyPrev;
          vector<IREDeref*> lDerefs;
          extr.eGetDerefOfObjs(lAssign->GetRHS(), lDerefs);
          for (hFUInt32 c(0); c < lDerefs.size(); ++ c) {
            lRHSAExprs.Add(lDerefs[c], lDummyPrev);
          } // for
          lRHSAExprs.Add(lLHSUse, lDummyPrev);
        } // block
        // Scan thru all stmts ahead till the end of bb for the uses.
        IRStmt* lCurrStmt(extr.eGetNextStmt(lAssign));
        for (/* LIE */; lCurrStmt != 0; lCurrStmt = extr.eGetNextStmt(lCurrStmt)) {
          // Check the uses of the lhs object.
          //! \todo M Design: Would it be better to call irbReplaceUses and give up stat record?
          ExprCollector<IREDeref> lExprCollector(EVOTPost, lCurrStmt, ExprVisitorOptions().SetLHSYes());
          for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
            if (pred.pIsUse(lExprCollector[c]) == true) {
              IRObject* lObj(extr.eGetObjOfUse(lExprCollector[c]));
              if (lObj == lLHSObj) {
                STATRECORDMSG("lfwdsubst: expr replaced", progcxt(lAssign) << endl << 
                    progcxt(lCurrStmt) << endl << " expr: " << progcxt(lExprCollector[c]));
                /* LIE */ lExprCollector[c]->ReplaceWith(lAssign->GetRHS()->GetRecCopyExpr());
                ++ retVal;
              } // if
            } // if
          } // for
          // Stop if LHS obj or RHS expression is killed.
          if (lRHSAExprs.Remove(lCurrStmt) == true) {
            break;
          } // if
        } // for
      } // if
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOLForwardSubstitution::AnatropDo



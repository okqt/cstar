// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_LCSE_HPP__
#include "ato_lcse.hpp"
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

bool ATOLCSE::
shouldDoExpr(const IRExpr* expr_) const {
  BMDEBUG2("ATOLCSE::shouldDoExpr", expr_);
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
      PDEBUG(GetName(), "detail", "should do expression: " << progcxt(expr_));
    } // if
  } // PDEBUGBLOCK
  EMDEBUG1(retVal);
  return retVal;
} // ATOLCSE::shouldDoExpr

hFInt32 ATOLCSE::
AnatropDo(IRBB* bb_) {
  BMDEBUG2("ATOLCSE::AnatropDo(IRBB*)", bb_);
  hFInt32 retVal(0);
  AvailableExpressions<IROTmp*> lAvailExprs;
  IRBuilder lIRBuilder(extr.eGetFunc(bb_), consts.cIRBNoOptims);
  bool lAnyReplaced(false);
  for (IRStmt* lStmt(extr.eGetFirstStmt(bb_)); lStmt != 0; lStmt = extr.eGetNextStmt(lStmt)) {
    // Do a preorder traversal to match the longest expressions, e.g. case *-&-a 
    ExprCollector<IRExpr> lExprCollector(EVOTPre, lStmt, ExprVisitorOptions().SetLHSNo()); // do post creates detached expressions.
    for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
      IRExpr* lExpr(lExprCollector[c]);
      hFSInt32 lPrevIndex;
      if (pred.pIsDetached(lExpr) == true || shouldDoExpr(lExpr) == false) {
        // LIE
      } else if (lAvailExprs.Add(lExpr, lPrevIndex) == true && lPrevIndex != -1) {
        IROTmp* lTmp(lAvailExprs.GetData(lPrevIndex));
        if (!lTmp) {
          lAnyReplaced = true;
          // Create a temporary variable that can hold the result of the expression.
          lTmp = lIRBuilder.irboTmp(lExpr->GetType(), "lcse");
          IRExpr* lPrevExpr(lAvailExprs.GetExpr(lPrevIndex));
          lIRBuilder.GetHelper()->irbsAssign(lTmp, lExpr->GetRecCopyExpr(), ICStmt(extr.eGetStmt(lPrevExpr), ICBefore), SrcLoc());
          /* LIE */ lPrevExpr->ReplaceWith(lIRBuilder.GetHelper()->irbeUseOf(lTmp));
          lAvailExprs.GetData(lPrevIndex) = lTmp;
          STATRECORD("lcse: expr found");
        } // if
        ASSERTMSG(lTmp != 0, ieStrInconsistentInternalStructure);
        /* LIE */ lExpr->ReplaceWith(lIRBuilder.GetHelper()->irbeUseOf(lTmp));
        STATRECORD("lcse: expr replaced");
      } // if
    } // for
    lAvailExprs.Remove(lStmt);
  } // for
  if (lAnyReplaced == true) {
    anatropManager->Trigger("LCSE", bb_);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOLCSE::AnatropDo


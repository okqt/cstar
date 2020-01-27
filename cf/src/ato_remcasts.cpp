// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_REMCASTS_HPP__
#include "ato_remcasts.hpp"
#endif

hFInt32 ATORemoveCasts::
AnatropDo(IRStmt* stmt_) {
  BMDEBUG2("ATORemoveCasts::AnatropDo(IRStmt*)", stmt_);
  hFInt32 retVal(0);
  {
    ExprCollector<IRECast> lExprCollector(EVOTPost, stmt_);
    for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
      retVal += processCast(static_cast<IRECast*>(lExprCollector.mCollected0[c]));
    } // for
  } // block
  EMDEBUG1(retVal);
  return retVal;
} // ATORemoveCasts::AnatropDo

hFInt32 ATORemoveCasts::
AnatropDo(IRModule* module_) {
  BMDEBUG2("ATORemoveCasts::AnatropDo(IRModule*)", module_);
  hFInt32 retVal(0);
  {
    ExprCollector<IRECast, IRModule> lExprCollector(EVOTPost, module_);
    for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
      retVal += processCast(static_cast<IRECast*>(lExprCollector.mCollected0[c]));
    } // for
  } // block
  EMDEBUG1(retVal);
  return retVal;
} // ATORemoveCasts::AnatropDo

hFUInt32 ATORemoveCasts::
processCast(IRECast* castExpr_) {
  BMDEBUG2("ATORemoveCasts::processCast", castExpr_);
  hFInt32 retVal(0);
  IRExpr* lExpr(castExpr_->GetExpr());
  if (lExpr->GetType() == castExpr_->GetType()) {
    /* LIE */ castExpr_->ReplaceWith(lExpr);
    anatropManager->Trigger("RemoveCasts", extr.eGetStmt(lExpr));
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATORemoveCasts::processCast



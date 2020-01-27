// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_CODESTRAIGTHENING_HPP__
#include "ato_codestraigthening.hpp"
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

hFInt32 ATOCodeStraigthening::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATOCodeStraigthening::AnatropDo(IRFunction*)", func_);
  hFUInt32 retVal(0);
  IRBB* lCurrBB(func_->GetCFG()->GetEntryBB()->GetNext());
  IRBB* lExitBB(func_->GetCFG()->GetExitBB());
  while (lCurrBB != lExitBB) {
    if (extr.eGetNumOfSuccs(lCurrBB) == 1) {
      IRBB* lSucc(extr.eGetSucc(lCurrBB));
      if (lSucc != lCurrBB && extr.eGetNumOfPreds(lSucc) == 1 && !pred.pIsExitBB(lSucc)) {
        lCurrBB->RemoveStmt(lCurrBB->GetLastStmt());
        if (!pred.pIsEmpty(lSucc)) {
          irhBuilder->irbMoveStmts(lSucc->GetFirstStmt(), lSucc->GetLastStmt(), ICBB(lCurrBB, ICEndOfBB));
        } // if
        irBuilder->irbToBeRemoved(lSucc);
        STATRECORD("straigthening: count");
      } // if
    } // if
    lCurrBB = lCurrBB->GetNext();
  } // while
  EMDEBUG1(retVal);
  return retVal;
} // ATOCodeStraigthening::AnatropDo



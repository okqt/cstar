// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_UNREACHABLECODE_HPP__
#include "ato_unreachablecode.hpp"
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

hFInt32 ATOUnreachableCode::
AnatropDo(IRBB* bb_) {
  BMDEBUG2("ATOUnreachableCode::AnatropDo(IRBB*)", bb_);
  hFUInt32 retVal(0);
  if (pred.pIsEntryOrExitBB(bb_) == false) {
    bool lSelfLoop(extr.eGetNumOfPreds(bb_) == 1 && extr.eGetPred(bb_) == bb_);
    if (pred.pHasPred(bb_) == false || lSelfLoop == true) {
      if (!pred.pIsEmpty(bb_)) {
        bb_->RemoveAllStmts();
      } // if
      irBuilder->irbToBeRemoved(bb_);
      vector<IRBB*> lSuccs;
      extr.eGetSuccsOfBB(bb_, lSuccs);
      for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
        if (bb_ != lSuccs[c]) {
          anatropManager->Trigger("UnreachableCode", lSuccs[c]);
        } // if
      } // for
      ++ retVal;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOUnreachableCode::AnatropDo


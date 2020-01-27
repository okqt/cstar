// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_REMOVENOPS_HPP__
#include "ato_removenops.hpp"
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

hFInt32 ATORemoveNops::
AnatropDo(IRBB* bb_) {
  BMDEBUG2("ATORemoveNops::AnatropDo(IRBB*)", bb_);
  hFUInt32 retVal(0);
  ListIterator<IRStmt*> lStmtIt(bb_->GetStmtIter());
  for (lStmtIt.First(); !lStmtIt.IsDone(); lStmtIt.Next()) {
    //! \todo M Design: below we can add an option to remove null/label
    //!       removal both or only one of them.
    // Address taken label is a nop but we cannot remove it since it may be
    // referenced.
    if (pred.pIsNop(*lStmtIt) == true && pred.pIsAddrTakenLabel(*lStmtIt) == false) {
      ++ retVal;
      irBuilder->irbToBeRemoved(*lStmtIt);
      STATRECORD("removenops: count");
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATORemoveNops::AnatropDo


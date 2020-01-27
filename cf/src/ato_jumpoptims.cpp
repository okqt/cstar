// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_JUMPOPTIMS_HPP__
#include "ato_jumpoptims.hpp"
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

hFInt32 ATOJumpOptims::
AnatropDo(IRBB* bb_) {
  BMDEBUG2("ATOJumpOptims::AnatropDo(IRBB*)", bb_);
  hFInt32 retVal(0);
  //! \todo M Design: After irbuilder optims to anatrop call convert this.
  IRBuilder lIRBuilder(irProgram, consts.cIRBNoOptims);
  if (pred.pIsExitBB(bb_) == false && pred.pHasSucc(bb_) == true) {
    if (extr.eGetNumOfStmts(bb_) == 1 && pred.pIsJump(bb_->GetLastStmt()) == true) {
      vector<IRBB*> lPreds;
      extr.eGetPredsOfBB(bb_, lPreds);
      for (hFUInt32 c(0); c < lPreds.size(); ++ c) {
        if (pred.pIsEntryBB(lPreds[c]) == false) {
          lIRBuilder.irbRemapJumpTarget(lPreds[c]->GetLastStmt(), bb_, extr.eGetSucc(bb_));
          STATRECORD("jumpoptims: count");
          ++ retVal;
        } // if
      } // for
    } // if
    if (!retVal) {
      vector<IRBB*> lSuccs;
      bool lAnySucc(extr.eGetSuccsOfBB(bb_, lSuccs));
      ASSERTMSG(lAnySucc == true && lSuccs.size() != 0, ieStrInconsistentInternalStructure);
      for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
        IRBB* lSucc(lSuccs[c]);
        if (extr.eGetNumOfSuccs(lSucc) == 1 && pred.pIsJump(lSucc->GetLastStmt()) == true) {
          if (lSucc->GetNumOfStmts() == 1) {
            //! \todo M Design: remove this code already handled above.
            // Now we can do the transformation
            lIRBuilder.irbRemapJumpTarget(bb_->GetLastStmt(), lSucc, extr.eGetSucc(lSucc));
            STATRECORD("jumpoptims: count");
            ++ retVal;
          } else if (pred.pIsNop(lSucc->GetFirstStmt()) == true) {
            // There is a possibility that all the statements are labels,
            // this may further lead to short cut jumps optimizations.
            anatropManager->Trigger("RemoveNops", lSucc);
          } // if
          if (!pred.pHasPred(lSucc)) {
            anatropManager->Trigger("UnreachableCode", lSucc);
          } // if
        } // if
      } // for
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOJumpOptims::AnatropDo


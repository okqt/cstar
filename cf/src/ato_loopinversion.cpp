// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_LOOPINVERSION_HPP__
#include "ato_loopinversion.hpp"
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

hFInt32 ATOLoopInversion::
AnatropDo(IRLoop* loop_) {
  BMDEBUG2("ATOLoopInversion::AnatropDo(IRLoop*)", loop_);
  hFUInt32 retVal(0);
  //! \todo M Design: Add in if simplifications that 2if stmts ni to one -> cond0 && or || cond1 
  //!       run the if simplification before inversion.
  if (extr.eGetNumOfPreds(loop_->GetHeader()) > 2) {
    PDEBUG(GetName(), "detail", " not doing loop inversion header pred count");
    // LIE
  } else if (pred.pIsHeaderHasExitEdge(loop_) == false) {
    PDEBUG(GetName(), "detail", " not doing loop inversion, header has no exit edge");
  } else if (extr.eGetNumOfSuccs(loop_->GetHeader()) != 2) {
    PDEBUG(GetName(), "detail", " not doing loop inversion, no condition found in the header");
  } else if (loop_->GetExitEdges().size() > 1) {
    PDEBUG(GetName(), "detail", " not doing loop inversion, multiple exit condition found");
    //! \todo M Design: This is not necessarily a conditional case, so improve it.
  } else if (extr.eGetNumOfStmts(loop_->GetHeader()) != 1) {
    // When there is more than one statement it is an indication of previous loop inversion.
    PDEBUG(GetName(), "detail", " not doing loop inversion, header should have one statement");
  } else {
    if (loop_->IsIteratesAtLeastOnce() == true) {
      // LIE: not yet implemented.
      NOTIMPLEMENTED();
    } else {
      // We need to add an if guard to guarantee an iteration.
      irBuilder->irbAddGuard(loop_);
      PDEBUG(GetName(), "detail-guard-added", "");
      // We assume with all the above conditions there is only a single
      // location that we need to insert the loop condition which is the
      // iteration block, or the pred of the header that is in the loop.
      STATRECORD("loopinversion: count");
      ASSERTMSG(pred.pIsIf(loop_->GetHeader()->GetLastStmt()) == true, ieStrInconsistentInternalStructure);
      IRSIf* lIf(static_cast<IRSIf*>(loop_->GetHeader()->GetLastStmt()));
      IRBB* lIfLoopBB(0);
      if (pred.pIsInLoop(loop_, lIf->GetTrueCaseBB()) == true) {
        lIfLoopBB = lIf->GetTrueCaseBB();
      } else {
        ASSERTMSG(pred.pIsInLoop(loop_, lIf->GetFalseCaseBB()) == true, ieStrInconsistentInternalStructure);
        lIfLoopBB = lIf->GetFalseCaseBB();
      } // if
      vector<IRBB*> lBackEdges;
      extr.eGetBackEdgeBBs(loop_, lBackEdges);
      for (hFUInt32 c(0); c < lBackEdges.size(); ++ c) {
        IRStmt* lJump(lBackEdges[c]->GetLastStmt());
        ASSERTMSG(pred.pIsJump(lJump) == true, ieStrInconsistentInternalStructure);
        lJump->ReplaceWith(lIf->GetRecCopyStmt());
      } // for
      lIf->ReplaceWith(irBuilder->irbsJump(lIfLoopBB, ICNull(), SrcLoc()));
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOLoopInversion::AnatropDo



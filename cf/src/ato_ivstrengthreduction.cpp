// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_IVSTRENGTHREDUCTION_HPP__
#include "ato_ivstrengthreduction.hpp"
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

hFInt32 ATOIVStrengthReduction::
AnatropDo(IRLoop* loop_) {
  BMDEBUG2("ATOIVStrengthReduction::AnatropDo(IRLoop*)", loop_);
  hFUInt32 retVal(0);
  const vector<IRInductionVar*>& mDepIVs(loop_->GetDepIndVars());
  IRBuilder lIRBuilder(extr.eGetFunc(loop_), consts.cIRBDoOptims);
  IRBuilderHelper& lIRHBuilder(*lIRBuilder.GetHelper());
  for (hFUInt32 c(0); c < mDepIVs.size(); ++ c) {
    // j = b * i + c;
    IRObject* lTJ(0);
    { // Place tj = b * i + c at the end of the preheader for proper initialization.
      IRExpr* lMulFactor(mDepIVs[c]->GetMulFactor()->GetRecCopyExpr());
      IRExpr* lAddFactor(mDepIVs[c]->GetAddFactor()->GetRecCopyExpr());
      IRExpr* lMul(lIRBuilder.irbeMul(lMulFactor, lIRHBuilder.irbeUseOf(mDepIVs[c]->GetBasisIVObj())));
      lTJ = lIRHBuilder.irbNewDef("ivsr", lIRBuilder.irbeAdd(lMul, lAddFactor), 
        ICStmt(loop_->GetPreHeader()->GetLastStmt(), ICBefore), SrcLoc());
    } // block
    { // replace j = b * i + c; with j = tj;
      mDepIVs[c]->GetDefStmt()->ReplaceWith(lIRHBuilder.irbsAssign(mDepIVs[c]->GetIVObj(),
        lIRHBuilder.irbeUseOf(lTJ), ICNull(), SrcLoc()));
    } // block
    { // After the defining statement of basis iv 'i' insert tj = tj + tdb;
      // where tdb is initialized in preheader as tdb = b*d, for i = i + d.
      IRExpr* lMulFactor(mDepIVs[c]->GetMulFactor()->GetRecCopyExpr());
      IRExpr* lAddFactor(mDepIVs[c]->GetBasisIV()->GetAddFactor()->GetRecCopyExpr());
      IRObject* lTDB(lIRHBuilder.irbNewDef("ivsrdb", lIRBuilder.irbeMul(lMulFactor, lAddFactor),
        ICStmt(loop_->GetPreHeader()->GetLastStmt(), ICBefore), SrcLoc()));
      /* LIE */ lIRHBuilder.irbsAssign(lTJ, lIRBuilder.irbeAdd(lIRHBuilder.irbeUseOf(lTJ), lIRHBuilder.irbeUseOf(lTDB)), 
        ICStmt(mDepIVs[c]->GetBasisIV()->GetDefStmt(), ICAfter), SrcLoc());
    } // block
  } // for
  if (anatropManager->Execute("LFwdSubst", loop_, GenericOptions().obSet("execute_triggers", true)) > 0) {
    anatropManager->Execute("DeadCodeElim", extr.eGetFunc(loop_));
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOIVStrengthReduction::AnatropDo



// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_IFSIMPLS_HPP__
#include "ato_ifsimpls.hpp"
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

void ATOIfSimplifications::
InitOptions() {
  BMDEBUG1("ATOIfSimplifications::InitOptions");
  addBoolOption("do_cond_merge", consts.cDefaultIsTrue);
  addBoolOption("do_not_inversion", consts.cDefaultIsTrue);
  addBoolOption("do_const_cond", consts.cDefaultIsTrue);
  addBoolOption("do_same_thenelse", consts.cDefaultIsTrue);
  addBoolOption("do_empty_thenelse", consts.cDefaultIsTrue);
  addBoolOption("do_almost_empty_thenelse", consts.cDefaultIsTrue);
  EMDEBUG0();
  return;
} // ATOIfSimplifications::InitOptions

hFInt32 ATOIfSimplifications::
AnatropDo(IRStmt* stmt_) {
  BMDEBUG2("ATOIfSimplifications::AnatropDo(IRStmt*)", stmt_);
  hFInt32 retVal(0);
  if (IRSIf* lIf = dynamic_cast<IRSIf*>(stmt_)) {
    retVal += doConditionMerge(lIf);
    if (retVal != 0) {
      // LIE: Skip other cases
    } else if (IRENot* lNot = dynamic_cast<IRENot*>(lIf->GetExpr())) {
      if (GetOptions().obGet("do_not_inversion") == true) {
        lIf->GetExpr()->ReplaceWith(lNot->GetExpr()->GetRecCopyExpr());
        irBuilder->irbSwapTrueFalseBBs(lIf);
        STATRECORD("ifsimpls: not condition");
        ++ retVal;
      } // if
    } else if (pred.pIsConst(lIf->GetExpr()) == true) {
      if (GetOptions().obGet("do_const_cond") == true) {
        // Convert the If in to an unconditional jump to the "true" bb.
        IRBB* lTargetBB(pred.pIsConstTrue(lIf->GetExpr()) == false ? lIf->GetFalseCaseBB() : lIf->GetTrueCaseBB());
        /* LIE */ irBuilder->irbsEval(lIf->GetExpr()->GetRecCopyExpr(), ICStmt(lIf, ICBefore), extr.eGetSrcLoc(lIf));
        lIf->ReplaceWith(irBuilder->irbsJump(lTargetBB, ICNull(), extr.eGetSrcLoc(lIf)));
        STATRECORD("ifsimpls: const condition");
        ++ retVal;
      } // if
    } else if (lIf->GetFalseCaseBB() == lIf->GetTrueCaseBB()) {
      if (GetOptions().obGet("do_same_thenelse") == true) {
        /* LIE */ irBuilder->irbsEval(lIf->GetExpr()->GetRecCopyExpr(), ICStmt(lIf, ICBefore), extr.eGetSrcLoc(lIf));
        lIf->ReplaceWith(irBuilder->irbsJump(lIf->GetTrueCaseBB(), ICNull(), extr.eGetSrcLoc(lIf)));
        STATRECORD("ifsimpls: same then/else block");
        ++ retVal;
      } // if
    } else {
      if (pred.pIsEmpty(lIf->GetFalseCaseBB()) == true) {
        if (GetOptions().obGet("do_empty_thenelse") == true) {
          /* LIE */ irBuilder->irbsEval(lIf->GetExpr()->GetRecCopyExpr(), ICStmt(lIf, ICBefore), extr.eGetSrcLoc(lIf));
          lIf->ReplaceWith(irBuilder->irbsJump(lIf->GetTrueCaseBB(), ICNull(), extr.eGetSrcLoc(lIf)));
          ++ retVal;
          STATRECORD("ifsimpls: empty then/else block");
        } // if
      } else if (pred.pIsEmpty(lIf->GetTrueCaseBB()) == true) {
        if (GetOptions().obGet("do_empty_thenelse") == true) {
          /* LIE */ irBuilder->irbsEval(lIf->GetExpr()->GetRecCopyExpr(), ICStmt(lIf, ICBefore), extr.eGetSrcLoc(lIf));
          lIf->ReplaceWith(irBuilder->irbsJump(lIf->GetFalseCaseBB(), ICNull(), extr.eGetSrcLoc(lIf)));
          STATRECORD("ifsimpls: empty then/else block");
          ++ retVal;
        } // if
      } else {
        if (GetOptions().obGet("do_almost_empty_thenelse") == true) {
          if (pred.pIsAlmostEmpty(lIf->GetFalseCaseBB()) == true) {
            STATRECORD("ifsimpls: almost empty then/else block");
            anatropManager->Trigger("JumpOptims", lIf->GetFalseCaseBB());
          } // if
          if (pred.pIsAlmostEmpty(lIf->GetTrueCaseBB()) == true) {
            STATRECORD("ifsimpls: almost empty then/else block");
            anatropManager->Trigger("JumpOptims", lIf->GetTrueCaseBB());
          } // if
        } // if
      } // if
    } // if
    if (retVal != 0) {
      // Try to get some other cases.
      anatropManager->Trigger("IfSimplifications", lIf);
    } // if
  } // if
  //! \todo M Opt: Have a look at same conditional expressions cases for if threads.
  EMDEBUG1(retVal);
  return retVal;
} // ATOIfSimplifications::AnatropDo

hFUInt32 ATOIfSimplifications::
doConditionMerge(IRSIf* if_) {
  BMDEBUG2("ATOIfSimplifications::doConditionMerge", if_);
  hFUInt32 retVal(0);
  IRBB* lTrueBB(if_->GetTrueCaseBB());
  IRBB* lFalseBB(if_->GetFalseCaseBB());
  if (GetOptions().obGet("do_cond_merge") == false) {
    // LIE: Skip the optimization.
  } else if (extr.eGetNumOfStmts(lTrueBB) == 1 && pred.pIsIf(lTrueBB->GetLastStmt()) == true) {
    IRSIf* lIf2(static_cast<IRSIf*>(lTrueBB->GetLastStmt()));
    if (extr.eGetNumOfPreds(lTrueBB) != 1) {
      // LIE: We cannot do a merge.
    } else if (lIf2->GetTrueCaseBB() == lFalseBB) {
      STATRECORD("ifsimpls: condition merge 0");
      IRExpr* lCond1(if_->GetExpr()->GetRecCopyExpr());
      IRExpr* lCond2(lIf2->GetExpr()->GetRecCopyExpr());
      if_->GetExpr()->ReplaceWith(irBuilder->irbeLAnd(lCond1, irBuilder->irbeNot(lCond2)));
      lIf2->ReplaceWith(irBuilder->irbsJump(lIf2->GetFalseCaseBB(), ICNull(), extr.eGetSrcLoc(lIf2)));
      ++ retVal;
    } else if (lIf2->GetFalseCaseBB() == lFalseBB) {
      STATRECORD("ifsimpls: condition merge 1");
      IRExpr* lCond1(if_->GetExpr()->GetRecCopyExpr());
      IRExpr* lCond2(lIf2->GetExpr()->GetRecCopyExpr());
      if_->GetExpr()->ReplaceWith(irBuilder->irbeLAnd(lCond1, lCond2));
      lIf2->ReplaceWith(irBuilder->irbsJump(lIf2->GetTrueCaseBB(), ICNull(), extr.eGetSrcLoc(lIf2)));
      ++ retVal;
    } else { 
      // LIE: We cannot do a merge.
    } // if
  } else if (extr.eGetNumOfStmts(lFalseBB) == 1 && pred.pIsIf(lFalseBB->GetLastStmt()) == true) {
    IRSIf* lIf2(static_cast<IRSIf*>(lFalseBB->GetLastStmt()));
    if (extr.eGetNumOfPreds(lFalseBB) != 1) {
      // LIE: We cannot do a merge.
    } else if (lIf2->GetTrueCaseBB() == lTrueBB) {
      STATRECORD("ifsimpls: condition merge 2");
      IRExpr* lCond1(if_->GetExpr()->GetRecCopyExpr());
      IRExpr* lCond2(lIf2->GetExpr()->GetRecCopyExpr());
      if_->GetExpr()->ReplaceWith(irBuilder->irbeLOr(lCond1, lCond2));
      lIf2->ReplaceWith(irBuilder->irbsJump(lIf2->GetFalseCaseBB(), ICNull(), extr.eGetSrcLoc(lIf2)));
      ++ retVal;
    } else if (lIf2->GetFalseCaseBB() == lTrueBB) {
      STATRECORD("ifsimpls: condition merge 3");
      IRExpr* lCond1(if_->GetExpr()->GetRecCopyExpr());
      IRExpr* lCond2(lIf2->GetExpr()->GetRecCopyExpr());
      if_->GetExpr()->ReplaceWith(irBuilder->irbeLOr(lCond1, irBuilder->irbeNot(lCond2)));
      lIf2->ReplaceWith(irBuilder->irbsJump(lIf2->GetTrueCaseBB(), ICNull(), extr.eGetSrcLoc(lIf2)));
      ++ retVal;
    } else { 
      // LIE: We cannot do a merge.
    } // if
  } // if
  if (retVal != 0) {
    if (extr.eGetNumOfPreds(if_->GetBB()) == 1) {
      anatropManager->Trigger("IfSimplifications", extr.eGetPred(if_->GetBB()));
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOIfSimplifications::doConditionMerge


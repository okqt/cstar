// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_STRENGTHREDUCE_HPP__
#include "ato_strengthreduce.hpp"
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

hFInt32 ATOStrengthReduce::
AnatropDo(IRStmt* stmt_) {
  BMDEBUG2("ATOStrengthReduce::AnatropDo(IRStmt*)", stmt_);
  hFUInt32 retVal(0);
  //! \todo M Design: Requiree constfold before here so that all constants are
  //!       on the right hand side.
  ExprCollector3<IREMul, IREDiv, IREMod> lStrongExprs(EVOTPost, stmt_);
  { // Strength reduce multiplications by 2^n -> m << n
    for (hFUInt32 c(0); c < lStrongExprs.mCollected0.size(); ++ c) {
      if (pred.pIsAttached(lStrongExprs.mCollected0[c]) == true) {
        if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(lStrongExprs.mCollected0[c]->GetRightExpr())) {
          hFSInt32 lPow(util.uIsPowOf2(lRight->GetValue()));
          if (lPow != -1) {
            lStrongExprs.mCollected0[c]->ReplaceWith(irhBuilder->irbeShiftLeft(lStrongExprs.mCollected0[c]->GetLeftExpr(), lPow));
            STATRECORD("strength: count");
          } // if
        } // if
      } // if
    } // for
  } // block
  { // Strength reduce divisions by 2^n -> m >> n
    for (hFUInt32 c(0); c < lStrongExprs.mCollected1.size(); ++ c) {
      if (pred.pIsAttached(lStrongExprs.mCollected1[c]) == true) {
        if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(lStrongExprs.mCollected1[c]->GetRightExpr())) {
          hFSInt32 lPow(util.uIsPowOf2(lRight->GetValue()));
          if (lPow != -1) {
            lStrongExprs.mCollected1[c]->ReplaceWith(irhBuilder->irbeAShiftRight(lStrongExprs.mCollected1[c]->GetLeftExpr(), lPow));
            STATRECORD("strength: count");
          } // if
        } // if
      } // if
    } // for
  } // block
  { // Strength reduce modulos by 2^n -> m&(2^n-1)
    for (hFUInt32 c(0); c < lStrongExprs.mCollected2.size(); ++ c) {
      if (pred.pIsAttached(lStrongExprs.mCollected2[c]) == true) {
        if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(lStrongExprs.mCollected2[c]->GetRightExpr())) {
          hFSInt32 lPow(util.uIsPowOf2(lRight->GetValue()));
          if (lPow != -1) {
            lStrongExprs.mCollected2[c]->ReplaceWith(irBuilder->irbeBAnd(lStrongExprs.mCollected2[c]->GetLeftExpr(), 
              irBuilder->irbeIntConst(static_cast<IRTInt*>(lRight->GetType()), (1<<lPow)-1)));
            STATRECORD("strength: count");
          } // if
        } // if
      } // if
    } // for
  } // block
  EMDEBUG1(retVal);
  return retVal;
} // ATOStrengthReduce::AnatropDo



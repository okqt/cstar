// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __DFA_INLINE_CPP__
#define __DFA_INLINE_CPP__

// \file dfa_inline.cpp This file is to be included from dfa.hpp.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

template<class P, class D>
fBitVector& DFA<P, D>::
GetOutSet(IRBB* bb_) {
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAOutSet;
} // DFA::GetOutSet

template<class P, class D>
bool DFA<P, D>::
HasIndexInTop(const TopElemType& dfaProgPnt_, hFUInt32& index_) const {
  BMDEBUG1("DFA::HasIndexInTop");
  bool retVal(false);
  for (hFUInt32 c(0); c < mTopVector.size(); ++ c) {
    if (*mTopVector[c] == dfaProgPnt_) {
      index_ = c;
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFA::HasIndexInTop

template<class P, class D>
hFSInt32 DFA<P, D>::
GetIndexInTop(const TopElemType& dfaProgPnt_) const {
  BMDEBUG1("DFA::GetIndexInTop");
  hFSInt32 retVal(-1);
  for (hFUInt32 c(0); c < mTopVector.size(); ++ c) {
    if (*mTopVector[c] == dfaProgPnt_) {
      retVal = c;
      break;
    } // if
  } // for
  ENSUREMSG(retVal != -1, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // DFA::GetIndexInTop

template<class P, class D>
void DFA<P, D>::
Do() {
  BMDEBUG1("DFA::Do");
  STATRECORD(GetName() + ": calculated");
  IRBB* lStartBB;
  vector<IRBB*> lWorkList;
  // Traverse the mFunc IR and build up the top vector.
  calcTopVector(mTopVector);
  calcBottomVector(); // mBottomVector is calculated from mTopVector.
  fBitVector* lInitVector(0);
  fBitVector lTopVector;
  lTopVector = mBottomVector;
  SetAsTop(lTopVector);
  if (!mInitIsTop) {
    lInitVector = &mBottomVector;
  } else {
    lInitVector = &lTopVector;
  } // if
  if (mDir == eDFADBackward) {
    lStartBB = mFunc->GetCFG()->GetExitBB();
    mDFASetsIndex = lStartBB->getDFASetId();
    GetOutSet(lStartBB) = *lInitVector;
    GetInSet(lStartBB) = mBottomVector;
    { // call analysis function to init gen/kill sets
      fBitVector lIgnoreResultSet(mBottomVector);
      AnalysisFunc(lStartBB, mBottomVector, lIgnoreResultSet);
    } // block
    // Post order is faster for backward analysis
    extr.eGetPostOrderBBs(mFunc->GetCFG(), lWorkList);
  } else if (mDir == eDFADForward) {
    lStartBB = mFunc->GetCFG()->GetEntryBB();
    mDFASetsIndex = lStartBB->getDFASetId();
    GetInSet(lStartBB) = *lInitVector;
    GetOutSet(lStartBB) = mBottomVector;
    { // call analysis function to init gen/kill sets
      fBitVector lIgnoreResultSet(mBottomVector);
      AnalysisFunc(lStartBB, mBottomVector, lIgnoreResultSet);
    } // block
    // Reverse post order is faster for forward analysis
    extr.eGetRevPostOrderBBs(mFunc->GetCFG(), lWorkList);
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  lWorkList.erase(find(lWorkList.begin(), lWorkList.end(), lStartBB));
  ListIterator<IRBB*> lBBIter(mFunc->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    //! \todo M Design: Check if we can remove the above initialization of
    //!       lStartBB, and do it here.
    if (*lBBIter != lStartBB) {
      //! \todo M Design: rename the getDFASetId to something related to alloc.
      //!       It needs to be called for every BB in cfg.
      lBBIter->getDFASetId();
      GetKillSet(*lBBIter) = mBottomVector;
      GetGenSet(*lBBIter) = mBottomVector;
      if (mDir == eDFADForward) {
        GetInSet(*lBBIter) = *lInitVector;
        // Below initalization need not happen, do it just to have a clean dump.
        GetOutSet(*lBBIter) = mBottomVector;
      } else {
        GetOutSet(*lBBIter) = *lInitVector;
        // Below initalization need not happen, do it just to have a clean dump.
        GetInSet(*lBBIter) = mBottomVector;
      } // if
    } // if
  } // for
  //! \todo H Design: duplicate the while for forward and backward flows.
  while (!lWorkList.empty()) {
    IRBB* lCurrBB(lWorkList.back());
    lWorkList.pop_back();
    // effectInit is TOP for eDFACTAnd, and BOTTOM for eDFACTOr.
    fBitVector lTotalEffect(mCombineOp == eDFACTAnd ? lTopVector : mBottomVector);
    { // Calculate the total effect
      vector<IRBB*> lNextBBs;
      if (mDir == eDFADForward) {
        extr.eGetPredsOfBB(lCurrBB, lNextBBs);
      } else {
        extr.eGetSuccsOfBB(lCurrBB, lNextBBs);
      } // if
      for (hFUInt32 c(0); c < lNextBBs.size(); ++ c) {
        IRBB* lNextBB(lNextBBs[c]);
        fBitVector* lOutSet(mDir == eDFADForward ? &GetOutSet(lNextBB) : &GetInSet(lNextBB));
        fBitVector* lInSet(mDir == eDFADForward ? &GetInSet(lNextBB) : &GetOutSet(lNextBB));
        AnalysisFunc(lNextBB, *lInSet, *lOutSet);
        if (mCombineOp == eDFACTOr) {
          lTotalEffect |= *lOutSet;
        } else if (mCombineOp == eDFACTAnd) {
          lTotalEffect &= *lOutSet;
        } // if
      } // for
    } // block
    fBitVector* lCurrDFASet(mDir == eDFADForward ? &GetInSet(lCurrBB) : &GetOutSet(lCurrBB));
    if (lTotalEffect != *lCurrDFASet) {
      *lCurrDFASet = lTotalEffect;
      vector<IRBB*> lNextWLBBs;
      if (mDir == eDFADForward) {
        extr.eGetSuccsOfBB(lCurrBB, lNextWLBBs);
      } else {
        extr.eGetPredsOfBB(lCurrBB, lNextWLBBs);
      } // if
      for (hFUInt32 c(0); c < lNextWLBBs.size(); ++ c) {
        lWorkList.push_back(lNextWLBBs[c]);
      } // for
    } // if
  } // while
  EMDEBUG0();
  return;
} // DFA::Do

template<class P, class D>
set<Instruction*>& DFA<P, D>::
GetDefs(Register* reg_) const {
  return reg_->mDefInsts;
} // DFA::GetDefs

template<class P, class D>
const fBitVector& DFA<P, D>::
GetKillSet(const IRBB* bb_) const {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAKillSet;
} // DFA::GetKillSet

template<class P, class D>
fBitVector& DFA<P, D>::
GetKillSet(IRBB* bb_) {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAKillSet;
} // DFA::GetKillSet

template<class P, class D>
const fBitVector& DFA<P, D>::
GetGenSet(const IRBB* bb_) const {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAGenSet;
} // DFA::GetGenSet

template<class P, class D>
fBitVector& DFA<P, D>::
GetGenSet(IRBB* bb_) {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAGenSet;
} // DFA::GetGenSet

template<class P, class D>
void DFA<P, D>::
SetInited(IRBB* bb_) {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  bb_->mDFASets[mDFASetsIndex].mInited = true;
} // DFA::SetInited

template<class P, class D>
bool DFA<P, D>::
IsInited(IRBB* bb_) const {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mInited == true;
} // DFA::IsInited

template<class P, class D>
const fBitVector& DFA<P, D>::
GetInSet(const IRBB* bb_) const {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAInSet;
} // DFA::GetInSet

template<class P, class D>
fBitVector& DFA<P, D>::
GetInSet(IRBB* bb_) {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAInSet;
} // DFA::GetInSet

template<class P, class D>
const fBitVector& DFA<P, D>::
GetOutSet(const IRBB* bb_) const {
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mDFASetsIndex >= 0, ieStrInconsistentInternalStructure);
  REQUIREDMSG((hFSInt32)bb_->mDFASets.size() > mDFASetsIndex, ieStrInconsistentInternalStructure);
  return bb_->mDFASets[mDFASetsIndex].mDFAOutSet;
} // DFA::GetOutSet

template<class P, class D>
char* DFAProgPntTopElem<P, D>::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DFAProgPntTopElem::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpDFA) {
    if (typeid(Instruction) == typeid(*mPoint)) {
      Instruction* lInst(reinterpret_cast<Instruction*>(mPoint));
      ASMSection lASMSection(&toStr_);
      const ASMDesc& lASMDesc(*debugTrace->GetASMDesc());
      toStr_ << mData << ", ";
      lInst->GetDisassembly(lASMSection, lASMDesc);
    } else if (typeid(IRStmt) == typeid(*mPoint)) {
      toStr_ << "<progpnt:stmt>";
    } else {
      toStr_ << "<progpnt:unknown>";
    } // if
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ ;
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DFAProgPnt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DFAProgPntTopElem::debug

extern string getRuler(hFSInt32 size_, hFSInt32 markAt_, hFSInt32 upperMark_);

template<class P, class D>
char* DFA<P, D>::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DFA::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpDFA) {
    DTContext lDTContext(context_);
    toStr_ << "DFA: " << GetName().c_str() << " { // " << mFunc->GetName().c_str() << ++ DTCCIndent << DTCCEndLine;
    toStr_ << DTCCIndent << "Direction: " << (mDir == eDFADForward ? "Forward" : "Backward") << DTCCEndLine;
    toStr_ << DTCCIndent << "Level: " << (mIsIRLevel == true ? "IR" : "Instruction") << DTCCEndLine;
    toStr_ << DTCCIndent << "Confluence Operator: " << (mCombineOp == eDFACTOr ? "Or" : "And") << DTCCEndLine;
    toStr_ << DTCCIndent << "Top vector: " << DTCCEndLine;
    for (hFUInt32 c(0); c < mTopVector.size(); c ++) {
      toStr_ << DTCCIndent << "  " << c << ": ";
      mTopVector[c]->debug(context_ + DTCCDumpDFA, toStr_);
      toStr_ << DTCCEndLine;
    } // for
    ListIterator<IRBB*> lBBIter(mFunc->GetCFG()->GetPhysBBIter());
    for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
      toStr_ << DTCCIndent << refcxt(*lBBIter) << " {" << ++ DTCCIndent << DTCCEndLine;
      { // dump preds
        toStr_ << DTCCIndent << "Preds: ";
        vector<IRBB*> lPreds;
        if (!extr.eGetPredsOfBB(*lBBIter, lPreds)) {
          toStr_ << "<none>";
        } else {
          for (hFUInt32 c(0); c < lPreds.size(); ++ c) {
            if (c != 0) {
              toStr_ << ", ";
            } // if
            toStr_ << refcxt(lPreds[c]);
          } // for
        } // if
        toStr_ << DTCCEndLine;
      } // block
      { // dump succs
        toStr_ << DTCCIndent << "Succs: ";
        vector<IRBB*> lSuccs;
        if (!extr.eGetSuccsOfBB(*lBBIter, lSuccs)) {
          toStr_ << "<none>";
        } else {
          for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
            if (c != 0) {
              toStr_ << ", ";
            } // if
            toStr_ << refcxt(lSuccs[c]);
          } // for
        } // if
        toStr_ << DTCCEndLine;
      } // block
      { // dump bit vectors
        const fBitVector& lKillSet(GetKillSet(*lBBIter));
        const fBitVector& lGenSet(GetGenSet(*lBBIter));
        const fBitVector& lInSet(GetInSet(*lBBIter));
        const fBitVector& lOutSet(GetOutSet(*lBBIter));
        hFUInt32 c;
        { // Put a ruler for indexes.
          //! \note Spaces must match to the length of "kill:".
          if (lKillSet.GetSize() < 10) {
            toStr_ << DTCCIndent << "     " << getRuler(lKillSet.GetSize(), 1, 10).c_str() << DTCCEndLine;
          } else if (lKillSet.GetSize() < 100) {
            toStr_ << DTCCIndent << "     " << getRuler(lKillSet.GetSize(), 10, 100).c_str() << DTCCEndLine;
            toStr_ << DTCCIndent << "     " << getRuler(lKillSet.GetSize(), 1, 10).c_str() << DTCCEndLine;
          } else if (lKillSet.GetSize() < 1000) {
            toStr_ << DTCCIndent << "     " << getRuler(lKillSet.GetSize(), 100, 1000).c_str() << DTCCEndLine;
            toStr_ << DTCCIndent << "     " << getRuler(lKillSet.GetSize(), 10, 100).c_str() << DTCCEndLine;
            toStr_ << DTCCIndent << "     " << getRuler(lKillSet.GetSize(), 1, 10).c_str() << DTCCEndLine;
          } // if
        } // block
        toStr_ << DTCCIndent << "kill:" << lKillSet << DTCCEndLine;
        toStr_ << DTCCIndent << " gen:" << lGenSet << DTCCEndLine;
        toStr_ << DTCCIndent << "  in:" << lInSet << DTCCEndLine;
        toStr_ << DTCCIndent << " out:" << lOutSet << DTCCEndLine;
      } // block
      if (IsInstLevel() == true) {
        const CGBB* lCGBB(static_cast<const CGBB*>(*lBBIter));
        if (lCGBB->mInsts.size() == 0) {
          toStr_ << DTCCIndent << "Insts: None" << DTCCEndLine;
        } else {
          toStr_ << DTCCIndent << "Insts: {" << ++ DTCCIndent << DTCCEndLine;
          for (hFUInt32 c(0); c < lCGBB->mInsts.size(); ++ c) {
            toStr_ << DTCCIndent;
            if (Instruction* lInst = dynamic_cast<Instruction*>(lCGBB->mInsts[c])) {
              BitVector lProgPnts;
              GetProgPnts(lInst, lProgPnts);
              for (hFUInt32 d(0); d < lProgPnts.size(); d ++) {
                if (d != 0) {
                  toStr_ << ", ";
                } // if
                toStr_ << lProgPnts[d].GetIndex();
              } // for
              toStr_ << " ";
            } // if
            toStr_ << refcxt(lCGBB->mInsts[c]) << " ";
            ASMSection lASMSection(&toStr_);
            lCGBB->mInsts[c]->GetDisassembly(lASMSection, *debugTrace->GetASMDesc());
            toStr_ << DTCCEndLine;
          } // for
          toStr_ << -- DTCCIndent << DTCCIndent << "} // Insts" << DTCCEndLine;
        } // if
      } else if (IsIRLevel() == true) {
        if (pred.pIsEmpty(*lBBIter) == true) {
          toStr_ << DTCCIndent << "Stmts: None";
        } else {
          toStr_ << DTCCIndent << "Stmts: {" << ++ DTCCIndent << DTCCEndLine;
          ListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtIter());
          for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
            toStr_ << DTCCIndent << progcxt(*lStmtIter) << DTCCEndLine;
          } // for
          toStr_ << -- DTCCIndent << DTCCIndent << "} // Stmts" << DTCCEndLine;
        } // if
      } else {
        ASSERTMSG(0, ieStrPCShouldNotReachHere);
      } // if
      toStr_ << -- DTCCIndent << DTCCIndent << "} // " << refcxt(*lBBIter) << DTCCEndLine;
    } // for
    toStr_ << -- DTCCIndent << DTCCIndent << "} // DFA " << GetName().c_str() << DTCCEndLine;
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DFA::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DFA::debug

#endif


// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __DFA_HPP__
#include "dfa.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

string
getRuler(hFSInt32 size_, hFSInt32 markAt_, hFSInt32 upperMark_) {
  ostringstream retVal;
  hFSInt32 currMark = markAt_;
  retVal << "0";
  -- size_;
  while (size_ >= markAt_) {
    retVal.setf(ios_base::right);
    retVal.width(markAt_);
    retVal << currMark % upperMark_;
    currMark += markAt_;
    size_ -= markAt_;
  } // while
  retVal << ends;
  return retVal.str();
} // getRuler

//! \brief Compares to bit vectors.
//! \pre Bit vectors must have the same size.
bool
operator != (const BitVector& leftVector, const BitVector& rightVector) {
  REQUIREDMSG(leftVector.size() == rightVector.size(), ieStrInconsistentInternalStructure);
  bool retVal(false);
  for (hFUInt32 c(0); c < leftVector.size(); ++ c) {
    if (leftVector[c] != rightVector[c]) {
      retVal = true;
      break;
    } // if
  } // for
  return retVal;
} // operator !=

fBitVector 
operator | (const fBitVector& leftVector_, const fBitVector& rightVector_) {
  REQUIREDMSG((leftVector_.mSize == 0 && rightVector_.mSize == 0) || 
    (leftVector_.mBits != 0 && rightVector_.mBits != 0), ieStrParamValuesDBCViolation);
  REQUIREDMSG(leftVector_.mSize == rightVector_.mSize, ieStrParamValuesDBCViolation);
  fBitVector retVal(leftVector_.GetSize());
  for (hFUInt32 c(0); c < leftVector_.mElemCount; ++ c) {
    retVal.mBits[c] = leftVector_.mBits[c] | rightVector_.mBits[c];
  } // for
  return retVal;
} // operator |

ostream& operator << (ostream& o, const fBitVector& bv_) {
  o << bv_.GetSize() << "|";
  for (hFUInt32 c(0); c < bv_.GetSize(); ++ c) {
    o << bv_.GetBit(c);
  } // for
  return o;
} // operator <<

fBitVector 
operator & (const fBitVector& leftVector_, const fBitVector& rightVector_) {
  REQUIREDMSG((leftVector_.mSize == 0 && rightVector_.mSize == 0) || 
    (leftVector_.mBits != 0 && rightVector_.mBits != 0), ieStrParamValuesDBCViolation);
  REQUIREDMSG(leftVector_.mSize == rightVector_.mSize, ieStrParamValuesDBCViolation);
  fBitVector retVal(leftVector_.GetSize());
  for (hFUInt32 c(0); c < leftVector_.mElemCount; ++ c) {
    retVal.mBits[c] = leftVector_.mBits[c] & rightVector_.mBits[c];
  } // for
  return retVal;
} // operator &

fBitVector 
operator - (const fBitVector& leftVector_, const fBitVector& rightVector_) {
  REQUIREDMSG((leftVector_.mSize == 0 && rightVector_.mSize == 0) || 
    (leftVector_.mBits != 0 && rightVector_.mBits != 0), ieStrParamValuesDBCViolation);
  REQUIREDMSG(leftVector_.mSize == rightVector_.mSize, ieStrParamValuesDBCViolation);
  fBitVector retVal(leftVector_.GetSize());
  for (hFUInt32 c(0); c < leftVector_.mElemCount; ++ c) {
    retVal.mBits[c] = leftVector_.mBits[c] & ~(rightVector_.mBits[c]);
    PDEBUG("sub", "detail", c << "->" << retVal.mElemCount << "|" << retVal << endl << leftVector_ << endl << rightVector_);
  } // for
  return retVal;
} // operator -

BitVector
operator | (const BitVector& leftVector_, const BitVector& rightVector_) {
  REQUIREDMSG(leftVector_.size() == rightVector_.size(), ieStrInconsistentInternalStructure);
  BitVector retVal(leftVector_);
  for (hFUInt32 c(0); c < leftVector_.size(); ++ c) {
    if (!rightVector_[c].IsBottom()) {
      retVal[c] = rightVector_[c];
    } // if
  } // for
  return retVal;
} // operator |

BitVector
operator & (const BitVector& leftVector_, const BitVector& rightVector_) {
  REQUIREDMSG(leftVector_.size() == rightVector_.size(), ieStrInconsistentInternalStructure);
  BitVector retVal(leftVector_);
  for (hFUInt32 c(0); c < leftVector_.size(); ++ c) {
    if (rightVector_[c].IsBottom() == true) {
      retVal[c].MakeBottom();
    } // if
  } // for
  return retVal;
} // operator &

BitVector
operator - (const BitVector& leftVector_, const BitVector& rightVector_) {
  REQUIREDMSG(leftVector_.size() == rightVector_.size(), ieStrInconsistentInternalStructure);
  BitVector retVal(leftVector_);
  for (hFUInt32 c(0); c < leftVector_.size(); ++ c) {
    if (!rightVector_[c].IsBottom()) {
      retVal[c].MakeBottom();
    } // if
  } // for
  return retVal;
} // operator -

void
operator &= (BitVector& leftVector_, const BitVector& rightVector_) {
  REQUIREDMSG(leftVector_.size() == rightVector_.size(), ieStrInconsistentInternalStructure);
  for (hFUInt32 c(0); c < leftVector_.size(); ++ c) {
    if (rightVector_[c].IsBottom() == true) {
      leftVector_[c].MakeBottom();
    } // if
  } // for
  return;
} // operator &=

void
operator |= (BitVector& leftVector_, const BitVector& rightVector_) {
  REQUIREDMSG(leftVector_.size() == rightVector_.size(), ieStrInconsistentInternalStructure);
  for (hFUInt32 c(0); c < leftVector_.size(); ++ c) {
    if (!rightVector_[c].IsBottom()) {
      leftVector_[c].MakeTop();
    } // if
  } // for
  return;
} // operator |=

ostream&
operator << (ostream& o, const BitVector& bv_) {
  for (hFUInt32 c(0); c < bv_.size(); ++ c) {
    if (!bv_[c].IsBottom()) {
      o << "1";
    } else {
      o << "0";
    } // if
  } // for
  return DBGFLUSH(o);
} // operator <<

bool DFAInstLiveness::
IsLiveAtEnd(IRBB* bb_, Instruction* inst_, Register* reg_) const {
  BMDEBUG4("DFAInstLiveness::IsLiveAtEnd", bb_, inst_, reg_);
  bool retVal(false);
  const fBitVector& lOutSet(GetOutSet(bb_));
  for (hFUInt32 c(0); c < lOutSet.GetSize(); ++ c) {
    if (!lOutSet.IsBottom(c) && *GetTopVector()[c] == TopElemType(inst_, reg_, 0)) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAInstLiveness::IsLiveAtEnd

bool DFAInstLiveness::
IsLiveAtBeg(IRBB* bb_, Instruction* inst_, Register* reg_) const {
  BMDEBUG4("DFAInstLiveness::IsLiveAtEnd", bb_, inst_, reg_);
  bool retVal(false);
  const fBitVector& lInSet(GetInSet(bb_));
  for (hFUInt32 c(0); c < lInSet.GetSize(); ++ c) {
    if (!lInSet.IsBottom(c) && *GetTopVector()[c] == TopElemType(inst_, reg_, 0)) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAInstLiveness::IsLiveAtBeg

void DFAInstLiveness::
calcTopVector(TopBitVector& topVector_) {
  BMDEBUG1("DFAInstLiveness::calcTopVector");
  REQUIREDMSG(topVector_.empty() == true, ieStrParamValuesDBCViolation);
  ListIterator<IRBB*> lBBIter(GetFunc()->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    CGBB* lBB = static_cast<CGBB*>(*lBBIter);
    vector<AsmSequenceItem*> lInsts;
    lBB->GetInsts(lInsts);
    for (hFUInt32 c(0); c < lInsts.size(); ++ c) {
      if (Instruction* lInst = dynamic_cast<Instruction*>(lInsts[c])) {
        set<Register*> lRegs;
        lInst->GetUses().GetRegs(lRegs);
        for (set<Register*>::iterator lIt(lRegs.begin()); lIt != lRegs.end(); ++ lIt) {
          topVector_.push_back(new TopElemType(lInst, *lIt, topVector_.size()));
        } // for
      } // if
    } // for
  } // for
  EMDEBUG1(topVector_.size());
  return;
} // DFAInstLiveness::calcTopVector

void DFAInstLiveness::
AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_) {
  BMDEBUG2("DFAInstLiveness::AnalysisFunc", bb_);
  CGBB* cgBB(static_cast<CGBB*>(bb_));
  // In liveness analysis GenSet corresponds to useSet,
  // KillSet corresponds to defSet.
  fBitVector& defSet(GetKillSet(bb_));
  fBitVector& useSet(GetGenSet(bb_));
  if (!IsInited(bb_)) {
    SetInited(bb_);
    // Generate useSet and defSet.
    // A variable is in useSet if it has a use in bb_
    // prior to any def scanning from the start to end of bb_.
    // A variable is in defSet if it has a def in bb_
    // prior to any use scanning from the start to end of bb_.
    useSet = GetBottomVector();
    defSet = GetBottomVector();
    vector<AsmSequenceItem*> lInsts;
    cgBB->GetInsts(lInsts);
    set<Register*> lDefsSeenInBB;
    set<Register*> usesSeenInBB;
    const TopBitVector& lTopVector(GetTopVector());
    for (hFUInt32 c(0); c < lInsts.size(); ++ c) {
      if (Instruction* lInst = dynamic_cast<Instruction*>(lInsts[c])) {
        const set<Register*>& defs(lInst->GetDefs().GetRegs());
        const set<Register*>& uses(lInst->GetUses().GetRegs());
        // In case x <- x, 'use' takes precedence, so do uses first.
        for (set<Register*>::const_iterator uit(uses.begin()); uit != uses.end(); ++ uit) {
          if (lDefsSeenInBB.find(*uit) == lDefsSeenInBB.end()) {
            hFUInt32 lIndex(GetIndexInTop(TopElemType(lInst, *uit, 0)));
            useSet.MakeTop(lIndex);
          } // if
          usesSeenInBB.insert(*uit);
          if (dynamic_cast<VirtReg*>(*uit) != 0) {
            mVirtRegsSeen.insert(static_cast<VirtReg*>(*uit));
          } // if
        } // for
        for (set<Register*>::const_iterator dit(defs.begin()); dit != defs.end(); ++ dit) {
          if (usesSeenInBB.find(*dit) == usesSeenInBB.end()) {
            for (hFUInt32 d(0); d < lTopVector.size(); ++ d) {
              if (*lTopVector[d] == *dit) {
                defSet.MakeTop(d);
              } // if
            } // for
          } // if
          lDefsSeenInBB.insert(*dit);
          if (dynamic_cast<VirtReg*>(*dit) != 0) {
            mVirtRegsSeen.insert(static_cast<VirtReg*>(*dit));
          } // if
          GetDefs(*dit).insert(lInst);
        } // for
      } // if
    } // for
  } // if
  // defSet and useSet are ready here.
  PDEBUG("DFAInstLiveness", "detail", " res = use| (curr- def)" << resultSet_ << endl << useSet << endl << 
    currSet_ << endl << defSet << endl << (currSet_-defSet));
  resultSet_ = useSet | (currSet_ - defSet);
  EMDEBUG0();
  return;
} // DFAInstLiveness::AnalysisFunc

void DFAIRAvailExpressions::
AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_) {
  BMDEBUG2("DFAIRAvailExpressions::AnalysisFunc", bb_);
  fBitVector& lKillSet(GetKillSet(bb_));
  fBitVector& lEvalSet(GetGenSet(bb_));
  if (!IsInited(bb_)) {
    SetInited(bb_);
    lKillSet = GetBottomVector();
    lEvalSet = GetBottomVector();
    const TopBitVector& lTopVector(GetTopVector());
    ListIterator<IRStmt*> lStmtIter(bb_->GetStmtIter());
    // Determine the kill set of this basic block.
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      for (hFUInt32 c(0); c < lTopVector.size(); ++ c) {
        if (pred.pCanKill(*lStmtIter, lTopVector[c]->GetData()) == true) {
          lKillSet.MakeTop(c);
        } // if
      } // for
    } // for
    // Determine the eval set of this basic block.
    ExprCollector<IRExpr, IRBB> lExprCollector(EVOTPre, bb_, ExprVisitorOptions().SetLHSNo());
    for (hFUInt32 d(0); d < lTopVector.size(); ++ d) {
      for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
        if (AvailableExpressions<bool>::smSameExpr(lTopVector[d]->GetData(), lExprCollector[c]) == true) {
          lEvalSet.MakeTop(d);
          break;
        } // if
      } // for
    } // for
    PDEBUG("DFA", "detail", "GenSet = " << lEvalSet << " @" << GetFunc()->GetName() << " bb:" << bb_->GetObjId());
    PDEBUG("DFA", "detail", "KillSet = " << lKillSet);
  } // if
  resultSet_ = lEvalSet | (currSet_ - lKillSet);
  PDEBUG("DFA", "detail", "resultSet = " << resultSet_);
  EMDEBUG1(resultSet_.GetSize());
  return;
} // DFAIRAvailExpressions::AnalysisFunc

void DFAIRAvailExpressions::
calcTopVector(TopBitVector& topVector_) {
  BMDEBUG1("DFAIRAvailExpressions::calcTopVector");
  REQUIREDMSG(topVector_.empty() == true, ieStrParamValuesDBCViolation);
  ListIterator<IRBB*> lBBIter(GetFunc()->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    ListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtIter());
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      ExprCollector<IRExpr> lExprCollector(EVOTPre, *lStmtIter, ExprVisitorOptions().SetLHSNo());
      for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
        if (mIsOkExprFunc(lExprCollector[c]) == true) {
          mAvailExprs.Add(lExprCollector[c], hFSInt32_ignore);
        } // if
      } // for
    } // for
  } // for
  const vector<AvailableExpressions<bool>::AEtuple>& lAEs(mAvailExprs.GetExprs());
  for (hFUInt32 c(0); c < lAEs.size(); ++ c) {
    if (lAEs[c].mT6 == true) {
      // Note that the program point of an available expression is not single,
      // same expression may be in different program points, so use Invalid
      // statement to reduce the confusion.
      topVector_.push_back(new TopElemType(&Invalid<IRStmt>::Obj(), lAEs[c].mT0, topVector_.size()));
    } // for
  } // for
  EMDEBUG1(topVector_.size());
  return;
} // DFAIRAvailExpressions::calcTopVector

void DFAIRAvailExpressions::
GetAvailExprsInSet(IRBB* bb_, TopBitVector& availExprs_) {
  BMDEBUG2("DFAIRAvailExpressions::GetAvailExprsInSet", bb_);
  const TopBitVector& lTopVector(GetTopVector());
  const fBitVector& lInSet(GetInSet(bb_));
  for (hFUInt32 c(0); c < lInSet.GetSize(); ++ c) {
    if (lInSet.IsTop(c) == true) {
      availExprs_.push_back(new TopElemType(*lTopVector[c]));
    } // if
  } // for
  EMDEBUG1(availExprs_.size());
  return;
} // DFAIRAvailExpressions::GetAvailExprsInSet

void DFAIRCopyAssignments::
AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_) {
  BMDEBUG2("DFAIRCopyAssignments::AnalysisFunc", bb_);
  fBitVector& lKillSet(GetKillSet(bb_));
  fBitVector& lCopySet(GetGenSet(bb_));
  if (!IsInited(bb_)) {
    SetInited(bb_);
    lKillSet = GetBottomVector();
    lCopySet = GetBottomVector();
    const TopBitVector& lTopVector(GetTopVector());
    ListIterator<IRStmt*> lStmtIter(bb_->GetStmtIter());
    // Tuple: stmt, lhsObj, lhsExpr, rhsExpr, iskilled.
    // Determine the kill and copy set of this basic block.
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      for (hFUInt32 c(0); c < lTopVector.size(); ++ c) {
        DFAIRCopyAssignData* lData(lTopVector[c]->GetData());
        if (pred.pIsConst(lData->mT3) == true || lData->mT1 != extr.eGetObjOfUse(lData->mT3)) { // skip "a = a" like cases.
          if (pred.pCanKill(*lStmtIter, lData->mT2) == true) {
            lKillSet.MakeTop(c);
          } else if (pred.pCanKill(*lStmtIter, lData->mT3) == true) {
            lKillSet.MakeTop(c);
          } // if
          if (lData->mT0 == *lStmtIter) {
            lCopySet.MakeTop(c);
          } // if
        } // if
      } // for
    } // for
    PDEBUG("DFA", "detail", "GenSet = " << lCopySet << " @" << GetFunc()->GetName() << " bb:" << bb_->GetObjId());
    PDEBUG("DFA", "detail", "KillSet = " << lKillSet);
  } // if
  resultSet_ = lCopySet | (currSet_ - lKillSet);
  PDEBUG("DFA", "detail", "resultSet = " << resultSet_);
  EMDEBUG1(resultSet_.GetSize());
  return;
} // DFAIRCopyAssignments::AnalysisFunc

void DFAIRCopyAssignments::
calcTopVector(TopBitVector& topVector_) {
  BMDEBUG1("DFAIRCopyAssignments::calcTopVector");
  REQUIREDMSG(topVector_.empty() == true, ieStrParamValuesDBCViolation);
  ListIterator<IRBB*> lBBIter(GetFunc()->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    RevListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtRevIter());
    set<IRObject*> lAddedLHSObjs;
    set<IRObject*> lKilledLHSObjs;
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      if (pred.pIsCopyAssign(*lStmtIter) == true || pred.pIsConstAssign(*lStmtIter) == true) {
        IRSAssign* lAssign(static_cast<IRSAssign*>(*lStmtIter));
        IRObject* lLHSObj(extr.eGetObjOfAddrOf(lAssign->GetLHS()));
        // Only add the last copy assignment of an object in a bb.
        if (lAddedLHSObjs.find(lLHSObj) == lAddedLHSObjs.end() && lKilledLHSObjs.find(lLHSObj) == lKilledLHSObjs.end()) {
          lAddedLHSObjs.insert(lLHSObj);
          DFAIRCopyAssignData* lData(new DFAIRCopyAssignData(*lStmtIter, lLHSObj, irhBuilder->irbeUseOf(lLHSObj), 
            lAssign->GetRHS(), false));
          topVector_.push_back(new TopElemType(*lStmtIter, lData, topVector_.size()));
          PDEBUG("DFAIRCopyAssignments", "detail", " top vector[" << topVector_.size()-1 << "]=" << 
            progcxt(*lStmtIter) << endl << " lhsobj=" << refcxt(lLHSObj) << " rhs=" << refcxt(lAssign->GetRHS()));
        } // if
      } else {
        set<IRObject*> lDefs;
        extr.eGetDefsOfStmt(*lStmtIter, lDefs);
        lKilledLHSObjs.insert(lDefs.begin(), lDefs.end());
      } // if
    } // for
  } // for
  EMDEBUG1(topVector_.size());
  return;
} // DFAIRCopyAssignments::calcTopVector

void DFAIRCopyAssignments::
GetAvailCopiesInSet(IRBB* bb_, TopBitVector& availCopies_) {
  BMDEBUG2("DFAIRCopyAssignments::GetAvailCopiesInSet", bb_);
  const TopBitVector& lTopVector(GetTopVector());
  const fBitVector& lInSet(GetInSet(bb_));
  for (hFUInt32 c(0); c < lInSet.GetSize(); ++ c) {
    if (lInSet.IsTop(c) == true) {
      availCopies_.push_back(new TopElemType(*lTopVector[c]));
    } // if
  } // for
  EMDEBUG1(availCopies_.size());
  return;
} // DFAIRCopyAssignments::GetAvailCopiesInSet

void DFAIRReachingDefs::
AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_) {
  BMDEBUG2("DFAIRReachingDefs::AnalysisFunc", bb_);
  // For reaching definitions KillSet is PreserveSet.
  fBitVector& lPreserveSet(GetKillSet(bb_));
  fBitVector& lGenSet(GetGenSet(bb_));
  if (!IsInited(bb_)) {
    SetInited(bb_);
    // Generate lGenSet and lPreserveSet.
    // A variable is in lGenSet if it has a def in bb_
    // which is not subsequently killed in bb_ (scanning from the start to end of bb).
    // A variable is in lPreserveSet if it has no def in bb_.
    // It is better to initialize lPreserve to top vector and remove the defs
    // in bb_ from it.
    lGenSet = GetBottomVector();
    lPreserveSet = GetTopBitVector();
    const TopBitVector& lTopVector(GetTopVector());
    RevListIterator<IRStmt*> lStmtIter(bb_->GetStmtRevIter());
    set<IRObject*> lBBSinglyDefedObjs;
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      set<IRObject*> lDefs;
      extr.eGetDefsOfStmt(*lStmtIter, lDefs);
      // Statement may define only the objects listed in lDefs.
      for (set<IRObject*>::iterator lDefIt(lDefs.begin()); lDefIt != lDefs.end(); ++ lDefIt) {
        hFUInt32 lIndex;
        if (HasIndexInTop(TopElemType(*lStmtIter, *lDefIt, 0), lIndex) == true) {
          lGenSet.MakeTop(lIndex);
        } // if
        for (hFUInt32 c(0); c < lTopVector.size(); ++ c) {
          if (!lPreserveSet.IsBottom(c) && *lTopVector[c] == *lDefIt) {
            lPreserveSet.MakeBottom(c);
          } // if
        } // for
      } // for
    } // for
    PDEBUG("DFA", "detail", "GenSet = " << lGenSet << " @" << GetFunc()->GetName() << " bb:" << bb_->GetObjId());
    PDEBUG("DFA", "detail", "PrvSet = " << lPreserveSet);
  } // if
  // GenSet and KillSet/PreserveSet are ready here.
  resultSet_ = lGenSet | (currSet_ & lPreserveSet);
  EMDEBUG1(resultSet_.GetSize());
  return;
} // DFAIRReachingDefs::AnalysisFunc

void DFAIRReachingDefs::
calcTopVector(TopBitVector& topVector_) {
  BMDEBUG1("DFAIRReachingDefs::calcTopVector");
  REQUIREDMSG(topVector_.empty() == true, ieStrParamValuesDBCViolation);
  // Note do not record globals' definitions and record only the last definition within
  // a basic block for singly defined objects, in other words for each object there is a 
  // single definition per basic block.
  // For example:
  //   int a,b; int* p = &a, *q = &b; 
  //   *p = 1; // this stmt may define both a and b. So stmt must be in top vector.
  //   a = 3; // stmt has singly defined object a.
  //   a = 6; // stmt has singly defined object a.
  // Since we traverse backwards we do not record a = 3.
  ListIterator<IRBB*> lBBIter(GetFunc()->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    set<IRObject*> lBBSinglyDefedObjs;
    RevListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtRevIter());
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      set<IRObject*> lDefinedObjs;
      extr.eGetDefsOfStmt(*lStmtIter, lDefinedObjs);
      bool lSinglyDefined(lDefinedObjs.size() == 1);
      for (set<IRObject*>::iterator lDefIt(lDefinedObjs.begin()); lDefIt != lDefinedObjs.end(); ++ lDefIt) {
        PDEBUG("DFAIRReachingDefs", "detail", " topvector[" << topVector_.size() << 
            "]= " << progcxt(*lStmtIter) << " <-> " << progcxt(*lDefIt));
        if (lSinglyDefined == false) {
          topVector_.push_back(new TopElemType(*lStmtIter, *lDefIt, topVector_.size()));
        } else if (lBBSinglyDefedObjs.find(*lDefIt) == lBBSinglyDefedObjs.end()) {
          lBBSinglyDefedObjs.insert(*lDefIt);
          topVector_.push_back(new TopElemType(*lStmtIter, *lDefIt, topVector_.size()));
        } // if
      } // for
    } // for
  } // for
  EMDEBUG1(topVector_.size());
  return;
} // DFAIRReachingDefs::calcTopVector 

bool DFAIRReachingDefs::
IsDefinedIn(IRBB* bb_, IRObject* obj_) const {
  BMDEBUG3("DFAIRReachingDefs::IsDefinedIn", bb_, obj_);
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(obj_ != 0, ieStrParamValuesDBCViolation);
  bool retVal(false);
  const TopBitVector& lTopVector(GetTopVector());
  //! \todo M Design: Is it possible/better to use bb_ dfaIn/dfaOut sets?
  for (hFUInt32 c(0); c < lTopVector.size(); ++ c) {
    if (*lTopVector[c] == obj_ && lTopVector[c]->GetProgPoint()->GetBB() == bb_) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAIRReachingDefs::IsDefinedIn

bool DFAIRReachingDefs::
IsDefinedAtBeg(IRBB* bb_, IRObject* obj_) const {
  BMDEBUG3("DFAIRReachingDefs::IsDefinedAtBeg", bb_, obj_);
  bool retVal(false);
  const fBitVector& lInSet(GetInSet(bb_));
  const TopBitVector& lTopVector(GetTopVector());
  for (hFUInt32 c(0); c < lInSet.GetSize(); ++ c) {
    if (!lInSet.IsBottom(c) && *lTopVector[c] == obj_) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAIRReachingDefs::IsDefinedAtBeg

bool DFAIRReachingDefs::
IsDefinedAtEnd(IRBB* bb_, IRObject* obj_) const {
  BMDEBUG3("DFAIRReachingDefs::IsDefinedAtEnd", bb_, obj_);
  bool retVal(false);
  const fBitVector& lOutSet(GetOutSet(bb_));
  const TopBitVector& lTopVector(GetTopVector());
  for (hFUInt32 c(0); c < lOutSet.GetSize(); ++ c) {
    if (!lOutSet.IsBottom(c) && *lTopVector[c] == obj_) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAIRReachingDefs::IsDefinedAtEnd

void DFAInstReachingDefs::
AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_) {
  BMDEBUG2("DFAInstReachingDefs::AnalysisFunc", bb_);
  CGBB* cgBB(static_cast<CGBB*>(bb_));
  // For reaching definitions KillSet is PreserveSet.
  fBitVector& lPreserveSet(GetKillSet(bb_));
  fBitVector& lGenSet(GetGenSet(bb_));
  if (!IsInited(bb_)) {
    SetInited(bb_);
    // Generate lGenSet and lPreserveSet.
    // A variable is in lGenSet if it has a def in bb_
    // which is not subsequently killed in bb_ (scanning from the start to end of bb).
    // A variable is in lPreserveSet if it has no def in bb_.
    // It is better to initialize lPreserve to top vector and remove the defs
    // in bb_ from it.
    lGenSet = GetBottomVector();
    lPreserveSet = GetTopBitVector();
    vector<AsmSequenceItem*> lInsts;
    cgBB->GetInsts(lInsts);
    set<Register*> lDefsSeenInBB;
    const TopBitVector& lTopVector(GetTopVector());
    for (hFUInt32 c(0); c < lInsts.size(); ++ c) {
      if (Instruction* lInst = dynamic_cast<Instruction*>(lInsts[c])) {
        const set<Register*>& defs(lInst->GetDefs().GetRegs());
        for (set<Register*>::const_iterator dit(defs.begin()); dit != defs.end(); ++ dit) {
          for (hFUInt32 d(0); d < lTopVector.size(); ++ d) {
            if (!lPreserveSet.IsBottom(d) && *lTopVector[d] == *dit) {
              lPreserveSet.MakeBottom(d);
            } // if
          } // for
          hFUInt32 lIndex(GetIndexInTop(TopElemType(lInst, *dit, 0)));
          if (lDefsSeenInBB.find(*dit) == lDefsSeenInBB.end()) {
            lGenSet.MakeTop(lIndex);
          } else {
            lGenSet.MakeBottom(lIndex);
          } // if
          lDefsSeenInBB.insert(*dit);
          GetDefs(*dit).insert(lInst);
        } // for
      } // if
    } // for
    PDEBUG("DFA", "detail", "GenSet = " << lGenSet << " @" << GetFunc()->GetName() << " bb:" << bb_->GetObjId());
    PDEBUG("DFA", "detail", "PrvSet = " << lPreserveSet);
  } // if
  // GenSet and KillSet/PreserveSet are ready here.
  resultSet_ = lGenSet | (currSet_ & lPreserveSet);
  EMDEBUG1(resultSet_.GetSize());
  return;
} // DFAInstReachingDefs::AnalysisFunc

bool DFAInstReachingDefs::
IsDefinedIn(IRBB* bb_, Register* reg_) const {
  BMDEBUG3("DFAInstReachingDefs::IsDefinedIn", bb_, reg_);
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(reg_ != 0, ieStrParamValuesDBCViolation);
  bool retVal(false);
  const TopBitVector& lTopVector(GetTopVector());
  //! \todo M Design: Is it possible/better to use bb_ dfaIn/dfaOut sets?
  for (hFUInt32 c(0); c < lTopVector.size(); ++ c) {
    if (*lTopVector[c] == reg_ && lTopVector[c]->GetProgPoint()->GetBB() == bb_) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAInstReachingDefs::IsDefinedIn

bool DFAInstReachingDefs::
IsDefinedAtBeg(IRBB* bb_, Register* reg_) const {
  BMDEBUG3("DFAInstReachingDefs::IsDefinedAtBeg", bb_, reg_);
  bool retVal(false);
  const fBitVector& lInSet(GetInSet(bb_));
  const TopBitVector& lTopVector(GetTopVector());
  for (hFUInt32 c(0); c < lInSet.GetSize(); ++ c) {
    if (!lInSet.IsBottom(c) && *lTopVector[c] == reg_) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAInstReachingDefs::IsDefinedAtBeg

bool DFAInstReachingDefs::
IsDefinedAtEnd(IRBB* bb_, Register* reg_) const {
  BMDEBUG3("DFAInstReachingDefs::IsDefinedAtEnd", bb_, reg_);
  bool retVal(false);
  const fBitVector& lOutSet(GetOutSet(bb_));
  const TopBitVector& lTopVector(GetTopVector());
  for (hFUInt32 c(0); c < lOutSet.GetSize(); ++ c) {
    if (!lOutSet.IsBottom(c) && *lTopVector[c] == reg_) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // DFAInstReachingDefs::IsDefinedAtEnd

void DFAInstReachingDefs::
calcTopVector(TopBitVector& topVector_) {
  BMDEBUG1("DFAInstReachingDefs::calcTopVector");
  REQUIREDMSG(topVector_.empty() == true, ieStrParamValuesDBCViolation);
  ListIterator<IRBB*> lBBIter(GetFunc()->GetCFG()->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    CGBB* lBB(static_cast<CGBB*>(*lBBIter));
    vector<AsmSequenceItem*> lInsts;
    lBB->GetInsts(lInsts);
    for (hFUInt32 c(0); c < lInsts.size(); ++ c) {
      if (Instruction* lInst = dynamic_cast<Instruction*>(lInsts[c])) {
        set<Register*> lRegs;
        lInst->GetDefs().GetRegs(lRegs);
        for (set<Register*>::iterator lIt(lRegs.begin()); lIt != lRegs.end(); ++ lIt) {
          topVector_.push_back(new TopElemType(lInst, *lIt, topVector_.size()));
        } // for
      } // if
    } // for
  } // for
  EMDEBUG1(topVector_.size());
  return;
} // DFAInstReachingDefs::calcTopVector

//! \todo M Design: I think this function can be generalized in DFA base class.
void DFAIRReachingDefs::
GetDefinesInSet(IRBB* bb_, TopBitVector& defines_) {
  BMDEBUG2("DFAIRReachingDefs::GetDefinesInSet", bb_);
  const TopBitVector& lTopVector(GetTopVector());
  const fBitVector& lInSet(GetInSet(bb_));
  for (hFUInt32 c(0); c < lInSet.GetSize(); ++ c) {
    if (lInSet.IsTop(c) == true) {
      defines_.push_back(new TopElemType(*lTopVector[c]));
    } // if
  } // for
  EMDEBUG0();
  return;
} // DFAIRReachingDefs::GetDefinesInSet




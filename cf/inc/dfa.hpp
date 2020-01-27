// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __DFA_HPP__
#define __DFA_HPP__

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __TEMPLATE_AVAILEXPR_HPP__
#include "template_availexpr.hpp"
#endif

//! \file dfa.hpp
//! \brief Data Flow Analysis class and utilities header file.

fBitVector operator | (const fBitVector& leftVector_, const fBitVector& rightVector_);
fBitVector operator & (const fBitVector& leftVector_, const fBitVector& rightVector_);
fBitVector operator - (const fBitVector& leftVector_, const fBitVector& rightVector_);
ostream& operator << (ostream& o, const fBitVector& bv_);

ostream& operator << (ostream& o, const BitVector& bv_);
bool operator != (const BitVector& leftVector_, const BitVector& rightVector_);
BitVector operator | (const BitVector& leftVector_, const BitVector& rightVector_);
BitVector operator & (const BitVector& leftVector_, const BitVector& rightVector_);
BitVector operator - (const BitVector& leftVector_, const BitVector& rightVector_);
void operator &= (BitVector& leftVector_, const BitVector& rightVector_);
void operator |= (BitVector& leftVector_, const BitVector& rightVector_);

//! \pre Template param P can be Instruction or IRStmt.
//! Template param D is analysis specific data that canbe attached to
//! a program point, it can be for exmaple, Register*, IRExpr*.
template<class P, class D>
class DFAProgPntTopElem : public DFAProgPnt {
public:
  DFAProgPntTopElem(P* point_, D data_, hFSInt32 bitIndex_) :
    DFAProgPnt(bitIndex_),
    mPoint(point_),
    mData(data_)
  {
  } // DFAProgPntTopElem::DFAProgPntTopElem
  DFAProgPntTopElem(const DFAProgPntTopElem& te_) :
    DFAProgPnt(te_),
    mPoint(te_.mPoint),
    mData(te_.mData)
  {
  } // DFAProgPntTopElem::DFAProgPntTopElem
  bool operator == (const DFAProgPntTopElem& bv_) {
    bool retVal(bv_.mPoint == mPoint && bv_.mData == mData);
    return retVal;
  } // DFAProgPntTopElem::operator ==
  bool operator == (const P* point_) const {
    return point_ == mPoint;
  } // DFAProgPntTopElem::operator ==
  bool operator == (const D& data_) const {
    return data_ == mData;
  } // DFAProgPntTopElem::operator ==
  P* GetProgPoint() const {return mPoint;}
  const D& GetData() const {return mData;}
  D& GetData() {return mData;}
  virtual bool isProgPoint(const P* point_) const {
    return mPoint == point_;
  } // DFAProgPntTopElem::isProgPoint
  virtual char* debug(const DTContext& context, ostream& toStr) const;
private:
  P* mPoint;
  D mData;
}; // class DFAProgPntTopElem

//! \brief Data flow analysis base class.
//! We want DFA to work on both IR objects and Instruction registers.
//! It should be able gather information from IR statements and Instructions.
//! The information is usually get uses, get defs kind.
//! \todo M Opt: It is possible to find out the signedness of values
//! by using data flow analysis. Check out how.
template<class P, class D>
class DFA : public IDebuggable {
public: // Member types
  typedef vector<DFAProgPntTopElem<P, D>*> TopBitVector;
  typedef DFAProgPntTopElem<P, D> TopElemType;
protected:
  //! \param [in] name_ Name of the analysis.
  //! \param [in] isIRLevel_ DFA can be at IR or Inst level, this variable
  //!        specifies that.
  //! \param [in] initIsTop_ for the start basic block initializes the in set.
  //          You can initialize the other blocks in AnalysisFunc.
  //! \param func_ \todo doc.
  //! \param dir_ \todo doc.
  //! \param combineOp_ \todo doc.
  DFA(const string& name_, IRFunction* func_, DFADirType dir_, DFACombineType combineOp_, bool isIRLevel_, bool initIsTop_) :
    mDFASetsIndex(-1),
    mName(name_),
    mDir(dir_),
    mCombineOp(combineOp_),
    mFunc(func_),
    mIsIRLevel(isIRLevel_),
    mInitIsTop(initIsTop_)
  {
  } // DFA::DFA
  virtual ~DFA() {}
public:
  //! \brief Starts the analysis.
  void Do();
  string GetName() const {return mName;}
  const TopBitVector& GetTopVector() const {return mTopVector;}
protected: // Member functions
  //! \brief Must produce a result set from current input set and by executing the basic block.
  //! \pre Before the first call of AnalysisFunc for an analysis
  //!      it is guaranteed that IRBB::dfaKillSet, and IRBB::dfaGenSet
  //!      of \p bb_ has zero elements.
  //! \param [in] bb_ Basic block in consideration.
  //! \param [in] currSet_ Current input to the \p bb_.
  //! \param [out] resultSet_ Transformed version of \p currSet_ by executing the \p bb_.
  //!
  //! \note AnalysisFunc typically goes through the basic block at hand and
  //! investigates the effect of current basic block on the elements of
  //! top vector. Top vector has all the information on all the basic blocks.
  //! \note Initialization of basic blocks can be done in AnalysisFunc.
  virtual void AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_) = 0;
  //! \brief The function which must calculate the TOP vector of the analysis.
  //! Each analysis has its own definition of interesting program points.
  //! There are two supported program point types in our framework,
  //! one is statement, the other is instruction. Actually a single
  //! statement, or instruction pointer is sufficient to locate the program point.
  //!
  //! The analysis must traverse all IR for \a mFunc to generate the
  //! mTopVector. Liveness analysis must for example insert the statement
  //! that uses a variable. Reaching definitions, must insert the statement
  //! that defines a variable.
  //! 
  //! Usually for evaluating the top vector you should visit all basic blocks.
  //! For each basic block you should populate the top vector by inserting interesting 
  //! points.
  virtual void calcTopVector(TopBitVector& topVector_) = 0;
  IRFunction* GetFunc() const {return mFunc;}
  virtual char* debug(const DTContext& context, ostream& toStr) const;
  //! \pre \p dfaProgPnt_ must be in the \p topVector, i.e. HasIndexInTop == true.
  //! \sa HasIndexInTop
  hFSInt32 GetIndexInTop(const TopElemType& dfaProgPnt_) const;
  bool HasIndexInTop(const TopElemType& dfaProgPnt_, hFUInt32& index_) const;
  //! \pre \p progPnts_ must be empty.
  void GetProgPnts(const void* point_, BitVector& progPnts_) const {
    BMDEBUG2("DFA::GetProgPnts", hFUInt32(point_));
    REQUIREDMSG(progPnts_.empty() == true, ieStrParamValuesDBCViolation);
    for (hFUInt32 c(0); c < mTopVector.size(); ++ c) {
      if (mTopVector[c]->GetProgPoint() == point_) {
        progPnts_.push_back(*mTopVector[c]);
      } // if
    } // for
    EMDEBUG1(progPnts_.size());
    return;
  } // DFA::GetProgPnts
  const fBitVector& GetInSet(const IRBB* bb_) const;
  const fBitVector& GetOutSet(const IRBB* bb_) const;
  const fBitVector& GetKillSet(const IRBB* bb_) const;
  const fBitVector& GetGenSet(const IRBB* bb_) const;
  fBitVector& GetInSet(IRBB* bb_);
  fBitVector& GetOutSet(IRBB* bb_);
  fBitVector& GetKillSet(IRBB* bb_);
  fBitVector& GetGenSet(IRBB* bb_);
  //! \todo H Design: This should not be here!
  set<Instruction*>& GetDefs(Register* reg_) const;
  const fBitVector& GetBottomVector() const {return mBottomVector;}
  void SetAsTop(fBitVector& bitVector_) const {
    bitVector_.SetAllOne();
    return;
  } // DFA::SetAsTop
  fBitVector GetTopBitVector() const {
    fBitVector retVal(GetBottomVector());
    SetAsTop(retVal);
    return retVal;
  } // DFA::GetTopBitVector
  bool IsInited(IRBB* bb_) const;
  void SetInited(IRBB* bb_);
  bool IsIRLevel() const {return mIsIRLevel == true;}
  bool IsInstLevel() const {return mIsIRLevel == false;}
private: // Member functions
  //! \pre You should call calcTopVector before calling this function.
  void calcBottomVector() {
    mBottomVector.Resize(mTopVector.size());
    mBottomVector.Reset();
  } // DFA::calcBottomVector
private: // Member data
  //! \brief Index of IRBB::dfaSets that this DFA is using.
  hFSInt32 mDFASetsIndex;
  TopBitVector mTopVector;
  string mName;
  fBitVector mBottomVector;
  DFADirType mDir; //!< Specified direction of analysis.
  DFACombineType mCombineOp; //!< Specified combine operator, And or Or.
  IRFunction* mFunc; //!< DFA will work on this function.
  bool mIsIRLevel; //!< True if this is an IR level analysis.
  bool mInitIsTop; //!< Init vector is set to top or bottom.
private:
  friend class IRBB;
}; // class DFA

//! \brief Does liveness analysis on instructions.
//! \todo M Design: Check if "inst d, u, u" case is properly handled, i.e.
//!       same reg is used twice in an instruction.
class DFAInstLiveness : public DFA<Instruction, Register*> {
public:
  DFAInstLiveness(IRFunction* func_) :
    DFA<Instruction, Register*>("Inst Liveness analysis", func_, eDFADBackward, eDFACTOr, consts.cDFAInstLevel, consts.cDFAInitBottom)
  {
  } // DFAInstLiveness
  virtual ~DFAInstLiveness() {}
  bool IsLiveAtEnd(IRBB* bb_, Instruction* inst_, Register* reg_) const;
  bool IsLiveAtBeg(IRBB* bb_, Instruction* inst_, Register* reg_) const;
protected:
  virtual void AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_);
  //! \brief Records the uses as program points of interest.
  virtual void calcTopVector(TopBitVector& topVector_);
private:
  //! \brief Keeps the set of virtual registers seen in defs and uses of
  //! of instructions in AnalysisFunc visited basic blocks.
  set<VirtReg*> mVirtRegsSeen;
private:
  friend class RegisterAllocator;
}; // class DFAInstLiveness

//! \brief Does reaching definitions analysis on statements and objects.
class DFAIRReachingDefs : public DFA<IRStmt, IRObject*> {
public:
  DFAIRReachingDefs(IRFunction* func_) :
    DFA<IRStmt, IRObject*>("IR Reaching definitions analysis", func_, eDFADForward, eDFACTOr, consts.cDFAIRLevel, consts.cDFAInitBottom)
  {
  } // DFAIRReachingDefs::DFAIRReachingDefs
  virtual ~DFAIRReachingDefs() {}
  bool IsDefinedIn(IRBB* bb_, IRObject* obj_) const;
  bool IsDefinedAtBeg(IRBB* bb_, IRObject* obj_) const;
  bool IsDefinedAtEnd(IRBB* bb_, IRObject* obj_) const;
  //! \brief Returns the objects that are live and reached to the given basic block.
  void GetDefinesInSet(IRBB* bb_, TopBitVector& defines_);
protected:
  virtual void AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_);
  //! \brief Records the definitions as program points of interest.
  virtual void calcTopVector(TopBitVector& topVector_);
}; // class DFAIRReachingDefs

//! \brief Does reaching definitions analysis on instructions.
class DFAInstReachingDefs : public DFA<Instruction, Register*> {
public:
  DFAInstReachingDefs(IRFunction* func_) :
    DFA<Instruction, Register*>("Inst Reaching definitions analysis", func_, eDFADForward, eDFACTOr, consts.cDFAInstLevel, consts.cDFAInitBottom)
  {
  } // DFAInstReachingDefs::DFAInstReachingDefs
  virtual ~DFAInstReachingDefs() {}
  bool IsDefinedIn(IRBB* bb_, Register* reg_) const;
  bool IsDefinedAtBeg(IRBB* bb_, Register* reg_) const;
  bool IsDefinedAtEnd(IRBB* bb_, Register* reg_) const;
protected:
  virtual void AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_);
  //! \brief Records the definitions as program points of interest.
  virtual void calcTopVector(TopBitVector& topVector_);
private:
  friend class RegisterAllocator;
}; // class DFAInstReachingDefs

//! \brief Does liveness analysis on IR statements.
class DFAIRLiveness : public DFA<IRStmt, IRObject*> {
public:
  DFAIRLiveness(IRFunction* func_) :
    DFA<IRStmt, IRObject*>("IR Liveness analysis", func_, eDFADBackward, eDFACTOr, consts.cDFAIRLevel, consts.cDFAInitBottom)
  {
  } // DFAIRLiveness
  virtual ~DFAIRLiveness() {}
protected:
  virtual void calcTopVector(TopBitVector& topVector_) {
    NOTIMPLEMENTED(H);
  } // DFAIRLiveness::calcTopVector
  virtual void AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_) {
    NOTIMPLEMENTED(H);
  } // DFAIRLiveness::AnalysisFunc
}; // class DFAIRLiveness

//! \brief <Statement, lhs obj, anlhsuse, rhsexpr, iskilled> tuple to be used as copy assignment data.
typedef tuple5<IRStmt*, IRObject*, IRExpr*, IRExpr*, bool> DFAIRCopyAssignData;

//! \brief Does available copy assignments at IR level.
class DFAIRCopyAssignments : public DFA<IRStmt, DFAIRCopyAssignData*> {
public:
  DFAIRCopyAssignments(IRFunction* func_) :
    DFA<IRStmt, DFAIRCopyAssignData*>("IR available copy assignments analysis", func_, eDFADForward, eDFACTAnd, consts.cDFAIRLevel, consts.cDFAInitBottom)
  {
  } // DFAIRCopyAssignments::DFAIRCopyAssignments
  virtual ~DFAIRCopyAssignments() {}
  void GetAvailCopiesInSet(IRBB* bb_, TopBitVector& availCopies_);
protected:
  virtual void AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_);
  //! \brief Records the copy assignments as program points of interest.
  virtual void calcTopVector(TopBitVector& topVector_);
}; // class DFAIRCopyAssignments

//! \brief Does available expressions at IR level.
class DFAIRAvailExpressions : public DFA<IRStmt, IRExpr*> {
public:
  typedef bool (*IsOkExprFunc)(const IRExpr*);
public:
  DFAIRAvailExpressions(IRFunction* func_, IsOkExprFunc isOkExprFunc_) :
    DFA<IRStmt, IRExpr*>("IR available expressions analysis", func_, eDFADForward, eDFACTAnd, consts.cDFAIRLevel, consts.cDFAInitBottom),
    mIsOkExprFunc(isOkExprFunc_)
  {
    REQUIREDMSG(isOkExprFunc_ != 0, ieStrParamValuesDBCViolation);
  } // DFAIRAvailExpressions::DFAIRAvailExpressions
  virtual ~DFAIRAvailExpressions() {}
  void GetAvailExprsInSet(IRBB* bb_, TopBitVector& availExprs_);
protected:
  virtual void AnalysisFunc(IRBB* bb_, const fBitVector& currSet_, fBitVector& resultSet_);
  //! \brief Records the proper expressions as program points of interest.
  virtual void calcTopVector(TopBitVector& topVector_);
  AvailableExpressions<bool> mAvailExprs;
  IsOkExprFunc mIsOkExprFunc;
}; // class DFAIRAvailExpressions

#ifndef __DFA_INLINE_CPP__
#include "dfa_inline.cpp"
#endif

#endif


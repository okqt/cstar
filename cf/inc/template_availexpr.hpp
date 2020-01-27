// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __TEMPLATE_AVAILEXPR_HPP__
#define __TEMPLATE_AVAILEXPR_HPP__

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __STATS_HPP__
#include "stats.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif

//! \file template_availexpr.hpp
//! \brief Contains the available expressions utility template class.

//! \brief Available expressions object.
//! You can create this object and insert expressions in to it.
//! Later you can query a AvailableExpressions::Remove with a statement
//! that will notify and remove the expressions that are killed by the
//! statement. 
//! 
//! You can store additional data per available expression
//! by using the template argument, by default it is a boolean.
template<typename T=bool>
class AvailableExpressions : public IDebuggable {
public: // Member types
  //! \brief expr, operand0, operand1, commutativeness, numchildren, stillavailable.
  typedef tuple7<IRExpr*, IRExpr*, IRExpr*, bool, hFUInt8, T, bool> AEtuple;
public:
  AvailableExpressions<T>() {
    BMDEBUG1("AvailableExpressions::AvailableExpressions");
    EMDEBUG0();
  } // AvailableExpressions::AvailableExpressions
  virtual ~AvailableExpressions<T>() {}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Add the provided expression to the available expressions list.
  //! \note Leaf nodes are not added, e.g. IRObject, IREIntConst.
  //! \note Function calls are not added.
  //! \note Operands of expr_ must be leaf nodes if they are to be added otherwise.
  //! \note If expression is added newly its index can be found from the
  //!       AvailableExpressions::GetSize(), new expressions are always added to the end.
  //! \return Returns true if expression was a proper expression, e.g. not a leaf node.
  //! \param [out] prevIndex_ set to -1 if the expression is newly added,
  //!        otherwise it returns the index of first matching available expression.
  //! \todo H Design: provide a non prevIndex version of this function.
  bool Add(IRExpr* expr_, hFSInt32& prevIndex_);
  //! \brief Removes expressions that are killed by the provided statement.
  //! \note Typically assignments may kill available expressions.
  //!       All address taken variables in available expressions are killed by
  //!       a write via pointer on the LHS of the assignment:
  //!       p = &c;
  //!       q = &b;
  //!         (a + b) => AE 0
  //!         (c + d) => AE 1
  //!         *p = 1; // will kill both AE 0 and 1.
  //! \return Returns true if any expression is killed.
  //! \todo M Design: Make use of alias analyses.
  bool Remove(const IRStmt* stmt_);
  //! \brief Returns true if given statement can kill any available expression.
  bool CanRemove(const IRStmt* stmt_) const;
  //! \brief Returns the number of available expressions collected.
  hFUInt32 GetSize() const {return mAvailExprs.size();}
  //! \brief Returns the data slot of available expression at a given index.
  T& GetData(hFSInt32 index_) {
    REQUIREDMSG(index_ >= 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(index_ < mAvailExprs.size(), ieStrParamValuesDBCViolation);
    return getData(mAvailExprs[index_]);
  } // AvailableExpressions::GetData
  IRExpr* GetExpr(hFSInt32 index_) {
    REQUIREDMSG(index_ >= 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(index_ < mAvailExprs.size(), ieStrParamValuesDBCViolation);
    return getExpr(mAvailExprs[index_]);
  } // AvailableExpressions::GetExpr
  const vector<AEtuple>& GetExprs() const {return mAvailExprs;}
  //! \pre expr_ must be a proper expression.
  static bool smSameExpr(IRExpr* expr0_, IRExpr* expr1_);
  bool& GetAvail(hFUInt32 index_) {return getAvail(mAvailExprs[index_]);}
  //! \brief Returns true if any expression is killed in the provided set of basic blocks.
  bool pAnyKilled(const set<IRBB*>& bbs_) const;
  //! \brief Returns true if any expression is killed in any path from from_ statement to to_ statement.
  //! \note from_ and to_ may kill.
  bool pAnyKilled(const IRStmt* from_, const IRStmt* to_) const;
private: // Member functions
  //! \brief Kills all available expressions.
  void AnatropPanic() {
    STATRECORD("availexpr: panic");
    mAvailExprs.clear();
    mAEAddrTakenObjIndexes.clear();
    mAEGlobalObjIndexes.clear();
    mAEObjIndexes.clear();
  } // AvailableExpressions::AnatropPanic
  //! Returns true if expr_ matches to the given available expression.
  static bool isAMatch(const AEtuple& ae_, const IRExpr* expr_, const vector<IRExpr*>& exprChildren_);
  //! \brief Returns true if expression can be added to expressions list.
  bool isProperExpr(const IRExpr* expr_, const vector<IRExpr*>& exprChildren) const;
  void onAddOperand(IRExpr* op_);
  void clearAddrTakenObjAEs();
  void clearAllObjAEs();
  //! \brief Returns true if any obj is cleared.
  bool clearObjAEs(IRObject* obj_);
  void clearGlobalObjAEs();
  // Get and set the members of tuple.
  static IRExpr*& getExpr(AEtuple& ae_) {return ae_.mT0;}
  static IRExpr*& getOp0(AEtuple& ae_) {return ae_.mT1;}
  static IRExpr*& getOp1(AEtuple& ae_) {return ae_.mT2;}
  //! \brief Return the commutativity of the available expression.
  static bool& getIsCommutative(AEtuple& ae_) {return ae_.mT3;}
  static hFUInt8& getNumChild(AEtuple& ae_) {return ae_.mT4;}
  static T& getData(AEtuple& ae_) {return ae_.mT5;}
  static bool& getAvail(AEtuple& ae_) {return ae_.mT6;}
  // CONSTANT versions
  static IRExpr* getExpr(const AEtuple& ae_) {return ae_.mT0;}
  static IRExpr* getOp0(const AEtuple& ae_) {return ae_.mT1;}
  static IRExpr* getOp1(const AEtuple& ae_) {return ae_.mT2;}
  static bool getIsCommutative(const AEtuple& ae_) {return ae_.mT3;}
  static hFUInt8 getNumChild(const AEtuple& ae_) {return ae_.mT4;}
  static T& getData(const AEtuple& ae_) {return ae_.mT5;}
  static bool getAvail(const AEtuple& ae_) {return ae_.mT6;}
private: // Member data
  //! \brief List of available expressions.
  vector<AEtuple> mAvailExprs;
  vector<hFUInt32> mAEGlobalObjIndexes;
  vector<hFUInt32> mAEAllObjIndexes;
  vector<hFUInt32> mAEAddrTakenObjIndexes;
  map<IRObject*, vector<hFUInt32> > mAEObjIndexes;
private:
  friend class Predicate;
}; // class AvailableExpressions

template<class T>
bool AvailableExpressions<T>::
pAnyKilled(const IRStmt* from_, const IRStmt* to_) const {
  BMDEBUG3("AvailableExpressions::pAnyKilled", from_, to_);
  bool retVal(false);
  set<IRBB*> bbs_;
  extr.eGetBBsInPath(from_->GetBB(), to_->GetBB(), bbs_);
  if (!bbs_.empty()) {
    bbs_.erase(from_->GetBB());
    if (from_->GetBB() != to_->GetBB()) {
      bbs_.erase(to_->GetBB());
    } // if
    retVal = pAnyKilled(bbs_);
  } // if
  if (retVal == false) {
    // Check the statements in from_ basic block.
    for (const IRStmt* lCurrStmt(from_); lCurrStmt != 0; lCurrStmt = extr.eGetNextStmt(lCurrStmt)) {
      if (CanRemove(lCurrStmt) == true) {
        retVal = true;
        break;
      } // if
    } // for
    if (retVal == false) { // Check the statements in to_ basic block.
      for (const IRStmt* lCurrStmt(to_); lCurrStmt != 0; lCurrStmt = extr.eGetPrevStmt(lCurrStmt)) {
        if (CanRemove(lCurrStmt) == true) {
          retVal = true;
          break;
        } // if
      } // for
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::pAnyKilled

template<class T>
bool AvailableExpressions<T>::
pAnyKilled(const set<IRBB*>& bbs_) const {
  BMDEBUG1("AvailableExpressions::pAnyKilled");
  bool retVal(false);
  for (set<IRBB*>::const_iterator lIt(bbs_.begin()); lIt != bbs_.end(); ++ lIt) {
    IRBB* lCurrBB(*lIt);
    ListIterator<IRStmt*> lStmtIter(lCurrBB->GetStmtIter());
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      if (CanRemove(*lStmtIter) == true) {
        retVal = true;
        break;
      } // if
    } // for
    if (retVal == true) {
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::pAnyKilled

template<class T>
void AvailableExpressions<T>::
clearAddrTakenObjAEs() {
  BMDEBUG1("AvailableExpressions::clearAddrTakenObjAEs");
  hFUInt32 retVal(0);
  for (hFUInt32 c(0); c < mAEAddrTakenObjIndexes.size(); ++ c) {
    getAvail(mAvailExprs[mAEAddrTakenObjIndexes[c]]) = false;
    ++ retVal;
  } // for
  mAEAddrTakenObjIndexes.clear();
  EMDEBUG1(retVal);
  return;
} // AvailableExpressions::clearAddrTakenObjAEs

template<class T>
void AvailableExpressions<T>::
clearAllObjAEs() {
  BMDEBUG1("AvailableExpressions::clearAllObjAEs");
  hFUInt32 retVal(0);
  for (hFUInt32 c(0); c < mAEAllObjIndexes.size(); ++ c) {
    getAvail(mAvailExprs[mAEAllObjIndexes[c]]) = false;
    ++ retVal;
  } // for
  mAEAllObjIndexes.clear();
  EMDEBUG1(retVal);
  return;
} // AvailableExpressions::clearAllObjAEs

template<class T>
bool AvailableExpressions<T>::
clearObjAEs(IRObject* obj_) {
  BMDEBUG2("AvailableExpressions::clearObjAEs", obj_);
  hFUInt32 retVal(0);
  map<IRObject*, vector<hFUInt32> >::iterator lIt(mAEObjIndexes.find(obj_));
  if (lIt != mAEObjIndexes.end()) {
    vector<hFUInt32>& lIndexes(lIt->second);
    for (hFUInt32 c(0); c < lIndexes.size(); ++ c) {
      getAvail(mAvailExprs[lIndexes[c]]) = false;
      ++ retVal;
    } // for
    lIndexes.clear();
  } // if
  EMDEBUG1(retVal);
  return retVal != 0;
} // AvailableExpressions::clearObjAEs

template<class T>
void AvailableExpressions<T>::
clearGlobalObjAEs() {
  BMDEBUG1("AvailableExpressions::clearGlobalObjAEs");
  hFUInt32 retVal(0);
  for (hFUInt32 c(0); c < mAEGlobalObjIndexes.size(); ++ c) {
    getAvail(mAvailExprs[mAEGlobalObjIndexes[c]]) = false;
    ++ retVal;
  } // for
  mAEGlobalObjIndexes.clear();
  EMDEBUG1(retVal);
  return;
} // AvailableExpressions::clearGlobalObjAEs

template<class T>
bool AvailableExpressions<T>::
smSameExpr(IRExpr* expr0_, IRExpr* expr1_) {
  BDEBUG3("AvailableExpressions::smSameExpr", expr0_, expr1_);
  vector<IRExpr*> lExpr0Children;
  vector<IRExpr*> lExpr1Children;
  expr0_->GetChildren(lExpr0Children);
  expr1_->GetChildren(lExpr1Children);
  bool retVal(lExpr0Children.size() == lExpr1Children.size());
  if (retVal == true) {
    IRExpr* lOp1(lExpr0Children.size() == 2 ? lExpr0Children[1] : 0);
    AEtuple lAEtuple(expr0_, lExpr0Children[0], lOp1, pred.pIsCommutative(expr0_), lExpr0Children.size(), T(), true);
    retVal = isAMatch(lAEtuple, expr1_, lExpr1Children);
  } // if
  EDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::smSameExpr

template<class T>
bool AvailableExpressions<T>::
isProperExpr(const IRExpr* expr_, const vector<IRExpr*>& exprChildren_) const {
  BMDEBUG2("AvailableExpressions::isProperExpr", expr_);
  bool retVal(true);
  if (pred.pIsUse(expr_) == true) {
    // LIE: uses are proper expressions.
  } else if (exprChildren_.size() != 2 && exprChildren_.size() != 1) {
    retVal = false;
  } else {
    for (hFUInt32 c(0); c < exprChildren_.size(); ++ c) {
      if (pred.pIsUse(exprChildren_[c]) == true) {
        // LIE
      } else if (pred.pIsConst(exprChildren_[c]) == true) {
        // LIE
      } else {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  PDEBUGBLOCK() {
    if (retVal == true) {
      PDEBUG("AvailableExpressions", "detail", "proper expression: " << progcxt(expr_));
    } // if
  } // PDEBUGBLOCK
  EMDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::isProperExpr

template<class T>
bool AvailableExpressions<T>::
Add(IRExpr* expr_, hFSInt32& prevIndex_) {
  BMDEBUG2("AvailableExpressions::Add", expr_);
  bool retVal(false);
  vector<IRExpr*> lChildren;
  expr_->GetChildren(lChildren);
  prevIndex_ = -2;
  if (isProperExpr(expr_, lChildren) == true) {
    retVal = true;
    bool lMatched(false);
    for (hFUInt32 c(0); c < mAvailExprs.size(); ++ c) {
      if (getAvail(mAvailExprs[c]) == true && isAMatch(mAvailExprs[c], expr_, lChildren) == true) {
        prevIndex_ = c;
        lMatched = true;
        break;
      } // if
    } // for
    if (!lMatched) {
      IRExpr* op1(lChildren.size() == 2 ? lChildren[1] : 0);
      if (pred.pIsUse(expr_) == true) {
        onAddOperand(expr_);
      } else {
        onAddOperand(lChildren[0]);
        if (op1 != 0) {
          onAddOperand(op1);
        } // if
      } // if
      mAvailExprs.push_back(AEtuple(expr_, lChildren[0], op1, pred.pIsCommutative(expr_), lChildren.size(), T(), true));
      prevIndex_ = -1;
    } // if
  } else {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::Add

template<class T>
void AvailableExpressions<T>::
onAddOperand(IRExpr* op_) {
  BMDEBUG2("AvailableExpressions::onAddOperand", op_);
  if (pred.pIsUse(op_) == true) {
    if (IRObject* lObj = extr.eGetObjOfUse(op_)) {
      if (pred.pIsGlobal(lObj) == true) {
        mAEGlobalObjIndexes.push_back(mAvailExprs.size());
        PDEBUG("AvailableExpressions", "detail", " glob added: " << refcxt(lObj));
      } // if
      mAEObjIndexes[lObj].push_back(mAvailExprs.size());
      mAEAllObjIndexes.push_back(mAvailExprs.size());
      PDEBUG("AvailableExpressions", "detail", " obj added: " << refcxt(lObj));
      if (lObj->IsAddrTaken() == true) {
        mAEAddrTakenObjIndexes.push_back(mAvailExprs.size());
        PDEBUG("AvailableExpressions", "detail", " addr taken added: " << refcxt(lObj));
      } // if
    } // if
  } // if
  EMDEBUG0();
  return;
} // AvailableExpressions::onAddOperand

template<class T>
bool AvailableExpressions<T>::
isAMatch(const AEtuple& ae_, const IRExpr* expr_, const vector<IRExpr*>& exprChildren_) {
  BDEBUG2("AvailableExpressions::isAMatch", expr_);
  tristate retVal;
  if (!pred.pIsSameExprClass(getExpr(ae_), expr_)) {
    retVal = false;
  } else if (getNumChild(ae_) == 1) {
    retVal = pred.pHasSameValue(exprChildren_[0], getOp0(ae_));
  } else {
    retVal = pred.pHasSameValue(exprChildren_[0], getOp0(ae_)) && 
      pred.pHasSameValue(exprChildren_[1], getOp1(ae_));
    if (getIsCommutative(ae_) == true) {
      retVal = retVal || (pred.pHasSameValue(exprChildren_[1], getOp0(ae_)) && 
        pred.pHasSameValue(exprChildren_[0], getOp1(ae_)));
      // LIE
    } // if
  } // if
  EDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::isAMatch

//! \attention This function should be adapted from Remove.
template<class T>
bool AvailableExpressions<T>::
CanRemove(const IRStmt* stmt_) const {
  BMDEBUG2("AvailableExpressions::CanRemove", stmt_);
  bool retVal(false);
  if (pred.pIsCall(stmt_) == true) {
    retVal = true;
  } else if (const IRSAssign* lAssign = dynamic_cast<const IRSAssign*>(stmt_)) {
    if (pred.pIsAssignThruPtr(lAssign) == true) {
      retVal = true;
    } else if (pred.pIsAddrOf(lAssign->GetLHS())) {
      IRObject* lObj(extr.eGetObjOfAddrOf(lAssign->GetLHS()));
      if (lObj != 0) {
        map<IRObject*, vector<hFUInt32> >::const_iterator lIt(mAEObjIndexes.find(lObj));
        retVal = lIt != mAEObjIndexes.end();
      } else {
        retVal = true;
      } // if
    } else {
      retVal = true;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::CanRemove

//! \attention Any change here may be required to be done in CanRemove also.
template<class T>
bool AvailableExpressions<T>::
Remove(const IRStmt* stmt_) {
  BMDEBUG2("AvailableExpressions::Remove", stmt_);
  bool retVal(false);
  if (pred.pIsCall(stmt_) == true) {
    clearAddrTakenObjAEs();
    clearGlobalObjAEs();
    clearAllObjAEs();
    retVal = true;
  } else if (const IRSAssign* lAssign = dynamic_cast<const IRSAssign*>(stmt_)) {
    // Assignment through a pointer kills all address taken expressions.
    if (pred.pIsAssignThruPtr(lAssign) == true) {
      clearAddrTakenObjAEs();
      clearGlobalObjAEs();
      clearAllObjAEs();
      retVal = true;
    } else if (pred.pIsAddrOf(lAssign->GetLHS())) {
      IRObject* lObj(extr.eGetObjOfAddrOf(lAssign->GetLHS()));
      if (lObj != 0) {
        // Kill all expressions involving the lObj.
        retVal = clearObjAEs(lObj);
      } else {
        AnatropPanic();
        retVal = true;
      } // if
    } else {
      AnatropPanic();
      retVal = true;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // AvailableExpressions::Remove

template<class T>
char* AvailableExpressions<T>::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AvailableExpressions::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AvailableExpressions::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AvailableExpressions::debug

#endif


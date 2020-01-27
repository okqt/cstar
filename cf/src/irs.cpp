// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __DFA_HPP__
#include "dfa.hpp"
#endif

#if DEBUGBUILD
#include "irs_inline.cpp"
#endif

// //////////////////////////
// Definition of static members
// //////////////////////////
IRTVoid* IRExprList::exprListVoidType;
IRTVoid* IRENull::nullVoidType;
IRTBool* IRECmp::cmpBoolType;
IRTInt* IREPtrSub::ptrDiffType;
IRTBool* IREBoolConst::boolConstBoolType;
IRTPtr*  IREStrConst::strConstStrType;
hFInt32 IRProgram::sRefCount;
IRProgram* irProgram(0);

void IRFunction::
AssignStackAddrToLocals(CGFuncContext* cgContext_) {
  BMDEBUG2("IRFunction::AssignStackAddrToLocals", cgContext_);
  mStackLocalSize = 0;
  //! \todo M Design: handle down and up case, below only down case is handled.
  bool lStackDown(cgContext_->IsStackGrowsDownwards());
  for (hFUInt32 c(0); c < mLocals.size(); ++ c) {
    PDEBUG("CodeGen", "detail", " Local address check: " << progcxt(mLocals[c]));
    bool lStackBased(dynamic_cast<AddrStack*>(mLocals[c]->GetAddress()) != 0);
    if (!pred.pIsAddrSet(mLocals[c]) || lStackBased == true) {
      // First alignment the offset to the type alignment requirement of local.
      hFInt32 lAlignment(util.uMax<hFInt32>(Target::GetTarget()->GetMinStackAlignment(),
        mLocals[c]->GetType()->GetAlignment()));
      mStackLocalSize = util.uGetUpAligned(mStackLocalSize, lAlignment);
      mLocals[c]->SetAddress(AddrStack(cgContext_->GetHWDesc()->GetFPReg(), mStackLocalSize + mParamsSize));
      PDEBUG("CodeGen", "detail", " Locals address set: " << progcxt(mLocals[c]));
      // Update the new stack size for locals.
      mStackLocalSize += mLocals[c]->GetType()->GetSize()/8;
      //! \todo M Design: Check the byte size conversion.
    } // if
  } // for
  ASSERTMSG(Target::GetTarget()->GetMinStackSizeAlignment() >= Target::GetTarget()->GetMinStackAlignment(),
    ieStrDBCViolation << "Stack size alignment must be greater than stack alignment.");
  hFInt32 lAlignment(Target::GetTarget()->GetMinStackSizeAlignment());
  mStackLocalSize = util.uGetUpAligned(mStackLocalSize, lAlignment);
  EMDEBUG0();
  return;
} // IRFunction::AssignStackAddrToLocals

IRTree* IRExprList::
GetRecCopy() const {
  BMDEBUG1("IRExprList::GetRecCopy");
  IRExprList* retVal(new IRExprList);
  for (hFUInt16 c(0); c < mExprList.size(); ++ c) {
    retVal->AddExpr(mExprList[c]->GetRecCopyExpr());
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRExprList::GetRecCopy

string IROTmp::
sGenerateName(const string& idName_, IRObject* highLevelObject_) {
  BDEBUG3("IROTmp::sGenerateName", idName_, highLevelObject_);
  ostrstream retVal;
  retVal << idName_.c_str();
  if (!pred.pIsInvalid(highLevelObject_)) {
    retVal << "_" << highLevelObject_->GetName();
  } // if
  retVal << ends;
  EDEBUG1(retVal.str());
  return retVal.str();
} // IROTmp::sGenerateName

void IREAddrOf::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IREAddrOf::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mExpr, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  ASSERTMSG(mLabel == 0, ieStrNotImplemented);
  irBuilder->smirbDetached(old_);
  mExpr = new_;
  new_->SetParent(this);
  old_->SetParent(0);
  if (!pred.pIsObj(new_)) {
    irBuilder->smirbAttached(new_);
  } else {
    irBuilder->smirbAttached(this);
  } // if
  EMDEBUG0();
  return;
} // IREAddrOf::ReplaceChild

bool IRModule::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IRModule::Accept");
  bool retVal(true);
  for (list<IRFunction*>::iterator it(mFuncs.begin()); it != mFuncs.end(); ++ it) {
    IRFunction* func(*it);
    if (!func->Accept(visitor_)) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRModule::Accept

bool IRModule::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRModule::Accept");
  bool retVal(true);
  for (list<IRFunction*>::iterator it(mFuncs.begin()); it != mFuncs.end(); ++ it) {
    IRFunction* func(*it);
    if (!func->Accept(visitor_)) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRModule::Accept

bool IRSSwitch::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IRSSwitch::Accept");
  bool retVal(true);
  if (!mValueExpr->Accept(visitor_)) {
    retVal = false;
  } else {
    for (hFUInt32 c(0); c < mCaseStmts.size(); ++ c) {
      if (!mCaseStmts[c].first->Accept(visitor_)) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRSSwitch::Accept

bool IRSSwitch::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG1("IRSSwitch::Accept");
  bool retVal(true);
  if (!mValueExpr->Accept(visitor_)) {
    retVal = false;
  } else {
    for (hFUInt32 c(0); c < mCaseStmts.size(); ++ c) {
      if (!mCaseStmts[c].first->Accept(visitor_)) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRSSwitch::Accept

bool IRStmt::
Accept(ExprVisitor& visitor_) {
  BMDEBUG2("IRStmt::Accept", &visitor_);
  bool retVal(true);
  vector<IRExpr*> lChildren;
  if (GetChildren(lChildren) == true) {
    for (hFUInt32 c(0); c < lChildren.size(); ++ c) {
      if (lChildren[c]->Accept(visitor_) == false) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRStmt::Accept

bool IRStmt::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG2("IRStmt::Accept", &visitor_);
  bool retVal(true);
  vector<IRExpr*> lChildren;
  if (GetChildren(lChildren) == true) {
    for (hFUInt32 c(0); c < lChildren.size(); ++ c) {
      if (lChildren[c]->Accept(visitor_) == false) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRStmt::Accept

bool IRBB::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG1("IRBB::Accept");
  bool retVal(true);
  for (list<IRStmt*>::const_iterator lIt(mStmts.begin()); lIt != mStmts.end(); ++ lIt) {
    IRStmt* lStmt(*lIt);
    if (!lStmt->Accept(visitor_)) {
      PDEBUG("Visitor", "detail", "stmt returned 0 " << refcxt(lStmt));
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRBB::Accept

bool IRBB::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IRBB::Accept");
  bool retVal(true);
  for (list<IRStmt*>::iterator lIt(mStmts.begin()); lIt != mStmts.end(); ++ lIt) {
    IRStmt* lStmt(*lIt);
    if (!lStmt->Accept(visitor_)) {
      PDEBUG("Visitor", "detail", "stmt returned 0 " << refcxt(lStmt));
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRBB::Accept

bool IRBB::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRBB::Accept");
  bool retVal(true);
  for (list<IRStmt*>::iterator lIt(mStmts.begin()); lIt != mStmts.end(); ++ lIt) {
    IRStmt* lStmt(*lIt);
    if (!lStmt->Accept(visitor_)) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRBB::Accept

bool CFG::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("CFG::Accept");
  bool retVal(true);
  for (list<IRBB*>::iterator lIt(mBBs.begin()); lIt != mBBs.end(); ++ lIt) {
    IRBB* lBB(*lIt);
    if (!lBB->Accept(visitor_)) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // CFG::Accept

bool CFG::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("CFG::Accept");
  bool retVal(true);
  for (list<IRBB*>::iterator lIt(mBBs.begin()); lIt != mBBs.end(); ++ lIt) {
    IRBB* lBB(*lIt);
    if (!lBB->Accept(visitor_)) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // CFG::Accept

bool IRProgram::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IRProgram::Accept");
  bool retVal(true);
  for (list<IRModule*>::iterator lIt(mModules.begin()); lIt != mModules.end(); ++ lIt) {
    IRModule* lModule(*lIt);
    if (!lModule->Accept(visitor_)) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRProgram::Accept

bool IRProgram::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRProgram::Accept");
  bool retVal(true);
  for (list<IRModule*>::iterator lIt(mModules.begin()); lIt != mModules.end(); ++ lIt) {
    IRModule* lModule(*lIt);
    if (!lModule->Accept(visitor_)) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRProgram::Accept

void IREBinary::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IREBinary::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mLeftExpr || old_ == mRightExpr, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  if (mLeftExpr == old_) {
    mLeftExpr = new_;
  } else {
    mRightExpr = new_;
  } // if
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IREBinary::ReplaceChild

IRBB::
IRBB(CFG* parent_) :
  mParentCFG(parent_),
  mPrev(0),
  mNext(0),
  mDIE(&Invalid<DIEBase>::Obj())
{
  BMDEBUG1("IRBB::IRBB");
  REQUIRED(parent_ != 0);
  // In an unoptimized compilation it does not matter
  // where the basic blocks are physically, so just assign a valid ordering.
  // There are optimizations that will order the bbs according to different
  // criterias.
  if (parent_->mEntryBB != 0 && parent_->mExitBB != 0) {
    IRBB* nextOfEntry = parent_->GetEntryBB()->GetNext();
    mPrev = parent_->GetEntryBB();
    mNext = parent_->GetEntryBB()->GetNext();
    nextOfEntry->mPrev = this;
    parent_->GetEntryBB()->mNext = this;
    parent_->mBBs.insert(++(parent_->mBBs.begin()), this);
  } // if
  EMDEBUG0();
} // IRBB::IRBB

CFG::
CFG() :
  mEntryBB(0),
  mExitBB(0),
  mDomTree(0),
  mPDomTree(0),
  mDUUDChains(0)
{
  BMDEBUG1("CFG::CFG");
  mEntryBB = new CGBB(this);
  mExitBB = new CGBB(this);
  mEntryBB->mNext = mExitBB;
  mEntryBB->mPrev = 0;
  mExitBB->mNext = 0;
  mExitBB->mPrev = mEntryBB;
  mBBs.push_front(mEntryBB);
  mBBs.push_back(mExitBB);
  //! \todo Done M Design: how can we move the cfg link creation to here? have a static function in irBuilder?
  //! or make the irbuilder a singleton that everyone can use at any time? -> There is no need to
  //! create a link from entry to exit. Since this is a natural situation for functions with infinite
  //! loops.
  ENSURE_VALID(this);
  EMDEBUG0();
} // CFG::CFG

//! \brief Adds a statement to the basic block.
//! \note It updates the \p prev, \p next, \p parent of \p stmt_.
//! \sa IRBuilder::irbInsertStmt
//! \pre Non-null \p stmt_.
//! \pre \p this and basic block of \p ic must be the same.
void IRBB::
InsertStmt(IRStmt* stmt_, const InsertContext& ic) {
  BMDEBUG3("IRBB::InsertStmt", stmt_, &ic);
  REQUIRED_VALID(&ic);
  REQUIRED(stmt_ != 0);
  REQUIRED(ic.GetBB() == this);
  REQUIRED(pred.pIsBeforeFirstOfBB(stmt_) == false);
  REQUIRED(pred.pIsAfterLastOfBB(stmt_) == false);
  // update parent
  stmt_->SetParent(this);
  // update the prev/next.
  if (pred.pIsEmpty(this)) {
    mStmts.push_front(stmt_);
    stmt_->SetPrevStmt(&Singleton<IRSBeforeFirstOfBB>::Obj());
    stmt_->SetNextStmt(&Singleton<IRSAfterLastOfBB>::Obj());
  } else {
    ICStmt icStmt(extr.eConvToICStmt(&ic));
    list<IRStmt*>::iterator it(find(mStmts.begin(), mStmts.end(), icStmt.GetStmt()));
    ASSERT(it != mStmts.end());
    if (*it == mStmts.front() && icStmt.GetPlace() == ICBefore) {
      IRStmt* nextOfStmt = GetFirstStmt();
      mStmts.push_front(stmt_);
      stmt_->SetPrevStmt(&Singleton<IRSBeforeFirstOfBB>::Obj());
      stmt_->SetNextStmt(nextOfStmt);
      nextOfStmt->SetPrevStmt(stmt_);
    } else if (*it == mStmts.back() && icStmt.GetPlace() == ICAfter) {
      IRStmt* prevOfStmt = GetLastStmt();
      mStmts.push_back(stmt_);
      stmt_->SetPrevStmt(prevOfStmt);
      stmt_->SetNextStmt(&Singleton<IRSAfterLastOfBB>::Obj());
      prevOfStmt->SetNextStmt(stmt_);
    } else {
      // Normalize \p it.
      if (icStmt.GetPlace() == ICAfter) {
        ++ it;
      } // if
      IRStmt* prevOfStmt = *(-- it);
      IRStmt* nextOfStmt = *(++ it);
      mStmts.insert(it, stmt_);
      stmt_->SetPrevStmt(prevOfStmt);
      stmt_->SetNextStmt(nextOfStmt);
      prevOfStmt->SetNextStmt(stmt_);
      nextOfStmt->SetPrevStmt(stmt_);
    } // if
  } // if
  if (pred.pIsCFStmt(stmt_)) {
    extr.eGetSuccsOfStmt(stmt_, mSuccs);
    for (hFUInt32 c(0); c < mSuccs.size(); ++ c) {
      mSuccs[c]->mPreds.push_back(this);
    } // for
  } // if
  ENSURE_VALID(this);
  EMDEBUG0();
  return;
} // IRBB::InsertStmt

//! \brief Removes a statement from the basic block.
//! \note Updates \p prev, \p next, \p parent of statements.
//! \note Updates CFG.
//! \pre Valid \p stmt_.
//! \pre \p stmt_ must be a member of the basic block.
//! \post Invalid \p stmt_.
void IRBB::
RemoveStmt(IRStmt* stmt_) {
  BMDEBUG1("IRBB::RemoveStmt");
  REQUIRED_VALID(stmt_);
  REQUIRED(pred.pIsMember(this, stmt_) == true);
  IRStmt* lPrev(stmt_->GetPrevStmt());
  IRStmt* lNext(stmt_->GetNextStmt());
  if (pred.pIsFirstStmt(stmt_) == true) {
    lNext->SetPrevStmt(lPrev);
  } else if (pred.pIsLastStmt(stmt_) == true) {
    lPrev->SetNextStmt(lNext);
  } else {
    lNext->SetPrevStmt(lPrev);
    lPrev->SetNextStmt(lNext);
  } // if
  if (pred.pIsCFStmt(stmt_) == true) {
    for (hFUInt32 c(0); c < mSuccs.size(); ++ c) {
      vector<IRBB*>& succPreds(mSuccs[c]->mPreds);
      ASSERT(find(succPreds.begin(), succPreds.end(), this) != succPreds.end());
      succPreds.erase(find(succPreds.begin(), succPreds.end(), this));
    } // for
    mSuccs.clear();
  } // if
  mStmts.erase(find(mStmts.begin(), mStmts.end(), stmt_));
  stmt_->Invalidate();
  ENSURE_VALID(this);
  EMDEBUG0();
  return;
} // IRBB::RemoveStmt

IRSSwitch::
IRSSwitch(IRExpr* value_, const vector<pair<IRExprList*, IRBB*> >& caseStmts_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mValueExpr(value_),
  mCaseStmts(caseStmts_)
{
  BMDEBUG2("IRSSwitch::IRSSwitch", mValueExpr);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(value_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(mCaseStmts.size() != 0, "Switch statement must have at least one 'case'");
  mValueExpr->SetParent(this);
  for (hFUInt32 c(0); c < mCaseStmts.size(); ++ c) {
    mCaseStmts[c].first->SetParent(this);
  } // for
  EMDEBUG0();
} // IRSSwitch::IRSSwitch

bool IRSSwitch::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSSwitch::remapJumpTargets", from_, to_);
  bool retVal(false);
  for (hFUInt32 c(0); c < mCaseStmts.size(); ++ c) {
    if (mCaseStmts[c].second == from_) {
      mCaseStmts[c].second = to_;
      retVal = true;
      //! \todo M Design: check if we can 'break' here.
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRSSwitch::remapJumpTargets

bool IREUnary::
IsValid() const {
  BMDEBUG1("IREUnary::IsValid");
  REQUIRED_VALIDMSG(mOperand, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IREUnary::IsValid

bool IREBinary::
IsValid() const {
  BMDEBUG1("IREBinary::IsValid");
  REQUIRED_VALIDMSG(mLeftExpr, ieStrObjectNotValid);
  REQUIRED_VALIDMSG(mRightExpr, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IREBinary::IsValid

bool IREAdd::
IsValid() const {
  BMDEBUG1("IREAdd::IsValid");
  REQUIREDMSG(pred.pIsNonTerminal(GetLeftExpr()) == true || !(pred.pIsPtr(GetLeftExpr()->GetType()) == true &&
    pred.pIsPtr(GetRightExpr()->GetType()) == true), ieStrObjectNotValid <<
    " : You cannot add two pointers." << endl <<
    "Left=" << progcxt(GetLeftExpr()) << endl << "Right=" << progcxt(GetRightExpr()));
  REQUIREDMSG(pred.pIsNonTerminal(GetLeftExpr()) == true || pred.pIsPtr(GetLeftExpr()->GetType()) == false,
    ieStrObjectNotValid << " : Use IREPtrAdd for addition to pointer types");
  REQUIREDMSG(pred.pIsNonTerminal(GetLeftExpr()) == true || pred.pIsPtr(GetRightExpr()->GetType()) == false,
    ieStrObjectNotValid << " : Use IREPtrAdd for addition to pointer types");
  EMDEBUG0();
  return true;
} // IREAdd::IsValid

bool IRESub::
IsValid() const {
  REQUIREDMSG(pred.pIsNonTerminal(GetLeftExpr()) == true || pred.pIsNonTerminal(GetRightExpr()) == true ||
    !(pred.pIsPtr(GetLeftExpr()->GetType()) == true &&
    pred.pIsPtr(GetRightExpr()->GetType()) == true), ieStrObjectNotValid <<
    " : Use IREPtrSub for subtraction of pointer types." << endl <<
    "Left=" << progcxt(GetLeftExpr()) << endl << "Right=" << progcxt(GetRightExpr()));
  REQUIREDMSG(pred.pIsNonTerminal(GetLeftExpr()) == true || pred.pIsPtr(GetLeftExpr()->GetType()) == false,
    ieStrObjectNotValid << " : Use IREPtrSub for subtraction of pointer types");
  REQUIREDMSG(pred.pIsNonTerminal(GetLeftExpr()) == true || pred.pIsPtr(GetRightExpr()->GetType()) == false,
    ieStrObjectNotValid << " : Use IREPtrSub for subtraction of pointer types");
  return true;
} // IRESub::IsValid

bool IREDiv::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREDiv::IsValid

bool IREMul::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREMul::IsValid

bool IRERem::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRERem::IsValid

bool IREMod::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREMod::IsValid

bool IROBitField::
IsValid() const {
  REQUIREDMSG(mSign == IRSSigned || mSign == IRSUnsigned, ieStrObjectNotValid);
  REQUIREDMSG(mBitSize >= 0, ieStrObjectNotValid);
  return true;
} // IROBitField::IsValid

bool IREQuo::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREQuo::IsValid

bool IREANeg::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREANeg::IsValid

bool IREBNeg::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREBNeg::IsValid

bool IRENot::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRENot::IsValid

bool IREBAnd::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREBAnd::IsValid

bool IRELAnd::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRELAnd::IsValid

bool IREBXOr::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREBXOr::IsValid

bool IREBOr::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREBOr::IsValid

bool IRELOr::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRELOr::IsValid

bool IRECast::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRECast::IsValid

bool IRECmp::
IsValid() const {
  BMDEBUG1("IRECmp::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IRECmp::IsValid

bool IREGt::
IsValid() const {
  BMDEBUG1("IREGt::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IREGt::IsValid

bool IRELt::
IsValid() const {
  BMDEBUG1("IRELt::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IRELt::IsValid

bool IREGe::
IsValid() const {
  BMDEBUG1("IREGe::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IREGe::IsValid

bool IRELe::
IsValid() const {
  BMDEBUG1("IRELe::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IRELe::IsValid

bool IREEq::
IsValid() const {
  BMDEBUG1("IREEq::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IREEq::IsValid

bool IRENe::
IsValid() const {
  BMDEBUG1("IRENe::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IRENe::IsValid

bool IREStrConst::
IsValid() const {
  BMDEBUG1("IREStrConst::IsValid");
  EMDEBUG0();
  return true;
} // IREStrConst::IsValid

bool IREIntConst::
IsValid() const {
  BMDEBUG1("IREIntConst::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IREIntConst::IsValid

bool IREBoolConst::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREBoolConst::IsValid

bool IREAddrConst::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREAddrConst::IsValid

bool IREShiftLeft::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREShiftLeft::IsValid

bool IREAShiftRight::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IREAShiftRight::IsValid

bool IRELShiftRight::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRELShiftRight::IsValid

bool IRENull::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRENull::IsValid

bool IREMember::
IsValid() const {
  REQUIRED_VALIDMSG(GetLeftExpr(), ieStrObjectNotValid);
  REQUIREDMSG(dynamic_cast<const IROField*>(GetRightExpr()), ieStrObjectNotValid);
  return true;
} // IREMember::IsValid

bool IRESubscript::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRESubscript::IsValid

bool IREAddrOf::
IsValid() const {
  BMDEBUG1("IREAddrOf::IsValid");
  REQUIREDMSG(!mExpr || mExpr->IsValid(), ieStrObjectNotValid);
  REQUIREDMSG(!mLabel || mLabel->IsValid(), ieStrObjectNotValid);
  bool retVal(IRExpr::IsValid());
  EMDEBUG1(retVal);
  return retVal;
} // IREAddrOf::IsValid

bool IREDeref::
IsValid() const {
  BMDEBUG1("IREDeref::IsValid");
  REQUIRED_VALIDMSG(GetExpr(), ieStrObjectNotValid);
  REQUIREDMSG(pred.pIsPtrOrArray(GetExpr()->GetType()) == true,
     ieStrInconsistentInternalStructure << "Deref of non-pointer type");
  EMDEBUG0();
  return true;
} // IREDeref::IsValid

bool IREPtrSub::
IsValid() const {
  BMDEBUG1("IREPtrSub::IsValid");
  REQUIREDMSG(pred.pIsInt(GetType()) == true, ieStrObjectNotValid);
  REQUIREDMSG(pred.pIsPtr(GetLeftExpr()->GetType()) == true, ieStrObjectNotValid);
  REQUIREDMSG(pred.pIsPtr(GetRightExpr()->GetType()) == true, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IREPtrSub::IsValid

bool IREPtrAdd::
IsValid() const {
  BMDEBUG1("IREPtrAdd::IsValid");
  REQUIREDMSG(pred.pIsPtr(GetType()) == true, ieStrObjectNotValid);
  REQUIREDMSG(pred.pIsNonTerminal(GetLeftExpr()) == true ||
    pred.pIsPtr(GetLeftExpr()->GetType()) == true, ieStrObjectNotValid);
  REQUIREDMSG(pred.pIsNonTerminal(GetRightExpr()) == true ||
    pred.pIsInt(GetRightExpr()->GetType()) == true, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IREPtrAdd::IsValid

bool IR::
IsValid() const {
  REQUIRED(0); // should not be called.
  return false;
} // IR::IsValid

bool IRType::
IsValid() const {
  BMDEBUG1("IRType::IsValid");
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // IRType::IsValid

bool IRTFunc::
IsValid() const {
  BMDEBUG1("IRTFunc::IsValid");
  REQUIRED_VALIDMSG(mRetType, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IRTFunc::IsValid

bool IRTArray::
IsValid() const {
  BMDEBUG1("IRTArray::IsValid");
  REQUIREDMSG(mElemType != 0, ieStrObjectNotValid);
  REQUIREDMSG(mCount >= 0, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IRTArray::IsValid

bool IRTPtr::
IsValid() const {
  BMDEBUG1("IRTPtr::IsValid");
  REQUIREDMSG(mRefType != 0, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IRTPtr::IsValid

bool IRTInt::
IsValid() const {
  BMDEBUG1("IRTInt::IsValid");
  REQUIREDMSG(mSign == IRSUnsigned || mSign == IRSSigned, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IRTInt::IsValid

bool IRTStruct::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRTStruct::IsValid

bool IRTUnion::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRTUnion::IsValid

bool IRTReal::
IsValid() const {
  NOTIMPLEMENTED(M);
  return true;
} // IRTReal::IsValid

bool InsertContext::
IsValid() const {
  BMDEBUG1("InsertContext::IsValid");
  REQUIRED(mPlace != ICInvalid);
  EMDEBUG0();
  return true;
} // InsertContext::IsValid

bool IRExprList::
IsValid() const {
  bool retVal(IRExpr::IsValid());
  return retVal;
} // IRExprList::IsValid

bool CFG::
IsValid() const {
  BMDEBUG1("CFG::IsValid");
  REQUIRED_VALID(mEntryBB);
  REQUIRED_VALID(mExitBB);
  NOTIMPLEMENTED(M);
  EMDEBUG0();
  return true;
} // CFG::IsValid

bool IRFunctionDecl::
IsValid() const {
  BMDEBUG1("IRFunctionDecl::IsValid");
  EMDEBUG0();
  return true;
} // IRFunctionDecl::IsValid

bool IRFunction::
IsValid() const {
  BMDEBUG1("IRFunction::IsValid");
  EMDEBUG0();
  return true;
} // IRFunction::IsValid

bool IRModule::
IsValid() const {
  BMDEBUG1("IRModule::IsValid");
  EMDEBUG0();
  return true;
} // IRModule::IsValid

bool IRProgram::
IsValid() const {
  BMDEBUG1("IRProgram::IsValid");
  EMDEBUG0();
  return true;
} // IRProgram::IsValid

bool IRObject::
IsValid() const {
  BMDEBUG1("IRObject::IsValid");
  EMDEBUG0();
  return true;
} // IRObject::IsValid

bool IROField::
IsValid() const {
  IRObject::IsValid();
  REQUIREDMSG(mOffsetInBits >= 0, ieStrObjectNotValid << ": fields must have positive offset");
  return true;
} // IROField::IsValid

bool IROParameter::
IsValid() const {
  BMDEBUG1("IROParameter::IsValid");
  EMDEBUG0();
  return true;
} // IROParameter

bool IROLocal::
IsValid() const {
  BMDEBUG1("IROLocal::IsValid");
  EMDEBUG0();
  return true;
} // IROLocal::IsValid

bool IROGlobal::
IsValid() const {
  BMDEBUG1("IROGlobal::IsValid");
  EMDEBUG0();
  return true;
} // IROGlobal::IsValid

bool IRSAssign::
IsValid() const {
  BMDEBUG1("IRSAssign::IsValid");
  EMDEBUG0();
  return true;
} // IRSAssign::IsValid

bool IRSEval::
IsValid() const {
  BMDEBUG1("IRSEval::IsValid");
  EMDEBUG0();
  return true;
} // IRSEval::IsValid

bool IRSPCall::
IsValid() const {
  IRSCall::IsValid();
  return true;
} // IRSPCall::IsValid

bool IRSFCall::
IsValid() const {
  REQUIREDMSG(GetReturnIn() != 0, ieStrInconsistentInternalStructure);
  IRSCall::IsValid();
  return true;
} // IRSFCall::IsValid

bool IRSLabel::
IsValid() const {
  BMDEBUG1("IRSLabel::IsValid");
  EMDEBUG0();
  return true;
} // IRSLabel::IsValid

bool IRExpr::
IsValid() const {
  BMDEBUG1("IRExpr::IsValid");
  // REQUIREDMSG(mParent != 0, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // IRExpr::IsValid

bool IRERealConst::
IsValid() const {
  BMDEBUG1("IRERealConst::IsValid");
  EMDEBUG0();
  return true;
} // IRERealConst::IsValid

bool ICBB::
IsValid() const {
  BMDEBUG1("ICBB::IsValid");
  REQUIRED(mBB != 0);
  REQUIRED(mPlace != ICInvalid);
  EMDEBUG0();
  return true;
} // ICBB::IsValid

bool ICStmt::
IsValid() const {
  BMDEBUG1("ICStmt::IsValid");
  REQUIRED(mStmt != 0);
  REQUIRED(mStmt->GetBB() != 0);
  REQUIRED(mBB != 0);
  REQUIRED(mPlace == ICAfter || mPlace == ICBefore);
  EMDEBUG0();
  return true;
} // ICStmt::IsValid

bool IRStmt::
IsValid() const {
  BMDEBUG1("IRStmt::IsValid");
  //! \note Parent must be non-null.
//  REQUIRED(parentBB != 0);
  //! \note If prev and next is null it must be the only statement in parent.
//  REQUIRED((prev != 0 || next != 0) || (extr.eGetNumOfStmts(parentBB) == 1));
  EMDEBUG0();
  return true;
} // IRStmt::IsValid

bool IRBB::
IsValid() const {
  BMDEBUG1("IRBB::IsValid");
  REQUIREDMSG(mParentCFG != 0, ieStrObjectNotValid);
  REQUIREDMSG(pred.pIsMember(mParentCFG, this) == true, ieStrObjectNotValid);
  list<IRStmt*>::const_iterator lIt(mStmts.begin());
  while (lIt != mStmts.end()) {
    IRStmt* currentStmt(*lIt);
    ++ lIt;
    ISVALIDMSG(currentStmt, ieStrObjectNotValid);
  } // while
  EMDEBUG0();
  return true;
} // IRBB::IsValid

bool IRSCall::
IsValid() const {
  BMDEBUG1("IRSCall::IsValid");
  REQUIREDMSG(mArgs != 0, ieStrObjectNotValid);
  //! \todo M Design: Find a way to do the check, I guess we should never
  //!       call isvalid within ctors, since it is a virtual function.
  // REQUIREDMSG(pred.pIsBuiltIn(this) == true || funcExpr != 0, ieStrObjectNotValid);
  EMDEBUG0();
  return IRStmt::IsValid();
} // IRSCall::IsValid

bool IRSIf::
IsValid() const {
  BMDEBUG1("IRSIf::IsValid");
  REQUIRED_VALIDMSG(mCondExpr, ieStrObjectNotValid);
  REQUIREDMSG(mTrueCaseBB != 0, ieStrObjectNotValid);
  REQUIREDMSG(mFalseCaseBB != 0, ieStrObjectNotValid);
  EMDEBUG0();
  return IRStmt::IsValid();
} // IRSIf::IsValid

bool IRSNull::
IsValid() const {
  BMDEBUG1("IRSNull::IsValid");
  EMDEBUG0();
  return IRStmt::IsValid();
} // IRSNull::IsValid

bool IRSAfterLastOfBB::
IsValid() const {
  BMDEBUG1("IRSAfterLastOfBB::IsValid");
  EMDEBUG0();
  return true;
} // IRSAfterLastOfBB::IsValid

bool IRSReturn::
IsValid() const {
  BMDEBUG1("IRSReturn::IsValid");
  EMDEBUG0();
  return IRStmt::IsValid();
} // IRSReturn::IsValid

bool IRSProlog::
IsValid() const {
  BMDEBUG1("IRSProlog::IsValid");
  EMDEBUG0();
  return IRStmt::IsValid();
} // IRSProlog::IsValid

bool IRSEpilog::
IsValid() const {
  BMDEBUG1("IRSEpilog::IsValid");
  EMDEBUG0();
  return IRStmt::IsValid();
} // IRSEpilog::IsValid

bool IRSSwitch::
IsValid() const {
  BMDEBUG1("IRSSwitch::IsValid");
  EMDEBUG0();
  return IRStmt::IsValid();
} // IRSSwitch::IsValid

//! \todo IRSBuiltInCall check if number and type of arguments are consistent
//!       with the built in enum identifier.
bool IRSBuiltInCall::
IsValid() const {
  BMDEBUG1("IRSBuiltInCall::IsValid");
  EMDEBUG0();
  return true;
} // IRSBuiltInCall::IsValid

bool IRSBeforeFirstOfBB::
IsValid() const {
  BMDEBUG1("IRSBeforeFirstOfBB::IsValid");
  EMDEBUG0();
  return true;
} // IRSBeforeFirstOfBB::IsValid

hFSInt32 IRBB::
getDFASetId() {
  BMDEBUG1("IRBB::getDFASetId");
  hFSInt32 retVal(-1);
  for (hFUInt32 c(0); c < mDFASets.size(); ++ c) {
    if (mDFASets[c].mInUse == false) {
      retVal = c;
      break;
    } // if
  } // for
  if (retVal == -1) {
    mDFASets.push_back(DFASets());
    retVal = mDFASets.size()-1;
  } // if
  ENSUREMSG(retVal != -1, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // IRBB::getDFASetId

bool IRSDynJump::
IsValid() const {
  REQUIREDMSG(mTargetExpr != 0, ieStrObjectNotValid);
  return IRStmt::IsValid();
} // IRSDynJump::IsValid

bool IRSJump::
IsValid() const {
  REQUIREDMSG(mTargetBB != 0, ieStrObjectNotValid);
  return IRStmt::IsValid();
} // IRSJump::IsValid

IRTree* IRSSwitch::
GetRecCopy() const {
  vector<pair<IRExprList*, IRBB*> > lCopyCaseStmts(mCaseStmts);
  for (hFUInt32 c(0); c < lCopyCaseStmts.size(); ++ c) {
    lCopyCaseStmts[c].first = static_cast<IRExprList*>(lCopyCaseStmts[c].first->GetRecCopyExpr());
  } // for
  return new IRSSwitch(mValueExpr->GetRecCopyExpr(), lCopyCaseStmts, diGetDIE()->Clone());
} // IRSSwitch::GetRecCopy

void IRExprList::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IRExprList::ReplaceChild", old_, new_);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  bool lOldFound(false);
  for (hFUInt32 c(0); c < mExprList.size(); ++ c) {
    if (mExprList[c] == old_) {
      mExprList[c] = new_;
      lOldFound = true;
      break;
    } // if
  } // for
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  ENSUREMSG(lOldFound == true, ieStrParamValuesDBCViolation);
  EMDEBUG0();
  return;
} // IRExprList::ReplaceChild

//! \todo M Design: Would it be better to visit the child expressions, to make
//!       it more uniform?
bool IRExprList::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG2("IRExprList::Accept", &visitor_);
  bool retVal(true);
  if (visitor_.GetOrderType() == EVOTPre) {
    retVal = visitor_.Visit(this);
  } // if
  if (retVal == true) {
    for (hFUInt32 c(0); c < mExprList.size(); ++ c) {
      if (!mExprList[c]->Accept(visitor_)) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  if (visitor_.GetOrderType() == EVOTPost) {
    retVal &= visitor_.Visit(this);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRExprList::Accept

bool IRExprList::
Accept(ExprVisitor& visitor_) {
  BMDEBUG2("IRExprList::Accept", &visitor_);
  bool retVal(true);
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  if (visitor_.GetOrderType() == EVOTPre) {
    retVal = visitor_.Visit(this);
  } // if
  if (retVal == true) {
    for (hFUInt32 c(0); c < mExprList.size(); ++ c) {
      if (!mExprList[c]->Accept(visitor_)) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  if (visitor_.GetOrderType() == EVOTPost) {
    retVal &= visitor_.Visit(this);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRExprList::Accept

IROGlobal* IRModule::
FindGlobal(const string& name_) const {
  BMDEBUG2("IRModule::FindGlobal", name_);
  IROGlobal* retVal(&Invalid<IROGlobal>::Obj());
  map<string, IROGlobal*>::const_iterator lIt(mMapNameGlobals.find(name_));
  if (lIt != mMapNameGlobals.end()) {
    retVal = lIt->second;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRModule::FindGlobal

void CFG::
SetPhysOrderOfBBs(const vector<IRBB*>& bbs_) {
  BMDEBUG1("CFG::SetPhysOrderOfBBs");
  REQUIREDMSG(bbs_.size() == mBBs.size(), ieStrParamValuesDBCViolation);
  REQUIREDMSG(bbs_.front() == mEntryBB && bbs_.back() == mExitBB, ieStrParamValuesDBCViolation);
  list<IRBB*> lNewList;
  // First get the mNext/mPrev of IRBBs right, also do some checks.
  for (hFUInt32 c(0); c < bbs_.size(); ++ c) {
    lNewList.push_back(bbs_[c]);
    ASSERTMSG(find(mBBs.begin(), mBBs.end(), bbs_[c]) != mBBs.end(), ieStrParamValuesDBCViolation);
    if (c == 0) {
      bbs_[c]->mNext = bbs_[c+1]; 
    } else if (c == bbs_.size()) {
      bbs_[c]->mPrev = bbs_[c-1]; 
    } else {
      bbs_[c]->mNext = bbs_[c+1]; 
      bbs_[c]->mPrev = bbs_[c-1]; 
    } // if
  } // for
  mBBs = lNewList;
  EMDEBUG0();
}; // CFG::SetPhysOrderOfBBs

void CFG::
ResetStmtMarks(eIRTreeMark what_) {
  BMDEBUG1("CFG::ResetStmtMarks");
  ListIterator<IRStmt*> lStmtIter(extr.eGetStmtIter(GetFunc()));
  for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
    lStmtIter->mIsMarked[what_].setUnknown();
  } // for
  EMDEBUG0();
  return;
} // CFG::ResetStmtMarks

IRFunction::
IRFunction(IRFunctionDecl* funcDecl_, CFG* cfg_) :
  mCFG(cfg_),
  mStackLocalSize(0),
  mStackSize(0),
  mParamsSize(0),
  mDeclaration(funcDecl_),
  mDIE(&Invalid<DIEBase>::Obj()),
  mCGContext(0),
  mInlineFlag(IFNotDecided)
{
  BMDEBUG3("IRFunction::IRFunction", funcDecl_, cfg_);
  mCFG->setFunction(this);
  EMDEBUG0();
} // IRFunction::IRFunction

CGFuncContext* IRFunction::
GetCGContext() {
  BMDEBUG1("IRFunction::GetCGContext");
  if (!mCGContext) {
    CGContext* lCGContext(GetDeclaration()->GetModule()->GetCGContext());
    mCGContext = new CGFuncContext(*lCGContext->GetDefFuncContext(), this);
  } // if
  ENSUREMSG(mCGContext != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(mCGContext);
  return mCGContext;
} // IRFunction::GetCGContext

AddrReg::
AddrReg() :
  mReg(&Invalid<Register>::Obj())
{
} // AddrReg::AddrReg

AddrStack::
AddrStack() :
  mReg(&Invalid<Register>::Obj()),
  mOffset(0)
{
} // AddrStack::AddrStack

void IRSSwitch::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IRSSwitch::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mValueExpr, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  mValueExpr = new_;
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IRSSwitch::ReplaceChild

void IRSAssign::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IRSAssign::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mLHS || old_ == mRHS, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  if (mRHS == old_) {
    mRHS = new_;
  } else {
    mLHS = new_;
  } // if
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IRSAssign::ReplaceChild

void IRSEval::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IRSEval::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mExpr, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  mExpr = new_;
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IRSEval::ReplaceChild

void IREUnary::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IREUnary::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mOperand, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  mOperand = new_;
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IREUnary::ReplaceChild

void IRSIf::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IRSIf::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mCondExpr, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  mCondExpr = new_;
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IRSIf::ReplaceChild

void IRSDynJump::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IRSDynJump::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mTargetExpr, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  mTargetExpr = new_;
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IRSDynJump::ReplaceChild

void IRSReturn::
ReplaceChild(IRExpr* old_, IRExpr* new_) {
  BMDEBUG3("IRSReturn::ReplaceChild", old_, new_);
  REQUIREDMSG(old_ == mRetExpr, ieStrParamValuesDBCViolation);
  REQUIREDMSG(new_ != 0, ieStrNonNullParam);
  irBuilder->smirbDetached(old_);
  mRetExpr = new_;
  new_->SetParent(this);
  old_->SetParent(0);
  irBuilder->smirbAttached(new_);
  EMDEBUG0();
  return;
} // IRSReturn::ReplaceChild

IRExpr* IRExpr::
ReplaceWith(IRExpr* replacement_) {
  BMDEBUG2("IRExpr::ReplaceWith", replacement_);
  IRExpr* retVal(irBuilder->smirbOnReplaceWith(this, replacement_));
  replacement_->SetParent(GetParent());
  GetParent()->ReplaceChild(this, retVal);
  //! \todo L Design: Check why register non-terminal is a child of fcall?
  //!       comment the mParent = 0 line.
  mParent = 0;
  ENSUREMSG(mParent == 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // IRExpr::ReplaceWith

IRStmt* IRStmt::
ReplaceWith(IRStmt* replacement_) {
  BMDEBUG2("IRStmt::ReplaceWith", replacement_);
  IRStmt* retVal(replacement_);
  IRBuilder irBuilder(irProgram, consts.cIRBNoOptims);
  InsertContext* lMarkIC(extr.eGetReplacementIC(this));
  irBuilder.irbRemoveStmt(this);
  irBuilder.irbInsertStmt(replacement_, *lMarkIC);
  EMDEBUG1(retVal);
  return retVal;
} // IRStmt::ReplaceWith

void IRProgram::
SetCGContext(CGContext* cgContext_) {
  BMDEBUG2("IRProgram::SetCGContext", cgContext_);
  REQUIREDMSG(cgContext_ != 0, ieStrNonNullParam);
  REQUIREDCALLONCE();
  mCGContext = cgContext_;
  EMDEBUG0();
  return;
} // IRProgram::SetCGContext

CGContext* IRProgram::
GetCGContext() const {
  BMDEBUG1("IRModule::GetCGContext");
  REQUIREDMSG(mCGContext != 0, ieStrDBCViolation);
  EMDEBUG1(mCGContext);
  return mCGContext;
} // IRProgram::GetCGContext

void IRModule::
SetCGContext(CGContext* cgContext_) {
  BMDEBUG2("IRModule::SetCGContext", cgContext_);
  REQUIREDMSG(cgContext_ != 0, ieStrNonNullParam);
  REQUIREDCALLONCE();
  mCGContext = cgContext_;
  EMDEBUG0();
  return;
} // IRModule::SetCGContext

CGContext* IRModule::
GetCGContext() {
  BMDEBUG1("IRModule::GetCGContext");
  if (!mCGContext) {
    ASSERTMSG(irProgram != 0 && !pred.pIsInvalid(irProgram), ieStrInconsistentInternalStructure);
    mCGContext = irProgram->GetCGContext();
  } // if
  ENSUREMSG(mCGContext != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(mCGContext);
  return mCGContext;
} // IRModule::GetCGContext

IRBB::
IRBB() : 
  mParentCFG(&Invalid<CFG>::Obj()), 
  mPrev(0), 
  mNext(0), 
  mDIE(&Invalid<DIEBase>::Obj()) 
{
} // IRBB::IRBB

bool IRENe::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRENe::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() != lRight->GetValue());
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRENe::constFold

bool IREEq::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREEq::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() == lRight->GetValue());
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREEq::constFold

bool IRELt::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRELt::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() < lRight->GetValue());
    } // if
  } // if
  if (!foldedExpr_ && pred.pIsConst(GetLeftExpr()) == true) {
    foldedExpr_ = irBuilder->irbeGt(GetRightExpr()->GetRecCopyExpr(), GetLeftExpr()->GetRecCopyExpr());
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRELt::constFold

bool IREGt::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREGt::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() > lRight->GetValue());
    } // if
  } // if
  if (!foldedExpr_ && pred.pIsConst(GetLeftExpr()) == true) {
    foldedExpr_ = irBuilder->irbeLt(GetRightExpr()->GetRecCopyExpr(), GetLeftExpr()->GetRecCopyExpr());
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREGt::constFold

bool IRELe::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRELe::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() <= lRight->GetValue());
    } // if
  } // if
  if (!foldedExpr_ && pred.pIsConst(GetLeftExpr()) == true) {
    foldedExpr_ = irBuilder->irbeGe(GetRightExpr()->GetRecCopyExpr(), GetLeftExpr()->GetRecCopyExpr());
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRELe::constFold

bool IREGe::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREGe::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() >= lRight->GetValue());
    } // if
  } // if
  if (!foldedExpr_ && pred.pIsConst(GetLeftExpr()) == true) {
    foldedExpr_ = irBuilder->irbeLe(GetRightExpr()->GetRecCopyExpr(), GetLeftExpr()->GetRecCopyExpr());
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREGe::constFold

bool IRENot::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRENot::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREBoolConst* lExpr = dynamic_cast<IREBoolConst*>(GetExpr())) {
    foldedExpr_ = irBuilder->irbeBoolConst(!lExpr->GetValue());
  } else if (IRENot* lNot = dynamic_cast<IRENot*>(GetExpr())) {
    foldedExpr_ = lNot->GetExpr()->GetRecCopyExpr();
  } else if (IRENe* lNe = dynamic_cast<IRENe*>(GetExpr())) {
    foldedExpr_ = irBuilder->irbeEq(lNe->GetLeftExpr()->GetRecCopyExpr(), lNe->GetRightExpr()->GetRecCopyExpr());
  } else if (IREEq* lEq = dynamic_cast<IREEq*>(GetExpr())) {
    foldedExpr_ = irBuilder->irbeNe(lEq->GetLeftExpr()->GetRecCopyExpr(), lEq->GetRightExpr()->GetRecCopyExpr());
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRENot::constFold

bool IRELAnd::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRELAnd::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREBoolConst* lLeft = dynamic_cast<IREBoolConst*>(GetLeftExpr())) {
    if (IREBoolConst* lRight = dynamic_cast<IREBoolConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() && lRight->GetValue());
    } else if (lLeft->GetValue() == true) {
      foldedExpr_ = GetRightExpr()->GetRecCopyExpr();
    } else if (lLeft->GetValue() == false) {
      foldedExpr_ = irBuilder->irbeBoolConst(false);
    } // if
  } else if (IREBoolConst* lRight = dynamic_cast<IREBoolConst*>(GetRightExpr())) {
    if (lRight->GetValue() == true) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } else if (lRight->GetValue() == false) {
      foldedExpr_ = irBuilder->irbeBoolConst(false);
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRELAnd::constFold

bool IRELOr::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRELOr::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREBoolConst* lLeft = dynamic_cast<IREBoolConst*>(GetLeftExpr())) {
    if (IREBoolConst* lRight = dynamic_cast<IREBoolConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeBoolConst(lLeft->GetValue() || lRight->GetValue());
    } else if (lLeft->GetValue() == true) {
      foldedExpr_ = irBuilder->irbeBoolConst(true);
    } else if (lLeft->GetValue() == false) {
      foldedExpr_ = GetRightExpr()->GetRecCopyExpr();
    } // if
  } else if (IREBoolConst* lRight = dynamic_cast<IREBoolConst*>(GetRightExpr())) {
    if (lRight->GetValue() == true) {
      foldedExpr_ = irBuilder->irbeBoolConst(true);
    } else if (lRight->GetValue() == false) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRELOr::constFold

bool IREBAnd::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREBAnd::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() & lRight->GetValue());
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lRight->GetType());
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREBAnd::constFold

bool IREBOr::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREBOr::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() | lRight->GetValue());
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = GetRightExpr()->GetRecCopyExpr();
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREBOr::constFold

bool IREAdd::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREAdd::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() + lRight->GetValue());
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = GetRightExpr()->GetRecCopyExpr();
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } else {
    IREANeg* lNegLeft(dynamic_cast<IREANeg*>(GetLeftExpr()));
    IREANeg* lNegRight(dynamic_cast<IREANeg*>(GetRightExpr()));
    if (lNegLeft != 0 && lNegRight != 0) {
      // -l + -r == -(l+r)
      foldedExpr_ = irBuilder->irbeANeg(irBuilder->irbeAdd(lNegLeft->GetExpr()->GetRecCopyExpr(), lNegRight->GetExpr()->GetRecCopyExpr()));
    } else if (lNegLeft != 0) {
      // -l + r == r - l
      foldedExpr_ = irBuilder->irbeSub(GetRightExpr()->GetRecCopyExpr(), lNegLeft->GetExpr()->GetRecCopyExpr());
    } else if (lNegRight != 0) {
      // l + (-r) == l - r
      foldedExpr_ = irBuilder->irbeSub(GetLeftExpr()->GetRecCopyExpr(), lNegRight->GetExpr()->GetRecCopyExpr());
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREAdd::constFold

bool IRESub::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRESub::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() - lRight->GetValue());
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeANeg(GetRightExpr()->GetRecCopyExpr());
    } // if
  } else if (IREANeg* lRight = dynamic_cast<IREANeg*>(GetRightExpr())) {
    foldedExpr_ = irBuilder->irbeAdd(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRESub::constFold

bool IREShiftLeft::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREShiftLeft::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() << lRight->GetValue());
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREShiftLeft::constFold

bool IRELShiftRight::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRELShiftRight::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() >> lRight->GetValue());
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRELShiftRight::constFold

bool IREAShiftRight::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREAShiftRight::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() >> lRight->GetValue());
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREAShiftRight::constFold

bool IREANeg::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREANeg::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lExpr = dynamic_cast<IREIntConst*>(GetExpr())) {
    foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lExpr->GetType()), -lExpr->GetValue());
  } else if (IREANeg* lExpr = dynamic_cast<IREANeg*>(GetExpr())) {
    foldedExpr_ = lExpr->GetExpr()->GetRecCopyExpr();
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREANeg::constFold

bool IREMul::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREMul::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() * lRight->GetValue());
    } else if (lLeft->GetValue() == -1) {
      foldedExpr_ = irBuilder->irbeANeg(GetRightExpr()->GetRecCopyExpr());
    } else if (lLeft->GetValue() == 1) {
      foldedExpr_ = GetRightExpr()->GetRecCopyExpr();
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == -1) {
      foldedExpr_ = irBuilder->irbeANeg(GetLeftExpr()->GetRecCopyExpr());
    } else if (lRight->GetValue() == 1) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } else if (lRight->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREMul::constFold

bool IREDiv::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREDiv::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      if (lRight->GetValue() == 0) {
        STATRECORD("constfold: div by zero skipped");
      } else {
        foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() / lRight->GetValue());
      } // if
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      STATRECORD("constfold: div by zero skipped");
    } else if (lRight->GetValue() == -1) {
      foldedExpr_ = irBuilder->irbeANeg(GetLeftExpr()->GetRecCopyExpr());
    } else if (lRight->GetValue() == 1) {
      foldedExpr_ = GetLeftExpr()->GetRecCopyExpr();
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREDiv::constFold

bool IREMod::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IREMod::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  if (IREIntConst* lLeft = dynamic_cast<IREIntConst*>(GetLeftExpr())) {
    if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
      if (lRight->GetValue() == 0) {
        STATRECORD("constfold: mod by zero skipped");
      } else {
        foldedExpr_ = irBuilder->irbeIntConst(static_cast<IRTInt*>(lLeft->GetType()), lLeft->GetValue() % lRight->GetValue());
      } // if
    } else if (lLeft->GetValue() == 0) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } else if (IREIntConst* lRight = dynamic_cast<IREIntConst*>(GetRightExpr())) {
    if (lRight->GetValue() == 0) {
      STATRECORD("constfold: mod by zero skipped");
    } else if (lRight->GetValue() == 1) {
      foldedExpr_ = irBuilder->irbeZero(lLeft->GetType());
    } // if
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IREMod::constFold

bool IRECast::
constFold(IRExpr*& foldedExpr_) {
  BMDEBUG1("IRECast::constFold");
  bool retVal(false);
  foldedExpr_ = 0;
  IRECast* lExprCast(dynamic_cast<IRECast*>(GetExpr()));
  if (GetType() == GetExpr()->GetType()) {
    // T expr; (T)expr -> "expr".
    foldedExpr_ = GetExpr()->GetRecCopyExpr();
  } else if (pred.pIsPtr(GetType()) == true && lExprCast != 0 && pred.pIsPtr(lExprCast->GetType()) == true) {
    // T* expr; (K*)(M*)expr -> (K*)expr; drop (M*)
    retVal = irBuilder->irbeCast(GetType(), lExprCast->GetExpr());
  } // if
  retVal = foldedExpr_ != 0;
  EMDEBUG1(retVal);
  return retVal;
} // IRECast::constFold





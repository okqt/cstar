// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __IRS_INLINE_CPP__
#define __IRS_INLINE_CPP__

// \file irs_inline.cpp This file is to be included from irs.hpp.

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

INLINE__
ICBB::
ICBB(ICPlace invalid_) {
  BMDEBUG1("ICBB::ICBB");
  REQUIRED(invalid_ == ICInvalid);
  EMDEBUG0();
} // ICBB::ICBB

//! \todo M Design: Make compiler work with different sizes of bools, 8, 16, 32.
INLINE__
IRTBool::
IRTBool() :
  IRType(INBITS(32), 0)
{
  BMDEBUG1("IRTBool::IRTBool");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTBool::IRTBool

INLINE__
IRTLabel::
IRTLabel() :
  IRType(INBITS(0), 0)
{
  BMDEBUG1("IRTLabel::IRTLabel");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTLabel::IRTLabel

INLINE__
void IRFunction::
AddLocal(IROLocal* local_) {
  BMDEBUG2("IRFunction::AddLocal", local_);
  mLocals.push_back(local_);
  EMDEBUG0();
  return;
} // IRFunction::AddLocal

//! \brief Constructs an insert context from a basic block and a place.
//! \post Valid this.
INLINE__
ICBB::
ICBB(IRBB* bb_, ICPlace place_) :
  InsertContext(bb_, place_)
{
  BMDEBUG3("ICBB::ICBB", bb_, place_);
  ENSURE_VALID(this);
  EMDEBUG0();
} // ICBB::ICBB

INLINE__
ICBB::
ICBB(const ICBB& ic_) :
  InsertContext(ic_.mBB, ic_.mPlace)
{
  BMDEBUG3("ICBB::ICBB", ic_.mBB, ic_.mPlace);
  ENSURE_VALID(this);
  EMDEBUG0();
} // ICBB::ICBB

INLINE__
ICBB& ICBB::
operator = (const ICBB& ic_) {
  mBB = ic_.mBB;
  mPlace = ic_.mPlace;
  ENSURE_VALID(this);
  return *this;
} // ICBB::operator =

//! \brief Returns the basic block of the insert context.
INLINE__
IRBB* ICBB::
GetBB() const {
  BMDEBUG1("ICBB::GetBB");
  REQUIRED(mBB != 0);
  EMDEBUG1(mBB);
  return mBB;
} // ICBB::GetBB

//! \brief Returns the basic block of the insert context.
INLINE__
IRBB* ICStmt::
GetBB() const {
  BMDEBUG1("ICBB::GetBB");
  //! \todo M Design: Add a warning message to debug output, if stmt->GetBB() != bb.
  REQUIRED(mStmt->GetBB() != 0);
  EMDEBUG1(mStmt->GetBB());
  return mStmt->GetBB();
} // ICStmt::GetBB

//! \brief Provide the default constructor to be used along with an assignment.
//! \post The object is invalid.
INLINE__
ICStmt::
ICStmt(ICPlace place_) :
  mStmt(&Invalid<IRStmt>::Obj())
{
  BMDEBUG1("ICStmt::ICStmt");
  REQUIRED(place_ == ICInvalid);
  EMDEBUG0();
} // ICStmt::ICStmt

//! \brief Constructs an insert context from a statement and a place.
//! \post Valid this.
INLINE__
ICStmt::
ICStmt(const ICStmt& ic_) :
  InsertContext(ic_.mBB, ic_.mPlace),
  mStmt(ic_.mStmt)
{
  BMDEBUG3("ICStmt::ICStmt", mStmt, mPlace);
  ENSURE_VALID(this);
  EMDEBUG0();
} // ICStmt::ICStmt

INLINE__
ICStmt& ICStmt::
operator = (const ICStmt& ic_) {
  mStmt = ic_.mStmt;
  mPlace = ic_.mPlace;
  mBB = ic_.mBB;
  ENSURE_VALID(this);
  return *this;
} // ICStmt::operator =

INLINE__
IRStmt* IRBB::
GetFirstStmt() const {
  BMDEBUG1("IRBB::GetFirstStmt");
  REQUIREDMSG(!mStmts.empty(), "There must be at least one statement, use extract or check for emptiness");
  EMDEBUG1(mStmts.front());
  return mStmts.front();
} // IRBB::GetFirstStmt

INLINE__
IRFunctionDecl::
IRFunctionDecl(const string& name_, IRTFunc* funcPrototype_, IRModule* parentModule_) :
  mParentModule(parentModule_),
  mPrototype(funcPrototype_),
  mName(name_)
{
  BMDEBUG4("IRFunctionDecl::IRFunctionDecl", name_, funcPrototype_, parentModule_);
  EMDEBUG0();
} // IRFunctionDecl::IRFunctionDecl

INLINE__
void IRProgram::
AddModule(IRModule* module_) {
  BMDEBUG1("IRProgram::AddModule");
  REQUIRED(module_ != 0);
  mModules.push_back(module_);
  EMDEBUG0();
  return;
} // IRProgram::AddModule

INLINE__
void IRFunction::
SetCGContext(CGFuncContext* cgContext_) {
  BMDEBUG2("IRFunction::SetCGContext", cgContext_);
  REQUIREDCALLONCE();
  REQUIREDMSG(cgContext_ != 0, ieStrNonNullParam);
  mCGContext = cgContext_;
  EMDEBUG0();
  return;
} // IRFunction::SetCGContext

INLINE__
CFG* IRFunction::
GetCFG() const {
  BMDEBUG1("IRFunction::GetCFG");
  REQUIREDMSG(mCFG != 0, ieStrParamValuesDBCViolation);
  EMDEBUG1(mCFG);
  return mCFG;
} // IRFunction::GetCFG

INLINE__
void IRFunctionDecl::
AddParameter(IROParameter* param_) {
  BMDEBUG2("IRFunction::AddParameter", param_);
  REQUIREDMSG(param_ != 0, ieStrNonNullParam);
  REQUIREDBLOCK() {
    for (hFUInt32 c(0); c < mParams.size(); ++ c) {
      REQUIREDMSG(mParams[c]->GetName() != param_->GetName(), 
          ieStrParamValuesDBCViolation << " : parameter names may not repeat.");
    } // for
  } // REQUIREDBLOCK
  mParams.push_back(param_);
  EMDEBUG0();
  return;
} // IRFunctionDecl::AddParameter

INLINE__
IRBB* CFG::
GetEntryBB() const {
  BMDEBUG1("CFG::GetEntryBB");
  REQUIRED(mEntryBB != 0);
  EMDEBUG1(mEntryBB);
  return mEntryBB;
} // CFG::GetEntryBB

INLINE__
IROTmp::
IROTmp(const string& idName_, IRType* type_, IRObject* highLevelObject_) :
  IROLocal(type_, sGenerateName(idName_, highLevelObject_)),
  mHighLevelObject(highLevelObject_)
{
  BMDEBUG4("IROTmp::IROTmp", idName_, type_, highLevelObject_);
  EMDEBUG0();
} // IROTmp::IROTmp

INLINE__
IROTmp::
IROTmp(const string& idName_, IRType* type_) :
  IROLocal(type_, sGenerateName(idName_, &Invalid<IRObject>::Obj())),
  mHighLevelObject(&Invalid<IRObject>::Obj())
{
  BMDEBUG3("IROTmp::IROTmp", idName_, type_);
  EMDEBUG0();
} // IROTmp::IROTmp

INLINE__
IRSAssign::
IRSAssign(IRExpr* lhs_, IRExpr* rhs_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mLHS(lhs_),
  mRHS(rhs_)
{
  BMDEBUG4("IRSAssign::IRSAssign", lhs_, rhs_, die_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(lhs_, rhs_) == true, ieStrExprUsedBefore);
  mLHS->SetParent(this);
  mRHS->SetParent(this);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRSAssign::IRSAssign

INLINE__
bool IRSAssign::
GetChildren(vector<IRExpr*>& exprs_) const {
  exprs_.push_back(mLHS);
  exprs_.push_back(mRHS);
  return true;
} // IRSAssign::GetChildren

INLINE__
IROParameter::
IROParameter(IRType* type_, const string& name_) :
  IRObject(type_, name_)
{
  BMDEBUG1("IROParameter::IROParameter");
  EMDEBUG0();
} // IROParameter::IROParameter

INLINE__
IROLocal::
IROLocal(IRType* type_, const string& name_) :
  IRObject(type_, name_)
{
  BMDEBUG3("IROLocal::IROLocal", type_, name_);
  EMDEBUG0();
} // IROLocal::IROLocal

INLINE__
bool IREAddrOf::
GetChildren(vector<IRExpr*>& children_) const {
  REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
  ASSERTMSG(mLabel == 0, ieStrNotImplemented);
  children_.push_back(mExpr);
  return true;
} // IREAddrOf::GetChildren

INLINE__
IRExpr* IREAddrOf::
GetExpr() const {
  REQUIREDMSG(mExpr != 0, ieStrParamValuesDBCViolation);
  return mExpr;
} // IREAddrOf::GetExpr

INLINE__
IRORelocSymbol::
IRORelocSymbol(IRType* type_, const string& name_) :
  IRObject(type_, name_)
{
  BMDEBUG3("IRORelocSymbol::IRORelocSymbol", type_, name_);
  EMDEBUG0();
} // IRORelocSymbol::IRORelocSymbol

INLINE__
IROGlobal::
IROGlobal(IRType* type_, const string& name_, IRLinkage linkage_) :
  IRObject(type_, name_),
  mConstInits(&Invalid<IRExpr>::Obj()),
  mLinkage(linkage_)
{
  BMDEBUG4("IROGlobal::IROGlobal", type_, name_, linkage_);
  EMDEBUG0();
} // IROGlobal::IROGlobal

INLINE__
IROBitField::
IROBitField(const string& name_, IRType* type_, tBigInt offsetInBits_, hFSInt16 bitOffset_, tBigInt bitSize_, IRSignedness sign_) :
  IROField(name_, type_, offsetInBits_),
  mSign(sign_),
  mBitSize(bitSize_),
  mBitOffset(bitOffset_)
{
  BMDEBUG6("IROBitField::IROBitField", type_, &offsetInBits_, bitOffset_, &bitSize_, sign_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IROBitField::IROBitField

INLINE__
Address* IRObject::
GetAddress() const {
  BMDEBUG1("IRObject::GetAddress");
  REQUIREDMSG(mAddr != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<const IRORelocSymbol*>(this) == 0,
      ieStrParamValuesDBCViolation << "You may not get address of a relocation symbol");
  EMDEBUG1(mAddr);
  return mAddr;
} // IRObject::GetAddress

INLINE__
void IRObject::
SetAddress(const Address& addr_) {
  BMDEBUG2("IRObject::SetAddress", &addr_);
  REQUIREDMSG(dynamic_cast<IRORelocSymbol*>(this) == 0,
      ieStrParamValuesDBCViolation << "You may not set address of a relocation symbol");
  mAddr = addr_.Clone();
  EMDEBUG0();
} // IRObject::SetAddress

INLINE__
IROField::
IROField(const string& name_, IRType* type_, tBigInt offsetInBits_) :
  IRObject(type_, name_),
  mOffsetInBits(offsetInBits_)
{
  BMDEBUG4("IROField::IROField", name_, type_, &offsetInBits_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IROField::IROField

INLINE__
IRObject::
IRObject(IRType* type_, const string& name_) :
  IRExpr(type_),
  mName(name_),
  mDIE(&Invalid<DIEBase>::Obj()),
  mAddr(&Invalid<Address>::Obj()),
  mAddrTaken(0)
{
  BMDEBUG3("IRObject::IRObject", type_, mName);
  REQUIRED_VALID(type_);
  EMDEBUG0();
} // IRObject::IRObject

INLINE__
IRProgram::
IRProgram() :
  mDIE(&Invalid<DIEBase>::Obj()),
  mCGContext(0)
{
  // Do not use DBC, DebugTrace in Singletons.
  assert(sRefCount == 0);
  ++ sRefCount;
} // IRProgram::IRProgram

INLINE__
IRModule::
IRModule() :
  mDIE(&Invalid<DIEBase>::Obj()),
  mCGContext(0)
{
  BMDEBUG1("IRModule::IRModule");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRModule::IRModule

INLINE__
IRStmt::
IRStmt(const Invalid<IRStmt>* inv_) :
  IRTree(&Invalid<IRTree>::Obj()),
  mDIE(&Invalid<DIEBase>::Obj()),
  mPrev(this),
  mNext(this),
  mParentBB(&Invalid<IRBB>::Obj())
{
} // IRStmt::IRStmt

INLINE__
IRObject::
IRObject(Invalid<IRObject>* inv_) :
  IRExpr(&Invalid<IRExpr>::Obj()),
  mAddr(&Invalid<Address>::Obj()),
  mDIE(&Invalid<DIEBase>::Obj())
{
} // IRObject::IRObject

INLINE__
IRBB::
IRBB(const Invalid<IRBB>* inv_) :
  mParentCFG(&Invalid<CFG>::Obj()),
  mPrev(this),
  mNext(this),
  mDIE(&Invalid<DIEBase>::Obj())
{
} // IRBB::IRBB

INLINE__
IRBB* CFG::
GetExitBB() const {
  BMDEBUG1("CFG::GetExitBB");
  REQUIRED(mExitBB != 0);
  EMDEBUG1(mExitBB);
  return mExitBB;
} // CFG::GetExitBB

INLINE__
bool IRSAssign::
Accept(ExprVisitor& visitor_) {
  BMDEBUG2("IRSAssign::Accept", &visitor_);
  bool retVal(true);
  if (!visitor_.mOptions.IsLHSNo()) {
    if (GetLHS()->Accept(visitor_) == false) {
      retVal = false;
    } // if
  } // if
  if (retVal == true) {
    retVal = GetRHS()->Accept(visitor_);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRSAssign::Accept

INLINE__
bool IRSAssign::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG2("IRSAssign::Accept", &visitor_);
  bool retVal(true);
  if (!visitor_.mOptions.IsLHSNo()) {
    if (GetLHS()->Accept(visitor_) == false) {
      retVal = false;
    } // if
  } // if
  if (retVal == true) {
    retVal = GetRHS()->Accept(visitor_);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRSAssign::Accept

INLINE__
IRTree* IRSBuiltInCall::
GetRecCopy() const {
  IRStmt* retVal = 0;
  if (!mIsFunction) {
    retVal = new IRSBuiltInCall(GetFuncExpr()->GetRecCopyExpr(),
      static_cast<IRExprList*>(GetArgs()->GetRecCopyExpr()), mBIRId, diGetDIE()->Clone());
  } else {
    retVal = new IRSBuiltInCall(GetReturnIn(), GetFuncExpr()->GetRecCopyExpr(),
      static_cast<IRExprList*>(GetArgs()->GetRecCopyExpr()), mBIRId, diGetDIE()->Clone());
  } // if
  return retVal;
} // IRSBuiltInCall::GetRecCopy

INLINE__
IRBB* IRBB::
GetNext() const {
  BMDEBUG1("IRBB::GetNext");
  ENSUREMSG(mNext != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(mNext);
  return mNext;
} // IRBB::GetPrev

INLINE__
IRBB* IRBB::
GetPrev() const {
  BMDEBUG1("IRBB::GetPrev");
  ENSUREMSG(mPrev != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(mPrev);
  return mPrev;
} // IRBB::GetPrev

INLINE__
IRStmt* IRBB::
GetLastStmt() const {
  BMDEBUG1("IRBB::GetLastStmt");
  REQUIREDMSG(!mStmts.empty(), "There must be at least one statement, use extract or check for emptiness ");
  EMDEBUG1(mStmts.back());
  return mStmts.back();
} // IRBB::GetFirstStmt

INLINE__
bool IRExprList::
GetChildren(vector<IRExpr*>& children_) const {
  REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
  children_ = mExprList;
  return !children_.empty();
} // IRExprList::GetChildren

INLINE__
IRExprList::
IRExprList() :
  IRExpr(IRExprList::exprListVoidType)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRExprList::IRExprList

INLINE__
IRExprList::
IRExprList(IRExpr* expr_) :
  IRExpr(IRExprList::exprListVoidType)
{
  AddExpr(expr_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRExprList::IRExprList

INLINE__
IRExprList::
IRExprList(IRExpr* expr0_, IRExpr* expr1_) :
  IRExpr(IRExprList::exprListVoidType)
{
  AddExpr(expr0_);
  AddExpr(expr1_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRExprList::IRExprList

INLINE__
IRExprList::
IRExprList(IRExpr* expr0_, IRExpr* expr1_, IRExpr* expr2_) :
  IRExpr(IRExprList::exprListVoidType)
{
  AddExpr(expr0_);
  AddExpr(expr1_);
  AddExpr(expr2_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRExprList::IRExprList

INLINE__
//! \brief Returns the statement of the insert context.
IRStmt* ICStmt::
GetStmt() const {
  BMDEBUG1("ICStmt::GetStmt");
  REQUIRED(mStmt != 0);
  EMDEBUG1(mStmt);
  return mStmt;
} // ICStmt::GetStmt

INLINE__
IRBB* IRSBeforeFirstOfBB::
GetBB() const {
  BMDEBUG1("IRSBeforeFirstOfBB::GetBB");
  REQUIRED(this != &Singleton<IRSBeforeFirstOfBB>::Obj());
  IRBB* retVal(IRStmt::GetBB());
  EMDEBUG1(retVal);
  return retVal;
} // IRSBeforeFirstOfBB::GetBB

INLINE__
CFG* IRBB::
GetCFG() const {
  BMDEBUG1("IRBB::GetCFG");
  REQUIRED(mParentCFG != 0);
  ENSURE(mParentCFG != 0);
  EMDEBUG0();
  return mParentCFG;
} // IRBB::GetCFG

INLINE__
void IRBB::
SetCFG(CFG* cfg_) {
  BMDEBUG1("IRBB::SetCFG");
  REQUIRED_VALID(cfg_);
  mParentCFG = cfg_;
  EMDEBUG0();
  return;
} // IRBB::SetCFG

INLINE__
void CFG::
setFunction(IRFunction* func_) {
  BMDEBUG1("CFG::setFunction");
  REQUIRED_VALID(func_);
  mFunc = func_;
  EMDEBUG0();
  return;
} // CFG::setFunction

INLINE__
IRFunction* CFG::
GetFunc() const {
  BMDEBUG1("CFG::GetFunc");
  REQUIRED_VALID(mFunc);
  EMDEBUG0();
  return mFunc;
} // CFG::GetFunc

//! \brief Constructs an insert context from a statement and a place.
//! \post Valid this.
INLINE__
ICStmt::
ICStmt(IRStmt* stmt_, ICPlace place_) :
  InsertContext(stmt_->GetBB(), place_),
  mStmt(stmt_)
{
  BMDEBUG3("ICStmt::ICStmt", mStmt, place_);
  REQUIRED(pred.pIsBeforeFirstOfBB(mStmt) == false);
  REQUIRED(pred.pIsAfterLastOfBB(mStmt) == false);
  ENSURE_VALID(this);
  EMDEBUG0();
} // ICStmt::ICStmt

INLINE__
IREIntConst::
IREIntConst(IRTInt* type_, const tBigInt& tbi_) :
  IREConst(type_),
  mValue(tbi_)
{
  BMDEBUG3("IREIntConst::IREIntConst", type_, &tbi_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IREIntConst::IREIntConst

INLINE__
IREStrConst::
IREStrConst(string value_) :
  IREConst(new IRTArray(new IRTInt(8, 0, IRSUnsigned), value_.size(), INBITS(8)*8)),
  mValue(value_)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREStrConst::IREStrConst

INLINE__
IREStrConst::
IREStrConst(const IREStrConst& strConst_) :
  IREConst(new IRTArray(new IRTInt(8, 0, IRSUnsigned), strConst_.mValue.size(), INBITS(8)*8)),
  mValue(strConst_.mValue)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREStrConst::IREStrConst

INLINE__
IREAddrConst::
IREAddrConst(IRType* type_, const tBigInt& constValue_) :
  IREConst(type_),
  mOffset(0),
  mConstValue(constValue_),
  //! \todo M Design: convert initialization of object to null in IREAddrConst.
  mObject(&Invalid<IRObject>::Obj()),
  mLabel(&Invalid<IRSLabel>::Obj())
{
  REQUIREDMSG(pred.pIsPtr(type_) == true, ieStrParamValuesDBCViolation);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrConst::IREAddrConst

INLINE__
IREAddrConst::
IREAddrConst(IRType* type_, IRSLabel* label_, const tBigAddr& offset_) :
  IREConst(type_),
  mOffset(offset_),
  mConstValue(0),
  mObject(&Invalid<IRObject>::Obj()),
  mLabel(label_)
{
  REQUIREDMSG(pred.pIsPtr(type_) == true, ieStrParamValuesDBCViolation);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrConst::IREAddrConst

INLINE__
IREAddrConst::
IREAddrConst(IRType* type_, IRObject* object_, const tBigAddr& offset_) :
  IREConst(type_),
  mOffset(offset_),
  mConstValue(0),
  mObject(object_),
  mLabel(&Invalid<IRSLabel>::Obj())
{
  REQUIREDMSG(pred.pIsPtr(type_) == true, ieStrParamValuesDBCViolation);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrConst::IREAddrConst

INLINE__
IREAddrConst::
IREAddrConst(const IREAddrConst& addrConst_) :
  IREConst(addrConst_.GetType()),
  mOffset(addrConst_.mOffset),
  mConstValue(addrConst_.mConstValue),
  mObject(addrConst_.mObject),
  mLabel(addrConst_.mLabel)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrConst::IREAddrConst

INLINE__
IRERealConst::
IRERealConst(IRTReal* type_, const BigReal& br_) :
  IREConst(type_),
  mValue(br_)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRERealConst::IRERealConst

INLINE__
IRERealConst::
IRERealConst(const IRERealConst& realConst_) :
  IREConst(realConst_.GetType()),
  mValue(realConst_.mValue)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRERealConst::IRERealConst

INLINE__
IREBoolConst::
IREBoolConst(bool value_) :
  IREConst(boolConstBoolType),
  mValue(value_)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREBoolConst::IREBoolConst

INLINE__
IREBoolConst::
IREBoolConst(const IREBoolConst& boolConst_) :
  IREConst(boolConstBoolType),
  mValue(boolConst_.mValue)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREBoolConst::IREBoolConst

INLINE__
IREIntConst::
IREIntConst(const IREIntConst& intConst_) :
  IREConst(intConst_.GetType()),
  mValue(intConst_.mValue)
{
  BMDEBUG2("IREIntConst::IREIntConst", &intConst_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IREIntConst::IREIntConst

INLINE__
void CFG::
RemoveBB(IRBB* bb_) {
  BMDEBUG2("CFG::RemoveBB", bb_);
  REQUIRED(bb_ != 0);
  REQUIRED(bb_->GetCFG() == this);
  REQUIRED(pred.pHasPred(bb_) == false);
  REQUIRED(pred.pIsEmpty(bb_) == true);
  REQUIRED(pred.pIsEntryBB(bb_) == false && pred.pIsExitBB(bb_) == false);
  REQUIRED(bb_->mPrev != 0 && bb_->mNext != 0);
  bb_->mParentCFG = 0;
  bb_->mPrev->mNext = bb_->mNext;
  bb_->mNext->mPrev = bb_->mPrev;
  bb_->mNext = 0;
  bb_->mPrev = 0;
  mBBs.erase(find(mBBs.begin(), mBBs.end(), bb_));
  EMDEBUG0();
  return;
} // CFG::RemoveBB

//! \pre The basic block must be non-empty.
INLINE__
void IRBB::
RemoveAllStmts() {
  BMDEBUG1("IRBB::RemoveAllStmts");
  REQUIRED(pred.pIsEmpty(this) == false);
  if (pred.pIsCFStmt(GetLastStmt())) {
    RemoveStmt(GetLastStmt());
  } // if
  mStmts.clear();
  EMDEBUG0();
} // IRBB::RemoveAllStmts

INLINE__
bool IRStmt::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  REQUIRED(0); // program counter should never come here.
  return false;
} // IRStmt::remapJumpTargets

INLINE__
bool IRSAssign::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSAssign::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSAssign::remapJumpTargets

INLINE__
bool IRSEval::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSEval::remapJumpTargets", from_, to_);
  bool retVal(remapJumpTargets(from_, to_));
  EMDEBUG1(retVal);
  return retVal;
} // IRSEval::remapJumpTargets

INLINE__
bool IRSPCall::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSPCall::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSPCall::remapJumpTargets

INLINE__
bool IRSFCall::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSFCall::remapJumpTargets", from_, to_);
  REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  EMDEBUG0();
  return false;
} // IRSFCall::remapJumpTargets

INLINE__
bool IRSLabel::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSLabel::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSLabel::remapJumpTargets

INLINE__
bool IRSDynJump::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  return false;
} // IRSDynJump::remapJumpTargets

INLINE__
bool IRSBuiltInCall::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSBuiltInCall::remapJumpTargets", from_, to_);
  REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  EMDEBUG0();
  return false;
} // IRSBuiltInCall::remapJumpTargets

INLINE__
IRTree* IRObject::
GetRecCopy() const {
  return const_cast<IRExpr*>(static_cast<const IRExpr*>(this));
} // IRObject::GetRecCopy

INLINE__
bool IRObject::
GetChildren(vector<IRExpr*>& children) const {
  REQUIREDMSG(children.empty() == true, ieStrParamValuesDBCViolation);
  return false;
} // IRObject::GetChildren

INLINE__
bool IRFunction::
GetAddressTakenLabels(vector<IRSLabel*>& labels_) {
  labels_ = mAddrTakenLabels;
  return !labels_.empty();
} // IRFunction::GetAddressTakenLabels

//! \pre \p label must have address taken set.
INLINE__
void IRFunction::
AddAddressTakenLabel(IRSLabel* label_) {
  REQUIREDMSG(label_->GetAddressTaken() == true, ieStrInconsistentInternalStructure);
  mAddrTakenLabels.push_back(label_);
} // IRFunction::AddAddressTakenLabel

INLINE__
void IRFunction::
AddDynJump(IRSDynJump* jump_) {
  REQUIREDMSG(jump_ != 0, ieStrNonNullParam);
  mDynJumps.push_back(jump_);
} // IRFunction::AddDynJump

INLINE__
void IRFunction::
RemoveAddrTakenLabel(IRSLabel* label_) {
  REQUIREDMSG(find(mAddrTakenLabels.begin(), mAddrTakenLabels.end(), label_) != mAddrTakenLabels.end(),
    ieStrInconsistentInternalStructure);
  mAddrTakenLabels.erase(find(mAddrTakenLabels.begin(), mAddrTakenLabels.end(), label_));
} // IRFunction::RemoveAddrTakenLabel

INLINE__
void IRFunction::
RemoveDynJump(IRSDynJump* jump_) {
  REQUIREDMSG(find(mDynJumps.begin(), mDynJumps.end(), jump_) != mDynJumps.end(),
    ieStrInconsistentInternalStructure);
  mDynJumps.erase(find(mDynJumps.begin(), mDynJumps.end(), jump_));
} // IRFunction::RemoveDynJump

INLINE__
bool IRFunction::
GetDynJumps(vector<IRSDynJump*>& jumps_) {
  jumps_ = mDynJumps;
  return !mDynJumps.empty();
} // IRFunction::GetDynJumps

INLINE__
IRStmt* IRStmt::
GetNextStmt() const {
  BMDEBUG1("IRStmt::GetNextStmt");
  ENSUREMSG(mNext != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(mNext);
  return mNext;
} // IRStmt::GetNextStmt

INLINE__
void IRStmt::
SetParent(IRBB* parent_) {
  BMDEBUG2("IRStmt::SetParent", parent_);
  REQUIREDMSG(parent_ != 0, ieStrNonNullParam);
  mParentBB = parent_;
  EMDEBUG0();
  return;
} // IRStmt::SetParent

INLINE__
void IRStmt::
SetPrevStmt(IRStmt* prev_) {
  BMDEBUG2("IRStmt::SetPrevStmt", prev_);
  REQUIREDMSG(prev_ != 0, ieStrNonNullParam);
  mPrev = prev_;
  EMDEBUG0();
  return;
} // IRStmt::SetPrevStmt

INLINE__
void IRStmt::
SetNextStmt(IRStmt* next_) {
  BMDEBUG2("IRStmt::SetNextStmt", next_);
  REQUIREDMSG(next_ != 0, ieStrNonNullParam);
  mNext = next_;
  EMDEBUG0();
  return;
} // IRStmt::SetNextStmt

//! \brief Invalidates the object.
INLINE__
void IRStmt::
Invalidate() {
  BMDEBUG1("IRStmt::Invalidate");
  mPrev = 0;
  mNext = 0;
  mParentBB = 0;
  EMDEBUG0();
  return;
} // IRStmt::Invalidate

INLINE__
IRBB* IRStmt::
GetBB() const {
  BMDEBUG1("IRStmt::GetBB");
  ENSUREMSG(mParentBB != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(mParentBB);
  return mParentBB;
} // IRStmt::GetBB

INLINE__
IREUnary::
IREUnary(IRType* type_, IRExpr* operand_) :
  IRExpr(type_),
  mOperand(operand_)
{
  REQUIREDMSG(operand_ != 0, ieStrNonNullParam);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(mOperand) == true, ieStrExprUsedBefore);
  mOperand->SetParent(this);
  ENSURE_VALID(this);
} // IREUnary::IREUnary

INLINE__
IREBinary::
IREBinary(IRType* type_, IRExpr* leftExpr_, IRExpr* rightExpr_) :
  IRExpr(type_),
  mLeftExpr(leftExpr_),
  mRightExpr(rightExpr_)
{
  BMDEBUG4("IREBinary::IREBinary", type_, leftExpr_, rightExpr_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(mLeftExpr, mRightExpr) == true, ieStrExprUsedBefore);
  mLeftExpr->SetParent(this);
  mRightExpr->SetParent(this);
  ENSURE_VALID(this);
  EMDEBUG0();
} // IREBinary::IREBinary

INLINE__
IRExprList& IRExprList::
AddExpr(IRExpr* expr_) {
  BMDEBUG2("IRExprList::AddExpr", expr_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(expr_) == true, ieStrExprUsedBefore);
  REQUIRED_VALID(expr_);
  mExprList.push_back(expr_);
  expr_->SetParent(this);
  EMDEBUG0();
  return *this;
} // IRExprList::AddExpr

INLINE__
IRSCall::
IRSCall(IRObject* returnIn_, IRExprList* args_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mReturnIn(returnIn_),
  mFuncExpr(0),
  mArgs(args_)
{
  BMDEBUG3("IRSCall::IRSCall", mReturnIn, mArgs);
  REQUIREDMSG(mReturnIn != 0, ieStrNonNullParam);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  mArgs->SetParent(this);
  EMDEBUG0();
} // IRSCall::IRSCall

INLINE__
IRSCall::
IRSCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mReturnIn(returnIn_),
  mFuncExpr(funcExpr_),
  mArgs(args_)
{
  BMDEBUG4("IRSCall::IRSCall", mReturnIn, mFuncExpr, mArgs);
  REQUIREDMSG(mReturnIn != 0, ieStrNonNullParam);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  mArgs->SetParent(this);
  mFuncExpr->SetParent(this);
  EMDEBUG0();
} // IRSCall::IRSCall

INLINE__
IRSCall::
IRSCall(IRExprList* args_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mReturnIn(0),
  mFuncExpr(0),
  mArgs(args_)
{
  BMDEBUG2("IRSCall::IRSCall", mArgs);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  mArgs->SetParent(this);
  EMDEBUG0();
} // IRSCall::IRSCall

INLINE__
IRSCall::
IRSCall(IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mReturnIn(0),
  mFuncExpr(funcExpr_),
  mArgs(args_)
{
  BMDEBUG3("IRSCall::IRSCall", mFuncExpr, mArgs);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  mFuncExpr->SetParent(this);
  mArgs->SetParent(this);
  EMDEBUG0();
} // IRSCall::IRSCall

INLINE__
void IRModule::
AddGlobal(IROGlobal* global_) {
  BMDEBUG2("IRModule::AddGlobal", global_);
  REQUIRED_VALIDMSG(global_, ieStrObjectNotValid);
  mGlobals.push_back(global_);
  mMapNameGlobals[global_->GetName()] = global_;
  EMDEBUG0();
  return;
} // IRModule::AddGlobal

INLINE__
void IRModule::
AddFunction(IRFunction* func_) {
  BMDEBUG2("IRModule::AddFuction", func_);
  REQUIRED_VALIDMSG(func_, ieStrObjectNotValid);
  mFuncs.push_back(func_);
  mName2Funcs[func_->GetName()] = func_;
  EMDEBUG0();
  return;
} // IRModule::AddFunction

INLINE__
IREAddrOf::
IREAddrOf(IRType* addrTypeOfObj, IRObject* obj) :
  IRExpr(addrTypeOfObj),
  mLabel(0),
  mExpr(obj)
{
  mExpr->SetParent(this);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrOf::IREAddrOf

INLINE__
IREAddrOf::
IREAddrOf(IRType* addrTypeOfObj_, IRSLabel* label_) :
  IRExpr(addrTypeOfObj_),
  mLabel(label_),
  mExpr(0)
{
  REQUIREDMSG(mLabel != 0, ieStrNonNullParam);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrOf::IREAddrOf

INLINE__
IREAddrOf::
IREAddrOf(IRType* addrTypeOfObj_, IRExpr* expr_) :
  IRExpr(addrTypeOfObj_),
  mLabel(0),
  mExpr(expr_)
{
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(expr_) == true, ieStrExprUsedBefore);
  mExpr->SetParent(this);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrOf::IREAddrOf

INLINE__
IRStmt* IRExpr::
GetParentStmt() const {
  BMDEBUG1("IRExpr::GetParentStmt");
  REQUIREDMSG(dynamic_cast<const IRStmt*>(mParent) != 0, ieStrParamValuesDBCViolation);
  IRStmt* retVal(static_cast<IRStmt*>(mParent));
  EMDEBUG1(retVal);
  return retVal;
} // IRExpr::GetParentStmt

INLINE__
IRExpr* IRExpr::
GetParentExpr() const {
  BMDEBUG1("IRExpr::GetParentExpr");
  REQUIREDMSG(dynamic_cast<const IRExpr*>(mParent) != 0, ieStrParamValuesDBCViolation);
  IRExpr* retVal(static_cast<IRExpr*>(mParent));
  EMDEBUG1(retVal);
  return retVal;
} // IRExpr::GetParentExpr

INLINE__
IREAddrOf::
IREAddrOf(const IREAddrOf& addrOf_) :
  IRExpr(addrOf_),
  mLabel(addrOf_.mLabel),
  mExpr(addrOf_.mExpr)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREAddrOf::IREAddrOf

INLINE__
IRTree* IREAddrOf::
GetRecCopy() const {
  IRExpr* retVal = 0;
  if (mLabel != 0) {
    retVal = new IREAddrOf(GetType(), mLabel);
  } else {
    retVal = new IREAddrOf(GetType(), mExpr->GetRecCopyExpr());
  } //if
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // IREAddrOf::GetRecCopy

INLINE__
bool IRSIf::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSIf::remapJumpTargets", from_, to_);
  REQUIRED(from_ != to_);
  tristate retVal;
  if (mTrueCaseBB == from_) {
    retVal = true;
    mTrueCaseBB = to_;
  } else if (mFalseCaseBB == from_) {
    retVal = true;
    mFalseCaseBB = to_;
  } else {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRSIf::remapJumpTargets

INLINE__
bool IRSNull::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSNull::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSNull::remapJumpTargets

INLINE__
IRExpr* IREBinary::
GetLeftExpr() const {
  BMDEBUG1("IREBinary::GetLeftExpr");
  EMDEBUG1(mLeftExpr);
  return mLeftExpr;
} // IREBinary::GetLeftExpr

INLINE__
IRExpr* IREBinary::
GetRightExpr() const {
  BMDEBUG1("IREBinary::GetRightExpr");
  EMDEBUG1(mRightExpr);
  return mRightExpr;
} // IREBinary::GetRightExpr

INLINE__
bool IREUnary::
GetChildren(vector<IRExpr*>& children_) const {
  REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
  children_.push_back(mOperand);
  return true;
} // IREUnary::GetChildren

INLINE__
IREUnary::
IREUnary(const IREUnary& unary_) :
  IRExpr(unary_.GetType()),
  mOperand(unary_.mOperand)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREUnary::IREUnary

INLINE__
IREBinary::
IREBinary(const IREBinary& binary_) :
  IRExpr(binary_.GetType()),
  mLeftExpr(binary_.mLeftExpr),
  mRightExpr(binary_.mRightExpr)
{
  BMDEBUG1("IREBinary::IREBinary");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IREBinary::IREBinary

INLINE__
bool IREBinary::
GetChildren(vector<IRExpr*>& children_) const {
  REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
  children_.push_back(mLeftExpr);
  children_.push_back(mRightExpr);
  return true;
} // IREBinary::GetChildren

INLINE__
IRBB* IRSAfterLastOfBB::
GetBB() const {
  BMDEBUG1("IRSAfterLastOfBB::GetBB");
  REQUIRED(this != &Singleton<IRSAfterLastOfBB>::Obj());
  IRBB* retVal = IRStmt::GetBB();
  EMDEBUG1(retVal);
  return retVal;
} // IRSAfterLastOfBB::GetBB

INLINE__
bool IRSAfterLastOfBB::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSAfterLastOfBB::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSAfterLastOfBB::remapJumpTargets

INLINE__
bool IRSBeforeFirstOfBB::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSBeforeFirstOfBB::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSBeforeFirstOfBB::remapJumpTargets

INLINE__
IRBB* IRSJump::
GetTargetBB() const {
  BMDEBUG1("IRSJump::GetTargetBB");
  REQUIRED(mTargetBB != 0);
  EMDEBUG1(mTargetBB);
  return mTargetBB;
} // IRSJump::GetTargetBB

INLINE__
bool IRSJump::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSJump::remapJumpTargets", from_, to_);
  tristate retVal;
  if (mTargetBB == from_) {
    retVal = true;
    mTargetBB = to_;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRSJump::remapJumpTargets

INLINE__
bool IRSReturn::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSReturn::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSReturn::remapJumpTargets

INLINE__
bool IRSProlog::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSProlog::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSProlog::remapJumpTargets

INLINE__
bool IRSEpilog::
remapJumpTargets(IRBB* from_, IRBB* to_) {
  BMDEBUG3("IRSEpilog::remapJumpTargets", from_, to_);
  EMDEBUG0();
  return remapJumpTargets(from_, to_);
} // IRSEpilog::remapJumpTargets

INLINE__
ICStmt::
ICStmt(const Invalid<ICStmt>* inv_) :
  InsertContext(),
  mStmt(&Invalid<IRStmt>::Obj())
{
} // ICStmt::ICStmt

INLINE__
IREGe::
IREGe(IRExpr* left_, IRExpr* right_) :
  IRECmp(left_, right_)
{
  BMDEBUG1("IREGe::IREGe");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IREGe::IREGe

INLINE__
IREEq::
IREEq(IRExpr* left_, IRExpr* right_) :
  IRECmp(left_, right_)
{
  BMDEBUG1("IREEq::IREEq");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IREEq::IREEq

INLINE__
IREEq::
IREEq(const IREEq& eq_) :
  IRECmp(eq_)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREEq::IREEq

INLINE__
IRELe::
IRELe(IRExpr* left_, IRExpr* right_) :
  IRECmp(left_, right_)
{
  BMDEBUG1("IRELe::IRELe");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRELe::IRELe

INLINE__
IRELe::
IRELe(const IRELe& le_) :
  IRECmp(le_)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRELe::IREGe

INLINE__
IRELt::
IRELt(IRExpr* left_, IRExpr* right_) :
  IRECmp(left_, right_)
{
  BMDEBUG1("IRELt::IRELt");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRELt::IRELt

INLINE__
IRELt::
IRELt(const IRELt& lt_) :
  IRECmp(lt_)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IRELt::IRELt

INLINE__
IRENe::
IRENe(IRExpr* left_, IRExpr* right_) :
  IRECmp(left_, right_)
{
  BMDEBUG1("IRENe::IRENe");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRENe::IRENe

INLINE__
IRENe::
IRENe(const IRENe& ne_) :
  IRECmp(ne_)
{
  BMDEBUG1("IRENe::IRENe");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRENe::IRENe

INLINE__
IREGe::
IREGe(const IREGe& ge_) :
  IRECmp(ge_)
{
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
} // IREGe::IREGe

INLINE__
IRStmt::
IRStmt(IRBB* parentBB_, IRStmt* prev_, IRStmt* next_, DIEBase* die_) :
  mDIE(die_),
  mPrev(prev_),
  mNext(next_),
  mParentBB(parentBB_)
{
} // IRStmt::IRStmt

INLINE__
IRExpr* IRSCall::
GetFuncExpr() const {
  ENSUREMSG(mFuncExpr != 0, ieStrInconsistentInternalStructure);
  return mFuncExpr;
} // IRSCall::GetFuncExpr

INLINE__
IRExprList* IRSCall::
GetArgs() const {
  ENSUREMSG(mArgs != 0, ieStrInconsistentInternalStructure);
  return mArgs;
} // IRSCall::GetFuncExpr

INLINE__
IRObject* IRSCall::
GetReturnIn() const {
  ENSUREMSG(mReturnIn != 0, ieStrInconsistentInternalStructure);
  return mReturnIn;
} // IRSCall::GetReturnIn

INLINE__
IRSPCall::
IRSPCall(IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_) :
  IRSCall(funcExpr_, args_, die_)
{
  BMDEBUG3("IRSPCall::IRSPCall", funcExpr_, args_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(funcExpr_, args_) == true, ieStrExprUsedBefore);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  funcExpr_->SetParent(this);
  args_->SetParent(this);
  EMDEBUG0();
} // IRSPCall::IRSPCall

INLINE__
IRSFCall::
IRSFCall(IRObject* returnIn_, IRExprList* args_, DIEBase* die_) :
  IRSCall(returnIn_, args_, die_)
{
  BMDEBUG3("IRSFCall::IRSFCall", returnIn_, args_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(args_) == true, ieStrExprUsedBefore);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  args_->SetParent(this);
  EMDEBUG0();
} // IRSFCall::IRSFCall

INLINE__
IRSBuiltInCall::
IRSBuiltInCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, IRBIRId birId_, DIEBase* die_) :
  IRSCall(returnIn_, funcExpr_, args_, die_),
  mBIRId(birId_),
  mIsFunction(true)
{
  BMDEBUG4("IRSBuiltInCall::IRSBuiltInCall", returnIn_, funcExpr_, args_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(funcExpr_, args_) == true, ieStrExprUsedBefore);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  args_->SetParent(this);
  EMDEBUG0();
} // IRSBuiltInCall::IRSBuiltInCall

INLINE__
IRSEval::
IRSEval(IRExpr* expr_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mExpr(expr_)
{
  BMDEBUG2("IRSReturn::IRSReturn", expr_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(expr_) == true, ieStrExprUsedBefore);
  mExpr->SetParent(this);
  EMDEBUG0();
} // IRSEval::IRSEval

INLINE__
IRSReturn::
IRSReturn(IRExpr* retExpr_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mRetExpr(retExpr_)
{
  BMDEBUG2("IRSReturn::IRSReturn", retExpr_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(retExpr_) == true, ieStrExprUsedBefore);
  mRetExpr->SetParent(this);
  EMDEBUG0();
} // IRSReturn::IRSReturn

INLINE__
IRSIf::
IRSIf(IRExpr* condExpr_, IRBB* trueCaseBB_, IRBB* falseCaseBB_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mCondExpr(condExpr_),
  mTrueCaseBB(trueCaseBB_),
  mFalseCaseBB(falseCaseBB_)
{
  BMDEBUG4("IRSIf::IRSIf", condExpr_, trueCaseBB_, falseCaseBB_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(condExpr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.pIsBool(condExpr_->GetType()) == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(pred.pIsAny(mTrueCaseBB) == false || pred.pIsAny(mFalseCaseBB) == true,
    ieStrParamValuesDBCViolation);
  REQUIREDMSG(pred.pIsAny(mFalseCaseBB) == false || pred.pIsAny(mTrueCaseBB) == true,
    ieStrParamValuesDBCViolation);
  REQUIREDMSG(pred.pIsAny(mTrueCaseBB) == true || (mTrueCaseBB != mFalseCaseBB),
    ieStrParamValuesDBCViolation);
  mCondExpr->SetParent(this);
  EMDEBUG0();
} // IRSIf::IRSIf

INLINE__
IRBBAny::
IRBBAny() :
  IRBB(&Invalid<CFG>::Obj())
{
} // IRBBAny::IRBBAny

INLINE__
IRSLabel::
IRSLabel(const string& name_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mName(name_),
  mAddressTaken(false)
{
  BMDEBUG1("IRSLabel::IRSLabel");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRSLabel::IRSLabel

INLINE__
IRSBuiltInCall::
IRSBuiltInCall(IRExpr* funcExpr_, IRExprList* args_, IRBIRId birId_, DIEBase* die_) :
  IRSCall(funcExpr_, args_, die_),
  mBIRId(birId_),
  mIsFunction(false)
{
  BMDEBUG2("IRSBuiltInCall::IRSBuiltInCall", args_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(funcExpr_, args_) == true, ieStrExprUsedBefore);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  args_->SetParent(this);
  EMDEBUG0();
} // IRSBuiltInCall::IRSBuiltInCall

INLINE__
IRSFCall::
IRSFCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_) :
  IRSCall(returnIn_, funcExpr_, args_, die_)
{
  BMDEBUG4("IRSFCall::IRSFCall", returnIn_, funcExpr_, args_);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(funcExpr_, args_) == true, ieStrExprUsedBefore);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  args_->SetParent(this);
  funcExpr_->SetParent(this);
  EMDEBUG0();
} // IRSFCall::IRSFCall

INLINE__
bool IRSCall::
GetChildren(vector<IRExpr*>& exprs_) const {
  REQUIREDMSG(exprs_.empty() == true, ieStrParamValuesDBCViolation);
  // Call mArgs first so that exprs_ is still empty (dbc req).
  mArgs->GetChildren(exprs_);
  exprs_.insert(exprs_.begin(), mFuncExpr);
  return true;
} // IRSCall::GetChildren

INLINE__
IRTFunc::
IRTFunc(IRType* retType_, const vector<IRType*>& paramTypes_, bool isVarArg_, CallingConvention* callConv_) :
  IRType(0, 0),
  mCallConv(callConv_),
  mRetType(retType_),
  mParamTypes(paramTypes_),
  mIsVarArg(isVarArg_)
{
  BMDEBUG4("IRTFunc::IRTFunc", retType_, isVarArg_, callConv_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(retType_, callConv_) == true,
    ieStrParamValuesDBCViolation);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTFunc::IRTFunc

INLINE__
InsertContext::
InsertContext() :
  mPlace(ICInvalid),
  mBB(&Invalid<IRBB>::Obj())
{
  //! \attention do not use debug trace.
  REQUIRED(mPlace == ICInvalid);
} // InsertContext::InsertContext

INLINE__
InsertContext::
InsertContext(IRBB* bb_, ICPlace place_) :
  mPlace(place_),
  mBB(bb_)
{
  //! \attention do not use debug trace. ??? Why not?
} // InsertContext::InsertContext

INLINE__
IRTReal::
IRTReal(tBigInt size_, hUInt16 alignment_, hUInt16 mantissa_, hUInt16 exponent_) :
  IRType(size_, alignment_),
  mMantissa(mantissa_),
  mExponent(exponent_)
{
  BMDEBUG5("IRTReal::IRTReal", &size_, alignment_, mantissa_, exponent_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTReal::IRTReal

INLINE__
IRTArray::
IRTArray(IRType* elemType_, tBigInt count_, hUInt16 alignment_) :
  IRType(elemType_->GetSize() * count_, alignment_),
  mLowerBound(-1),
  mUpperBound(-1),
  mCount(count_),
  mIsIncomplete(count_ == 0),
  mElemType(elemType_)
{
  BMDEBUG4("IRTArray::IRTArray", elemType_, &count_, alignment_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTArray::IRTArray

INLINE__
IRTArray::
IRTArray(IRType* elemType_, hBigInt lowerBound_, hBigInt upperBound_, tBigInt count_, hUInt16 alignment_) :
  IRType(elemType_->GetSize() * count_, alignment_),
  mLowerBound(lowerBound_),
  mUpperBound(upperBound_),
  mCount(count_),
  mIsIncomplete(count_ == 0),
  mElemType(elemType_)
{
  BMDEBUG6("IRTArray::IRTArray", elemType_, &lowerBound_, &upperBound_, &count_, alignment_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTArray::IRTArray

INLINE__
IRExpr* IRTree::
GetRecCopyExpr() const {
  IRTree* retVal(GetRecCopy());
  ENSUREMSG(dynamic_cast<IRExpr*>(retVal) != 0, ieStrMustEnsureValid);
  return static_cast<IRExpr*>(retVal);
} // IRTree::GetRecCopyExpr

INLINE__
IRStmt* IRTree::
GetRecCopyStmt() const {
  IRTree* retVal(GetRecCopy());
  ENSUREMSG(dynamic_cast<IRStmt*>(retVal) != 0, ieStrMustEnsureValid);
  return static_cast<IRStmt*>(retVal);
} // IRTree::GetRecCopyStmt

//! \todo M Design: acquire the size and alignment for the current target.
INLINE__
IRTPtr::
IRTPtr(IRType* refType_) :
  IRType(32, 32),
  mRefType(refType_)
{
  BMDEBUG1("IRTPtr::IRTPtr");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTPtr::IRTPtr

INLINE__
IRTPtr::
IRTPtr(IRType* refType_, tBigInt size_, hUInt16 alignment_) :
  IRType(size_, alignment_),
  mRefType(refType_)
{
  BMDEBUG1("IRTPtr::IRTPtr");
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTPtr::IRTPtr

INLINE__
tBigInt IRTFunc::
GetSize() const {
  NOTIMPLEMENTED(M);
  return 0;
} // IRType::GetSize

INLINE__
IRStmt* IRStmt::
GetPrevStmt() const {
  BMDEBUG1("IRStmt::GetPrevStmt");
  ENSUREMSG(mPrev != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(mPrev);
  return mPrev;
} // IRStmt::GetPrevStmt

INLINE__
IRSDynJump::
IRSDynJump(IRExpr* targetExpr_, DIEBase* die_) :
  IRStmt(0, 0, 0, die_),
  mTargetExpr(targetExpr_)
{
  REQUIREDMSG(targetExpr_ != 0, ieStrNonNullParam);
  REQUIREDMSG(pred.GetDBCHelper()->pCheckNeverUsedAndAddUsedExpr(targetExpr_) == true, ieStrExprUsedBefore);
  mTargetExpr->SetParent(this);
} // IRSDynJump::IRSDynJump

INLINE__
IRExpr::
IRExpr(IRType* type_) :
  mParent(0),
  mType(type_),
  mAssocGroupNo(0)
{
  BMDEBUG2("IRExpr::IRExpr", mType);
  REQUIREDMSG(mType != 0, ieStrNonNullParam);
  EMDEBUG0();
} // IRExpr::IRExpr

INLINE__
IRTStruct::
IRTStruct(const string& name_, const vector<IROField*>& fields_, tBigInt size_, hUInt16 alignment_) :
  IRType(size_, alignment_),
  mFields(fields_),
  mName(name_)
{
  BMDEBUG4("IRTStruct::IRTStruct", name_, &size_, alignment_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTStruct::IRTStruct

INLINE__
IRTUnion::
IRTUnion(const string& name_, const vector<IROField*>& fields_, tBigInt size_, hUInt16 alignment_) :
  IRTStruct(name_, fields_, size_, alignment_)
{
  BMDEBUG4("IRTUnion::IRTUnion", name_, &size_, alignment_);
  ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  EMDEBUG0();
} // IRTUnion::IRTUnion

INLINE__
bool IRSEval::
GetChildren(vector<IRExpr*>& exprs_) const {
  exprs_.push_back(mExpr);
  return true;
} // IRSEval::GetChildren

INLINE__
void CFG::
ResetVisits() {
  for (list<IRBB*>::iterator it(mBBs.begin()); it != mBBs.end(); ++ it) {
    (*it)->mVisited = false;
  } // for
} // CFG::ResetVisits

//! \note Although we pass the debug/trace related exe point parameter,
//!       a reasonable host compiler is expected to inline expand GenericExprAccept
//!       function. After inlining the parameter should be detected as not
//!       used and eliminated. In short there is no run-time overhead of dbgExePoint_
//!       parameter.
template<typename tcBase, typename tcVisitor, typename tcBeingVisited>
bool GenericExprAccept(const char* dbgExePoint_, tcVisitor& visitor_, tcBeingVisited* visited_) {
  BDEBUG3(dbgExePoint_, &visitor_, visited_);
  bool retVal(true);
  if (visitor_.GetOrderType() == EVOTPre) {
    retVal = visitor_.Visit(visited_);
  } // if
  if (retVal == true) {
    retVal = visited_->tcBase::Accept(visitor_);
  } // if
  if (visitor_.GetOrderType() == EVOTPost) {
    retVal &= visitor_.Visit(visited_);
  } // if
  EDEBUG1(retVal);
  return retVal;
} // GenericExprAccept

INLINE__ bool IRESubscript::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRESubscript::Accept", visitor_, this);}
INLINE__ bool IRESubscript::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRESubscript::Accept", visitor_, this);}
INLINE__ bool IREMember::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREMember::Accept", visitor_, this);}
INLINE__ bool IREMember::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREMember::Accept", visitor_, this);}
INLINE__ bool IRELShiftRight::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRELShiftRight::Accept", visitor_, this);}
INLINE__ bool IRELShiftRight::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRELShiftRight::Accept", visitor_, this);}
INLINE__ bool IREShiftLeft::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREShiftLeft::Accept", visitor_, this);}
INLINE__ bool IREShiftLeft::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREShiftLeft::Accept", visitor_, this);}
INLINE__ bool IREAShiftRight::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREAShiftRight::Accept", visitor_, this);}
INLINE__ bool IREAShiftRight::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREAShiftRight::Accept", visitor_, this);}
INLINE__ bool IRECmp::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRECmp::Accept", visitor_, this);}
INLINE__ bool IRECmp::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRECmp::Accept", visitor_, this);}
INLINE__ bool IRENe::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRENe::Accept", visitor_, this);}
INLINE__ bool IRENe::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRENe::Accept", visitor_, this);}
INLINE__ bool IREEq::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREEq::Accept", visitor_, this);}
INLINE__ bool IREEq::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREEq::Accept", visitor_, this);}
INLINE__ bool IRELe::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRELe::Accept", visitor_, this);}
INLINE__ bool IRELe::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRELe::Accept", visitor_, this);}
INLINE__ bool IREGe::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREGe::Accept", visitor_, this);}
INLINE__ bool IREGe::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREGe::Accept", visitor_, this);}
INLINE__ bool IRELt::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRELt::Accept", visitor_, this);}
INLINE__ bool IRELt::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRELt::Accept", visitor_, this);}
INLINE__ bool IREGt::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREGt::Accept", visitor_, this);}
INLINE__ bool IREGt::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREGt::Accept", visitor_, this);}
INLINE__ bool IRECast::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREUnary>("IRECast::Accept", visitor_, this);}
INLINE__ bool IRECast::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREUnary>("IRECast::Accept", visitor_, this);}
INLINE__ bool IRELOr::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRELOr::Accept", visitor_, this);}
INLINE__ bool IRELOr::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRELOr::Accept", visitor_, this);}
INLINE__ bool IREBOr::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREBOr::Accept", visitor_, this);}
INLINE__ bool IREBOr::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREBOr::Accept", visitor_, this);}
INLINE__ bool IREBXOr::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREBXOr::Accept", visitor_, this);}
INLINE__ bool IREBXOr::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREBXOr::Accept", visitor_, this);}
INLINE__ bool IRELAnd::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRELAnd::Accept", visitor_, this);}
INLINE__ bool IRELAnd::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRELAnd::Accept", visitor_, this);}
INLINE__ bool IREBAnd::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREBAnd::Accept", visitor_, this);}
INLINE__ bool IREBAnd::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREBAnd::Accept", visitor_, this);}
INLINE__ bool IRENot::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREUnary>("IRENot::Accept", visitor_, this);}
INLINE__ bool IRENot::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREUnary>("IRENot::Accept", visitor_, this);}
INLINE__ bool IREBNeg::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREUnary>("IREBNeg::Accept", visitor_, this);}
INLINE__ bool IREBNeg::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREUnary>("IREBNeg::Accept", visitor_, this);}
INLINE__ bool IREANeg::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREUnary>("IREANeg::Accept", visitor_, this);}
INLINE__ bool IREANeg::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREUnary>("IREANeg::Accept", visitor_, this);}
INLINE__ bool IREQuo::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREQuo::Accept", visitor_, this);}
INLINE__ bool IREQuo::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREQuo::Accept", visitor_, this);}
INLINE__ bool IREMod::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREMod::Accept", visitor_, this);}
INLINE__ bool IREMod::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREMod::Accept", visitor_, this);}
INLINE__ bool IRERem::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRERem::Accept", visitor_, this);}
INLINE__ bool IRERem::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRERem::Accept", visitor_, this);}
INLINE__ bool IREMul::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREMul::Accept", visitor_, this);}
INLINE__ bool IREMul::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREMul::Accept", visitor_, this);}
INLINE__ bool IREDiv::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREDiv::Accept", visitor_, this);}
INLINE__ bool IREDiv::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREDiv::Accept", visitor_, this);}
INLINE__ bool IRESub::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IRESub::Accept", visitor_, this);}
INLINE__ bool IRESub::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IRESub::Accept", visitor_, this);}
INLINE__ bool IREAdd::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREAdd::Accept", visitor_, this);}
INLINE__ bool IREAdd::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREAdd::Accept", visitor_, this);}
INLINE__ bool IREDeref::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREUnary>("IREDeref::Accept", visitor_, this);}
INLINE__ bool IREDeref::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREUnary>("IREDeref::Accept", visitor_, this);}
INLINE__ bool IREPtrSub::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREPtrSub::Accept", visitor_, this);}
INLINE__ bool IREPtrSub::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREPtrSub::Accept", visitor_, this);}
INLINE__ bool IREPtrAdd::Accept(ExprVisitor& visitor_)
  {return GenericExprAccept<IREBinary>("IREPtrAdd::Accept", visitor_, this);}
INLINE__ bool IREPtrAdd::Accept(const CExprVisitor& visitor_) const
  {return GenericExprAccept<IREBinary>("IREPtrAdd::Accept", visitor_, this);}

INLINE__
bool IREAddrOf::
Accept(ExprVisitor& visitor_) {
  BMDEBUG2("IREAddrOf::Accept", &visitor_);
  bool retVal(true);
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  if (visitor_.GetOrderType() == EVOTPre) {
    retVal = visitor_.Visit(this);
  } // if
  if (retVal == true) {
    if (mExpr != 0 && !mExpr->Accept(visitor_)) {
      retVal = false;
    } // if
  } // if
  if (visitor_.GetOrderType() == EVOTPost) {
    retVal &= visitor_.Visit(this);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IREAddrOf::Accept

INLINE__
bool IREAddrOf::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG2("IREAddrOf::Accept", &visitor_);
  bool retVal(true);
  if (visitor_.GetOrderType() == EVOTPre) {
    retVal = visitor_.Visit(this);
  } // if
  if (retVal == true) {
    if (mExpr != 0 && !mExpr->Accept(visitor_)) {
      retVal = false;
    } // if
  } // if
  if (visitor_.GetOrderType() == EVOTPost) {
    retVal &= visitor_.Visit(this);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IREAddrOf::Accept

INLINE__
bool IREBinary::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IREBinary::Accept");
  bool retVal(true);
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  if (!mLeftExpr->Accept(visitor_)) {
    retVal = false;
  } else if (!mRightExpr->Accept(visitor_)) {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IREBinary::Accept

INLINE__
bool IREBinary::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG1("IREBinary::Accept");
  bool retVal(true);
  if (!mLeftExpr->Accept(visitor_)) {
    retVal = false;
  } else if (!mRightExpr->Accept(visitor_)) {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IREBinary::Accept

INLINE__
bool IREUnary::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IREUnary::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(true);
  if (!GetExpr()->Accept(visitor_)) {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IREUnary::Accept

INLINE__
bool IREUnary::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG1("IREUnary::Accept");
  bool retVal(true);
  if (!GetExpr()->Accept(visitor_)) {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IREUnary::Accept

INLINE__
bool IRSSwitch::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSSwitch::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} //IRSSwitch::Accept

INLINE__
bool IRSEpilog::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSEpilog::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSEpilog::Accept

INLINE__
bool IRSProlog::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSProlog::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSProlog::Accept

INLINE__
bool IRSReturn::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSReturn::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSReturn::Accept

INLINE__
bool IRSDynJump::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSDynJump::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSDynJump::Accept

INLINE__
bool IRSJump::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSJump::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSJump::Accept

INLINE__
bool IRSIf::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSIf::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSIf::Accept

INLINE__
bool IRSIf::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IRSIf::Accept");
  bool retVal(mCondExpr->Accept(visitor_));
  EMDEBUG1(retVal);
  return retVal;
} // IRSIf::Accept

INLINE__
bool IRSLabel::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSLabel::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSLabel::Accept

INLINE__
bool IRSBuiltInCall::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSBuiltInCall::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSBuiltInCall::Accept

INLINE__
bool IRSFCall::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSFCall::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSFCall::Accept

INLINE__
bool IRSPCall::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSPCall::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSPCall::Accept

INLINE__
bool IREConst::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG1("IREConst::Accept");
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IREConst::Accept

INLINE__
bool IREAddrConst::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IREAddrConst::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IREAddrConst::Accept

INLINE__
bool IRERealConst::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IRERealConst::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRERealConst::Accept

INLINE__
bool IREBoolConst::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IREBoolConst::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IREBoolConst::Accept

INLINE__
bool IREStrConst::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IREStrConst::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IREStrConst::Accept

INLINE__
bool IREIntConst::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IREIntConst::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IREIntConst::Accept

INLINE__
bool IREConst::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IREConst::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IREConst::Accept

INLINE__
bool IRSCall::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSCall::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSCall::Accept

INLINE__
bool IRSAssign::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSAssign::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSAssign::Accept

INLINE__
bool IRSEval::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSEval::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSEval::Accept

INLINE__
bool IRSNull::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRSNull::Accept");
  if (visitor_.GetOptions().IsShouldMark() == true) {
    SetMark2(visitor_.GetOptions().GetMark(), visitor_.GetOptions().GetMarkWhat());
  } // if
  bool retVal(visitor_.Visit(this));
  EMDEBUG1(retVal);
  return retVal;
} // IRSNull::Accept

INLINE__
IRECmp* IREGt::
GetInverse() const {
  return new IRELe(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IREGt::GetInverse

INLINE__
IRTree* IREGt::
GetRecCopy() const {
  return new IREGt(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IREGt::GetRecCopy

INLINE__
IRECmp* IRELt::
GetInverse() const {
  return new IREGe(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IRELt::GetInverse

INLINE__
IRTree* IRELt::
GetRecCopy() const {
  return new IRELt(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IRELt::GetRecCopy

INLINE__
IRECmp* IREGe::
GetInverse() const {
  return new IRELt(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IREGe::GetInverse

INLINE__
IRTree* IREGe::
GetRecCopy() const {
  return new IREGe(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IREGe::GetRecCopy

INLINE__
IRECmp* IRELe::
GetInverse() const {
  return new IREGt(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IRELe::GetInverse

INLINE__
IRTree* IRELe::
GetRecCopy() const {
  return new IRELe(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IRELe::GetRecCopy

INLINE__
IRECmp* IREEq::
GetInverse() const {
  return new IRENe(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IREEq::GetInverse

INLINE__
IRTree* IREEq::
GetRecCopy() const {
  return new IREEq(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IREEq::GetRecCopy

INLINE__
IRECmp* IRENe::
GetInverse() const {
  return new IREEq(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IRENe::GetInverse

INLINE__
IRTree* IRENe::
GetRecCopy() const {
  return new IRENe(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
} // IRENe::GetRecCopy

INLINE__
IRModule* IRFunctionDecl::
GetModule() const {
  REQUIREDMSG(!pred.pIsInvalid(mParentModule), ieStrParamValuesDBCViolation);
  return mParentModule;
} // IRFunctionDecl::GetModule

INLINE__
IRInductionVar::
IRInductionVar(IRLoop* parent_, IRStmt* defStmt_, IRObject* basicIndVar_, IRExpr* mulFactor_, IRExpr* addFactor_) :
  mParent(parent_),
  mIndVar(basicIndVar_),
  mBasisVar(basicIndVar_),
  mMulFactor(mulFactor_),
  mAddFactor(addFactor_),
  mDefStmt(defStmt_),
  mBasisIV(0)
{
  //! \todo H DBC: add is all non invalid and use it in all missing places.
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(basicIndVar_, mulFactor_, addFactor_) == true, ieStrParamValuesDBCViolation);
} // IRInductionVar::IRInductionVar

INLINE__
IRInductionVar::
IRInductionVar(IRLoop* parent_, IRStmt* defStmt_, IRObject* indVar_, IRObject* basisVar_, IRExpr* mulFactor_, IRExpr* addFactor_) :
  mParent(parent_),
  mIndVar(indVar_),
  mBasisVar(basisVar_),
  mMulFactor(mulFactor_),
  mAddFactor(addFactor_),
  mDefStmt(defStmt_),
  mBasisIV(0)
{
  //! \todo H DBC: add is all non invalid and use it in all missing places.
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(indVar_, basisVar_, mulFactor_, addFactor_) == true, ieStrParamValuesDBCViolation);
} // IRInductionVar::IRInductionVar

INLINE__
IRStmt* IRStmt::
GetRecCopyStmt() const {
  IRTree* retVal(GetRecCopy());
  ASSERTMSG(pred.pIsStmt(retVal) == true, ieStrInconsistentInternalStructure);
  return static_cast<IRStmt*>(retVal);
} // IRStmt::GetRecCopyStmt

INLINE__
void IRObject::
setAddrTaken(bool newValue_) {
  BMDEBUG2("IRObject::setAddrTaken", newValue_);
  if (newValue_ == consts.cAddrTaken) {
    if (mAddrTaken >= 0) {
      ++ mAddrTaken;
    } else {
      mAddrTaken = 1;
    } // if
  } else {
    if (mAddrTaken > 0) {
      -- mAddrTaken;
    } else {
      mAddrTaken = 0;
    } // if
  } // if
  EMDEBUG1(mAddrTaken);
  return;
} // IRObject::setAddrTaken



#endif


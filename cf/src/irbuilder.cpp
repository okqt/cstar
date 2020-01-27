// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
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
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __HOSTTYPES_HPP__
#include "hosttypes.hpp"
#endif
#ifndef __OPTIONS_HPP__
#include "options.hpp"
#endif
#ifndef __STATS_HPP__
#include "stats.hpp"
#endif

//! \file irbuilder.cpp Implementation of IRBuilder and IRBuilderHelper member functions.

//! \todo H Design: In caase binary distribution is provided how could DBC help?
//!       I guess we need to dump the expr string (we do it already) and function
//!       parameter names should match with the header parameter names.

//! \todo H Design: Add to coding styles that you may not use a locally declared variable
//!       in REQUIRED and preferably in ENSURE. Due to the case of binary distribution.

static const char* ieStrDBCSingletonUseContext =
  "You should not change the context when using Singleton<IRBuilder>";
map<hUInt64, IRTReal*> IRBuilder::smRealTypes;
map<hUInt64, IRTInt*> IRBuilder::smIntTypes;
vector<IRTStruct*> IRBuilder::smStructTypes;
vector<IRTArray*> IRBuilder::smArrayTypes;
vector<IRTUnion*> IRBuilder::smUnionTypes;
vector<IRType*> IRBuilder::smAllTypes;
map<pair<const IR*, string>, string> IRBuilder::smTaggedLabels;
DIEBuilder* IRBuilder::smDIEBuilder;
IRBuilder* irBuilder(0);
IRBuilderHelper* irhBuilder(0);
hFUInt32 IRBuilder::smAssocGroupNoIndex = 1;

IRBuilder::
IRBuilder(IRModule* cxtModule_, bool doOptims_) :
  mDoOptims(doOptims_),
  mIRHBuilder(new IRBuilderHelper(this))
{
  REQUIREDMSG(cxtModule_ != 0, ieStrParamValuesDBCViolation);
  mContext.mModule = cxtModule_;
} // IRBuilder::IRBuilder

IRBuilder::
IRBuilder(IRFunction* cxtFunc_, bool doOptims_) :
  mDoOptims(doOptims_),
  mIRHBuilder(new IRBuilderHelper(this))
{
  REQUIREDMSG(cxtFunc_ != 0, ieStrParamValuesDBCViolation);
  mContext.mFunc = cxtFunc_;
  mContext.mModule = extr.eGetModule(mContext.mFunc);
} // IRBuilder::IRBuilder

IRBuilder::
IRBuilder(IRBB* cxtBB_, bool doOptims_) :
  mDoOptims(doOptims_),
  mIRHBuilder(new IRBuilderHelper(this))
{
  mContext.mBB = cxtBB_;
  mContext.mFunc = mContext.mBB->GetCFG()->GetFunc();
  mContext.mModule = extr.eGetModule(mContext.mFunc);
} // IRBuilder::IRBuilder

/////////////////////////////
// IRBuilder members
/////////////////////////////
IRModule* IRBuilder::
irbModule() {
  BMDEBUG1("IRBuilder::irbModule");
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  IRModule* retVal(new IRModule());
  mContext.mModule = retVal;
  if (options.obGet("debuginfo") == true) {
    retVal->diSetDIE(smDIEBuilder->CreateModuleDIE(retVal));
  } // if
  irProgram->AddModule(mContext.mModule);
  ENSURE_VALIDMSG(mContext.mModule, ieStrMustEnsureValid);
  EMDEBUG1(mContext.mModule);
  return mContext.mModule;
} // IRBuilder::irbModule

bool IRBuilder::
irbInsertSucc(IRBB* fromBB_, IRBB* toBB_) const {
  BMDEBUG3("IRBuilder::irbInsertSucc", fromBB_, toBB_);
  REQUIREDMSG(fromBB_ != 0, ieStrNonNullParam);
  REQUIREDMSG(toBB_ != 0, ieStrNonNullParam);
  REQUIREDMSG(extr.eGetNumOfSuccs(fromBB_) <= 1, ieStrParamValuesDBCViolation << " " << extr.eGetNumOfSuccs(fromBB_));
  REQUIREDMSG(extr.eGetNumOfSuccs(fromBB_) == 0 || pred.pIsASucc(fromBB_, toBB_) == true,
    ieStrParamValuesDBCViolation << " " << extr.eGetNumOfSuccs(fromBB_) << " " << pred.pIsASucc(fromBB_, toBB_));
  tristate retVal;
  if (!pred.pIsASucc(fromBB_, toBB_)) {
    irbInsertStmt(new IRSJump(toBB_, new DIEBase(SrcLoc())), ICBB(fromBB_, ICEndOfBB));
    retVal = true;
  } else {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuider::irbInsertSucc

IRFunction* IRBuilder::
irbFunction(const string& name_, IRTFunc* funcPrototype_, IRModule* module_, const SrcLoc& srcLoc_) {
  BMDEBUG4("IRBuilder::irbFunction", funcPrototype_, module_, &srcLoc_);
  REQUIRED_VALID(module_);
  REQUIRED_VALID(funcPrototype_);
  IRFunctionDecl* lFuncDecl(new IRFunctionDecl(name_, funcPrototype_, module_));
  CFG* lCFG(new CFG());
  irbInsertSucc(lCFG->GetEntryBB(), lCFG->GetExitBB());
  irbsProlog(ICBB(lCFG->GetEntryBB(), ICBegOfBB), srcLoc_);
  irbsEpilog(ICBB(lCFG->GetExitBB(), ICBegOfBB), SrcLoc());
  IRFunction* retVal(new IRFunction(lFuncDecl, lCFG));
  if (options.obGet("debuginfo") == true) {
    retVal->diSetDIE(smDIEBuilder->CreateFuncDIE(retVal));
  } // if
  module_->AddFunction(retVal);
  SetFuncContext(retVal);
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbFunction

IRSAssign* IRBuilderHelper::
irbsIncrement(IRObject* obj_, const ICStmt& incCxt_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilderHelper::irbsIncrement", obj_, &incCxt_, &srcLoc_);
  IRSAssign* retVal(mIRBuilder->irbsAssign(mIRBuilder->irbeAddrOf(obj_),
    mIRBuilder->irbeAdd(obj_, irbeGetIntConst_One()), incCxt_, srcLoc_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsIncrement

IREIntConst* IRBuilderHelper::
irbeGetIntConst_Zero() const {
  return mIRBuilder->irbeIntConst(*Target::GetTarget()->GetIntType(8, 0, IRSDoNotCare), 0);
} // IRBuilderHelper::irbeGetIntConst_Zero

IREIntConst* IRBuilderHelper::
irbeGetIntConst_One() const {
  return mIRBuilder->irbeIntConst(*Target::GetTarget()->GetIntType(8, 0, IRSDoNotCare), 1);
} // IRBuilderHelper::irbeGetIntConst_One

IRSAssign* IRBuilderHelper::
irbsDecrement(IRObject* obj_, const ICStmt& decCxt_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilderHelper::irbsDecrement", obj_, &decCxt_, &srcLoc_);
  IRSAssign* retVal(mIRBuilder->irbsAssign(mIRBuilder->irbeAddrOf(obj_),
    mIRBuilder->irbeSub(obj_, irbeGetIntConst_One()), decCxt_, srcLoc_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsDecrement

IRSJump* IRBuilder::
irbsJump(IRBB* targetBB_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilder::irbsJump", targetBB_, &ic_, &srcLoc_);
  REQUIRED_VALID(targetBB_);
  IRSJump* retVal(new IRSJump(targetBB_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALID(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsJump

IRSDynJump* IRBuilder::
irbsDynJump(IRExpr* targetExpr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilder::irbsDynJump", targetExpr_, &ic_, &srcLoc_);
  REQUIRED_VALID(targetExpr_);
  IRSDynJump* retVal(new IRSDynJump(targetExpr_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALID(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsDynJump

IRSJump* IRBuilderHelper::
irbsJump(const ICStmt& targetContext_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilderHelper::irbsJump", &targetContext_, &ic_, &srcLoc_);
  REQUIREDMSG(!(pred.pIsLastStmt(targetContext_.GetStmt()) && ic_.GetPlace() == ICAfter), 
    "Target context may not be after the last statement");
  REQUIREDMSG(!(pred.pIsFirstStmt(targetContext_.GetStmt()) && ic_.GetPlace() == ICBefore), 
    "Target context may not be before the first statement");
  REQUIREDMSG(pred.pIsNullIC(&ic_) == false, ieStrParamValuesDBCViolation);
  IRSJump* retVal(0);
  if (ic_.GetPlace() == ICBefore) {
    retVal = irbsJump(targetContext_.GetStmt()->GetPrevStmt(), ic_, srcLoc_);
  } else {
    retVal = irbsJump(targetContext_.GetStmt()->GetNextStmt(), ic_, srcLoc_);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsJump

IRSJump* IRBuilderHelper::
irbsJump(IRStmt* targetStmt_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilderHelper::irbsJump", targetStmt_, &ic_, &srcLoc_);
  REQUIRED(targetStmt_ != 0);
  REQUIRED(extr.eGetFunc(targetStmt_) == extr.eGetFunc(ic_.GetBB()));
  REQUIREDMSG(pred.pIsNullIC(&ic_) == false, ieStrParamValuesDBCViolation);
  IRSJump* retVal(0);
  bool lIsSplit(mIRBuilder->irbSplitBB(ICStmt(targetStmt_, ICBefore)));
  if (ic_.GetBB() == targetStmt_->GetBB() && pred.pIsBegOfBB(ic_)) {
    if (lIsSplit == true) {
      // Normally we are not allowed to remove statements in insertions,
      // but we know that we have created the Jump due to the split.
      vector<IRBB*> pred;
      extr.eGetPredsOfBB(targetStmt_->GetBB(), pred);
      mIRBuilder->irbRemoveStmt(pred.front()->GetLastStmt());
      retVal = mIRBuilder->irbsJump(targetStmt_->GetBB(), ICBB(pred.front(), ICEndOfBB), srcLoc_);
    } else {
      IRBB* lNewBB(mIRBuilder->irbInsertBB(ICBB(targetStmt_->GetBB(), ICBefore)));
      retVal = mIRBuilder->irbsJump(targetStmt_->GetBB(), ICBB(lNewBB, ICBegOfBB), srcLoc_);
    } // if
  } else {
    retVal = mIRBuilder->irbsJump(targetStmt_->GetBB(), ic_, srcLoc_);
  } // if
  ENSURE_VALID(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsJump

IRSReturn* IRBuilder::
irbsReturn(IRExpr* expr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilder::irbsReturn", expr_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(extr.eGetRetType(GetFuncContext()) == expr_->GetType(),
    ieStrParamValuesDBCViolation << "Return type of current fuction must match"
      "to the return statement expression type; return type=" << 
      progcxt(extr.eGetRetType(GetFuncContext())) <<
      " function type=" << progcxt(expr_->GetType()));
  REQUIRED_VALID(expr_);
  IRSReturn* retVal(new IRSReturn(expr_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALID(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsReturn

//! \todo M Design: Is it better to return the successor bb or the bb itself if not created.
bool IRBuilder::
irbSplitBB(const ICStmt& ic_) const {
  BMDEBUG2("IRBuilder::irbSplitBB", &ic_);
  REQUIRED_VALIDMSG(&ic_, ieStrParamValuesDBCViolation);
  tristate retVal;
  if (!pred.pIsEndOfBB(ic_) && !pred.pIsBegOfBB(ic_)) {
    // lICBB: since we move the stmt, ic context's bb changes, store the value beforehand.
    IRBB* lICBB(ic_.GetBB());
    IRBB* lNewBB(getNewBB(ic_.GetBB()->GetCFG()));
    GetHelper()->irbMoveStmts(ic_.GetPlace() == ICAfter ? ic_.GetStmt()->GetNextStmt() : ic_.GetStmt(),
      lICBB->GetLastStmt(), ICBB(lNewBB, ICBegOfBB));
    bool lInserted(irbInsertSucc(lICBB, lNewBB));
    updateDynamicJumps(extr.eGetFunc(lICBB));
    ASSERTMSG(lInserted == true, ieStrInconsistentInternalStructure);
    retVal = true;
  } else {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbSplitBB

IRSFCall* IRBuilder::
irbsFCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG6("IRBuilder::irbsFCall", returnIn_, funcExpr_, args_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(funcExpr_, args_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.pIsFunc(extr.eGetElemType(funcExpr_)) == false ||
    returnIn_->GetType() == extr.eGetFuncTypeOfCallExpr(funcExpr_)->GetReturnType(), 
    ieStrParamValuesDBCViolation << endl << "Return in obj and return type must match: ot=" << 
      progcxt(returnIn_->GetType()) << " ft=" << progcxt(extr.eGetFuncTypeOfCallExpr(funcExpr_)->GetReturnType()));
  //! \todo M Design: add dbc for void return typed cases.
  IRSFCall* retVal(new IRSFCall(returnIn_, funcExpr_, args_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsFCall

IROGlobal* IRBuilder::
getBuiltInObject(IRBIRId birId_) const {
  vector<IRType*> lParamTypes;
  IRType* lRetType;
  tristate lHasEllipses;
  string lFuncName;
  switch (birId_) {
    case IRBIRva_start:
      lHasEllipses = false;
      lFuncName = "va_start";
      break;
    case IRBIRva_arg:
      lHasEllipses = false;
      lFuncName = "va_arg";
      break;
    case IRBIRva_end:
      lHasEllipses = false;
      lFuncName = "va_end";
      break;
    case IRBIRva_copy:
      lHasEllipses = false;
      lFuncName = "va_copy";
      break;
    case IRBIRmemcpy:
      lRetType = irbtGetVoid();
      lParamTypes.push_back(irbtGetPtr(irbtGetVoid()));
      lParamTypes.push_back(irbtGetPtr(irbtGetVoid()));
      lParamTypes.push_back(irbtGetInt(Target::GetTarget()->Get_size_t()));
      lHasEllipses = false;
      lFuncName = "memcpy";
      break;
    default:
      ASSERTMSG(0, ieStrNotImplemented);
      break;
  } // switch
  IRTFunc* lFuncType(irbtGetFunc(lRetType, lParamTypes, lHasEllipses, &Singleton<CallConvDefault>::Obj()));
  IROGlobal* lFuncObj(irboGlobal(lFuncName, lFuncType, IRLStatic));
  return lFuncObj;
} // IRBuilder::getBuiltInObject

IRSBuiltInCall* IRBuilder::
irbsBIPCall(IRExprList* args_, IRBIRId birId_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG5("IRBuilder::irbsBIPCall", args_, birId_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(args_) == true, ieStrExprUsedBefore);
  IROGlobal* lFuncObj(getBuiltInObject(birId_));
  ASSERTMSG(extr.eGetParamCount(lFuncObj) == args_->GetSize(),
    ieStrInconsistentInternalStructure << " : Number of arguments do not match the parameter count");
  IRSBuiltInCall* retVal(new IRSBuiltInCall(irbeAddrOf(lFuncObj), args_, birId_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsBIPCall

IRSBuiltInCall* IRBuilder::
irbsBIFCall(IRObject* returnIn_, IRExprList* args_, IRBIRId birId_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG6("IRBuilder::irbsBIFCall", returnIn_, args_, birId_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(args_) == true, ieStrExprUsedBefore);
  IROGlobal* lFuncObj(getBuiltInObject(birId_));
  ASSERTMSG(extr.eGetParamCount(lFuncObj) == args_->GetSize(),
    ieStrInconsistentInternalStructure << " : Number of arguments do not match the parameter count");
  IRSBuiltInCall* retVal(new IRSBuiltInCall(returnIn_, irbeAddrOf(lFuncObj), args_, birId_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsBIFCall

IRENull* IRBuilder::
irbeNull() const {
  BMDEBUG1("IRBuilder::irbeNull");
  IRENull* retVal(new IRENull());
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeNull

IRSPCall* IRBuilder::
irbsPCall(IRExpr* funcExpr_, IRExprList* args_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG5("IRBuilder::irbsPCall", funcExpr_, args_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(funcExpr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(args_) == true, ieStrExprUsedBefore);
  IRSPCall* retVal(new IRSPCall(funcExpr_, args_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsPCall

IRSAssign* IRBuilder::
irbsAssign(IRExpr* lhs_, IRExpr* rhs_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG5("IRBuilder::irbsAssign", lhs_, rhs_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(rhs_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(lhs_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.pIsObj(lhs_) == false, ieStrParamValuesDBCViolation);
  REQUIREDMSG(extr.eGetElemType(lhs_) == rhs_->GetType(), ieStrParamValuesDBCViolation << 
    " ref type of lhs " << progcxt(extr.eGetElemType(lhs_)) << " is not equal to rhs " << progcxt(rhs_->GetType()));
  REQUIREDMSG(pred.pIsStructUnion(extr.eGetElemType(lhs_)) == false && pred.pIsStructUnion(rhs_->GetType()) == false, 
    ieStrParamValuesDBCViolation << " : use memcpy for assignment of structs");
  IRSAssign* retVal(new IRSAssign(lhs_, rhs_, new DIEBase(srcLoc_)));
  if (pred.pIsAddrOf(lhs_) == true) {
    if (IRObject* lObj = extr.eGetObjOfAddrOf(lhs_)) {
      lObj->setAddrTaken(consts.cAddrNotTaken);
    } // if
  } // if
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsAssign

IRSEval* IRBuilder::
irbsEval(IRExpr* expr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG2("IRBuilder::irbsEval", &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IRSEval* retVal(new IRSEval(expr_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsEval

IRSEpilog* IRBuilder::
irbsEpilog(const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG2("IRBuilder::irbsEpilog", &srcLoc_);
  IRSEpilog* retVal(new IRSEpilog(new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsEpilog

IRSProlog* IRBuilder::
irbsProlog(const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG3("IRBuilder::irbsProlog", &ic_, &srcLoc_);
  IRSProlog* retVal(new IRSProlog(new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsProlog

IRSSwitch* IRBuilder::
irbsSwitch(IRExpr* val_, const vector<pair<IRExprList*, IRBB*> >& caseStmts_, const InsertContext& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("IRBuilder::irbsSwitch", val_, &ic_, &srcLoc_);
  REQUIREDMSG(caseStmts_.size() != 0, "Switch statement must have at least one 'case'");
  REQUIREDMSG(caseStmts_.back().first->IsEmpty() == true, ieStrParamValuesDBCViolation << 
    " : last case statement (default case) must have empty expression list");
  IRSSwitch* retVal(new IRSSwitch(val_, caseStmts_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsSwitch

//! \note Actually all the labels are local. Their name is useless unless to be used as comments.
IRSLabel* IRBuilder::
irbsLabel(const string& name_, bool isAddrTaken_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG5("IRBuilder::irbsLabel", name_, isAddrTaken_, &ic_, &srcLoc_);
  IRSLabel* retVal(new IRSLabel(name_, new DIEBase(srcLoc_)));
  if (isAddrTaken_ == true) {
    retVal->SetAddressTaken();
  } // if
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALID(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsLabel

IRSNull* IRBuilder::
irbsNull(const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG3("IRBuilder::irbsNull", &ic_, &srcLoc_);
  IRSNull* retVal(new IRSNull(new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  ENSURE_VALID(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsNull

IREAdd* IRBuilder::
irbeAdd(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbeAdd", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IREAdd* retVal(0);
  if (pred.pIsConst(left_) == true) {
    retVal = new IREAdd(right_, left_);
  } else {
    retVal = new IREAdd(left_, right_);
  } // if
  onExprCreated(retVal);
  { 
    IREBinary* lBinRetVal(static_cast<IREBinary*>(retVal));
    if (pred.pIsAddOrSub(lBinRetVal->GetLeftExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetLeftExpr()->mAssocGroupNo;
    if (pred.pIsAddOrSub(lBinRetVal->GetRightExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetRightExpr()->mAssocGroupNo;
    if (!retVal->mAssocGroupNo) {
      retVal->mAssocGroupNo = smAssocGroupNoIndex ++;
    } // if
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeAdd

IRESub* IRBuilder::
irbeSub(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbeSub", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IRType* lType(!pred.pIsPtr(left_->GetType()) ? left_->GetType() : irbtGetInt(Target::GetTarget()->Get_ptrdiff_t()));
  IRESub* retVal(new IRESub(lType, left_, right_));
  onExprCreated(retVal);
  { 
    IREBinary* lBinRetVal(static_cast<IREBinary*>(retVal));
    if (pred.pIsAddOrSub(lBinRetVal->GetLeftExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetLeftExpr()->mAssocGroupNo;
    if (pred.pIsAddOrSub(lBinRetVal->GetRightExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetRightExpr()->mAssocGroupNo;
    if (!retVal->mAssocGroupNo) {
      retVal->mAssocGroupNo = smAssocGroupNoIndex ++;
    } // if
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeSub

IREMul* IRBuilder::
irbeMul(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbeMul", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IREMul* retVal(0);
  if (pred.pIsConst(left_) == true) {
    retVal = new IREMul(right_, left_);
  } else {
    retVal = new IREMul(left_, right_);
  } // if
  onExprCreated(retVal);
  { 
    IREBinary* lBinRetVal(static_cast<IREBinary*>(retVal));
    if (pred.pIsMul(lBinRetVal->GetLeftExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetLeftExpr()->mAssocGroupNo;
    if (pred.pIsMul(lBinRetVal->GetRightExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetRightExpr()->mAssocGroupNo;
    if (!retVal->mAssocGroupNo) {
      retVal->mAssocGroupNo = smAssocGroupNoIndex ++;
    } // if
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeMul

IREDiv* IRBuilder::
irbeDiv(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbeDiv", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IREDiv* retVal(new IREDiv(left_, right_));
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeDiv

IREMod* IRBuilder::
irbeMod(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbeMod", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IREMod* retVal(new IREMod(left_, right_));
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeMod

IRERem* IRBuilder::
irbeRem(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbeRem", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IRERem* retVal(new IRERem(left_, right_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeRem

IREQuo* IRBuilder::
irbeQuo(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbeQuo", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IREQuo* retVal(new IREQuo(left_, right_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeQuo

IREANeg* IRBuilder::
irbeANeg(IRExpr* expr_) const {
  BMDEBUG2("IRBuilder::irbeANeg", expr_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IREANeg* retVal(new IREANeg(expr_));
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeANeg

IRECast* IRBuilder::
irbeCast(IRType* castType_, IRExpr* expr_) const {
  BMDEBUG3("IRBuilder::irbeCast", castType_, expr_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IRECast* retVal(new IRECast(castType_, expr_));
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeCast

IREPtrSub* IRBuilder::
irbePtrSub(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbePtrSub", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.pIsNonTerminal(left_) == true || pred.pIsPtr(left_->GetType()) == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(pred.pIsNonTerminal(right_) == true || pred.pIsPtr(right_->GetType()) == true, ieStrParamValuesDBCViolation);
  //! \todo M Design: perhaps dbc can be improved to match IREPtrSub.
  if (pred.pIsObj(left_) == true) {
    left_ = GetHelper()->irbeUseOf(left_);
  } // if
  if (pred.pIsObj(right_) == true) {
    right_ = GetHelper()->irbeUseOf(right_);
  } // if
  IREPtrSub* retVal(new IREPtrSub(left_, right_));
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbePtrSub

IREPtrAdd* IRBuilder::
irbePtrAdd(IRExpr* ptr_, IRExpr* offset_) const {
  BMDEBUG3("IRBuilder::irbePtrAdd", ptr_, offset_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(ptr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(offset_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.pIsNonTerminal(ptr_) == true || pred.pIsPtr(ptr_->GetType()) == true,
    ieStrParamValuesDBCViolation << " : " << progcxt(ptr_));
  REQUIREDMSG(pred.pIsNonTerminal(ptr_) == true || pred.pIsInt(offset_->GetType()) == true,
    ieStrParamValuesDBCViolation << " : " << progcxt(offset_));
  IREPtrAdd* retVal(0);
  if (!pred.pIsObj(ptr_)) {
    retVal = new IREPtrAdd(ptr_, offset_);
  } else {
    retVal = new IREPtrAdd(GetHelper()->irbeUseOf(ptr_), offset_);
  } // if
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbePtrAdd

IREAddrOf* IRBuilder::
irbeAddrOf(IRObject* object_) const {
  BMDEBUG2("IRBuilder::irbeAddrOf", object_);
  REQUIRED_VALIDMSG(object_, ieStrObjectNotValid);
  IREAddrOf* retVal = new IREAddrOf(irbtGetPtr(object_->GetType()), object_);
  object_->setAddrTaken(consts.cAddrTaken);
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeAddrOf

IREAddrOf* IRBuilder::
irbeAddrOf(IRExpr* expr_) const {
  BMDEBUG2("IRBuilder::irbeAddrOf", expr_);
  REQUIRED_VALIDMSG(expr_, ieStrObjectNotValid);
  IREAddrOf* retVal(0);
  if (IRTArray* lArray = dynamic_cast<IRTArray*>(expr_->GetType())) {
    retVal = new IREAddrOf(irbtGetPtr(lArray->GetElemType()), expr_);
  } else {
    retVal = new IREAddrOf(irbtGetPtr(expr_->GetType()), expr_);
  } // if
  if (IRObject* lObj = dynamic_cast<IRObject*>(expr_)) {
    lObj->setAddrTaken(consts.cAddrTaken);
  } // if
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeAddrOf

IRSJump* IRBuilder::
irbpsJump() const {
  return new IRSJump(&Singleton<IRBBAny>::Obj(), new DIEBase(SrcLoc()));
} // IRBuilder::irbpsJump

IRSIf* IRBuilder::
irbpsIf(IRExpr* cond_) const {
  REQUIREDMSG(cond_ != 0, ieStrParamValuesDBCViolation);
  return new IRSIf(cond_, &Singleton<IRBBAny>::Obj(), &Singleton<IRBBAny>::Obj(), new DIEBase(SrcLoc()));
} // IRBuilder::irbpsIf

IRSFCall* IRBuilder::
irbpsFCall(IRExpr* expr_) const {
  REQUIREDMSG(expr_ != 0, ieStrParamValuesDBCViolation);
  return new IRSFCall(&Invalid<IRObject>::Obj(), expr_, new IRExprList(), new DIEBase(SrcLoc()));
} // IRBuilder::irbpsFCall

IRSBuiltInCall* IRBuilder::
irbpsBICall(IRExpr* expr_) const {
  REQUIREDMSG(expr_ != 0, ieStrParamValuesDBCViolation);
  return new IRSBuiltInCall(expr_, new IRExprList(), IRBIRCGPattern, new DIEBase(SrcLoc()));
} // IRBuilder::irbpsBICall

IRSPCall* IRBuilder::
irbpsPCall(IRExpr* expr_) const {
  REQUIREDMSG(expr_ != 0, ieStrParamValuesDBCViolation);
  return new IRSPCall(expr_, new IRExprList(), new DIEBase(SrcLoc()));
} // IRBuilder::irbpsPCall

IRSReturn* IRBuilder::
irbpsReturn(IRExpr* expr_) const {
  REQUIREDMSG(expr_ != 0, ieStrParamValuesDBCViolation);
  return new IRSReturn(expr_, new DIEBase(SrcLoc()));
} // IRBuilder::irbpsReturn

IRSEval* IRBuilder::
irbpsEval(IRExpr* expr_) const {
  REQUIREDMSG(expr_ != 0, ieStrParamValuesDBCViolation);
  return new IRSEval(expr_, new DIEBase(SrcLoc()));
} // IRBuilder::irbpsEval

IRSNull* IRBuilder::
irbpsNull() const {
  return new IRSNull(new DIEBase(SrcLoc()));
} // IRBuilder::irbpsNull

IRSLabel* IRBuilder::
irbpsLabel() const {
  return new IRSLabel("PatternLabel", new DIEBase(SrcLoc()));
} // IRBuilder::irbpsLabel

IRSAssign* IRBuilder::
irbpsAssign(IRExpr* lhs_, IRExpr* rhs_) const {
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(lhs_, rhs_) == true, ieStrParamValuesDBCViolation);
  return new IRSAssign(lhs_, rhs_, new DIEBase(SrcLoc()));
} // IRBuilder::irbpsAssign

IRSProlog* IRBuilder::
irbpsProlog() const {
  return new IRSProlog(new DIEBase(SrcLoc()));
} // IRBuilder::irbpsProlog

IRSEpilog* IRBuilder::
irbpsEpilog() const {
  return new IRSEpilog(new DIEBase(SrcLoc()));
} // IRBuilder::irbpsEpilog

IRECmp* IRBuilder::
irbpeCmp(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG3("IRBuilder::irbpeCmp", left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(left_, right_) == true, ieStrParamValuesDBCViolation);
  IRECmp* retVal(new IRECmp(left_, right_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbpeCmp

IREMember* IRBuilder::
irbeMember(IRExpr* base_, IROField* member_) const {
  BMDEBUG3("IRBuilder::irbeMember", base_, member_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(base_) == true, ieStrExprUsedBefore);
  IREMember* retVal(new IREMember(base_, member_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeMember

IREDeref* IRBuilder::
irbeDeref(IRExpr* ptr_) const {
  BMDEBUG2("IRBuilder::irbeDeref", ptr_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(ptr_) == true, ieStrExprUsedBefore);
  REQUIRED_VALIDMSG(ptr_, ieStrObjectNotValid);
  REQUIREDMSG(dynamic_cast<IRTPtr*>(ptr_->GetType()) != 0,
    ieStrInconsistentInternalStructure << ": Deref can only be used on pointer typed expressions");
  IRTPtr* lPtrType(dynamic_cast<IRTPtr*>(ptr_->GetType()));
  IREDeref* retVal(new IREDeref(lPtrType->GetRefType(), ptr_));
  if (pred.pIsAddrOf(ptr_) == true) {
    if (IRObject* lObj = extr.eGetObjOfAddrOf(ptr_)) {
      lObj->setAddrTaken(consts.cAddrNotTaken);
    } // if
  } // if
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeDeref

IRESubscript* IRBuilder::
irbeSubscript(IRExpr* basePtr_, IRExpr* index_) const {
  BMDEBUG3("IRBuilder::irbeSubscript", basePtr_, index_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(basePtr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(index_) == true, ieStrExprUsedBefore);
  REQUIRED_VALIDMSG(basePtr_, ieStrObjectNotValid);
  REQUIRED_VALIDMSG(index_, ieStrObjectNotValid);
  REQUIREDMSG(pred.pIsArray(basePtr_->GetType()) == true,
    ieStrInconsistentInternalStructure << ": subscript base type must be of array type");
  IRESubscript* retVal(new IRESubscript(extr.eGetElemType(basePtr_), basePtr_, index_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeSubscript

IREAShiftRight* IRBuilder::
irbeAShiftRight(IRExpr* expr_, IRExpr* numBits_) const {
  BMDEBUG1("IRBuilder::irbeAShiftRight");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(numBits_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.pIsUnsignedInt(expr_->GetType()) == false, ieStrParamValuesDBCViolation << "Use LShiftRight for unsigned");
  IREAShiftRight* retVal(new IREAShiftRight(expr_, numBits_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeAShiftRight

IRELShiftRight* IRBuilder::
irbeLShiftRight(IRExpr* expr_, IRExpr* numBits_) const {
  BMDEBUG1("IRBuilder::irbeLShiftRight");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(numBits_) == true, ieStrExprUsedBefore);
  IRELShiftRight* retVal(new IRELShiftRight(expr_, numBits_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeLShiftRight

IREShiftLeft* IRBuilder::
irbeShiftLeft(IRExpr* expr_, IRExpr* numBits_) const {
  BMDEBUG1("IRBuilder::irbeShiftLeft");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(numBits_) == true, ieStrExprUsedBefore);
  IREShiftLeft* retVal(new IREShiftLeft(expr_, numBits_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeShiftLeft

IREBAnd* IRBuilder::
irbeBAnd(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeBAnd");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(right_) == true, ieStrExprUsedBefore);
  IREBAnd* retVal(new IREBAnd(left_, right_));
  onExprCreated(retVal);
  { 
    IREBinary* lBinRetVal(static_cast<IREBinary*>(retVal));
    if (pred.pIsBAnd(lBinRetVal->GetLeftExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetLeftExpr()->mAssocGroupNo;
    if (pred.pIsBAnd(lBinRetVal->GetRightExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetRightExpr()->mAssocGroupNo;
    if (!retVal->mAssocGroupNo) {
      retVal->mAssocGroupNo = smAssocGroupNoIndex ++;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeBAnd

IREBOr* IRBuilder::
irbeBOr(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeBOr");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IREBOr* retVal(0);
  if (pred.pIsConst(left_) == true) {
    retVal = new IREBOr(right_, left_);
  } else {
    retVal = new IREBOr(left_, right_);
  } // if
  onExprCreated(retVal);
  { 
    IREBinary* lBinRetVal(static_cast<IREBinary*>(retVal));
    if (pred.pIsBOr(lBinRetVal->GetLeftExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetLeftExpr()->mAssocGroupNo;
    if (pred.pIsBOr(lBinRetVal->GetRightExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetRightExpr()->mAssocGroupNo;
    if (!retVal->mAssocGroupNo) {
      retVal->mAssocGroupNo = smAssocGroupNoIndex ++;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeBOr

IREBXOr* IRBuilder::
irbeBXOr(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeBXOr");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IREBXOr* retVal(0);
  if (pred.pIsConst(left_) == true) {
    retVal = new IREBXOr(right_, left_);
  } else {
    retVal = new IREBXOr(left_, right_);
  } // if
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeBXOr

IREBNeg* IRBuilder::
irbeBNeg(IRExpr* expr_) const {
  BMDEBUG1("IRBuilder::irbeBNeg");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IREBNeg* retVal(new IREBNeg(expr_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeBNeg

IRELAnd* IRBuilder::
irbeLAnd(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeLAnd");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IRELAnd* retVal(new IRELAnd(left_, right_));
  onExprCreated(retVal);
  {
    IREBinary* lBinRetVal(static_cast<IREBinary*>(retVal));
    if (pred.pIsLAnd(lBinRetVal->GetLeftExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetLeftExpr()->mAssocGroupNo;
    if (pred.pIsLAnd(lBinRetVal->GetRightExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetRightExpr()->mAssocGroupNo;
    if (!retVal->mAssocGroupNo) {
      retVal->mAssocGroupNo = smAssocGroupNoIndex ++;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeLAnd

IRELOr* IRBuilder::
irbeLOr(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeLOr");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IRELOr* retVal(new IRELOr(left_, right_));
  onExprCreated(retVal);
  {
    IREBinary* lBinRetVal(static_cast<IREBinary*>(retVal));
    if (pred.pIsLOr(lBinRetVal->GetLeftExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetLeftExpr()->mAssocGroupNo;
    if (pred.pIsLOr(lBinRetVal->GetRightExpr()) == true) lBinRetVal->mAssocGroupNo = lBinRetVal->GetRightExpr()->mAssocGroupNo;
    if (!retVal->mAssocGroupNo) {
      retVal->mAssocGroupNo = smAssocGroupNoIndex ++;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeLOr

IRENot* IRBuilder::
irbeNot(IRExpr* expr_) const {
  BMDEBUG1("IRBuilder::irbeNot");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IRENot* retVal(new IRENot(expr_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeNot

IREGt* IRBuilder::
irbeGt(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeGt");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IREGt* retVal(new IREGt(left_, right_));
  onExprCreated(retVal);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeGt

IRELt* IRBuilder::
irbeLt(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeLt");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IRELt* retVal(new IRELt(left_, right_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeLt

//! \todo H Design: Handle const fold triggers.
void IRBuilder::
onExprCreated(IRExpr* newExpr_) const {
  BMDEBUG2("IRBuilder::onExprCreated", newExpr_);
  if (mDoOptims == true) {
    if (newExpr_->constFold(newExpr_) == true) {
      STATRECORD("irbuilder: optim count");
    } // if
  } // if
  EMDEBUG0();
  return;
} // IRBuilder::onExprCreated

IREEq* IRBuilder::
irbeEq(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeEq");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IREEq* retVal(0);
  if (pred.pIsConst(left_) == true) {
    retVal = new IREEq(right_, left_);
  } else {
    retVal = new IREEq(left_, right_);
  } // if
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeEq

IRENe* IRBuilder::
irbeNe(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeNe");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IRENe* retVal(0);
  if (pred.pIsConst(left_) == true) {
    retVal = new IRENe(right_, left_);
  } else {
    retVal = new IRENe(left_, right_);
  } // if
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeNe

IREGe* IRBuilder::
irbeGe(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeGe");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IREGe* retVal(new IREGe(left_, right_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeGe

IRELe* IRBuilder::
irbeLe(IRExpr* left_, IRExpr* right_) const {
  BMDEBUG1("IRBuilder::irbeLe");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(left_, right_) == true, ieStrExprUsedBefore);
  IRELe* retVal(new IRELe(left_, right_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeLe

IREIntConst* IRBuilder::
irbeIntConst(tInt type_, tBigInt value_) const {
  BMDEBUG3("IRBuilder::irbeIntConst", &type_, &value_);
  IREIntConst* retVal(irbeIntConst(irbtGetInt(type_), value_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeIntConst

IREBoolConst* IRBuilder::
irbeBoolConst(bool value_) const {
  BMDEBUG1("IRBuilder::irbeBoolConst");
  IREBoolConst* retVal(new IREBoolConst(value_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeBoolConst

IRObject* IRBuilder::
irbpoObject() const {
  BMDEBUG1("IRBuilder::irbpoObject");
  IRObject* retVal(new IRObject(irbtGetVoid(), "$pat_object"));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbpoObject

IROGlobal* IRBuilder::
irbpoGlobal() const {
  BMDEBUG1("IRBuilder::irbpoGlobal");
  IRLinkage lDoNotCare(IRLStatic);
  IROGlobal* retVal(new IROGlobal(irbtGetVoid(), "$pat_obj_glob", lDoNotCare));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbpoGlobal

IROLocal* IRBuilder::
irbpoLocal() const {
  BMDEBUG1("IRBuilder::irbpoLocal");
  IROLocal* retVal(new IROLocal(irbtGetVoid(), "$pat_obj_local"));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbpoLocal

IROTmp* IRBuilder::
irbpoTmp() const {
  BMDEBUG1("IRBuilder::irbpoTmp");
  IROTmp* retVal(new IROTmp("$pat_obj_tmp", irbtGetVoid()));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboTmp

IROParameter* IRBuilder::
irbpoParam() const {
  BMDEBUG1("IRBuilder::irbpoParam");
  IROParameter* retVal(new IROParameter(irbtGetVoid(), "$pat_obj_param"));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbpoParam

IREStrConst* IRBuilder::
irbeStrConst(const string& value_) const {
  BMDEBUG2("IRBuilder::irbeStrConst", value_);
  IREStrConst* retVal(new IREStrConst(value_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeStrConst

IREAddrConst* IRBuilder::
irbeAddrConst(IRType* type_, tBigInt addrConst_) const {
  BMDEBUG3("IRBuilder::irbeAddrConst", type_, &addrConst_);
  IREAddrConst* retVal(new IREAddrConst(type_, addrConst_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeAddrConst

IREAddrConst* IRBuilder::
irbeAddrConst(IRType* type_, IRSLabel* label_, tBigAddr offset_) const {
  BMDEBUG4("IRBuilder::irbeAddrConst", type_, label_, &offset_);
  IREAddrConst* retVal(new IREAddrConst(type_, label_, offset_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeAddrConst

IREAddrConst* IRBuilder::
irbeAddrConst(IRType* type_, IRObject* object_, tBigAddr offset_) const {
  BMDEBUG4("IRBuilder::irbeAddrConst", type_, object_, &offset_);
  IREAddrConst* retVal(new IREAddrConst(type_, object_, offset_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeAddrConst

IREIntConst* IRBuilder::
irbeIntConst(IRTInt* type_, tBigInt value_) const {
  BMDEBUG1("IRBuilder::irbeIntConst");
  IREIntConst* retVal(new IREIntConst(type_, value_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeIntConst

IRERealConst* IRBuilder::
irbeRealConst(tReal type_, tBigReal value_) const {
  BMDEBUG1("IRBuilder::irbeRealConst");
  IRERealConst* retVal(irbeRealConst(irbtGetReal(type_), value_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeRealConst

IRERealConst* IRBuilder::
irbeRealConst(IRTReal* type_, tBigReal value_) const {
  BMDEBUG1("IRBuilder::irbeRealConst");
  IRERealConst* retVal(new IRERealConst(type_, value_));
  onExprCreated(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeRealConst

bool IRTypeSetSort::
operator() (const IRType* left_, const IRType* right_) const {
  //! \todo For the time being always insert elements to set.
  return true;
  NOTIMPLEMENTED(M);
} // IRTypeSetSort::operator()

IRTPtr* IRBuilder::
irbtGetPtr(IRType* refType_) const {
  BMDEBUG2("IRBuilder::irbtGetPtr", refType_);
  REQUIREDMSG(refType_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(!pred.pIsInvalid(refType_), ieStrParamValuesDBCViolation);
  static map<IRType*, IRTPtr*> lPtrTypes;
  map<IRType*, IRTPtr*>::iterator lIt(lPtrTypes.find(refType_));
  IRTPtr* retVal(0);
  if (lIt != lPtrTypes.end()) {
    retVal = lIt->second;
  } else {
    retVal = new IRTPtr(refType_);
    lPtrTypes[refType_] = retVal;
    smAllTypes.push_back(retVal);
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbtGetPtr

// //////////////////////////
// Object Creation
// //////////////////////////
IROGlobal* IRBuilder::
irboGlobal(const string& name_, IRType* type_, IRLinkage linkage_) const {
  BMDEBUG4("IRBuilder::irboGlobal", name_.c_str(), type_, linkage_);
  REQUIREDMSG(type_ != 0 && !pred.pIsInvalid(type_), ieStrParamValuesDBCViolation);
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  REQUIRED_VALIDMSG(mContext.mModule, ieStrObjectNotValid);
  ostrstream lName;
  lName << name_;
  if (linkage_ == IRLStatic && !pred.pIsFunc(type_)) {
    // name should be made unique.
    static hFInt32 lIndex;
    lName << "." << lIndex++;
  } // if
  lName << ends;
  IROGlobal* retVal(new IROGlobal(type_, lName.str(), linkage_));
  if (options.obGet("debuginfo") == true) {
    retVal->diSetDIE(smDIEBuilder->CreateObjDIE(retVal));
  } // if
  retVal->SetAddress(AddrLTConst(lName.str(), 0));
  mContext.mModule->AddGlobal(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboGlobal

IROLocal* IRBuilder::
irboLocal(const string& name_, IRType* type_) const {
  BMDEBUG3("IRBuilder::irboLocal", name_.c_str(), type_);
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  REQUIRED_VALIDMSG(mContext.mFunc, ieStrObjectNotValid);
  IROLocal* retVal(new IROLocal(type_, name_));
  if (options.obGet("debuginfo") == true) {
    retVal->diSetDIE(smDIEBuilder->CreateObjDIE(retVal));
  } // if
  mContext.mFunc->AddLocal(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboLocal

IROLocal* IRBuilder::
irboLocal(const string& name_, IRType* type_, const Address& addr_) const {
  BMDEBUG4("IRBuilder::irboLocal", name_.c_str(), type_, &addr_);
  IROLocal* retVal(irboLocal(name_, type_));
  retVal->SetAddress(addr_);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboLocal

IRORelocSymbol* IRBuilder::
irboRelocSymbol(const string& name_, IRType* type_) const {
  BMDEBUG3("IRBuilder::irboRelocSymbol", name_, type_);
  IRORelocSymbol* retVal(new IRORelocSymbol(type_, name_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboRelocSymbol

IROGlobal* IRBuilder::
irboTmpGlobal(IRType* type_) const {
  BMDEBUG2("IRBuilder::irboTmpGlobal", type_);
  REQUIRED_VALIDMSG(mContext.mModule, ieStrObjectNotValid);
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  char lName[128];
  static hFInt32 slIndex;
  //! \todo M Design: Name of the temporary global must be selected by
  //!       assembly style, ASMDesc. Or postpone the name assignment somehow?
  sprintf(lName, ".LTG%d", slIndex++);
  IROGlobal* retVal(new IROGlobal(type_, lName, IRLStatic));
  mContext.mModule->AddGlobal(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboTmpGlobal

IROTmp* IRBuilder::
irboTmp(IRType* type_, const Address& addr_, const string& prefix_) const {
  BMDEBUG4("IRBuilder::irboTmp", type_, &addr_, prefix_);
  IROTmp* retVal(irboTmp(type_, prefix_));
  retVal->SetAddress(addr_);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboTmp

IROTmp* IRBuilder::
irboTmp(IRType* type_, const string& prefix_) const {
  BMDEBUG3("IRBuilder::irboTmp", type_, prefix_);
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  REQUIRED_VALIDMSG(mContext.mFunc, ieStrObjectNotValid);
  char lName[64];
  static hFInt32 slIndex;
  sprintf(lName, "$%s_%d", prefix_.c_str(), slIndex ++);
  IROTmp* retVal(new IROTmp(lName, type_));
  mContext.mFunc->AddLocal(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboTmp

IROParameter* IRBuilder::
irboParameter(const string& name_, IRType* type_) const {
  BMDEBUG3("IRBuilder::irboParameter", name_.c_str(), type_);
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  REQUIRED_VALIDMSG(mContext.mFunc, ieStrObjectNotValid);
  IROParameter* retVal(new IROParameter(type_, name_));
  if (options.obGet("debuginfo") == true) {
    retVal->diSetDIE(smDIEBuilder->CreateObjDIE(retVal));
  } // if
  mContext.mFunc->GetDeclaration()->AddParameter(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboParameter

// //////////////////////////
// Context functions
// //////////////////////////
//! \todo M Design: I do not think we really need a statement context.
IRBuilder& IRBuilder::
SetStmtContext(IRStmt* stmt_, bool isBefore_) {
  BMDEBUG1("IRBuilder::SetStmtContext");
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  NOTIMPLEMENTED(L);
  EMDEBUG0();
  return *this;
} // IRBuilder::SetStmtContext

//! \return Returns the previous function context.
IRFunction* IRBuilder::
SetFuncContext(IRFunction* func_) {
  BMDEBUG1("IRBuilder::SetFuncContext");
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  swap(func_, mContext.mFunc);
  EMDEBUG0();
  return func_;
} // IRBuilder::SetFuncContext

IRFunction* IRBuilder::
GetFuncContext() const {
  BMDEBUG1("IRBuilder::GetFuncContext");
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  EMDEBUG0();
  return mContext.mFunc;
} // IRBuilder::GetFuncContext

//! \return Returns the previous module context.
IRModule* IRBuilder::
SetModuleContext(IRModule* module_) {
  BMDEBUG1("IRBuilder::SetModuleContext");
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  swap(mContext.mModule, module_);
  EMDEBUG1(module_);
  return module_;
} // IRBuilder::SetModuleContext

IRModule* IRBuilder::
GetModuleContext() const {
  BMDEBUG1("IRBuilder::GetModuleContext");
  REQUIREDMSG(this != &Singleton<IRBuilder>::Obj(),
    ieStrParamValuesDBCViolation << ieStrDBCSingletonUseContext);
  EMDEBUG1(mContext.mModule);
  return mContext.mModule;
} // IRBuilder::GetModuleContext

void IRBuilder::
updateDynamicJumps(IRFunction* func_) const {
  BMDEBUG2("IRBuilder::updateDynamicJumps", func_);
  REQUIREDMSG(func_ != 0, ieStrNonNullParam);
  // All dynamic Jumps need update.
  vector<IRSDynJump*> lDynJumps;
  if (func_->GetDynJumps(lDynJumps) == true) {
    for (hFUInt32 c = 0; c < lDynJumps.size(); ++ c) {
      IRBB* lBB(lDynJumps[c]->GetBB());
      irbRemoveStmt(lDynJumps[c]);
      irbInsertStmt(lDynJumps[c], ICBB(lBB, ICEndOfBB));
    } // for
  } // if
  EMDEBUG0();
  return;
} // IRBuilder::updateDynamicJumps

//! \note \p stmt is no longer valid after removal, CFG is up to date.
//! \todo L Design: check if we can do the cfg update in IRBB remove statement.
void IRBuilder::
irbRemoveStmt(IRStmt* stmt_) const {
  BMDEBUG2("IRBuilder::irbRemoveStmt", stmt_);
  REQUIRED(!pred.pIsAfterLastOfBB(stmt_));
  REQUIRED(!pred.pIsBeforeFirstOfBB(stmt_));
  REQUIRED_VALID(stmt_);
  IRBB* lBB(stmt_->GetBB());
  IRFunction* lFunc(extr.eGetFunc(stmt_));
  { // Invalidate DU-UD chains
    CFG* lCFG(extr.eGetCFG(stmt_));
    lCFG->mDUUDChains = 0;
  } // block
  if (pred.pIsCFStmt(stmt_)) {
    smirbOnEdgeChange(lBB->GetCFG());
  } // if
  lBB->RemoveStmt(stmt_);
  if (pred.pIsAddrTakenLabel(stmt_) == true) {
    lFunc->RemoveAddrTakenLabel(static_cast<IRSLabel*>(stmt_));
  } else if (pred.pIsDynJump(stmt_) == true) {
    lFunc->RemoveDynJump(static_cast<IRSDynJump*>(stmt_));
  } // if
  EMDEBUG0();
  return;
} // IRBuilder::irbRemoveStmt

//! \todo M Design: Check if prolog/epilogs are paired.
//! \todo H Design: Add that prolog must be the first statement of a basic block.
IRStmt* IRBuilder::
irbInsertStmt(IRStmt* stmt_, const InsertContext& ic_) const {
  BMDEBUG3("IRBuilder::irbInsertStmt", stmt_, &ic_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  REQUIRED_VALIDMSG(&ic_, ieStrObjectNotValid);
  REQUIREDMSG(ic_.GetBB()->GetCFG() != 0, ieStrInconsistentInternalStructure);
  // epilog must go to the end of basic blocks.
  REQUIRED(!pred.pIsEpilog(stmt_) || pred.pIsEndOfBB(ic_) == true);
  // exit basic blocks may only contain a single epilog.
  REQUIRED(!pred.pIsExitBB(ic_.GetBB()) || pred.pIsEpilog(stmt_) == true);
  REQUIRED(!pred.pIsExitBB(ic_.GetBB()) || pred.pIsEmpty(ic_.GetBB()) == true);
  // epilog must be the last statement of its basic block.
  REQUIRED(pred.pIsEmpty(ic_.GetBB()) == true || !pred.pIsEpilog(ic_.GetBB()->GetLastStmt()));
  // do not let insertion before a prolog statement
  REQUIRED(!pred.pIsBegOfBB(ic_) || pred.pIsEmpty(ic_.GetBB()) == true || !pred.pIsProlog(ic_.GetBB()->GetFirstStmt()));
  IRBB* lICBB(ic_.GetBB());
  CFG* lCFG(lICBB->GetCFG());
  // Invalidate DU-UD chains
  lCFG->mDUUDChains = 0;
  stmt_->Invalidate(); // invalidate so that we check at the end if it is valid.
  if (pred.pIsCFStmt(stmt_) == true) {
    smirbOnEdgeChange(lCFG);
    // Need to update CFG. There are 3 possible cases:
    // 1. ic is at the end and ic->prev is not a CF statement:
    //    - no need to split bb of stmt
    //    - update succs and preds
    // 2. stmt is at the end and stmt->prev is a CF statement.
    //    - need to split bb of stmt
    //    - need to move stmt in to new bb
    // 3. stmt is not at the end.
    //    - need to split bb of stmt
    //    - need to move statements that are after stmt in to new bb
    if (pred.pIsEndOfBB(ic_) == true && !pred.pHasCFStmt(lICBB)) {
      // case 1. ic at the end but not after CF stmt.
      lICBB->InsertStmt(stmt_, ICBB(lICBB, ICEndOfBB));
    } else if (pred.pIsEndOfBB(ic_) == true && pred.pHasCFStmt(lICBB) == true) {
      // case 2. ic at the end and after CF stmt.
      IRBB* lNewBB(getNewBB(lCFG));
      lNewBB->InsertStmt(stmt_, ICBB(lNewBB, ICBegOfBB));
      updateDynamicJumps(lCFG->GetFunc());
    } else {
      // case 3. ic is not at the end.
      ASSERT(pred.pIsEmpty(lICBB) == false);
      ICStmt lICStmt(extr.eConvToICStmt(&ic_));
      IRStmt* lFromStmt(lICStmt.GetPlace() == ICAfter ? lICStmt.GetStmt()->GetNextStmt() : lICStmt.GetStmt());
      IRBB* lNewBB(getNewBB(lCFG));
      // Note that indirect recursion is safe (here) in this implementation.
      GetHelper()->irbMoveStmts(lFromStmt, lICBB->GetLastStmt(), ICBB(lNewBB, ICEndOfBB));
      lICBB->InsertStmt(stmt_, ICBB(lICBB, ICEndOfBB));
      updateDynamicJumps(lCFG->GetFunc());
    } // if
  } else {
    if (pred.pIsEndOfBB(ic_) == true && pred.pHasCFStmt(lICBB) == true) {
      IRBB* lNewBB(getNewBB(lCFG));
      lNewBB->InsertStmt(stmt_, ICBB(lNewBB, ICEndOfBB));
      updateDynamicJumps(lCFG->GetFunc());
    } else {
      lICBB->InsertStmt(stmt_, ic_);
    } // if
  } // if
  if (pred.pIsDynJump(stmt_) == true) {
    extr.eGetFunc(stmt_)->AddDynJump(static_cast<IRSDynJump*>(stmt_));
  } else if (pred.pIsAddrTakenLabel(stmt_) == true) {
    extr.eGetFunc(stmt_)->AddAddressTakenLabel(static_cast<IRSLabel*>(stmt_));
    // We always need address taken labels at the beginning of a basic block.
    irbSplitBB(ICStmt(stmt_, ICBefore));
  } // if
  ENSURE_VALID(lICBB);
  ENSURE_VALID(stmt_);
  ENSURE_VALID(stmt_->GetBB());
  EMDEBUG1(stmt_);
  return stmt_;
} // IRBuilder::irbInsertStmt

/////////////////////////////
// IRBuilderHelper members
/////////////////////////////

IRBuilderHelper::
IRBuilderHelper(const IRBuilder* parent_) :
  mIRBuilder(parent_)
{
  BMDEBUG2("IRBuilderHelper::IRBuilderHelper", parent_);
  ENSURE_VALID(this);
  EMDEBUG0();
} // IRBuilderHelper::IRBuilderHelper

IRSIf* IRBuilderHelper::
irbsIf(IRExpr* cond_, IRStmt* trueStmt_, IRStmt* falseStmt_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG6("IRBuilderHelper::irbsIf", cond_, trueStmt_, falseStmt_, &ic_, &srcLoc_);
  REQUIRED_VALIDMSG(cond_, ieStrObjectNotValid);
  REQUIRED_VALIDMSG(trueStmt_, ieStrObjectNotValid);
  REQUIRED_VALIDMSG(falseStmt_, ieStrObjectNotValid);
  REQUIRED_VALIDMSG(&ic_, ieStrObjectNotValid);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(cond_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(trueStmt_ != falseStmt_, "True and false case statements must be different");
  REQUIREDMSG(pred.pIsNullIC(&ic_) == false, ieStrParamValuesDBCViolation);
  InsertContext* lIfIC(0);
  bool lTrueSplit(mIRBuilder->irbSplitBB(ICStmt(trueStmt_, ICBefore)));
  bool lFalseSplit(mIRBuilder->irbSplitBB(ICStmt(falseStmt_, ICBefore)));
  // now trueStmt and falseStmt statements are at the beginning of their bbs.
  ASSERTMSG(pred.pIsFirstStmt(trueStmt_) && pred.pIsFirstStmt(falseStmt_),
    ieStrInconsistentInternalStructure);
  //! \todo M Desing: try to reduce the number of basic blocks added to the CFG while inserting CF statements.
  if (!pred.pIsBool(cond_->GetType()) || !pred.pIsCmp(cond_)) {
    //! \todo M Design: Here we unnecessarily create recursive copy find a
    //!       solution to this.
    cond_ = irbeIsTrue(cond_->GetRecCopyExpr());
  } // if
  if (ic_.GetBB() == trueStmt_->GetBB() && pred.pIsBegOfBB(ic_) == true) {
    if (lTrueSplit == true) {
      // Normally we are not allowed to remove statements in insertions,
      // but we know that we have created the Jump due to the split.
      vector<IRBB*> lPreds;
      extr.eGetPredsOfBB(trueStmt_->GetBB(), lPreds);
      mIRBuilder->irbRemoveStmt(lPreds.front()->GetLastStmt());
      lIfIC = new ICBB(lPreds.front(), ICEndOfBB);
      mIRBuilder->updateDynamicJumps(extr.eGetFunc(trueStmt_));
    } else {
      IRBB* lNewBB = mIRBuilder->irbInsertBB(ICBB(trueStmt_->GetBB(), ICBefore));
      lIfIC = new ICBB(lNewBB, ICBegOfBB);
      mIRBuilder->updateDynamicJumps(extr.eGetFunc(trueStmt_));
    } // if
  } else if (ic_.GetBB() == falseStmt_->GetBB() && pred.pIsBegOfBB(ic_) == true) {
    if (lFalseSplit == true) {
      // Normally we are not allowed to remove statements in insertions,
      // but we know that we have created the Jump due to the split.
      vector<IRBB*> lPreds;
      extr.eGetPredsOfBB(falseStmt_->GetBB(), lPreds);
      mIRBuilder->irbRemoveStmt(lPreds.front()->GetLastStmt());
      lIfIC = new ICBB(lPreds.front(), ICEndOfBB);
      mIRBuilder->updateDynamicJumps(extr.eGetFunc(trueStmt_));
    } else {
      IRBB* lNewBB(mIRBuilder->irbInsertBB(ICBB(falseStmt_->GetBB(), ICBefore)));
      lIfIC = new ICBB(lNewBB, ICBegOfBB);
      mIRBuilder->updateDynamicJumps(extr.eGetFunc(trueStmt_));
    } // if
  } // if
  IRSIf* retVal(mIRBuilder->irbsIf(cond_, trueStmt_->GetBB(), falseStmt_->GetBB(), lIfIC == 0 ? ic_ : *lIfIC, srcLoc_));
  //! \todo M Design: I do not like the new and delete use!
  delete lIfIC;
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsIf

IRSIf* IRBuilder::
irbsIf(IRExpr* cond_, IRBB* trueBB_, IRBB* falseBB_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG6("IRBuilder::irbsIf", cond_, trueBB_, falseBB_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(cond_) == true, ieStrExprUsedBefore);
  REQUIREDMSG(cond_ != 0, ieStrNonNullParam);
  REQUIREDMSG(trueBB_ != 0, ieStrNonNullParam);
  REQUIREDMSG(falseBB_ != 0, ieStrNonNullParam);
  REQUIREDMSG(trueBB_ != falseBB_, ieStrParamValuesDBCViolation);
  if (!pred.pIsBool(cond_->GetType()) || !pred.pIsCmp(cond_)) {
    //! \todo M Design: Here we unnecessarily create recursive copy find a solution to this.
    cond_ = GetHelper()->irbeIsTrue(cond_->GetRecCopyExpr());
  } // if
  if (pred.pIsUse(cond_) == true) {
    cond_ = GetHelper()->irbeIsTrue(cond_);
  } // if
  IRSIf* retVal(new IRSIf(cond_, trueBB_, falseBB_, new DIEBase(srcLoc_)));
  if (!pred.pIsNullIC(&ic_)) {
    irbInsertStmt(retVal, ic_);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbsIf

IRBB* IRBuilder::
irbInsertBB(const ICBB& ic_) const {
  BMDEBUG2("IRBuilder::irbInsertBB", &ic_);
  REQUIRED(ic_.GetPlace() == ICAfter || ic_.GetPlace() == ICBefore);
  REQUIRED(ic_.GetPlace() != ICAfter || !pred.pIsExitBB(ic_.GetBB()));
  REQUIRED(ic_.GetPlace() != ICBefore || !pred.pIsEntryBB(ic_.GetBB()));
  IRBB* lICBB(ic_.GetBB());
  IRBB* retVal(getNewBB(lICBB->GetCFG()));
  updateDynamicJumps(extr.eGetFunc(lICBB));
  if (ic_.GetPlace() == ICAfter) {
    // - Last stmt of ic BB should be moved in to retVal.
    // - A succ edge should be inserted from ic BB to retVal.
    if (pred.pHasSucc(lICBB)) {
      ASSERT(pred.pIsEmpty(lICBB) == false);
      GetHelper()->irbMoveStmt(lICBB->GetLastStmt(), ICBB(retVal, ICEndOfBB));
    } // if
    /* LIE */ irbInsertSucc(lICBB, retVal);
  } else {
    // - Remap all predecessors' CF statement targets (pointing to
    //   lICBB) to the retVal. Remove the preds of lICBB.
    // - A succ edge should be inserted from retVal to lICBB.
    /* LIE */ irbRemapPreds(lICBB, retVal);
    /* LIE */ irbInsertSucc(retVal, lICBB);
  } // if
  ENSURE_VALID(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbInsertBB

bool IRBuilder::
irbRemapJumpTarget(IRStmt* stmt_, IRBB* from_, IRBB* to_) const {
  REQUIREDMSG(stmt_ != 0 && from_ != 0 && to_ != 0, ieStrNonNullParam);
  REQUIREDMSG(pred.pIsCFStmt(stmt_) == true, "Must be a control flow statement");
  REQUIREDMSG(!pred.pIsReturn(stmt_), "Cannot change the target of return statement");
  IRBB* lBBOfStmt(stmt_->GetBB());
  smirbOnEdgeChange(lBBOfStmt->GetCFG());
  irbRemoveStmt(stmt_);
  bool retVal(stmt_->remapJumpTargets(from_, to_));
  irbInsertStmt(stmt_, ICBB(lBBOfStmt, ICEndOfBB));
  //! \todo M Design: write a predicate function for target checks, and put
  //!       this as requirement not ensure.
  ENSUREMSG(retVal == true, "'From' must be a jump target of 'aStmt'");
  return retVal;
} // IRBuilder::irbRemapJumpTarget

bool IRBuilder::
irbRemapPreds(IRBB* from_, IRBB* to_) const {
  BMDEBUG3("IRBuilder::irbRemapPreds", from_, to_);
  REQUIRED_VALID(from_);
  REQUIRED_VALID(to_);
  REQUIRED(pred.pHasReturn(extr.eGetFunc(from_)) == false || !pred.pIsExitBB(from_));
  vector<IRBB*> lPreds;
  extr.eGetPredsOfBB(from_, lPreds);
  for (hFUInt16 c(0); c < lPreds.size(); ++ c) {
    IRStmt* lCFStmt(lPreds[c]->GetLastStmt());
    ASSERT(pred.pIsCFStmt(lCFStmt) == true);
    irbRemoveStmt(lCFStmt);
    lCFStmt->remapJumpTargets(from_, to_);
    irbInsertStmt(lCFStmt, ICBB(lPreds[c], ICEndOfBB));
  } // for
  bool retVal(!lPreds.empty());
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbRemapPreds

void IRBuilderHelper::
irbMoveStmt(IRStmt* stmt_, const InsertContext& ic_) const {
  BMDEBUG3("IRBuilderHelper::irbMoveStmt", stmt_, &ic_);
  REQUIREDMSG(pred.pIsNullIC(&ic_) == false, ieStrParamValuesDBCViolation);
  REQUIRED_VALID(stmt_);
  mIRBuilder->irbRemoveStmt(stmt_);
  mIRBuilder->irbInsertStmt(stmt_, ic_);
  EMDEBUG0();
  return;
} // IRBuilderHelper::irbMoveStmt

IRSLabel* IRBuilderHelper::
irbsLocalLabel(const string& prefix_, bool isAddrTaken_, const InsertContext& ic_) const {
  BMDEBUG3("IRBuilderHelper::irbsLocalLabel", prefix_, &ic_);
  REQUIREDMSG(pred.pIsNullIC(&ic_) == false, ieStrParamValuesDBCViolation);
  static hFUInt32 lLabelId;
  char lFmt[16];
  sprintf(lFmt, "_%d", lLabelId ++);
  IRSLabel* retVal(mIRBuilder->irbsLabel(prefix_+lFmt, isAddrTaken_, ic_, SrcLoc()));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsLocalLabel

IREAShiftRight* IRBuilderHelper::
irbeAShiftRight(IRExpr* expr_, hUInt16 numBits_) const {
  BMDEBUG3("IRBuilderHelper::irbeAShiftRight", expr_, numBits_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IREAShiftRight* retVal(mIRBuilder->irbeAShiftRight(expr_, 
    mIRBuilder->irbeIntConst(static_cast<IRTInt*>(expr_->GetType()), numBits_)));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbeAShiftRight

IRELShiftRight* IRBuilderHelper::
irbeLShiftRight(IRExpr* expr_, hUInt16 numBits_) const {
  BMDEBUG3("IRBuilderHelper::irbeLShiftRight", expr_, numBits_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IRELShiftRight* retVal(mIRBuilder->irbeLShiftRight(expr_, 
    mIRBuilder->irbeIntConst(static_cast<IRTInt*>(expr_->GetType()),numBits_)));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbeLShiftRight

IREShiftLeft* IRBuilderHelper::
irbeShiftLeft(IRExpr* expr_, hUInt16 numBits_) const {
  BMDEBUG1("IRBuilderHelper::irbeShiftLeft");
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IREShiftLeft* retVal(mIRBuilder->irbeShiftLeft(expr_, 
    mIRBuilder->irbeIntConst(static_cast<IRTInt*>(expr_->GetType()), numBits_)));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbeShiftLeft

IREDeref* IRBuilderHelper::
irbeUseOf(IRExpr* expr_) const {
  BMDEBUG2("IRBuilderHelper::irbeUseOf", expr_);
  IREDeref* retVal(mIRBuilder->irbeDeref(mIRBuilder->irbeAddrOf(expr_)));
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbeUseOf

IREDeref* IRBuilderHelper::
irbeUseOf(IRObject* object_) const {
  BMDEBUG2("IRBuilderHelper::irbeUseOf", object_);
  IREDeref* retVal(mIRBuilder->irbeDeref(mIRBuilder->irbeAddrOf(object_)));
  ENSURE_VALIDMSG(retVal, ieStrMustEnsureValid);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbeUseOf

//! \todo M Design: Can we relax below pre? I guess so.
hFUInt32 IRBuilderHelper::
irbMoveStmts(IRStmt* from_, IRStmt* to_, const InsertContext& ic_) const {
  BMDEBUG4("IRBuilderHelper::irbMoveStmts", from_, to_, &ic_);
  REQUIRED_VALID(from_);
  REQUIRED_VALID(to_);
  REQUIRED_VALID(&ic_);
  REQUIRED(!pred.pIsBeforeFirstOfBB(from_) && !pred.pIsAfterLastOfBB(to_));
  REQUIRED(from_->GetBB() == to_->GetBB());
  REQUIRED(from_ == to_ || pred.pIsBeforeInBB(from_, to_) == true);
  REQUIRED(!pred.pIsICStmt(ic_) || !pred.GetDBCHelper()->pIsInStmtsRange(from_, to_, extr.eGetStmt(ic_)));
  REQUIREDMSG(pred.pIsNullIC(&ic_) == false, ieStrParamValuesDBCViolation);
  hFUInt32 retVal(0);
  IRBB* lSrcBB(from_->GetBB());
  if (from_ == to_) {
    irbMoveStmt(from_, ic_);
    ++ retVal;
  } else {
    //! \todo L Design: check if we can move statements across functions.
    ICStmt lMoveIC(ICInvalid);
    // set up the lMoveIC, it must be an ICBefore context.
    // For that move the \p to first.
    IRStmt* lEndStmt(extr.eGetNextStmt(to_));
    irbMoveStmt(to_, ic_);
    lMoveIC = ICStmt(to_, ICBefore);
    ++ retVal;
    IRStmt* lCurrStmt(from_);
    while (lCurrStmt != lEndStmt) {
      IRStmt* lNextStmt(extr.eGetNextStmt(lCurrStmt));
      irbMoveStmt(lCurrStmt, lMoveIC);
      ++ retVal;
      lCurrStmt = lNextStmt;
    } // while
  } // if
  ENSURE_VALID(lSrcBB);
  ENSURE_VALID(ic_.GetBB());
  ENSURE_VALID(lSrcBB->GetCFG());
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbMoveStmts

IROBitField* IRBuilder::
irboBitField(const string& name_, IRType* type_, tBigInt offset_, hFSInt16 bitOffset_, hFSInt16 sizeInBits_, IRSignedness sign_) const {
  BMDEBUG7("IRBuilder::irboField", name_, type_, &offset_, bitOffset_, sizeInBits_, sign_);
  REQUIRED_VALID(type_);
  REQUIRED(offset_ >= 0);
  REQUIRED(name_.length() != 0);
  REQUIRED(sizeInBits_ >= 0);
  REQUIRED(sign_ == IRSSigned || sign_ == IRSUnsigned);
  IROBitField* retVal(new IROBitField(name_, type_, offset_, bitOffset_, sizeInBits_, sign_));
  if (options.obGet("debuginfo") == true) {
    retVal->diSetDIE(smDIEBuilder->CreateObjDIE(retVal));
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboBitField

IROField* IRBuilder::
irboField(const string& name_, IRType* type_, tBigInt offset_) const {
  BMDEBUG4("IRBuilder::irboField", name_, type_, &offset_);
  REQUIRED_VALID(type_);
  REQUIRED(offset_ >= 0);
  REQUIRED(name_.length() != 0);
  IROField* retVal(new IROField(name_, type_, offset_));
  if (options.obGet("debuginfo") == true) {
    retVal->diSetDIE(smDIEBuilder->CreateObjDIE(retVal));
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irboField

bool IRBuilder::
IsValid() const {
  BMDEBUG1("IRBuilder::IsValid");
  // check host types
  const int lHostBitsPerByte(8);
  REQUIRED(sizeof(hUInt8)*lHostBitsPerByte >= 8);
  REQUIRED(sizeof(hSInt8)*lHostBitsPerByte >= 8);
  REQUIRED(sizeof(hUInt16)*lHostBitsPerByte >= 16);
  REQUIRED(sizeof(hSInt16)*lHostBitsPerByte >= 16);
  REQUIRED(sizeof(hUInt32)*lHostBitsPerByte >= 32);
  REQUIRED(sizeof(hSInt32)*lHostBitsPerByte >= 32);
  REQUIRED(sizeof(hFInt8)*lHostBitsPerByte >= 8);
  REQUIRED(sizeof(hFInt16)*lHostBitsPerByte >= 16);
  REQUIRED(sizeof(hFInt32)*lHostBitsPerByte >= 32);
  REQUIRED(sizeof(hFUInt8)*lHostBitsPerByte >= 8);
  REQUIRED(sizeof(hFUInt16)*lHostBitsPerByte >= 16);
  REQUIRED(sizeof(hFUInt32)*lHostBitsPerByte >= 32);
  REQUIRED(sizeof(hFSInt8)*lHostBitsPerByte >= 8);
  REQUIRED(sizeof(hFSInt16)*lHostBitsPerByte >= 16);
  REQUIRED(sizeof(hFSInt32)*lHostBitsPerByte >= 32);
  hUInt8 hUInt8_(-1); REQUIRED(hUInt8_ * -1.0 < 0);
  hSInt8 hSInt8_(-1); REQUIRED(hSInt8_ * -1.0 > 0);
  hUInt16 hUInt16_(-1); REQUIRED(hUInt16_ * -1.0 < 0);
  hSInt16 hSInt16_(-1); REQUIRED(hSInt16_ * -1.0 > 0);
  hUInt32 hUInt32_(-1); REQUIRED(hUInt32_ * -1.0 < 0);
  hSInt32 hSInt32_(-1); REQUIRED(hSInt32_ * -1.0 > 0);
  hFUInt8 hFUInt8_(-1); REQUIRED(hFUInt8_ * -1.0 < 0);
  hFSInt8 hFSInt8_(-1); REQUIRED(hFSInt8_ * -1.0 > 0);
  hFUInt16 hFUInt16_(-1); REQUIRED(hFUInt16_ * -1.0 < 0);
  hFSInt16 hFSInt16_(-1); REQUIRED(hFSInt16_ * -1.0 > 0);
  hFUInt32 hFUInt32_(-1); REQUIRED(hFUInt32_ * -1.0 < 0);
  hFSInt32 hFSInt32_(-1); REQUIRED(hFSInt32_ * -1.0 > 0);
  EMDEBUG0();
  return true;
} // IRBuilder::IsValid

IRExpr* IRBuilder::
irbeInverseCmp(const IRExpr* expr_) const {
  BMDEBUG2("IRBuilder::irbeInverseCmp", expr_);
  REQUIREDMSG(pred.pIsCmp(expr_) == true || pred.pIsBoolConst(expr_) == true, ieStrParamValuesDBCViolation);
  IRExpr* retVal(0);
  if (const IREBoolConst* lBoolConst = dynamic_cast<const IREBoolConst*>(expr_)) {
    retVal = irbeBoolConst(!lBoolConst->GetValue());
  } else {
    retVal = static_cast<const IRECmp*>(expr_)->GetInverse();
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeInverseCmp

IRTReal* IRBuilder::
irbtGetReal(hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_) const {
  BMDEBUG4("IRBuilder::irbtGetReal", mantissa_, exponent_, alignment_);
  REQUIRED(pred.pTargetHasReal(mantissa_, exponent_, alignment_));
  union {
    struct {
      hUInt32 mantissa : 16;
      hUInt32 exponent : 16;
      hUInt32 alignment : 16;
    } in;
    hUInt64 key;
  } lKey;
  lKey.key = 0; // Clear off all the bits.
  lKey.in.mantissa = mantissa_;
  lKey.in.alignment = alignment_;
  lKey.in.exponent = exponent_;
  IRTReal* retVal(0);
  if (smRealTypes.find(lKey.key) != smRealTypes.end()) {
    retVal = smRealTypes[lKey.key];
  } else {
    tReal lReal(*Target::GetTarget()->GetRealType(mantissa_, exponent_, alignment_));
    retVal = new IRTReal(lReal.GetSize(), lReal.GetAlignment(), lReal.GetMantissa(), lReal.GetExponent());
    smRealTypes[lKey.key] = retVal;
    smAllTypes.push_back(retVal);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbtGetReal

IRTInt* IRBuilder::
irbtGetInt(hUInt16 size_, hUInt16 alignment_, IRSignedness sign_) const {
  BMDEBUG4("IRBuilder::irbtGetInt", size_, alignment_, sign_);
  REQUIRED(pred.pTargetHasInt(size_, alignment_, sign_));
  //! \todo M CodingStyle: Correct the coding style.
  union {
    struct {
      hUInt32 size : 16;
      hUInt32 alignment : 16;
      hUInt32 sign : 2;
    } in;
    hUInt64 key;
  } lKey;
  lKey.key = 0; // Clear off all the bits.
  lKey.in.size = size_;
  lKey.in.alignment = alignment_;
  lKey.in.sign = sign_;
  IRTInt* retVal(0);
  if (smIntTypes.find(lKey.key) != smIntTypes.end()) {
    retVal = smIntTypes[lKey.key];
  } else {
    //! \todo M Design: try to make the IR types unique, e.g. keep them in a set.
    tInt lInt(*Target::GetTarget()->GetIntType(size_, alignment_, sign_));
    retVal = new IRTInt(lInt.GetSize(), lInt.GetAlignment(), lInt.GetSign());
    smIntTypes[lKey.key] = retVal;
    smAllTypes.push_back(retVal);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbtGetInt

IRBuilder::
IRBuilder(IRProgram* prog_, bool doOptims_) :
  mDoOptims(doOptims_),
  mIRHBuilder(new IRBuilderHelper(this))
{
  BMDEBUG2("IRBuilder::IRBuilder", doOptims_);
  ENSURE_VALID(this);
  EMDEBUG0();
} // IRBuilder::IRBuilder

IRBuilder::
IRBuilder() :
  mDoOptims(consts.cIRBDoOptims),
  mIRHBuilder(new IRBuilderHelper(this))
{
  BMDEBUG1("IRBuilder::IRBuilder");
  ENSURE_VALID(this);
  EMDEBUG0();
} // IRBuilder::IRBuilder

IRTBool* IRBuilder::
irbtGetBool() const {
  static bool lFirstTime(true);
  if (lFirstTime == true) {
    smAllTypes.push_back(&Singleton<IRTBool>::Obj());
  } // if
  lFirstTime = false;
  return &Singleton<IRTBool>::Obj();
} // IRBuilder::irbtGetBool

IRTVoid* IRBuilder::
irbtGetVoid() const {
  static bool lFirstTime(true);
  if (lFirstTime == true) {
    smAllTypes.push_back(&Singleton<IRTVoid>::Obj());
  } // if
  lFirstTime = false;
  return &Singleton<IRTVoid>::Obj();
} // IRBuilder::irbtGetVoid

IRTUnion* IRBuilder::
irbtGetUnion(const string& name_, const vector<IROField*>& fields_, tBigInt size_, hUInt16 alignment_) const {
  IRTUnion* retVal(new IRTUnion(name_, fields_, size_, alignment_));
  smUnionTypes.push_back(retVal);
  smAllTypes.push_back(retVal);
  return retVal;
} // IRBuilder::irbtGetUnion

IRTStruct* IRBuilder::
irbtGetStruct(const string& name_, const vector<IROField*>& fields_, tBigInt size_, hUInt16 alignment_) const {
  IRTStruct* retVal(new IRTStruct(name_, fields_, size_, alignment_));
  smStructTypes.push_back(retVal);
  smAllTypes.push_back(retVal);
  return retVal;
} // IRBuilder::irbtGetStruct

IRTArray* IRBuilder::
irbtGetArray(IRType* elemType_, tBigInt count_, hUInt16 alignment_) const {
  BMDEBUG4("IRBuilder::irbtGetArray", elemType_, &count_, alignment_);
  IRTArray* retVal(new IRTArray(elemType_, -1 , -1, count_, alignment_));
  smArrayTypes.push_back(retVal);
  smAllTypes.push_back(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbtGetArray

IRTFunc* IRBuilder::
irbtGetFunc(IRType* retType_,
  const vector<IRType*>& paramTypes_,
  bool hasEllipses_,
  CallingConvention* callConv_) const
{
  return new IRTFunc(retType_, paramTypes_, hasEllipses_, callConv_);
} // IRBuilder::irbtGetFunc

IRExpr* IRBuilderHelper::
irbeIsFalse(IRExpr* expr_) const {
  BMDEBUG2("IRBuilderHelper::irbeIsFalse", expr_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IRExpr* retVal(mIRBuilder->irbeEq(expr_, irbeGetIntConst_Zero()));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbeIsFalse

IRExpr* IRBuilderHelper::
irbeIsTrue(IRExpr* expr_) const {
  BMDEBUG2("IRBuilderHelper::irbeIsTrue", expr_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsExprNeverUsed(expr_) == true, ieStrExprUsedBefore);
  IRExpr* retVal(mIRBuilder->irbeNe(expr_, irbeGetIntConst_Zero()));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbeIsTrue

void IRBuilder::
smirbAttachLabel(const IR* ir_, const string& tag_, const string& label_) {
  REQUIREDMSG(ir_ != 0 || !tag_.empty(), ieStrParamValuesDBCViolation);
  smTaggedLabels[make_pair(ir_, tag_)] = label_;
} // IRBuilder::smirbAttachLabel

const string& IRBuilder::
smirbGetAttachedLabel(const IR* ir_, const string& tag_) {
  BDEBUG3("IRBuilder::smirbGetAttachedLabel", ir_, tag_);
  REQUIREDMSG(smTaggedLabels.find(make_pair(ir_, tag_)) != smTaggedLabels.end(), ieStrParamValuesDBCViolation);
  const string& retVal(smTaggedLabels[make_pair(ir_, tag_)]);
  EDEBUG1(retVal);
  return retVal;
} // IRBuilder::smirbGetAttachedLabel

void IRBuilder::
smSetDIEBuilder(DIEBuilder* dieBuilder_) {
  BDEBUG2("IRBuilder::smSetDIEBuilder", dieBuilder_);
  REQUIREDMSG(dieBuilder_ != 0, ieStrParamValuesDBCViolation);
  smDIEBuilder = dieBuilder_;
  EDEBUG0();
  return;
} // IRBuilder::smSetDIEBuilder

void IRBuilder::
irbSetInits(IROGlobal* global_, IRExpr* inits_) const {
  BMDEBUG3("IRBuilder::irbSetInits", global_, inits_);
  REQUIREDMSG(global_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(inits_ != 0, ieStrParamValuesDBCViolation);
  if (!pred.pIsInvalid(global_->mConstInits)) {
    smirbDetached(global_->mConstInits);
  } // if
  global_->mConstInits = inits_;
  inits_->SetParent(global_);
  smirbAttached(inits_);
  if (pred.pIsIncomplete(global_->GetType()) == true) {
    if (IRTArray* lArrType = dynamic_cast<IRTArray*>(global_->GetType())) {
      lArrType->mCount = (extr.eGetTotalTypeSize(inits_) / lArrType->GetElemType()->GetSize()) + 1;
    } else if (IRTStruct* lStrType = dynamic_cast<IRTStruct*>(global_->GetType())) {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
  } // if
  EMDEBUG0();
  return;
} // IRBuilder::irbSetInits

IRSIf* IRBuilderHelper::
irbsConvertToIf(IRExpr* cond_, IRBB* bb_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilderHelper::irbsConvertToIf", cond_, bb_, &srcLoc_);
  REQUIREDMSG(cond_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(extr.eGetNumOfSuccs(bb_) == 1, ieStrParamValuesDBCViolation <<
    " successor count must be 1 not " << extr.eGetNumOfSuccs(bb_));
  IRBB* lSucc(extr.eGetSucc(bb_));
  IRBB* lTrueCaseBB(mIRBuilder->irbInsertBB(ICBB(bb_, ICAfter)));
  IRBB* lFalseCaseBB(mIRBuilder->irbInsertBB(ICBB(bb_, ICAfter)));
  mIRBuilder->irbRemoveStmt(bb_->GetLastStmt());
  IRSIf* retVal(mIRBuilder->irbsIf(cond_, lTrueCaseBB, lFalseCaseBB, ICBB(bb_, ICEndOfBB), srcLoc_));
  mIRBuilder->irbRemoveStmt(lFalseCaseBB->GetLastStmt());
  mIRBuilder->irbInsertSucc(lFalseCaseBB, lSucc);
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsConvertToIf

void IRBuilder::
smirbDetached(IRExpr* expr_) {
  BDEBUG2("IRBuilder::smirbDetached", expr_);
  REQUIREDMSG(pred.pIsInvalid(expr_) == true || pred.pIsTopNodeObj(expr_) == true || 
    pred.pIsNonTerminal(extr.eGetTopNodeExpr(expr_)) == true ||
    pred.pIsDetached(expr_) == false, ieStrParamValuesDBCViolation);
  // Keep the expressions related analyses current.
  { // Check for address taken cases.
    // LIE: we can leave this unhandled, we would miss optimization
    // opportunities but not cause wrong code generation.
  } // block
  { // Invalidate DU-UD chains
    if (!pred.pIsInvalid(expr_) && !pred.pIsTopNodeObj(expr_) && !pred.pIsNonTerminal(extr.eGetTopNodeExpr(expr_))) {
      extr.eGetCFG(expr_)->mDUUDChains = 0;
    } // if
  } // block
  EDEBUG0();
} // IRBuilder::smirbDetached

void IRBuilder::
smsetAssocGroup(const IRExpr* src_, IRExpr* dst_) {
  BDEBUG3("IRBuilder::smsetAssocGroup", src_, dst_);
  REQUIREDMSG(src_ != 0 || dst_ != 0, ieStrNonNullParam);
  if (pred.pIsSameExprClass(src_, dst_) == true) {
    dst_->mAssocGroupNo = src_->mAssocGroupNo;
    smsetAssocGroup(src_, static_cast<IREBinary*>(dst_)->GetLeftExpr());
    smsetAssocGroup(src_, static_cast<IREBinary*>(dst_)->GetRightExpr());
  } else if (pred.pIsAddOrSub(src_) == true && pred.pIsAddOrSub(dst_) == true) {
    dst_->mAssocGroupNo = src_->mAssocGroupNo;
    smsetAssocGroup(src_, static_cast<IREBinary*>(dst_)->GetLeftExpr());
    smsetAssocGroup(src_, static_cast<IREBinary*>(dst_)->GetRightExpr());
  } // if
  EDEBUG0();
  return;
} // IRBuilder::smsetAssocGroup

void IRBuilder::
smirbAttached(IRExpr* expr_) {
  BDEBUG2("IRBuilder::smirbAttached", expr_);
  // Keep the expressions related analyses current.
  { // Check for address taken cases.
    if (pred.pIsAddrOf(expr_) == true && pred.pIsStmt(expr_->GetParent()) == true && pred.pIsAssign(expr_->GetParentStmt()) == true) {
      IREAddrOf* lAddrOf(static_cast<IREAddrOf*>(expr_));
      if (IRObject* lObj = dynamic_cast<IRObject*>(lAddrOf->GetExpr())) {
        lObj->setAddrTaken(consts.cAddrNotTaken);
      } // if
    } // if
  } // block
  { // Keep the mAssocGroupNo current.
    if (pred.pIsParentExpr(expr_) == true && (pred.pIsAssociative(expr_, expr_->GetParentExpr()) == true || 
      pred.pIsAddOrSub(expr_, expr_->GetParentExpr()) == true)) {
      smsetAssocGroup(expr_->GetParentExpr(), expr_);
    } // if
  } // block
  { // Invalidate DU-UD chains
    if (!pred.pIsInvalid(expr_) && !pred.pIsTopNodeObj(expr_) && !pred.pIsNonTerminal(extr.eGetTopNodeExpr(expr_))) {
      extr.eGetCFG(expr_)->mDUUDChains = 0;
    } // if
  } // block
  EDEBUG0();
} // IRBuilder::smirbAttached

IRExpr* IRBuilder::
smirbOnReplaceWith(const IRExpr* old_, IRExpr* new_) {
  BDEBUG3("IRBuilder::smirbOnReplaceWith", old_, new_);
  IRExpr* retVal(new_);
  IRStmt* lParent(dynamic_cast<IRStmt*>(old_->GetParent()));
  { // Invalidate DU-UD chains
    //! \todo M Design: We may want to keep the DUUD up-to-date.
    if (!pred.pIsInvalid(old_) && !pred.pIsTopNodeObj(old_) && !pred.pIsNonTerminal(extr.eGetTopNodeExpr(old_))) {
      extr.eGetCFG(old_)->mDUUDChains = 0;
    } // if
  } // block
  if (lParent != 0 && pred.pIsIf(lParent) == true) {
    if (pred.pIsUse(new_) == true || pred.pIsBoolConst(new_) == true) {
      retVal = irhBuilder->irbeIsTrue(new_);
    } // if
  } // if
  EDEBUG1(retVal);
  return retVal;
} // IRBuilder::smirbOnReplaceWith

void IRBuilder::
smclearDFASets(CFG* cfg_) {
  BDEBUG2("IRBuilder::smclearDFASets", cfg_);
  ListIterator<IRBB*> lBBIter(cfg_->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    lBBIter->mDFASets.clear();
  } // for
  EDEBUG0();
  return;
} // IRBuilder::smclearDFASets

void IRBuilder::
sminvalidateDominators(CFG* cfg_) {
  BDEBUG2("IRBuilder::sminvalidateDominators", cfg_);
  cfg_->mDomTree = 0;
  cfg_->mPDomTree = 0;
  EDEBUG0();
  return;
} // IRBuilder::sminvalidateDominators

IRBB* IRBuilder::
getNewBB(CFG* cfg_) const {
  BMDEBUG2("IRBuilder::getNewBB", cfg_);
  IRBB* retVal(new CGBB(cfg_));
  sminvalidateDominators(cfg_);
  smclearDFASets(cfg_);
  // Invalidate the loops.
  cfg_->mLoops = 0;
  cfg_->mDUUDChains = 0;
  STATRECORD("irbuilder: bb count");
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::getNewBB

void IRBuilder::
smirbOnEdgeChange(CFG* cfg_) {
  BDEBUG2("IRBuilder::smirbOnEdgeChange", cfg_);
  sminvalidateDominators(cfg_);
  smclearDFASets(cfg_);
  // Invalidate the loops.
  cfg_->mLoops = 0;
  cfg_->mDUUDChains = 0;
  STATRECORD("irbuilder: edge change");
  EDEBUG0();
  return;
} // IRBuilder::smirbOnEdgeChange

IROTmp* IRBuilderHelper::
irbNewDef(const string& namePrefix_, IRExpr* expr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilderHelper::irbNewDef", namePrefix_, expr_, &ic_);
  REQUIREDMSG(pred.pIsNullIC(&ic_) == false, ieStrParamValuesDBCViolation);
  IROTmp* retVal(mIRBuilder->irboTmp(expr_->GetType(), namePrefix_));
  /* LIE */ irbsAssign(retVal, expr_, ic_, srcLoc_);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbNewDef

IRExpr* IRBuilder::
irbeIdentity(const IRExpr* expr_) const {
  BMDEBUG2("IRBuilder::ireIdentity", expr_);
  REQUIREDMSG(pred.pIsAddOrSub(expr_) || pred.pIsMulOrDiv(expr_) || pred.pIsShift(expr_) ||
    pred.pIsLAndOr(expr_) || pred.pIsBAndOr(expr_) || pred.pIsBXOr(expr_), ieStrParamValuesDBCViolation);
  IRExpr* retVal(0);
  if (pred.pIsMulOrDiv(expr_) == true) {
    if (!pred.pIsReal(expr_->GetType())) {
      retVal = irbeIntConst(static_cast<IRTInt*>(expr_->GetType()), 1);
    } else {
      retVal = irbeRealConst(static_cast<IRTReal*>(expr_->GetType()), 1.0);
    } // if
  } else if (pred.pIsLAnd(expr_) == true) {
    retVal = irbeBoolConst(true);
  } else if (pred.pIsLOr(expr_) == true) {
    retVal = irbeBoolConst(false);
  } else if (pred.pIsBAnd(expr_) == true) {
    retVal = irbeIntConst(static_cast<IRTInt*>(expr_->GetType()), tBigInt::getTop());
  } else {
    ASSERTMSG(pred.pIsInt(expr_->GetType()) == true, ieStrInconsistentInternalStructure);
    retVal = irbeIntConst(static_cast<IRTInt*>(expr_->GetType()), 0);
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeIdentity

IRExpr* IRBuilder::
irbeFromBinary(const IREBinary* prototype_, IRExpr* left_, IRExpr* right_) const {
  BMDEBUG4("IRBuilder::ireFromBinary", prototype_, left_, right_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(prototype_, left_, right_) == true, ieStrParamValuesDBCViolation);
  IRExpr* retVal(0);
  if (pred.pIsMul(prototype_) == true) {
    retVal = irbeMul(left_, right_);
  } else if (pred.pIsDiv(prototype_) == true) {
    retVal = irbeDiv(left_, right_);
  } else if (pred.pIsAdd(prototype_) == true) {
    retVal = irbeAdd(left_, right_);
  } else if (pred.pIsSub(prototype_) == true) {
    retVal = irbeSub(left_, right_);
  } else if (pred.pIsBAnd(prototype_) == true) {
    retVal = irbeBAnd(left_, right_);
  } else if (pred.pIsBOr(prototype_) == true) {
    retVal = irbeBOr(left_, right_);
  } else if (pred.pIsBXOr(prototype_) == true) {
    retVal = irbeBXOr(left_, right_);
  } else if (pred.pIsLAnd(prototype_) == true) {
    retVal = irbeLAnd(left_, right_);
  } else if (pred.pIsLOr(prototype_) == true) {
    retVal = irbeLOr(left_, right_);
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeFromBinary

IRExpr* IRBuilder::
irbeZero(IRType* type_) const {
  BMDEBUG2("IRBuilder::irbeZero", type_);
  IRExpr* retVal(0);
  if (pred.pIsInt(type_) == true) {
    retVal = irbeIntConst(static_cast<IRTInt*>(type_), 0);
  } else if (pred.pIsBool(type_) == true) {
    retVal = irbeBoolConst(false);
  } else if (pred.pIsPtr(type_) == true) {
    retVal = irbeAddrConst(type_, 0);
  } else if (pred.pIsReal(type_) == true) {
    retVal = irbeRealConst(static_cast<IRTReal*>(type_), 0);
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbeZero

bool IRBuilder::
irbClearToBeRemovedStmts() {
  BMDEBUG1("IRBuilder::irbClearToBeRemovedStmts");
  bool retVal(!mToBeRemovedStmts.empty());
  mToBeRemovedStmts.clear();
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbClearToBeRemovedStmts

bool IRBuilder::
irbClearToBeRemovedBBs() {
  BMDEBUG1("IRBuilder::irbClearToBeRemovedStmts");
  bool retVal(!mToBeRemovedBBs.empty());
  mToBeRemovedBBs.clear();
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbClearToBeRemovedBBs

void IRBuilder::
irbToBeRemoved(IRBB* bb_) {
  BMDEBUG2("IRBuilder::irbToBeRemoved", bb_);
  mToBeRemovedBBs.insert(bb_);
  EMDEBUG0();
  return;
} // IRBuilder::irbToBeRemoved

void IRBuilder::
irbToBeRemoved(IRStmt* stmt_) {
  BMDEBUG2("IRBuilder::irbToBeRemoved", stmt_);
  mToBeRemovedStmts.insert(stmt_);
  EMDEBUG0();
  return;
} // IRBuilder::irbToBeRemoved

void IRBuilder::
irbRemoveToBeRemovedStmts() {
  BMDEBUG1("IRBuilder::irbRemoveToBeRemovedStmts");
  hFUInt32 retVal(mToBeRemovedStmts.size());
  for (set<IRStmt*>::iterator lIt(mToBeRemovedStmts.begin()); lIt != mToBeRemovedStmts.end(); ++ lIt) {
    irbRemoveStmt(*lIt);
  } // for
  mToBeRemovedStmts.clear();
  EMDEBUG1(retVal);
  return;
} // IRBuilder::irbRemoveToBeRemovedStmts

void IRBuilder::
irbRemoveToBeRemovedBBs() {
  BMDEBUG1("IRBuilder::irbRemoveToBeRemovedBBs");
  hFUInt32 retVal(mToBeRemovedBBs.size());
  for (set<IRBB*>::iterator lIt(mToBeRemovedBBs.begin()); lIt != mToBeRemovedBBs.end(); ++ lIt) {
    IRBB* lBB(*lIt);
    lBB->GetCFG()->RemoveBB(lBB);
  } // for
  mToBeRemovedBBs.clear();
  EMDEBUG1(retVal);
  return;
} // IRBuilder::irbRemoveToBeRemovedBBs

template<class T>
hFUInt32 
sfReplaceUses(T* visitable_, IRObject* obj_, IRExpr* newExpr_) {
  BDEBUG4("IRBuilderHelper::irbReplaceUses", visitable_, obj_, newExpr_);
  hFUInt32 retVal(0);
  ExprCollector<IREDeref, T> lExprCollector(EVOTPost, visitable_, ExprVisitorOptions().SetLHSYes());
  for (hFUInt32 c(0); c < lExprCollector.size(); ++ c) {
    if (pred.pIsUse(lExprCollector[c]) == true) {
      if (extr.eGetObjOfUse(lExprCollector[c]) == obj_) {
        /* LIE */ lExprCollector[c]->ReplaceWith(newExpr_->GetRecCopyExpr());
        ++ retVal;
      } // if
    } // if
  } // for
  EDEBUG1(retVal);
  return retVal;
} // sfReplaceUses

hFUInt32 IRBuilderHelper::
irbReplaceUses(IRFunction* func_, IRObject* obj_, IRExpr* newExpr_) const {
  BMDEBUG4("IRBuilderHelper::irbReplaceUses", func_, obj_, newExpr_);
  hFUInt32 retVal(sfReplaceUses(func_->GetCFG(), obj_, newExpr_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbReplaceUses

hFUInt32 IRBuilderHelper::
irbReplaceUses(IRLoop* loop_, IRObject* obj_, IRExpr* newExpr_) const {
  BMDEBUG4("IRBuilderHelper::irbReplaceUses", loop_, obj_, newExpr_);
  hFUInt32 retVal(sfReplaceUses(loop_, obj_, newExpr_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbReplaceUses

hFUInt32 IRBuilderHelper::
irbReplaceUses(IRBB* bb_, IRObject* obj_, IRExpr* newExpr_) const {
  BMDEBUG4("IRBuilderHelper::irbReplaceUses", bb_, obj_, newExpr_);
  hFUInt32 retVal(sfReplaceUses(bb_, obj_, newExpr_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbReplaceUses

hFUInt32 IRBuilderHelper::
irbReplaceUses(IRStmt* stmt_, IRObject* obj_, IRExpr* newExpr_) const {
  BMDEBUG4("IRBuilderHelper::irbReplaceUses", stmt_, obj_, newExpr_);
  hFUInt32 retVal(sfReplaceUses(stmt_, obj_, newExpr_));
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbReplaceUses

IRSIf* IRBuilderHelper::
irbsIf(IRExpr* cond_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
  BMDEBUG4("IRBuilderHelper::irbsIf", cond_, &ic_, &srcLoc_);
  REQUIREDMSG(pred.pIsEndOfBB(ic_) == false, ieStrParamValuesDBCViolation);
  IRSIf* retVal(0);
  if (pred.pIsBegOfBB(ic_) == true) {
    if (extr.eGetNumOfPreds(ic_.GetBB()) == 1) {
      retVal = irbsConvertToIf(cond_, extr.eGetPred(ic_.GetBB()), srcLoc_);
    } else {
      IRBB* lNewBB(mIRBuilder->irbInsertBB(ICBB(ic_.GetBB(), ICBefore)));
      retVal = irbsConvertToIf(cond_, lNewBB, srcLoc_);
    } // if
  } else {
    // Here ic can only be a statement context.
    ASSERTMSG(pred.pIsICStmt(ic_) == true, ieStrInconsistentInternalStructure);
    const ICStmt* lStmtIC(static_cast<const ICStmt*>(&ic_));
    bool lSplit(mIRBuilder->irbSplitBB(*lStmtIC));
    ASSERTMSG(lSplit == true, ieStrInconsistentInternalStructure);
    if (pred.pIsFirstStmt(lStmtIC->GetStmt()) == true) {
      retVal = irbsConvertToIf(cond_, extr.eGetPred(lStmtIC->GetBB()), srcLoc_);
    } else {
      retVal = irbsConvertToIf(cond_, lStmtIC->GetBB(), srcLoc_);
    } // if
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilderHelper::irbsIf

hFUInt32 IRBuilder::
irbRemapObjs(IRStmt* stmt_, const map<IRObject*, IRObject*>& old2NewObjs_) const {
  BMDEBUG2("IRBuilder::irbRemapObjs", stmt_);
  hFUInt32 retVal(0);
  // Each object is under addr of expression, so collect addrofs.
  ExprCollector<IREAddrOf, IRStmt> lAddrOfs(EVOTDoNotCare, stmt_);
  for (hFUInt32 c(0); c < lAddrOfs.size(); ++ c) {
    if (pred.pIsAddrOfObj(lAddrOfs[c]) == true) {
      IRObject* lOldObj(extr.eGetObjOfAddrOf(lAddrOfs[c]));
      map<IRObject*, IRObject*>::const_iterator lIt(old2NewObjs_.find(lOldObj));
      if (lIt != old2NewObjs_.end()) {
        lAddrOfs[c]->ReplaceChild(lOldObj, lIt->second);
      } // if
      ++ retVal;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // IRBuilder::irbRemapObjs

void IRBuilder::
irbAddGuard(IRLoop* loop_) const {
  BMDEBUG2("IRBuilder::irbAddGuard", loop_);
  REQUIREDMSG(loop_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(loop_->GetExitBBCount() == 1, ieStrParamValuesDBCViolation);
  if (!loop_->IsIteratesAtLeastOnce()) {
    if (IRSIf* lIf = dynamic_cast<IRSIf*>(loop_->GetHeader()->GetLastStmt())) {
      PDEBUG("IRBuilder", "detail", " Loop: " << progcxt(loop_));
      IRBB* lTrueBB(lIf->GetTrueCaseBB());
      IRExpr* lCond(lIf->GetExpr()->GetRecCopyExpr());
      if (!pred.pIsInLoop(loop_, lTrueBB)) {
        lCond = irBuilder->irbeNot(lCond);
      } // if
      IRBB* lNewBB(irbInsertBB(ICBB(loop_->GetPreHeader(), ICBefore)));
      IRSIf* lGuard(irbsIf(lCond, loop_->GetPreHeader(), extr.eGetExitBB(loop_), ICNull(), SrcLoc()));
      lNewBB->GetLastStmt()->ReplaceWith(lGuard);
      PDEBUG("IRBuilder", "detail", "Guard added " << progcxt(lGuard));
    } // if
  } // if
  EMDEBUG0();
  return;
} // IRBuilder::irbAddGuard

void IRBuilder::
irbSwapTrueFalseBBs(IRSIf* if_) const {
  BMDEBUG2("IRBuilder::irbSwapTrueFalseBBs", if_);
  util.uSwap(if_->mFalseCaseBB, if_->mTrueCaseBB);
  EMDEBUG0();
  return;
} // IRBuilder::irbSwapTrueFalseBBs


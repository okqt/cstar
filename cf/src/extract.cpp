// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __DEBUGINFO_HPP__
#include "debuginfo.hpp"
#endif
#ifndef __DEBUGINFO_DWARF_HPP__
#include "debuginfo_dwarf.hpp"
#endif
#ifndef __STATS_HPP__
#include "stats.hpp"
#endif
#ifndef __ITERATORS_HPP__
#include "iterators.hpp"
#endif
#ifndef __VISITOR_HPP__
#include "visitor.hpp"
#endif

Extract::
Extract() {
  // Do not use DBC in global objects constructor.
  assert(smRefCount == 0);
  ++ smRefCount;
} // Extract

IROLocal* Extract::
eGetLocal(const IRFunction* func_, const string& name_) const {
  BMDEBUG3("Extract::eGetLocal", func_, name_);
  IROLocal* retVal(&Invalid<IROLocal>::Obj());
  for (hFUInt32 c(0); c < func_->mLocals.size(); ++ c) {
    if (func_->mLocals[c]->GetName() == name_) {
      retVal = func_->mLocals[c];
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetLocal

hUInt32 Extract::
eGetNumOfStmts(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetNumOfStmts", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  hUInt32 retVal(0);
  if (!pred.pIsEmpty(bb_)) {
    retVal = bb_->mStmts.size();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNumOfStmts

hUInt16 Extract::
eGetNumOfPreds(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetNumOfPreds", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  hUInt16 retVal(bb_->mPreds.size());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNumOfPreds

hUInt16 Extract::
eGetNumOfSuccs(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetNumOfSuccs", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  hUInt16 retVal(bb_->mSuccs.size());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNumOfSuccs

IRStmt* Extract::
eGetFirstStmt(IRBB* bb_) const {
  BMDEBUG2("Extract::eGetFirstStmt", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  IRStmt* retVal(pred.pIsEmpty(bb_) ? 0 : bb_->GetFirstStmt());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetFirstStmt

IRStmt* Extract::
eGetPrevStmt(IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetPrevStmt", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  IRStmt* retVal(0);
  if (!pred.pIsFirstStmt(stmt_)) {
    retVal = stmt_->GetPrevStmt();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetPrevStmt

const IRStmt* Extract::
eGetPrevStmt(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetPrevStmt", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  const IRStmt* retVal(0);
  if (!pred.pIsFirstStmt(stmt_)) {
    retVal = stmt_->GetPrevStmt();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetPrevStmt

IRStmt* Extract::
eGetNextStmt(IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetNextStmt", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  IRStmt* retVal(0);
  if (!pred.pIsLastStmt(stmt_)) {
    retVal = stmt_->GetNextStmt();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNextStmt

IRExpr* Extract::
eGetParentExpr(const IRExpr* expr_, hFSInt32 level_) const {
  BMDEBUG3("Extract::eGetParentExpr", expr_, level_);
  REQUIREDMSG(level_ >= 1, ieStrParamValuesDBCViolation);
  IRExpr* retVal(0);
  if (IRExpr* lParentExpr = dynamic_cast<IRExpr*>(expr_->GetParent())) {
    if (level_ > 1) {
      retVal = eGetParentExpr(lParentExpr, level_ - 1);
    } else {
      retVal = lParentExpr;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetParentExpr

const IRStmt* Extract::
eGetNextStmt(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetNextStmt", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  const IRStmt* retVal(0);
  if (!pred.pIsLastStmt(stmt_)) {
    retVal = stmt_->GetNextStmt();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNextStmt

IRStmt* Extract::
eGetStmt(const InsertContext& ic_) const {
  BMDEBUG2("Extract::eGetStmt", &ic_);
  IRStmt* retVal(0);
  const ICStmt* lStmtIC(dynamic_cast<const ICStmt*>(&ic_));
  if (lStmtIC != 0) {
    retVal = lStmtIC->GetStmt();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetStmt

IRStmt* Extract::
eGetStmt(const IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetStmt", expr_);
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  REQUIREDMSG(pred.pIsObj(expr_) == false, ieStrParamValuesDBCViolation <<
    ": It is not possible to get the statements from objects or initialization expression.");
  IRStmt* retVal(0);
  if (!dynamic_cast<IRStmt*>(expr_->GetParent())) {
    retVal = eGetStmt(expr_->GetParentExpr());
  } else {
    retVal = expr_->GetParentStmt();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetStmt

RegSet Extract::
eGetVirtRegs(const RegSet& regSet_) const {
  BMDEBUG1("Extract::eGetVirtRegs");
  RegSet retVal;
  set<Register*>::iterator lIt(regSet_.mRegs.begin());
  while (lIt != regSet_.mRegs.end()) {
    if (dynamic_cast<VirtReg*>(*lIt) != 0) {
      retVal.Insert(*lIt);
    } // if
    ++ lIt;
  } // while
  EMDEBUG0();
  return retVal;
} // Extract::eGetVirtRegs

void Extract::
eGetRevPostOrderBBs(CFG* cfg_, vector<IRBB*>& bbs_) const {
  BMDEBUG2("Extract::eGetRevPostOrderBBs", cfg_);
  REQUIREDMSG(cfg_ != 0, ieStrNonNullParam);
  REQUIREDMSG(bbs_.empty(), ieStrParamValuesDBCViolation);
  eGetPostOrderBBs(cfg_, bbs_);
  std::reverse(bbs_.begin(), bbs_.end());
  EMDEBUG0();
  return;
} // Extract::eGetRevPostOrderBBs

tBigInt Extract::
eGetOffsetAddrStack(const Address* addr_) const {
  BMDEBUG2("Extract::eGetOffsetAddrStack", addr_);
  REQUIREDMSG(dynamic_cast<const AddrStack*>(addr_) != 0, ieStrParamValuesDBCViolation);
  const AddrStack* lAddr(static_cast<const AddrStack*>(addr_));
  tBigInt retVal(lAddr->GetOffset());
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eGetOffsetAddrStack

IRObject* Extract::
eGetObjOfUse(const IRExpr* deref_, bool dbcEnsureNonNull_) const {
  BMDEBUG2("Extract::eGetObjOfUse", deref_);
  REQUIREDMSG(pred.pIsDeref(deref_) == true, ieStrParamValuesDBCViolation);
  IRObject* retVal(0);
  const IREDeref* lDeref(static_cast<const IREDeref*>(deref_));
  if (pred.pIsAddrOf(lDeref->GetExpr()) == true) {
    retVal = eGetObjOfAddrOf(lDeref->GetExpr(), dbcEnsureNonNull_);
  } // if
  ENSUREMSG(retVal != 0 || dbcEnsureNonNull_ != scDBCEnsureNonNull, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetObjOfUse

IRObject* Extract::
eGetObjOfAddrOf(const IRExpr* addrOf_, bool dbcEnsureNonNull_) const {
  BMDEBUG2("Extract::eGetObjOfAddrOf", addrOf_);
  REQUIREDMSG(pred.pIsAddrOf(addrOf_) == true, ieStrParamValuesDBCViolation);
  const IREAddrOf* lAddrOf(static_cast<const IREAddrOf*>(addrOf_));
  IRObject* retVal(dynamic_cast<IRObject*>(lAddrOf->GetExpr()));
  ENSUREMSG(retVal != 0 || dbcEnsureNonNull_ != scDBCEnsureNonNull, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetObjOfAddrOf

const string& Extract::
eGetLabelOf(const IRStmt* stmt_, CGFuncContext& context_) const {
  ASSERTMSG(0, ieStrNotImplemented);
} // Extract::eGetLabelOf

const string& Extract::
eGetLabelOf(const IRFunction* func_, CGFuncContext& context_) const {
  ASSERTMSG(0, ieStrNotImplemented);
} // Extract::eGetLabelOf

const string& Extract::
eGetLabelOf(const IRBB* bb_, CGFuncContext& context_) const {
  ASSERTMSG(0, ieStrNotImplemented);
} // Extract::eGetLabelOf

SrcLoc Extract::
eGetSrcLoc(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetSrcLoc", stmt_);
  SrcLoc retVal;
  if (pred.pHasDIE(stmt_) == true) {
    retVal = stmt_->diGetDIE()->GetSrcLoc();
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eGetSrcLoc

IRExpr* Extract::
eGetInits(const IRObject* obj_) const {
  BMDEBUG2("Extract::eGetInits", obj_);
  REQUIREDMSG(obj_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<const IROGlobal*>(obj_) != 0, ieStrParamValuesDBCViolation);
  IRExpr* retVal(static_cast<const IROGlobal*>(obj_)->GetInits());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetInits

SrcLoc Extract::
eGetSrcLoc(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetSrcLoc", bb_);
  SrcLoc retVal;
  if (pred.pHasDIE(bb_->GetFirstStmt()) == true) {
    retVal = bb_->GetFirstStmt()->diGetDIE()->GetSrcLoc();
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eGetSrcLoc

SrcLoc Extract::
eGetSrcLoc(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetSrcLoc", func_);
  SrcLoc retVal;
  if (pred.pHasDIE(func_) == true) {
    retVal = func_->diGetDIE()->GetSrcLoc();
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eGetSrcLoc

SrcLoc Extract::
eGetSrcLoc(const IRObject* obj_) const {
  BMDEBUG2("Extract::eGetSrcLoc", obj_);
  SrcLoc retVal;
  if (pred.pHasDIE(obj_) == true) {
    retVal = obj_->diGetDIE()->GetSrcLoc();
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eGetSrcLoc

IRFunction* Extract::
eGetFunc(const IRLoop* loop_) const {
  BMDEBUG2("Extract::eGetFunc", loop_);
  REQUIREDMSG(loop_ != 0, ieStrParamValuesDBCViolation);
  IRFunction* retVal(loop_->GetCFG()->GetFunc());
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetFunc

IRModule* Extract::
eGetModule(const IRStmt* stmt_) const {
  REQUIREDMSG(stmt_ != 0, ieStrParamValuesDBCViolation);
  return eGetModule(eGetFunc(stmt_));
} // Extract::eGetModule

static void
fsGetPreOrderBBs(IRBB* parent_, vector<IRBB*>& bbs_) {
  BDEBUG2("fsGetPreOrderBBs", parent_);
  REQUIREDMSG(parent_ != 0, ieStrNonNullParam);
  vector<IRBB*> lSuccs;
  extr.eGetSuccsOfBB(parent_, lSuccs);
  if (!parent_->IsVisited()) {
    bbs_.push_back(parent_);
    parent_->SetVisited();
    for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
      fsGetPreOrderBBs(lSuccs[c], bbs_);
    } // for
  } // if
  EDEBUG0();
  return;
} // fsGetPreOrderBBs

static void
fsGetPostOrderBBs(IRBB* parent_, vector<IRBB*>& bbs_) {
  BDEBUG2("fsGetPostOrderBBs", parent_);
  REQUIREDMSG(parent_ != 0, ieStrNonNullParam);
  vector<IRBB*> lSuccs;
  extr.eGetSuccsOfBB(parent_, lSuccs);
  if (!parent_->IsVisited()) {
    parent_->SetVisited();
    for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
      fsGetPostOrderBBs(lSuccs[c], bbs_);
    } // for
    bbs_.push_back(parent_);
  } // if
  EDEBUG0();
  return;
} // fsGetPostOrderBBs

hFInt32 Extract::
eGetParamCount(const IRObject* obj_) const {
  BMDEBUG2("Extract::eGetParamCount", obj_);
  REQUIREDMSG(pred.pIsFuncType(obj_) == true, ieStrParamValuesDBCViolation);
  IRTFunc* lFuncType(static_cast<IRTFunc*>(obj_->GetType()));
  hFInt32 retVal(lFuncType->GetNumOfParams());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetParamCount

IRTFunc* Extract::
eGetFuncTypeOfCallExpr(const IRExpr* funcCall_) const {
  BMDEBUG2("Extract::eGetFuncTypeOfCallExpr", funcCall_);
  REQUIREDMSG(dynamic_cast<IRTFunc*>(eGetElemType(funcCall_)) != 0, 
    ieStrParamValuesDBCViolation << " expected to have pointer to function " << progcxt(funcCall_));
  IRTFunc* retVal(static_cast<IRTFunc*>(eGetElemType(funcCall_)));
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetFuncOfCallExpr

tBigInt Extract::
eGetTotalTypeSize(const IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetTotalTypeSize", expr_);
  tBigInt retVal(0);
  if (pred.pIsExprList(expr_)) {
    retVal = eGetTotalTypeSize(static_cast<const IRExprList*>(expr_));
  } else {
    retVal = expr_->GetType()->GetSize();
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eGetTotalTypeSize

tBigInt Extract::
eGetTotalTypeSize(const IRExprList* exprList_) const {
  BMDEBUG2("Extract::eGetTotalTypeSize", exprList_);
  REQUIREDMSG(exprList_ != 0, ieStrNonNullParam);
  tBigInt retVal(0);
  vector<IRExpr*> lElems;
  exprList_->GetChildren(lElems);
  for (hFUInt32 c(0); c < lElems.size(); ++ c) {
    if (const IRExprList* lExprList = dynamic_cast<const IRExprList*>(lElems[c])) {
      retVal += eGetTotalTypeSize(lExprList);
    } else {
      retVal += lElems[c]->GetType()->GetSize();
    } // if
  } // for
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eGetTotalTypeSize

IRType* Extract::
eGetElemType(const IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetElemType", expr_);
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  REQUIREDMSG(pred.pIsPtrOrArray(expr_->GetType()) == true, ieStrParamValuesDBCViolation);
  IRType* retVal(0);
/*  if (const IREAddrOf* lAddrOf = dynamic_cast<const IREAddrOf*>(expr_)) {
    //! \todo M Design: probably we do not need this block anymore.
    //! \todo M Design: Can we guarantee that this is always the case, i.e.
    //!       addrof object.
    if (pred.pIsObj(lAddrOf->GetExpr()) == true) {
      expr_ = lAddrOf->GetExpr();
    } // if
  } // if
*/
  if (const IRTArray* lArray = dynamic_cast<const IRTArray*>(expr_->GetType())) {
    retVal = lArray->GetElemType();
  } else {
    ASSERTMSG(dynamic_cast<const IRTPtr*>(expr_->GetType()) != 0, ieStrInconsistentInternalStructure);
    const IRTPtr* lPtr(static_cast<const IRTPtr*>(expr_->GetType()));
    retVal = lPtr->GetRefType();
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetElemType

IRStmt* Extract::
eGetFirstStmt(IRFunction* func_) const {
  BMDEBUG2("Extract::eGetFirstStmt", func_);
  IRStmt* retVal(eGetFirstStmt(func_->GetCFG()->GetEntryBB()));
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetFirstStmt

IRType* Extract::
eGetRetType(const IRSCall* call_) const {
  BMDEBUG2("Extract::eGetRetType", call_);
  REQUIREDMSG(call_ != 0, ieStrNonNullParam);
  IRType* retVal(0);
  if (const IRSFCall* lFCall = dynamic_cast<const IRSFCall*>(call_)) {
    retVal = lFCall->GetReturnIn()->GetType();
  } else {
    ASSERTMSG(pred.pIsPCall(call_) == true, ieStrInconsistentInternalStructure);
    retVal = Singleton<IRBuilder>::Obj().irbtGetVoid();
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetRetType

string Extract::
eGetName(const IRSCall* call_) const {
  BMDEBUG2("Extract::eGetName", call_);
  REQUIREDMSG(call_ != 0, ieStrNonNullParam);
  string retVal("<indirect>");
  if (!pred.pIsIndirect(call_)) {
    if (const IRSBuiltInCall* lBICall = dynamic_cast<const IRSBuiltInCall*>(call_)) {
      retVal = toStr(lBICall->GetBIRId());
    } else {
      IREAddrOf* lExpr(static_cast<IREAddrOf*>(call_->GetFuncExpr()));
      ASSERTMSG(dynamic_cast<IROGlobal*>(lExpr->GetExpr()) != 0, ieStrInconsistentInternalStructure);
      IROGlobal* lObj(static_cast<IROGlobal*>(lExpr->GetExpr()));
      retVal = lObj->GetName();
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetName

hFInt32 Extract::
eGetNumOfArgs(const IRSCall* call_) const {
  BMDEBUG2("Extract::eGetNumOfArgs", call_);
  REQUIREDMSG(call_ != 0, ieStrNonNullParam);
  hFInt32 retVal(call_->mArgs->mExprList.size());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNumOfArgs

void Extract::
eGetPreOrderBBs(CFG* cfg_, vector<IRBB*>& bbs_) const {
  BMDEBUG2("Extract::eGetPreOrderBBs", cfg_);
  REQUIREDMSG(cfg_ != 0, ieStrNonNullParam);
  REQUIREDMSG(bbs_.empty(), ieStrParamValuesDBCViolation);
  cfg_->ResetVisits();
  fsGetPreOrderBBs(cfg_->GetEntryBB(), bbs_);
  EMDEBUG0();
  return;
} // Extract::eGetPreOrderBBs

const set<IRBB*>& Extract::
eGetPostDominators(IRBB* bb_) const {
  BMDEBUG2("Extract::eGetPostDominators", bb_);
  if (!bb_->GetCFG()->mPDomTree) {
    bb_->GetCFG()->updatePDomTree();
  } // if
  const set<IRBB*>& retVal(bb_->mPDoms);
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetPostDominators

const set<IRBB*>& Extract::
eGetDominators(IRBB* bb_) const {
  BMDEBUG2("Extract::eGetDominators", bb_);
  if (!bb_->GetCFG()->mDomTree) {
    bb_->GetCFG()->updateDomTree();
  } // if
  const set<IRBB*>& retVal(bb_->mDoms);
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetDominators

void Extract::
eGetPostOrderBBs(CFG* cfg_, vector<IRBB*>& bbs_) const {
  BMDEBUG2("Extract::eGetPostOrderBBs", cfg_);
  REQUIREDMSG(cfg_ != 0, ieStrNonNullParam);
  REQUIREDMSG(bbs_.empty(), ieStrParamValuesDBCViolation);
  cfg_->ResetVisits();
  fsGetPostOrderBBs(cfg_->GetEntryBB(), bbs_);
  EMDEBUG0();
  return;
} // Extract::eGetPostOrderBBs

const ICBB* Extract::
eGetICBB(const InsertContext& ic_) const {
  BMDEBUG2("Extract::eGetICBB", &ic_);
  const ICBB* lBBIC(dynamic_cast<const ICBB*>(&ic_));
  EMDEBUG1(lBBIC);
  return lBBIC;
} // Extract::eGetICBB

const ICStmt* Extract::
eGetICStmt(const InsertContext* ic_) const {
  BMDEBUG2("Extract::eGetICStmt", ic_);
  const ICStmt* lStmtIC(dynamic_cast<const ICStmt*>(ic_));
  EMDEBUG1(lStmtIC);
  return lStmtIC;
} // Extract::eGetICStmt

ICStmt Extract::
eConvToICStmt(const InsertContext* ic_) const {
  BMDEBUG2("Extract::eConvToICStmt", ic_);
  REQUIREDMSG(pred.pIsICStmt(*ic_) == true || (pred.pIsEmpty(ic_->GetBB()) == false &&
    (ic_->GetPlace() == ICEndOfBB || ic_->GetPlace() == ICBegOfBB)), ieStrParamValuesDBCViolation);
  const ICStmt* lICStmt(eGetICStmt(ic_));
  ICStmt retVal(ICInvalid);
  if (lICStmt != 0) {
    retVal = *lICStmt;
  } else if (ic_->GetPlace() == ICEndOfBB) {
    retVal = ICStmt(ic_->GetBB()->GetLastStmt(), ICAfter);
  } else if (ic_->GetPlace() == ICBegOfBB) {
    retVal = ICStmt(ic_->GetBB()->GetFirstStmt(), ICBefore);
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // Extract::eConvToICStmt

CallingConvention* Extract::
eGetCallConv(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetCallConv", func_);
  CallingConvention* retVal(func_->GetDeclaration()->GetType()->GetCallConv());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetCallConv

IRFunction* Extract::
eGetFunc(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetFunc", bb_);
  REQUIRED(bb_ != 0 && bb_->GetCFG() != 0);
  IRFunction* retVal(bb_->GetCFG()->GetFunc());
  ENSURE(retVal != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetFunc

HWReg* Extract::
eGetHWReg(Register* reg_) const {
  BMDEBUG2("Extract::eGetHWReg", reg_);
  HWReg* retVal(0);
  if (dynamic_cast<HWReg*>(reg_) != 0) {
    retVal = static_cast<HWReg*>(reg_);
  } else {
    VirtReg* lVReg(static_cast<VirtReg*>(reg_));
    HWReg* lAssignedReg(lVReg->GetHWReg());
    if (!pred.pIsInvalid(lAssignedReg)) {
      retVal = lAssignedReg;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetHWReg

IRFunction* Extract::
eGetFunc(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetFunc", stmt_);
  REQUIRED(stmt_ != 0 && stmt_->GetBB() != 0);
  IRFunction* retVal(eGetFunc(stmt_->GetBB()));
  ENSURE(retVal != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetFunc

IRFunction* Extract::
eGetFunc(const IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetFunc", expr_);
  REQUIRED(expr_ != 0);
  IRFunction* retVal(eGetFunc(eGetStmt(expr_)));
  ENSURE(retVal != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetFunc

IRStmt* Extract::
eGetLastStmt(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetLastStmt", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  IRStmt* retVal(pred.pIsEmpty(bb_) ? 0 : bb_->GetLastStmt());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetLastStmt

IRType* Extract::
eGetElemType(const IRType* ptrType_) const {
  BMDEBUG2("Extract::eGetElemType", ptrType_);
  REQUIREDMSG(ptrType_ != 0, ieStrNonNullParam);
  REQUIREDMSG(pred.pIsPtrOrArray(ptrType_) == true, ieStrParamValuesDBCViolation);
  IRType* retVal(0);
  if (const IRTArray* lArray = dynamic_cast<const IRTArray*>(ptrType_)) {
    retVal = lArray->GetElemType();
  } else {
    ASSERTMSG(dynamic_cast<const IRTPtr*>(ptrType_) != 0, ieStrInconsistentInternalStructure);
    const IRTPtr* lPtr(static_cast<const IRTPtr*>(ptrType_));
    retVal = lPtr->GetRefType();
  } // if
  EMDEBUG1(retVal);
} // Extract::eGetElemType

void Extract::
eGetSuccsOfStmt(const IRStmt* stmt_, vector<IRBB*>& succs_) const {
  BMDEBUG2("Extract::eGetSuccsOfStmt", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  REQUIRED(stmt_->GetBB() != 0);
  REQUIRED(pred.pIsCFStmt(stmt_) == true);
  succs_.clear();
  if (const IRSJump* lJumpStmt = dynamic_cast<const IRSJump*>(stmt_)) {
    succs_.push_back(lJumpStmt->GetTargetBB());
  } else if (const IRSDynJump* lDynJumpStmt = dynamic_cast<const IRSDynJump*>(stmt_)) {
    ListIterator<IRBB*> lBBIter(eGetFunc(stmt_)->GetCFG()->GetPhysBBIter());
    for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
      if (!pred.pIsEntryBB(*lBBIter)) {
        succs_.push_back(*lBBIter);
      } // if
    } // for
  } else if (const IRSReturn* lRetStmt = dynamic_cast<const IRSReturn*>(stmt_)) {
    succs_.push_back(lRetStmt->GetBB()->GetCFG()->GetExitBB());
  } else if (const IRSIf* lIfStmt = dynamic_cast<const IRSIf*>(stmt_)) {
    succs_.push_back(lIfStmt->GetTrueCaseBB());
    succs_.push_back(lIfStmt->GetFalseCaseBB());
  } else if (const IRSSwitch* lSwitchStmt = dynamic_cast<const IRSSwitch*>(stmt_)) {
    for (hFUInt32 c(0); c < lSwitchStmt->mCaseStmts.size(); ++ c) {
      succs_.push_back(lSwitchStmt->mCaseStmts[c].second);
    } // for
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  ENSUREMSG(succs_.size() != 0, ieStrInconsistentInternalStructure);
  EMDEBUG0();
  return;
} // Extract::eGetSuccsOfStmt

bool Extract::
eGetSuccsOfBB(const IRBB* bb_, vector<IRBB*>& succs_) const {
  BMDEBUG2("Extract::eGetSuccsOfBB", bb_);
  succs_.clear();
  succs_ = bb_->mSuccs;
  bool retVal(!succs_.empty());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetSuccsOfBB

bool Extract::
eGetPredsOfBB(const IRBB* bb_, vector<IRBB*>& preds_) const {
  BMDEBUG2("Extract::eGetPredsOfBB", bb_);
  preds_.clear();
  preds_ = bb_->mPreds;
  bool retVal(!preds_.empty());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetPredsOfBB

IRBB* Extract::
eGetPred(const IRBB* bb_) const {
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  REQUIRED(bb_->mPreds.size() == 1);
  return bb_->mPreds.front();
} // Extract::eGetPred

IRTPtr* Extract::
eGetPtrType(IRExpr* expr_) {
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  IRTPtr* retVal(dynamic_cast<IRTPtr*>(expr_->GetType()));
  return retVal;
} // Extract::eGetPtrType

IRBB* Extract::
eGetSucc(const IRBB* bb_) const {
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  REQUIRED(bb_->mSuccs.size() == 1);
  return bb_->mSuccs.front();
} // Extract::eGetSucc

void Extract::
eGetPostOrderTraversal(IRExpr* root_, vector<IRExpr*>& nodes_) const {
  vector<IRExpr*> lChildren;
  /* LIE */ root_->GetChildren(lChildren);
  for (hFUInt32 c(0); c < lChildren.size(); ++ c) {
    eGetPostOrderTraversal(lChildren[c], nodes_);
  } // for
  nodes_.push_back(root_);
} // Extract::eGetPostOrderTraversal

void Extract::
eGetPreOrderTraversal(IRExpr* root_, vector<IRExpr*>& nodes_) const {
  vector<IRExpr*> lChildren;
  nodes_.push_back(root_);
  /* LIE */ root_->GetChildren(lChildren);
  for (hFUInt32 c(0); c < lChildren.size(); ++ c) {
    eGetPreOrderTraversal(lChildren[c], nodes_);
  } // for
} // Extract::eGetPreOrderTraversal

ListIterator<IRStmt*> Extract::
eGetStmtIter(IRLoop* loop_) const {
  BMDEBUG2("Extract::eGetStmtIter", loop_);
  ListIterator<IRStmt*> retVal;
  set<IRBB*>::iterator lBBIter(loop_->mBodyBBs.begin());
  for (/* LIE */; lBBIter != loop_->mBodyBBs.end(); ++ lBBIter) {
    IRBB* lBB(*lBBIter);
    FwdListIterator<IRStmt*> lStmtIter(lBB->GetStmtIter());
    retVal.Add(lStmtIter);
  } // for
  EMDEBUG0();
  return retVal;
} // Extract::eGetStmtIter

FwdListIterator<IRStmt*> Extract::
eGetStmtIter(IRFunction* func_) const {
  BMDEBUG2("Extract::eGetStmtIter", func_);
  CFG* lCFG(func_->GetCFG());
  FwdListIterator<IRStmt*> retVal;
  FwdListIterator<IRBB*> lBBIter(lCFG->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    FwdListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtIter());
    retVal.Add(lStmtIter);
  } // for
  EMDEBUG0();
  return retVal;
} // Extract::eGetStmtIter

//! \todo M Design: This function can be made efficient by putting it in IRExpr.
hFUInt32 Extract::
eGetNumOfChildren(const IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetNumOfChildren", expr_);
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  hFUInt32 retVal(0);
  vector<IRExpr*> lChildren;
  expr_->GetChildren(lChildren);
  retVal = lChildren.size();
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNumOfChildren

IRModule* Extract::
eGetModule(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetModule", func_);
  REQUIREDMSG(func_ != 0, ieStrNonNullParam);
  IRModule* retVal(func_->GetDeclaration()->GetModule());
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetModule

IRType* Extract::
eGetRetType(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetRetType", func_);
  REQUIREDMSG(func_ != 0, ieStrNonNullParam);
  IRType* retVal(func_->GetDeclaration()->GetType()->GetReturnType());
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetRetType

const map<string, hFUInt32>& Extract::
eGetFileNames(const SrcLoc& srcLoc_) const {
  BMDEBUG2("Extract::eGetFileNames", &srcLoc_);
  const map<string, hFUInt32>& retVal(SrcLoc::smFileNums);
  EMDEBUG0();
  return retVal;
} // Extract::eGetFileNames

DwarfProgDIE* Extract::
eGetDwarfDIE(const IRProgram* prog_) const {
  BMDEBUG2("Extract::eGetDwarfDIE", prog_);
  REQUIREDMSG(prog_ != 0, ieStrParamValuesDBCViolation);
  DwarfProgDIE* retVal(static_cast<DwarfProgDIE*>(prog_->diGetDIE()));
  ENSUREMSG(retVal != 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetDwarfDIE

DwarfModuleDIE* Extract::
eGetDwarfDIE(const IRModule* module_) const {
  BMDEBUG2("Extract::eGetDwarfDIE", module_);
  REQUIREDMSG(module_ != 0, ieStrParamValuesDBCViolation);
  DwarfModuleDIE* retVal(static_cast<DwarfModuleDIE*>(module_->diGetDIE()));
  ENSUREMSG(retVal != 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetDwarfDIE

DwarfFuncDIE* Extract::
eGetDwarfDIE(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetDwarfDIE", func_);
  REQUIREDMSG(func_ != 0, ieStrParamValuesDBCViolation);
  DwarfFuncDIE* retVal(static_cast<DwarfFuncDIE*>(func_->diGetDIE()));
  ENSUREMSG(retVal != 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetDwarfDIE

DwarfStmtDIE* Extract::
eGetDwarfDIE(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetDwarfDIE", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrParamValuesDBCViolation);
  DwarfStmtDIE* retVal(static_cast<DwarfStmtDIE*>(stmt_->diGetDIE()));
  ENSUREMSG(retVal != 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetDwarfDIE

DwarfBBDIE* Extract::
eGetDwarfDIE(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetDwarfDIE", bb_);
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  DwarfBBDIE* retVal(static_cast<DwarfBBDIE*>(bb_->diGetDIE()));
  ENSUREMSG(retVal != 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetDwarfDIE

DwarfObjDIE* Extract::
eGetDwarfDIE(const IRObject* object_) const {
  BMDEBUG2("Extract::eGetDwarfDIE", object_);
  REQUIREDMSG(object_ != 0, ieStrParamValuesDBCViolation);
  DwarfObjDIE* retVal(static_cast<DwarfObjDIE*>(object_->diGetDIE()));
  ENSUREMSG(retVal != 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetDwarfDIE

const set<IRStmt*>& Extract::
eGetAllDefs(CFG* cfg_) const {
  BMDEBUG2("Extract::eGetAllDefs", cfg_);
  const set<IRStmt*>& retVal(cfg_->GetDUUDChains()->mAllUsesStmt);
  EMDEBUG0();
  return retVal;
} // Extract::eGetAllDefs

void Extract::
eGetDefsOfStmt(const IRStmt* stmt_, set<IRObject*>& defines_) const {
  BMDEBUG2("Extract::eGetDefsOfStmt", stmt_);
  REQUIREDMSG(defines_.empty() == true, ieStrParamValuesDBCViolation);
  if (const IRSAssign* lAssign = dynamic_cast<const IRSAssign*>(stmt_)) {
    eGetDefsOfStmt(lAssign, defines_);
  } else if (const IRSCall* lCall = dynamic_cast<const IRSCall*>(stmt_)) {
    eGetDefsOfStmt(lCall, defines_);
  } // if
  EMDEBUG1(defines_.size());
  return;
} // Extract::eGetDefsOfStmt

void Extract::
eGetDefsOfStmt(const IRSCall* call_, set<IRObject*>& defines_) const {
  BMDEBUG2("Extract::eGetDefsOfStmt", call_);
  REQUIREDMSG(defines_.empty() == true, ieStrParamValuesDBCViolation);
  IRFunction* lFunc(eGetCFG(call_)->GetFunc());
  eGetGlobalsAndAddrTakenLocalsAndParams(lFunc, defines_);
  if (pred.pIsFCall(call_) == true) {
    defines_.insert(static_cast<const IRSFCall*>(call_)->GetReturnIn());
  } // if
  STATRECORDMSG("extract call defines: panic", " call = " << progcxt(call_));
  EMDEBUG1(defines_.size());
  return;
} // Extract::eGetDefsOfStmt

void Extract::
eGetDefsOfStmt(const IRSAssign* assign_, set<IRObject*>& defines_) const {
  BMDEBUG2("Extract::eGetDefsOfStmt", assign_);
  REQUIREDMSG(defines_.empty() == true, ieStrParamValuesDBCViolation);
  if (pred.pIsAssignThruPtr(assign_) == true) {
    // All external storage locations and addr taken objects can be defined in this case. 
    IRFunction* lFunc(eGetCFG(assign_)->GetFunc());
    eGetGlobalsAndAddrTakenLocalsAndParams(lFunc, defines_);
  } else if (pred.pIsAddrOfObj(assign_->GetLHS()) == true) {
    IRObject* lObj(eGetObjOfAddrOf(assign_->GetLHS()));
    if (pred.pIsPtrOrArray(lObj->GetType())) {
      IRFunction* lFunc(eGetCFG(assign_)->GetFunc());
      eGetGlobalsAndAddrTakenLocalsAndParams(lFunc, defines_);
    } else {
      defines_.insert(lObj);
    } // if
  } else {
    //! \todo M Design: populate the cases where we are sure about the uses.
    IRFunction* lFunc(eGetCFG(assign_)->GetFunc());
    vector<IRObject*> lLocalsAndParams;
    eGetLocalsAndParams(lFunc, lLocalsAndParams);
    for (hFUInt32 c(0); c < lLocalsAndParams.size(); ++ c) {
      defines_.insert(lLocalsAndParams[c]);
    } // for
    const vector<IROGlobal*>& lGlobals(eGetModule(lFunc)->GetGlobals());
    for (hFUInt32 d(0); d < lGlobals.size(); ++ d) {
      defines_.insert(lGlobals[d]);
    } // for
    STATRECORDMSG("extract assign defines: panic", " assignment = " << progcxt(assign_));
  } // if
  EMDEBUG1(defines_.size());
  return;
} // Extract::eGetDefsOfStmt

IRExpr* Extract::
eGetTopNodeExpr(IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetTopNode", expr_);
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  IRExpr* retVal(expr_);
  if (!pred.pIsInvalid(expr_) && !pred.pIsObj(expr_) && !pred.pIsNonTerminal(expr_)) {
    IRTree* lParent(expr_->GetParent());
    while (!pred.pIsStmt(lParent)) {
      retVal = static_cast<IRExpr*>(lParent);
      if (pred.pIsObj(static_cast<IRExpr*>(lParent)) == true) {
        break;
      } // if
      lParent = lParent->GetParent();
    } // while
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetTopNodeExpr

IRTree* Extract::
eGetTopNode(IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetTopNode", expr_);
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  REQUIREDMSG(pred.pIsInvalid(expr_) == false, ieStrParamValuesDBCViolation);
  IRTree* retVal(expr_);
  while (!pred.pIsStmt(retVal) && !pred.pIsObj(static_cast<IRExpr*>(retVal)) && 
    !pred.pIsNonTerminal(static_cast<IRExpr*>(retVal))) {
    retVal = retVal->GetParent();
  } // while
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetTopNode

const set<IRStmt*>& Extract::
eGetAllUses(CFG* cfg_) const {
  BMDEBUG2("Extract::eGetAllUses", cfg_);
  const set<IRStmt*>& retVal(cfg_->GetDUUDChains()->mAllUsesStmt);
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetAllUses

void Extract::
eGetDerefOfObjs(const IRExpr* expr_, vector<IREDeref*>& derefs_) const {
  BMDEBUG2("Extract::eGetDerefOfObjs", expr_);
  REQUIREDMSG(derefs_.empty() == true, ieStrParamValuesDBCViolation);
  ExprCollector<IREDeref, IRExpr> lDerefs(EVOTDoNotCare, const_cast<IRExpr*>(expr_));
  for (hFUInt32 c(0); c < lDerefs.size(); ++ c) {
    if (pred.pIsUseOfObj(lDerefs[c]) == true) {
      derefs_.push_back(lDerefs[c]);
    } // if
  } // for
  EMDEBUG1(derefs_.size());
  return;
} // Extract::eGetDerefOfObjs

const set<IRStmt*>& Extract::
eGetDefs(IRDUUDChains* duudChains_, IRStmt* useStmt_, IRObject* obj_) const {
  BMDEBUG4("Extract::eGetDefs", duudChains_, useStmt_, obj_);
  const set<IRStmt*>& retVal(duudChains_->mObj2Uses2DefsStmt[obj_][useStmt_]);
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetDefs

const set<IRStmt*>& Extract::
eGetUses(IRDUUDChains* duudChains_, IRStmt* defStmt_, IRObject* obj_) const {
  BMDEBUG4("Extract::eGetUses", duudChains_, defStmt_, obj_);
  const set<IRStmt*>& retVal(duudChains_->mObj2Defs2UsesStmt[obj_][defStmt_]);
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetUses

void Extract::
eGetGlobalsAndAddrTakenLocalsAndParams(IRFunction* func_, set<IRObject*>& funcExtStorage_) const {
  BMDEBUG2("Extract::eGetGlobalsAndAddrTakenLocalsAndParams", func_);
  { // Add address taken locals and params.
    vector<IRObject*> lLocalsAndParams;
    eGetLocalsAndParams(func_, lLocalsAndParams);
    for (hFUInt32 c(0); c < lLocalsAndParams.size(); ++ c) {
      if (lLocalsAndParams[c]->IsAddrTaken() == true) {
        funcExtStorage_.insert(lLocalsAndParams[c]);
      } // if
    } // for
  } // block
  const vector<IROGlobal*>& lGlobals(eGetModule(func_)->GetGlobals());
  for (hFUInt32 c(0); c < lGlobals.size(); ++ c) {
//    if (pred.pIsFuncType(lGlobals[c]) == false) {
      funcExtStorage_.insert(lGlobals[c]);
//    } // if
  } // for
  EMDEBUG1(funcExtStorage_.size());
  return;
} // Extract::eGetGlobalsAndAddrTakenLocalsAndParams

void Extract::
eGetUsesOfStmt(const IRStmt* stmt_, set<IRObject*>& uses_) const {
  BMDEBUG2("Extract::eGetUsesOfStmt", stmt_);
  REQUIREDMSG(uses_.empty() == true, ieStrParamValuesDBCViolation);
  bool lExtStorageAdded(false);
  if (const IRSCall* lCall = dynamic_cast<const IRSCall*>(stmt_)) {
    // all address taken locals and parameters, all globals, all uses of arguments.
    //! \todo M Design: Do interprocedural analysis to reduce the uses_ set.
    IRFunction* lFunc(eGetCFG(stmt_)->GetFunc());
    eGetGlobalsAndAddrTakenLocalsAndParams(lFunc, uses_);
    lExtStorageAdded = true;
    { // Add all uses of arguments.
      ExprCollector<IREDeref> lDerefs(EVOTDoNotCare, const_cast<IRStmt*>(stmt_), ExprVisitorOptions().SetLHSNo());
      for (hFUInt32 c(0); c < lDerefs.size(); ++ c) {
        if (pred.pIsUseOfObj(lDerefs[c]) == true) {
          uses_.insert(eGetObjOfUse(lDerefs[c]));
        } // if
      } // for
    } // block
  } else if (pred.pIsAssign(stmt_) == true && pred.pIsAssignThruPtr(static_cast<const IRSAssign*>(stmt_)) == true) {
    eGetGlobalsAndAddrTakenLocalsAndParams(eGetCFG(stmt_)->GetFunc(), uses_);
    lExtStorageAdded = true;
  } // if
  // We use LHSYes since *(c_str + i) = 3; may involve use of LHS objects.
  ExprCollector<IREDeref> lDerefs(EVOTDoNotCare, const_cast<IRStmt*>(stmt_), ExprVisitorOptions().SetLHSYes());
  for (hFUInt32 c(0); c < lDerefs.size(); ++ c) {
    if (pred.pIsUse(lDerefs[c]) == true) {
      IRObject* lObj(eGetObjOfUse(lDerefs[c]));
      if (lObj == 0 && lExtStorageAdded == false) {
        lExtStorageAdded = true;
        eGetGlobalsAndAddrTakenLocalsAndParams(eGetCFG(stmt_)->GetFunc(), uses_);
      } else if (lObj != 0) {
        uses_.insert(lObj);
      } // if
    } else if (lExtStorageAdded == false) {
      lExtStorageAdded = true;
      eGetGlobalsAndAddrTakenLocalsAndParams(eGetCFG(stmt_)->GetFunc(), uses_);
    } // if
  } // for
  EMDEBUG1(uses_.size());
  return;
} // Extract::eGetUsesOfStmt

CFG* Extract::
eGetCFG(const IRExpr* expr_) const {
  return eGetStmt(expr_)->GetBB()->GetCFG();
} // Extract::eGetCFG

CFG* Extract::
eGetCFG(const IRStmt* stmt_) const {
  return stmt_->GetBB()->GetCFG();
} // Extract::eGetCFG

const set<IRStmt*>& Extract::
eGetAllDefsOfObj(IRDUUDChains* duudChains_, IRObject* obj_) const {
  BMDEBUG3("Extract::eGetAllDefsOfObj", duudChains_, obj_);
  REQUIREDMSG(duudChains_ != 0, ieStrNonNullParam);
  REQUIREDMSG(obj_ != 0, ieStrNonNullParam);
  const set<IRStmt*>& retVal(duudChains_->mObj2AllDefs[obj_]);
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetAllDefsOfObj

const set<IRStmt*>& Extract::
eGetAllDefsOfObj(CFG* cfg_, IRObject* obj_) const {
  BMDEBUG3("Extract::eGetAllDefsOfObj", cfg_, obj_);
  REQUIREDMSG(cfg_ != 0, ieStrNonNullParam);
  REQUIREDMSG(obj_ != 0, ieStrNonNullParam);
  const set<IRStmt*>& retVal(eGetAllDefsOfObj(cfg_->GetDUUDChains(), obj_));
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetAllDefsOfObj

IRExpr* Extract::
eGetBinOtherOp(const IRExpr* op_) const {
  BMDEBUG2("Extract::eGetBinOtherOp", op_);
  REQUIREDMSG(op_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(pred.pIsBinary(extr.eGetParentExpr(op_)) == true, ieStrParamValuesDBCViolation);
  const IREBinary* binary_(static_cast<const IREBinary*>(extr.eGetParentExpr(op_)));
  IRExpr* retVal(binary_->GetLeftExpr() == op_ ? binary_->GetRightExpr() : binary_->GetLeftExpr());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetBinOtherOp

IRBB* Extract::
eGetPreHeader(IRLoop* loop_) const {
  BMDEBUG2("Extract::eGetPreHeader", loop_);
  IRBB* retVal(loop_->GetPreHeader());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetPreHeader

void Extract::
eGetPredsSet(const IRBB* bb_, IRBB* stopBB_, set<IRBB*>& predSet_) const {
  BMDEBUG3("Extract::eGetPredsSet", bb_, stopBB_);
  if (bb_ != stopBB_) {
    vector<IRBB*> lPreds;
    eGetPredsOfBB(bb_, lPreds);
    for (hFUInt32 c(0); c < lPreds.size(); ++ c) {
      if (predSet_.find(lPreds[c]) == predSet_.end()) {
        predSet_.insert(lPreds[c]);
        eGetPredsSet(lPreds[c], stopBB_, predSet_);
      } // if
    } // for
  } else {
    predSet_.insert(stopBB_);
  } // if
  EMDEBUG1(predSet_.size());
  return;
} // Extract::eGetPredsSet

const set<IRStmt*>& Extract::
eGetAllUsesOfObj(CFG* cfg_, IRObject* obj_) const {
  BMDEBUG3("Extract::eGetAllUsesOfObj", cfg_, obj_);
  const set<IRStmt*>& retVal(eGetAllUsesOfObj(cfg_->GetDUUDChains(), obj_));
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetAllUsesOfObj

const set<IRStmt*>& Extract::
eGetAllUsesOfObj(IRDUUDChains* duudChains_, IRObject* obj_) const {
  BMDEBUG3("Extract::eGetAllUsesOfObj", duudChains_, obj_);
  const set<IRStmt*>& retVal(duudChains_->mObj2AllUses[obj_]);
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetAllUsesOfObj

IRExpr* Extract::
eGetChild(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetChild", stmt_);
  vector<IRExpr*> lChildren;
  stmt_->GetChildren(lChildren);
  ASSERTMSG(lChildren.size() == 1, ieStrInconsistentInternalStructure << 
    " You may not call eGetChild for multiple children having statements");
  IRExpr* retVal(lChildren[0]);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetChild

const vector<IROLocal*>& Extract::
eGetLocals(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetLocals", func_);
  const vector<IROLocal*>& retVal(func_->GetLocals());
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetLocals

const vector<IROParameter*>& Extract::
eGetParams(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetParams", func_);
  const vector<IROParameter*>& retVal(func_->GetDeclaration()->GetParams());
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetParams

void Extract::
eGetLocalsAndParams(const IRFunction* func_, vector<IRObject*>& localsAndParams_) const {
  BMDEBUG2("Extract::eGetLocalsAndParams", func_);
  const vector<IROLocal*>& lLocals(eGetLocals(func_));
  const vector<IROParameter*>& lParams(eGetParams(func_));
  for (hFUInt32 c(0); c < lLocals.size(); ++ c) {
    localsAndParams_.push_back(lLocals[c]);
  } // for
  for (hFUInt32 d(0); d < lParams.size(); ++ d) {
    localsAndParams_.push_back(lParams[d]);
  } // for
  EMDEBUG1(localsAndParams_.size());
  return;
} // Extract::eGetLocalsAndParams

InsertContext* Extract::
eGetReplacementIC(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetReplacementIC", stmt_);
  InsertContext* retVal(0);
  if (pred.pIsFirstStmt(stmt_) == true) {
    retVal = new ICBB(stmt_->GetBB(), ICBegOfBB);
  } else if (pred.pIsLastStmt(stmt_) == true) {
    retVal = new ICBB(stmt_->GetBB(), ICEndOfBB);
  } else {
    retVal = new ICStmt(stmt_->GetPrevStmt(), ICAfter);
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetReplacementIC

static bool 
sfGetBBsInPath(IRBB* fromBB_, IRBB* toBB_, set<IRBB*>& bbs_) {
  BDEBUG3("sfGetBBsInPath", fromBB_, toBB_);
  bool retVal(false);
  if (!fromBB_->IsVisited()) {
    fromBB_->SetVisited();
    if (fromBB_ == toBB_) {
      bbs_.insert(toBB_);
      bbs_.insert(fromBB_);
      retVal = true;
    } // if
    vector<IRBB*> lSuccs;
    extr.eGetSuccsOfBB(fromBB_, lSuccs);
    for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
      if (sfGetBBsInPath(lSuccs[c], toBB_, bbs_)) {
        bbs_.insert(fromBB_);
        retVal = true;
      } // if
    } // for
  } // if
  EDEBUG1(retVal);
  return retVal;
} // sfGetBBsInPath

void Extract::
eGetBBsInPath(IRBB* fromBB_, IRBB* toBB_, set<IRBB*>& bbs_) const {
  BMDEBUG3("Extract::eGetBBsInPath", fromBB_, toBB_);
  REQUIREDMSG(bbs_.empty() == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(fromBB_->GetCFG() == toBB_->GetCFG(), ieStrParamValuesDBCViolation);
  fromBB_->GetCFG()->ResetVisits();
  sfGetBBsInPath(fromBB_, toBB_, bbs_);
  EMDEBUG1(bbs_.size());
  return;
} // Extract::eGetBBsInPath

CGContext* Extract::
eGetCGContext(const IRBB* bb_) const {
  BMDEBUG2("Extract::eGetCGContext", bb_);
  CGContext* retVal(bb_->GetCFG()->GetFunc()->GetCGContext());
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetCGContext

CGContext* Extract::
eGetCGContext(const IRStmt* stmt_) const {
  BMDEBUG2("Extract::eGetCGContext", stmt_);
  CGContext* retVal(eGetCGContext(stmt_->GetBB()));
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetCGContext

IRExpr* Extract::
eGetRightExpr(const IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetRightExpr", expr_);
  REQUIREDMSG(pred.pIsBinary(expr_) == true, ieStrParamValuesDBCViolation);
  IRExpr* retVal(static_cast<const IREBinary*>(expr_)->GetRightExpr());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetRightExpr

IRExpr* Extract::
eGetLeftExpr(const IRExpr* expr_) const {
  BMDEBUG2("Extract::eGetRightExpr", expr_);
  REQUIREDMSG(pred.pIsBinary(expr_) == true, ieStrParamValuesDBCViolation);
  IRExpr* retVal(static_cast<const IREBinary*>(expr_)->GetLeftExpr());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetLeftExpr

Target* Extract::
eGetTarget(IRFunction* func_) const {
  BMDEBUG2("Extract::eGetTarget", func_);
  REQUIREDMSG(func_ != 0, ieStrParamValuesDBCViolation);
  Target* retVal(func_->GetCGContext()->GetTarget());
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetTarget

Target* Extract::
eGetTarget(IRLoop* loop_) const {
  BMDEBUG2("Extract::eGetTarget", loop_);
  REQUIREDMSG(loop_ != 0, ieStrParamValuesDBCViolation);
  Target* retVal(loop_->GetCFG()->GetFunc()->GetCGContext()->GetTarget());
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetTarget

const vector<IRLoop*>& Extract::
eGetLoops(CFG* cfg_) const {
  BMDEBUG2("Extract::eGetLoops", cfg_);
  const vector<IRLoop*>& retVal(cfg_->getLoops());
  EMDEBUG1(retVal.size());
  return retVal;
} // Extract::eGetLoops

//! \todo H Design: I think we should integrate function declaration and
//!       function in to one. 
IRFunction* Extract::
eGetCalledFunc(const IRSCall* call_) const {
  BMDEBUG2("Extract::eGetCalledFunc", call_);
  REQUIREDMSG(pred.pIsDirect(call_) == true, ieStrParamValuesDBCViolation);
  IRFunction* retVal(0);
  if (dynamic_cast<const IRSBuiltInCall*>(call_)) {
    retVal = 0;
  } else if (IREAddrOf* lExpr = dynamic_cast<IREAddrOf*>(call_->GetFuncExpr())) {
    IROGlobal* lFuncObj(dynamic_cast<IROGlobal*>(lExpr->GetExpr()));
    if (eGetModule(call_)->IsFuncDefined(lFuncObj->GetName()) == true) {
      retVal = eGetModule(call_)->GetFunc(lFuncObj->GetName());
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetCalledFunc

hFUInt32 Extract::
eGetNumOfParams(const IRFunction* func_) const {
  BMDEBUG2("Extract::eGetNumOfParams", func_);
  bool retVal(eGetParams(func_).size());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetNumOfParams

void Extract::
eGetBackEdgeBBs(IRLoop* loop_, vector<IRBB*>& bbs_) const {
  BMDEBUG2("Extract::eGetBackEdgeBBs", loop_);
  REQUIREDMSG(loop_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(bbs_.empty() == true, ieStrParamValuesDBCViolation);
  vector<IRBB*> lPreds;
  eGetPredsOfBB(loop_->GetHeader(), lPreds);
  for (hFUInt32 c(0); c < lPreds.size(); ++ c) {
    if (pred.pIsInLoop(loop_, lPreds[c]) == true) {
      bbs_.push_back(lPreds[c]);
    } // if
  } // for
  EMDEBUG1(bbs_.size());
  return;
} // Extract::eGetBackEdgeBBs

IRBB* Extract::
eGetExitBB(IRLoop* loop_) const {
  BMDEBUG2("Extract::eGetExitBB", loop_);
  REQUIREDMSG(loop_->GetExitBBCount() == 1, ieStrParamValuesDBCViolation);
  IRBB* retVal(loop_->mExitEdges.begin()->GetTo());
  EMDEBUG1(retVal);
  return retVal;
} // Extract::eGetExitBB

void Extract::
eGetBBs(const CFG* cfg_, vector<IRBB*>& bbs_) const {
  BMDEBUG2("Extract::eGetBBs", cfg_);
  REQUIREDMSG(bbs_.empty() == true, ieStrParamValuesDBCViolation);
  bbs_.insert(bbs_.begin(), cfg_->mBBs.begin(), cfg_->mBBs.end());
  EMDEBUG1(bbs_.size());
  return;
} // Extract::eGetBBs

void Extract::
eGetStmts(const IRBB* bb_, vector<IRStmt*>& stmts_) const {
  BMDEBUG2("Extract::eGetStmts", bb_);
  REQUIREDMSG(stmts_.empty() == true, ieStrParamValuesDBCViolation);
  stmts_.insert(stmts_.begin(), bb_->mStmts.begin(), bb_->mStmts.end());
  EMDEBUG1(stmts_.size());
  return;
} // Extract::eGetStmts


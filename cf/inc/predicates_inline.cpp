// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __PREDICATE_INLINE_CPP__
#define __PREDICATE_INLINE_CPP__

// \file irs_inline.cpp This file is to be included from irs.hpp.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

INLINE__
PredicateDBCHelper* Predicate::
GetDBCHelper() {
  BMDEBUG1("Predicate::GetDBCHelper");
  if (!dbcHelper) {
    dbcHelper = new PredicateDBCHelper(this);
  } // if
  ENSURE_VALID(dbcHelper);
  EMDEBUG1(dbcHelper);
  return dbcHelper;
} // Predicate::GetDBCHelper

//! \brief returns true if \p aStmt is in the range of \p from / \p to
//! \param from can be IRSBeforeFirstOfBB or must have a basic block.
//! \param to can be IRSAfterLastOfBB or must have a basic block.
//! \param aStmt can be any statement.
//! \pre \p from, \p to parent bb must be the same, if they have basic block.
//! \pre \p from must precede \p to.
INLINE__
bool PredicateDBCHelper::
pIsInStmtsRange(const IRStmt* from_, const IRStmt* to_, const IRStmt* aStmt_) {
  BMDEBUG4("PredicateDBCHelper::pIsInStmtsRange", from_, to_, aStmt_);
  REQUIREDMSG(from_ != 0, ieStrNonNullParam);
  REQUIREDMSG(to_ != 0, ieStrNonNullParam);
  REQUIREDMSG(aStmt_ != 0, ieStrNonNullParam);
  REQUIRED(pred.pIsAfterInBB(to_, from_) == true);
  REQUIRED(!(!pred.pIsBeforeFirstOfBB(from_) && !pred.pIsAfterLastOfBB(to_)) || (from_->GetBB() == to_->GetBB()));
  from_ = pred.pIsBeforeFirstOfBB(from_) == true ? extr.eGetFirstStmt(to_->GetBB()) : from_;
  to_ = pred.pIsAfterLastOfBB(to_) == true ? extr.eGetLastStmt(from_->GetBB()) : to_;
  tristate retVal;
  if (aStmt_->GetBB() == from_->GetBB()) {
    retVal = !pred.pIsBeforeInBB(aStmt_, from_) || !pred.pIsAfterInBB(to_, aStmt_);
  } else {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsInStmtsRange

INLINE__
Predicate::
Predicate() :
  dbcHelper(0)
{
  // Do not use DBC in global objects constructor.
  assert(smRefCount == 0);
  ++ smRefCount;
} // Predicate::Predicate

//! \brief Checks if \p to can be reached by repeatedly applying nexts of \p from.
INLINE__
bool Predicate::
pIsAfterInBB(const IRStmt* from_, const IRStmt* to_) const {
  BMDEBUG3("Predicate::pIsAfterInBB", from_, to_);
  bool retVal(!pIsBeforeInBB(from_, to_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAfterInBB

INLINE__
bool Predicate::
pIsBitField(const IROField* member_) const {
  BMDEBUG2("Predicate::pIsBitField", member_);
  bool retVal(dynamic_cast<const IROBitField*>(member_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBitField

INLINE__
bool Predicate::
pIsEntryBB(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pIsEntryBB", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(bb_->GetCFG()->GetEntryBB() == bb_);
  EMDEBUG1(retVal);
  return retVal;
} // Predcate::pIsExitBB

INLINE__
bool Predicate::
pIsExitBB(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pIsExitBB", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(bb_->GetCFG()->GetExitBB() == bb_);
  EMDEBUG1(retVal);
  return retVal;
} // Predcate::pIsExitBB

//! \brief Checks if \p aPred is a predecessor of \p bb.
INLINE__
bool Predicate::
pIsAPred(const IRBB* bb_, const IRBB* aPred_) const {
  BMDEBUG3("Predicate::pIsAPred", bb_, aPred_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  REQUIREDMSG(aPred_ != 0, ieStrNonNullParam);
  bool retVal(find(bb_->mPreds.begin(), bb_->mPreds.end(), aPred_) != bb_->mPreds.end());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAPred

INLINE__
bool Predicate::
pIsAny(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pIsAny", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRBBAny*>(bb_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAny

INLINE__
bool Predicate::
pIsBuiltIn(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsBuiltIn", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSBuiltInCall*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBuiltIn

INLINE__
bool Predicate::
pIsEval(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsEval", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSEval*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsEval

INLINE__
bool Predicate::
pIsLabel(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsLabel", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSLabel*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsLabel

INLINE__
bool Predicate::
pIsNop(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsNop", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  //! \todo H Design: Dirty hack: remove the pIsEval from below condition.
  bool retVal(pIsNull(stmt_) || pIsLabel(stmt_) || pIsEval(stmt_));
  if (!retVal && pIsEval(stmt_)) {
    const IRSEval* eval = static_cast<const IRSEval*>(stmt_);
    retVal = dynamic_cast<const IRENull*>(eval->GetExpr()) != 0;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNop

INLINE__
bool Predicate::
pIsNull(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsNull", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSNull*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNull

INLINE__
bool Predicate::
pIsProlog(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsProlog", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSProlog*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsProlog

INLINE__
bool Predicate::
pIsJump(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsJump", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSJump*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsJump

INLINE__
bool Predicate::
pIsEpilog(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsEpilog", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSEpilog*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsEpilog

INLINE__
bool Predicate::
pIsReturn(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsReturn", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSReturn*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsReturn

//! \brief Checks if \p aSucc is a successor of \p bb.
//! \param bb Basic block in question.
//! \param aSucc May be null.
INLINE__
bool Predicate::
pIsASucc(const IRBB* bb_, const IRBB* succ_) const {
  BMDEBUG3("Predicate::pIsASucc", bb_, succ_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(find(bb_->mSuccs.begin(), bb_->mSuccs.end(), succ_) != bb_->mSuccs.end());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsASucc

//! \brief Checks if \p stmt_ is the first statement of its basic block.
INLINE__
bool Predicate::
pIsFirstStmt(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsFirstStmt", stmt_);
  REQUIRED_VALID(stmt_);
  bool retVal(stmt_->GetBB()->mStmts.front() == stmt_);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsFirstStmt

//! \brief Checks if \p stmt_ is the last statement of its basic block.
//! \pre Valid \p stmt_.
INLINE__
bool Predicate::
pIsLastStmt(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsLastStmt", stmt_);
  REQUIRED_VALID(stmt_);
  bool retVal(stmt_->GetBB()->mStmts.back() == stmt_);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsLastStmt

//! \brief Checks if \p stmt_ is a IRSAfterLastOfBB.
INLINE__
bool Predicate::
pIsAfterLastOfBB(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsAfterLastOfBB", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSAfterLastOfBB*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAfterLastOfBB

INLINE__
bool Predicate::
pIsDynJump(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsDynJump", stmt_);
  bool retVal(dynamic_cast<const IRSDynJump*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDynJump

INLINE__
bool Predicate::
pIsReal(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsReal", type_);
  bool retVal(dynamic_cast<const IRTReal*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsReal

INLINE__
bool Predicate::
pIsInt(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsInt", type_);
  bool retVal(dynamic_cast<const IRTInt*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsInt

INLINE__
bool Predicate::
pIsPtrOrArray(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsPtrOrArray", type_);
  bool retVal(pred.pIsPtr(type_) == true || pred.pIsArray(type_) == true);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsPtrOrArray

INLINE__
bool Predicate::
pIsPtr(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsPtr", type_);
  //! \note Array type is derived from pointer type so we return true for both
  //!       pointers and arrays.
  bool retVal(dynamic_cast<const IRTPtr*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsPtr

//! \brief Checks if statement to be inserted goes to the end of a basic block.
//! \pre Valid \p ic.
//! \sa pIsBegOfBB
INLINE__
bool Predicate::
pIsEndOfBB(const InsertContext& ic_) const {
  BMDEBUG2("Predicate::pIsEndOfBB", &ic_);
  REQUIRED_VALID(&ic_);
  tristate retVal;
  if (pIsEmpty(ic_.GetBB())) {
    retVal = true;
  } else {
    const ICStmt* lStmtIC(extr.eGetICStmt(&ic_));
    if (lStmtIC && pIsLastStmt(lStmtIC->GetStmt()) && ic_.GetPlace() == ICAfter) {
      retVal = true;
    } else if (ic_.GetPlace() == ICEndOfBB) {
      retVal = true;
    } else {
      retVal = false;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsEndOfBB

//! \brief Checks if statement to be inserted goes to the beginning of a basic block.
//! \pre Valid \p ic.
//! \sa pIsEndOfBB
INLINE__
bool Predicate::
pIsBegOfBB(const InsertContext& ic_) const {
  BMDEBUG2("Predicate::pIsBegOfBB", &ic_);
  REQUIRED_VALID(&ic_);
  tristate retVal;
  if (pIsEmpty(ic_.GetBB())) {
    retVal = true;
  } else {
    const ICStmt* lStmtIC(extr.eGetICStmt(&ic_));
    if (lStmtIC && pIsFirstStmt(lStmtIC->GetStmt()) && ic_.GetPlace() == ICBefore) {
      retVal = true;
    } else if (ic_.GetPlace() == ICBegOfBB) {
      retVal = true;
    } else {
      retVal = false;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBegOfBB

INLINE__
bool Predicate::
pIsIntConst(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsIntConst", expr_);
  bool retVal(dynamic_cast<const IREIntConst*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsIntConst

INLINE__
bool Predicate::
pIsAddrTakenLabel(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsAddrTakenLabel", stmt_);
  bool retVal(pIsAddrTaken(stmt_) == true && pIsLabel(stmt_) == true);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrTakenLabel

INLINE__
bool Predicate::
pIsAddrTaken(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pIsAddrTaken", obj_);
  bool retVal(obj_->IsAddrTaken());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrTaken

INLINE__
bool Predicate::
pIsAddrTaken(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsAddrTaken", stmt_);
  bool retVal(false);
  if (const IRSLabel* lLabel = dynamic_cast<const IRSLabel*>(stmt_)) {
    retVal = lLabel->GetAddressTaken() == true;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrTaken

//! \brief Checks if \p stmt_ is a IRSBeforeFirstOfBB.
INLINE__
bool Predicate::
pIsBeforeFirstOfBB(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsBeforeFirstOfBB", stmt_);
  bool retVal(dynamic_cast<const IRSBeforeFirstOfBB*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBeforeFirstOfBB

//! \brief Checks if a statement is a contrrol flow statement.
//!
//! \note Control flow statements are: IRSIf, IRSSwitch, IRSJump, and IRSReturn.
INLINE__
bool Predicate::
pIsCFStmt(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsCFStmt", stmt_);
  bool retVal(dynamic_cast<const IRSJump*>(stmt_) != 0 ||
    dynamic_cast<const IRSIf*>(stmt_) != 0 ||
    dynamic_cast<const IRSDynJump*>(stmt_) != 0 ||
    dynamic_cast<const IRSSwitch*>(stmt_) != 0 ||
    dynamic_cast<const IRSReturn*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsCFStmt

INLINE__
bool Predicate::
pIsICBB(const InsertContext& ic_) const {
  BMDEBUG2("Predicate::pIsICBB", &ic_);
  bool retVal(dynamic_cast<const ICBB*>(&ic_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsICBB

INLINE__
bool Predicate::
pIsICStmt(const InsertContext& ic_) const {
  BMDEBUG2("Predicate::pIsICStmt", &ic_);
  bool retVal(dynamic_cast<const ICStmt*>(&ic_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsICStmt

INLINE__
bool Predicate::
pIsCall(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsCall", stmt_);
  bool retVal(dynamic_cast<const IRSCall*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsCall

INLINE__
bool Predicate::
pIsFunc(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsFunc", type_);
  bool retVal(false);
  if (dynamic_cast<const IRTFunc*>(type_) != 0) {
    retVal = true;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsFunc

INLINE__
bool Predicate::
pIsFuncType(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pIsFuncType", obj_);
  bool retVal(false);
  if (dynamic_cast<IRTFunc*>(obj_->GetType()) != 0) {
    retVal = true;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsFuncType

INLINE__
bool Predicate::
pIsVoid(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsVoid", type_);
  bool retVal(dynamic_cast<const IRTVoid*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsVoid

INLINE__
bool Predicate::
pIsAddrSet(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pIsAddrSet", obj_);
  bool retVal(!pIsInvalid(obj_->mAddr));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrSet

INLINE__
bool Predicate::
pHasDIE(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pHasDIE", stmt_);
  bool retVal(!pred.pIsInvalid(stmt_->mDIE));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasDIE

INLINE__
bool Predicate::
pIsTmp(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pIsTmp", obj_);
  bool retVal(dynamic_cast<const IROTmpGlobal*>(obj_) != 0 || 
    dynamic_cast<const IROTmp*>(obj_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsTmp

INLINE__
bool Predicate::
pHasDIE(const IRFunction* func_) const {
  BMDEBUG2("Predicate::pHasDIE", func_);
  bool retVal(!pred.pIsInvalid(func_->mDIE));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasDIE

INLINE__
bool Predicate::
pHasDIE(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pHasDIE", obj_);
  bool retVal(!pred.pIsInvalid(obj_->mDIE));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasDIE

INLINE__
bool Predicate::
pIsSigned(const IRTInt* intType_) const {
  BMDEBUG2("Predicate::pIsSigned", intType_);
  bool retVal(intType_->GetSign() == IRSSigned);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsSigned

INLINE__
bool Predicate::
pIsUnsigned(const IRTInt* intType_) const {
  BMDEBUG2("Predicate::pIsUnsigned", intType_);
  bool retVal(intType_->GetSign() == IRSUnsigned);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsUnsigned

INLINE__
bool Predicate::
pIsAssociative(const IRExpr* expr0_, const IRExpr* expr1_) const {
  BMDEBUG3("Predicate::pIsAssociative", expr0_, expr1_);
  bool retVal(pIsAssociative(expr0_) == true && pIsAssociative(expr1_) == true);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAssociative

INLINE__
bool Predicate::
pIsAssociative(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsAssociative", expr_);
  bool retVal(false);
  retVal = dynamic_cast<const IREAdd*>(expr_) != 0 ||
    dynamic_cast<const IREMul*>(expr_) != 0 ||
    dynamic_cast<const IREBAnd*>(expr_) != 0 ||
    dynamic_cast<const IREBOr*>(expr_) != 0 ||
    dynamic_cast<const IRELOr*>(expr_) != 0 ||
    dynamic_cast<const IRELAnd*>(expr_) != 0;
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAssociative

INLINE__
bool Predicate::
pIsCommutative(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsCommutative", expr_);
  bool retVal(false);
  retVal = dynamic_cast<const IREAdd*>(expr_) != 0 ||
    dynamic_cast<const IREMul*>(expr_) != 0 ||
    dynamic_cast<const IREBAnd*>(expr_) != 0 ||
    dynamic_cast<const IREBOr*>(expr_) != 0 ||
    dynamic_cast<const IREBXOr*>(expr_) != 0 ||
    dynamic_cast<const IRELOr*>(expr_) != 0 ||
    dynamic_cast<const IRELAnd*>(expr_) != 0;
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsCommutative

INLINE__
bool Predicate::
pIsGlobal(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pIsGlobal", obj_);
  bool retVal(dynamic_cast<const IROGlobal*>(obj_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsGlobal

INLINE__
bool Predicate::
pIsBoolConst(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsBoolConst", expr_);
  bool retVal(dynamic_cast<const IREBoolConst*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBoolConst

INLINE__
bool Predicate::
pIsConst(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsConst", expr_);
  bool retVal(pIsIntConst(expr_) || pIsBoolConst(expr_) || pIsRealConst(expr_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsConst

INLINE__
bool Predicate::
pIsParam(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pIsParam", obj_);
  bool retVal(dynamic_cast<const IROParameter*>(obj_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsParam

INLINE__
bool Predicate::
pIsLocal(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pIsLocal", obj_);
  bool retVal(dynamic_cast<const IROLocal*>(obj_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsLocal

INLINE__
bool Predicate::
pIsCast(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsCast", expr_);
  bool retVal(dynamic_cast<const IRECast*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsCast

INLINE__
bool Predicate::
pIsUse(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsUse", expr_);
  const IREDeref* lDeref(dynamic_cast<const IREDeref*>(expr_));
  bool retVal(lDeref != 0 && pred.pIsAddrOf(lDeref->GetExpr()) == true);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsUse

INLINE__
bool Predicate::
pHasSameValue(const IRExpr* expr0_, const IRExpr* expr1_) const {
  BMDEBUG3("Predicate::pHasSameValue", expr0_, expr1_);
  REQUIREDMSG(expr0_ != 0 && expr1_ != 0, ieStrNonNullParam);
  bool retVal(false);
  if (pred.pIsObj(expr0_) == true && expr0_ == expr1_) {
    retVal = true;
  } else if (pred.pIsAddrOf(expr0_) == true && pred.pIsAddrOf(expr1_) == true) {
    retVal = (extr.eGetObjOfAddrOf(expr0_) != 0) && extr.eGetObjOfAddrOf(expr0_) == extr.eGetObjOfAddrOf(expr1_);
  } else if (pred.pIsDeref(expr0_) == true && pred.pIsDeref(expr1_) == true) {
    retVal = (extr.eGetObjOfUse(expr0_) != 0) && extr.eGetObjOfUse(expr0_) == extr.eGetObjOfUse(expr1_);
  } else if (const IREIntConst* lIntExpr0 = dynamic_cast<const IREIntConst*>(expr0_)) {
    const IREIntConst* lIntExpr1 = dynamic_cast<const IREIntConst*>(expr1_);
    retVal = lIntExpr1 != 0 && (lIntExpr0->GetValue() == lIntExpr1->GetValue());
  } else if (const IREBoolConst* lBoolExpr0 = dynamic_cast<const IREBoolConst*>(expr0_)) {
    const IREBoolConst* lBoolExpr1 = dynamic_cast<const IREBoolConst*>(expr1_);
    retVal = lBoolExpr1 != 0 && (lBoolExpr0->GetValue() == lBoolExpr1->GetValue());
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasSameValue

INLINE__
bool Predicate::
pIsSameExprClass(const IRExpr* expr0_, const IRExpr* expr1_) const {
  BMDEBUG3("Predicate::pIsSameExprClass", expr0_, expr1_);
  bool retVal(typeid(*expr0_) == typeid(*expr1_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsSameExprClass

INLINE__
bool Predicate::
pIsLeafIR(const IRTree* irNode_) const {
  BMDEBUG2("Predicate::pIsLeafIR", irNode_);
  bool retVal(false);
  retVal = dynamic_cast<const IREIntConst*>(irNode_) != 0 ||
    dynamic_cast<const IREBoolConst*>(irNode_) != 0 ||
    dynamic_cast<const IRERealConst*>(irNode_) != 0 ||
    dynamic_cast<const IREStrConst*>(irNode_) != 0 ||
    dynamic_cast<const IREAddrConst*>(irNode_) != 0 ||
    dynamic_cast<const IRObject*>(irNode_) != 0;
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsLeafIR

INLINE__
bool Predicate::
pIsUnsignedInt(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsUnsignedInt", type_);
  bool retVal(false);
  if (pred.pIsInt(type_) == true) {
    IRTInt* lInt(static_cast<const IRTInt*>(type_));
    retVal = lInt->GetSign() == IRSUnsigned;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsUnsignedInt

INLINE__
bool Predicate::
pIsFundamentalType(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsFundamentalType", type_);
  bool retVal(false);
  if (pIsPtr(type_) == true ||
    dynamic_cast<const IRTReal*>(type_) != 0 ||
    dynamic_cast<const IRTBool*>(type_) != 0 ||
    dynamic_cast<const IRTInt*>(type_) != 0) {
    retVal = true;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsFundamentalType

INLINE__
bool Predicate::
pIsSignedToUnsigned(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsSignedToUnsigned", cast_);
  REQUIREDMSG(pIsInts(cast_) == true, ieStrParamValuesDBCViolation);
  IRTInt* lDstInt(static_cast<IRTInt*>(cast_->GetType()));
  IRTInt* lSrcInt(static_cast<IRTInt*>(cast_->GetExpr()->GetType()));
  bool retVal(lSrcInt->GetSign() == IRSSigned && lDstInt->GetSign() == IRSUnsigned);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsSignedToUnsigned

INLINE__
bool Predicate::
pIsUnsignedToSigned(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsUnsignedToSigned", cast_);
  REQUIREDMSG(pIsInts(cast_) == true, ieStrParamValuesDBCViolation);
  IRTInt* lDstInt(static_cast<IRTInt*>(cast_->GetType()));
  IRTInt* lSrcInt(static_cast<IRTInt*>(cast_->GetExpr()->GetType()));
  bool retVal(lSrcInt->GetSign() == IRSUnsigned && lDstInt->GetSign() == IRSSigned);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsUnsignedToSigned

INLINE__
bool Predicate::
pIsUnsignedToUnsigned(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsUnsignedToUnsigned", cast_);
  REQUIREDMSG(pIsInts(cast_) == true, ieStrParamValuesDBCViolation);
  IRTInt* lDstInt(static_cast<IRTInt*>(cast_->GetType()));
  IRTInt* lSrcInt(static_cast<IRTInt*>(cast_->GetExpr()->GetType()));
  bool retVal(lSrcInt->GetSign() == IRSUnsigned && lDstInt->GetSign() == IRSUnsigned);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsUnsignedToUnsigned

INLINE__
bool Predicate::
pIsNarrows(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsNarrows", cast_);
  bool retVal(cast_->GetType()->GetSize() < cast_->GetExpr()->GetType()->GetSize());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNarrows

INLINE__
bool Predicate::
pIsWidens(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsWidens", cast_);
  bool retVal(cast_->GetType()->GetSize() > cast_->GetExpr()->GetType()->GetSize());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsWidens

INLINE__
bool Predicate::
pIsSignedToSigned(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsSignedToSigned", cast_);
  REQUIREDMSG(pIsInts(cast_) == true, ieStrParamValuesDBCViolation);
  IRTInt* lDstInt(static_cast<IRTInt*>(cast_->GetType()));
  IRTInt* lSrcInt(static_cast<IRTInt*>(cast_->GetExpr()->GetType()));
  bool retVal(lSrcInt->GetSign() == IRSSigned && lDstInt->GetSign() == IRSSigned);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsSignedToSigned

INLINE__
bool Predicate::
pIsPtrOrArrays(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsPtrOrArrays", cast_);
  bool retVal(pIsPtrOrArray(cast_->GetType()) && pIsPtrOrArray(cast_->GetExpr()->GetType()));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsPtrOrArrays

INLINE__
bool Predicate::
pIsBool(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsBool", type_);
  bool retVal(dynamic_cast<const IRTBool*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBool

INLINE__
bool Predicate::
pIsExpr(const IRTree* tree_) const {
  BMDEBUG2("Predicate::pIsExpr", tree_);
  bool retVal(dynamic_cast<const IRExpr*>(tree_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsExpr

INLINE__
bool Predicate::
pIsStmt(const IRTree* tree_) const {
  BMDEBUG2("Predicate::pIsStmt", tree_);
  bool retVal(dynamic_cast<const IRStmt*>(tree_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsStmt

INLINE__
bool Predicate::
pIsAddrReg(const Address* addr_) const {
  BMDEBUG2("Predicate::pIsAddrReg", addr_);
  bool retVal(dynamic_cast<const AddrReg*>(addr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrReg

INLINE__
bool Predicate::
pIsAddrLTConst(const Address* addr_) const {
  BMDEBUG2("Predicate::pIsAddrLTConst", addr_);
  bool retVal(dynamic_cast<const AddrLTConst*>(addr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrLTConst

INLINE__
bool Predicate::
pIsAddrStack(const Address* addr_) const {
  BMDEBUG2("Predicate::pIsAddrStack", addr_);
  bool retVal(dynamic_cast<const AddrStack*>(addr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrStack

INLINE__
bool Predicate::
pIsPtrs(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsPtrs", cast_);
  bool retVal(pIsPtr(cast_->GetType()) && pIsPtr(cast_->GetExpr()->GetType()));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsPtrs

INLINE__
bool Predicate::
pIsInts(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsInts", cast_);
  bool retVal(pIsInt(cast_->GetType()) && pIsInt(cast_->GetExpr()->GetType()));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsInts

INLINE__
bool Predicate::
pIsSameSize(const IRECast* cast_) const {
  BMDEBUG2("Predicate::pIsSameSize", cast_);
  bool retVal(cast_->GetType()->GetSize() == cast_->GetExpr()->GetType()->GetSize());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsSameSize

INLINE__
bool Predicate::
pIsCmp(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsCmp", expr_);
  bool retVal(dynamic_cast<const IRECmp*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsCmp

INLINE__
bool Predicate::
pIsAddrOfObj(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsAddrOfObj", expr_);
  bool retVal(false);
  if (const IREAddrOf* lAddrOf = dynamic_cast<const IREAddrOf*>(expr_)) {
    retVal = pred.pIsObj(lAddrOf->GetExpr());
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrOfObj

INLINE__
bool Predicate::
pIsAddrConst(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsAddrConst", expr_);
  bool retVal(dynamic_cast<const IREAddrConst*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrConst

INLINE__
bool Predicate::
pHasLTConstAddr(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pHasLTConstAddr", obj_);
  bool retVal(false);
  if (pred.pIsAddrSet(obj_) == true) {
    retVal = dynamic_cast<AddrLTConst*>(obj_->GetAddress()) != 0;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasLTConstAddr

INLINE__
bool Predicate::
pIsStruct(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsStruct", type_);
  bool retVal(dynamic_cast<const IRTStruct*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsStruct

INLINE__
bool Predicate::
pIsRealConst(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsRealConst", expr_);
  bool retVal(dynamic_cast<const IRERealConst*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsRealConst

INLINE__
bool Predicate::
pIsStrConst(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsStrConst", expr_);
  bool retVal(dynamic_cast<const IREStrConst*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsStrConst

INLINE__
bool Predicate::
pIsArray(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsArray", type_);
  bool retVal(dynamic_cast<const IRTArray*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsArray

INLINE__
bool Predicate::
pIsObj(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsObj", expr_);
  bool retVal(dynamic_cast<const IRObject*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsObj

INLINE__
bool Predicate::
pIsInRange(const hBigInt& value_, const hBigInt& low_, const hBigInt& high_) const {
  BMDEBUG4("Predicate::pIsInRange", &value_, &low_, &high_);
  bool retVal(value_ >= low_ && value_ <= high_);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsInRange

INLINE__
bool Predicate::
pIsDeref(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsDeref", expr_);
  bool retVal(dynamic_cast<const IREDeref*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDeref

INLINE__
bool Predicate::
pIsAddrOf(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsAddrOf", expr_);
  bool retVal(dynamic_cast<const IREAddrOf*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddrOf

INLINE__
bool Predicate::
pIsNullExpr(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsNullExpr", expr_);
  bool retVal(dynamic_cast<const IRENull*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNullExpr

INLINE__
bool Predicate::
pIsStructUnion(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsStructUnion", type_);
  bool retVal(dynamic_cast<const IRTStruct*>(type_) != 0 || dynamic_cast<const IRTUnion*>(type_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsStructUnion

INLINE__
bool Predicate::
pHasInits(const IROGlobal* obj_) const {
  BMDEBUG2("Predicate::pHasInits", obj_);
  bool retVal(!pred.pIsInvalid(obj_->mConstInits));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasInits

INLINE__
bool Predicate::
pIsAggregate(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsAggregate", type_);
  bool retVal(pred.pIsArray(type_) || pred.pIsStructUnion(type_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAggregate

INLINE__
bool Predicate::
pIsDirect(const IRSCall* call_) const {
  BMDEBUG2("Predicate::pIsDirect", call_);
  bool retVal(false);
  if (dynamic_cast<const IRSBuiltInCall*>(call_)) {
    retVal = true;
  } else if (IREAddrOf* lExpr = dynamic_cast<IREAddrOf*>(call_->GetFuncExpr())) {
    retVal = dynamic_cast<IROGlobal*>(lExpr->GetExpr()) != 0;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDirect

//! \brief Returns true if \p bb contains a CF statement.
//! \pre Non-null \p bb.
INLINE__
bool Predicate::
pHasCFStmt(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pHasCFStmt", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  tristate retVal;
  if (pIsEmpty(bb_)) {
    retVal = false;
  } else {
    retVal = pIsCFStmt(bb_->GetLastStmt());
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasCFStmt

//! \brief Returns true if a basic block has any successors.
INLINE__
bool Predicate::
pHasSucc(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pHasSucc", bb_);
  bool retVal(pHasCFStmt(bb_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasSucc

INLINE__
bool Predicate::
pHasPred(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pHasPred", bb_);
  bool retVal(!bb_->mPreds.empty());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasPred

//! \brief Checks if there is no statement in the basic block.
//! \pre Non-null \p bb.
INLINE__
bool Predicate::
pIsEmpty(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pIsEmpty", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(bb_->mStmts.empty());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsEmpty

//! \brief Checks if \p stmt_ belongs to \p bb.
//! \pre Non-null \p bb
INLINE__
bool Predicate::
pIsMember(const IRBB* bb_, const IRStmt* stmt_) const {
  BMDEBUG3("Predicate::pIsMember", bb_, stmt_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(find(bb_->mStmts.begin(), bb_->mStmts.end(), stmt_) != bb_->mStmts.end());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsMember

//! \brief Checks if \p bb belongs to \p cfg.
//! \pre Non-null \p cfg.
//! \pre Non-null \p bb.
INLINE__
bool Predicate::
pIsMember(const CFG* cfg_, const IRBB* bb_) const {
  BMDEBUG3("Predicate::pIsMember", cfg_, bb_);
  REQUIREDMSG(cfg_ != 0, ieStrNonNullParam);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(bb_->GetCFG() == cfg_);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsMember

//! \brief Returns \a true if the target has the specified int type.
//! \pre A valid set target.
//! \sa Target::GetTarget
INLINE__
bool Predicate::
pTargetHasInt(hUInt16 size_, hUInt16 alignment_, IRSignedness sign_) const {
  BMDEBUG4("Predicate::pTargetHasInt", size_, alignment_, sign_);
  REQUIRED_VALID(Target::GetTarget());
  bool retVal(Target::GetTarget()->GetIntType(size_, alignment_, sign_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pTargetHasInt

INLINE__
bool Predicate::
pIsAssign(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsAssign", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSAssign*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAssign

INLINE__
bool Predicate::
pIsIf(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsIf", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSIf*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsIf

//! \brief Returns \a true if the target has the specified real type.
//! \pre A valid set target.
//! \sa Target::GetTarget
INLINE__
bool Predicate::
pTargetHasReal(hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_) const {
  BMDEBUG4("Predicate::pTargetHasReal", mantissa_, exponent_, alignment_);
  REQUIRED_VALID(Target::GetTarget());
  bool retVal(Target::GetTarget()->GetRealType(mantissa_, exponent_, alignment_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pTargetHasReal

INLINE__
bool PredicateDBCHelper::
pIsExprNeverUsed(const IRExpr* expr1_, const IRExpr* expr2_) const {
  return pIsExprNeverUsed(expr1_) && pIsExprNeverUsed(expr2_);
} // PredicateDBCHelper::pIsExprNeverUsed

INLINE__
bool PredicateDBCHelper::
pCheckNeverUsedAndAddUsedExpr(const IRExpr* usedExpr_) {
  BMDEBUG2("PredicateDBCHelper::pCheckNeverUsedAndAddUsedExpr", usedExpr_);
  REQUIREDMSG(pIsExprNeverUsed(usedExpr_) == true, ieStrExprUsedBefore << " expr: " << progcxt(usedExpr_));
  // Do not add objects to the set.
  if (!dynamic_cast<const IRObject*>(usedExpr_) && !mParent->pIsInvalid(usedExpr_)) {
    mUsedExprSet.insert(usedExpr_);
  } // if
  EMDEBUG1(true);
  return true;
} // PredicateDBCHelper::pCheckNeverUsedAndAddUsedExpr

INLINE__
bool PredicateDBCHelper::
pCheckNeverUsedAndAddUsedExpr(const IRExpr* usedExpr1_, const IRExpr* usedExpr2_) {
  BMDEBUG3("PredicateDBCHelper::pCheckNeverUsedAndAddUsedExpr", usedExpr1_, usedExpr2_);
  bool retVal(pCheckNeverUsedAndAddUsedExpr(usedExpr1_) && pCheckNeverUsedAndAddUsedExpr(usedExpr2_));
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pCheckNeverUsedAndAddUsedExpr

INLINE__
bool PredicateDBCHelper::
IsValid() const {
  BMDEBUG1("PredicateDBCHelper::IsValid");
  REQUIREDMSG(mParent != 0, ieStrClassInvariance);
  EMDEBUG1(true);
  return true;
} // PredicateDBCHelper::IsValid

//! \brief Returns true if only one of the arguments is null.
INLINE__
bool PredicateDBCHelper::
pIsOnlyOneNonNull(const void* ptr1_, const void* ptr2_) const {
  BMDEBUG3("PredicateDBCHelper::pIsOnlyOneNonNull", (int)ptr1_, (int)ptr2_);
  hFInt32 count(ptr1_ != 0);
  count += ptr2_ != 0;
  bool retVal(count == 1);
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsOnlyOneNonNull

//! \brief Returns true if only one of the arguments is null.
INLINE__
bool PredicateDBCHelper::
pIsOnlyOneNonNull(const void* ptr1_, const void* ptr2_, const void* ptr3_) const {
  BMDEBUG4("PredicateDBCHelper::pIsOnlyOneNonNull", (int)ptr1_, (int)ptr2_, (int)ptr3_);
  hFInt32 count(ptr1_ != 0);
  count += ptr2_ != 0;
  count += ptr3_ != 0;
  bool retVal(count == 1);
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsOnlyOneNonNull

INLINE__
bool PredicateDBCHelper::
pIsAllNull(const void* ptr1_, const void* ptr2_, const void* ptr3_) const {
  BMDEBUG4("PredicateDBCHelper::pIsAllNull", (int)ptr1_, (int)ptr2_, (int)ptr3_);
  bool retVal(ptr1_ == 0 && ptr2_ == 0 && ptr3_ == 0);
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsAllNull

INLINE__
bool PredicateDBCHelper::
pIsAllNonNull(const void* ptr1_, const void* ptr2_, const void* ptr3_, const void* ptr4_) const {
  BMDEBUG5("PredicateDBCHelper::pIsAllNonNull", (int)ptr1_, (int)ptr2_, (int)ptr3_, (int)ptr4_);
  bool retVal(ptr1_ != 0 && ptr2_ != 0 && ptr3_ != 0 && ptr4_ != 0);
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsAllNonNull

INLINE__
bool PredicateDBCHelper::
pIsAllNonNull(const void* ptr1_, const void* ptr2_, const void* ptr3_) const {
  BMDEBUG4("PredicateDBCHelper::pIsAllNonNull", (int)ptr1_, (int)ptr2_, (int)ptr3_);
  bool retVal(ptr1_ != 0 && ptr2_ != 0 && ptr3_ != 0);
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsAllNonNull

INLINE__
bool PredicateDBCHelper::
pIsAllNull(const void* ptr1_, const void* ptr2_) const {
  BMDEBUG3("PredicateDBCHelper::pIsAllNull", (int)ptr1_, (int)ptr2_);
  bool retVal(ptr1_ == 0 && ptr2_ == 0);
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsAllNull

INLINE__
bool PredicateDBCHelper::
pIsAllNonNull(const void* ptr1_, const void* ptr2_) const {
  BMDEBUG3("PredicateDBCHelper::pIsAllNonNull", (int)ptr1_, (int)ptr2_);
  bool retVal(ptr1_ != 0 && ptr2_ != 0);
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsAllNonNull

//! \brief Returns false if all arguments are non-null or null.
INLINE__
bool PredicateDBCHelper::
pXorPtrs(const void* ptr1_, const void* ptr2_) const {
  BMDEBUG3("PredicateDBCHelper::pXorPtrs", (int)ptr1_, (int)ptr2_);
  bool retVal(!(pIsAllNull(ptr1_, ptr2_) || pIsAllNonNull(ptr1_, ptr2_)));
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pXorPtrs

INLINE__
bool PredicateDBCHelper::
pXorPtrs(const void* ptr1_, const void* ptr2_, const void* ptr3_) const {
  BMDEBUG4("PredicateDBCHelper::pXorPtrs", (int)ptr1_, (int)ptr2_, (int)ptr3_);
  bool retVal(!(pIsAllNull(ptr1_, ptr2_, ptr3_) || pIsAllNonNull(ptr1_, ptr2_, ptr3_)));
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pXorPtrs

INLINE__
bool Predicate::
pIsFCall(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsFCall", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSFCall*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsFCall

INLINE__
bool Predicate::
pIsPCall(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsPCall", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const IRSPCall*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsPCall

INLINE__
bool PredicateDBCHelper::
pIsExprNeverUsed(const IRExpr* expr_) const {
  BMDEBUG2("PredicateDBCHelper::pIsExprNeverUsed", expr_);
  REQUIREDMSG(expr_ != 0, ieStrParamValuesDBCViolation);
  bool retVal(mUsedExprSet.find(expr_) == mUsedExprSet.end());
  EMDEBUG1(retVal);
  return retVal;
} // PredicateDBCHelper::pIsExprNeverUsed

INLINE__
bool Predicate::
pIsExprList(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsExprList", expr_);
  bool retVal(dynamic_cast<const IRExprList*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsExprList

INLINE__
bool Predicate::
pIsIncomplete(const IRType* type_) const {
  BMDEBUG2("Predicate::pIsIncomplete", type_);
  bool retVal(false);
  if (const IRTArray* lArr = dynamic_cast<const IRTArray*>(type_)) {
    retVal = lArr->mIsIncomplete;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsIncomplete

INLINE__
bool Predicate::
pIsConstAssign(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsConstAssign", stmt_);
  bool retVal(false);
  if (const IRSAssign* lAssign = dynamic_cast<const IRSAssign*>(stmt_)) {
    retVal = pred.pIsAddrOfObj(lAssign->GetLHS()) == true && pred.pIsConst(lAssign->GetRHS()) == true;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsConstAssign

INLINE__
bool Predicate::
pIsCopyAssign(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsCopyAssign", stmt_);
  bool retVal(false);
  if (const IRSAssign* lAssign = dynamic_cast<const IRSAssign*>(stmt_)) {
    retVal = pred.pIsAddrOfObj(lAssign->GetLHS()) == true && pred.pIsUseOfObj(lAssign->GetRHS()) == true;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsCopyAssign

INLINE__
bool Predicate::
pIsUseOfObj(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsUseOfObj", expr_);
  bool retVal(false);
  retVal = pred.pIsDeref(expr_) == true && extr.eGetObjOfUse(expr_) != 0;
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsUseOfObj

INLINE__
bool Predicate::
pIsIDom(IRBB* bb_, IRBB* isDomBB_) const {
  BMDEBUG3("Predicate::pIsIDom", bb_, isDomBB_);
  if (!bb_->GetCFG()->mDomTree) {
    bb_->GetCFG()->updateDomTree();
  } // if
  bool retVal(bb_->mIDom == isDomBB_); 
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsIDom

INLINE__
bool Predicate::
pIsDom(IRBB* bb_, IRBB* isDomBB_) const {
  BMDEBUG3("Predicate::pIsDom", bb_, isDomBB_);
  if (!bb_->GetCFG()->mDomTree) {
    bb_->GetCFG()->updateDomTree();
  } // if
  bool retVal(bb_->mDoms.find(isDomBB_) != bb_->mDoms.end());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDom

INLINE__
bool Predicate::
pIsIPDom(IRBB* bb_, IRBB* isPDomBB_) const {
  BMDEBUG3("Predicate::pIsIPDom", bb_, isPDomBB_);
  if (!bb_->GetCFG()->mPDomTree) {
    bb_->GetCFG()->updatePDomTree();
  } // if
  bool retVal(bb_->mIPDom == isPDomBB_); 
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsIPDom

INLINE__
bool Predicate::
pIsPDom(IRBB* bb_, IRBB* isPDomBB_) const {
  BMDEBUG3("Predicate::pIsPDom", bb_, isPDomBB_);
  if (!bb_->GetCFG()->mPDomTree) {
    bb_->GetCFG()->updatePDomTree();
  } // if
  bool retVal(bb_->mPDoms.find(isPDomBB_) != bb_->mPDoms.end());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsPDom

INLINE__
bool Predicate::
pIsTopNodeObj(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsTopNodeObj", expr_);
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  bool retVal(false);
  if (pIsInvalid(expr_) == false) {
    const IRTree* lTopNode(extr.eGetTopNode(expr_));
    retVal = !pIsStmt(lTopNode);
    ASSERTMSG(pIsStmt(lTopNode) == true || pIsObj(static_cast<const IRExpr*>(lTopNode)) == true ||
      pIsNonTerminal(static_cast<const IRExpr*>(lTopNode)) == true, ieStrInconsistentInternalStructure);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsTopNodeObj

INLINE__
bool Predicate::
pIsBOr(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsBOr", expr_);
  bool retVal(dynamic_cast<const IREBOr*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBOr

INLINE__
bool Predicate::
pIsBAnd(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsBAnd", expr_);
  bool retVal(dynamic_cast<const IREBAnd*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBAnd

INLINE__
bool Predicate::
pIsLOr(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsLOr", expr_);
  bool retVal(dynamic_cast<const IRELOr*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsLOr

INLINE__
bool Predicate::
pIsLAnd(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsLAnd", expr_);
  bool retVal(dynamic_cast<const IRELAnd*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsLAnd

INLINE__
bool Predicate::
pIsLAndOr(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsLAndOr", expr_);
  bool retVal(dynamic_cast<const IRELAnd*>(expr_) != 0 || dynamic_cast<const IRELOr*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsLAndOr

INLINE__
bool Predicate::
pIsBAndOr(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsBAndOr", expr_);
  bool retVal(dynamic_cast<const IREBAnd*>(expr_) != 0 || dynamic_cast<const IREBOr*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBAndOr

INLINE__
bool Predicate::
pIsAddOrSub(const IRExpr* expr0_, const IRExpr* expr1_) const {
  BMDEBUG3("Predicate::pIsAddOrSub", expr0_, expr1_);
  bool retVal(pIsAddOrSub(expr0_) == true && pIsAddOrSub(expr1_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddOrSub

INLINE__
bool Predicate::
pIsAddOrSub(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsAddOrSub", expr_);
  bool retVal(dynamic_cast<const IREAdd*>(expr_) != 0 || dynamic_cast<const IRESub*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAddOrSub

INLINE__
bool Predicate::
pIsUnary(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsUnary", expr_);
  bool retVal(dynamic_cast<const IREUnary*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsUnary

INLINE__
bool Predicate::
pIsBinary(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsBinary", expr_);
  bool retVal(dynamic_cast<const IREBinary*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBinary

INLINE__
bool Predicate::
pIsSub(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsSub", expr_);
  bool retVal(dynamic_cast<const IRESub*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsSub

INLINE__
bool Predicate::
pIsAdd(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsAdd", expr_);
  bool retVal(dynamic_cast<const IREAdd*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAdd

INLINE__
bool Predicate::
pIsDiv(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsDiv", expr_);
  bool retVal(dynamic_cast<const IREDiv*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDiv

INLINE__
bool Predicate::
pIsBXOr(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsBXOr", expr_);
  bool retVal(dynamic_cast<const IREBXOr*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBXOr

INLINE__
bool Predicate::
pIsParentExpr(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsParentExpr", expr_);
  bool retVal(dynamic_cast<IRExpr*>(expr_->mParent) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsParentExpr

INLINE__
bool Predicate::
pIsMul(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsMul", expr_);
  bool retVal(dynamic_cast<const IREMul*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsMul

INLINE__
bool Predicate::
pIsModOrDiv(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsModOrDiv", expr_);
  bool retVal(dynamic_cast<const IREDiv*>(expr_) != 0 ||
    dynamic_cast<const IREMod*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsModOrDiv

INLINE__
bool Predicate::
pIsMulOrDiv(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsMulOrDiv", expr_);
  bool retVal(dynamic_cast<const IREMul*>(expr_) != 0 ||
    dynamic_cast<const IREDiv*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsMulOrDiv

INLINE__
bool Predicate::
pIsShift(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsShift", expr_);
  bool retVal(dynamic_cast<const IREAShiftRight*>(expr_) != 0 ||
    dynamic_cast<const IRELShiftRight*>(expr_) != 0 ||
    dynamic_cast<const IREShiftLeft*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsShift

INLINE__
bool Predicate::
pIsNegOrNot(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsNegOrNot", expr_);
  bool retVal(dynamic_cast<const IREANeg*>(expr_) != 0 ||
    dynamic_cast<const IREBNeg*>(expr_) != 0 ||
    dynamic_cast<const IRENot*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNegOrNot

INLINE__
bool Predicate::
pIsSwitch(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsSwitch", stmt_);
  bool retVal(dynamic_cast<const IRSSwitch*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsSwitch

INLINE__
bool Predicate::
pIsPrologOrEpilog(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsPrologOrEpilog", stmt_);
  bool retVal(dynamic_cast<const IRSProlog*>(stmt_) != 0 ||
    dynamic_cast<const IRSEpilog*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsPrologOrEpilog

INLINE__
bool Predicate::
pIsIfOrSwitch(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsIfOrSwitch", stmt_);
  bool retVal(dynamic_cast<const IRSIf*>(stmt_) != 0 ||
    dynamic_cast<const IRSSwitch*>(stmt_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsIfOrSwitch

#endif


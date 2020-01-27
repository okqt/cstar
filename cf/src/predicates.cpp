// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __DEBUGINFO_DWARF_HPP__
#include "debuginfo_dwarf.hpp"
#endif
#ifndef __TEMPLATE_AVAILEXPR_HPP__
#include "template_availexpr.hpp"
#endif

#if DEBUGBUILD
#include "predicates_inline.cpp"
#endif

//! \pre \p from and \p to may not be IRSBeforeFirstOfBB, IRSAfterLastOfBB.
//! \pre \p from and \p to must be different.
bool Predicate::
pIsBeforeInBB(const IRStmt* from, const IRStmt* to) const {
  BMDEBUG3("Predicate::pIsBeforeInBB", from, to);
  REQUIREDMSG(from != 0, ieStrNonNullParam);
  REQUIREDMSG(to != 0, ieStrNonNullParam);
  REQUIRED(!pIsBeforeFirstOfBB(from) && !pIsBeforeFirstOfBB(to) &&
    !pIsAfterLastOfBB(from) && !pIsAfterLastOfBB(to));
  REQUIRED(from->GetBB() == to->GetBB());
  REQUIRED(from != to);
  while (from != 0 && from != to) {
    from = extr.eGetNextStmt(from);
  } // while
  bool retVal(to == from);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsBeforeInBB

bool Predicate::
pHasReturn(const IRFunction* func_) const {
  BMDEBUG2("Predicate::pHasReturn", func_);
  vector<IRBB*> preds;
  extr.eGetPredsOfBB(func_->GetCFG()->GetExitBB(), preds);
  tristate retVal(false);
  for (hFUInt32 c(0); c < preds.size(); ++ c) {
    if (pIsReturn(preds[c]->GetLastStmt())) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasReturn

bool Predicate::
pHasLocal(const IRFunction* func_, const string& name_) const {
  BMDEBUG3("Predicate::pHasLocal", func_, name_);
  bool retVal(false);
  for (hFUInt32 c(0); c < func_->mLocals.size(); ++ c) {
    if (func_->mLocals[c]->GetName() == name_) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasLocal

bool Predicate::
pIsDetached(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pIsDetached", bb_);
  REQUIREDMSG(pred.pIsInvalid(bb_) == false, ieStrParamValuesDBCViolation);
  bool retVal(bb_->mParentCFG == 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDetached

bool Predicate::
pIsDetached(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsDetached", expr_);
  REQUIREDMSG(pred.pIsInvalid(expr_) == false, ieStrParamValuesDBCViolation);
  while (expr_->mParent != 0 && !pred.pIsStmt(expr_->mParent)) {
    ASSERTMSG(dynamic_cast<IRExpr*>(expr_->mParent), ieStrInconsistentInternalStructure);
    expr_ = static_cast<IRExpr*>(expr_->mParent);
  } // while
  bool retVal(expr_->mParent == 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDetached

bool Predicate::
pIsAssignThruPtr(const IRSAssign* assign_) const {
  BMDEBUG2("Predicate::pIsAssignThruPtr", assign_);
  ExprCollector<IREDeref, IRExpr> lAnyDeref(EVOTDoNotCare, assign_->GetLHS());
  bool retVal(!lAnyDeref.empty());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAssignThruPtr

bool Predicate::
pCanKill(const IRStmt* stmt_, const IRExpr* expr_) const {
  BDEBUG3("Predicate::pCanKill", stmt_, expr_);
  tristate retVal;
  if (pred.pIsConst(expr_) == true) {
    retVal = false;
  } else {
    AvailableExpressions<bool> lAE;
    vector<IREDeref*> lDerefs;
    extr.eGetDerefOfObjs(expr_, lDerefs);
    for (hFUInt32 c(0); c < lDerefs.size(); ++ c) {
      hFSInt32 lPrevDummy;
      bool lIsProper(lAE.Add(lDerefs[c], lPrevDummy));
      REQUIREDMSG(lIsProper == true, ieStrParamValuesDBCViolation);
    } // for
    retVal = lAE.Remove(stmt_) == true;
  } // if
  EDEBUG1(retVal);
  return retVal;
} // Predicate::pCanKill

bool Predicate::
pHasObjUse(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pHasObjUse", stmt_);
  bool retVal;
  ExprCollector<IRObject> lExprCollector(EVOTDoNotCare, const_cast<IRStmt*>(stmt_));
  retVal = !lExprCollector.empty();
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasObjUse

bool Predicate::
pIsNonTerminal(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsNonTerminal", expr_);
  REQUIREDMSG(expr_ != 0, ieStrNonNullParam);
  bool retVal(dynamic_cast<const NonTerminal*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNonTerminal

bool Predicate::
pHasDebugInfo(const IRObject* obj_) const {
  BMDEBUG2("Predicate::pHasDebugInfo", obj_);
  bool retVal(pHasDIE(obj_) == true);
  if (retVal == true) {
    retVal = extr.eGetDwarfDIE(obj_)->mTag != 0;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasDebugInfo

//! \todo M Build: Check below fuctions for inlining. Do not remove this todo
//!       and always place a new function here.

bool Predicate::
pIsConstTrue(const IRExpr* constExpr_) const {
  BMDEBUG2("Predicate::pIsConstTrue", constExpr_);
  REQUIREDMSG(constExpr_ != 0 && pred.pIsConst(constExpr_) == true, ieStrParamValuesDBCViolation);
  tristate retVal;
  if (const IREIntConst* lIntConst = dynamic_cast<const IREIntConst*>(constExpr_)) {
    retVal = lIntConst->GetValue() != 0;
  } else if (const IREBoolConst* lBoolConst = dynamic_cast<const IREBoolConst*>(constExpr_)) {
    retVal = lBoolConst->GetValue() != false;
  } else if (pred.pIsStrConst(constExpr_) == true) {
    retVal = true;
  } else if (const IRERealConst* lRealConst = dynamic_cast<const IRERealConst*>(constExpr_)) {
    retVal = lRealConst->GetValue() != 0.0;
  } else if (const IREAddrConst* lAddrConst = dynamic_cast<const IREAddrConst*>(lAddrConst)) {
    if (lAddrConst->IsConst() == true) {
      retVal = lAddrConst->GetConstValue() != 0;
    } else {
      retVal = true;
    } // if
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsConstTrue

bool Predicate::
pIsEntryOrExitBB(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pIsEntryOrExitBB", bb_);
  bool retVal(pIsEntryBB(bb_) == true || pIsExitBB(bb_) == true);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsEntryOrExitBB

bool Predicate::
pIsAlmostEmpty(const IRBB* bb_) const {
  BMDEBUG2("Predicate::pIsAlmostEmpty", bb_);
  REQUIREDMSG(bb_ != 0, ieStrNonNullParam);
  bool retVal(extr.eGetNumOfStmts(bb_) == 1 && pred.pIsJump(extr.eGetLastStmt(bb_)) == true);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsAlmostEmpty

bool Predicate::
pIsNullIC(const InsertContext* ic_) const {
  BMDEBUG2("Predicate::pIsNullIC", ic_);
  bool retVal(dynamic_cast<const ICNull*>(ic_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNullIC

bool Predicate::
pIsObjAssign(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsObjAssign", stmt_);
  tristate retVal;
  if (const IRSAssign* lAssign = dynamic_cast<const IRSAssign*>(stmt_)) {
    retVal = pIsAddrOfObj(lAssign->GetLHS());
  } else {
    retVal = false;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsObjAssign

bool Predicate::
pHasSingleDefInLoop(IRLoop* loop_, IRObject* obj_) const {
  BMDEBUG3("Predicate::pHasSingleDefInLoop", loop_, obj_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(loop_, obj_) == true, ieStrParamValuesDBCViolation);
  bool retVal(false);
  const set<IRStmt*>& lDefs(extr.eGetAllDefsOfObj(loop_->GetCFG(), obj_));
  hFUInt32 lDefSeenCount(0);
  for (set<IRStmt*>::iterator lIt(lDefs.begin()); lIt != lDefs.end(); ++ lIt) {
    IRStmt* lStmt(*lIt);
    if (pred.pIsInLoop(loop_, lStmt) == true) {
      ++ lDefSeenCount;
      if (lDefSeenCount == 2) {
        break;
      } // if
    } // if
  } // for
  retVal = lDefSeenCount == 1;
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasSingleDefInLoop

bool Predicate::
pIsInLoop(const IRLoop* loop_, const IRBB* bb_) const {
  BMDEBUG3("Predicate::pIsInLoop", loop_, bb_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(loop_, bb_) == true, ieStrParamValuesDBCViolation);
  bool retVal(loop_->IsInLoopBody(bb_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsInLoop

bool Predicate::
pIsInLoop(const IRLoop* loop_, const IRStmt* stmt_) const {
  BMDEBUG3("Predicate::pIsInLoop", loop_, stmt_);
  REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(loop_, stmt_) == true, ieStrParamValuesDBCViolation);
  bool retVal(loop_->IsInLoopBody(stmt_->GetBB()));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsInLoop

bool Predicate::
pMayDirectRecurse(const IRFunction* func_) const {
  BMDEBUG2("Predicate::pMayDirectRecurse", func_);
  bool retVal(false);
  StmtCollector<IRSCall, IRFunction> lCallStmts(const_cast<IRFunction*>(func_));
  for (hFUInt32 c(0); c < lCallStmts.size(); ++ c) {
    if (pred.pIsDirect(lCallStmts[c]) == true) {
      IRFunction* lCalledFunc(extr.eGetCalledFunc(lCallStmts[c]));
      if (lCalledFunc != 0 && lCalledFunc == func_) {
        retVal = true;
        break;
      } // if
    } else {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pMayDirectRecurse

bool Predicate::
pHasAnyLoop(const IRFunction* func_) const {
  BMDEBUG2("Predicate::pHasAnyLoop", func_);
  bool retVal(!extr.eGetLoops(func_->GetCFG()).empty());
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pHasAnyLoop

bool Predicate::
pIsNeOrEq(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsNeOrEq", expr_);
  bool retVal(dynamic_cast<const IREEq*>(expr_) != 0 || dynamic_cast<const IRENe*>(expr_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNeOrEq

bool Predicate::
pIsEq(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsEq", expr_);
  bool retVal(dynamic_cast<const IREEq*>(expr_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsEq

bool Predicate::
pIsNe(const IRExpr* expr_) const {
  BMDEBUG2("Predicate::pIsNe", expr_);
  bool retVal(dynamic_cast<const IRENe*>(expr_));
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsNe

bool Predicate::
pIsHWRegAssigned(const VirtReg* vreg_) const {
  return pred.pIsInvalid(vreg_->GetHWReg()) == false;
} // Predicate::pIsHWRegAssigned

bool Predicate::
pIsHWReg(const Register* reg_) const {
  BMDEBUG2("Predicate::pIsHWReg", reg_);
  bool retVal(dynamic_cast<const HWReg*>(reg_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsHWReg

bool Predicate::
pIsVReg(const Register* reg_) const {
  BMDEBUG2("Predicate::pIsVReg", reg_);
  bool retVal(dynamic_cast<const VirtReg*>(reg_) != 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsVReg

bool Predicate::
pIsDetached(const IRStmt* stmt_) const {
  BMDEBUG2("Predicate::pIsDetached", stmt_);
  bool retVal(stmt_->mParentBB == 0);
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsDetached

bool Predicate::
pIsHeaderHasExitEdge(const IRLoop* loop_) const {
  BMDEBUG2("Predicate::pIsHeaderHasExitEdge", loop_);
  bool retVal(false);
  vector<IRBB*> lSuccs;
  extr.eGetSuccsOfBB(loop_->GetHeader(), lSuccs);
  for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
    if (!pIsInLoop(loop_, lSuccs[c])) {
      retVal = true;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Predicate::pIsHeaderHasExitEdge


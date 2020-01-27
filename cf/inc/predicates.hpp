// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __PREDICATES_HPP__
#define __PREDICATES_HPP__

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __BCF_TARGET_HPP__
#include "bcf_target.hpp"
#endif

//! \brief Predicate utility class library.
//!
//! Predicate utility should be used for any kind of bool inquiries
//! on IR. Predicate shall not modify any of its arguments.
//! \note Predicate is a run time singleton.
//! \todo M Design: discuss pros and cons of having const members for predicate,
//! i.e., it should be const to reduce the states needs to be remembered or checked.
//! \sa pred
class Predicate : public IDebuggable {
public:
  Predicate();
  //! \brief Checks if \p from can be reached by repeatedly applying prevs of \p to.
  bool pIsBeforeInBB(const IRStmt* from_, const IRStmt* to_) const;
  //! \brief Checks if \p to_ can be reached by repeatedly applying nexts of \p from_.
  bool pIsAfterInBB(const IRStmt* from_, const IRStmt* to_) const;
  bool pIsEven(hFInt32 val_) const {return !(val_ % 2);}
  bool pIsOdd(hFInt32 val_) const {return val_ % 2;}
  //! \brief Checks if the statement is of type eval.
  //! \pre Non-null \p stmt_.
  bool pIsEval(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is of type dynamic jump.
  bool pIsDynJump(const IRStmt* stmt_) const;
  //! \brief Checks if \p expr_ is a IREAddrOf.
  bool pIsAddrOf(const IRExpr* expr_) const;
  bool pIsDeref(const IRExpr* expr_) const;
  //! \brief Checks if \p expr_ is a IREStrConst.
  bool pIsStrConst(const IRExpr* expr_) const;
  //! \brief Checks if \p expr_ is a IRENull.
  bool pIsNullExpr(const IRExpr* expr_) const;
  //! \brief Checks if \p expr_ is of type NonTerminal.
  bool pIsNonTerminal(const IRExpr* expr_) const;
  bool pIsAddrTakenLabel(const IRStmt* stmt_) const;
  //! \brief Checks if the address of statement is taken.
  //! Current only the address of label statements can be taken.
  bool pIsAddrTaken(const IRStmt* stmt_) const;
  //! \brief Returns true if an objects address is ever taken.
  bool pIsAddrTaken(const IRObject* obj_) const;
  //! \brief Returns true if this statement uses any object in its children expression trees.
  bool pHasObjUse(const IRStmt* stmt_) const;
  //! \brief Checks if a statement is of call type.
  bool pIsCall(const IRStmt* stmt_) const;
  //! \brief Returns when the expression is commutative.
  //! \note Addition is for example commutative, i.e. a + b == b + a.
  bool pIsCommutative(const IRExpr* expr_) const;
  //! \brief returns true for associative expressions.
  //! \note Addition is for example associative, i.e. a + (b + c) == (a + b) + c.
  bool pIsAssociative(const IRExpr* expr_) const;
  //! \brief Returns true if all of the expressions are associative.
  bool pIsAssociative(const IRExpr* expr0_, const IRExpr* expr1_) const;
  //! \brief Returns true if two expressions hold the same value.
  //! \note If both are objects or constants with the same value
  //!       it returns true. If both are use of the same object, or
  //!       address of the same object, it returns true.
  //! \return It may return false even if the two values are same
  //!         in unhandled cases.
  bool pHasSameValue(const IRExpr* expr0_, const IRExpr* expr1_) const;
  //! \brief Returns true if the class of two given expressions are same.
  bool pIsSameExprClass(const IRExpr* expr0_, const IRExpr* expr1_) const;
  //! \brief Returns true if deref of addr of is the expression.
  //! \param expr_ Any expression.
  bool pIsUse(const IRExpr* expr_) const;
  //! \brief Returns true if a given loop has a single definition of given object.
  bool pHasSingleDefInLoop(IRLoop* loop_, IRObject* obj_) const;
  //! \brief Returns true if a given statement is in a given loop.
  bool pIsInLoop(const IRLoop* loop_, const IRStmt* stmt_) const;
  //! \brief Returns true if the given function may directly recurse.
  //! \note Direct recursion may happen due to direct call or by any indirect call.
  bool pMayDirectRecurse(const IRFunction* func_) const;
  //! \brief Returns true if function has any loop.
  bool pHasAnyLoop(const IRFunction* func_) const;
  //! \brief Returns true if a given basic block is in a given loop body.
  bool pIsInLoop(const IRLoop* loop_, const IRBB* bb_) const;
  //! \brief Returns true if stmt_ can kill the given expression.
  //! \pre expr_ must be a proper expression, i.e. AvailableExpressions can add it.
  bool pCanKill(const IRStmt* stmt_, const IRExpr* expr_) const;
  //! \brief Returns true if a given object may be killed between to given statements.
  //! \return Returns false if to_ may not be reachable from from_.
  bool pCanBeKilled(const IRObject* obj_, const IRStmt* from_, const IRStmt* to_) const;
  //! \brief Returns true if an object is assigned to another object.
  //! \note a = b is a copy assignment.
  bool pIsCopyAssign(const IRStmt* stmt_) const;
  //! \brief Returns true if a variable is assigned to an object.
  //! \note a = const; is a constant assignment.
  bool pIsConstAssign(const IRStmt* stmt_) const;
  //! \brief Returns true if lhs is an addr of object of an assignment.
  bool pIsObjAssign(const IRStmt* stmt_) const;
  //! \brief Returns true if the value of a given constant is true in conditional context.
  //! \pre constExpr_ must be a constant, i.e. pIsConst should be true for it.
  //! \sa pIsConstFalse.
  bool pIsConstTrue(const IRExpr* constExpr_) const;
  //! \sa pIsConstTrue.
  bool pIsConstFalse(const IRExpr* constExpr_) const {return !pIsConstTrue(constExpr_);}
  bool pIsUseOfObj(const IRExpr* expr_) const;
  //! \brief returns true if there is immediate post domination relation.
  bool pIsIPDom(IRBB* bb_, IRBB* isPDomBB_) const;
  //! \brief returns true if there is post domination relation.
  bool pIsPDom(IRBB* bb_, IRBB* isPDomBB_) const;
  //! \brief returns true if there is immediate domination relation.
  bool pIsIDom(IRBB* bb_, IRBB* isDomBB_) const;
  //! \brief returns true if there is domination relation.
  bool pIsDom(IRBB* bb_, IRBB* isDomBB_) const;
  //! \brief Returns true if a given expression is of type cast.
  //! \param expr_ Any expression.
  bool pIsCast(const IRExpr* expr_) const;
  //! \brief Returns true if assignment is through a pointer.
  //! 
  //! Example:
  //!   int a;
  //!   int* p;
  //!   p = &a; // this is not assignment thru pointer.
  //!   *p = a; // this is assignment thru pointer.
  bool pIsAssignThruPtr(const IRSAssign* assign_) const;
  bool pIsGlobal(const IRObject* obj_) const;
  bool pIsParam(const IRObject* obj_) const;
  bool pIsLocal(const IRObject* obj_) const;
  //! \brief Checks if the statement is of type return.
  //! \pre Non-null \p stmt_.
  bool pIsReturn(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is of type if.
  //! \pre Non-null \p stmt_.
  bool pIsIf(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is of type assign.
  //! \pre Non-null \p stmt_.
  bool pIsAssign(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is has no effect.
  //! \pre Non-null \p stmt_.
  //! \note Labels, and null statement are examples of nop statements.
  bool pIsNop(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is of type null.
  //! \pre Non-null \p stmt_.
  bool pIsNull(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is of type prolog.
  //! \pre Non-null \p stmt_.
  bool pIsProlog(const IRStmt* stmt_) const;
  bool pIsBitField(const IROField* member_) const;
  //! \brief Checks if the statement is of type label.
  //! \pre Non-null \p stmt_.
  bool pIsLabel(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is of type jump.
  //! \pre Non-null \p stmt_.
  bool pIsJump(const IRStmt* stmt_) const;
  //! \brief Checks if the statement is of type epilog.
  //! \pre Non-null \p stmt_.
  bool pIsEpilog(const IRStmt* stmt_) const;
  //! \brief Return true if provided type is a fundamental type, i.e. float,
  //!        pointer, integer.
  bool pIsFundamentalType(const IRType* type_) const;
  //! \brief Returns true if the type of object is function.
  bool pIsFuncType(const IRObject* obj_) const;
  bool pIsEntryBB(const IRBB* bb_) const;
  //! \brief Returns true if \p stmt_ is a built-in routine call.
  bool pIsBuiltIn(const IRStmt* stmt_) const;
  bool pIsSigned(const IRTInt* intType_) const;
  bool pIsUnsigned(const IRTInt* intType_) const;
  //! \brief Checks if \p expr_ is a IRERealConst.
  bool pIsRealConst(const IRExpr* expr_) const;
  //! \brief Returns true if expression is int constant.
  bool pIsIntConst(const IRExpr* expr_) const;
  //! \brief Returns true if expression is bool constant.
  bool pIsBoolConst(const IRExpr* expr_) const;
  //! \brief Returns true if expression is int, bool or real constant.
  bool pIsConst(const IRExpr* expr_) const;
  bool pHasDebugInfo(const IRObject* obj_) const;
  //! \brief Returns true if array or struct size is incomplete.
  bool pIsIncomplete(const IRType* type_) const;
  //! \brief Returns true if the expression is of type IRExprList.
  bool pIsExprList(const IRExpr* expr_) const;
  bool pHasDIE(const IRStmt* stmt_) const;
  bool pHasDIE(const IRFunction* func_) const;
  bool pHasDIE(const IRObject* obj_) const;
  bool pIsFCall(const IRStmt* stmt_) const;
  bool pIsPCall(const IRStmt* stmt_) const;
  //! \brief Returns true if an object is of type IROTmp, IROGlobal.
  bool pIsTmp(const IRObject* obj_) const;
  bool pIsSet(const SrcLoc& srcLoc_) const {
    BMDEBUG2("Predicate::pIsSet", &srcLoc_);
    bool retVal(srcLoc_.GetFileNum() != -1);
    EMDEBUG1(retVal);
    return retVal;
  } // Predicate::pIsSet
  bool pIsExitBB(const IRBB* bb_) const;
  bool pIsEntryOrExitBB(const IRBB* bb_) const;
  //! \brief Checks if \p obj_ is of Invalid type.
  template<class T>
  bool pIsInvalid(const T* obj_) const {
    BMDEBUG2("Predicate::pIsInvalid", obj_);
    REQUIREDMSG(obj_ != 0, ieStrNonNullParam);
    bool retVal(dynamic_cast<const Invalid<T>*>(obj_) != 0);
    EMDEBUG1(retVal);
    return retVal;
  } // Predicate::pIsInvalid
  bool pIsAny(const IRBB* bb_) const;
  //! \brief Checks if \p aPred is a predecessor of \p bb.
  //! \sa pIsASucc
  bool pIsAPred(const IRBB* bb_, const IRBB* pred_) const;
  //! \brief Checks if a type is of type IRTArray.
  bool pIsArray(const IRType* type_) const;
  //! \brief Checks if the expression tree is attached to a statement.
  //! \pre expr_ must be a non Invalid object.
  //! \sa pIsAttached
  bool pIsDetached(const IRExpr* expr_) const;
  //! \brief Returns true if statement is removed from ir.
  bool pIsDetached(const IRStmt* stmt_) const;
  //! \brief Returns true if given register is a virtual register.
  bool pIsVReg(const Register* reg_) const;
  //! \brief Returns true if given register is a hardware register.
  bool pIsHWReg(const Register* reg_) const;
  bool pIsHWRegAssigned(const VirtReg* vreg_) const;
  //! \brief Returns true if there is an edge from header to exit basic block.
  bool pIsHeaderHasExitEdge(const IRLoop* loop_) const;
  //! \brief Returns true if the given expression is attached.
  //! \sa pIsDetached
  bool pIsAttached(const IRExpr* expr_) const {return !pIsDetached(expr_);}
  //! \brief Returns true for addition or subtraction expressions.
  bool pIsAddOrSub(const IRExpr* expr_) const;
  //! \brief Returns true for ANEG, BNEG, NOT expressions.
  bool pIsNegOrNot(const IRExpr* expr_) const;
  //! \brief Returns true if an expression is equal or not equal comparison.
  bool pIsNeOrEq(const IRExpr* expr_) const;
  bool pIsEq(const IRExpr* expr_) const;
  bool pIsNe(const IRExpr* expr_) const;
  //! \brief Returns true for MOD and DIV expressions.
  bool pIsModOrDiv(const IRExpr* expr_) const;
  bool pIsMulOrDiv(const IRExpr* expr_) const;
  bool pIsBinary(const IRExpr* expr_) const;
  bool pIsUnary(const IRExpr* expr_) const;
  bool pIsShift(const IRExpr* expr_) const;
  bool pIsBXOr(const IRExpr* expr_) const;
  //! \brief Returns true if both expressions are ADD or SUB.
  bool pIsAddOrSub(const IRExpr* expr0_, const IRExpr* expr1_) const;
  //! \brief Returns true if expression is a multiplication.
  bool pIsMul(const IRExpr* expr_) const;
  bool pIsDiv(const IRExpr* expr_) const;
  bool pIsAdd(const IRExpr* expr_) const;
  bool pIsSub(const IRExpr* expr_) const;
  bool pIsSwitch(const IRStmt* stmt_) const;
  bool pIsIfOrSwitch(const IRStmt* stmt_) const;
  bool pIsPrologOrEpilog(const IRStmt* stmt_) const;
  //! \brief Returns true if expression is a bitwise AND or OR.
  bool pIsBAndOr(const IRExpr* expr_) const;
  //! \brief Returns true if expression is a logical AND or OR.
  bool pIsLAndOr(const IRExpr* expr_) const;
  //! \brief Returns true if expression is a logical AND.
  bool pIsLAnd(const IRExpr* expr_) const;
  //! \brief Returns true if expression is a logical OR.
  bool pIsLOr(const IRExpr* expr_) const;
  //! \brief Returns true if expression is a bitwise AND
  bool pIsBAnd(const IRExpr* expr_) const;
  //! \brief Returns true if expression is a bitwise OR.
  bool pIsBOr(const IRExpr* expr_) const;
  //! \brief returns true if parent of the expression is an expression.
  bool pIsParentExpr(const IRExpr* expr_) const;
  //! \brief Returns true if top node of this expression is an object.
  //! \note For initialization expressions this is true.
  bool pIsTopNodeObj(const IRExpr* expr_) const;
  //! \brief Return true if type is integer and it is unsigned.
  bool pIsUnsignedInt(const IRType* type_) const;
  //! \brief Returns true if given ir is a leaf IR.
  //! \todo M Design: add list of all leaf nodes.
  //! \note leaf IRs are constants, and objects.
  bool pIsLeafIR(const IRTree* irNode_) const;
  bool pIsBool(const IRType* type_) const;
  //! \brief returns true if tree node is a statement.
  //! \sa pIsExpr
  bool pIsStmt(const IRTree* tree_) const;
  //! \brief returns true if tree node is an expression.
  //! \sa pIsStmt
  bool pIsExpr(const IRTree* tree_) const;
  bool pIsFunc(const IRType* type_) const;
  bool pIsAddrConst(const IRExpr* expr_) const;
  //! \brief returns true if the expression is a relational, <,>,<=,>=,==,!=.
  bool pIsCmp(const IRExpr* expr_) const;
  bool pHasLTConstAddr(const IRObject* obj_) const;
  bool pIsPtrOrArray(const IRType* type_) const;
  //! \brief Checks if the object has a valid address set.
  bool pIsAddrSet(const IRObject* obj_) const;
  //! \brief Checks if the expression is of type IRObject.
  bool pIsObj(const IRExpr* expr_) const;
  //! \brief Returns true if given value is in range (including the low and high).
  bool pIsInRange(const hBigInt& value_, const hBigInt& low_, const hBigInt& high_) const;
  //! \brief Returns true if argument is a direct call of a function.
  bool pIsDirect(const IRSCall* call_) const;
  //! \brief Returns true if the given basic block has been removed.
  bool pIsDetached(const IRBB* bb_) const;
  //! \brief Returns true if argument is an indirect call of a function.
  bool pIsIndirect(const IRSCall* call_) const {return !pIsDirect(call_);}
  //! \brief Returns true if a function has the given named local.
  bool pHasLocal(const IRFunction* func_, const string& name_) const;
  //! \brief Checks if \p aSucc is a successor of \p bb.
  //! \param bb Basic block in question.
  //! \param aSucc May be null.
  bool pIsASucc(const IRBB* bb_, const IRBB* succ_) const;
  //! \brief Checks if \p stmt is the first statement of its basic block.
  bool pIsFirstStmt(const IRStmt* stmt_) const;
  //! \brief Checks if \p stmt is the last statement of its basic block.
  //! \pre Valid \p stmt.
  bool pIsLastStmt(const IRStmt* stmt_) const;
  //! \brief Checks if \p stmt is a IRSAfterLastOfBB.
  bool pIsAfterLastOfBB(const IRStmt* stmt_) const;
  //! \brief Checks if given type is a pointer type, i.e. IRTPtr, or IRTArray.
  bool pIsPtr(const IRType* type_) const;
  //! \brief Returns true if a type is array or struct/union.
  bool pIsAggregate(const IRType* type_) const;
  bool pHasInits(const IROGlobal* obj_) const;
  bool pIsStructUnion(const IRType* type_) const;
  bool pIsStruct(const IRType* type_) const;
  //! \param expr_ Any expression.
  bool pIsAddrOfObj(const IRExpr* expr_) const;
  bool pIsInt(const IRType* type_) const;
  bool pIsReal(const IRType* type_) const;
  //! \brief Checks if statement to be inserted goes to the end of a basic block.
  //! \pre Valid \p ic.
  //! \sa pIsBegOfBB
  bool pIsEndOfBB(const InsertContext& ic_) const;
  //! \brief Checks if statement to be inserted goes to the beginning of a basic block.
  //! \pre Valid \p ic.
  //! \sa pIsEndOfBB
  bool pIsBegOfBB(const InsertContext& ic_) const;
  //! \brief Returns true if given insert context is of type ICNull.
  bool pIsNullIC(const InsertContext* ic_) const;
  //! \brief Checks if \p stmt is a IRSBeforeFirstOfBB.
  bool pIsBeforeFirstOfBB(const IRStmt* stmt_) const;
  //! \brief Checks if a statement is a contrrol flow statement.
  //! \note Control flow statements are: IRSIf, IRSSwitch, IRSJump, and IRSReturn.
  bool pIsCFStmt(const IRStmt* stmt_) const;
  bool pIsICBB(const InsertContext& ic_) const;
  bool pIsICStmt(const InsertContext& ic_) const;
  //! \brief Checks if \p bb contains a CF statement.
  //! \pre Non-null \p bb.
  bool pHasCFStmt(const IRBB* bb_) const;
  //! \brief Checks if a basic block has any successors.
  bool pHasSucc(const IRBB* bb_) const;
  bool pHasPred(const IRBB* bb_) const;
  //! \brief Checks if there is no statement in the basic block.
  //! \pre Non-null \p bb.
  //! \note There is no unconditional jump in an empty basic block.
  bool pIsEmpty(const IRBB* bb_) const;
  //! \brief Checks if there is no statement in the basic block except an unconditional jump.
  //! \pre Non-null \p bb.
  bool pIsAlmostEmpty(const IRBB* bb_) const;
  //! \brief Checks if \p stmt belongs to \p bb.
  //! \pre Non-null \p bb
  bool pIsMember(const IRBB* bb_, const IRStmt* stmt_) const;
  //! \brief Checks if \p bb belongs to \p cfg.
  //! \pre Non-null \p cfg.
  //! \pre Non-null \p bb.
  bool pIsMember(const CFG* cfg_, const IRBB* bb_) const;
  //! \brief Checks if the target has the specified \a int type.
  //! \pre A valid set target.
  //! \sa Target::GetTarget
  bool pTargetHasInt(hUInt16 size_, hUInt16 alignment_, IRSignedness sign_) const;
  //! \brief Checks if the target has the specified real type.
  //! \pre A valid set target.
  //! \sa Target::GetTarget
  bool pTargetHasReal(hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_) const;
  //! \brief Checks if a function has any \a return statement.
  //! \note A function may have multiple \a epilog statements but no \a return
  //! statement, function returns \a false in this case.
  bool pHasReturn(const IRFunction* func_) const;
  bool pIsVoid(const IRType* type_) const;
  //! \brief Returns true if cast results in the same type size.
  bool pIsSameSize(const IRECast* cast_) const;
  //! \pre Source and destination must be integer types.
  bool pIsSignedToUnsigned(const IRECast* cast_) const;
  //! \pre Source and destination must be integer types.
  bool pIsUnsignedToSigned(const IRECast* cast_) const;
  //! \pre Source and destination must be integer types.
  bool pIsUnsignedToUnsigned(const IRECast* cast_) const;
  //! \pre Source and destination must be integer types.
  bool pIsSignedToSigned(const IRECast* cast_) const;
  //! \brief Returns true if source and destination types of cast are integer types.
  bool pIsInts(const IRECast* cast_) const;
  //! \brief Returns true if source and destination types of cast are pointer types.
  bool pIsPtrs(const IRECast* cast_) const;
  //! \brief Returns true if source and destination types of cast are pointer/array types.
  bool pIsPtrOrArrays(const IRECast* cast_) const;
  //! \brief Returns true if target type size is smaller than the source.
  bool pIsNarrows(const IRECast* cast_) const;
  //! \brief Returns true if target size is bigger than the source.
  bool pIsWidens(const IRECast* cast_) const;
  //! \brief Returns true if the type of address is AddrReg.
  bool pIsAddrReg(const Address* addr_) const;
  //! \brief Returns true if the type of address is AddrStack.
  bool pIsAddrStack(const Address* addr_) const;
  //! \brief Returns true if the type of address is AddrLTConst.
  bool pIsAddrLTConst(const Address* addr_) const;
  PredicateDBCHelper* GetDBCHelper();
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  static hFSInt8 smRefCount;
  PredicateDBCHelper* dbcHelper;
}; // class Predicate

class PredicateSortHelper : public IDebuggable {
public:
  PredicateSortHelper()
  {
  } // PredicateSortHelper::PredicateSortHelper
  //! \brief less operator for tInt.
  bool operator () (const tInt& left, const tInt& right) {
    tristate isLess;
    if (left.GetSize() < right.GetSize()) {
      isLess = true;
    } else if (left.GetSize() == right.GetSize()) {
      if (left.GetAlignment() < right.GetAlignment()) {
        isLess = true;
      } else if (left.GetAlignment() == right.GetAlignment()) {
        if (left.GetSign() == IRSSigned) {
          isLess = true;
        } else {
          isLess = false;
        } // if
      } else {
        isLess = false;
      } // if
    } else {
      isLess = false;
    } // if
    return isLess;
  } // PredicateSortHelper::operator()
  virtual char* debug(const DTContext& context, ostream& toStr) const;
}; // class PredicateSortHelper

//! \brief Helper class of Predicate has functions to be used by
//! Design By Contract codes.
//!
//! Sometimes DBC checks gets quite complicated. This helper class
//! is used to accumulate the implementation of such complex checks.
//!
//! \note It should be common to find that member functions are only used
//! once.
//! \todo H Design: add a can insert function to be used in every
//!       irbuilder insert functions.
class PredicateDBCHelper : public IDebuggable {
public:
  PredicateDBCHelper(Predicate* parent_) :
    mParent(parent_)
  {
    ENSURE_VALID(this);
  } // PredicateDBCHelper::PredicateDBCHelper
  //! \brief returns true if \p aStmt is in the range of \p from / \p to
  //! \param from can be IRSBeforeFirstOfBB or must have a basic block.
  //! \param to can be IRSAfterLastOfBB or must have a basic block.
  //! \param aStmt can be any statement.
  //! \pre \p from, \p to parent bb must be the same, if they have basic block.
  //! \pre \p from must precede \p to.
  bool pIsInStmtsRange(const IRStmt* from_, const IRStmt* to_, const IRStmt* stmt_);
  //! \brief Returns true if the expression is never used in a statement.
  bool pIsExprNeverUsed(const IRExpr* expr_) const;
  bool pIsExprNeverUsed(const IRExpr* expr1_, const IRExpr* expr2_) const;
  bool pCheckNeverUsedAndAddUsedExpr(const IRExpr* usedExpr_);
  bool pCheckNeverUsedAndAddUsedExpr(const IRExpr* usedExpr1_, const IRExpr* usedExpr2_);
  virtual bool IsValid() const;
  //! \brief Returns true if only one of the arguments is null.
  bool pIsOnlyOneNonNull(const void* ptr1_, const void* ptr2_) const;
  //! \brief Returns true if only one of the arguments is null.
  bool pIsOnlyOneNonNull(const void* ptr1_, const void* ptr2_, const void* ptr3_) const;
  bool pIsAllNull(const void* ptr1_, const void* ptr2_, const void* ptr3_) const;
  bool pIsAllNonNull(const void* ptr1_, const void* ptr2_, const void* ptr3_) const;
  bool pIsAllNonNull(const void* ptr1_, const void* ptr2_, const void* ptr3_, const void* ptr4_) const;
  bool pIsAllNull(const void* ptr1_, const void* ptr2_) const;
  bool pIsAllNonNull(const void* ptr1_, const void* ptr2_) const;
  //! \brief Returns false if all arguments are non-null or null.
  bool pXorPtrs(const void* ptr1_, const void* ptr2_) const;
  bool pXorPtrs(const void* ptr1_, const void* ptr2_, const void* ptr3_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  set<const IRExpr*> mUsedExprSet; //!< Set of expressions that are used in statements.
  Predicate* mParent;
}; // class PredicateDBCHelper

//! \brief Singleton Predicate object that is used to access predicate functions.
//! \note You should not create more than one instance of Predicate class.
extern Predicate pred;

#if RELEASEBUILD
#include "predicates_inline.cpp"
#endif

#endif


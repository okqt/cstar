// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __IRBUILDER_HPP__
#define __IRBUILDER_HPP__

#ifndef __HOSTTARGETTYPES_HPP__
#include "hosttargettypes.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif

//! \file irbuilder.hpp
//! \brief IR builder utility.
//! \todo L Doc: Check if doxygen is okay when processing multiline \code and \endcode and C++
//! style comments. The bug is reported as 342669 in bugzilla under doxygen product.

//! \todo Can we move IRTypeSetSort to a predicate helper class?
class IRTypeSetSort {
  public:
    bool operator() (const IRType* left, const IRType* right_) const;
}; // class IRTypeSetSort

//! \brief IRBuilder class is responsible from creation of all IR objects.
//!
//! Expressions are just created and returned.
//! \note First try to use IRBuilderHelper. If you cannot find what you need
//! in the helper class use this core class IRBuilder.
//! \sa IRBuilderHelper InsertContext
//! \todo H Design: How should we handle creation of objects from constructors or
//! from assignment statements?
//! \par Control Flow Graph Management
//! Control Flow Graph is managed by inserting, and moving statements.
//! If you want to add edges you should insert a statement with the desired
//! edge properties.
//! \par Basic Block Management
//! Basic blocks are added by splitting an existing basic block or control flow edge.
//! All other management is done by moving/inserting statements. Removal of
//! basic blocks are done in an optimization phase.
//! \attention Pay attention to the following for statement insertions:
//! Do not delete a statement when inserting a statement. Since there may be
//! references to that statement, in insert contexts. You may move the
//! statements it does not invalidate any insertion context.
//! \todo H Design: Perhaps it is better to have a different
//!       set of pattern building expressions.
//! \todo H Design: Create global pointer to Singleton objects. pred e.g. should be Singleton<Predicate>
class IRBuilder : public IDebuggable {
protected:
  //! \brief Creates an IRBuilder having no context.
  //! \note mDoOptims is true by default.
  //! \note This constructor is used by Singleton, the created object should be used indirectly.
  IRBuilder();
public: // Constructors
  //! \param [in] prog_ you can provide singleton irProgram.
  //! \sa Consts::cIRBDoOptims Consts::cIRBNoOptims
  //! \todo L Design: The reason prog_ param is present that when IRFunc
  //!       version is used without the dooptim bool option, it converts the pointer
  //!       to bool and uses that constructor which is almost always not the desired ctor.
  IRBuilder(IRProgram* prog_, bool doOptims_);
  //! \brief Creates an IRBuilder by setting module context.
  //! \param [in] doOptims_ If true it performs constant folding and algebraic
  //!             simplicications on expression creations.
  //! \sa Consts::cIRBDoOptims Consts::cIRBNoOptims
  IRBuilder(IRModule* cxtModule_, bool doOptims_);
  //! \brief Creates an IRBuilder by setting function context.
  //! \note It will automatically set the module context from \p cxtFunc_.
  //! \param [in] doOptims_ If true it performs constant folding and algebraic
  //!             simplicications on expression creations.
  //! \sa Consts::cIRBDoOptims Consts::cIRBNoOptims
  IRBuilder(IRFunction* cxtFunc_, bool doOptims_);
  //! \brief Creates an IRBuilder by setting basic block context.
  //! \note It will automatically set the function and module context from \p cxtBB_.
  //! \param [in] doOptims_ If true it performs constant folding and algebraic
  //!             simplicications on expression creations.
  //! \sa Consts::cIRBDoOptims Consts::cIRBNoOptims
  IRBuilder(IRBB* cxtBB_, bool doOptims_);
public: // Member functions
  //! \brief Creates a new module, sets it as new context and adds it to the program.
  IRModule* irbModule();
  //! \brief Creates a new function, sets it as new context and adds it to the module.
  //! \param[in] funcPrototype_ Prototype of the function. Must be allocated on the heap
  //!            and should not be deleted.
  //! \todo Done M Design: Should we provide auto_ptr mechanism for the owner ship of these
  //!       prototype pointers? -> No.
  //! \param[in] module_ parent module of the function.
  //! \param name_ \todo doc.
  //! \attention Sets the current function context to the returned function.
  //! \sa GetFuncContext, SetFuncContext
  IRFunction* irbFunction(const string& name_, IRTFunc* funcPrototype_, IRModule* module_, const SrcLoc& srcLoc_);
  //! \brief Creates a new basic block in the current functions' CFG.
  //! \note GetFuncContext should return non-null.
  //! \todo H Design: do we really need to give this interface to outside?
  IRBB* irbBB() const;
  //! \brief Removes \a stmt_ from its basic block, keeps the CFG up-to-date.
  //! \todo M Optim: Auto-do empty basic block optims.
  void irbRemoveStmt(IRStmt* stmt_) const;
  //! \brief Adds the statement to the list of to be removed statements.
  //! \note You can use this function during iteration.
  void irbToBeRemoved(IRStmt* stmt_);
  void irbToBeRemoved(IRBB* bb_);
  //! \brief Removes the statements that are collected by irbToBeRemoved.
  //! \todo M Optim: Auto-do empty basic block optims.
  void irbRemoveToBeRemovedStmts();
  //! \sa irbRemoveToBeRemovedStmts
  void irbRemoveToBeRemovedBBs();
  //! \brief Empties the to be removed list.
  //! \return Returns true if there was an item in the remove list.
  bool irbClearToBeRemovedStmts();
  //! \sa irbClearToBeRemovedStmts
  bool irbClearToBeRemovedBBs();
  //! \brief Inserts \a stmt at \a ic point, keeps the CFG up-to-date.
  //! \return Returns the parameter \p stmt_.
  IRStmt* irbInsertStmt(IRStmt* stmt_, const InsertContext& ic_) const;
  //! \brief Splits and links CFG of the basic block at insert context point.
  //! \param ic_ If \a ic_ place is \a ICAfter \a ic_ \a stmt stays in the predecessor.
  //!        If \a ic_ place is \a ICBefore \a ic_ \a stmt stays in the successor.
  //! \note It may insert a Jump if there is a need to split the basic block.
  //! \return Returns \a true if it split the basic block otherwise returns \a false.
  bool irbSplitBB(const ICStmt& ic_) const;
  //! \brief Inserts an empty basic block before or after a basic block.
  //! \param ic_ \p ic.place must be ICBefore or ICAfter,
  //!        (if ICBefore, new block is added before the basic block, similar for ICAfter).
  //! \return Returns the newly created empty basic block.
  //! \note CFG is updated as if \a ic.GetBB() is split, a single pred/succ relation is
  //!       inserted between new block and \a ic.GetBB().
  //! \pre You may not call this function for inserting before and after,
  //!      entry and exit basic blocks.
  IRBB* irbInsertBB(const ICBB& ic_) const;
  // ////////////////////////
  // Control Flow Graph
  // ////////////////////////
  //! \brief Inserts a successor edge if not already existing.
  //!
  //! \note First, If there is already a successor from \a fromBB to \a toBB
  //!       function does nothing and returns.
  //! \note Second, If fromBB already has a successor it will abort the application.
  //! \note There is no irbInsertPred or irbInsertEdge, since we
  //!       achieve the same result with this function.
  //! \return Returns \a true if a new successor is added, returns \a false if
  //!         there was a successor.
  bool irbInsertSucc(IRBB* fromBB_, IRBB* toBB_) const;
  // ////////////////////////
  // STATEMENTS
  // ////////////////////////
  // --------------------------
  // Pattern statement creation
  //! \todo Instead of using IRXXXInvalid perhaps we can use IRXXXAny?
  IRSJump* irbpsJump() const;
  //! \brief If pattern statement creation.
  IRSIf* irbpsIf(IRExpr* cond_) const;
  IRSReturn* irbpsReturn(IRExpr* expr_) const;
  IRSEval* irbpsEval(IRExpr* expr_) const;
  IRSNull* irbpsNull() const;
  IRSLabel* irbpsLabel() const;
  IRSAssign* irbpsAssign(IRExpr* lhs_, IRExpr* rhs_) const;
  IRSProlog* irbpsProlog() const;
  IRSEpilog* irbpsEpilog() const;
  IRSPCall* irbpsPCall(IRExpr* routine_) const;
  IRSFCall* irbpsFCall(IRExpr* routine_) const;
  IRSBuiltInCall* irbpsBICall(IRExpr* routine_) const;
  // Pattern Object creation
  IRObject* irbpoObject() const;
  IROParameter* irbpoParam() const;
  IROGlobal* irbpoGlobal() const;
  IROLocal* irbpoLocal() const;
  IROTmp* irbpoTmp() const;
  // --------------------------
  //! \brief Inserts a Jump statement to the context point.
  //! \param targetBB_ Target basic block of Jump.
  //! \param ic_ Insert context, the place of Jump statement itself.
  //!        Note that the owner basic block of Jump is split such that
  //!        this Jump becomes the last statement of owner.
  //! \return Returns the inserted IRSJump statement.
  IRSJump* irbsJump(IRBB* targetBB_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  IRSDynJump* irbsDynJump(IRExpr* targetExpr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts a return statement with return expression \a expr.
  //!
  //! Inserts a Jump statement to the \a exit block of the current function.
  //! \param expr_ An expression. For void functions this should be IRENull.
  //! \param ic_ Insert context.
  //! \return Returns the inserted IRSReturn statement.
  //! \pre The type of expression must match to the current context function's return type.
  IRSReturn* irbsReturn(IRExpr* expr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts an \a if statement targeting basic block \a trueBB and \a falseBB.
  //! \pre \p trueBB and \p falseBB must be different.
  //! \sa IRBuilderHelper::irbsIf
  //! \todo M Optim: Check the constness of condition and eliminate the branch
  //!       to one of then/else.
  IRSIf* irbsIf(IRExpr* cond_, IRBB* trueBB_, IRBB* falseBB_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  IRSBuiltInCall* irbsBIPCall(IRExprList* args_, IRBIRId birId_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \todo M Design: add the dbc check for return in and function return type match.
  IRSBuiltInCall* irbsBIFCall(IRObject* returnIn_, IRExprList* args_, IRBIRId birId_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts a call statement to a function.
  //! \return Returns the inserted IRSFCall statement.
  //! \pre funcExpr_ must have non-void return type.
  //! \pre if funcExpr_ is of type pointer to function type the return value
  //!       of function type must match to the returnIn_ object's type.
  //! \note Note that we do not really need to have a pointer to function type
  //!       for funcExpr_. Some front-ends do not have a strong type check.
  IRSFCall* irbsFCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, const 
    InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts a call statement to a procedure.
  //! \return Returns the inserted IRSPCall statement.
  //! \note You can create a procedure call for non-void functions.
  IRSPCall* irbsPCall(IRExpr* funcExpr_, IRExprList* args_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts an assignment statement.
  //! \return Returns the inserted IRSAssign statement.
  //! \pre rhs_ may not be a struct, use builtin memcpy.
  //! \pre rhs_ must be the reference type of lhs_ pointer type, e.g. *refT = refT.
  //! \pre lhs_ may not be an object, use IRBuilderHelper::irbsAssign.
  //! \sa IRBuilderHelper::irbsAssign
  IRSAssign* irbsAssign(IRExpr* lhs_, IRExpr* rhs_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts a special statement that is never optimized away and
  //!        evaluates the expression provided.
  //! \return Returns the inserted IRSEval statement.
  IRSEval* irbsEval(IRExpr* expr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts an epilog statement.
  //! \todo H Design: should we give a return statement like interface?
  //! \return Returns the inserted IRSEpilog statement.
  IRSEpilog* irbsEpilog(const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts a prolog statement.
  //! \return Returns the inserted IRSProlog statement.
  IRSProlog* irbsProlog(const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  /*! \brief Inserts a switch statement.
      \param val The value expression.
      \param caseStmts List of value and first statement pairs for switch cases.
             IRExprList must be list of constant values. IRStmt is the first statement
             after consequtive case labels ("1: 2: 3:") or a single case ("1:").
      Example:
      \code
      switch(x) {
        case 1:
        case 4: stmt1; break; // caseStmts[0] => pair<"1, 4", stmt1>
        case 7: stmt2; break; // caseStmts[1] => pair<"7", stmt2>
        default: stmt3;       // caseStmts[2] => pair<"", stmt3>
      }
      exitLabel:
      \endcode
      \param ic Insert context.
      \return Returns the inserted IRSSwitch switch statement.
  !*/
  IRSSwitch* irbsSwitch(IRExpr* val_, const vector<pair<IRExprList*, IRBB*> >& caseStmts_,
    const InsertContext& ic_, const SrcLoc& srcLoc_);
  //! \brief Inserts a label statement.
  //! \param name_ Name of the label. For local labels, a localizing
  //!        prefix will be added during assembly code generation.
  //!        Name \a name should be unique.
  //! \param ic_ Insertion context.
  //! \param isAddrTaken_ Should be true if address of this label is taken.
  //! \note If you want to create a local label use IRBuilderHelper::irbsLocalLabel.
  //! \note Insertion of a label does not necessarily cause a basic block split.
  IRSLabel* irbsLabel(const string& name_, bool isAddrTaken_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts a Null statement.
  //!
  //! Null statement may be present in cases like:
  //! \code while (1) /* this is Null statement */; \endcode
  IRSNull* irbsNull(const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  // ////////////////////////
  // EXPRESSIONS
  // ////////////////////////
  // ------------------------
  // Arithmetic operations
  // ------------------------
  // Code generator pattern building expressions
  //! \brief Creates a generic compare node, used by code generator patterns.
  //! \attention You should hardly need to create a generic compare node.
  IRECmp* irbpeCmp(IRExpr* left_, IRExpr* right_) const;
  // ------------------------
  IREAdd* irbeAdd(IRExpr* left_, IRExpr* right_) const;
  IRESub* irbeSub(IRExpr* left_, IRExpr* right_) const;
  IREMul* irbeMul(IRExpr* left_, IRExpr* right_) const;
  IREDiv* irbeDiv(IRExpr* left_, IRExpr* right_) const;
  IREMod* irbeMod(IRExpr* left_, IRExpr* right_) const;
  IRERem* irbeRem(IRExpr* left_, IRExpr* right_) const;
  IREQuo* irbeQuo(IRExpr* left_, IRExpr* right_) const;
  IREANeg* irbeANeg(IRExpr* expr_) const;
  // ------------------------
  // Misc. operations
  // ------------------------
  IRECast* irbeCast(IRType* castType_, IRExpr* expr_) const;
  //! \brief Creates a pointer plus offset expression.
  //! \param[in] ptr_ must be of type pointer.
  //! \param[in] offset_ must be an integer typed expression, the amount is in bytes.
  //! \return Return value is of type pointer (same reference type as \a ptr), IRTPtr.
  IREPtrAdd* irbePtrAdd(IRExpr* ptr_, IRExpr* offset_) const;
  //! \brief Creates pointer minus pointer expression.
  //! \note Use irbePtrAdd for pointer minus integer expressions.
  //! \sa irbePtrAdd
  IREPtrSub* irbePtrSub(IRExpr* left_, IRExpr* right_) const;
  IREAddrOf* irbeAddrOf(IRObject* obj_) const;
  //! \brief Creates an address of expression.
  //! We usually need address of non object expressions when member operator
  //! is used on structures.
  //! \todo M Design: there seems to be a redundancy to have both IRObject and
  //!       IRExpr version of this function, since IRObjects are also IRExprs.
  IREAddrOf* irbeAddrOf(IRExpr* obj_) const;
  //! \brief Creates a dereference expression node.
  //! \note \a ptr must be of type pointer to non-void.
  IREDeref* irbeDeref(IRExpr* ptr_) const;
  IREMember* irbeMember(IRExpr* base_, IROField* member_) const;
  //! \brief Creates a subscript operation.
  IRESubscript* irbeSubscript(IRExpr* array_, IRExpr* index_) const;
  //! \brief Creates a null expression.
  IRENull* irbeNull() const;
  // ------------------------
  // Bit operations
  // ------------------------
  //! \brief Creates an arithmetic shift right expression of \a numBits.
  IREAShiftRight* irbeAShiftRight(IRExpr* expr_, IRExpr* numBits_) const;
  //! \brief Creates a logical shift right expression of \a numBits.
  IRELShiftRight* irbeLShiftRight(IRExpr* expr_, IRExpr* numBits_) const;
  //! \brief Creates a shift left expression.
  //! \note There is no arithmetic or logical left shift they are same.
  IREShiftLeft* irbeShiftLeft(IRExpr* expr_, IRExpr* numBits_) const;
  //! \brief Creates a 'bitwise and' expression node.
  IREBAnd* irbeBAnd(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates a 'bitwise or' expression node.
  IREBOr* irbeBOr(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates a 'bitwise xor' expression node.
  IREBXOr* irbeBXOr(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates a 'bitwise negation' expression node.
  IREBNeg* irbeBNeg(IRExpr* expr_) const;
  // ------------------------
  // Logic operations
  // ------------------------
  //! \brief Creates a 'logical and' expression node.
  //! \return Return value is of type boolean, IRTBool.
  IRELAnd* irbeLAnd(IRExpr* l_, IRExpr* r_) const;
  //! \brief Creates a 'logical or' expression node.
  //! \return Return value is of type boolean, IRTBool.
  IRELOr* irbeLOr(IRExpr* l_, IRExpr* r_) const;
  //! \brief Creates a 'logical not' expression node.
  //! \return Return value is of type boolean, IRTBool.
  IRENot* irbeNot(IRExpr* e_) const;
  //! \brief Creates a greater than expression.
  //! \return Return value is of type boolean, IRTBool.
  IREGt* irbeGt(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates a less than expression.
  //! \return Return value is of type boolean, IRTBool.
  IRELt* irbeLt(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates an equality expression.
  //! \return Return value is of type boolean, IRTBool.
  IREEq* irbeEq(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates an inequality expression.
  //! \return Return value is of type boolean, IRTBool.
  IRENe* irbeNe(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates a greater than or equal expression.
  //! \return Return value is of type boolean, IRTBool.
  IREGe* irbeGe(IRExpr* left_, IRExpr* right_) const;
  //! \brief Creates a less than or equal expression.
  //! \return Return value is of type boolean, IRTBool.
  IRELe* irbeLe(IRExpr* left_, IRExpr* right_) const;
  //! \brief Returns the inverse of a relational expression.
  //! \note It uses recursive copies of operands of \p expr_.
  //! \pre expr_ must be of type IRECmp, or IREBoolConst.
  //! \sa Predicate::pIsCmp
  IRExpr* irbeInverseCmp(const IRExpr* expr_) const;
  //! \brief Returns an identity operand for a given expression.
  //! \pre expression must be of type ADD 0, MUL 1, DIV 1, SUB 0, LOR F, LAND T, BAND 1s,
  //!      BOR 0, ASHR 0, LSHR 0, BXOR 0, SHL 0.
  IRExpr* irbeIdentity(const IRExpr* expr_) const;
  //! \brief Returns a zero correcponding to a given type.
  //! \note for bool types it returns false constant.
  IRExpr* irbeZero(IRType* type_) const;
  //! \brief Creates an expression from given prototype and operands.
  //! \return returned expression may not be the same class as the given one 
  //!         depending on the IRBuilder optimizations.
  IRExpr* irbeFromBinary(const IREBinary* prototype_, IRExpr* left_, IRExpr* right_) const;
  // ------------------------
  // Constants
  // ------------------------
  //! \brief Creates an integer constant expression.
  //! \param type_ Type of constant
  //! \param value_ The value of constant. Must be within the range of number of
  //! bits of \a type.
  IREIntConst* irbeIntConst(tInt type_, tBigInt value_) const;
  //! \todo M Design: define the interface for creating integer constants. (Need a type -> Yes)?
  IREIntConst* irbeIntConst(IRTInt* type_, tBigInt value_) const;
  IREBoolConst* irbeBoolConst(bool value_) const;
  //! \brief Creates a floating point constant expression.
  //! \param type_ Type of constant
  //! \param value_ The value of constant. Must be within the range of number
  //! of bits of \a type.
  IRERealConst* irbeRealConst(tReal type_, tBigReal value_) const;
  IRERealConst* irbeRealConst(IRTReal* type_, tBigReal value_) const;
  //! \todo M Design: I guess the type is always a pointer? should we enforce here?
  IREAddrConst* irbeAddrConst(IRType* type_, IRObject* symbol_, tBigAddr offset_) const;
  IREAddrConst* irbeAddrConst(IRType* type_, IRSLabel* label_, tBigAddr offset_) const;
  IREAddrConst* irbeAddrConst(IRType* type_, tBigInt constValue_) const;
  IREStrConst* irbeStrConst(const string& value_) const;
  // ////////////////////////
  // TYPES
  // ////////////////////////
  //! \brief Returns an IRTBool type.
  //! \note If a bool type is not in symbol table if will be created.
  IRTBool* irbtGetBool() const;
  //! \brief Returns a new void type.
  IRTVoid* irbtGetVoid() const;
  IRTUnion* irbtGetUnion(const string& name_, const vector<IROField*>& fields_, tBigInt size_, hUInt16 alignment_) const;
  IRTStruct* irbtGetStruct(const string& name_, const vector<IROField*>& fields_, tBigInt size_, hUInt16 alignment_) const;
  IRTPtr* irbtGetVoidPtr() const {return irbtGetPtr(irbtGetVoid());}
  IRTArray* irbtGetArray(IRType* elemType_, tBigInt count_, hUInt16 alignment_) const;
  //! \sa cHasNoEllipses, cHasEllipses
  IRTFunc* irbtGetFunc(IRType* retType_,
    const vector<IRType*>& paramTypes_,
    bool hasEllipses_,
    CallingConvention* callConv_) const;
  //! \brief Returns an IR integer type given the size, maximum alignment, and signedness.
  //! \param size_ Size of integer in bits.
  //! \param alignment_ Maximum alignment of type in number of bits.
  //! \param sign_ A valid signedness for the type.
  //! \return Returns a valid IRTInt.
  //! \pre Target must have the specified integer type.
  //! \sa Predicate::pTargetHasInt
  IRTInt* irbtGetInt(hUInt16 size_, hUInt16 alignment_, IRSignedness sign_) const;
  //! \brief Return an integer type from a target integer.
  IRTInt* irbtGetInt(const tInt& targetInt_) const {
    return irbtGetInt(targetInt_.GetSize(), targetInt_.GetAlignment(), targetInt_.GetSign());
  } // IRBuilder::irbtGetInt
  //! \brief Returns an IR real type from a target real type, tReal.
  IRTReal* irbtGetReal(const tReal& targetReal_) const {
    return irbtGetReal(targetReal_.GetMantissa(), targetReal_.GetExponent(), targetReal_.GetAlignment());
  } // IRBuilder::irbtGetReal
  IRTPtr* irbtGetPtr(IRType* type_) const;
  //! \brief Returns an IR real type given the size of mantissa, exponent and maximum alignment.
  //! \param mantissa_ Minimum number of mantissa bits the type should have.
  //! \param exponent_ Minimum number of exponent bits the type should have.
  //! \param alignment_ Maximum alignment type should have.
  //! \return Returns a valid IRTReal.
  //! \pre Target must have the specified real type.
  //! \sa Predicate::pTargetHasReal
  IRTReal* irbtGetReal(hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_) const;
  // ////////////////////////
  // OBJECTS
  // ////////////////////////
  // Objects are typically created during the processing of front-end
  // declarations of modules, functions, statements, etc.
  //! \todo Done H Design: What about compiler created temporaries? -> see irobTmp.
  //! \brief Creates a global object and adds it to context module or function.
  //!
  //! If function context is present a static local variable of function
  //! is assumed. If function context is null then IROGlobal is added to
  //! the context module. \a type has a storage class specifier to signify
  //! file static objects.
  IROGlobal* irboGlobal(const string& name_, IRType* type_, IRLinkage linkage_) const;
  //! \brief Creates a temporary global object.
  //! Temporary global objects are used in initialization of local scoped
  //! aggregates. Locals are initialized by a memcpy from the globals address.
  IROGlobal* irboTmpGlobal(IRType* type_) const;
  //! \brief Creates and adds a local object to the current function.
  //! \note IROLocal objects are present in the debug information.
  //!       If you do not want debug information use irboTmp.
  //! \sa irboTmp
  IROLocal* irboLocal(const string& name_, IRType* type_) const;
  //! \brief Creates and adds a local object to the current function.
  IROLocal* irboLocal(const string& name_, IRType* type_, const Address& addr_) const;
  //! \brief Creates a relocation symbol.
  IRORelocSymbol* irboRelocSymbol(const string& name_, IRType* type_) const;
  //! \brief Creates a temporary variable.
  IROTmp* irboTmp(IRType* type_, const string& prefix_) const;
  //! \brief Creates a temporary variable and assigns its address to \p addr.
  IROTmp* irboTmp(IRType* type_, const Address& addr_, const string& prefix_) const;
  //! \brief Creates and adds a parameter object to the current function.
  IROParameter* irboParameter(const string& name_, IRType* type_) const;
  //! \brief Creates a structure field object.
  //! \sa IRTBitField
  IROField* irboField(const string& name_, IRType* type_, tBigInt offset_) const;
  IROBitField* irboBitField(const string& name_, IRType* type_, tBigInt offsetInBits_, hFSInt16 bitOffset_,
    hFSInt16 sizeInBits_, IRSignedness sign_) const;
  // ////////////////////////
  // Other Functions
  // ////////////////////////
  //! \brief Adds a guarding if to the loop if loop may not iterate at least once.
  //! Some optimizations can do more if a guard is added to a loop. A division
  //! may not be safe to be taken out of a loop if loop is not guaranteed to
  //! be executed once.
  //! \note The guard test is added to protect the preheader, i.e. preheader
  //!       is a successor of the guard.
  void irbAddGuard(IRLoop* loop_) const;
  //! \brief Sets the initializations of a global object.
  //! \note If the type of global is incomplete the type size is recalculated and set.
  void irbSetInits(IROGlobal* global_, IRExpr* inits_) const;
  //! \brief Swaps true and false case basic blocks of a given if.
  void irbSwapTrueFalseBBs(IRSIf* if_) const;
  IRBuilderHelper* GetHelper() const {return mIRHBuilder;}
  //! \brief Sets the context to \a stmt.
  //!
  //! \param stmt_ This statement becomes the reference statement.
  //! All the statements are inserted just after or before
  //! the statement \a stmt.
  //! \param isBefore_ if \a true, all the statements are inserted before
  //! the \a stmt. If \a isBefore is false statements are inserted after
  //! the statement \a stmt.
  //! \note IRBuilder statement creation functions (irbs*) automatically
  //! sets the context to the newly created statement.
  IRBuilder& SetStmtContext(IRStmt* stmt_, bool isBefore_);
  //! \brief Sets the \a func as current function.
  //! \sa irbFunction, GetFuncContext
  IRFunction* SetFuncContext(IRFunction* func_);
  //! \brief Returns the current function.
  //! \return May return 0 or the current function.
  //! \retVal 0 There is no current function.
  //! \sa SetFuncContext, irbFunction
  IRFunction* GetFuncContext() const;
  //! \brief Sets the \a module as current module.
  //! \sa irbModule
  IRModule* SetModuleContext(IRModule* module_);
  //! \brief Returns the current module.
  //! \return May return 0 or the current module.
  IRModule* GetModuleContext() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Changes the jump target of statements, keeps CFG correct.
  //! \pre \p anStmt must be a control flow statement.
  //! \pre \p anStmt may not be a return statement.
  bool irbRemapJumpTarget(IRStmt* anStmt_, IRBB* from_, IRBB* to_) const;
  //! \brief Changes the objects in the map to new ones.
  //! \return Returns the number of times the objects are replaced.
  hFUInt32 irbRemapObjs(IRStmt* stmt_, const map<IRObject*, IRObject*>& old2newObjs_) const;
  static const vector<IRType*>& smGetTypes() {return smAllTypes;}
  //! \param ir_ May be null.
  //! \pre you may not reassign a label using a previously used ir_ and tag_.
  //! \todo M Design: change the order of params, so that ir_ may be skipped, at the end.
  static void smirbAttachLabel(const IR* ir_, const string& tag_, const string& label_);
  //! \brief Returns a previously attached label.
  //! \pre Label must have been attached.
  static const string& smirbGetAttachedLabel(const IR* ir_, const string& tag_);
  //! \brief Sets the debug information entry builder.
  static void smSetDIEBuilder(DIEBuilder* dieBuilder_);
  //! \brief Notification that the expression is detached from IR.
  //! \note This function is almost always called by the core functions
  //!       you do not need to call it. For example, ReplaceChild calls it.
  //! \pre expr_ should not be already detached, e.g. pred.pIsDetached must be false for expr_.
  static void smirbDetached(IRExpr* expr_);
  //! \brief Notification that the expression is attached to IR.
  //! \note This function is almost always called by the core functions
  //!       you do not need to call it. For example, ReplaceChild calls it.
  static void smirbAttached(IRExpr* expr_);
  //! \brief Called when an expression is about to be replaced.
  //! \note IRBuilder may change the new expression to be replaced.
  //!       For example, if you replace conditional expression of an if
  //!       statement with a constant bool T it will change the expression in to
  //        (T == true), the new (T == true) is returned.
  static IRExpr* smirbOnReplaceWith(const IRExpr* old_, IRExpr* new_);
  //! \brief This function is called whenever an edge is changed in a CFG.
  //! \note Edge changes may happen when a basic block is deleted or added,
  //!       or a control flow statement is added/deleted/moved.
  static void smirbOnEdgeChange(CFG* cfg_);
private:
  //! \brief the basic function to get a new basic block.
  //! \note book keeping of basic block related information is done in this function.
  IRBB* getNewBB(CFG* cfg_) const;
  //! \brief Sets the associative group no of src_ to the nodes of dst_.
  static void smsetAssocGroup(const IRExpr* src_, IRExpr* dst_);
  //! \brief Clears the DFA sets of all basic blocks in the given cfg.
  static void smclearDFASets(CFG* cfg_);
  //! \brief Invalidates the domination information of cfg and its basic blocks.
  static void sminvalidateDominators(CFG* cfg_);
  //! \note You should not call this function, use IRBuilderHelper::irbsAssign.
  IRSAssign* irbsAssign(IRObject* object_, IRExpr* rhs_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
    return 0;
  } // IRBuilder::irbsAssign
  //! \brief Moves all predecessors of a basic block to another basic block.
  //! \note This function is to be used by IRBuilder internally.
  //! \return Returns true if \p from had any predecessor.
  //! \pre \p from may not be exit block if there is a return statement in the function.
  bool irbRemapPreds(IRBB* from_, IRBB* to_) const;
  //! \brief Updates the CFG of dynamic Jumps in \p func.
  //! When an address taken label is added or removed dynamic Jumps needs update.
  void updateDynamicJumps(IRFunction* func_) const;
  IROGlobal* getBuiltInObject(IRBIRId birId_) const;
  //! \brief This function is called after a new expression is created in one of irbe*.
  void onExprCreated(IRExpr* newExpr_) const;
private: // Member data
  set<IRStmt*> mToBeRemovedStmts;
  set<IRBB*> mToBeRemovedBBs;
  IRBuilderHelper* mIRHBuilder;
  static DIEBuilder* smDIEBuilder;
  static map<pair<const IR*, string>, string> smTaggedLabels;
  static map<hUInt64, IRTReal*> smRealTypes;
  static map<hUInt64, IRTInt*> smIntTypes;
  static vector<IRTStruct*> smStructTypes;
  static vector<IRTArray*> smArrayTypes;
  static vector<IRTUnion*> smUnionTypes;
  static vector<IRType*> smAllTypes;
  static set<IRType*, IRTypeSetSort> smTypes; //!< Keeps all the types.
  struct IRBContext {
    IRBContext() :
      mModule(0),
      mFunc(0),
      mBB(0),
      mStmt(0)
    {
    } // IRBContext::IRBContext
    IRModule* mModule;
    //! \brief Current function context.
    //! \sa IRBuilder::SetFuncContext IRBuilder::GetFuncContext IRBuilder::irbFunction
    IRFunction* mFunc;
    IRBB* mBB;
    IRStmt* mStmt;
  }; // struct IRBContext
  //! \brief Context of this IRBuilder.
  //! \pre You may not use member functions that set or get this context
  //!      using Singleton<IRBuilder>.
  IRBContext mContext;
  bool mDoOptims;
  virtual bool IsValid() const;
  static hFUInt32 smAssocGroupNoIndex;
private:
  friend class IRBuilderHelper;
  template<class T> friend class Singleton;
}; // class IRBuilder

//! \brief Helper class of IRBuilder.
//! \note Try to use the functions in this class rather than IRBuilder.
//! \sa IRBuilder InsertContext
class IRBuilderHelper : public IDebuggable {
public:
  IRBuilderHelper(const IRBuilder* irBuilder_);
  virtual ~IRBuilderHelper() {}
  //! \brief Inserts a Jump statement to the context point.
  //! \param targetStmt_ Target statement of Jump. Actually
  //!        Jumps have only basic block targets. So targetStmt must be the
  //!        first statement of its basic block. If this is not so
  //!        basic block is split first.
  //! \param ic_ Insert context, the pace of Jump statement itself.
  //!        Note that the owner basic block of Jump is split such that
  //!        this Jump becomes the last statement of owner.
  //! \return Returns the inserted IRSJump statement.
  //! \note It may create two predecessors of basic block of targetStmt_.
  //! \pre ic_ may not be an ICNull.
  IRSJump* irbsJump(IRStmt* targetStmt_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Creates a Jump with target at the statement before or after
  //!        the insert context statement.
  //! \param targetContext_ The statement context from which the target
  //!        will be determined.
  //! \param ic_ Insert point the Jump will be placed.
  //! \pre targetContext may not be before the first or after the last
  //!      statement of the statements basic block.
  //! \sa IRBuilder::irbsJump
  //! \note It may create two predecessors of basic block of targetContext_.
  //! \pre ic_ may not be an ICNull.
  IRSJump* irbsJump(const ICStmt& targetContext_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Creates and inserts an assignment in to \p object.
  //! \sa IRBuilder::irbsAssign
  IRSAssign* irbsAssign(IRObject* object_, IRExpr* rhs_, const InsertContext& ic_, const SrcLoc& srcLoc_) const {
    return mIRBuilder->irbsAssign(mIRBuilder->irbeAddrOf(object_), rhs_, ic_, srcLoc_);
  } // IRBuilderHelper::irbsAssign
  //! \brief Inserts an \a if statement.
  //! \return Returns the inserted IRSIf statement.
  //! \pre \p trueStmt and \p falseStmt must be different.
  //! \note It first splits the basic blocks of target statements if necessary.
  //! \sa irbsIf IRBuilder::irbsIf
  //! \pre ic_ may not be an ICNull.
  IRSIf* irbsIf(IRExpr* cond_, IRStmt* trueStmt_, IRStmt* falseStmt_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Creates an if statement that has edges to two newly created basic
  //!        blocks for true and false case.
  //! \note True and false case bbs has the successor that follows the flow of insert context.
  //! \pre You may not use an insert context that is end of bb, i.e.
  //!      pred.pIsEndOfBB must be false.
  //! \sa pred.pIsEndOfBB
  IRSIf* irbsIf(IRExpr* cond_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Creates increment by 1 assignment statement for a given object.
  IRSAssign* irbsIncrement(IRObject* obj_, const ICStmt& incCxt_, const SrcLoc& srcLoc_) const;
  //! \brief Creates decrement by 1 assignment statement for a given object.
  IRSAssign* irbsDecrement(IRObject* obj_, const ICStmt& incCxt_, const SrcLoc& srcLoc_) const;
  //! \brief Converts a Jump statement to if and inserts empty true and false
  //!        case basic blocks on the edge of the Jump.
  //! \pre \a bb_ must have a Jump at the end.
  //! \note Successor of \a bb_ becomes the successor of true and false case bbs.
  IRSIf* irbsConvertToIf(IRExpr* cond_, IRBB* bb_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts an if with targets specified by insert statement contexts.
  //! \sa irbsIf IRBuilder::irbsIf
  //! \pre ic_ may not be an ICNull.
  IRSIf* irbsIf(IRExpr* cond_, const ICStmt& trueStmtCxt_, const ICStmt& falseStmtCxt_,
    const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Inserts a local label with unique name having prefix \p prefix.
  //! \note Insertion of a label does not necessarily cause a basic block split.
  //! \sa IRBuilder::irbsLabel
  //! \pre ic_ may not be an ICNull.
  IRSLabel* irbsLocalLabel(const string& prefix_, bool isAddrTaken_, const InsertContext& ic_) const;
  //! \brief Returns the integer constant 1.
  IREIntConst* irbeGetIntConst_One() const;
  //! \brief Returns the integer constant 0.
  IREIntConst* irbeGetIntConst_Zero() const;
  //! \brief Creates an inequivalance expression with zero for a given expression.
  //! returns "expr != 0"
  IRExpr* irbeIsTrue(IRExpr* expr_) const;
  //! \brief Creates equivalance to zero expression
  //! returns "expr == 0"
  IRExpr* irbeIsFalse(IRExpr* expr_) const;
  //! \brief Creates an arithmetic shift right expression of \p numBits.
  IREAShiftRight* irbeAShiftRight(IRExpr* expr_, hUInt16 numBits_) const;
  //! \brief Creates a logical shift right expression of \p numBits.
  IRELShiftRight* irbeLShiftRight(IRExpr* expr_, hUInt16 numBits_) const;
  //! \brief Creates a shift left expression.
  IREShiftLeft* irbeShiftLeft(IRExpr* expr_, hUInt16 numBits_) const;
  //! \brief Creates an assignment to a new temporary with the given RHS expression.
  //! \return Returns the newly created temporary.
  //! \todo H Design: replace the old way with this call, search for the places.
  //! \pre ic_ may not be an ICNull.
  IROTmp* irbNewDef(const string& namePrefix_, IRExpr* expr_, const InsertContext& ic_, const SrcLoc& srcLoc_) const;
  //! \brief Creates use of an object.
  //! \sa IRBuilder::irbeAddrOf IRBuilder::irbeDeref
  //!     Typically creates 'deref' of 'addrof' of \p object.
  //! \return returns an IRExpr representing the use of \p object.
  IREDeref* irbeUseOf(IRObject* object_) const;
  IREDeref* irbeUseOf(IRExpr* object_) const;
  //! \brief Replaces uses of given object with the expression provided in the given statement.
  //! \return Returns the number of replacements made.
  hFUInt32 irbReplaceUses(IRStmt* inStmt_, IRObject* object_, IRExpr* newExpr_) const;
  //! \return Returns the number of replacements made.
  hFUInt32 irbReplaceUses(IRBB* inBB_, IRObject* object_, IRExpr* newExpr_) const;
  //! \return Returns the number of replacements made.
  hFUInt32 irbReplaceUses(IRLoop* inLoop_, IRObject* object_, IRExpr* newExpr_) const;
  //! \return Returns the number of replacements made.
  hFUInt32 irbReplaceUses(IRFunction* inFunc_, IRObject* object_, IRExpr* newExpr_) const;
  //! \brief Moves statements of a basic block to an insert context point.
  //! \param[in] fromStmt_ non-null, starting statement, can be the singleton IRSFirstOfBB
  //! \param[in] toStmt_ non-null, end statement, can be the singleton IRSLastOfBB
  //! \param[in] ic_ Must be non-null insert context.
  //! \pre \p from and \p to must be in the same basic block.
  //! \pre Valid \p from, \p to, \p ic.
  //! \pre \p from must precede \p to.
  //! \pre Non overlapping \p ic and \p from-\p to range.
  //! \note Note that CFG edges are to the basic blocks not to the statements,
  //!       therefore unless you move a CF statement CFG will not be updated (e.g.
  //!       because of the first statement).
  //! \return Returns the number of statements actually moved.
  //! \pre ic_ may not be an ICNull.
  hFUInt32 irbMoveStmts(IRStmt* fromStmt_, IRStmt* toStmt_, const InsertContext& ic_) const;
  virtual bool IsValid() const {
    BMDEBUG1("IRBuilderHelper::IsValid");
    REQUIRED(mIRBuilder != 0);
    EMDEBUG0();
    return true;
  } // IRBuilderHelper::IsValid
  //! \brief Moves a single statement to an insert context.
  //! \pre \p ic must have a valid basic block.
  //! \sa Predicate::pHasBB
  //! \todo add a Predicate for stmt, pIsConnected, for requirements?
  //! \pre ic_ may not be an ICNull.
  void irbMoveStmt(IRStmt* stmt_, const InsertContext& ic_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  const IRBuilder* mIRBuilder;
}; // class IRBuilderHelper

//! \brief irBuilder is set to the Singleton IRBuilder, which is an optimizing IRBuilder.
extern IRBuilder* irBuilder;
extern IRBuilderHelper* irhBuilder;

#endif


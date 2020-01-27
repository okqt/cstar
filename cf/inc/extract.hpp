// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __EXTRACT_HPP__
#define __EXTRACT_HPP__

#ifndef __ITERATORS_HPP__
#include "iterators.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif

static bool scDBCEnsureNonNull = true;

//! \brief Extract utility class library.
//!
//! \note Extract is a run-time singleton.
//! \par What goes in to Extract utility?
//! What Extract does in most of the cases could have been placed
//! in to a class as a member function. The difference
//! is in the time/space complexity of function. High complexity
//! functions goes in to Extract. Extract version of functions
//! also provide more functionality. For example, a class member
//! function may bail out in DBC. But the same function will
//! try to do more and shall not bail out because of lack of
//! information, instead Extract shall require more parameters.
//!
//! Extract members shall try to use extract methods, then class
//! member functions and then directly class data members.
//! Class functions shall not use extract methods, they will
//! stay primitive.
//!
//! Predicate follows a similar pattern. Extract and Predicate
//! may use each others functionality.
//! \sa extr
class Extract : public IDebuggable {
public:
  Extract();
  hFUInt32 eGetNumOfChildren(const IRExpr* expr_) const;
  hUInt32 eGetNumOfStmts(const IRBB* bb_) const;
  hUInt16 eGetNumOfSuccs(const IRBB* bb_) const;
  hUInt16 eGetNumOfPreds(const IRBB* bb_) const;
  //! \brief Returns the first statement of a basic block or null.
  //! \pre Non-null \p bb.
  IRStmt* eGetFirstStmt(IRBB* bb_) const;
  IRStmt* eGetFirstStmt(IRFunction* func_) const;
  //! \brief Returns the previous statement of a statement or null.
  //! \pre Non-null \p stmt.
  //! \return If \p stmt is the first statement returns null.
  IRStmt* eGetPrevStmt(IRStmt* stmt_) const;
  const IRStmt* eGetPrevStmt(const IRStmt* stmt_) const;
  //! \brief Returns the next statement of a statement or null.
  //! \pre Non-null \p stmt.
  //! \return If \p stmt is the last statement returns null.
  IRStmt* eGetNextStmt(IRStmt* stmt_) const;
  CGContext* eGetCGContext(const IRBB* bb_) const;
  CGContext* eGetCGContext(const IRStmt* stmt_) const;
  CGContext* eGetCGContext(const IRExpr* expr_) const {return eGetCGContext(eGetStmt(expr_));}
  const IRStmt* eGetNextStmt(const IRStmt* stmt_) const;
  //! \brief Returns the statement of a statement insert context.
  //! \return May return null if context is a basic block context.
  IRStmt* eGetStmt(const InsertContext& ic_) const;
  //! \brief Returns the virtual registers set of a given register set.
  RegSet eGetVirtRegs(const RegSet& regSet_) const;
  //! \brief Returns the ICBB interface of an insert context.
  //! \return May return null if \p ic is not a basic block context.
  const ICBB* eGetICBB(const InsertContext& ic_) const;
  //! \brief Returns the ICStmt interface of an insert context.
  //! \return May return null if \p ic is not a statement context.
  const ICStmt* eGetICStmt(const InsertContext* ic_) const;
  //! \brief Converts an insert context in to an insert statement context.
  //! \pre \p ic insert context must be convertable, i.e. must be
  //!      already a statement context, or a basic block context of a non-empty
  //!      basic block with ICEndOfBB, ICBegOfBB.
  //! \return Returns a valid ICStmt equivalent to ic.
  //! \todo M Design: Shall we add an argument place to create a
  //!       statement context with that place?
  ICStmt eConvToICStmt(const InsertContext* ic_) const;
  //! \brief Returns the function containing the given basic block.
  //! \pre Non-null \p bb. Function must be reachable.
  //! \return Non-null function.
  IRFunction* eGetFunc(const IRBB* bb_) const;
  IRFunction* eGetFunc(const IRLoop* loop_) const;
  //! \brief Returns the function containing the given expression.
  IRFunction* eGetFunc(const IRExpr* expr_) const;
  //! \brief Returns the function containing the given statement.
  //! \pre Non-null \p stmt. Function must be reachable.
  //! \return Non-null function.
  IRFunction* eGetFunc(const IRStmt* stmt_) const;
  //! \brief Returns the called function from a call statement.
  //! \pre Call must be direct call.
  //! \return May return null if the called function has no body.
  //! \sa Predicate::pIsDirect
  IRFunction* eGetCalledFunc(const IRSCall* stmt_) const;
  //! \brief Returns the function type that call expression is calling.
  IRTFunc* eGetFuncTypeOfCallExpr(const IRExpr* funcCall_) const;
  //! \brief Returns the last statement of a basic block or null.
  //! \pre Non-null \p bb.
  IRStmt* eGetLastStmt(const IRBB* bb_) const;
  //! \brief Returns the list of successor basic blocks of a given control flow statement.
  //! \pre Non-null \p stmt.
  //! \pre \p stmt must be a control flow statement.
  void eGetSuccsOfStmt(const IRStmt* stmt_, vector<IRBB*>& succs_) const;
  bool eGetSuccsOfBB(const IRBB* bb_, vector<IRBB*>& succs_) const;
  bool eGetPredsOfBB(const IRBB* bb_, vector<IRBB*>& preds_) const;
  //! \brief Returns the only predecessor of a basic block.
  //! \pre \p bb must have a single predecessor.
  IRBB* eGetPred(const IRBB* bb_) const;
  //! \brief Returns the only successor of a basic block.
  //! \pre \p bb must have a single successor.
  IRBB* eGetSucc(const IRBB* bb_) const;
  //! \brief Returns the local of a given function.
  //! \post Function may return Invalid local is the name is not found.
  //! \sa IRFunction::GetLocals
  IROLocal* eGetLocal(const IRFunction* func_, const string& name_) const;
  void eGetGlobalsAndAddrTakenLocalsAndParams(IRFunction* func_, set<IRObject*>& funcExtStorage_) const;
  //! \sa IRFunction::GetLocals, IRFunctionDecl::GetParams,
  //!     Extract::eGetLocalsAndParams, Extract::eGetParams
  const vector<IROLocal*>& eGetLocals(const IRFunction* func_) const;
  //! \sa IRFunction::GetLocals, IRFunctionDecl::GetParams,
  //!     Extract::eGetLocalsAndParams, Extract::eGetLocals
  const vector<IROParameter*>& eGetParams(const IRFunction* func_) const;
  //! \brief Returns the number of parameters of function.
  hFUInt32 eGetNumOfParams(const IRFunction* func_) const;
  //! \sa IRFunction::GetLocals, IRFunctionDecl::GetParams,
  //!     Extract::eGetLocals, Extract::eGetParams
  void eGetLocalsAndParams(const IRFunction* func_, vector<IRObject*>& localsAndParams_) const;
  //! \brief Returns the name of the function being called by a call statement.
  //! \note Calls may be indirect in that case <indirect> is returned.
  string eGetName(const IRSCall* call_) const;
  //! \brief Returns the offset portion of stack address.
  //! \pre addr_ must be of type AddrStack.
  tBigInt eGetOffsetAddrStack(const Address* addr_) const;
  const map<string, hFUInt32>& eGetFileNames(const SrcLoc& srcLoc_) const;
  const string& eGetLabelOf(const IRStmt* stmt_, CGFuncContext& context_) const;
  const string& eGetLabelOf(const IRFunction* func_, CGFuncContext& context_) const;
  const string& eGetLabelOf(const IRBB* bb_, CGFuncContext& context_) const;
  DwarfProgDIE* eGetDwarfDIE(const IRProgram* prog_) const;
  DwarfModuleDIE* eGetDwarfDIE(const IRModule* module_) const;
  DwarfFuncDIE* eGetDwarfDIE(const IRFunction* func_) const;
  DwarfStmtDIE* eGetDwarfDIE(const IRStmt* stmt_) const;
  DwarfBBDIE* eGetDwarfDIE(const IRBB* bb_) const;
  DwarfObjDIE* eGetDwarfDIE(const IRObject* object_) const;
  //! \return Returns the source location of statement or default constructed SrcLoc.
  SrcLoc eGetSrcLoc(const IRStmt* stmt_) const;
  //! \return Returns the source location of first statement or default constructed SrcLoc.
  SrcLoc eGetSrcLoc(const IRBB* bb_) const;
  //! \return Returns the source location of function or default constructed SrcLoc.
  SrcLoc eGetSrcLoc(const IRFunction* func_) const;
  //! \return Returns the declaration source location of object or default constructed SrcLoc.
  SrcLoc eGetSrcLoc(const IRObject* obj_) const;
  IRModule* eGetModule(const IRStmt* stmt_) const;
  //! \pre addrOf_ must be of type IREAddrOf.
  //! \param dbcEnsureNonNull_ please use cDBCEnsureNonNull if used.
  IRObject* eGetObjOfAddrOf(const IRExpr* addrOf_, bool dbcEnsureNonNull_ = false) const;
  //! \brief Returns an insert context that is before or after the given
  //!        statement but it does not refer to the given statement.
  InsertContext* eGetReplacementIC(const IRStmt* stmt_) const;
  //! \pre deref_ must be of type IREDeref.
  //! \param dbcEnsureNonNull_ please use cDBCEnsureNonNull if used.
  IRObject* eGetObjOfUse(const IRExpr* deref_, bool dbcEnsureNonNull_ = false) const;
  //! \brief Returns the number of parameters of function type of object.
  //! \pre \p obj_ must have function type.
  hFInt32 eGetParamCount(const IRObject* obj_) const;
  //! \brief returns the node that has null parent when walked up from this expression.
  //! \return The return value can be expr_ itself,
  //!         a statement or an object typed expression (in case expr_ is part
  //!         of an initialization).
  //! \pre expr_ may not be null, may not be Invalid.
  IRTree* eGetTopNode(IRExpr* expr_) const;
  //! \brief Returns the top expression node of a given expression.
  IRExpr* eGetTopNodeExpr(IRExpr* expr_) const;
  const IRTree* eGetTopNode(const IRExpr* expr_) const {return eGetTopNode(const_cast<IRExpr*>(expr_));}
  const IRExpr* eGetTopNodeExpr(const IRExpr* expr_) const {return eGetTopNodeExpr(const_cast<IRExpr*>(expr_));}
  //! \brief Returns the parent expression of a given expression.
  //! \return If there is no parent expression it returns null.
  //! \pre level_ must be greater or equal to 1.
  IRExpr* eGetParentExpr(const IRExpr* expr_, hFSInt32 level_ = 1) const;
  //! \brief Returns all definitions of all objects in a CFG.
  const set<IRStmt*>& eGetAllDefs(CFG* cfg_) const;
  //! \brief Returns the defining statements of a given object.
  const set<IRStmt*>& eGetAllDefsOfObj(CFG* cfg_, IRObject* obj_) const;
  //! \brief Returns all definition points of an object.
  const set<IRStmt*>& eGetAllDefsOfObj(IRDUUDChains* duudChains_, IRObject* obj_) const;
  //! \brief Returns all definition points of an object given the use.
  const set<IRStmt*>& eGetDefs(IRDUUDChains* duudChains_, IRStmt* useStmt_, IRObject* obj_) const;
  //! \brief Returns the targeted HW of a given function.
  //! \post Non null target.
  Target* eGetTarget(IRFunction* func_) const;
  Target* eGetTarget(IRLoop* loop_) const;
  const vector<IRLoop*>& eGetLoops(CFG* cfg_) const;
  //! \brief Returns the set of all predecessors, stops at a given basic block.
  //! \param [out] predSet_ includes the stopBB_ but may not bb_.
  void eGetPredsSet(const IRBB* bb_, IRBB* stopBB_, set<IRBB*>& predSet_) const;
  //! \pre parent of op_ must be binary expression.
  IRExpr* eGetBinOtherOp(const IRExpr* op_) const;
  IRBB* eGetPreHeader(IRLoop* loop_) const;
  //! \pre assign_ must be non-null.
  //! \note Assignment through pointers can define many objects.
  //! \return Returns true if there is any defined object. Furthermore,
  //!         if defines_ is empty it means assignment is through pointer
  //!         and it defines all the objects.
  void eGetDefsOfStmt(const IRSAssign* assign_, set<IRObject*>& defines_) const;
  void eGetDefsOfStmt(const IRSCall* call_, set<IRObject*>& defines_) const;
  //! \return Returns true if any object is found to be defined. Furthermore,
  //!         if defines_ is empty it means statement may define all the
  //!         objects, e.g. a call with a pointer parameter, assignment
  //!         through pointer.
  void eGetDefsOfStmt(const IRStmt* stmt_, set<IRObject*>& defines_) const;
  //! \brief Returns all uses of all objects in a CFG.
  const set<IRStmt*>& eGetAllUses(CFG* cfg_) const;
  //! \brief Returns all uses of a given object.
  //! \attention This function possibly recalculates the duud chains.
  const set<IRStmt*>& eGetAllUsesOfObj(CFG* cfg_, IRObject* obj_) const;
  const set<IRStmt*>& eGetAllUsesOfObj(IRDUUDChains* duudChains_, IRObject* obj_) const;
  //! \brief Returns all uses of a given object for the definition statement.
  const set<IRStmt*>& eGetUses(IRDUUDChains* duudChains_, IRStmt* defStmt_, IRObject* obj_) const;
  //! \brief Returns may use information of this statement.
  //! \pre uses_ must be empty.
  void eGetUsesOfStmt(const IRStmt* stmt_, set<IRObject*>& uses_) const;
  //! \brief Returns all deref of objects in a given expression tree.
  //! \pre derefs_ must be empty.
  void eGetDerefOfObjs(const IRExpr* expr_, vector<IREDeref*>& derefs_) const;
  //! \brief Returns the element type of a pointer typed expression.
  //! \pre \p expr_ must be of pointer type, i.e. Predicate::pIsPtr must be true.
  IRType* eGetElemType(const IRExpr* expr_) const;
  IRType* eGetElemType(const IRType* ptrType_) const;
  //! \pre obj_ must be of type IROGlobal.
  IRExpr* eGetInits(const IRObject* obj_) const;
  //! \brief Returns the sum of type size of all members of an expression list.
  //! \note It recurses in to the list elements.
  tBigInt eGetTotalTypeSize(const IRExprList* exprList_) const;
  //! \brief Returns the type size of given expression tree.
  //! \note if expr_ is of type IRExprList it calls
  //!       Extract::eGetTotalTypeSize(const IRExprList*) const;
  tBigInt eGetTotalTypeSize(const IRExpr* expr_) const;
  //! \pre expr_ must be non-null, non-Invalid, and top node may not be an object.
  IRStmt* eGetStmt(const IRExpr* expr_) const;
  CFG* eGetCFG(const IRExpr* expr_) const;
  CFG* eGetCFG(const IRStmt* stmt_) const;
  //! \brief Returns the return type of a call statement.
  //! \note If it is function type it returns the type of return in object.
  //!       If it is procedure type it returns void type.
  IRType* eGetRetType(const IRSCall* call_) const;
  hFInt32 eGetNumOfArgs(const IRSCall* call_) const;
  //! \brief Returns the hw register of a register.
  //! If run-time type of reg_ is HWReg reg_ is returned.
  //! If run-time type is VirtReg it checks for assigned HWReg,
  //! and returns the assigned register.
  //! \return It may return null.
  HWReg* eGetHWReg(Register* reg_) const;
  //! \brief Returns the reverse post order traversal of a CFG.
  //! \pre \p bbs_ must be empty.
  //! \pre \p cfg_ must be non-null.
  void eGetRevPostOrderBBs(CFG* cfg_, vector<IRBB*>& bbs_) const;
  const set<IRBB*>& eGetDominators(IRBB* bb_) const;
  const set<IRBB*>& eGetPostDominators(IRBB* bb_) const;
  //! \brief Returns the post order traversal of a CFG.
  //! \pre \p bbs_ must be empty.
  //! \pre \p cfg_ must be non-null.
  void eGetPostOrderBBs(CFG* cfg_, vector<IRBB*>& bbs_) const;
  //! \brief Returns the pre order traversal of a CFG.
  //! \pre \p bbs_ must be empty.
  //! \pre \p cfg_ must be non-null.
  void eGetPreOrderBBs(CFG* cfg_, vector<IRBB*>& bbs_) const;
  //! \brief Returns the post order traversal list of nodes under this expression tree.
  //! \attention Appends the expression nodes to \p nodes.
  void eGetPostOrderTraversal(IRExpr* root_, vector<IRExpr*>& nodes_) const;
  //! \brief Returns the pre order traversal list of nodes under this expression tree.
  //! \attention Appends the expression nodes to \p nodes.
  //! \todo Perhaps it is better to have an iterator for pre/post order
  //!       traversal so that the tree may not be modified.
  void eGetPreOrderTraversal(IRExpr* root_, vector<IRExpr*>& nodes_) const;
  //! \brief Returns the return type of a given function.
  IRType* eGetRetType(const IRFunction* func_) const;
  //! \brief Returns the module of a given function.
  IRModule* eGetModule(const IRFunction* func_) const;
  CallingConvention* eGetCallConv(const IRFunction* func_) const;
  //! \brief Returns null if type of \p expr is not of pointer type.
  IRTPtr* eGetPtrType(IRExpr* expr_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Returns an iterator that traverses all the statements in a function.
  //! \note The statements are ordered in the physical basic block order.
  FwdListIterator<IRStmt*> eGetStmtIter(IRFunction* func_) const;
  //! \brief Returns an iterator that traverses all the statements in a loop body.
  //! \note The body does not include preheader.
  ListIterator<IRStmt*> eGetStmtIter(IRLoop* loop_) const;
  //! \brief Returns the only child of a statement.
  //! \pre There must be exactly one child of the statement, e.g.
  //!      you may not call this function for assignments.
  IRExpr* eGetChild(const IRStmt* stmt_) const;
  //! \pre expr_ must be binary.
  IRExpr* eGetRightExpr(const IRExpr* expr_) const;
  //! \pre expr_ must be binary.
  IRExpr* eGetLeftExpr(const IRExpr* expr_) const;
  //! \brief Returns the set of basic blocks that are in the path of execution
  //!        from one basic block to another.
  //! \note Destination basic block need not be reachable from the source,
  //!       an empty set is returned in this case.
  //! \pre bbs_ must be empty.
  //! \pre both source and destination must be in the same cfg.
  void eGetBBsInPath(IRBB* fromBB_, IRBB* toBB_, set<IRBB*>& bbs_) const;
  //! \brief Returns the basic blocks of a given loop that has an edge to the header block.
  //! \pre bbs_ must be empty.
  //! \pre loop_ must be non-null.
  void eGetBackEdgeBBs(IRLoop* loop_, vector<IRBB*>& bbs_) const;
  //! \brief Returns the only exit block of a given loop.
  //! \pre Loop must have exactly one exit basic block, IRLoop::GetExitBBCount.
  IRBB* eGetExitBB(IRLoop* loop_) const;
  //! \brief Returns the statements of a basic block.
  void eGetStmts(const IRBB* bb_, vector<IRStmt*>& stmts_) const;
  //! \brief Returns the basic block of a cfg.
  void eGetBBs(const CFG* cfg_, vector<IRBB*>& bbs_) const;
private:
  static hFSInt8 smRefCount;
}; // class Extract

//! \brief Singleton Extract object that is used to access extraction functions.
extern Extract extr;

#endif // __EXTRACT_HPP__

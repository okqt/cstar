// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __IRS_HPP__
#define __IRS_HPP__

#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif
#ifndef __HOSTTARGETTYPES_HPP__
#include "hosttargettypes.hpp"
#endif
#ifndef __ITERATORS_HPP__
#include "iterators.hpp"
#endif
#ifndef __VISITOR_HPP__
#include "visitor.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __DEBUGINFO_HPP__
#include "debuginfo.hpp"
#endif

//! \file irs.hpp
//! \brief This file contains the definitions of all intermediate representation classes.
//! \note You may add as many friend classes as you like if they are helper classes of IRBuilder,
//! Predicate, and Extract classes.

//! \addtogroup weekly_todo
//! \todo H Design: Complete the IRType derived classes.

//! \todo L Syntax: Change the "Other Expressions" comment appropriately
//!       for each expression type below.

//! \todo M Doc: Add see also items to IRBuilder, and IRBuilderHelper in to the class
//!       definitions.

//! \todo M Design: Regularly control dynamic_casts and convert them to static casts.
//! \todo L Design: Add virtual destructors to classes.

//! \todo M Design: Add what should be done, step by step when a new IRENode
//!       is added, e.g. for IREPtrSub add ExprVisitor interface.

//! \defgroup hirstatements HIR Statements
//! IR constructs related to statements.

//! \defgroup hirexpressions HIR Expressions
//! IR constructs related to expressions.

//! \defgroup hirtypes HIR Types
//! IR constructs related to HIR types.

//! \defgroup hir HIR Overview
//!
//! \todo H Doc: Improve HIR overview text.
//!
//! HIR must be able to represent all the features of different languages.
//! Therefore, types prototypes and all must be representable.

//! \brief Base class for all IR constructs, e.g. expressions,
//! statements, functions, types.
class IR : public IDebuggable {
public:
  IR() {}
  virtual ~IR() {}
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
}; // class IR

//! \brief Base class for IRExpr and IRStmt, gives tree like interface to them.
class IRTree : public IR {
protected:
  IRTree(const Invalid<IRTree>* inv_) {}
public:
  IRTree() {}
  IRTree(const IRTree* tree_) {
    for (hFSInt32 c(0); c < eIRTMLast; ++ c) {
      mIsMarked[c] = tree_->mIsMarked[c];
    } // for
  } // IRTree::IRTree
  virtual ~IRTree() {}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // IRTree::ReplaceChild
  virtual bool GetChildren(vector<IRExpr*>& children) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return false;
  } // IRTree::GetChildren
  virtual IRTree* GetRecCopy() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // IRTree::GetRecCopy
  IRExpr* GetRecCopyExpr() const;
  IRStmt* GetRecCopyStmt() const;
  virtual IRTree* GetParent() const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
    return 0;
  } // IRTree::GetParent
  void SetMark(eIRTreeMark what_ = eIRTMGeneric) {mIsMarked[what_] = true;}
  void SetMark2(bool mark_, eIRTreeMark what_ = eIRTMGeneric) {mIsMarked[what_] = mark_;}
  void ResetMark(eIRTreeMark what_ = eIRTMGeneric) {mIsMarked[what_] = false;}
  //! \warning Expressions are not initially marked so be sure to first use SetMark or ResetMark.
  bool IsMarkTrue(eIRTreeMark what_ = eIRTMGeneric) const {return mIsMarked[what_] == true;}
  bool IsMarkFalse(eIRTreeMark what_ = eIRTMGeneric) const {return mIsMarked[what_] == false;}
  bool HasMarked(eIRTreeMark what_ = eIRTMGeneric) const {return mIsMarked[what_].isUnknown() == false && mIsMarked[what_].IsValid() == true;}
private:
  //! \brief Can be used by anatrops it is a scratch variable so be sure to initialize it before the use.
  tristate mIsMarked[eIRTMLast];
private:
  friend class CFG;
  friend class IRBuilder;
  friend class Extract;
  friend class Predicate;
}; // class IRTree

// //////////////////////////
// TYPES
// //////////////////////////
//! \ingroup hirtypes
//! \brief Base class of all types.
class IRType : public IR {
protected:
  IRType(const Invalid<IRType>* inv_) :
    mSize(0),
    mAlignment(0)
  {
  } // IRType::IRType
public:
  //! \param size_ type size in bits.
  //! \param alignment_ alignment of type in bits.
  IRType(tBigInt size_, hUInt16 alignment_) :
    mSize(size_),
    mAlignment(alignment_)
  {
    BMDEBUG3("IRType::IRType", &mSize, mAlignment);
    EMDEBUG0();
  } // IRType::IRType
  virtual ~IRType() {}
  //! \brief Alignment of type in bits.
  virtual hUInt16 GetAlignment() const {return mAlignment;}
  //! \brief Type size is in bits.
  virtual tBigInt GetSize() const {return mSize;}
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  tBigInt mSize; //!< size of type, in bits
  hUInt16 mAlignment; //!< Alignment, in bits.
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
}; // class IRType

//! \ingroup hirtypes
//! \brief Function pointer type.
class IRTFunc : public IRType {
public:
  //! \brief Constructor of function types.
  //! \param retType_ return type of function types.
  //! \param paramTypes_ Ordered list of parameter types.
  //! \param isVarArg_ Should be \a true if function is a variable
  //! \param callConv_ \todo doc.
  //! argument, i.e. in C "..." is used.
  //! \post \a this must be valid.
  IRTFunc(IRType* retType_, const vector<IRType*>& paramTypes_, bool isVarArg_, CallingConvention* callConv_);
  //! \brief Overriding of base class member GetSize.
  //! \attention You should not call GetSize of IRTFunc.
  //! \todo M Design: How to put DBC for execution paths that
  //!       should not have been reached.
  virtual tBigInt GetSize() const;
  virtual ~IRTFunc() {}
  virtual bool IsValid() const;
  IRType* GetReturnType() const {return mRetType;}
  IRType* GetParamType(hUInt16 index_) const {return mParamTypes[index_];}
  bool IsVarArg() const {return mIsVarArg == true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Optimizations may want to change the default calling convention
  //!        for a function.
  void SetCallConv(CallingConvention* callConv_) {
    REQUIREDMSG(callConv_ != 0, ieStrNonNullParam);
    mCallConv = callConv_;
  } // IRTFunc::SetCallConv
  CallingConvention* GetCallConv() const {
    REQUIREDMSG(mCallConv != 0, ieStrNonNullParam);
    return mCallConv;
  } // IRTFunc::GetCallConv
  hFInt32 GetNumOfParams() const {return mParamTypes.size();}
private:
  CallingConvention* mCallConv; //!< Could be invalid for built-in functions.
  IRType* mRetType; //!< Return value of function type.
  vector<IRType*> mParamTypes; //!< Ordered list of parameter types.
  bool mIsVarArg; //!< \a True if function is variable argument, otherwise \a false.
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
}; // class IRTFunc

//! \ingroup hirtypes
class IRTPtr : public IRType {
protected:
  IRTPtr(IRType* refType_, tBigInt size_, hUInt16 alignment_);
public:
  IRTPtr(IRType* refType_);
  virtual bool IsValid() const;
  virtual IRType* GetRefType() const {return mRefType;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  IRType* mRefType;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
}; // class IRTPtr

//! \ingroup hirtypes
class IRTArray : public IRType {
public:
  //! \param elemType_ type of the elements.
  //! \param lowerBound_ minimum index value, can be -1 if information is not available.
  //! \param upperBound_ maximum index value, can be -1 if information is not available.
  //! \param count_ number of elements.
  //! \param alignment_ alignment in bits.
  IRTArray(IRType* elemType_, hBigInt lowerBound_, hBigInt upperBound_, tBigInt count_, hUInt16 alignment_);
  IRTArray(IRType* elemType_, tBigInt count_, hUInt16 alignment_);
  virtual ~IRTArray() {}
  virtual bool IsValid() const;
  virtual tBigInt GetSize() const {return mCount * mElemType->GetSize();}
  IRType* GetElemType() const {return mElemType;}
  hBigInt GetLowerBound() const {return mLowerBound;}
  hBigInt GetUpperBound() const {return mUpperBound;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  tBigInt GetCount() const {return mCount;}
private:
  hBigInt mLowerBound; //!< Lower bound of array index, usually zero.
  hBigInt mUpperBound; //!< Upper bound of array index.
  //! \brief Count may be modified later when mIsIncomplete is true.
  //! \note Usually initializations set the size of incomplete types,
  //!       so an initialization may cause modification of this value.
  tBigInt mCount;
  //! \brief Set to true if the count is zero in the ctors.
  bool mIsIncomplete;
  IRType* mElemType;
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IRTArray

//! \ingroup hirtypes
class IRTLabel : public IRType {
public:
  IRTLabel();
  virtual bool IsValid() const {return true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
}; // class IRTLabel

//! \ingroup hirtypes
class IRTBool : public IRType {
public:
  //! \todo M Design: can we provide a fast integer type here, from target?
  //! \todo M Design: Use byte size instead of fixed 8.
  IRTBool();
  virtual bool IsValid() const {return true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
}; // class IRTBool

//! \ingroup hirtypes
class IRTInt : public IRType {
public:
  //! \param size_ Must be in bits.
  //! \param alignment_ Must be in bits.
  //! \param sign_ Must be IRSSigned or IRSUnsigned.
  IRTInt(hUInt16 size_, hUInt16 alignment_, IRSignedness sign_) :
    IRType(size_, alignment_),
    mSign(sign_)
  {
    BMDEBUG1("IRTInt::IRTInt");
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
    EMDEBUG0();
  } // IRTInt::IRTInt
  virtual bool IsValid() const;
  IRSignedness GetSign() const {return mSign;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  IRSignedness mSign; //!< Sign of the integer, it can be IRSigned or IRSUnsigned.
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IRTInt

//! \ingroup hirtypes
class IRTVoid : public IRType {
public:
  IRTVoid() :
    IRType(0, 0)
  {
    BMDEBUG1("IRTVoid::IRTVoid");
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
    EMDEBUG0();
  } // IRTVoid::IRTVoid
  virtual bool IsValid() const {
    return true;
  } // IRTVoid::IsValid
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IRTVoid

//! \ingroup hirtypes
//! \todo M Design: Implement the incomplete case, i.e. size is zero and
//!       initializations later on sets the size.
class IRTStruct : public IRType {
public:
  //! \param fields_ Ordered list of fields.
  //! \param size_ size of the struct, (sizeof size).
  //! \param alignment_ alignment of the struct.
  IRTStruct(const string& name_, const vector<IROField*>& fields_, tBigInt size_, hUInt16 alignment_);
  //! \todo M Design: Have a look at the constraints of C language and apply them here.
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  void SetFields(const vector<IROField*>& fields_) {mFields = fields_;}
  void GetFields(vector<IROField*>& fields_) const {fields_ = mFields;}
  const vector<IROField*>& GetFields() const {return mFields;}
  const string& GetName() const {return mName;}
private:
  vector<IROField*> mFields;
  string mName;
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IRTStruct

//! \ingroup hirtypes
class IRTUnion : public IRTStruct {
public:
  IRTUnion(const string& name_, const vector<IROField*>& fields, tBigInt size, hUInt16 alignment);
  //! \todo M Design: Have a look at the constraints of C
  //!    language and place them here.
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IRTUnion

//! \ingroup hirtypes
class IRTReal : public IRType {
public:
  //! \param size_ size in bits, usually sum of \p exponent_ and \p mantissa_.
  //! \param alignment_ minimum alignment of type
  //! \param mantissa_ number of mantissa bits.
  //! \param exponent_ number of exponent bits.
  IRTReal(tBigInt size_, hUInt16 alignment_, hUInt16 mantissa_, hUInt16 exponent_);
  virtual bool IsValid() const;
  virtual hUInt16 GetExponent() const {return mExponent;}
  virtual hUInt16 GetMantissa() const {return mMantissa;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  hUInt16 mMantissa;
  hUInt16 mExponent;
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IRTReal

//! \brief Insert context, a point in a basic block, before or after a statement.
//!
//! \sa IRBuilder IRBuilderHelper
//! \todo Done M Design: use of auto_ptr has become ugly, clean it up.
//! \todo Done H Design: Seriously split the insert context in to two, Stmt and BB.
//!       BB will have only a basic block member, Stmt will have only an stmt.
class InsertContext : public IDebuggable {
protected:
  InsertContext(const Invalid<InsertContext>& inv_);
  InsertContext();
  InsertContext(IRBB* bb_, ICPlace place_);
public:
  //! \brief Returns the \p place parameter of insertion.
  //! \pre \p place may not be ICInvalid.
  ICPlace GetPlace() const {
    REQUIRED(mPlace != ICInvalid);
    return mPlace;
  } // InsertContext::GetPlace
  virtual bool IsValid() const;
  //! \brief Returns the basic block of the insert context.
  virtual IRBB* GetBB() const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
    return 0;
  } // InsertContext::GetBB
protected:
  ICPlace mPlace;
  IRBB* mBB;
private:
  //! \attention Do not call this function.
  virtual char* debug(const DTContext& context_, ostream& toStr_) {
    REQUIRED(0);
    return "InsertContext::error";
  } // InsertContext::debugTrace
private:
  friend class Extract;
}; // class InsertContext

//! \brief Basic block insert context object.
class ICBB : public InsertContext {
public:
  //! \brief Provide the default constructor to be used along with an assignment.
  //! \post The object is invalid.
  //! \pre \p invalid must be ICInvalid.
  ICBB(ICPlace invalid_);
  //! \brief Constructs an insert context from a basic block and a place.
  //! \post Valid this.
  ICBB(IRBB* bb_, ICPlace place_);
  ICBB(const ICBB& ic_);
  ICBB& operator = (const ICBB& ic_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool IsValid() const;
  IRBB* GetBB() const;
}; // class ICBB

//! \brief Statement insert context object.
class ICStmt : public InsertContext {
protected:
  ICStmt(const Invalid<ICStmt>* inv_);
public:
  //! \brief Provide the default constructor to be used along with an assignment.
  //! \post The object is invalid.
  //! \pre \p invalid must be ICInvalid.
  ICStmt(ICPlace invalid_);
  //! \brief Constructs an insert context from a statement and a place.
  //! \post Valid this.
  ICStmt(IRStmt* stmt_, ICPlace place_);
  ICStmt(const ICStmt& ic_);
  ICStmt& operator = (const ICStmt& ic_);
  virtual bool IsValid() const;
  //! \brief Returns the statement of the insert context.
  IRStmt* GetStmt() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Returns the statement's current basic block.
  //! \attention As stmt of insert context may move from bb to bb this function
  //!            may return different bb's at different calls.
  IRBB* GetBB() const;
private:
  //! \brief When present ICAfter/ICBefore signifies the location of insertion.
  //! it must be a physical statement (i.e. not before/after first/last stmt of basic block).
  IRStmt* mStmt;
}; // class ICStmt

class ICNull : public InsertContext {
protected:
  ICNull(const Invalid<ICNull>* inv_) {}
public:
  ICNull() {}
  virtual bool IsValid() const {return true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class ICNull

//! \brief Contains the definition-use, use-definition chains for a function.
//! \note this data structure is obtained from a CFG. It is built by CFG.
class IRDUUDChains : public IDebuggable {
public: // Member types
  typedef map<IRStmt*, set<IRStmt*> > Defs2UsesStmt;
  typedef map<IRStmt*, set<IRStmt*> > Uses2DefsStmt;
public:
  IRDUUDChains(CFG* cfg_) :
    mCFG(cfg_)
  {
  } // IRDUUDChains::IRDUUDChains
  virtual ~IRDUUDChains() {}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private: // Member data
  set<IRStmt*> mAllUsesStmt;
  map<IRObject*, set<IRStmt*> > mObj2AllUses;
  map<IRObject*, Uses2DefsStmt> mObj2Uses2DefsStmt;
  set<IRStmt*> mAllDefsStmt; //!< Holds all the definition points of the function.
  map<IRObject*, set<IRStmt*> > mObj2AllDefs; //!< Holds all the definition points of each object.
  //! \brief Holds all the objects that are defined in this function, it also maps 
  //!        them to the definition and uses.
  map<IRObject*, Defs2UsesStmt> mObj2Defs2UsesStmt; 
  CFG* mCFG; //!< The CFG that this definition use chain belongs to.
private:
  friend class CFG;
  friend class Predicate;
  friend class Extract;
}; // class IRDUUDChains

//! \ingroup hirexpressions
//! \brief Base class for all expressions.
class IRExpr : public IRTree {
protected:
  IRExpr(const Invalid<IRExpr>* inv_) {}
public:
  IRExpr(IRType* type_);
  virtual ~IRExpr() {}
  virtual bool IsValid() const;
  IRType* GetType() const {return mType;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Returns a copy of the expression tree under this expression.
  //! \par Why recursive copies?
  //! We make use of recursive copies since often in the optimizations we
  //! delete expressions that are redundant. If we keep sharing the
  //! expressions there is a danger of delete an expression in one statement
  //! and making some other statement that use the same expression invalid.
  //! By making recursive copies we can delete any statement or expression
  //! with the peace of mind. Also some expressions may get decorated in some
  //! statement context, this would be too messy if we would not create
  //! recursive copies.
  virtual IRTree* GetRecCopy() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // IRExpr::GetRecCopy
  //! \brief Returns the child nodes of this expression.
  //! \return Returns true if there is any child.
  //! \note This function is a tree-like interface of expressions.
  //! Binary operations have two, unary operations have one children.
  //! \pre \p children must be empty.
  virtual bool GetChildren(vector<IRExpr*>& children_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return false;
  } // IRExpr::GetChildren
  //! \brief Must replace old child expression with the new one.
  //! \pre \p old_ must be one of the child.
  //! \pre \p new_ must be non-null.
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // IRExpr::ReplaceChild
  //! \brief Replaces this expression with the one provided.
  //! \return Since the input expression may get changed, the new expression is returned.
  //! The return value changes for example when a bool use is condition of an
  //! 'if' statement. The returned expression is 'boolvar == true'.
  //! \todo M Design: Check if we should set the parent of this to null.
  IRExpr* ReplaceWith(IRExpr* replacement_);
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  virtual IRTree* GetParent() const {
    BMDEBUG1("IRExpr::GetParent");
    ENSUREMSG(mParent != 0, ieStrMustEnsureNonNull);
    EMDEBUG1(mParent);
    return mParent;
  } // IRExpr::GetParent
  IRStmt* GetParentStmt() const;
  IRExpr* GetParentExpr() const;
  //! \todo M Design: somehow could not made this protected.
  void SetParent(IRTree* parent_) {
    BMDEBUG2("IRExpr::SetParent", parent_);
    //! \todo M Design: Add DBC to parent.
    mParent = parent_;
    EMDEBUG0();
  } // IRExpr::SetParent
  hFUInt32 GetAssocGroupNo() const {return mAssocGroupNo;}
private:
  //! \brief Can be overridden by derived classes if expression can be folded,
  //!        e.g. if any operand is a proper constant.
  //! \return Must return false if expression may not be constant folded,
  //!         otherwise it returns true and places a replacement expression in foldedExpr_.
  virtual bool constFold(IRExpr*& foldedExpr_) {
    return false;
  } // IRExpr::constFold
private:
  IRTree* mParent; //!< May be null.
  IRType* mType;
  //! \brief Associative operations (+, *, etc.) that are operands gets the
  //! same group number.
  //! \note Actually subtractions that are operands of addition also gets the
  //!       same group number, e.g. (a - b - c + d) both '-' has the same number as '+'.
  //!       because we can always represent a SUB with ADD of NEG.
  //! \note Group number '0' is special and used for not same with any other
  //!       mainly used for '/', '%' like.
  //! \note Group numbers are kept up-to-date by IRBuilder.
  hFUInt32 mAssocGroupNo;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
  friend class ATOConstFold;
}; // class IRExpr

//! \ingroup hirexpressions
//! \brief Ordered list of expressions.
//! \todo M Design: Shall we lock addition for constructors accepting expressions?
class IRExprList : public IRExpr {
public:
  IRExprList();
  IRExprList(IRExpr* expr_);
  IRExprList(IRExpr* expr0_, IRExpr* expr1_);
  IRExprList(IRExpr* expr0_, IRExpr* expr1_, IRExpr* expr2_);
  IRExprList& AddExpr(IRExpr* expr_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  //! \brief You can receive the added expression by GetChildren.
  virtual bool GetChildren(vector<IRExpr*>& children_) const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  //! \todo M Design: Would it be better to follow stl standard function names
  //!       for framework objects, i.e. iso IsEmpty, empty, GetSize, size?
  bool IsEmpty() const {return mExprList.empty();}
  virtual hFInt32 GetSize() const {return mExprList.size();}
public: // Member data
  static IRTVoid* exprListVoidType;
private:
  vector<IRExpr*> mExprList;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
}; // class IRExprList

//! \ingroup hirstatements
//! \brief Base class for all the statements.
//! \note IRBB-IRStmt has aggregation relation.
class IRStmt : public IRTree {
protected:
  IRStmt(const IRStmt& stmt_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRStmt(const Invalid<IRStmt>* inv_);
public:
  //! \brief Constructor.
  IRStmt(IRBB* parentBB_, IRStmt* prev_, IRStmt* next_, DIEBase* die_);
  virtual ~IRStmt() {}
  virtual IRBB* GetBB() const;
  //! \brief Design by contract ISVALID implementation for this class.
  virtual bool IsValid() const;
  //! \brief Returns the previous statement in the basic block or 0.
  //! \return Returns the previous statement if not the first statement
  //! of its parent basic block. If it is the first statement of its
  //! basic block it returns 0.
  virtual IRStmt* GetPrevStmt() const;
  //! \brief Returns the next statement in the basic block or 0.
  //! \return Returns the next statement if not the last statement
  //! of its parent basic block. If it is the last statement of its
  //! basic block it returns 0.
  virtual IRStmt* GetNextStmt() const;
  void SetParent(IRBB* parent_);
  void SetPrevStmt(IRStmt* prev_);
  void SetNextStmt(IRStmt* next_);
  //! \brief Invalidates the object.
  void Invalidate();
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Returns the top level expressions of this statement.
  //! \pre \p exprs must be empty.
  //! \return Returns false if there is no top level expression.
  //! Example of top level expressions: condition expression of IRSIf,
  //! LHS and RHS of IRSAssign.
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return false;
  } // IRStmt::GetChildren
  //! \brief Must replace old top level expression with the new one.
  //! \pre \p old_ must be one of the top level.
  //! \pre \p new_ must be non-null.
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // IRExpr::ReplaceChild
  virtual IRStmtType GetStmtType() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return IRSTInvalid;
  } // IRStmt::GetStmtType
  //! \brief Creates a copy of the statement tree.
  //! Top level expressions are also recursive copy of the expression
  //! trees. CFG targets are just same, i.e. basic blocks are not
  //! copied.
  virtual IRTree* GetRecCopy() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // IRStmt::GetRecCopy
  IRStmt* GetRecCopyStmt() const;
  virtual bool Accept(StmtVisitor& visitor_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // IRStmt::Accept
  IRStmt* ReplaceWith(IRStmt* replacement_);
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  virtual IRTree* GetParent() const {
    ASSERTMSG(0, ieStrFuncShouldNotBeCalled);
  } // IRStmt::GetParent
  DIEBase* diGetDIE() const {
    REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
    return mDIE;
  } // IRStmt::diGetDIE
private: // Member functions
  void diSetDIE(DIEBase* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    mDIE = die_;
  } // IRStmt::diSetDIE
  //! \brief should be called
  //! \return Returns \a true if there has been a change, otherwise returns \a false.
  //! \pre The statement that implements this must be a control flow statement.
  //! \attention Remapping jump targets requires parent basic blocks' cfg
  //!            update, there for do not call this function directly.
  //! \sa IRBuilder::RemapJumpTargets
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
private: // Member data
  IRStmt* mPrev;
  IRStmt* mNext;
  IRBB* mParentBB;
  DIEBase* mDIE;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
  friend class IRBB;
}; // class IRStmt

//! \brief The basic block class.
//! \note CFG-IRBB has aggregation relation.
//! \sa IRSBeforeFirstOfBB IRSAfterLastOfBB
//! \par Semantics of Entry and Exit basic blocks:
//!      Entry basic block will contain a single prolog statement
//!      in many cases. However a function may (depending on its parameter values)
//!      need not to create a prolog. It is expected that entry will have a
//!      control flow statement. If this is not the case it must have an epilog
//!      statement in it. Since it is always required to have a matching prolog/epilog
//!      pairs in every execution path it should also contain a prolog (in case
//!      epilog is also in the entry). Note that epilog statement must be the last
//!      statement of its basic block.
//!
//! Exit basic blocks may only contain a single epilog statement.
class IRBB : public IR {
protected:
  IRBB(const Invalid<IRBB>* inv_);
  IRBB();
public:
  //! \param parent_ Parent CFG of the basic block.
  //! \pre Non-null \p parent.
  IRBB(CFG* parent_);
  virtual ~IRBB() {}
public:
  virtual bool IsValid() const;
  //! \brief Sets the CFG of this basic block.
  //! \pre Valid \p cfg.
  void SetCFG(CFG* cfg_);
  //! \brief Returns the CFG of this basic block.
  //! \pre Valid parent CFG.
  CFG* GetCFG() const;
  //! \brief Adds a statement to the basic block.
  //! \param stmt_ statement to be inserted.
  //! \param ic_ If null \p stmt is added to the end of the basic block.
  //! \note It updates the \p prev, \p next, \p parent of \p stmt.
  //! \note It updates CFG.
  //! \sa IRBuilder::irbInsertStmt
  void InsertStmt(IRStmt* stmt_, const InsertContext& ic_);
  //! \brief Removes a statement from the basic block, updates the CFG.
  //! \note It updates the \p prev, \p next, \p parent of \p stmt.
  //! \note It updates CFG.
  //! \sa IRBuilder::irbRemoveStmt
  void RemoveStmt(IRStmt* stmt_);
  void RemoveAllStmts();
  //! \brief Returns the first statement or if there is no statement aborts the program.
  //! \note Note that it does not return IRSBeforeFirstOfBB.
  //! \todo L Design: why would you need the first statement? Can IRSBeforeFirstOfBB be used in all
  //! places and remove this function? Same for last statement.
  IRStmt* GetFirstStmt() const;
  //! \brief Returns the last statement or if there is no statement aborts the program.
  //! \note Note that it does not return IRSAfterLastOfBB.
  //! \pre There must be at least one statement in the basic block.
  //! \sa Predicate::IsEmpty
  IRStmt* GetLastStmt() const;
  //! \brief Returns the physically previous basic block.
  //! \pre Non-null \p prev, you may check if this is the entry basic block.
  //! \sa Predicate::pIsEntryBB
  IRBB* GetPrev() const;
  //! \brief Returns the physically next basic block.
  //! \pre Non-null \p next, you may check if this is the exit basic block.
  //! \sa Predicate::pIsExitBB
  IRBB* GetNext() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Returns the number of statements.
  hFInt32 GetNumOfStmts() const {
    BMDEBUG1("IRBB::GetNumOfStmts");
    hFInt32 retVal(mStmts.size());
    EMDEBUG1(retVal);
    return retVal;
  } // IRBB::GetNumOfStmts
  //! \brief Returns a statement iterator.
  //! \post It locks the insert/deletion of statements in to this basic block,
  //!       i.e., if you try aborts the program.
  //! \todo M Design: Make this function const.
  ListIterator<IRStmt*> GetStmtIter() {return ListIterator<IRStmt*>(mStmts);}
  RevListIterator<IRStmt*> GetStmtRevIter() {return RevListIterator<IRStmt*>(mStmts);}
  //! \sa CFG::ResetVisits
  void SetVisited() {mVisited = true;}
  //! \sa CFG::ResetVisits
  void ResetVisited() {mVisited = false;}
  //! \sa CFG::ResetVisits
  bool IsVisited() const {return mVisited;}
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  DIEBase* diGetDIE() const {
    REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
    return mDIE;
  } // IRBB::diGetDIE
private: // Member functions
  void diSetDIE(DIEBase* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    mDIE = die_;
  } // IRBB::diSetDIE
private: // Member functions
  //! \brief Returns an index in mDFASets which is not in use.
  hFSInt32 getDFASetId();
private: // Member data
  class DFASets {
    public:
    DFASets() : mInUse(true), mInited(false) {}
    fBitVector mDFAInSet;   //!< Input set of DFA analysis items.
    fBitVector mDFAOutSet;  //!< Output set of DFA analysis items.
    fBitVector mDFAGenSet;  //!< Generates/Uses set
    fBitVector mDFAKillSet; //!< Kills/Defines set
    //! \brief Must be set to true if the variables are in use.
    //! When a DFA starts it tries to 'recycle' mDFASets by checking
    //! this variable. When inUse is false the DFA starts using
    //! this set.
    bool mInUse;
    bool mInited; //!< False if gen/kill sets are not yet calculated.
  }; // class DFASets
  //! \brief DFA (Data flow analysis) scratch and result variables.
  //! \note There may be multiple DFA results kept at the same time,
  //!       e.g. for register allocation we need liveness and reaching
  //!       definitions results.
  //! \attention This variable is a scratch variable, it is used everytime a DFA is run.
  vector<DFASets> mDFASets;
  //! \brief List of predecessors of this basic block.
  vector<IRBB*> mPreds;
  set<IRBB*> mDoms; //!< List of dominators, use CFG to update it.
  set<IRBB*> mPDoms; //!< List of post dominators, use CFG to update it.
  IRBB* mIDom; //!< Immediate dominator of the basic block. It is null for entry basic blocks.
  IRBB* mIPDom; //!< Immediate post dominator of the basic block. It is null for entry basic blocks.
  //! \brief List of successors of this basic block.
  vector<IRBB*> mSuccs;
  //! \brief Statements of this basic block.
  list<IRStmt*> mStmts;
  //! \brief A valid BB must have a parent CFG.
  CFG* mParentCFG;
  IRBB* mPrev; //!< Physically previous basic block, it is set in the IRBB constructor.
  IRBB* mNext; //!< Physically next basic block, it is set in the IRBB constructor.
  //! \brief For algorithms that need to mark this basic block as visited.
  //! \note Some algorithms need to visit all the basic blocks,
  //!       however the cyclic nature of CFG causes infinite loops
  //!       for loop constructs. This variable should be reset
  //!       before such algorithms and must be marked when visited.
  bool mVisited;
  DIEBase* mDIE;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
  friend class CFG; //!< It is okay to call one up level class friend.
  template<class P, class D> friend class DFA;
  template<class S> friend class Singleton;
}; // class IRBB

class IRBBAny : public IRBB {
public:
  IRBBAny();
  virtual ~IRBBAny() {}
  virtual bool IsValid() const{return true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class IRBBAny

//! \brief Control flow graph object.
//! \note This object is automatically created by IRBuilder while
//! creating a new IRFunction.
//! \sa IRFunction IRBuilder
//! \note CFG-IRBB has aggregation relation.
//! \note CFG-entryBB, CFG-exitBB has composition relation.
//! \attention Exit basic block must be physically the last basic block,
//! similarly, entry must be physically the first basic block.
class CFG : public IR {
protected:
  CFG(const Invalid<CFG>* inv_) {}
  //! \post Valid \p this.
  CFG();
public:
  virtual ~CFG() {}
public:
  //! \brief Returns the entry basic block.
  //! \return It always returns a non-null pointer to a basic block.
  //! \pre Non-null entry block.
  IRBB* GetEntryBB() const;
  //! \brief Returns the exit basic block.
  //! \return It always returns a non-null pointer to a basic block.
  //! \pre Non-null exit block.
  IRBB* GetExitBB() const;
  //! \pre Valid \p entryBB.
  //! \pre Valid \p exitBB.
  virtual bool IsValid() const;
  IRFunction* GetFunc() const;
  //! \brief Returns the dominator tree of the CFG.
  //! \note The dominator tree is emptied if CFG changes. Therefore,
  //!       you may need to create a recursive copy of the tree.
  //! \sa tree::GetRecCopy
  const tree<IRBB*>& GetDominatorTree() {updateDomTree(); return *mDomTree;}
  //! \brief Returns the post dominator tree of the CFG.
  const tree<IRBB*>& GetPDominatorTree() {updatePDomTree(); return *mPDomTree;}
  //! \todo M Design: Should below Jump IRBuilder?
  void RemoveBB(IRBB* bb_);
  //! \brief Returns an iterator that traverses basic blocks in physical order
  //!        starting from entry ending at exit block.
  ListIterator<IRBB*> GetPhysBBIter() {return ListIterator<IRBB*>(mBBs);}
  RevListIterator<IRBB*> GetPhysBBRevIter() {return RevListIterator<IRBB*>(mBBs);}
  //! \brief Sets the physical order of basic blocks.
  //! \note It updates the IRBB members mNext and mPrev.
  //! \pre first and last members of the list must be entry and exit basic blocks.
  //! \pre bbs_ must contain exact the same members of GetPhysBBIter.
  void SetPhysOrderOfBBs(const vector<IRBB*>& bbs_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Resets the visited flag of all basic blocks.
  //! \sa IRBB::IsVisited IRBB::ResetVisited IRBB::SetVisited
  void ResetVisits();
  //! \brief Sets all statements' HasMarked to false.
  void ResetStmtMarks(eIRTreeMark what_);
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_);
  IRDUUDChains* GetDUUDChains() {updateDUUDChains(); return mDUUDChains;}
private: // Member functions
  void setFunction(IRFunction* func_);
  void updateDomTree();
  void updatePDomTree();
  void updateLoops();
  void updateDUUDChains();
  void computeDominators();
  const vector<IRLoop*>& getLoops() {updateLoops(); return *mLoops;}
private: // Member data
  //! \brief May be null if not calculated.
  //! \note mDUChains may be invalidated (set to null) by IRBuilder.
  IRDUUDChains* mDUUDChains;
  IRFunction* mFunc;
  //! \brief List of basic blocks ordered from entry to exit in physical order.
  //! \note It includes entry and exit basic blocks.
  //! \note IRBB::mNext, IRBB::mPrev needs to be updated also when the order is changed.
  list<IRBB*> mBBs;
  //! \brief Entry basic block.
  IRBB* mEntryBB;
  //! \brief Exit basic block.
  //! \note Return statements always target the exit basic block.
  IRBB* mExitBB;
  tree<IRBB*>* mDomTree; //!< May be null if not calculated or if it is invalidated.
  tree<IRBB*>* mPDomTree; //!< May be null if not calculated or if it is invalidated.
  //! \brief Top level natural loops of the CFG. 
  //! When null it means loops are not calculated. It may contain zero
  //! elements meaning no loops in the cfg.
  //! \note Loops are invalidated by IRBuilder if there is any change in the
  //!       CFG edges, e.g. a bb is inserted or deleted or preds, succs changes.
  vector<IRLoop*>* mLoops;
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
  friend class IRFunction;
  friend class IRBB;
}; // class CFG

//! \brief Function declaration.
//! \note IRFunction-IRFunctionDecl has composition relation,
//!       but not strong association.
//! \par Difference between function declaration and functions.
//!      IRFunction object is used for functions with definitions, i.e.
//!      implementation or body of the function is available.
//!      Function declaration is mainly used for function without the body,
//!      note that such functions are heavily used, e.g. calling an external
//!      function is quite common.
class IRFunctionDecl : public IR {
public:
  //! \param [in] funcPrototype_ prototype of the function. Must have been
  //!        allocated on the heap and should not be deleted.
  //! \param [in] parentModule_ Owning module of this function. Must have been
  //!        allocated on the heap and should not be deleted. It may be
  //!        Invalid in case of compiler defined functions.
  IRFunctionDecl(const string& name_, IRTFunc* funcPrototype_, IRModule* parentModule_);
  virtual ~IRFunctionDecl() {}
public:
  //! \brief Adds a new parameter, note that order of the parameters are important.
  //! \param[in] parameter_ Must have been allocated on the heap and should not be deleted.
  //! \note you should add parameters via IRBuilder::irboParameter.
  //! \sa IRBuilder::irboParameter.
  void AddParameter(IROParameter* parameter_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Returns the function type, prototype.
  IRTFunc* GetType() const {return mPrototype;}
  //! \pre Parent module must be invalid.
  IRModule* GetModule() const;
  //! \sa IRFunction::GetLocals, Extract::eGetParams, Extract::eGetLocals
  const vector<IROParameter*>& GetParams() const {
    return mParams;
  } // IRFunctionDecl::GetParams
  string GetName() const {return mName;}
  DIEBase* diGetDIE() const {
    REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
    return mDIE;
  } // IRFunction::diGetDIE
private:
  void diSetDIE(DIEBase* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    mDIE = die_;
  } // IRFunction::diSetDIE
private: // Member data
  //! \brief Parameters of the function.
  vector<IROParameter*> mParams;
  IRModule* mParentModule;
  IRTFunc* mPrototype;
  string mName; //!< name of the function.
  DIEBase* mDIE;
private:
  friend class IRFunction;
}; // class IRFunctionDecl

//! \brief Function definition.
//! \note Functions can only be created through IRBuilder.
//! \note IRFunction-CFG has composition relation.
//! \sa IRBuilder IRFunctionDecl
class IRFunction : public IR {
protected:
  IRFunction(const Invalid<IRFunction>* inv_) {}
  IRFunction(IRFunctionDecl* funcDecl_, CFG* cfg_);
  virtual ~IRFunction() {}
public:
  //! \pre Non-null \p cfg.
  CFG* GetCFG() const;
  IRFunctionDecl* GetDeclaration() const {return mDeclaration;}
  virtual bool IsValid() const;
public:
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \note Updates the stack size needed by the function.
  //! \note Automatically assigns stack based addresses using frame pointer
  //!       (does it when there is no address already assigned to it).
  //! \todo H Design: In the AST2IR we may generate unnecessary locals,
  //!       optimizations will clean some of them, they should also be
  //!       reflected in function locals to update the stack size need.
  void AddLocal(IROLocal* local_);
  //! \return Returns true if there is any address taken label.
  bool GetAddressTakenLabels(vector<IRSLabel*>& labels_);
  //! \pre \p label must have address taken set.
  void AddAddressTakenLabel(IRSLabel* label_);
  void RemoveAddrTakenLabel(IRSLabel* label_);
  bool GetDynJumps(vector<IRSDynJump*>& Jumps_);
  void AddDynJump(IRSDynJump* jump_);
  void RemoveDynJump(IRSDynJump* jump_);
  //! \brief Returns the stack size needed by this function.
  //! \todo H Design: Need to think about the _alloca cases.
  tBigInt GetStackSize() const {return mStackSize;}
  //! \pre \p cgContext_ must be non-null.
  //! \pre May only be set once.
  //! \note If you want to try out several code generation contexts
  //!       you should first get a copy of the function before using SetCGContext.
  void SetCGContext(CGFuncContext* cgContext_);
  //! \pre \p cgContext must be non-null.
  CGFuncContext* GetCGContext();
  const string& GetName() const {return mDeclaration->mName;}
  //! \brief Assigns a stack address to locals.
  //! \note It reassigns the address of locals that are stack based,
  //!       and unassigned. It does not assign a stack address otherwise.
  //! \note You may call this several times, usually before the code generation
  //!       of the function. The stack space required for locals may only be
  //!       known after calling this function.
  //! \note Stack size is not simply the addition of locals' type sizes,
  //!       alignment of types and minimum alignment of stack slots should
  //!       also be considered.
  //! \pre Non-null, non-invalid cgContext_.
  void AssignStackAddrToLocals(CGFuncContext* cgContext_);
  tBigInt GetParamsSize() const {
    return mParamsSize;
  } // IRFunction::GetParamsSize
  //! \brief Returns the size of stack required to hold all the locals.
  tBigInt GetLocalsSize() const {
    return mStackLocalSize;
  } // IRFunction::GetLocalsSize
  void SetParamsSize(const tBigInt& paramSize_) {
    BMDEBUG2("IRFunction::SetParamsSize", &paramSize_);
    mParamsSize = paramSize_;
    EMDEBUG0();
  } // IRFunction::SetParamsSize
  void SetStackSize(const tBigInt& stackSize_) {
    BMDEBUG2("IRFunction::SetStackSize", &stackSize_);
    mStackSize = stackSize_;
    EMDEBUG0();
  } // IRFunction::SetStackSize
  virtual bool Accept(ExprVisitor& visitor_) {
    return mCFG->Accept(visitor_);
  } // IRFunction::Accept
  virtual bool Accept(StmtVisitor& visitor_) {
    return mCFG->Accept(visitor_);
  } // IRFunction::Accept
  DIEBase* diGetDIE() const {
    REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
    return mDIE;
  } // IRFunction::diGetDIE
  //! \sa IRFunctionDecl::GetParams, Extract::eGetParams, Extract::eGetLocals
  const vector<IROLocal*>& GetLocals() const {return mLocals;}
  //! \pre Current inline flag must be undecided.
  //! \pre if_ may not be IFInvalid or IFNotDecided.
  void SetInlineFlag(InlineFlag if_) {
    REQUIREDMSG(if_ != IFNotDecided && if_ != IFInvalid, ieStrParamValuesDBCViolation);
    REQUIREDMSG(mInlineFlag == IFNotDecided, ieStrParamValuesDBCViolation);
    mInlineFlag = if_;
    return;
  } // IRFunction::SetInlineFlag
  InlineFlag GetInlineFlag() const {return mInlineFlag;}
private:
  void diSetDIE(DIEBase* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    mDIE = die_;
  } // IRFunction::diSetDIE
  vector<IROLocal*> mLocals; //!< Local variables of the function.
  vector<IRSLabel*> mAddrTakenLabels; //!< List of address taken labels.
  vector<IRSDynJump*> mDynJumps; //!< List of dynamic Jumps, they need to be kept track.
  //! \brief Control flow graph of the function.
  //! Statements of the function can be accessed from \p cfg.
  CFG* mCFG;
  //! \brief Amount of current space required for holding locals on the stack.
  //! \note stack alignment in most HW is required. For example, a byte typed
  //!       local will probably occupy 4 bytes. Also, alignment of local's
  //!       type may force adding paddings. \p mStackLocalSize is also the
  //!       current offset for the next local.
  tBigInt mStackLocalSize;
  //! \brief Stack size required by the function.
  //! \post It should always be greater than or equal to stackLocalSize.
  //! This value is usually externally set. The stack size is the sum of
  //! local variables and spill locations.
  tBigInt mStackSize;
  tBigInt mParamsSize;
  //! \brief Pointer to the declaration of this function.
  IRFunctionDecl* mDeclaration;
  CGFuncContext* mCGContext; //!< Code generation context, holds the code generated for this function.
  DIEBase* mDIE;
  InlineFlag mInlineFlag; 
private:
  friend class Predicate;
  friend class Extract;
  friend class IRBuilder;
  friend class IRModule;
}; // class IRFunction

//! \brief IR representation of a source file.
//! \note IRProgram-IRModule has aggregation relation.
//! \todo H Design: Modules should have names.
class IRModule : public IR {
protected:
  IRModule(const Invalid<IRModule>* inv_) {}
public:
  IRModule();
  virtual ~IRModule() {}
public: // Member functions
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  ListIterator<IRFunction*> GetFuncIter() {return ListIterator<IRFunction*>(mFuncs);}
  RevListIterator<IRFunction*> GetFuncRevIter() {return RevListIterator<IRFunction*>(mFuncs);}
  void GetGlobals(vector<IROGlobal*>& globals_) const {globals_ = mGlobals;}
  const vector<IROGlobal*>& GetGlobals() const {return mGlobals;}
  //! \brief Calls the Accept function of all the functions of this module.
  virtual bool Accept(StmtVisitor& stmtVisitor_);
  virtual bool Accept(ExprVisitor& visitor_);
  IROGlobal* FindGlobal(const string& name_) const;
  DIEBase* diGetDIE() const {
    REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
    return mDIE;
  } // IRModule::diGetDIE
  //! \pre \p cgContext_ must be non-null.
  //! \pre May only be set once.
  void SetCGContext(CGContext* cgContext_);
  //! \brief Returns the code generation context.
  //! If code generation context was not set it gets it from the program
  //! context and that context becomes the context of this module, i.e. a cg
  //! context may only be set once.
  CGContext* GetCGContext();
  bool IsFuncDefined(const string name_) const {
    return mName2Funcs.find(name_) != mName2Funcs.end();
  } // IRModule::IsFuncDefined
  IRFunction* GetFunc(const string name_) const {
    BMDEBUG2("IRModule::GetFunc", name_);
    REQUIREDMSG(mName2Funcs.find(name_) != mName2Funcs.end(), ieStrParamValuesDBCViolation);
    IRFunction* retVal(const_cast<IRModule*>(this)->mName2Funcs[name_]);
    EMDEBUG1(retVal);
    return retVal;
  } // IRModule::GetFunc
private: // Member functions
  void diSetDIE(DIEBase* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    mDIE = die_;
  } // IRModule::diSetDIE
  void AddGlobal(IROGlobal* global_);
  void AddFunction(IRFunction* function_);
private: // Member data
  CGContext* mCGContext; //!< Code generation context for the module, may be null if not set.
  list<IRFunction*> mFuncs;
  map<string, IRFunction*> mName2Funcs;
  //! \brief Global variables of this module including static, and extern.
  vector<IROGlobal*> mGlobals;
  map<string, IROGlobal*> mMapNameGlobals;
  DIEBase* mDIE;
private:
  friend class IRBuilder;
  friend class IRBuilderHelper;
  friend class Predicate;
  friend class Extract;
  friend class IRProgram;
}; // class IRModule

//! \brief The program IR.
//! \note IRProgram is a Singleton.
//! \sa Singleton
class IRProgram : public IR {
protected:
  IRProgram(const Invalid<IRProgram>* inv_) {}
public: // Member functions
  IRProgram();
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  ListIterator<IRModule*> GetModuleIter() {return ListIterator<IRModule*>(mModules);}
  RevListIterator<IRModule*> GetModuleRevIter() {return RevListIterator<IRModule*>(mModules);}
  //! \brief Calls the Accept function of all the modules of this program.
  virtual bool Accept(StmtVisitor& stmtVisitor_);
  virtual bool Accept(ExprVisitor& visitor_);
  DIEBase* diGetDIE() const {
    REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
    return mDIE;
  } // IRProgram::diGetDIE
  void SetCGContext(CGContext* cgContext_);
  CGContext* GetCGContext() const;
private: // Member functions
  void diSetDIE(DIEBase* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    mDIE = die_;
  } // IRProgram::diSetDIE
  void AddModule(IRModule* module_);
private: // Member data
  list<IRModule*> mModules;
  static hFInt32 sRefCount;
  DIEBase* mDIE;
  //! \brief Code generation context for the program. 
  //! \note cg context may be null, in that case every function/module should
  //!       have a set cg context. 
  CGContext* mCGContext;
private:
  friend class IRBuilder;
  friend class IRBuilderHelper;
  friend class Predicate;
  friend class Extract;
  friend class DebugTrace;
}; // class IRProgram

//! \brief Points to Singleton<IRProgram>::Obj().
extern IRProgram* irProgram;

// //////////////////////////
// OBJECTS
// //////////////////////////
//! \brief HW Address, home place of objects.
//!
//! Objects can be in memory, stack, or registers.
//! \note High level source objects may live in several addresses.
//! There is always a one to one mapping between temporary
//! objects and an address of them. Each temporary may have
//! a high level object link.
//! \sa IRObject, AddrReg, AddrLTConst, AddrStack
class Address : public IDebuggable {
protected:
  Address(const Invalid<Address>* inv_) {}
public:
  Address() :
    mIsAssigned(false)
  {
  } // Address::Address
  virtual ~Address() {}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Must return a heap allocated clone of this address.
  virtual Address* Clone() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // Address::Clone
  bool IsAssigned() const {return mIsAssigned;}
  void SetAssigned() {mIsAssigned = true;}
private:
  //! \brief signifies that a concrete address is assigned.
  //! Usually we set the type of address for an object earlier in the
  //! compilation, e.g. AST2HIR,  but the assignment of real address is done in a
  //! later phase, code generation.
  bool mIsAssigned;
}; // class Address

//! \note In case address spans multiple registers you should use a virtual register.
//! \todo M Design: Force use of virtual registers at all times.
class AddrReg : public Address {
public:
  AddrReg();
  //! \param reg_ Can be both HW and virtual register.
  AddrReg(Register* reg_) :
    mReg(reg_)
  {
    REQUIREDMSG(reg_ != 0, ieStrParamValuesDBCViolation);
    SetAssigned();
  } // AddrReg::AddrReg
  AddrReg& operator = (const AddrReg& addr_) {
    mReg = addr_.mReg;
    if (addr_.IsAssigned()) {
      SetAssigned();
    } // if
    return *this;
  } // AddrReg::operator =
  virtual ~AddrReg() {}
  Register* GetRegister() const {
    REQUIREDMSG(IsAssigned() == true, ieStrParamValuesDBCViolation);
    return mReg;
  } // AddrReg::GetRegister
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual Address* Clone() const {
    return IsAssigned() ? new AddrReg(mReg) : new AddrReg();
  } // AddrReg::Clone
private:
  Register* mReg; //!< May not be null.
}; // class AddrReg

class AddrStack : public Address {
public:
  AddrStack();
  //! \brief Specifies a stack based address, (fp or sp relative).
  //! \param reg_ Is generally HWReg stack or frame pointer.
  //! \param offset_ Offset relative to \p reg_.
  //! \todo H Design: Offset should be in bytes.
  AddrStack(Register* reg_, const tBigInt& offset_) :
    mReg(reg_),
    mOffset(offset_)
  {
    SetAssigned();
  } // AddrStack::AddrStack
  AddrStack& operator = (const AddrStack& addr_) {
    mReg = addr_.mReg;
    mOffset = addr_.mOffset;
    if (addr_.IsAssigned()) {
      SetAssigned();
    } // if
    return *this;
  } // AddrStack::operator =
  virtual ~AddrStack() {}
  Register* GetRegister() const {
    REQUIREDMSG(IsAssigned() == true, ieStrParamValuesDBCViolation);
    return mReg;
  } // AddrStack::GetRegister
  tBigInt GetOffset() const {
    REQUIREDMSG(IsAssigned() == true, ieStrParamValuesDBCViolation);
    return mOffset;
  } // AddrStack::GetOffset
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual Address* Clone() const {
    return IsAssigned() ? new AddrStack(mReg, mOffset) : new AddrStack();
  } // AddrStack::Clone
private:
  Register* mReg;
  tBigInt mOffset;
}; // class AddrStack

//! \brief Link-time address constant, plus offset.
class AddrLTConst : public Address {
protected:
  AddrLTConst(const Invalid<AddrLTConst>* inv_) {}
public:
  AddrLTConst() :
    mLTConst(""),
    mOffset(0)
  {
  } // AddrLTConst::AddrLTConst
  AddrLTConst(const AddrLTConst& addr_) :
    mLTConst(addr_.mLTConst),
    mOffset(0)
  {
    if (addr_.IsAssigned()) {
      SetAssigned();
    } // if
  } // AddrLTConst::AddrLTConst
  AddrLTConst(const string& ltConst_) :
    mLTConst(ltConst_),
    mOffset(0)
  {
    SetAssigned();
  } // AddrLTConst::AddrLTConst
  AddrLTConst(const string& ltConst_, const tBigAddr& offset_) :
    mLTConst(ltConst_),
    mOffset(offset_)
  {
    SetAssigned();
  } // AddrLTConst::AddrLTConst
  virtual ~AddrLTConst() {}
  AddrLTConst& operator = (const AddrLTConst& addr_) {
    mLTConst = addr_.mLTConst;
    mOffset = addr_.mOffset;
    if (addr_.IsAssigned()) {
      SetAssigned();
    } // if
    return *this;
  } // AddrLTConst::operator =
  operator string () const {
    REQUIREDMSG(mOffset == 0, ieStrParamValuesDBCViolation);
    return mLTConst;
  } // AddrLTConst::operator ()
  const string& GetLTConst() const {
    REQUIREDMSG(IsAssigned() == true, ieStrParamValuesDBCViolation);
    return mLTConst;
  } // AddrLTConst::GetLTConst
  tBigAddr GetOffset() const {
    REQUIREDMSG(IsAssigned() == true, ieStrParamValuesDBCViolation);
    return mOffset;
  } // AddrLTConst::GetOffset
  virtual Address* Clone() const {
    return IsAssigned() ? new AddrLTConst(mLTConst, mOffset) : new AddrLTConst();
  } // AddrLTConst::Clone
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \todo M Design: adapt the size to the size of target that this address is created for.
  hFUInt32 GetSize() const {return 4;}
private:
  string mLTConst;
  tBigAddr mOffset;
}; // class AddrLTConst

class AddrDiff : public IDebuggable {
protected:
  AddrDiff(const Invalid<AddrDiff>* inv_) {}
public:
  AddrDiff() {}
  virtual ~AddrDiff() {}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class AddrDiff

class AddrConstDiff : public AddrDiff {
public:
  AddrConstDiff(const tBigAddr& start_, const tBigAddr& end_) :
    mStartAddr(start_),
    mEndAddr(end_)
  {
  } // AddrConstDiff::AddrConstDiff
  AddrConstDiff() :
    mStartAddr(0),
    mEndAddr(0)
  {
  } // AddrConstDiff::AddrConstDiff
  AddrConstDiff(const AddrConstDiff& addrDiff_) :
    mStartAddr(addrDiff_.mStartAddr),
    mEndAddr(addrDiff_.mEndAddr)
  {
  } // AddrConstDiff::AddrConstDiff
  tBigAddr GetStartAddr() const {return mStartAddr;}
  tBigAddr GetEndAddr() const {return mEndAddr;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  tBigAddr mStartAddr;
  tBigAddr mEndAddr;
}; // class AddrConstDiff

//! \brief The difference of two addresses.
class AddrLTConstDiff : public AddrDiff {
protected:
  AddrLTConstDiff(const Invalid<AddrLTConstDiff>* inv_) {}
public:
  AddrLTConstDiff(const AddrLTConstDiff& addrDiff_) :
    mHigh(addrDiff_.mHigh),
    mLow(addrDiff_.mLow)
  {
  } // AddrLTConstDiff::AddrLTConstDiff
  AddrLTConstDiff() :
    mHigh("<err>"),
    mLow("<err>")
  {
  } // AddrLTConstDiff::AddrLTConstDiff
  AddrLTConstDiff(const string& high_, const string& low_) :
    mHigh(high_),
    mLow(low_)
  {
  } // AddrLTConstDiff::AddrLTConstDiff
  AddrLTConstDiff(AddrLTConst& high_, AddrLTConst& low_) :
    mHigh(high_),
    mLow(low_)
  {
  } // AddrLTConstDiff::AddrLTConstDiff
  const AddrLTConst& GetHighAddr() const {return mHigh;}
  const AddrLTConst& GetLowAddr() const {return mLow;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  AddrLTConst mHigh;
  AddrLTConst mLow;
}; // class AddrLTConstDiff

//! \brief Base class for objects, parameters, locals, globals,
//! structure fields, fields, labels.
//! \todo M Design: Handle not setting of parent for objects.
class IRObject : public IRExpr {
protected:
  IRObject(Invalid<IRObject>* inv_);
public:
  IRObject(IRType* type_, const string& name_);
  virtual bool IsValid() const;
  const string& GetName() const {return mName;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const;
  virtual bool GetChildren(vector<IRExpr*>& children_) const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  //! \pre Must have a set address, to check this use Predicate::pIsAddrSet.
  Address* GetAddress() const;
  void SetAddress(const Address& addr_);
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  DIEBase* diGetDIE() const {
    REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
    return mDIE;
  } // IRObject::diGetDIE
  //! \note Address taken information is maintained by IRBuilder as
  //!       expressions are created and deleted.
  bool IsAddrTaken() const {return mAddrTaken > 0;}
private: // Member functions
  //! \pre You must use Consts::cAddrTaken and Consts::cAddrNotTaken for the newValue_.
  //! \todo M Design: It sounds stupid to not be able to do this:
  //!       (const bool& newValue) {&newValue == &consts.cAddrNotTaken}
  void setAddrTaken(bool newValue_);
  void diSetDIE(DIEBase* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    mDIE = die_;
  } // IRObject::diSetDIE
private: // Member data
  string mName;
  Address* mAddr;
  DIEBase* mDIE;
  hFSInt32 mAddrTaken;
private:
  friend class IRBuilder;
  friend class Extract;
  friend class Predicate;
}; // class IRObject

//! \brief Fields of structures.
//!
//! Since each field of a structure has a name (unnamed bitfields are also included)
//! structure fields are treated as objects.
//!
//! Bitfields are also IROField but their type is IRTBitField.
//! \sa IRTBitField
class IROField : public IRObject {
public:
  IROField(const string& name_, IRType* type_, tBigInt offsetInBits_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  //! \brief returns the offset of this field within its parent structure.
  //! \note The returned offset is in bits.
  tBigInt GetOffset() const {return mOffsetInBits;}
private:
  tBigInt mOffsetInBits;
private:
  friend class IRBuilder;
  friend class Extract;
  friend class Predicate;
}; // class IROField

class IROBitField : public IROField {
public:
  IROBitField(const string& name_, IRType* type_, tBigInt offsetInBits_, hFSInt16 bitOffset_, tBigInt bitSize_, IRSignedness sign_);
public: // Member functions
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  //! \brief Size of bit field in number of bits.
  tBigInt GetBitSize() const {return mBitSize;}
  IRSignedness GetSign() const {return mSign;}
  //! \brief Remainder offset of its position in the struct.
  //! Byte offset can be obtained from IROField::GetOffset in bits.
  hFSInt16 GetBitOffset() const {return mBitOffset;}
private: // Member data
  IRSignedness mSign;
  tBigInt mBitSize;
  hFSInt16 mBitOffset;
private:
  friend class IRBuilder;
  friend class Extract;
  friend class Predicate;
}; // class IROBitField

class IROParameter : public IRObject {
protected:
  IROParameter(const Invalid<IROParameter>* inv_) : IRObject(&Invalid<IRObject>::Obj()) {}
public:
  IROParameter(IRType* type_, const string& name_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
private:
  friend class Extract;
  friend class Predicate;
}; // class IROParameter

//! \brief IR Object that should be used for high level local variables.
class IROLocal : public IRObject {
protected:
  IROLocal(const Invalid<IROLocal>* inv_) : IRObject(&Invalid<IRObject>::Obj()) {}
public:
  IROLocal(IRType* type_, const string& name_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
private:
  friend class Extract;
  friend class Predicate;
}; // class IROLocal

//! \brief IR Object that should be used for temporary objects created by compiler.
//! \note Temporary object do not appear in the debug information.
//! \sa IROTmpGlobal
class IROTmp : public IROLocal {
public:
  IROTmp(const string& idName_, IRType* type_, IRObject* highLevelObject_);
  IROTmp(const string& idName_, IRType* type_);
  IRObject* GetHighLevelObject() const {return mHighLevelObject;}
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
private: // Member functions
  static string sGenerateName(const string& idName_, IRObject* highLevelObject_);
private: // Member data
  IRObject* mHighLevelObject;
}; // class IROTmp

//! \brief A symbol that can only be used in relocation expressions.
//! IRORelocSymbol is used in generated assembly code. A relocation
//! symbol is usually an address that can only be determined at link-time.
class IRORelocSymbol : public IRObject {
public:
  IRORelocSymbol(IRType* type_, const string& name_);
  virtual bool IsValid() const {return true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
private:
  friend class Extract;
  friend class Predicate;
}; // class IRORelocSymbol

class IROGlobal : public IRObject {
protected:
  IROGlobal(const Invalid<IROGlobal>* inv_) : IRObject(&Invalid<IRObject>::Obj()) {}
public:
  IROGlobal(IRType* type_, const string& name_, IRLinkage linkage_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  IRExpr* GetInits() const {return mConstInits;}
  IRLinkage GetLinkage() const {return mLinkage;}
private:
  //! \brief List of constant initializers of this global.
  //! \note Parent of initializations are global objects.
  IRExpr* mConstInits;
  IRLinkage mLinkage;
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IROGlobal

//! \note Temporary object do not appear in the debug information.
//! \sa IROTmp
class IROTmpGlobal : public IROGlobal {
public:
  IROTmpGlobal(IRType* type_, const string& name_, IRLinkage linkage_) :
    IROGlobal(type_, name_, linkage_)
  {
  } // IROTmpGlobal::IROTmpGlobal
  virtual ~IROTmpGlobal() {}
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class IROTmpGlobal

// //////////////////////////
// STATEMENTS
// //////////////////////////

//! \brief Assignment statement construct.
//! \ingroup hirstatements
//! \note LHS of an assignment is of pointer type. RHS
//! of an assignment determines the format (from the type of the expression)
//! of the data to be stored at the LHS pointer address. For example,
//! structure members are written by an assignment with LHS structure
//! base address plus some offset and rhs with the type of the member.
//! \todo M Design: Not sure if we should force the LHS to be of ptr type of rhs?
//! \note Q&A) Can top LHS node be not IREAddrOf?
class IRSAssign : public IRStmt {
private:
  IRSAssign(const IRSAssign& assign_) : IRStmt(assign_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSAssign& operator = (const IRSAssign& assign_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSAssign(const Invalid<IRSAssign>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  virtual ~IRSAssign() {}
  //! \pre \p lhs_ must be of type pointer.
  IRSAssign(IRExpr* lhs_, IRExpr* rhs_, DIEBase* die_);
  virtual bool IsValid() const;
  IRExpr* GetLHS() const {return mLHS;}
  IRExpr* GetRHS() const {return mRHS;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRSAssign(mLHS->GetRecCopyExpr(), mRHS->GetRecCopyExpr(), diGetDIE()->Clone());
  } // IRSAssign::GetRecCopy
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  virtual IRStmtType GetStmtType() const {return IRSTAssign;}
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  //! \attention Do not implement this function for assignment statement.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRExpr* mLHS; //!< Left hand side of assignment.
  IRExpr* mRHS; //!< Right hand side of assignment.
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSAssign

//! \ingroup hirstatements
//! \brief Evaluation construct, evaluates the expression although it has no side-effect.
//!
//! It is possible to have expressions alone without any side effects in C/C++.
//! When this expressions involve volatile objects they must not be optimized away.
//! This statement guarantees that the expression is evaluated without being optimized away.
//! \todo M Syntax: should we change the name of this class to IRSExpr? or
//! something that involves expression?
class IRSEval : public IRStmt {
private:
  IRSEval(const IRSEval& eval_) : IRStmt(eval_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSEval& operator = (const IRSEval& eval_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSEval(const Invalid<IRSEval>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  IRSEval(IRExpr* expr_, DIEBase* die_);
  virtual ~IRSEval() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  IRExpr* GetExpr() {return mExpr;}
  const IRExpr* GetExpr() const {return mExpr;}
  virtual IRTree* GetRecCopy() const {return new IRSEval(mExpr->GetRecCopyExpr(), diGetDIE()->Clone());}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const;
  virtual IRStmtType GetStmtType() const {return IRSTEval;}
  virtual bool Accept(ExprVisitor& visitor_) {return mExpr->Accept(visitor_);}
  virtual bool Accept(StmtVisitor& visitor_);
private:
  //! \attention Do not implement this function for eval statement.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRExpr* mExpr; //!< Expression to be evaluated.
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSEval

//! \ingroup hirstatements
class IRSCall : public IRStmt {
protected:
  IRSCall(const Invalid<IRSCall>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  //! \brief To be used by built-in routine constructor.
  IRSCall(IRObject* returnIn_, IRExprList* args_, DIEBase* die_);
  //! \brief To be used by built-in routine constructor.
  IRSCall(IRExprList* args_, DIEBase* die_);
  IRSCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_);
  IRSCall(IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_);
  virtual ~IRSCall() {}
public:
  //! \todo M Build: Can we place IsValid in to another module since in
  //!       release mode they are not used at all?
  virtual bool IsValid() const;
  IRExpr* GetFuncExpr() const;
  IRExprList* GetArgs() const;
  //! \brief Returns the object that will hold the return value.
  //! \pre You may not this function for procedures.
  IRObject* GetReturnIn() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Returns the function expression and all argument expressions, consecutively.
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool Accept(StmtVisitor& visitor_);
  void AddArgument(IRExpr* arg_) {
    BMDEBUG2("IRSCall::AddArgument", arg_);
    REQUIREDMSG(arg_ != 0, ieStrNonNullParam);
    mArgs->AddExpr(arg_);
    EMDEBUG0();
    return;
  } // IRSCall::AddArgument
private:
  //! \brief Returned value from call should be placed in to this object.
  //! May be null.
  IRObject* mReturnIn;
  //! \brief May be null only if this is a built-in routine.
  //! In the simplest case it is the address of a IROGlobal object with
  //! IRTFunc, e.g. foo() -> mFuncExpr == IREAddrOf(IROGlobal).
  IRExpr* mFuncExpr;
  IRExprList* mArgs;
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSCall

//! \ingroup hirstatements
//! \brief Procedure call statement, return type must be void.
class IRSPCall : public IRSCall {
private:
  IRSPCall& operator = (const IRSPCall& pcall_);
protected:
  IRSPCall(const Invalid<IRSPCall>* inv_) : IRSCall(&Invalid<IRSCall>::Obj()) {}
public:
  IRSPCall();
  IRSPCall(IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_);
  IRSPCall(const IRSPCall& pcall_, DIEBase* die_);
  virtual ~IRSPCall() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRStmtType GetStmtType() const {return IRSTPCall;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {}
  virtual IRTree* GetRecCopy() const {
    return new IRSPCall(GetFuncExpr()->GetRecCopyExpr(),
      static_cast<IRExprList*>(GetArgs()->GetRecCopyExpr()), diGetDIE()->Clone());
  } // IRSPCall::GetRecCopy
  virtual bool Accept(StmtVisitor& visitor_);
private:
  //! \attention Do not implement this function for procedure call statements.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSPCall

//! \ingroup hirstatements
//! \brief Function call statement, return type may not be void.
//! \sa IRSPCall
class IRSFCall : public IRSCall {
private:
  IRSFCall(const IRSFCall& fcall_) : IRSCall(fcall_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSFCall& operator = (const IRSFCall& fcall_);
protected:
  IRSFCall(const Invalid<IRSFCall>* inv_) : IRSCall(&Invalid<IRSCall>::Obj()) {}
  //! \brief Constructor to be used by IRSBuiltInCall.
  IRSFCall(IRObject* returnIn_, IRExprList* args_, DIEBase* die_);
public:
  IRSFCall();
  IRSFCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, DIEBase* die_);
  virtual ~IRSFCall() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRStmtType GetStmtType() const {return IRSTFCall;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {}
  virtual IRTree* GetRecCopy() const {
    return new IRSFCall(GetReturnIn(), GetFuncExpr()->GetRecCopyExpr(),
      static_cast<IRExprList*>(GetArgs()->GetRecCopyExpr()), diGetDIE()->Clone());
  } // IRSFCall::GetRecCopy
  virtual bool Accept(StmtVisitor& visitor_);
private:
  //! \pre Do not call this function.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSFCall

//! \brief Built in routine call statement.
//! \ingroup hirstatements
class IRSBuiltInCall : public IRSCall {
protected:
  IRSBuiltInCall(const Invalid<IRSBuiltInCall>* inv_) : IRSCall(&Invalid<IRSCall>::Obj()) {}
public:
  //! \brief Built-in function call.
  IRSBuiltInCall(IRObject* returnIn_, IRExpr* funcExpr_, IRExprList* args_, IRBIRId birId_, DIEBase* die_);
  //! \brief Built-in procedure call.
  IRSBuiltInCall(IRExpr* funcExpr_, IRExprList* args_, IRBIRId birId_, DIEBase* die_);
  virtual ~IRSBuiltInCall() {}
public:
  bool IsFunction() const {return mIsFunction;}
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRStmtType GetStmtType() const {return IRSTBICall;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(StmtVisitor& visitor_);
  IRBIRId GetBIRId() const {return mBIRId;}
private:
  //! \pre Do not call this function.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRBIRId mBIRId;
  //! \brief Should be set to true if this is a function. If it is
  //!        a procedure it should be false.
  bool mIsFunction;
}; // class IRSBuiltInCall

//! \ingroup hirstatements
//! \brief Label statement.
class IRSLabel : public IRStmt {
private:
  IRSLabel(const IRSLabel& label_) : IRStmt(label_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSLabel& operator = (const IRSLabel& label_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSLabel(const Invalid<IRSLabel>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  IRSLabel(const string& name_, DIEBase* die_);
  virtual ~IRSLabel() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  const char* GetName() const {return mName.c_str();}
  void SetAddressTaken() {mAddressTaken = true;}
  bool GetAddressTaken() const {return mAddressTaken;}
  virtual IRStmtType GetStmtType() const {return IRSTLabel;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual IRTree* GetRecCopy() const {
    return new IRSLabel(mName, diGetDIE()->Clone());
  } // IRSLabel::GetRecCopy
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_) {return true;}
private:
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {return false;}
private:
  //! \attention Do not implement this function for label statement.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  string mName;
  bool mAddressTaken; //!< True if address of label is taken, e.g. by &&label in GCC.
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSLabel

//! \ingroup hirstatements
//! \brief IR level \p if statement construct.
//!
class IRSIf : public IRStmt {
private:
  IRSIf(const IRSIf& if_) : IRStmt(if_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSIf& operator = (const IRSIf& if_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSIf(const Invalid<IRSIf>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  IRSIf(IRExpr* condExpr_, IRBB* trueCaseBB_, IRBB* falseCaseBB_, DIEBase* die_);
  virtual ~IRSIf() {}
public:
  virtual bool IsValid() const;
  //! \brief Returns the basic block that is jumped in case the condition expression is true.
  IRBB* GetTrueCaseBB() const {return mTrueCaseBB;}
  //! \brief Returns the basic block that is jumped in case the condition expression is false.
  IRBB* GetFalseCaseBB() const {return mFalseCaseBB;}
  IRExpr* GetExpr() const {return mCondExpr;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {
    exprs_.push_back(mCondExpr);
    return true;
  } // IRSIf::GetChildren
  virtual IRTree* GetRecCopy() const {
    return new IRSIf(mCondExpr->GetRecCopyExpr(), mTrueCaseBB, mFalseCaseBB, diGetDIE()->Clone());
  } // IRSIf::GetRecCopy
  virtual IRStmtType GetStmtType() const {return IRSTIf;}
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_);
private:
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  //! \brief Condition expression.
  IRExpr* mCondExpr;
  //! \brief Basic block that will be taken when the \p condExpr is evaluated to true.
  IRBB* mTrueCaseBB;
  //! \brief Basic block that will be taken when the \p condExpr is evaluated to false.
  IRBB* mFalseCaseBB;
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
}; // class IRSIf

//! \ingroup hirstatements
//! \brief Null statement construct.
class IRSNull : public IRStmt {
private:
  IRSNull(const IRSNull& null_) : IRStmt(null_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSNull& operator = (const IRSNull& null_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSNull(const Invalid<IRSNull>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  IRSNull(DIEBase* die_) :
    IRStmt(0, 0, 0, die_)
  {
  } // IRSNull::IRSNull
  virtual ~IRSNull() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRStmtType GetStmtType() const {return IRSTNull;}
  virtual IRTree* GetRecCopy() const {
    return new IRSNull(diGetDIE()->Clone());
  } // IRSNull::GetRecCopy
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_) {return true;}
private:
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {return false;}
  //! \attention Do not implement this function for null statements.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSNull

//! \brief Virtual statement signifying the last statement of a basic block.
//! \ingroup hirstatements
class IRSAfterLastOfBB : public IRStmt {
public:
  IRSAfterLastOfBB(IRBB* parent_) :
    IRStmt(parent_, 0, 0, new DIEBase(SrcLoc()))
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRSAfterLastOfBB::IRSLastOfStmt
  virtual ~IRSAfterLastOfBB() {}
public:
  //! \warning You may not call GetBB of singleton IRAfterLastOfBB.
  virtual IRBB* GetBB() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRStmtType GetStmtType() const {return IRSTAfterLastOfBB;}
  virtual IRTree* GetRecCopy() const {return new IRSAfterLastOfBB(GetBB());}
private:
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {return false;}
  //! \attention Do not implement this function for IRSAfterLastOfBB statement.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRSAfterLastOfBB() :
    IRStmt(0, 0, 0, new DIEBase(SrcLoc()))
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRSAfterLastOfBB::IRSLastOfStmt
  virtual IRStmt* GetPrevStmt() const {
    ASSERTMSG(0, ieStrFuncShouldNotBeCalled);
    return 0;
  } // IRSAfterLastOfBB::GetPrevStmt
  //! \brief Next statement of IRSAfterLastOfBB is itself, this.
  virtual IRStmt* GetNextStmt() const {
    return const_cast<IRSAfterLastOfBB*>(this);
  } // IRSAfterLastOfBB::GetNextStmt
  //! \brief IRSAfterLastOfBB is always a valid object.
  virtual bool IsValid() const;
private:
  template<typename T> friend class Singleton;
}; // class IRSAfterLastOfBB

//! \brief Virtual statement signifying the first statement of a basic block.
//! \note Use this object in statements prev and next links. Do not insert it
//! in to a basic block.
//! \todo L Design: prevent assignment of IRSBeforeFirstOfBB, IRSAfterLastOfBB.
//! \ingroup hirstatements
class IRSBeforeFirstOfBB : public IRStmt {
public:
  IRSBeforeFirstOfBB(IRBB* parent_) :
    IRStmt(parent_, 0, 0, new DIEBase(SrcLoc()))
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRSBeforeFirstOfBB::IRSFirstOfStmt
  virtual ~IRSBeforeFirstOfBB() {}
public:
  //! \warning You may not call GetBB of singleton IRSBeforeFirstOfBB.
  virtual IRBB* GetBB() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {return false;}
  virtual IRStmtType GetStmtType() const {return IRSTBeforeFirstOfBB;}
  virtual IRTree* GetRecCopy() const {return new IRSBeforeFirstOfBB(GetBB());}
private:
  //! \attention Do not implement this function for IRSBeforeFirstOfBB statement.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRSBeforeFirstOfBB() :
    IRStmt(0, 0, 0, new DIEBase(SrcLoc()))
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRSBeforeFirstOfBB::IRSFirstOfStmt
  //! \brief Previous statement of IRSBeforeFirstOfBB is the same statement, or this.
  virtual IRStmt* PrevStmt() const {
    return const_cast<IRSBeforeFirstOfBB*>(this);
  } // IRSBeforeFirstOfBB::GetPrev
  virtual IRStmt* NextStmt() const {
    ASSERTMSG(0, ieStrFuncShouldNotBeCalled);
    return 0;
  } // IRSBeforeFirstOfBB::GetPrev
  //! \brief IRSBeforeFirstOfBB is always a valid object.
  virtual bool IsValid() const;
private:
  template<typename T> friend class Singleton;
}; // class IRSBeforeFirstOfBB

//! \ingroup hirstatements
//! \brief Go-to statement construct.
//!
//! A Jump must always target a basic block.
class IRSJump : public IRStmt {
private:
  IRSJump& operator = (const IRSJump& jump_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSJump(const Invalid<IRSJump>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  //! \param[in] targetBB_ Target basic block.
  IRSJump(IRBB* targetBB_, DIEBase* die_) :
    IRStmt(0, 0, 0, die_),
    mTargetBB(targetBB_)
  {
    REQUIREDMSG(mTargetBB != 0, ieStrNonNullParam);
  } // IRSJump::IRSJump
  virtual ~IRSJump() {}
public:
  //! \pre Valid \p targetBB.
  //! \pre Valid base class IRStmt.
  virtual bool IsValid() const;
  //! \pre Valid \p this.
  IRBB* GetTargetBB() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {return false;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual IRStmtType GetStmtType() const {return IRSTJump;}
  virtual IRTree* GetRecCopy() const {return new IRSJump(mTargetBB, diGetDIE()->Clone());}
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_) {return true;}
private:
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRBB* mTargetBB;
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSJump

//! \brief Dynamic Jump class, for supporting the GCC features.
//!
//! Dynamic Jumps are quite different than normal Jumps therefore
//! it is not derived from IRSJump.
//! \note The target of dynamic Jumps are assumed to be with the contained
//!       function. The CFG assumes there is an edge to all the basic blocks
//!       of the function from the dynamic Jump basic block. Therefore,
//!       it is not good for optimizations. Try to create dynamic Jumps
//!       as late as possible in the compilation. A Switch statement may be
//!       lowered by using dynamic Jump.
//! \ingroup hirstatements
class IRSDynJump : public IRStmt {
protected:
  IRSDynJump(const Invalid<IRSDynJump>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  //! \param targetExpr_ Any expression that must evaluate to an address.
  IRSDynJump(IRExpr* targetExpr_, DIEBase* die_);
  virtual ~IRSDynJump() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {
    exprs_.push_back(mTargetExpr);
    return true;
  } // IRSDynJump::GetChildren
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  virtual IRStmtType GetStmtType() const {return IRSTDynJump;}
  virtual IRTree* GetRecCopy() const {
    return new IRSDynJump(mTargetExpr->GetRecCopyExpr(), diGetDIE()->Clone());
  } // IRSDynJump::GetRecCopy
  virtual bool Accept(StmtVisitor& visitor_);
private:
  //! \pre You may not call this function.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRExpr* mTargetExpr;
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSDynJump

//! \ingroup hirstatements
//! \brief Return statement construct.
class IRSReturn : public IRStmt {
private:
  IRSReturn(const IRSReturn& return_) : IRStmt(return_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSReturn& operator = (const IRSReturn& return_);
protected:
  IRSReturn(const Invalid<IRSReturn>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  //! \param retExpr_ May be IRENull in case of "return;" statement.
  IRSReturn(IRExpr* retExpr_, DIEBase* die_);
  virtual ~IRSReturn() {}
public:
  virtual bool IsValid() const;
  IRExpr* GetExpr() const {return mRetExpr;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {
    exprs_.push_back(mRetExpr);
    return true;
  } // IRSReturn::GetChildren
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  virtual IRStmtType GetStmtType() const {return IRSTReturn;}
  virtual IRTree* GetRecCopy() const {
    return new IRSReturn(mRetExpr->GetRecCopyExpr(), diGetDIE()->Clone());
  } // IRSReturn::GetRecCopy
  virtual bool Accept(StmtVisitor& visitor_);
private:
  //! \attention Although return statement creates a jump target, i.e. to the exit block,
  //! it is not possible to remap its target. So this function should not be called for return
  //! statements.
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
  IRExpr* mRetExpr;
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSReturn

//! \ingroup hirstatements
//! \brief Prolog statement for function frame setup generation.
//!
//! \par What is a prolog?
//! Function prolog statement is responsible from setting up a function
//! frame. Similarly epilog statement cleans up the frame.
//! For some functions there may not be a need to setup a frame.
//! In an optimized code generation there may be different frames.
//! A function that is called may need to save some set of registers,
//! these are called callee save registers. If one or more of these
//! registers are not re-defined in the function there is no need to
//! save them. Not saving some or all of registers usually translates in to a
//! faster, better program.
//!
//! When there are local temporaries used in a function we again need to set up a frame.
//! We need to allocate stack space for the locals. Usually the size of
//! locals are known at compile time. Therefore, there is no need to save
//! this information at run-time. We need the size of stack since in the
//! epilog we need to free the stack allocated for the function.
//! It is possible that stack size is not known at compile time,
//! this happens with _alloca kind of functions that allocates spaces in the
//! stack, rather than in the heap.
//!
//! When the stack size is not known at compile time generally a HW register
//! is used to point the incoming stack at the prolog. The register is
//! called 'frame pointer', fp. Also when generating debug information, it is
//! somehow easier to locate local variables, they are all at "fp+const offset".
//! In an optimized code we try to avoid the use of frame pointer since it is
//! a valueable resource.
//!
//! In the prolog we may also need to save "link register". Link register
//! is a dedicated HW register to store the function return addresses.
//! In the prolog we need to save link registers if we use the link register
//! as a general purpose register or when we call another function.
//!
//! To summarize, in the prolog, we check for the callee save registers used,
//! link register use, these may need to be saved. Then we may need to save
//! stack pointer, to make frame cleanup easy or debug information easy. Then
//! we need to allocate stack space. Prolog/Epilog may need to know the calling
//! convention of the function.
//!
//! \par References
//! - Dragon book, Activation Records, in Storage Organization.
class IRSProlog : public IRStmt {
private:
  IRSProlog(const IRSProlog& prolog_) : IRStmt(prolog_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSProlog& operator = (const IRSProlog& prolog_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSProlog(const Invalid<IRSProlog>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  IRSProlog(DIEBase* die_) :
    IRStmt(0, 0, 0, die_)
  {
  } // IRSProlog::IRSProlog
  virtual ~IRSProlog() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRStmtType GetStmtType() const {return IRSTProlog;}
  virtual IRTree* GetRecCopy() const {return new IRSProlog(diGetDIE()->Clone());}
  virtual bool Accept(StmtVisitor& visitor_);
private:
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {return false;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSProlog

//! \ingroup hirstatements
//! \brief Epilog statement for function frame clean up generation.
class IRSEpilog : public IRStmt {
private:
  IRSEpilog(const IRSEpilog& epilog_) : IRStmt(epilog_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSEpilog& operator = (const IRSEpilog& epilog_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSEpilog(const Invalid<IRSEpilog>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  IRSEpilog(DIEBase* die_) :
    IRStmt(0, 0, 0, die_)
  {
  } // IRSEpilog::IRSEpilog
  virtual ~IRSEpilog() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRStmtType GetStmtType() const {return IRSTEpilog;}
  virtual IRTree* GetRecCopy() const {return new IRSEpilog(diGetDIE()->Clone());}
  virtual bool Accept(StmtVisitor& visitor_);
private:
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {return false;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  //! \todo M Design: Not sure if epilog should create a control flow to
  //! exit block or not at all (creates multiple return functions, perhaps
  //! better in optimized code.)
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSEpilog

//! \ingroup hirstatements
//! \brief Switch statement construct.
//! \note IRSSwitch must be lowered before the code generation, see ATOLowerHIR.
class IRSSwitch : public IRStmt {
private:
  IRSSwitch(const IRSSwitch& switch_) : IRStmt(switch_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
  IRSSwitch& operator = (const IRSSwitch& switch_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
protected:
  IRSSwitch(const Invalid<IRSSwitch>* inv_) : IRStmt(&Invalid<IRStmt>::Obj()) {}
public:
  IRSSwitch(IRExpr* value_, const vector<pair<IRExprList*, IRBB*> >& caseStmts_, DIEBase* die_);
  virtual ~IRSSwitch() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  virtual bool GetChildren(vector<IRExpr*>& exprs_) const {
    exprs_.push_back(mValueExpr);
    return true;
  } // IRSSwitch::GetChildren
  virtual IRStmtType GetStmtType() const {return IRSTSwitch;}
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(StmtVisitor& visitor_);
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  IRExpr* GetExpr() const {return mValueExpr;}
  void GetCases(vector<pair<IRExprList*, IRBB*> >& cases_) {
    cases_ = mCaseStmts;
  } // IRSSwitch::GetCases
private: // Member functions
  virtual bool remapJumpTargets(IRBB* from_, IRBB* to_);
private: // Data
  IRExpr* mValueExpr; //!< The switch value.
  //! \brief Case statements. Expression list is list of constant labels for a
  //!        case statement. Corresponding basic block holds the target statements.
  //! \note An empty expression list is the default label.
  vector<pair<IRExprList*, IRBB*> > mCaseStmts;
private:
  friend class Extract;
  friend class IRBuilder;
  friend class Predicate;
}; // class IRSSwitch

// ///////////////////////////
// EXPRESSIONS
// ///////////////////////////

// --------------------------
// Arithmetic Expressions
// --------------------------

//! \ingroup hirexpressions
//! \brief Base class of unary operations.
class IREUnary : public IRExpr {
protected:
  IREUnary(const Invalid<IREUnary>* inv_) : IRExpr(&Invalid<IRExpr>::Obj()) {}
public:
  IREUnary(IRType* type_, IRExpr* operand_);
  IREUnary(const IREUnary& unary_);
  virtual ~IREUnary() {}
public:
  const IRExpr* GetExpr() const {return mOperand;}
  IRExpr* GetExpr() {return mOperand;}
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool GetChildren(vector<IRExpr*>& children_) const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
private:
  IRExpr* mOperand;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREUnary

//! \brief Base class of binary operations.
//! \note IRBuilder arranges the binary operands so that 
//! right expression is always the constant, if present. In other words, there
//! is no "const binop expr_tree", but always "expr_tree binop const" is created.
class IREBinary : public IRExpr {
public:
  IREBinary(IRType* type_, IRExpr* leftExpr_, IRExpr* rightExpr_);
  //! \todo M Design: Check if we need to control 'expr used before'?
  IREBinary(const IREBinary& binary_);
  virtual ~IREBinary() {}
public:
  virtual bool IsValid() const;
  IRExpr* GetLeftExpr() const;
  IRExpr* GetRightExpr() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool GetChildren(vector<IRExpr*>& children_) const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
private:
  IRExpr* mLeftExpr;
  IRExpr* mRightExpr;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREBinary

//! \ingroup hirexpressions
//! \brief Addition operator.
class IREAdd : public IREBinary {
private:
  IREAdd& operator = (const IREAdd& add_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \pre \p left and \p right must be both IRTReal or IRTInt.
  IREAdd(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    BMDEBUG3("IREAdd::IREAdd", left_, right_);
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
    EMDEBUG0();
  } // IREAdd::IREAdd
  IREAdd(const IREAdd& add_) :
    IREBinary(add_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREAdd::IREAdd
  virtual ~IREAdd() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREAdd(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREAdd

//! \ingroup hirexpressions
//! \brief Subtraction operator.
//!
//! \warning Beware, do not make a typo, IRESubscript is also present!
class IRESub : public IREBinary {
private:
  IRESub& operator = (const IRESub& sub_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTReal, IRTInt. Use IREPtrSub
  //!       for subtraction of pointer types.
  //! \sa IREPtrSub.
  IRESub(IRType* type_, IRExpr* left_, IRExpr* right_) :
    IREBinary(type_, left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRESub::IRESub
  IRESub(const IRESub& sub_) :
    IREBinary(sub_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRESub::IRESub
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRESub(GetType(), GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRESub

//! \todo M Design: Do we need the assignment operator for expressions? Just make them private?

//! \ingroup hirexpressions
//! \brief Division operator.
class IREDiv : public IREBinary {
private:
  IREDiv& operator = (const IREDiv& div_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTReal or IRTInt.
  IREDiv(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREDiv::IREDiv
  IREDiv(const IREDiv& div_) :
    IREBinary(div_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREDiv::IREDiv
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREDiv(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREDiv

//! \ingroup hirexpressions
//! \brief Multiplication operator.
class IREMul : public IREBinary {
private:
  IREMul& operator = (const IREMul& mul_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTReal or IRTInt.
  IREMul(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREMul::IREMul
  IREMul(const IREMul& mul_) :
    IREBinary(mul_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREMul::IREMul
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREMul(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREMul

//! \ingroup hirexpressions
//! \brief Remainder operator.
class IRERem : public IREBinary {
private:
  IRERem& operator = (const IRERem& rem_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTInt.
  IRERem(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRERem::IRERem
  IRERem(const IRERem& rem_) :
    IREBinary(rem_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRERem::IRERem
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRERem(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IRERem

//! \ingroup hirexpressions
//! \brief Modulo operator.
//! \todo M Design: Modulo and Remainder is same? Remove one of them.
class IREMod : public IREBinary {
private:
  IREMod& operator = (const IREMod& mod_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREMod(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREMod::IREMod
  IREMod(const IREMod& mod_) :
    IREBinary(mod_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREMod::IREMod
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREMod(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREMod

//! \ingroup hirexpressions
//! \todo M Design: We may not need this operator?
class IREQuo : public IREBinary {
private:
  IREQuo& operator = (const IREQuo& quo_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREQuo(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
    {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREQuo::IREQuo
  IREQuo(const IREQuo& quo_) :
    IREBinary(quo_)
    {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREQuo::IREQuo
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREQuo(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREQuo

//! \ingroup hirexpressions
//! \brief Arithmetic negation.
class IREANeg : public IREUnary {
private:
  IREANeg& operator = (const IREANeg& neg_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p expr must be IRTReal or IRTInt.
  IREANeg(IRExpr* expr_) :
    IREUnary(expr_->GetType(), expr_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREANeg::IREANeg
  IREANeg(const IREANeg& neg_) :
    IREUnary(neg_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREANeg::IREANeg
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREANeg(GetExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREANeg

// --------------------------
// Bitwise Operations
// --------------------------

//! \ingroup hirexpressions
//! \brief Bitwise negation.
//!
//! \warning Beware, do not make a typo, IRENe is also present!
class IREBNeg : public IREUnary {
private:
  IREBNeg& operator = (const IREBNeg& neg_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p expr must be IRTInt.
  IREBNeg(IRExpr* expr_) :
    IREUnary(expr_->GetType(), expr_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBNeg::IREBNeg
  IREBNeg(const IREBNeg& neg_) :
    IREUnary(neg_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBNeg::IREBNeg
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREBNeg(GetExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREBNeg

//! \ingroup hirexpressions
//! \brief Logical not.
class IRENot : public IREUnary {
private:
  IRENot& operator = (const IRENot& lnot_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p expr_ must be of type IRTBool.
  IRENot(IRExpr* expr_) :
    IREUnary(expr_->GetType(), expr_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBNeg::IREBNeg
  IRENot(const IRENot& lnot_) :
    IREUnary(lnot_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRENot::IRENot
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRENot(GetExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRENot

//! \ingroup hirexpressions
//! \brief Bitwise and.
class IREBAnd : public IREBinary {
private:
  IREBAnd& operator = (const IREBAnd& bAnd_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTInt.
  IREBAnd(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBAnd::IREBAnd
  IREBAnd(const IREBAnd& bAnd_) :
    IREBinary(bAnd_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBAnd::IREBAnd
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREBAnd(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREBAnd

//! \ingroup hirexpressions
//! \brief Logical and.
class IRELAnd : public IREBinary {
private:
  IRELAnd& operator = (const IRELAnd& lAnd_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTBool.
  IRELAnd(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRELAnd::IRELAnd
  IRELAnd(const IRELAnd& lAnd) :
    IREBinary(lAnd)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRELAnd::IRELAnd
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRELAnd(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRELAnd

//! \ingroup hirexpressions
//! \brief Bitwise xor.
class IREBXOr : public IREBinary {
private:
  IREBXOr& operator = (const IREBXOr& bXOr_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTInt.
  IREBXOr(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBXOr::IREBXOr
  IREBXOr(const IREBXOr& bXOr_) :
    IREBinary(bXOr_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBXOr::IREBXOr
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREBXOr(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREBXOr

//! \ingroup hirexpressions
//! \brief Bitwise or.
class IREBOr : public IREBinary {
private:
  IREBOr& operator = (const IREBOr& bOr_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTInt.
  IREBOr(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBOr::IREBOr
  IREBOr(const IREBOr& bOr_) :
    IREBinary(bOr_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREBOr::IREBOr
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREBOr(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  } // IREBOr::
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREBOr

//! \ingroup hirexpressions
//! \brief Logical or.
class IRELOr : public IREBinary {
private:
  IRELOr& operator = (const IRELOr& lOr_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \note \p left and \p right must be both IRTBool.
  IRELOr(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRELOr::IRELOr
  IRELOr(const IRELOr& lOr) :
    IREBinary(lOr)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRELOr::IRELOr
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRELOr(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  } // IRELOr::
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRELOr

// --------------------------
// Type Conversions
// --------------------------

//! \ingroup hirexpressions
//! \brief Type cast operator.
//!
//! Type cast operator changes the type of its operand. Typecast
//! does not modify the bit representation of its operand.
//! Type cast is useful for type conversions of pointer types.
//!
//! Type cast of an int into float will result in wrong value.
//! Since bit representation of ints are different than floats
//! (for many values except 1 or 2).
//!
//! \sa IREConv.
class IRECast : public IREUnary {
protected:
  IRECast(const Invalid<IRECast>* inv_) : IREUnary(&Invalid<IREUnary>::Obj()) {}
private:
  IRECast& operator = (const IRECast& cast_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRECast(IRType* targetType_, IRExpr* expr_) :
    IREUnary(targetType_, expr_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRECast::IRECast
  IRECast(const IRECast& cast_) :
    IREUnary(cast_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRECast::IRECast
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRECast(GetType(), GetExpr()->GetRecCopyExpr());
  } // IRECast::GetRecCopy
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRECast

// --------------------------
// Comparison Expressions
// --------------------------
//! \ingroup hirexpressions
//! \brief Base class for all comparison operators, lt, gt, ne, eq, le, ge.
class IRECmp : public IREBinary {
private:
  IRECmp& operator = (const IRECmp& cmp_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRECmp(IRExpr* left_, IRExpr* right_) :
    IREBinary(IRECmp::cmpBoolType, left_, right_)
  {
    BMDEBUG3("IRECmp::IRECmp", left_, right_);
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
    EMDEBUG0();
  } // IRECmp::IRECmp
  IRECmp(const IRECmp& cmp_) :
    IREBinary(cmp_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRECmp::IRECmp
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \todo M Design: Go over all public data members and place them to private.
  static IRTBool* cmpBoolType;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  //! \brief Must return the inverse relation of this expression.
  //! \note it must use recursive copy of operands.
  virtual IRECmp* GetInverse() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // IRECmp::GetInverse
private:
  virtual IRTree* GetRecCopy() const {
    return new IRECmp(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  } // IRECmp::GetRecCopy
  friend class Extract;
  friend class Predicate;
}; // class IRECmp

//! \ingroup hirexpressions
//! \brief Greater than comparison operator.
class IREGt : public IRECmp {
private:
  IREGt& operator = (const IREGt& gt_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREGt(IRExpr* left_, IRExpr* right_) :
    IRECmp(left_, right_)
  {
    BMDEBUG1("IREGt::IREGt");
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
    EMDEBUG0();
  } // IREGt::IREGt
  IREGt(const IREGt& gt_) :
    IRECmp(gt_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREGt::IREGt
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRECmp* GetInverse() const;
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREGt

//! \ingroup hirexpressions
//! \brief Less than comparison operator.
class IRELt : public IRECmp {
private:
  IRELt& operator = (const IRELt& lt_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRELt(IRExpr* left_, IRExpr* right_);
  IRELt(const IRELt& lt_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRECmp* GetInverse() const;
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRELt

//! \ingroup hirexpressions
//! \brief Greater than or equal comparison operator.
class IREGe : public IRECmp {
private:
  IREGe& operator = (const IREGe& ge_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREGe(IRExpr* left_, IRExpr* right_);
  IREGe(const IREGe& ge_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRECmp* GetInverse() const;
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREGe

//! \ingroup hirexpressions
//! \brief Less than or equal comparison operator.
class IRELe : public IRECmp {
private:
  IRELe& operator = (const IRELe& le_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRELe(IRExpr* left_, IRExpr* right_);
  IRELe(const IRELe& le_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRECmp* GetInverse() const;
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRELe

//! \ingroup hirexpressions
//! \brief Equal comparison operator.
class IREEq : public IRECmp {
private:
  IREEq& operator = (const IREEq& eq_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREEq(IRExpr* left_, IRExpr* right_);
  IREEq(const IREEq& eq_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRECmp* GetInverse() const;
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREEq

//! \ingroup hirexpressions
//! \brief Inequality comparison operator.
class IRENe : public IRECmp {
private:
  IRENe& operator = (const IRENe& ne_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRENe(IRExpr* left_, IRExpr* right_);
  IRENe(const IRENe& ne_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRECmp* GetInverse() const;
  virtual IRTree* GetRecCopy() const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRENe

// --------------------------
// Constants
// --------------------------

//! \brief Base class for literal constants, int, real, string, bool.
class IREConst : public IRExpr {
protected:
  IREConst(const Invalid<IREConst>* inv_) : IRExpr(&Invalid<IRType>::Obj()) {}
public:
  IREConst(IRType* type_) : IRExpr(type_) {}
  virtual ~IREConst() {}
public:
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
}; // class IREConst

//! \ingroup hirexpressions
//! \brief Constant value of type integer.
//!
//! Operations on constant operands must be performed as
//! if performed on the target hardware. Therefore, the value type
//! of constants are of target types. Moreover, representation of value
//! is of type tBigInt, which is sufficient enough to hold any target value.
//! \note Note that you can create IREIntConst from host type, see the
//! IRBuilder, IRBuilderHelper.
//! \note We need the \a IREIntConst::type of the constant since
//! the \a IREIntConst::value member is actually the bit representation.
//! For example, when value is "0xff" for target type <signed, 8bits>,
//! and we add it to another 0xff 16 bits target type, result is 255 -1 = 254.
//! \sa IRBuilder::irbeIntConst IRBuilderHelper::irbeIntConst
//! \todo M Design: can we change the constructors type to target type? Or better keep it here like this,
//! but in irbuilder/helper provide only target type interface for construction.
class IREIntConst : public IREConst {
private:
  IREIntConst& operator = (const IREIntConst& intConst_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREIntConst(IRTInt* type_, const tBigInt& tbi_);
  IREIntConst(const IREIntConst& intConst_);
  virtual ~IREIntConst() {}
public:
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREIntConst(static_cast<IRTInt*>(GetType()), mValue);
  }
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& children_) const {
    REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
    return false;
  } // IREIntConst::GetChildren
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
public:
  tBigInt GetValue() const {return mValue;}
private:
  tBigInt mValue; //!< The constant's bit representation.
  friend class Extract;
  friend class Predicate;
}; // class IREIntConst

//! \brief Representation of string literals.
class IREStrConst : public IREConst {
private:
  IREStrConst& operator = (const IREStrConst& strConst_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREStrConst(string value_);
  IREStrConst(const IREStrConst& strConst_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREStrConst(mValue);
  } // IREStrConst::GetRecCopy
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& children_) const {
    REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
    return false;
  } // IREStrConst::GetChildren
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  static IRTPtr* strConstStrType;
  const string& GetValue() const {return mValue;}
private:
  string mValue;
  friend class Extract;
  friend class Predicate;
}; // class IREStrConst

//! \ingroup hirexpressions
//! \brief Constant value of type boolean.
//! \todo Done H Design: Do we need this class, or bool constant? -> Yes if we want to propagate a true or false
//! value to a conditional statement, e.g. IRSIf.
class IREBoolConst : public IREConst {
private:
  IREBoolConst& operator = (const IREBoolConst& boolConst_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREBoolConst(bool value_);
  IREBoolConst(const IREBoolConst& boolConst_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {return new IREBoolConst(mValue);}
  bool GetValue() const {return mValue;}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& children_) const {
    REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
    return false;
  } // IREBoolConst::GetChildren
  static IRTBool* boolConstBoolType;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
private:
  bool mValue;
  friend class Extract;
  friend class Predicate;
}; // class IREBoolConst

//! \ingroup hirexpressions
//! \brief Constant value of type real.
//! \par Similar to IREIntConst, target type is used as value
//! representation.
//! \sa IREIntConst
//! \todo M Design: Should the floating point values be stored as byte array?
//!       This makes it easy to store them, but we may not be able to do
//!       constant folding like optimizations.
class IRERealConst : public IREConst {
private:
  IRERealConst& operator = (const IRERealConst& realConst_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRERealConst(IRTReal* type_, const BigReal& br_);
  IRERealConst(const IRERealConst& realConst_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRERealConst(static_cast<IRTReal*>(GetType()), mValue);
  }
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& children_) const {
    REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
    return false;
  } // IRERealConst::GetChildren
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  tBigReal GetValue() const {return mValue;}
private:
  tBigReal mValue; //!< The constant's value.
  friend class Extract;
  friend class Predicate;
}; // class IRERealConst

//! \ingroup hirexpressions
//! \brief Constant value of type address.
//! An Address constant has two parts. One part is the symbolic part
//! the other part is the integer offset part. Symbolic part is an IRObject
//! (address of a variable, routine, or label). Integer offset part must be
//! able to hold target pointer differences. We use tBigAddr for holding
//! the offset part since this can be different in different targets.
//! \note Pointer initializations:
//!      String literal initializations for pointers requires
//!      a temporary global object. Then IREAddrConst is used that points to
//!      the temporary object:
//!      const char* t = "Hello";
//!      char tmpObj[sizeof("Hello")] = "Hello"; // Initialization of tmpObj is of type IREStrConst.
//!      Initialization of 't' is of type IREAddrConst pointing to tmpObj.
//! \todo M Design: split this class in to three, AddrConstObj, Label, Value.
class IREAddrConst : public IREConst {
private:
  IREAddrConst& operator = (const IREAddrConst& addrConst_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \brief Address constant expression from constant value.
  //!
  //! \note This expression happens when programmers know the address
  //!       of the object, e.g. a port. "  int* p = (int*)0xfeebdaed;"
  //! \pre type_ must be of pointer type.
  IREAddrConst(IRType* type_, const tBigInt& constValue_);
  //! \brief Address constant expression "&&lab + 1" cases, happens in GNU only.
  //! \pre type_ must be of pointer type.
  IREAddrConst(IRType* type_, IRSLabel* label_, const tBigAddr& offset_);
  //! \brief Address constant expression "&var + const" cases.
  //! \param type_ Typically pointer to the object_ type.
  //! \pre type_ must be of pointer type.
  IREAddrConst(IRType* type_, IRObject* object_, const tBigAddr& offset_);
  IREAddrConst(const IREAddrConst& addrConst_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREAddrConst(*this);
  } // IREAddrConst::GetRecCopy
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& children_) const {
    REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
    return false;
  } // IREAddrConst::GetChildren
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
  bool IsObject() const {
    return mObject != &Invalid<IRObject>::Obj();
  } // IREAddrConst::IsObject
  bool IsLabel() const {
    return mLabel != &Invalid<IRSLabel>::Obj();
  } // IREAddrConst::IsLabel
  bool IsConst() const {
    return !IsLabel() && !IsObject();
  } // IREAddrConst::IsConst
  tBigAddr GetOffset() const {
    REQUIREDMSG(IsLabel() || IsObject(), ieStrFuncShouldNotBeCalled);
    return mOffset;
  } // IREAddrConst::GetOffset
  IRObject* GetObject() const {
    REQUIREDMSG(IsObject(), ieStrFuncShouldNotBeCalled);
    return mObject;
  } // IREAddrConst::GetObject
  tBigInt GetConstValue() const {
    REQUIREDMSG(IsConst(), ieStrFuncShouldNotBeCalled);
    return mConstValue;
  } // IREAddrConst::GetConstValue
  IRSLabel* GetLabel() const {
    REQUIREDMSG(IsLabel(), ieStrFuncShouldNotBeCalled);
    return mLabel;
  } // IREAddrConst::GetLabel
private:
  tBigAddr mOffset;
  tBigInt mConstValue;
  IRObject* mObject;
  IRSLabel* mLabel;
  friend class Extract;
  friend class Predicate;
}; // class IREAddrConst

// --------------------------
// Shift Operations
// --------------------------

//! \ingroup hirexpressions
//! \brief Bit shift left operator, note that arithmetic and logical shift left is same.
class IREShiftLeft : public IREBinary {
private:
  IREShiftLeft& operator = (const IREShiftLeft& shiftLeft_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREShiftLeft(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREShiftLeft::IREShiftLeft
  IREShiftLeft(const IREShiftLeft& shiftLeft_) :
    IREBinary(shiftLeft_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREShiftLeft::IREShiftLeft
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREShiftLeft(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREShiftLeft

//! \ingroup hirexpressions
//! \brief Arithmetic shift right operator, preserves the sign bit.
class IREAShiftRight : public IREBinary {
private:
  IREAShiftRight& operator = (const IREAShiftRight& aShiftRight_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IREAShiftRight(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREAShiftRight::IREAShiftRight
  IREAShiftRight(const IREAShiftRight& aShiftRight) :
    IREBinary(aShiftRight)
  {
  } // IREAShiftRight::IREAShiftRight
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREAShiftRight(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IREAShiftRight

//! \ingroup hirexpressions
//! \brief Logical shift right operator, zeros are inserted to the left.
class IRELShiftRight : public IREBinary {
private:
  IRELShiftRight& operator = (const IRELShiftRight& lShiftRight_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRELShiftRight(IRExpr* left_, IRExpr* right_) :
    IREBinary(left_->GetType(), left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRELShiftRight::IRELShiftRight
  IRELShiftRight(const IRELShiftRight& lShiftRight) :
    IREBinary(lShiftRight)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRELShiftRight::IRELShiftRight
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRELShiftRight(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  virtual bool constFold(IRExpr*& foldedExpr_);
private:
  friend class Extract;
  friend class Predicate;
}; // class IRELShiftRight

// --------------------------
// Other Expressions
// --------------------------

//! \ingroup hirexpressions
//! \brief Null, missing expression.
//!
//! Here are example codes:
//! \code return /null expression/; \endcode
//!
//! \code for (; /null expression/; /null expression/) \endcode
//! \note IRENull is also used for missing initialization, e.g.
//!       char buf[10][10] = {{},{"1"}};
//!       Initialization of the first item is missing and it is represented by IRENull.
class IRENull : public IRExpr {
private:
  IRENull& operator = (const IRENull& null_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  IRENull() :
    IRExpr(IRENull::nullVoidType)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRENull::IRENull
  IRENull(const IRENull& null_) :
    IRExpr(nullVoidType)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRENull::IRENull
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  static IRTVoid* nullVoidType;
  virtual IRTree* GetRecCopy() const {return new IRENull(*this);}
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_) {/* LIE */}
  virtual bool GetChildren(vector<IRExpr*>& children_) const {
    REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
    return false;
  } // IRENullConst::GetChildren
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual bool Accept(const CExprVisitor& visitor_) const {return visitor_.Visit(this);}
private:
  friend class Extract;
  friend class Predicate;
}; // class IRENull

//! \ingroup hirexpressions
//! \brief Member operator for class/struct/unions member variables.
//! \note IREMember must be lowered before the code generation, see ATOLowerHIR.
//! \note Base is always of type pointer to a structure.
class IREMember : public IREBinary {
private:
  IREMember& operator = (const IREMember& member_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \brief Creates a member access operation.
  //! \param base_ Must be a pointer expression of reference type
  //! structure.
  //! \param field_ Must be a field of structure that is the reference
  //! type of parameter \p base.
  IREMember(IRExpr* base_, IROField* field_) :
    IREBinary(field_->GetType(), base_, field_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREMember::IREMember
  IREMember(const IREMember& member_) :
    IREBinary(member_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREMember::IREMember
  //! \post Returns pointer to a struct typed address.
  IRExpr* GetBase() const {return GetLeftExpr();}
  IROField* GetMember() const {return static_cast<IROField*>(GetRightExpr());}
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREMember(GetLeftExpr()->GetRecCopyExpr(),
      static_cast<IROField*>(GetRightExpr()->GetRecCopyExpr()));
  } // IREMember::GetRecCopy
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREMember

//! \ingroup hirexpressions
//! \brief Subscript operator for arrays.
//!
//! \warning Beware, do not make a typo, IRESub is also present!
//! \note IRESubscript must be lowered before the code generation, see ATOLowerHIR.
//! \todo M Design: Would it be better to have two different subscript
//!       expressions, one for array object subscripts the other for pointer
//!       typed subscripts. Point type would be present just for easy of
//!       expression creation.
class IRESubscript : public IREBinary {
private:
  IRESubscript& operator = (const IRESubscript& subscript_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \param type_ Type should be provided explicitly (Actually this
  //! object should be created by IRBuilder/IRBuilderHelper). It is the
  //! dereferenced type of \p basePtr.
  //! \param basePtr_ A pointer typed base pointer of expression, basePtr[index].
  //! \param index_ Index expression of subscript expression.
  IRESubscript(IRType* type_, IRExpr* basePtr_, IRExpr* index_) :
    IREBinary(type_, basePtr_, index_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRESubscript::IRESubscript
  IRESubscript(const IRESubscript& subscript_) :
    IREBinary(subscript_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IRESubscript::IRESubscript
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IRESubscript(GetType(), GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  } // IRESubscript::GetRecCopy
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IRESubscript

//! \ingroup hirexpressions
//! \brief Address of operator.
class IREAddrOf : public IRExpr {
private:
  IREAddrOf& operator = (const IREAddrOf& addrOf_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \param addrTypeOfObj_ This type can be derived from \p obj->GetType(),
  //! however we expect it to be explicitly provided by IRBuilder/IRBuilderHelper.
  //! \param obj_ Object whose address is taken.
  //! \todo M Design: IRObject is an expression, so we may not need this function.
  IREAddrOf(IRType* addrTypeOfObj_, IRObject* obj_);
  IREAddrOf(IRType* addrTypeOfObj_, IRSLabel* label_);
  IREAddrOf(IRType* addrTypeOfObj_, IRExpr* expr_);
  IREAddrOf(const IREAddrOf& addrOf_);
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const;
  virtual void ReplaceChild(IRExpr* old_, IRExpr* new_);
  virtual bool GetChildren(vector<IRExpr*>& children_) const;
  IRExpr* GetExpr() const;
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  IRSLabel* mLabel; //!< May be null if this is not address of a label.
  IRExpr* mExpr;
  friend class Extract;
  friend class Predicate;
}; // class IREAddrOf

//! \ingroup hirexpressions
//! \brief Dereferencing operator, or read contents.
class IREDeref : public IREUnary {
private:
  IREDeref& operator = (const IREDeref& deref_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \param type_ Type actually can be derived from \p ptr. However,
  //! we expect it to be provided explicitly by IRBuilder/IRBuilderHelper.
  //! \param ptr_ Pointer expression to be dereferenced.
  IREDeref(IRType* type_, IRExpr* ptr_) :
    IREUnary(type_, ptr_)
  {
    REQUIREDMSG(ptr_ != 0, ieStrNonNullParam);
    REQUIREDMSG(type_ != 0, ieStrNonNullParam);
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREDeref::IREDeref
  IREDeref(const IREDeref& deref_) :
    IREUnary(deref_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREDeref::IREDeref
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {return new IREDeref(GetType(), GetExpr()->GetRecCopyExpr());}
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREDeref

//! \ingroup hirexpressions
//! \brief Subtraction of two pointer typed expressions.
//! \note The subtraction is in pointer arithmetic, i.e. result is not in bytes.
//! \note You may need to lower the pointer subtraction before the code generator.
//! \note You should use IREPtrAdd for subtracting integer from pointer, by
//!       negating the integer expression. This class is only for (ptr - ptr)
//!       not for (ptr - int) or (int - ptr).
class IREPtrSub : public IREBinary {
private:
  IREPtrSub& operator = (const IREPtrSub& ptrAdd_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \pre \p left_ and \p right_ must be of pointer type.
  IREPtrSub(IRExpr* left_, IRExpr* right_) :
    IREBinary(IREPtrSub::ptrDiffType, left_, right_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREPtrSub::IREPtrSub
  IREPtrSub(const IREPtrSub& ptrSub_) :
    IREBinary(ptrSub_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREPtrSub::IREPtrSub
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREPtrSub(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  static IRTInt* ptrDiffType;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREPtrSub

//! \ingroup hirexpressions
//! \brief Addition of integer to a pointer.
//! \note The addition is in pointer arithmetic, i.e. \p offset is not in bytes.
//! \note You may need to lower the pointer addition before the code generator.
//! \note There is no subtraction of integer from pointer, you should negate
//!       the integer expression and use IREPtrAdd.
class IREPtrAdd : public IREBinary {
private:
  IREPtrAdd& operator = (const IREPtrAdd& ptrAdd_) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled); return *this;}
public:
  //! \pre \p ptr must be of a pointer type.
  //! \pre \p offset must be of integer type.
  IREPtrAdd(IRExpr* ptr_, IRExpr* offset_) :
    IREBinary(ptr_->GetType(), ptr_, offset_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREPtrAdd::IREPtrAdd
  IREPtrAdd(const IREPtrAdd& ptrAdd_) :
    IREBinary(ptrAdd_)
  {
    ENSURE_VALIDMSG(this, ieStrMustEnsureValid);
  } // IREPtrAdd::IREPtrAdd
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRTree* GetRecCopy() const {
    return new IREPtrAdd(GetLeftExpr()->GetRecCopyExpr(), GetRightExpr()->GetRecCopyExpr());
  }
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
private:
  friend class Extract;
  friend class Predicate;
}; // class IREPtrAdd

//! \brief Representation of induction variables (basic and dependent ones).
class IRInductionVar : public IDebuggable {
public:
  IRInductionVar(IRLoop* parent_, IRStmt* defStmt_, IRObject* basicIndVar_, IRExpr* mulFactor_, IRExpr* addFactor_);
  IRInductionVar(IRLoop* parent_, IRStmt* defStmt_, IRObject* indVar_, IRObject* basisVar_, IRExpr* mulFactor_, IRExpr* addFactor_);
  virtual ~IRInductionVar() {}
public: // Member functions, overridden
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public: // Member functions
  bool isDependent() const {return mIndVar != mBasisVar;}
  bool IsBasic() const {return mIndVar == mBasisVar;}
  IRExpr* GetMulFactor() const {return mMulFactor;}
  //! \note Top node of additive factor can be a unary negation. 
  IRExpr* GetAddFactor() const {return mAddFactor;}
  IRObject* GetIVObj() const {return mIndVar;}
  IRObject* GetBasisIVObj() const {return mBasisVar;}
  IRInductionVar* GetBasisIV() const {return mBasisIV;}
  IRStmt* GetDefStmt() const {return mDefStmt;}
private: // Member data
  IRLoop* mParent; //!< Loop that this induction variable belongs to.
  IRObject* mIndVar; //!< The induction variable.
  //! \brief Basis of this induction variable.
  //! For basic induction variables basis is equal to mIndVar.
  IRObject* mBasisVar;
  IRInductionVar* mBasisIV; //!< Set by the ATOInductionVars, may be null if this is not a valid IV.
  IRExpr* mMulFactor; //!< May be IRENull if not present.
  IRExpr* mAddFactor; //!< May be IRENull if not present.
  IRStmt* mDefStmt; //!< Defining statement of this induction variable.
private:
  friend class Extract;
  friend class Predicate;
  friend class IRBuilder;
  friend class ATOInductionVars;
}; // class IRInductionVar

class IREdge : public IDebuggable {
public:
  IREdge(IRBB* from_, IRBB* to_) :
    mFrom(from_),
    mTo(to_)
  {
    REQUIREDMSG(from_ != 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(to_ != 0, ieStrParamValuesDBCViolation);
  } // IREdge::IREdge
  IREdge(const IREdge& edge_) :
    mFrom(edge_.mFrom),
    mTo(edge_.mTo)
  {
  } // IREdge::IREdge
  virtual ~IREdge() {}
public:
  IRBB* GetFrom() const {return mFrom;}
  IRBB* GetTo() const {return mTo;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  IRBB* mFrom;
  IRBB* mTo;
}; // class IREdge

//! \brief Represents the loop structures in IR.
//!
//! \note Loops belong to a CFG. A loop may have nested loops.
//! Only the natural loops are represented.
//! \note Loops are created in CFG::updateLoops.
//! \sa CFG::updateLoops
class IRLoop : public IDebuggable {
protected:
  IRLoop(Invalid<IRLoop>* inv_) :
    mCFG(&Invalid<CFG>::Obj()),
    mHeader(&Invalid<IRBB>::Obj()),
    mPreHeader(&Invalid<IRBB>::Obj()),
    mParentLoop(0),
    mBasicIndVars(0),
    mDependentIVs(0),
    mIteratesAtLeastOnce(false),
    mExitBBCount(0)
  {
  } // IRLoop::IRLoop
  IRLoop() :
    mCFG(&Invalid<CFG>::Obj()),
    mHeader(&Invalid<IRBB>::Obj()),
    mPreHeader(&Invalid<IRBB>::Obj()),
    mParentLoop(&Invalid<IRLoop>::Obj()),
    mBasicIndVars(0),
    mDependentIVs(0),
    mIteratesAtLeastOnce(false),
    mExitBBCount(0)
  {
  } // IRLoop::IRLoop
public:
  IRLoop(CFG* cfg_, IRLoop* parentLoop_) : 
    mCFG(cfg_),
    mHeader(0),
    mPreHeader(0),
    mParentLoop(parentLoop_),
    mBasicIndVars(0),
    mDependentIVs(0),
    mExitBBCount(0),
    mIteratesAtLeastOnce(false)
  {
  } // IRLoop::IRLoop
  virtual ~IRLoop() {}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  CFG* GetCFG() const {return mCFG;}
  //! \brief Visits statements within the loop.
  virtual bool Accept(StmtVisitor& visitor_);
  //! \brief Visits expressions within the loop.
  virtual bool Accept(ExprVisitor& visitor_);
  virtual bool Accept(const CExprVisitor& visitor_) const;
  //! \brief Returns true if given basic block is in the body of the loop.
  //! \note Preheader is not in loop body.
  bool IsInLoopBody(const IRBB* bb_) const {return mBodyBBs.find(const_cast<IRBB*>(bb_)) != mBodyBBs.end();}
  //! \post Non-null header is returned.
  IRBB* GetHeader() const {
    ENSUREMSG(mHeader != 0, ieStrMustEnsureNonNull);
    return mHeader;
  } // IRLoop::GetHeader
  IRBB* GetPreHeader() {
    updatePreHeader();
    return mPreHeader;
  } // IRLoop::GetPreHeader
  const vector<IRInductionVar*>& GetDepIndVars() {
    updateInductionVariables();
    return *mDependentIVs;
  } // IRLoop::GetBasicIndVars
  const vector<IRInductionVar*>& GetBasicIndVars() {
    updateInductionVariables();
    return *mBasicIndVars;
  } // IRLoop::GetBasicIndVars
  const set<IRBB*>& GetBodyBBs() const {return mBodyBBs;}
  const vector<IREdge>& GetExitEdges() const {return mExitEdges;}
  hFUInt32 GetExitBBCount() const {return mExitBBCount;}
  bool IsIteratesAtLeastOnce() const {return mIteratesAtLeastOnce;}
private: // Member functions
  void updatePreHeader();
  void updateInductionVariables();
private: // Member data
  CFG* mCFG;
  vector<IRInductionVar*>* mBasicIndVars;
  vector<IRInductionVar*>* mDependentIVs;
  vector<IRLoop*> mSharedHeader;
  IRBB* mHeader; //!< Entry point of the loop, dominates all the bbs in the body.
  IRBB* mPreHeader; //!< Initially null, created on demand.
  IRLoop* mParentLoop; //!< May be null if it is top level loop.
  set<IRBB*> mBodyBBs; //!< mPreHeader is not included in body list, mHeader is included.
  vector<IRLoop*> mNestedLoops;
  vector<IREdge> mExitEdges; //!< First of pair is in the loop, second is not.
  hFUInt32 mExitBBCount; //!< Number of different exit blocks.
  bool mIteratesAtLeastOnce; //!< True if loop iterates at least once.
private:
  friend class CFG;
  friend class Predicate;
  friend class Extract;
  friend class ATOInductionVars;
  template<class S> friend class Singleton;
}; // class IRLoop

#if RELEASEBUILD
#include "irs_inline.cpp"
#endif

#endif


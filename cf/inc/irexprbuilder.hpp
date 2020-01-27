// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __IREXPRBUILDER_HPP__
#define __IREXPRBUILDER_HPP__

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif

class IRExprBuilder : public IDebuggable {
public:
  IRExprBuilder(IRExpr* expr_) : mExpr(expr_) {}
  IRExpr* GetExpr() const {return mExpr;}
  virtual bool IsValid() const {
    NOTIMPLEMENTED(M);
    return true;
  } // IRExprBuilder::IsValid
protected:
  //! \brief Returns a recursively copied expression tree.
  //! \note In HIR an expr pointer node can only be present in one place, i.e.
  //!       expression tree may not form a DAG, expressions may not be present
  //!       in two different statements also.
  static IRExpr* getRecCopy(IRExpr* exprTree_);
  void setExpr(IRExpr* expr_) {mExpr = expr_;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  IRExpr* mExpr;
}; // class IRExprBuilder

class IRExprRef : public IRExprBuilder {
protected:
  IRExprRef(IRExpr* expr_) : IRExprBuilder(expr_) {}
  friend class IRExprAnyRef;
  friend class IRExprIntRef;
  friend class IRExprRealRef;
  friend class IRExprPtrRef;
  friend class IRExprBoolRef;
  friend class IRExprAny;
  friend class IRExprInt;
  friend class IRExprReal;
  friend class IRExprPtr;
  friend class IRExprBool;
}; // class IRExprRef

class IRExprIntRef : public IRExprRef {
public:
  IRExprIntRef& operator = (const IRExprIntRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprIntRef() : IRExprRef(&Invalid<IRExpr>::Obj()) {}
  IRExprIntRef GetRecCopy() const {return getRecCopy(GetExpr());}
  //operator IRExpr*() {return GetExpr();}
private:
  IRExprIntRef(IRExpr* expr_) : IRExprRef(expr_) {}
  friend class IRExprInt;
}; // class IRExprIntRef

class IRExprPtrRef : public IRExprRef {
public:
  IRExprPtrRef& operator = (const IRExprPtrRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprPtrRef() : IRExprRef(&Invalid<IRExpr>::Obj()) {}
  IRExprPtrRef GetRecCopy() const {return getRecCopy(GetExpr());}
  //operator IRExpr*() {return GetExpr();}
private:
  IRExprPtrRef(IRExpr* expr_) : IRExprRef(expr_) {}
  friend class IRExprPtr;
}; // class IRExprPtrRef

class IRExprRealRef : public IRExprRef {
public:
  IRExprRealRef& operator = (const IRExprRealRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprRealRef() : IRExprRef(&Invalid<IRExpr>::Obj()) {}
  IRExprRealRef GetRecCopy() const {return getRecCopy(GetExpr());}
  //operator IRExpr*() {return GetExpr();}
private:
  IRExprRealRef(IRExpr* expr_) : IRExprRef(expr_) {}
  friend class IRExprReal;
}; // class IRExprRealRef

class IRExprBoolRef : public IRExprRef {
public:
  IRExprBoolRef& operator = (const IRExprBoolRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprBoolRef() : IRExprRef(&Invalid<IRExpr>::Obj()) {}
  IRExprBoolRef GetRecCopy() const {return getRecCopy(GetExpr());}
  //operator IRExpr*() {return GetExpr();}
private:
  IRExprBoolRef(IRExpr* expr_) : IRExprRef(expr_) {}
  friend class IRExprBool;
}; // class IRExprBoolRef

class IRExprVoidRef : public IRExprRef {
public:
  IRExprVoidRef& operator = (const IRExprVoidRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprVoidRef() : IRExprRef(&Invalid<IRExpr>::Obj()) {}
  IRExprVoidRef GetRecCopy() const {return getRecCopy(GetExpr());}
  //operator IRExpr*() {return GetExpr();}
private:
  IRExprVoidRef(IRExpr* expr_) : IRExprRef(expr_) {}
  friend class IRExprVoid;
}; // class IRExprVoidRef

class IRExprAnyRef : public IRExprRef {
public:
  IRExprAnyRef() : IRExprRef(&Invalid<IRExpr>::Obj()) {}
  // Returns a reference to recursively copied expression tree.
  //operator IRExpr*() {return GetExpr();}
  IRExprAnyRef GetRecCopy() const {return IRExprAnyRef(getRecCopy(GetExpr()));}
  IRExprAnyRef(const IRExprIntRef& ref_) : IRExprRef(ref_.GetExpr()) {}
  IRExprAnyRef(const IRExprPtrRef& ref_) : IRExprRef(ref_.GetExpr()) {}
  IRExprAnyRef(const IRExprBoolRef& ref_) : IRExprRef(ref_.GetExpr()) {}
  IRExprAnyRef(const IRExprRealRef& ref_) : IRExprRef(ref_.GetExpr()) {}
  IRExprAnyRef(const IRExprAnyRef& ref_) : IRExprRef(ref_.GetExpr()) {}
  IRExprAnyRef(const IRExprVoidRef& ref_) : IRExprRef(ref_.GetExpr()) {}
  IRExprAnyRef& operator = (const IRExprVoidRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprAnyRef& operator = (const IRExprIntRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprAnyRef& operator = (const IRExprAnyRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprAnyRef& operator = (const IRExprBoolRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprAnyRef& operator = (const IRExprRealRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
  IRExprAnyRef& operator = (const IRExprPtrRef& ref_) {setExpr(ref_.GetExpr()); return *this;}
private:
  IRExprAnyRef(IRExpr* expr_) : IRExprRef(expr_) {}
  friend class IRExprAny;
}; // class IRExprAnyRef

class IRExprVoid : public IRExprBuilder {
public:
  IRExprVoid() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  IRExprVoid GetRecCopy() const {return IRExprVoid(getRecCopy(GetExpr()));}
  IRExprVoid(const IRExprVoidRef& voidRefExpr_) : IRExprBuilder(voidRefExpr_) {}
  IRExprVoidRef Ref() const {return IRExprVoidRef(GetExpr());}
  IRExprVoid(IRExpr* expr_);
}; // class IRExprVoid

class IRExprAny : public IRExprBuilder {
private:
  //! \brief Assignment operator is prohibited.
  void operator = (const IRExprAny&) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
public:
  IRExprAny() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  IRExprAny GetRecCopy() const {return IRExprAny(getRecCopy(GetExpr()));}
  IRExprAny(const IRExprAnyRef& anyRefExpr_) : IRExprBuilder(anyRefExpr_) {}
  IRExprAnyRef Ref() const {return IRExprAnyRef(GetExpr());}
  //! \todo should below constructors be made conversion operators?, we did
  //!       not use explicit. It is not clear if they construct an expression or not?
  IRExprAny(IRExprInt& intExpr_);
  IRExprAny(IRExprVoid& voidExpr_);
  IRExprAny(IRExprPtr& ptrExpr_);
  IRExprAny(IRExprReal& realExpr_);
  IRExprAny(IRExprBool& boolExpr_);
  // Constructors from IRExpr's
  IRExprAny(IRExpr* expr_) : IRExprBuilder(expr_) {}
  IRExprAny(IRESubscript* subsExpr_);
  IRExprAny(IREMember* memExpr_);
  IRExprAny(IRENull* nulExpr_);
  IRExprAny(IREDeref* drfExpr_);
  IRExprAny(IRECast* castExpr_);
  IRExprAny(IREStrConst* strConstExpr_);
  IRExprAny(IREAddrConst* addrConstExpr_);
  IRExprAny(IRObject* objExpr_);
  IRExprAny(IRExprList* exprList_);
  //operator IRExpr*() {return GetExpr();}
}; // class IRExprAny

class IRExprBool : public IRExprBuilder {
private:
  //! \brief Assignment operator is prohibited.
  void operator = (const IRExprBool&) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
public:
  IRExprBool() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  IRExprBool GetRecCopy() const {return IRExprBool(getRecCopy(GetExpr()));}
  explicit IRExprBool(const IRExprAny& anyExpr_) : IRExprBuilder(anyExpr_) {}
  explicit IRExprBool(const IRExprAnyRef& anyRefExpr_) : IRExprBuilder(anyRefExpr_) {}
  IRExprBoolRef Ref() const {return IRExprBoolRef(GetExpr());}
  // provide implicit conversions
  IRExprBool(IRExpr* expr_) : IRExprBuilder(expr_) {
    REQUIREDMSG(pred.pIsBool(expr_->GetType()), ieStrParamValuesDBCViolation);
  } // IRExprBool::IRExprBool
  IRExprBool(IREBoolConst* boolExpr_);
  IRExprBool(IREEq* boolExpr_);
  IRExprBool(IREGe* boolExpr_);
  IRExprBool(IREGt* boolExpr_);
  IRExprBool(IRELe* boolExpr_);
  IRExprBool(IRELt* boolExpr_);
  IRExprBool(IRENe* boolExpr_);
  IRExprBool(IRENot* notExpr_);
  IRExprBool(IRELOr* lorExpr_);
  IRExprBool(IRELAnd* landExpr_);
  IRExprBool(IREDeref* drfExpr_);
  //operator IRExpr*() {return GetExpr();}
public:
  typedef IRExprBoolRef RefType;
  typedef IRExprBoolPtr PtrType;
}; // class IRExprBool

class IRExprReal : public IRExprBuilder {
private:
  //! \brief Assignment operator is prohibited.
  void operator = (const IRExprReal&) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
public:
  IRExprReal() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  explicit IRExprReal(const IRExprAny& anyExpr_) : IRExprBuilder(anyExpr_) {}
  explicit IRExprReal(const IRExprAnyRef& anyRefExpr_) : IRExprBuilder(anyRefExpr_) {}
  IRExprReal GetRecCopy() const {return IRExprReal(getRecCopy(GetExpr()));}
  IRExprRealRef Ref() const {return IRExprRealRef(GetExpr());}
  IRExprReal(IRExpr* expr_) : IRExprBuilder(expr_) {
    REQUIREDMSG(pred.pIsReal(expr_->GetType()) == true, ieStrParamValuesDBCViolation);
  } // IRExprReal::IRExprReal
  IRExprReal(IRERealConst* realcExpr_);
  IRExprReal(IREANeg* angExpr_);
  IRExprReal(IREMul* mulExpr_);
  IRExprReal(IREDiv* divExpr_);
  IRExprReal(IREAdd* addExpr_);
  IRExprReal(IRESub* subExpr_);
  IRExprReal(IREDeref* drfExpr_);
  //operator IRExpr*() {return GetExpr();}
public:
  typedef IRExprRealRef RefType;
  typedef IRExprRealPtr PtrType;
}; // class IRExprReal

class IRExprPtr : public IRExprBuilder {
  //! \brief Assignment operator is prohibited.
  void operator = (const IRExprPtr&) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
public:
  IRExprPtr() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  explicit IRExprPtr(const IRExprAny& anyExpr_) : IRExprBuilder(anyExpr_) {}
  explicit IRExprPtr(const IRExprAnyRef& anyRefExpr_) : IRExprBuilder(anyRefExpr_) {}
  IRExprPtr GetRecCopy() const {return IRExprPtr(getRecCopy(GetExpr()));}
  IRExprPtrRef Ref() const {return IRExprPtrRef(GetExpr());}
  IRExprPtr(IRExpr* expr_) : IRExprBuilder(expr_) {
    REQUIREDMSG(pred.pIsPtrOrArray(expr_->GetType()) == true, ieStrParamValuesDBCViolation);
  } // IRExprPtr::IRExprPtr
  IRExprPtr(IREPtrAdd* ptrAddExpr_);
  IRExprPtr(IREAddrOf* addrOfExpr_);
  IRExprPtr(IREAddrConst* addrConst_);
  IRExprPtr(IREStrConst* strConst_);
  //! \todo M Design: do we really need IRExpr conversion operator in all
  //!       Ptr,Int,Real,Bool?
  //operator IRExpr*() {return GetExpr();}
  IRExprAny operator [] (const IRExprInt& index_);
public:
  typedef IRExprPtrRef RefType;
  typedef IRExprPtr PtrType;
}; // class IRExprPtr

class IRExprIntPtr : public IRExprBuilder {
public:
  IRExprIntPtr() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  explicit IRExprIntPtr(const IRExprAnyRef& ref_) : IRExprBuilder(ref_) {}
  explicit IRExprIntPtr(IRExpr* expr_) : IRExprBuilder(expr_) {}
  IRExprIntPtr GetRecCopy() const {return IRExprIntPtr(getRecCopy(GetExpr()));}
}; // class IRExprIntPtr

class IRExprBoolPtr : public IRExprBuilder {
public:
  IRExprBoolPtr() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  explicit IRExprBoolPtr(const IRExprAnyRef& ref_) : IRExprBuilder(ref_) {}
  explicit IRExprBoolPtr(IRExpr* expr_) : IRExprBuilder(expr_) {}
  IRExprBoolPtr GetRecCopy() const {return IRExprBoolPtr(getRecCopy(GetExpr()));}
}; // class IRExprBoolPtr

class IRExprRealPtr : public IRExprBuilder {
public:
  IRExprRealPtr() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  explicit IRExprRealPtr(const IRExprAnyRef& ref_) : IRExprBuilder(ref_) {}
  explicit IRExprRealPtr(IRExpr* expr_) : IRExprBuilder(expr_) {}
  IRExprRealPtr GetRecCopy() const {return IRExprRealPtr(getRecCopy(GetExpr()));}
}; // class IRExprRealPtr

class IRExprObj {
public:
  IRExprObj(IRObject* obj_) : mObj(obj_) {}
  operator IRObject* () const {return mObj;}
  IRExprAny operator [](const IRExprInt& index_);
private:
  IRObject* mObj;
}; // class IRExprObj

class IRExprInt : public IRExprBuilder {
  //! \brief Assignment operator is prohibited.
  void operator = (const IRExprInt&) {ASSERTMSG(0, ieStrFuncShouldNotBeCalled);}
public:
  IRExprInt() : IRExprBuilder(&Invalid<IRExpr>::Obj()) {}
  explicit IRExprInt(const IRExprAny& anyExpr_) : IRExprBuilder(anyExpr_) {}
  explicit IRExprInt(const IRExprAnyRef& anyRefExpr_) : IRExprBuilder(anyRefExpr_) {}
  IRExprInt GetRecCopy() const {return IRExprInt(getRecCopy(GetExpr()));}
  IRExprIntRef Ref() const {return IRExprIntRef(GetExpr());}
  IRExprInt(IRExpr* expr_) : IRExprBuilder(expr_) {
    REQUIREDMSG(pred.pIsInt(expr_->GetType()) == true, ieStrParamValuesDBCViolation);
  } // IRExprInt::IRExprInt
  IRExprInt(IREAdd* addExpr_);
  IRExprInt(IRESub* subExpr_);
  IRExprInt(IREAShiftRight* sftrExpr_);
  IRExprInt(IREBAnd* andExpr_);
  IRExprInt(IREBOr* orExpr_);
  IRExprInt(IREBXOr* xorExpr_);
  IRExprInt(IREDiv* divExpr_);
  IRExprInt(IRELShiftRight* sftrExpr_);
  IRExprInt(IREMod* modExpr_);
  IRExprInt(IREPtrSub* ptrSubExpr_);
  IRExprInt(IREMul* mulExpr_);
  IRExprInt(IREQuo* quoExpr_);
  IRExprInt(IRERem* remExpr_);
  IRExprInt(IRENot* notExpr_);
  IRExprInt(IREShiftLeft* sftlftExpr_);
  IRExprInt(IREIntConst* intcExpr_);
  IRExprInt(IREBitWrite* bitwExpr_);
  IRExprInt(IREANeg* angExpr_);
  IRExprInt(IREBNeg* bngExpr_);
  IRExprInt(IREDeref* drfExpr_);
  //operator IRExpr*() {return GetExpr();}
public:
  typedef IRExprIntRef RefType;
  typedef IRExprIntPtr PtrType;
}; // class IRExprInt

//! \attention Coding style: You should try to localize the use of
//!            IRExprBuilderOperators, you can use local block scope.
namespace IRExprBuilderOperators {
  IRExprReal operator *(const IRExprRealPtr& op_);
  IRExprBool operator *(const IRExprBoolPtr& op_);
  IRExprInt operator *(const IRExprIntPtr& op_);
  IRExprAny operator *(const IRExprPtr& op_);
  // addrof operators
  //! \attention We do not want developers to obtain and use the
  //! address of expression trees. Therefore, we overload the address of
  //! operator. The new meaning of operator is to create an IREAddrOf
  //! expression. Note that this is safe since we do not return a pointer
  //! type compiler will give an error message in case user does not know
  //! the overloading of this operator. If you want to pass the expression
  //! pointers use Ref() function to obtain the reference of an expression
  //! tree.
  IRExprPtr operator &(const IRExprAny& op_);
  IRExprPtr operator &(const IRExprBool& op_);
  IRExprPtr operator &(const IRExprInt& op_);
  IRExprPtr operator &(const IRExprPtr& op_);
  IRExprPtr operator &(const IRExprObj& exprObj_);
  IRExprBool operator !=(const IRExprPtr& left_, const IRExprPtr& right_);
  IRExprBool operator ==(const IRExprPtr& left_, const IRExprPtr& right_);
  IRExprPtr operator +(const IRExprPtr& left_, const IRExprInt& right_);
  IRExprPtr operator -(const IRExprPtr& left_, const IRExprInt& right_);
  IRExprInt operator -(const IRExprPtr& left_, const IRExprPtr& right_);
  IRExprReal operator -(const IRExprReal& op_);
  IRExprBool operator >(const IRExprPtr& left_, const IRExprPtr& right_);
  IRExprBool operator <(const IRExprPtr& left_, const IRExprPtr& right_);
  IRExprBool operator <=(const IRExprPtr& left_, const IRExprPtr& right_);
  IRExprBool operator >=(const IRExprPtr& left_, const IRExprPtr& right_);
  IRExprBool operator >(const IRExprReal& left_, const IRExprReal& right_);
  IRExprBool operator <(const IRExprReal& left_, const IRExprReal& right_);
  IRExprBool operator <=(const IRExprReal& left_, const IRExprReal& right_);
  IRExprBool operator >=(const IRExprReal& left_, const IRExprReal& right_);
  IRExprBool operator !=(const IRExprReal& left_, const IRExprReal& right_);
  IRExprBool operator ==(const IRExprReal& left_, const IRExprReal& right_);
  IRExprReal operator +(const IRExprReal& left_, const IRExprReal& right_);
  IRExprReal operator -(const IRExprReal& left_, const IRExprReal& right_);
  IRExprReal operator *(const IRExprReal& left_, const IRExprReal& right_);
  IRExprReal operator /(const IRExprReal& left_, const IRExprReal& right_);
  IRExprBool operator ||(const IRExprBool& left_, const IRExprBool& right_);
  IRExprBool operator &&(const IRExprBool& left_, const IRExprBool& right_);
  IRExprBool operator !(const IRExprBool& op_);
  IRExprInt operator -(const IRExprInt& op_);
  IRExprInt operator ~(const IRExprInt& op_);
  IRExprBool operator >(const IRExprInt& left_, const IRExprInt& right_);
  IRExprBool operator <(const IRExprInt& left_, const IRExprInt& right_);
  IRExprBool operator <=(const IRExprInt& left_, const IRExprInt& right_);
  IRExprBool operator >=(const IRExprInt& left_, const IRExprInt& right_);
  IRExprBool operator !=(const IRExprInt& left_, const IRExprInt& right_);
  IRExprBool operator ==(const IRExprInt& left_, const IRExprInt& right_);
  //! \todo >> is arithmetic shift, can we make IRExprUInt type? for logical
  //!       shift? or interpret the actual type of IRExprInt at compile-time
  //!       and do it there?
  IRExprInt operator >>(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator <<(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator +(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator -(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator /(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator *(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator %(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator |(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator &(const IRExprInt& left_, const IRExprInt& right_);
  IRExprInt operator ^(const IRExprInt& left_, const IRExprInt& right_);
} // namespace IRExprBuilderOperators

#endif // __IREXPRBUILDER_HPP__

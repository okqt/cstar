// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IREXPRBUILDER_HPP__
#include "irexprbuilder.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif

// /////////////////////////////
// Operator overloading
// /////////////////////////////
namespace IRExprBuilderOperators {
  IRExprBool operator || (const IRExprBool& left_, const IRExprBool& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLOr(left_.GetExpr(), right_.GetExpr()));
  } // operator ||
  IRExprBool operator && (const IRExprBool& left_, const IRExprBool& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLAnd(left_.GetExpr(), right_.GetExpr()));
  } // operator &&
  IRExprBool operator !(const IRExprBool& op) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeNot(op.GetExpr()));
  } // operator !
  IRExprReal operator -(const IRExprReal& op) {
    return IRExprReal(Singleton<IRBuilder>::Obj().irbeANeg(op.GetExpr()));
  } // operator -
  IRExprBool operator > (const IRExprPtr& left_, const IRExprPtr& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeGt(left_.GetExpr(), right_.GetExpr()));
  } // operator >
  IRExprBool operator < (const IRExprPtr& left_, const IRExprPtr& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLt(left_.GetExpr(), right_.GetExpr()));
  } // operator <
  IRExprBool operator <= (const IRExprPtr& left_, const IRExprPtr& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLe(left_.GetExpr(), right_.GetExpr()));
  } // operator <=
  IRExprBool operator >= (const IRExprPtr& left_, const IRExprPtr& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeGe(left_.GetExpr(), right_.GetExpr()));
  } // operator >=
  IRExprBool operator > (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeGt(left_.GetExpr(), right_.GetExpr()));
  } // operator >
  IRExprBool operator < (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLt(left_.GetExpr(), right_.GetExpr()));
  } // operator <
  IRExprBool operator <= (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLe(left_.GetExpr(), right_.GetExpr()));
  } // operator <=
  IRExprBool operator >= (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeGe(left_.GetExpr(), right_.GetExpr()));
  } // operator >=
  IRExprBool operator != (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeNe(left_.GetExpr(), right_.GetExpr()));
  } // operator !=
  IRExprBool operator == (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeEq(left_.GetExpr(), right_.GetExpr()));
  } // operator ==
  IRExprReal operator + (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprReal(Singleton<IRBuilder>::Obj().irbeAdd(left_.GetExpr(), right_.GetExpr()));
  } // operator +
  IRExprReal operator - (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprReal(Singleton<IRBuilder>::Obj().irbeSub(left_.GetExpr(), right_.GetExpr()));
  } // operator -
  IRExprReal operator * (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprReal(Singleton<IRBuilder>::Obj().irbeMul(left_.GetExpr(), right_.GetExpr()));
  } // operator *
  IRExprReal operator / (const IRExprReal& left_, const IRExprReal& right_) {
    return IRExprReal(Singleton<IRBuilder>::Obj().irbeDiv(left_.GetExpr(), right_.GetExpr()));
  } // operator /
  IRExprAny operator *(const IRExprPtr& op) {
    return IRExprAny(Singleton<IRBuilder>::Obj().irbeDeref(op.GetExpr()));
  } // operator *
  IRExprInt operator *(const IRExprIntPtr& op) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeDeref(op.GetExpr()));
  } // operator *
  IRExprReal operator *(const IRExprRealPtr& op) {
    return IRExprReal(Singleton<IRBuilder>::Obj().irbeDeref(op.GetExpr()));
  } // operator *
  IRExprBool operator *(const IRExprBoolPtr& op) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeDeref(op.GetExpr()));
  } // operator *
  IRExprBool operator != (const IRExprPtr& left_, const IRExprPtr& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeNe(left_.GetExpr(), right_.GetExpr()));
  } // operator !=
  IRExprBool operator == (const IRExprPtr& left_, const IRExprPtr& right_){
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeEq(left_.GetExpr(), right_.GetExpr()));
  } // operator ==
  IRExprPtr operator + (const IRExprPtr& left_, const IRExprInt& right_){
    return IRExprPtr(Singleton<IRBuilder>::Obj().irbePtrAdd(left_.GetExpr(), right_.GetExpr()));
  } // operator +
  IRExprPtr operator - (const IRExprPtr& left_, const IRExprInt& right_){
    return left_ + (-right_);
  } // operator
  IRExprInt operator - (const IRExprPtr& left_, const IRExprPtr& right_){
    return IRExprInt(Singleton<IRBuilder>::Obj().irbePtrSub(left_.GetExpr(), right_.GetExpr()));
  } // operator -
  IRExprPtr operator &(const IRExprAny& op) {
    return IRExprPtr(Singleton<IRBuilder>::Obj().irbeAddrOf(op.GetExpr()));
  } // operator &
  // //////////////////////
  // integer operators
  // //////////////////////

  IRExprInt operator -(const IRExprInt& op) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeANeg(op.GetExpr()));
  } // operator -
  IRExprInt operator ~(const IRExprInt& op) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeBNeg(op.GetExpr()));
  } // operator ~
  IRExprBool operator > (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeGt(left_.GetExpr(), right_.GetExpr()));
  } // operator >
  IRExprBool operator < (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLt(left_.GetExpr(), right_.GetExpr()));
  } // operator <
  IRExprBool operator <= (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeLe(left_.GetExpr(), right_.GetExpr()));
  } // operator <=
  IRExprBool operator >= (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeGe(left_.GetExpr(), right_.GetExpr()));
  } // operator >=
  IRExprBool operator != (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeNe(left_.GetExpr(), right_.GetExpr()));
  } // operator !=
  IRExprBool operator == (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprBool(Singleton<IRBuilder>::Obj().irbeEq(left_.GetExpr(), right_.GetExpr()));
  } // operator ==
  IRExprInt operator >> (const IRExprInt& left_, const IRExprInt& right_) {
    IRExprIntRef retVal;
    if (!pred.pIsUnsignedInt(left_.GetExpr()->GetType())) {
      retVal = IRExprInt(Singleton<IRBuilder>::Obj().irbeAShiftRight(left_.GetExpr(), right_.GetExpr())).Ref();
    } else {
      retVal = IRExprInt(Singleton<IRBuilder>::Obj().irbeLShiftRight(left_.GetExpr(), right_.GetExpr())).Ref();
    } // if
    return IRExprInt(retVal);
  } // operator >>
  IRExprInt operator << (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeShiftLeft(left_.GetExpr(), right_.GetExpr()));
  } // operator <<
  IRExprInt operator + (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeAdd(left_.GetExpr(), right_.GetExpr()));
  } // operator +
  IRExprInt operator - (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeSub(left_.GetExpr(), right_.GetExpr()));
  } // operator -
  IRExprInt operator / (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeDiv(left_.GetExpr(), right_.GetExpr()));
  } // operator /
  IRExprInt operator * (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeMul(left_.GetExpr(), right_.GetExpr()));
  } // operator *
  IRExprInt operator % (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeMod(left_.GetExpr(), right_.GetExpr()));
  } // operator %
  IRExprInt operator | (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeBOr(left_.GetExpr(), right_.GetExpr()));
  } // operator |
  IRExprInt operator & (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeBAnd(left_.GetExpr(), right_.GetExpr()));
  } // operator &
  IRExprInt operator ^ (const IRExprInt& left_, const IRExprInt& right_) {
    return IRExprInt(Singleton<IRBuilder>::Obj().irbeBXOr(left_.GetExpr(), right_.GetExpr()));
  } // operator ^
  IRExprPtr operator &(const IRExprObj& exprObj_) {
    return IRExprPtr(Singleton<IRBuilder>::Obj().irbeAddrOf(exprObj_));
  } // operator &
} // namespace IRExprBuilderOperators

IRExprAny IRExprObj::
operator[](const IRExprInt& index_) {
  return IRExprAny(Singleton<IRBuilder>::Obj().irbeSubscript(mObj, index_.GetExpr()));
} // IRExprObj::operator []

IRExprAny IRExprPtr::
operator [] (const IRExprInt& index_) {
  return IRExprAny(Singleton<IRBuilder>::Obj().irbeSubscript(GetExpr(), index_.GetExpr()));
} // IRExprPtr::operator []

IRExprAny::IRExprAny(IRExprVoid& voidExpr_) : IRExprBuilder(voidExpr_) {}
IRExprAny::IRExprAny(IRExprInt& intExpr_) : IRExprBuilder(intExpr_) {}
IRExprAny::IRExprAny(IRExprPtr& ptrExpr_) : IRExprBuilder(ptrExpr_) {}
IRExprAny::IRExprAny(IRExprReal& realExpr_) : IRExprBuilder(realExpr_) {}
IRExprAny::IRExprAny(IRExprBool& boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprAny::IRExprAny(IREStrConst* strConstExpr_) : IRExprBuilder(strConstExpr_) {}
IRExprAny::IRExprAny(IREAddrConst* addrConstExpr_) : IRExprBuilder(addrConstExpr_) {}
IRExprAny::IRExprAny(IRESubscript* subsExpr_) : IRExprBuilder(subsExpr_) {}
IRExprAny::IRExprAny(IREMember* memExpr_) : IRExprBuilder(memExpr_) {}
IRExprAny::IRExprAny(IRENull* nullExpr_) : IRExprBuilder(nullExpr_) {}
IRExprAny::IRExprAny(IRObject* objExpr_) : IRExprBuilder(objExpr_) {}
IRExprAny::IRExprAny(IREDeref* derefExpr_) : IRExprBuilder(derefExpr_) {}
IRExprAny::IRExprAny(IRECast* castExpr_) : IRExprBuilder(castExpr_) {}
IRExprAny::IRExprAny(IRExprList* exprList_) : IRExprBuilder(exprList_) {}
IRExprInt::IRExprInt(IREDeref* derefExpr_) : IRExprBuilder(derefExpr_) {}
IRExprReal::IRExprReal(IREDeref* derefExpr_) : IRExprBuilder(derefExpr_) {}
IRExprBool::IRExprBool(IREDeref* derefExpr_) : IRExprBuilder(derefExpr_) {}
IRExprVoid::IRExprVoid(IRExpr* expr_) : IRExprBuilder(expr_) {}

IRExprBool::IRExprBool(IREBoolConst* boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprBool::IRExprBool(IREEq* boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprBool::IRExprBool(IREGe* boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprBool::IRExprBool(IREGt* boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprBool::IRExprBool(IRELe* boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprBool::IRExprBool(IRELt* boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprBool::IRExprBool(IRENe* boolExpr_) : IRExprBuilder(boolExpr_) {}
IRExprBool::IRExprBool(IRENot* notExpr_) : IRExprBuilder(notExpr_) {}
IRExprBool::IRExprBool(IRELOr* lorExpr_) : IRExprBuilder(lorExpr_) {}
IRExprBool::IRExprBool(IRELAnd* landExpr_) : IRExprBuilder(landExpr_) {}

IRExprReal::IRExprReal(IRERealConst* realcExpr_) : IRExprBuilder(realcExpr_) {}
IRExprReal::IRExprReal(IREANeg* aNegExpr_) : IRExprBuilder(aNegExpr_) {}
IRExprReal::IRExprReal(IREMul* mulExpr_) : IRExprBuilder(mulExpr_) {}
IRExprReal::IRExprReal(IREDiv* divExpr_) : IRExprBuilder(divExpr_) {}
IRExprReal::IRExprReal(IREAdd* addExpr_) : IRExprBuilder(addExpr_) {}
IRExprReal::IRExprReal(IRESub* subExpr_) : IRExprBuilder(subExpr_) {}

IRExprPtr::IRExprPtr(IREPtrAdd* ptrAddExpr_) : IRExprBuilder(ptrAddExpr_) {}
IRExprPtr::IRExprPtr(IREAddrOf* addrOfExpr_) : IRExprBuilder(addrOfExpr_) {}
IRExprPtr::IRExprPtr(IREStrConst* strExpr_) : IRExprBuilder(strExpr_) {}
IRExprPtr::IRExprPtr(IREAddrConst* addrConst_) : IRExprBuilder(addrConst_) {}

IRExprInt::IRExprInt(IREAdd* addExpr_) : IRExprBuilder(addExpr_) {}
IRExprInt::IRExprInt(IRESub* addExpr_) : IRExprBuilder(addExpr_) {}
IRExprInt::IRExprInt(IREAShiftRight* shiftRightExpr_) : IRExprBuilder(shiftRightExpr_) {}
IRExprInt::IRExprInt(IREBAnd* andExpr_) : IRExprBuilder(andExpr_) {}
IRExprInt::IRExprInt(IREBOr* orExpr_) : IRExprBuilder(orExpr_) {}
IRExprInt::IRExprInt(IREBXOr* xorExpr_) : IRExprBuilder(xorExpr_) {}
IRExprInt::IRExprInt(IREDiv* divExpr_) : IRExprBuilder(divExpr_) {}
IRExprInt::IRExprInt(IRELShiftRight* shiftRightExpr_) : IRExprBuilder(shiftRightExpr_) {}
IRExprInt::IRExprInt(IREMod* modExpr_) : IRExprBuilder(modExpr_) {}
IRExprInt::IRExprInt(IREMul* mulExpr_) : IRExprBuilder(mulExpr_) {}
IRExprInt::IRExprInt(IREQuo* quoExpr_) : IRExprBuilder(quoExpr_) {}
IRExprInt::IRExprInt(IRERem* remExpr_) : IRExprBuilder(remExpr_) {}
IRExprInt::IRExprInt(IREPtrSub* ptrSubExpr_) : IRExprBuilder(ptrSubExpr_) {}
IRExprInt::IRExprInt(IRENot* notExpr_) : IRExprBuilder(notExpr_) {}
IRExprInt::IRExprInt(IREShiftLeft* shiftLeftExpr_) : IRExprBuilder(shiftLeftExpr_) {}
IRExprInt::IRExprInt(IREIntConst* intcExpr_) : IRExprBuilder(intcExpr_) {}
IRExprInt::IRExprInt(IREANeg* angExpr_) : IRExprBuilder(angExpr_) {}
IRExprInt::IRExprInt(IREBNeg* bngExpr_) : IRExprBuilder(bngExpr_) {}

// ---------------------------
// Expressions' recursive copy
// ---------------------------
IRExpr* IRExprBuilder::
getRecCopy(IRExpr* exprTree_) {
  return exprTree_->GetRecCopyExpr();
} // IRExprBuilder::getRecCopy



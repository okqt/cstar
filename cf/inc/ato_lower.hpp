// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_LOWER_HPP__
#define __ATO_LOWER_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_lower.hpp
//! \brief Contains the HIR lowering transformations for IREMember, IRESubscript, IRSSwitch.

class ATOLowerHIR : public Anatrop {
private:
  ATOLowerHIR() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOLowerHIR(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOLowerHIR() {}
public:
  virtual const char* GetName() const {return "HIRLower";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return true;}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOLowerHIR(scope_);}
private:
  virtual hFInt32 AnatropDo(IRModule* module_);
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
private:
  virtual hFInt32 InsertCasts(IRECast* cast_);
  virtual hFInt32 LowerSwitch(IRSSwitch* switch_);
  virtual hFInt32 LowerMember(IREMember* member_);
  virtual hFInt32 LowerSubscript(IRESubscript* subscript_);
  virtual hFInt32 LowerLOr(IRELOr* lor_);
  virtual hFInt32 LowerLAnd(IRELAnd* land_);
  virtual hFInt32 LowerPtrSub(IREPtrSub* ptrSub_);
  virtual hFInt32 LowerCmp(IRECmp* cmp_);
  virtual hFInt32 LowerNot(IRENot* not_);
  virtual hFInt32 LowerExprToCall(IRExpr* expr_, IRFunctionDecl* func_, bool convRetVal_);
  //! \brief Lowers real typed constants in to global variables.
  virtual hFInt32 LowerRealConsts(IRERealConst* realConst_);
  //! \brief Lowers assignment of structures to builtin memcpy calls.
  virtual hFInt32 LowerStrAssign(IRSAssign* assign_);
private:
  friend class ExprLowerer;
  template<typename T> friend class Singleton;
}; // class ATOLowerHIR

#endif


// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_INDUCTIONVARS_HPP__
#define __ATO_INDUCTIONVARS_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_inductionvars.hpp
//! \brief Contains the induction variables analysis.

class ATOInductionVars : public Anatrop {
private:
  ATOInductionVars() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOInductionVars(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOInductionVars() {}
  virtual const char* GetName() const {return "InductionVars";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal() == true || scope_.IsLoop() == true;}
private:
  virtual hFInt32 AnatropDo(IRLoop* loop_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOInductionVars(scope_);}
private: // Member functions
  //! \brief Returns a candidate induction variable or null.
  //! \return If assignment does not define an induction variable it returns null.
  //! It returns an induction variable for the following assignment patterns:
  //! - Single definition in the loop
  //! - i = i +/- const -> basic induction var case
  //! - k = m * j +/- c
  //!   - m = 1 => k = j +/- c
  //!   - c = 0 => k = m * j
  //!   - m = 1 & c = 0 => k = j. // basic induction variable candidate when j is also biv.
  IRInductionVar* GetCandidInductionVar(IRLoop* loop_, IRSAssign* assign_);
  //! \brief Returns loop invariant and object use operand of an expression.
  //! \returns Returns true if one operand is object use and the other is loop invariant.
  bool IsBinObjUseAndLoopInvariant(const IREBinary* expr_, IRObject*& objUse_, IRExpr*& loopInv_);
  bool IsLoopInvariant(IRExpr* expr_) const;
private:
  template<typename T> friend class Singleton;
}; // class ATOInductionVars

#endif




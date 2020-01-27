// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_IVSTRENGTHREDUCTION_HPP__
#define __ATO_IVSTRENGTHREDUCTION_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_ivstrengthreduction.hpp
//! \brief Contains the strength reduction of induction variables optimization.

class ATOIVStrengthReduction : public Anatrop {
private:
  ATOIVStrengthReduction() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOIVStrengthReduction(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOIVStrengthReduction() {}
public:
  virtual const char* GetName() const {return "IVStrengthReduction";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal() == true || scope_.IsLoop() == true;}
private:
  virtual hFInt32 AnatropDo(IRLoop* loop_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOIVStrengthReduction(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOIVStrengthReduction

#endif





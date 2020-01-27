// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_IVELIMINATION_HPP__
#define __ATO_IVELIMINATION_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_ivelimination.hpp
//! \brief Contains the elimination of induction variables optimization.

class ATOIVElimination : public Anatrop {
private:
  ATOIVElimination() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOIVElimination(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOIVElimination() {}
public:
  virtual const char* GetName() const {return "IVElimination";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal() == true || scope_.IsLoop() == true;}
private:
  virtual hFInt32 AnatropDo(IRLoop* loop_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOIVElimination(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOIVElimination

#endif






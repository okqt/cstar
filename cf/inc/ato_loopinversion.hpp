// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_LOOPINVERSION_HPP__
#define __ATO_LOOPINVERSION_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_loopinversion.hpp
//! \brief Contains the inversion of loops optimization.

class ATOLoopInversion : public Anatrop {
private:
  ATOLoopInversion() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOLoopInversion(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOLoopInversion() {}
public:
  virtual const char* GetName() const {return "LoopInversion";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsLoop() == true || scope_.IsGlobal() == true;}
private:
  virtual hFInt32 AnatropDo(IRLoop* loop_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOLoopInversion(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOLoopInversion

#endif





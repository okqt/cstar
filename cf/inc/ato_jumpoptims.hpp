// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_JUMPOPTIMS_HPP__
#define __ATO_JUMPOPTIMS_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_jumpoptims.hpp
//! \brief Contains the jump optimizations, e.g. jump into jumps.

class ATOJumpOptims : public Anatrop {
private:
  ATOJumpOptims() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOJumpOptims(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOJumpOptims() {}
public:
  virtual const char* GetName() const {return "JumpOptims";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return !scope_.IsStmt();}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOJumpOptims(scope_);}
private:
  virtual hFInt32 AnatropDo(IRBB* bb_);
private:
  template<typename T> friend class Singleton;
}; // class ATOJumpOptims

#endif




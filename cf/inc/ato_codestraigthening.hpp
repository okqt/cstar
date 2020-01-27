// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_CODESTRAIGTHENING_HPP__
#define __ATO_CODESTRAIGTHENING_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_codestraigthening.hpp
//! \brief Contains the code straigthening optimization.
//! It merges two basic blocks in to one if they are the only pred/succ of each other.

class ATOCodeStraigthening : public Anatrop {
private:
  ATOCodeStraigthening() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOCodeStraigthening(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOCodeStraigthening() {}
public:
  virtual const char* GetName() const {return "CodeStraigthening";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal();}
private:
  virtual hFInt32 AnatropDo(IRFunction* func_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOCodeStraigthening(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOCodeStraigthening

#endif



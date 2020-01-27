// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_REMOVENOPS_HPP__
#define __ATO_REMOVENOPS_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_removenops.hpp
//! \brief Contains the removal of no operation optimization.
//! IRSNull statements and "a = a;" like statements are removed in this optimization.

class ATORemoveNops : public Anatrop {
private:
  ATORemoveNops() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATORemoveNops(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATORemoveNops() {}
public:
  virtual const char* GetName() const {return "RemoveNops";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return !scope_.IsStmt();}
private:
  virtual hFInt32 AnatropDo(IRBB* bb_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATORemoveNops(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATORemoveNops

#endif




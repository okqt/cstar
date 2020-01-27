// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_GCOPYPROP_HPP__
#define __ATO_GCOPYPROP_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_gcopyprop.hpp
//! \brief Contains the global copy propagation optimization.

class ATOGCopyProp : public Anatrop {
private:
  ATOGCopyProp() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOGCopyProp(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOGCopyProp() {}
  virtual const char* GetName() const {return "GCopyProp";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal() == true;}
private:
  virtual hFInt32 AnatropDo(IRFunction* func_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOGCopyProp(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOGCopyProp

#endif



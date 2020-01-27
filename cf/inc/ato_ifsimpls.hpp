// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_IFSIMPLS_HPP__
#define __ATO_IFSIMPLS_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_ifsimpls.hpp
//! \brief Contains the if simplifications optimizations, e.g. const condition, empty then/else.

class ATOIfSimplifications : public Anatrop {
private:
  ATOIfSimplifications() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOIfSimplifications(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual const char* GetName() const {return "IfSimplifications";}
  virtual ~ATOIfSimplifications() {}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return true;}
private:
  virtual void InitOptions();
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOIfSimplifications(scope_);}
private:
  //! \brief Tries to merge ifs to form logical and/ors.
  hFUInt32 doConditionMerge(IRSIf* if_);
private:
  template<typename T> friend class Singleton;
}; // class ATOIfSimplifications

#endif



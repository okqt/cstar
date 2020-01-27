// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_CONSTFOLD_HPP__
#define __ATO_CONSTFOLD_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_constfold.hpp
//! \brief Contains the constant folding optimization.

class ATOConstFold : public Anatrop {
private:
  ATOConstFold() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOConstFold(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOConstFold() {}
  virtual const char* GetName() const {return "ConstFold";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return true;}
private:
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOConstFold(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOConstFold

#endif





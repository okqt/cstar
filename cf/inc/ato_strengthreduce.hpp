// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_STRENGTHREDUCE_HPP__
#define __ATO_STRENGTHREDUCE_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_strengthreduce.hpp
//! \brief Contains the strength reduction optimizations.

class ATOStrengthReduce : public Anatrop {
private:
  ATOStrengthReduce() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOStrengthReduce(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOStrengthReduce() {}
  virtual const char* GetName() const {return "StrengthReduction";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return true;}
private:
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOStrengthReduce(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOStrengthReduce

#endif


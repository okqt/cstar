// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_UNREACHABLECODE_HPP__
#define __ATO_UNREACHABLECODE_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_unreachablecode.hpp
//! \brief Contains the elimination of unreachable code optimization.

class ATOUnreachableCode : public Anatrop {
private:
  ATOUnreachableCode() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOUnreachableCode(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOUnreachableCode() {}
public:
  virtual const char* GetName() const {return "UnreachableCode";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return !scope_.IsStmt();}
private:
  virtual hFInt32 AnatropDo(IRBB* bb_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOUnreachableCode(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOUnreachableCode

#endif


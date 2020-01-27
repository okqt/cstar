// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_REMCASTS_HPP__
#define __ATO_REMCASTS_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_remcasts.hpp
//! \brief Contains the removal of redundant casts Anatrop.

class ATORemoveCasts : public Anatrop {
  ATORemoveCasts() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATORemoveCasts(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATORemoveCasts() {}
  virtual const char* GetName() const {return "RemoveCasts";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return true;}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATORemoveCasts(scope_);}
private:
  virtual hFInt32 AnatropDo(IRModule* module_);
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
private:
  hFUInt32 processCast(IRECast* castExpr_);
private:
  template<typename T> friend class Singleton;
}; // class ATORemoveCasts

#endif



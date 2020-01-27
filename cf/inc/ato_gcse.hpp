// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_GCSE_HPP__
#define __ATO_GCSE_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_gcse.hpp
//! \brief Contains the global common subexpression elimination optimization.

class ATOGCSE : public Anatrop {
private:
  ATOGCSE() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOGCSE(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOGCSE() {}
  virtual const char* GetName() const {return "GCSE";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return !scope_.IsLocal();}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOGCSE(scope_);}
private:
  virtual hFInt32 AnatropDo(IRFunction* func_);
private:
  //! \brief Finds the last occurance of expr_ in all the predecessors.
  //! \param [out] sources_
  void findSources(IRExpr* expr_, IRBB* bb_, vector<IRExpr*>& sources_);
  //! \brief Function returns true for expressions that can be considered for CSE.
  static bool shouldDoExpr(const IRExpr* expr_);
private:
  template<typename T> friend class Singleton;
}; // class ATOGCSE

#endif


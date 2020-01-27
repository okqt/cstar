// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_LCOPYPROP_HPP__
#define __ATO_LCOPYPROP_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_lcopyprop.hpp
//! \brief Contains the local copy propagation optimization.

//! \note What is copy propagation?
//! a = k;
//! b = a + c;
//! -> after copy prop, i.e. replacing the RHS of simple assigns.
//! b = k + c; // now a = k is redundant you should run dead copy/code elimination.
//! Note that RHS can be both variable and constant. When the RHS
//! is constant it does NOT make it a "constant propagation" optimization.
//! Constant propagation is a different optimization.
//! \todo M Design: Provide a statement scoped copy prop.
class ATOLCopyProp : public Anatrop {
private:
  ATOLCopyProp() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOLCopyProp(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOLCopyProp() {}
  virtual const char* GetName() const {return "LCopyProp";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return !scope_.IsStmt();}
private:
  virtual hFInt32 AnatropDo(IRBB* bb_);
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOLCopyProp(scope_);}
private:
  template<typename T> friend class Singleton;
}; // class ATOLCopyProp

#endif




// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_INLINER_HPP__
#define __ATO_INLINER_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_inliner.hpp
//! \brief Contains the inlining optimization.

class ATOInliner : public Anatrop {
private:
  ATOInliner() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  //! If scope is a function it sets the inlining property of that function.
  //! If scope is bigger than a function it determines the inlining property
  //! of all functions that are not yet determined and performs the inlining of
  //! the to be inlined function. 
  ATOInliner(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOInliner() {}
public:
  virtual const char* GetName() const {return "Inliner";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal();}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOInliner(scope_);}
private:
  virtual hFInt32 AnatropDo(IRModule* module_);
  virtual hFInt32 AnatropDo(IRFunction* func_);
  hFInt32 inlineCallsOf(IRFunction* func_);
  //! \brief inlines a given function at the given call statement.
  void inlineCall(IRSCall* call_, IRFunction* func_);
private:
  template<typename T> friend class Singleton;
}; // class ATOInliner

#endif





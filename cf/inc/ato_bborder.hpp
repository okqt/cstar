// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_BBORDER_HPP__
#define __ATO_BBORDER_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_bborder.hpp
//! \brief Contains the basic block reordering transformations.

//! \brief Orders the physical sequence of basic blocks.
class ATOBBOrder : public Anatrop {
private:
  ATOBBOrder() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOBBOrder(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOBBOrder() {}
  virtual const char* GetName() const {return "BBOrder";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal() == true;}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOBBOrder(scope_);}
private:
  virtual void InitOptions();
  virtual hFInt32 AnatropDo(IRFunction* func_);
private:
  //! \brief Orders the basic blocks in increasing source locations of contained statements.
  hFInt32 orderBySrcLoc(IRFunction* func_);
  hFSInt32 getMinLineNumber(const IRBB* bb_) const;
private:
  template<typename T> friend class Singleton;
}; // class ATOBBOrder

#endif


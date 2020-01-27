// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_CALLCONV_HPP__
#define __ATO_CALLCONV_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_callconv.hpp
//! \brief Contains the IR lowering transformation calling convention, ATOCallConv.

//! \todo H Design: Since we do not have a good options passing mechanism
//!       we are unable to say if we want the call expresions to be processed
//!       or function parameters to be processed. The scope of the anatrop
//!       gives an answer for IRBB and IRStmt but when the scope is program,
//!       for example, things are not clear. We also want to have a uniform
//!       anatrop interface to manage them nicely in the future, by anatrop
//!       manager.

//! \note CallingConvention::processParams is called once and first for each
//!       function, processCall and processReturn, if present in the function, will be
//!       called after processParams. processCall and processReturn may be
//!       called in any order, e.g. there may be very well a return statement before a
//!       call in a function.
class ATOCallConv : public Anatrop {
private:
  ATOCallConv() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOCallConv(const AnatropScope& scope_) : Anatrop(scope_) {}
  ATOCallConv(IRModule* module_) : Anatrop(module_) {}
  ATOCallConv(IRStmt* stmt_) : Anatrop(stmt_) {}
  virtual ~ATOCallConv() {}
  virtual const char* GetName() const {return "CallConv";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return true;}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOCallConv(scope_);}
private:
  virtual hFInt32 AnatropDo(IRModule* module_);
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
private:
  template<typename T> friend class Singleton;
}; // class ATOCallConv

#endif


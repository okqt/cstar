// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_GFORWARDSUBST_HPP__
#define __ATO_GFORWARDSUBST_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_gforwardsubst.hpp
//! \brief Contains the global forward substitution optimization.

//! \note What is forward substitution?
//! Forward substitution is the inverse of common subexpresion elimination,
//! ATOLCSE. It replaces RHS of a variables assignment with the uses of the variable.
//! \sa ATOLForwardSubstitution
//! \todo M Design: I think we can operate on globals too.
class ATOGForwardSubstitution : public Anatrop {
private:
  ATOGForwardSubstitution() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOGForwardSubstitution(const AnatropScope& scope_) : Anatrop(scope_), mDUUDChains(0) {}
  virtual ~ATOGForwardSubstitution() {}
public:
  virtual const char* GetName() const {return "GFwdSubst";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal();}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOGForwardSubstitution(scope_);}
private: // Member functions
  virtual hFInt32 AnatropDo(IRFunction* func_);
private: // Member data
  IRDUUDChains* mDUUDChains;
private:
  template<typename T> friend class Singleton;
}; // class ATOGForwardSubstitution

#endif



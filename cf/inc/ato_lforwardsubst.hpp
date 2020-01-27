// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_LFORWARDSUBST_HPP__
#define __ATO_LFORWARDSUBST_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_lforwardsubst.hpp
//! \brief Contains the local forward substitution optimization.

//! \note What is forward substitution?
//! Forward substitution is the inverse of common subexpresion elimination,
//! ATOLCSE. It replaces RHS of a variables assignment with the uses of the variable.
//! \note Local forward substitution is done on non-global, single definition,
//!       non-addr taken objects.
//! \sa ATOGForwardSubstitution
class ATOLForwardSubstitution : public Anatrop {
private:
  ATOLForwardSubstitution() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOLForwardSubstitution(const AnatropScope& scope_) : Anatrop(scope_), mDUUDChains(0) {}
  virtual ~ATOLForwardSubstitution() {}
public:
  virtual const char* GetName() const {return "LFwdSubst";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return true;}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOLForwardSubstitution(scope_);}
private: // Member functions
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
  //! \brief Initializes mDUUDChains.
  virtual void OnStart(IRStmt* stmt_);
private: // Member data
  IRDUUDChains* mDUUDChains;
private:
  template<typename T> friend class Singleton;
}; // class ATOLForwardSubstitution

#endif


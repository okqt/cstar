// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_LCSE_HPP__
#define __ATO_LCSE_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_lcse.hpp
//! \brief Contains the local common subexpression elimination optimization.

//! \note What is CSE.
//!   CSE literally checks the common sub expressions, this means:
//!     k = (a + b) + c;
//!     l = (c + a) + b;
//!   No common sub expressions will be found.
//!     k = c + (a + b);
//!     l = c + (a + b); 
//!   (a + b) will be recognized in the first pass:
//!     t = (a + b);
//!     k = c + t;
//!     l = c + t;
//!     (c + t) will be recognized in the second pass.
//!   So LCSE should be called repeatedly.
class ATOLCSE : public Anatrop {
private:
  ATOLCSE() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOLCSE(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOLCSE() {}
  virtual const char* GetName() const {return "LCSE";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return !scope_.IsStmt();}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOLCSE(scope_);}
private:
  virtual hFInt32 AnatropDo(IRBB* bb_);
private:
  virtual bool shouldDoExpr(const IRExpr* expr_) const;
private:
  template<typename T> friend class Singleton;
}; // class ATOLCSE

#endif



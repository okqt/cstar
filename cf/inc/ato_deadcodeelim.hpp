// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_DEADCODEELIM_HPP__
#define __ATO_DEADCODEELIM_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_deadcodeelim.hpp
//! \brief Contains the dead code elimination optimization.

//! \brief Dead code elimination optimization, dead code elimination removes both 
//!        dead objects and dead statements.
class ATODeadCodeElim : public Anatrop {
private:
  ATODeadCodeElim() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATODeadCodeElim(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATODeadCodeElim() {}
  virtual const char* GetName() const {return "DeadCodeElim";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal() == true;}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATODeadCodeElim(scope_);}
private: // Member functions
  virtual hFInt32 AnatropDo(IRFunction* func_);
private:
  void getEssentialStmts(IRFunction* func_, list<IRStmt*>& worklist_);
  void markStmts(list<IRStmt*>& worklist_);
  void removeStmts(IRFunction* func_);
private: // Member data
  IRDUUDChains* mDUUDChains;
private:
  template<typename T> friend class Singleton;
}; // class ATODeadCodeElim

#endif


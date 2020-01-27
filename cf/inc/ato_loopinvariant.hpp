// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ATO_LOOPINVARIANT_HPP__
#define __ATO_LOOPINVARIANT_HPP__

#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file ato_loopinvariant.hpp
//! \brief Contains the loop invariant code motion optimization.

//! \note Options:
//!   only_analyse[false]: When true IR is not changed but IR trees (expressions and statements) 
//!       that are loop invariants are marked with eIRTMLoopInvariant.
class ATOLoopInvariant : public Anatrop {
private:
  ATOLoopInvariant() : Anatrop(&Invalid<IRProgram>::Obj()) {}
public:
  ATOLoopInvariant(const AnatropScope& scope_) : Anatrop(scope_) {}
  virtual ~ATOLoopInvariant() {}
  virtual const char* GetName() const {return "LoopInvariant";}
  virtual bool IsValidScope(const AnatropScope& scope_) const {return scope_.IsGlobal() == true || scope_.IsLoop() == true;}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {return new ATOLoopInvariant(scope_);}
private: // Member functions
  //! \brief Initializes the options of this anatrop.
  virtual void InitOptions();
  //! \brief Function is called just after the options specified for this anatrop is set.
  virtual void OnOptionsSet();
  virtual hFInt32 AnatropDo(IRFunction* func_);
  virtual hFInt32 AnatropDo(IRLoop* loop_);
  //! \brief Marks the tree and its parents as loop invariant if appropriate.
  void markExprTree(IRExpr* expr_, IRLoop* loop_);
private: // Member data
  bool mOnlyAnalyse;
  map<hFUInt32, IRExpr*> mGrpNo2Expr;
  IRBuilder* mIRBuilder;
  IRBuilderHelper* mIRHBuilder;
  // Save the pointer to duud chains since we may invalidate CFGs duud chains.
  IRDUUDChains* mDUUDChains;
private:
  template<typename T> friend class Singleton;
}; // class ATOLoopInvariant

#endif



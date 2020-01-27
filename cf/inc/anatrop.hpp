// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ANATROP_HPP__
#define __ANATROP_HPP__

#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif
#ifndef __OPTIONS_HPP__
#include "options.hpp"
#endif
#ifndef __STATS_HPP__
#include "stats.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

//! \file anatrop.hpp
//! \brief Contains the base class for all anatrops, Anatrop.

class AnatropScope : public IDebuggable {
protected:
  AnatropScope() : mProg(0), mModule(0), mFunc(0), mLoop(0), mStmt(0), mBB(0) {}
public:
  AnatropScope(const AnatropScope& scope_) : 
    mProg(scope_.mProg),
    mBB(scope_.mBB),
    mStmt(scope_.mStmt),
    mFunc(scope_.mFunc),
    mLoop(scope_.mLoop),
    mModule(scope_.mModule)
  {
  } // AnatropScope::AnatropScope
  AnatropScope(IRProgram* prog_) : mProg(prog_), mModule(0), mFunc(0), mLoop(0), mStmt(0), mBB(0) {}
  AnatropScope(IRFunction* func_) : mProg(0), mModule(0), mFunc(func_), mLoop(0), mStmt(0), mBB(0) {}
  AnatropScope(IRLoop* loop_) : mProg(0), mModule(0), mFunc(0), mLoop(loop_), mStmt(0), mBB(0) {}
  AnatropScope(IRBB* bb_) : mProg(0), mModule(0), mFunc(0), mLoop(0), mStmt(0), mBB(bb_) {}
  AnatropScope(IRStmt* stmt_) : mProg(0), mModule(0), mFunc(0), mLoop(0), mStmt(stmt_), mBB(0) {}
  AnatropScope(IRModule* module_) : mProg(0), mModule(module_), mFunc(0), mLoop(0), mStmt(0), mBB(0) {}
  bool IsProg() const {return mProg != 0;}
  bool IsModule() const {return mModule != 0;}
  bool IsFunc() const {return mFunc != 0;}
  bool IsBB() const {return mBB != 0;}
  bool IsLoop() const {return mLoop != 0;}
  bool IsStmt() const {return mStmt != 0;}
  bool IsLocal() const {return IsBB() || IsStmt() || IsLoop();}
  bool IsGlobal() const {return !IsLocal();}
  IRProgram* GetProg() const {REQUIREDMSG(IsProg() == true, ieStrParamValuesDBCViolation); return mProg;}
  IRFunction* GetFunc() const {REQUIREDMSG(IsFunc() == true, ieStrParamValuesDBCViolation); return mFunc;}
  IRModule* GetModule() const {REQUIREDMSG(IsModule() == true, ieStrParamValuesDBCViolation); return mModule;}
  IRLoop* GetLoop() const {REQUIREDMSG(IsLoop() == true, ieStrParamValuesDBCViolation); return mLoop;}
  IRStmt* GetStmt() const {REQUIREDMSG(IsStmt() == true, ieStrParamValuesDBCViolation); return mStmt;}
  IRBB* GetBB() const {REQUIREDMSG(IsBB() == true, ieStrParamValuesDBCViolation); return mBB;}
  //! \brief Returns true if the scope object has been detached from ir.
  bool IsDetached() const {
    bool retVal(false);
    if (mBB != 0) {
      retVal = pred.pIsDetached(mBB);
    } else if (mStmt != 0) {
      retVal = pred.pIsDetached(mStmt);
    } // if
    return retVal;
  } // AnatropScope::IsDetached
private:
  // Note only one of below is non-null.
  IRProgram* mProg;
  IRModule* mModule;
  IRFunction* mFunc;
  IRLoop* mLoop;
  IRBB* mBB;
  IRStmt* mStmt;
private:
  friend class AnatropManager;
}; // class AnatropScope

//! \brief Base class for all anatrops (analysis, transformation, and optimization).
//! \note Note that each anatrop has bool typed execute_triggers option.
class Anatrop : public IDebuggable {
private:
  Anatrop() : mScope(&Invalid<IRProgram>::Obj()) {}
public:
  Anatrop(const AnatropScope& scope_);
  //! \brief Calls one of AnatropDo depending on the scope.
  //! \note when execute_triggers option is true executes all the triggers after
  //!        running the anatrop.
  hFInt32 Do();
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  Options& GetOptions() {return mOptions;}
  virtual const char* GetName() const {REQUIREDMSG(0, ieStrFuncShouldBeOverridden);}
  virtual Anatrop* Clone(const AnatropScope& scope_) const {REQUIREDMSG(0, ieStrFuncShouldBeOverridden);}
protected:
  //! \brief If anatrop has options these should be set up in the derived anatrop's function.
  virtual void InitOptions() {/* LIE */}
  //! \brief The function is called when options specified for the anatrop is set.
  //! You may want to handle this function for checking the consistency of options or
  //! setting faster variables for accessing the options.
  virtual void OnOptionsSet() {/* LIE */}
  // A default implementation is provided for DoAnatrop functions,
  // but you should not call them.
  //! \brief Does the analysis/transformation/optimization at program scope, as applicable
  //!        it calls the module signatured version.
  virtual hFInt32 AnatropDo(IRProgram* prog_);
  //! \brief Does the analysis/transformation/optimization at module scope, as applicable
  //!        it calls the function signatured version.
  virtual hFInt32 AnatropDo(IRModule* module_);
  //! \brief Does the analysis/transformation/optimization at function scope, as applicable
  //!        it calls the basic block signatured version.
  //! \note You should do CFG related at/optimizations in this function.
  //! \note It removed bbs and stmt that are marked for removal of singleton irbuilder.
  virtual hFInt32 AnatropDo(IRFunction* func_);
  //! \brief Does the analysis/transformation/optimization at basic block scope, as applicable
  //!        it calls the statement signatured version.
  virtual hFInt32 AnatropDo(IRBB* bb_);
  //! \brief Does the analysis/transformation/optimization at loop scope.
  //! \todo H Design: Add an option that min scope is a loop and if e.g.
  //!       function scope given it should call loop not bb anatrop do.
  virtual hFInt32 AnatropDo(IRLoop* loop_);
  //! \brief Does the analysis/transformation/optimization at statement scope.
  virtual hFInt32 AnatropDo(IRStmt* stmt_);
  const AnatropScope& GetScope() const {return mScope;}
  //! \brief Must return true if given scope is a valid one for the anatrop.
  virtual bool IsValidScope(const AnatropScope& scope_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
  } // Anatrop::IsValidScope
  //! \brief Called when all the items in the provided scope are processed.
  //! It removes the collected statements and basic blocks as default action.
  virtual void OnFinish();
  // These functions is called before any AnatropDo is called.
  virtual void OnStart(IRProgram* prog_) {}
  virtual void OnStart(IRModule* module_) {}
  virtual void OnStart(IRFunction* func_) {}
  virtual void OnStart(IRLoop* loop_) {}
  virtual void OnStart(IRBB* bb_) {}
  virtual void OnStart(IRStmt* stmt_) {}
protected:
  void addBoolOption(const string& optionName_, bool defaultValue_) {
    mOptions.addBoolOption(optionName_, defaultValue_);
  } // Anatrop::addBoolOption
  void addEnumOption(const string& optionName_, const string& value_, bool isDefault_ = false) {
    mOptions.addEnumOption(optionName_, value_, isDefault_);
  } // Anatrop::addEnumOption
private:
  Options mOptions; //!< Options related to this Anatrop.
  AnatropScope mScope;
  vector<Anatrop*> mTriggers;
private:
  friend class AnatropManager;
}; // class Anatrop

//! \brief Manages the options of anatrops.
//! \note AnatropManager is a Singleton.
class AnatropManager : public IDebuggable {
public:
  AnatropManager() : 
    mExecutionTree(0) 
  {
  } // AnatropManager::AnatropManager
  virtual ~AnatropManager() {}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Processes a single option.
  void ProcessOption(const string& optText_);
  //! \brief You may not register an anatrop name more than once.
  void RegisterAnatrop(Anatrop* protAnatrop_);
  //! \brief Registers all anatrops.
  //! Must be called once just before any anatrop is executed.
  //! \pre Call only once.
  void RegisterAnatrops();
  //! \brief Returns an anatrop specified by name and a scope.
  //! \pre The scope must be a valid scope for the given anatrop.
  //! \sa Anatrop::IsValidScope
  Anatrop* CreateAnatrop(const string& atoName_, const AnatropScope& scope_);
  //! \pre The scope must be a valid scope for the given anatrop.
  //! \sa Anatrop::IsValidScope
  hFUInt32 Execute(const string& atoName_, const AnatropScope& scope_, const GenericOptions& options_ = GenericOptions());
  //! \brief Adds the specified anatrop to the triggers list.
  void Trigger(const string& atoName_, const AnatropScope& scope_, const GenericOptions& options_ = GenericOptions());
private: // Member functions
  //! \brief Called by anatrops just before the execution.
  //! \param parent_ may be Singleton Anatrop for top level anatrops.
  void onAnatropExecution(Anatrop* parent_, Anatrop* anatrop_);
private: // Member data
  class ATOCALL {
  public:
    ATOCALL(Anatrop* parent_, Anatrop* anatrop_) :
      mAnatrop(anatrop_),
      mParent(parent_)
    {
    } // ATOCALL::ATOCALL
    vector<hFUInt32> mIndex;
    Anatrop* mParent;
    Anatrop* mAnatrop;
  }; // class ATOCALL
  //! \brief Maps anatrop names to a prototype anatrop.
  map<string, Anatrop*> name2Anatrop;
  //! \brief You can define a name for a list of anatrops by options.
  //! \note Name must be unique among other list names and anatrop names.
  //! \sa ProcessOption.
  map<string, list<Anatrop*> > listName2Anatrop;
  //! \brief Keeps the execution history of anatrops.
  tree<ATOCALL*> mExecutionTree;
  stack<Anatrop*> mExecutionStack;
  vector<Anatrop*> mTriggers;
private:
  friend class Anatrop;
}; // class AnatropManager

extern AnatropManager* anatropManager;

#endif


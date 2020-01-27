// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __DEBUGTRACE_HPP__
#define __DEBUGTRACE_HPP__

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __ENUMS_HPP__
#include "enums.hpp"
#endif
#ifndef __HOSTTARGETTYPES_HPP__
#include "hosttargettypes.hpp"
#endif

/*!
  \defgroup debugtrace Debug and Trace Utilities
  Every function should contain either debug or trace utility begin and end macros.
  \par Debug and Trace Macros
  Debug macros ([BE][M]DEBUG[N]) are present in the compiler that is build with DEBUG mode.
  They will not be present in the RELEASE mode compiler. Trace macros are same as debug macros
  but they are present in TRACE enabled RELEASE mode compiler. They are exactly same in
  DEBUG mode compiler.

  \par Where to use Debug and Trace macros?
  Debug and Trace macros should be placed at the beginning and end of every function.
  They should be placed before the DBC (design by contract macros). In class member functions
  where "this" pointer is available, you should use the "M" version of the macros, e.g. BMDEBUG1(aname).
  Use the Trace macros at important execution points, i.e. important compiler phases, regions of
  execution that can be skipped by command line options. Since trace is not used for debugging
  but rather timing and high level bug handling in released compiler.

  \par How to configure Debug and Trace utility?
  It is a lot of information if you want to dump debug/trace output in every function.
  However, it is also better to have as much information as possible.
  Therefore, debug and trace utility has a configuration file. This file is read only once
  at the beginning of compilation. Depending on the configuration you may change the amount of
  data that is produced during the execution of compiler.

  \attention There should be no side effect in the Debug, Trace macro arguments since they will be
  removed from release mode compiler and side effects may change the program behaviour.
*/

class DTCCommandToken {
private:
  DTCCommandToken() :
    mCommand(DTCCInvalid),
    mIsRemove(0),
    mObject(0)
  {
    DTASSERT(0, "Do not use default constructor");
  } // DTCCommandToken::DTCCommandToken
public:
  //! \brief Applies a context command only to an object.
  DTCCommandToken(DTCCommand command_, const IDebuggable* object_) :
    mCommand(command_),
    mIsRemove(false),
    mObject(object_)
  {
  } // DTCCommandToken::DTCCommandToken
  DTCCommandToken(DTCCommand command_) :
    mCommand(command_),
    mIsRemove(false),
    mObject(0)
  {
  } // DTCCommandToken::DTCCommandToken
  DTCCommandToken(DTCCommand command_, bool isRemove_) :
    mCommand(command_),
    mIsRemove(isRemove_),
    mObject(0)
  {
  } // DTCCommandToken::DTCCommandToken
  DTCCommandToken(const DTCCommandToken& ct_) :
    mCommand(ct_.mCommand),
    mIsRemove(ct_.mIsRemove),
    mObject(ct_.mObject)
  {
  } // DTCCommandToken::DTCCommandToken
  const DTCCommandToken& operator = (const DTCCommandToken& ct_) {
    mCommand = ct_.mCommand;
    mIsRemove = ct_.mIsRemove;
    mObject = ct_.mObject;
  } // DTCCommandToken::operator =
  DTCCommand mCommand;
  bool mIsRemove;
  const IDebuggable* mObject;
  static hFInt32 lIndent;
}; // DTCCommandToken

DTCCommandToken operator ++ (DTCCommand command_); // prefix operator
DTCCommandToken operator -- (DTCCommand command_); // prefix operator
ostream& operator << (ostream& o, const DTCCommandToken& commandToken_);
ostream& operator << (ostream& o, DTCCommand command_);

//! \brief Helper function for expression dumping.
DTCCommandToken exprcxt(const IDebuggable* obj_);
//! \brief Helper function for dumping a use of an expression.
//! Use of an expression is (*&)expr, or IREDeref(IREAddrOf(expr)).
//! The effect of this helper is to inhibit the "*&" output.
DTCCommandToken usecxt(const IDebuggable* obj_);
//! \brief Helper function for reference dumping.
//! \pre Non-null \p obj.
DTCCommandToken refcxt(const IDebuggable* obj_);
//! \brief Helper function to dump the object in program dump format with match info.
DTCCommandToken matcherrcxt(const IDebuggable* obj_, CGFuncContext& funcContext_);
//! \brief Helper function to dump the object in program dump format.
DTCCommandToken progcxt(const IDebuggable* obj_);
//! \brief Helper function to dump the object in hwdescription format.
DTCCommandToken hwdesccxt(const IDebuggable* obj_);
ostream& operator << (ostream& o, const DTCCommandToken& commandToken_);

class Debuggable {
public:
  Debuggable(long int int_) : mIDebuggable(0) {mString << int_ << ends;}
  Debuggable(const char* str_) : mIDebuggable(0) {mString << str_ << ends;}
  Debuggable(int int_) : mIDebuggable(0) {mString << int_ << ends;}
  Debuggable(tristate tristate_) : mIDebuggable(0) {mString << tristate_ << ends;}
  Debuggable(size_t int_) : mIDebuggable(0) {mString << int_ << ends;}
  Debuggable(double double_) : mIDebuggable(0) {mString << double_ << ends;}
  Debuggable(const string& string_) : mIDebuggable(0) {mString << string_ << ends;}
  Debuggable(const IRSignedness& sign_) : mIDebuggable(0) {mString << sign_ << ends;}
  Debuggable(const ICPlace& place_) : mIDebuggable(0) {mString << place_ << ends;}
  Debuggable(const IDebuggable* debuggable_) :
    mIDebuggable(debuggable_)
  {
    if (!debuggable_) {
      mString << "<null>" << ends;
    } // if
  } // Debuggable::Debuggable
  bool IsString() const {return mIDebuggable == 0;}
  const IDebuggable* GetObj() const {return mIDebuggable;}
  string GetStr() const {return mString.str();}
private: // Member data
  mutable ostrstream mString;
  const IDebuggable* mIDebuggable;  //!< May be null.
}; // class Debuggable

//! \brief DebugTrace Utility
//! \note DebugTrace is a Singleton.
//! \attention There should not be any direct or indirect call to DBC macros within DebugTrace,
//! as well as DTConfig, DTContext, member functions and globals that they call.
class DebugTrace {
public:
  DebugTrace();
  virtual ~DebugTrace();
  // ////////////////////////
  // Define all objects' debug outputs
  // ////////////////////////
  DebugTrace& debug(const DTContext& context_);
  DebugTrace& debug(const DTContext& context_, const Debuggable& debugObj_);
  bool IsValid() const;
  void DumpCallStack();
  //! \brief an implementation of DTCCDumpRef context for IDebuggable objects.
  char* dumpObjRef(const IDebuggable* obj_, const DTContext& context_, ostream& toStr_);
  void pdebug(const string& category_, const string& subCategory_, const string& msg_);
  //! \brief You must set the assembly description for instruction dumps.
  void SetASMDesc(const ASMDesc& asmDesc_) {mASMDesc = &asmDesc_;}
  const ASMDesc* GetASMDesc() const {return mASMDesc;}
  void debugTraceStackPush() {mDebugTraceCallStack.push_back(true);}
  void debugTraceStackPop() {mDebugTraceCallStack.pop_back();}
  bool inDebugTraceCall() const {return !mDebugTraceCallStack.empty();}
  void beginFuncTrace(const SrcLoc& srcLoc_, const string& exePoint_, const IDebuggable* this_, const string& paramText_);
  void endCurrFuncTrace(const SrcLoc& srcLoc_, const IDebuggable* this_, const string& paramText_);
  DebugTrace& debugStr(const DTContext& context_, const string& str_);
  //! \brief You can set the current relevant source file location.
  //! \note This information may not be exact.
  void SetSrcLoc(const SrcLoc& srcLoc_) {mSrcLoc = srcLoc_;}
  const SrcLoc& GetSrcLoc() const {return mSrcLoc;}
  void SetCGFuncContextParam(const CGFuncContext* context_) {
    mCurrCGCxtParamPtr = context_;
  } // DebugTrace::SetCGFuncContextParam
private: // Member functions
  //! \brief checkAndDumpProg: checks the exePoint agains the set if in the set program is dumped.
  //! \return Returns true if program is dumped.
  bool checkAndDumpProg(const string& exePoints_, const set<string>& exePoints_, const string& where_);
  bool checkAndDumpStats(const string& exePoint_, const set<string>& exePoints_, const string& where_);
  bool checkAndDumpHWDesc(const string& exePoints_, const set<string>& exePoints_, const string& where_);
  void dumpStats();
  //! \param amount Amount of indentation units - multiplied by tab size.
  void indent(hFInt32 amount_);
  //! \brief When true debug trace does not dump any information.
  bool inhibitOutput(const string& exePoint_, const DTContext& context_) const;
  bool doNotInhibitOutput(const string& exePoint_, const DTContext& context_) const;
private: // Member data
  //! \brief Indent amount for functions trace.
  //! \note mDTStack has both trace true/false functions, mIndent keeps track
  //!       of the count of true ones.
  hFUInt32 mIndent;
  SrcLoc mSrcLoc; //!< Current relevant source location.
  //! \brief When not empty a debugTrace function is active.
  //! We should not call any other debugTrace function.
  //! \todo M Design: Would a single bool flag be sufficient?
  vector<bool> mDebugTraceCallStack;
  const ASMDesc* mASMDesc; //!< Assembly description for instructions' dump.
  //! \brief Keeps the currently executing functions this pointer.
  //! Valid only in begin and end lines.
  const IDebuggable* mCurrThisPointer;
  const CGFuncContext* mCurrCGCxtParamPtr;
  struct {
    HTML::HTMLNode* mTopNode;
    HTML::HTMLNode* mCurrFunc;
    HTML::HTMLNode* mCurrExePoint;
  } mHTMLOutput;
  ostream* mOutputFile; //!< Output can be cout or a file.
  //! \brief outputFile points to cout, when true.
  //! This variable is checked to dump the output to cout instead of file.
  //! Note that it is also used to dump intermediate stream to cout instead of
  //! intermediate ostrstream, etc.
  bool mOutputToCOut;
  typedef struct DTSTACK {
    DTSTACK(const string& funcName_, const IDebuggable* this_, bool trace_) :
      mFuncName(funcName_),
      mThis(this_),
      mTrace(trace_)
    {
    } // DTSTACK::DTSTACK
    string mFuncName;
    const IDebuggable* mThis;
    bool mTrace;
  } DTSTACK;
  deque<DTSTACK> mDTStack; //!< executionPoints
  hFInt8 mParamCount;
  map<string, hFInt32> mHitCount; //!< number of times each execution point is hit, counts only the starts.
  hSInt16 mSkipFuncStackSize; //!< Size mark if dtStack.size is bigger no output shall be displayed.
  hSInt16 mSkipChildCallStackSize;
  hSInt16 mFirstShowOnlyStackSize;
  static hFInt32 smRefCount; //!< Do a run-time singleton-ness check.
  friend class DTConfig;
}; // class DebugTrace

extern DebugTrace* debugTrace;

//! \brief Base class for all invalid objects.
//! \note You should call the helper hDebugTrace in the derived invalid class.
//! \par For Invalid to work.
//! We need classes with non-pure member functions.
//! We need classes (of type T) that implements a constructor accepting const Invalid<T>*.
//! \todo M Design: This class is misplaced it should be in utils.hpp but
//!       dependencies in debugTrace will consume some time.
template<class T>
class Invalid : public T, public Singleton<Invalid<T> > {
public:
  Invalid() : T(this) {}
  virtual ~Invalid() {}
  //! \brief An invalid object itself is always valid.
  virtual bool IsValid() const {return true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const {
    DTLOG("Invalid::debugTrace(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
    Singleton<DebugTrace>::Obj().dumpObjRef(this, context_, toStr_);
    if (context_ != DTCCDumpProg && context_ != DTCCDumpRef && context_ != DTCCDumpHWDesc) {
      toStr_ << ends;
    } // if
    DTLOG(dtlUnindent << dtlEndl << "} // Invalid::debugTrace" << dtlEndl);
    return "";
  } // Invalid<T>::debugTrace
protected:
  typename T::mustBeIDebuggable mbid; //!< T must be derived from IDebuggable.
  typedef Invalid<T> Base;
}; // class Invalid

// //////////////////////////
// DEBUG macros
// //////////////////////////
//! \brief Dumps a debug string in to debug/trace.
//! \note the format of dump is "what[>sub]:txt", if sub is "" then [>sub] is not dumped.
#if RELEASEBUILD
#define PDEBUG(what__, sub__, txt__)
#else
#define PDEBUG(what__, sub__, txt__) if (DEBUGBUILD) {Singleton<DebugTrace>::Obj().pdebug(std::string("PDEBUG>") + what__, sub__, \
  static_cast<const ostringstream&>(ostringstream().flush() << txt__ << ends).str());}
#endif

//! \brief In debug mode flushes the given ostream.
//! \note In release mode it does nothing.
#define DBGFLUSH(o) ((o) << flush)

#define FIRSTParams const string& exePoint_, const IDebuggable* this_
#define SRCParam const char* file_, hFUInt32 line_
#define DEBParam const Debuggable&
//! \note We have the numbered functions for strong type checking. All these
//!       functions internally calls the DEBUGFunc.
void DEBUG1Func(FIRSTParams, SRCParam);
void DEBUG2Func(FIRSTParams, DEBParam, SRCParam);
void DEBUG3Func(FIRSTParams, DEBParam, DEBParam, SRCParam);
void DEBUG4Func(FIRSTParams, DEBParam, DEBParam, DEBParam, SRCParam);
void DEBUG5Func(FIRSTParams, DEBParam, DEBParam, DEBParam, DEBParam, SRCParam);
void DEBUG6Func(FIRSTParams, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, SRCParam);
void DEBUG7Func(FIRSTParams, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, SRCParam);
void DEBUG8Func(FIRSTParams, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, SRCParam);
void DEBUG9Func(FIRSTParams, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, SRCParam);
void DEBUG10Func(FIRSTParams, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, DEBParam, SRCParam);
void DEBUGFunc(FIRSTParams, SRCParam, ...);
#undef SRCParam
#undef DEBParam
#undef FIRSTParams

#if DEBUGBUILD
#define BMDEBUG1(name) if (DEBUGBUILD) {DEBUG1Func(name, this, __FILE__, __LINE__);}
#define BDEBUG1(name) if (DEBUGBUILD) {DEBUG1Func(name, 0, __FILE__, __LINE__);}
#define EMDEBUG0() if (DEBUGBUILD) {DEBUG1Func("$end$", this, __FILE__, __LINE__);}
#define EDEBUG0() if (DEBUGBUILD) {DEBUG1Func("$end$", 0, __FILE__, __LINE__);}
#define BMDEBUG2(name, p1) if (DEBUGBUILD) {DEBUG2Func(name, this, p1, __FILE__, __LINE__);}
#define BDEBUG2(name, p1) if (DEBUGBUILD) {DEBUG2Func(name, 0, p1, __FILE__, __LINE__);}
#define EMDEBUG1(p1) if (DEBUGBUILD) {DEBUG2Func("$end$", this, p1, __FILE__, __LINE__);}
#define EDEBUG1(p1) if (DEBUGBUILD) {DEBUG2Func("$end$", 0, p1, __FILE__, __LINE__);}
#define BMDEBUG3(name, p1, p2) if (DEBUGBUILD) {DEBUG3Func(name, this, p1, p2, __FILE__, __LINE__);}
#define BDEBUG3(name, p1, p2) if (DEBUGBUILD) {DEBUG3Func(name, 0, p1, p2, __FILE__, __LINE__);}
#define EMDEBUG2(p1, p2) if (DEBUGBUILD) {DEBUG3Func("$end$", this, p1, p2, __FILE__, __LINE__);}
#define EDEBUG2(p1, p2) if (DEBUGBUILD) {DEBUG3Func("$end$", 0, p1, p2, __FILE__, __LINE__);}
#define BMDEBUG4(name, p1, p2, p3) if (DEBUGBUILD) {DEBUG4Func(name, this, p1, p2, p3, __FILE__, __LINE__);}
#define BDEBUG4(name, p1, p2, p3) if (DEBUGBUILD) {DEBUG4Func(name, 0, p1, p2, p3, __FILE__, __LINE__);}
#define EMDEBUG3(p1, p2, p3) if (DEBUGBUILD) {DEBUG4Func("$end$", this, p1, p2, p3, __FILE__, __LINE__);}
#define EDEBUG3(p1, p2, p3) if (DEBUGBUILD) {DEBUG4Func("$end$", 0, p1, p2, p3, __FILE__, __LINE__);}
#define BMDEBUG5(name, p1, p2, p3, p4) if (DEBUGBUILD) {DEBUG5Func(name, this, p1, p2, p3, p4, __FILE__, __LINE__);}
#define BDEBUG5(name, p1, p2, p3, p4) if (DEBUGBUILD) {DEBUG5Func(name, 0, p1, p2, p3, p4, __FILE__, __LINE__);}
#define EMDEBUG4(p1, p2, p3, p4) if (DEBUGBUILD) {DEBUG5Func("$end$", this, p1, p2, p3, p4, __FILE__, __LINE__);}
#define EDEBUG4(p1, p2, p3, p4) if (DEBUGBUILD) {DEBUG5Func("$end$", 0, p1, p2, p3, p4, __FILE__, __LINE__);}
#define BMDEBUG6(name, p1, p2, p3, p4, p5) if (DEBUGBUILD) {DEBUG6Func(name, this, p1, p2, p3, p4, p5, __FILE__, __LINE__);}
#define BDEBUG6(name, p1, p2, p3, p4, p5) if (DEBUGBUILD) {DEBUG6Func(name, 0, p1, p2, p3, p4, p5, __FILE__, __LINE__);}
#define EMDEBUG5(p1, p2, p3, p4, p5) if (DEBUGBUILD) {DEBUG6Func("$end$", this, p1, p2, p3, p4, p5, __FILE__, __LINE__);}
#define EDEBUG5(p1, p2, p3, p4, p5) if (DEBUGBUILD) {DEBUG6Func("$end$", 0, p1, p2, p3, p4, p5, __FILE__, __LINE__);}
#define BMDEBUG7(name, p1, p2, p3, p4, p5, p6) if (DEBUGBUILD) {DEBUG7Func(name, this, p1, p2, p3, p4, p5, p6, __FILE__, __LINE__);}
#define BDEBUG7(name, p1, p2, p3, p4, p5, p6) if (DEBUGBUILD) {DEBUG7Func(name, 0, p1, p2, p3, p4, p5, p6, __FILE__, __LINE__);}
#define EMDEBUG6(p1, p2, p3, p4, p5, p6) if (DEBUGBUILD) {DEBUG7Func("$end$", this, p1, p2, p3, p4, p5, p6, __FILE__, __LINE__);}
#define EDEBUG6(p1, p2, p3, p4, p5, p6) if (DEBUGBUILD) {DEBUG7Func("$end$", 0, p1, p2, p3, p4, p5, p6, __FILE__, __LINE__);}
#define BMDEBUG8(name, p1, p2, p3, p4, p5, p6, p7) if (DEBUGBUILD) {DEBUG8Func(name, this, p1, p2, p3, p4, p5, p6, p7, __FILE__, __LINE__);}
#define BDEBUG8(name, p1, p2, p3, p4, p5, p6, p7) if (DEBUGBUILD) {DEBUG8Func(name, 0, p1, p2, p3, p4, p5, p6, p7, __FILE__, __LINE__);}
#define EMDEBUG7(p1, p2, p3, p4, p5, p6, p7) if (DEBUGBUILD) {DEBUG8Func("$end$", this, p1, p2, p3, p4, p5, p6, p7, __FILE__, __LINE__);}
#define EDEBUG7(p1, p2, p3, p4, p5, p6, p7) if (DEBUGBUILD) {DEBUG8Func("$end$", 0, p1, p2, p3, p4, p5, p6, p7, __FILE__, __LINE__);}
#define BMDEBUG9(name, p1, p2, p3, p4, p5, p6, p7, p8) if (DEBUGBUILD) {DEBUG9Func(name, this, p1, p2, p3, p4, p5, p6, p7, p8, __FILE__, __LINE__);}
#define BDEBUG9(name, p1, p2, p3, p4, p5, p6, p7, p8) if (DEBUGBUILD) {DEBUG9Func(name, 0, p1, p2, p3, p4, p5, p6, p7, p8, __FILE__, __LINE__);}
#define EMDEBUG8(p1, p2, p3, p4, p5, p6, p7, p8) if (DEBUGBUILD) {DEBUG9Func("$end$", this, p1, p2, p3, p4, p5, p6, p7, p8, __FILE__, __LINE__);}
#define EDEBUG8(p1, p2, p3, p4, p5, p6, p7, p8) if (DEBUGBUILD) {DEBUG9Func("$end$", 0, p1, p2, p3, p4, p5, p6, p7, p8, __FILE__, __LINE__);}
#define BMDEBUG10(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) if (DEBUGBUILD) {DEBUG10Func(name, this, p1, p2, p3, p4, p5, p6, p7, p8, p9, __FILE__, __LINE__);}
#define BDEBUG10(name, p1, p2, p3, p4, p5, p6, p7, p8, p9) if (DEBUGBUILD) {DEBUG10Func(name, 0, p1, p2, p3, p4, p5, p6, p7, p8, p9, __FILE__, __LINE__);}
#define EMDEBUG9(p1, p2, p3, p4, p5, p6, p7, p8, p9) if (DEBUGBUILD) {DEBUG10Func("$end$", this, p1, p2, p3, p4, p5, p6, p7, p8, p9, __FILE__, __LINE__);}
#define EDEBUG9(p1, p2, p3, p4, p5, p6, p7, p8, p9) if (DEBUGBUILD) {DEBUG10Func("$end$", 0, p1, p2, p3, p4, p5, p6, p7, p8, p9, __FILE__, __LINE__);}
#else
#define BMDEBUG1(name)
#define BDEBUG1(name)
#define EMDEBUG0()
#define EDEBUG0()
#define BMDEBUG2(name, p1)
#define BDEBUG2(name, p1)
#define EMDEBUG1(p1)
#define EDEBUG1(p1)
#define BMDEBUG3(name, p1, p2)
#define BDEBUG3(name, p1, p2)
#define EMDEBUG2(p1, p2)
#define EDEBUG2(p1, p2)
#define BMDEBUG4(name, p1, p2, p3)
#define BDEBUG4(name, p1, p2, p3)
#define EMDEBUG3(p1, p2, p3)
#define EDEBUG3(p1, p2, p3)
#define BMDEBUG5(name, p1, p2, p3, p4)
#define BDEBUG5(name, p1, p2, p3, p4)
#define EMDEBUG4(p1, p2, p3, p4)
#define EDEBUG4(p1, p2, p3, p4)
#define BMDEBUG6(name, p1, p2, p3, p4, p5)
#define BDEBUG6(name, p1, p2, p3, p4, p5)
#define EMDEBUG5(p1, p2, p3, p4, p5)
#define EDEBUG5(p1, p2, p3, p4, p5)
#define BMDEBUG7(name, p1, p2, p3, p4, p5, p6)
#define BDEBUG7(name, p1, p2, p3, p4, p5, p6)
#define EMDEBUG6(p1, p2, p3, p4, p5, p6)
#define EDEBUG6(p1, p2, p3, p4, p5, p6)
#define BMDEBUG8(name, p1, p2, p3, p4, p5, p6, p7)
#define BDEBUG8(name, p1, p2, p3, p4, p5, p6, p7)
#define EMDEBUG7(p1, p2, p3, p4, p5, p6, p7)
#define EDEBUG7(p1, p2, p3, p4, p5, p6, p7)
#define BMDEBUG9(name, p1, p2, p3, p4, p5, p6, p7, p8)
#define BDEBUG9(name, p1, p2, p3, p4, p5, p6, p7, p8)
#define EMDEBUG8(p1, p2, p3, p4, p5, p6, p7, p8)
#define EDEBUG8(p1, p2, p3, p4, p5, p6, p7, p8)
#define BMDEBUG10(name, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define BDEBUG10(name, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define EMDEBUG9(p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define EDEBUG9(p1, p2, p3, p4, p5, p6, p7, p8, p9)
#endif

#if RELEASEBUILD
#define BDEBUGCALL() if (0) {
#define EDEBUGCALL() }
#else
//! \brief Begin IDebuggable::debug function macro.
//! This macros should be at the beginning of IDebuggable::debug functions of all objects that calls
//! other IDebuggable::debug functions. It is used to prevent infinite recursion
//! when some other member function is called.
#define BDEBUGCALL() {debugTrace->debugTraceStackPush();}
//! \brief End IDebuggable::debug function macro.
//! This macro should be used at the end of IDebuggable::debug functions of all objects.
#define EDEBUGCALL() {debugTrace->debugTraceStackPop();}
#endif

#endif // __DEBUGTRACE_HPP__


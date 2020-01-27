// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __HOSTTARGETTYPES_HPP__
#include "hosttargettypes.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __DFA_HPP__
#include "dfa.hpp"
#endif
#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif
#ifndef __DEBUGINFO_DWARF_HPP__
#include "debuginfo_dwarf.hpp"
#endif
#ifndef __HTMLDOC_HPP__
#include "htmldoc.hpp"
#endif
#include <signal.h>
#include <assert.h>

//! \todo M Design: Add smart paranthesis around expression dump in C style
//!       output.
//! \todo M Design: try to create unqiue label names from curr func name and
//!       restart numbering.
//! \todo M Design: Unify *& in var use dump.
//! \todo M Design: Unique label names restart from func name.
//! \todo M Design: Add id count per class, next to unique obj id.
//! \todo M Build: Add a new build mode where DBC macros do not produce
//!       debug output since it makes it difficult to debug the application.
//!       Whole debug trace can be disabled too, may be it slows down the
//!       compilation, debugger only guys can do their job faster?

hFInt32 DebugTrace::smRefCount;
map<string, HTML::HTMLNode*> HTML::HTMLNode::smAllNodes;
hFInt32 HTML::HTMLNode::smAutoIDIndex;
hFInt32 DTCCommandToken::lIndent;
DebugTrace* debugTrace(0);

//! \brief Context to be used when match info needs to be dumped along with program dump.
//! \note This pointer is valid during matcherrcxt call (when context is DTCCDumpMatchInfo).
static CGFuncContext* sflMatchInfoFuncContext(0);

static bool
anyMatch(const string& exePoint_, const set<string>& exePointPatterns_);

//! \brief Helper function for dumping the ostrstreams in debuggers.
void 
dtp_ostrstream(ostream& o) {
  ostrstream* ostr = dynamic_cast<ostrstream*>(&o);
  DTASSERT(ostr != 0, "Internal error");
  DTLOG(ostr->str() << flush);
} // dtp_ostrstream

void 
dtp_print(IDebuggable* debuggable_) {
  ostrstream dbg;
  dbg << refcxt(debuggable_) << " " << progcxt(debuggable_) << endl << ends;
  dtp_ostrstream(dbg);
} // dtp_print

//! \todo M Design: prepare checklist of debugging:
//! \li first check the debug trace if everything is okay
//! \li then investigate the program dump.

//! \todo M Design: Do not use automatic indentation after end lines.
static map<DTCCommand, string> toStrCommands;
static const char*
toStr(DTCCommand command) {
  if (toStrCommands.empty()) {
    set<string> lAlreadyUsed;
#define CMDSTR(cmd, str)\
  DTASSERT(lAlreadyUsed.find(str) == lAlreadyUsed.end(), ieStrInconsistentInternalStructure); \
  toStrCommands[cmd] = str; lAlreadyUsed.insert(str);
    CMDSTR(DTCCInvalid, "inv");
    CMDSTR(DTCCNotSet, "nst");
    CMDSTR(DTCCAbort, "abr");
    CMDSTR(DTCCDumpProg, "dpr");
    CMDSTR(DTCCDumpRef, "drf");
    CMDSTR(DTCCIndent, "ind");
    CMDSTR(DTCCEndLine, "endl");
    CMDSTR(DTCCSEGV, "seg");
    CMDSTR(DTCCDumpExpr, "dex");
    CMDSTR(DTCCDBC, "dbc");
    CMDSTR(DTCCDumpUseExpr, "due");
    CMDSTR(DTCCDumpHWDesc, "dhw");
    CMDSTR(DTCCDumpDFA, "dfa");
    CMDSTR(DTCCDumpCGCxt, "cgx");
    CMDSTR(DTCCIncIndent, "++i");
    CMDSTR(DTCCDecIndent, "--i");
    CMDSTR(DTCCDumpMatchInfo, "dmi");
    CMDSTR(DTCCDumpParam, "prm");
    CMDSTR(DTCCDumpThis, "dth");
    CMDSTR(DTCCDumpStats, "dst");
#undef CMDSTR
  } // if
  return toStrCommands[command].c_str();
} // toStr

//! \brief returns "T" or "F" depending on the mark of the statement.
static string 
getMark(const IRTree* tree_) {
  string retVal("[");
  for (hFUInt32 c(0); c < eIRTMLast; ++ c) {
    if (tree_->HasMarked(eIRTreeMark(c)) == true) {
      retVal += tree_->IsMarkFalse(eIRTreeMark(c)) == true ? "F" : "T";
    } else {
      retVal += ".";
    } // if
  } // for
  return retVal+"]";
} // getMark

static const char* 
getAddrTaken(const IRObject* obj_) {
  const char* retVal(obj_->IsAddrTaken() == true ? "&&+" : "&&-");
  return retVal;
} // getAddrTaken

const char* 
dumpMatchInfo(const DTContext& context_, const IRExpr* expr_) {
  static string retVal;
  if (sflMatchInfoFuncContext != 0) {
    map<IRExpr*, IRExprNodeMatcherInfo*>::const_iterator 
      lIt(sflMatchInfoFuncContext->mIRNodeMatchInfo.find(const_cast<IRExpr*>(expr_)));
    if (lIt != sflMatchInfoFuncContext->mIRNodeMatchInfo.end()) {
      IRExprNodeMatcherInfo* lInfo(lIt->second);
      retVal = "mi{";
      for (hFInt32 c(0); c < lInfo->mMatchedRules.size(); ++ c) {
        if (c != 0) {
          retVal += ",";
        } // if
        retVal += lInfo->mMatchedRules[c]->GetName();
      } // for
      if (lInfo->mMatchedRules.empty() == true) {
        retVal += "<warning: check that if a parent rule has covered this expression>";
      } // if
      retVal += "}";
    } else {
      retVal = "mi{<not yet matched>}";
    } // if
  } else {
    retVal = "";
  } // if
  return retVal.c_str();
} // dumpMatchInfo

const char*
dtToStr(const DTContext& context_) {
  ostrstream retVal;
  bool lFlagged = false;
  retVal << "[";
  map<DTCCommand, string>::iterator lIt(toStrCommands.begin());
  while (lIt != toStrCommands.end()) {
    if (context_.mCommand & lIt->first) {
      if (lFlagged == true) {
        retVal << ",";
      } // if
      retVal << lIt->second;
      lFlagged = true;
    } // if
    ++ lIt;
  } // while
  retVal << "]" << ends;
  return retVal.str();
} // dtToStr

// /////////////////////////////////////////
// Helper operators for debugTrace functions
// /////////////////////////////////////////
DTCCommandToken
exprcxt(const IDebuggable* obj_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  return DTCCommandToken(DTCCommand(DTCCDumpExpr | DTCCDumpProg), obj_);
} // exprcxt

DTCCommandToken
hwdesccxt(const IDebuggable* obj_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  return DTCCommandToken(DTCCDumpHWDesc, obj_);
} // hwdesccxt

DTCCommandToken 
matcherrcxt(const IDebuggable* obj_, CGFuncContext& funcContext_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  sflMatchInfoFuncContext = &funcContext_;
  return DTCCommandToken(DTCCommand(DTCCDumpProg | DTCCDumpMatchInfo), obj_);
} // matcherrcxt

DTCCommandToken
progcxt(const IDebuggable* obj_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  return DTCCommandToken(DTCCDumpProg, obj_);
} // progcxt

DTCCommandToken
usecxt(const IDebuggable* obj_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  return DTCCommandToken(DTCCommand(DTCCDumpUseExpr | DTCCDumpProg), obj_);
} // usecxt

DTCCommandToken
refcxt(const IDebuggable* obj_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  DTCCommandToken retVal(DTCCDumpRef, obj_);
  if (dynamic_cast<const IRType*>(obj_) != 0 && dtConfig == DTCVExpandTypes) {
    retVal = DTCCommandToken(DTCCDumpProg, obj_);
  } // if
  return retVal;
} // refcxt

//! \brief operator that emits a debuggable object.
//! \pre Non-null \p obj.
static ostream&
operator << (ostream& o, const IDebuggable* obj_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  ASSERTMSG(0, ieStrFuncShouldNotBeCalled << 
    "You should use one of ...cxt or call debugTrace indirectly");
  return o;
} // operator <<

string
graphcxt(const IDebuggable* obj_) {
  DTASSERT(obj_ != 0, "Non-null obj required");
  ostrstream lOS;
  DTContext lDTContext(DTCCDumpProg);
  obj_->debug(lDTContext, lOS);
  lOS << ends;
  string retVal;
  string lSrc(lOS.str());
  // new lines must be converted to \\n. double-quotes (") must be escaped.
  for (hFUInt32 c(0); c < lSrc.size(); ++ c) {
    if (lSrc[c] == '\n') {
      retVal += "\\n";
    } else if (lSrc[c] == '"') {
      retVal += "\\\"";
    } else {
      retVal += lSrc[c];
    } // if
  } // for
  return retVal;
} // graphcxt

//! \brief Adds the command to the context.
//! \note same as unary "operator+".
//! \pre command may not be DTCCDumpRef, use refcxt function.
//! \pre command may not be DTCCDumpExpr, use exprcxt function.
ostream&
operator << (ostream& o, DTCCommand command_) {
  DTASSERT(command_ != DTCCDumpRef, "You are expected to use refcxt");
  DTASSERT(command_ != DTCCDumpExpr, "You are expected to use exprcxt");
  if (command_ == DTCCEndLine) {
    o << endl;
  } else if (command_ == DTCCIndent) {
    for (hFInt32 c(0); c < DTCCommandToken::lIndent; ++ c) {
      o << "  ";
    } // for
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  return o;
} // operator <<

ostream&
operator << (ostream& o, const DTCCommandToken& commandToken_) {
  static hFInt32 lIndent;
  if (commandToken_.mCommand == DTCCIncIndent) {
    DTCCommandToken::lIndent ++;
  } else if (commandToken_.mCommand == DTCCDecIndent) {
    DTCCommandToken::lIndent --;
  } else {
    DTContext lDTContext(commandToken_.mCommand);
    if (commandToken_.mObject != 0) {
      commandToken_.mObject->debug(lDTContext, o);
    } // if
    debugTrace->debugStr(lDTContext, "");
  } // if
  return o;
} // operator <<

char* DebugTrace::
dumpObjRef(const IDebuggable* obj_, const DTContext& context_, ostream& toStr_) {
  DTASSERT(obj_ != 0, "Non-null object required");
  char lClassName[128];
  //! \attention below is a gcc work around.
  char lTypeidName[256];
  strcpy(lTypeidName, typeid(*obj_).name());
  if (lTypeidName[0] == 'N') {
    // namespace case, N#name#?name
    sscanf(lTypeidName, "N%*d%s", lClassName);
  } else {
    sscanf(lTypeidName, "%*d%s", lClassName);
  } // if
  toStr_ << "<" << lClassName;
  if (dtConfig == DTCVShowThis) {
    toStr_ << "=";
    if (dtConfig == DTCVUseObjIds) {
     toStr_ << obj_->GetObjId();
    } else {
     DTASSERT(0, "Internal Error"); // check this case.
     toStr_ << (int)obj_;
    } // if
  } // if
  toStr_ << ">";
  return "";
} // DebugTrace::dumpObjRef

// SIGV signal handler
static void
segv_handler(int code_) {
  debugTrace->debugStr(DTContext(DTCCSEGV), "SEGV signal received.");
} // segv_handler

DebugTrace::
DebugTrace() :
  mASMDesc(0),
  mCurrThisPointer(0),
  mOutputToCOut(false),
  mParamCount(0),
  mIndent(0),
  mSkipFuncStackSize(-1),
  mFirstShowOnlyStackSize(-1),
  mSkipChildCallStackSize(-1),
  mCurrCGCxtParamPtr(0)
{
  if (DEBUGBUILD) {
    //! \attention DTConfig::LoadFromFile will set the console output mode if
    //!            specified in the config file.
    mOutputFile = new ofstream("/tmp/bcf_debugtrace.log", ios_base::out | ios_base::trunc);
    // Do not use Debug Trace or any other function that calls it, e.g. DBC.
    DTASSERT(smRefCount == 0, "DebugTrace is run-time singleton, may not be created more than one");
    signal(SIGSEGV, segv_handler);
    DTLOG("DebugTrace started" << dtlIndent << dtlEndl);
    *mOutputFile << "DebugTrace started" << endl;
    //! \todo M Design: provide a command line read file name for configuration.
    ++ smRefCount;
    // Construct HTML output header.
    mHTMLOutput.mTopNode = new tagHTML("html");
    HTMLNode::Add("html", new tagHEADER("header"));
    HTMLNode::Add("html", new tagBODY("body"));
    HTMLNode::Add("header", new tagTITLE("title", "BCC version 0.0.1 Debug/Trace output"));
    HTMLNode::Add("body", new tagH1("", "All Execution Point Dumps"));
    HTMLNode::Add("body", new tagLIST("progDumps"));
    HTMLNode::Add("body", new tagH1("", "Execution Point Dumps"));
    HTMLNode::Add("body", new tagLIST("exePointDumps"));
    HTMLNode::Add("body", new tagH1("", "Function Dumps"));
    HTMLNode::Add("body", new tagLIST("funcDumps"));
    // Call toStr to initialize command string maps.
    toStr(DTCCDumpParam);
  } // if
} // DebugTrace::DebugTrace

DebugTrace::
~DebugTrace() {
  if (DEBUGBUILD) {
    dumpStats();
    DTLOG(dtlUnindent << dtlEndl << "DebugTrace is dead now" << dtlEndl);
    *mOutputFile << "DebugTrace is dead now" << endl;
    ofstream lHTMLOutput("/tmp/dtindex.html");
    mHTMLOutput.mTopNode->Write(lHTMLOutput);
  } // if
} // DebugTrace::DebugTrace

bool DebugTrace::
IsValid() const {
  return true;
} // DebugTrace::IsValid

void DebugTrace::
pdebug(const string& category_, const string& subCategory_, const string& msg_) {
  if (!mDTStack.empty()) {
    if (!subCategory_.length()) {
      if (mDTStack.back().mTrace == true) {
        *mOutputFile << category_ << ":" << msg_.c_str() << endl;
        checkAndDumpProg(category_ + "::-", dtConfig[DTCVDumpProg], "at the execution point");
      } // if
    } else {
      if (mDTStack.back().mTrace == true) {
        *mOutputFile << category_ << "::" << subCategory_ << ":" << msg_.c_str() << endl;
        checkAndDumpProg(category_ + "::" + subCategory_, dtConfig[DTCVDumpProg], "at the execution point");
      } // if
    } // if
  } // if
  return;
} // DebugTrace::pdebug

void 
DEBUG1Func(const string& exePoint_, const IDebuggable* this_, const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, 0);
} // DEBUG1Func

void 
DEBUG2Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, 0);
} // DEBUG2Func

void 
DEBUG3Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, const Debuggable& d1_, 
  const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, 0);
} // DEBUG3Func

void 
DEBUG4Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, 
  const Debuggable& d1_, const Debuggable& d2_, const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, &d2_, 0);
} // DEBUG4Func

void 
DEBUG5Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, 
  const Debuggable& d1_, const Debuggable& d2_, const Debuggable& d3_, const char* file_, 
  hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, &d2_, &d3_, 0);
} // DEBUG5Func

void 
DEBUG6Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, 
  const Debuggable& d1_, const Debuggable& d2_, const Debuggable& d3_, const Debuggable& d4_, 
  const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, &d2_, &d3_, &d4_, 0);
} // DEBUG6Func

void 
DEBUG7Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, 
  const Debuggable& d1_, const Debuggable& d2_, const Debuggable& d3_, const Debuggable& d4_, 
  const Debuggable& d5_, const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, &d2_, &d3_, &d4_, &d5_, 0);
} // DEBUG7Func

void 
DEBUG8Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, 
  const Debuggable& d1_, const Debuggable& d2_, const Debuggable& d3_, const Debuggable& d4_, 
  const Debuggable& d5_, const Debuggable& d6_, const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, &d2_, &d3_, &d4_, &d5_, &d6_, 0);
} // BMDEBUG8Func

void 
DEBUG9Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, 
  const Debuggable& d1_, const Debuggable& d2_, const Debuggable& d3_, const Debuggable& d4_, 
  const Debuggable& d5_, const Debuggable& d6_, const Debuggable& d7_, 
  const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, &d2_, &d3_, &d4_, &d5_, &d6_, &d7_, 0);
} // BMDEBUG9Func

void 
DEBUG10Func(const string& exePoint_, const IDebuggable* this_, const Debuggable& d0_, 
  const Debuggable& d1_, const Debuggable& d2_, const Debuggable& d3_, const Debuggable& d4_, 
  const Debuggable& d5_, const Debuggable& d6_, const Debuggable& d7_, const Debuggable& d8_, 
  const char* file_, hFUInt32 line_) {
  DEBUGFunc(exePoint_, this_, file_, line_, &d0_, &d1_, &d2_, &d3_, &d4_, &d5_, &d6_, &d7_, &d8_, 0);
} // BMDEBUG10Func

void 
DEBUGFunc(const string& exePoint_, const IDebuggable* this_, const char* file_, hFUInt32 line_, ...) {
  if (dtEnabled == true) {
    if (debugTrace != 0 && debugTrace->inDebugTraceCall() == false) {
      va_list lAP;
      va_start(lAP, line_);
      ostrstream lParamText;
      bool lFirstParam(true);
      for (Debuggable* lDebuggable(va_arg(lAP, Debuggable*)); lDebuggable != 0; lDebuggable = va_arg(lAP, Debuggable*)) {
        if (!lFirstParam) {
          lParamText << ", ";
        } else {
          lFirstParam = false;
        } // if
        if (lDebuggable->GetObj() != 0) {
          debugTrace->SetCGFuncContextParam(dynamic_cast<const CGFuncContext*>(lDebuggable->GetObj()));
          lDebuggable->GetObj()->debug(DTCCDumpParam, lParamText);
        } else if (lDebuggable->IsString()) {
          lParamText << lDebuggable->GetStr();
        } else {
          ASSERTMSG(0, ieStrPCShouldNotReachHere);
        } // if
      } // for
      lParamText << ends;
      if (exePoint_ == "$end$") {
        debugTrace->endCurrFuncTrace(SrcLoc(file_, line_, 0), this_, lParamText.str());
      } else {
        debugTrace->beginFuncTrace(SrcLoc(file_, line_, 0), exePoint_, this_, lParamText.str());
      } // if
      va_end(lAP);
    } // if
  } // if
} // DEBUGFunc

//! \param [in] exePoint_ An execution point string name, can be a single "identifier" or C++ "class::member".
//! \param [out] className_ class name part of execution point, if not present it is "".
//! \param [out] funcName_ function name part of execution point, if "::" not present the whole exePoint.
static bool
parseExePoint(const string& exePoint_, string& className_, string& funcName_) {
  DTLOG("parseExePoint(" << exePoint_.c_str() << ") {" << dtlIndent << dtlEndl);
  string::size_type lIndex = exePoint_.find_first_of("::");
  if (lIndex == string::npos) {
    funcName_ = exePoint_;
    className_ = "";
  } else {
    className_ = exePoint_.substr(0, lIndex);
    funcName_ = exePoint_.substr(lIndex+2);
  } // if
  DTLOG(dtlUnindent << dtlEndl <<
    "} // parseExePoint: <" << className_.c_str() << ", " << funcName_.c_str() << ">" << dtlEndl);
  return true;
} // parseExePoint

static bool
anyMatch(const string& exePoint_, const set<string>& exePointPatterns_) {
  DTLOG("anyMatch(" << exePoint_.c_str() << ") {" << dtlIndent << dtlEndl);
  // Keep the previous results, there is no need to evaluate them again and again.
  static map<pair<string, const set<string>*>, bool> prevResults;
  string epFuncName;
  string epClassName;
  bool isMatch(false);
  if (prevResults.find(make_pair(exePoint_, &exePointPatterns_)) != prevResults.end()) {
    isMatch = prevResults[make_pair(exePoint_, &exePointPatterns_)];
  } else {
    parseExePoint(exePoint_, epClassName, epFuncName);
    set<string>::const_iterator lIt(exePointPatterns_.begin());
    while (lIt != exePointPatterns_.end()) {
      string lPattern(*lIt++);
      string lPatFuncName;
      string lPatClassName;
      parseExePoint(lPattern, lPatClassName, lPatFuncName);
      if (lPatClassName == "*") {
        // e.g. "*::IsValid"
        if (epClassName != "" && epFuncName == lPatFuncName) {
          isMatch = true;
          break;
        } // if
      } else if (lPatFuncName == "*") {
        // e.g. "Predicate::*"
        if (epClassName != "" && epClassName == lPatClassName) {
          isMatch = true;
          break;
        } // if
      } else if (lPatClassName == "") {
        // e.g. "globfunc"
        if (epClassName == "" && epFuncName == lPatFuncName) {
          isMatch = true;
          break;
        } // if
      } else {
        // e.g. "class::func"
        if (epClassName != "" && epFuncName == lPatFuncName && epClassName == lPatClassName) {
          isMatch = true;
          break;
        } // if
      } // if
    } // while
    prevResults[make_pair(exePoint_, &exePointPatterns_)] = isMatch;
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // anyMatch: " << isMatch << dtlEndl);
  return isMatch;
} // anyMatch

static bool 
anyMatch(const string& exePoint_, const string& str_) {
  // We need to be ugly to conform the requirements of anyMatch.
  static map<string, set<string>*> exePoints_;
  if (!exePoints_[str_]) {
    exePoints_[str_] = new set<string>();
    exePoints_[str_]->insert(str_);
  } // if
  return anyMatch(exePoint_, *exePoints_[str_]);
} // anyMatch

//! \brief this is the main output function, every type is
//! converted to string and here it will be output to \a mOutputFile.
//! \todo M Design: I think it is better to handle function begin and end cases separately.
DebugTrace& DebugTrace::
debugStr(const DTContext& context_, const string& str_) {
  DTLOG("DebugTrace::debugStr(" << dtToStr(context_) << ", " << str_.c_str() << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCAbort || context_ == DTCCSEGV) {
    *mOutputFile << str_.c_str() << flush << endl;
    cout << str_.c_str() << flush << endl;
    DTLOG(str_.c_str() << dtlEndl);
    ofstream lHTMLOutput("/tmp/dtindex.html");
    mHTMLOutput.mTopNode->Write(lHTMLOutput);
    DumpCallStack();
    //! \todo M Design: Take care of compiler exit codes.
    exit(1);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DebugTrace::debug" << dtlEndl);
  return *this;
} // DebugTrace::debugStr

bool DebugTrace::
checkAndDumpHWDesc(const string& exePoint_, const set<string>& exePoints_, const string& where_) {
  DTLOG("DebugTrace::checkAndDumpHWDesc(" << exePoint_.c_str() << ") {" << dtlIndent << dtlEndl);
  hFSInt32 retVal(-1);
  //! \attention assume that dumpProgExePoints are fully qualified.
  bool lIsValidThisPointer(dynamic_cast<const HWDescription*>(mCurrThisPointer));
  if (lIsValidThisPointer == true && anyMatch(exePoint_, exePoints_) == true) {
    *mOutputFile << "HWDescription dump; " << where_.c_str() << " hc#" <<
      mHitCount[exePoint_] << ": " << exePoint_.c_str() << DTCCEndLine;
    *mOutputFile << hwdesccxt(mCurrThisPointer) << DTCCEndLine;
    retVal = 1;
  } else {
    retVal = 0;
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DebugTrace::checkAndDumpHWDesc: " << retVal << dtlEndl);
  return retVal;
} // DebugTrace::checkAndDumpHWDesc

bool DebugTrace::
checkAndDumpStats(const string& exePoint_, const set<string>& exePoints_, const string& where_) {
  DTLOG("DebugTrace::checkAndDumpStats(" << exePoint_.c_str() << ") {" << dtlIndent << dtlEndl);
  hFSInt32 retVal(-1);
  if (anyMatch(exePoint_, exePoints_)) {
    stats->debug(DTCCDumpStats, *mOutputFile);
    retVal = 1;
  } else {
    retVal = 0;
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DebugTrace::checkAndDumpProg: " << retVal << dtlEndl);
  return retVal;
} // DebugTrace::checkAndDumpProg

bool DebugTrace::
checkAndDumpProg(const string& exePoint_, const set<string>& exePoints_, const string& where_) {
  DTLOG("DebugTrace::checkAndDumpProg(" << exePoint_.c_str() << ") {" << dtlIndent << dtlEndl);
  hFSInt32 retVal(-1);
  if (anyMatch(exePoint_, exePoints_)) {
    ostrstream lTitle;
    lTitle << "IRProgram dump; " << where_.c_str() << " hc#" << mHitCount[exePoint_] << ": " << exePoint_.c_str() << endl << ends;
    if (dtConfig == DTCVDumpCFGGraphs) {
      HTMLNode::Add("progDumps", new tagLISTITEM("progDump"));
      HTMLNode::Add("exePointDumps", new tagH2("exePointDumpTitle", lTitle.str()));
      HTMLNode::Add("exePointDumps", new tagLIST("exePointDump"));
      HTMLNode::Add("progDump", new tagLINK("", HTMLNode::Get("exePointDumpTitle"), lTitle.str()));
    } // if
    *mOutputFile << lTitle.str();
    DumpCallStack();
    *mOutputFile << progcxt(irProgram);
    retVal = 1;
  } else {
    retVal = 0;
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DebugTrace::checkAndDumpProg: " << retVal << dtlEndl);
  return retVal;
} // DebugTrace::checkAndDumpProg

void DebugTrace::
beginFuncTrace(const SrcLoc& srcLoc_, const string& exePoint_, const IDebuggable* this_, const string& paramText_) {
  DTLOG("DebugTrace::beginFuncTrace " << exePoint_ << " this=" << hFUInt32(this_));
  DTASSERT(mDTStack.size() < dtConfig.GetMaxCallDepth(),
    "Maximum call depth reached in DebugTrace, increase it from config file(or a bug?)");
  ++ mHitCount[exePoint_];
  bool lTraceOutput(true);
  static bool lFirstCall(true);
  static bool lHasSkipFuncs;
  static bool lHasSkipChildCalls;
  bool lShowOnlyMatched(false);
  if (lFirstCall == true) {
    lHasSkipFuncs = dtConfig.HasSkipFuncs();
    lHasSkipChildCalls = dtConfig.HasSkipChildCalls();
    lFirstCall = false;
  } // if
  // Note that order of checks is important do not change or insert without a second thought.
  if (dtContext->IsInDumpContext() == true) {
    lTraceOutput = false;
  } else if (*dtContext == DTCCDBC && dtConfig != DTCVShowDBC) {
    lTraceOutput = false;
  } else if (dtConfig.HasShowOnly() == true) {
    if (anyMatch(exePoint_, dtConfig[DTCVShowOnly]) == true) {
      lTraceOutput = true;
      lShowOnlyMatched = true;
      if (mFirstShowOnlyStackSize == -1) {
        mFirstShowOnlyStackSize = mDTStack.size();
      } // if
      goto handle_skip_child_calls;
    } else {
      if (mFirstShowOnlyStackSize >= mDTStack.size()) {
        lTraceOutput = false;
      } else {
        handle_skip_cases: 
        // Handle skip funcs and child calls case.
        if ((mSkipFuncStackSize == -1) && lHasSkipFuncs == true && anyMatch(exePoint_, dtConfig[DTCVSkipFuncs]) == true) {
          lTraceOutput = lShowOnlyMatched;
          mSkipFuncStackSize = mDTStack.size();
        } else if (mSkipFuncStackSize <= mDTStack.size()) {
          lTraceOutput = lShowOnlyMatched;
        } // if
        handle_skip_child_calls: 
        if ((mSkipChildCallStackSize == -1) && lHasSkipChildCalls == true && anyMatch(exePoint_, dtConfig[DTCVSkipChildCalls]) == true) {
          lTraceOutput = true;
          mSkipChildCallStackSize = mDTStack.size();
        } else if (mSkipChildCallStackSize <= mDTStack.size()) {
          lTraceOutput = lShowOnlyMatched;
        } // if
      } // if
    } // if
  } else {
    // No has show only functions case.
    goto handle_skip_cases;
  } // if
  mDTStack.push_back(DTSTACK(exePoint_, this_, lTraceOutput));
  if (lTraceOutput == true) {
    indent(mIndent ++);
    *mOutputFile << exePoint_.c_str() << "#" << mHitCount[exePoint_];
    if (this_ != 0) { // dump this
      *mOutputFile << " this[";
      this_->debug(DTCCDumpThis, *mOutputFile);
      *mOutputFile << "]";
    } // if
    if (dtConfig == DTCVShowFileLoc) {
      *mOutputFile << " <@" << srcLoc_.GetFileName().c_str() << ":" << srcLoc_.GetLineNum() << "> ";
    } // if
    *mOutputFile << "(" << paramText_.c_str() << ") {" << endl;
    checkAndDumpProg(exePoint_, dtConfig[DTCVDumpProg], "at execution point");
    checkAndDumpHWDesc(exePoint_, dtConfig[DTCVDumpHWDesc], "at execution point");
    if (mCurrCGCxtParamPtr != 0 && anyMatch(exePoint_, "HWDescription::coverStmt") == true) {
      mCurrCGCxtParamPtr->debug(DTCCDumpCGCxt, *mOutputFile);
    } // if
    if (dynamic_cast<const RegisterAllocator*>(this_) != 0 && anyMatch(exePoint_, "*::Start") == true) {
      *mOutputFile << progcxt(this_);
    } // if
  } // if
  return;
} // DebugTrace::beginFuncTrace

void DebugTrace::
endCurrFuncTrace(const SrcLoc& srcLoc_, const IDebuggable* this_, const string& paramText_) {
  DTLOG("DebugTrace::endCurrFuncTrace " << " this=" << hFUInt32(this_));
  DTASSERT(!mDTStack.empty(), "DebugTrace command stack underflow, mismatch in beg/end of end of execution point commands");
  DTASSERT(this_ == mDTStack.back().mThis, "Mismatched B.DEBUG/E.DEBUG pairs");
  string lExePoint(mDTStack.back().mFuncName);
  bool lTraceOutput(mDTStack.back().mTrace);
  mDTStack.pop_back();
  if (mSkipChildCallStackSize != -1 && mSkipChildCallStackSize >= mDTStack.size()) {
    mSkipChildCallStackSize = -1;
  } // if
  if (mSkipFuncStackSize != -1 && mSkipFuncStackSize >= mDTStack.size()) {
    mSkipFuncStackSize = -1;
  } // if
  if (mFirstShowOnlyStackSize != -1 && mFirstShowOnlyStackSize >= mDTStack.size()) {
    mFirstShowOnlyStackSize = -1;
  } // if
  if (lTraceOutput == true) {
    if (mCurrCGCxtParamPtr != 0 && anyMatch(lExePoint, "HWDescription::coverStmt") == true) {
      mCurrCGCxtParamPtr->debug(DTCCDumpCGCxt, *mOutputFile);
    } // if
    indent(mIndent --);
    *mOutputFile << "return " << paramText_.c_str() << ";" << endl;
    indent(mIndent);
    *mOutputFile << "} // " << lExePoint.c_str() << endl;
    if (dynamic_cast<const RegisterAllocator*>(this_) != 0 && anyMatch(lExePoint, "*::Start") == true) {
      *mOutputFile << progcxt(this_);
    } // if
    if (anyMatch(lExePoint, "DFA::Do") == true) {
      this_->debug(DTCCDumpDFA, *mOutputFile);
    } // if
    DTLOG("Checking for DTCVDumpProgAtEnd" << dtlEndl);
    checkAndDumpProg(lExePoint, dtConfig[DTCVDumpProgAtEnd], "at the end of execution point");
    DTLOG("Checking for DTCVDumpStats" << dtlEndl);
    checkAndDumpStats(lExePoint, dtConfig[DTCVDumpStats], "at the end of execution point");
  } // if
  return;
} // DebugTrace::endCurrFuncTrace

void DebugTrace::
dumpStats() {
  DTLOG("DebugTrace::dumpStats {" << dtlIndent << dtlEndl);
  // Display total hit count.
  map<string, hFInt32>::iterator lHit(mHitCount.begin());
  hUInt32 lTotalHitCount(0);
  while (lHit != mHitCount.end()) {
    lTotalHitCount += lHit->second;
    ++ lHit;
  } // while
  *mOutputFile << "Total hit count: " << lTotalHitCount << endl;
  // Total number of debuggable objects
  *mOutputFile << "Total number of objects: " << IDebuggable::mObjIds << endl;
  DTLOG(dtlUnindent << dtlEndl << "} // DebugTrace::dumpStats" << dtlEndl);
  return;
} // DebugTrace::dumpStats

void DebugTrace::
DumpCallStack() {
  DTLOG("DebugTrace::DumpCallStack {" << dtlIndent << dtlEndl);
  *mOutputFile << "**** CALL STACK | hit# | ****" << endl;
  deque<DTSTACK>::iterator lIt(mDTStack.begin());
  hFInt32 lIndentAmount(0);
  while (lIt != mDTStack.end()) {
    const DTSTACK& lCall(*lIt);
    *mOutputFile << lCall.mFuncName.c_str() << "#" << mHitCount[lCall.mFuncName] << 
      " this=" << (hFUInt32)lCall.mThis << " trace=" << lCall.mTrace << endl;
    ++ lIndentAmount;
    ++ lIt;
    indent(lIndentAmount);
  } // while
  dumpStats();
  DTLOG(dtlUnindent << dtlEndl << "} // DebugTrace::DumpCallStack" << dtlEndl);
  return;
} // DebugTrace::DumpCallStack

void DebugTrace::
indent(hFInt32 amount_) {
  const hFInt8 lTabSize(2);
  for (hFInt16 c(0); c < lTabSize*amount_; ++ c) {
    *mOutputFile << " ";
  } // for
  return;
} // DebugTrace::indent

// ////////////////////////////////////////
// debugTrace implementation of objects
// ////////////////////////////////////////
char* Predicate::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Predicate::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Predicate::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Predicate::debug

char* Target::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Target::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Target::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Target::debug

char* TestTarget::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("TestTarget::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // TestTarget::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // TestTarget::debug

char* Extract::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Extract::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Extract::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Extract::debug

char* BigInt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("BigInt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    toStr_ << mValue;
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // BigInt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // BigInt::debug

char* BigReal::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("BigReal::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // BigReal::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // BigReal::debug

char* tInt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("tInt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // tInt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // tInt::debug

char* tReal::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("tReal::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // tReal::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // tReal::debug

char* IRBuilder::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRBuilder::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRBuilder::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRBuilder::debug

char* IRBuilderHelper::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRBuilderHelper::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRBuilderHelper::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRBuilderHelper::debug

char* IRType::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRType::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRType::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRType::debug

char* IRTFunc::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTFunc::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    mRetType->debug(context_, toStr_);
    toStr_ << "(*)(";
    for (hFUInt16 c(0); c < mParamTypes.size(); ++ c) {
      toStr_ << (c != 0 ? ", ":"");
      mParamTypes[c]->debug(context_, toStr_);
    } // for
    if (mIsVarArg == true) {
      toStr_ << (!mParamTypes.size() ? "..." : ", ...");
    } // if
    toStr_ << ")";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTFunc::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTFunc::debug

char* IRTArray::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTArray::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << "cnt:[" << mCount << "] ";
    GetElemType()->debug(context_, toStr_);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTArray::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTArray::debug

char* IRTPtr::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTPtr::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(GetRefType()) << "*";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTPtr::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTPtr::debug

char* IRTBool::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTBool::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << "bool";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTBool::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTBool::debug

char* IRTInt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTInt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVFuncFmtC) {
      toStr_ << "int(" << GetSize() << ", " << GetAlignment() << ", " << mSign << ")";
    } else if (dtConfig == DTCVFuncFmtC) {
      toStr_ << (mSign == IRSUnsigned ? "unsigned " : "") << "int" << GetSize();
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTInt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTInt::debug

char* IRTVoid::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTVoid::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << "void";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTVoid::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTVoid::debug

char* IRTStruct::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTStruct::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << GetName() << " {" << ++ DTCCIndent << DTCCEndLine;
    for (hFInt32 c(0); c < mFields.size(); ++ c) {
      if (!pred.pIsBitField(mFields[c])) {
        toStr_ << DTCCIndent << refcxt(mFields[c]->GetType()) << " " << mFields[c]->GetName() << ";" << DTCCEndLine;
      } else {
        IROBitField* lBitField(static_cast<IROBitField*>(mFields[c]));
        toStr_ << DTCCIndent << lBitField->GetSign() << " " << refcxt(lBitField->GetType()) << " " << 
          lBitField->GetName() << ":" << lBitField->GetBitSize() << " @" << 
          lBitField->GetOffset() << "+" << lBitField->GetBitOffset() << ";" << DTCCEndLine;
      } // if
    } // for
    toStr_ << --DTCCIndent << DTCCIndent << "}";
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTStruct::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTStruct::debug

char* IRTUnion::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTUnion::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTUnion::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTUnion::debug

char* IRTReal::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTReal::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (GetSize() == 32) {
      toStr_ << "real32";
    } else if (GetSize() == 64) {
      toStr_ << "real64";
    } else {
      toStr_ << "real" << GetSize();
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTReal::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTReal::debug

char* ICBB::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ICBB::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpParam) {
    toStr_ << refcxt(this) << "{" << refcxt(mBB) << ", " << mPlace << "}";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ICBB::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ICBB::debug

char* ICNull::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ICNull::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpParam) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ICNull::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ICNull::debug

char* ICStmt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ICStmt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpParam) {
    toStr_ << refcxt(this) << "{" << refcxt(mStmt) << ", " << mPlace << "}";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ICStmt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ICStmt::debug

char* IRExpr::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRExpr::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRExpr::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRExpr::debug

char* IRExprList::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRExprList::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtMultiLine) {
      toStr_ << refcxt(this) << (mExprList.empty() == true ? "<empty>" : "");
      if (!mExprList.empty()) {
        toStr_ << ++ DTCCIndent << DTCCEndLine;
        for (hFUInt16 c(0); c < mExprList.size(); ++ c) {
          toStr_ << DTCCIndent << exprcxt(mExprList[c]);
          if (c != mExprList.size()-1) {
            toStr_ << DTCCEndLine;
          } // if
        } // for
        toStr_ << -- DTCCIndent;
      } // if
    } else {
      for (hFUInt16 c(0); c < mExprList.size(); ++ c) {
        toStr_ << (c != 0 ? ", " : "");
        toStr_ << exprcxt(mExprList[c]);
      } // for
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRExprList::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRExprList::debug

char* IRStmt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRStmt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    if (dtConfig == DTCVStmtExprFmtC) {
      // LIE
    } else {
      const SrcLoc& lSrcLoc(extr.eGetSrcLoc(this));
      toStr_ << refcxt(&lSrcLoc);
      toStr_ << "(BB:" << refcxt(mParentBB) << ", P:" << refcxt(mPrev) << ", N:" << refcxt(mNext) << ")";
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRStmt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRStmt::debug

//! \todo M Design: In the reference dump output "entry, exit" information also.
char* IRBB::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRBB::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    string lSuppInfo("");
    if (mParentCFG != 0 && mParentCFG->mExitBB == this) {
      lSuppInfo = "[exitBB]";
    } else if (mParentCFG != 0 && mParentCFG->mEntryBB == this) {
      lSuppInfo = "[entryBB]";
    } // if
    if (context_ != DTCCDumpProg) {
      debugTrace->dumpObjRef(this, context_, toStr_);
      toStr_ << lSuppInfo;
    } // if
    if (context_ == DTCCDumpProg) {
      if (lSuppInfo == "") {
        debugTrace->dumpObjRef(this, context_, toStr_);
      } else {
        toStr_ << lSuppInfo;
      } // if
    } // if
  } else if (context_ == DTCCDumpProg) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << DTCCIndent << GetObjId() << "D: ";
    } // if
    if (context_ != DTCCDumpDFA && dtConfig == DTCVDumpCFGGraphs && HTMLNode::Get("exePointDumps")->GetChildCount() != 0) {
      ostream& o(DTContext::GetCFGGraph());
      for (hFUInt32 c(0); c < mSuccs.size(); ++ c) {
        o << "\t\"" << refcxt(this) << "\" -> " << "\"" << refcxt(mSuccs[c]) << "\"" << endl; 
      } // for
      o << "\t\"" << refcxt(this) << "\" [label=\"" << refcxt(this) << "\\n";
      for (list<IRStmt*>::const_iterator lSit(mStmts.begin()); lSit != mStmts.end(); ++ lSit) {
        DTContext::GetCFGGraph() << graphcxt(*lSit) << "\\n";
      } // for
      o << "\"]" << endl;
    } // if
    if (context_ != DTCCDumpDFA && dtConfig == DTCVFuncFmtC) {
      // Dump statements only.
      if (mParentCFG->mEntryBB != this) {
        toStr_ << DTCCEndLine;
      } // if
      toStr_ << DTCCIndent << refcxt(this) << ":" << DTCCEndLine;
      for (list<IRStmt*>::const_iterator lSit(mStmts.begin()); lSit != mStmts.end(); ++ lSit) {
        toStr_ << DTCCIndent << progcxt(*lSit) << DTCCEndLine;
      } // for
      CGBB* lCGBB(const_cast<CGBB*>(static_cast<const CGBB*>(this)));
      if (!lCGBB->mInsts.size()) {
        toStr_ << DTCCIndent;
        toStr_ << "Insts: <none>" << DTCCEndLine;
      } else {
        toStr_ << DTCCIndent << "Insts: {" << ++ DTCCIndent << DTCCEndLine;
        for (vector<AsmSequenceItem*>::iterator lIt(lCGBB->mInsts.begin()); lIt != lCGBB->mInsts.end(); ++ lIt) {
          toStr_ << DTCCIndent;
          ASMSection lASMSection(&toStr_);
          (*lIt)->GetDisassembly(lASMSection, *debugTrace->GetASMDesc());
          toStr_ << DTCCEndLine;
        } // for
        toStr_ << -- DTCCIndent << DTCCIndent << "} // Insts" << DTCCEndLine;
      } // if
    } else {
      toStr_ << refcxt(this) << " ";
      toStr_ << " (" << refcxt(GetCFG()) << ", prev=";
      if (pred.pIsEntryBB(this) == true) {
        toStr_ << "0";
      } else {
        toStr_ << refcxt(GetPrev());
      } // if
      toStr_ << ", next=";
      if (pred.pIsExitBB(this) == true) {
        toStr_ << "0";
      } else {
        toStr_ << refcxt(GetNext());
      } // if
      toStr_ << ") {" << ++ DTCCIndent << DTCCEndLine;
      // Dump predecessor references.
      toStr_ << DTCCIndent << "Preds: " << (mPreds.size() == 0 ? "None" : "{");
      for (hFUInt32 c(0); c < mPreds.size(); ++ c) {
        if (c != 0) {
          toStr_ << ", ";
        } // if
        toStr_ << refcxt(mPreds[c]);
      } // for
      toStr_ << (mPreds.size() == 0 ? "" : "}") << DTCCEndLine;
      // Dump successor references.
      toStr_ << DTCCIndent << "Succs: " << (mSuccs.size() == 0 ? "None" : "{");
      for (hFUInt32 d(0); d < mSuccs.size(); ++ d) {
        if (d != 0) {
          toStr_ << ", ";
        } // if
        toStr_ << refcxt(mSuccs[d]);
      } // for
      toStr_ << (mSuccs.size() == 0 ? "" : "}") << DTCCEndLine;
      // Dump statements.
      if (mStmts.size() == 0) {
        toStr_ << DTCCIndent << "Stmts: None";
      } else {
        toStr_ << DTCCIndent << "Stmts: {" << ++ DTCCIndent << DTCCEndLine;
        for (list<IRStmt*>::const_iterator lIt(mStmts.begin()); lIt != mStmts.end(); ++ lIt) {
          toStr_ << DTCCIndent << progcxt(*lIt) << DTCCEndLine;
        } // for
        toStr_ << -- DTCCIndent << DTCCIndent << "} // Stmts" << DTCCEndLine;
      } // if
      // Dump instructions
      const CGBB* lThisCGBB(static_cast<const CGBB*>(this));
      if (lThisCGBB->mInsts.size() == 0) {
        toStr_ << DTCCIndent << "Insts: None" << DTCCEndLine;
      } else {
        toStr_ << DTCCIndent << "Insts: {" << ++ DTCCIndent << DTCCEndLine;
        for (hFUInt32 c(0); c < lThisCGBB->mInsts.size(); ++ c) {
          toStr_ << DTCCIndent << refcxt(lThisCGBB->mInsts[c]) << " ";
          ASMSection lASMSection(&toStr_);
          lThisCGBB->mInsts[c]->GetDisassembly(lASMSection, *debugTrace->GetASMDesc());
          toStr_ << DTCCEndLine;
        } // for
        toStr_ << -- DTCCIndent << DTCCIndent << "} // Insts" << DTCCEndLine;
      } // if
      if (GetCFG()->mDomTree != 0) { // Dump dominators
        set<IRBB*>::const_iterator lIt(mDoms.begin());
        toStr_ << DTCCIndent << "Dominators: ";
        while (lIt != mDoms.end()) {
          if (lIt != mDoms.begin()) {
            toStr_ << ", ";
          } // if
          toStr_ << refcxt(*lIt);
          ++ lIt;
        } // while
        toStr_ << DTCCEndLine;
      } // if
      toStr_ << -- DTCCIndent << DTCCIndent << "} // IRBB" << DTCCEndLine;
    } // if
  } else {
    toStr_ << refcxt(this);
  } //if
  DTLOG(dtlUnindent << dtlEndl << "} // IRBB::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRBB::debug

char* CFG::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("CFG::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    // Dump the basic blocks in physical order starting from Entry.
    if (dtConfig != DTCVFuncFmtC) {
      toStr_ << DTCCIndent << refcxt(this) << " (" << refcxt(GetEntryBB()) << ", " << refcxt(GetExitBB()) <<
        ") {" << ++ DTCCIndent << DTCCEndLine;
      { // dump the loops
        if (!mLoops) {
          toStr_ << DTCCIndent << "// Loops are not calculated." << DTCCEndLine;
        } else {
          for (hFUInt32 c(0); c < mLoops->size(); ++ c) {
            toStr_ << progcxt((*mLoops)[c]);
          } // for
        } // if
      } // block
      { // dump definition use chains
        if (!mDUUDChains) {
          toStr_ << DTCCIndent << "// DU/UD chains are not calculated." << DTCCEndLine;
        } else {
          toStr_ << progcxt(mDUUDChains) << DTCCEndLine;
        } // if
      } // block
      //! \todo M Design: list all of basic blocks?
      IRBB* lBB(GetEntryBB());
      do {
        toStr_ << DTCCIndent << progcxt(lBB);
        if (pred.pIsExitBB(lBB)) {
          break;
        } // if
        toStr_ << DTCCEndLine;
        lBB = lBB->GetNext();
      } while (lBB != 0);
      toStr_ << -- DTCCIndent << DTCCIndent << "} // CFG" << DTCCEndLine;
    } else {
      toStr_ << DTCCIndent << "// Loops and DUUD chains not displayed in C/C++ format dumps." << DTCCEndLine;
      IRBB* lBB(GetEntryBB());
      do {
        toStr_ << progcxt(lBB);
        if (pred.pIsExitBB(lBB)) {
          break;
        } // if
        lBB = lBB->GetNext();
      } while (lBB != 0);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // CFG::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // CFG::debug

char* IRFunctionDecl::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRFunctionDecl::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
    toStr_ << mName.c_str() << " (" << refcxt(GetType()) << ")";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRFunctionDecl::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRFunctionDecl::debug

char* IRFunction::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRFunction::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  DTContext lDTContext(context_);
  ofstream* lCFGGraphOutput(0);
  if (lDTContext == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, lDTContext, toStr_);
  } else if (lDTContext == DTCCDumpProg) {
    if (dtConfig == DTCVDumpCFGGraphs && HTMLNode::Get("exePointDumps")->GetChildCount() != 0) {
      static hFInt32 slCFGGraphIndex;
      char lFileNameDot[256];
      char lFileNamePNG[256];
      sprintf(lFileNameDot, "/tmp/cfg%d.dot", slCFGGraphIndex);
      sprintf(lFileNamePNG, "/tmp/cfg%d.dot.png", slCFGGraphIndex++);
      lCFGGraphOutput = new ofstream(lFileNameDot);
      DTContext::SetCFGGraph(lCFGGraphOutput);
      HTMLNode::Add("exePointDump", new tagLISTITEM("currFuncLI"));
      HTMLNode::Add("funcDumps", new tagH2("funcDumpTitle", mDeclaration->mName));
      HTMLNode::Add("funcDumps", new tagH3("", HTMLNode::Get("exePointDumpTitle")->GetData()));
      HTMLNode::Add("funcDumps", new tagLIST("funcDumpLIST"));
      HTMLNode::Add("funcDumps", new tagIMG("", lFileNamePNG));
      HTMLNode::Add("funcDumps", new tagHR(""));
      HTMLNode::Add("currFuncLI", new tagLINK("", HTMLNode::Get("funcDumpTitle"), mDeclaration->mName));
      *lCFGGraphOutput << "digraph {";
    } // if
    if (dtConfig != DTCVFuncFmtC) {
      toStr_ << DTCCIndent << mDeclaration->mName.c_str() << "(";
    } else {
      toStr_ << DTCCIndent << "IRFunction " << refcxt(extr.eGetRetType(this)) << " " << 
        mDeclaration->mName.c_str() << "(";
    } // if
    if (!mDeclaration->mParams.size()) {
      toStr_ << "void";
    } else {
      for (hFUInt32 c(0); c < mDeclaration->mParams.size(); ++ c) {
        toStr_ << (c != 0 ? ", " : "");
        toStr_ << progcxt(mDeclaration->mParams[c]);
      } // for
    } // if
    toStr_ << ") {" << ++ DTCCIndent << DTCCEndLine;
    if (dtConfig != DTCVFuncFmtC) {
      toStr_ << DTCCIndent << "Declaration: " << progcxt(mDeclaration) << DTCCEndLine;
      toStr_ << DTCCIndent << "SrcLoc: " << extr.eGetSrcLoc(this) << DTCCEndLine;
      if (!mLocals.size()) {
        toStr_ << DTCCIndent << "Locals: None" << DTCCEndLine;
      } // if
    } // if
    for (hFUInt16 c(0); c < mLocals.size(); ++ c) {
      toStr_ << DTCCIndent << progcxt(mLocals[c]) << DTCCEndLine;
    } // for
    if (mLocals.size() != 0) {
      toStr_ << DTCCEndLine; // separate declarations from statements
    } // if
    toStr_ << progcxt(mCFG);
    toStr_ << -- DTCCIndent << DTCCIndent << "} // IRFunction" << DTCCEndLine;
  } else {
    toStr_ << refcxt(this);
  } // if
  if (lCFGGraphOutput != 0) {
    *lCFGGraphOutput << "}";
    lCFGGraphOutput->close();
    DTContext::SetCFGGraph(0);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRFunction::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRFunction::debug

char* IRModule::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRModule::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << DTCCIndent << "IRModule {" << ++ DTCCIndent << DTCCEndLine;
    //! \todo dump the module name.
    if (!mGlobals.size()) {
      toStr_ << DTCCIndent << "Globals: None" << DTCCEndLine;
    } else {
      toStr_ << DTCCIndent << "Globals {" << ++ DTCCIndent << DTCCEndLine;
      for (hFUInt16 c(0); c < mGlobals.size(); ++ c) {
        toStr_ << DTCCIndent << mGlobals[c]->GetObjId() << "D: " << progcxt(mGlobals[c]) << DTCCEndLine;
      } // for
      toStr_ << -- DTCCIndent << DTCCIndent << "}" << DTCCEndLine;
    } // if
    if (mGlobals.size() != 0) {
      toStr_ << DTCCEndLine; // separate declarations from functions
    } // if
    if (!mFuncs.size()) {
      toStr_ << DTCCIndent << "Functions: None" << DTCCEndLine;
    } // if
    for (list<IRFunction*>::const_iterator lIt(mFuncs.begin()); lIt != mFuncs.end(); ++ lIt) {
      toStr_ << progcxt(*lIt);
    } // for
    toStr_ << -- DTCCIndent << DTCCIndent << "} // IRModule" << DTCCEndLine;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRModule::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRModule::debug

char* IRProgram::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRProgram::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << "IRProgram {" << ++ DTCCIndent << DTCCEndLine;
    { // dump the types.
      const vector<IRType*>& lTypes(IRBuilder::smGetTypes());
      if (!lTypes.empty()) {
        toStr_ << DTCCIndent << "Types {" << ++ DTCCIndent << DTCCEndLine;
        for (hFInt32 c(0); c < lTypes.size(); ++ c) {
          toStr_ << DTCCIndent << refcxt(lTypes[c]) << ": " << progcxt(lTypes[c]) << ";" << DTCCEndLine;
        } // for
        toStr_ << -- DTCCIndent << DTCCIndent << "} // Types" << DTCCEndLine;
      } else {
        toStr_ << "Types: none" << DTCCEndLine;
      } // if
    } // block
    if (!mModules.size()) {
      toStr_ << DTCCIndent << "Modules: None" << DTCCEndLine;
    } // if
    for (list<IRModule*>::const_iterator lIt(mModules.begin()); lIt != mModules.end(); ++ lIt) {
      toStr_ << progcxt(*lIt);
    } // for
    toStr_ << -- DTCCIndent << DTCCIndent << "} // IRProgram" << DTCCEndLine;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRProgram::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRProgram::debug

char* Anatrop::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Anatrop::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Anatrop::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Anatrop::debug

char* IRORelocSymbol::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRORelocSymbol::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpExpr) {
    toStr_ << GetName().c_str();
  } else if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << GetName().c_str() << " : " << refcxt(GetType()) << ";";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRORelocSymbol::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRORelocSymbol::debug

char* IRObject::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRObject::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpExpr) {
    if (dtConfig == DTCVStmtExprFmtMultiLine) {
      toStr_ << refcxt(this);
      toStr_ << " (" << refcxt(GetType()) << ") " << mName.c_str();
    } else {
      toStr_ << mName.c_str();
    } // if
  } else if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << mName.c_str() << " : " << refcxt(GetType()) << ";";
    toStr_ << " Addr: ";
    if (!pred.pIsAddrSet(this)) {
      toStr_ << "-";
    } else {
      toStr_ << progcxt(GetAddress());
    } // if
    toStr_ << ", " << getAddrTaken(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRObject::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRObject::debug

char* IROBitField::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IROBitField::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << GetName().c_str() << "<bf>";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IROBitField::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IROBitField::debug

char* IROField::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IROField::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    if (dtConfig == DTCVStmtExprFmtMultiLine) {
      toStr_ << refcxt(this);
      toStr_ << " (" << refcxt(GetType()) << ") " << GetName().c_str();
    } else {
      toStr_ << GetName().c_str();
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IROField::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IROField::debug

char* IROParameter::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IROParameter::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpExpr) {
    IRObject::debug(context_, toStr_);
  } else if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(GetType()) << " " << GetName().c_str() << "[";
    toStr_ << "Addr: ";
    if (!pred.pIsAddrSet(this)) {
      toStr_ << "-";
    } else {
      toStr_ << refcxt(GetAddress());
    } // if
    toStr_ << ", " << getAddrTaken(this);
    toStr_ << "]";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IROParameter::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IROParameter::debug

char* IROLocal::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IROLocal::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpExpr) {
    IRObject::debug(context_, toStr_);
  } else if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(GetType()) << " " << GetName().c_str() << ";";
    toStr_ << " Addr: ";
    if (!pred.pIsAddrSet(this)) {
      toStr_ << "-";
    } else {
      toStr_ << progcxt(GetAddress());
    } // if 
    toStr_ << ", " << getAddrTaken(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IROLocal::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IROLocal::debug

char* IROTmpGlobal::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IROTmpGlobal::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  IROGlobal::debug(context_, toStr_);
  DTLOG(dtlUnindent << dtlEndl << "} // IROTmpGlobal::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IROTmpGlobal::debug

char* IROGlobal::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IROGlobal::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpExpr) {
    IRObject::debug(context_, toStr_);
  } else if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    if (mLinkage == IRLStatic) {
      toStr_ << "static ";
    } else if (mLinkage == IRLImport) {
      toStr_ << "extern ";
    } else if (mLinkage == IRLExport) {
      /* LIE */
    } else {
      toStr_ << "?";
    } // if
    toStr_ << refcxt(GetType()) << " " << GetName().c_str() << ";" << ++ DTCCIndent << DTCCEndLine;
    toStr_ << DTCCIndent << "Inits: ";
    if (!pred.pHasInits(this)) {
      toStr_ << "None" << DTCCEndLine;
    } else {
      toStr_ << progcxt(mConstInits) << DTCCEndLine;
    } // if
    toStr_ << DTCCIndent << "Addr: ";
    if (!pred.pIsAddrSet(this)) {
      toStr_ << "-";
    } else {
      toStr_ << progcxt(GetAddress());
    } // if
    toStr_ << ", " << getAddrTaken(this);
    toStr_ << -- DTCCIndent;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IROGlobal::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IROGlobal::debug

char* IRSAssign::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSAssign::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  " << progcxt(mLHS) << " = " << progcxt(mRHS) << ";";
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(mLHS) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(mRHS) << -- DTCCIndent;
      } else {
        toStr_ << refcxt(this) << " " << progcxt(mLHS) << " = " << progcxt(mRHS) << ";";
        IRStmt::debug(context_, toStr_);
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSAssign::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSAssign::debug

char* IRSEval::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSEval::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  eval(" << exprcxt(mExpr) << ");";
    } else {
      toStr_ << refcxt(this);
      toStr_ << "eval(" << exprcxt(mExpr) << ");";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSEval::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSEval::debug

char* IRSLabel::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSLabel::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  const char* lAddrTakenStr = !mAddressTaken ? "" : "&&";
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpParam) {
    toStr_ << refcxt(this) << "{" << lAddrTakenStr << mName.c_str() << "}";
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << " " << lAddrTakenStr << mName.c_str() << ":";
    } else {
      toStr_ << refcxt(this) << " " << lAddrTakenStr << mName.c_str() << ":";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << lAddrTakenStr << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSLabel::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSLabel::debug

char* IRSBuiltInCall::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSBuiltInCall::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    toStr_ << getMark(this) << "  ";
    if (IsFunction()) {
      toStr_ << exprcxt(GetReturnIn()) << " = ";
    } // if
    toStr_ << ::toStr(mBIRId) << "(" << exprcxt(GetArgs()) << ");";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSBuiltInCall::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSBuiltInCall::debug

char* IRSFCall::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSFCall::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  " << exprcxt(GetReturnIn()) << " = " <<
        exprcxt(GetFuncExpr()) << "(" << exprcxt(GetArgs()) << ");";
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " funcexpr: (" << exprcxt(GetFuncExpr()) << ")" << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << "retin: " << exprcxt(GetReturnIn()) << DTCCEndLine;
        toStr_ << DTCCIndent << "args: " << exprcxt(GetArgs()) << -- DTCCIndent;
      } else {
        toStr_ << refcxt(this);
        toStr_ << exprcxt(GetReturnIn()) << " = " <<
          exprcxt(GetFuncExpr()) << "(" << exprcxt(GetArgs()) << ");";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSFCall::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSFCall::debug

char* IRSCall::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSCall::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    DTASSERT(0, ieStrPCShouldNotReachHere);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSCall::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSCall::debug

char* IRSPCall::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSPCall::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  " << exprcxt(GetFuncExpr()) << "(" << exprcxt(GetArgs()) << ");";
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << exprcxt(GetFuncExpr()) << ")" << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetArgs()) << -- DTCCIndent;
      } else {
        toStr_ << refcxt(this) << "  " << exprcxt(GetFuncExpr()) << "(" << exprcxt(GetArgs()) << ");";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSPCall::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSPCall::debug

char* IRSIf::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSIf::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  if (" << exprcxt(mCondExpr) << ") {jump " << refcxt(mTrueCaseBB) <<
        ";} else {jump " << refcxt(mFalseCaseBB) << ";}";
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(mCondExpr) << DTCCEndLine;
        toStr_ << DTCCIndent << refcxt(mTrueCaseBB) << DTCCEndLine;
        toStr_ << DTCCIndent << refcxt(mFalseCaseBB) << -- DTCCIndent;
        IRStmt::debug(context_, toStr_);
      } else {
        toStr_ << refcxt(this) << " if (" << exprcxt(mCondExpr) << ") {jump " << refcxt(mTrueCaseBB) <<
          "} else {jump " << refcxt(mFalseCaseBB) << "}";
        IRStmt::debug(context_, toStr_);
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSIf::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSIf::debug

char* IRSNull::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSNull::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "   ;";
    } else {
      toStr_ << refcxt(this) << "; // IRSNull";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSNull::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSNull::debug

char* IRSAfterLastOfBB::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSAfterLastOfBB::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    DTASSERT(0, "Internal Error");
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSAfterLastOfBB::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSAfterLastOfBB::debug

char* AddrDiff::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AddrDiff::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    DTASSERT(0, "Internal Error");
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AddrDiff::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AddrDiff::debug

char* AddrConstDiff::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AddrConstDiff::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    DTASSERT(0, "Internal Error");
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AddrConstDiff::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AddrConstDiff::debug

char* AddrLTConstDiff::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AddrLTConstDiff::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    DTASSERT(0, "Internal Error");
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AddrLTConstDiff::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AddrLTConstDiff::debug

char* DIEBuilder::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DIEBuilder::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    DTASSERT(0, "Internal Error");
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DIEBuilder::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DIEBuilder::debug

char* IRSBeforeFirstOfBB::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSBeforeFirstOfBB::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    DTASSERT(0, "Internal Error");
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSBeforeFirstOfBB::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSBeforeFirstOfBB::debug

char* IRSDynJump::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSDynJump::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  jump " << exprcxt(mTargetExpr) << ";";
    } else {
      toStr_ << refcxt(this) << " " << exprcxt(mTargetExpr) << ";";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSDynJump::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSDynJump::debug

char* IRSJump::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSJump::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  jump " << refcxt(mTargetBB) << ";";
    } else {
      toStr_ << refcxt(this) << " " << refcxt(mTargetBB) << ";";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSJump::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSJump::debug

char* IRSReturn::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSReturn::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << getMark(this) << "  return " << exprcxt(mRetExpr) << ";";
    } else {
      toStr_ << refcxt(this) << " " << exprcxt(mRetExpr) << ";";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSReturn::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSReturn::debug

char* IRSProlog::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSProlog::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << "  " << refcxt(this) << ";";
    } else {
      toStr_ << refcxt(this) << ";";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSProlog::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSProlog::debug

char* IRSEpilog::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSEpilog::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << "  " << refcxt(this) << ";";
    } else {
      toStr_ << refcxt(this) << ";";
      IRStmt::debug(context_, toStr_);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSEpilog::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSEpilog::debug

char* IRSSwitch::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRSSwitch::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  NOTIMPLEMENTED();
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << getMark(this);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig != DTCVDumpCFGGraphs) {
      toStr_ << GetObjId() << "D: ";
    } // if
    toStr_ << getMark(this) << "  switch (" << exprcxt(mValueExpr) << ") {" << DTCCEndLine << ++ DTCCIndent;
    for (hFUInt32 c(0); c < mCaseStmts.size(); ++ c) {
      toStr_ << DTCCIndent << "    case " << exprcxt(mCaseStmts[c].first) << ": " <<
        refcxt(mCaseStmts[c].second) << ";" << DTCCEndLine;
    } // for
    toStr_ << -- DTCCIndent << DTCCIndent << "  } // switch";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRSSwitch::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRSSwitch::debug

char* IREUnary::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREUnary::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREUnary::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREUnary::debug

char* IREBinary::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREBinary::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    DTASSERT(0, "Internal Error");
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREBinary::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREBinary::debug

char* IREAdd::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREAdd::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " + " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " + " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREAdd::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREAdd::debug

char* IRESub::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRESub::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " - " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " - " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRESub::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRESub::debug

char* IREDiv::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREDiv::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    // debugTrace->dumpObjRef(this, context_, toStr_);
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " / " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " / " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREDiv::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREDiv::debug

char* IREMul::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREMul::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " * " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " * " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREMul::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREMul::debug

char* IRERem::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRERem::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " REM " << exprcxt(GetRightExpr());
    } else {
      toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " REM " << exprcxt(GetRightExpr()) << "))";
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRERem::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRERem::debug

char* IREMod::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREMod::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " % " << exprcxt(GetRightExpr());
    } else {
      toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " % " << exprcxt(GetRightExpr()) << "))";
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREMod::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREMod::debug

char* IREQuo::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREQuo::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREQuo::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREQuo::debug

char* IREANeg::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREANeg::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      if (!pred.pIsLeafIR(GetExpr())) {
        toStr_ << "-(" << exprcxt(GetExpr()) << ")";
      } else {
        toStr_ << "-" << exprcxt(GetExpr());
      } // if
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")-(" << exprcxt(GetExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREANeg::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREANeg::debug

char* IREBNeg::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREBNeg::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      if (!pred.pIsLeafIR(GetExpr())) {
        toStr_ << "~(" << exprcxt(GetExpr()) << ")";
      } else {
        toStr_ << "~" << exprcxt(GetExpr());
      } // if
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")~(" << exprcxt(GetExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREBNeg::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREBNeg::debug

char* IRENot::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRENot::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      if (!pred.pIsLeafIR(GetExpr())) {
        toStr_ << "!(" << exprcxt(GetExpr()) << ")";
      } else {
        toStr_ << "!" << exprcxt(GetExpr());
      } // if
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")!(" << exprcxt(GetExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRENot::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRENot::debug

char* IREBAnd::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREBAnd::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " & " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " & " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREBAnd::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREBAnd::debug

char* IRELAnd::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRELAnd::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " && " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " && " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRELAnd::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRELAnd::debug

char* IREBXOr::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREBXOr::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " ^ " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " ^ " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREBXOr::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREBXOr::debug

char* IREBOr::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREBOr::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " | " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " | " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREBOr::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREBOr::debug

char* IRELOr::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRELOr::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " || " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " || " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRELOr::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRELOr::debug

char* IRECast::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRECast::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << "<" << refcxt(GetType()) << ">(" << exprcxt(GetExpr()) << ")";
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
         getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "<" << refcxt(GetType()) << ">(" << exprcxt(GetExpr()) << ")";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRECast::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRECast::debug

char* IRECmp::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRECmp::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRECmp::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRECmp::debug

char* IREGt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREGt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " > " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << refcxt(GetType()) << getMark(this) << dumpMatchInfo(context_, this) << 
          ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << 
          " > " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREGt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREGt::debug

char* IRELt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRELt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " < " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << refcxt(GetType()) << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " < " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRELt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRELt::debug

char* IREGe::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREGe::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " >= " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << refcxt(GetType()) << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " >= " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREGe::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREGe::debug

char* IRELe::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRELe::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " <= " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << refcxt(GetType()) << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " <= " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRELe::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRELe::debug

char* IREEq::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREEq::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " == " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << refcxt(GetType()) << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " == " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREEq::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREEq::debug

char* IRENe::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRENe::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) <<  " != " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << refcxt(GetType()) << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " != " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRENe::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRENe::debug

char* IREStrConst::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREStrConst::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << "\"";
    for (unsigned char c(0); c < mValue.size(); ++ c) {
      if (mValue[c] == '\r') {
        toStr_ << "\\r";
      } else if (mValue[c] == '\n') {
        toStr_ << "\\n";
      } else if (mValue[c] == 0) {
        toStr_ << "\\0";
      } else {
        toStr_ << mValue[c];
      } // if
    } // for
    toStr_ << "\"";
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREStrConst::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREStrConst::debug

char* IREIntConst::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREIntConst::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    //! \todo Should we never dump the reference of expressions?
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << mValue;
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") = " << mValue << 
          " " << getMark(this) << dumpMatchInfo(context_, this);
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")" << mValue << ")";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREIntConst::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREIntConst::debug

char* IREBoolConst::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREBoolConst::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << (mValue == true ? "{T}" : "{F}");
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREBoolConst::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREBoolConst::debug

char* IRERealConst::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRERealConst::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << mValue;
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") = " << mValue << 
          " " << getMark(this) << dumpMatchInfo(context_, this);
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")" << mValue << ")";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRERealConst::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRERealConst::debug

char* IREAddrConst::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREAddrConst::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      std_dump:
      if (IsLabel() == true) {
        const char* addrTakenStr = !mLabel->GetAddressTaken() ? "" : "&&";
        toStr_ << addrTakenStr << mLabel->GetName();
        if (mOffset != 0) {
          toStr_ << " + " << mOffset;
        } // if
      } else if (IsObject() == true) {
        toStr_ << mObject->GetName().c_str();
        if (mOffset != 0) {
          toStr_ << " + " << mOffset;
        } // if
      } else {
        toStr_ << mConstValue;
      } // if
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << getMark(this) << 
          dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        if (IsLabel() == true) {
          ASSERTMSG(0, ieStrNotImplemented);
        } else if (IsObject() == true) {
          toStr_ << mObject->GetName().c_str();
          if (mOffset != 0) {
            toStr_ << " + " << mOffset;
          } // if
        } else {
          toStr_ << mConstValue;
        } // if
        toStr_ << -- DTCCIndent;
      } else {
        //! \todo Fix this jump.
        goto std_dump;
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREAddrConst::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREAddrConst::debug

char* IREShiftLeft::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREShiftLeft::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " << " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " << " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREShiftLeft::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREShiftLeft::debug

char* IREAShiftRight::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREAShiftRight::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " A>> " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " A>> " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREAShiftRight::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREAShiftRight::debug

char* IRELShiftRight::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRELShiftRight::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " L>> " << exprcxt(GetRightExpr());
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " L>> " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRELShiftRight::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRELShiftRight::debug

char* IRENull::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRENull::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      // LIE
    } else {
      toStr_ << refcxt(this);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRENull::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRENull::debug

char* IREMember::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREMember::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtMultiLine) {
      toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
        getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
      toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
      toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
    } else {
      toStr_ << exprcxt(GetLeftExpr());
      toStr_ << (!pred.pIsPtr(GetLeftExpr()->GetType()) ? "." : "->") << exprcxt(GetRightExpr());
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREMember::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREMember::debug

char* IRESubscript::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRESubscript::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << exprcxt(GetLeftExpr()) << "[" << exprcxt(GetRightExpr()) << "]";
      } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRESubscript::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRESubscript::debug

char* IREAddrOf::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREAddrOf::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      if (context_ == DTCCDumpUseExpr) {
        if (!mExpr) {
          toStr_ << mLabel->GetName();
        } else {
          toStr_ << exprcxt(mExpr);
        } // if
      } else {
        if (!mExpr) {
          toStr_ << "&(" << mLabel->GetName() << ")";
        } else {
          if (!pred.pIsLeafIR(mExpr)) {
            toStr_ << "&(" << exprcxt(mExpr) << ")";
          } else {
            toStr_ << "&" << exprcxt(mExpr);
          } // if
        } // if
      } // if
    } else {
      //! \todo M Design: Not sure if "&" before or after the type, check it.
      if (!mExpr) {
        toStr_ << "((" << refcxt(GetType()) << ")&" << mLabel->GetName() << ")";
      } else {
        if (dtConfig == DTCVStmtExprFmtMultiLine) {
          toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
            getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
          toStr_ << DTCCIndent << exprcxt(GetExpr()) << -- DTCCIndent;
        } else {
          toStr_ << "((" << refcxt(GetType()) << ")&" << exprcxt(mExpr) << ")";
        } // if
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREAddrOf::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREAddrOf::debug

char* IREDeref::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREDeref::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      if (!pred.pIsAddrOf(GetExpr())) {
          if (!pred.pIsLeafIR(GetExpr())) {
            toStr_ << "*(" << exprcxt(GetExpr()) << ")";
          } else {
            toStr_ << "*" << exprcxt(GetExpr());
          } // if
      } else {
        toStr_ << usecxt(GetExpr());
      } // if
    } else {
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "(*(" << refcxt(GetType()) << ")" << exprcxt(GetExpr()) << ")";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREDeref::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREDeref::debug

char* IREPtrSub::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREPtrSub::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " - " << exprcxt(GetRightExpr());
    } else {
      //! \todo M Design: do more decoration to IREPtrSub::debug.
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " - " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREPtrSub::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREPtrSub::debug

char* IREPtrAdd::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREPtrAdd::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    if (dtConfig == DTCVStmtExprFmtC) {
      toStr_ << exprcxt(GetLeftExpr()) << " + " << exprcxt(GetRightExpr());
    } else {
      //! \todo M Design: do more decoration to IREPtrAdd::debug.
      if (dtConfig == DTCVStmtExprFmtMultiLine) {
        toStr_ << refcxt(this) << " (" << refcxt(GetType()) << ") " << 
          getMark(this) << dumpMatchInfo(context_, this) << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetLeftExpr()) << DTCCEndLine;
        toStr_ << DTCCIndent << exprcxt(GetRightExpr()) << -- DTCCIndent;
      } else {
        toStr_ << "((" << refcxt(GetType()) << ")(" << exprcxt(GetLeftExpr()) << " + " << exprcxt(GetRightExpr()) << "))";
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREPtrAdd::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREPtrAdd::debug

char* PredicateSortHelper::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("PredicateSortHelper::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // PredicateSortHelper::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // PredicateSortHelper::debug

char* GenInstOp::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("GenInstOp::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef || context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    switch(mKind) {
      case GIO_String: toStr_ << "string(" << *mValue.mString << ")";break;
      case GIO_Shift: toStr_ << "shift(" << toStr(mValue.mShift) << ")"; break;
      case GIO_Cond: toStr_ << "cond(" << toStr(mValue.mCondition) << ")"; break;
      case GIO_RegSet: toStr_ << "regset(" << *mValue.mRegSet << ")"; break;
      case GIO_Register: toStr_ << "reg(" << progcxt(mValue.mReg) << ")"; break;
      case GIO_Int: toStr_ << "int(" << *mValue.mInt << ")"; break;
      case GIO_Invalid: toStr_ << "invalid"; break;
      default:
        ASSERTMSG(0, ieStrNotImplemented);
    } // switch
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // GenInstOp::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // GenInstOp::debug

char* InstSetDef_Op::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_Op::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_Op::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_Op::debug

char* InstSetDef_OpTemplate::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_OpTemplate::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "name(" << mName << ")";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << "template=" << mName;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_OpTemplate::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_OpTemplate::debug

char* InstSetDef_OpLabel::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_OpLabel::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "value(" << mValue << ")";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << "label=" << mValue;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_OpLabel::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_OpLabel::debug

char* InstSetDef_OpRegSet::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_OpRegSet::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "regset(" << mValue << ")";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << "regset=" << mValue << ", parent=" << refcxt(mParent);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_OpRegSet::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_OpRegSet::debug

char* InstSetDef_OpReg::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_OpReg::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    if (IsValueSet() == true) {
      toStr_ << "reg(" << GetRegsType()->GetName() << "=" << mValue->GetName() << ")";
    } else {
      toStr_ << "reg(" << GetRegsType()->GetName() << ")";
    } // if
  } else if (context_ == DTCCDumpProg) {
    if (IsValueSet() == true) {
      toStr_ << "reg=" << GetRegsType()->GetName() << "=" << (mValue != 0 ? mValue->GetName() : "0");
    } else {
      toStr_ << "reg=" << GetRegsType()->GetName();
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_OpReg::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_OpReg::debug

char* InstSetDef_OpFlag::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_OpFlag::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    if (IsValueSet() == true) {
      toStr_ << "flag(" << GetFlag()->GetName() << "=" << mValue << ")";
    } else {
      toStr_ << "flag(" << GetFlag()->GetName() << ")";
    } // if
  } else if (context_ == DTCCDumpProg) {
    if (IsValueSet() == true) {
      toStr_ << "flag=" << GetFlag()->GetName() << "=" << mValue;
    } else {
      toStr_ << "flag=" << GetFlag()->GetName();
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_OpFlag::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_OpFlag::debug

char* InstSetDef_OpInt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_OpInt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    if (IsValueSet() == true) {
      toStr_ << "OpInt:value(" << GetValue() << ")";
    } else {
      toStr_ << "OpInt";
    } // if
  } else if (context_ == DTCCDumpProg) {
    if (IsValueSet() == true) {
      toStr_ << "OpInt=" << GetValue();
    } else {
      toStr_ << "OpInt";
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_OpInt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_OpInt::debug

char* InstSetDef_Flag::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_Flag::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_Flag::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_Flag::debug

char* InstSetDef_Regs::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_Regs::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_Regs::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_Regs::debug

char* InstSetDef_Inst::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_Inst::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "name: " << mName << ", template=" << mTemplate << ", size=" << mSizeInBytes << 
      ", fmtstr=" << mFormatStr;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_Inst::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_Inst::debug

char* InstSetDef_Template::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstSetDef_Template::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << DTCCIndent << "name=" << mName << ", isOr=" << (mIsOR ? "Y" : "N") << ", fmtstr=" << mFormatStr << ", parent=" << progcxt(mParent);
    toStr_ << DTCCEndLine << ++ DTCCIndent << DTCCIndent << "operands(";
    for (hFUInt32 c(0); c < mOperands.size(); ++ c) {
      if (c != 0) {
        toStr_ << ", ";
      } // if
      toStr_ << progcxt(mOperands[c]);
    } // for
    toStr_ << ")" << -- DTCCIndent;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstSetDef_Template::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstSetDef_Template::debug

char* PredicateDBCHelper::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("PredicateDBCHelper::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // PredicateDBCHelper::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // PredicateDBCHelper::debug

char* tristate::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("tristate::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // tristate::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // tristate::debug

char* AddrStack::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AddrStack::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    if (!IsAssigned()) {
      toStr_ << refcxt(this) << "<not assigned>";
    } else {
      toStr_ << progcxt(mReg) << " + " << mOffset;
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AddrStack::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AddrStack::debug

char* AddrLTConst::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AddrLTConst::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    if (!IsAssigned()) {
      toStr_ << refcxt(this) << "<not assigned>";
    } else {
      if (!GetOffset()) {
        toStr_ << mLTConst.c_str();
      } else {
        toStr_ << mLTConst.c_str() << "+" << mOffset;
      } // if
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AddrLTConst::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AddrLTConst::debug

char* AddrReg::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AddrReg::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    if (!IsAssigned()) {
      toStr_ << refcxt(this) << "<not assigned>";
    } else {
      toStr_ << progcxt(mReg);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AddrReg::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AddrReg::debug

char* Address::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Address::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Address::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Address::debug

char* IRTLabel::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRTLabel::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRTLabel::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRTLabel::debug

char* IRBBAny::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRBBAny::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRBBAny::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRBBAny::debug

char* IR::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  EDEBUGCALL();
  return "";
} // IR::debug

char* AsmSeqData::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AsmSeqData::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  NOTIMPLEMENTED();
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AsmSeqData::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AsmSeqData::debug

char* AsmSeqLabel::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AsmSeqLabel::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  NOTIMPLEMENTED();
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AsmSeqLabel::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AsmSeqLabel::debug

char* ASMDesc::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ASMDesc::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ASMDesc::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ASMDesc::debug

char* CallingConvention::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("CallingConvention::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // CallingConvention::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // CallingConvention::debug

char* RegSet::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("RegSet::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    toStr_ << "{";
    for (set<Register*>::iterator lIt(mRegs.begin()); lIt != mRegs.end(); ++ lIt) {
      Register* lReg(*lIt);
      if (lIt != mRegs.begin()) {
        toStr_ << ", ";
      } // if
      toStr_ << lReg->GetName().c_str();
    } // for
    toStr_ << "}";
  } else if (context_ == DTCCDumpHWDesc) {
    NOTIMPLEMENTED();
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // RegSet::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // RegSet::debug

char* CGFuncContext::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("CGFuncContext::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpCGCxt) {
    toStr_ << DTCCIndent << "Dumping CG Context: {" << ++ DTCCIndent << DTCCEndLine;
    toStr_ << DTCCIndent << "Func: " << refcxt(mFunc) << DTCCEndLine;
    toStr_ << DTCCIndent << "Must match: " << mMustMatch << DTCCEndLine;
    toStr_ << DTCCIndent << "mIsFailed: " << mIsFailed << DTCCEndLine;
    { // Dump the matcher info.
      map<IRExpr*, IRExprNodeMatcherInfo*>::const_iterator lIt;
      for (lIt = mIRNodeMatchInfo.begin(); lIt != mIRNodeMatchInfo.end(); ++ lIt) {
        IRExpr* lExpr(lIt->first);
        IRExprNodeMatcherInfo* lExprNode(lIt->second);
        toStr_ << refcxt(lExpr) << " => {" << ++ DTCCIndent << DTCCEndLine;
        toStr_ << DTCCIndent << "Matched Rules: ";
        if (!lExprNode->mMatchedRules.size()) {
          toStr_ << "0";
        } else {
          for (hFUInt32 c(0); c < lExprNode->mMatchedRules.size(); ++ c) {
            if (c != 0) {
              toStr_ << ", ";
            } else {
              toStr_ << refcxt(lExprNode->mMatchedRules[c]);
            } // if
          } // for
        } // if
        toStr_ << DTCCEndLine;
        toStr_ << DTCCIndent << "Cost vector: ";
        bool lAnyCostElemPut(false);
        for (hFInt32 ntType(NTTFirst); ntType < NTTLast; ++ ntType) {
          if (lExprNode->mCostVector[ntType].first != consts.cInfiniteCost) {
            if (lAnyCostElemPut == true) {
              toStr_ << ", ";
            } // if
            lAnyCostElemPut = true;
            toStr_ << "<" << NTType(ntType) << ", ";
            toStr_ << lExprNode->mCostVector[ntType].first << ", " << refcxt(lExprNode->mCostVector[ntType].second);
            toStr_ << ", ";
            if (!pred.pIsInvalid(lExprNode->mNTConvRule[ntType])) {
              toStr_ << refcxt(lExprNode->mNTConvRule[ntType]);
            } else {
              toStr_ << "0";
            } // if
            toStr_ << ">";
          } // if
        } // for
        toStr_ << DTCCEndLine;
        toStr_ << -- DTCCIndent << DTCCIndent << "}" << DTCCEndLine;
      } // for
    } // block
    toStr_ << -- DTCCIndent << DTCCIndent << "} // CGContext dump" << DTCCEndLine; 
  } else if (context_ == DTCCDumpProg) {
    NOTIMPLEMENTED();
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // CGFuncContext::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // CGFuncContext::debug

char* CGContext::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("CGContext::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // CGContext::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // CGContext::debug

char* ShiftOpNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ShiftOpNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "ShiftOpNT(" << mReg << ", ";
    if (!dynamic_cast<Invalid<Register>*>(mRegShiftAmount)) {
      toStr_ << hwdesccxt(mRegShiftAmount);
    } else {
      toStr_ << "int";
    } // if
    toStr_ << ", " << mShiftType << ")";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ShiftOpNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ShiftOpNT::debug

char* Instruction::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Instruction::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    NOTIMPLEMENTED();
  } else if (context_ == DTCCDumpProg) {
    toStr_ << "disasm: " << mDisassembly << ", defs=" << mDefs << ", uses=" << mUses;
    if (!mOperands.empty()) {
      toStr_ << "ops=";
      for (hFUInt32 c(0); c < mOperands.size(); ++ c) {
        if (c != 0) {
          toStr_ << ", ";
        } // if
        toStr_ << progcxt(&mOperands[c]);
      } // for
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Instruction::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Instruction::debug

char* ConditionNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ConditionNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "ConditionNT";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ConditionNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ConditionNT::debug

char* NullNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("NullNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "NullNT";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // NullNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // NullNT::debug

char* AnyNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AnyNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "AnyNT";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AnyNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AnyNT::debug

char* RealConstNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("RealConstNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "RealConstNT";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // RealConstNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // RealConstNT::debug

char* IntConstNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IntConstNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "IntConstNT";
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IntConstNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IntConstNT::debug

char* MemoryNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("MemoryNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "MemoryNT";
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // MemoryNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // MemoryNT::debug

char* IgnoreNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IgnoreNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "IgnoreNT";
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IgnoreNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IgnoreNT::debug

char* RegisterNT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("RegisterNT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "RegisterNT{";
    if (!mReg) {
      toStr_ << "null";
    } else {
      toStr_ << hwdesccxt(mReg);
    } // if
    toStr_ << "}";
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // RegisterNT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // RegisterNT::debug

char* VirtReg::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("VirtReg::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
    toStr_ << "VirtReg{" << refcxt(this) << GetName().c_str() << ", " << GetId() << ", ";
    if (!pred.pIsInvalid(mHWReg)) {
      toStr_ << progcxt(mHWReg);
    } else {
      toStr_ << "HWReg(null)";
    } // if
    toStr_ << "}";
    if (!mRegAllocRules.empty()) {
      toStr_ << ++ DTCCIndent << DTCCEndLine;
      for (hFUInt32 c(0); c < mRegAllocRules.size(); ++ c) {
        if (c != 0) {
          toStr_ << DTCCEndLine;
        } // if
        toStr_ << DTCCIndent << "Rule:" << c << " " << progcxt(mRegAllocRules[c]);
      } // for
      toStr_ << -- DTCCIndent;
    } else {
      toStr_ << "<no RARule>";
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // VirtReg::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // VirtReg::debug

char* DFAProgPnt::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DFAProgPnt::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpDFA) {
    toStr_ << mBitIndex;
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DFAProgPnt::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DFAProgPnt::debug

char* RegAllocRule::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("RegAllocRule::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // RegAllocRule::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // RegAllocRule::debug

char* HWReg::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("HWReg::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    toStr_ << "HWReg{" << GetName().c_str() << ", " << GetId() << "}";
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // HWReg::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // HWReg::debug

char* StmtRule::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("StmtRule::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "{" << GetName().c_str() << ", " << GetHWDesc()->GetName().c_str() << "}";
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this) << " {" << ++ DTCCIndent << DTCCEndLine;
    toStr_ << DTCCIndent << "stmtPattern: " << hwdesccxt(mStmtPattern);
    if (!pred.pIsInvalid(mIRStmt)) {
      toStr_ << DTCCEndLine << DTCCIndent << "matched IRStmt: " << refcxt(mIRStmt) << DTCCEndLine;
    } else {
      toStr_ << DTCCEndLine << DTCCIndent << "matched IRStmt: null" << DTCCEndLine;
    } // if
    toStr_ << -- DTCCIndent << DTCCIndent << "} // StmtRule" << DTCCEndLine;
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // StmtRule::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // StmtRule::debug

char* ExprRule::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ExprRule::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "{" << GetName().c_str() << ", " << GetHWDesc()->GetName().c_str() << "}";
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this) << " {" << ++ DTCCIndent << DTCCEndLine;
    toStr_ << DTCCIndent << "resultNT: " << hwdesccxt(mResultNT);
    toStr_ << DTCCEndLine << DTCCIndent << "exprPattern: " << hwdesccxt(mExprPattern);
    if (!pred.pIsInvalid(mIRExpr)) {
      toStr_ << DTCCEndLine << DTCCIndent << "matched IRExpr: " << refcxt(mIRExpr);
    } else {
      toStr_ << DTCCEndLine << DTCCIndent << "matched IRExpr: null";
    } // if
    for (hFUInt32 c(0); c < mPatternNTs.size(); ++ c) {
      if (mPatternNTs[c]) {
        toStr_ << DTCCEndLine << DTCCIndent << "patternNTs[" << c << "]: " << hwdesccxt(mPatternNTs[c]);
      } else {
        break;
      } // if
    } // for
    toStr_ << -- DTCCIndent << DTCCIndent << "} // ExprRule" << DTCCEndLine;
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ExprRule::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ExprRule::debug

//! \todo M Design: I think we do not need the rule debugtrace.
char* Rule::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Rule::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << GetName().c_str() << ", " << GetHWDesc()->GetName().c_str();
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Rule::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Rule::debug

char* AsmSeqDirective::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AsmSeqDirective::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  NOTIMPLEMENTED();
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AsmSeqDirective::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AsmSeqDirective::debug

char* RegisterAllocator::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("RegisterAllocator::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    NOTIMPLEMENTED();
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
    if (!pred.pIsInvalid(mFuncContext)) {
      toStr_ << "Register allocator dump ";
      toStr_ << mName.c_str() << " {" << ++ DTCCIndent << DTCCEndLine;
      toStr_ << progcxt(mFuncContext->GetFunc());
      toStr_ << -- DTCCIndent << DTCCIndent << "}" << DTCCEndLine;
    } else {
      ASSERTMSG(0, ieStrInconsistentInternalStructure);
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // RegisterAllocator::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // RegisterAllocator::debug

char* NonTerminal::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("NonTerminal::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  NOTIMPLEMENTED();
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // NonTerminal::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // NonTerminal::debug

char* HWDescription::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("HWDescription::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "{" << GetName().c_str() << "}";
  } else if (context_ == DTCCDumpProg) {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << "HWDescription: " << mName.c_str() << "{" << ++ DTCCIndent << DTCCEndLine;
    if (!mStmtRules.size()) {
      toStr_ << DTCCIndent << "StmtRules: None" << DTCCEndLine;
    } else {
      toStr_ << DTCCIndent << "StmtRules {" << ++ DTCCIndent << DTCCEndLine;
      for (hFUInt32 c(0); c < mStmtRules.size(); ++ c) {
        toStr_ << DTCCIndent << hwdesccxt(mStmtRules[c]) << DTCCEndLine;
      } // for
      toStr_ << -- DTCCIndent << DTCCIndent << "} // StmtRules " << mName.c_str() << DTCCEndLine;
    } // if
    if (!mExprRules.size()) {
      toStr_ << DTCCIndent << "ExprRules: None" << DTCCEndLine;
    } else {
      toStr_ << DTCCIndent << "ExprRules {" << ++ DTCCIndent << DTCCEndLine;
      for (hFUInt32 c(0); c < mExprRules.size(); ++ c) {
        toStr_ << DTCCIndent << hwdesccxt(mExprRules[c]) << DTCCEndLine;
      } // for
      toStr_ << -- DTCCIndent << DTCCIndent << "} // ExprRules " << mName.c_str() << DTCCEndLine;
    } // if
    if (!mNTConvRules.size()) {
      toStr_ << DTCCIndent << "Conversion Rules: None" << DTCCEndLine;
    } else {
      toStr_ << DTCCIndent << "Conversion Rules {" << ++ DTCCIndent << DTCCEndLine;
      for (hFUInt32 c(0); c < mNTConvRules.size(); ++ c) {
        toStr_ << hwdesccxt(mNTConvRules[c]);
      }   // for
      toStr_ << -- DTCCIndent << DTCCIndent << "} // Conversion Rules" << mName.c_str() << DTCCEndLine;
    } // if
    toStr_ << -- DTCCIndent << DTCCIndent << "} // HWDescription " << mName.c_str() << DTCCEndLine;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // HWDescription::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // HWDescription::debug

char* AnatropManager::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("AnatropManager::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // AnatropManager::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // AnatropManager::debug

char* StmtVisitor::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("StmtVisitor::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // StmtVisitor::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // StmtVisitor::debug

char* ExprVisitor::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ExprVisitor::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ExprVisitor::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ExprVisitor::debug

char* CExprVisitor::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("CExprVisitor::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // CExprVisitor::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // CExprVisitor::debug

char* Dwarf2CLangHelper::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Dwarf2CLangHelper::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Dwarf2CLangHelper::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Dwarf2CLangHelper::debug

char* DIEBase::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DIEBase::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DIEBase::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DIEBase::debug

char* DW_DIE::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DW_DIE::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DW_DIE::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DW_DIE::debug

char* SrcLoc::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("SrcLoc::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    //! \todo M Design: add a config option to dump params as in progcxt.
    if (pred.pIsSet(*this) == true) {
      toStr_ << "<" << GetFileNum() << ", " << GetLineNum() << ", " << GetColNum() << ">";
    } else {
      toStr_ << "<none>"; 
    } // if
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    if (pred.pIsSet(*this) == true) {
      toStr_ << "<" << GetFileNum() << ", " << GetLineNum() << ", " << GetColNum() << ">";
    } else {
      toStr_ << "<none>"; 
    } // if
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // SrcLoc::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // SrcLoc::debug

char* Utils::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Utils::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Utils::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Utils::debug

char* DW_AT::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DW_AT::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DW_AT::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DW_AT::debug

char* IRLoop::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRLoop::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << DTCCIndent << "IRLoop { // " << refcxt(this) << ++ DTCCIndent << DTCCEndLine;
    if (!mParentLoop) {
      toStr_ << DTCCIndent << "parent: null" << DTCCEndLine;
    } else {
      toStr_ << DTCCIndent << "parent: " << refcxt(mParentLoop) << DTCCEndLine;
    } // if
    toStr_ << DTCCIndent << "header: " << refcxt(mHeader) << DTCCEndLine;
    { // Dump loop references that share header with this one.
      toStr_ << DTCCIndent << "shares header with: ";
      if (mSharedHeader.empty() == true) {
        toStr_ << "null";
      } // if
      for (hFUInt32 c(0); c < mSharedHeader.size(); ++ c) {
        if (c != 0) {
          toStr_ << ", ";
        } // if
        toStr_ << refcxt(mSharedHeader[c]);
      } // for
      toStr_ << DTCCEndLine;
    } // block
    if (!mPreHeader) {
      toStr_ << DTCCIndent << "preheader: null" << DTCCEndLine;
    } else {
      toStr_ << DTCCIndent << "preheader: " << refcxt(mPreHeader) << DTCCEndLine;
    } // if
    { // Dump loop references nested in this.
      toStr_ << DTCCIndent << "nested loops: ";
      if (mNestedLoops.empty() == true) {
        toStr_ << "null";
      } // if
      for (hFUInt32 c(0); c < mNestedLoops.size(); ++ c) {
        if (c != 0) {
          toStr_ << ", ";
        } // if
        toStr_ << refcxt(mNestedLoops[c]);
      } // for
      toStr_ << DTCCEndLine;
    } // block
    { // Dump basic and dependent induction variables.
      if (!mBasicIndVars || mBasicIndVars->empty() == true) {
        toStr_ << DTCCIndent << "basic induction vars: none" << DTCCEndLine;
        toStr_ << DTCCIndent << "dependent induction vars: none" << DTCCEndLine;
      } else {
        toStr_ << DTCCIndent << "basic induction vars: " << ++ DTCCIndent << DTCCEndLine;
        for (hFUInt32 c(0); c < mBasicIndVars->size(); ++ c) {
          toStr_ << DTCCIndent << progcxt((*mBasicIndVars)[c]) << DTCCEndLine;
        } // for
        toStr_ << -- DTCCIndent << DTCCIndent << "dependent induction vars: " << ++ DTCCIndent << DTCCEndLine;
        for (hFUInt32 d(0); d < mDependentIVs->size(); ++ d) {
          toStr_ << DTCCIndent << progcxt((*mDependentIVs)[d]) << DTCCEndLine;
        } // for
        toStr_ << -- DTCCIndent;
      } // if
    } // block
    { // Dump exit edges.
      toStr_ << DTCCIndent << "exit bb count: " << mExitBBCount << DTCCEndLine;
      if (mExitEdges.empty() == true) {
        toStr_ << DTCCIndent << "exit edges: none" << DTCCEndLine;
      } else {
        toStr_ << DTCCIndent << "exit edges: ";
        for (hFUInt32 c(0); c < mExitEdges.size(); ++ c) {
          if (c != 0) {
            toStr_ << ", ";
          } // if
          toStr_ << refcxt(mExitEdges[c].GetFrom()) << " -> " << refcxt(mExitEdges[c].GetTo());
        } // for
        toStr_ << DTCCEndLine;
      } // if
    } // block
    { // Dump body basic blocks.
      toStr_ << DTCCIndent << "body bbs: ";
      for (set<IRBB*>::const_iterator lIt(mBodyBBs.begin()); lIt != mBodyBBs.end(); ++ lIt) {
        if (lIt != mBodyBBs.begin()) {
          toStr_ << ", ";
        } // if
        toStr_ << refcxt(*lIt);
      } // for
      toStr_ << DTCCEndLine;
    } // block
    toStr_ << -- DTCCIndent << DTCCIndent << "} // IRLoop" << DTCCEndLine;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRLoop::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRLoop::debug

char* Options::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Options::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Options::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Options::debug

char* IREdge::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IREdge::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(GetFrom()) << " -> " << refcxt(GetTo());
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IREdge::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IREdge::debug

char* GenericOptions::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("GenericOptions::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // GenericOptions::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // GenericOptions::debug

char* DebugInfoDwarf2::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("DebugInfoDwarf2::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // DebugInfoDwarf2::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // DebugInfoDwarf2::debug

char* InstructionSetDefinition::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("InstructionSetDefinition::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // InstructionSetDefinition::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // InstructionSetDefinition::debug

char* RARMustNotBeSame::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("RARMustNotBeSame::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this) << " regs=" << mRegs;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // RARMustNotBeSame::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // RARMustNotBeSame::debug

char* RARMustBeInList::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("RARMustBeInList::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << refcxt(this) << " {";
    for (hFUInt32 c(0); c < mVRegs.size(); ++ c) {
      if (c != 0) {
        toStr_ << ", ";
      } // if
      toStr_ << refcxt(mVRegs[c]);
    } // for
    toStr_ << "}" << DTCCEndLine;
    { // emit the hard register lists, only once.
      static set<void*> lIsDumped;
      if (lIsDumped.find((void*)mValidVectors) == lIsDumped.end()) {
        lIsDumped.insert((void*)mValidVectors);
        toStr_ << DTCCIndent << "D:" << *lIsDumped.find((void*)mValidVectors) << "{";
        for (hFUInt32 c(0); c < (*mValidVectors).size(); ++ c) {
          toStr_ << DTCCIndent << "{";
          for (hFUInt32 d(0); d < (*mValidVectors)[c].size(); ++ d) {
            if (d != 0) {
              toStr_ << ", ";
            } // if
            toStr_ << progcxt((*mValidVectors)[c][d]);
          } // for
          toStr_ << "}" << DTCCEndLine;
        } // for
        toStr_ << "}";
      } else {
        toStr_ << DTCCIndent << "Valid vectors=" << *lIsDumped.find((void*)mValidVectors);
      } // if
    } // block
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // RARMustBeInList::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // RARMustBeInList::debug

char* IRDUUDChains::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRDUUDChains::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    if (dtConfig == DTCVDumpCFGGraphs && HTMLNode::Get("exePointDumps")->GetChildCount() != 0) {
      static hFInt32 slDUUDGraphIndex;
      char lFileNameDot[256];
      char lFileNamePNG[256];
      sprintf(lFileNameDot, "/tmp/duud%d.dot", slDUUDGraphIndex);
      sprintf(lFileNamePNG, "/tmp/duud%d.dot.png", slDUUDGraphIndex++);
      ofstream lDUUDGraph(lFileNameDot);
      lDUUDGraph << "digraph {\n\tcompound=true;\n\tcenter=true;\n\t";
      lDUUDGraph << "node [shape=plaintext,style=filled,fillcolor=gray];" << endl;
      ListIterator<IRBB*> lBBIter(mCFG->GetPhysBBIter());
      ostrstream lStmtEdges;
      ostrstream lBBEdges;
      //! \todo M Design: Add an option for invisible style.
      // weight should be incresed as the number of du edges increase.
      lStmtEdges << "\n\t{ edge [style=invis,weight=100];" << endl;
      // Dump the basic blocks and CFG edges.
      for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
        lDUUDGraph << "\tsubgraph clusterBB" << lBBIter->GetObjId() << " {" << endl;
        lDUUDGraph << "\t\trank=samerank; rankdir=TB;" << endl;
        lDUUDGraph << "\t\tBB" << lBBIter->GetObjId() << " [sphape=rect];" << endl;
        lStmtEdges << "\t\tBB" << lBBIter->GetObjId();
        ListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtIter());
        IRStmt* lLastStmt(0);
        for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
          lDUUDGraph << "    stmt" << lStmtIter->GetObjId() << " [label=\"" << graphcxt(*lStmtIter) << "\"];" << endl;
          lStmtEdges << " -> stmt" << lStmtIter->GetObjId();
          lLastStmt = *lStmtIter;
        } // for
        if (!lLastStmt) {
          lStmtEdges << "  ->  BB" << lBBIter->GetObjId();
        } // if
        lStmtEdges << ";" << endl;
        { // dump successor edges
          vector<IRBB*> lSuccs;
          extr.eGetSuccsOfBB(*lBBIter, lSuccs);
          for (hFUInt32 c(0); c < lSuccs.size(); ++ c) {
            if (!lLastStmt) {
              lBBEdges << "\tBB" << lBBIter->GetObjId() << " -> ";
            } else {
              lBBEdges << "\tstmt" << lLastStmt->GetObjId() << " -> ";
            } // if
            lBBEdges << "BB" << lSuccs[c]->GetObjId() << " [ltail=clusterBB" << lBBIter->GetObjId() << ",";
            lBBEdges << "lhead=clusterBB" << lSuccs[c]->GetObjId() << "];" << endl;
          } // for
        } // block
        lDUUDGraph << "\t}" << endl;
      } // for
      lStmtEdges << "\t}" << ends;
      lBBEdges << ends;
      ostrstream lDUUDEdges;
      { // Dump uddu chain edges.
        lDUUDEdges << "\t{ edge [dir=both,color=red];\n";
        map<IRObject*, Defs2UsesStmt>::const_iterator lDUIt(mObj2Defs2UsesStmt.begin());
        while (lDUIt != mObj2Defs2UsesStmt.end()) {
          Defs2UsesStmt::const_iterator lDefIt(lDUIt->second.begin());
          while (lDefIt != lDUIt->second.end()) {
            set<IRStmt*>::iterator lUsesIt(lDefIt->second.begin());
            while (lUsesIt != lDefIt->second.end()) {
              IRStmt* lUse(*lUsesIt);
              lDUUDEdges << "    stmt" << lDefIt->first->GetObjId() << " -> stmt" << lUse->GetObjId() << 
                " [headlabel=\"" << lDUIt->first->GetName() << "\"];" << endl;
              ++ lUsesIt;
            } // while
            ++ lDefIt;
          } // while
          ++ lDUIt;
        } // while
        lDUUDEdges << "\t}\n";
        if (0) { // Convert this in to a config file option
          lDUUDEdges << "\t{ edge [color=green];\n";
          map<IRObject*, Uses2DefsStmt>::const_iterator lUDIt(mObj2Uses2DefsStmt.begin());
          while (lUDIt != mObj2Uses2DefsStmt.end()) {
            Uses2DefsStmt::const_iterator lUseIt(lUDIt->second.begin());
            while (lUseIt != lUDIt->second.end()) {
              set<IRStmt*>::iterator lDefsIt(lUseIt->second.begin());
              while (lDefsIt != lUseIt->second.end()) {
                IRStmt* lDef(*lDefsIt);
                lDUUDEdges << "\tstmt" << lUseIt->first->GetObjId() << " -> stmt" << lDef->GetObjId() << ";" <<endl;
                ++ lDefsIt;
              } // while
              ++ lUseIt;
            } // while
            ++ lUDIt;
          } // while
          lDUUDEdges << "  }";
        } // if
        lDUUDEdges << ends;
      } // block
      lDUUDGraph << lStmtEdges.str() << endl << lBBEdges.str() << endl << lDUUDEdges.str() << "} " << endl;
    } // if
    toStr_ << DTCCIndent << "IRDUChains {" << ++ DTCCIndent << DTCCEndLine;
    toStr_ << DTCCIndent << "CFG: " << refcxt(mCFG) << DTCCEndLine;
    map<IRObject*, Defs2UsesStmt>::const_iterator lIt(mObj2Defs2UsesStmt.begin());
    while (lIt != mObj2Defs2UsesStmt.end()) {
      toStr_ << DTCCIndent << "Obj= " << refcxt(lIt->first) << " {" << ++ DTCCIndent << DTCCEndLine;
      Defs2UsesStmt::const_iterator lDefIt(lIt->second.begin());
      while (lDefIt != lIt->second.end()) {
        //! \todo M Design: Can we mark the column here " -> " and continue from that in the next line?
        toStr_ << DTCCIndent << refcxt(lDefIt->first) << " -> ";
        set<IRStmt*>::const_iterator lUsesIt(lDefIt->second.begin());
        while (lUsesIt != lDefIt->second.end()) {
          if (lUsesIt != lDefIt->second.begin()) {
            toStr_ << ", ";
          } // if
          toStr_ << refcxt(*lUsesIt);
          ++ lUsesIt;
        } // while
        toStr_ << DTCCEndLine;
        ++ lDefIt;
      } // while
      toStr_ << -- DTCCIndent << DTCCIndent << "} // IRObject" << DTCCEndLine;
      ++ lIt;
    } // while
    toStr_ << -- DTCCIndent << DTCCIndent << "} // IRDUChains" << DTCCEndLine;
    { // Dump UD chains
      toStr_ << DTCCIndent << "IRUDChains {" << ++ DTCCIndent << DTCCEndLine;
      toStr_ << DTCCIndent << "CFG: " << refcxt(mCFG) << DTCCEndLine;
      map<IRObject*, Uses2DefsStmt>::const_iterator lIt(mObj2Uses2DefsStmt.begin());
      while (lIt != mObj2Uses2DefsStmt.end()) {
        toStr_ << DTCCIndent << "Obj= " << refcxt(lIt->first) << " {" << ++ DTCCIndent << DTCCEndLine;
        Uses2DefsStmt::const_iterator lUseIt(lIt->second.begin());
        while (lUseIt != lIt->second.end()) {
          //! \todo M Design: Can we mark the column here " -> " and continue from that in the next line?
          toStr_ << DTCCIndent << refcxt(lUseIt->first) << " -> ";
          set<IRStmt*>::const_iterator lDefsIt(lUseIt->second.begin());
          while (lDefsIt != lUseIt->second.end()) {
            if (lDefsIt != lUseIt->second.begin()) {
              toStr_ << ", ";
            } // if
            toStr_ << refcxt(*lDefsIt);
            ++ lDefsIt;
          } // while
          toStr_ << DTCCEndLine;
          ++ lUseIt;
        } // while
        toStr_ << -- DTCCIndent << DTCCIndent << "} // IRObject" << DTCCEndLine;
        ++ lIt;
      } // while
      toStr_ << -- DTCCIndent << DTCCIndent << "} // IRUDChains" << DTCCEndLine;
    } // block
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRDUChains::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRDUUDChains::debug

char* IRInductionVar::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRInductionVar::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    toStr_ << refcxt(this);
  } else if (context_ == DTCCDumpProg) {
    toStr_ << exprcxt(mIndVar) << " = <" << exprcxt(mBasisVar) << ", Mul=" <<
      exprcxt(mMulFactor) << ", Add=" << exprcxt(mAddFactor) << "> @" << refcxt(mParent);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRInductionVar::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRInductionVar::debug





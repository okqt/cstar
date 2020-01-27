// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif

// Define the global utility data here
bool dtEnabled = true; //! Do not change this value here, use the config file option.
bool dtLogEnabled = true; //! Do not change this value here, use the config file option.
bool dtAssertMsgEnabled = true; //! Do not change this value here, use the config file option.

map<string, const IDebuggable*> DTContext::mContextObjs;
map<string, hFUInt32> SrcLoc::smFileNums;

ostream* DTContext::mCFGGraph = 0;
DTContext* dtContext = &Singleton<DTContext>::Obj();

//! \brief A helper stream to be used in DBC macros.
ostrstream errMsg;

//! \brief Definition point of Predicate utility.
//! \note Note that \a pred is a run-time singleton.
Predicate pred;
hFSInt8 Predicate::smRefCount;
Consts consts;

Utils util;
hFSInt8 Utils::smRefCount;

//! \brief Definition point of Extract utility.
//! \note Note that \a extr is a run-time singleton.
Extract extr;
hFSInt8 Extract::smRefCount;

//! \brief Definition point of debug trace config.
//! \note Note that \a dtConfig is a run-time singleton.
DTConfig dtConfig;
hFSInt8 DTConfig::smRefCount;

hFUInt32 IDebuggable::mObjIds;

ofstream dtLog("/tmp/dtLog.log");
static hFSInt16 dtlIndentAmount;

ostream& 
operator << (ostream& o, const SrcLoc& srcLoc_) {
  o << "@<" << srcLoc_.GetFileName().c_str() << ", ";
  o << srcLoc_.GetLineNum() << ", ";
  o << srcLoc_.GetColNum();
  o << ">";
  return DBGFLUSH(o);
} // operator <<

static bool
dbcAbort(const string& msg_, const string& fileName_, hFSInt32 lineNum_, const string& exprStr_, const string& explanation_) {
  ostrstream strmsg;
  //! \todo M Design: Not sure if we need below if here, check.
  if (dtAssertMsgEnabled == false) {
    strmsg << "Note: Assert messages are not enabled, use config file to enable, if you like." << endl;
  } // if
  strmsg << msg_ << " failed :" << explanation_ << endl << exprStr_ << " @" << fileName_ << ":" << lineNum_ << "\n";
  strmsg << "Relevant SrcLoc: " << debugTrace->GetSrcLoc() <<  endl << ends;
  debugTrace->debugStr(DTContext(DTCCAbort, fileName_, lineNum_), strmsg.str());
  return true;
} // dbcAbort

const char* 
toStr(DBCType dbcType_) {
  const char* retVal = 0;
  switch (dbcType_) {
    case DBCTRequired: retVal = "Required"; break;
    case DBCTAssert: retVal = "Assert"; break;
    case DBCTEnsure: retVal = "Ensure"; break;
    case DBCTValid: retVal = "Valid"; break;
  } // switch
  return retVal;
} // toStr

bool 
DBCFunc(DBCType dbcType_, bool expr_, const string& exprStr_, const char* file_, hFInt32 line_, const string& msg_) {
  if (dtEnabled == true && debugTrace->inDebugTraceCall() == false) {
    debugTrace->debugStr(DTContext(DTCCDBC, file_, line_), ""); 
  } // if
  if (expr_ == false) {
    dbcAbort(toStr(dbcType_), file_, line_, exprStr_, msg_);
  } // if
  if (dtEnabled == true && debugTrace->inDebugTraceCall() == false) {
    debugTrace->debugStr(DTContext(DTCCDBC, file_, line_), "");
  } // if
  return true;
} // DBCFunc

bool 
DBC_VALIDFunc(DBCType dbcType_, const IDebuggable* obj_, const string& objStr_, const char* file_, hFInt32 line_, const string& msg_) {
  if (DBCFunc(dbcType_, obj_ != 0, objStr_, file_, line_, msg_) != false) {
    DBCFunc(dbcType_, obj_->IsValid(), objStr_, file_, line_, msg_);
  } // if
  return true;
} // DBC_VALIDFunc

bool
dtAssert(bool exprResult_, const string& fileName_, hFSInt32 lineNum_, const string& exprStr_, const string& msg_) {
  if (exprResult_ == false) {
    cout << msg_.c_str() << endl;
    dbcAbort("Assert", fileName_, lineNum_, exprStr_, msg_);
    return false;
  } // if
  return true;
} // dtAssert

// dtLog related i/o manipulators.
ostream&
dtlEndl(ostream& o) {
  o << endl;
  for (hFSInt16 c = 0; c < dtlIndentAmount; ++ c) {
    o << "  ";
  } // for
  return o;
} // dtlEndl

ostream&
dtlUnindent(ostream& o) {
  DTASSERT(dtlIndentAmount > 0, "Indentation underflow, check matching dtlIndent's");
  -- dtlIndentAmount;
  return o;
} // dtlEndl

ostream&
dtlIndent(ostream& o) {
  //! \todo remove the hard coded limit
  DTASSERT(dtlIndentAmount < 1000, "Indentation overflow, try to increase the limit(or a bug?)");
  ++ dtlIndentAmount;
  return o;
} // dtlEndl

static const char*
toStr(DTConfigVar cv) {
  static string retVal;
  retVal = ("|");
  if (cv == DTCVInvalid) {
    retVal = "inv|";
  } else if (cv == DTCVNotSet) {
    retVal = "nst|";
  } else {
    if (cv & DTCVSkipFuncs) {
      retVal += "skpfn|";
    } else if (cv & DTCVSkipChildCalls) {
      retVal += "skpcc|";
    } else if (cv & DTCVDumpHWDesc) {
      retVal += "dphwd|";
    } else if (cv & DTCVDumpStats) {
      retVal += "dmpst|";
    } else if (cv & DTCVDumpProg) {
      retVal += "dmprg|";
    } else if (cv & DTCVShowOnly) {
      retVal += "shwon|";
    } else if (cv & DTCVDumpProgAtEnd) {
      retVal += "dmpen|";
    } else if (cv & DTCVMaxCallDepth) {
      retVal += "mxcad|";
    } else if (cv & DTCVExpandTypes) {
      retVal += "extyp|";
    } else if (cv & DTCVDumpCFGGraphs) {
      retVal += "dmcfg|";
    } else if (cv & DTCVStmtExprFmtMultiLine) {
      retVal += "fmtml|";
    } else if (cv & DTCVShowThis) {
      retVal += "shoth|";
    } else if (cv & DTCVDumpToConsole) {
      retVal += "dmpcn|";
    } else if (cv & DTCVUseObjIds) {
      retVal += "useoi|";
    } else if (cv & DTCVShowFileLoc) {
      retVal += "shflo|";
    } else if (cv & DTCVShowDBC) {
      retVal += "shdbc|";
    } else if (cv & DTCVStmtExprFmtC) {
      retVal += "sefmc|";
    } else if (cv & DTCVFuncFmtC) {
      retVal += "fufmc|";
    } else if (cv & DTCVDefEPSet) {
      retVal += "exept|";
    } else if (cv & DTCVDisableLog) {
      retVal += "dslog|";
    } // if
  } // if
  return retVal.c_str();
} // toStr

ostream& Utils::
uEmitString(ostream& o, const string& string_) const {
  BMDEBUG2("Utils::uEmitString", string_);
  o << "\"";
  for (hFUInt32 c(0); c < string_.size(); ++ c) {
    if (string_[c] == '\\') {
      o << "\\\\";
    } else if (string_[c] == '"') {
      o << "\\\"";
    } else if (!isprint(string_[c])) {
      if (!string_[c]) {
        o << "\\000";
      } else if (string_[c] == '\t') {
        o << "\\t";
      } else if (string_[c] == '\f') {
        o << "\\f";
      } else if (string_[c] == '\v') {
        o << "\\v";
      } else if (string_[c] == '\b') {
        o << "\\b";
      } else if (string_[c] == '\r') {
        o << "\\r";
      } else if (string_[c] == '\n') {
        o << "\\n";
      } else {
        o << "\\0x" << hex << (hSInt32)string_[c];
      } // if
    } else {
      o << string_[c];
    } // if
  } // for
  o << dec << "\\000\"";
  EMDEBUG0();
  return o;
} // Utils::uEmitString

hUInt32 Utils::
uGetMask_32(hFSInt8 start_, hFSInt8 end_) const {
  BMDEBUG3("Utils::uGetMask_32", start_, end_);
  REQUIREDMSG(start_ >= 0 && start_ <= 31 && end_ >= 0 && end_ <= 31 && start_ <= end_, 
      ieStrParamValuesDBCViolation);
  hUInt32 retVal(0);
  for (hFInt32 c(start_); c <= end_; ++ c) {
    retVal |= 1 << c;
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Utils::uGetMask_32

hUInt16 Utils::
uGetMask_16(hFSInt8 start_, hFSInt8 end_) const {
  BMDEBUG3("Utils::uGetMask_16", start_, end_);
  REQUIREDMSG(start_ >= 0 && start_ <= 15 && end_ >= 0 && end_ <= 15 && start_ <= end_, 
      ieStrParamValuesDBCViolation);
  hUInt32 retVal(0);
  for (hFInt32 c(start_); c <= end_; ++ c) {
    retVal |= 1 << c;
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Utils::uGetMask_16

hUInt8 Utils::
uGetMask_8(hFSInt8 start_, hFSInt8 end_) const {
  BMDEBUG3("Utils::uGetMask_8", start_, end_);
  REQUIREDMSG(start_ >= 0 && start_ <= 7 && end_ >= 0 && end_ <= 7 && start_ <= end_, 
      ieStrParamValuesDBCViolation);
  hUInt32 retVal(0);
  for (hFInt32 c(start_); c <= end_; ++ c) {
    retVal |= 1 << c;
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Utils::uGetMask_8

//! \brief Loads a new configuration from a text file.
//! \note It is a line based configuration file. Lines must be at most 2048 chars.
//! \todo L Design: Display a message if exec points are never hit.
//! \todo M Design: provide a kind of error message line/file name output for
//! asserts.
bool DTConfig::
LoadFromFile(const string& fileName) {
  DTLOG("DTConfig::LoadFromFile(" << fileName.c_str() << ") {" << dtlIndent << dtlEndl);
  ifstream configFile("dtconfig.cf2g");
  char currLine[2048];
  DTConfigVar currVar = DTCVInvalid;
  static map<string, DTConfigVar> varTypeLines;
  if (varTypeLines.empty()) {
    //! \attention Below texts are important do not change them,
    //!            they are used in the configuration files.
    varTypeLines["skip functions (exe.points):"] = DTCVSkipFuncs;
    varTypeLines["skip child calls of (exe.points):"] = DTCVSkipChildCalls;
    varTypeLines["dump prog at (exe.points):"] = DTCVDumpProg;
    varTypeLines["dump stats at end of (exe.points):"] = DTCVDumpStats;
    varTypeLines["dump prog at end of (exe.points):"] = DTCVDumpProgAtEnd;
    varTypeLines["max call depth (int[1000]):"] = DTCVMaxCallDepth;
    varTypeLines["show only (exe.points):"] = DTCVShowOnly;
    varTypeLines["show this pointer? (boolean/1/0[1]):"] = DTCVShowThis;
    varTypeLines["show file loc? (boolean/1/0[1]):"] = DTCVShowFileLoc;
    varTypeLines["show dbc? (boolean/1/0[1]):"] = DTCVShowDBC;
    varTypeLines["dump hwdesc at (exe.points):"] = DTCVDumpHWDesc;
    varTypeLines["use obj ids? (boolean/1/0[1]):"] = DTCVUseObjIds;
    varTypeLines["debug/trace to console? (boolean/1/0[1]):"] = DTCVDumpToConsole;
    varTypeLines["show stmt/expr in C/C++? (boolean/1/0[1]):"] = DTCVStmtExprFmtC;
    varTypeLines["show stmt/expr in multiline? (boolean/1/0[1]):"] = DTCVStmtExprFmtMultiLine;
    varTypeLines["dump cfg graphs at prog dump? (boolean/1/0[1]):"] = DTCVDumpCFGGraphs;
    varTypeLines["expand types? (boolean/1/0[1]):"] = DTCVExpandTypes;
    varTypeLines["show func in C/C++? (boolean/1/0[1]):"] = DTCVFuncFmtC;
    varTypeLines["define execution point set:"] = DTCVDefEPSet;
    varTypeLines["disable dtLog tracing? (boolean/1/0[1]):"] = DTCVDisableLog;
    varTypeLines["disable assert messages? (boolean/1/0[1]):"] = DTCVDisableMsg;
    varTypeLines["disable debug/trace? (boolean/1/0[1]):"] = DTCVDisableDT;
  } // if
  // First clear all the setting to default.
  bool epSetJustSeen = false;
  string epSetName;
  while (configFile.getline(currLine, 2047)) {
    string line(currLine);
    istringstream istrline(currLine);
    // Skip comment lines
    if (line[0] == '/' && line[1] == '/') {
      DTLOG("Comment line skipped: " << line << dtlEndl);
      continue;
    } // if
    if (varTypeLines.find(line) != varTypeLines.end()) {
      DTLOG("Command line encountered: " << line << dtlEndl);
      currVar = varTypeLines[line];
      epSetJustSeen = currVar == DTCVDefEPSet;
      continue;
    } // if
    DTLOG("Command argument line:" << line << dtlEndl);
    DTASSERT(line[0] == ' ' && line[1] == ' ' && line[2] != ' ',
        "There must be exactly 2 spaces in the beginning of the line:" << line);
    DTASSERT(currVar != DTCVInvalid, "Internal error!");
    //! \todo Handle empty lines also.
    line = line.substr(2); // chop two spaces.
    string::size_type trailingIndex = line.find_last_not_of("\t \n");
    line = trailingIndex == string::npos ? line : line.substr(0, trailingIndex+1); // chop trailing spaces.
    hFSInt8 varVal = -1;
    DTLOG("command= " << toStr(currVar) << " : " << line << dtlEndl);
    if (mVarType[currVar] == "strset") {
      DTASSERT(line.length() >= 2, "Line length is not sufficient: " << line);
      bool isSetMinus = *(line.end()-1) == '-';
      if (isSetMinus == true) {
        line.erase(line.end()-1);
      } // if
      DTASSERT(line[0] != '{' || mEPSets.find(line) != mEPSets.end(),
        "Use of undefined/unseen set is not allowed, first define the set: " << line);
      if (line[0] == '{') {
        if (isSetMinus == true) {
          DTLOG("Removing set " << line << " from " << toStr(currVar) << dtlEndl);
          for(set<string>::iterator it(mEPSets[line].begin()); it != mEPSets[line].end(); ++ it) {
            mStrSetVars[currVar].erase(*it);
          } // while
        } else {
          DTLOG("Inserting set " << line << " into " << toStr(currVar) << dtlEndl);
          mStrSetVars[currVar].insert(mEPSets[line].begin(), mEPSets[line].end());
        } // if
      } else {
        DTLOG("Inserting string " << line << " into " << toStr(currVar) << dtlEndl);
        mStrSetVars[currVar].insert(line);
      } // if
    } else if (currVar == DTCVDefEPSet) {
      DTASSERT(line.length() >= 2, "Line length is not sufficient: " << line);
      bool isSetMinus = *(line.end()-1) == '-';
      if (isSetMinus == true) {
        line.erase(line.end()-1);
      } // if
      if (epSetJustSeen == true) {
        DTASSERT(isSetMinus == false, "You may not use '-' in set definitions:" << line);
        epSetName = line;
        DTASSERT(line[0] == '{' && *(line.end()-1) == '}', "Set name must be enclosed in {} braces");
        epSetJustSeen = false;
        DTLOG("Name definition of set encountered: " << epSetName << dtlEndl);
        // Just make it present in the epSets
        mEPSets[epSetName] /* LIE */;
      } else {
        // do not recurse
        DTASSERT(line != epSetName, "You may not use the name of the defining set in its list: " << epSetName);
        // do not ignore undefined sets
        DTASSERT(line[0] != '{' || mEPSets.find(line) != mEPSets.end(), "Set is not defined:" << line);
        if (line[0] == '{') {
          if (isSetMinus == true) {
            DTLOG("Removing set " << line << " from " << epSetName << dtlEndl);
            for(set<string>::iterator it(mEPSets[line].begin()); it != mEPSets[line].end(); ++ it) {
              mEPSets[epSetName].erase(*it);
            } // for
          } else {
            DTLOG("Inserting set " << line << " into " << epSetName << dtlEndl);
            mEPSets[epSetName].insert(mEPSets[line].begin(), mEPSets[line].end());
          } // if
        } else {
          DTLOG("Inserting string " << line << " into " << epSetName << dtlEndl);
          mEPSets[epSetName].insert(line);
        } // if
      } // if
    } else if (currVar == DTCVMaxCallDepth) {
      istrline >> mMaxCallDepth;
    } else if (mVarType[currVar] == "bitvar") {
      istrline >> varVal;
      DTASSERT(varVal == 0 || varVal == 1, "Please use 0 or 1 for: " << line);
      if (varVal == 1) {
        *this += currVar;
      } else if (varVal == 0) {
        *this -= currVar;
      } // if
    } // if
  } // while
  // Dump the contents of maps
  map<DTConfigVar, set<string> >::iterator it(mStrSetVars.begin());
  while (it != mStrSetVars.end()) {
    set<string>::iterator strIt(it->second.begin());
    DTLOG(dtlIndent << toStr(it->first) << dtlEndl);
    while (strIt != it->second.end()) {
      DTLOG((*strIt++).c_str() << dtlEndl);
    } // while
    DTLOG(dtlUnindent << dtlEndl);
    ++ it;
  } // while
  DTLOG(dtlUnindent << dtlEndl << "} DTConfig::LoadFromFile" << dtlEndl);
  dtLogEnabled = *this != DTCVDisableLog;
  dtAssertMsgEnabled = *this != DTCVDisableMsg;
  dtEnabled = *this != DTCVDisableDT;
  if (*this == DTCVDumpToConsole) {
    debugTrace->mOutputFile = &cout;
    debugTrace->mOutputToCOut = true;
  } // if
  return true;
} // DTConfig::LoadFromFile

DTConfig::DTConfig() :
  mConfigVars(DTCVNotSet),
  mMaxCallDepth(1000)
{
  // Do not use DBC in global objects constructor.
  DTASSERT(smRefCount == 0, "DTConfig is a run-time singleton, you may not create more than one");
  ++ smRefCount;
  // set default values
  *this += DTCVShowThis;
  *this += DTCVShowDBC;
  *this += DTCVUseObjIds;
  *this += DTCVShowFileLoc;
  // *this += DTCVStmtExprFmtC;
  *this += DTCVStmtExprFmtMultiLine;
  *this += DTCVDumpCFGGraphs;
  *this += DTCVFuncFmtC;
  *this += DTCVDisableMsg;
  *this += DTCVDisableLog;
  *this += DTCVDisableDT;
  // set variable types
  mVarType[DTCVSkipFuncs] = "strset";
  mVarType[DTCVSkipChildCalls] = "strset";
  mVarType[DTCVDumpProg] = "strset";
  mVarType[DTCVDumpStats] = "strset";
  mVarType[DTCVDumpHWDesc] = "strset";
  mVarType[DTCVShowOnly] = "strset";
  mVarType[DTCVDumpProgAtEnd] = "strset";
  mVarType[DTCVShowThis] = "bitvar";
  mVarType[DTCVStmtExprFmtMultiLine] = "bitvar";
  mVarType[DTCVDumpCFGGraphs] = "bitvar";
  mVarType[DTCVUseObjIds] = "bitvar";
  mVarType[DTCVDumpToConsole] = "bitvar";
  mVarType[DTCVShowFileLoc] = "bitvar";
  mVarType[DTCVShowDBC] = "bitvar";
  mVarType[DTCVStmtExprFmtC] = "bitvar";
  mVarType[DTCVFuncFmtC] = "bitvar";
  mVarType[DTCVDisableLog] = "bitvar";
  mVarType[DTCVDisableMsg] = "bitvar";
  mVarType[DTCVDisableDT] = "bitvar";
  mVarType[DTCVExpandTypes] = "bitvar";
  mVarType[DTCVMaxCallDepth] = "int";
  mVarType[DTCVDefEPSet] = "other";
} // DTConfig::DTConfig

tristate::
tristate() :
  mValue(TV_Unknown)
{
} // tristate::tristate

tristate::
tristate(bool value_) :
  mValue(value_ == true ? TV_True : TV_False)
{
} // tristate::tristate

bool tristate::
isTrue() const {
  return mValue == TV_True;
} // tristate::isTrue

bool tristate::
isFalse() const {
  return mValue == TV_False;
} // tristate::isFalse

bool tristate::
isUnknown() const {
  return mValue == TV_Unknown;
} // tristate::isUnknown

bool tristate::
operator != (bool value_) const {
  REQUIREDMSG(mValue == TV_True || mValue == TV_False, ieStrDBCViolation);
  bool retVal((mValue == TV_True ? true : false) != value_);
  return retVal;
} // tristate::operator=

bool tristate::
operator == (bool value_) const {
  REQUIREDMSG(mValue == TV_True || mValue == TV_False, ieStrDBCViolation);
  bool retVal((mValue == TV_True ? true : false) == value_);
  return retVal;
} // tristate::operator=

void tristate::
operator = (bool value_) {
  mValue = value_ == true ? TV_True : TV_False;
} // tristate::operator=

bool tristate::
IsValid() const {
  REQUIREDMSG(mValue == TV_True || mValue == TV_False, ieStrDBCViolation);
  return true;
} // tristate::IsValid

void tristate::
setUnknown() {
  mValue = TV_Unknown;
} // tristate::setUnknown

bool tristate::
operator !() {
  REQUIREDMSG(mValue == TV_True || mValue == TV_False, ieStrDBCViolation);
  return mValue == TV_False;
} // tristate::operator!

tristate::
operator bool() const {
  REQUIREDMSG(mValue != TV_Unknown, ieStrDBCViolation);
  return mValue == TV_True;
} // tristate::operator bool

bool IDebuggable::
IsValid() const {
  REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
  return true;
} // IDebuggable::IsValid

char* IDebuggable::
debug(const DTContext& context_, ostream& toStr_) const {
  REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::debug");
  return 0;
} // IDebuggable::debug

void 
dbgPrint(IDebuggable* debuggable_) {
  REQUIREDMSG(debuggable_ != 0, ieStrNonNullParam);
  debuggable_->debug(DTContext(DTCCDumpProg), cout);
  cout << flush;
} // dbgPrint

DTContext::
DTContext(DTCCommand command_) :
  mCommand(command_),
  mFile("<file not set>"),
  mLine(-1)
{
} // DTContext::DTContext

DTContext::
DTContext(const DTContext& dtContext_) :
  mCommand(dtContext_.mCommand),
  mFile(dtContext_.mFile),
  mLine(dtContext_.mLine)
{
} // DTContext::DTContext

DTContext::
DTContext(DTCCommand command_, const string& file_, hFSInt32 line_) :
  mCommand(command_),
  mFile(file_),
  mLine(line_)
{
} // DTContext::DTContext

DTContext::
DTContext() :
  mCommand(DTCCNotSet),
  mFile("<DTContext file not set>"),
  mLine(-1)
{
} // DTContext::DTContext

SrcLoc::
SrcLoc() :
  mFile("<not set>"),
  mLine(0),
  mFileNum(-1),
  mColumn(0)
{
} // SrcLoc::SrcLoc

SrcLoc::
SrcLoc(const string& file_, hFSInt32 line_, hFSInt32 column_) :
  mFile(file_),
  mLine(line_),
  mColumn(column_),
  mFileNum(-1)
{
  if (smFileNums.find(file_) == smFileNums.end()) {
    smFileNums[mFile] = smFileNums.size();
  } // if
  mFileNum = smFileNums[mFile];
} // SrcLoc::SrcLoc

hFSInt32 Utils::
uIsPowOf2(const BigInt& value_) const {
  BMDEBUG2("Utils::uIsPowOf2", &value_);
  hFSInt32 retVal(-1);
  hFUInt32 lNumOfBits(value_.getNumOfBits());
  if (value_ == (1 << lNumOfBits)) {
    retVal = lNumOfBits;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Utils::uIsPowOf2

BigInt Utils::
uGetUpAligned(const BigInt& value_, hFUInt32 alignment_) const {
  BMDEBUG3("Utils::uGetUpAligned", &value_, alignment_);
  BigInt retVal(value_);
  if (alignment_ != 0) {
    hFUInt32 lRem(value_ % alignment_);
    if (lRem != 0) {
      retVal += alignment_ - lRem;
    } // if
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // Utils::uGetUpAligned

bool Utils::
uIsMultiples(const tBigInt& val0_, const tBigInt& val1_) const {
  BMDEBUG3("Utils::uIsMultiples", &val0_, &val1_);
  REQUIREDMSG(val0_ != 0 && val1_ != 0, ieStrParamValuesDBCViolation);
  bool retVal(false);
  if (val0_ > val1_) {
    retVal = (val0_ % val1_) == 0;
  } else {
    retVal = (val1_ % val0_) == 0;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Utils::uIsMultiples

hFUInt8 Utils::
uGetMinNumBytes(const tBigInt& value_) const {
  BMDEBUG2("Utils::uGetMinNumBytes", &value_);
  REQUIREDMSG(value_ > 0, ieStrParamValuesDBCViolation);
  hFUInt8 retVal(0);
  if (value_ < 0xff) {
    retVal = 1;
  } else if (value_ < 0xffff) {
    retVal = 2;
  } else if (value_ < 0xffffffff) {
    retVal = 4;
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Utils::uGetMinNumBytes

bool Utils::
uInRange(hFSInt32 value_, hFSInt32 low_, hFSInt32 high_) const {
  BMDEBUG4("Utils::uInRange", value_, low_, high_);
  REQUIREDMSG(high_ >= low_, ieStrParamValuesDBCViolation);
  bool retVal(value_ >= low_ && value_ <= high_);
  EMDEBUG1(retVal);
  return retVal;
} // Utils::uInRange

hFUInt32 Utils::
uGetUpAligned(hFUInt32 value_, hFUInt32 alignment_) const {
  BMDEBUG3("Utils::uGetUpAligned", value_, alignment_);
  hFUInt32 retVal(value_);
  if (alignment_ != 0) {
    hFUInt32 lRem(value_ % alignment_);
    if (lRem != 0) {
      value_ += alignment_ - lRem;
    } // if
  } // if
  EMDEBUG1(value_);
  return value_;
} // Utils::uGetUpAligned


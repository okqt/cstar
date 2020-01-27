// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __STATS_HPP__
#include "stats.hpp"
#endif

Statistics* stats;

Statistics::
Statistics() {
  mValidStats.insert("gfwdsubst: expr replaced");
  mValidStats.insert("lfwdsubst: expr replaced");
  mValidStats.insert("gcse: expr replaced");
  mValidStats.insert("gcse: expr found");
  mValidStats.insert("lcse: expr replaced");
  mValidStats.insert("lcse: expr found");
  mValidStats.insert("availexpr: panic");
  mValidStats.insert("lcopyprop: expr replaced");
  mValidStats.insert("gcopyprop: expr replaced");
  mValidStats.insert("jumpoptims: count");
  mValidStats.insert("straigthening: count");
  mValidStats.insert("removenops: count");
  mValidStats.insert("irbuilder: bb count");
  mValidStats.insert("irbuilder: edge change");
  mValidStats.insert("irbuilder: optim count");
  mValidStats.insert("cfg: loop count (last)");
  mValidStats.insert("cfg: dominators calculated");
  mValidStats.insert("cfg: loops calculated");
  mValidStats.insert("extract assign defines: panic");
  mValidStats.insert("extract call defines: panic");
  mValidStats.insert("extract pointer uses: panic");
  mValidStats.insert("extract expr uses: panic");
  mValidStats.insert("loopinvariant: div/mod");
  mValidStats.insert("loopinvariant: expr moved");
  mValidStats.insert("loopinvariant: stmt moved");
  mValidStats.insert("definition use chains: use without a def");
  mValidStats.insert("deadcode: stmt removed");
  mValidStats.insert("deadcode: expr changed to zero");
  mValidStats.insert("duud: calculated");
  mValidStats.insert("loopinversion: count");
  mValidStats.insert("inliner: call inlined");
  mValidStats.insert("constfold: count");
  mValidStats.insert("anatropman: execute on detached");
  mValidStats.insert("anatropman: trigger on detached");
  mValidStats.insert("constfold: div by zero skipped");
  mValidStats.insert("constfold: mod by zero skipped");
  mValidStats.insert("strength: count");
  mValidStats.insert("ifsimpls: not condition");
  mValidStats.insert("ifsimpls: condition merge 0");
  mValidStats.insert("ifsimpls: condition merge 1");
  mValidStats.insert("ifsimpls: condition merge 2");
  mValidStats.insert("ifsimpls: condition merge 3");
  mValidStats.insert("ifsimpls: const condition");
  mValidStats.insert("ifsimpls: same then/else block");
  mValidStats.insert("ifsimpls: empty then/else block");
  mValidStats.insert("ifsimpls: almost empty then/else block");
  // Insert DFAs below: place the name of DFA and ": calculated".
  mValidStats.insert("Inst Liveness analysis: calculated");
  mValidStats.insert("IR Reaching definitions analysis: calculated");
  mValidStats.insert("Inst Reaching definitions analysis: calculated");
  mValidStats.insert("IR Liveness analysis: calculated");
  mValidStats.insert("IR available copy assignments analysis: calculated");
  mValidStats.insert("IR available expressions analysis: calculated");
} // Statistics::Statistics

char* Statistics::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("Statistics::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpHWDesc) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpStats) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << DTCCIndent << "Statistics {" << ++ DTCCIndent << DTCCEndLine;
    set<string>::const_iterator lIt(mValidStats.begin());
    while (lIt != mValidStats.end()) {
      map<string, hFSInt32>::const_iterator lSIT(mStats.find(*lIt));
      if (lSIT == mStats.end()) {
        toStr_ << DTCCIndent << "<" << *lIt << ">= 0" << DTCCEndLine;
      } else {
        toStr_ << DTCCIndent << "<" << *lIt << ">=" << lSIT->second << DTCCEndLine;
      } // if
      ++ lIt;
    } // while
    toStr_ << -- DTCCIndent << DTCCIndent << "} // Statistics" << DTCCEndLine;
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // Statistics::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // Statistics::debug

void Statistics::
Reset(const string& what_) {
  BMDEBUG2("Statistics::Reset", what_);
  REQUIREDMSG(mValidStats.find(what_) != mValidStats.end(), ieStrParamValuesDBCViolation <<
      " Insert the what string as a valid statistic in ctor Statistics: '" << what_ << "'");
  mStats[what_] = 0;
  EMDEBUG0();
  return;
} // Statistics::Reset

void Statistics::
Record(const string& what_) {
  BMDEBUG2("Statistics::Record", what_);
  REQUIREDMSG(mValidStats.find(what_) != mValidStats.end(), ieStrParamValuesDBCViolation <<
      " Insert the what string as a valid statistic in ctor Statistics: '" << what_ << "'");
  ++ mStats[what_];
  EMDEBUG0();
  return;
} // Statistics::Record

void Statistics::
OnProgEnd(const string& fileNamePrefix_) {
  BMDEBUG2("Statistics::OnProgEnd", fileNamePrefix_);
  //! \todo M Design: Handle corrupt stat files.
  ofstream lStatFile(string(fileNamePrefix_ + ".stats").c_str());
  set<string>::const_iterator lIt(mValidStats.begin());
  while (lIt != mValidStats.end()) {
    map<string, hFSInt32>::const_iterator lSIT(mStats.find(*lIt));
    if (lSIT == mStats.end()) {
      lStatFile << "<" << *lIt << ">= 0" << endl;
    } else {
      lStatFile << "<" << *lIt << ">= " << lSIT->second << endl;
    } // if
    ++ lIt;
  } // while
  EMDEBUG0();
} // Statistics::OnProgEnd



// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_BBORDER_HPP__
#include "ato_bborder.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif

void ATOBBOrder::
InitOptions() {
  BMDEBUG1("ATOBBOrder::InitOptions");
  addEnumOption("type", "srcloc", Options::scDefault);
  addEnumOption("type", "minjumps");
  EMDEBUG0();
  return;
} // ATOBBOrder::InitOptions

hFInt32 ATOBBOrder::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATOBBOrder::AnatropDo(IRFunction*)", func_);
  hFInt32 retVal(0);
  if (GetOptions().oeGet("type") == "srcloc") {
    retVal += orderBySrcLoc(func_);
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOBBOrder::AnatropDo

hFSInt32 ATOBBOrder::
getMinLineNumber(const IRBB* bb_) const {
  BMDEBUG2("ATOBBOrder::getMinLineNumber", bb_);
  hFSInt32 retVal(0);
  bool lAnySet(false);
  ListIterator<IRStmt*> lStmtIter(const_cast<IRBB*>(bb_)->GetStmtIter());
  for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
    SrcLoc lSrcLoc(extr.eGetSrcLoc(*lStmtIter));
    if (pred.pIsSet(lSrcLoc) == true) {
      if (!lAnySet) {
        lAnySet = true;
        retVal = lSrcLoc.GetLineNum();
      } else {
        retVal = util.uMin(lSrcLoc.GetLineNum(), retVal);
      } // if
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal; 
} // ATOBBOrder::getMinLineNumber

struct CmpSrcLocs {
  CmpSrcLocs(map<IRBB*, hFSInt32>& lineNums_) : mLineNums(lineNums_) {}
  int operator ()(IRBB* bb0_, IRBB* bb1_) const {
    return mLineNums[bb0_] < mLineNums[bb1_];
  }
  map<IRBB*, hFSInt32>& mLineNums;
}; // class CmpSrcLocs

hFInt32 ATOBBOrder::
orderBySrcLoc(IRFunction* func_) {
  BMDEBUG2("ATOBBOrder::orderBySrcLoc", func_);
  hFUInt32 retVal(1);
  CFG* lCFG(func_->GetCFG());
  ListIterator<IRBB*> lBBIter(lCFG->GetPhysBBIter());
  vector<IRBB*> lBBs;
  map<IRBB*, hFSInt32> lLineNums;
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    if (*lBBIter != lCFG->GetEntryBB() && *lBBIter != lCFG->GetExitBB()) {
      lBBs.push_back(*lBBIter);
      lLineNums[*lBBIter] = getMinLineNumber(*lBBIter);
    }
  } // for
  sort(lBBs.begin(), lBBs.end(), CmpSrcLocs(lLineNums));
  lBBs.insert(lBBs.begin(), lCFG->GetEntryBB());
  lBBs.push_back(lCFG->GetExitBB());
  lCFG->SetPhysOrderOfBBs(lBBs);
  EMDEBUG1(retVal);
  return retVal;
} // ATOBBOrder::orderBySrcLoc



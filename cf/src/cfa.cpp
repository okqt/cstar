// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __DFA_HPP__
#include "dfa.hpp"
#endif
#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif

//! \file cfa.cpp
//! \brief This file contains the control flow analysis related implementations.

void CFG::
computeDominators() {
  BMDEBUG1("CFG::computeDominators");
  bool lChange(true);
  IRBB* lEntryBB(GetEntryBB());
  lEntryBB->mDoms.clear();
  lEntryBB->mDoms.insert(lEntryBB);
  lEntryBB->mIDom = 0;
  ListIterator<IRBB*> lBBIter(GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    if (*lBBIter != lEntryBB) {
      lBBIter->mDoms.clear();
      lBBIter->mDoms.insert(mBBs.begin(), mBBs.end());
      lBBIter->mIDom = 0;
    } // if
  } // for
  while (lChange == true) {
    lChange = false;
    for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
      if (*lBBIter != lEntryBB) {
        set<IRBB*> lTmp(mBBs.begin(), mBBs.end());
        vector<IRBB*> lPreds;
        extr.eGetPredsOfBB(*lBBIter, lPreds);
        for (hFUInt32 c(0); c < lPreds.size(); ++ c) {
          lTmp = util.intersect(lTmp, lPreds[c]->mDoms);
        } // for
        set<IRBB*> lTmp2(lTmp);
        lTmp2.insert(*lBBIter);
        if (lTmp2 != lBBIter->mDoms) {
          lChange = true;
          lBBIter->mDoms = lTmp2;
        } // if
      } // if
    } // for
  } // while
  EMDEBUG0();
  return;
} // CFG::computeDominators

void CFG::
updateDomTree() {
  BMDEBUG1("CFG::updateDomTree");
  if (!mDomTree) {
    anatropManager->Execute("UnreachableCode", GetFunc());
    STATRECORD("cfg: dominators calculated");
    mDomTree = new tree<IRBB*>(0);
    computeDominators();
    IRBB* lEntryBB(GetEntryBB());
    { // Compute the immediate dominators of basic blocks, see muchnick@184.
      map<IRBB*, set<IRBB*> > lBB2Tmp;
      ListIterator<IRBB*> nIter(GetPhysBBIter());
      for (nIter.First(); !nIter.IsDone(); nIter.Next()) {
        set<IRBB*>& lTmp(lBB2Tmp[*nIter]);
        lTmp = nIter->mDoms;
        lTmp.erase(*nIter);
      } // for
      for (nIter.First(); !nIter.IsDone(); nIter.Next()) {
        if (lEntryBB == *nIter) {
          continue;
        } // if
        vector<IRBB*> lToBeRemoved;
        set<IRBB*>& lnTmp(lBB2Tmp[*nIter]);
        for (set<IRBB*>::iterator lSIt(lnTmp.begin()); lSIt != lnTmp.end(); ++ lSIt) {
          for (set<IRBB*>::iterator lTIt(lnTmp.begin()); lTIt != lnTmp.end(); ++ lTIt) {
            if (*lTIt == *lSIt) {
              continue;
            } // if
            set<IRBB*>& lsTmp(lBB2Tmp[*lSIt]);
            if (lsTmp.find(*lTIt) != lsTmp.end()) {
              lToBeRemoved.push_back(*lTIt);
            } // if
          } // for
        } // for
        for (hFUInt32 f(0); f < lToBeRemoved.size(); ++ f) {
          lnTmp.erase(lToBeRemoved[f]);
        } // for
      } // for
      { // Assign the immediate dominators.
        ListIterator<IRBB*> nIter(GetPhysBBIter());
        for (nIter.First(); !nIter.IsDone(); nIter.Next()) {
          if (lEntryBB == *nIter) {
            continue;
          } // if
          if (extr.eGetNumOfPreds(*nIter) != 0) {
            set<IRBB*>& lTmp(lBB2Tmp[*nIter]);
            ASSERTMSG(lTmp.size() == 1, ieStrInconsistentInternalStructure << 
                " tmp size must have been reduced to 1: " << lTmp.size() << " bb:"<< refcxt(*nIter));
            nIter->mIDom = *lTmp.begin();
          } else {
            nIter->mIDom = 0;
          } // if
        } // for
      } // block
    } // block
  } // if
  EMDEBUG0();
  return;
} // CFG::updateDomTree

void CFG::
updatePDomTree() {
  BMDEBUG1("CFG::updatePDomTree");
  ASSERTMSG(0, ieStrNotImplemented);
  EMDEBUG0();
  return;
} // CFG::updatePDomTree

void CFG::
updateLoops() {
  BMDEBUG1("CFG::updateLoops");
  if (!mLoops) {
    STATRECORD("cfg: loops calculated");
    STATRESET("cfg: loop count (last)");
    mLoops = new vector<IRLoop*>;
    // first dominates second, these are natural loops.
    vector<pair<IRBB*, IRBB*> > lBackEdges;
    { // Find the back edges
      ListIterator<IRBB*> nIter(GetPhysBBIter());
      ListIterator<IRBB*> mIter(GetPhysBBIter());
      for (nIter.First(); !nIter.IsDone(); nIter.Next()) {
        for (mIter.First(); !mIter.IsDone(); mIter.Next()) {
          if (pred.pIsASucc(*nIter, *mIter) == true) {
            if (*nIter == *mIter || pred.pIsDom(*nIter, *mIter) == true) {
              lBackEdges.push_back(make_pair(*mIter, *nIter));
            } // if
          } // if
        } // for
      } // for
    } // block
    // Populate the loop bodies.
    for (hFUInt32 c(0); c < lBackEdges.size(); ++ c) {
      STATRECORD("cfg: loop count (last)");
      stack<IRBB*> lNodeStack;
      IRLoop* lCurrLoop(new IRLoop(this, 0));
      mLoops->push_back(lCurrLoop);
      // lBackEdges: first dominates second
      lCurrLoop->mHeader = lBackEdges[c].first;
      lCurrLoop->mBodyBBs.insert(lBackEdges[c].first);
      lCurrLoop->mBodyBBs.insert(lBackEdges[c].second);
      if (lBackEdges[c].first != lBackEdges[c].second) {
        lNodeStack.push(lBackEdges[c].second);
      } // if
      while (!lNodeStack.empty()) {
        IRBB* lCurrNode(lNodeStack.top());
        lNodeStack.pop();
        vector<IRBB*> lPreds;
        extr.eGetPredsOfBB(lCurrNode, lPreds);
        for (hFUInt32 d(0); d < lPreds.size(); ++ d) {
          if (lCurrLoop->mBodyBBs.find(lPreds[d]) == lCurrLoop->mBodyBBs.end()) {
            lCurrLoop->mBodyBBs.insert(lPreds[d]);
            lNodeStack.push(lPreds[d]);
          } // if
        } // for
      } // while
    } // for
    { // Determine the nesting information of loops.
      vector<IRLoop*>& lLoops(*mLoops);
      for (hFUInt32 c(1); c < lLoops.size(); ++ c) {
        for (hFUInt32 d(0); d < lLoops.size(); ++ d) {
          if (c > d) {
            if (lLoops[c]->mHeader == lLoops[d]->mHeader) {
              lLoops[c]->mSharedHeader.push_back(lLoops[d]);
              lLoops[d]->mSharedHeader.push_back(lLoops[c]);
            } else if (lLoops[c]->mBodyBBs.find(lLoops[d]->mHeader) != lLoops[c]->mBodyBBs.end()) {
              lLoops[c]->mNestedLoops.push_back(lLoops[d]);
              lLoops[d]->mParentLoop = lLoops[c];
            } // if
          } // if
        } // for
      } // for
    } // block
    { // Fill in the exits information of loops.
      vector<IRLoop*>& lLoops(*mLoops);
      for (hFUInt32 c(0); c < lLoops.size(); ++ c) {
        set<IRBB*>::iterator lBBIter(lLoops[c]->mBodyBBs.begin());
        set<IRBB*> lExitBBs;
        for (/* LIE */; lBBIter != lLoops[c]->mBodyBBs.end(); ++ lBBIter) {
          vector<IRBB*> lSuccs;
          extr.eGetSuccsOfBB(*lBBIter, lSuccs);
          for (hFUInt32 d(0); d < lSuccs.size(); ++ d) {
            if (lLoops[c]->IsInLoopBody(lSuccs[d]) == false) {
              lLoops[c]->mExitEdges.push_back(IREdge(*lBBIter, lSuccs[d]));
              lExitBBs.insert(lSuccs[d]);
            } // if
          } // for
        } // for
        lLoops[c]->mExitBBCount = lExitBBs.size();
      } // for
    } // block
  } // if
  EMDEBUG0();
  return;
} // CFG::updateLoops

void IRLoop::
updatePreHeader() {
  BMDEBUG1("IRLoop::updatePreHeader");
  if (!mPreHeader) {
    mPreHeader = irBuilder->irbInsertBB(ICBB(mHeader, ICBefore));
    // All predecessors in the loop should be retargeted to the header.
    vector<IRBB*> lPreds;
    extr.eGetPredsOfBB(mPreHeader, lPreds);
    for (hFUInt32 c(0); c < lPreds.size(); ++ c) {
      if (pred.pIsInLoop(this, lPreds[c]) == true) {
        bool lIsMapped(irBuilder->irbRemapJumpTarget(lPreds[c]->GetLastStmt(), mPreHeader, mHeader));
        ASSERTMSG(lIsMapped == true, ieStrInconsistentInternalStructure);
      } // if
    } // for
  } // if
  EMDEBUG1(mPreHeader);
  return;
} // IRLoop::updatePreHeader

bool IRLoop::
Accept(StmtVisitor& visitor_) {
  BMDEBUG1("IRLoop::Accept");
  bool retVal(true);
  if (visitor_.GetOptions().IsPreheaderNo() == false && mPreHeader != 0) {
    if (!mPreHeader->Accept(visitor_)) {
      retVal = false;
    } // if
  } // if
  if (retVal != false) {
    for (set<IRBB*>::iterator lIt(mBodyBBs.begin()); lIt != mBodyBBs.end(); ++ lIt) {
      IRBB* lBB(*lIt);
      if (!lBB->Accept(visitor_)) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRLoop::Accept

bool IRLoop::
Accept(ExprVisitor& visitor_) {
  BMDEBUG1("IRLoop::Accept");
  bool retVal(true);
  if (visitor_.GetOptions().IsPreheaderNo() == false && mPreHeader != 0) {
    if (!mPreHeader->Accept(visitor_)) {
      retVal = false;
    } // if
  } // if
  if (retVal != false) {
    for (set<IRBB*>::iterator lIt(mBodyBBs.begin()); lIt != mBodyBBs.end(); ++ lIt) {
      IRBB* lBB(*lIt);
      if (!lBB->Accept(visitor_)) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRLoop::Accept

bool IRLoop::
Accept(const CExprVisitor& visitor_) const {
  BMDEBUG1("IRLoop::Accept");
  bool retVal(true);
  if (visitor_.GetOptions().IsPreheaderNo() == false && mPreHeader != 0) {
    if (!mPreHeader->Accept(visitor_)) {
      retVal = false;
    } // if
  } // if
  if (retVal != false) {
    for (set<IRBB*>::const_iterator lIt(mBodyBBs.begin()); lIt != mBodyBBs.end(); ++ lIt) {
      IRBB* lBB(*lIt);
      if (!lBB->Accept(visitor_)) {
        retVal = false;
        break;
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // IRLoop::Accept

void IRLoop::
updateInductionVariables() {
  BMDEBUG1("IRLoop::updateInductionVariables");
  if (!mBasicIndVars) {
    mBasicIndVars = new vector<IRInductionVar*>;
    mDependentIVs = new vector<IRInductionVar*>;
    //! \todo L Design: Add assert of others is non-null.
    anatropManager->Execute("InductionVars", this, GenericOptions().obSet("execute_triggers", false));
  } // if
  EMDEBUG0();
  return;
} // IRLoop::updateInductionVariables


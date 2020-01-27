// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_DEADCODEELIM_HPP__
#include "ato_deadcodeelim.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __DFA_HPP__
#include "dfa.hpp"
#endif

//! \todo M Design: Deadcode elimination may lead to shortcut jumps/code straightening optims.

void ATODeadCodeElim::
getEssentialStmts(IRFunction* func_, list<IRStmt*>& worklist_) {
  BMDEBUG1("ATODeadCodeElim::getEssentialStmts");
  // Note that there is no need to mark prolog/epilog/jumps/labels.
  StmtCollector5<IRSIf, IRSSwitch, IRSReturn, IRSCall, IRSAssign, IRFunction> lCandidNonDead(func_, StmtVisitorOptions().SetMark2False());
  hFUInt32 c(0);
  for (c = 0; c < lCandidNonDead.mCollected0.size(); ++ c) {
    IRSIf* lCurrStmt(lCandidNonDead.mCollected0[c]);
    ExprCollector<IRObject> lUsedObjs(EVOTDoNotCare, lCurrStmt);
    bool lAdded(false);
    for (hFUInt32 d(0); d < lUsedObjs.size(); ++ d) {
      if (!pred.pIsLocal(lUsedObjs[d]) || pred.pIsAddrTaken(lUsedObjs[d]) == true) {
        lAdded = true;
        break;
      } // if
    } // for
    if (!pred.pHasObjUse(lCurrStmt)) {
      lAdded = true;
    } // if
    lCurrStmt->SetMark2(lAdded);
  } // for
  { // Assignments in to locals and params that are not addr taken are not essential, unmark them.
    for (c = 0; c < lCandidNonDead.mCollected4.size(); ++ c) {
      IRSAssign* lCurrStmt(lCandidNonDead.mCollected4[c]);
      set<IRObject*> lDefs;
      extr.eGetDefsOfStmt(lCurrStmt, lDefs);
      if (lDefs.size() == 1) {
        IRObject* lObj(*lDefs.begin());
        if (!pred.pIsGlobal(lObj) && pred.pIsAddrTaken(lObj) == false) {
          lCurrStmt->ResetMark();
        } // if
      } // if
    } // for
  } // block
  for (c = 0; c < lCandidNonDead.mCollected1.size(); ++ c) {
    IRSSwitch* lCurrStmt(lCandidNonDead.mCollected1[c]);
    ExprCollector<IRObject> lUsedObjs(EVOTDoNotCare, lCurrStmt);
    bool lAdded(false);
    for (hFUInt32 d(0); d < lUsedObjs.size(); ++ d) {
      if (!pred.pIsLocal(lUsedObjs[d]) || pred.pIsAddrTaken(lUsedObjs[d]) == true) {
        lAdded = true;
        break;
      } // if
    } // for
    lCurrStmt->SetMark2(lAdded);
  } // for
  for (c = 0; c < lCandidNonDead.mCollected2.size(); ++ c) worklist_.push_back(lCandidNonDead.mCollected2[c]);
  for (c = 0; c < lCandidNonDead.mCollected3.size(); ++ c) worklist_.push_back(lCandidNonDead.mCollected3[c]);
  for (c = 0; c < lCandidNonDead.mCollected4.size(); ++ c) worklist_.push_back(lCandidNonDead.mCollected4[c]);
  EMDEBUG1(worklist_.size());
  return;
} // ATODeadCodeElim::getEssentialStmts

void ATODeadCodeElim::
markStmts(list<IRStmt*>& worklist_) {
  BMDEBUG1("ATODeadCodeElim::markStmts");
  set<IRStmt*> lAlreadyProcessed;
  while (!worklist_.empty()) {
    IRStmt* lCurrStmt(worklist_.front());
    worklist_.pop_front();
    if (lAlreadyProcessed.find(lCurrStmt) == lAlreadyProcessed.end()) {
      lAlreadyProcessed.insert(lCurrStmt);
      ExprCollector<IRObject> lUsedObjs(EVOTDoNotCare, lCurrStmt, ExprVisitorOptions().SetLHSYes());
      for (hFUInt32 c(0); c < lUsedObjs.mCollected0.size(); ++ c) {
        IRObject* lUsedObj(lUsedObjs.mCollected0[c]);
        const set<IRStmt*>& lDefs(extr.eGetDefs(mDUUDChains, lCurrStmt, lUsedObj));
        for (set<IRStmt*>::const_iterator lIt(lDefs.begin()); lIt != lDefs.end(); ++ lIt) {
          IRStmt* lStmt(*lIt);
          PDEBUG(GetName(), "detail", " Statement is marked " << progcxt(lStmt));
          lStmt->SetMark();
          worklist_.push_back(lStmt);
        } // for
      } // for
      if (pred.pIsAssign(lCurrStmt) == true || pred.pIsFCall(lCurrStmt) == true) {
        PDEBUG(GetName(), "detail", " Considering assign " << progcxt(lCurrStmt));
        set<IRObject*> lDefs;
        extr.eGetDefsOfStmt(lCurrStmt, lDefs);
        if (!lDefs.empty()) {
          if (lDefs.size() != 1) {
            PDEBUG(GetName(), "detail", " Statement is marked " << progcxt(lCurrStmt));
            lCurrStmt->SetMark();
          } else {
            IRObject* lObj(*lDefs.begin());
            if (pred.pIsGlobal(lObj) == true || pred.pIsAddrTaken(lObj) == true) {
              PDEBUG(GetName(), "detail", " Statement is marked " << progcxt(lCurrStmt));
              lCurrStmt->SetMark();
            } // if
          } // if
        } // if
        for (set<IRObject*>::iterator lIt(lDefs.begin()); lIt != lDefs.end(); ++ lIt) {
          const set<IRStmt*>& lUses(extr.eGetUses(mDUUDChains, lCurrStmt, *lIt));
          for (set<IRStmt*>::iterator lUsesIt(lUses.begin()); lUsesIt != lUses.end(); ++ lUsesIt) {
            IRStmt* lUseStmt(*lUsesIt);
            if (pred.pIsIfOrSwitch(lUseStmt) == true) {
              PDEBUG(GetName(), "detail", " Statement is marked " << progcxt(lUseStmt));
              PDEBUG(GetName(), "detail", " assignment is marked " << progcxt(lCurrStmt));
              lUseStmt->SetMark();
              lCurrStmt->SetMark();
              worklist_.push_back(lUseStmt);
            } // if
          } // for
        } // for
      } // if
    } // if
  } // while
  EMDEBUG0();
  return;
} // ATODeadCodeElim::markStmts

void ATODeadCodeElim::
removeStmts(IRFunction* func_) {
  BMDEBUG2("ATODeadCodeElim::removeStmts", func_);
  ListIterator<IRStmt*> lStmtIter(extr.eGetStmtIter(func_));
  ASSERTMSG(irBuilder->irbClearToBeRemovedStmts() == false, ieStrInconsistentInternalStructure);
  for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
    IRStmt* lStmt(*lStmtIter);
    PDEBUG(GetName(), "detail", " statement considering for removal " << progcxt(lStmt));
    if (pred.pIsPrologOrEpilog(lStmt) == true || pred.pIsReturn(lStmt) == true || pred.pIsCall(lStmt) == true ||
        pred.pIsLabel(lStmt) == true || pred.pIsJump(lStmt) == true || pred.pIsDynJump(lStmt) == true) {
      // LIE: do nothing for those.
    } else if (pred.pIsNull(lStmt) == true) {
      irBuilder->irbToBeRemoved(lStmt);
      STATRECORDMSG("deadcode: stmt removed", progcxt(lStmt));
    } else if (lStmt->HasMarked() == true && lStmt->IsMarkFalse() == true) {
      if (pred.pIsIfOrSwitch(lStmt) == true) {
        //! \todo M Design: More optims for if cases.
        // Here we have an if/switch that uses an object and stmt is marked as
        // dead. We will assume condition expression for if/switch is
        // constant zero. We could actually eliminate both branch paths of if.
        STATRECORDMSG("deadcode: expr changed to zero", progcxt(lStmt));
        IRExpr* lExpr(extr.eGetChild(lStmt));
        lExpr->ReplaceWith(irBuilder->irbeZero(lExpr->GetType()));
      } else {
        irBuilder->irbToBeRemoved(lStmt);
        STATRECORDMSG("deadcode: stmt removed", progcxt(lStmt));
      } // if
    } // if
  } // for
  irBuilder->irbRemoveToBeRemovedStmts();
  EMDEBUG0();
  return;
} // ATODeadCodeElim::removeStmts

hFInt32 ATODeadCodeElim::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("ATODeadCodeElim::AnatropDo(IRFunction*)", func_);
  //! \todo M Design: handle the labels that are not used by jumps or computed jumps.
  hFInt32 retVal(0);
  list<IRStmt*> lWorklist;
  mDUUDChains = func_->GetCFG()->GetDUUDChains();
  getEssentialStmts(func_, lWorklist);
  markStmts(lWorklist);
  removeStmts(func_);
  DEBUGMODE(func_->GetCFG()->ResetStmtMarks(eIRTMGeneric));
  EMDEBUG1(retVal);
  return retVal;
} // ATODeadCodeElim::AnatropDo


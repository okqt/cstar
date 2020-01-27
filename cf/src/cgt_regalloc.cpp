// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __DFA_HPP__
#include "dfa.hpp"
#endif

void RegisterAllocator::
BuildInterferenceGraph(CGFuncContext& context_) {
  BMDEBUG1("RegisterAllocator::BuildInterferenceGraph");
  DFAInstLiveness lLiveness(context_.GetFunc());
  DFAInstReachingDefs lReachingDefs(context_.GetFunc());
  lLiveness.Do();
  lReachingDefs.Do();
  set<VirtReg*>& lVirtRegs(lLiveness.mVirtRegsSeen);
  { // allocate space and initialize the adjacency matrix.
    mAdjacencyMatrix.resize(lVirtRegs.size(), vector<bool>(lVirtRegs.size(), false));
    hFUInt32 lIndex(0);
    for (set<VirtReg*>::iterator lIt(lVirtRegs.begin()); lIt != lVirtRegs.end(); ++ lIt) {
      (*lIt)->mAdjMatrixIndex = lIndex++;
    } // for
  } // block
  // Iterate over rows and columns of adjacency matrix.
  for (set<VirtReg*>::iterator lCit(lVirtRegs.begin()); lCit != lVirtRegs.end(); ++ lCit) {
    for (set<VirtReg*>::iterator lRit(lVirtRegs.begin()); lRit != lCit; ++ lRit) {
      if (isInterfere(&lLiveness, *lCit, *lRit) == true) {
      } // if
    } // for
  } // for
  EMDEBUG0();
  return;
} // RegisterAllocator::BuildInterferenceGraph

bool RAGreedy::
Start(CGFuncContext& context_) {
  BMDEBUG1("RAGreedy::Start");
  SetFuncContext(context_);
  RegisterAllocator::BuildInterferenceGraph(context_);
  EMDEBUG0();
  return true;
} // RAGreedy::Start

void RegisterAllocator::
AddToAvailableRegs(CGFuncContext& context_, HWReg* reg_) {
  BMDEBUG2("RegisterAllocator::AddToAvailableRegs", reg_);
  context_.mAvailableRegs |= reg_;
  EMDEBUG0();
  return;
} // RegisterAllocator::AddToAvailableRegs

// Go thru all the available hw regs and ask each reg alloc rule 
// if a picked available HW reg satisfies the rule. The first
// hw reg in available regs that satisfies all the reg alloc rules is returned.
HWReg* RegisterAllocator::
GetAnAvailableReg(CGFuncContext& context_, VirtReg* vReg_) {
  BMDEBUG1("RegisterAllocator::GetAnAvailableReg");
  REQUIREDMSG(pred.pIsInvalid(vReg_->GetHWReg()) == true, ieStrParamValuesDBCViolation);
  HWReg* retVal(0);
  // lRules: all the rules that must hold true for vReg_.
  const vector<RegAllocRule*>& lRules(vReg_->GetRegAllocRules());
  const set<Register*>& lAvailRegs(context_.mAvailableRegs.GetRegs());
  for (set<Register*>::const_iterator lRit(lAvailRegs.begin()); lRit != lAvailRegs.end(); ++ lRit) {
    bool lRulesOk(true);
    for (hFUInt32 c(0); c < lRules.size(); ++ c) {
      if (!lRules[c]->CanHWRegBeAllocated(static_cast<HWReg*>(*lRit), vReg_, context_.mAvailableRegs)) {
        lRulesOk = false;
        break;
      } // if
    } // for
    if (lRulesOk == true) {
      retVal = static_cast<HWReg*>(*lRit);
      break;
    } // if
  } // for
  if (retVal != 0) {
    context_.mAvailableRegs -= retVal;
  } else {
    // Spill case not implemented.
    NOTIMPLEMENTED(H);
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  ENSUREMSG(retVal != 0, ieStrDBCEnsureViolation);
  EMDEBUG1(retVal);
  return retVal;
} // RegisterAllocator::GetAnAvailableReg

bool RegisterAllocator::
isInterfere(DFAInstLiveness* dfaLiveness_, VirtReg* vReg0_, VirtReg* vReg1_) {
  bool retVal(false);
  // Is vReg1_ live at program point inst?
  NOTIMPLEMENTED(H);
  return retVal;
} // RegisterAllocator::isInterfere

bool RALocal::
Start(CGFuncContext& context_) {
  //! \todo H Design: Find a solution to this! RA dump needs function context
  //!       to be set, but try to do it after BMDEBUG.
  SetFuncContext(context_);
  BMDEBUG1("RALocal::Start");
  // - The virtual reg must be death at the start and end of the basic block.
  //   The ones that are alive should be handled by global register allocator.
  //   We need DFAInstLiveness to determine if a vreg is live or death.
  // - We need to assign a register even if there is only the definition
  //   (without a use), or a use without a definition. Although in
  //   optimized code we should not have these cases, for debug mode code
  //   generation we need to handle them.
  // Algorithm:
  // 0- Setup liveness and reaching defs DFAs.
  // 1- Visit each inst backwards in bb,
  // 2- For all uses of inst:
  // 2a- if not death at begin and end of bb skip this use.
  // 2b- if not defined in bb assign any hw reg.
  // 2c- Allocate next available hwreg for this use's vreg.
  //     Remove hwreg from available list.
  //     Do this for already assigned vregs, i.e. only remove it from
  //     available list.
  // 3- For all defs of inst:
  // 3a- Place the hwreg back in to available list.
  // 4- Now visit all insts: for definitions that are not assigned a hwreg
  //    and death at end of bb_ assign an available reg, for each such
  //    definition assign the same hw register. Put the available reg back in
  //    to availables list.
  //! \todo H Design: You can get this analysis results during construction.
  DFAInstLiveness lLiveness(context_.GetFunc());
  DFAInstReachingDefs lReachingDefs(context_.GetFunc());
  lLiveness.Do();
  lReachingDefs.Do();
  CFG* lCFG(context_.GetFunc()->GetCFG());
  ListIterator<IRBB*> lBBIter(lCFG->GetPhysBBIter());
  HWDescription* lHWDesc(context_.GetHWDesc());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    CGBB* lCGBB(static_cast<CGBB*>(*lBBIter));
    vector<AsmSequenceItem*> lInsts;
    lCGBB->GetInsts(lInsts);
    for (hFSInt32 c(lInsts.size()-1); c >= 0; -- c) {
      if (Instruction* lInst = dynamic_cast<Instruction*>(lInsts[c])) {
        // We must first do the defs.
        const set<Register*> lDefs(lInst->GetDefs().GetRegs());
        for (set<Register*>::const_iterator lDit(lDefs.begin()); lDit != lDefs.end(); ++ lDit) {
          HWReg* lHWReg(extr.eGetHWReg(*lDit));
          // Assume that hard coded registers need not be in registers
          // available for register allocator.
          if (lHWReg != 0 && lHWDesc->GetRegsForRegAlloc().HasReg(lHWReg)) {
            AddToAvailableRegs(context_, lHWReg);
          } // if
        } // for
        const set<Register*> lUses(lInst->GetUses().GetRegs());
        for (set<Register*>::const_iterator lUit(lUses.begin()); lUit != lUses.end(); ++ lUit) {
          HWReg* lHWReg(extr.eGetHWReg(*lUit));
          if (lLiveness.IsLiveAtBeg(lCGBB, lInst, *lUit) ||
              lLiveness.IsLiveAtEnd(lCGBB, lInst, *lUit)) {
            /* LIE */
          } else if (lHWReg != 0) {
            RemoveFromAvailables(context_, lHWReg);
          } else if (!lReachingDefs.IsDefinedIn(lCGBB, *lUit)) {
            alloc_regs:
            ASSERTMSG(dynamic_cast<VirtReg*>(*lUit), ieStrInconsistentInternalStructure);
            VirtReg* lVReg(static_cast<VirtReg*>(*lUit));
            HWReg* lHWReg(GetAnAvailableReg(context_, lVReg));
            lVReg->SetHWReg(lHWReg);
          } else {
            goto alloc_regs;
          } // if
        } // for
      } // if
    } // for
    // Find definitions that has no use in the bb_.
    for (hFUInt32 d(0); d < lInsts.size(); ++ d) {
      if (Instruction* lInst = dynamic_cast<Instruction*>(lInsts[d])) {
        const set<Register*> lDefs(lInst->GetDefs().GetRegs());
        for (set<Register*>::const_iterator lDit(lDefs.begin()); lDit != lDefs.end(); ++ lDit) {
          HWReg* lHWReg(extr.eGetHWReg(*lDit));
          if (lHWReg != 0) {
            /* LIE: Register is already assigned */
          } else if (!lReachingDefs.IsDefinedAtEnd(lCGBB, *lDit)) {
            ASSERTMSG(dynamic_cast<VirtReg*>(*lDit), ieStrInconsistentInternalStructure);
            VirtReg* lVReg(static_cast<VirtReg*>(*lDit));
            HWReg* lHWReg(GetAnAvailableReg(context_, lVReg));
            AddToAvailableRegs(context_, lHWReg);
            lVReg->SetHWReg(lHWReg);
          } // if
        } // for
      } // if
    } // for
  } // for
  EMDEBUG0();
  return true;
} // RALocal::Start


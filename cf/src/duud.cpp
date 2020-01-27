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

//! \file duud.cpp
//! \brief This file contains the definition-use/use-definition chain related implementations.

void CFG::
updateDUUDChains() {
  BMDEBUG1("CFG::updateDUUDChains");
  if (!mDUUDChains) {
    mDUUDChains = new IRDUUDChains(this);
    STATRECORD("duud: calculated");
    // Gather definitions by DFA and traverse the bbs for uses.
    DFAIRReachingDefs lDFAIRReachDefs(GetFunc());
    lDFAIRReachDefs.Do();
    { // Fill in all definition points.
      const DFAIRReachingDefs::TopBitVector& lAllDefs(lDFAIRReachDefs.GetTopVector());
      for (hFUInt32 c(0); c < lAllDefs.size(); ++ c) {
        mDUUDChains->mAllDefsStmt.insert(lAllDefs[c]->GetProgPoint());
        mDUUDChains->mObj2AllDefs[lAllDefs[c]->GetData()].insert(lAllDefs[c]->GetProgPoint());
      } // for
    } // block
    ListIterator<IRBB*> lBBIter(GetPhysBBIter());
    for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
      DFAIRReachingDefs::TopBitVector lBBDefs;
      lDFAIRReachDefs.GetDefinesInSet(*lBBIter, lBBDefs);
      FwdListIterator<IRStmt*> lStmtIter(lBBIter->GetStmtIter());
      for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
        { // First handle uses
          set<IRObject*> lStmtUses;
          extr.eGetUsesOfStmt(*lStmtIter, lStmtUses);
          PDEBUG("duud", "detail", "checking uses of stmt: " << progcxt(*lStmtIter) << " uses#=" << lStmtUses.size());
          for (set<IRObject*>::iterator lUseIt(lStmtUses.begin()); lUseIt != lStmtUses.end(); ++ lUseIt) {
            IRObject* lObj(*lUseIt);
            bool lADefFound(false);
            mDUUDChains->mObj2AllUses[lObj].insert(*lStmtIter);
            PDEBUG("duud", "detail", "checking use of stmt: " << progcxt(lObj) << " #indefs=" << lBBDefs.size());
            for (hFUInt32 c(0); c < lBBDefs.size(); ++ c) {
              PDEBUG("duud", "detail", "checking bb def[" << c << "]:" << progcxt(lBBDefs[c]->GetData()) << " <=> " << progcxt(lObj));
              if (!lBBDefs[c]->IsBottom() && lBBDefs[c]->GetData() == lObj) {
                lADefFound = true;
                IRStmt* lDefStmt(lBBDefs[c]->GetProgPoint());
                mDUUDChains->mObj2Defs2UsesStmt[lObj][lDefStmt].insert(*lStmtIter);
                mDUUDChains->mObj2Uses2DefsStmt[lObj][*lStmtIter].insert(lDefStmt);
                PDEBUG("duud", "detail", "set the definition of use of stmt: " << progcxt(lDefStmt));
              } // if
            } // for
            if (!lADefFound) {
              //! \todo M Design: Here we have a use that is not defined.
              // this may create a different program behaviour than a
              // debug version.
              STATRECORDMSG("definition use chains: use without a def", progcxt(lObj));
            } // if
          } // for
        } // block
        { // second handle the definitions of this statement.
          set<IRObject*> lStmtDefs;
          extr.eGetDefsOfStmt(*lStmtIter, lStmtDefs);
          // Remove all definitions in lBBDefs that matches to these
          // statement's definitions. And add the new defs.
          PDEBUG("duud", "detail", "checking defs of stmt: " << progcxt(*lStmtIter) << " defs#=" << lStmtDefs.size());
          for (set<IRObject*>::iterator lDefIt(lStmtDefs.begin()); lDefIt != lStmtDefs.end(); ++ lDefIt) {
            PDEBUG("duud", "detail", "Handling definitions of obj @ stmt: " << progcxt(*lStmtIter) << progcxt(*lDefIt));
            for (hFUInt32 c(0); c < lBBDefs.size(); ++ c) {
              if (lBBDefs[c]->GetData() == *lDefIt && lStmtDefs.size() == 1) {
                PDEBUG("duud", "detail", " killing a previous def : " << progcxt(*lStmtIter) << " <-> " << progcxt(*lDefIt));
                lBBDefs[c]->MakeBottom();
              } // if
            } // for
            PDEBUG("duud", "detail", " adding definition " << progcxt(*lStmtIter) << " <-> " << progcxt(*lDefIt));
            mDUUDChains->mAllDefsStmt.insert(*lStmtIter);
            mDUUDChains->mObj2AllDefs[*lDefIt].insert(*lStmtIter);
            lBBDefs.push_back(new DFAIRReachingDefs::TopElemType(*lStmtIter, *lDefIt, lBBDefs.size()));
          } // for
        } // block
      } // for
    } // for
  } // if
  EMDEBUG0();
  return;
} // CFG::updateDUUDChains



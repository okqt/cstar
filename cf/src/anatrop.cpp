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
#ifndef __ATO_STRENGTHREDUCE_HPP__
#include "ato_strengthreduce.hpp"
#endif
#ifndef __ATO_LOOPINVERSION_HPP__
#include "ato_loopinversion.hpp"
#endif
#ifndef __ATO_CONSTFOLD_HPP__
#include "ato_constfold.hpp"
#endif
#ifndef __ATO_INLINER_HPP__
#include "ato_inliner.hpp"
#endif
#ifndef __ATO_REMOVENOPS_HPP__
#include "ato_removenops.hpp"
#endif
#ifndef __ATO_CODESTRAIGTHENING_HPP__
#include "ato_codestraigthening.hpp"
#endif
#ifndef __ATO_UNREACHABLECODE_HPP__
#include "ato_unreachablecode.hpp"
#endif
#ifndef __ATO_GFORWARDSUBST_HPP__
#include "ato_gforwardsubst.hpp"
#endif
#ifndef __ATO_LFORWARDSUBST_HPP__
#include "ato_lforwardsubst.hpp"
#endif
#ifndef __ATO_IVELIMINATION_HPP__
#include "ato_ivelimination.hpp"
#endif
#ifndef __ATO_IVSTRENGTHREDUCTION_HPP__
#include "ato_ivstrengthreduction.hpp"
#endif
#ifndef __ATO_IFSIMPLS_HPP__
#include "ato_ifsimpls.hpp"
#endif
#ifndef __ATO_REMCASTS_HPP__
#include "ato_remcasts.hpp"
#endif
#ifndef __ATO_LOOPINVARIANT_HPP__
#include "ato_loopinvariant.hpp"
#endif
#ifndef __ATO_INDUCTIONVARS_HPP__
#include "ato_inductionvars.hpp"
#endif
#ifndef __ATO_LCSE_HPP__
#include "ato_lcse.hpp"
#endif
#ifndef __ATO_LCOPYPROP_HPP__
#include "ato_lcopyprop.hpp"
#endif
#ifndef __ATO_DEADCODEELIM_HPP__
#include "ato_deadcodeelim.hpp"
#endif
#ifndef __ATO_GCOPYPROP_HPP__
#include "ato_gcopyprop.hpp"
#endif
#ifndef __ATO_GCSE_HPP__
#include "ato_gcse.hpp"
#endif
#ifndef __ATO_LOWER_HPP__
#include "ato_lower.hpp"
#endif
#ifndef __ATO_BBORDER_HPP__
#include "ato_bborder.hpp"
#endif
#ifndef __ATO_CALLCONV_HPP__
#include "ato_callconv.hpp"
#endif
#ifndef __ATO_JUMPOPTIMS_HPP__
#include "ato_jumpoptims.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

AnatropManager* anatropManager(0);

Anatrop::
Anatrop(const AnatropScope& scope_) :
  mScope(scope_)
{
  mOptions.addBoolOption("execute_triggers", options.obGet("execute_triggers"));
} // Anatrop::Anatrop

hFInt16 Anatrop::
AnatropDo(IRProgram* prog_) {
  BMDEBUG2("Anatrop::AnatropDo(IRProgram*)", prog_);
  hFInt32 retVal(0);
  ListIterator<IRModule*> lModuleIter(prog_->GetModuleIter());
  for (lModuleIter.First(); !lModuleIter.IsDone(); lModuleIter.Next()) {
    OnStart(*lModuleIter);
    retVal += AnatropDo(*lModuleIter);
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Anatrop::AnatropDo

hFInt16 Anatrop::
AnatropDo(IRModule* module_) {
  BMDEBUG2("Anatrop::AnatropDo(IRModule*)", module_);
  hFInt32 retVal(0);
  ListIterator<IRFunction*> lFuncIter(module_->GetFuncIter());
  for (lFuncIter.First(); !lFuncIter.IsDone(); lFuncIter.Next()) {
    OnStart(*lFuncIter);
    retVal += AnatropDo(*lFuncIter);
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Anatrop::AnatropDo

hFInt16 Anatrop::
AnatropDo(IRFunction* func_) {
  BMDEBUG2("Anatrop::AnatropDo(IRFunction*)", func_);
  hFInt32 retVal(0);
  if (IsValidScope(&Singleton<IRBB>::Obj()) == true) {
    vector<IRBB*> lBBs;
    extr.eGetBBs(func_->GetCFG(), lBBs);
    for (hFUInt32 c(0); c < lBBs.size(); ++ c) {
      if (pred.pIsDetached(lBBs[c]) == false) {
        OnStart(lBBs[c]);
        retVal += AnatropDo(lBBs[c]);
      } // if
    } // for
  } else if (IsValidScope(&Singleton<IRLoop>::Obj()) == true) {
    const vector<IRLoop*>& lLoops(extr.eGetLoops(func_->GetCFG()));
    for (hFUInt32 c(0); c < lLoops.size(); ++ c) {
      OnStart(lLoops[c]);
      retVal += AnatropDo(lLoops[c]);
    } // for
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);   
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Anatrop::AnatropDo

hFInt16 Anatrop::
AnatropDo(IRBB* bb_) {
  BMDEBUG2("Anatrop::AnatropDo(IRBB*)", bb_);
  hFInt32 retVal(0);
  if (pred.pIsDetached(bb_) == false) {
    vector<IRStmt*> lStmts;
    extr.eGetStmts(bb_, lStmts);
    for (hFUInt32 c(0); c < lStmts.size(); ++ c) {
      if (pred.pIsDetached(lStmts[c]) == false) {
        OnStart(lStmts[c]);
        retVal += AnatropDo(lStmts[c]);
      } // if
    } // for
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // Anatrop::AnatropDo

hFInt16 Anatrop::
AnatropDo(IRLoop* loop_) {
  BMDEBUG2("Anatrop::AnatropDo(IRLoop*)", loop_);
  hFInt32 retVal(0);
  const set<IRBB*>& lBodyBBs(loop_->GetBodyBBs());
  for (set<IRBB*>::const_iterator lBBIter(lBodyBBs.begin()); lBBIter != lBodyBBs.end(); ++ lBBIter) {
    if (pred.pIsDetached(*lBBIter) == false) {
      OnStart(*lBBIter);
      retVal += AnatropDo(*lBBIter);
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // Anatrop::AnatropDo

hFInt16 Anatrop::
AnatropDo(IRStmt* stmt_) {
  REQUIREDMSG(0, ieStrFuncShouldNotBeCalled << " AT/Optimization cannot be applied to statements." <<
      endl << "You probably have a ctor for IRStmt but not the corresponding" <<
      "AnatropDo for the statement.");
  return 0;
} // Anatrop::AnatropDo

hFInt32 Anatrop::
Do() {
  BMDEBUG1("Anatrop::Do");
  hFInt32 retVal(-1);
  if (mScope.IsFunc() == true) {
    OnStart(mScope.GetFunc());
    retVal = AnatropDo(mScope.GetFunc());
  } else if (mScope.IsModule() == true) {
    OnStart(mScope.GetModule());
    retVal = AnatropDo(mScope.GetModule());
  } else if (mScope.IsProg() == true) {
    OnStart(mScope.GetProg());
    retVal = AnatropDo(mScope.GetProg());
  } else if (mScope.IsBB() == true) {
    if (pred.pIsDetached(mScope.GetBB()) == false) {
      OnStart(mScope.GetBB());
      retVal = AnatropDo(mScope.GetBB());
    } // if
  } else if (mScope.IsLoop() == true) {
    OnStart(mScope.GetLoop());
    retVal = AnatropDo(mScope.GetLoop());
  } else if (mScope.IsStmt() == true) {
    OnStart(mScope.GetStmt());
    retVal = AnatropDo(mScope.GetStmt());
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  OnFinish();
  EMDEBUG1(retVal);
  return retVal;
} // Anatrop::Do

void AnatropManager::
ProcessOption(const string& optText_) {
  BMDEBUG2("AnatropManager::ProcessOption", optText_);
  EMDEBUG0();
  return;
} // AnatropManager::processOption

void AnatropManager::
onAnatropExecution(Anatrop* parent_, Anatrop* anatrop_) {
  BMDEBUG2("AnatropManager::onAnatropExecution", anatrop_);
  EMDEBUG0();
  return;
} // AnatropManager::onAnatropExecution

void AnatropManager::
RegisterAnatrop(Anatrop* anatrop_) {
  BMDEBUG2("AnatropManager:RegisterAnatrop", anatrop_);
  REQUIREDMSG(name2Anatrop.find(anatrop_->GetName()) == name2Anatrop.end(), ieStrParamValuesDBCViolation);
  name2Anatrop[anatrop_->GetName()] = anatrop_;
  EMDEBUG0();
  return;
} // AnatropManager:RegisterAnatrop

void AnatropManager::
RegisterAnatrops() {
  BMDEBUG1("AnatropManager::RegisterAnatrops");
  REQUIREDCALLONCE();
  // Order of registration does not matter.
  RegisterAnatrop(&Singleton<ATOIfSimplifications>::Obj());
  RegisterAnatrop(&Singleton<ATORemoveCasts>::Obj());
  RegisterAnatrop(&Singleton<ATOLCopyProp>::Obj());
  RegisterAnatrop(&Singleton<ATOGCopyProp>::Obj());
  RegisterAnatrop(&Singleton<ATOStrengthReduce>::Obj());
  RegisterAnatrop(&Singleton<ATOLoopInversion>::Obj());
  RegisterAnatrop(&Singleton<ATOLCSE>::Obj());
  RegisterAnatrop(&Singleton<ATOInliner>::Obj());
  RegisterAnatrop(&Singleton<ATOGCSE>::Obj());
  RegisterAnatrop(&Singleton<ATODeadCodeElim>::Obj());
  RegisterAnatrop(&Singleton<ATORemoveNops>::Obj());
  RegisterAnatrop(&Singleton<ATOLowerHIR>::Obj());
  RegisterAnatrop(&Singleton<ATOLoopInvariant>::Obj());
  RegisterAnatrop(&Singleton<ATOCallConv>::Obj());
  RegisterAnatrop(&Singleton<ATOBBOrder>::Obj());
  RegisterAnatrop(&Singleton<ATOJumpOptims>::Obj());
  RegisterAnatrop(&Singleton<ATOCodeStraigthening>::Obj());
  RegisterAnatrop(&Singleton<ATOLForwardSubstitution>::Obj());
  RegisterAnatrop(&Singleton<ATOGForwardSubstitution>::Obj());
  RegisterAnatrop(&Singleton<ATOInductionVars>::Obj());
  RegisterAnatrop(&Singleton<ATOIVStrengthReduction>::Obj());
  RegisterAnatrop(&Singleton<ATOIVElimination>::Obj());
  RegisterAnatrop(&Singleton<ATOUnreachableCode>::Obj());
  RegisterAnatrop(&Singleton<ATOConstFold>::Obj());
  EMDEBUG0();
  return;
} // AnatropManager::RegisterAnatrops

Anatrop* AnatropManager::
CreateAnatrop(const string& name_, const AnatropScope& scope_) {
  BMDEBUG2("AnatropManager::CreateAnatrop", name_);
  REQUIREDMSG(name2Anatrop.find(name_) != name2Anatrop.end(), ieStrParamValuesDBCViolation);
  Anatrop* retVal(name2Anatrop[name_]->Clone(scope_));
  retVal->InitOptions();
  REQUIREDMSG(retVal->IsValidScope(scope_) == true, ieStrParamValuesDBCViolation);
  EMDEBUG1(retVal);
  return retVal;
} // AnatropManager::CreateAnatrop

hFUInt32 AnatropManager::
Execute(const string& atoName_, const AnatropScope& scope_, const GenericOptions& options_) {
  BMDEBUG3("AnatropManager::Execute", atoName_, &options_);
  bool retVal(0);
  if (scope_.IsDetached() == false) {
    Anatrop* lAnatrop(CreateAnatrop(atoName_, scope_));
    lAnatrop->GetOptions().Set(options_);
    lAnatrop->OnOptionsSet();
    mExecutionStack.push(lAnatrop);
    retVal = lAnatrop->Do();
    if (options.obGet("execute_triggers") == true && lAnatrop->GetOptions().obGet("execute_triggers") == true) {
      mTriggers.insert(mTriggers.end(), lAnatrop->mTriggers.begin(), lAnatrop->mTriggers.end());
    } // if
    mExecutionStack.pop();
    if (mExecutionStack.empty() == true) {
      while (!mTriggers.empty()) {
        vector<Anatrop*> lTriggers(mTriggers);
        mTriggers.clear();
        for (hFUInt32 c(0); c < lTriggers.size(); ++ c) {
          retVal += lTriggers[c]->Do();
        } // if
      } // while
    } // if
  } else {
    STATRECORD("anatropman: execute on detached");
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // AnatropManager::Execute

void AnatropManager::
Trigger(const string& atoName_, const AnatropScope& scope_, const GenericOptions& options_) {
  BMDEBUG3("AnatropManager::Trigger", atoName_, &options_);
  if (scope_.IsDetached() == false) {
    Anatrop* lAnatrop(CreateAnatrop(atoName_, scope_));
    lAnatrop->GetOptions().Set(options_);
    lAnatrop->OnOptionsSet();
    if (mExecutionStack.empty() == true) {
      mTriggers.push_back(lAnatrop);
    } else {
      mExecutionStack.top()->mTriggers.push_back(lAnatrop);
    } // if
  } else {
    STATRECORD("anatropman: trigger on detached");
  } // if
  EMDEBUG0();
  return;
} // AnatropManager::Trigger

void Anatrop::
OnFinish() {
  BMDEBUG1("Anatrop::OnFinish");
  irBuilder->irbRemoveToBeRemovedStmts();
  irBuilder->irbRemoveToBeRemovedBBs();
  EMDEBUG0();
  return;
} // Anatrop::OnFinish



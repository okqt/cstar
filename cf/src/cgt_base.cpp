// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __OPTIONS_HPP__
#include "options.hpp"
#endif
#ifndef __DEBUGINFO_DWARF_HPP__
#include "debuginfo_dwarf.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif

NonTerminal* IRExprNodeMatcherInfo::mNTPrototypes[NTTLast];
IRBuilder* Rule::smIRBuilder;
IRTPtr* NonTerminal::ptrToVoidType;
hFUInt32 Register::sIdIndex;

string 
sfMatchHelp() {
  string retVal;
  retVal = " - All the expressions must match to a rule. Even the ones covered by statement rules.";
  return retVal;
} // sfMatchHelp

ostream&
operator << (ostream& o, const VirtReg& virtReg_) {
  o << virtReg_.GetName().c_str();
  if (!pred.pIsInvalid(virtReg_.GetHWReg())) {
    o << "[" << *virtReg_.GetHWReg() << "]";
  } // if
  return o;
} // operator <<

ostream&
operator << (ostream& o, const HWReg& hwReg_) {
  o << hwReg_.GetName().c_str();
  return o;
} // operator <<

ostream&
operator << (ostream& o, const RegSet& regSet_) {
  o << "{";
  for (set<Register*>::const_iterator lIt(regSet_.mRegs.begin()); lIt != regSet_.mRegs.end(); ++ lIt) {
    if (lIt != regSet_.mRegs.begin()) {
      o << ", ";
    } // if
    if (dynamic_cast<const VirtReg*>(*lIt) != 0) {
      o << *static_cast<const VirtReg*>(*lIt);
    } else if (dynamic_cast<const HWReg*>(*lIt) != 0) {
      o << *static_cast<const HWReg*>(*lIt);
    } else {
      o << "errRegNotKnown";
    } // if
  } // for
  o << "}";
  return o;
} // operator <<

RegSet
operator | (const RegSet& left_, const RegSet& right_) {
  BDEBUG3("operator |", &left_, &right_);
  RegSet retVal(left_);
  retVal.Insert(right_);
  EDEBUG1(&retVal);
  return retVal;
} // operator |

RegSet
operator | (Register& left_, const RegSet& right_) {
  BDEBUG3("operator |", &left_, &right_);
  RegSet retVal(right_);
  retVal.Insert(&left_);
  EDEBUG1(&retVal);
  return retVal;
} // operator |

RegSet
operator | (const RegSet& left_, Register& right_) {
  BDEBUG3("operator |", &left_, &right_);
  RegSet retVal(left_);
  retVal.Insert(&right_);
  EDEBUG1(&retVal);
  return retVal;
} // operator |

RegSet
operator | (Register& left_, Register& right_) {
  BDEBUG3("operator |", &left_, &right_);
  RegSet retVal;
  retVal.Insert(&left_).Insert(&right_);
  EDEBUG1(&retVal);
  return retVal;
} // operator |

ostream&
operator << (ostream& o, ShiftType st) {
  switch (st) {
    case STInvalid: o << "STInvalid"; break;
    case STNIL: o << "STNIL"; break;
    case STLSR: o << "STLSR"; break;
    case STASR: o << "STASR"; break;
    case STLSL: o << "STLSL"; break;
    case STROR: o << "STROR"; break;
    case STROL: o << "STROL"; break;
    case STRRX: o << "STRRX"; break;
    default:
      o << "err: ShiftType unhandled type";
      break;
  } // switch
  return o;
} // operator <<

NodeInfo::
NodeInfo(IRExpr* irExpr_, NonTerminal* nt_) :
  mIRExpr(irExpr_),
  mNT(nt_)
{
} // NodeInfo::NodeInfo

Register::
Register(const Register& reg_) :
  mName(reg_.mName),
  mId(reg_.mId)
{
  BMDEBUG2("Register::Register", &reg_);
  EMDEBUG0();
} // Register::Register

Register::
Register(const string& name_) :
  mName(name_),
  mId(sIdIndex++)
{
  BMDEBUG2("Register::Register", name_);
  EMDEBUG0();
} // Register::Register

Register::
Register(const string& name_, hFUInt32 id_) :
  mName(name_),
  mId(id_)
{
  BMDEBUG3("Register::Register", name_, id_);
  EMDEBUG0();
} // Register::Register

Rule::
Rule(const Rule& rule_) :
  mPatternNTs(cMaxPatternNTs),
  mName(rule_.mName),
  mHWDescription(rule_.mHWDescription)
{
  BMDEBUG2("Rule::Rule", &rule_);
  EMDEBUG0();
} // Rule::Rule

Rule::
Rule(const string& name_, HWDescription* hwDesc_) :
  mPatternNTs(cMaxPatternNTs),
  mName(name_),
  mHWDescription(hwDesc_)
{
  BMDEBUG3("Rule::Rule", name_, hwDesc_);
  EMDEBUG0();
} // Rule::Rule

void StmtRule::
SetStmtPattern(IRStmt* stmtPattern_) {
  BMDEBUG2("StmtRule::SetStmtPattern", stmtPattern_);
  REQUIREDMSG(stmtPattern_ != 0, ieStrNonNullParam);
  REQUIREDMSG(stmtPattern_->GetStmtType() != IRSTInvalid, ieStrInconsistentInternalStructure);
  mStmtPattern = stmtPattern_;
  EMDEBUG0();
} // StmtRule::SetStmtPattern

IRStmt* StmtRule::
GetStmtPattern() const {
  BMDEBUG1("StmtRule::GetStmtPattern");
  REQUIREDMSG(mStmtPattern != 0, ieStrInconsistentInternalStructure);
  EMDEBUG1(mStmtPattern);
  return mStmtPattern;
} // StmtRule::GetStmtPattern

void ExprRule::
SetExprPattern(NonTerminal* result_, IRPatExpr* exprPattern_) {
  BMDEBUG3("ExprRule::SetExprPattern", result_, exprPattern_);
  //! \todo H Design: Add checks for consecutive id usage.
  mResultNT = result_;
  mExprPattern = exprPattern_;
  EMDEBUG0();
} // ExprRule::SetExprPattern

ShiftOpNT* Rule::
GetShiftOpNT(hFInt32 ntIndex_) {
  BMDEBUG2("Rule::GetShiftOpNT", ntIndex_);
  REQUIREDMSG(pred.pIsInRange(ntIndex_, 0, mPatternNTs.size()-1) == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<ShiftOpNT*>(mPatternNTs[ntIndex_]) != 0, ieStrParamValuesDBCViolation);
  ShiftOpNT* retVal(static_cast<ShiftOpNT*>(mPatternNTs[ntIndex_]));
  EMDEBUG1(retVal);
  return retVal;
} // Rule::GetShiftOpNT

AnyNT* Rule::
GetAnyNT(hFInt32 ntIndex_) {
  BMDEBUG2("Rule::GetAnyNT", ntIndex_);
  REQUIREDMSG(pred.pIsInRange(ntIndex_, 0, mPatternNTs.size()-1) == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<AnyNT*>(mPatternNTs[ntIndex_]) != 0, ieStrParamValuesDBCViolation);
  AnyNT* retVal(static_cast<AnyNT*>(mPatternNTs[ntIndex_]));
  ENSUREMSG(retVal != 0, ieStrDBCViolation << "Probably you did not use the index " <<
    ntIndex_ << " in the pattern or result expression.");
  EMDEBUG1(retVal);
  return retVal;
} // Rule::GetAnyNT

void Rule::
SetNT(hFInt32 ntIndex_, NonTerminal* newNT_) {
  BMDEBUG3("Rule::SetNT", ntIndex_, newNT_);
  REQUIREDMSG(pred.pIsInRange(ntIndex_, 0, mPatternNTs.size()-1) == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<AnyNT*>(mPatternNTs[ntIndex_]) != 0, ieStrParamValuesDBCViolation);
  mPatternNTs[ntIndex_] = newNT_;
  EMDEBUG0();
  return;
} // Rule::SetNT

RegisterNT* Rule::
GetRegNT(hFInt32 ntIndex_) {
  BMDEBUG2("Rule::GetRegNT", ntIndex_);
  REQUIREDMSG(pred.pIsInRange(ntIndex_, 0, mPatternNTs.size()-1) == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<RegisterNT*>(mPatternNTs[ntIndex_]) != 0, ieStrParamValuesDBCViolation);
  RegisterNT* retVal(static_cast<RegisterNT*>(mPatternNTs[ntIndex_]));
  ENSUREMSG(retVal != 0, ieStrDBCViolation << "Probably you did not use the index " <<
    ntIndex_ << " in the pattern or result expression.");
  EMDEBUG1(retVal);
  return retVal;
} // Rule::GetRegNT

ConditionNT* Rule::
GetCondNT(hFInt32 ntIndex_) {
  BMDEBUG2("Rule::GetCondNT", ntIndex_);
  REQUIREDMSG(pred.pIsInRange(ntIndex_, 0, mPatternNTs.size()-1) == true, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<ConditionNT*>(mPatternNTs[ntIndex_]) != 0, ieStrParamValuesDBCViolation);
  ConditionNT* retVal(static_cast<ConditionNT*>(mPatternNTs[ntIndex_]));
  EMDEBUG1(retVal);
  return retVal;
} // Rule::GetCondNT

RealConstNT::
RealConstNT() :
  NonTerminal(),
  value(0)
{
  BMDEBUG1("RealConstNT::RealConstNT");
  EMDEBUG0();
} // RealConstNT::RealConstNT

RealConstNT::
RealConstNT(tBigReal value_) :
  NonTerminal(),
  value(value_)
{
  BMDEBUG1("RealConstNT::RealConstNT");
  EMDEBUG0();
} // RealConstNT::RealConstNT

IntConstNT::
IntConstNT() :
  NonTerminal(),
  value(0)
{
  BMDEBUG1("IntConstNT::IntConstNT");
  EMDEBUG0();
} // IntConstNT::IntConstNT

IntConstNT::
IntConstNT(tBigInt value_) :
  NonTerminal(),
  value(value_)
{
  BMDEBUG1("IntConstNT::IntConstNT");
  EMDEBUG0();
} // IntConstNT::IntConstNT

VirtReg::
VirtReg(HWReg* preSetHWReg_) :
  Register("vReg"),
  mHWReg(preSetHWReg_),
  mAdjMatrixIndex(-1)
{
  BMDEBUG2("VirtReg::VirtReg", preSetHWReg_);
  EMDEBUG0();
} // VirtReg::VirtReg

VirtReg::
VirtReg(const string& name_) :
  Register(name_, -1),
  mHWReg(&Invalid<HWReg>::Obj()),
  mAdjMatrixIndex(-1)
{
  BMDEBUG2("VirtReg::VirtReg", name_);
  EMDEBUG0();
} // VirtReg::VirtReg

VirtReg::
VirtReg() :
  Register("vReg"),
  mHWReg(&Invalid<HWReg>::Obj()),
  mAdjMatrixIndex(-1)
{
  BMDEBUG1("VirtReg::VirtReg");
  EMDEBUG0();
} // VirtReg::VirtReg

VirtReg::
VirtReg(const VirtReg& vReg_) :
  Register("vReg"),
  mHWReg(vReg_.mHWReg),
  mAdjMatrixIndex(vReg_.mAdjMatrixIndex)
{
  BMDEBUG2("VirtReg::VirtReg", &vReg_);
  for (hFUInt32 c(0); c < vReg_.mRegAllocRules.size(); ++ c) {
    mRegAllocRules.push_back(vReg_.mRegAllocRules[c]->Clone());
  } // for
  EMDEBUG0();
} // VirtReg::VirtReg

void Rule::
AddInst(CGFuncContext& context_, AsmSequenceItem* inst_) {
  context_.AddInst(inst_, GetName());
} // Rule::AddInst

void VirtReg::
SetHWReg(HWReg* hwReg_) {
  BMDEBUG2("VirtReg::SetHWReg", hwReg_);
  REQUIREDMSG(hwReg_ != 0, ieStrNonNullParam);
  mHWReg = hwReg_;
  EMDEBUG0();
  return;
} // VirtReg::SetHWReg

HWReg* VirtReg::
GetHWReg() const {
  BMDEBUG1("VirtReg::GetHWReg");
  EMDEBUG1(mHWReg);
  return mHWReg;
} // VirtReg::GetHWReg

IRExprNodeMatcherInfo::
IRExprNodeMatcherInfo() {
  for (hFInt32 c = 0; c < NTTLast+1; ++ c) {
    mNTConvRule[c] = &Invalid<ExprRule>::Obj();
    mCostVector[c] = make_pair<hReal32, ExprRule*>(consts.cInfiniteCost, &Invalid<ExprRule>::Obj());
  } // for
} // IRExprNodeMatcherInfo::IRExprNodeMatcherInfo

HWDescription::
HWDescription(const string& name_, Target* target_) :
  mISADef(0),
  mCurrBB(0),
  mName(name_),
  mTarget(target_)
{
  BMDEBUG3("HWDescription::HWDescription", name_, target_);
  //! \warning ptrToVoidType must be initialized before creating/using a NonTerminal.
  NonTerminal::ptrToVoidType = new IRTPtr(IRENull::nullVoidType);
  //! \todo do IRExprNodeMatcherInfo::ntPrototypes initialization to a proper place.
  IRExprNodeMatcherInfo::mNTPrototypes[NTTRegister] = new RegisterNT();
  IRExprNodeMatcherInfo::mNTPrototypes[NTTMemory] = new MemoryNT();
  IRExprNodeMatcherInfo::mNTPrototypes[NTTIntConst] = new IntConstNT();
  IRExprNodeMatcherInfo::mNTPrototypes[NTTRealConst] = new RealConstNT();
  IRExprNodeMatcherInfo::mNTPrototypes[NTTNull] = new NullNT();
  IRExprNodeMatcherInfo::mNTPrototypes[NTTShiftOp] = new ShiftOpNT();
  IRExprNodeMatcherInfo::mNTPrototypes[NTTCondition] = new ConditionNT();
  IRExprNodeMatcherInfo::mNTPrototypes[NTTAny] = new AnyNT();
  Rule::smIRBuilder = new IRBuilder(irProgram, consts.cIRBNoOptims);
  EMDEBUG0();
} // HWDescription::HWDescription

ExprRule::
ExprRule(const ExprRule& rule_) :
  Rule(rule_),
  mExprPattern(rule_.mExprPattern->GetRecCopyExpr()),
  mIRExpr(rule_.mIRExpr),
  mResultNT(rule_.mResultNT->Clone()->SetNewVirtRegs()),
  dbcIsResultNTValid(rule_.dbcIsResultNTValid),
  dbcIsIRExprValid(rule_.dbcIsIRExprValid)
{
  BMDEBUG1("ExprRule::ExprRule");
  { // keep the correct id - nonterminal mapping,
    // we could get rid of this complex operation by having ruleId field in NonTerminals.
    vector<IRExpr*> lRuleOrder;
    vector<IRExpr*> lThisOrder;
    extr.eGetPostOrderTraversal(rule_.mExprPattern, lRuleOrder);
    extr.eGetPostOrderTraversal(mExprPattern, lThisOrder);
    for (hFUInt32 c(0); c < lRuleOrder.size(); ++ c) {
      if (dynamic_cast<NonTerminal*>(lRuleOrder[c]) != 0) {
        hFInt32 lNTId(rule_.GetNTIndex(static_cast<NonTerminal*>(lRuleOrder[c])));
        ASSERTMSG(dynamic_cast<NonTerminal*>(lThisOrder[c]) != 0,
          ieStrInconsistentInternalStructure);
        mPatternNTs[lNTId] = static_cast<NonTerminal*>(lThisOrder[c]);
      } // if
    } // for
  } // block
  mPatternNTs[0] = mResultNT;
  EMDEBUG0();
} // ExprRule::ExprRule

ExprRule::
ExprRule(const string& name_, HWDescription* hwDesc_) :
  Rule(name_, hwDesc_),
  mExprPattern(&Invalid<IRPatExpr>::Obj()),
  mIRExpr(&Invalid<IRExpr>::Obj()),
  mResultNT(&Invalid<NonTerminal>::Obj())
{
  BMDEBUG3("ExprRule::ExprRule", name_, hwDesc_);
  EMDEBUG0();
} // ExprRule::ExprRule

bool HWDescription::
GenerateProg(IRProgram* prog_, CGContext& context_) {
  BMDEBUG3("HWDescription::GenerateProg", prog_, &context_);
  OnProgStart(prog_, context_);
  ListIterator<IRModule*> moduleIter(prog_->GetModuleIter());
  for (moduleIter.First(); !moduleIter.IsDone(); moduleIter.Next()) {
    GenerateModule(*moduleIter, context_);
  } // for
  OnProgEnd(prog_, context_);
  EMDEBUG1(true);
  return true;
} // HWDescription::GenerateProg

bool HWDescription::
GenerateModule(IRModule* module_, CGContext& context_) {
  BMDEBUG3("HWDescription::GenerateModule", module_, &context_);
  AssignLTConstAddresses(module_, context_);
  OnModuleStart(module_, context_);
  { // Process globals
    // We should first dump the globals, this way we will have the address of
    // them available in the functions.
    vector<IROGlobal*> globals;
    module_->GetGlobals(globals);
    for (hFUInt32 c(0); c < globals.size(); ++ c) {
      OnGlobal(globals[c], context_);
    } // for
  } // block
  if (context_.mFuncContexts.empty() == true) {
    ListIterator<IRFunction*> lFuncIter(module_->GetFuncIter());
    for (lFuncIter.First(); !lFuncIter.IsDone(); lFuncIter.Next()) {
      /* LIE */ GenerateFunc(*lFuncIter, *lFuncIter->GetCGContext());
      context_.AddFuncContext(lFuncIter->GetCGContext());
    } // for
  } // if
  OnModuleEnd(module_, context_);
  EMDEBUG0();
  return true;
} // HWDescription::GenerateModule

void HWDescription::
Emit(ostream& o, CGContext& context_) {
  BMDEBUG1("HWDescription::Emit");
  vector<string> lEmitSequenceBeg;
  vector<string> lEmitSequenceEnd;
  lEmitSequenceBeg.push_back("ProgBeg-dir");
  lEmitSequenceBeg.push_back("ProgBeg-data");
  lEmitSequenceBeg.push_back("ProgBeg-text");
  lEmitSequenceBeg.push_back("ModuleBeg-dir");
  lEmitSequenceBeg.push_back("ModuleBeg-data");
  lEmitSequenceBeg.push_back("ModuleBeg-text");
  lEmitSequenceEnd.push_back("ModuleEnd-text");
  lEmitSequenceEnd.push_back("ModuleEnd-data");
  lEmitSequenceEnd.push_back("ModuleEnd-dir");
  lEmitSequenceEnd.push_back("ProgEnd-text");
  lEmitSequenceEnd.push_back("ProgEnd-data");
  lEmitSequenceEnd.push_back("ProgEnd-dir");
  ASMSection lASMSection(&o);
  hFUInt32 c;
  for (c = 0; c < lEmitSequenceBeg.size(); ++ c) {
    vector<AsmSequenceItem*>& insts(context_.mInsts[lEmitSequenceBeg[c]]);
    for (hFUInt32 d(0); d < insts.size(); ++ d) {
      insts[d]->GetDisassembly(lASMSection, *context_.GetASMDesc());
      lASMSection.Endl();
    } // for
  } // for
  for (c = 0; c < context_.mFuncContexts.size(); ++ c) {
    for (hFUInt32 d = 0; d < context_.mFuncContexts[c]->mInsts.size(); ++ d) {
      context_.mFuncContexts[c]->mInsts[d]->GetDisassembly(lASMSection, *context_.GetASMDesc());
      lASMSection.Endl();
    } // for
  } // for
  for (c = 0; c < lEmitSequenceEnd.size(); ++ c) {
    vector<AsmSequenceItem*>& insts(context_.mInsts[lEmitSequenceEnd[c]]);
    for (hFUInt32 d(0); d < insts.size(); ++ d) {
      insts[d]->GetDisassembly(lASMSection, *context_.GetASMDesc());
      lASMSection.Endl();
    } // for
  } // for
  EMDEBUG0();
  return;
} // HWDescription::Emit

StmtRule::
StmtRule(const StmtRule& rule_) :
  Rule(rule_),
  mStmtPattern(rule_.mStmtPattern->GetRecCopyStmt()),
  mIRStmt(rule_.mIRStmt)
{
  BMDEBUG1("StmtRule::StmtRule");
  { // keep the correct id - nonterminal mapping,
    // we could get rid of this complex operation by having ruleId field in NonTerminals.
    vector<NodeInfo> ruleNodes;
    vector<NodeInfo> thisNodes;
    rule_.GetIRLeaves(0, ruleNodes);
    this->GetIRLeaves(0, thisNodes);
    for (hFUInt32 c(0); c < ruleNodes.size(); ++ c) {
      hFInt32 lNTId(rule_.GetNTIndex(ruleNodes[c].mNT));
      mPatternNTs[lNTId] = thisNodes[c].mNT;
    } // for
  } // block
  EMDEBUG0();
} // StmtRule::StmtRule

StmtRule::
StmtRule(const string& name_, HWDescription* hwDesc_) :
  Rule(name_, hwDesc_),
  mStmtPattern(&Invalid<IRStmt>::Obj()),
  mIRStmt(&Invalid<IRStmt>::Obj())
{
  BMDEBUG3("StmtRule::StmtRule", name_, hwDesc_);
  EMDEBUG0();
} // StmtRule::StmtRule

IRStmt* StmtRule::
GetMatchedStmt() const {
  BMDEBUG1("StmtRule::GetMatchedStmt");
  REQUIREDMSG(!pred.pIsInvalid(mIRStmt), ieStrObjectNotValid);
  EMDEBUG1(mIRStmt);
  return mIRStmt;
} // StmtRule::GetMatchedStmt

ConditionNT::
ConditionNT() :
  NonTerminal(IRECmp::cmpBoolType),
  mCondition(CNTInvalid)
{
  BMDEBUG1("ConditionNT::ConditionNT");
  EMDEBUG0();
} // ConditionNT::ConditionNT

ConditionNT::
ConditionNT(CNTType condition_) :
  NonTerminal(IRECmp::cmpBoolType),
  mCondition(condition_)
{
  BMDEBUG2("ConditionNT::ConditionNT", condition_);
  EMDEBUG0();
} // ConditionNT::ConditionNT

ShiftOpNT& ShiftOpNT::
operator = (const ShiftOpNT& nt_) {
  BMDEBUG2("ShiftOpNT::operator =", &nt_);
  NonTerminal::operator=(nt_);
  mReg = nt_.mReg;
  mIntShiftAmount = nt_.mIntShiftAmount;
  mRegShiftAmount = nt_.mRegShiftAmount;
  EMDEBUG0();
  return *this;
} // ShiftOpNT::operator =

ConditionNT& ConditionNT::
operator = (const ConditionNT& condNT_) {
  BMDEBUG2("ConditionNT::operator =", &condNT_);
  NonTerminal::operator = (condNT_);
  mCondition = condNT_.mCondition;
  EMDEBUG0();
  return *this;
} // ConditionNT::operator =

ConditionNT ConditionNT::
GetInverted() const {
  BMDEBUG1("ConditionNT::GetInverted");
  ConditionNT retVal(*this);
  switch (mCondition) {
    case CNTEq: retVal.mCondition = CNTNe; break; // '=='  ->  '!='
    case CNTNe: retVal.mCondition = CNTEq; break; // '!='  ->  '=='
    case CNTGe: retVal.mCondition = CNTLt; break; // '>='  ->  '<'
    case CNTGt: retVal.mCondition = CNTLe; break; // '>'   ->  '<='
    case CNTLe: retVal.mCondition = CNTGt; break; // '<='  ->  '>'
    case CNTLt: retVal.mCondition = CNTGe; break; // '<'   ->  '>='
    case CNTUGe: retVal.mCondition = CNTULt; break; // '>='  ->  '<'
    case CNTUGt: retVal.mCondition = CNTULe; break; // '>'   ->  '<='
    case CNTULe: retVal.mCondition = CNTUGt; break; // '<='  ->  '>'
    case CNTULt: retVal.mCondition = CNTUGe; break; // '<'   ->  '>='
    default:
      ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // switch
  EMDEBUG1(&retVal);
  return retVal;
} // ConditionNT::GetInverted

ASMSection& VirtReg::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) {
  BMDEBUG1("VirtReg::GetDisassembly");
  if (asmDesc_.IsReleaseModeDump() == true) {
    ASSERTMSG(!pred.pIsInvalid(mHWReg), ieStrInconsistentInternalStructure <<
        ": Virtual registers must be assigned a HW register.");
    o << mHWReg->GetName().c_str();
  } else {
    o << GetName().c_str();
    if (!pred.pIsInvalid(mHWReg)) {
      o << "[" << mHWReg->GetName().c_str() << "]";
    } // if
  } // if
  EMDEBUG0();
  return o;
} // VirtReg::GetDisassembly

string VirtReg::
GetName() const {
  BMDEBUG1("VirtReg::GetName");
  string retVal(Register::GetName());
  if (GetId() != -1) {
    char lId[128];
    sprintf(lId, "%s_%d", retVal.c_str(), GetId());
    retVal = lId;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // VirtReg::GetName

MemoryNT::
MemoryNT() :
  NonTerminal(),
  mReg(0),
  mOffset(0)
{
  BMDEBUG1("MemoryNT::MemoryNT");
  NOTIMPLEMENTED();
  EMDEBUG0();
} // MemoryNT::MemoryNT

ASMSection& AsmSequenceItem::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
  BMDEBUG1("AsmSequenceItem::GetDisassembly");
  if (mComment.length()) {
    asmDesc_.PutCommentStr(o);
    o << mComment;
  } // if
  EMDEBUG0();
  return o;
} // AsmSequenceItem::GetDisassembly

ASMSection& AsmSeqLabel::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
  BMDEBUG1("AsmSeqLabel::GetDisassembly");
  asmDesc_.PutLabel(o, mLabel);
  // Emit the comment.
  AsmSequenceItem::GetDisassembly(o, asmDesc_);
  EMDEBUG0();
  return o;
} // AsmSeqLabel::GetDisassembly

CGFuncContext::
CGFuncContext(const CGContext& cgContext_, RegisterAllocator* regAllocator_, bool mustMatch_, bool stackGrowsDownwards_) :
  CGContext(cgContext_),
  mRegAllocator(regAllocator_),
  mAvailableRegs(cgContext_.GetHWDesc()->GetRegsForRegAlloc()),
  mMustMatch(mustMatch_),
  mIsFailed(false),
  mStackGrowsDownwards(stackGrowsDownwards_),
  mFunc(0)
{
  BMDEBUG4("CGFuncContext::CGFuncContext", regAllocator_, mustMatch_, stackGrowsDownwards_);
  EMDEBUG0();
} // CGFuncContext::CGFuncContext

CGFuncContext::
CGFuncContext(const CGFuncContext& cgContext_, IRFunction* func_) :
  CGContext(cgContext_),
  mFunc(func_),
  mRegAllocator(cgContext_.mRegAllocator),
  mAvailableRegs(cgContext_.GetHWDesc()->GetRegsForRegAlloc()),
  mMustMatch(cgContext_.mMustMatch),
  mIsFailed(cgContext_.mIsFailed),
  mStackGrowsDownwards(cgContext_.mStackGrowsDownwards),
  mInsts(cgContext_.mInsts)
{
  BMDEBUG2("CGFuncContext::CGFuncContext", func_);
  EMDEBUG0();
} // CGFuncContext::CGFuncContext

CGFuncContext::
CGFuncContext(const CGFuncContext& cgContext_) :
  CGContext(cgContext_),
  mFunc(cgContext_.mFunc),
  mRegAllocator(cgContext_.mRegAllocator),
  mAvailableRegs(cgContext_.GetHWDesc()->GetRegsForRegAlloc()),
  mMustMatch(cgContext_.mMustMatch),
  mIsFailed(cgContext_.mIsFailed),
  mStackGrowsDownwards(cgContext_.mStackGrowsDownwards)
{
  BMDEBUG1("CGFuncContext::CGFuncContext");
  EMDEBUG0();
} // CGFuncContext::CGFuncContext

CGFuncContext::
CGFuncContext(const CGContext& cgContext_, IRFunction* func_, RegisterAllocator* regAllocator_,
  bool mustMatch_, bool stackGrowsDownwards_) :
  CGContext(cgContext_),
  mRegAllocator(regAllocator_),
  mAvailableRegs(cgContext_.GetHWDesc()->GetRegsForRegAlloc()),
  mMustMatch(mustMatch_),
  mIsFailed(false),
  mStackGrowsDownwards(stackGrowsDownwards_),
  mFunc(func_)
{
  BMDEBUG5("CGFuncContext::CGFuncContext", func_, regAllocator_, mustMatch_, stackGrowsDownwards_);
  EMDEBUG0();
} // CGFuncContext::CGFuncContext

CGFuncContext& CGFuncContext::
AddInst(AsmSequenceItem* inst_, const string& comment_) {
  BMDEBUG3("CGFuncContext::AddInst", inst_, comment_);
  REQUIREDMSG(inst_ != 0, ieStrNonNullParam);
  inst_->SetComment(comment_);
  mInsts.push_back(inst_);
  if (CGBB* lCurrBB = GetHWDesc()->GetCurrBB()) {
    lCurrBB->AddInst(inst_);
  } // if
  EMDEBUG0();
  return *this;
} // CGFuncContext::AddInst

CGFuncContext& CGFuncContext::
AddInst(AsmSequenceItem* inst_) {
  BMDEBUG2("CGFuncContext::AddInst", inst_);
  REQUIREDMSG(inst_ != 0, ieStrNonNullParam);
  mInsts.push_back(inst_);
  if (CGBB* lCurrBB = GetHWDesc()->GetCurrBB()) {
    lCurrBB->AddInst(inst_);
  } // if
  EMDEBUG0();
  return *this;
} // CGFuncContext::AddInst

ASMSection& AsmSeqData::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
  BMDEBUG1("AsmSeqData::GetDisassembly");
  mLabel->GetDisassembly(o, asmDesc_).Endl();
  for (hFUInt32 c(0); c < mValue.size(); ++ c) {
    asmDesc_.PutReloc(o, mValue[c]);
  } // for
  // Emit the comment.
  AsmSequenceItem::GetDisassembly(o, asmDesc_);
  EMDEBUG0();
  return o;
} // AsmSeqData::GetDisassembly

bool RelocExprDisassemblerVisitor::
Visit(const IREAddrConst* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  if (expr_->IsObject() == true) {
    IRObject* lObj(expr_->GetObject());
    ASSERTMSG(pred.pIsAddrSet(lObj) == true, ieStrInconsistentInternalStructure);
    Address* lAddr(lObj->GetAddress());
    ASSERTMSG(dynamic_cast<AddrLTConst*>(lAddr) != 0, ieStrInconsistentInternalStructure);
    AddrLTConst* lLTConst(static_cast<AddrLTConst*>(lAddr));
    if (!expr_->GetOffset()) {
      mOStream << lLTConst->GetLTConst();
    } else {
      mOStream << lLTConst->GetLTConst() << "+" << expr_->GetOffset();
    } // if
  } else if (expr_->IsConst() == true) {
    mOStream << expr_->GetConstValue();
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

bool RelocExprDisassemblerVisitor::
Visit(const IREAdd* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  mOStream << "(";
  expr_->GetLeftExpr()->Accept(*this);
  mOStream << "+";
  expr_->GetRightExpr()->Accept(*this);
  mOStream << ")";
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

void HWDescription::
OnBBStart(IRBB* bb_, CGFuncContext& context_) {
  BMDEBUG2("HWDescription::OnBBStart", bb_);
  if (options.obGet("debuginfo") == true) {
    dwarf.OnBBStart(bb_, context_);
  } // if
  context_.AddInst(context_.GetLabelOf(bb_));
  EMDEBUG0();
  return;
} // HWDescription::OnBBStart

void HWDescription::
OnStmtStart(IRStmt* stmt_, CGFuncContext& context_) {
  BMDEBUG3("HWDescription:OnStmtStart", stmt_, &context_);
  REQUIREDMSG(stmt_ != 0, ieStrParamValuesDBCViolation);
  if (options.obGet("debuginfo") == true) {
    dwarf.OnStmtStart(stmt_, context_);
  } // if
  EMDEBUG0();
  return;
} // HWDescription::OnStmtStart

bool RelocExprDisassemblerVisitor::
Visit(const IREANeg* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  mOStream << "(-";
  expr_->GetExpr()->Accept(*this);
  mOStream << ")";
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

bool RelocExprDisassemblerVisitor::
Visit(const IRECast* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  expr_->GetExpr()->Accept(*this);
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

bool RelocExprDisassemblerVisitor::
Visit(const IREIntConst* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  mOStream << expr_->GetValue();
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

bool RelocExprDisassemblerVisitor::
Visit(const IRORelocSymbol* obj_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", obj_);
  mOStream << obj_->GetName().c_str();
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

bool RelocExprDisassemblerVisitor::
Visit(const IRERealConst* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  //! \todo M Design: generalize this part.
  //! \todo M Design: Fix this.
  if (expr_->GetType()->GetSize() == 32) {
    mOStream << util.uBitConvert<hUInt32, hReal32>(expr_->GetValue().get_hReal32());
  } else if (expr_->GetType()->GetSize() == 64) {
    mOStream << util.uBitConvert<hUInt64, hReal64>(expr_->GetValue().get_hReal64());
  } else {
    ASSERTMSG(0, ieStrNonNullParam);
  } // if
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

bool RelocExprDisassemblerVisitor::
Visit(const IRESub* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  mOStream << "(";
  expr_->GetLeftExpr()->Accept(*this);
  mOStream << "-";
  expr_->GetRightExpr()->Accept(*this);
  mOStream << ")";
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

bool RelocExprDisassemblerVisitor::
Visit(const IREStrConst* expr_) const {
  BMDEBUG2("RelocExprDisassemblerVisitor::Visit", expr_);
  util.uEmitString(mOStream, expr_->GetValue());
  EMDEBUG0();
  return true;
} // RelocExprDisassemblerVisitor::Visit

ASMSection& Instruction::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
  BMDEBUG1("Instruction::GetDisassembly");
  string lDisassembly(mDisassembly);
  for (hFUInt32 c(0); c < mOperands.size(); ++ c) {
    if ((mOperands[c].IsReg() == true && pred.pIsVReg(mOperands[c].GetReg()) == true) || mOperands[c].IsRegSet() == true) {
      InstructionSetDefinition::smReplaceFormatStr(lDisassembly, c, mOperands[c].GetDisassembly(asmDesc_));
    } // if
  } // for
  o << lDisassembly;
  // Emit the comment.
  AsmSequenceItem::GetDisassembly(o, asmDesc_);
  EMDEBUG0();
  return o;
} // Instruction::GetDisassembly

MemoryNT::
MemoryNT(VirtReg* reg_, tBigInt offset_) :
  NonTerminal(),
  mReg(reg_),
  mOffset(offset_)
{
  BMDEBUG3("MemoryNT::MemoryNT", mReg, &mOffset);
  EMDEBUG0();
} // MemoryNT::MemoryNT

IRPatExpr* Rule::
iroObject() {
  BDEBUG1("Rule::iroObject");
  IRPatExpr* retVal(smIRBuilder->irbpoObject());
  EDEBUG1(retVal);
  return retVal;
} // Rule::iroObject

IRPatExpr* Rule::
iroGlobal() {
  BDEBUG1("Rule::iroGlobal");
  IRPatExpr* retVal(smIRBuilder->irbpoGlobal());
  EDEBUG1(retVal);
  return retVal;
} // Rule::iroGlobal

IRPatExpr* Rule::
iroTmp() {
  BDEBUG1("Rule::iroTmp");
  IRPatExpr* retVal(smIRBuilder->irbpoTmp());
  EDEBUG1(retVal);
  return retVal;
} // Rule::iroTmp

IRPatExpr* Rule::
iroLocal() {
  BDEBUG1("Rule::iroLocal");
  IRPatExpr* retVal(smIRBuilder->irbpoLocal());
  EDEBUG1(retVal);
  return retVal;
} // Rule::iroLocal

IRPatExpr* Rule::
iroParam() {
  BDEBUG1("Rule::iroParam");
  IRPatExpr* retVal(smIRBuilder->irbpoParam());
  EDEBUG1(retVal);
  return retVal;
} // Rule::iroParam

IRPatExpr* Rule::
ireNull() {
  BDEBUG1("Rule::ireNull");
  IRPatExpr* retVal(smIRBuilder->irbeNull());
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireNull

IRPatExpr* Rule::
ireNe(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireNe", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeNe(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireNe

IRPatExpr* Rule::
ireEq(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireEq", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeEq(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireEq

IRPatExpr* Rule::
ireLt(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireLt", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeLt(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireLt

IRPatExpr* Rule::
ireLe(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireLe", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeLe(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireLe

IRPatExpr* Rule::
ireGt(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireGt", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeGt(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireGt

IRPatExpr* Rule::
ireGe(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireGe", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeGe(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireGe

IRPatExpr* Rule::
ireCmp(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireCmp", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbpeCmp(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireCmp

IRPatExpr* Rule::
ireAdd(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireAdd", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeAdd(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireAdd

IRPatExpr* Rule::
ireCast(IRPatExpr* expr_) {
  BDEBUG2("Rule::ireCast", expr_);
  static IRType* lDummyType(new IRTVoid);
  IRPatExpr* retVal(smIRBuilder->irbeCast(lDummyType, expr_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireCast

IRPatExpr* Rule::
ireBNeg(IRPatExpr* expr_) {
  BDEBUG2("Rule::ireBNeg", expr_);
  IRPatExpr* retVal(smIRBuilder->irbeBNeg(expr_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireBNeg

IRPatExpr* Rule::
ireANeg(IRPatExpr* expr_) {
  BDEBUG2("Rule::ireANeg", expr_);
  IRPatExpr* retVal(smIRBuilder->irbeANeg(expr_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireANeg

IRPatExpr* Rule::
ireLShiftRight(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireLShiftRight", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeLShiftRight(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireLShiftRight

IRPatExpr* Rule::
ireAShiftRight(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireAShiftRight", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeAShiftRight(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireAShiftRight

IRPatExpr* Rule::
ireShiftLeft(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireShiftLeft", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeShiftLeft(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireShiftLeft

IRPatExpr* Rule::
irePtrAdd(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::irePtrAdd", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbePtrAdd(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::irePtrAdd

IRPatExpr* Rule::
ireMul(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireMul", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeMul(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireMul

IRPatExpr* Rule::
ireDiv(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireDiv", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeDiv(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireDiv

IRPatExpr* Rule::
ireQuo(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireQuo", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeQuo(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireQuo

IRPatExpr* Rule::
ireRem(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireRem", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeRem(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireRem

IRPatExpr* Rule::
ireMod(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireMod", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeMod(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireMod

IRPatExpr* Rule::
ireBOr(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireBOr", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeBOr(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireBOr

IRPatExpr* Rule::
ireBXor(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireBXor", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeBXOr(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireBXor

IRPatExpr* Rule::
ireNot(IRPatExpr* expr_) {
  BDEBUG2("Rule::ireNot", expr_);
  IRPatExpr* retVal(smIRBuilder->irbeNot(expr_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireNot

IRPatExpr* Rule::
ireBAnd(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireBAnd", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeBAnd(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireBAnd

IRPatExpr* Rule::
ireSub(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("Rule::ireSub", left_, right_);
  IRPatExpr* retVal(smIRBuilder->irbeSub(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireSub

IRPatExpr* Rule::
ireRealConst() {
  BDEBUG1("Rule::ireRealConst");
  // We actually do not care the values passed here,
  // just try to create a real const node.
  IRPatExpr* retVal(smIRBuilder->irbeRealConst(*Target::GetTarget()->GetRealSingle(), 0.0F));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireRealConst

IRPatExpr* Rule::
ireStrConst() {
  BDEBUG1("Rule::ireStrConst");
  IRPatExpr* retVal(smIRBuilder->irbeStrConst("$rule_pattern$"));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireStrConst

IRPatExpr* Rule::
ireBoolConst() {
  BDEBUG1("Rule::ireBoolConst");
  IRPatExpr* retVal(smIRBuilder->irbeBoolConst(false));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireBoolConst

IRPatExpr* Rule::
ireAddrConst() {
  BDEBUG1("Rule::ireAddrConst");
  IRPatExpr* retVal(smIRBuilder->irbeAddrConst(smIRBuilder->irbtGetPtr(IRENull::nullVoidType), 0));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireAddrConst

IRPatExpr* Rule::
ireIntConst() {
  BDEBUG1("Rule::ireIntConst");
  // Get any integer type.
  IRPatExpr* retVal(smIRBuilder->irbeIntConst(*Target::GetTarget()->GetIntType(INBITS(8), INBITS(0), IRSDoNotCare), 0));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireIntConst

IRPatExpr* Rule::
ireDeref(IRPatExpr* expr_) {
  BDEBUG2("Rule::ireDeref", expr_);
  IRPatExpr* retVal(smIRBuilder->irbeDeref(expr_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireDeref

IRPatExpr* Rule::
ireAddrOf(IRPatExpr* expr_) {
  BDEBUG1("Rule::ireAddrOf");
  IRPatExpr* retVal(smIRBuilder->irbeAddrOf(expr_));
  EDEBUG1(retVal);
  return retVal;
} // Rule::ireAddrOf

IRSIf* StmtRule::
irsIf(ConditionNT* cond_) {
  BDEBUG2("StmtRule::irsIf", cond_);
  IRSIf* retVal(smIRBuilder->irbpsIf(cond_));
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsIf

RegSet::
RegSet(const RegSet& regSet_) :
  mRegs(regSet_.mRegs)
{
  BMDEBUG2("RegSet::RegSet", &regSet_);
  EMDEBUG0();
} // RegSet::RegSet

RegSet::
RegSet(Register* reg_) {
  BMDEBUG2("RegSet::RegSet", reg_);
  mRegs.insert(reg_);
  EMDEBUG0();
} // RegSet::RegSet

RegSet::
RegSet() {
  BMDEBUG1("RegSet::RegSet");
  EMDEBUG0();
} // RegSet::RegSet

RegSet& RegSet::
Insert(Register* reg_) {
  BMDEBUG2("RegSet::Insert", reg_);
  mRegs.insert(reg_);
  EMDEBUG0();
  return *this;
} // RegSet::Insert

RegSet& RegSet::
Insert(const RegSet& regSet_) {
  BMDEBUG2("RegSet::Insert", &regSet_);
  mRegs.insert(regSet_.mRegs.begin(), regSet_.mRegs.end());
  EMDEBUG0();
  return *this;
} // RegSet::Insert

void RegSet::
Remove(const RegSet& regSet_) {
  BMDEBUG2("RegSet::Remove", &regSet_);
  set<Register*>::iterator lIt(regSet_.mRegs.begin());
  while (lIt != regSet_.mRegs.end()) {
    mRegs.erase(*lIt);
    ++ lIt;
  } // while
  EMDEBUG0();
  return;
} // RegSet::Remove

RegSet& RegSet::
operator -= (const Register* reg_) {
  BMDEBUG2("RegSet::operator-=", reg_);
  REQUIREDMSG(mRegs.empty() == false, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mRegs.find(const_cast<Register*>(reg_)) != mRegs.end(),
    ieStrParamValuesDBCViolation << " Reg no found in the list= " << progcxt(this) << " !< " <<  progcxt(reg_));
  mRegs.erase(mRegs.find(const_cast<Register*>(reg_)));
  EMDEBUG0();
  return *this;
} // RegSet::operator -=

RegSet& RegSet::
operator |= (Register* reg_) {
  BMDEBUG2("RegSet::operator|=", reg_);
  REQUIREDMSG(reg_ != 0, ieStrNonNullParam);
  mRegs.insert(reg_);
  EMDEBUG0();
  return *this;
} // RegSet::operator |=

IRSEval* StmtRule::
irsEval(IRPatExpr* expr_) {
  BDEBUG2("StmtRule::irsEval", expr_);
  IRSEval* retVal(smIRBuilder->irbpsEval(expr_));
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsEval

AsmSeqLabel* CGFuncContext::
GetLabelOf(const IRBB* bb_) {
  BMDEBUG2("CGFuncContext::GetLabelOf", bb_);
  AsmSeqLabel* retVal(0);
  map<const IRBB*, AsmSeqLabel*>::iterator lIt(mBBLabels.find(bb_));
  if (lIt != mBBLabels.end()) {
    retVal = lIt->second;
  } else {
    retVal = GenAsmSeqLabel();
    mBBLabels[bb_] = retVal;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // CGFuncContext::GetLabelOf

void Rule::
ReplaceNT(NonTerminal* old_, NonTerminal* new_) {
  BMDEBUG3("Rule::ReplaceNT", old_, new_);
  mPatternNTs[GetNTIndex(old_)] = new_;
  old_->ReplaceWith(new_);
  EMDEBUG0();
  return;
} // Rule::ReplaceNT

hFInt32 Rule::
GetNTIndex(const NonTerminal* nt_) const {
  BMDEBUG2("Rule::GetNTIndex", nt_);
  hFInt32 retVal(-1);
  for (hFUInt32 c(0); c < mPatternNTs.size(); ++ c) {
    if (mPatternNTs[c] == nt_) {
      retVal = c;
      break;
    } // if
  } // for
  ASSERTMSG(retVal != -1, ieStrInconsistentInternalStructure);
  EMDEBUG1(retVal);
  return retVal;
} // Rule::GetNTIndex

IRSJump* StmtRule::
irsJump() {
  BDEBUG1("StmtRule::irsJump");
  IRSJump* retVal(smIRBuilder->irbpsJump());
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsJump

IRSNull* StmtRule::
irsNull() {
  BDEBUG1("StmtRule::irsNull");
  IRSNull* retVal(smIRBuilder->irbpsNull());
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsNull

IRSLabel* StmtRule::
irsLabel() {
  BDEBUG1("StmtRule::irsLabel");
  IRSLabel* retVal(smIRBuilder->irbpsLabel());
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsLabel

IRSAssign* StmtRule::
irsAssign(IRPatExpr* left_, IRPatExpr* right_) {
  BDEBUG3("StmtRule::irsAssign", left_, right_);
  IRSAssign* retVal(smIRBuilder->irbpsAssign(left_, right_));
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsAssign

IRSPCall* StmtRule::
irsPCall(IRPatExpr* routine_) {
  BDEBUG2("StmtRule::irsPCall", routine_);
  IRSPCall* retVal(smIRBuilder->irbpsPCall(routine_));
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsPCall

IRSBuiltInCall* StmtRule::
irsBICall(IRPatExpr* routine_) {
  BDEBUG2("StmtRule::irsBICall", routine_);
  IRSBuiltInCall* retVal(smIRBuilder->irbpsBICall(routine_));
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsBICall

IRSFCall* StmtRule::
irsFCall(IRPatExpr* routine_) {
  BDEBUG2("StmtRule::irsFCall", routine_);
  IRSFCall* retVal(smIRBuilder->irbpsFCall(routine_));
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsFCall

IRSReturn* StmtRule::
irsReturn(IRPatExpr* ret_) {
  BDEBUG2("StmtRule::irsReturn", ret_);
  IRSReturn* retVal(smIRBuilder->irbpsReturn(ret_));
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsReturn

IRSProlog* StmtRule::
irsProlog() {
  BDEBUG1("StmtRule::irsProlog");
  IRSProlog* retVal(smIRBuilder->irbpsProlog());
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsProlog

IRSEpilog* StmtRule::
irsEpilog() {
  BDEBUG1("StmtRule::irsEpilog");
  IRSEpilog* retVal(smIRBuilder->irbpsEpilog());
  EDEBUG1(retVal);
  return retVal;
} // StmtRule::irsEpilog

CNTType ConditionNT::
GetCondition(const IRECmp* cmp_) {
  BDEBUG2("ConditionNT::GetCondition", cmp_);
  CNTType retVal(CNTInvalid);
  // Note that type of cmp is always bool, we need to check the unsignedness
  // either from left or right expression.
  if (dynamic_cast<const IREGt*>(cmp_) != 0) {
    retVal = !pred.pIsUnsignedInt(cmp_->GetLeftExpr()->GetType()) ? CNTGt : CNTUGt;
  } else if (dynamic_cast<const IREGe*>(cmp_) != 0) {
    retVal = !pred.pIsUnsignedInt(cmp_->GetLeftExpr()->GetType()) ? CNTGe : CNTUGe;
  } else if (dynamic_cast<const IRELe*>(cmp_) != 0) {
    retVal = !pred.pIsUnsignedInt(cmp_->GetLeftExpr()->GetType()) ? CNTLe : CNTULe;
  } else if (dynamic_cast<const IRELt*>(cmp_) != 0) {
    retVal = !pred.pIsUnsignedInt(cmp_->GetLeftExpr()->GetType()) ? CNTLt : CNTULt;
  } else if (dynamic_cast<const IREEq*>(cmp_) != 0) {
    retVal = CNTEq;
  } else if (dynamic_cast<const IRENe*>(cmp_) != 0) {
    retVal = CNTNe;
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  ENSUREMSG(retVal != CNTInvalid, ieStrInconsistentInternalStructure);
  EDEBUG1(retVal);
  return retVal;
} // ConditionNT::GetCondition

ShiftOpNT::
ShiftOpNT(const ShiftOpNT& shiftOpNT_) :
  NonTerminal(shiftOpNT_),
  mReg(shiftOpNT_.mReg),
  mIntShiftAmount(shiftOpNT_.mIntShiftAmount),
  mRegShiftAmount(shiftOpNT_.mRegShiftAmount),
  mShiftType(shiftOpNT_.mShiftType)
{
  BMDEBUG2("ShiftOpNT::ShiftOpNT", &shiftOpNT_);
  EMDEBUG0();
} // ShiftOpNT::ShiftOpNT

StmtPrologEpilogRule::
StmtPrologEpilogRule(const StmtPrologEpilogRule& stmtRule_) :
  StmtRule(stmtRule_)
{
  BMDEBUG1("StmtPrologEpilogRule::StmtPrologEpilogRule");
  EMDEBUG0();
} // StmtPrologEpilogRule::StmtPrologEpilogRule

StmtPrologEpilogRule::
StmtPrologEpilogRule(const string& name_, HWDescription* hwDesc_) :
  StmtRule(name_, hwDesc_)
{
  BMDEBUG1("StmtPrologEpilogRule::StmtPrologEpilogRule");
  EMDEBUG0();
} // StmtPrologEpilogRule::StmtPrologEpilogRule

CGContext::
CGContext(HWDescription* hwDesc_, CallingConvention* callConv_, RegisterAllocator* defRA_, ASMDesc* asmDesc_) :
  mASMLabelIndex(0),
  mHWDesc(hwDesc_),
  mCallConv(callConv_),
  mDefaultRegAllocator(defRA_),
  mASMDesc(asmDesc_),
  mCGFlags(CGFLocalRA),
  mDefFuncContext(0)
{
  BMDEBUG5("CGContext::CGContext", hwDesc_, callConv_, defRA_, asmDesc_);
  debugTrace->SetASMDesc(*asmDesc_);
  EMDEBUG0();
} // CGContext::CGContext

CGContext::
CGContext(const CGContext& context_) :
  mASMLabelIndex(context_.mASMLabelIndex),
  mHWDesc(context_.mHWDesc),
  mCallConv(context_.mCallConv),
  mASMDesc(context_.mASMDesc),
  mCGFlags(context_.mCGFlags),
  mDefaultRegAllocator(context_.mDefaultRegAllocator),
  mDefFuncContext(context_.mDefFuncContext),
  mInsts(context_.mInsts),
  mFuncContexts(context_.mFuncContexts)
{
  BMDEBUG2("CGContext::CGContext", &context_);
  EMDEBUG0();
} // CGContext::CGContext

ShiftOpNT::
ShiftOpNT() :
  NonTerminal(),
  mReg(&Invalid<VirtReg>::Obj()),
  mIntShiftAmount(-1),
  mRegShiftAmount(&Invalid<VirtReg>::Obj()),
  mShiftType(STInvalid)
{
} // ShiftOpNT::ShiftOpNT

ShiftOpNT::
ShiftOpNT(VirtReg* reg_, tBigInt shiftAmount_, ShiftType shiftType_) :
  NonTerminal(),
  mReg(reg_),
  mIntShiftAmount(shiftAmount_),
  mRegShiftAmount(&Invalid<VirtReg>::Obj()),
  mShiftType(shiftType_)
{
  REQUIREDMSG(mReg != 0, ieStrNonNullParam);
} // ShiftOpNT::ShiftOpNT

ShiftOpNT::
ShiftOpNT(VirtReg* reg_, VirtReg* shiftAmount_, ShiftType shiftType_) :
  NonTerminal(),
  mReg(reg_),
  mIntShiftAmount(-1),
  mRegShiftAmount(shiftAmount_),
  mShiftType(shiftType_)
{
  REQUIREDMSG(mReg != 0, ieStrNonNullParam);
  REQUIREDMSG(mRegShiftAmount != 0, ieStrNonNullParam);
} // ShiftOpNT::ShiftOpNT

ShiftOpNT* Rule::
shiftOpNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::shiftOpNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  ShiftOpNT* retVal(new ShiftOpNT());
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::shiftOpNT

RegisterNT::
RegisterNT() :
  NonTerminal(),
  mReg(new VirtReg)
{
} // RegisterNT::RegisterNT

RegisterNT::
RegisterNT(VirtReg* reg_) :
  NonTerminal(),
  mReg(reg_)
{
} // RegisterNT::RegisterNT

RegisterNT* Rule::
regNT(hFInt32 ntIdInRule_, VirtReg* virtReg_) {
  BDEBUG3("Rule::regNT", ntIdInRule_, virtReg_);
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  RegisterNT* retVal(new RegisterNT(virtReg_));
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::regNT

RegisterNT* Rule::
regNT(hFInt32 ntIdInRule_) {
  BDEBUG2("Rule::regNT", ntIdInRule_);
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  RegisterNT* retVal(new RegisterNT());
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::regNT

MemoryNT* Rule::
memNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::memNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  MemoryNT* retVal(new MemoryNT());
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::memNT

IgnoreNT* Rule::
ignoreNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::ignoreNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  IgnoreNT* retVal = new IgnoreNT();
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::intNT

IntConstNT* Rule::
intNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::intNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  IntConstNT* retVal = new IntConstNT();
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::intNT

IntConstNT* Rule::
intNT(hFInt32 ntIdInRule_, const tBigInt& value_) {
  BDEBUG2("Rule::intNT", &value_);
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  IntConstNT* retVal(new IntConstNT(value_));
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::intNT

RealConstNT* Rule::
realNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::realNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  RealConstNT* retVal(new RealConstNT());
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::realNT

RealConstNT* Rule::
realNT(hFInt32 ntIdInRule_, const tBigReal& value_) {
  BDEBUG2("Rule::realNT", &value_);
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  RealConstNT* retVal(new RealConstNT(value_));
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::realNT

NullNT* Rule::
nullNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::nullNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  NullNT* retVal(new NullNT());
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::nullNT

ConditionNT* Rule::
condNT(hFInt32 ntIdInRule_, CNTType cond_) {
  BDEBUG2("Rule::condNT", cond_);
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  ConditionNT* retVal(new ConditionNT(cond_));
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::condNT

ConditionNT* Rule::
condNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::condNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  ConditionNT* retVal(new ConditionNT());
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::condNT

AnyNT* Rule::
anyNT(hFInt32 ntIdInRule_) {
  BDEBUG1("Rule::anyNT");
  REQUIREDMSG(ntIdInRule_ >= 0, ieStrParamValuesDBCViolation);
  AnyNT* retVal(new AnyNT());
  mPatternNTs[ntIdInRule_] = retVal;
  EDEBUG1(retVal);
  return retVal;
} // Rule::anyNT

Instruction::
Instruction(const tBigInt& length_, const string& disassembly_, const DefRegSet& defs_, const UseRegSet& uses_,
    const vector<GenInstOp>& ops_, bool ignoreDefsAndUses_) :
  AsmSequenceItem(length_),
  mDefs(defs_),
  mUses(uses_),
  mDisassembly(disassembly_),
  mOperands(ops_)
{
  BMDEBUG5("Instruction::Instruction", &length_, disassembly_, &defs_, &uses_);
  if (ignoreDefsAndUses_ == true) {
    mDefs.Clear();
    mUses.Clear();
  } // if
  AppendComment(FORMATSTR(" uses: " << mUses << " defs: " << mDefs));
  EMDEBUG0();
} // Instruction::Instruction

Instruction::
Instruction(const tBigInt& length_, const string& disassembly_, const DefRegSet& defs_, const UseRegSet& uses_, bool ignoreDefsAndUses_) :
  AsmSequenceItem(length_),
  mDefs(defs_),
  mUses(uses_),
  mDisassembly(disassembly_)
{
  BMDEBUG5("Instruction::Instruction", &length_, disassembly_, &defs_, &uses_);
  if (ignoreDefsAndUses_ == true) {
    mDefs.Clear();
    mUses.Clear();
  } // if
  AppendComment(FORMATSTR(" uses: " << mUses << " defs: " << mDefs));
  EMDEBUG0();
} // Instruction::Instruction

Instruction::
Instruction(const Instruction& inst_) :
  AsmSequenceItem(inst_),
  mOperands(inst_.mOperands)
{
  BMDEBUG2("Instruction::Instruction", &inst_);
  AppendComment(FORMATSTR(" uses: " << mUses << " defs: " << mDefs));
  EMDEBUG0();
} // Instruction::Instruction

void IRExprNodeMatcherInfo::
SetCostInfo(NTType ntType_, hReal32 cost_, ExprRule* exprRule_, ExprRule* convRule_) {
  mCostVector[ntType_] = make_pair(cost_, exprRule_);
  mNTConvRule[ntType_] = convRule_;
  if (cost_ < mCostVector[NTTLast].first) {
    mCostVector[NTTLast] = mCostVector[ntType_];
    mNTConvRule[NTTLast] = mNTConvRule[ntType_];
  } // if
  return;
} // IRExprNodeMatcherInfo::SetCostInfo

AsmSeqData::
AsmSeqData(AsmSeqLabel* label_, IRRelocExpr* value_, const tBigInt& length_) :
  AsmSequenceItem(0),
  mLabel(label_)
{
  BMDEBUG4("AsmSeqData::AsmSeqData", label_, value_, &length_);
  REQUIREDMSG(mLabel != 0, ieStrNonNullParam);
  Add(value_, length_);
  EMDEBUG0();
} // AsmSeqData::AsmSeqData

AsmSeqData::
AsmSeqData(AsmSeqLabel* label_, IRRelocExpr* value_) :
  AsmSequenceItem(0),
  mLabel(label_)
{
  BMDEBUG3("AsmSeqData::AsmSeqData", label_, value_);
  REQUIREDMSG(mLabel != 0, ieStrNonNullParam);
  Add(value_);
  EMDEBUG0();
} // AsmSeqData::AsmSeqData

AsmSeqData::
AsmSeqData(AsmSeqLabel* label_) :
  AsmSequenceItem(0),
  mLabel(label_)
{
  BMDEBUG2("AsmSeqData::AsmSeqData", label_);
  REQUIREDMSG(mLabel != 0, ieStrNonNullParam);
  EMDEBUG0();
} // AsmSeqData::AsmSeqData

AsmSeqData& AsmSeqData::
Add(IRRelocExpr* value_) {
  BMDEBUG2("AsmSeqData::Add", value_);
  REQUIREDMSG(value_ != 0, ieStrParamValuesDBCViolation);
  mValue.push_back(value_);
  EMDEBUG0();
  return *this;
} // AsmSeqData::Add

AsmSeqData& AsmSeqData::
Add(IRRelocExpr* value_, const tBigInt& length_) {
  BMDEBUG3("AsmSeqData::Add", value_, &length_);
  REQUIREDMSG(value_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(length_ > 0, ieStrParamValuesDBCViolation);
  IRBuilder& irBuilder = Singleton<IRBuilder>::Obj();
  //! \todo M Design: It is better to use in bytes 1 here.
  IRRelocExpr* lCasted(irBuilder.irbeCast(
    irBuilder.irbtGetArray(irBuilder.irbtGetInt(INBITS(8), 0, IRSUnsigned), length_, 0), value_));
  mValue.push_back(lCasted);
  EMDEBUG0();
  return *this;
} // AsmSeqData::Add

bool HWDescription::
matchStmt(IRStmt* stmt_, vector<StmtRule*>& results_) {
  BMDEBUG2("HWDescription::matchStmt", stmt_);
  REQUIREDMSG(stmt_ != 0, ieStrNonNullParam);
  REQUIREDMSG(results_.empty() == true, ieStrParamValuesDBCViolation);
  for (hFUInt32 c(0); c < mStmtRules.size(); ++ c) {
    StmtRule* lCurrRule(mStmtRules[c]);
    if (lCurrRule->IsStmtPatternMatch(stmt_) == true) {
      if (lCurrRule->CheckCondition(stmt_) == true) {
        StmtRule* lClonedRule(lCurrRule->Clone());
        lClonedRule->SetMatchedStmt(stmt_);
        lClonedRule->OnPatternMatch(stmt_);
        results_.push_back(lClonedRule);
      } // if
    } // if
  } // for
  EMDEBUG1(!results_.empty());
  return !results_.empty();
} // HWDescription::matchStmt

bool HWDescription::
coverTree(const NodeInfo& topIRNode_, CGFuncContext& context_, bool isRecFirstCall_) {
  BMDEBUG3("HWDescription::coverTree", &context_, isRecFirstCall_);
  bool retVal(true);
  IRExprNodeMatcherInfo* lNodeInfo(context_.mIRNodeMatchInfo[topIRNode_.mIRExpr]);
  ASSERTMSG(lNodeInfo != 0, ieStrInconsistentInternalStructure << 
    " A node that is not required to be matched by framework is not covered by upper tree "
    " nodes. Did you create proper IR, e.g. use of objects is one frequently missed point?");
  NTType lTopNodeNTType(topIRNode_.mNT->GetNTType());
  ExprRule* lTopNodeRule(lNodeInfo->GetExprRule(lTopNodeNTType));
  ASSERTMSG(lNodeInfo->GetCost(lTopNodeNTType) != consts.cInfiniteCost,
    ieStrInconsistentInternalStructure << ": Should not have matched");
  vector<NodeInfo> lLeaves;
  /* LIE */ lTopNodeRule->GetIRLeaves(topIRNode_.mIRExpr, lLeaves);
  for (hFUInt32 c(0); c < lLeaves.size(); ++ c) {
    if (!coverTree(lLeaves[c], context_, consts.cRecNotFirstCall)) {
      retVal = false;
      break;
    } // if
  } // for
  if (retVal == true) {
    ASSERTMSG(topIRNode_.mIRExpr == lTopNodeRule->GetMatchedExpr(), ieStrInconsistentInternalStructure);
    // Before we call the OnCover we need to set the leaf non-terminals,
    // from the child rule result non-terminals.
    // It is possible to merge this loop with the above one. But here
    // code is more understandable.
    for (hFUInt32 c(0); c < lLeaves.size(); ++ c) {
      mergeLeafAndResultNTs(lTopNodeRule, lLeaves[c], context_);
    } // for
    lTopNodeRule->OnCover(topIRNode_.mIRExpr, context_);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // HWDescription::coverTree

void HWDescription::
mergeLeafAndResultNTs(Rule* rule_, const NodeInfo& nodeInfo_, CGFuncContext& context_) {
  BMDEBUG3("HWDescription::mergeLeafAndResultNTs", rule_, &context_);
  PDEBUG("CodeGen", "detail", "Merging leaf of rule: " << hwdesccxt(rule_));
  // Get the ExprRule that has the result non-terminal of leaves[c].
  IRExprNodeMatcherInfo* lLeafInfo(context_.mIRNodeMatchInfo[nodeInfo_.mIRExpr]);
  ExprRule* lLeafRule(lLeafInfo->GetMinCostRule(nodeInfo_.mNT->GetNTType()));
  ExprRule* lLeafConvRule(lLeafInfo->GetMinCostConvRule(nodeInfo_.mNT->GetNTType()));
  // Get the resultNT of the leaf rule.
  NonTerminal* lResultNT(lLeafRule->GetResultNT());
  if (!pred.pIsInvalid(lLeafConvRule)) {
    //! \attention Assumption: Here we assume that conversion rules
    //!            have only 2 nonterminal patterns, at index 0, we
    //!            have the result nonterminal, at index 1 we have the
    //!            operand nonterminal.
    NOTIMPLEMENTED(H);
    lLeafConvRule->setPatternNT(1, lResultNT);
    lResultNT = lLeafConvRule->GetResultNT();
  } // if
  vector<pair<VirtReg*, VirtReg*> > lUnmerged;
  lResultNT->MergeHWRegs(*nodeInfo_.mNT, lUnmerged);
  ASSERTMSG(lUnmerged.empty() == true, ieStrNotImplemented);
  // Get the pattern non-terminal index (id) of the leaf.
  rule_->ReplaceNT(nodeInfo_.mNT, lResultNT);
  PDEBUG("CodeGen", "detail", "Merged leaf and result NTs:");
  PDEBUG("CodeGen", "detail", "Rule: " << hwdesccxt(rule_));
  PDEBUG("CodeGen", "detail", "Leaf rule: " << hwdesccxt(lLeafRule));
  PDEBUG("CodeGen", "detail", "Leaf conv rule: " << hwdesccxt(lLeafConvRule));
  EMDEBUG0();
} // HWDescription::mergeLeafAndResultNTs

bool HWDescription::
coverStmt(IRStmt* matchedStmt_, StmtRule* matchedStmtRule_, CGFuncContext& context_) {
  BMDEBUG4("HWDescription::coverStmt", matchedStmt_, matchedStmtRule_, &context_);
  PDEBUG("CodeGen", "detail", "Cover for stmt: " << progcxt(matchedStmt_));
  bool retVal(true);
  vector<NodeInfo> lLeaves;
  if (matchedStmtRule_->GetIRLeaves(matchedStmt_, lLeaves) == true) {
    for (hFUInt32 c(0); c < lLeaves.size(); ++ c) {
      if (dynamic_cast<IgnoreNT*>(lLeaves[c].mNT) != 0) {
        // LIE: Do nothing, do not generate code
      } else if (coverTree(lLeaves[c], context_, consts.cRecFirstCall) != true) {
        retVal = false;
        break;
      } else {
        mergeLeafAndResultNTs(matchedStmtRule_, lLeaves[c], context_);
      } // if
    } // for
  } // if
  matchedStmtRule_->OnCover(matchedStmtRule_->GetMatchedStmt(), context_);
  EMDEBUG1(retVal);
  return retVal;
} // HWDescription::coverStmt

bool HWDescription::
generateStmt(IRStmt* stmt_, CGFuncContext& context_) {
  BMDEBUG3("HWDescription::generateStmt", stmt_, &context_);
  PDEBUG("CodeGen", "", "Generating code for stmt: " << progcxt(stmt_) << "{");
  vector<StmtRule*> lMatchedStmts;
  bool retVal(false);
  // Match is done per statement so clear out previous match results.
  context_.mIRNodeMatchInfo.clear();
  OnStmtStart(stmt_, context_);
  if (matchStmt(stmt_, lMatchedStmts) == true) {
    PDEBUG("CodeGen", "", "Matched StmtRule: " << refcxt(stmt_) << " pattern:" << hwdesccxt(lMatchedStmts[0]));
    //! \todo M Design: Check if more than one statement match could have an advantage?
    //! \todo M Design: misUse of progcxt of rules should not cause crash.
    ASSERTMSG(lMatchedStmts.size() == 1, ieStrInconsistentInternalStructure <<
      ": There must be exactly 1 rule match of statement: " << lMatchedStmts.size() << endl <<
      "StmtRule1: " << hwdesccxt(lMatchedStmts[0]) << endl <<
      "StmtRule2: " << hwdesccxt(lMatchedStmts[1]) << endl <<
      "Stmt: " << progcxt(stmt_));
    retVal = true;
    vector<NodeInfo> lLeaves;
    if (lMatchedStmts[0]->GetIRLeaves(stmt_, lLeaves) == true) {
      for (hFSInt32 c(lLeaves.size()-1); c >= 0; -- c) {
        IRExpr* lCurrLeaf(lLeaves[c].mIRExpr);
        if (pred.pIsObj(lCurrLeaf) == true) {
          // LIE: do not match objects
        } else if (matchTree(lCurrLeaf, context_) == true) {
          NTType lCurrLeafNTType = lLeaves[c].mNT->GetNTType();
          if (lCurrLeafNTType == NTTIgnore) {
            // LIE: Do nothing
          } else if (lCurrLeafNTType == NTTAny) {
            bool lAnyMatch(false);
            for (NTType ntType(NTTFirst); ntType < NTTLast; ntType = (NTType)((hFInt32)ntType + 1)) {
              if (context_.mIRNodeMatchInfo[lCurrLeaf]->GetCost(ntType) != consts.cInfiniteCost) {
                context_.mIRNodeMatchInfo[lCurrLeaf]->mCostVector[NTTAny] =
                  context_.mIRNodeMatchInfo[lCurrLeaf]->mCostVector[ntType];
                lAnyMatch = true;
                break;
              } // if
            } // for
            if (!lAnyMatch) {
              retVal = false;
              break;
            } // if
          } else if (context_.mIRNodeMatchInfo[lCurrLeaf]->GetCost(lCurrLeafNTType) == consts.cInfiniteCost) {
            //! \todo M Design: Here dump the list of IR nodes that did not match any rule.
            ASSERTMSG(!context_.IsMustMatch(), ieStrDBCViolation <<
              ": Expression tree is not matched, due to unmatched NTType=" << lCurrLeafNTType << 
              ". \nStmt=" << progcxt(stmt_) << "\nStmtRule=" << hwdesccxt(lMatchedStmts[0]) <<
              "\nExpr Tree=" << matcherrcxt(lCurrLeaf, context_) << endl << sfMatchHelp());
            retVal = false;
            break;
          } // if
          // Current leaf is okay, continue with the next.
          continue;
        } else {
          ASSERTMSG(!context_.IsMustMatch(), ieStrDBCViolation <<
            ": Expression tree is not matched, most probably due to missing rule, or rule conditions");
          retVal = false;
          break;
        } // if
      } // for
    } // if
    if (retVal == true) {
      retVal = coverStmt(lMatchedStmts[0]->GetMatchedStmt(), lMatchedStmts[0], context_);
    } else {
      ASSERTMSG(!context_.IsMustMatch(), ieStrParamValuesDBCViolation << ": Expression tree is not matched");
    } // if
  } else {
    ASSERTMSG(!context_.IsMustMatch(), ieStrParamValuesDBCViolation << ": Statement is not matched");
  } // if
  PDEBUG("CodeGen", "", "} // Generated code for stmt");
  EMDEBUG1(retVal);
  return retVal;
} // HWDescription::generateStmt

void Rule::
setPatternNT(hFUInt32 patternIndex_, NonTerminal* newNT_) {
  BMDEBUG3("Rule::setPatternNT", patternIndex_, newNT_);
  REQUIREDMSG(newNT_ != 0, ieStrNonNullParam);
  REQUIREDMSG(mPatternNTs.size() > patternIndex_, ieStrInconsistentInternalStructure);
  mPatternNTs[patternIndex_] = newNT_;
  EMDEBUG0();
} // Rule::setPatternNT

bool HWDescription::
generateBB(IRBB* bb_, CGFuncContext& context_) {
  BMDEBUG2("HWDescription::generateBB", bb_);
  bool retVal(true);
  OnBBStart(bb_, context_);
  mCurrBB = static_cast<CGBB*>(bb_);
  ListIterator<IRStmt*> lStmtIt(bb_->GetStmtIter());
  for (lStmtIt.First(); !lStmtIt.IsDone(); lStmtIt.Next()) {
    if (!generateStmt(*lStmtIt, context_)) {
      retVal = false;
      break;
    } // if
  } // for
  mCurrBB = 0;
  EMDEBUG1(retVal);
  return retVal;
} // HWDescription::generateBB

//! \todo H Design: implement a state machine where you can control the
//!       state changes that must happen in the compiler for DBC.
bool HWDescription::
GenerateFunc(IRFunction* irFunc_, CGFuncContext& context_) {
  BMDEBUG2("HWDescription::GenerateFunc", irFunc_);
  PDEBUG("CodeGen", "", "Generating code for function: " << refcxt(irFunc_) << "{");
  REQUIREDMSG(irFunc_ != 0, ieStrNonNullParam);
  bool retVal(true);
  CFG* lCFG(irFunc_->GetCFG());
  OnFuncStart(irFunc_, context_);
  irFunc_->AssignStackAddrToLocals(&context_);
  //! \todo H Design: Correct the stack size considering other things.
  irFunc_->SetStackSize(irFunc_->GetLocalsSize()+irFunc_->GetParamsSize());
  ListIterator<IRBB*> lBBIter(lCFG->GetPhysBBIter());
  for (lBBIter.First(); !lBBIter.IsDone(); lBBIter.Next()) {
    if (!generateBB(*lBBIter, context_)) {
      retVal = false;
      break;
    } // if
  } // for
  bool lSuccess(context_.GetRegisterAllocator()->Start(context_));
  OnFuncEnd(irFunc_, context_);
  //! \todo H Design: Actually must check with mustmatch...
  ASSERTMSG(lSuccess == true, ieStrInconsistentInternalStructure);
  PDEBUG("CodeGen", "", "} // Finished code generation for function: " << refcxt(irFunc_));
  EMDEBUG1(retVal);
  return retVal;
} // HWDescription::GenerateFunc

bool HWDescription::
matchExpr(IRExpr* irExpr_, const vector<ExprRule*>& rules_, vector<ExprRule*>& results_) {
  BMDEBUG2("HWDescription::matchExpr", irExpr_);
  REQUIREDMSG(irExpr_ != 0, ieStrNonNullParam);
  REQUIREDMSG(results_.empty() == true, ieStrParamValuesDBCViolation);
  for (hFUInt32 c(0); c < rules_.size(); ++ c) {
    ExprRule* lCurrRule(rules_[c]);
    if (lCurrRule->IsExprPatternMatch(irExpr_) == true) {
      if (lCurrRule->CheckCondition(irExpr_) == true) {
        ExprRule* lClonedRule(lCurrRule->Clone());
        lClonedRule->SetMatchedExpr(irExpr_);
        lClonedRule->OnPatternMatch(irExpr_);
        results_.push_back(lClonedRule);
        PDEBUG("CodeGen", "detail", "expr rule matched: " << hwdesccxt(lClonedRule) << " to Expr " <<
          refcxt(irExpr_) << " @" << refcxt(extr.eGetStmt(irExpr_)));
      } // if
    } // if
  } // for
  EMDEBUG1(!results_.empty());
  return !results_.empty();
} // HWDescription::matchExpr

bool HWDescription::
updateCostVectorOf(ExprRule* exprRule_, CGFuncContext& context_) {
  BMDEBUG3("HWDescription::updateCostVectorOf", exprRule_, &context_);
  IRExpr* lIRNode(exprRule_->GetMatchedExpr());
  vector<NodeInfo> lLeaves;
  hReal32 lRuleCost(0.0F);
  bool retVal(false);
  // Calculate the cost of leaves.
  IRExprNodeMatcherInfo* lNodeInfo(context_.mIRNodeMatchInfo[lIRNode]);
  if (exprRule_->GetIRLeaves(lIRNode, lLeaves) == true) {
    for (hFUInt32 c(0); c < lLeaves.size(); ++ c) {
      IRExpr* lLeafIRExpr(lLeaves[c].mIRExpr);
      NTType lLeafNTType(lLeaves[c].mNT->GetNTType());
      IRExprNodeMatcherInfo* lCurrLeafInfo(context_.mIRNodeMatchInfo[lLeafIRExpr]);
      hReal32 lLeafCost(lCurrLeafInfo->GetCost(lLeafNTType));
      if (lLeafCost == consts.cInfiniteCost) {
        lRuleCost = consts.cInfiniteCost;
        break;
      } // if
      lRuleCost += lLeafCost;
    } // for
  } // if
  if (lRuleCost != consts.cInfiniteCost) {
    // Add the cost of the rule
    lRuleCost += exprRule_->GetCost();
    // Update the cost vector by adding the conversion costs to the ruleCost.
    // We should update the cost vector only when the resultant cost is smaller.
    // Note that we may not get all possible non-terminal types, we only process
    // the ones that can be converted by mNTConvRules.
    vector<ExprRule*> lConvNTs;
    NTType lExprRuleNTType = exprRule_->GetResultNT()->GetNTType();
    //! \todo M Design: Not sure if this condition should be here.
    if (lNodeInfo->GetCost(lExprRuleNTType) > lRuleCost) {
      lNodeInfo->SetCostInfo(lExprRuleNTType, lRuleCost, exprRule_, &Invalid<ExprRule>::Obj());
      retVal = true;
    } // if
    if (matchExpr(exprRule_->GetResultNT(), mNTConvRules, lConvNTs) == true) {
      for (hFUInt32 e(0); e < lConvNTs.size(); ++ e) {
        NTType lDestNT(lConvNTs[e]->GetResultNT()->GetNTType());
        if (lDestNT != lExprRuleNTType) {
          hReal32 lTotalCost(lRuleCost + lConvNTs[e]->GetCost());
          if (lNodeInfo->GetCost(lDestNT) > lTotalCost) {
            lNodeInfo->SetCostInfo(lDestNT, lTotalCost, exprRule_, lConvNTs[e]);
            retVal = true;
          } // if
        } // if
      } // for
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // HWDescription::updateCostVectorOf

bool HWDescription::
matchTree(IRExpr* irExpr_, CGFuncContext& context_) {
  BMDEBUG2("HWDescription::matchTree", irExpr_);
  bool retVal(true);
  vector<IRExpr*> lPostOrderNodes;
  extr.eGetPostOrderTraversal(irExpr_, lPostOrderNodes);
  for (hFUInt32 c(0); c < lPostOrderNodes.size(); ++ c) {
    IRExpr* lCurrIRNode(lPostOrderNodes[c]);
    IRExprNodeMatcherInfo* mMatchInfo(new IRExprNodeMatcherInfo);
    context_.mIRNodeMatchInfo[lCurrIRNode] = mMatchInfo;
    if (pred.pIsObj(lCurrIRNode) == true) {
      // LIE: do not match objects
    } else if (matchExpr(lCurrIRNode, mExprRules, mMatchInfo->mMatchedRules) == true) {
      for (hFUInt32 d(0); d < mMatchInfo->mMatchedRules.size(); ++ d) {
        updateCostVectorOf(mMatchInfo->mMatchedRules[d], context_);
      } // for
    } else if (c == (lPostOrderNodes.size() - 1)) {
      retVal = false;
      ASSERTMSG(!context_.IsMustMatch(), ieStrInconsistentInternalStructure <<
        ": could not match the expression tree: " << progcxt(lCurrIRNode) << "\n of stmt: " <<
        progcxt(extr.eGetStmt(lCurrIRNode)));
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // HWDescription::matchTree

bool ExprRule::
IsExprPatternMatch(IRExpr* candidIRExpr_) {
  BMDEBUG2("ExprRule::IsExprPatternMatch", candidIRExpr_);
  bool retVal(sIsExprPatternMatch(candidIRExpr_, mExprPattern));
  EMDEBUG1(retVal);
  return retVal;
} // ExprRule::IsExprPatternMatch

bool NonTerminal::
MergeHWRegs(const NonTerminal& nt_, vector<pair<VirtReg*, VirtReg*> >& unmerged_) {
  BMDEBUG2("NonTerminal::MergeHWRegs", &nt_);
  REQUIREDMSG(unmerged_.empty() == true, ieStrParamValuesDBCViolation);
  vector<VirtReg*> lVRegsThis;
  vector<VirtReg*> lVRegsNT;
  GetVRegs(lVRegsThis);
  nt_.GetVRegs(lVRegsNT);
  ASSERTMSG(lVRegsThis.size() == lVRegsNT.size(), ieStrInconsistentInternalStructure);
  tristate retVal;
  for (hFUInt32 c(0); c < lVRegsThis.size(); ++ c) {
    if (pred.pIsInvalid(lVRegsThis[c]->GetHWReg()) == true) {
      lVRegsThis[c]->SetHWReg(lVRegsNT[c]->GetHWReg());
    } else if (!pred.pIsInvalid(lVRegsNT[c]->GetHWReg())) {
      unmerged_.push_back(make_pair(lVRegsThis[c], lVRegsNT[c]));
    } // if
  } // for
  retVal = !unmerged_.empty();
  EMDEBUG1(retVal);
  return retVal;
} // NonTerminal::MergeHWRegs

bool ExprRule::
smatchExprNode(IRExpr* irExpr_, IRPatExpr* patExpr_) {
  BDEBUG3("ExprRule::smatchExprNode", irExpr_, patExpr_);
  tristate retVal;
  if (pred.pIsSameExprClass(irExpr_, patExpr_) == true) {
    retVal = true;
  } else if (dynamic_cast<IgnoreNT*>(patExpr_) != 0) {
    retVal = true;
  } else if (dynamic_cast<IRObject*>(patExpr_) != 0 && dynamic_cast<IRObject*>(irExpr_) != 0) {
    retVal = dynamic_cast<IROLocal*>(patExpr_) == 0 &&
             dynamic_cast<IROParameter*>(patExpr_) == 0 &&
             dynamic_cast<IROGlobal*>(patExpr_) == 0 &&
             dynamic_cast<IROTmp*>(patExpr_) == 0;
  } else if (dynamic_cast<IRECmp*>(patExpr_) != 0 && dynamic_cast<IRECmp*>(irExpr_) != 0) {
    retVal = dynamic_cast<IREGt*>(patExpr_) == 0 &&
            dynamic_cast<IREGe*>(patExpr_) == 0 &&
            dynamic_cast<IRELt*>(patExpr_) == 0 &&
            dynamic_cast<IRELe*>(patExpr_) == 0 &&
            dynamic_cast<IRENe*>(patExpr_) == 0 &&
            dynamic_cast<IREEq*>(patExpr_) == 0;
  } else {
    retVal = false;
  } // if
  EDEBUG1(retVal);
  return retVal;
} // ExprRule::smatchExprNode

bool ExprRule::
sIsExprPatternMatch(IRExpr* irExpr_, IRPatExpr* patExpr_) {
  BDEBUG3("ExprRule::sIsExprPatternMatch", irExpr_, patExpr_);
  tristate retVal(true);
  if (smatchExprNode(irExpr_, patExpr_) == true) {
    vector<IRPatExpr*> lPatChildren;
    vector<IRExpr*> lChildren;
    patExpr_->GetChildren(lPatChildren);
    irExpr_->GetChildren(lChildren);
    ASSERTMSG(lChildren.size() == lPatChildren.size(), ieStrInconsistentInternalStructure);
    for (hFUInt32 c(0); c < lPatChildren.size(); ++ c) {
      if (!pred.pIsNonTerminal(lPatChildren[c])) {
        if (!sIsExprPatternMatch(lChildren[c], lPatChildren[c])) {
          retVal = false;
          break;
        } // if
      } // if
    } // for
  } else {
    retVal = false;
  } // if
  EDEBUG1(retVal);
  return retVal;
} // ExprRule::sIsExprPatternMatch

bool StmtRule::
GetIRLeaves(IRStmt* irTopNode_, vector<NodeInfo>& leaves_) const {
  BMDEBUG2("StmtRule::GetIRLeaves", irTopNode_);
  REQUIREDMSG(leaves_.empty() == true, ieStrParamValuesDBCViolation);
  vector<IRPatExpr*> lPatExprs;
  if (mStmtPattern->GetChildren(lPatExprs) == true) {
    if (!irTopNode_) {
      for (hFUInt32 c(0); c < lPatExprs.size(); ++ c) {
        if (NonTerminal* lNT = dynamic_cast<NonTerminal*>(lPatExprs[c])) {
          if (!dynamic_cast<IgnoreNT*>(lNT)) {
            leaves_.push_back(NodeInfo(0, lNT));
          } // if
        } else {
          ExprRule::sGetIRLeaves(0, lPatExprs[c], leaves_);
        } // if
      } // for
    } else {
      vector<IRExpr*> lExprs;
      irTopNode_->GetChildren(lExprs);
      ASSERTMSG(!lExprs.empty(), ieStrInconsistentInternalStructure);
      for (hFUInt32 c(0); c < lPatExprs.size(); ++ c) {
        if (NonTerminal* lNT = dynamic_cast<NonTerminal*>(lPatExprs[c])) {
          if (!dynamic_cast<IgnoreNT*>(lNT)) {
            leaves_.push_back(NodeInfo(lExprs[c], lNT));
          } // if
        } else {
          ExprRule::sGetIRLeaves(lExprs[c], lPatExprs[c], leaves_);
        } // if
      } // for
    } // if
  } // if
  EMDEBUG1(!leaves_.empty());
  return !leaves_.empty();
} // StmtRule::GetIRLeaves

void ExprRule::
sGetIRLeaves(IRExpr* irNode_, IRPatExpr* patNode_, vector<NodeInfo>& leaves_) {
  BDEBUG3("ExprRule::sGetIRLeaves", irNode_, patNode_);
  vector<IRPatExpr*> lPatChildren;
  if (patNode_->GetChildren(lPatChildren) == true) {
    if (!irNode_) {
      for (hFUInt32 c(0); c < lPatChildren.size(); ++ c) {
        if (NonTerminal* lNT = dynamic_cast<NonTerminal*>(lPatChildren[c])) {
          if (!dynamic_cast<IgnoreNT*>(lNT)) {
            leaves_.push_back(NodeInfo(0, lNT));
          } // if
        } else {
          sGetIRLeaves(0, lPatChildren[c], leaves_);
        } // if
      } // for
    } else {
      vector<IRExpr*> lChildren;
      irNode_->GetChildren(lChildren);
      for (hFUInt32 c(0); c < lPatChildren.size(); ++ c) {
        if (NonTerminal* lNT = dynamic_cast<NonTerminal*>(lPatChildren[c])) {
          if (!dynamic_cast<IgnoreNT*>(lNT)) {
            leaves_.push_back(NodeInfo(lChildren[c], lNT));
          } // if
        } else {
          sGetIRLeaves(lChildren[c], lPatChildren[c], leaves_);
        } // if
      } // for
    } // if
  } // if
  EDEBUG0();
} // ExprRule::sGetIRLeaves

bool ExprRule::
GetIRLeaves(IRExpr* irTopNode_, vector<NodeInfo>& leaves_) const {
  BMDEBUG2("ExprRule::GetIRLeaves", irTopNode_);
  REQUIREDMSG(leaves_.empty() == true, ieStrParamValuesDBCViolation);
  sGetIRLeaves(irTopNode_, mExprPattern, leaves_);
  EMDEBUG1(!leaves_.empty());
  return !leaves_.empty();
} // ExprRule::GetIRLeaves

void HWDescription::
AddRule(Rule* rule_) {
  BMDEBUG2("HWDescription::AddRule", rule_);
  REQUIREDMSG(rule_ != 0, ieStrNonNullParam);
  REQUIREDMSG(dynamic_cast<ExprRule*>(rule_) != 0 ||
    dynamic_cast<StmtRule*>(rule_) != 0, ieStrInconsistentInternalStructure);
  REQUIREDBLOCK() {
    for (hFUInt32 c(0); c < mExprRules.size(); ++ c) {
      REQUIREDMSG(rule_->GetName() != mExprRules[c]->GetName(), 
        ieStrParamValuesDBCViolation << " rule names must be unique expr rule: " << rule_->GetName());
    } // for
    for (hFUInt32 d(0); d < mStmtRules.size(); ++ d) {
      REQUIREDMSG(rule_->GetName() != mStmtRules[d]->GetName(),
        ieStrParamValuesDBCViolation << " rule names must be unique stmt rule: " << rule_->GetName());
    } // for
  } // REQUIREDBLOCK
  if (ExprRule* exprRule = dynamic_cast<ExprRule*>(rule_)) {
    mExprRules.push_back(exprRule);
    if (dynamic_cast<NonTerminal*>(exprRule->GetPatRoot())) {
      mNTConvRules.push_back(exprRule);
    } // if
  } else {
    mStmtRules.push_back(static_cast<StmtRule*>(rule_));
  } // if
  EMDEBUG0();
} // HWDescription::AddRule

void StmtPrologEpilogRule::
OnCover(IRStmt* irStmt_, CGFuncContext& context_) {
  BMDEBUG3("StmtPrologEpilogRule::OnCover", irStmt_, &context_);
  // Get the calling convention of the function containing the statement.
  CallingConvention* callConv = extr.eGetCallConv(context_.GetFunc());
  OnCover(irStmt_, context_, callConv);
  EMDEBUG0();
  return;
} // StmtPrologEpilogRule::OnCover

bool StmtRule::
IsStmtPatternMatch(IRStmt* candidIRStmt_) {
  BMDEBUG2("StmtRule::IsStmtPatternMatch", candidIRStmt_);
  REQUIREDMSG(candidIRStmt_->GetStmtType() != IRSTInvalid, ieStrInconsistentInternalStructure);
  bool retVal(false);
  if (typeid(*candidIRStmt_) == typeid(*mStmtPattern)) {
    retVal = true;
    vector<IRPatExpr*> lPatExprs;
    if (mStmtPattern->GetChildren(lPatExprs) == true) {
      vector<IRExpr*> lExprs;
      candidIRStmt_->GetChildren(lExprs);
      ASSERTMSG(!lExprs.empty(), ieStrInconsistentInternalStructure);
      for (hFUInt32 c(0); c < lPatExprs.size(); ++ c) {
        if (!pred.pIsNonTerminal(lPatExprs[c])) {
          if (!ExprRule::sIsExprPatternMatch(lExprs[c], lPatExprs[c])) {
            retVal = false;
            break;
          } // if
        } // if
      } // for
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // StmtRule::IsStmtPatternMatch

void CGBB::
AddInst(AsmSequenceItem* inst_) {
  BMDEBUG2("CGBB::AddInst", inst_);
  mInsts.push_back(inst_);
  inst_->mParent = this;
  EMDEBUG0();
  return;
} // CGBB::AddInst

void VirtReg::
ReplaceRegAllocRuleReg(const VirtReg* vRegOld_, VirtReg* vRegNew_) const {
  BMDEBUG3("VirtReg::ReplaceRegAllocRuleReg", vRegOld_, vRegNew_);
  for (hFInt32 c(0); c < mRegAllocRules.size(); ++ c) {
    mRegAllocRules[c]->ReplaceVRegs(vRegOld_, vRegNew_);
  } // for
  EMDEBUG0();
  return;
} // VirtReg::ReplaceRegAllocRuleReg

Target* CGContext::
GetTarget() const {
  return mHWDesc->GetTarget();
} // CGContext::GetTarget

void RegAllocRule::
AddRegAllocRule(VirtReg* vReg_) {
  BMDEBUG2("RegAllocRule::AddRegAllocRule", vReg_);
  vReg_->mRegAllocRules.push_back(this);
  EMDEBUG0();
  return;
} // RegAllocRule::AddRegAllocRule

void RegAllocRule::
AddRegAllocRule(const RegSet& vRegs_) {
  BMDEBUG2("RegAllocRule::AddRegAllocRule", &vRegs_);
  set<Register*>::const_iterator lRit(vRegs_.GetRegs().begin());
  for (/* LIE */; lRit != vRegs_.GetRegs().end(); ++ lRit) {
    ASSERTMSG(dynamic_cast<VirtReg*>(*lRit) != 0, ieStrInconsistentInternalStructure);
    VirtReg* lVReg(static_cast<VirtReg*>(*lRit));
    lVReg->mRegAllocRules.push_back(this);
  } // for
  EMDEBUG0();
  return;
} // RegAllocRule::AddRegAllocRule

bool RARMustNotBeSame::
CanHWRegBeAllocated(HWReg* hwReg_, const VirtReg* vReg_, const RegSet& availHWRegs_) const {
  BMDEBUG4("RARMustNotBeSame::CanHWRegBeAllocated", hwReg_, vReg_, &availHWRegs_);
  bool retVal(true);
  // Note that there is no need to use availHWRegs_.
  const set<Register*>& lRegs(mRegs.GetRegs());
  for (set<Register*>::const_iterator lRit(lRegs.begin()); lRit != lRegs.end(); ++ lRit) {
    ASSERTMSG(dynamic_cast<VirtReg*>(*lRit), ieStrInconsistentInternalStructure);
    VirtReg* lVReg(static_cast<VirtReg*>(*lRit));
    HWReg* lHWReg(lVReg->GetHWReg());
    if (!pred.pIsInvalid(lHWReg) && hwReg_ == lHWReg) {
      retVal = false;
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // RARMustNotBeSame::CanHWRegBeAllocated

bool RARMustBeInList::
CanHWRegBeAllocated(HWReg* hwReg_, const VirtReg* vReg_, const RegSet& availHWRegs_) const {
  BMDEBUG4("RARMustBeInList::CanHWRegBeAllocated", hwReg_, vReg_, &availHWRegs_);
  bool retVal(false);
  for (hFInt32 c(0); c < mVRegs.size(); ++ c) {
    if (mVRegs[c] == vReg_) {
      for (hFInt32 d(0); d < mValidVectors->size(); ++ d) {
        const vector<HWReg*>& lCurrHWSet((*mValidVectors)[d]);
        bool lVectorOk(false);
        if (lCurrHWSet[c] == hwReg_) {
          lVectorOk = true;
          for (hFInt32 e(0); e < lCurrHWSet.size(); ++ e) {
            if (!availHWRegs_.HasReg(hwReg_)) {
              lVectorOk = false;
            } // if
          } // for
        } // if
        if (lVectorOk == true) {
          retVal = true;
          break;
        } // if
      } // for
    } // if
    if (retVal == true) {
      break;
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // RARMustBeInList::CanHWRegBeAllocated

void RARMustBeInList::
ReplaceVRegs(const VirtReg* vRegOld_, VirtReg* vRegNew_) {
  BMDEBUG3("RARMustBeInList::ReplaceVRegs", vRegOld_, vRegNew_);
  bool lRegFound(false);
  for (hFInt32 c(0); c < mVRegs.size(); ++ c) {
    if (mVRegs[c] == vRegOld_) {
      mVRegs[c] = vRegNew_;
      lRegFound = true;
      break;
    } // if
  } // for
  ENSUREMSG(lRegFound == true, ieStrParamValuesDBCViolation << endl << progcxt(this));
  EMDEBUG0();
  return;
} // RARMustBeInList::ReplaceVRegs

void RARMustNotBeSame::
ReplaceVRegs(const VirtReg* vRegOld_, VirtReg* vRegNew_) {
  BMDEBUG3("RARMustNotBeSame::ReplaceVRegs", vRegOld_, vRegNew_);
  REQUIREDMSG(mRegs.HasReg(vRegOld_) == true, ieStrParamValuesDBCViolation);
  mRegs -= vRegOld_;
  mRegs |= vRegNew_;
  EMDEBUG0();
  return;
} // RARMustNotBeSame::ReplaceVRegs

RARMustNotBeSame::
RARMustNotBeSame(const RegSet& regs_) :
  mRegs(regs_)
{
  BMDEBUG2("RARMustNotBeSame::RARMustNotBeSame", &regs_);
  AddRegAllocRule(regs_);
  //! \todo M Design: Add a DBC check that all incoming args' hwregs are different.
  EMDEBUG0();
} // RARMustNotBeSame::RARMustNotBeSame

RARMustBeInList::
RARMustBeInList(const vector<VirtReg*>& vRegs_, const vector<vector<HWReg*> >* validVectors_) :
  mVRegs(vRegs_),
  mValidVectors(validVectors_)
{
  BMDEBUG1("RARMustBeInList::RARMustBeInList");
  AddRegAllocRule(vRegs_);
  EMDEBUG0();
} // RARMustBeInList::RARMustBeInList

void RegAllocRule::
AddRegAllocRule(const vector<VirtReg*>& vRegs_) {
  BMDEBUG1("RegAllocRule::AddRegAllocRule");
  for (hFInt32 c(0); c < vRegs_.size(); ++ c) {
    vRegs_[c]->mRegAllocRules.push_back(this);
  } // for
  EMDEBUG0();
  return;
} // RegAllocRule::AddRegAllocRule

VirtReg* VirtReg::
Clone() const {
  BMDEBUG1("VirtReg::Clone");
  VirtReg* retVal(new VirtReg(*this));
  retVal->ReplaceRegAllocRuleReg(this, retVal);
  PDEBUG("VirtReg", "detail", progcxt(this) << endl << " cloned as: " << progcxt(retVal));
  EMDEBUG1(retVal);
  return retVal;
} // VirtReg::Clone

RegSet::
RegSet(const vector<HWReg*>& regs_) {
  BMDEBUG1("RegSet::RegSet");
  for (hFInt32 c(0); c < regs_.size(); ++ c) {
    mRegs.insert(regs_[c]);
  } // for
  EMDEBUG0();
} // RegSet::RegSet

RegSet::
RegSet(const vector<VirtReg*>& regs_) {
  BMDEBUG1("RegSet::RegSet");
  for (hFInt32 c(0); c < regs_.size(); ++ c) {
    mRegs.insert(regs_[c]);
  } // for
  EMDEBUG0();
} // RegSet::RegSet

RegSet::
RegSet(const vector<Register*>& regs_) {
  BMDEBUG1("RegSet::RegSet");
  for (hFInt32 c(0); c < regs_.size(); ++ c) {
    mRegs.insert(regs_[c]);
  } // for
  EMDEBUG0();
} // RegSet::RegSet

void StmtRule::
diAddCallFrameInst(IRFunction* func_, CFInst* inst_) {
  extr.eGetDwarfDIE(func_)->GetFDE()->AddInst(inst_);
} // StmtRule::diAddCallFrameInst

const AddrLTConst& AsmSequenceItem::
GetAddr(CGFuncContext& context_) {
  if (!mAddr.IsAssigned()) {
    mAddr = AddrLTConst(context_.GetALocalLabel());
  } // if
  return mAddr;
} // AsmSequenceItem::GetAddr

void HWDescription::
OnFuncEnd(IRFunction* func_, CGFuncContext& context_) {
  BMDEBUG3("HWDescription::OnFuncEnd", func_, &context_);
  const vector<IROParameter*>& lParams(func_->GetDeclaration()->GetParams());
  for (hFUInt32 c(0); c < lParams.size(); ++ c) {
    OnParam(func_, lParams[c], context_);
  } // if
  const vector<IROLocal*>& lLocals(func_->GetLocals());
  for (hFUInt32 d(0); d < lLocals.size(); ++ d) {
    OnLocal(func_, lLocals[d], context_);
  } // if
  EMDEBUG0();
  return;
} // HWDescription::OnFuncEnd

CGContext& CGContext::
AddInst(const string& where_, const string& section_, AsmSequenceItem* inst_, const string& comment_) {
  BMDEBUG4("CGContext::AddInst", where_, inst_, comment_);
  REQUIREDMSG(where_ == "ProgBeg" || where_ == "ProgEnd" || where_ == "ModuleEnd" ||
      where_ == "ModuleBeg", ieStrParamValuesDBCViolation);
  REQUIREDMSG(section_ == "dir" || section_ == "data" || section_ == "text", ieStrParamValuesDBCViolation);
  if (!comment_.empty()) {
    inst_->AppendComment(comment_);
  } // if
  mInsts[where_ + "-" + section_].push_back(inst_);
  EMDEBUG1(this);
  return *this;
} // CGContext::AddInst

Instruction* Rule::
AddInst(CGFuncContext& context_, const string& instId_) {
  Instruction* retVal(CreateInst(context_, instId_));
  AddInst(context_, retVal);
  return retVal;
} // Rule::AddInst

Instruction* Rule::
AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_) {
  Instruction* retVal(CreateInst(context_, instId_, op0_));
  AddInst(context_, retVal);
  return retVal;
} // Rule::AddInst

Instruction* Rule::
AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, const GenInstOp& op1_) {
  Instruction* retVal(CreateInst(context_, instId_, op0_, op1_));
  AddInst(context_, retVal);
  return retVal;
} // Rule::AddInst

Instruction* Rule::
AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_) {
  Instruction* retVal(CreateInst(context_, instId_, op0_, op1_, op2_));
  AddInst(context_, retVal);
  return retVal;
} // Rule::AddInst

Instruction* Rule::
AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_) {
  Instruction* retVal(CreateInst(context_, instId_, op0_, op1_, op2_, op3_));
  AddInst(context_, retVal);
  return retVal;
} // Rule::AddInst

Instruction* Rule::
AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_, const GenInstOp& op4_) {
  Instruction* retVal(CreateInst(context_, instId_, op0_, op1_, op2_, op3_, op4_));
  AddInst(context_, retVal);
  return retVal;
} // Rule::AddInst

GenInstOp::
GenInstOp(const char* what_) :
  mKind(GIO_String)
{
  mValue.mString = new string(what_);
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(const string& what_) :
  mKind(GIO_String)
{
  mValue.mString = new string(what_);
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(Register* reg_) :
  mKind(GIO_Register)
{
  mValue.mReg = reg_;
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(const ConditionNT& what_) :
  mKind(GIO_Cond)
{
  mValue.mCondition = what_.GetCondition();
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(CNTType what_) :
  mKind(GIO_Cond)
{
  mValue.mCondition = what_;
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(const RegisterNT& what_) :
  mKind(GIO_Register)
{
  mValue.mReg = what_.GetReg();
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(const tBigInt& what_) :
  mKind(GIO_Int)
{
  mValue.mInt = new tBigInt(what_);
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(ShiftType what_) :
  mKind(GIO_Shift)
{
  mValue.mShift = what_;
} // GenInstOp::GenInstOp

GenInstOp::
GenInstOp(const RegSet& what_) :
  mKind(GIO_RegSet)
{
  mValue.mRegSet = new RegSet(what_);
} // GenInstOp::GenInstOp

Instruction* Rule::
CreateInst(CGContext& context_, const string& instId_) {
} // Rule::CreateInst

Instruction* Rule::
CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_) {
  InstructionSetDefinition* lISADef(context_.GetHWDesc()->GetISADef());
  return lISADef->CreateInst(instId_, 1, &op0_);
} // Rule::CreateInst

Instruction* Rule::
CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, const GenInstOp& op1_) {
  InstructionSetDefinition* lISADef(context_.GetHWDesc()->GetISADef());
  return lISADef->CreateInst(instId_, 2, &op0_, &op1_);
} // Rule::CreateInst

Instruction* Rule::
CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, const GenInstOp& op1_, const GenInstOp& op2_) {
  InstructionSetDefinition* lISADef(context_.GetHWDesc()->GetISADef());
  return lISADef->CreateInst(instId_, 3, &op0_, &op1_, &op2_);
} // Rule::CreateInst

Instruction* Rule::
CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_) {
  InstructionSetDefinition* lISADef(context_.GetHWDesc()->GetISADef());
  return lISADef->CreateInst(instId_, 4, &op0_, &op1_, &op2_, &op3_);
} // Rule::CreateInst

Instruction* Rule::
CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_, const GenInstOp& op4_) {
  InstructionSetDefinition* lISADef(context_.GetHWDesc()->GetISADef());
  return lISADef->CreateInst(instId_, 5, &op0_, &op1_, &op2_, &op3_, &op4_);
} // Rule::CreateInst

InstSetDef_Template* InstSetDef_Template::
AddOp(const string& name_) {
  mOperands.push_back(new InstSetDef_OpTemplate(name_, this));
  return this;
} // InstSetDef_Template::AddOp

InstSetDef_Template* InstSetDef_Template::
AddOpFlag(const string& name_, const string& value_) {
  mOperands.push_back(new InstSetDef_OpFlag(mParent->GetFlag(name_), value_, this)); 
  return this;
} // InstSetDef_Template::AddOpFlag

InstSetDef_Template* InstSetDef_Template::
AddOpFlag(const string& name_) {
  mOperands.push_back(new InstSetDef_OpFlag(mParent->GetFlag(name_), this)); 
  return this;
} // InstSetDef_Template::AddOpFlag

InstSetDef_Template* InstSetDef_Template::
AddOpRegDef(const string& regsType_, const string& id_) {
  mOperands.push_back(new InstSetDef_OpReg(id_, mParent->GetRegs(regsType_), this, consts.cDefOfReg));
  return this;
} // InstSetDef_Template::AddOpRegDef

InstSetDef_Template* InstSetDef_Template::
AddOpRegUse(const string& regsType_, const string& id_) {
  mOperands.push_back(new InstSetDef_OpReg(id_, mParent->GetRegs(regsType_), this, consts.cUseOfReg));
  return this;
} // InstSetDef_Template::AddOpRegUse

InstSetDef_Template* InstSetDef_Template::
AddOpRegSetUse(const string& name_) {
  mOperands.push_back(new InstSetDef_OpRegSet(mParent->GetRegs(name_), this)); 
  return this;
} // InstSetDef_Template::AddOpRegSetUse

InstSetDef_Template* InstSetDef_Template::
AddOpLabel() {
  mOperands.push_back(new InstSetDef_OpLabel(this)); 
  return this;
} // InstSetDef_Template::AddOpLabel

InstSetDef_Template* InstSetDef_Template::
AddOpInt() {
  mOperands.push_back(new InstSetDef_OpInt(this)); 
  return this;
} // InstSetDef_Template::AddOpInt

InstSetDef_Inst* InstSetDef_Inst::
UseTemplate(const string& name_) {
  mTemplate = name_;
  return this;
} // InstSetDef_Inst::UseTemplate

InstSetDef_Inst* InstSetDef_Inst::
AddRARule(RegAllocRule* rule_) {
  mRARules.push_back(rule_); 
  return this;
} // InstSetDef_Inst::AddRARule

InstSetDef_Inst* InstSetDef_Inst::
AddDef(const string& regName_) {
  mDefs |= mParent->GetHWDesc()->GetHWReg(regName_);
  return this;
} // InstSetDef_Inst::AddDef

InstSetDef_Inst* InstSetDef_Inst::
AddDefs(const char* reg0_, ...) {
  va_list lAP;
  va_start(lAP, reg0_);
  const char* lRegName(reg0_);
  mDefs |= mParent->GetHWDesc()->GetHWReg(reg0_);
  while(lRegName = va_arg(lAP, const char*)) {
    mDefs |= mParent->GetHWDesc()->GetHWReg(lRegName);
    ASSERTMSG(mDefs.GetSize() < 1024, ieStrParamValuesDBCViolation << 
      " Either null element is missing or number of arguments are not supported.");
  } // while
  va_end(lAP);
  return this;
} // InstSetDef_Inst::AddDefs

InstSetDef_Flag* InstSetDef_Flag::
Flags(const char* reg0_, ...) {
  BMDEBUG2("InstSetDef_Flag::Flags", reg0_);
  va_list lAP;
  va_start(lAP, reg0_);
  const char* lFlagName(reg0_);
  mFlags.insert(reg0_);
  mOptional = string(reg0_) == "";
  while(lFlagName = va_arg(lAP, const char*)) {
    if (!mOptional && string(lFlagName) == "") {
      mOptional = true;
    } // if
    mFlags.insert(lFlagName);
    PDEBUG("InstSetDef_Flag", "detail", "Flag added: " << lFlagName);
    ASSERTMSG(mFlags.size() < 1024, ieStrParamValuesDBCViolation << 
      " Either null element is missing or number of arguments are not supported.");
  } // while
  va_end(lAP);
  EMDEBUG0();
  return this;
} // InstSetDef_Regs::Regs

InstSetDef_Regs* InstSetDef_Regs::
Regs(const char* reg0_, ...) {
  BMDEBUG2("InstSetDef_Regs::Regs", reg0_);
  va_list lAP;
  va_start(lAP, reg0_);
  const char* lRegName(reg0_);
  mRegs |= mParent->GetHWDesc()->GetHWReg(reg0_);
  mRegNames.insert(reg0_);
  while(lRegName = va_arg(lAP, const char*)) {
    mRegs |= mParent->GetHWDesc()->GetHWReg(lRegName);
    ASSERTMSG(IsAReg(lRegName) == false, ieStrParamValuesDBCViolation << " register names may not be repeated.");
    mRegNames.insert(lRegName);
    PDEBUG("InstSetDef_Regs", "detail", "RegName added: " << lRegName);
    ASSERTMSG(mRegs.GetSize() < 1024, ieStrParamValuesDBCViolation << 
      " Either null element is missing or number of arguments are not supported.");
  } // while
  va_end(lAP);
  EMDEBUG0();
  return this;
} // InstSetDef_Regs::Regs

RARMustNotBeSame* RARBuilder::
GetRARMustNotBeSame(const char* reg0_, ...) {
  va_list lAP;
  va_start(lAP, reg0_);
  const char* lRegName(reg0_);
  RegSet lRegs;
  if (mISADef->GetHWDesc()->IsAHWReg(reg0_) == true) {
    lRegs |= mISADef->GetHWDesc()->GetHWReg(reg0_);
  } else if (mISADef->IsRegType(reg0_) == true) {
    ASSERTMSG(0, ieStrNotImplemented << " use of register sets are not yet supported: " << reg0_);
  } else {
    lRegs |= new VirtReg(reg0_);
  } // if
  while(lRegName = va_arg(lAP, const char*)) {
    if (mISADef->GetHWDesc()->IsAHWReg(lRegName) == true) {
      lRegs |= mISADef->GetHWDesc()->GetHWReg(lRegName);
    } else if (mISADef->IsRegType(lRegName) == true) {
      ASSERTMSG(0, ieStrNotImplemented << " use of register sets are not yet supported: " << lRegName);
    } else {
      lRegs |= new VirtReg(lRegName);
    } // if
    ASSERTMSG(lRegs.GetSize() < 1024, ieStrParamValuesDBCViolation << 
      " Either null element is missing or number of arguments are not supported.");
  } // while
  va_end(lAP);
  RARMustNotBeSame* retVal(new RARMustNotBeSame(lRegs));
  ENSUREMSG(lRegs.GetSize() > 1, ieStrParamValuesDBCViolation);
  return retVal;
} // RARBuilder::GetRARMustNotBeSame

bool InstructionSetDefinition::
parseOps(const InstSetDef_Template* template_, const vector<GenInstOp*>& ops_, hFSInt32& opsArgIndex_, 
    vector<InstSetDef_Op*>& outOps_, string& formatStr_, bool isFirstCall_) const {
  BMDEBUG2("InstructionSetDefinition::parseOps(template_)", template_->GetName());
  REQUIREDMSG(opsArgIndex_ >= 0 && opsArgIndex_ < ops_.size(), ieStrParamValuesDBCViolation << opsArgIndex_ << " ops_:" << ops_.size());
  // Format string cannot be concrete disassembly, it should be having format
  // specifiers for virtregs and after RA. We need to alter format string by
  // renumbering the position specifiers.
  const vector<InstSetDef_Op*>& lOps(template_->GetOperands());
  hFSInt32 lOpsArgIndex(opsArgIndex_);
  vector<InstSetDef_Op*> lOutOps;
  bool retVal(true);
  string lTemplateFormatStr(template_->mFormatStr);
  vector<hFSInt32> lMissingFlags;
  for (hFUInt32 c(0); c < lOps.size(); ++ c) {
    if (lOpsArgIndex >= ops_.size()) {
      // Template has more operands than the given instruction operands.
      retVal = false;
      break;
    } // if
    InstSetDef_Op* lCurrOp(lOps[c]);
    if (InstSetDef_OpTemplate* lTemplateOp = dynamic_cast<InstSetDef_OpTemplate*>(lCurrOp)) {
      vector<InstSetDef_Op*> lTmpOutOps;
      string lFormatStr;
      hFSInt32 lInOpsArgIndex(lOpsArgIndex);
      InstSetDef_Template* lMatchedTemplate(parseOps(lTemplateOp->GetName(), ops_, lOpsArgIndex, lTmpOutOps, lFormatStr, consts.cRecNotFirstCall));
      if (lMatchedTemplate != 0) {
        if (lFormatStr == "") {
          lMissingFlags.push_back(c);
        } // if
        lOutOps.insert(lOutOps.end(), lTmpOutOps.begin(), lTmpOutOps.end());
        if (template_->IsOR() == true) {
          lTemplateFormatStr = lFormatStr;
          break;
        } else {
          smRenumberFormatStr(lFormatStr, 0, isFirstCall_ == true ? c+1 : c);
          smRenumberFormatStr(lTemplateFormatStr, isFirstCall_ == true ? c+2 : c+1, lOpsArgIndex-lInOpsArgIndex);
          smReplaceFormatStr(lTemplateFormatStr, isFirstCall_ == true ? c+1 : c, lFormatStr);
        } // if
      } else if (template_->IsOR() == false) {
        retVal = false;
        break;
      } else {
        // LIE
      } // if
    } else {
      if (parseDoOpsMatch(lCurrOp, ops_[lOpsArgIndex]) == false) {
        if (isOptionalFlag(lCurrOp) == true && lCurrOp->IsValueSet() == false) {
          smReplaceFormatStr(lTemplateFormatStr, isFirstCall_ == true ? c+1:c, "");
          lMissingFlags.push_back(c);
          // LIE
        } else if (template_->IsOR() == false) {
          retVal = false;
          break;
        } else {
          // LIE
        } // if
      } else {
        hFSInt32 lOpPos(isFirstCall_ == true ? c+1:c);
        if (template_->IsOR() == true) {
          lOpPos = 0;
        } // if
        if (ops_[lOpsArgIndex]->IsInt() == true) {
          smReplaceFormatStr(lTemplateFormatStr, lOpPos, ops_[lOpsArgIndex]->GetInt().toStr());
        } else if (ops_[lOpsArgIndex]->IsString() == true) {
          smReplaceFormatStr(lTemplateFormatStr, lOpPos, ops_[lOpsArgIndex]->GetString());
        } else if (ops_[lOpsArgIndex]->IsReg() == true && pred.pIsHWReg(ops_[lOpsArgIndex]->GetReg()) == true) {
          smReplaceFormatStr(lTemplateFormatStr, lOpPos, ops_[lOpsArgIndex]->GetReg()->GetName());
        } // if
        lOutOps.push_back(lCurrOp);
        ++ lOpsArgIndex;
        if (template_->IsOR() == true) {
          break;
        } // if
      } // if
    } // if
  } // for
  if (retVal == true) {
    if (isFirstCall_ == true) {
      smReplaceFormatStr(lTemplateFormatStr, 0, "%s");
    } // if
    for (hFSInt32 c(lMissingFlags.size()-1); c >= 0; -- c) {
      smRenumberFormatStr(lTemplateFormatStr, isFirstCall_ == true ? lMissingFlags[c]+1 : lMissingFlags[c], -1);
    } // for
    formatStr_ = lTemplateFormatStr;
    opsArgIndex_ = lOpsArgIndex;
    outOps_.insert(outOps_.end(), lOutOps.begin(), lOutOps.end());
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // InstructionSetDefinition::parseOps

InstSetDef_Template* InstructionSetDefinition::
parseOps(const string& templateName_, const vector<GenInstOp*>& ops_, hFSInt32& opsArgIndex_, 
    vector<InstSetDef_Op*>& outOps_, string& formatStr_, bool isFirstCall_) const {
  BMDEBUG4("InstructionSetDefinition::parseOps(templateName_)", templateName_, opsArgIndex_, formatStr_);
  REQUIREDMSG(opsArgIndex_ >= 0 && opsArgIndex_ < ops_.size(), ieStrParamValuesDBCViolation);
  vector<InstSetDef_Template*> lTemplates;
  GetTemplates(templateName_, lTemplates);
  InstSetDef_Template* retVal(0);
  hFSInt32 lInOpsArgIndex(opsArgIndex_);
  for (hFUInt32 c(0); c < lTemplates.size(); ++ c) {
    hFSInt32 lOpsArgIndex(lInOpsArgIndex);
    vector<InstSetDef_Op*> lOutOps;
    string lFormatStr;
    if (parseOps(lTemplates[c], ops_, lOpsArgIndex, lOutOps, lFormatStr, isFirstCall_) == true) {
      PDEBUGBLOCK() {
        PDEBUG("InstructionSetDefinition", "detail-matched-template", progcxt(lTemplates[c]));
        for (hFUInt32 c(0); c < lOutOps.size(); ++ c) {
          PDEBUG("InstructionSetDefinition", "detail-matched-ops", progcxt(lOutOps[c]) << " -> " << progcxt(ops_[lInOpsArgIndex+c]));
        } // for
      } // PDEBUGBLOCK
      ASSERTMSG(retVal == 0, ieStrParamValuesDBCViolation <<
        " Ambiguous instruction set definition, multiple instruction template matches: " <<
        retVal->GetName() << " and " << lTemplates[c]->GetName());
      opsArgIndex_ = lOpsArgIndex;
      outOps_ = lOutOps;
      formatStr_ = lFormatStr;
      retVal = lTemplates[c];
      //! \todo H Design: Document this behaviour first match is accepted. It
      //! may cause unmatched instructions.
      break;
    } else {
      PDEBUG("InstructionSetDefinition", "detail-unmatched-template", progcxt(lTemplates[c]));
    } // if
  } // for
  EMDEBUG2(retVal, formatStr_);
  return retVal;
} // InstructionSetDefinition::parseOps

Instruction* InstructionSetDefinition::
parseOps(const string& opCode_, const InstSetDef_Inst* instDef_, const vector<GenInstOp*>& genOps_) const {
  BMDEBUG3("InstructionSetDefinition::parseOps(opCode_)", opCode_, instDef_);
  Instruction* retVal(0);
  vector<InstSetDef_Op*> lOps;
  hFSInt32 lArgsIndex(0);
  string lDisassembly;
  InstSetDef_Template* lMatchedTemplate(parseOps(instDef_->GetTemplateName(), genOps_, lArgsIndex, lOps, lDisassembly, consts.cRecFirstCall));
  vector<GenInstOp> lInstOperands; // Operands of the instruction to be created.
  if (lMatchedTemplate != 0) {
    if (lArgsIndex == genOps_.size()) { // All arguments are consumed.
      RegSet lDefs;
      RegSet lUses;
      map<string, Register*> lRegs;
      { // Populate the uses and defs.
        for (hFUInt32 c(0); c < lOps.size(); ++ c) {
          lInstOperands.push_back(*genOps_[c]);
          if (InstSetDef_OpReg* lRegOp = dynamic_cast<InstSetDef_OpReg*>(lOps[c])) {
            Register* lReg(0);
            if (genOps_[c]->IsReg() == true) {
              lReg = genOps_[c]->GetReg();
            } else {
              lReg = GetHWDesc()->GetHWReg(genOps_[c]->GetString());
            } // if
            if (lRegOp->IsDef() == true) {
              lDefs |= lReg;
            } else {
              lUses |= lReg;
            } // if
            lRegs[lRegOp->GetID()] = lReg;
          } else if (InstSetDef_OpRegSet* lRegSet = dynamic_cast<InstSetDef_OpRegSet*>(lOps[c])) {
            //! \todo M Design: Handle the regset reg alloc rules.
            lUses.Insert(genOps_[c]->GetRegSet());
          } // if
        } // for
        lDefs.Insert(instDef_->GetDefs());
        lUses.Insert(instDef_->GetUses());
      } // block
      { // Place the opcode in to disassembly
        static char lInstDisassemblyBuf[1024];
        sprintf(lInstDisassemblyBuf, lDisassembly.c_str(), opCode_.c_str());
        lDisassembly = lInstDisassemblyBuf;
        smRenumberFormatStr(lDisassembly, 1, -1);
      } // block
      { // Process the attached register allocation rules
        //! \todo M Design: Check if given hard regs constraints are satisfied.
        for (hFUInt32 c(0); c < instDef_->mRARules.size(); ++ c) {
          RegAllocRule* lCurrRule(instDef_->mRARules[c]->Clone());
          map<string, Register*>::iterator lIt(lRegs.begin());
          for (/* LIE */; lIt != lRegs.end(); ++ lIt) {
            lCurrRule->ReplaceVReg(lIt->first, lIt->second);
          } // for
          PDEBUG("InstructionSetDefinition", "detail-rarule-adapted", progcxt(lCurrRule));
        } // for
      } // block
      retVal = new Instruction(instDef_->GetSize(), lDisassembly, lDefs, lUses, lInstOperands, false);
      PDEBUG("InstructionSetDefinition", "detail-inst-created", progcxt(retVal));
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // InstructionSetDefinition::parseOps

bool InstructionSetDefinition::
parseDoOpsMatch(InstSetDef_Op* defOp_, GenInstOp* genOp_) const {
  BMDEBUG3("InstructionSetDefinition::parseDoOpsMatch", defOp_, genOp_);
  REQUIREDMSG(dynamic_cast<InstSetDef_OpTemplate*>(defOp_) == 0, ieStrParamValuesDBCViolation);
  bool retVal(true);
  if (InstSetDef_OpFlag* lOpFlag = dynamic_cast<InstSetDef_OpFlag*>(defOp_)) {
    if (genOp_->IsString() == false) {
      retVal = false;
    } else if (lOpFlag->GetFlag()->IsAFlag(genOp_->GetString()) == false) {
      retVal = false;
    } else if (lOpFlag->IsValueSet() == true && lOpFlag->GetValue() != genOp_->GetString()) {
      retVal = false;
    } // if
  } else if (InstSetDef_OpInt* lOpInt = dynamic_cast<InstSetDef_OpInt*>(defOp_)) {
    if (genOp_->IsInt() == false) {
      retVal = false;
    } else if (lOpInt->IsValueSet() == true && lOpInt->GetValue() != genOp_->GetInt()) {
      retVal = false;
    } // if
  } else if (InstSetDef_OpLabel* lOpLabel = dynamic_cast<InstSetDef_OpLabel*>(defOp_)) {
    if (genOp_->IsString() == false) {
      retVal = false;
    } // if
  } else if (InstSetDef_OpRegSet* lOpRegSet = dynamic_cast<InstSetDef_OpRegSet*>(defOp_)) {
    if (genOp_->IsRegSet() == false) {
      retVal = false;
    } // if
  } else if (InstSetDef_OpReg* lOpReg = dynamic_cast<InstSetDef_OpReg*>(defOp_)) {
    if (genOp_->IsReg() == true) {
      if (lOpReg->GetReg() != 0 && genOp_->GetReg() != lOpReg->GetReg()) {
        retVal = false;
      } // if
    } else if (genOp_->IsString() == false) {
      retVal = false;
    } else if (lOpReg->GetRegsType()->IsAReg(genOp_->GetString()) == false) {
      retVal = false;
    } // if
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // InstructionSetDefinition::parseDoOpsMatch

string GenInstOp::
GetDisassembly(const ASMDesc& asmDesc_) const {
  string retVal("");
  if (IsString() == true) {
    retVal = GetString();
  } else if (IsInt() == true) {
    ostrstream o;
    o << GetInt() << ends;
    retVal = o.str();
  } else if (IsShift() == true) {
    ostrstream o;
    asmDesc_.PutInstShiftCode(o, GetShift());
    o << ends;
    retVal = o.str();
  } else if (IsCondition() == true) {
    ostrstream o;
    asmDesc_.PutInstCondCode(o, GetCondition());
    o << ends;
    retVal = o.str();
  } else if (IsRegSet() == true) {
    ostrstream o;
    asmDesc_.PutInstRegSet(o, GetRegSet());
    o << ends;
    retVal = o.str();
  } else if (IsReg() == true) {
    if (pred.pIsHWReg(GetReg()) == true) {
      retVal = GetReg()->GetName();
    } else {
      VirtReg* lVReg(static_cast<VirtReg*>(GetReg()));
      if (pred.pIsHWRegAssigned(lVReg) == true) {
        retVal = lVReg->GetHWReg()->GetName();
      } else {
        retVal = lVReg->GetName();
      } // if
    } // if
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  return retVal;
} // GenInstOp::GetDisassembly

Instruction* InstructionSetDefinition::
CreateInst(const string& opCode_, hFSInt32 numOfOps_, ...) {
  BMDEBUG3("InstructionSetDefinition::CreateInst", opCode_, numOfOps_);
  REQUIREDMSG(numOfOps_ >= 0 && numOfOps_ < 128, ieStrParamValuesDBCViolation);
  vector<GenInstOp*> lGenOps;
  { // Fill generic parameters in to a vector.
    va_list lAP;
    va_start(lAP, numOfOps_);
    while (numOfOps_ --) {
      GenInstOp* lOp(va_arg(lAP, GenInstOp*));
      if (lOp->IsString() == true) {
        if (GetHWDesc()->IsAHWReg(lOp->GetString()) == true) {
          lOp->mValue.mReg = GetHWDesc()->GetHWReg(lOp->GetString());
          lOp->mKind = GenInstOp::GIO_Register;
        } // if
      } else if (lOp->IsShift() == true) {
        lOp->mValue.mString = new string(mShiftFlags[lOp->GetShift()]);
        lOp->mKind = GenInstOp::GIO_String;
      } else if (lOp->IsCondition() == true) {
        lOp->mValue.mString = new string(mCondFlags[lOp->GetCondition()]);
        lOp->mKind = GenInstOp::GIO_String;
      } // if
      lGenOps.push_back(lOp);
    } // while
    va_end(lAP);
  } // block
  Instruction* retVal(0);
  vector<InstSetDef_Inst*> lInstDefs;
  GetInstDefs(opCode_, lInstDefs);
  for (hFUInt32 c = 0; c < lInstDefs.size(); ++ c) {
    Instruction* lInst(parseOps(opCode_, lInstDefs[c], lGenOps));
    if (lInst != 0) {
      PDEBUG("InstructionSetDefinition", "detail", "Instruction matched: " << progcxt(lInst));
      ASSERTMSG(retVal == 0, ieStrParamValuesDBCViolation << " multiple instructions match for given arguments.");
      retVal = lInst;
      if (RELEASEBUILD) {
        break;
      } // if
    } else {
      PDEBUG("InstructionSetDefinition", "detail", "Instruction template not-matched: " << progcxt(lInstDefs[c]));
    } // if
  } // for
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull << " Could not match any instruction: " << opCode_);
  EMDEBUG3(retVal, opCode_, retVal->GetFormatStr());
  return retVal;
} // InstructionSetDefinition::CreateInst

void RARMustNotBeSame::
ReplaceVReg(const string& name_, Register* regNew_) {
  BMDEBUG3("RARMustNotBeSame::ReplaceVReg", name_, regNew_);
  const set<Register*>& lRegs(mRegs.GetRegs());
  set<Register*>::iterator lIt(lRegs.begin());
  while (lIt != lRegs.end()) {
    if (pred.pIsVReg(*lIt) == true) {
      VirtReg* lVReg(static_cast<VirtReg*>(*lIt));
      if (lVReg->GetName() == name_) {
        mRegs -= lVReg;
        mRegs |= regNew_;
        //! \todo M Design: Uniformly handle the reg alloc rule additions in vregs.
        if (VirtReg* lVRegNew = dynamic_cast<VirtReg*>(regNew_)) {
          AddRegAllocRule(lVRegNew);
        } // if
      } // if
    } // if
    ++ lIt;
  } // while
  EMDEBUG0();
  return;
} // RARMustNotBeSame::ReplaceVReg

void RARMustBeInList::
ReplaceVReg(const string& name_, Register* regNew_) {
  BMDEBUG3("RARMustBeInList::ReplaceVReg", name_, regNew_);
  if (pred.pIsVReg(regNew_) == true) {
    for (hFInt32 c(0); c < mVRegs.size(); ++ c) {
      if (mVRegs[c]->GetName() == name_) {
        mVRegs[c] = static_cast<VirtReg*>(regNew_);
        AddRegAllocRule(static_cast<VirtReg*>(regNew_));
      } // if
    } // for
  } else {
    //! \todo M Design: Must check the HWreg against the rule.
  } // if
  EMDEBUG0();
  return;
} // RARMustNotBeSame::ReplaceVReg

hFSInt32 InstructionSetDefinition::
smMaxPosInFormatStr(const string& formatStr_) {
  BDEBUG2("InstructionSetDefinition::smMaxPosInFormatStr", formatStr_);
  hFSInt32 retVal(-1);
  for (hFSInt32 c = 9; c >= 0; -- c) {
    string::size_type lPos(formatStr_.find(string("@") + char(int('0') + c)));
    if (lPos != string::npos) {
      retVal = c;
      break;
    } // if
  } // for
  EDEBUG1(retVal);
  return retVal;
} // InstructionSetDefinition::smMaxPosInFormatStr

void InstructionSetDefinition::
smRenumberFormatStr(string& formatStr_, hFSInt32 startPos_, hFSInt32 offset_) {
  BDEBUG4("InstructionSetDefinition::smRenumberFormatStr", formatStr_, startPos_, offset_);
  REQUIREDMSG(startPos_ >= 0 && startPos_ <= 9, ieStrParamValuesDBCViolation);
  if (offset_ > 0) {
    for (hFSInt32 c(8); c >= startPos_; -- c) {
      if (smReplaceFormatStr(formatStr_, c, string("@") + char(int('0') + c + offset_)) == true) {
        ASSERTMSG(util.uInRange(c + offset_, 0, 9) == true, ieStrNotImplemented << 
          " position specifiers not in 0-9 are not yet implemented." << c);
      } // if
    } // for
  } else if (offset_ < 0) {
    for (hFSInt32 c(startPos_); c <= 9; ++ c) {
      if (smReplaceFormatStr(formatStr_, c, string("@") + char(int('0') + c + offset_)) == true) {
        ASSERTMSG(util.uInRange(c + offset_, 0, 9) == true, ieStrNotImplemented << 
          " position specifiers not in 0-9 are not yet implemented." << c);
      } // if
    } // for
  } // if
  EDEBUG1(formatStr_);
  return;
} // InstructionSetDefinition::smRenumberFormatStr

bool InstructionSetDefinition::
smReplaceFormatStr(string& formatStr_, hFSInt32 pos_, const string& str_) {
  BDEBUG4("InstructionSetDefinition::smReplaceFormatStr", formatStr_, pos_, str_);
  bool retVal(false);
  string::size_type lPos(formatStr_.find(string("@") + char(int('0') + pos_)));
  if (lPos != string::npos) {
    formatStr_.replace(lPos, 2, str_.c_str());
    retVal = true;
  } // if
  EDEBUG2(retVal, formatStr_);
  return retVal;
} // InstructionSetDefinition::smReplaceFormatStr

InstSetDef_Template* InstSetDef_Template::
FormatStr(const char* formatStr_) {
  REQUIREDMSG(mFormatStr == 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mIsOR == false, ieStrParamValuesDBCViolation);
  REQUIREDMSG(InstructionSetDefinition::smMaxPosInFormatStr(formatStr_) <= mOperands.size(), ieStrParamValuesDBCViolation);
  mFormatStr = formatStr_;
  return this;
} // InstSetDef_Template::FormatStr



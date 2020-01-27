// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006.

#ifndef __ARM_HWDEF_HPP__
#include "arm_hwdef.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __OPTIONS_HPP__
#include "options.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif
#ifndef __DEBUGINFO_DWARF_HPP__
#include "debuginfo_dwarf.hpp"
#endif

//! \todo M Design: Provide a mechanism to specify IRExpr nodes that is not
//!       available in the target machine as a separate instruction. This
//!       means they should be lowered to function calls before Match/Cover.

HWReg* ARMHW::csRegR0;
HWReg* ARMHW::csRegR1;
HWReg* ARMHW::csRegR2;
HWReg* ARMHW::csRegR3;
HWReg* ARMHW::csRegR4;
HWReg* ARMHW::csRegR5;
HWReg* ARMHW::csRegR6;
HWReg* ARMHW::csRegR7;
HWReg* ARMHW::csRegR8;
HWReg* ARMHW::csRegR9;
HWReg* ARMHW::csRegR10;
HWReg* ARMHW::csRegFP; // r11
HWReg* ARMHW::csRegR12;
HWReg* ARMHW::csRegSP; // r13
HWReg* ARMHW::csRegLR; // r14
HWReg* ARMHW::csRegPC; // r15
RegSet* ARMHW::csRegSetCalleeSave;
RegSet* ARMHW::csRegAllocSet;
HWReg* ARMHW::csRegArray[16];
RegSet* ARMHW::csRegParamSet;
RegSet* ARMHW::csRegsR0R1;

static bool
isDouble(IRType* type_) {
  bool retVal(pred.pIsReal(type_) == true && type_->GetSize() == 64);
  return retVal;
} // isDouble

//! \brief Virtual register that represents the place of a double precision
//!        real type for ARM architecture.
class VirtRegDouble : public VirtReg {
protected:
  VirtRegDouble(const Invalid<VirtRegDouble>* inv_) : VirtReg(&Invalid<VirtReg>::Obj()) {}
public:
  VirtRegDouble(HWReg* regEven_, HWReg* regOdd_) :
    mRegEven(new VirtReg(regEven_)),
    mRegOdd(new VirtReg(regOdd_))
  {
    BMDEBUG3("VirtRegDouble::VirtRegDouble", regEven_, regOdd_);
    if (smValidPairs.empty() == true) {
      for (hFInt32 c(0); c < 11; c += 2) {
        smValidPairs.push_back(getPairVector(ARMHW::csRegArray[c], ARMHW::csRegArray[c+1]));
      } // for
    } // if
    vector<VirtReg*> lRegPair;
    lRegPair.push_back(mRegEven);
    lRegPair.push_back(mRegOdd);
    /* LIE */ new RARMustBeInList(lRegPair, &smValidPairs);
    EMDEBUG0();
  } // VirtRegDouble::VirtRegDouble
  VirtRegDouble(const VirtRegDouble& vRegDouble_) :
    VirtReg(vRegDouble_),
    mRegEven(vRegDouble_.mRegEven),
    mRegOdd(vRegDouble_.mRegOdd)
  {
    BMDEBUG2("VirtRegDouble::VirtRegDouble", &vRegDouble_);
    EMDEBUG0();
  } // VirtRegDouble::VirtRegDouble
  VirtRegDouble() :
    mRegEven(new VirtReg),
    mRegOdd(new VirtReg)
  {
    BMDEBUG1("VirtRegDouble::VirtRegDouble");
    if (smValidPairs.empty() == true) {
      for (hFInt32 c(0); c < 11; c += 2) {
        smValidPairs.push_back(getPairVector(ARMHW::csRegArray[c], ARMHW::csRegArray[c+1]));
      } // for
    } // if
    vector<VirtReg*> lRegPair;
    lRegPair.push_back(mRegEven);
    lRegPair.push_back(mRegOdd);
    /* LIE */ new RARMustBeInList(lRegPair, &smValidPairs);
    EMDEBUG0();
  } // VirtRegDouble::VirtRegDouble
  virtual ~VirtRegDouble() {}
  VirtReg* GetRegEven() const {return mRegEven;}
  VirtReg* GetRegOdd() const {return mRegOdd;}
  virtual VirtReg* Clone() const {
    BMDEBUG1("VirtRegDouble::Clone");
    VirtRegDouble* retVal(new VirtRegDouble(*this));
    retVal->mRegEven = mRegEven->Clone();
    retVal->mRegOdd = mRegOdd->Clone();
    PDEBUG("VirtRegDouble", "detail", progcxt(this) << endl << " cloned as: " << progcxt(retVal));
    EMDEBUG1(retVal);
    return retVal;
  } // VirtRegDouble::Clone
  virtual char* debugTrace(DTContext& context_, ostream& toStr_) const {
    DTLOG("VirtRegDouble::debugTrace(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
    if (context_ == DTCCDumpRef) {
      Singleton<DebugTrace>::Obj().dumpObjRef(this, context_, toStr_);
    } else if (context_ == DTCCDumpProg || context_ == DTCCDumpHWDesc) {
      toStr_ << "VirtRegDouble{" << refcxt(this) << GetName().c_str() << ", " << GetId();
      toStr_ << ++ DTCCIndent << DTCCEndLine;
      toStr_ << DTCCIndent << "RegEven:" << progcxt(mRegEven) << DTCCEndLine;
      toStr_ << DTCCIndent << "RegOdd:" << progcxt(mRegOdd) << DTCCEndLine;
      toStr_ << -- DTCCIndent << DTCCIndent << "}";
    } else {
      Singleton<DebugTrace>::Obj().dumpObjRef(this, context_, toStr_);
    } // if
    DTLOG(dtlUnindent << dtlEndl << "} // VirtRegDouble::debugTrace" << dtlEndl);
    return "";
  } // VirtRegDouble::debugTrace
private:
  vector<HWReg*> getPairVector(HWReg* reg1_, HWReg* reg2_) {
    vector<HWReg*> retVal;
    retVal.push_back(reg1_);
    retVal.push_back(reg2_);
    return retVal;
  } // VirtRegDouble::getPairVector
  VirtReg* mRegEven;
  VirtReg* mRegOdd;
  static vector<vector<HWReg*> > smValidPairs;
}; // class VirtRegDouble

vector<vector<HWReg*> > VirtRegDouble::smValidPairs;

namespace ARMInstructions {

namespace ARMInstCGHelpers {

AsmSeqData* AddInstsArmDataWord(CGFuncContext& context_, const string& symbol_);
AsmSeqData* AddInstsArmAddrDataWord(CGFuncContext& context_, const IROGlobal& obj_);
AsmSeqData* AddInstsArmDataString(CGFuncContext& context_, const string& string_);

} // namespace ARMInstCGHelpers

using namespace ARMInstCGHelpers;

// --------------------------
// Member function definitions outside class bodies
// --------------------------

namespace ARMInstCGHelpers {

AsmSeqData*
AddInstsArmDataString(CGFuncContext& context_, const string& string_) {
  //    B .Lend
  // .LC0:
  //    .ascii string_
  // .Lxx:
  //    .word .LC0
  // .Lend
  IRBuilder& irBuilder(Singleton<IRBuilder>::Obj());
  static IRType* lWordType;
  if (!lWordType) {
    lWordType = irBuilder.irbtGetInt(INBITS(32), 0, IRSUnsigned);
  } // if
  IRRelocExpr* lStrConstReloc(irBuilder.irbeStrConst(string_));
  AsmSeqData* lStrConstData(new AsmSeqData(context_.GenAsmSeqLabel(), lStrConstReloc));
  AsmSeqLabel* lEndLabel(context_.GenAsmSeqLabel());
  IRRelocExpr* lStrConstAddr(irBuilder.irboRelocSymbol(*lStrConstData->GetLabel(), lWordType));
  AsmSeqData* retVal(new AsmSeqData(context_.GenAsmSeqLabel(), lStrConstAddr));
  context_.AddInst(Rule::CreateInst(context_, "b", lEndLabel->GetLabel()));
  context_.AddInst(new AsmSeqDirInfo(string("\t.ltorg")));
  context_.AddInst(lStrConstData);
  context_.AddInst(new AsmSeqDirAlign(4));
  context_.AddInst(retVal);
  context_.AddInst(lEndLabel);
  return retVal;
} // AddInstsArmDataString

AsmSeqData*
AddInstsArmAddrDataWord(CGFuncContext& context_, const IROGlobal& obj_) {
  BDEBUG1("AddInstsArmAddrDataWord");
  IRBuilder& irBuilder(Singleton<IRBuilder>::Obj());
  static IRType* lWordType;
  if (!lWordType) {
    lWordType = irBuilder.irbtGetInt(INBITS(32), 0, IRSUnsigned);
  } // if
  string lSymbol;
  if (!pred.pHasLTConstAddr(&obj_)) {
    lSymbol = obj_.GetName();
  } else {
    AddrLTConst* lAddr(static_cast<AddrLTConst*>(obj_.GetAddress()));
    ASSERTMSG(lAddr->GetOffset() == 0, ieStrNotImplemented);
    lSymbol = lAddr->GetLTConst();
  } // if
  IRRelocExpr* lReloc(irBuilder.irboRelocSymbol(lSymbol, lWordType));
  AsmSeqData* retVal(new AsmSeqData(context_.GenAsmSeqLabel(), lReloc));
  AsmSeqLabel* lEndLabel(context_.GenAsmSeqLabel());
  context_.AddInst(Rule::CreateInst(context_, "b", lEndLabel->GetLabel()));
  context_.AddInst(new AsmSeqDirInfo(string("\t.ltorg")));
  context_.AddInst(retVal);
  context_.AddInst(lEndLabel);
  EDEBUG0();
  return retVal;
} // AddInstsArmAddrDataWord

AsmSeqData*
AddInstsArmDataWord(CGFuncContext& context_, const string& symbol_) {
  BDEBUG3("AddInstsArmDataWord", &context_, symbol_.c_str());
  IRBuilder& irBuilder(Singleton<IRBuilder>::Obj());
  static IRType* lWordType;
  if (!lWordType) {
    lWordType = irBuilder.irbtGetInt(INBITS(32), 0, IRSUnsigned);
  } // if
  IRRelocExpr* lReloc(irBuilder.irboRelocSymbol(symbol_, lWordType));
  AsmSeqData* retVal(new AsmSeqData(context_.GenAsmSeqLabel(), lReloc));
  AsmSeqLabel* lEndLabel(context_.GenAsmSeqLabel());
  context_.AddInst(Rule::CreateInst(context_, "b", lEndLabel->GetLabel()));
  context_.AddInst(new AsmSeqDirInfo(string("\t.ltorg")));
  context_.AddInst(retVal);
  context_.AddInst(lEndLabel);
  EDEBUG1(retVal);
  return retVal;
} // AddInstsArmDataWord

}; // namespace ARMInstCGHelpers

}; // namespace ARMInstructions
using namespace ARMInstructions;

namespace ARMRules {

// ------------------------------
// Statement Rules
// ------------------------------

class SRIf : public StmtRule {
public:
  SRIf(const SRIf& rule_) : StmtRule(rule_) {}
  SRIf(HWDescription* hwDesc_) :
    StmtRule("if", hwDesc_)
  {
    SetStmtPattern(irsIf(condNT(0)));
  } // SRIf::SRIf
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRIf::OnCover", stmt_);
    IRSIf* lIfStmt(static_cast<IRSIf*>(stmt_));
    ConditionNT* lCondNT(GetCondNT(0));
    // InstB(const InstOpARMFlags& flags, InstOpLabel* label)
    IRBB* lThenBB(lIfStmt->GetTrueCaseBB());
    IRBB* lElseBB(lIfStmt->GetFalseCaseBB());
    if (lIfStmt->GetBB()->GetNext() == lThenBB) {
      // If next physical block is then target we just need to handle else
      // condition.
      AddInst(context_, "b", lCondNT->GetInverted(), context_.GetLabelOf(lElseBB)->GetLabel());
    } else if (lIfStmt->GetBB()->GetNext() == lElseBB) {
      AddInst(context_, "b", *lCondNT, context_.GetLabelOf(lThenBB)->GetLabel());
    } else {
      AddInst(context_, "b", *lCondNT, context_.GetLabelOf(lThenBB)->GetLabel());
      AddInst(context_, "b", lCondNT->GetInverted(), context_.GetLabelOf(lElseBB)->GetLabel());
    } // if
    EMDEBUG0();
    return;
  } // SRIf::OnCover
  virtual StmtRule* Clone() const {return new SRIf(*this);}
}; // class SRIf

//! \todo M Design: Generalize the call pattern, for procedures and functions.
class SRPCallIndirect : public StmtRule {
public:
  SRPCallIndirect(const SRPCallIndirect& rule_) : StmtRule(rule_) {}
  SRPCallIndirect(HWDescription* hwDesc_) :
    StmtRule("pcallind", hwDesc_)
  {
    //! \todo H Design: Is it okay to implement ignore non-terminal?
    SetStmtPattern(irsPCall(regNT(0)));
  } // SRPCallIndirect::SRPCallIndirect
  virtual hReal32 GetCost() {return 2.0;}
  virtual bool CheckCondition(const IRStmt* stmt_) {
    BMDEBUG2("SRPCallIndirect::CheckCondition", stmt_);
    const IRSFCall* lPCall(static_cast<const IRSFCall*>(stmt_));
    bool retVal(pred.pIsIndirect(lPCall));
    EMDEBUG1(retVal);
    return retVal;
  } // SRPCallIndirect::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRPCallIndirect::OnCover", stmt_);
    IRSFCall* lFCall(static_cast<IRSFCall*>(stmt_));
    { // calculate the uses of this call.
      RegSet lUses;
      hFInt32 lNumArgs(extr.eGetNumOfArgs(lFCall));
      for (hFInt32 c(0); c < lNumArgs; ++ c) {
        lUses |= ARMHW::csRegArray[c];
      } // for
      AddInst(context_, "blx", *GetRegNT(0))->AddUses(lUses);
    } // block
    EMDEBUG0();
    return;
  } // SRPCallIndirect::OnCover
  virtual StmtRule* Clone() const {return new SRPCallIndirect(*this);}
}; // class SRPCallIndirect

class SRFCallIndirect : public StmtRule {
public:
  SRFCallIndirect(const SRFCallIndirect& rule_) : StmtRule(rule_) {}
  SRFCallIndirect(HWDescription* hwDesc_) :
    StmtRule("fcallind", hwDesc_)
  {
    //! \todo H Design: Is it okay to implement ignore non-terminal?
    SetStmtPattern(irsFCall(regNT(0)));
  } // SRFCallIndirect::SRFCallIndirect
  virtual hReal32 GetCost() {return 2.0;}
  virtual bool CheckCondition(const IRStmt* stmt_) {
    BMDEBUG2("SRFCallIndirect::CheckCondition", stmt_);
    const IRSFCall* lFCall(static_cast<const IRSFCall*>(stmt_));
    bool retVal(pred.pIsIndirect(lFCall));
    EMDEBUG1(retVal);
    return retVal;
  } // SRFCallIndirect::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRFCallIndirect::OnCover", stmt_);
    IRSFCall* lFCall(static_cast<IRSFCall*>(stmt_));
    { // calculate the uses of this call.
      RegSet lUses;
      hFInt32 lNumArgs(extr.eGetNumOfArgs(lFCall));
      for (hFInt32 c(0); c < lNumArgs; ++ c) {
        lUses |= ARMHW::csRegArray[c];
      } // for
      lUses |= *GetRegNT(0);
      AddInst(context_, "blx", *GetRegNT(0))->AddUses(lUses);
    } // block
    EMDEBUG0();
    return;
  } // SRFCallIndirect::OnCover
  virtual StmtRule* Clone() const {return new SRFCallIndirect(*this);}
}; // class SRFCallIndirect

class SRFCall : public StmtRule {
public:
  SRFCall(const SRFCall& rule_) : StmtRule(rule_) {}
  SRFCall(HWDescription* hwDesc_) :
    StmtRule("fcall", hwDesc_)
  {
    SetStmtPattern(irsFCall(ireAddrOf(iroObject())));
  } // SRFCall::SRFCall
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRFCall::OnCover", stmt_);
    IRSFCall* lFCall(static_cast<IRSFCall*>(stmt_));
    IRObject* lFuncObj(extr.eGetObjOfAddrOf(lFCall->GetFuncExpr(), scDBCEnsureNonNull));
    { // calculate the uses of this call.
      RegSet lUses;
      hFInt32 lNumArgs(extr.eGetNumOfArgs(lFCall));
      for (hFInt32 c(0); c < lNumArgs; ++ c) {
        lUses |= ARMHW::csRegArray[c];
      } // for
      AddInst(context_, "bl", lFuncObj->GetName())->AddUses(lUses);
    } // block
    EMDEBUG0();
    return;
  } // SRFCall::OnCover
  virtual StmtRule* Clone() const {return new SRFCall(*this);}
}; // class SRFCall

class SRBuiltInCall : public StmtRule {
public:
  SRBuiltInCall(const SRBuiltInCall& rule_) : StmtRule(rule_) {}
  SRBuiltInCall(HWDescription* hwDesc_) :
    StmtRule("bicall", hwDesc_)
  {
    SetStmtPattern(irsBICall(ignoreNT(0)));
  } // SRBuiltInCall::SRBuiltInCall
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRBuiltInCall::OnCover", stmt_);
    IRSBuiltInCall* lFCall(static_cast<IRSBuiltInCall*>(stmt_));
    ASSERTMSG(lFCall->GetBIRId() == IRBIRmemcpy, ieStrNotImplemented);
    IRObject* lFuncObj(extr.eGetObjOfAddrOf(lFCall->GetFuncExpr(), scDBCEnsureNonNull));
    { // calculate the uses of this call.
      RegSet lUses;
      hFInt32 lNumArgs(extr.eGetNumOfArgs(lFCall));
      for (hFInt32 c(0); c < lNumArgs; ++ c) {
        lUses |= ARMHW::csRegArray[c];
      } // for
      AddInst(context_, "bl", lFuncObj->GetName())->AddUses(lUses);
    } // block
    EMDEBUG0();
    return;
  } // SRBuiltInCall::OnCover
  virtual StmtRule* Clone() const {return new SRBuiltInCall(*this);}
}; // class SRBuiltInCall

class SRPCall : public StmtRule {
public:
  SRPCall(const SRPCall& rule_) : StmtRule(rule_) {}
  SRPCall(HWDescription* hwDesc_) :
    StmtRule("pcall", hwDesc_)
  {
    SetStmtPattern(irsPCall(ireAddrOf(iroObject())));
  } // SRPCall::SRPCall
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRPCall::OnCover", stmt_);
    IRSPCall* lPCall(static_cast<IRSPCall*>(stmt_));
    IRObject* lFuncObj(extr.eGetObjOfAddrOf(lPCall->GetFuncExpr(), scDBCEnsureNonNull));
    { // calculate the uses of this call.
      RegSet lUses;
      hFInt32 lNumArgs(extr.eGetNumOfArgs(lPCall));
      for (hFInt32 c(0); c < lNumArgs; ++ c) {
        lUses |= ARMHW::csRegArray[c];
      } // for
      AddInst(context_, "bl", lFuncObj->GetName())->AddUses(lUses);
    } // block
    EMDEBUG0();
    return;
  } // SRPCall::OnCover
  virtual StmtRule* Clone() const {return new SRPCall(*this);}
}; // class SRPCall

class SREpilog : public StmtPrologEpilogRule {
public:
  SREpilog(const SREpilog& rule_) : StmtPrologEpilogRule(rule_) {}
  SREpilog(HWDescription* hwDesc_) :
    StmtPrologEpilogRule("epilog", hwDesc_)
  {
    SetStmtPattern(irsEpilog());
  } // SREpilog::SREpilog
  virtual hReal32 GetCost() {return 2.0;}
  //! \attention OnCover for Epilog statements are called after register allocation.
  //! \sa SRProlog
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_, const CallingConvention* callConv_) {
    BMDEBUG2("SREpilog::OnCover", stmt_);
    // For details have a look at SRProlog. We will clean up the frame.
    // First get back the stack pointer
    IRFunction* lFunc(context_.GetFunc());
    tBigInt lStackSize(lFunc->GetStackSize());
    string lLabel(context_.GetALocalLabel("epilog_beg"));
    if (options.obGet("debuginfo")) {
      if (options.oeGet("debugformat") == "Dwarf2") {
        IRBuilder::smirbAttachLabel(lFunc, "epilog_beg", lLabel);
        AddInst(context_, "label", lLabel);
      } // if
    } // if
    if (ARMHW::IsImm8r(lStackSize) == true) {
      AddInst(context_, "add", "sp", "sp", lStackSize)->IgnoreDefsAndUses();
    } else {
      VirtReg* lTmpReg(new VirtReg());
      AddInst(context_, "ldr", lTmpReg, lStackSize);
      AddInst(context_, "add", "sp", "sp", lTmpReg);
    } // if
    // Pop back the saved registers from the stack. Note that
    // we loaded LR back to PC so we are back to the caller.
    AddInst(context_, "ldm", "fd", "!", "sp", 
      *ARMHW::csRegSetCalleeSave | *ARMHW::csRegPC)->IgnoreDefsAndUses();
    { // Generate the debug information for epilog.
      if (options.obGet("debuginfo")) {
        if (options.oeGet("debugformat") == "Dwarf2") {
          diAddCallFrameInst(lFunc, new DW_CFA_set_loc(lLabel));
          diAddCallFrameInst(lFunc, new DW_CFA_restore_state());
        } else {
          ASSERTMSG(0, ieStrNotImplemented);
        } // if
      } // if
    } // block
    EMDEBUG0();
    return;
  } // SREpilog::OnCover
  virtual StmtRule* Clone() const {return new SREpilog(*this);}
}; // class SREpilog

class SREval : public StmtRule {
public:
  SREval(const SREval& rule_) : StmtRule(rule_) {}
  SREval(HWDescription* hwDesc_) :
    StmtRule("eval", hwDesc_)
  {
    SetStmtPattern(irsEval(ignoreNT(0)));
  } // SREval::SREval
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SREval::OnCover", stmt_);
    // Eval statement does nothing.
    /* LIE */
    EMDEBUG0();
    return;
  } // SREval::OnCover
  virtual StmtRule* Clone() const {return new SREval(*this);}
}; // class SREval

class SRNull : public StmtRule {
public:
  SRNull(const SRNull& rule_) : StmtRule(rule_) {}
  SRNull(HWDescription* hwDesc_) :
    StmtRule("null_stmt", hwDesc_)
  {
    SetStmtPattern(irsNull());
  } // SRNull::SRNull
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRNull::OnCover", stmt_);
    // Null statement does nothing.
    /* LIE */
    EMDEBUG0();
    return;
  } // SRNull::OnCover
  virtual StmtRule* Clone() const {return new SRNull(*this);}
}; // class SRNull

class SRProlog : public StmtPrologEpilogRule {
public:
  SRProlog(const SRProlog& rule_) : StmtPrologEpilogRule(rule_) {}
  SRProlog(HWDescription* hwDesc_) :
    StmtPrologEpilogRule("prolog", hwDesc_)
  {
    SetStmtPattern(irsProlog());
  } // SRProlog::SRProlog
  virtual hReal32 GetCost() {return 2.0;}
  //! \attention OnCover for Prolog statements are called after register allocation.
  //! \sa IRSProlog
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_, const CallingConvention* callConv_) {
    BMDEBUG2("SRProlog::OnCover", stmt_);
    // We need to do the followings:
    //   - store callee save registers,
    //   - store link register
    //   - mark stack with frame pointer
    //     v) The reason we mark frame pointer at this step is that we want to
    //     retrieve back the LR, and calee save registers in the epilog.
    //     For that, now we need to copy fp back to sp and load the saved
    //     regs. We could have forced in some hw archs to do it the other way.
    //     Such HW lacks operations on fp, e.g. "fp-off" is not present, but
    //     "fp+off" is present (locals are accessed by fp+/-off).
    //   - allocate stack space. Stack space needs to be calculated
    //     by checking the locals and temporaries used in the function.
    //     ^) If we would mark frame pointer here we need to do extra
    //     calculation on sp for winding back the stack.
    //-------
    //     Since we have marked the stack with fp, we do not need to worry
    //     about alloca.
    //! \todo M Design: In an optimizing code generator we need to check if
    //!       if LR needs to be saved or not, also callee-saves may not be
    //!       needed to be saved.
    IRFunction* lFunc(context_.GetFunc());
    { // Add definitions for params in regs.
      const vector<IROParameter*>& lParams(context_.GetFunc()->GetDeclaration()->GetParams());
      RegSet lParamRegs;
      for (hFUInt32 c(0); c < lParams.size(); ++ c) {
        lParamRegs |= ARMHW::csRegArray[c];
      } // for
      AddInst(context_, new InstFuncEnter(lParamRegs));
    } // block
    AddInst(context_, "stm", "fd", "!", "sp",
      *ARMHW::csRegSetCalleeSave | *ARMHW::csRegLR)->IgnoreDefsAndUses();
    tBigInt lStackSize(lFunc->GetStackSize());
    if (ARMHW::IsImm8r(lStackSize) == true) {
      AddInst(context_, "sub", "sp", "sp", lStackSize)->IgnoreDefsAndUses();
    } else {
      VirtReg* lTmpReg(new VirtReg());
      AddInst(context_, "ldr", lTmpReg, lStackSize);
      AddInst(context_, "sub", "sp", "sp", lTmpReg);
    } // if
    AddInst(context_, "mov", "fp", "sp")->IgnoreDefsAndUses();
    { // Generate the debug information for prolog.
      if (options.obGet("debuginfo")) {
        if (options.oeGet("debugformat") == "Dwarf2") {
          string lLabel(context_.GetALocalLabel("prolog_end"));
          IRBuilder::smirbAttachLabel(lFunc, "prolog_end", lLabel);
          AddInst(context_, "label", lLabel);
          diAddCallFrameInst(lFunc, new DW_CFA_remember_state());
          diAddCallFrameInst(lFunc, new DW_CFA_def_cfa(ULEB128(DW_REGNUM_FP), ULEB128(lStackSize)));
        } else {
          ASSERTMSG(0, ieStrNotImplemented);
        } // if
      } // if
    } // block
    EMDEBUG0();
    return;
  } // SRProlog::OnCover
  virtual StmtRule* Clone() const {return new SRProlog(*this);}
}; // class SRProlog

//! \todo M Design: Do we really need label statements in cg? Aren't they
//!       lowered by chainging the references to bbs?
//! \todo H Design: Since labels are not present it is sufficient that
//!       InstB handles only BB arguments.
class SRLabel : public StmtRule {
public:
  SRLabel(const SRLabel& rule_) : StmtRule(rule_) {}
  SRLabel(HWDescription* hwDesc_) :
    StmtRule("label", hwDesc_)
  {
    SetStmtPattern(irsLabel());
  } // SRLabel::SRLabel
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRLabel::OnCover", stmt_);
    IRSLabel* lLabelStmt(static_cast<IRSLabel*>(stmt_));
    AddInst(context_, "label", lLabelStmt->GetName());
    EMDEBUG0();
    return;
  } // SRLabel::OnCover
  virtual StmtRule* Clone() const {return new SRLabel(*this);}
}; // class SRLabel

class SRJump : public StmtRule {
public:
  SRJump(const SRJump& rule_) : StmtRule(rule_) {}
  SRJump(HWDescription* hwDesc_) :
    StmtRule("goto", hwDesc_)
  {
    SetStmtPattern(irsJump());
  } // SRJump::SRJump
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRJump::OnCover", stmt_);
    IRSJump* lGotoStmt(static_cast<IRSJump*>(stmt_));
    IRBB* lGotoBB(lGotoStmt->GetBB());
    IRBB* lTargetBB(lGotoStmt->GetTargetBB());
    if (lGotoBB->GetNext() != lTargetBB) {
      AddInst(context_, "b", context_.GetLabelOf(lTargetBB)->GetLabel());
      AddInst(context_, new AsmSeqDirInfo(string("\t.ltorg")));
    } // if
    EMDEBUG0();
    return;
  } // SRJump::OnCover
  virtual StmtRule* Clone() const {return new SRJump(*this);}
}; // class SRJump

class SRReturnVoid : public StmtRule {
public:
  SRReturnVoid(const SRReturnVoid& rule_) : StmtRule(rule_) {}
  SRReturnVoid(HWDescription* hwDesc_) :
    StmtRule("returnVoid", hwDesc_)
  {
    SetStmtPattern(irsReturn(nullNT(0)));
  } // SRReturnVoid::SRReturnVoid
  virtual hReal32 GetCost() {return 2.0;}
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRReturnVoid::CheckCondition", matchedStmt_);
    const IRSReturn* lStmt(static_cast<const IRSReturn*>(matchedStmt_));
    bool retVal(pred.pIsNullExpr(lStmt->GetExpr()) == true);
    EMDEBUG1(retVal);
    return retVal;
  } // SRReturnVoid::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRReturnVoid::OnCover", stmt_);
    // Basically do nothing but jump to the exit block where there is an epilog
    // that does the return from function.
    IRBB* lExitBB(extr.eGetFunc(stmt_)->GetCFG()->GetExitBB());
    IRBB* lCurrBB(stmt_->GetBB());
    if (lCurrBB->GetNext() != lExitBB) {
      AddInst(context_, "b", context_.GetLabelOf(lExitBB)->GetLabel());
      AddInst(context_, new AsmSeqDirInfo(string("\t.ltorg")));
    } // if
    //! \todo M Opt: We can also return from function by generating the epilog here.
    EMDEBUG0();
    return;
  } // SRReturnVoid::OnCover
  virtual StmtRule* Clone() const {return new SRReturnVoid(*this);}
}; // class SRReturnVoid

class SRReturn : public StmtRule {
public:
  SRReturn(const SRReturn& rule_) : StmtRule(rule_) {}
  SRReturn(HWDescription* hwDesc_) :
    StmtRule("return", hwDesc_)
  {
    // Return expression is ignored since calling convention
    // would create proper statements.
    SetStmtPattern(irsReturn(ignoreNT(0)));
  } // SRReturn::SRReturn
  virtual hReal32 GetCost() {return 2.0;}
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRReturn::CheckCondition", matchedStmt_);
    const IRSReturn* lStmt(static_cast<const IRSReturn*>(matchedStmt_));
    bool retVal(!pred.pIsNullExpr(lStmt->GetExpr()));
    EMDEBUG1(retVal);
    return retVal;
  } // SRReturn::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRReturn::OnCover", stmt_);
    // We just need to jump in to epilog or do something more efficient.
    // The return value is assumed to be handled by the calling convention.
    IRBB* lCurrBB(stmt_->GetBB());
    IRBB* lExitBB(extr.eGetFunc(stmt_)->GetCFG()->GetExitBB());
    if (lCurrBB->GetNext() != lExitBB) {
      AddInst(context_, "b", context_.GetLabelOf(lExitBB)->GetLabel());
      context_.AddInst(new AsmSeqDirInfo(string("\t.ltorg")));
    } // if
    EMDEBUG0();
    return;
  } // SRReturn::OnCover
  virtual StmtRule* Clone() const {return new SRReturn(*this);}
}; // class SRReturn

//! \brief Code generation rule for IRSAssign statement.
class SRAssignToStackObj_dbl : public StmtRule {
public:
  SRAssignToStackObj_dbl(const SRAssignToStackObj_dbl& rule_) :
    StmtRule(rule_),
    mObjAddr(rule_.mObjAddr)
  {
  } // SRAssignToStackObj_dbl
  SRAssignToStackObj_dbl(HWDescription* hwDesc_) :
    StmtRule("assignToStackObj_dbl", hwDesc_),
    mObjAddr(0)
  {
    SetStmtPattern(irsAssign(ireAddrOf(iroObject()), regNT(0, new VirtRegDouble())));
  } // SRAssignToStackObj_dbl::SRAssignToStackObj_dbl
  virtual hReal32 GetCost() {return 2.0;}
  static bool sCheckCondition(const IRStmt* matchedStmt_, const AddrStack** addrStack_) {
    BDEBUG3("SRAssignToStackObj_dbl::sCheckCondition", matchedStmt_, (hFInt32)addrStack_);
    bool retVal(false);
    const IRSAssign* lStmt(static_cast<const IRSAssign*>(matchedStmt_));
    //! \note since this static function is called by other rules which may
    //!       have different rule patterns we cannot guarantee that LHS is of type addrof.
    if (pred.pIsAddrOf(lStmt->GetLHS()) == true) {
      IRObject* lObj(extr.eGetObjOfAddrOf(lStmt->GetLHS()));
      if (lObj != 0 && dynamic_cast<AddrStack*>(lObj->GetAddress())) {
        if (addrStack_ != 0) {
          *addrStack_ = static_cast<AddrStack*>(lObj->GetAddress());
        } // if
        if (isDouble(lStmt->GetRHS()->GetType())) {
          retVal = true;
        } // if
      } // if
    } // if
    EDEBUG1(retVal);
    return retVal;
  } // SRAssignToStackObj_dbl::sCheckCondition
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRAssignToStackObj_dbl::CheckCondition", matchedStmt_);
    bool retVal(sCheckCondition(matchedStmt_, &mObjAddr));
    ENSUREMSG(!retVal || mObjAddr != 0, ieStrInconsistentInternalStructure);
    EMDEBUG1(retVal);
    return retVal;
  } // SRAssignToStackObj_dbl::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRAssignToStackObj_dbl::OnCover", stmt_);
    REQUIREDMSG(mObjAddr != 0, ieStrInconsistentInternalStructure);
    IRSAssign* lAssign(static_cast<IRSAssign*>(stmt_));
    VirtRegDouble* lSrcRegs(static_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()));
    VirtReg* lTmpReg(new VirtReg());
    AddInst(context_, "ldr", lTmpReg, mObjAddr->GetOffset());
    VirtReg* lTmpReg4(new VirtReg());
    AddInst(context_, "ldr", lTmpReg4, mObjAddr->GetOffset()+4);
    AddInst(context_, "str", lSrcRegs->GetRegEven(), "pre_indexed", mObjAddr->GetRegister(), lTmpReg);
    AddInst(context_, "str", lSrcRegs->GetRegOdd(), "pre_indexed", mObjAddr->GetRegister(), lTmpReg4);
    EMDEBUG0();
    return;
  } // SRAssignToStackObj_dbl::OnCover
  virtual StmtRule* Clone() const {return new SRAssignToStackObj_dbl(*this);}
private:
  mutable const AddrStack* mObjAddr;
}; // class SRAssignToStackObj_dbl

//! \brief Code generation rule for IRSAssign statement.
class SRAssignToStackObj : public StmtRule {
public:
  SRAssignToStackObj(const SRAssignToStackObj& rule_) :
    StmtRule(rule_),
    mObjAddr(rule_.mObjAddr)
  {
  } // SRAssignToStackObj
  SRAssignToStackObj(HWDescription* hwDesc_) :
    StmtRule("assignToStackObj", hwDesc_),
    mObjAddr(0)
  {
    SetStmtPattern(irsAssign(ireAddrOf(iroObject()), regNT(0)));
  } // SRAssignToStackObj::SRAssignToStackObj
  virtual hReal32 GetCost() {return 2.0;}
  static bool sCheckCondition(const IRStmt* matchedStmt_, const AddrStack** addrStack_) {
    BDEBUG3("SRAssignToStackObj::sCheckCondition", matchedStmt_, (hFInt32)addrStack_);
    bool retVal(false);
    const IRSAssign* lStmt(static_cast<const IRSAssign*>(matchedStmt_));
    //! \note since this static function is called by other rules which may
    //!       have different rule patterns we cannot guarantee that LHS is of type addrof.
    if (dynamic_cast<IREAddrOf*>(lStmt->GetLHS()) != 0) {
      IREAddrOf* lLHS(static_cast<IREAddrOf*>(lStmt->GetLHS()));
      IRObject* lObj(dynamic_cast<IRObject*>(lLHS->GetExpr()));
      if (lObj != 0 && dynamic_cast<AddrStack*>(lObj->GetAddress())) {
        if (addrStack_ != 0) {
          *addrStack_ = static_cast<AddrStack*>(lObj->GetAddress());
        } // if
        if (!isDouble(lStmt->GetRHS()->GetType())) {
          retVal = true;
        } // if
      } // if
    } // if
    EDEBUG1(retVal);
    return retVal;
  } // SRAssignToStackObj::sCheckCondition
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRAssignToStackObj::CheckCondition", matchedStmt_);
    bool retVal(sCheckCondition(matchedStmt_, &mObjAddr));
    ENSUREMSG(!retVal || mObjAddr != 0, ieStrInconsistentInternalStructure);
    EMDEBUG1(retVal);
    return retVal;
  } // SRAssignToStackObj::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRAssignToStackObj::OnCover", stmt_);
    REQUIREDMSG(mObjAddr != 0, ieStrInconsistentInternalStructure);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    //! \todo H Design: Handle storing of different typed variables, i.e.
    //!       byte, short, dword, float, etc.
    //! \todo M Design: 1 << 11, check for the exact number e.g. 11 or the whole 1 << 11.
    IRSAssign* lAssign(static_cast<IRSAssign*>(stmt_));
    IRType* lType(extr.eGetElemType(lAssign->GetLHS()));
    if (mObjAddr->GetOffset() < (1 << 8)) {
      if (lType->GetSize() == 8) {
        AddInst(context_, "strb", *GetRegNT(0), "pre_indexed", mObjAddr->GetRegister(), mObjAddr->GetOffset());
      } else if (lType->GetSize() == 16) {
        AddInst(context_, "strh", *GetRegNT(0), "pre_indexed", mObjAddr->GetRegister(), mObjAddr->GetOffset());
      } else if (lType->GetSize() == 32) {
        AddInst(context_, "str", *GetRegNT(0), "pre_indexed", mObjAddr->GetRegister(), mObjAddr->GetOffset());
      } // if
    } else {
      VirtReg* lTmpReg(new VirtReg());
      AddInst(context_, "ldr", lTmpReg, mObjAddr->GetOffset());
      if (lType->GetSize() == 8) {
        AddInst(context_, "strb", *GetRegNT(0), "pre_indexed", mObjAddr->GetRegister(), lTmpReg);
      } else if (lType->GetSize() == 16) {
        AddInst(context_, "strh", *GetRegNT(0), "pre_indexed", mObjAddr->GetRegister(), lTmpReg);
      } else if (lType->GetSize() == 32) {
        AddInst(context_, "str", *GetRegNT(0), "pre_indexed", mObjAddr->GetRegister(), lTmpReg);
      } // if
    } // if
    EMDEBUG0();
    return;
  } // SRAssignToStackObj::OnCover
  virtual StmtRule* Clone() const {return new SRAssignToStackObj(*this);}
private:
  mutable const AddrStack* mObjAddr;
}; // class SRAssignToStackObj

//! \brief Code generation rule for IRSAssign statement.
class SRAssignToRegObj_dbl : public StmtRule {
public:
  SRAssignToRegObj_dbl(const SRAssignToRegObj_dbl& rule_) :
    StmtRule(rule_),
    mObjAddr(rule_.mObjAddr)
  {
  } // SRAssignToRegObj_dbl
  SRAssignToRegObj_dbl(HWDescription* hwDesc_) :
    StmtRule("assignToRegObj_dbl", hwDesc_),
    mObjAddr(0)
  {
    SetStmtPattern(irsAssign(ireAddrOf(iroObject()), regNT(0, new VirtRegDouble())));
  } // SRAssignToRegObj_dbl::SRAssignToRegObj_dbl
  virtual hReal32 GetCost() {return 2.0;}
  //! \param [in,out] addrReg_ you may pass null pointer, or address of
  //!        an AddrReg pointer to be filled in.
  //! \param matchedStmt_ \todo doc.
  static bool sCheckCondition(const IRStmt* matchedStmt_, const AddrReg** addrReg_) {
    BDEBUG3("SRAssignToRegObj_dbl::sCheckCondition", matchedStmt_, (hFInt32)addrReg_);
    bool retVal(false);
    const IRSAssign* lStmt(static_cast<const IRSAssign*>(matchedStmt_));
    if (pred.pIsAddrOf(lStmt->GetLHS()) == true) {
      IRObject* lObj(extr.eGetObjOfAddrOf(lStmt->GetLHS()));
      if (lObj != 0 && dynamic_cast<AddrReg*>(lObj->GetAddress())) {
        if (addrReg_ != 0) {
          *addrReg_ = static_cast<AddrReg*>(lObj->GetAddress());
        } // if
        if (isDouble(lStmt->GetRHS()->GetType())) {
          retVal = true;
        } // if
      } // if
    } // if
    EDEBUG1(retVal);
    return retVal;
  } // SRAssignToRegObj_dbl::sCheckCondition
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRAssignToRegObj_dbl::CheckCondition", matchedStmt_);
    bool retVal(sCheckCondition(matchedStmt_, &mObjAddr));
    ENSUREMSG(!retVal || mObjAddr != 0, ieStrInconsistentInternalStructure);
    EMDEBUG1(retVal);
    return retVal;
  } // SRAssignToRegObj_dbl::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRAssignToRegObj_dbl::OnCover", stmt_);
    REQUIREDMSG(mObjAddr != 0, ieStrInconsistentInternalStructure);
    ASSERTMSG(dynamic_cast<VirtRegDouble*>(mObjAddr->GetRegister()), ieStrInconsistentInternalStructure);
    ASSERTMSG(dynamic_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()), ieStrInconsistentInternalStructure);
    VirtRegDouble* lSrcRegs(static_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()));
    VirtRegDouble* lObjReg(static_cast<VirtRegDouble*>(mObjAddr->GetRegister()));
    AddInst(context_, "mov", lObjReg->GetRegEven(), lSrcRegs->GetRegEven());
    AddInst(context_, "mov", lObjReg->GetRegOdd(), lSrcRegs->GetRegOdd());
    EMDEBUG0();
    return;
  } // SRAssignToRegObj_dbl::OnCover
  virtual StmtRule* Clone() const {return new SRAssignToRegObj_dbl(*this);}
private:
  mutable const AddrReg* mObjAddr;
}; // class SRAssignToRegObj_dbl

//! \brief Code generation rule for IRSAssign statement.
class SRAssignToRegObj : public StmtRule {
public:
  SRAssignToRegObj(const SRAssignToRegObj& rule_) :
    StmtRule(rule_),
    mObjAddr(rule_.mObjAddr)
  {
  } // SRAssignToRegObj
  SRAssignToRegObj(HWDescription* hwDesc_) :
    StmtRule("assignToRegObj", hwDesc_),
    mObjAddr(0)
  {
    SetStmtPattern(irsAssign(ireAddrOf(iroObject()), regNT(0)));
  } // SRAssignToRegObj::SRAssignToRegObj
  virtual hReal32 GetCost() {return 2.0;}
  //! \param [in,out] addrReg_ you may pass null pointer, or address of
  //!        an AddrReg pointer to be filled in.
  //! \param matchedStmt_ \todo doc.
  static bool sCheckCondition(const IRStmt* matchedStmt_, const AddrReg** addrReg_) {
    BDEBUG3("SRAssignToRegObj::sCheckCondition", matchedStmt_, (hFInt32)addrReg_);
    bool retVal(false);
    const IRSAssign* lStmt(static_cast<const IRSAssign*>(matchedStmt_));
    if (pred.pIsAddrOf(lStmt->GetLHS()) == true) {
      IRObject* lObj(extr.eGetObjOfAddrOf(lStmt->GetLHS()));
      if (lObj != 0 && dynamic_cast<AddrReg*>(lObj->GetAddress())) {
        if (addrReg_ != 0) {
          *addrReg_ = static_cast<AddrReg*>(lObj->GetAddress());
        } // if
        if (!isDouble(lStmt->GetRHS()->GetType())) {
          retVal = true;
        } // if
      } // if
    } // if
    EDEBUG1(retVal);
    return retVal;
  } // SRAssignToRegObj::sCheckCondition
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRAssignToRegObj::CheckCondition", matchedStmt_);
    bool retVal(sCheckCondition(matchedStmt_, &mObjAddr));
    ENSUREMSG(!retVal || mObjAddr != 0, ieStrInconsistentInternalStructure);
    EMDEBUG1(retVal);
    return retVal;
  } // SRAssignToRegObj::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRAssignToRegObj::OnCover", stmt_);
    REQUIREDMSG(mObjAddr != 0, ieStrInconsistentInternalStructure);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    //! \todo H Design: Handle storing of different typed variables, i.e.
    //!       byte, short, dword, float, etc.
    AddInst(context_, "mov", mObjAddr->GetRegister(), *GetRegNT(0));
    EMDEBUG0();
    return;
  } // SRAssignToRegObj::OnCover
  virtual StmtRule* Clone() const {return new SRAssignToRegObj(*this);}
private:
  mutable const AddrReg* mObjAddr;
}; // class SRAssignToRegObj

class SRAssign_dbl : public StmtRule {
public:
  SRAssign_dbl(const SRAssign_dbl& rule_) : StmtRule(rule_) {}
  SRAssign_dbl(HWDescription* hwDesc_) :
    StmtRule("assign_dbl", hwDesc_)
  {
    SetStmtPattern(irsAssign(regNT(0), regNT(1, new VirtRegDouble())));
  } // SRAssign_dbl::SRAssign_dbl
  virtual hReal32 GetCost() {return 2.0;}
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRAssign_dbl::CheckCondition", matchedStmt_);
    bool retVal(!SRAssignToRegObj_dbl::sCheckCondition(matchedStmt_, 0) &&
      !SRAssignToStackObj_dbl::sCheckCondition(matchedStmt_, 0));
    if (retVal == true) {
      const IRSAssign* lAssign(static_cast<const IRSAssign*>(matchedStmt_));
      retVal = isDouble(lAssign->GetRHS()->GetType());
    } // if
    EMDEBUG1(retVal);
    return retVal;
  } // SRAssign_dbl::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRAssign_dbl::OnCover", stmt_);
    ASSERTMSG(dynamic_cast<VirtRegDouble*>(GetRegNT(1)->GetVReg()), ieStrInconsistentInternalStructure);
    VirtRegDouble* lSrcRegs(static_cast<VirtRegDouble*>(GetRegNT(1)->GetVReg()));
    AddInst(context_, "str", lSrcRegs->GetRegEven(), "post_indexed", *GetRegNT(0), tBigInt(4));
    AddInst(context_, "str", lSrcRegs->GetRegOdd(), "post_indexed", *GetRegNT(0));
    EMDEBUG0();
    return;
  } // SRAssign_dbl::OnCover
  virtual StmtRule* Clone() const {return new SRAssign_dbl(*this);}
}; // class SRAssign_dbl

//! \brief Handles the assignments in to memory addresses.
//! Source is register address destination is memory address.
class SRAssign : public StmtRule {
public:
  SRAssign(const SRAssign& rule_) : StmtRule(rule_) {}
  SRAssign(HWDescription* hwDesc_) :
    StmtRule("assign", hwDesc_)
  {
    SetStmtPattern(irsAssign(regNT(0), regNT(1)));
  } // SRAssign::SRAssign
  virtual hReal32 GetCost() {return 2.0;}
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {
    BMDEBUG2("SRAssign::CheckCondition", matchedStmt_);
    bool retVal(!SRAssignToRegObj::sCheckCondition(matchedStmt_, 0) &&
      !SRAssignToStackObj::sCheckCondition(matchedStmt_, 0));
    if (retVal == true) {
      IRSAssign* lAssign(static_cast<const IRSAssign*>(matchedStmt_));
      retVal = !isDouble(lAssign->GetRHS()->GetType());
    } // if
    EMDEBUG1(retVal);
    return retVal;
  } // SRAssign::CheckCondition
  virtual void OnCover(IRStmt* stmt_, CGFuncContext& context_) {
    BMDEBUG2("SRAssign::OnCover", stmt_);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    IRSAssign* lAssign(static_cast<IRSAssign*>(stmt_));
    IRType* lType(extr.eGetElemType(lAssign->GetLHS()));
    if (pred.pIsFundamentalType(lType) == true) {
      if (pred.pIsInt(lType) == true) {
        if (lType->GetSize() == 8) {
          AddInst(context_, "strb", *GetRegNT(1), "post_indexed", *GetRegNT(0));
        } else if (lType->GetSize() == 16) {
          AddInst(context_, "strh", *GetRegNT(1), "pre_indexed", *GetRegNT(0));
        } else if (lType->GetSize() == 32) {
          AddInst(context_, "str", *GetRegNT(1), "post_indexed", *GetRegNT(0));
        } // if
      } else if (pred.pIsPtr(lType) == true) {
        AddInst(context_, "str", *GetRegNT(1), "post_indexed", *GetRegNT(0));
      } else if (pred.pIsReal(lType) == true) {
        AddInst(context_, "str", *GetRegNT(1), "post_indexed", *GetRegNT(0));
      } else {
        ASSERTMSG(0, ieStrNotImplemented);
      } // if
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
    EMDEBUG0();
    return;
  } // SRAssign::OnCover
  virtual StmtRule* Clone() const {return new SRAssign(*this);}
}; // class SRAssign

// ------------------------------
// Expression Rules
// ------------------------------

//! \brief Moves IR real constants in to registers.
class ERRealConst : public ExprRule {
public:
  ERRealConst(const ERRealConst& rule_) : ExprRule(rule_) {}
  ERRealConst(HWDescription* hwDesc_) :
    ExprRule("realconst", hwDesc_)
  {
    SetExprPattern(regNT(0), ireRealConst());
  } // ERRealConst::ERRealConst
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    bool retVal(false);
    if (matchedExpr_->GetType()->GetSize() == 32) {
      retVal = true;
    } else {
      ASSERTMSG(0, ieStrNotImplemented << " : non 32bits real constants should have been lowered to variables.");
    } // if
    return retVal;
  } // ERRealConst::CheckCondition
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    IRERealConst* lRealConst(static_cast<IRERealConst*>(expr_));
    AddInst(context_, "ldr", *GetRegNT(0), tBigInt(util.uBitConvert<hUInt32, hReal32>(lRealConst->GetValue().get_hReal32())));
    return;
  } // ERRealConst::OnCover
  virtual ExprRule* Clone() const {return new ERRealConst(*this);}
}; // class ERRealConst

class ERAddrConst : public ExprRule {
public:
  ERAddrConst(const ERAddrConst& rule_) : ExprRule(rule_) {}
  ERAddrConst(HWDescription* hwDesc_) :
    ExprRule("addrconst", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAddrConst());
  } // ERAddrConst::ERAddrConst
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAddrConst::OnCover", expr_);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    IREAddrConst* lAddrConst(static_cast<IREAddrConst*>(expr_));
    if (lAddrConst->IsConst() == true) {
      tBigInt lValue(lAddrConst->GetConstValue());
      if (lValue >= 0 && ARMHW::IsImm8r(lValue) == true) {
        AddInst(context_, "mov", *GetRegNT(0), lValue);
//      } else if (lValue < 0 && ARMHW::IsImm8r(-lValue-1) == true) {
//        AddInst(context_, "mvn", *GetRegNT(0), -lValue-1);
      } else if (lValue < 0) {
        AddInst(context_, "ldr", *GetRegNT(0), lValue);
      } else {
        AddInst(context_, "ldr", *GetRegNT(0), lValue);
      } // if
    } else if (lAddrConst->IsLabel() == true) {
      ASSERTMSG(0, ieStrNotImplemented);
    } else  if (lAddrConst->IsObject() == true) {
      ASSERTMSG(dynamic_cast<IROGlobal*>(lAddrConst->GetObject()) != 0, ieStrInconsistentInternalStructure);
      IROGlobal* lObj(static_cast<IROGlobal*>(lAddrConst->GetObject()));
      AsmSeqData* lAddrData(AddInstsArmAddrDataWord(context_, *lObj));
      AddInst(context_, "ldr", *GetRegNT(0), lAddrData->GetLabel()->GetLabel());
    } // if
    EMDEBUG0();
    return;
  } // ERAddrConst::OnCover
  virtual ExprRule* Clone() const {return new ERAddrConst(*this);}
}; // class ERAddrConst

class ERStrConst : public ExprRule {
public:
  ERStrConst(const ERStrConst& rule_) : ExprRule(rule_) {}
  ERStrConst(HWDescription* hwDesc_) :
    ExprRule("strconst", hwDesc_)
  {
    SetExprPattern(regNT(0), ireStrConst());
  } // ERStrConst::ERStrConst
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERStrConst::OnCover", expr_);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    IREStrConst* lStrConst(static_cast<IREStrConst*>(expr_));
    AsmSeqData* lAddrData(AddInstsArmDataString(context_, lStrConst->GetValue()));
    AddInst(context_, "ldr", *GetRegNT(0), lAddrData->GetLabel()->GetLabel());
    EMDEBUG0();
    return;
  } // ERStrConst::OnCover
  virtual ExprRule* Clone() const {return new ERStrConst(*this);}
}; // class ERStrConst

//! \brief Moves IR bool constants in to registers.
class ERBoolConst : public ExprRule {
public:
  ERBoolConst(const ERBoolConst& rule_) : ExprRule(rule_) {}
  ERBoolConst(HWDescription* hwDesc_) :
    ExprRule("boolconst", hwDesc_)
  {
    SetExprPattern(regNT(0), ireBoolConst());
  } // ERBoolConst::ERBoolConst
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERBoolConst::OnCover", expr_);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    IREBoolConst* lBoolConst(static_cast<IREBoolConst*>(expr_));
    if (lBoolConst->GetValue() == true) {
      AddInst(context_, "mov", *GetRegNT(0), tBigInt(1));
    } else {
      AddInst(context_, "mov", *GetRegNT(0), tBigInt(0));
    } // if
    EMDEBUG0();
    return;
  } // ERBoolConst::OnCover
  virtual ExprRule* Clone() const {return new ERBoolConst(*this);}
}; // class ERBoolConst

//! \brief Moves IR integer constants in to registers.
class ERIntConst : public ExprRule {
public:
  ERIntConst(const ERIntConst& rule_) : ExprRule(rule_) {}
  ERIntConst(HWDescription* hwDesc_) :
    ExprRule("intconst", hwDesc_)
  {
    SetExprPattern(regNT(0), ireIntConst());
  } // ERIntConst::ERIntConst
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERIntConst::OnCover", expr_);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    IREIntConst* lIntConst(static_cast<IREIntConst*>(expr_));
    // The easiest way to get a 32bit value in to a register is to put the
    // value in a literal pool and load it from there.
    //! \todo H Design: Implement literal pools.
    //! \todo M Opt: We can also rematerialize the constant by at most 4 instructions.
    if (lIntConst->GetValue() >= 0 && ARMHW::IsImm8r(lIntConst->GetValue()) == true) {
      hFUInt32 lValue(lIntConst->GetValue().get_hFUInt32());
      AddInst(context_, "mov", *GetRegNT(0), tBigInt(lValue));
    } else if (lIntConst->GetValue() < 0) {
//    } else if (lIntConst->GetValue() < 0 && ARMHW::IsImm8r(-lIntConst->GetValue()-1) == true) {
//      AddInst(context_, "mvn", *GetRegNT(0), -lIntConst->GetValue()-1);
      hFUInt32 lValue(lIntConst->GetValue().get_hFUInt32());
      AddInst(context_, "ldr", *GetRegNT(0), tBigInt(lValue));
    } else {
      hFUInt32 lValue(lIntConst->GetValue().get_hFUInt32());
      AddInst(context_, "ldr", *GetRegNT(0), tBigInt(lValue));
    } // if
    EMDEBUG0();
    return;
  } // ERIntConst::OnCover
  virtual ExprRule* Clone() const {return new ERIntConst(*this);}
}; // class ERIntConst

class ERMul : public ExprRule {
public:
  ERMul(const ERMul& rule_) : ExprRule(rule_) {}
  ERMul(HWDescription* hwDesc_) :
    ExprRule("mul", hwDesc_)
  {
    SetExprPattern(regNT(0), ireMul(regNT(1), regNT(2)));
  } // ERMul::ERMul
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERMul::OnCover", expr_);
    AddInst(context_, "mul", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERMul::OnCover
  virtual ExprRule* Clone() const {return new ERMul(*this);}
}; // class ERMul

class ERCast_dbl : public ExprRule {
public:
  ERCast_dbl(const ERCast_dbl& rule_) : ExprRule(rule_) {}
  ERCast_dbl(HWDescription* hwDesc_) :
    ExprRule("cast_dbl", hwDesc_)
  {
    SetExprPattern(regNT(0, new VirtRegDouble()), ireCast(regNT(1, new VirtRegDouble())));
  } // ERCast_dbl::ERCast_dbl
  virtual hReal32 GetCost() {return 0.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERCast_dbl::CheckCondition", matchedExpr_);
    const IRECast* lCastExpr(static_cast<const IRECast*>(matchedExpr_));
    bool retVal(isDouble(lCastExpr->GetType()) == true && isDouble(lCastExpr->GetExpr()->GetType()) == true);
    EMDEBUG1(retVal);
    return retVal;
  } // ERCast_dbl::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERCast_dbl::OnCover", expr_);
    VirtRegDouble* lSrcRegs(static_cast<VirtRegDouble*>(GetRegNT(1)->GetVReg()));
    VirtRegDouble* lDstRegs(static_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()));
    AddInst(context_, "mov", lDstRegs->GetRegEven(), lSrcRegs->GetRegEven());
    AddInst(context_, "mov", lDstRegs->GetRegOdd(), lSrcRegs->GetRegOdd());
    EMDEBUG0();
    return;
  } // ERCast_dbl::OnCover
  virtual ExprRule* Clone() const {return new ERCast_dbl(*this);}
}; // class ERCast_dbl

//! \note Handling of type conversions:
//!       Integer up conversions; deref operator loads a value from
//! an address in to a register. In ARM HW registers can hold the
//! values up to 32 bits. Therefore a type cast from int8 to int32
//! it nothing but a copy operation(or even it should be null).
//!       Integer down conversions; when a negative integer is
//! down casted to an unsigned type, e.g. from int to unsigned short,
//! we need to mask the bits, note that the representation may still be in the
//! same register.
//! \sa ERDeref.
class ERCast : public ExprRule {
public:
  ERCast(const ERCast& rule_) : ExprRule(rule_) {}
  ERCast(HWDescription* hwDesc_) :
    ExprRule("cast", hwDesc_)
  {
    SetExprPattern(regNT(0), ireCast(regNT(1)));
  } // ERCast::ERCast
  virtual hReal32 GetCost() {return 0.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERCast::CheckCondition", matchedExpr_);
    const IRECast* lCastExpr(static_cast<const IRECast*>(matchedExpr_));
    bool retVal(false);
    IRType* lFrom(lCastExpr->GetExpr()->GetType());
    IRType* lTo(lCastExpr->GetType());
    if (pred.pIsPtrOrArrays(lCastExpr) == true) {
      retVal = true;
    } else if (pred.pIsInts(lCastExpr) == true) {
      retVal = true;
    } else if (pred.pIsBool(lFrom) == true) {
      retVal = true;
    } else if (pred.pIsReal(lFrom) == true && pred.pIsReal(lTo) == true && !isDouble(lFrom) && !isDouble(lTo)) {
      retVal = true;
    } else if (pred.pIsInt(lFrom) == true && pred.pIsBool(lTo) == true) {
      // int to bool
      retVal = true;
    } else if (pred.pIsBool(lFrom) == true && pred.pIsInt(lTo) == true) {
      // bool to int
      retVal = true;
    } else if (pred.pIsPtr(lFrom) == true && pred.pIsInt(lTo) == true) {
      // ptr to int
      retVal = true;
    } // if
    EMDEBUG1(retVal);
    return retVal;
  } // ERCast::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERCast::OnCover", expr_);
    const IRECast* lCastExpr(static_cast<const IRECast*>(expr_));
    IRType* lFrom(lCastExpr->GetExpr()->GetType());
    IRType* lTo(lCastExpr->GetType());
    if (pred.pIsPtrOrArrays(lCastExpr) == true) {
      AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
    } else if (pred.pIsInts(lCastExpr) == true) {
      IRTInt* lTargetInt(static_cast<IRTInt*>(lTo));
      if (pred.pIsSignedToUnsigned(lCastExpr) == true && pred.pIsNarrows(lCastExpr) == true) {
        tBigInt lShiftAmount(lFrom->GetSize()-lTo->GetSize());
        AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1), "lsl", lShiftAmount);
        AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(0), "lsr", lShiftAmount);
      } else {
        AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
      } // if
    } else if (pred.pIsReal(lFrom) == true) {
      ASSERTMSG(pred.pIsReal(lTo) == true && !isDouble(lFrom) && !isDouble(lTo), ieStrInconsistentInternalStructure);
      AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
    } else if (pred.pIsBool(lFrom) == true) {
      AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
    } else if (pred.pIsInt(lFrom) == true && pred.pIsBool(lTo) == true) {
      AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
    } else if (pred.pIsBool(lFrom) == true && pred.pIsInt(lTo) == true) {
      AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
    } else if (pred.pIsPtr(lFrom) == true && pred.pIsInt(lTo) == true) {
      AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
    EMDEBUG0();
    return;
  } // ERCast::OnCover
  virtual ExprRule* Clone() const {return new ERCast(*this);}
}; // class ERCast

class ERAddrOfDeref_dbl : public ExprRule {
public:
  ERAddrOfDeref_dbl(const ERAddrOfDeref_dbl& rule_) : ExprRule(rule_) {}
  ERAddrOfDeref_dbl(HWDescription* hwDesc_) :
    ExprRule("addrofderef_dbl", hwDesc_)
  {
    SetExprPattern(regNT(0, new VirtRegDouble()), ireAddrOf(ireDeref(regNT(1, new VirtRegDouble()))));
  } // ERAddrOfDeref_dbl::ERAddrOfDeref_dbl
  virtual hReal32 GetCost() {return 0.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    const IREAddrOf* lAddrOf(static_cast<const IREAddrOf*>(matchedExpr_));
    bool retVal(isDouble(lAddrOf->GetExpr()->GetType()) == true);
    return retVal;
  } // ERAddrOfDeref_dbl::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAddrOfDeref_dbl::OnCover", expr_);
    VirtRegDouble* lDstRegs(static_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()));
    VirtRegDouble* lSrcRegs(static_cast<VirtRegDouble*>(GetRegNT(1)->GetVReg()));
    AddInst(context_, "mov", lDstRegs->GetRegEven(), lSrcRegs->GetRegEven());
    AddInst(context_, "mov", lDstRegs->GetRegOdd(), lSrcRegs->GetRegOdd());
    EMDEBUG0();
  } // ERAddrOfDeref_dbl::OnCover
  virtual ExprRule* Clone() const {return new ERAddrOfDeref_dbl(*this);}
}; // class ERAddrOfDeref_dbl

class ERAddrOfDeref : public ExprRule {
public:
  ERAddrOfDeref(const ERAddrOfDeref& rule_) : ExprRule(rule_) {}
  ERAddrOfDeref(HWDescription* hwDesc_) :
    ExprRule("addrofderef", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAddrOf(ireDeref(regNT(1))));
  } // ERAddrOfDeref::ERAddrOfDeref
  virtual hReal32 GetCost() {return 0.0;}
//  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
//    const IREAddrOf* lAddrOf(static_cast<const IREAddrOf*>(matchedExpr_));
//    bool retVal(!isDouble(lAddrOf->GetExpr()->GetType()));
//    return retVal;
//  } // ERAddrOfDeref::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAddrOfDeref::OnCover", expr_);
    AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1));
    EMDEBUG0();
  } // ERAddrOfDeref::OnCover
  virtual ExprRule* Clone() const {return new ERAddrOfDeref(*this);}
}; // class ERAddrOfDeref

class ERAddrOf_dbl : public ExprRule {
public:
  ERAddrOf_dbl(const ERAddrOf_dbl& rule_) : ExprRule(rule_) {}
  ERAddrOf_dbl(HWDescription* hwDesc_) :
    ExprRule("addrof_dbl", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAddrOf(iroObject()));
  } // ERAddrOf_dbl::ERAddrOf_dbl
  virtual hReal32 GetCost() {return 0.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERAddrOf_dbl::CheckCondition", matchedExpr_);
    REQUIREDMSG(extr.eGetObjOfAddrOf(matchedExpr_, scDBCEnsureNonNull), ieStrInconsistentInternalStructure);
    const IRObject* lObj(extr.eGetObjOfAddrOf(matchedExpr_));
    PDEBUG("CodeGen", "detail", " obj: " << progcxt(lObj));
    bool retVal(isDouble(lObj->GetType()) == true);
    EMDEBUG1(retVal);
    return retVal;
  } // ERAddrOf_dbl::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAddrOf_dbl::OnCover", expr_);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    IRObject* lObj(extr.eGetObjOfAddrOf(expr_));
    if (IROGlobal* lGlobObj = dynamic_cast<IROGlobal*>(lObj)) {
      AsmSeqData* lAddrData(AddInstsArmAddrDataWord(context_, *lGlobObj));
      AddInst(context_, "ldr", *GetRegNT(0), lAddrData->GetLabel()->GetLabel());
    } else if (AddrStack* lAddrStack = dynamic_cast<AddrStack*>(lObj->GetAddress())) {
      Register* lReg(lAddrStack->GetRegister());
      tBigInt lOffset(lAddrStack->GetOffset());
      if (lOffset >= 0 && ARMHW::IsImm8r(lOffset) == true) {
        AddInst(context_, "add", *GetRegNT(0), lReg, lOffset);
      } else if (lOffset >= 0) {
        AddInst(context_, "ldr", *GetRegNT(0), lOffset);
        AddInst(context_, "add", *GetRegNT(0), lReg, *GetRegNT(0));
      } else if (lOffset < 0) {
        AddInst(context_, "ldr", *GetRegNT(0), lOffset);
        AddInst(context_, "sub", *GetRegNT(0), lReg, *GetRegNT(0));
      } // if
    } else if (AddrReg* lAddrReg = dynamic_cast<AddrReg*>(lObj->GetAddress())) {
      VirtRegDouble* lDstRegs(static_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()));
      VirtRegDouble* lObjRegs(static_cast<VirtRegDouble*>(lAddrReg->GetRegister()));
      AddInst(context_, "mov", lDstRegs->GetRegEven(), lObjRegs->GetRegEven());
      AddInst(context_, "mov", lDstRegs->GetRegOdd(), lObjRegs->GetRegOdd());
    } // if
    EMDEBUG0();
    return;
  } // ERAddrOf_dbl::OnCover
  virtual ExprRule* Clone() const {return new ERAddrOf_dbl(*this);}
}; // class ERAddrOf_dbl

//! \brief Rule to match the address of expression.
//! \note It does not match to register addresses.
class ERAddrOf : public ExprRule {
public:
  ERAddrOf(const ERAddrOf& rule_) : ExprRule(rule_) {}
  ERAddrOf(HWDescription* hwDesc_) :
    ExprRule("addrof", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAddrOf(iroObject()));
  } // ERAddrOf::ERAddrOf
  virtual hReal32 GetCost() {return 1.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERAddrOf::CheckCondition", matchedExpr_);
    REQUIREDMSG(pred.pIsAddrOf(matchedExpr_) == true, ieStrInconsistentInternalStructure);
    REQUIREDMSG(pred.pIsObj(static_cast<const IREAddrOf*>(matchedExpr_)->GetExpr()) == true,
      ieStrInconsistentInternalStructure);
    const IREAddrOf* lAddrOf(static_cast<const IREAddrOf*>(matchedExpr_));
    const IRObject* lObj(static_cast<const IRObject*>(lAddrOf->GetExpr()));
    PDEBUG("CodeGen", "detail", " obj: " << progcxt(lObj));
    bool retVal(!isDouble(lObj->GetType()));
    EMDEBUG1(retVal);
    return retVal;
  } // ERAddrOf::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAddrOf::OnCover", expr_);
    PDEBUG("CodeGen", "detail", "OnCover beg: " << hwdesccxt(this));
    IREAddrOf* lAddrOf(static_cast<IREAddrOf*>(expr_));
    IRExpr* lAddrOfExpr(lAddrOf->GetExpr());
    //! \todo M Design: it is guaranteed that the expr is object.
    if (IRObject* lObj = dynamic_cast<IRObject*>(lAddrOfExpr)) {
      Address* lObjAddr(lObj->GetAddress());
      if (IROGlobal* lGlobObj = dynamic_cast<IROGlobal*>(lObj)) {
        // Generate the following 3 lines:
        // .Lxxx: // <- label GenAsmSeqLabel()
        //    .word aGlobalVar // aGlobalVar will be patched by linker
        // LDR GetRegNT(0), .Lxxx
        // Somewhere else, could be in another module, definition of aGlobalVar:
        // aGlobalVar:
        //    .space 4 // aGlobalVar is 4 bytes long variable, e.g. "char aGlobalVar[4];"
        AsmSeqData* lAddrData(AddInstsArmAddrDataWord(context_, *lGlobObj));
        AddInst(context_, "ldr", *GetRegNT(0), lAddrData->GetLabel()->GetLabel());
      } else if (AddrStack* lAddrStack = dynamic_cast<AddrStack*>(lObjAddr)) {
        Register* lReg(lAddrStack->GetRegister());
        tBigInt lOffset(lAddrStack->GetOffset());
        //! \todo M Design: In optimizing case it is better to assign
        //!       different registers for const load target.
        if (lOffset >= 0 && ARMHW::IsImm8r(lOffset) == true) {
          AddInst(context_, "add", *GetRegNT(0), lReg, lOffset);
        //! \todo M Design: handle the negative numbers.
//        } else if (lOffset < 0 && ARMHW::IsImm8r(-lOffset-1) == true) {
//          AddInst(context_, "sub", *GetRegNT(0), lReg, -lOffset-1);
        } else if (lOffset >= 0) {
          AddInst(context_, "ldr", *GetRegNT(0), lOffset);
          AddInst(context_, "add", *GetRegNT(0), lReg, *GetRegNT(0));
        } else if (lOffset < 0) {
          AddInst(context_, "ldr", *GetRegNT(0), lOffset);
          AddInst(context_, "sub", *GetRegNT(0), lReg, *GetRegNT(0));
        } // if
      } else if (AddrLTConst* lAddrLTConst = dynamic_cast<AddrLTConst*>(lObjAddr)) {
        NOTIMPLEMENTED(H);
        ASSERTMSG(0, ieStrNotImplemented);
      } else if (AddrReg* lAddrReg = dynamic_cast<AddrReg*>(lObjAddr)) {
        AddInst(context_, "mov", *GetRegNT(0), lAddrReg->GetRegister());
      } else {
        ASSERTMSG(0, ieStrPCShouldNotReachHere);
      } // if
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
      NOTIMPLEMENTED(M);
    } // if
    EMDEBUG0();
    return;
  } // ERAddrOf::OnCover
  virtual ExprRule* Clone() const {return new ERAddrOf(*this);}
}; // class ERAddrOf

//! \brief Since variables in registers has no real address it is better
//!        to handle use of it separately.
class ERUseOfReg_dbl : public ExprRule {
public:
  ERUseOfReg_dbl(const ERUseOfReg_dbl& rule_) :
    ExprRule(rule_),
    mRegAddr(rule_.mRegAddr)
  {
  } // ERUseOfReg_dbl::ERUseOfReg_dbl
  ERUseOfReg_dbl(HWDescription* hwDesc_) :
    ExprRule("useofreg_dbl", hwDesc_),
    mRegAddr(0)
  {
    SetExprPattern(regNT(0, new VirtRegDouble()), ireDeref(ireAddrOf(iroObject())));
  } // ERUseOfReg_dbl::ERUseOfReg_dbl
  virtual hReal32 GetCost() {return 1.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERUseOfReg_dbl::CheckCondition", matchedExpr_);
    bool retVal(false);
    const IREDeref* lIRDeref(static_cast<const IREDeref*>(matchedExpr_));
    //! \todo H Design: Need to reset the rule, same rule object is called
    //!       many times and member data may be set at different runs.
    mRegAddr = 0;
    if (isDouble(lIRDeref->GetType()) == true) {
      const IREAddrOf* lIRAddrOf(static_cast<const IREAddrOf*>(lIRDeref->GetExpr()));
      IRObject* lObj(dynamic_cast<IRObject*>(lIRAddrOf->GetExpr()));
      if (lObj != 0 && pred.pIsAddrSet(lObj) == true) {
        mRegAddr = dynamic_cast<AddrReg*>(lObj->GetAddress());
        if (mRegAddr != 0) {
          retVal = true;
        } // if
      } // if
    } // if
    EMDEBUG1(retVal);
    return retVal;
  } // ERUseOfReg_dbl::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERUseOfReg_dbl::OnCover", expr_);
    ASSERTMSG(dynamic_cast<VirtRegDouble*>(mRegAddr->GetRegister()) != 0, ieStrInconsistentInternalStructure);
    VirtRegDouble* lDstRegs(static_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()));
    VirtRegDouble* lSrcRegs(static_cast<VirtRegDouble*>(mRegAddr->GetRegister()));
    AddInst(context_, "mov", lDstRegs->GetRegEven(), lSrcRegs->GetRegEven());
    AddInst(context_, "mov", lDstRegs->GetRegOdd(), lSrcRegs->GetRegOdd());
    EMDEBUG0();
    return;
  } // ERUseOfReg_dbl::OnCover
  virtual ExprRule* Clone() const {return new ERUseOfReg_dbl(*this);}
private:
  mutable AddrReg* mRegAddr;
}; // class ERUseOfReg_dbl

//! \brief Since variables in registers has no real address it is better
//!        to handle use of it separately.
class ERUseOfReg : public ExprRule {
public:
  ERUseOfReg(const ERUseOfReg& rule_) :
    ExprRule(rule_),
    mRegAddr(rule_.mRegAddr)
  {
  } // ERUseOfReg::ERUseOfReg
  ERUseOfReg(HWDescription* hwDesc_) :
    ExprRule("useofreg", hwDesc_),
    mRegAddr(0)
  {
    SetExprPattern(regNT(0), ireDeref(ireAddrOf(iroObject())));
  } // ERUseOfReg::ERUseOfReg
  virtual hReal32 GetCost() {return 1.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERUseOfReg::CheckCondition", matchedExpr_);
    bool retVal(false);
    const IREDeref* lIRDeref(static_cast<const IREDeref*>(matchedExpr_));
    //! \todo H Design: Need to reset the rule, same rule object is called
    //!       many times and member data may be set at different runs.
    mRegAddr = 0;
    if (pred.pIsFundamentalType(lIRDeref->GetType()) == true) {
      const IREAddrOf* lIRAddrOf(static_cast<const IREAddrOf*>(lIRDeref->GetExpr()));
      IRObject* lObj(dynamic_cast<IRObject*>(lIRAddrOf->GetExpr()));
      if (lObj != 0 && pred.pIsAddrSet(lObj) == true) {
        mRegAddr = dynamic_cast<AddrReg*>(lObj->GetAddress());
        if (mRegAddr != 0) {
          retVal = true;
        } // if
      } // if
    } // if
    EMDEBUG1(retVal);
    return retVal;
  } // ERUseOfReg::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERUseOfReg::OnCover", expr_);
    AddInst(context_, "mov", *GetRegNT(0), mRegAddr->GetRegister());
    EMDEBUG0();
    return;
  } // ERUseOfReg::OnCover
  virtual ExprRule* Clone() const {return new ERUseOfReg(*this);}
private:
  mutable AddrReg* mRegAddr;
}; // class ERUseOfReg

class ERDeref_dbl : public ExprRule {
public:
  ERDeref_dbl(const ERDeref_dbl& rule_) : ExprRule(rule_) {}
  ERDeref_dbl(HWDescription* hwDesc_) :
    ExprRule("deref_dbl", hwDesc_)
  {
    SetExprPattern(regNT(0, new VirtRegDouble), ireDeref(regNT(1)));
  } // ERDeref_dbl::ERDeref_dbl
  virtual hReal32 GetCost() {return 1.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    bool retVal(isDouble(matchedExpr_->GetType()) == true);
    return retVal;
  } // ERDeref::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERDeref_dbl::OnCover", expr_);
    VirtRegDouble* lDstRegs(static_cast<VirtRegDouble*>(GetRegNT(0)->GetVReg()));
    AddInst(context_, "ldr", lDstRegs->GetRegEven(), "post_indexed", *GetRegNT(1), tBigInt(4));
    AddInst(context_, "ldr", lDstRegs->GetRegOdd(), "post_indexed", *GetRegNT(1));
    /* LIE */ new RARMustNotBeSame(RegSet(lDstRegs->GetRegEven()) | GetRegNT(1)->GetVReg());
    /* LIE */ new RARMustNotBeSame(RegSet(lDstRegs->GetRegOdd()) | GetRegNT(1)->GetVReg());
    EMDEBUG0();
    return;
  } // ERDeref_dbl::OnCover
  virtual ExprRule* Clone() const {return new ERDeref_dbl(*this);}
}; // class ERDeref_dbl

//! \brief Deref loads a value pointer by a register.
//! It also considers the type of the to be loaded value.
class ERDeref : public ExprRule {
public:
  ERDeref(const ERDeref& rule_) : ExprRule(rule_) {}
  ERDeref(HWDescription* hwDesc_) :
    ExprRule("deref", hwDesc_)
  {
    SetExprPattern(regNT(0), ireDeref(regNT(1)));
  } // ERDeref::ERDeref
  virtual hReal32 GetCost() {return 1.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    bool retVal(!isDouble(matchedExpr_->GetType()));
    return retVal;
  } // ERDeref::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERDeref::OnCover", expr_);
    IREDeref* lDeref(static_cast<IREDeref*>(expr_));
    if (IRTInt* lIntType = dynamic_cast<IRTInt*>(lDeref->GetType())) {
      if (lDeref->GetType()->GetSize() == INBITS(8)*1) {
        if (lIntType->GetSign() == IRSSigned) {
          AddInst(context_, "ldrsb", *GetRegNT(0), "pre_indexed", *GetRegNT(1));
        } else {
          AddInst(context_, "ldrb", *GetRegNT(0), "post_indexed", *GetRegNT(1));
        } // if
      } else if (lDeref->GetType()->GetSize() == INBITS(8)*2) {
        if (lIntType->GetSign() == IRSSigned) {
          AddInst(context_, "ldrsh", *GetRegNT(0), "pre_indexed", *GetRegNT(1));
        } else {
          AddInst(context_, "ldrh", *GetRegNT(0), "pre_indexed", *GetRegNT(1));
        } // if
      } else if (lDeref->GetType()->GetSize() == INBITS(8)*4) {
        AddInst(context_, "ldr", *GetRegNT(0), "post_indexed", *GetRegNT(1));
      } // if
    } else if (IRTPtr* lPtrType = dynamic_cast<IRTPtr*>(lDeref->GetType())) {
      AddInst(context_, "ldr", *GetRegNT(0), "post_indexed", *GetRegNT(1));
    } else if (lDeref->GetType()->GetSize() == INBITS(8)*4) {
      AddInst(context_, "ldr", *GetRegNT(0), "post_indexed", *GetRegNT(1));
    } else if (IRTBool* lBoolType = dynamic_cast<IRTBool*>(lDeref->GetType())) {
      AddInst(context_, "ldr", *GetRegNT(0), "post_indexed", *GetRegNT(1));
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
    EMDEBUG0();
    return;
  } // ERDeref::OnCover
  virtual ExprRule* Clone() const {return new ERDeref(*this);}
}; // class ERDeref

class ERBAnd : public ExprRule {
public:
  ERBAnd(const ERBAnd& rule_) : ExprRule(rule_) {}
  ERBAnd(HWDescription* hwDesc_) :
    ExprRule("band", hwDesc_)
  {
    SetExprPattern(regNT(0), ireBAnd(regNT(1), regNT(2)));
  } // ERBAnd::ERBAnd
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERBAnd::OnCover", expr_);
    AddInst(context_, "and", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERBAnd::OnCover
  virtual ExprRule* Clone() const {return new ERBAnd(*this);}
}; // class ERBAnd

class ERNot : public ExprRule {
public:
  ERNot(const ERNot& rule_) : ExprRule(rule_) {}
  ERNot(HWDescription* hwDesc_) :
    ExprRule("not", hwDesc_)
  {
    SetExprPattern(condNT(0), ireNot(condNT(1)));
  } // ERNot::ERNot
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERNot::OnCover", expr_);
    *GetCondNT(0) = GetCondNT(1)->GetInverted();
    EMDEBUG0();
    return;
  } // ERNot::OnCover
  virtual ExprRule* Clone() const {return new ERNot(*this);}
}; // class ERNot

class ERBNeg : public ExprRule {
public:
  ERBNeg(const ERBNeg& rule_) : ExprRule(rule_) {}
  ERBNeg(HWDescription* hwDesc_) :
    ExprRule("bneg", hwDesc_)
  {
    SetExprPattern(regNT(0), ireBNeg(regNT(1)));
  } // ERBNeg::ERBNeg
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERBNeg::OnCover", expr_);
    AddInst(context_, "mvn", *GetRegNT(0), *GetRegNT(1));
    EMDEBUG0();
    return;
  } // ERBNeg::OnCover
  virtual ExprRule* Clone() const {return new ERBNeg(*this);}
}; // class ERBNeg

class ERANeg : public ExprRule {
public:
  ERANeg(const ERANeg& rule_) : ExprRule(rule_) {}
  ERANeg(HWDescription* hwDesc_) :
    ExprRule("aneg", hwDesc_)
  {
    SetExprPattern(regNT(0), ireANeg(regNT(1)));
  } // ERANeg::ERANeg
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERANeg::OnCover", expr_);
    //! \todo H Design: we need to consider floating point case of aneg.
    //! \todo H Design: Check if it works for byte/hword cases etc.
    AddInst(context_, "rsb", *GetRegNT(0), *GetRegNT(1), tBigInt(0));
    EMDEBUG0();
    return;
  } // ERANeg::OnCover
  virtual ExprRule* Clone() const {return new ERANeg(*this);}
}; // class ERANeg

class ERMod : public ExprRule {
public:
  ERMod(const ERMod& rule_) : ExprRule(rule_) {}
  ERMod(HWDescription* hwDesc_) :
    ExprRule("mod", hwDesc_)
  {
    SetExprPattern(regNT(0), ireMod(regNT(1), regNT(2)));
    GetRegNT(0)->GetVReg()->SetHWReg(ARMHW::csRegR0);
    GetRegNT(1)->GetVReg()->SetHWReg(ARMHW::csRegR0);
    GetRegNT(2)->GetVReg()->SetHWReg(ARMHW::csRegR1);
  } // ERMod::ERMod
  virtual hReal32 GetCost() {return 1.0;}
  //! \pre This operation should be lowered to a function call, division.
  //!      Therefore, this function should never be called.
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERMod::OnCover", expr_);
    AddInst(context_, "bl", "__modsi3")->AddUses(getUseVRegSet(GetRegNT(1), GetRegNT(2)));
    EMDEBUG0();
    return;
  } // ERMod::OnCover
  virtual ExprRule* Clone() const {return new ERMod(*this);}
}; // class ERMod

class ERRem : public ExprRule {
public:
  ERRem(const ERRem& rule_) : ExprRule(rule_) {}
  ERRem(HWDescription* hwDesc_) :
    ExprRule("rem", hwDesc_)
  {
    SetExprPattern(regNT(0), ireRem(regNT(1), regNT(2)));
    GetRegNT(0)->GetVReg()->SetHWReg(ARMHW::csRegR0);
    GetRegNT(1)->GetVReg()->SetHWReg(ARMHW::csRegR0);
    GetRegNT(2)->GetVReg()->SetHWReg(ARMHW::csRegR1);
  } // ERRem::ERRem
  virtual hReal32 GetCost() {return 1.0;}
  //! \pre This operation should be lowered to a function call, division.
  //!      Therefore, this function should never be called.
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERRem::OnCover", expr_);
    AddInst(context_, "bl", "__modsi3")->AddUses(getUseVRegSet(GetRegNT(1), GetRegNT(2)));
    EMDEBUG0();
    return;
  } // ERRem::OnCover
  virtual ExprRule* Clone() const {return new ERRem(*this);}
}; // class ERRem

class ERBXor : public ExprRule {
public:
  ERBXor(const ERBXor& rule_) : ExprRule(rule_) {}
  ERBXor(HWDescription* hwDesc_) :
    ExprRule("bxOr", hwDesc_)
  {
    SetExprPattern(regNT(0), ireBXor(regNT(1), regNT(2)));
  } // ERBXor::ERBXor
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERBXor::OnCover", expr_);
    AddInst(context_, "eor", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERBXor::OnCover
  virtual ExprRule* Clone() const {return new ERBXor(*this);}
}; // class ERBXor

class ERBOr : public ExprRule {
public:
  ERBOr(const ERBOr& rule_) : ExprRule(rule_) {}
  ERBOr(HWDescription* hwDesc_) :
    ExprRule("bor", hwDesc_)
  {
    SetExprPattern(regNT(0), ireBOr(regNT(1), regNT(2)));
  } // ERBOr::ERBOr
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERBOr::OnCover", expr_);
    AddInst(context_, "orr", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERBOr::OnCover
  virtual ExprRule* Clone() const {return new ERBOr(*this);}
}; // class ERBOr

//! \brief Handle IRENull for debug code generation.
class ERNull : public ExprRule {
public:
  ERNull(const ERNull& rule_) : ExprRule(rule_) {}
  ERNull(HWDescription* hwDesc_) :
    ExprRule("null_expr", hwDesc_)
  {
    SetExprPattern(nullNT(0), ireNull());
  } // ERNull::ERNull
  virtual hReal32 GetCost() {return 1.0;}
  //! \brief Null expression generation does nothing.
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERNull::OnCover", expr_);
    /* LIE */
    EMDEBUG0();
    return;
  } // ERNull::OnCover
  virtual ExprRule* Clone() const {return new ERNull(*this);}
}; // class ERNull

class ERDiv : public ExprRule {
public:
  ERDiv(const ERDiv& rule_) : ExprRule(rule_) {}
  ERDiv(HWDescription* hwDesc_) :
    ExprRule("div", hwDesc_)
  {
    SetExprPattern(regNT(0), ireDiv(regNT(1), regNT(2)));
    GetRegNT(0)->GetVReg()->SetHWReg(ARMHW::csRegR0);
    GetRegNT(1)->GetVReg()->SetHWReg(ARMHW::csRegR0);
    GetRegNT(2)->GetVReg()->SetHWReg(ARMHW::csRegR1);
  } // ERDiv::ERDiv
  virtual hReal32 GetCost() {return 1.0;}
  //! \pre This operation should be lowered to a function call, division.
  //!      Therefore, this function should never be called.
  virtual bool CheckCondition(const IRExpr* expr_) {
    BMDEBUG2("ERDiv::CheckCondition", expr_);
    bool retVal(pred.pIsInt(expr_->GetType()));
    EMDEBUG1(retVal);
    return retVal;
  } // ERDiv::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERDiv::OnCover", expr_);
    IREDiv* lDiv(static_cast<IREDiv*>(expr_));
    if (!pred.pIsUnsignedInt(lDiv->GetLeftExpr()->GetType())) {
      AddInst(context_, "bl", "__divsi3")->AddDefs(*ARMHW::csRegsR0R1)->AddUses(getUseVRegSet(GetRegNT(1), GetRegNT(2)));
    } else {
      AddInst(context_, "bl", "__udivsi3")->AddDefs(*ARMHW::csRegsR0R1)->AddUses(getUseVRegSet(GetRegNT(1), GetRegNT(2)));
    } // if
    EMDEBUG0();
    return;
  } // ERDiv::OnCover
  virtual ExprRule* Clone() const {return new ERDiv(*this);}
}; // class ERDiv

class ERSub : public ExprRule {
public:
  ERSub(const ERSub& rule_) : ExprRule(rule_) {}
  ERSub(HWDescription* hwDesc_) :
    ExprRule("sub", hwDesc_)
  {
    SetExprPattern(regNT(0), ireSub(regNT(1), regNT(2)));
  } // ERSub::ERSub
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERSub::OnCover", expr_);
    AddInst(context_, "sub", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERSub::OnCover
  virtual ExprRule* Clone() const {return new ERSub(*this);}
}; // class ERSub

class ERAddImm : public ExprRule {
public:
  ERAddImm(const ERAddImm& rule_) : ExprRule(rule_) {}
  ERAddImm(HWDescription* hwDesc_) :
    ExprRule("addimm", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAdd(regNT(1), ireIntConst()));
  } // ERAddImm::ERAddImm
  virtual hReal32 GetCost() {return 1.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERAddImm::CheckCondition", matchedExpr_);
    bool retVal(false);
    const IREAdd* irAdd(static_cast<const IREAdd*>(matchedExpr_));
    IREIntConst* irIntConst(static_cast<IREIntConst*>(irAdd->GetRightExpr()));
    if (irIntConst->GetValue() >= 0 && ARMHW::IsImm8r(irIntConst->GetValue()) == true) {
      retVal = true;
    } // if
    EMDEBUG1(retVal);
    return retVal;
  } // ERAddImm::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAddImm::OnCover", expr_);
    IREAdd* irAdd(static_cast<IREAdd*>(expr_));
    IREIntConst* irIntConst(static_cast<IREIntConst*>(irAdd->GetRightExpr()));
    AddInst(context_, "add", *GetRegNT(0), *GetRegNT(1), irIntConst->GetValue());
    EMDEBUG0();
    return;
  } // ERAddImm::OnCover
  virtual ExprRule* Clone() const {return new ERAddImm(*this);}
}; // class ERAddImm

class ERAdd : public ExprRule {
public:
  ERAdd(const ERAdd& rule_) : ExprRule(rule_) {}
  ERAdd(HWDescription* hwDesc_) :
    ExprRule("add", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAdd(regNT(1), regNT(2)));
  } // ERAdd::ERAdd
  virtual hReal32 GetCost() {return 1.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAdd::OnCover", expr_);
    AddInst(context_, "add", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERAdd::OnCover
  virtual ExprRule* Clone() const {return new ERAdd(*this);}
}; // class ERAdd

//! \brief Multiply accumulate rule.
class ERMAC : public ExprRule {
public:
  ERMAC(const ERMAC& rule_) : ExprRule(rule_) {}
  ERMAC(HWDescription* hwDesc_) :
    ExprRule("mac", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAdd(ireMul(regNT(1), regNT(2)), regNT(3)));
  } // ERMAC::ERMAC
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERMAC::OnCover", expr_);
    AddInst(context_, "mla", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2), *GetRegNT(3));
    EMDEBUG0();
    return;
  } // ERMAC::OnCover
  virtual ExprRule* Clone() const {return new ERMAC(*this);}
}; // class ERMAC

class ERLShiftRight : public ExprRule {
public:
  ERLShiftRight(const ERLShiftRight& rule_) : ExprRule(rule_) {}
  ERLShiftRight(HWDescription* hwDesc_) :
    ExprRule("lshiftright", hwDesc_)
  {
    SetExprPattern(regNT(0), ireLShiftRight(regNT(1), regNT(2)));
  } // ERLShiftRight::ERLShiftRight
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERLShiftRight::OnCover", expr_);
    AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1), "lsr", *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERLShiftRight::OnCover
  virtual ExprRule* Clone() const {return new ERLShiftRight(*this);}
}; // class ERLShiftRight

class ERLShiftRightNT : public ExprRule {
public:
  ERLShiftRightNT(const ERLShiftRightNT& rule_) : ExprRule(rule_) {}
  ERLShiftRightNT(HWDescription* hwDesc_) :
    ExprRule("lshiftright", hwDesc_)
  {
    SetExprPattern(shiftOpNT(0), ireLShiftRight(regNT(1), regNT(2)));
  } // ERLShiftRightNT::ERLShiftRightNT
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERLShiftRightNT::OnCover", expr_);
    ASSERTMSG(0, ieStrNotImplemented);
    ShiftOpNT* lResultNT(GetShiftOpNT(0));
    *lResultNT = ShiftOpNT(*GetRegNT(1), *GetRegNT(2), STLSR);
    EMDEBUG0();
    return;
  } // ERLShiftRightNT::OnCover
  virtual ExprRule* Clone() const {return new ERLShiftRightNT(*this);}
}; // class ERLShiftRightNT

class ERAShiftRightNT : public ExprRule {
public:
  ERAShiftRightNT(const ERAShiftRightNT& rule_) : ExprRule(rule_) {}
  ERAShiftRightNT(HWDescription* hwDesc_) :
    ExprRule("ashiftright", hwDesc_)
  {
    SetExprPattern(shiftOpNT(0), ireAShiftRight(regNT(1), regNT(2)));
  } // ERAShiftRightNT::ERAShiftRightNT
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAShiftRightNT::OnCover", expr_);
    ASSERTMSG(0, ieStrNotImplemented);
    ShiftOpNT* lResultNT(GetShiftOpNT(0));
    *lResultNT = ShiftOpNT(*GetRegNT(1), *GetRegNT(2), STASR);
    EMDEBUG0();
    return;
  } // ERAShiftRightNT::OnCover
  virtual ExprRule* Clone() const {return new ERAShiftRightNT(*this);}
}; // class ERAShiftRightNT

class ERAShiftRight : public ExprRule {
public:
  ERAShiftRight(const ERAShiftRight& rule_) : ExprRule(rule_) {}
  ERAShiftRight(HWDescription* hwDesc_) :
    ExprRule("ashiftright", hwDesc_)
  {
    SetExprPattern(regNT(0), ireAShiftRight(regNT(1), regNT(2)));
  } // ERAShiftRight::ERAShiftRight
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERAShiftRight::OnCover", expr_);
    AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1), "asr", *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERAShiftRight::OnCover
  virtual ExprRule* Clone() const {return new ERAShiftRight(*this);}
}; // class ERAShiftRight

//! \brief Converts shift non-terminals in to register non-terminals.
class ERConvShift2Reg : public ExprRule{
  ERConvShift2Reg(const ERConvShift2Reg& rule_) : ExprRule(rule_) {}
  ERConvShift2Reg(HWDescription* hwDesc_) :
    ExprRule("shift2reg", hwDesc_)
  {
    SetExprPattern(regNT(0), shiftOpNT(1));
  } // ERConvShift2Reg::ERConvShift2Reg
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERConvShift2Reg::OnCover", expr_);
    ShiftOpNT* lShiftNT(GetShiftOpNT(1));
    if (!pred.pIsInvalid(lShiftNT->GetShiftReg())) {
      AddInst(context_, "mov", *GetRegNT(0), lShiftNT->GetReg(), lShiftNT->GetShiftType(), lShiftNT->GetShiftReg());
    } else {
      AddInst(context_, "mov", *GetRegNT(0), lShiftNT->GetReg(), lShiftNT->GetShiftType(), lShiftNT->GetShiftAmount());
    } // if
    EMDEBUG0();
    return;
  } // ERConvShift2Reg::OnCover
  virtual ExprRule* Clone() const {return new ERConvShift2Reg(*this);}
}; // class ERConvShift2Reg

class ERShiftLeftNT : public ExprRule {
public:
  ERShiftLeftNT(const ERShiftLeftNT& rule_) : ExprRule(rule_) {}
  ERShiftLeftNT(HWDescription* hwDesc_) :
    ExprRule("shiftleft", hwDesc_)
  {
    SetExprPattern(shiftOpNT(0), ireShiftLeft(regNT(1), regNT(2)));
  } // ERShiftLeftNT::ERShiftLeftNT
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERShiftLeftNT::OnCover", expr_);
    ShiftOpNT* lResultNT(GetShiftOpNT(0));
    *lResultNT = ShiftOpNT(*GetRegNT(1), *GetRegNT(2), STLSL);
    EMDEBUG0();
    return;
  } // ERShiftLeftNT::OnCover
  virtual ExprRule* Clone() const {return new ERShiftLeftNT(*this);}
}; // class ERShiftLeftNT

class ERShiftLeft : public ExprRule {
public:
  ERShiftLeft(const ERShiftLeft& rule_) : ExprRule(rule_) {}
  ERShiftLeft(HWDescription* hwDesc_) :
    ExprRule("shiftleft", hwDesc_)
  {
    SetExprPattern(regNT(0), ireShiftLeft(regNT(1), regNT(2)));
  } // ERShiftLeft::ERShiftLeft
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERShiftLeft::OnCover", expr_);
    AddInst(context_, "mov", *GetRegNT(0), *GetRegNT(1), "lsl", *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERShiftLeft::OnCover
  virtual ExprRule* Clone() const {return new ERShiftLeft(*this);}
}; // class ERShiftLeft

class ERPtrAdd : public ExprRule {
public:
  ERPtrAdd(const ERPtrAdd& rule_) : ExprRule(rule_) {}
  ERPtrAdd(HWDescription* hwDesc_) :
    ExprRule("ptradd", hwDesc_)
  {
    SetExprPattern(regNT(0), irePtrAdd(regNT(1), regNT(2)));
  } // ERPtrAdd::ERPtrAdd
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERPtrAdd::OnCover", expr_);
    AddInst(context_, "add", *GetRegNT(0), *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERPtrAdd::OnCover
  virtual ExprRule* Clone() const {return new ERPtrAdd(*this);}
}; // class ERPtrAdd

class ERCmpImm : public ExprRule {
public:
  ERCmpImm(const ERCmpImm& rule_) : ExprRule(rule_) {}
  ERCmpImm(HWDescription* hwDesc_) :
    ExprRule("cmpimm", hwDesc_)
  {
    SetExprPattern(condNT(0), ireCmp(regNT(1), ireIntConst()));
  } // ERCmpImm::ERCmpImm
  virtual hReal32 GetCost() {return 2.0;}
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {
    BMDEBUG2("ERCmpImm::CheckCondition", matchedExpr_);
    bool retVal(false);
    const IRECmp* lIRCmp(static_cast<const IRECmp*>(matchedExpr_));
    IREIntConst* lIRIntConst(static_cast<IREIntConst*>(lIRCmp->GetRightExpr()));
    if (lIRIntConst->GetValue() >= 0 && ARMHW::IsImm8r(lIRIntConst->GetValue()) == true) {
      retVal = true;
    } // if
    EMDEBUG1(retVal);
    return retVal;
  } // ERCmpImm::CheckCondition
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    IRECmp* lCmpExpr(static_cast<IRECmp*>(expr_));
    IREIntConst* lIRIntConst(static_cast<IREIntConst*>(lCmpExpr->GetRightExpr()));
    ConditionNT* lCond(GetCondNT(0));
    *lCond = ConditionNT(ConditionNT::GetCondition(lCmpExpr));
    AddInst(context_, "cmp", *GetRegNT(1), lIRIntConst->GetValue());
    return;
  } // ERCmpImm::OnCover
  virtual ExprRule* Clone() const {return new ERCmpImm(*this);}
}; // class ERCmpImm

class ERCmp : public ExprRule {
public:
  ERCmp(const ERCmp& rule_) : ExprRule(rule_) {}
  ERCmp(HWDescription* hwDesc_) :
    ExprRule("cmp", hwDesc_)
  {
    SetExprPattern(condNT(0), ireCmp(regNT(1), regNT(2)));
  } // ERCmp::ERCmp
  virtual hReal32 GetCost() {return 2.0;}
  virtual void OnCover(IRExpr* expr_, CGFuncContext& context_) {
    BMDEBUG2("ERCmp::OnCover", expr_);
    IRECmp* lCmpExpr(static_cast<IRECmp*>(expr_));
    ConditionNT* lCond(GetCondNT(0));
    *lCond = ConditionNT(ConditionNT::GetCondition(lCmpExpr));
    AddInst(context_, "cmp", *GetRegNT(1), *GetRegNT(2));
    EMDEBUG0();
    return;
  } // ERCmp::OnCover
  virtual ExprRule* Clone() const {return new ERCmp(*this);}
}; // class ERCmp

} // namespace ARMRules
using namespace ARMRules;

void ARMHW::
OnBBStart(IRBB* bb_, CGFuncContext& context_) {
  BMDEBUG2("ARMHW::OnBBStart", bb_);
  REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  HWDescription::OnBBStart(bb_, context_);
  EMDEBUG0();
  return;
} // ARMHW::OnBBStart

void ARMHW::
InitializeRules() {
  BMDEBUG1("ARMHW::InitializeRules");
  //! \todo M Design: Check the rule names' uniqueness.
  //! \todo M Design: Do a number of rules/compilation speed benchmark by removing the not necessary rules.
  // Statement rules
  AddRule(new SRIf(this));
  AddRule(new SRFCall(this));
  AddRule(new SRFCallIndirect(this));
  AddRule(new SRPCallIndirect(this));
  AddRule(new SRBuiltInCall(this));
  AddRule(new SRPCall(this));
  AddRule(new SREpilog(this));
  AddRule(new SREval(this));
  AddRule(new SRNull(this));
  AddRule(new SRProlog(this));
  AddRule(new SRLabel(this));
  AddRule(new SRJump(this));
  AddRule(new SRReturnVoid(this));
  AddRule(new SRReturn(this));
  AddRule(new SRAssignToRegObj(this));
  AddRule(new SRAssignToStackObj(this));
  AddRule(new SRAssign(this));
  AddRule(new SRAssignToStackObj_dbl(this));
  AddRule(new SRAssignToRegObj_dbl(this));
  AddRule(new SRAssign_dbl(this));
  AddRule(new ERUseOfReg_dbl(this));
  AddRule(new ERDeref_dbl(this));
//  AddRule(new ERAddrOfDeref_dbl(this));
  // Expression rules
  // IRESubscript, IREMember must have been lowered to other IR expressions.
  // IRLAnd, IRLOr are similarly,
  // IREBitWrite, IREReadBitField, should also be lowered.
  // There may not be nested relationals in expression trees.
  AddRule(new ERAddrOf(this));
  AddRule(new ERAddrOf_dbl(this));
  //! \todo M Design: Check if we really benefit from use of?
  AddRule(new ERUseOfReg(this));
  AddRule(new ERDeref(this));
  AddRule(new ERAddrOfDeref(this));
//  AddRule(new ERMAC(this));
  AddRule(new ERCast(this));
  AddRule(new ERCast_dbl(this));
  AddRule(new ERMul(this));
  AddRule(new ERCmp(this));
  AddRule(new ERCmpImm(this));
  AddRule(new ERAdd(this));
  AddRule(new ERAddImm(this));
  AddRule(new ERDiv(this));
  AddRule(new ERPtrAdd(this));
  AddRule(new ERShiftLeft(this));
  AddRule(new ERAShiftRight(this));
  AddRule(new ERLShiftRight(this));
  AddRule(new ERSub(this));
  AddRule(new ERNot(this));
  AddRule(new ERMod(this));
  AddRule(new ERNull(this));
  AddRule(new ERRem(this));
  AddRule(new ERANeg(this));
  AddRule(new ERBNeg(this));
  AddRule(new ERBAnd(this));
  AddRule(new ERBOr(this));
  AddRule(new ERBXor(this));
  AddRule(new ERIntConst(this));
  AddRule(new ERBoolConst(this));
  AddRule(new ERRealConst(this));
  AddRule(new ERStrConst(this));
  AddRule(new ERAddrConst(this));
  {
    SetInstSetDefinition(new InstructionSetDefinition(this, "ARM Instruction Set", new GNUASMDesc));
    InstructionSetDefinition* lISADef(GetISADef());
    RARBuilder lRARBuilder(lISADef);
    lISADef->DefineShift(STLSR, "lsr")->DefineShift(STASR, "asr")->DefineShift(STLSL, "lsl")->
      DefineShift(STROR, "ror")->DefineShift(STROL, "rol")->DefineShift(STRRX, "rrx");
    lISADef->DefineCond(CNTEq, "eq")->DefineCond(CNTNe, "ne")->DefineCond(CNTGe, "ge")->DefineCond(CNTGt, "gt")->
      DefineCond(CNTLe, "le")->DefineCond(CNTLt, "lt")->DefineCond(CNTUGe, "hs")->DefineCond(CNTUGt, "hi")->
      DefineCond(CNTULe, "ls")->DefineCond(CNTULt, "lo");
    lISADef->DefineFlag("shift_flags")->Flags("lsr", "asr", "lsl", "ror", "rol", 0);
    lISADef->DefineFlag("shift_rrx_flag")->Flags("rrx", 0);
    lISADef->DefineFlag("cond_flags")->Flags("", "lo", "ne", "eq", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "al", "hs", 0);
    lISADef->DefineFlag("update_cond_flag")->Flags("", "s", 0);
    lISADef->DefineFlag("ldstm_flags")->Flags("fd", "ed", "fa", "ea", 0);
    lISADef->DefineFlag("plus_minus")->Flags("", "-", "+", 0);
    lISADef->DefineFlag("pre_post_indexed")->Flags("pre_indexed", "post_indexed", 0);
    lISADef->DefineFlag("update_base_flag")->Flags("", "!", 0);
    // Register names used below must be present in the ISA definition HWRegs.
    lISADef->DefineRegs("armregs")->Regs("r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", 
      "r11", "r12", "r13", "r14", "r15", "sp", "fp", "pc", "lr", "ip", 0);
    lISADef->DefineTemplate("scond_op")->AddOpFlag("cond_flags")->AddOpFlag("update_cond_flag")->FormatStr("@0@1");
    lISADef->DefineTemplate("asmimm")->AddOpInt()->FormatStr("=@0");
    lISADef->DefineTemplate("imm8r")->AddOpInt()->FormatStr("#@0");
    lISADef->DefineTemplate("imm8")->AddOpInt()->FormatStr("#@0");
    lISADef->DefineTemplate("imm12")->AddOpInt()->FormatStr("#@0");
    lISADef->DefineTemplate("am2_postindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("imm12")->FormatStr("[@0], @1@2");
    lISADef->DefineTemplate("am2_postindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOpRegUse("armregs", "Rm")->FormatStr("[@0], @1@2");
    lISADef->DefineTemplate("am2_postindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("flex2_shift_imm")->FormatStr("[@0], @1@2");
    lISADef->DefineTemplate("am2_postindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("flex2_shift_rrx")->FormatStr("[@0], @1@2");
    lISADef->DefineTemplate("am2_postindex")->AddOpRegUse("armregs", "Rn")->FormatStr("[@0]");
    lISADef->DefineTemplate("am2_preindex_upbase")->AddOpRegDef("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("imm12")->FormatStr("[@0, @1@2]!");
    lISADef->DefineTemplate("am2_preindex_upbase")->AddOpRegDef("armregs", "Rn")->AddOpFlag("plus_minus")->AddOpRegUse("armregs", "Rm")->FormatStr("[@0, @1@2]!");
    lISADef->DefineTemplate("am2_preindex_upbase")->AddOpRegDef("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("flex2_shift_imm")->FormatStr("[@0, @1@2]!");
    lISADef->DefineTemplate("am2_preindex_upbase")->AddOpRegDef("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("flex2_shift_rrx")->FormatStr("[@0, @1@2]!");
    lISADef->DefineTemplate("am2_preindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("imm12")->FormatStr("[@0, @1@2]");
    lISADef->DefineTemplate("am2_preindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOpRegUse("armregs", "Rm")->FormatStr("[@0, @1@2]");
    lISADef->DefineTemplate("am2_preindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("flex2_shift_imm")->FormatStr("[@0, @1@2]");
    lISADef->DefineTemplate("am2_preindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("flex2_shift_rrx")->FormatStr("[@0, @1@2]");
    lISADef->DefineTemplate("am2")->AddOpFlag("pre_post_indexed", "post_indexed")->AddOp("am2_postindex")->FormatStr("@1");
    lISADef->DefineTemplate("am2")->AddOpFlag("pre_post_indexed", "pre_indexed")->AddOp("am2_preindex")->FormatStr("@1");
    lISADef->DefineTemplate("am2")->AddOpFlag("pre_post_indexed", "pre_indexed")->AddOpFlag("update_base_flag", "!")->
      AddOp("am2_preindex_upbase")->FormatStr("@2");
    lISADef->DefineTemplates("amode2")->AddOp("am2")->AddOpLabel();
    lISADef->DefineTemplate("am3_preindex_upbase")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("imm8")->FormatStr("[@0, @1@2]!");
    lISADef->DefineTemplate("am3_preindex_upbase")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->
      AddOpRegUse("armregs", "Rm")->FormatStr("[@0, @1@2]!");
    lISADef->DefineTemplate("am3_preindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("imm8")->FormatStr("[@0, @1@2]");
    lISADef->DefineTemplate("am3_preindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->
      AddOpRegUse("armregs", "Rm")->FormatStr("[@0, @1@2]");
    lISADef->DefineTemplate("am3_preindex")->AddOpRegUse("armregs", "Rn")->FormatStr("[@0]");
    lISADef->DefineTemplate("am3_postindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOp("imm8")->FormatStr("[@0], @1@2");
    lISADef->DefineTemplate("am3_postindex")->AddOpRegUse("armregs", "Rn")->AddOpFlag("plus_minus")->AddOpRegUse("armregs", "Rm")->FormatStr("[@0], @1@2");
    lISADef->DefineTemplate("amode3")->AddOpFlag("pre_post_indexed", "post_indexed")->AddOp("am3_postindex")->FormatStr("@1");
    lISADef->DefineTemplate("amode3")->AddOpFlag("pre_post_indexed", "pre_indexed")->AddOp("am3_preindex")->FormatStr("@1");
    lISADef->DefineTemplate("amode3")->AddOpFlag("pre_post_indexed", "pre_indexed")->AddOpFlag("update_base_flag", "!")->
      AddOp("am3_preindex_upbase")->FormatStr("@1");
    lISADef->DefineTemplate("shift_imm_0_32")->AddOpInt()->FormatStr("#@0");
    lISADef->DefineTemplate("flex2_shift_reg")->AddOpRegUse("armregs", "Rm")->AddOpFlag("shift_flags")->AddOpRegUse("armregs", "Rs")->FormatStr("@0, @1 @2");
    lISADef->DefineTemplate("flex2_shift_imm")->AddOpRegUse("armregs", "Rm")->AddOpFlag("shift_flags")->AddOp("shift_imm_0_32")->FormatStr("@0, @1 @2");
    lISADef->DefineTemplate("flex2_shift_rrx")->AddOpFlag("shift_rrx_flag", "rrx")->AddOpRegUse("armregs", "Rm")->FormatStr("@0, @1");
    // Place Rm to the end to achieve the longest match manually.
    lISADef->DefineTemplates("flex2")->AddOp("imm8r")->AddOp("flex2_shift_reg")->
      AddOp("flex2_shift_imm")->AddOp("flex2_shift_rrx")->AddOpRegUse("armregs", "Rm");

    lISADef->DefineInstTemplate("1op")->AddOpFlag("cond_flags")->AddOpRegDef("armregs", "Rd")->AddOp("flex2")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("1op_use")->AddOpFlag("cond_flags")->AddOpRegUse("armregs", "Rn")->AddOp("flex2")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("2op")->AddOpFlag("cond_flags")->AddOpRegDef("armregs", "Rd")->AddOpRegUse("armregs", "Rn")->
      AddOp("flex2")->FormatStr("\t@0@1 @2, @3, @4");
    lISADef->DefineInstTemplate("1ops")->AddOp("scond_op")->AddOpRegDef("armregs", "Rd")->AddOp("flex2")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("1ops_use")->AddOp("scond_op")->AddOpRegUse("armregs", "Rn")->AddOp("flex2")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("2ops")->AddOp("scond_op")->AddOpRegDef("armregs", "Rd")->AddOpRegUse("armregs", "Rn")->
      AddOp("flex2")->FormatStr("\t@0@1 @2, @3, @4");
    lISADef->DefineInstTemplate("2opmuls")->AddOp("scond_op")->AddOpRegDef("armregs", "Rd")->AddOpRegUse("armregs", "Rm")->
      AddOpRegUse("armregs", "Rs")->FormatStr("\t@0@1 @2, @3, @4");
    lISADef->DefineInstTemplate("3opmlas")->AddOp("scond_op")->AddOpRegDef("armregs", "Rd")->AddOpRegUse("armregs", "Rm")->
      AddOpRegUse("armregs", "Rs")->AddOpRegUse("armregs", "Rn")->FormatStr("\t@0@1 @2, @3, @4, @5");
    lISADef->DefineInstTemplate("b_ops")->AddOpFlag("cond_flags")->AddOpLabel()->FormatStr("\t@0@1 @2");
    lISADef->DefineInstTemplate("blx_ops")->AddOpLabel()->FormatStr("\t@0 @1");
    lISADef->DefineInstTemplate("label_op")->AddOpLabel()->FormatStr("@1:");
    lISADef->DefineInstTemplate("nop_op")->FormatStr("\t@0");
    lISADef->DefineInstTemplate("blx_ops")->AddOpFlag("cond_flags")->AddOpRegUse("armregs", "Rm")->FormatStr("\t@0@1 @2");
    lISADef->DefineInstTemplate("bx_ops")->AddOpFlag("cond_flags")->AddOpRegUse("armregs", "Rm")->FormatStr("\t@0@1 @2");
    lISADef->DefineInstTemplate("st_am2ops")->AddOpFlag("cond_flags")->AddOpRegUse("armregs", "Rd")->
      AddOp("amode2")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("st_am3ops")->AddOpFlag("cond_flags")->AddOpRegUse("armregs", "Rd")->
      AddOp("amode3")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("stm_op")->AddOpFlag("cond_flags")->AddOpFlag("ldstm_flags")->AddOpFlag("update_base_flag")->
      AddOpRegUse("armregs", "Rn")->AddOpRegSetUse("armregs")->FormatStr("\t@0@1@2 @4@3, {@5}  ");
    lISADef->DefineInstTemplate("ld_am2ops")->AddOpFlag("cond_flags")->AddOpRegDef("armregs", "Rd")->
      AddOp("amode2")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("ld_am2ops_asmimm")->AddOpRegDef("armregs", "Rd")->AddOp("asmimm")->FormatStr("\t@0 @1, @2");
    lISADef->DefineInstTemplate("ld_am3ops")->AddOpFlag("cond_flags")->AddOpRegDef("armregs", "Rd")->
      AddOp("amode3")->FormatStr("\t@0@1 @2, @3");
    lISADef->DefineInstTemplate("ldm_op")->AddOpFlag("cond_flags")->AddOpFlag("ldstm_flags")->AddOpFlag("update_base_flag")->
      AddOpRegDef("armregs", "Rn")->AddOpRegSetUse("armregs")->FormatStr("\t@0@1@2 @4@3, {@5}  ");

    lISADef->DefineInst("add", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("adc", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("sub", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("sbc", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("rsc", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("rsb", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("mul", INBYTES(4))->UseTemplate("2opmuls")->AddRARule(lRARBuilder.GetRARMustNotBeSame("Rd", "Rm", 0));
    lISADef->DefineInst("mla", INBYTES(4))->UseTemplate("3opmlas")->
      AddRARule(lRARBuilder.GetRARMustNotBeSame("Rd", "Rm", 0))->
      AddRARule(lRARBuilder.GetRARMustNotBeSame("Rd", "Rs", 0));
    lISADef->DefineInst("mov", INBYTES(4))->UseTemplate("1ops");
    lISADef->DefineInst("mvn", INBYTES(4))->UseTemplate("1ops");
    lISADef->DefineInst("nop", INBYTES(4))->UseTemplate("nop_op");
    lISADef->DefineInst("label", INBYTES(0))->UseTemplate("label_op");
    lISADef->DefineInst("b", INBYTES(4))->UseTemplate("b_ops");
    lISADef->DefineInst("bx", INBYTES(4))->UseTemplate("bx_ops");
    lISADef->DefineInst("bl", INBYTES(4))->UseTemplate("b_ops")->AddDef("r0");
    lISADef->DefineInst("blx", INBYTES(4))->UseTemplate("blx_ops")->AddDef("r0");
    lISADef->DefineInst("tst", INBYTES(4))->UseTemplate("1op_use");
    lISADef->DefineInst("teq", INBYTES(4))->UseTemplate("1op_use");
    lISADef->DefineInst("cmp", INBYTES(4))->UseTemplate("1ops_use");
    lISADef->DefineInst("and", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("eor", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("bic", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("orr", INBYTES(4))->UseTemplate("2ops");
    lISADef->DefineInst("ldm", INBYTES(4))->UseTemplate("ldm_op");
    lISADef->DefineInst("stm", INBYTES(4))->UseTemplate("stm_op");
    lISADef->DefineInst("ldr", INBYTES(4))->UseTemplate("ld_am2ops");
    lISADef->DefineInst("ldr", INBYTES(-1))->UseTemplate("ld_am2ops_asmimm");
    lISADef->DefineInst("ldrb", INBYTES(4))->UseTemplate("ld_am2ops");
    lISADef->DefineInst("ldrsb", INBYTES(4))->UseTemplate("ld_am3ops");
    lISADef->DefineInst("ldrh", INBYTES(4))->UseTemplate("ld_am3ops");
    lISADef->DefineInst("ldrsh", INBYTES(4))->UseTemplate("ld_am3ops");
    lISADef->DefineInst("ldrd", INBYTES(4))->UseTemplate("ld_am3ops");
    lISADef->DefineInst("str", INBYTES(4))->UseTemplate("st_am2ops");
    lISADef->DefineInst("strb", INBYTES(4))->UseTemplate("st_am2ops");
    lISADef->DefineInst("strh", INBYTES(4))->UseTemplate("st_am3ops");
    lISADef->DefineInst("strd", INBYTES(4))->UseTemplate("st_am3ops");
  } // block
  EMDEBUG0();
  return;
} // ARMHW::InitializeRules

//! \todo H Doc: Make a list of trouble shooting for CG writers: e.g.
//!       - When a new rule class is written remember to add the rule in
//!         InitializeRules.

void ARMHW::
AddHWRegs() {
  BMDEBUG1("ARMHW::AddHWRegs");
  mHWRegs["r0"] = new HWReg("r0");
  mHWRegs["r1"] = new HWReg("r1");
  mHWRegs["r2"] = new HWReg("r2");
  mHWRegs["r3"] = new HWReg("r3");
  mHWRegs["r4"] = new HWReg("r4");
  mHWRegs["r5"] = new HWReg("r5");
  mHWRegs["r6"] = new HWReg("r6");
  mHWRegs["r7"] = new HWReg("r7");
  mHWRegs["r8"] = new HWReg("r8");
  mHWRegs["r9"] = new HWReg("r9");
  mHWRegs["r10"] = new HWReg("r10");
  mHWRegs["r11"] = new HWReg("r11");
  mHWRegs["r12"] = new HWReg("r12");
  mHWRegs["r13"] = new HWReg("r13");
  mHWRegs["r14"] = new HWReg("r14");
  mHWRegs["r15"] = new HWReg("r15");
  mHWRegs["fp"] = GetHWReg("r11");
  mHWRegs["ip"] = GetHWReg("r12");
  mHWRegs["sp"] = GetHWReg("r13");
  mHWRegs["lr"] = GetHWReg("r14");
  mHWRegs["pc"] = GetHWReg("r15");
  csRegArray[0] = csRegR0 = GetHWReg("r0");
  csRegArray[1] = csRegR1 = GetHWReg("r1");
  csRegArray[2] = csRegR2 = GetHWReg("r2");
  csRegArray[3] = csRegR3 = GetHWReg("r3");
  csRegArray[4] = csRegR4 = GetHWReg("r4");
  csRegArray[5] = csRegR5 = GetHWReg("r5");
  csRegArray[6] = csRegR6 = GetHWReg("r6");
  csRegArray[7] = csRegR7 = GetHWReg("r7");
  csRegArray[8] = csRegR8 = GetHWReg("r8");
  csRegArray[9] = csRegR9 = GetHWReg("r9");
  csRegArray[10] = csRegR10 = GetHWReg("r10");
  csRegArray[11] = csRegFP  = GetHWReg("fp"); // r11
  csRegArray[12] = csRegR12 = GetHWReg("r12");
  csRegArray[13] = csRegSP = GetHWReg("sp"); // r13
  csRegArray[14] = csRegLR = GetHWReg("lr"); // r14
  csRegArray[15] = csRegPC = GetHWReg("pc"); // r15
  csRegSetCalleeSave = new RegSet(*csRegR4 | *csRegR5 | *csRegR6 | *csRegR7 |
      *csRegR8 | *csRegR9 | *csRegR10 | *csRegFP | *csRegR12);
  csRegParamSet = new RegSet(*csRegR0 | *csRegR1 | *csRegR2 | *csRegR3);
  csRegsR0R1 = new RegSet(*csRegR0 | *csRegR1);
  csRegAllocSet = new RegSet();
  // Set registers available for register allocation, skip PC and argument registers.
  for (hFUInt32 c(4); c < 15; ++ c) {
    *csRegAllocSet |= csRegArray[c];
  } // for
  EMDEBUG0();
  return;
} // ARMHW::AddHWRegs

ARMHW::
ARMHW(Target* target_) :
  HWDescription("ARM generic", target_)
{
  BMDEBUG1("ARMHW::ARMHW");
  // Expression operators that needs to be lowered at HIR
  IRBuilder& irBuilder(Singleton<IRBuilder>::Obj());
  IRTReal* lFltType(irBuilder.irbtGetReal(*target_->GetRealSingle()));
  IRTReal* lDblType(irBuilder.irbtGetReal(*target_->GetRealDouble()));
  IRTInt* lSIntType(irBuilder.irbtGetInt(*target_->GetIntType(32, 32, IRSSigned)));
  IRTInt* lUIntType(irBuilder.irbtGetInt(*target_->GetIntType(32, 32, IRSUnsigned)));
  IRTInt* lSInt16Type(irBuilder.irbtGetInt(*target_->GetIntType(16, 16, IRSSigned)));
  IRTInt* lUInt16Type(irBuilder.irbtGetInt(*target_->GetIntType(16, 16, IRSUnsigned)));
  IRTInt* lSInt8Type(irBuilder.irbtGetInt(*target_->GetIntType(8, 8, IRSSigned)));
  IRTInt* lUInt8Type(irBuilder.irbtGetInt(*target_->GetIntType(8, 8, IRSUnsigned)));
  vector<IRType*> lDblTypeV2;
  vector<IRType*> lFltTypeV2;
  vector<IRType*> lSIntTypeV2;
  vector<IRType*> lUIntTypeV2;
  vector<IRType*> lFltTypeV1;
  vector<IRType*> lDblTypeV1;
  vector<IRType*> lSIntTypeV1;
  vector<IRType*> lUIntTypeV1;
  lUIntTypeV2.push_back(lUIntType);
  lUIntTypeV2.push_back(lUIntType);
  lSIntTypeV2.push_back(lSIntType);
  lSIntTypeV2.push_back(lSIntType);
  lFltTypeV2.push_back(lFltType);
  lFltTypeV2.push_back(lFltType);
  lDblTypeV2.push_back(lDblType);
  lDblTypeV2.push_back(lDblType);
  lDblTypeV1.push_back(lDblType);
  lFltTypeV1.push_back(lFltType);
  lSIntTypeV1.push_back(lSIntType);
  lUIntTypeV1.push_back(lUIntType);
  // double X(double, double); binary operations, add
  IRTFunc* lDblFuncType2(new IRTFunc(lDblType, lDblTypeV2, false, &Singleton<CallConvGNUEABI>::Obj()));
  // float X(float, float); binary operations, add
  IRTFunc* lFltFuncType2(new IRTFunc(lFltType, lFltTypeV2, false, &Singleton<CallConvGNUEABI>::Obj()));
  // double X(double); unary operations, neg
  IRTFunc* lDblFuncType1(new IRTFunc(lDblType, lDblTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // float X(float); unary operations, neg
  IRTFunc* lFltFuncType1(new IRTFunc(lFltType, lFltTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // sint X(sint, sint)
  IRTFunc* lSIntFuncType2(new IRTFunc(lSIntType, lSIntTypeV2, false, &Singleton<CallConvGNUEABI>::Obj()));
  // uint X(uint, uint)
  IRTFunc* lUIntFuncType2(new IRTFunc(lUIntType, lUIntTypeV2, false, &Singleton<CallConvGNUEABI>::Obj()));
  // float X(double)
  IRTFunc* lDbl2FltFuncType1(new IRTFunc(lFltType, lDblTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // double X(float)
  IRTFunc* lFlt2DblFuncType1(new IRTFunc(lDblType, lFltTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // double X(sint)
  IRTFunc* lSInt2DblFuncType1(new IRTFunc(lDblType, lSIntTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // double X(uint)
  IRTFunc* lUInt2DblFuncType1(new IRTFunc(lDblType, lUIntTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // float X(uint)
  IRTFunc* lUInt2FltFuncType1(new IRTFunc(lFltType, lUIntTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // float X(sint)
  IRTFunc* lSInt2FltFuncType1(new IRTFunc(lFltType, lSIntTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // uint X(float)
  IRTFunc* lFlt2UIntFuncType1(new IRTFunc(lUIntType, lFltTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // uint X(double)
  IRTFunc* lDbl2UIntFuncType1(new IRTFunc(lUIntType, lDblTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // sint X(double)
  IRTFunc* lDbl2SIntFuncType1(new IRTFunc(lSIntType, lDblTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // sint X(float)
  IRTFunc* lFlt2SIntFuncType1(new IRTFunc(lSIntType, lFltTypeV1, false, &Singleton<CallConvGNUEABI>::Obj()));
  // sint X(float, float); comparison operators
  IRTFunc* lFltFltSIntFuncType2(new IRTFunc(lSIntType, lFltTypeV2, false, &Singleton<CallConvGNUEABI>::Obj()));
  // sint X(double, double); comparison operators
  IRTFunc* lDblDblSIntFuncType2(new IRTFunc(lSIntType, lDblTypeV2, false, &Singleton<CallConvGNUEABI>::Obj()));
  // use new AnyNT since expressions may not be reused.
  //! \todo M Design: Find a way to get around reused dbc for below case.
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__truncdfsf2", lDbl2FltFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__extendsfdf2", lFlt2DblFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__floatsidf", lUInt2DblFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__floatsisf", lUInt2FltFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__floatsidf", lSInt2DblFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__floatsisf", lSInt2FltFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__fixunsdfsi", lDbl2UIntFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__fixunssfsi", lFlt2UIntFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__fixdfsi", lDbl2SIntFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireCast(new AnyNT), new IRFunctionDecl("__fixsfsi", lFlt2SIntFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireANeg(new AnyNT), new IRFunctionDecl("__negdf2", lDblFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireANeg(new AnyNT), new IRFunctionDecl("__negsf2", lFltFuncType1, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireMod(new AnyNT, new AnyNT), new IRFunctionDecl("__modsi3", lSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireDiv(new AnyNT, new AnyNT), new IRFunctionDecl("__divsi3", lSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireDiv(new AnyNT, new AnyNT), new IRFunctionDecl("__udivsi3", lUIntFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireDiv(new AnyNT, new AnyNT), new IRFunctionDecl("__divdf3", lDblFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireDiv(new AnyNT, new AnyNT), new IRFunctionDecl("__divsf3", lFltFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireMul(new AnyNT, new AnyNT), new IRFunctionDecl("__muldf3", lDblFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireMul(new AnyNT, new AnyNT), new IRFunctionDecl("__mulsf3", lFltFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireSub(new AnyNT, new AnyNT), new IRFunctionDecl("__subdf3", lDblFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireSub(new AnyNT, new AnyNT), new IRFunctionDecl("__subsf3", lFltFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireAdd(new AnyNT, new AnyNT), new IRFunctionDecl("__adddf3", lDblFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  AddToBeLowered(Rule::ireAdd(new AnyNT, new AnyNT), new IRFunctionDecl("__addsf3", lFltFuncType2, &Invalid<IRModule>::Obj()), consts.cDoNotConvRetVal);
  // float comparisons
  AddToBeLowered(Rule::ireGe(new AnyNT, new AnyNT), new IRFunctionDecl("___gesf2", lFltFltSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireGt(new AnyNT, new AnyNT), new IRFunctionDecl("___gtsf2", lFltFltSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireEq(new AnyNT, new AnyNT), new IRFunctionDecl("___eqsf2", lFltFltSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireNe(new AnyNT, new AnyNT), new IRFunctionDecl("___nesf2", lFltFltSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireLt(new AnyNT, new AnyNT), new IRFunctionDecl("___ltsf2", lFltFltSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireLe(new AnyNT, new AnyNT), new IRFunctionDecl("___lesf2", lFltFltSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  // double comparisons
  AddToBeLowered(Rule::ireGe(new AnyNT, new AnyNT), new IRFunctionDecl("___gedf2", lDblDblSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireGt(new AnyNT, new AnyNT), new IRFunctionDecl("___gtdf2", lDblDblSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireEq(new AnyNT, new AnyNT), new IRFunctionDecl("___eqdf2", lDblDblSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireNe(new AnyNT, new AnyNT), new IRFunctionDecl("___nedf2", lDblDblSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireLt(new AnyNT, new AnyNT), new IRFunctionDecl("___ltdf2", lDblDblSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  AddToBeLowered(Rule::ireLe(new AnyNT, new AnyNT), new IRFunctionDecl("___ledf2", lDblDblSIntFuncType2, &Invalid<IRModule>::Obj()), consts.cConvRetVal);
  // Add casts
  AddInsertCast(make_pair(lSInt16Type, lDblType), lSIntType);
  AddInsertCast(make_pair(lUInt16Type, lDblType), lUIntType);
  AddInsertCast(make_pair(lSInt16Type, lFltType), lSIntType);
  AddInsertCast(make_pair(lUInt16Type, lFltType), lUIntType);
  AddInsertCast(make_pair(lSInt8Type, lDblType), lSIntType);
  AddInsertCast(make_pair(lUInt8Type, lDblType), lUIntType);
  AddInsertCast(make_pair(lSInt8Type, lFltType), lSIntType);
  AddInsertCast(make_pair(lUInt8Type, lFltType), lUIntType);
  // float to ints
  AddInsertCast(make_pair(lDblType, lSInt16Type), lSIntType);
  AddInsertCast(make_pair(lDblType, lUInt16Type), lUIntType);
  AddInsertCast(make_pair(lFltType, lSInt16Type), lSIntType);
  AddInsertCast(make_pair(lFltType, lUInt16Type), lUIntType);
  AddInsertCast(make_pair(lDblType, lSInt8Type), lSIntType);
  AddInsertCast(make_pair(lDblType, lUInt8Type), lUIntType);
  AddInsertCast(make_pair(lFltType, lSInt8Type), lSIntType);
  AddInsertCast(make_pair(lFltType, lUInt8Type), lUIntType);
  EMDEBUG0();
} // ARMHW::ARMHW

bool ARMHW::
IsImm8r(const tBigInt& bigValue_) {
  BDEBUG2("ARMHW::IsImm8r", &bigValue_);
  REQUIREDMSG(bigValue_ >= 0, ieStrParamValuesDBCViolation);
  bool retVal(false);
  hFUInt32 lValue(bigValue_.get_hFUInt32());
  hFUInt32 lMask(0xFF);
  if (bigValue_ == 0) {
    retVal = true;
  } else {
    for (; lMask != 0xF0000000; lMask <<= 2) {
      if (!(lValue & ~lMask)) {
        retVal = true;
        break;
      } // if
    } // for
  } // if
  if (!retVal) {
    if (!(lValue & ~0xC000003FU)) {
      retVal = true;
    } else if (!(lValue & ~0xC000003FU)) {
      retVal = true;
    } else if (!(lValue & ~0xF000000FU)) {
      retVal = true;
    } else if (!(lValue & ~0xFC000003U)) {
      retVal = true;
    } // if
  } // if
  EDEBUG1(retVal);
  return retVal;
} // ARMHW::IsImm8r

void GNUASMDesc::
PutInstShiftCode(ostream& o, ShiftType shiftType_) const {
  switch(shiftType_) {
    case STLSR: o << "lsr"; break;
    case STASR: o << "asr"; break;
    case STLSL: o << "lsl"; break;
    case STROR: o << "ror"; break;
    case STRRX: o << "rrx"; break;
    case STROL: 
      ASSERTMSG(0, ieStrParamValuesDBCViolation << " you should lower ROL into other shift operations");
    default:
      ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // switch
  return;
} // GNUASMDesc::PutInstShiftCode

void GNUASMDesc::
PutInstCondCode(ostream& o, CNTType condType_) const {
  switch(condType_) {
    case CNTEq: o << "eq"; break;
    case CNTNe: o << "ne"; break;
    case CNTGe: o << "ge"; break;
    case CNTGt: o << "lt"; break;
    case CNTLe: o << "le"; break;
    case CNTLt: o << "lt"; break;
    case CNTUGe: o << "cs"; break;
    case CNTUGt: o << "hi"; break;
    case CNTULe: o << "ls"; break;
    case CNTULt: o << "cc"; break;
    default:
      ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // switch
  return;
} // GNUASMDesc::PutInstCondCode

void GNUASMDesc::
PutInstRegSet(ostream& o, const RegSet& regSet_) const {
  const set<Register*>& lRegs(regSet_.GetRegs());
  for (set<Register*>::iterator lIt(lRegs.begin()); lIt != lRegs.end(); ++ lIt) {
    Register* lReg(*lIt);
    if (lIt != lRegs.begin()) {
      o << ", ";
    } // if
    o << lReg->GetName();
  } // for
  return;
} // GNUASMDesc::PutInstRegSet

ASMSection& GNUASMDesc::
PutSpace(ASMSection& o, const tBigInt& sizeInBytes_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutSpace", &sizeInBytes_);
  o.getAsm() << "\t.space " << sizeInBytes_;
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  IncreaseSectionSize(o, sizeInBytes_.get_hFUInt32());
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutSpace

ASMSection& GNUASMDesc::
PutAlign(ASMSection& o, const tBigInt& alignInBytes_) const {
  BMDEBUG2("GNUASMDesc::PutAlign", &alignInBytes_);
  switch (alignInBytes_.get_hFUInt32()) {
    case 1 << 0: o.getAsm() << "\t.align 0"; break; // align : 1 byte
    case 1 << 1: o.getAsm() << "\t.align 1"; break; // align : 2 bytes
    case 1 << 2: o.getAsm() << "\t.align 2"; break; // align : 4 bytes
    case 1 << 3: o.getAsm() << "\t.align 3"; break; // align : 8 bytes
    case 1 << 4: o.getAsm() << "\t.align 4"; break; // align : 16 bytes
    case 1 << 5: o.getAsm() << "\t.align 5"; break; // align : 32 bytes
    case 1 << 6: o.getAsm() << "\t.align 6"; break; // align : 64 bytes
    case 1 << 7: o.getAsm() << "\t.align 7"; break; // align : 128 bytes
    case 1 << 8: o.getAsm() << "\t.align 8"; break; // align : 256 bytes
    case 1 << 9: o.getAsm() << "\t.align 9"; break; // align : 512 bytes
    default:
      // Add more cases as needed.
      ASSERTMSG(0, ieStrNotImplemented);
  } // switch
  //! \todo M Design: We may actually get it right size, unknown size may be
  //!       redundant if 'o' at the beg was not unknown.
  o.SetSizeUnknown();
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutAlign

ASMSection& GNUASMDesc::
PutDirLoc(ASMSection& o, hFUInt32 fileNo_, hFUInt32 lineNo_, hFUInt32 colNo_, DWARF_LNSDir lnsDir_, hFUInt32 value_) const {
  BMDEBUG6("GNUASMDesc::PutDirLoc", fileNo_, lineNo_, colNo_, toStr(lnsDir_), value_);
  o << "\t.loc " << fileNo_ << " " << lineNo_ << " " << colNo_;
/*
  if (lnsDir_ != DLNSD_none) {
    char* lOptionNames[] = {"", "basic_block", "prologue_end", "epilog_begin", "is_stmt", "isa"};
    o.getAsm() << " " << lOptionNames[lnsDir_];
    if (lnsDir_ == DLNSD_isa || lnsDir_ == DLNSD_is_stmt) {
      o.getAsm() << " " << value_;
    } // if
  } // if
*/
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutDirLoc

ASMSection& GNUASMDesc::
PutDirFileLoc(ASMSection& o, hFUInt32 fileNo_, const string& fileName_) const {
  BMDEBUG3("GNUASMDesc::PutDirFileLoc", fileNo_, fileName_);
  o.getAsm() << "\t.file " << fileNo_ << " ";
  util.uEmitString(o, fileName_);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutDirFileLoc

ASMSection& GNUASMDesc::
PutCommentStr(ASMSection& o) const {
  BMDEBUG1("GNUASMDesc::PutCommentStr");
  const hFSInt32 lCommentColNum(40);
  for (hFSInt32 c(o.GetColNum()); c < lCommentColNum; ++ c) {
    o << " ";
  } // for
  o << "@";
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutCommentStr

ASMSection& GNUASMDesc::
PutLabel(ASMSection& o, const string& label_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutLabel", label_);
  o.getAsm() << label_ << ":";
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutLabel

ASMSection& GNUASMDesc::
PutFuncProlog(ASMSection& o, const IRFunction* func_) const {
  BMDEBUG2("GNUASMDesc::PutFuncProlog", func_);
  o << "\t.text" << Endl(o);
  o << "\t.align 2" << Endl(o);
//  o.getAsm() << "\t.global " << func_->GetName().c_str() << endl;
  o << "\t.type" << func_->GetName().c_str() << ", %function" << Endl(o);
  o << "\t" << func_->GetName().c_str() << ": ; {" << Endl(o);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutFuncProlog

//! \todo M Design: Make the assembly as less text output as possible with an
//!       option. This somehow speeds up the assembler.
ASMSection& GNUASMDesc::
PutFuncEpilog(ASMSection& o, const IRFunction* func_) const {
  BMDEBUG2("GNUADMDesc::PutFuncEpilog", func_);
  o << "\t; } // " << func_->GetName().c_str() << Endl(o);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutFuncEpilog

ASMSection& GNUASMDesc::
PutReloc(ASMSection& o, const IRRelocExpr* relocExpr_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutReloc", relocExpr_);
  bool lHandled(false);
  //! \todo M Design: Can we simply use the type size of relocExpr_ for
  //!       incrementing the section size?
  if (pred.pIsAddrConst(relocExpr_) == true) {
    o << "\t.word ";
    IncreaseSectionSize(o, 4);
  } else if (pred.pIsStrConst(relocExpr_) == true) {
    o << "\t.ascii ";
    IncreaseSectionSize(o, static_cast<const IREStrConst*>(relocExpr_)->GetValue().size());
  } else if (isDouble(relocExpr_->GetType()) == true && pred.pIsRealConst(relocExpr_) == true) {
    union {
      hReal64 dword;
      struct {
        hFSInt32 low;
        hFSInt32 high;
      } words;
    } words;
    words.dword = static_cast<const IRERealConst*>(relocExpr_)->GetValue().get_hReal64();
    o << "\t.word " << words.words.high << Endl(o);
    o << "\t.word " << words.words.low;
    IncreaseSectionSize(o, 8);
    if (!comment_.empty()) {
      PutCommentStr(o) << comment_;
    } // if
    lHandled = true;
  } else {
    tBigInt lTypeSize(relocExpr_->GetType()->GetSize()/8);
    IncreaseSectionSize(o, lTypeSize.get_hFUInt32());
    if (lTypeSize == 1) {
      o.getAsm() << "\t.byte ";
    } else if (lTypeSize == 2) {
      o.getAsm() << "\t.hword ";
    } else if (lTypeSize == 4) {
      o.getAsm() << "\t.word ";
    } else if (lTypeSize > 4) {
      ASSERTMSG(0, ieStrInconsistentInternalStructure <<
        " : You should handle this case on your own." );
    } // if
  } // if
  if (!lHandled) {
    RelocExprDisassemblerVisitor lRelocVisitor(o, *this);
    relocExpr_->Accept(lRelocVisitor);
    if (!comment_.empty()) {
      PutCommentStr(o) << comment_;
    } // if
  } // if
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutReloc

ostream& GNUASMDesc::
PutDirFileName(ostream& o, const string& inputFileName_) const {
  BMDEBUG2("GNUASMDesc::PutDirFileName", inputFileName_);
  o << "\t.file \"" << inputFileName_.c_str() << "\"" << endl;
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutDirFileName

ASMSection& GNUASMDesc::
PutAddrConst(ASMSection& o, const tBigAddr& addr_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutAddrConst", &addr_);
  PutData4(o, addr_.get_hFUInt32(), comment_);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutAddrConst

ostream& GNUASMDesc::
PutIdentificationString(ostream& o) const {
  BMDEBUG1("GNUASMDesc::PutIdentificationString");
  //! \todo M Design: Correct the program identification string in the
  //!       assembly output.
  o << "\t.ident \"bcc compiler version 0.0\"" << endl;
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutIdentificationString

string GNUASMDesc::
GetLocalLabel(const string& prefix_, hFUInt32 labelId_) const {
  char lBuf[32];
  sprintf(lBuf, ".L%s%d", prefix_.c_str(), labelId_);
  return lBuf;
} // GNUASMDesc::GetLocalLabel

ASMSection& GNUASMDesc::
PutData1(ASMSection& o, hFUInt8 data_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutData1", data_);
  o.getAsm() << "\t.byte " << data_;
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  IncreaseSectionSize(o, 1);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutData1

ASMSection& GNUASMDesc::
PutData2(ASMSection& o, hFUInt16 data_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutData2", data_);
  o.getAsm() << "\t.2byte " << data_;
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  IncreaseSectionSize(o, 2);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutData2

ASMSection& GNUASMDesc::
PutDataString(ASMSection& o, const string& data_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutDataString", data_);
  o.getAsm() << "\t.ascii ";
  IncreaseSectionSize(o, data_.size()+1);
  util.uEmitString(o, data_);
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutDataString

ASMSection& GNUASMDesc::
PutData4(ASMSection& o, hFUInt32 data_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutData4", data_);
  o.getAsm() << "\t.4byte " << data_;
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  IncreaseSectionSize(o, 4);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutData4

ASMSection& GNUASMDesc::
PutRelocAddr(ASMSection& o, const string& address_, const string& comment_) const {
  BMDEBUG2("GNUASMDesc::PutRelocAddr", address_);
  o.getAsm() << "\t.4byte " << address_;
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  IncreaseSectionSize(o, 4);
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutRelocAddr

ASMSection& GNUASMDesc::
PutRelocAddrDiff(ASMSection& o, const string& addressHigh_, const string& addressLow_, const string& comment_) const {
  BMDEBUG3("GNUASMDesc::PutData4", addressHigh_, addressLow_);
  o.getAsm() << "\t.4byte " << addressHigh_ << "-" << addressLow_;
  if (!comment_.empty()) {
    PutCommentStr(o) << comment_;
  } // if
  IncreaseSectionSize(o, 4);
  EMDEBUG0();
  return o;
} // ASMDesc::PutRelocAddrDiff

ostream& GNUASMDesc::
PutSection(ostream& o, AsmSectionType sectionType_, const string& customName_) const {
  BMDEBUG1("GNUASMDesc::PutSection");
  if (sectionType_ & ASTCustom) {
    o << "\t.section " << customName_;
  } else {
    switch (sectionType_) {
      case ASTText: o << "\t.text"; break;
      case ASTBSS: o << "\t.bss"; break;
      case ASTData: o << "\t.data"; break;
      case ASTCustom:
        ASSERTMSG(0, ieStrPCShouldNotReachHere);
        break;
      case ASTConstData:
      case ASTInvalid:
      default:
        ASSERTMSG(0, ieStrNotImplemented);
    } // switch
  } // if
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutSection

ASMSection& GNUASMDesc::
PutGlobal(ASMSection& o, const IROGlobal* globalObj_) const {
  BMDEBUG2("GNUASSMDesc::PutGlobal", globalObj_);
  if (globalObj_->GetType()->GetSize() != 0) {
    o.getAsm() << "\t.comm " << globalObj_->GetName().c_str() << "," <<
      globalObj_->GetType()->GetSize() << "," << globalObj_->GetType()->GetAlignment();
    IncreaseSectionSize(o, globalObj_->GetType()->GetSize().get_hFUInt32());
    //! \todo M Design: Take into account the alignments for the size.
    o.SetSizeUnknown(); // Because of alignment.
  } // if
  EMDEBUG0();
  return o;
} // GNUASMDesc::PutGlobal

void CallConvGNUEABI::
processWriteBitField(IRSAssign* assign_) const {
  // We need to read the bits (except if the size and alignment is multiples
  // of 8 bits). Depending on alignment we may need to read two words. This is
  // similar to bit-fields reading.
  IRBuilder irBuilder(extr.eGetFunc(assign_), consts.cIRBDoOptims);
  IRBuilderHelper* irhBuilder(irBuilder.GetHelper());
  IREMember* lMember(static_cast<IREMember*>(static_cast<IREAddrOf*>(assign_->GetLHS())->GetExpr()));
  IRExpr* lBasePtr(lMember->GetBase());
  IROBitField* lBitField(static_cast<IROBitField*>(lMember->GetMember()));
  SrcLoc lSrcLoc(extr.eGetSrcLoc(assign_));
  ASSERTMSG(lBitField->GetBitSize() <= 32, ieStrNotImplemented);
  // First load the RHS of assign_ in to a word sized variable, lRHSTmp.
  IRTInt* lRHSType(dynamic_cast<IRTInt*>(assign_->GetRHS()->GetType()));
  ASSERTMSG(lRHSType != 0, ieStrNotImplemented);
  IROTmp* lRHSTmp(irBuilder.irboTmp(irBuilder.irbtGetInt(
    *Target::GetTarget()->GetIntType(32, 0, lRHSType->GetSign())), "bfwrhs"));
  /* LIE */ irhBuilder->irbsAssign(lRHSTmp, assign_->GetRHS()->GetRecCopyExpr(), ICStmt(assign_, ICBefore), lSrcLoc);
  // Read the words, below codes are common to read bit-fields.
  //! \todo M Optim: Consider the case 8-16-32 bits wide aligned bit-fields, no need to read.
  IROTmp* lReadWord0(irBuilder.irboTmp(irBuilder.irbtGetInt(
    *Target::GetTarget()->GetIntType(32, 0, lBitField->GetSign())), "bfwread"));
  IRTInt* lWordType(irBuilder.irbtGetInt(*Target::GetTarget()->GetIntType(32, 0, IRSUnsigned)));
  hFSInt32 lBS(lBitField->GetBitSize().get_hFUInt32()); // Bit-field Size
  hFSInt32 lBO(lBitField->GetBitOffset()); // Bit-field bit offset
  hFSInt32 lAB(((lBitField->GetOffset() / 8) % 4) * 8); // Align bytes in bits
  hFSInt32 lBSW0(32 - (lAB + lBO)); // Bit-field size in the first word.
  hFSInt32 lBSW1(lBS - lBSW0); // Bit-field size in the second word.
  // lByteOffsetInt is aligned offset of bit-field offset.
  tBigInt lByteOffsetInt(4 * (lBitField->GetOffset() / 32));
  IREPtrAdd* lPtrAdd(irBuilder.irbePtrAdd(lBasePtr->GetRecCopyExpr(),
    irBuilder.irbeIntConst(lWordType, lByteOffsetInt)));
  IRSAssign* lReadAssign(irhBuilder->irbsAssign(lReadWord0,
    irBuilder.irbeDeref(irBuilder.irbeCast(irBuilder.irbtGetPtr(lWordType), lPtrAdd)),
      ICStmt(assign_, ICBefore), lSrcLoc));
  // Consider 1 word and 2 word required cases separately.
  if (lAB == 0 && lBO == 0 && lBS == 32) {
    /* LIE */ irBuilder.irbsAssign(irBuilder.irbeAddrOf(irBuilder.irbeDeref(irBuilder.irbeCast(
      irBuilder.irbtGetPtr(lWordType), lPtrAdd->GetRecCopyExpr()))), irhBuilder->irbeUseOf(lReadWord0),
        ICStmt(assign_, ICBefore), lSrcLoc);
  } else if (lAB + lBO + lBS <= 32) {
    // 1 word case, we need to read 1 word.
    // First clear the bits of read word that corresponds to the bit-field bits.
    hUInt32 lMask(0);
    { // Create the mask.
      if (lAB + lBO) {
        lMask = util.uGetMask_32(0, lAB + lBO - 1);
      } // if
      if (lAB + lBO + lBS < 32) {
        lMask |= util.uGetMask_32(lAB + lBO + lBS, 31);
      } // if
      ASSERTMSG(lMask != 0, ieStrInconsistentInternalStructure);
    } // block
    IRExpr* lBIC(irBuilder.irbeBAnd(irhBuilder->irbeUseOf(lReadWord0),
      irBuilder.irbeIntConst(lWordType, lMask)));
    IRExpr* lLeftShift(0);
    // Depending on the sign of RHS of assign_ we may need to clear high bits.
    if (pred.pIsUnsigned(lRHSType) == true) {
      // upper bits are already zero.
      lLeftShift = irBuilder.irbeShiftLeft(irhBuilder->irbeUseOf(lRHSTmp),
        irBuilder.irbeIntConst(lWordType, lAB + lBO));
    } else {
      // upper bits might be one, they must be cleared to be sure.
      lLeftShift = irBuilder.irbeLShiftRight(
        irBuilder.irbeShiftLeft(
          irhBuilder->irbeUseOf(lRHSTmp),
          irBuilder.irbeIntConst(lWordType, 31-lBS)),
        irBuilder.irbeIntConst(lWordType, 31-(lAB+lBO+lBS)));
    } // if
    IRExpr* lRHS(irBuilder.irbeBOr(lLeftShift, lBIC));
    // lPtrAdd is of type struct ptr, we need to cast it to the lhs type of
    // assign and add addr of and deref on it.
    /* LIE */ irBuilder.irbsAssign(irBuilder.irbeAddrOf(irBuilder.irbeDeref(irBuilder.irbeCast(
      irBuilder.irbtGetPtr(lWordType), lPtrAdd->GetRecCopyExpr()))),
        irBuilder.irbeCast(lWordType, lRHS), ICStmt(assign_, ICBefore), lSrcLoc);
  } else {
    // Two words need to be handled.
    { // Handle the first read word.
      // simply left shift the RHS.
      hUInt32 lMaskW0(~util.uGetMask_32(32-lBSW0, 31));
      IRExpr* lBIC(irBuilder.irbeBAnd(irhBuilder->irbeUseOf(lReadWord0),
        irBuilder.irbeIntConst(lWordType, lMaskW0)));
      IRExpr* lShiftLeft(irBuilder.irbeShiftLeft(irhBuilder->irbeUseOf(lRHSTmp),
        irBuilder.irbeIntConst(lWordType, 32 - lBSW0)));
      IRExpr* lRHS(irBuilder.irbeBOr(lShiftLeft, lBIC));
      /* LIE */ irBuilder.irbsAssign(irBuilder.irbeAddrOf(irBuilder.irbeDeref(irBuilder.irbeCast(
        irBuilder.irbtGetPtr(lWordType), lPtrAdd->GetRecCopyExpr()))),
          irBuilder.irbeCast(lWordType, lRHS), ICStmt(assign_, ICBefore), lSrcLoc);
    } // block
    { // Handle the second word.
      IROTmp* lReadWord1(irBuilder.irboTmp(lWordType, "bfwhword"));
      hUInt32 lMaskW1(~util.uGetMask_32(0, lBSW1-1));
      IRExpr* lBIC(irBuilder.irbeBAnd(irhBuilder->irbeUseOf(lReadWord1),
        irBuilder.irbeIntConst(lWordType, lMaskW1)));
      /* LIE */ irhBuilder->irbsAssign(lReadWord1,
        irBuilder.irbeDeref(irBuilder.irbeCast(irBuilder.irbtGetPtr(lWordType),
          irBuilder.irbePtrAdd(lBasePtr->GetRecCopyExpr(), irBuilder.irbeIntConst(lWordType, lByteOffsetInt+4)))),
          ICStmt(assign_, ICBefore), lSrcLoc);
      IRExpr* lShiftLeft(0);
      // Depending on the sign of RHS of assign_ we may need to clear high bits.
      if (pred.pIsUnsigned(lRHSType) == true) {
        // upper bits are already zero.
        lShiftLeft = irBuilder.irbeShiftLeft(irhBuilder->irbeUseOf(lRHSTmp),
          irBuilder.irbeIntConst(lWordType, lBSW0));
      } else {
        // upper bits might be one, they must be cleared to be sure.
        lShiftLeft = irBuilder.irbeLShiftRight(
            irBuilder.irbeShiftLeft(
              irhBuilder->irbeUseOf(lRHSTmp),
              irBuilder.irbeIntConst(lWordType, 31-lBS)),
            irBuilder.irbeIntConst(lWordType, 31-lBSW1));
      } // if
      IRExpr* lRHS(irBuilder.irbeBOr(lShiftLeft, lBIC));
      /* LIE */ irBuilder.irbsAssign(irBuilder.irbeAddrOf(irBuilder.irbeDeref(irBuilder.irbeCast(
        irBuilder.irbtGetPtr(lWordType), irBuilder.irbePtrAdd(lBasePtr->GetRecCopyExpr(),
          irBuilder.irbeIntConst(lWordType, lByteOffsetInt+4))))),
            irBuilder.irbeCast(lWordType, lRHS), ICStmt(assign_, ICBefore), lSrcLoc);
    } // block
  } // if
  irBuilder.irbRemoveStmt(assign_);
} // CallConvGNUEABI::processWriteBitField

void CallConvGNUEABI::
processReadBitField(IREDeref* deref_) const {
  // deref_ => deref(addrof(iremember))
  // Read alignment is important, first we will read a word that is aligned
  // therefore we may need to read two words.
  IRBuilder irBuilder(extr.eGetFunc(deref_), consts.cIRBDoOptims);
  IRBuilderHelper* irhBuilder(irBuilder.GetHelper());
  IREMember* lMember(static_cast<IREMember*>(static_cast<IREAddrOf*>(deref_->GetExpr())->GetExpr()));
  IRExpr* lBasePtr(lMember->GetBase());
  IROBitField* lBitField(static_cast<IROBitField*>(lMember->GetMember()));
  // Currently only 32 bit wide bit fields are supported. Use 32 bits
  // long word for the read value.
  ASSERTMSG(lBitField->GetBitSize() <= 32, ieStrNotImplemented);
  IRStmt* lCurrStmt(extr.eGetStmt(deref_));
  // As result use a temporary variable having the same sign as bit-field and at least
  // as wide as the bit-field. Since optimizations may schedule the instructions, create
  // a new result variable for every read operation.
  IROTmp* lResultTmp(irBuilder.irboTmp(irBuilder.irbtGetInt(
    *Target::GetTarget()->GetIntType(32, 0, lBitField->GetSign())), "bfread"));
  IRTInt* lWordType(irBuilder.irbtGetInt(*Target::GetTarget()->GetIntType(32, 0, IRSUnsigned)));
  //! \todo M CodingStyle: You may use very short variable names if they belong to formulas.
  hFSInt32 lBS(lBitField->GetBitSize().get_hFUInt32()); // Bit-field Size
  hFSInt32 lBO(lBitField->GetBitOffset()); // Bit-field bit offset
  hFSInt32 lAB(((lBitField->GetOffset() / 8) % 4) * 8); // Align bytes in bits
  hFSInt32 lBSW0(32 - (lAB + lBO)); // Bit-field size in the first word.
  hFSInt32 lBSW1(lBS - lBSW0); // Bit-field size in the second word.
  // lByteOffsetInt is aligned offset of bit-field offset.
  tBigInt lByteOffsetInt(4 * (lBitField->GetOffset() / 32));
  IREPtrAdd* lPtrAdd(irBuilder.irbePtrAdd(lBasePtr->GetRecCopyExpr(), irBuilder.irbeIntConst(lWordType, lByteOffsetInt)));
  IRSAssign* lReadAssign(irhBuilder->irbsAssign(lResultTmp,
    irBuilder.irbeDeref(irBuilder.irbeCast(irBuilder.irbtGetPtr(lWordType), lPtrAdd)),
      ICStmt(lCurrStmt, ICBefore), SrcLoc()));
  IRSAssign* lResAssign(0);
  // Cases:
  // 0- Reading one word is sufficient.
  // 0.0- shift left and right (logical or arithmetic depending on bit-field sign) the read
  //      word. Assign the word to the result.
  // 0.1- When size is 32 bits long and we just read the whole word do nothing.
  // 1- Two word reads are required.
  // 1.0- logical right shift the first word.
  // 1.1- logical left shift, right shift (according to bit-field sign) the
  //      second word and ORR with the first word.
  if (lAB == 0 && lBO == 0 && lBS == 32) {
    // LIE: Case 0.1, no need to do anything.
  } else if (lAB + lBO + lBS <= 32) {
    // Case 0.
    IRExpr* lRHS(0);
    IRExpr* lShiftLeft(0);
    if (32 - (lBO + lBS + lAB) > 0) {
      lShiftLeft = irBuilder.irbeShiftLeft(irhBuilder->irbeUseOf(lResultTmp),
        irBuilder.irbeIntConst(lWordType, 32 - (lBO + lBS + lAB)));
    } else {
      lShiftLeft = irhBuilder->irbeUseOf(lResultTmp);
    } // if
    if (lBitField->GetSign() == IRSUnsigned) {
      lRHS = irBuilder.irbeLShiftRight(lShiftLeft, irBuilder.irbeIntConst(lWordType, 32 - lBS));
    } else {
      lRHS = irBuilder.irbeAShiftRight(lShiftLeft, irBuilder.irbeIntConst(lWordType, 32 - lBS));
    } // if
    /* LIE */ irhBuilder->irbsAssign(lResultTmp, lRHS, ICStmt(lCurrStmt, ICBefore), SrcLoc());
  } else {
    // Case 1.
    IROTmp* lReadWord1(irBuilder.irboTmp(lWordType, "bfrhword"));
    /* LIE */ irhBuilder->irbsAssign(lReadWord1,
      irBuilder.irbeDeref(irBuilder.irbeCast(irBuilder.irbtGetPtr(lWordType),
        irBuilder.irbePtrAdd(lBasePtr->GetRecCopyExpr(), irBuilder.irbeIntConst(lWordType, lByteOffsetInt+4)))),
          ICStmt(lCurrStmt, ICBefore), SrcLoc());
    // simply right shift the first word.
    /* LIE */ irhBuilder->irbsAssign(lResultTmp, irBuilder.irbeLShiftRight(irhBuilder->irbeUseOf(lResultTmp),
      irBuilder.irbeIntConst(lWordType, 32 - lBSW0)), ICStmt(lCurrStmt, ICBefore), SrcLoc());
    // shift left the high word so that msb is msb of bit-field,
    // then shift right according to the sign of bit-field, then ORR the
    // result with previously patched word.
    IRExpr* lRHS(0);
    IRExpr* lShiftLeft(irBuilder.irbeShiftLeft(irhBuilder->irbeUseOf(lReadWord1),
      irBuilder.irbeIntConst(lWordType, 32 - lBSW1)));
    if (lBitField->GetSign() == IRSUnsigned) {
      lRHS = irBuilder.irbeLShiftRight(lShiftLeft, irBuilder.irbeIntConst(lWordType, 32 - lBS));
    } else {
      lRHS = irBuilder.irbeAShiftRight(lShiftLeft, irBuilder.irbeIntConst(lWordType, 32 - lBS));
    } // if
    /* LIE */ irhBuilder->irbsAssign(lResultTmp,
      irBuilder.irbeBOr(lRHS, irhBuilder->irbeUseOf(lResultTmp)), ICStmt(lCurrStmt, ICBefore), SrcLoc());
  } // if
  deref_->ReplaceWith(irhBuilder->irbeUseOf(lResultTmp));
} // CallConvGNUEABI::processReadBitField

void CallConvGNUEABI::
processReturn(IRSReturn* returnStmt_, CGContext* context_) const {
  BMDEBUG2("CallConvGNUEABI::processReturn", returnStmt_);
  REQUIREDMSG(returnStmt_ != 0, ieStrNonNullParam);
  if (!pred.pIsNullExpr(returnStmt_->GetExpr())) {
    SrcLoc lSrcLoc(extr.eGetSrcLoc(returnStmt_));
    IRFunction* lFunc(extr.eGetFunc(returnStmt_));
    IRType* lRetType(extr.eGetRetType(lFunc));
    IRBuilder irBuilder(lFunc, consts.cIRBDoOptims);
    if (isDouble(lRetType) == true) {
      // Create the $ret variable.
      IROLocal* lRetObj(irBuilder.irboLocal("$ret", lRetType));
      VirtRegDouble* lVReg(new VirtRegDouble(ARMHW::csRegR0, ARMHW::csRegR1));
      lRetObj->SetAddress(AddrReg(lVReg));
      irBuilder.irbsAssign(irBuilder.irbeAddrOf(lRetObj), returnStmt_->GetExpr()->GetRecCopyExpr(),
        ICStmt(returnStmt_, ICBefore), lSrcLoc);
    } else if (pred.pIsFundamentalType(lRetType) == true) {
      // Create the $ret variable.
      IROLocal* lRetObj(irBuilder.irboLocal("$ret", lRetType));
      lRetObj->SetAddress(AddrReg(ARMHW::csRegR0));
      irBuilder.irbsAssign(irBuilder.irbeAddrOf(lRetObj), returnStmt_->GetExpr()->GetRecCopyExpr(),
        ICStmt(returnStmt_, ICBefore), lSrcLoc);
    } else if (IRTStruct* lStrRetType = dynamic_cast<IRTStruct*>(lRetType)) {
      if (lStrRetType->GetSize() == 8*4) {
        ASSERTMSG(0, ieStrNotImplemented);
      } else {
        const vector<IROParameter*>& lParams(lFunc->GetDeclaration()->GetParams());
        // We need to store the return value in the address pointed by last argument.
        IRExpr* lLength(irBuilder.irbeIntConst(Target::GetTarget()->Get_size_t(), lStrRetType->GetSize()/8));
        IRExpr* lDst(irBuilder.GetHelper()->irbeUseOf(lParams.back()));
        IRExpr* lSrc(irBuilder.irbeAddrOf(returnStmt_->GetExpr()->GetRecCopyExpr()));
        IRSCall* lMemcpy(irBuilder.irbsBIPCall(new IRExprList(lDst, lSrc, lLength), IRBIRmemcpy,
          ICStmt(returnStmt_, ICBefore), lSrcLoc));
        processCall(lMemcpy, context_);
        returnStmt_->GetExpr()->ReplaceWith(irBuilder.irbeNull());
      } // if
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
      NOTIMPLEMENTED(M);
    } // if
  } else {
    // Do nothing for returns of void functions.
    /* LIE */
  } // if
  EMDEBUG0();
  return;
} // CallConvGNUEABI::processReturn

void CallConvGNUEABI::
processCall(IRSCall* callStmt_, CGContext* context_) const {
  BMDEBUG2("CallConvGNUEABI::processCall", callStmt_);
  REQUIREDMSG(callStmt_ != 0, ieStrNonNullParam);
  IRFunction* lFunc(extr.eGetFunc(callStmt_));
  ARMHW* lHWDesc(static_cast<ARMHW*>(context_->GetHWDesc()));
  IRBuilder irBuilder(lFunc, consts.cIRBDoOptims);
  SrcLoc lSrcLoc(extr.eGetSrcLoc(callStmt_));
  PDEBUG("CCGNUEABI", "detail", "Processing call in function " << refcxt(lFunc) << lFunc->GetName() <<
    " for call statement " << refcxt(callStmt_) << extr.eGetName(callStmt_));
  // Process the return value first since struct returning functions may need
  // to add an extra argument.
  if (pred.pIsFCall(callStmt_) == true) { // Get the return value
    PDEBUG("CCGNUEABI", "detail", "Call statement return value is being processed.");
    IRType* lRetType(extr.eGetRetType(callStmt_));
    if (isDouble(lRetType) == true) {
      PDEBUG("CCGNUEABI", "detail", "Call statement return value is double precision real type.");
      // In this case return value is in r0, r1.
      IROTmp* lReturnedValue(irBuilder.irboTmp(lRetType,
        AddrReg(new VirtRegDouble(ARMHW::csRegR0, ARMHW::csRegR1)), "ccret"));
      IRObject* lRetInObj(callStmt_->GetReturnIn());
      // reset the address of return in object to a virtual register.
      lRetInObj->SetAddress(AddrReg(new VirtRegDouble()));
      irBuilder.irbsAssign(irBuilder.irbeAddrOf(lRetInObj), irBuilder.GetHelper()->irbeUseOf(lReturnedValue),
        ICStmt(callStmt_, ICAfter), lSrcLoc);
    } else if (pred.pIsFundamentalType(lRetType) == true) {
      PDEBUG("CCGNUEABI", "detail", "Call statement return value is fundamental type.");
      // In this case return value is in r0.
      IROTmp* lReturnedValue(irBuilder.irboTmp(lRetType, AddrReg(ARMHW::csRegR0), "ccret"));
      IRObject* lRetInObj(callStmt_->GetReturnIn());
      // reset the address of return in object to a virtual register.
      lRetInObj->SetAddress(AddrReg(new VirtReg()));
      irBuilder.irbsAssign(irBuilder.irbeAddrOf(lRetInObj), irBuilder.GetHelper()->irbeUseOf(lReturnedValue),
        ICStmt(callStmt_, ICAfter), lSrcLoc);
    } else if (pred.pIsVoid(lRetType) == true) {
      PDEBUG("CCGNUEABI", "detail", "Call statement return value is void.");
      /* LIE */
    } else if (IRTStruct* lStrRetType = dynamic_cast<IRTStruct*>(lRetType)) {
      //! \todo make it in bytes
      if (lStrRetType->GetSize() == 8*4) {
        // must be returned in r0.
        REQUIREDMSG(0, ieStrNotImplemented);
      } else {
        // Must be returned as an extra argument.
        IRObject* lRetInObj(callStmt_->GetReturnIn());
        callStmt_->AddArgument(irBuilder.irbeAddrOf(lRetInObj));
      } // if
    } else {
      REQUIREDMSG(0, ieStrNotImplemented);
      NOTIMPLEMENTED(M);
    } // if
  } // block
  { // Process arguments
    vector<IRExpr*> lArgs;
    IRExprList* lArgList(callStmt_->GetArgs());
    lArgList->GetChildren(lArgs);
    hFUInt32 lRegIndex(0);
    for (hFUInt32 c(0); c < lArgs.size(); ++ c, ++ lRegIndex) {
      PDEBUG("CCGNUEABI", "detail", "Call statement argument " << c << " is being processed: " << progcxt(lArgs[c]));
      if (isDouble(lArgs[c]->GetType()) == true) {
        PDEBUG("CCGNUEABI", "detail", "Argument is double precision real type.");
        IROTmp* lArgObj(irBuilder.irboTmp(lArgs[c]->GetType(),
          AddrReg(new VirtRegDouble(ARMHW::csRegArray[lRegIndex], ARMHW::csRegArray[lRegIndex+1])), "ccarg"));
        ++ lRegIndex;
        irBuilder.irbsAssign(irBuilder.irbeAddrOf(lArgObj),
          lArgs[c]->GetRecCopyExpr(), ICStmt(callStmt_, ICBefore), lSrcLoc);
        lArgList->ReplaceChild(lArgs[c], irBuilder.GetHelper()->irbeUseOf(lArgObj));
      } else if (pred.pIsFundamentalType(lArgs[c]->GetType()) == true) {
        PDEBUG("CCGNUEABI", "detail", "Argument is fundamental type.");
        IROTmp* lArgObj(irBuilder.irboTmp(lArgs[c]->GetType(), AddrReg(new VirtReg(ARMHW::csRegArray[lRegIndex])), "ccarg"));
        //! \todo M Design: Perhaps it is better to use typed temporary
        //!       objects with stack reg assigned, now we might need to manage
        //!       type conversions.
        irBuilder.irbsAssign(irBuilder.irbeAddrOf(lArgObj),
          lArgs[c]->GetRecCopyExpr(), ICStmt(callStmt_, ICBefore), lSrcLoc);
        lArgList->ReplaceChild(lArgs[c], irBuilder.GetHelper()->irbeUseOf(lArgObj));
        // argList->ReplaceChild(lArgs[c], irBuilder.irbeNull());
      } else {
        REQUIREDMSG(0, ieStrNotImplemented << " : arg type = " << progcxt(lArgs[c]->GetType()));
        NOTIMPLEMENTED(M);
      } // if
    } // for
  } // block
  EMDEBUG0();
  return;
} // CallConvGNUEABI::processCall

//! \todo M Design: Would it be nice to have artificial or compiler generated
//!       flag for IR items. This may ease the debug information generation,
//!       which should mostly skip artificial items.
void CallConvGNUEABI::
processParams(IRFunction* func_, CGContext* context_) const {
  BMDEBUG2("CallConvGNUEABI::processParams", func_);
  REQUIREDMSG(func_ != 0, ieStrNonNullParam);
  IRBuilder irBuilder(func_, consts.cIRBDoOptims);
  { // First check if we need to modify the function signature, which is the
    // case for struct returning functions.
    IRType* lRetType(extr.eGetRetType(func_));
    if (IRTStruct* lStrRetType = dynamic_cast<IRTStruct*>(lRetType)) {
      if (lStrRetType->GetSize() > 8*4) {
        /* LIE */ irBuilder.irboParameter("$ccstrretparam", irBuilder.irbtGetPtr(lStrRetType));
      } // if
    } // if
  } // block
  vector<IROParameter*> lParams(func_->GetDeclaration()->GetParams());
  // All the parameters are frame-pointer based.
  // | p0    | p1     | p2     | local0 | local1   | ... |
  // |       |        |        | ^ fp   |
  // | fp-12 | fp - 8 | fp - 4 | fp + 0 | fp + ... | ... |
  ASSERTMSG(lParams.size() <= 4, ieStrNotImplemented);
  hFInt32 lRegIndex(0);
  hFInt32 lParamsSize(0);
  for (hFUInt32 c(0); c < lParams.size(); ++ c) {
    if (isDouble(lParams[c]->GetType()) == true) {
      lParams[c]->SetAddress(AddrStack(ARMHW::csRegFP, lParamsSize + func_->GetLocalsSize()));
      IROTmp* paramObj(irBuilder.irboTmp(lParams[c]->GetType(),
        AddrReg(new VirtRegDouble(ARMHW::csRegArray[lRegIndex], ARMHW::csRegArray[lRegIndex + 1])), "ccprm"));
      irBuilder.irbsAssign(irBuilder.irbeAddrOf(lParams[c]),
          irBuilder.GetHelper()->irbeUseOf(paramObj), ICStmt(extr.eGetFirstStmt(func_), ICAfter), SrcLoc());
      lRegIndex += 2;
      lParamsSize += 8;
    } else if (pred.pIsFundamentalType(lParams[c]->GetType()) == true) {
      if (context_->GetCGFlags() & CGFLocalRA) {
        //! \todo M Design: change constant 4 into type size or so.
        lParams[c]->SetAddress(AddrStack(ARMHW::csRegFP, lParamsSize + func_->GetLocalsSize()));
        IROTmp* paramObj(irBuilder.irboTmp(lParams[c]->GetType(), AddrReg(ARMHW::csRegArray[lRegIndex]), "ccprm"));
        irBuilder.irbsAssign(irBuilder.irbeAddrOf(lParams[c]),
          irBuilder.GetHelper()->irbeUseOf(paramObj), ICStmt(extr.eGetFirstStmt(func_), ICAfter), SrcLoc());
      } else {
        lParams[c]->SetAddress(AddrReg(ARMHW::csRegArray[lRegIndex]));
        IROTmp* paramObj(irBuilder.irboTmp(lParams[c]->GetType(), AddrReg(new VirtReg()), "ccprm"));
        irBuilder.irbsAssign(irBuilder.irbeAddrOf(paramObj),
          irBuilder.GetHelper()->irbeUseOf(lParams[c]), ICStmt(extr.eGetFirstStmt(func_), ICAfter), SrcLoc());
      } // if
      lRegIndex ++;
      lParamsSize += 4;
    } else {
      REQUIREDMSG(0, ieStrNotImplemented);
      NOTIMPLEMENTED(M);
    } // if
    func_->SetParamsSize(lParamsSize);
  } // for
  EMDEBUG0();
  return;
} // CallConvGNUEABI::processParams

void
sEmitInits(IRType* type_, IRExpr* init_, CGContext& context_) {
  BDEBUG4("sEmitInits", type_, init_, &context_);
  ASMDesc* lAsmDesc(context_.GetASMDesc());
  // lInitSize holds the exact size of the init_ in bytes.
  tBigInt lInitSize(0);
  bool lPadding(true);
  if (IRTArray* arr_ = dynamic_cast<IRTArray*>(type_)) {
    if (IREStrConst* lStrInit = dynamic_cast<IREStrConst*>(init_)) {
      lInitSize = lStrInit->GetType()->GetSize();
      ASMSection o;
      lAsmDesc->PutReloc(o, lStrInit);
      o.getAsm() << ends;
      context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(o.str()));
    } else if (IRExprList* lInitList = dynamic_cast<IRExprList*>(init_)) {
      vector<IRExpr*> lInits;
      lInitList->GetChildren(lInits);
      for (hFUInt32 c(0); c < lInits.size(); ++ c) {
        lInitSize += arr_->GetElemType()->GetSize();
        sEmitInits(arr_->GetElemType(), lInits[c], context_);
      } // for
    } // if
  } else if (IRTStruct* str_ = dynamic_cast<IRTStruct*>(type_)) {
    if (IRExprList* lInitList = dynamic_cast<IRExprList*>(init_)) {
      vector<IRExpr*> lInits;
      lInitList->GetChildren(lInits);
      const vector<IROField*> lFields(str_->GetFields());
      hFInt32 lCurrField(0);
      for (hFUInt32 c(0); c < lInits.size(); ++ c, ++ lCurrField) {
        lInitSize += lFields[lCurrField]->GetType()->GetSize();
        sEmitInits(lFields[lCurrField]->GetType(), lInits[c], context_);
      } // for
    } else {
      sEmitInits(init_->GetType(), init_, context_);
      lPadding = false;
    } // if
  } else if (pred.pIsNullExpr(init_) == true) {
    /* LIE */
  } else {
    lInitSize = init_->GetType()->GetSize();
    ASMSection o;
    lAsmDesc->PutReloc(o, init_);
    o.getAsm() << ends;
    context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(o.str()));
  } // if
  // Emit the space for left over bytes, if any.
  if (lPadding == true && type_->GetSize() > lInitSize) {
    tBigInt lPadding(type_->GetSize() - lInitSize);
    ASMSection o;
    lAsmDesc->PutSpace(o, lPadding / 8);
    o.getAsm() << ends;
    context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(o.str()));
  } // if
  EDEBUG0();
  return;
} // sEmitInits

void ARMHW::
OnGlobal(IROGlobal* global_, CGContext& context_) {
  BMDEBUG3("ARMHW::OnGlobal", global_, &context_);
  REQUIREDMSG(global_ != 0, ieStrParamValuesDBCViolation);
  PDEBUG("CodeGen", "detail", "OnGlobal : " << progcxt(global_));
  if (global_->GetType()->GetSize() != 0) {
    char lBuf[1024];
    if (GNUASMDesc* lGNUDesc = dynamic_cast<GNUASMDesc*>(context_.GetASMDesc())) {
      if (pred.pHasInits(global_) == true || global_->GetLinkage() == IRLStatic) {
        PDEBUG("CodeGen", "detail", "Global has inits");
        context_.AddInst("ModuleBeg", "data", new AsmSeqDirSection(ASTData));
        if (global_->GetLinkage() != IRLStatic) {
          sprintf(lBuf, "\t.global %s", global_->GetName().c_str());
          context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
        } // if
        { // Set alignment
          hFInt32 lAlignment(global_->GetType()->GetAlignment());
          if (pred.pIsArray(global_->GetType()) == true) {
            // Align arrays to minimum of double word, array copying is
            // frequent and it can be efficiently implemented by double word
            // load/stores.
            lAlignment = util.uMax(INBITS(8) * 8, lAlignment);
          } // if
          context_.AddInst("ModuleBeg", "data", new AsmSeqDirAlign(lAlignment/8));
        } // block
        { // emit the address label
          if (!pred.pHasLTConstAddr(global_)) {
            sprintf(lBuf, "\t.type %s, %%object", global_->GetName().c_str());
            context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
            sprintf(lBuf, "\t.size %s, %d", global_->GetName().c_str(), global_->GetType()->GetSize().get_hFUInt32()/8);
            context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
            sprintf(lBuf, "%s:", global_->GetName().c_str());
            context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
          } else {
            AddrLTConst* lAddr(static_cast<AddrLTConst*>(global_->GetAddress()));
            //! \note we do not emit the label when the address has an offset.
            //! This may happen in the case of base binding optimization.
            if (!lAddr->GetOffset()) {
              sprintf(lBuf, "\t.type %s, %%object", lAddr->GetLTConst().c_str());
              context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
              sprintf(lBuf, "\t.size %s, %d", lAddr->GetLTConst().c_str(), global_->GetType()->GetSize().get_hFUInt32()/8);
              context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
              sprintf(lBuf, "%s:", lAddr->GetLTConst().c_str());
              context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
            } // if
          } // if
        } // block
        if (pred.pHasInits(global_) == false) {
          sprintf(lBuf, "\t.space %d", global_->GetType()->GetSize().get_hFUInt32()/8);
          context_.AddInst("ModuleBeg", "data", new AsmSeqDirInfo(lBuf));
        } else {
          sEmitInits(global_->GetType(), global_->GetInits(), context_);
        } // if
      } else {
        if (!pred.pHasLTConstAddr(global_)) {
          sprintf(lBuf, "\t.comm %s, %d, %d", global_->GetName().c_str(),
            global_->GetType()->GetSize().get_hFUInt32()/8, global_->GetType()->GetAlignment());
        } else {
          AddrLTConst* lAddr(static_cast<AddrLTConst*>(global_->GetAddress()));
          sprintf(lBuf, "\t.comm %s, %d, %d", lAddr->GetLTConst().c_str(),
            global_->GetType()->GetSize().get_hFUInt32()/8, global_->GetType()->GetAlignment());
        } // if
        context_.AddInst("ModuleEnd", "data", new AsmSeqDirInfo(lBuf));
      } // if
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
  } // if
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      if (AddrLTConst* lAddr = dynamic_cast<AddrLTConst*>(global_->GetAddress())) {
        vector<DW_LOC_OP*> lLocExpr;
        lLocExpr.push_back(new DW_OP_addr(*lAddr));
        extr.eGetDwarfDIE(global_)->SetAddress(lLocExpr);
      } else {
        ASSERTMSG(0, ieStrNotImplemented);
        NOTIMPLEMENTED();
      } // if
    } // if
  } // if
  EMDEBUG0();
  return;
} // ARMHW::OnGlobal

void ARMHW::
OnProgStart(IRProgram* prog_, CGContext& context_) {
  BMDEBUG3("ARMHW:OnProgStart", prog_, &context_);
  REQUIREDMSG(prog_ != 0, ieStrParamValuesDBCViolation);
  if (dynamic_cast<GNUASMDesc*>(context_.GetASMDesc())) {
    context_.AddInst("ProgBeg", "dir", new AsmSeqDirInfo("\t.ident \"bcc version 0.0.1\""));
    if (options.obGet("debuginfo") == true) {
      if (options.oeGet("debugformat") == "Dwarf2") {
        dwarf.OnProgStart(prog_, context_);
      } // if
    } // if
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  EMDEBUG0();
  return;
} // ARMHW::OnProgStart

void ARMHW::
OnStmtStart(IRStmt* stmt_, CGFuncContext& context_) {
  BMDEBUG3("ARMHW:OnStmtStart", stmt_, &context_);
  REQUIREDMSG(stmt_ != 0, ieStrParamValuesDBCViolation);
  HWDescription::OnStmtStart(stmt_, context_);
  EMDEBUG0();
  return;
} // ARMHW::OnStmtStart

void ARMHW::
OnStmtEnd(IRStmt* stmt_, CGFuncContext& context_) {
  BMDEBUG3("ARMHW:OnProgStart", stmt_, &context_);
  REQUIREDMSG(stmt_ != 0, ieStrParamValuesDBCViolation);
  EMDEBUG0();
  return;
} // ARMHW::OnStmtEnd

void ARMHW::
OnProgEnd(IRProgram* prog_, CGContext& context_) {
  BMDEBUG3("ARMHW::OnProgEnd", prog_, &context_);
  /* LIE */
  EMDEBUG0();
  return;
} // ARMHW::OnProgEnd

void ARMHW::
OnModuleStart(IRModule* module_, CGContext& context_) {
  BMDEBUG3("ARMHW::OnModuleStart", module_, &context_);
  //! \todo M Design: ".file" directive may be emitted for multi module compilations.
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      { // Create a label for statement list in .debug_line section
        string lLabelDebugLineBeg(context_.GetALocalLabel("stmt_list"));
        IRBuilder::smirbAttachLabel(0, "stmt_list", lLabelDebugLineBeg);
        // \todo Use of dir is a work around fix it.
        context_.AddInst("ModuleBeg", "dir", new AsmSeqDirSection(ASTCustom, ".debug_line"));
        context_.AddInst("ModuleBeg", "dir", Rule::CreateInst(context_, "label", lLabelDebugLineBeg));
      } // block
      DF_CIE* lCIE(new DF_CIE(
        0xffffffff,  // id
        1,  // version
        "", // augmentation
        1,  // code align factor
        -1,  // data align factor
        15  // return address register num, r14/lr.
        ));
      dwarf.SetCurrentCIE(lCIE);
      lCIE->AddInst(new DW_CFA_undefined(ULEB128(DW_REGNUM_R0)));
      lCIE->AddInst(new DW_CFA_undefined(ULEB128(DW_REGNUM_R1)));
      lCIE->AddInst(new DW_CFA_undefined(ULEB128(DW_REGNUM_R2)));
      lCIE->AddInst(new DW_CFA_undefined(ULEB128(DW_REGNUM_R3)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R4)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R5)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R6)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R7)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R8)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R9)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R10)));
      lCIE->AddInst(new DW_CFA_same_value(ULEB128(DW_REGNUM_R11)));
      lCIE->AddInst(new DW_CFA_def_cfa(ULEB128(DW_REGNUM_R13), ULEB128(0)));
      dwarf.OnModuleStart(module_);
    } // if
  } // if
  { // Emit module begin label.
    string lModuleBegLabel(context_.GetALocalLabel("modbeg"));
    context_.AddInst("ModuleBeg", "text", new AsmSeqDirSection(ASTText));
    context_.AddInst("ModuleBeg", "text", Rule::CreateInst(context_, "label", lModuleBegLabel));
    IRBuilder::smirbAttachLabel(module_, "beg", lModuleBegLabel);
  } // block
  EMDEBUG0();
  return;
} // ARMHW::OnModuleStart

void ARMHW::
OnModuleEnd(IRModule* module_, CGContext& context_) {
  BMDEBUG3("ARMHW::OnModuleEnd", module_, &context_);
  { // Emit module end label.
    string lModuleEndLabel(context_.GetALocalLabel("modend"));
    context_.AddInst("ModuleEnd", "text", Rule::CreateInst(context_, "label", lModuleEndLabel));
    IRBuilder::smirbAttachLabel(module_, "end", lModuleEndLabel);
  } // block
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      dwarf.OnModuleEnd(module_);
    } // if
  } // if
  EMDEBUG0();
  return;
} // ARMHW::OnModuleEnd

void ARMHW::
OnFuncStart(IRFunction* func_, CGFuncContext& context_) {
  BMDEBUG3("ARMHW::OnFuncStart", func_, &context_);
  if (dynamic_cast<GNUASMDesc*>(context_.GetASMDesc())) {
    //    .text
    //    .align 2
    //    .global <funcname>
    //    .type <funcname>, %function
    // <funcname>:
    context_.AddInst(new AsmSeqDirSection(ASTText));
    context_.AddInst(new AsmSeqDirInfo(string("\t.ltorg")));
    context_.AddInst(new AsmSeqDirAlign(4));
    context_.AddInst(new AsmSeqDirInfo(string("\t.global ") + func_->GetName()));
    context_.AddInst(new AsmSeqDirInfo(string("\t.type ") + func_->GetName() + ", %function"));
    context_.AddInst(Rule::CreateInst(context_, "label", func_->GetName()));
    IRBuilder::smirbAttachLabel(func_, "beg", func_->GetName());
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      dwarf.OnFuncStart(func_, context_);
    } // if
  } // if
  EMDEBUG0();
  return;
} // ARMHW::OnFuncStart

void ARMHW::
OnLocal(IRFunction* func_, IROLocal* local_, CGFuncContext& context_) {
  BMDEBUG4("ARMHW::OnLocal", func_, local_, &context_);
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      vector<DW_LOC_OP*> lLocExpr;
      if (AddrStack* lAddr = dynamic_cast<AddrStack*>(local_->GetAddress())) {
        lLocExpr.push_back(new DW_OP_fbreg(extr.eGetOffsetAddrStack(lAddr)));
      } else if (AddrReg* lAddr = dynamic_cast<AddrReg*>(local_->GetAddress())) {
        NOTIMPLEMENTED();
      } else {
        ASSERTMSG(0, ieStrNotImplemented);
      } // if
      if (!lLocExpr.empty()) {
        extr.eGetDwarfDIE(local_)->SetAddress(lLocExpr);
      } // if
    } // if
  } // if
  EMDEBUG0();
  return;
} // ARMHW::OnLocal

void ARMHW::
OnParam(IRFunction* func_, IROParameter* param_, CGFuncContext& context_) {
  BMDEBUG4("ARMHW::OnParam", func_, param_, &context_);
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      vector<DW_LOC_OP*> lLocExpr;
      if (AddrStack* lAddr = dynamic_cast<AddrStack*>(param_->GetAddress())) {
        lLocExpr.push_back(new DW_OP_fbreg(extr.eGetOffsetAddrStack(lAddr)));
      } else if (AddrReg* lAddr = dynamic_cast<AddrReg*>(param_->GetAddress())) {
        NOTIMPLEMENTED();
      } else {
        ASSERTMSG(0, ieStrNotImplemented);
      } // if
      if (!lLocExpr.empty()) {
        extr.eGetDwarfDIE(param_)->SetAddress(lLocExpr);
      } // if
    } // if
  } // if
  EMDEBUG0();
  return;
} // ARMHW::OnParam

void ARMHW::
OnFuncEnd(IRFunction* func_, CGFuncContext& context_) {
  BMDEBUG3("ARMHW::OnFuncEnd", func_, &context_);
  if (dynamic_cast<GNUASMDesc*>(context_.GetASMDesc())) {
    context_.AddInst(Rule::CreateInst(context_, "label", func_->GetName()+"_end"));
    IRBuilder::smirbAttachLabel(func_, "end", func_->GetName()+"_end");
    context_.AddInst(new AsmSeqDirInfo("\t.size " + func_->GetName() + ", .-" + func_->GetName()));
  } else {
    ASSERTMSG(0, ieStrNotImplemented);
  } // if
  HWDescription::OnFuncEnd(func_, context_);
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      vector<DW_LOC_OP*> lLocExpr;
      lLocExpr.push_back(new DW_OP_reg11);
      // set DW_AT_frame_base attribute.
      extr.eGetDwarfDIE(func_)->SetFrameBase(0, lLocExpr);
      dwarf.OnFuncEnd(func_, context_);
    } // if
  } // if
  EMDEBUG0();
  return;
} // ARMHW::OnFuncEnd

void ARMHW::
AssignLTConstAddresses(const IRModule* module_, CGContext& context_) {
  BMDEBUG2("ARMHW::AssignLTConstAddresses", module_);
  vector<IROGlobal*> lGlobals;
  module_->GetGlobals(lGlobals);
  for (hFUInt32 c(0); c < lGlobals.size(); ++ c) {
    if (pred.pIsAddrSet(lGlobals[c]) == true) {
      if (AddrLTConst* lLTConst = dynamic_cast<AddrLTConst*>(lGlobals[c]->GetAddress())) {
        if (!lLTConst->IsAssigned()) {
          lGlobals[c]->SetAddress(AddrLTConst(context_.GetALocalLabel(), 0));
          PDEBUG("CodeGen", "detail", " glob addr assigned: " << progcxt(lGlobals[c]));
        } // if
      } // if
    } else {
      lGlobals[c]->SetAddress(AddrLTConst(lGlobals[c]->GetName(), 0));
      PDEBUG("CodeGen", "detail", " glob addr set: " << progcxt(lGlobals[c]));
    } // if
  } // for
  EMDEBUG0();
  return;
} // ARMHW::AssignLTConstAddresses

DTCCommandToken
operator ++ (DTCCommand command_) {
  DTASSERT(command_ == DTCCIndent, "You may use unary ++ only for DTCCIndent");
  return DTCCommandToken(DTCCIncIndent);
} // operator ++

DTCCommandToken
operator -- (DTCCommand command_) {
  DTASSERT(command_ == DTCCIndent, "You may use unary -- only for DTCCIndent");
  return DTCCommandToken(DTCCDecIndent);
} // operator --


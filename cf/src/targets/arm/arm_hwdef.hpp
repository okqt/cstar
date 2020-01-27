// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006.
#ifndef __ARM_HWDEF_HPP__
#define __ARM_HWDEF_HPP__

#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __BCF_TARGET_HPP__
#include "bcf_target.hpp"
#endif

enum {
  DW_REGNUM_R0 = 0,
  DW_REGNUM_R1 = 1,
  DW_REGNUM_R2 = 2,
  DW_REGNUM_R3 = 3,
  DW_REGNUM_R4 = 4,
  DW_REGNUM_R5 = 5,
  DW_REGNUM_R6 = 6,
  DW_REGNUM_R7 = 7,
  DW_REGNUM_R8 = 8,
  DW_REGNUM_R9 = 9,
  DW_REGNUM_R10 = 10,
  DW_REGNUM_R11 = 11,
  DW_REGNUM_FP  = DW_REGNUM_R11,
  DW_REGNUM_R12 = 12,
  DW_REGNUM_IP  = DW_REGNUM_R12,
  DW_REGNUM_R13 = 13,
  DW_REGNUM_SP  = DW_REGNUM_R13,
  DW_REGNUM_R14 = 14,
  DW_REGNUM_LR  = DW_REGNUM_R14,
}; //

//! \note Singleton object.
class CallConvGNUEABI : public CallingConvention {
public:
  CallConvGNUEABI() : CallingConvention("ARM-GNU-EABI") {}
  virtual void processParams(IRFunction* func_, CGContext* cgContext_) const;
  virtual void processReturn(IRSReturn* returnStmt_, CGContext* cgContext_) const;
  virtual void processCall(IRSCall* callStmt_, CGContext* cgContext_) const;
  virtual void processWriteBitField(IRSAssign* assign_) const;
  virtual void processReadBitField(IREDeref* deref_) const;
}; // class CallConvGNUEABI

class ARMISADef : public InstructionSetDefinition {
public:
  ARMISADef(HWDescription* hwDesc_, const string name_, ASMDesc* asmDesc_) : 
    InstructionSetDefinition(hwDesc_, name_, asmDesc_)
  {
  } // ARMISADef::ARMISADef
  virtual ~ARMISADef() {}
public:
}; // class

//! \brief ARM hardware description.
class ARMHW : public HWDescription {
public:
  ARMHW(Target* target_);
  virtual void OnBBStart(IRBB* bb_, CGFuncContext& context_);
  virtual void InitializeRules();
  static bool IsImm8r(const tBigInt& value);
  virtual HWReg* GetFPReg() const {
    REQUIREDMSG(dynamic_cast<HWReg*>(csRegFP) != 0, ieStrParamValuesDBCViolation);
    return static_cast<HWReg*>(csRegFP);
  } // ARMHW::GetFPReg
  virtual HWReg* GetLRReg() const {
    REQUIREDMSG(dynamic_cast<HWReg*>(csRegLR) != 0, ieStrParamValuesDBCViolation);
    return static_cast<HWReg*>(csRegLR);
  } // ARMHW::GetLRReg
  virtual HWReg* GetSPReg() const {
    REQUIREDMSG(dynamic_cast<HWReg*>(csRegSP) != 0, ieStrParamValuesDBCViolation);
    return static_cast<HWReg*>(csRegSP);
  } // ARMHW::GetSPReg
  virtual HWReg* GetPCReg() const {
    REQUIREDMSG(dynamic_cast<HWReg*>(csRegPC) != 0, ieStrParamValuesDBCViolation);
    return static_cast<HWReg*>(csRegPC);
  } // ARMHW::GetPCReg
  virtual void OnGlobal(IROGlobal* global_, CGContext& context_);
  virtual void OnProgStart(IRProgram* prog_, CGContext& context_);
  virtual void OnLocal(IRFunction* func_, IROLocal* local_, CGFuncContext& context_);
  virtual void OnParam(IRFunction* func_, IROParameter* param_, CGFuncContext& context_);
  virtual void OnProgEnd(IRProgram* prog_, CGContext& context_);
  virtual void OnModuleStart(IRModule* module_, CGContext& context_);
  virtual void OnModuleEnd(IRModule* module_, CGContext& context_);
  virtual void OnStmtStart(IRStmt* stmt_, CGFuncContext& context_);
  virtual void OnStmtEnd(IRStmt* stmt_, CGFuncContext& context_);
  virtual void OnFuncStart(IRFunction* func_, CGFuncContext& context_);
  virtual void OnFuncEnd(IRFunction* func_, CGFuncContext& context_);
  const RegSet& GetRegsForRegAlloc() const {return *csRegAllocSet;}
  virtual void AssignLTConstAddresses(const IRModule* module_, CGContext& context_);
  virtual void AddHWRegs();
public: // Member data
  static HWReg* csRegR0;
  static HWReg* csRegR1;
  static HWReg* csRegR2;
  static HWReg* csRegR3;
  static HWReg* csRegR4;
  static HWReg* csRegR5;
  static HWReg* csRegR6;
  static HWReg* csRegR7;
  static HWReg* csRegR8;
  static HWReg* csRegR9; // sb, static base
  static HWReg* csRegR10; // sl, stack limit
  static HWReg* csRegFP; // r11
  static HWReg* csRegR12; // ip, intra-procedure-call scratch
  static HWReg* csRegSP; // r13
  static HWReg* csRegLR; // r14
  static HWReg* csRegPC; // r15
  static HWReg* csRegArray[16]; // Initialized to R0-PC.
  static RegSet* csRegSetCalleeSave; // r4-r11,r13
  static RegSet* csRegAllocSet; // r0-r14 can be used for register allocation.
  static RegSet* csRegParamSet; // r0-r3.
  static RegSet* csRegsR0R1; // r0-r1.
}; // class ARMHW

class GNUASMDesc : public ASMDesc {
public:
  GNUASMDesc() :
    ASMDesc("GNU ARM Assembly")
  {
  } // GNUASMDesc::GNUASMDesc
public: // ASMDesc interface functions
  virtual ASMSection& PutCommentStr(ASMSection& o) const;
  virtual ASMSection& PutFuncProlog(ASMSection& o, const IRFunction* func_) const;
  virtual ASMSection& PutFuncEpilog(ASMSection& o, const IRFunction* func_) const;
  virtual ASMSection& PutGlobal(ASMSection& o, const IROGlobal* globalObj_) const;
  virtual ostream& PutDirFileName(ostream& o, const string& inputFileName_) const;
  virtual ASMSection& PutLabel(ASMSection& o, const string& label_, const string& comment_ = string()) const;
  virtual ostream& PutIdentificationString(ostream& o) const;
  virtual ASMSection& PutAlign(ASMSection& o, const tBigInt& alignInBytes_) const;
  virtual ASMSection& PutAddrConst(ASMSection& o, const tBigAddr& addr_, const string& comment_ = string()) const;
  virtual ASMSection& PutSpace(ASMSection& o, const tBigInt& sizeInBytes_, const string& comment_ = string()) const;
  virtual ASMSection& PutReloc(ASMSection& o, const IRRelocExpr* relocExpr_, const string& comment_ = string()) const;
  virtual ASMSection& PutRelocAddrDiff(ASMSection& o, const string& addressHigh_, const string& addressLow_,
    const string& comment_ = string()) const;
  virtual ASMSection& PutRelocAddr(ASMSection& o, const string& address_, const string& comment_ = string()) const;
  virtual ostream& PutSection(ostream& o, AsmSectionType sectionType_, const string& customName_) const;
  virtual string GetLocalLabel(const string& prefix_, hFUInt32 labelId_) const;
  virtual ASMSection& PutData1(ASMSection& o, hFUInt8 data_, const string& comment_ = string()) const;
  virtual ASMSection& PutData2(ASMSection& o, hFUInt16 data_, const string& comment_ = string()) const;
  virtual ASMSection& PutData4(ASMSection& o, hFUInt32 data_, const string& comment_ = string()) const;
  virtual ASMSection& PutDataString(ASMSection& o, const string& data_, const string& comment_ = string()) const;
  virtual ASMSection& PutDirLoc(ASMSection& o, hFUInt32 fileNo_, hFUInt32 lineNo_, hFUInt32 colNo_,
    DWARF_LNSDir lnsDir_, hFUInt32 value_) const;
  virtual ASMSection& PutDirFileLoc(ASMSection& o, hFUInt32 fileNo_, const string& fileName_) const;
  virtual void PutInstRegSet(ostream& o, const RegSet& regSet_) const;
  virtual void PutInstShiftCode(ostream& o, ShiftType shiftType_) const;
  virtual void PutInstCondCode(ostream& o, CNTType condType_) const;
}; // class GNUASMDesc

#endif

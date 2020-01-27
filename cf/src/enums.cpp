// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __ENUMS_HPP__
#include "enums.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif

AsmSectionType
operator | (AsmSectionType left_, AsmSectionType right_) {
  //! \todo M Design: Do some combination checks.
  return (AsmSectionType)((hFUInt32)left_ | (hFUInt32)right_);
} // operator |

ostream&
operator << (ostream& o, IRSignedness irSignedness_) {
  o << toStr(irSignedness_);
  return o;
} // operator <<

ostream&
operator << (ostream& o, ICPlace icPlace_) {
  o << toStr(icPlace_);
  return o;
} // operator <<

ostream& operator << (ostream& o, NTType ntType_) {
  o << toStr(ntType_);
  return o;
} // operator <<

ostream& operator << (ostream& o, DWARF_LNSDir lnsDir_) {
  o << toStr(lnsDir_);
  return o;
} // operator <<

const char*
toStr(DWARF_LNSDir lnsDir_) {
  const char* retVal(0);
  switch(lnsDir_) {
    case DLNSD_Invalid: retVal = "DLNSD_Invalid"; break;
    case DLNSD_none: retVal = "DLNSD_none"; break;
    case DLNSD_basic_block: retVal = "DLNSD_basic_block"; break;
    case DLNSD_prologue_end: retVal = "DLNSD_prologue_end"; break;
    case DLNSD_epilogue_begin: retVal = "DLNSD_epilogue_begin"; break;
    case DLNSD_is_stmt: retVal = "DLNSD_is_stmt"; break;
    case DLNSD_isa: retVal = "DLNSD_isa"; break;
    default:
      ASSERTMSG(0, ieStrPCShouldNotReachHere << " " << (int)lnsDir_);
      break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr

const char*
toStr(NTType ntType_) {
  const char* retVal(0);
  switch(ntType_) {
    case NTTLast: retVal = "NTTLast"; break;
    case NTTRegister: retVal = "NTTRegister"; break;
    case NTTIgnore: retVal = "NTTIgnore"; break;
    case NTTMemory: retVal = "NTTMemory"; break;
    case NTTCondition: retVal = "NTTCondition"; break;
    case NTTIntConst: retVal = "NTTIntConst"; break;
    case NTTRealConst: retVal = "NTTRealConst"; break;
    case NTTShiftOp: retVal = "NTTShiftOp"; break;
    case NTTNull: retVal = "NTTNull"; break;
    case NTTAny: retVal = "NTTAny"; break;
    default: break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr

const char*
toStr(IRSignedness irs_) {
  const char* retVal(0);
  switch(irs_) {
    case IRSInvalid: retVal = "IRSInvalid"; break;
    case IRSSigned: retVal = "IRSSigned"; break;
    case IRSUnsigned: retVal = "IRSUnsigned"; break;
    case IRSDoNotCare: retVal = "IRSDoNotCare"; break;
    default: break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr

const char*
toStr(IRLinkage irl_) {
  const char* retVal(0);
  switch(irl_) {
    case IRLInvalid: retVal = "IRLInvalid"; break;
    case IRLStatic: retVal = "IRLStatic"; break;
    case IRLExport: retVal = "IRLExport"; break;
    case IRLImport: retVal = "IRLImport"; break;
    default: break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr

const char*
toStr(ICPlace icp_) {
  const char* retVal(0);
  switch(icp_) {
    case ICInvalid: retVal = "ICInvalid"; break;
    case ICBefore: retVal = "ICBefore"; break;
    case ICAfter: retVal = "ICAfter"; break;
    case ICBegOfBB: retVal = "ICBegOfBB"; break;
    case ICEndOfBB: retVal = "ICEndOfBB"; break;
    default: break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr

const char*
toStr(IRBIRId irbir_) {
  const char* retVal(0);
  switch(irbir_) {
    case IRBIRInvalid: retVal = "IRBIRInvalid"; break;
    case IRBIRCGPattern: retVal = "IRBIRcgpattern"; break;
    case IRBIRva_start: retVal = "IRBIRva_start"; break;
    case IRBIRva_end: retVal = "IRBIRva_end"; break;
    case IRBIRva_copy: retVal = "IRBIRva_copy"; break;
    case IRBIRva_arg: retVal = "IRBIRva_arg"; break;
    case IRBIRmemcpy: retVal = "IRBIRmemcpy"; break;
    default: break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr

const char* toStr(eDW_OP dw_op_) {
  const char* retVal(0);
  switch(dw_op_) {
    case eDW_OP_addr: retVal = "eDW_OP_addr"; break;
    case eDW_OP_deref: retVal = "eDW_OP_deref"; break;
    case eDW_OP_const1u: retVal = "eDW_OP_const1u"; break;
    case eDW_OP_const1s: retVal = "eDW_OP_const1s"; break;
    case eDW_OP_const2u: retVal = "eDW_OP_const2u"; break;
    case eDW_OP_const2s: retVal = "eDW_OP_const2s"; break;
    case eDW_OP_const4u: retVal = "eDW_OP_const4u"; break;
    case eDW_OP_const4s: retVal = "eDW_OP_const4s"; break;
    case eDW_OP_const8u: retVal = "eDW_OP_const8u"; break;
    case eDW_OP_const8s: retVal = "eDW_OP_const8s"; break;
    case eDW_OP_constu: retVal = "eDW_OP_constu"; break;
    case eDW_OP_consts: retVal = "eDW_OP_consts"; break;
    case eDW_OP_dup: retVal = "eDW_OP_dup"; break;
    case eDW_OP_drop: retVal = "eDW_OP_drop"; break;
    case eDW_OP_over: retVal = "eDW_OP_over"; break;
    case eDW_OP_pick: retVal = "eDW_OP_pick"; break;
    case eDW_OP_swap: retVal = "eDW_OP_swap"; break;
    case eDW_OP_rot: retVal = "eDW_OP_rot"; break;
    case eDW_OP_xderef: retVal = "eDW_OP_xderef"; break;
    case eDW_OP_abs: retVal = "eDW_OP_abs"; break;
    case eDW_OP_and: retVal = "eDW_OP_and"; break;
    case eDW_OP_div: retVal = "eDW_OP_div"; break;
    case eDW_OP_minus: retVal = "eDW_OP_minus"; break;
    case eDW_OP_mod: retVal = "eDW_OP_mod"; break;
    case eDW_OP_mul: retVal = "eDW_OP_mul"; break;
    case eDW_OP_neg: retVal = "eDW_OP_neg"; break;
    case eDW_OP_not: retVal = "eDW_OP_not"; break;
    case eDW_OP_or: retVal = "eDW_OP_or"; break;
    case eDW_OP_plus: retVal = "eDW_OP_plus"; break;
    case eDW_OP_plus_uconst: retVal = "eDW_OP_plus_uconst"; break;
    case eDW_OP_shl: retVal = "eDW_OP_shl"; break;
    case eDW_OP_shr: retVal = "eDW_OP_shr"; break;
    case eDW_OP_shra: retVal = "eDW_OP_shra"; break;
    case eDW_OP_xor: retVal = "eDW_OP_xor"; break;
    case eDW_OP_skip: retVal = "eDW_OP_skip"; break;
    case eDW_OP_bra: retVal = "eDW_OP_bra"; break;
    case eDW_OP_eq: retVal = "eDW_OP_eq"; break;
    case eDW_OP_ge: retVal = "eDW_OP_ge"; break;
    case eDW_OP_gt: retVal = "eDW_OP_gt"; break;
    case eDW_OP_le: retVal = "eDW_OP_le"; break;
    case eDW_OP_lt: retVal = "eDW_OP_lt"; break;
    case eDW_OP_ne: retVal = "eDW_OP_ne"; break;
    case eDW_OP_lit0: retVal = "eDW_OP_lit0"; break;
    case eDW_OP_lit1: retVal = "eDW_OP_lit1"; break;
    case eDW_OP_lit2: retVal = "eDW_OP_lit2"; break;
    case eDW_OP_lit3: retVal = "eDW_OP_lit3"; break;
    case eDW_OP_lit4: retVal = "eDW_OP_lit4"; break;
    case eDW_OP_lit5: retVal = "eDW_OP_lit5"; break;
    case eDW_OP_lit6: retVal = "eDW_OP_lit6"; break;
    case eDW_OP_lit7: retVal = "eDW_OP_lit7"; break;
    case eDW_OP_lit8: retVal = "eDW_OP_lit8"; break;
    case eDW_OP_lit9: retVal = "eDW_OP_lit9"; break;
    case eDW_OP_lit10: retVal = "eDW_OP_lit10"; break;
    case eDW_OP_lit11: retVal = "eDW_OP_lit11"; break;
    case eDW_OP_lit12: retVal = "eDW_OP_lit12"; break;
    case eDW_OP_lit13: retVal = "eDW_OP_lit13"; break;
    case eDW_OP_lit14: retVal = "eDW_OP_lit14"; break;
    case eDW_OP_lit15: retVal = "eDW_OP_lit15"; break;
    case eDW_OP_lit16: retVal = "eDW_OP_lit16"; break;
    case eDW_OP_lit17: retVal = "eDW_OP_lit17"; break;
    case eDW_OP_lit18: retVal = "eDW_OP_lit18"; break;
    case eDW_OP_lit19: retVal = "eDW_OP_lit19"; break;
    case eDW_OP_lit20: retVal = "eDW_OP_lit20"; break;
    case eDW_OP_lit21: retVal = "eDW_OP_lit21"; break;
    case eDW_OP_lit22: retVal = "eDW_OP_lit22"; break;
    case eDW_OP_lit23: retVal = "eDW_OP_lit23"; break;
    case eDW_OP_lit24: retVal = "eDW_OP_lit24"; break;
    case eDW_OP_lit25: retVal = "eDW_OP_lit25"; break;
    case eDW_OP_lit26: retVal = "eDW_OP_lit26"; break;
    case eDW_OP_lit27: retVal = "eDW_OP_lit27"; break;
    case eDW_OP_lit28: retVal = "eDW_OP_lit28"; break;
    case eDW_OP_lit29: retVal = "eDW_OP_lit29"; break;
    case eDW_OP_lit30: retVal = "eDW_OP_lit30"; break;
    case eDW_OP_lit31: retVal = "eDW_OP_lit31"; break;
    case eDW_OP_reg0: retVal = "eDW_OP_reg0"; break;
    case eDW_OP_reg1: retVal = "eDW_OP_reg1"; break;
    case eDW_OP_reg2: retVal = "eDW_OP_reg2"; break;
    case eDW_OP_reg3: retVal = "eDW_OP_reg3"; break;
    case eDW_OP_reg4: retVal = "eDW_OP_reg4"; break;
    case eDW_OP_reg5: retVal = "eDW_OP_reg5"; break;
    case eDW_OP_reg6: retVal = "eDW_OP_reg6"; break;
    case eDW_OP_reg7: retVal = "eDW_OP_reg7"; break;
    case eDW_OP_reg8: retVal = "eDW_OP_reg8"; break;
    case eDW_OP_reg9: retVal = "eDW_OP_reg9"; break;
    case eDW_OP_reg10: retVal = "eDW_OP_reg10"; break;
    case eDW_OP_reg11: retVal = "eDW_OP_reg11"; break;
    case eDW_OP_reg12: retVal = "eDW_OP_reg12"; break;
    case eDW_OP_reg13: retVal = "eDW_OP_reg13"; break;
    case eDW_OP_reg14: retVal = "eDW_OP_reg14"; break;
    case eDW_OP_reg15: retVal = "eDW_OP_reg15"; break;
    case eDW_OP_reg16: retVal = "eDW_OP_reg16"; break;
    case eDW_OP_reg17: retVal = "eDW_OP_reg17"; break;
    case eDW_OP_reg18: retVal = "eDW_OP_reg18"; break;
    case eDW_OP_reg19: retVal = "eDW_OP_reg19"; break;
    case eDW_OP_reg20: retVal = "eDW_OP_reg20"; break;
    case eDW_OP_reg21: retVal = "eDW_OP_reg21"; break;
    case eDW_OP_reg22: retVal = "eDW_OP_reg22"; break;
    case eDW_OP_reg23: retVal = "eDW_OP_reg23"; break;
    case eDW_OP_reg24: retVal = "eDW_OP_reg24"; break;
    case eDW_OP_reg25: retVal = "eDW_OP_reg25"; break;
    case eDW_OP_reg26: retVal = "eDW_OP_reg26"; break;
    case eDW_OP_reg27: retVal = "eDW_OP_reg27"; break;
    case eDW_OP_reg28: retVal = "eDW_OP_reg28"; break;
    case eDW_OP_reg29: retVal = "eDW_OP_reg29"; break;
    case eDW_OP_reg30: retVal = "eDW_OP_reg30"; break;
    case eDW_OP_reg31: retVal = "eDW_OP_reg31"; break;
    case eDW_OP_breg0: retVal = "eDW_OP_breg0"; break;
    case eDW_OP_breg1: retVal = "eDW_OP_breg1"; break;
    case eDW_OP_breg2: retVal = "eDW_OP_breg2"; break;
    case eDW_OP_breg3: retVal = "eDW_OP_breg3"; break;
    case eDW_OP_breg4: retVal = "eDW_OP_breg4"; break;
    case eDW_OP_breg5: retVal = "eDW_OP_breg5"; break;
    case eDW_OP_breg6: retVal = "eDW_OP_breg6"; break;
    case eDW_OP_breg7: retVal = "eDW_OP_breg7"; break;
    case eDW_OP_breg8: retVal = "eDW_OP_breg8"; break;
    case eDW_OP_breg9: retVal = "eDW_OP_breg9"; break;
    case eDW_OP_breg10: retVal = "eDW_OP_breg10"; break;
    case eDW_OP_breg11: retVal = "eDW_OP_breg11"; break;
    case eDW_OP_breg12: retVal = "eDW_OP_breg12"; break;
    case eDW_OP_breg13: retVal = "eDW_OP_breg13"; break;
    case eDW_OP_breg14: retVal = "eDW_OP_breg14"; break;
    case eDW_OP_breg15: retVal = "eDW_OP_breg15"; break;
    case eDW_OP_breg16: retVal = "eDW_OP_breg16"; break;
    case eDW_OP_breg17: retVal = "eDW_OP_breg17"; break;
    case eDW_OP_breg18: retVal = "eDW_OP_breg18"; break;
    case eDW_OP_breg19: retVal = "eDW_OP_breg19"; break;
    case eDW_OP_breg20: retVal = "eDW_OP_breg20"; break;
    case eDW_OP_breg21: retVal = "eDW_OP_breg21"; break;
    case eDW_OP_breg22: retVal = "eDW_OP_breg22"; break;
    case eDW_OP_breg23: retVal = "eDW_OP_breg23"; break;
    case eDW_OP_breg24: retVal = "eDW_OP_breg24"; break;
    case eDW_OP_breg25: retVal = "eDW_OP_breg25"; break;
    case eDW_OP_breg26: retVal = "eDW_OP_breg26"; break;
    case eDW_OP_breg27: retVal = "eDW_OP_breg27"; break;
    case eDW_OP_breg28: retVal = "eDW_OP_breg28"; break;
    case eDW_OP_breg29: retVal = "eDW_OP_breg29"; break;
    case eDW_OP_breg30: retVal = "eDW_OP_breg30"; break;
    case eDW_OP_breg31: retVal = "eDW_OP_breg31"; break;
    case eDW_OP_regx: retVal = "eDW_OP_regx"; break;
    case eDW_OP_fbreg: retVal = "eDW_OP_fbreg"; break;
    case eDW_OP_bregx: retVal = "eDW_OP_bregx"; break;
    case eDW_OP_piece: retVal = "eDW_OP_piece"; break;
    case eDW_OP_deref_size: retVal = "eDW_OP_deref_size"; break;
    case eDW_OP_xderef_size: retVal = "eDW_OP_xderef_size"; break;
    case eDW_OP_nop: retVal = "eDW_OP_nop"; break;
    case eDW_OP_lo_user: retVal = "eDW_OP_lo_user"; break;
    case eDW_OP_hi_user: retVal = "eDW_OP_hi_user"; break;
    default: break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr

const char*
toStr(ShiftType shiftType_) {
  const char* retVal(0);
  switch(shiftType_) {
    case STNIL: retVal = "STNIL"; break;
    case STLSR: retVal = "STLSR"; break;
    case STASR: retVal = "STASR"; break;
    case STLSL: retVal = "STLSL"; break;
    case STROR: retVal = "STROR"; break;
    case STROL: retVal = "STROL"; break;
    case STRRX: retVal = "STRRX"; break;
    case STInvalid: retVal = "STInvalid"; break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr(ShiftType)

const char*
toStr(CNTType condType_) {
  const char* retVal(0);
  switch(condType_) {
    case CNTInvalid: retVal = "CNTInvalid"; break;
    case CNTEq: retVal = "CNTEq"; break;
    case CNTNe: retVal = "CNTNe"; break;
    case CNTGe: retVal = "CNTGe"; break;
    case CNTGt: retVal = "CNTGt"; break;
    case CNTLe: retVal = "CNTLe"; break;
    case CNTLt: retVal = "CNTLt"; break;
    case CNTULt: retVal = "CNTULt"; break;
    case CNTULe: retVal = "CNTULe"; break;
    case CNTUGe: retVal = "CNTUGe"; break;
    case CNTUGt: retVal = "CNTUGt"; break;
  } // switch
  ENSUREMSG(retVal != 0, ieStrInconsistentInternalStructure);
  return retVal;
} // toStr(CNTType)


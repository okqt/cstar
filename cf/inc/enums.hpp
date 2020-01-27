// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __ENUMS_HPP__
#define __ENUMS_HPP__

//! \todo L Design: Should we add IRXLast member to all as last enum?

//! \brief InsertContext place enumeration.
//! \sa InsertContext
typedef enum {
  ICInvalid = -1, //!< A use of ICPlace variable may never have this value.
  ICBefore,       //!< Insert context is before the statement \a stmt.
  ICAfter,        //!< Insert context is after the statement \a stmt.
  ICBegOfBB,      //!< Insert context is beginning of basic block
  ICEndOfBB,      //!< Insert context is end of basic block
  ICBeforeCFStmt,  //!< Insert context is end of basic block, but if it has control flow statement, it is before the cf stmt.
} ICPlace;

//! \brief Inline flag of an IRFunction.
//! \note Once the flag changes from not decided to one of to be inlined or
//!       not, flag should not be changed.
typedef enum tagInlineFlag {
  IFInvalid = -1,
  IFNotDecided = 0,
  IFToBeInlined = 1,
  IFNotToBeInlined = 2,
} InlineFlag;

typedef enum tagTreeMarks {
  eIRTMInvalid = -1,
  eIRTMGeneric = 0,
  eIRTMLoopInvariant = 1,
  // Add new marks by incrementing the value.
  eIRTMLast, // Note that last is always have numeric value more than the previous.
} eIRTreeMark;

//! \brief Linkage specification of objects.
//! \sa IRObject, IROGlobal.
typedef enum {
  IRLInvalid = -1,
  IRLStatic, //!< For file or function scoped static variables
  IRLExport, //!< For variables defined in global scope
  IRLImport, //!< For variables that are imported in global scope
} IRLinkage;

typedef enum {
  IRSTInvalid = -1,
  IRSTAssign,
  IRSTIf,
  IRSTPCall,
  IRSTFCall,
  IRSTBICall, //!< IRSBuiltInCall
  IRSTProlog,
  IRSTEpilog,
  IRSTReturn,
  IRSTLabel,
  IRSTNull,
  IRSTJump,
  IRSTDynJump,
  IRSTSwitch,
  IRSTEval,
  IRSTAfterLastOfBB,
  IRSTBeforeFirstOfBB,
} IRStmtType;

typedef enum {
  eDFACTInvalid = -1,
  eDFACTAnd,
  eDFACTOr,
} DFACombineType;

//! \brief Data flow analysis direction type.
typedef enum {
  eDFADInvalid = -1,
  eDFADForward,
  eDFADBackward,
} DFADirType;

//! \todo H Design: Is there any case that we may need register set non-terminal?
typedef enum {
  NTTInvalid = -1,
  NTTFirst,               //!< Special enum for the value of first enum.
  NTTRegister = NTTFirst,
  NTTIgnore,
  NTTMemory,
  NTTCondition,
  NTTIntConst,
  NTTRealConst,
  NTTShiftOp,
  NTTNull,
  NTTAny,
  NTTLast,          //!< Special enum for the value of last enum.
} NTType;

typedef enum {
  CNTInvalid = -1,
  CNTEq, //!< "==" condition
  CNTNe, //!< "!=" condition
  CNTGe, //!< ">=" condition
  CNTGt, //!< ">"  condition
  CNTLe, //!< "<=" condition
  CNTLt, //!< "<"  condition
  CNTUGe, //!< ">=" condition, unsigned
  CNTUGt, //!< ">"  condition, unsigned
  CNTULe, //!< "<=" condition, unsigned
  CNTULt, //!< "<"  condition, unsigned
  CNTLast,
} CNTType;

//! \brief Shift types.
typedef enum {
  STInvalid = -1,
  STNIL, //!< no shift, provided for uniformity
  STLSR, //!< logical shift right
  STASR, //!< arithmetic shift right
  STLSL, //!< logical shift left
  STROR, //!< rotate right
  STROL, //!< rotate left - can be achived by ROR.
  STRRX, //!< rotate right with extend
  STLast,
} ShiftType;

//! \note When ASTCustom, you can combine it with text, data, bss, const data.
typedef enum {
  ASTInvalid    = -1, //!< Invalid section
  ASTText       = 1 << 0,  //!< Code section
  ASTData       = 1 << 1,  //!< Read/Write Data section
  ASTBSS        = 1 << 2,  //!< \todo fill in the description for BSS.
  ASTConstData  = 1 << 3,  //!< Read only data section
  ASTCustom     = 1 << 4,  //!< A custom section, e.g., ".debug_info"
} AsmSectionType;

AsmSectionType operator | (AsmSectionType left_, AsmSectionType right_);

typedef enum {
  DLNSD_Invalid         = -1,
  DLNSD_none            = 0,
  DLNSD_basic_block     = 1,
  DLNSD_prologue_end    = 2,
  DLNSD_epilogue_begin  = 3,
  DLNSD_is_stmt         = 4, //!< Must be supplied with a 0, or 1 value.
  DLNSD_isa             = 5, //!< Must be supplied with an unsigned integer.
} DWARF_LNSDir;

typedef enum {
  LBEInvalid  = -1,
  LBELittleEndian = 0, //!< Little endian
  LBEBigEndian = 1,    //!< Big endian
} LittleBigEndian;

//! \brief Signedness specification of integers.
//! \todo M Design: Perhaps it is better to have a different naming convention
//!       for enumerations, IRSignedness is looking like IRStatements.
typedef enum {
  IRSInvalid = -1, //!< An invalid signedness specification.
  IRSSigned,       //!< Signed case.
  IRSUnsigned,     //!< Unsigned case.
  IRSDoNotCare,    //!< Means signedness is not important.
} IRSignedness;

//! \brief List of built in routines' identifiers, all built in routines must be listed here.
//! \todo M Design: Shall we provide a string conversion for each enum.
//! \par When adding a new builtin:
//!   - Also handle the toStr function.
typedef enum {
  IRBIRInvalid = -1, //!< An invalid built-in routine specification.
  IRBIRCGPattern, //!< used for code generator statement patterns.
  IRBIRva_start, //!< void va_start(va_list ap, last_argument);
  IRBIRva_arg, //!< type va_arg(va_list ap, type);
  IRBIRva_end, //!< void va_end(va_list ap);
  IRBIRva_copy, //!< void va_copy(va_list dst, va_list src);
  IRBIRmemcpy, //!< void* memcpy(void* dst, void* src, size_t length);
} IRBIRId;

typedef enum {
  EVOTInvalid = -1,
  EVOTPost, //!< Children nodes are visited first then the node itself.
  EVOTPre,  //!< First the node then the children nodes are visited.
  EVOTDoNotCare = EVOTPre,
} ExprVisitOrderType;

namespace Dwarf {

enum eDW_AT {
  eDW_AT_sibling = 0x01, 
  eDW_AT_location = 0x02,
  eDW_AT_name = 0x03,
  eDW_AT_ordering = 0x09,
  eDW_AT_byte_size = 0x0b,
  eDW_AT_bit_offset = 0x0c,
  eDW_AT_bit_size = 0x0d,
  eDW_AT_stmt_list = 0x10,
  eDW_AT_low_pc = 0x11,
  eDW_AT_high_pc = 0x12,
  eDW_AT_language = 0x13,
  eDW_AT_discr = 0x15,
  eDW_AT_discr_value = 0x16,
  eDW_AT_visibility = 0x17,
  eDW_AT_import = 0x18,
  eDW_AT_string_length = 0x19,
  eDW_AT_common_reference = 0x1a,
  eDW_AT_comp_dir = 0x1b,
  eDW_AT_const_value = 0x1c,
  eDW_AT_containing_type = 0x1d,
  eDW_AT_default_value = 0x1e,
  eDW_AT_inline = 0x20,
  eDW_AT_is_optional = 0x21,
  eDW_AT_lower_bound = 0x22,
  eDW_AT_producer = 0x25,
  eDW_AT_prototyped = 0x27,
  eDW_AT_return_addr = 0x2a,
  eDW_AT_start_scope = 0x2c,
  eDW_AT_stride_size = 0x2e,
  eDW_AT_upper_bound = 0x2f,
  eDW_AT_abstract_origin = 0x31,
  eDW_AT_accessibility = 0x32,
  eDW_AT_address_class = 0x33,
  eDW_AT_artificial = 0x34,
  eDW_AT_base_types = 0x35,
  eDW_AT_calling_convention = 0x36,
  eDW_AT_count = 0x37,
  eDW_AT_data_member_location = 0x38,
  eDW_AT_decl_column = 0x39,
  eDW_AT_decl_file = 0x3a,
  eDW_AT_decl_line = 0x3b,
  eDW_AT_declaration = 0x3c,
  eDW_AT_discr_list = 0x3d,
  eDW_AT_encoding = 0x3e,
  eDW_AT_external = 0x3f,
  eDW_AT_frame_base = 0x40,
  eDW_AT_friend = 0x41,
  eDW_AT_identifier_case = 0x42,
  eDW_AT_macro_info = 0x43,
  eDW_AT_namelist_item = 0x44,
  eDW_AT_priority = 0x45,
  eDW_AT_segment = 0x46,
  eDW_AT_specification = 0x47,
  eDW_AT_static_link = 0x48,
  eDW_AT_type = 0x49,
  eDW_AT_use_location = 0x4a,
  eDW_AT_variable_parameter = 0x4b,
  eDW_AT_virtuality = 0x4c,
  eDW_AT_vtable_elem_location = 0x4d,
  eDW_AT_lo_user = 0x2000,
  eDW_AT_hi_user = 0x3fff,
}; // enum eDW_AT

enum eDW_FORM {
  eDW_FORM_addr = 0x01,
  eDW_FORM_block2 = 0x3,
  eDW_FORM_block4 = 0x4,
  eDW_FORM_data2 = 0x5,
  eDW_FORM_data4 = 0x6,
  eDW_FORM_data8 = 0x7,
  eDW_FORM_string = 0x8,
  eDW_FORM_block = 0x9,
  eDW_FORM_block1 = 0xa,
  eDW_FORM_data1 = 0xb,
  eDW_FORM_flag = 0xc,
  eDW_FORM_sdata = 0xd,
  eDW_FORM_strp = 0xe,
  eDW_FORM_udata = 0xf,
  eDW_FORM_ref_addr = 0x10,
  eDW_FORM_ref1 = 0x11,
  eDW_FORM_ref2 = 0x12,
  eDW_FORM_ref4 = 0x13,
  eDW_FORM_ref8 = 0x14,
  eDW_FORM_ref_udata = 0x15,
  eDW_FORM_indirect = 0x16,
}; // enum eDW_FORM

enum eDW_LANG {
  eDW_LANG_C89 = 0x0001,
  eDW_LANG_C = 0x0002,
  eDW_LANG_Ada83 = 0x0003,
  eDW_LANG_C_plus_plus = 0x0004,
  eDW_LANG_Cobol74 = 0x0005,
  eDW_LANG_Cobol85 = 0x0006,
  eDW_LANG_Fortran77 = 0x0007,
  eDW_LANG_Fortran90 = 0x0008,
  eDW_LANG_Pascal83 = 0x0009,
  eDW_LANG_Modula2 = 0x000a,
  eDW_LANG_lo_user = 0x8000,
  eDW_LANG_hi_user = 0xffff,
}; // enum eDW_LANG

// DW_AT_encoding
enum eDW_ATE {
  eDW_ATE_address = 0x1,
  eDW_ATE_boolean = 0x2,
  eDW_ATE_complex_float = 0x3,
  eDW_ATE_float = 0x4,
  eDW_ATE_signed = 0x5,
  eDW_ATE_signed_char = 0x6,
  eDW_ATE_unsigned = 0x7,
  eDW_ATE_unsigned_char = 0x8,
  eDW_ATE_lo_user = 0x80,
  eDW_ATE_hi_user = 0xff,
}; // enum eDW_ATE

// DW_AT_accessibility
enum eDW_ACCESS {
  eDW_ACCESS_public = 1,
  eDW_ACCESS_protected = 2,
  eDW_ACCESS_private = 3,
}; // enum eDW_ACCESS

// DW_AT_visibility
enum eDW_VIS {
  eDW_VIS_local = 1,
  eDW_VIS_exported = 2,
  eDW_VIS_qualified = 3,
}; // enum eDW_VIS

// DW_AT_virtuality
enum eDW_VIRT {
  eDW_VIRT_none = 0,
  eDW_VIRT_virtual = 1,
  eDW_VIRT_pure_virtual = 2,
}; // enum eDW_VIRT

// discriminant lists
enum eDW_DSC {
  eDW_DSC_label = 0,
  eDW_DSC_range = 1,
}; // enum eDW_DSC

// array ordering
enum eDW_ORD {
  eDW_ORD_row_major = 0,
  eDW_ORD_col_major = 1,
}; // enum eDW_ORD

// DW_AT_inline
enum eDW_INL {
  eDW_INL_not_inlined = 0,
  eDW_INL_inlined = 1,
  eDW_INL_declared_not_inlined = 2,
  eDW_INL_declared_inlined = 3,
}; // enum eDW_INL

// DW_AT_identifier_case
enum eDW_ID {
  eDW_ID_case_sensitive = 0,
  eDW_ID_up_case = 1,
  eDW_ID_down_case = 2,
  eDW_ID_case_insensitive = 3,
}; // enum eDW_ID

// DW_AT_calling_convention
enum eDW_CC {
  eDW_CC_normal = 1,
  eDW_CC_program = 2,
  eDW_CC_nocall = 3,
  eDW_CC_lo_user = 0x40,
  eDW_CC_hi_user = 0xff,
}; // enum eDW_CC

enum eDW_CHILDREN {
  eDW_CHILDREN_no = 0,
  eDW_CHILDREN_yes = 1,
}; // enum DW_CHILDREN

// Line number related
enum eDW_LNS {
  eDW_LNS_copy = 1,
  eDW_LNS_advance_pc = 2,
  eDW_LNS_advance_line = 3,
  eDW_LNS_set_file = 4,
  eDW_LNS_set_column = 5,
  eDW_LNS_negate_stmt = 6,
  eDW_LNS_set_basic_block = 7,
  eDW_LNS_const_add_pc = 8,
  eDW_LNS_fixed_advance_pc = 9,
}; // enum eDW_LNS

// Line number extended opcodes
enum eDW_LNE {
  eDW_LNE_end_sequence = 1,
  eDW_LNE_set_address = 2,
  eDW_LNE_define_file = 3,
}; // enum eDW_LNE

// Macro information related
enum eDW_MACINFO {
  eDW_MACINFO_define = 1,
  eDW_MACINFO_undef = 2,
  eDW_MACINFO_start_file = 3,
  eDW_MACINFO_end_file = 4,
  eDW_MACINFO_vendor_ext = 255,
}; // enum eDW_MACINFO

// Canonical frame address
enum DW_CFA {
  eDW_CFA_set_loc = 0x01,
  eDW_CFA_advance_loc1 = 0x02,
  eDW_CFA_advance_loc2 = 0x03,
  eDW_CFA_advance_loc4 = 0x04,
  eDW_CFA_offset_extended = 0x05,
  eDW_CFA_restore_extended = 0x06,
  eDW_CFA_undefined = 0x07,
  eDW_CFA_same_value = 0x08,
  eDW_CFA_register = 0x09,
  eDW_CFA_remember_state = 0x0a,
  eDW_CFA_restore_state = 0x0b,
  eDW_CFA_def_cfa = 0x0c,
  eDW_CFA_def_cfa_register = 0x0d,
  eDW_CFA_def_cfa_offset = 0x0e,
  eDW_CFA_nop = 0x0,
  eDW_CFA_lo_user = 0x1c,
  eDW_CFA_hi_user = 0x3f,
}; // enum DW_CFA
// define eDW_CFA_advance_loc  hi2=0x1, lo6=delta
// define eDW_CFA_offset       hi2=0x2, lo6=register
// define eDW_CFA_restore      hi2=0x3, lo6=register

enum eDW_TAG {
 eDW_TAG_array_type = 0x01,  
 eDW_TAG_class_type = 0x02,  
 eDW_TAG_entry_point = 0x03,  
 eDW_TAG_enumeration_type = 0x04,  
 eDW_TAG_formal_parameter = 0x05,  
 eDW_TAG_imported_declaration = 0x06,  
 eDW_TAG_label = 0x08,  
 eDW_TAG_lexical_block = 0x0b,  
 eDW_TAG_member = 0x0d,  
 eDW_TAG_pointer_type = 0x0f,  
 eDW_TAG_reference_type = 0x10,  
 eDW_TAG_compile_unit = 0x11,  
 eDW_TAG_string_type = 0x12,  
 eDW_TAG_structure_type = 0x13,  
 eDW_TAG_subroutine_type = 0x15,  
 eDW_TAG_typedef = 0x16,  
 eDW_TAG_union_type = 0x17,  
 eDW_TAG_unspecified_parameters = 0x18,  
 eDW_TAG_variant = 0x19,  
 eDW_TAG_common_block = 0x1a,  
 eDW_TAG_common_inclusion = 0x1b,  
 eDW_TAG_inheritance = 0x1c,  
 eDW_TAG_inlined_subroutine = 0x1d,  
 eDW_TAG_module = 0x1e,  
 eDW_TAG_ptr_to_member_type = 0x1f,  
 eDW_TAG_set_type = 0x20,  
 eDW_TAG_subrange_type = 0x21,  
 eDW_TAG_with_stmt = 0x22,  
 eDW_TAG_access_declaration = 0x23,  
 eDW_TAG_base_type = 0x24,  
 eDW_TAG_catch_block = 0x25,  
 eDW_TAG_const_type = 0x26,  
 eDW_TAG_constant = 0x27,  
 eDW_TAG_enumerator = 0x28,  
 eDW_TAG_file_type = 0x29,  
 eDW_TAG_friend = 0x2a,  
 eDW_TAG_namelist = 0x2b,  
 eDW_TAG_namelist_item = 0x2c,  
 eDW_TAG_packed_type = 0x2d,  
 eDW_TAG_subprogram = 0x2e,  
 eDW_TAG_template_type_param = 0x2f,  
 eDW_TAG_template_value_param = 0x30,  
 eDW_TAG_thrown_type = 0x31,  
 eDW_TAG_try_block = 0x32,  
 eDW_TAG_variant_part = 0x33,  
 eDW_TAG_variable = 0x34,  
 eDW_TAG_volatile_type = 0x35,  
 eDW_TAG_lo_user = 0x4080,  
 eDW_TAG_hi_user = 0xffff,  
}; // enum eDW_TAG

enum eDW_OP {
  eDW_OP_addr = 0x03,
  eDW_OP_deref = 0x06,
  eDW_OP_const1u = 0x08,
  eDW_OP_const1s = 0x09,
  eDW_OP_const2u = 0x0a,
  eDW_OP_const2s = 0x0b,
  eDW_OP_const4u = 0x0c,
  eDW_OP_const4s = 0x0d,
  eDW_OP_const8u = 0x0e,
  eDW_OP_const8s = 0x0f,
  eDW_OP_constu = 0x10,
  eDW_OP_consts = 0x11,
  eDW_OP_dup = 0x12,
  eDW_OP_drop = 0x13,
  eDW_OP_over = 0x14,
  eDW_OP_pick = 0x15,
  eDW_OP_swap = 0x16,
  eDW_OP_rot = 0x17,
  eDW_OP_xderef = 0x18,
  eDW_OP_abs = 0x19,
  eDW_OP_and = 0x1a,
  eDW_OP_div = 0x1b,
  eDW_OP_minus = 0x1c,
  eDW_OP_mod = 0x1d,
  eDW_OP_mul = 0x1e,
  eDW_OP_neg = 0x1f,
  eDW_OP_not = 0x20,
  eDW_OP_or = 0x21,
  eDW_OP_plus = 0x22,
  eDW_OP_plus_uconst = 0x23,
  eDW_OP_shl = 0x24,
  eDW_OP_shr = 0x25,
  eDW_OP_shra = 0x26,
  eDW_OP_xor = 0x27,
  eDW_OP_skip = 0x28,
  eDW_OP_bra = 0x29,
  eDW_OP_eq = 0x2a,
  eDW_OP_ge = 0x2b,
  eDW_OP_gt = 0x2c,
  eDW_OP_le = 0x2d,
  eDW_OP_lt = 0x2e,
  eDW_OP_ne = 0x2f,
  eDW_OP_lit0 = 0x30, eDW_OP_lit1 = 0x31, eDW_OP_lit2 = 0x32, eDW_OP_lit3 = 0x33,
  eDW_OP_lit4 = 0x34, eDW_OP_lit5 = 0x35, eDW_OP_lit6 = 0x36, eDW_OP_lit7 = 0x37,
  eDW_OP_lit8 = 0x38, eDW_OP_lit9 = 0x39, eDW_OP_lit10 = 0x3a, eDW_OP_lit11 = 0x3b,
  eDW_OP_lit12 = 0x3c, eDW_OP_lit13 = 0x3d, eDW_OP_lit14 = 0x3e, eDW_OP_lit15 = 0x3f,
  eDW_OP_lit16 = 0x40, eDW_OP_lit17 = 0x41, eDW_OP_lit18 = 0x42, eDW_OP_lit19 = 0x43,
  eDW_OP_lit20 = 0x44, eDW_OP_lit21 = 0x45, eDW_OP_lit22 = 0x46, eDW_OP_lit23 = 0x47,
  eDW_OP_lit24 = 0x48, eDW_OP_lit25 = 0x49, eDW_OP_lit26 = 0x4a, eDW_OP_lit27 = 0x4b,
  eDW_OP_lit28 = 0x4c, eDW_OP_lit29 = 0x4d, eDW_OP_lit30 = 0x4e, eDW_OP_lit31 = 0x4f,
  eDW_OP_reg0 = 0x50, eDW_OP_reg1 = 0x51, eDW_OP_reg2 = 0x52, eDW_OP_reg3 = 0x53,
  eDW_OP_reg4 = 0x54, eDW_OP_reg5 = 0x55, eDW_OP_reg6 = 0x56, eDW_OP_reg7 = 0x57,
  eDW_OP_reg8 = 0x58, eDW_OP_reg9 = 0x59, eDW_OP_reg10 = 0x5a, eDW_OP_reg11 = 0x5b,
  eDW_OP_reg12 = 0x5c, eDW_OP_reg13 = 0x5d, eDW_OP_reg14 = 0x5e, eDW_OP_reg15 = 0x5f,
  eDW_OP_reg16 = 0x60, eDW_OP_reg17 = 0x61, eDW_OP_reg18 = 0x62, eDW_OP_reg19 = 0x63,
  eDW_OP_reg20 = 0x64, eDW_OP_reg21 = 0x65, eDW_OP_reg22 = 0x66, eDW_OP_reg23 = 0x67,
  eDW_OP_reg24 = 0x68, eDW_OP_reg25 = 0x69, eDW_OP_reg26 = 0x6a, eDW_OP_reg27 = 0x6b,
  eDW_OP_reg28 = 0x6c, eDW_OP_reg29 = 0x6d, eDW_OP_reg30 = 0x6e, eDW_OP_reg31 = 0x6f,
  eDW_OP_breg0 = 0x70, eDW_OP_breg1 = 0x71, eDW_OP_breg2 = 0x72, eDW_OP_breg3 = 0x73,
  eDW_OP_breg4 = 0x74, eDW_OP_breg5 = 0x75, eDW_OP_breg6 = 0x76, eDW_OP_breg7 = 0x77,
  eDW_OP_breg8 = 0x78, eDW_OP_breg9 = 0x79, eDW_OP_breg10 = 0x7a, eDW_OP_breg11 = 0x7b,
  eDW_OP_breg12 = 0x7c, eDW_OP_breg13 = 0x7d, eDW_OP_breg14 = 0x7e, eDW_OP_breg15 = 0x7f,
  eDW_OP_breg16 = 0x80, eDW_OP_breg17 = 0x81, eDW_OP_breg18 = 0x82, eDW_OP_breg19 = 0x83,
  eDW_OP_breg20 = 0x84, eDW_OP_breg21 = 0x85, eDW_OP_breg22 = 0x86, eDW_OP_breg23 = 0x87,
  eDW_OP_breg24 = 0x88, eDW_OP_breg25 = 0x89, eDW_OP_breg26 = 0x8a, eDW_OP_breg27 = 0x8b,
  eDW_OP_breg28 = 0x8c, eDW_OP_breg29 = 0x8d, eDW_OP_breg30 = 0x8e, eDW_OP_breg31 = 0x8f,
  eDW_OP_regx = 0x90,
  eDW_OP_fbreg = 0x91,
  eDW_OP_bregx = 0x92,
  eDW_OP_piece = 0x93,
  eDW_OP_deref_size = 0x94,
  eDW_OP_xderef_size = 0x95,
  eDW_OP_nop = 0x96,
  eDW_OP_lo_user = 0xe0,
  eDW_OP_hi_user = 0xff,
}; // enum eDW_OP
}; // namespace Dwarf

using namespace Dwarf;

const char* toStr(eDW_OP dw_op_);
const char* toStr(ICPlace icp_);
const char* toStr(IRLinkage irl_);
const char* toStr(IRSignedness irs_);
const char* toStr(IRBIRId irbir_);
const char* toStr(NTType ntType_);
const char* toStr(ShiftType shiftType_);
const char* toStr(CNTType condType_);
const char* toStr(DWARF_LNSDir lnsDir_);
ostream& operator << (ostream& o, IRSignedness irSignedness_);
ostream& operator << (ostream& o, ICPlace icPlace_);
ostream& operator << (ostream& o, NTType ntType_);
ostream& operator << (ostream& o, DWARF_LNSDir lnsDir_);

#endif

// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __FORWARDDECLS_HPP__
#define __FORWARDDECLS_HPP__

//! \todo M Design: It would be nice to generate this file automatically from
//!       sources and headers. Do not forget to process multi line template
//!       class declarations.

//! \todo L Design: Perhaps typedefs for short hand notations should also be included.

// DWARF related
class DW_TAG;
class DW_LOC_OP; // Base class for location list expressions.
class ULEB128;
class SLEB128;
class DW_AT;
class DW_Value;
class DIE;
class DW_DIE;
class CFInst;
class DwarfSection;
class DIEBase;
class ASMSection;

class Options;
class GenericOptions;
class Address;
class AddrLTConst;
class AddrLTConstDiff;
class AddrDiff;
class AddrConstDiff;
class AddrReg;
class AddrStack;
class Anatrop;
class AnatropManager;
class AnyNT;
class AsmSequenceItem;
class AsmSeqData;
class AsmSeqLabel;
class ASMDesc;
class BigInt;
class BigReal;
class CallingConvention;
class CFG;
class CGContext;
class CGFuncContext;
class ConditionNT;
class DebugTrace;
class DFAIRLiveness;
class DFAInstLiveness;
class DFAInstReachingDefs;
class DTCCommandToken;
class DTConfig;
class DTContext;
class DwarfProgDIE;
class DwarfModuleDIE;
class DwarfFuncDIE;
class DwarfStmtDIE;
class DwarfBBDIE;
class DwarfObjDIE;
class DF_FDE;
class EDGC2HIR;
class ExprBEPair;
class ExprRule;
class Extract;
namespace HTML {
  class HTMLNode;
};
class HWDescription;
class HWReg;
class IAST2HIR;
class ICBB;
class ICStmt;
class IDebuggable;
class IEDGC2HIR;
class IgnoreNT;
class InsertContext;
class Instruction;
class IntConstNT;
class IR;
class IRBB;
class IRBBAny;
class IRBuilder;
class IRBuilderHelper;
class IRDUUDChains;
class IREAdd;
class IREAddrConst;
class IREAddrOf;
class IREANeg;
class IREAShiftRight;
class IREBAnd;
class IREBinary;
class IREBitWrite;
class IREBNeg;
class IREBoolConst;
class IREBOr;
class IREBXOr;
class IRECast;
class IREConst;
class IRECmp;
class IREDeref;
class IREDiv;
class IREEq;
class IREGe;
class IREGt;
class IREIntConst;
class IRELAnd;
class IRELe;
class IRELOr;
class IRELShiftRight;
class IRELt;
class IREMember;
class IREMod;
class IREMul;
class IRENe;
class IRENot;
class IRENull;
class IREPtrAdd;
class IREPtrSub;
class IREQuo;
class IRERealConst;
class IRERem;
class IREShiftLeft;
class IREStrConst;
class IRESub;
class IRESubscript;
class IREUnary;
class IRExpr;
class IRExprAny;
class IRExprAnyRef;
class IRExprBool;
class IRExprBoolPtr;
class IRExprBoolRef;
class IRExprBuilder;
class IRExprInt;
class IRExprIntPtr;
class IRExprIntRef;
class IRExprList;
class IRExprObj;
class IRExprPtr;
class IRExprPtrRef;
class IRExprReal;
class IRExprRealPtr;
class IRExprRealRef;
class IRExprRef;
class IRFunction;
class IRFunctionDecl;
class IRLoop;
class IRModule;
class IROBitField;
class IRObject;
class IROField;
class IROGlobal;
class IROTmpGlobal;
class IROLocal;
class IROParameter;
class IRORelocSymbol;
class IROTmp;
class IRProgram;
class IRSAfterLastOfBB;
class IRSAssign;
class IRSBeforeFirstOfBB;
class IRSBuiltInCall;
class IRSCall;
class IRSDynJump;
class IRSEpilog;
class IRSEval;
class IRSFCall;
class IRSJump;
class IRSIf;
class IRSLabel;
class IRSNull;
class IRSPCall;
class IRSProlog;
class IRSReturn;
class IRSSwitch;
class IRStmt;
class IRTArray;
class IRTBool;
class IRTFunc;
class IRTInt;
class IRTPtr;
class IRTReal;
class IRTree;
class IRTStruct;
class IRTUnion;
class IRTVoid;
class IRType;
class IRTypeSetSort;
class MemoryNT;
class NonTerminal;
class NullNT;
class Opt;
class OptJoinBBs;
class OptRemoveNoPredBBs;
class OptRemoveNops;
class OptShortCutJumps;
class Predicate;
class PredicateDBCHelper;
class PredicateSortHelper;
class RealConstNT;
class RegAllocRule;
class Register;
class RegisterAllocator;
class RegisterNT;
class RegSet;
class Rule;
class Statistics;
class ShiftOpNT;
class StmtBEPair;
class StmtRule;
class Target;
class TestTarget;
class tInt;
class tReal;
class tristate;
class VirtReg;
template<class T> class Invalid;
template<class P, class D> class DFA;

namespace InstructionSet {
  class InstructionSetDefinition;
  class InstSetDef_Template;
  class InstSetDef_Op;
}
using namespace InstructionSet;
//! \brief Typedef of IRExpr to differentiate relocation expression types from
//!        HIR expressions.
typedef IRExpr IRRelocExpr;
//! \brief Typedef of IRExpr to differentiate pattern expression types from
//!        HIR expressions.
typedef IRExpr IRPatExpr;

typedef RegSet DefRegSet;
typedef RegSet UseRegSet;
//! \brief Set of HW registers.
typedef RegSet HWRegSet;

typedef BigInt tBigInt;

#endif

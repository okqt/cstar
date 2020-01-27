// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __ARM_HWDEF_HPP__
#include "arm_hwdef.hpp"
#endif
#ifndef __DEBUGINFO_DWARF_HPP__
#include "debuginfo_dwarf.hpp"
#endif
#ifndef __OPTIONS_HPP__
#include "options.hpp"
#endif
#ifndef __STATS_HPP__
#include "stats.hpp"
#endif
#ifndef __AST2HIR_HPP__
#include "ast2hir.hpp"
#endif
#ifndef __ANATROP_HPP__
#include "anatrop.hpp"
#endif
// include lcc headers
#undef list
#undef tree
#define list tag_list
#define tree tag_tree
extern "C" {
#include "c.h"
}
#undef list
#undef tree
#undef debug

//! \file lcc2hir.cpp
//! \brief This file contains sources related to LCC C front-end output conversion in to HIR.

//! \todo M Design: add option setting in the debug/trace config file.

enum eLCCType {
  eINT=INT, eFLOAT=FLOAT, eDOUBLE=DOUBLE, eSTRUCT=STRUCT, eUNSIGNED=UNSIGNED,
  eCHAR=CHAR, eSHORT=SHORT, ePOINTER=POINTER, eFUNCTION=FUNCTION, eVOID=VOID,
  eLONG=LONG, eUNION=UNION, eARRAY=ARRAY, eENUM=ENUM, eCONST=CONST,
  eVOLATILE=VOLATILE, eCONSTVOL=CONST+VOLATILE
}; // enum eLCCType

enum eLCCOperator {
  eCALL=CALL, eLE=LE, eEQ=EQ, eNE=NE, eLT=LT, eGE=GE, eGT=GT, eASGN=ASGN, eJUMP=JUMP,
  eRET=RET, eARG=ARG, eLABEL=LABEL, eADD=ADD, eSUB=SUB, eMUL=MUL, eMOD=MOD,
  eDIV=DIV, eBOR=BOR, eBXOR=BXOR, eBCOM=BCOM, eNEG=NEG, eCNST=CNST,
  eBAND=BAND, eINDIR=INDIR, eADDRF=ADDRF, eADDRG=ADDRG, eADDRL=ADDRL,
  eCVF=CVF, eCVI=CVI, eCVU=CVU, eCVP=CVP, eLSH=LSH, eRSH=RSH, eLOAD=LOAD,
  eVREG=VREG
}; // enum eLCCOperator

// Debug utilities
static std::string toStr(Node node_, hFSInt32 indent_ = 0);
static std::string toStr(Symbol symbol_, hFSInt32 indent_ = 0);
static std::string toStr(Type type_, bool ref_=false);
static std::string toStr(Field field_);
static std::string toStr(Coordinate coordinate_);
static std::string toStr(int suffix_, const Value& value_);
static std::string indent(hFSInt32 indent_);
static const bool scDumpRef = true;

static std::string 
indent(hFSInt32 indent_) {
  std::string retVal;
  for (hFSInt32 c(0); c < indent_; ++ c) {
    retVal += "  ";
  } // for
  return retVal;
} // indent

static std::string 
toStr(Field field_) {
  ostrstream retVal;
  retVal << toStr(field_->type, scDumpRef) << " " << field_->name << " @" << field_->offset;
  if (field_->lsb != 0) {
    retVal << " : " << field_->bitsize << ", lsb=" << field_->lsb;
  } // if
  retVal << ends;
  return retVal.str();
} // toStr(Field)

static std::string
toStr(int suffix_, const Value& val_) {
  ostrstream retVal;
  retVal << "Val(";
  if (suffix_ == eFLOAT) {
    retVal << "F|" << val_.d;
  } else if (suffix_ == eINT) {
    retVal << "I|" << val_.i;
  } else if (suffix_ == eUNSIGNED) {
    retVal << "U|" << val_.u;
  } else if (suffix_ == ePOINTER) {
    retVal << "P|" << val_.p;
  } else if (suffix_ == eVOID) {
    retVal << "V|" << val_.g;
  } else if (suffix_ == eSTRUCT) {
    retVal << "S|" << val_.g;
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  retVal << ")" << ends;
  return retVal.str();
} // toStr(int, const Value&)

static SrcLoc
lcc2SrcLoc(const Coordinate& loc_) {
  SrcLoc retVal(loc_.file != 0 ? loc_.file : "", loc_.y, loc_.x);
  return retVal;
} // lcc2SrcLoc

static std::string
toStr(Type type_, bool ref_) {
  REQUIREDMSG(type_ != 0, ieStrParamValuesDBCViolation);
  ostrstream retVal;
  retVal << "Type{";
  if (ref_ == true) {
    retVal << type_;
  } else {
    if (type_->op != eFUNCTION) {
      retVal << "al:" << type_->align << ", sz:" << type_->size;
    } // if
    if (type_->op == eFLOAT) {retVal << ", FLOAT";}
    if (type_->op == eDOUBLE) {retVal << ", DOUBLE";}
    if (type_->op == eCHAR) {retVal << ", CHAR";}
    if (type_->op == eSHORT) {retVal << ", SHORT";}
    if (type_->op == eINT) {retVal << ", INT";}
    if (type_->op == eUNSIGNED) {retVal << ", UNSIGNED";}
    if (type_->op == ePOINTER) {
      retVal << ", POINTER<" << toStr(type_->type) << ">";
    } // if
    if (type_->op == eVOID) {retVal << ", VOID";}
    if (type_->op == eSTRUCT) {
      retVal << ", STRUCT " << type_->u.sym->name << "{" << endl;
      for (Field lField(type_->u.sym->u.s.flist); lField != 0; lField = lField->link) {
        retVal << "  " << toStr(lField) << ";" << endl;
      } // for
      retVal << "}";
    } // if
    if (type_->op == eUNION) {retVal << ", UNION";}
    if (type_->op == eARRAY) {
      retVal << ", ARRAY<" << toStr(type_->type) << ">";
    } // if
    if (type_->op == eENUM) {retVal << ", ENUM";}
    if (type_->op == eLONG) {retVal << ", LONG";}
    if (type_->op == eCONST) {retVal << ", CONST " << toStr(type_->type);}
    if (type_->op == eVOLATILE) {retVal << ", VOLATILE " << toStr(type_->type);}
    if (type_->op == eCONST+eVOLATILE) {retVal << ", CONSTVOL";}
    if (type_->op == eFUNCTION) {
      retVal << "FUNCTION(";
      retVal << "<" << toStr(type_->type) << "> ";
      Type* lOperands(type_->u.f.proto);
      hFInt32 lParamIndex(0);
      if (type_->u.f.oldstyle == 1) {
        retVal << "<oldstyle>";
      } else {
        while (lOperands[lParamIndex] != 0) {
          if (lParamIndex != 0 && lOperands[lParamIndex] == voidtype) {
            retVal << "...";
          } else {
            retVal << toStr(lOperands[lParamIndex]);
          } // if
          if (lOperands[lParamIndex+1] != 0) {
            retVal << ", ";
          } // if
          ++ lParamIndex;
        } // while
      } // if
      retVal << ")";
    } // if
  } // if
  retVal << "}" << ends;
  return retVal.str();
} // toStr(Type)

static std::string 
toStr(Coordinate coordinate_) {
  ostrstream retVal;
  retVal << "SrcLoc{"; 
  if (coordinate_.file) {
    retVal << coordinate_.file << ", "; 
  } else {
    retVal << "<null>, ";
  } // if
  retVal << "L" << coordinate_.y << ", C" << coordinate_.x << "}" << ends;
  return retVal.str();
} // toStr(Coordinate)

static std::string 
toStr(Symbol symbol_, hFSInt32 indent_) {
  ostrstream retVal;
  for (hFSInt32 c(0); c < indent_; ++ c) {
    retVal << "  ";
  } // for
  retVal << "Symbol{" << symbol_;
  if (!symbol_) {
    retVal << "<null>";
  } else {
    if (!symbol_->defined) {
      retVal << "D ";
    } else {
      retVal << " ";
    } // if
    if (symbol_->scope == LABELS) {
      retVal << "Label:" << symbol_->name;
    } else if (symbol_->scope == CONSTANTS) {
      retVal << "Const:";
      if (isscalar(symbol_->type)) {
        retVal << symbol_->name;
      } else {
        int lSuffix;
        if (isfloat(symbol_->type) == true) {
          retVal << toStr(eFLOAT, symbol_->u.c.v);
        } else {
          retVal << toStr(eINT, symbol_->u.c.v);
        } // if
      } // if
    } else {
      retVal << symbol_->name;
      if (!symbol_->defined) {
        retVal << ", " << toStr(symbol_->src) << ", ";
        if (!symbol_->type) {
          retVal << "type<null>";
        } else {
          retVal << toStr(symbol_->type);
        } // if
      } // if
      if (symbol_->scope == GLOBAL) {
        retVal << ", GLOB";
      } else if (symbol_->scope == PARAM) {
        retVal << ", PARAM";
      } else {
        retVal << ", LOCAL(" << symbol_->scope << ")";
      } // if
      if (!symbol_->defined) {
        if (symbol_->sclass == STATIC) {
          retVal << ", STATIC";
        } else if (symbol_->sclass == AUTO) {
          retVal << ", AUTO";
        } else if (symbol_->sclass == EXTERN) {
          retVal << ", EXT";
        } else if (symbol_->sclass == REGISTER) {
          retVal << ", REG";
        } // if
        if (symbol_->structarg == 1) {
          retVal << ", strarg";
        } // if
        if (symbol_->addressed == 1) {
          retVal << ", addred";
        } // if
        if (symbol_->temporary == 1) {
          retVal << ", temp";
        } // if
        if (symbol_->generated == 1) {
          retVal << ", gened";
        } // if
      } // if
    } // if
  } // if
  retVal << "}" << ends;
  return retVal.str();
} // toStr(Symbol, hFSInt32)

class LCC2HIR : public IAST2HIR {
public:
  LCC2HIR(CGContext* cgContext_);
  virtual ~LCC2HIR() {}
  //! \todo M Design: It seems this function need not be in the base class,
  //!       front-ends have different interfaces to the backends.
  virtual void ConvertMain(void* ast) {
    ASSERTMSG(0, ieStrFuncShouldNotBeCalled);
  } // LCC2HIR::ConvertMain
  void OnProgBeg(int argc_, char* argv_[]);
  //! \brief Called by lcc front-end at the end of compilation.
  void OnProgEnd();
  //! \brief Called by lcc front-end whenever a new symbol is created by front-end.
  //! \param lccCall_ true/scLCCCall if called directly from LCC callback.
  void OnDefSymbol(Symbol symbol_, bool lccCall_);
  //! \brief Called by lcc front-end whenever a global external symbol is seen.
  //! \note Called before the symbol is defined.
  void OnExport(Symbol symbol_);
  //! \brief emit the code to define a global symbol.
  //! \param lccCall_ true/scLCCCall if called directly from LCC callback.
  void OnGlobal(Symbol symbol_, bool lccCall_);
  //! \brief Called for local variables.
  void OnLocal(Symbol symbol_);
  //! \brief Called by lcc front-end whenever a global symbol is imported by front-end.
  void OnImport(Symbol symbol_);
  //! \brief notifies segment change.
  void OnSegment(int seg_);
  void OnDefAddress(Symbol p_);
  void OnDefConst(int suffix_, int size_, Value v_);
  void OnDefString(int n_, char* s_);
  void OnSpace(int n_);
  //! \brief Called by lcc at the end of each function.
  //! \param ncalls_ can be ignored for our backend.
  void OnFunc(Symbol func_, Symbol caller_[], Symbol callee_[], int ncalls_);
  void OnEmit(Node node_);
  Node OnGen(Node& node_, bool onlyOne_);
  //! \brief Called when a block scope starts.
  void OnBlockBeg(Env* e_);
  //! \brief Called when a block scope ends.
  void OnBlockEnd(Env* e_);
  void OnStabBlock(int brace_, int lev_, Symbol* symbol_) {
    BMDEBUG3("LCC2HIR::OnStabBlock", brace_, lev_);
    EMDEBUG0();
  } // LCC2HIR::OnStabBlock
  void OnStabFEnd(Symbol symbol_, int lineNum_) {
    BMDEBUG3("LCC2HIR::OnStabFEnd", toStr(symbol_), lineNum_);
    EMDEBUG0();
  } // LCC2HIR::OnStabFEnd
  void OnStabEnd(Coordinate* cp, Symbol p, Coordinate** cpp, Symbol* sp, Symbol* stab) {
    BMDEBUG1("LCC2HIR::OnStabEnd");
    EMDEBUG0();
  } // LCC2HIR::OnStabEnd
  void OnStabType(Symbol symbol_) {
    BMDEBUG2("LCC2HIR::OnStabType", toStr(symbol_));
    EMDEBUG0();
  } // LCC2HIR::OnStabType
  void OnStabSym(Symbol symbol_) {
    BMDEBUG2("LCC2HIR::OnStabSym", toStr(symbol_));
    EMDEBUG0();
  } // LCC2HIR::OnStabSym
  void OnStabInit(char* file_, int argc_, char* argv_[]) {
    BMDEBUG3("LCC2HIR::OnStabInit", file_, argc_);
    if (file_ != 0) {
      mFileName = file_;
    } // if
    EMDEBUG0();
  } // LCC2HIR::OnStabInit
  void OnStabLine(Coordinate* coord_) {
    BMDEBUG2("LCC2HIR::OnStabLine", toStr(*coord_));
    if (coord_->file != 0) {
      mFileName = coord_->file;
    } // if
    EMDEBUG0();
  } // LCC2HIR::OnStabLine
  virtual char* debug(const DTContext& context, ostream& toStr) const;
public: // Member data
  static map<Node, bool> smDTNodeGened; //!< Keeps track of generated nodes.
  static const bool scLCCCall = true;
  static const bool scNotLCCCall = false;
private: // Member functions
  void addInit(IRExpr* init_) {
    if (pred.pIsArray(mCurrGlobal->GetType()) == true || pred.pIsStructUnion(mCurrGlobal->GetType())) {
      IRExpr* lInits(extr.eGetInits(mCurrGlobal));
      if (pred.pIsInvalid(lInits)) {
        lInits = new IRExprList;
      } // if
      static_cast<IRExprList*>(lInits)->AddExpr(init_);
      mIRBuilder->irbSetInits(mCurrGlobal, lInits);
    } else {
      mIRBuilder->irbSetInits(mCurrGlobal, init_);
    } // if
  } // LCC2HIR::addInit
  bool isFuncCall(Node node_) {
    return generic(node_->op) == eASGN && generic(node_->kids[1]->op) == eCALL;
  } // LCC2HIR::isFuncCallNode
  bool isCall(Node node_) {
    return isFuncCall(node_) || generic(node_->op) == eCALL;
  } // LCC2HIR::isCallNode
  IRType* getType(Node node_);
  //! \todo M Design: Develop a mechanism for generating, forming target compatible symbol names.
  std::string getName(Symbol symbol_);
  StmtBEPair convertStmtCall(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_);
  StmtBEPair convertStmtAssign(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_);
  StmtBEPair convertStmtIf(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_);
  StmtBEPair convertStmtJump(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_);
  StmtBEPair convertStmtLabel(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_);
  StmtBEPair convertStmtReturn(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_);
  void insertPendingJumps();
  IRType* convertType(Type type_);
  ExprBEPair convertExprNode(Node& node_, const ICStmt& ic_);
  StmtBEPair convertStmtNode(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_);
  IRExpr* getConst(Symbol symbol_);
  IRObject* getObject(Symbol symbol_);
private: // Member data
  IRBuilder* mIRBuilder;
  IRBuilderHelper* mIRHBuilder;
  tInt* mSize_t; //!< size_t of current target.
  tInt* ptfInt8; //!< A fast target integer for increment/decrement.
  tReal* ptReal32; //!< A 32 bits real number for increment/decrement.
  tReal* ptReal64; //!< A 64 bits real number for increment/decrement.
  Target* mTarget; //!< The target for compilation.
  IRModule* mIRModule; //!< There is only one module.
  map<Type, IRType*> mLCC2irTypes;
  map<Symbol, IRExpr*> mLCC2irConst;
  map<Symbol, IRObject*> mLCC2irObj;
  map<Field, IROField*> mLCC2irFields;
  set<Symbol> mExports;
  set<Symbol> mImports;
  map<Symbol, IRSLabel*> mLCC2irLabels;
  //! list of Jumps that needs to be inserted at the end of each function.
  vector<pair<ICStmt, Symbol> > mJumps;
  ICStmt mCurrICStmt;
  IROGlobal* mCurrGlobal; //!< The Global that is created in last OnGlobal call.
  std::string mFileName; //!< Input file name.
}; // class LCC2HIR

map<Node, bool> LCC2HIR::smDTNodeGened;
static LCC2HIR* lcc2hir;

//! \brief Called by lcc front-end at the beginning of compilation.
void 
lcc_progbeg(int argc_, char* argv_[]) {
  stats = &Singleton<Statistics>::Obj();
  { // Load config file immediately after the debugTrace creation.
    debugTrace = &Singleton<DebugTrace>::Obj();
    if (DEBUGBUILD) {
      dtConfig.LoadFromFile("");
    } // if
  } // block
  irBuilder = &Singleton<IRBuilder>::Obj();
  irhBuilder = irBuilder->GetHelper();
  anatropManager = &Singleton<AnatropManager>::Obj();
  irProgram = &Singleton<IRProgram>::Obj();
  // call the debug/trace after the object is initialized.
  BDEBUG1("lcc_progbeg");
  IRExprList::exprListVoidType = new IRTVoid();
  IRENull::nullVoidType = new IRTVoid();
  IRECmp::cmpBoolType = new IRTBool();
  IREBoolConst::boolConstBoolType = new IRTBool();
  //! \todo H Design: Need to create a char type?
  IREStrConst::strConstStrType = new IRTPtr(new IRTInt(8, 0, IRSUnsigned));
  IRBuilder::smSetDIEBuilder(new DwarfDIEBuilder);
  cout << "Backend is called!" << endl;
  Target* lTarget(new TestTarget());
  Target::SetTarget(lTarget);
  ARMHW* lARMDescription(new ARMHW(lTarget));
  lARMDescription->Initialize();
  ASMDesc* lASMDesc(new GNUASMDesc());
  CGContext* lCGContext(new CGContext(lARMDescription, &Singleton<CallConvGNUEABI>::Obj(), 
    new RALocal, lASMDesc));
  lCGContext->SetDefFuncContext(new CGFuncContext(*lCGContext, new RALocal, consts.cMustMatch, consts.cStackDownwards));
  irProgram->SetCGContext(lCGContext);
  { // Define and set the options
    options.addBoolOption("debuginfo", consts.cDefaultIsTrue);
    options.addBoolOption("execute_triggers", consts.cDefaultIsTrue);
    options.addEnumOption("debugformat", "Dwarf2", Options::scDefault);
    options.addEnumOption("debugformat", "none");
    // Set them
    options.obSet("debuginfo", false);
    options.oeSet("debugformat", "Dwarf2");
  } // block
  anatropManager->RegisterAnatrops();
  {
    lcc2hir = new LCC2HIR(lCGContext);
    IREPtrSub::ptrDiffType = Singleton<IRBuilder>::Obj().irbtGetInt(Target::GetTarget()->Get_ptrdiff_t());
    lcc2hir->OnProgBeg(argc_, argv_);
  } // block
  EDEBUG0();
  return;
} // lcc_progbeg

void lcc_progend() {lcc2hir->OnProgEnd();}
void lcc_defsymbol(Symbol symbol_) {lcc2hir->OnDefSymbol(symbol_, LCC2HIR::scLCCCall);}
void lcc_export(Symbol symbol_) {lcc2hir->OnExport(symbol_);}
void lcc_import(Symbol symbol_) {lcc2hir->OnImport(symbol_);}
void lcc_global(Symbol symbol_) {lcc2hir->OnGlobal(symbol_, LCC2HIR::scLCCCall);}
void lcc_local(Symbol symbol_) {lcc2hir->OnLocal(symbol_);}
void lcc_segment(int seg_) {lcc2hir->OnSegment(seg_);}
void lcc_defaddress(Symbol p_) {lcc2hir->OnDefAddress(p_);}
void lcc_defconst(int suffix_, int size_, Value v_) {lcc2hir->OnDefConst(suffix_, size_, v_);}
void lcc_defstring(int n_, char* s_) {lcc2hir->OnDefString(n_, s_);}
void lcc_space(int n_) {lcc2hir->OnSpace(n_);}
void lcc_func(Symbol func_, Symbol caller_[], Symbol callee_[], int ncalls_)
{lcc2hir->OnFunc(func_, caller_, callee_, ncalls_);}
void lcc_emit(Node node_) {lcc2hir->OnEmit(node_);}
Node lcc_gen(Node node_) {lcc2hir->OnGen(node_, false);}
void lcc_blockbeg(Env* e_) {lcc2hir->OnBlockBeg(e_);}
void lcc_blockend(Env* e_) {lcc2hir->OnBlockEnd(e_);}
void lcc_stabline(Coordinate* coord_) {lcc2hir->OnStabLine(coord_);}
void lcc_stabinit(char* file_, int argc_, char* argv_[]) {lcc2hir->OnStabInit(file_, argc_, argv_);}
void lcc_stabblock(int brace_, int lev_, Symbol* symbol_) {lcc2hir->OnStabBlock(brace_, lev_, symbol_);}
void lcc_stabfend(Symbol symbol_, int lineNum_) {lcc2hir->OnStabFEnd(symbol_, lineNum_);}
void lcc_stabend(Coordinate* cp, Symbol p, Coordinate** cpp, Symbol* sp, Symbol* stab) {lcc2hir->OnStabEnd(cp, p, cpp, sp, stab);}
void lcc_stabtype(Symbol symbol_) {lcc2hir->OnStabType(symbol_);}
void lcc_stabsym(Symbol symbol_) {lcc2hir->OnStabSym(symbol_);}

void LCC2HIR::
OnProgBeg(int argc_, char* argv_[]) {
  BMDEBUG2("LCC2HIR::OnProgBeg", argc_);
  for (hFUInt32 c(1); c < argc_; ++ c) {
    if (argv_[c][0] != '-') {
      mFileName = argv_[c];
      break;
    } // if
  } // for
  EMDEBUG0();
} // LCC2HIR::OnProgBeg

void LCC2HIR::
OnProgEnd() {
  BMDEBUG1("LCC2HIR::OnProgEnd");
  CGContext& lCGContext(*GetCGContext());
  { // Calling convention processing should be done by anatrop manager, for the time being do it here.
////    anatropManager->Execute("Inliner", irProgram);
    anatropManager->Execute("ConstFold", irProgram);
    anatropManager->Execute("UnreachableCode", irProgram);
    anatropManager->Execute("RemoveNops", irProgram);
    anatropManager->Execute("CodeStraigthening", irProgram);
    anatropManager->Execute("JumpOptims", irProgram);
    anatropManager->Execute("RemoveCasts", irProgram);
    anatropManager->Execute("LFwdSubst", irProgram);
    anatropManager->Execute("GFwdSubst", irProgram);
    anatropManager->Execute("LCopyProp", irProgram);
    anatropManager->Execute("GCopyProp", irProgram);
    anatropManager->Execute("DeadCodeElim", irProgram);
    anatropManager->Execute("UnreachableCode", irProgram);
    while (anatropManager->Execute("IfSimplifications", irProgram) != 0) {
      anatropManager->Execute("UnreachableCode", irProgram);
    } // while
    anatropManager->Execute("LoopInversion", irProgram);
    anatropManager->Execute("JumpOptims", irProgram);
    while (anatropManager->Execute("IfSimplifications", irProgram) != 0) {
      anatropManager->Execute("UnreachableCode", irProgram);
    } // while
    anatropManager->Execute("HIRLower", irProgram);

    anatropManager->Execute("IVStrengthReduction", irProgram);
    anatropManager->Execute("IVElimination", irProgram);
    anatropManager->Execute("LoopInvariant", irProgram);
    anatropManager->Execute("LCSE", irProgram);
    anatropManager->Execute("GCSE", irProgram);

    anatropManager->Execute("IfSimplifications", irProgram, GenericOptions().obSet("do_cond_merge", false));
    anatropManager->Execute("ConstFold", irProgram);
    anatropManager->Execute("UnreachableCode", irProgram);
    anatropManager->Execute("RemoveNops", irProgram);

    anatropManager->Execute("StrengthReduction", irProgram);
    anatropManager->Execute("HIRLower", irProgram);
    anatropManager->Execute("CallConv", irProgram);
  } // block
  if (options.obGet("debuginfo") == true) {
    if (options.oeGet("debugformat") == "Dwarf2") {
      // It looks better to have line numbers in increasing order.
      anatropManager->Execute("BBOrder", irProgram, GenericOptions().oeSet("type", "srcloc"));
    } // if
  } // if
  GetCGContext()->GetHWDesc()->GenerateProg(irProgram, lCGContext);
  {
    ASSERTMSG(mFileName != "", ieStrInconsistentInternalStructure);
    std::string lAsmFileName = "/tmp/" + mFileName;
    ofstream lASMFile(std::string(lAsmFileName + ".s").c_str());
    lCGContext.GetASMDesc()->SetReleaseModeDump();
    GetCGContext()->GetHWDesc()->Emit(lASMFile, lCGContext);
    if (options.obGet("debuginfo") == true) {
      dwarf.OnFinish(lASMFile, lCGContext);
    } // if
    // If everything went okay copy the output file to where it should be.
    system((std::string("cp /tmp/") + mFileName + ".s " + mFileName + ".s").c_str());
  } // block
  stats->OnProgEnd(mFileName);
  cout << "Backend has finished!" << endl;
  EMDEBUG0();
} // LCC2HIR::OnProgEnd

void  LCC2HIR::
OnDefSymbol(Symbol symbol_, bool lccCall_) {
  BMDEBUG2("LCC2HIR::OnDefSymbol", toStr(symbol_));
  if (symbol_->scope == CONSTANTS) {
    IRExpr* lConst(0);
    if (symbol_->type->op == eDOUBLE) {
      lConst = mIRBuilder->irbeRealConst(*mTarget->GetRealDouble(), symbol_->u.c.v.d);
    } else if (symbol_->type->op == eFLOAT) {
      lConst = mIRBuilder->irbeRealConst(*mTarget->GetRealSingle(), symbol_->u.c.v.d);
    } else if (symbol_->type->op == ePOINTER) {
      ASSERTMSG(dynamic_cast<IRTPtr*>(convertType(symbol_->type)) != 0, ieStrInconsistentInternalStructure);
      lConst = mIRBuilder->irbeIntConst(static_cast<IRTInt*>(convertType(symbol_->type)), (hFUInt32)symbol_->u.c.v.p);
    } else if (symbol_->type->op == eUNSIGNED) {
      ASSERTMSG(dynamic_cast<IRTInt*>(convertType(symbol_->type)) != 0, ieStrInconsistentInternalStructure);
      lConst = mIRBuilder->irbeIntConst(static_cast<IRTInt*>(convertType(symbol_->type)), symbol_->u.c.v.u);
    } else if (util.uIsOneOf(symbol_->type->op, eINT, eCHAR, eSHORT, eLONG) == true) {
      ASSERTMSG(dynamic_cast<IRTInt*>(convertType(symbol_->type)) != 0, ieStrInconsistentInternalStructure);
      lConst = mIRBuilder->irbeIntConst(static_cast<IRTInt*>(convertType(symbol_->type)), symbol_->u.c.v.i);
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
    ASSERTMSG(lConst != 0, ieStrInconsistentInternalStructure);
    mLCC2irConst[symbol_] = lConst;
  } else if (symbol_->scope == PARAM) {
    if (symbol_->type != 0) {
      if (mLCC2irObj.find(symbol_) == mLCC2irObj.end()) {
        // Find the symbol from the params of the current function by looking at name.
        vector<IROParameter*> lParams(mIRBuilder->GetFuncContext()->GetDeclaration()->GetParams());
        for (hFUInt32 c(0); c < lParams.size(); ++ c) {
          if (lParams[c]->GetName() == symbol_->name) {
            mLCC2irObj[symbol_] = lParams[c];
            break;
          } // if
        } // for
        ASSERTMSG(mLCC2irObj.find(symbol_) != mLCC2irObj.end(), ieStrInconsistentInternalStructure);
      } // if
    } // if
  } else if (symbol_->scope == GLOBAL) {
    if (symbol_->type != 0) {
      if (symbol_->type->op == eFUNCTION) {
        OnGlobal(symbol_, scNotLCCCall);
      } else if (symbol_->generated == 1) {
        OnGlobal(symbol_, scNotLCCCall);
      } else {
        OnGlobal(symbol_, scNotLCCCall);
      } // if
    } // if
  } // if
  if (symbol_->type != 0) {
  } else {
    // LIE: Ignore typeless symbols.
  } // if
  EMDEBUG0();
} // LCC2HIR::OnDefSymbol

void  LCC2HIR::
OnExport(Symbol symbol_) {
  BMDEBUG2("LCC2HIR::OnExport", toStr(symbol_));
  mExports.insert(symbol_);
  EMDEBUG0();
} // LCC2HIR::OnExport

void  LCC2HIR::
OnImport(Symbol symbol_) {
  BMDEBUG2("LCC2HIR::OnImport", toStr(symbol_));
  mImports.insert(symbol_);
  EMDEBUG0();
} // LCC2HIR::OnImport

void LCC2HIR::
OnGlobal(Symbol symbol_, bool lccCall_) {
  BMDEBUG2("LCC2HIR::OnGlobal", toStr(symbol_));
  if (mLCC2irObj.find(symbol_) == mLCC2irObj.end()) {
    IRLinkage lLinkage(IRLInvalid);
    if (mExports.find(symbol_) != mExports.end()) {
      lLinkage = IRLExport;
    } else if (mImports.find(symbol_) != mImports.end() || symbol_->sclass == EXTERN) {
      lLinkage = IRLImport;
    } else if (symbol_->sclass == STATIC) {
      lLinkage = IRLStatic;
    } else if (symbol_->sclass == REGISTER) {
      ASSERTMSG(0, ieStrInconsistentInternalStructure);
    } else if (symbol_->sclass == AUTO) {
      lLinkage = IRLImport;
    } // if
    mLCC2irObj[symbol_] = mIRBuilder->irboGlobal(getName(symbol_), convertType(symbol_->type), lLinkage);
  } // if
  if (lccCall_ == true) {
    mCurrGlobal = static_cast<IROGlobal*>(mLCC2irObj[symbol_]);
  } // if
  EMDEBUG0();
} // LCC2HIR::OnGlobal

void LCC2HIR::
OnLocal(Symbol symbol_) {
  BMDEBUG2("LCC2HIR::OnLocal", toStr(symbol_));
  IROLocal* lLocal(mIRBuilder->irboLocal(getName(symbol_), convertType(symbol_->type)));
  mLCC2irObj[symbol_] = lLocal;
  EMDEBUG0();
} // LCC2HIR::OnLocal

void LCC2HIR::
OnSegment(int seg_) {
  BMDEBUG1("LCC2HIR::OnSegment");
  // LIE: We are not interested in segment information and changes.
  EMDEBUG0();
} // LCC2HIR::OnSegment

void  LCC2HIR::
OnDefAddress(Symbol p_) {
  BMDEBUG2("LCC2HIR::OnDefAddress", toStr(p_));
  addInit(mIRBuilder->irbeAddrConst(mIRBuilder->irbtGetPtr(convertType(p_->type)), getObject(p_), 0));
  EMDEBUG0();
} // LCC2HIR::OnDefAddress

void  LCC2HIR::
OnDefConst(int suffix_, int size_, Value val_) {
  BMDEBUG4("LCC2HIR::OnDefConst", suffix_, size_, toStr(suffix_, val_));
  IRExpr* lConst(0);
  hFInt8 lByteSize(mTarget->GetByteSize());
  if (suffix_ == F) {
    if (size_ == 4) { // float case
      lConst = mIRBuilder->irbeRealConst(*mTarget->GetRealSingle(), val_.d);
    } else if (size_ == 8) { // double
      lConst = mIRBuilder->irbeRealConst(*mTarget->GetRealDouble(), val_.d);
    } // if
  } else if (suffix_ == U) {
    tInt* lIntType(mTarget->GetIntType(size_*lByteSize, size_*lByteSize, IRSUnsigned));
    lConst = mIRBuilder->irbeIntConst(mIRBuilder->irbtGetInt(*lIntType), val_.u);
  } else if (suffix_ == I) {
    tInt* lIntType(mTarget->GetIntType(size_*lByteSize, size_*lByteSize, IRSSigned));
    lConst = mIRBuilder->irbeIntConst(mIRBuilder->irbtGetInt(*lIntType), val_.i);
  } else if (suffix_ == P) {
    lConst = mIRBuilder->irbeAddrConst(mIRBuilder->irbtGetVoidPtr(), (hFUInt32)val_.p);
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  ASSERTMSG(lConst != 0, ieStrInconsistentInternalStructure);
  addInit(lConst);
  EMDEBUG0();
} // LCC2HIR::OnDefConst

void LCC2HIR::
OnDefString(int n_, char* s_) {
  BMDEBUG3("LCC2HIR::OnDefString", n_, s_);
  REQUIREDMSG(!pred.pIsInvalid(mCurrGlobal), ieStrInconsistentInternalStructure);
  addInit(mIRBuilder->irbeStrConst(std::string(s_, n_)));
  EMDEBUG0();
} // LCC2HIR::OnDefString

void  LCC2HIR::
OnSpace(int n) {
  BMDEBUG1("LCC2HIR::OnSpace");
  NOTIMPLEMENTED();
  EMDEBUG0();
} // LCC2HIR::OnSpace

void LCC2HIR::
OnFunc(Symbol func_, Symbol caller_[], Symbol callee_[], int ncalls_) {
  BMDEBUG2("LCC2HIR::OnFunc", toStr(func_));
  IRTFunc* lFuncType(static_cast<IRTFunc*>(convertType(func_->type)));
  ASSERTMSG(dynamic_cast<IRTFunc*>(convertType(func_->type)) != 0, ieStrInconsistentInternalStructure);
  IRFunction* lIRFunc(mIRBuilder->irbFunction(getName(func_), lFuncType, mIRModule, lcc2SrcLoc(func_->src)));
  { // Process parameters.
    for (hFUInt32 c(0); callee_[c] != 0; ++ c) {
      IROParameter* lParam(mIRBuilder->irboParameter(getName(callee_[c]), convertType(callee_[c]->type)));
      mLCC2irObj[callee_[c]] = lParam;
    } // for
  } // block
  { // Do not start inserting at the end of entry basic block,
    // instead create an empty bb between entry and exit and insert in to that.
    IRBB* lEmptyBB(mIRBuilder->irbInsertBB(ICBB(lIRFunc->GetCFG()->GetEntryBB(), ICAfter)));
    IRSLabel* lFirstStmt(mIRHBuilder->irbsLocalLabel("funcBeg", consts.cAddrNotTaken, ICBB(lEmptyBB, ICBegOfBB)));
    mCurrICStmt = ICStmt(lFirstStmt, ICAfter);
  } // block
  // Initialize
  gencode(caller_, callee_);
  // Emit prolog
  // emitcode();
  // Emit epilog
  insertPendingJumps();
  mLCC2irLabels.clear();
  mJumps.clear();
  EMDEBUG0();
  return;
} // LCC2HIR::OnFunc

void LCC2HIR::
OnEmit(Node node_) {
  BMDEBUG2("LCC2HIR::OnEmit", toStr(node_));
  NOTIMPLEMENTED();
  EMDEBUG0();
} // LCC2HIR::OnEmit

StmtBEPair LCC2HIR::
convertStmtCall(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("LCC2HIR::convertStmtCall", toStr(node_), &ic_, &srcLoc_);
  StmtBEPair retVal(ic_);
  // if there is any eARG it comes first, then if function returns a
  // value eASGN comes then the eCALL is attached to the eASGN.
  // eARG or eASGN or both might be missing. There may be any number of 
  // statements coming after the eARG node.
  IRExprList* lArgs(new IRExprList());
  stack<IRExpr*> lArgExprs;
  while (!isCall(node_)) {
    if (generic(node_->op) == eARG) {
      lArgExprs.push(convertExprNode(node_->kids[0], retVal));
    } else {
      mCurrICStmt = retVal;
      OnGen(node_, true);
      retVal = mCurrICStmt;
    } // if
    node_ = node_->link;
  } // while
  while (!lArgExprs.empty()) {
    lArgs->AddExpr(lArgExprs.top());
    lArgExprs.pop();
  } // while
  PDEBUG("LCC2HIR", "detail", " is function call: " << isFuncCall(node_));
  if (isFuncCall(node_) == true) {
    IRExpr* lFuncExpr(convertExprNode(node_->kids[1]->kids[0], retVal));
    IRType* lRetInType(0);
    if (pred.pIsFunc(extr.eGetElemType(lFuncExpr)) == true) {
      lRetInType = extr.eGetFuncTypeOfCallExpr(lFuncExpr)->GetReturnType();
    } else {
      lRetInType = getType(node_->kids[1]);
    } // if
    IRObject* lReturnIn(mIRBuilder->irboTmp(lRetInType, "ret"));
    retVal = mIRBuilder->irbsFCall(lReturnIn, lFuncExpr, lArgs, retVal, SrcLoc());
    { // Assign IR return value in to lcc return value.
      IRExpr* lLHS(mIRBuilder->irbeCast(mIRBuilder->irbtGetPtr(lReturnIn->GetType()), 
        mIRBuilder->irbeAddrOf(getObject(node_->kids[0]->syms[0]))));
      retVal = mIRBuilder->irbsAssign(lLHS, mIRHBuilder->irbeUseOf(lReturnIn), retVal, SrcLoc());
    } // block
  } else {
    ASSERTMSG(generic(node_->op) == eCALL, ieStrInconsistentInternalStructure << 
      " expected eCALL got " << toStr(node_));
    IRExpr* lFuncExpr(convertExprNode(node_->kids[0], retVal));
    retVal = mIRBuilder->irbsPCall(lFuncExpr, lArgs, retVal, SrcLoc());
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertStmtCall

StmtBEPair LCC2HIR::
convertStmtAssign(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("LCC2HIR::convertStmtAssign", toStr(node_), &ic_, &srcLoc_);
  StmtBEPair retVal(ic_);
  // 2S 2K  kids[0].syms[0] = kids[1]
  //! \todo H Design: Implement an anatrop that removes unnecessary casts.
  if (optype(node_->op) == B) {
    IRExpr* lLength(getConst(node_->syms[0]));
    IRExpr* lLeft(convertExprNode(node_->kids[0], ic_));
    IRExpr* lRight(convertExprNode(node_->kids[1]->kids[0], ic_));
    ASSERTMSG(generic(node_->kids[1]->op) == eINDIR && util.uIsOneOf(generic(node_->kids[1]->kids[0]->op), 
      eADDRF, eADDRG, eADDRL), ieStrInconsistentInternalStructure);
    IRExprList* lArgs(new IRExprList(lLeft, lRight, lLength));
    retVal = mIRBuilder->irbsBIPCall(lArgs, IRBIRmemcpy, ic_, srcLoc_);
  } else {
    IRExpr* lRHS(mIRBuilder->irbeCast(getType(node_), convertExprNode(node_->kids[1], ic_)));
    IRExpr* lLHS(mIRBuilder->irbeCast(mIRBuilder->irbtGetPtr(getType(node_)), convertExprNode(node_->kids[0], ic_)));
    retVal = mIRBuilder->irbsAssign(lLHS, lRHS, ic_, SrcLoc());
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertStmtAssign

StmtBEPair LCC2HIR::
convertStmtIf(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("LCC2HIR::convertStmtIf", toStr(node_), &ic_, &srcLoc_);
  REQUIREDMSG(extr.eGetNumOfSuccs(extr.eGetStmt(ic_)->GetBB()) <= 1, ieStrInconsistentInternalStructure);
  StmtBEPair retVal(ic_);
  IRExpr* lCond(0);
  { // Generate the condition expression.
    IRExpr* lLeft(convertExprNode(node_->kids[0], ic_));
    IRExpr* lRight(convertExprNode(node_->kids[1], ic_));
    if (generic(node_->op) == eLT) {
      lCond = mIRBuilder->irbeLt(lLeft, lRight);
    } else if (generic(node_->op) == eLE) {
      lCond = mIRBuilder->irbeLe(lLeft, lRight);
    } else if (generic(node_->op) == eGE) {
      lCond = mIRBuilder->irbeGe(lLeft, lRight);
    } else if (generic(node_->op) == eGT) {
      lCond = mIRBuilder->irbeGt(lLeft, lRight);
    } else if (generic(node_->op) == eNE) {
      lCond = mIRBuilder->irbeNe(lLeft, lRight);
    } else if (generic(node_->op) == eEQ) {
      lCond = mIRBuilder->irbeEq(lLeft, lRight);
    } // if
  } // block
  if (mLCC2irLabels.find(node_->syms[0]) != mLCC2irLabels.end()) {
    IRSLabel* lFalseCase(0);
    IRSLabel* lTrueCase(0);
    lFalseCase = mLCC2irLabels[node_->syms[0]];
    lTrueCase = mIRHBuilder->irbsLocalLabel("cmpbt", consts.cAddrNotTaken, ic_);
    mIRHBuilder->irbsIf(mIRBuilder->irbeInverseCmp(lCond), lTrueCase, lFalseCase, ic_, srcLoc_);
    retVal = lTrueCase;
  } else {
    IRSIf* lIf(mIRHBuilder->irbsConvertToIf(mIRBuilder->irbeInverseCmp(lCond), ic_.GetBB(), srcLoc_));
    mLCC2irLabels[node_->syms[0]] = mIRHBuilder->irbsLocalLabel("cmpf", consts.cAddrNotTaken, 
      ICBB(lIf->GetFalseCaseBB(), ICBegOfBB));
    retVal = mIRHBuilder->irbsLocalLabel("cmpt", consts.cAddrNotTaken, ICBB(lIf->GetTrueCaseBB(), ICBegOfBB));
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertStmtIf

StmtBEPair LCC2HIR::
convertStmtJump(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("LCC2HIR::convertStmtJump", toStr(node_), &ic_, &srcLoc_);
  StmtBEPair retVal(ic_);
  if (generic(node_->kids[0]->op) != eADDRG) {
    retVal += mIRBuilder->irbsDynJump(convertExprNode(node_->kids[0], ic_), ic_, srcLoc_);
  } else if (mLCC2irLabels.find(node_->kids[0]->syms[0]) != mLCC2irLabels.end()) {
    IRStmt* irLabel(mLCC2irLabels[node_->kids[0]->syms[0]]);
    retVal += mIRHBuilder->irbsJump(irLabel, retVal, SrcLoc());
  } else {
    IRStmt* lIRStmt(mIRHBuilder->irbsLocalLabel(std::string("jump_") + getName(node_->kids[0]->syms[0]),
      consts.cAddrNotTaken, ic_));
    retVal = StmtBEPair(lIRStmt);
    mJumps.push_back(make_pair(ICStmt(lIRStmt, ICAfter), node_->kids[0]->syms[0]));
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertStmtJump

StmtBEPair LCC2HIR::
convertStmtLabel(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("LCC2HIR::convertStmtLabel", toStr(node_), &ic_, &srcLoc_);
  StmtBEPair retVal(ic_);
  IRSLabel* lStmt(0);
  if (mLCC2irLabels.find(node_->syms[0]) == mLCC2irLabels.end()) {
    lStmt = mIRHBuilder->irbsLocalLabel(std::string("feLabel_") + getName(node_->syms[0]), consts.cAddrNotTaken, ic_);
    mLCC2irLabels[node_->syms[0]] = lStmt;
  } else {
    lStmt = mLCC2irLabels[node_->syms[0]];
  } // if
  retVal = lStmt;
  mIRHBuilder->irbsJump(mLCC2irLabels[node_->syms[0]], ic_, srcLoc_);
  if (extr.eGetNumOfSuccs(lStmt->GetBB()) == 0) {
    mIRBuilder->irbInsertSucc(lStmt->GetBB(), mIRBuilder->GetFuncContext()->GetCFG()->GetExitBB());
  } // if
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertStmtLabel

StmtBEPair LCC2HIR::
convertStmtReturn(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("LCC2HIR::convertStmtReturn", toStr(node_), &ic_, &srcLoc_);
  StmtBEPair retVal(ic_);
  IRExpr* lRetExpr(0);
  if (optype(node_->op) == eVOID) {
    lRetExpr = mIRBuilder->irbeNull();
  } else {
    lRetExpr = mIRBuilder->irbeCast(extr.eGetRetType(mIRBuilder->GetFuncContext()), convertExprNode(node_->kids[0], ic_));
  } // if
  retVal = mIRBuilder->irbsReturn(lRetExpr, ic_, srcLoc_);
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertStmtReturn

StmtBEPair LCC2HIR::
convertStmtNode(Node& node_, const ICStmt& ic_, const SrcLoc& srcLoc_) {
  BMDEBUG4("LCC2HIR::convertStmtNode", toStr(node_), &ic_, &srcLoc_);
  REQUIREDMSG(node_ != 0, ieStrParamValuesDBCViolation);
  StmtBEPair retVal(ic_);
  if (util.uIsOneOf(generic(node_->op), eCALL, eARG) == true || isFuncCall(node_)) {
    retVal = convertStmtCall(node_, ic_, srcLoc_);
  } else if (util.uIsOneOf(generic(node_->op), eLT, eLE, eGE, eGT, eEQ, eNE) == true) {
    retVal = convertStmtIf(node_, ic_, srcLoc_);
  } else if (generic(node_->op) == eJUMP) {
    retVal = convertStmtJump(node_, ic_, srcLoc_);
  } else if (generic(node_->op) == eRET) {
    retVal = convertStmtReturn(node_, ic_, srcLoc_);
  } else if (generic(node_->op) == eLABEL) {
    retVal = convertStmtLabel(node_, ic_, srcLoc_);
  } else if (generic(node_->op) == eASGN) {
    retVal = convertStmtAssign(node_, ic_, srcLoc_);
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  smDTNodeGened[node_] = true;
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertStmtNode 

ExprBEPair LCC2HIR::
convertExprNode(Node& node_, const ICStmt& ic_) {
  BMDEBUG3("LCC2HIR::convertExprNode", toStr(node_), &ic_);
  REQUIREDMSG(node_ != 0, ieStrParamValuesDBCViolation);
  ExprBEPair retVal(ic_);
  if (util.uIsOneOf(generic(node_->op), eADDRF, eADDRG, eADDRL) == true) {
    retVal = IRExprPtr(mIRBuilder->irbeAddrOf(getObject(node_->syms[0]))).Ref();
  } else if (generic(node_->op) == eADD) {
    IRExpr* lLeft(convertExprNode(node_->kids[0], ic_));
    IRExpr* lRight(convertExprNode(node_->kids[1], ic_));
    if (pred.pIsPtr(lLeft->GetType()) == true) {
      retVal = IRExprPtr(mIRBuilder->irbePtrAdd(lLeft, lRight)).Ref();
    } else if (pred.pIsPtr(lRight->GetType()) == true) {
      retVal = IRExprPtr(mIRBuilder->irbePtrAdd(lRight, lLeft)).Ref();
    } else {
      retVal = IRExprInt(mIRBuilder->irbeAdd(lLeft, lRight)).Ref();
    } // if
  } else if (generic(node_->op) == eSUB) {
    IRExpr* lLeft(convertExprNode(node_->kids[0], ic_));
    IRExpr* lRight(convertExprNode(node_->kids[1], ic_));
    if (pred.pIsPtr(lLeft->GetType()) == true && pred.pIsPtr(lRight->GetType()) == true) {
      retVal = IRExprInt(mIRBuilder->irbePtrSub(lLeft, lRight)).Ref();
    } else if (pred.pIsPtr(lLeft->GetType()) == true) {
      retVal = IRExprPtr(mIRBuilder->irbePtrAdd(lLeft, mIRBuilder->irbeANeg(lRight))).Ref();
    } else if (pred.pIsPtr(lRight->GetType()) == true) {
      retVal = IRExprPtr(mIRBuilder->irbePtrAdd(lRight, mIRBuilder->irbeANeg(lLeft))).Ref();
    } else {
      retVal = IRExprInt(mIRBuilder->irbeSub(lLeft, lRight)).Ref();
    } // if
  } else if (generic(node_->op) == eMUL) {
    retVal = IRExprInt(mIRBuilder->irbeMul(convertExprNode(node_->kids[0], ic_), convertExprNode(node_->kids[1], ic_))).Ref();
  } else if (generic(node_->op) == eMOD) {
    retVal = IRExprInt(mIRBuilder->irbeMod(convertExprNode(node_->kids[0], ic_), convertExprNode(node_->kids[1], ic_))).Ref();
  } else if (generic(node_->op) == eDIV) {
    retVal = IRExprInt(mIRBuilder->irbeDiv(convertExprNode(node_->kids[0], ic_), convertExprNode(node_->kids[1], ic_))).Ref();
  } else if (generic(node_->op) == eBXOR) {
    retVal = IRExprInt(mIRBuilder->irbeBXOr(convertExprNode(node_->kids[0], ic_), convertExprNode(node_->kids[1], ic_))).Ref();
  } else if (generic(node_->op) == eBOR) {
    retVal = IRExprInt(mIRBuilder->irbeBOr(convertExprNode(node_->kids[0], ic_), convertExprNode(node_->kids[1], ic_))).Ref();
  } else if (generic(node_->op) == eBCOM) {
    retVal = IRExprInt(mIRBuilder->irbeBNeg(convertExprNode(node_->kids[0], ic_))).Ref();
  } else if (generic(node_->op) == eINDIR) {
    IRExpr* lExpr(convertExprNode(node_->kids[0], ic_));
    retVal = IRExprAny(mIRBuilder->irbeDeref(mIRBuilder->irbeCast(mIRBuilder->irbtGetPtr(getType(node_)), lExpr))).Ref();
  } else if (generic(node_->op) == eNEG) {
    retVal = IRExprInt(mIRBuilder->irbeANeg(convertExprNode(node_->kids[0], ic_))).Ref();
  } else if (generic(node_->op) == eRSH) {
    IRExpr* lExpr(convertExprNode(node_->kids[0], ic_));
    if (!pred.pIsUnsignedInt(lExpr->GetType())) {
      retVal = IRExprInt(mIRBuilder->irbeAShiftRight(lExpr, convertExprNode(node_->kids[1], ic_))).Ref();
    } else {
      retVal = IRExprInt(mIRBuilder->irbeLShiftRight(lExpr, convertExprNode(node_->kids[1], ic_))).Ref();
    } // if
  } else if (generic(node_->op) == eLSH) {
    retVal = IRExprInt(mIRBuilder->irbeShiftLeft(convertExprNode(node_->kids[0], ic_), convertExprNode(node_->kids[1], ic_))).Ref();
  } else if (generic(node_->op) == eBAND) {
    retVal = IRExprInt(mIRBuilder->irbeBAnd(convertExprNode(node_->kids[0], ic_), convertExprNode(node_->kids[1], ic_))).Ref();
  } else if (generic(node_->op) == eCNST) {
    retVal = IRExprVoid(getConst(node_->syms[0])).Ref();
  } else if (util.uIsOneOf(generic(node_->op), eCVF, eCVI, eCVU, eCVP)) {
    retVal = IRExprAny(mIRBuilder->irbeCast(getType(node_), convertExprNode(node_->kids[0], ic_))).Ref();
  } else {
    ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // if
  smDTNodeGened[node_] = true;
  EMDEBUG1(&retVal);
  return retVal;
} // LCC2HIR::convertExprNode

Node LCC2HIR::
OnGen(Node& node_, bool onlyOne_) {
  BMDEBUG3("LCC2HIR::OnGen", toStr(node_), onlyOne_);
  for (Node& lCurrNode(node_); lCurrNode; lCurrNode = lCurrNode->link) {
    if (util.uIsOneOf(generic(lCurrNode->op), eCALL, eJUMP, eRET, eASGN, eEQ, eARG) == true ||
      util.uIsOneOf(generic(lCurrNode->op), eNE, eLE, eGE, eGT, eLT, eLABEL) == true) {
      StmtBEPair lBEPair(convertStmtNode(lCurrNode, mCurrICStmt, SrcLoc()));
      mCurrICStmt = lBEPair;
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
    if (onlyOne_) {
      break;
    } // if
  } // for
  EMDEBUG0();
} // LCC2HIR::OnGen

void LCC2HIR::
OnBlockBeg(Env* e_) {
  BMDEBUG1("LCC2HIR::OnBlockBeg");
  NOTIMPLEMENTED();
  EMDEBUG0();
} // LCC2HIR::OnBlockBeg

void LCC2HIR::
OnBlockEnd(Env* e_) {
  BMDEBUG1("LCC2HIR::OnBlockEnd");
  NOTIMPLEMENTED();
  EMDEBUG0();
} // LCC2HIR::OnBlockEnd

Interface bccLinuxIR = {
  1, 1, 0,  /* char */
  2, 2, 0,  /* short */
  4, 4, 0,  /* int */
  4, 4, 0,  /* long */
  4, 4, 0,  /* long long */
  4, 4, 1,  /* float */
  8, 4, 1,  /* double */
  8, 4, 1,  /* long double */
  4, 4, 0,  /* T * */
  0, 1, 0,  /* struct */
  1,        /* little_endian */
  0,        /* mulops_calls */
  0,        /* wants_callb */
  1,        /* wants_argb */
  0,        /* left_to_right */
  0,        /* wants_dag */
  0,        /* unsigned_char */
  0, // address is optional and we do not need it.
  lcc_blockbeg,
  lcc_blockend,
  lcc_defaddress,
  lcc_defconst,
  lcc_defstring,
  lcc_defsymbol,
  lcc_emit,
  lcc_export,
  lcc_func,
  lcc_gen,
  lcc_global,
  lcc_import,
  lcc_local,
  lcc_progbeg,
  lcc_progend,
  lcc_segment,
  lcc_space, 
  // debug info interfaces
  lcc_stabblock, // stabblock
  lcc_stabend, // stabend
  lcc_stabfend, // stabfend
  lcc_stabinit, // stabinit
  lcc_stabline, // stabline
  lcc_stabsym, // stabsym
  lcc_stabtype, // stabtype
  {0} // Xinterface
};

char* LCC2HIR::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("LCC2HIR::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    Singleton<DebugTrace>::Obj().dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    Singleton<DebugTrace>::Obj().dumpObjRef(this, context_, toStr_);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // LCC2HIR::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // LCC2HIR::debug

IRType* LCC2HIR::
convertType(Type type_) {
  BMDEBUG2("LCC2HIR::convertType", toStr(type_));
  REQUIREDMSG(type_ != 0, ieStrNonNullParam);
  IRType* retVal(0);
  hFInt8 lByteSize(mTarget->GetByteSize());
  if (mLCC2irTypes.find(type_) != mLCC2irTypes.end()) {
    retVal = mLCC2irTypes[type_];
  } else {
    if (util.uIsOneOf(type_->op, eUNSIGNED, eINT, eCHAR, eSHORT, eLONG) == true) {
      IRSignedness lSign(type_->op == eUNSIGNED ? IRSUnsigned : IRSSigned);
      tInt* lIntType(mTarget->GetIntType(type_->size*lByteSize, type_->align*lByteSize, lSign));
      retVal = mIRBuilder->irbtGetInt(*lIntType);
    } else if (type_->op == ePOINTER) {
      retVal = mIRBuilder->irbtGetPtr(convertType(type_->type));
    } else if (type_->op == eFUNCTION) {
      IRType* lRetType(convertType(type_->type));
      vector<IRType*> lParamTypes;
      bool lIsVarArg(false);
      if (type_->u.f.proto != 0) {
        for (hFUInt32 c(0); type_->u.f.proto[c] != 0; ++ c) {
          if (type_->u.f.proto[c] == voidtype && c != 0) {
            lIsVarArg = true;
          } else {
            lParamTypes.push_back(convertType(type_->u.f.proto[c]));
          } // if
        } // for
      } else {
        lIsVarArg = true;
      } // if
      retVal = mIRBuilder->irbtGetFunc(lRetType, lParamTypes, lIsVarArg, GetCGContext()->GetCallConv());
    } else if (type_->op == eENUM) {
      retVal = convertType(type_->type);
    } else if (type_->op == eARRAY) {
      // type_->type is element type
      IRType* lElemType(convertType(type_->type));
      retVal = mIRBuilder->irbtGetArray(lElemType, type_->size*lByteSize / lElemType->GetSize(), 
        lByteSize*type_->align);
    } else if (type_->op == eSTRUCT || type_->op == eUNION) {
      vector<IROField*> lFields;
      // Due to the possible member fields pointing to the currently processed
      // struct we need to first create the struct, and then start adding the fields.
      std::string lName(type_->u.sym->name);
      IRTStruct* lStrType(type_->op == eUNION ? 
        mIRBuilder->irbtGetUnion(lName, lFields, type_->size * lByteSize, type_->align * lByteSize) :
        mIRBuilder->irbtGetStruct(lName, lFields, type_->size * lByteSize, type_->align * lByteSize));
      mLCC2irTypes[type_] = lStrType;
      // type_->u.sym->name tag name, type_->u.sym->u.s->fields
      { // Add the fields.
        Field lCurrField(type_->u.sym->u.s.flist);
        for (/* LIE */; lCurrField != 0; lCurrField = lCurrField->link) {
          IROField* lCurrIRField(0);
          IRType* lFieldType(convertType(lCurrField->type));
          std::string lFieldName(lCurrField->name);
          if (!lCurrField->lsb) { // bit field.
            //! \todo M Design: Checkout if the lsb is used correctly.
            IRSignedness lSign(lCurrField->type == inttype ? IRSSigned : IRSUnsigned);
            lCurrIRField = mIRBuilder->irboBitField(lFieldName, lFieldType, lCurrField->offset, 
              lCurrField->lsb, lCurrField->bitsize, lSign);
          } else {
            lCurrIRField = mIRBuilder->irboField(lFieldName, lFieldType, lCurrField->offset);
          } // if
          ASSERTMSG(lCurrIRField != 0, ieStrInconsistentInternalStructure);
          lFields.push_back(lCurrIRField);
          mLCC2irFields[lCurrField] = lCurrIRField;
        } // for
      } // block
      lStrType->SetFields(lFields);
      retVal = lStrType;
    } else if (type_->op == eFLOAT) {
      if (type_->size == 4) {
        retVal = mIRBuilder->irbtGetReal(*ptReal32);
      } else {
        retVal = mIRBuilder->irbtGetReal(*ptReal64);
      } // if
    } else if (type_->op == eDOUBLE) {
      retVal = mIRBuilder->irbtGetReal(*ptReal64);
    } else if (type_->op == eVOID) {
      retVal = mIRBuilder->irbtGetVoid();
    } else if (type_->op == eCONST) {
      retVal = convertType(type_->type);
    } else if (type_->op == eVOLATILE) {
      retVal = convertType(type_->type);
    } else if (type_->op == eCONSTVOL) {
      retVal = convertType(type_->type);
    } else {
      ASSERTMSG(0, ieStrPCShouldNotReachHere << " type: " << toStr(type_));
    } // if
    mLCC2irTypes[type_] = retVal;
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // LCC2HIR::convertType

void LCC2HIR::
insertPendingJumps() {
  BMDEBUG1("LCC2HIR::insertPendingJumps");
  for (hUInt32 c(0); c < mJumps.size(); ++ c) {
    mIRHBuilder->irbsJump(mLCC2irLabels[mJumps[c].second], mJumps[c].first, SrcLoc());
  } // for
  EMDEBUG0();
  return;
} // LCC2HIR::insertPendingJumps

IRType* LCC2HIR::
getType(Node node_) {
  IRType* retVal(0);
  switch (optype(node_->op)) {
    case F:
      if (opsize(node_->op) == 4) {
        retVal = mIRBuilder->irbtGetReal(*ptReal32);
      } else if (opsize(node_->op) == 8) {
        retVal = mIRBuilder->irbtGetReal(*ptReal64);
      } else {
        ASSERTMSG(0, ieStrNotImplemented);
      } // if
      break;
    case P:
      retVal = mIRBuilder->irbtGetPtr(mIRBuilder->irbtGetVoid());
      break;
    case V:
      retVal = mIRBuilder->irbtGetVoid();
      break;
    case B:
      ASSERTMSG(0,ieStrNotImplemented);
      break;
    case U:
    case I: {
      IRSignedness lSign(optype(node_->op) == U ? IRSUnsigned : IRSSigned);
      hFUInt32 lByteSize(mTarget->GetByteSize());
      tInt* lIntType(mTarget->GetIntType(opsize(node_->op)*lByteSize, opsize(node_->op)*lByteSize, lSign));
      retVal = mIRBuilder->irbtGetInt(*lIntType);
      break;
      } // case I,U
    default:
      ASSERTMSG(0, ieStrPCShouldNotReachHere);
  } // switch
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  return retVal;
} // LCC2HIR::getType

LCC2HIR::
LCC2HIR(CGContext* cgContext_) : 
  IAST2HIR(cgContext_),
  mIRBuilder(new IRBuilder(irProgram, consts.cIRBDoOptims)),
  mCurrICStmt(ICInvalid),
  mCurrGlobal(&Invalid<IROGlobal>::Obj()),
  mFileName("")
{
  BMDEBUG2("LCC2HIR::LCC2HIR", cgContext_);
  mTarget = cgContext_->GetHWDesc()->GetTarget();
  mSize_t = new tInt(mTarget->Get_size_t());
  ptfInt8 = mTarget->GetFastIntType(INBITS(8), IRSDoNotCare);
  ptReal32 = mTarget->GetRealSingle();
  ptReal64 = mTarget->GetRealDouble();
  mIRHBuilder = mIRBuilder->GetHelper();
  mIRModule = mIRBuilder->irbModule();
  EMDEBUG0();
} // LCC2HIR::LCC2HIR

std::string LCC2HIR::
getName(Symbol symbol_) {
  std::string retVal(symbol_->name);
  for (hFUInt32 c(0); c < retVal.size(); ++ c) {
    if (retVal[c] == '.') {
      retVal[c] = '_';
    } // if
  } // for
  if (symbol_->generated == 1) {
    retVal = "_" + retVal;
  } // if
  return retVal;
} // LCC2HIR::getName

IRExpr* LCC2HIR::
getConst(Symbol symbol_) {
  BMDEBUG2("LCC2HIR::getConst", toStr(symbol_));
  REQUIREDMSG(symbol_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(mLCC2irConst.find(symbol_) != mLCC2irConst.end(), ieStrInconsistentInternalStructure);
  IRExpr* retVal(mLCC2irConst[symbol_]->GetRecCopyExpr());
  EMDEBUG1(retVal);
  return retVal;
} // LCC2HIR::getConst

IRObject* LCC2HIR::
getObject(Symbol symbol_) {
  BMDEBUG2("LCC2HIR::getObject", toStr(symbol_));
  REQUIREDMSG(symbol_ != 0, ieStrParamValuesDBCViolation);
  if (mLCC2irObj.find(symbol_) == mLCC2irObj.end()) {
    OnDefSymbol(symbol_, scNotLCCCall);
  } // if
  ASSERTMSG(mLCC2irObj.find(symbol_) != mLCC2irObj.end(), ieStrInconsistentInternalStructure);
  IRObject* retVal(mLCC2irObj[symbol_]);
  EMDEBUG1(retVal);
  return retVal;
} // LCC2HIR::getObject

//! \todo L Design: Create a toStr class and add options in config file
//!       for the formatting of the objects.
static std::string 
toStr(Node node_, hFSInt32 indent_) {
  if (!node_) {
    return "<null>";
  } // if
  REQUIREDMSG(node_ != 0, ieStrParamValuesDBCViolation);
  ostrstream retVal;
  ostrstream child;
  ostrstream extrainfo;
  child << "";
  extrainfo << "";
  bool lNodeGened(LCC2HIR::smDTNodeGened[node_]);
  retVal << indent(indent_) << "Node<0x" << hex << hFUInt32(node_) << ">" << lNodeGened <<"(";
  if (0) {
  } else if (generic(node_->op) == eADDRF) {retVal << "ADDRF";
    child << endl << toStr(node_->syms[0], indent_+1);
  } else if (generic(node_->op) == eADDRG) {retVal << "ADDRG";
    child << endl << toStr(node_->syms[0], indent_+1);
  } else if (generic(node_->op) == eADDRL) {retVal << "ADDRL";
    child << endl << toStr(node_->syms[0], indent_+1);
  } else if (generic(node_->op) == eCNST) {retVal << "CNST";
    extrainfo << " " << toStr(node_->syms[0]);
  } else if (generic(node_->op) == eBCOM) {retVal << "BCOM";
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eCVF) {retVal << "CVF";
    child << endl << toStr(node_->syms[0], indent_+1);
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eCVI) {retVal << "CVI";
    child << endl << toStr(node_->syms[0], indent_+1);
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eCVP) {retVal << "CVP";
    child << endl << toStr(node_->syms[0], indent_+1);
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eCVU) {retVal << "CVU";
    child << endl << toStr(node_->syms[0], indent_+1);
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eINDIR) {retVal << "INDIR";
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eNEG) {retVal << "NEG";
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eADD) {retVal << "ADD";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eBAND) {retVal << "BAND";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eBOR) {retVal << "BOR";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eBXOR) {retVal << "BXOR";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eDIV) {retVal << "DIV";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eLSH) {retVal << "LSH";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eMOD) {retVal << "MOD";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eMUL) {retVal << "MUL";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eRSH) {retVal << "RSH";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eSUB) {retVal << "SUB";
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eASGN) {retVal << "ASGN";
    child << endl << toStr(node_->syms[0], indent_+1);
    child << endl << toStr(node_->syms[1], indent_+1);
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eEQ) {retVal << "EQ";
    extrainfo << " " << toStr(node_->syms[0]);
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eGE) {retVal << "GE";
    extrainfo << " " << toStr(node_->syms[0]);
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eGT) {retVal << "GT";
    extrainfo << " " << toStr(node_->syms[0]);
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eLE) {retVal << "LE";
    extrainfo << " " << toStr(node_->syms[0]);
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eLT) {retVal << "LT";
    extrainfo << " " << toStr(node_->syms[0]);
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eNE) {retVal << "NE";
    extrainfo << " " << toStr(node_->syms[0]);
    child << endl << toStr(node_->kids[0], indent_+1);
    child << endl << toStr(node_->kids[1], indent_+1);
  } else if (generic(node_->op) == eARG) {retVal << "ARG";
    child << endl << toStr(node_->syms[0], indent_+1);
    child << endl << toStr(node_->syms[1], indent_+1);
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eCALL) {retVal << "CALL";
    child << endl << toStr(node_->kids[0], indent_+1);
    if (optype(node_->op) != eSTRUCT) {
      child << endl << toStr(node_->kids[1], indent_+1);
    } // if
  } else if (generic(node_->op) == eRET) {retVal << "RET";
    if (optype(node_->op) != eVOID) {
      child << endl << toStr(node_->kids[0], indent_+1);
    } // if
  } else if (generic(node_->op) == eJUMP) {retVal << "JUMP";
    child << endl << toStr(node_->kids[0], indent_+1);
  } else if (generic(node_->op) == eLABEL) {retVal << "LABEL";
    extrainfo << " " << toStr(node_->syms[0]);
  } else if (generic(node_->op) == eLOAD) {retVal << "LOAD";
  } else if (generic(node_->op) == eVREG) {retVal << "VREG";
  } else {
    retVal << "!Op!";
    ASSERTMSG(1, ieStrPCShouldNotReachHere << " op:" << node_->op << " gen:" << generic(node_->op) << 
      " spc: " << specific(node_->op));
  } // if
  child << ends;
  if (optype(node_->op) == eFLOAT) { 
    retVal << "F" << opsize(node_->op);
  } else if (optype(node_->op) == ePOINTER) { 
    retVal << "P" << opsize(node_->op);
  } else if (optype(node_->op) == eUNSIGNED) { 
    retVal << "U" << opsize(node_->op);
  } else if (optype(node_->op) == eSTRUCT) { retVal << "B";
  } else if (optype(node_->op) == eVOID) { retVal << "V";
  } else if (optype(node_->op) == eINT) { 
    retVal << "I" << opsize(node_->op);
  } else {
    retVal << "!Type!";
    ASSERTMSG(1, ieStrPCShouldNotReachHere);
  } // if
  extrainfo << ends;
  retVal << extrainfo.str();
  if (strlen(child.str()) != 0) {
    retVal << child.str() << endl << indent(indent_);
  } // if
  if (!node_->link) {
    // LIE
  } else {
//    retVal << indent(indent_) << "LINK-> " << toStr(node_->link, indent_) << endl << indent(indent_);
  } // if
  retVal << ")" << ends;
  return retVal.str();
} // toStr(Node, hFSInt32)





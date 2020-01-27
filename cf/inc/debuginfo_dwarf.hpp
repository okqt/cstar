// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __DEBUGINFO_DWARF_HPP__
#define __DEBUGINFO_DWARF_HPP__

#ifndef __DEBUGINFO_HPP__
#include "debuginfo.hpp"
#endif
#ifndef __BCF_TARGET_HPP__
#include "bcf_target.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif

//! \file debuginfo_dwarf.hpp
//! \brief This file contains the DWARF version 2 implementation of generic debug
//!        information generation class (DebugInfo) definitions.

class DwarfDIEBase : public DIEBase {
public:
  DwarfDIEBase() :
    mTag(0)
  {
  } // DwarfDIEBase::DwarfDIEBase
  DwarfDIEBase(const DwarfDIEBase& die_) :
    DIEBase(die_),
    mTag(0)
  {
  } // DwarfDIEBase::DwarfDIEBase
  virtual ~DwarfDIEBase() {}
  void SetTag(DW_TAG* die_) {
    REQUIREDMSG(die_ != 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(mTag == 0, ieStrParamValuesDBCViolation);
    mTag = die_;
  } // DwarfDIEBase::SetTag
  DW_TAG* GetTag() const {
    REQUIREDMSG(mTag != 0, ieStrParamValuesDBCViolation);
    return mTag;
  } // DwarfDIEBase::GetTag
private:
  DW_TAG* mTag;
private:
  friend class Predicate;
  friend class Extract;
}; // class DwarfDIEBase

class DwarfProgDIE : public DwarfDIEBase {
public:
  DwarfProgDIE(const DwarfProgDIE& progDIE_) :
    DwarfDIEBase(progDIE_)
  {
  } // DwarfProgDIE::DwarfProgDIE
  DwarfProgDIE(IRProgram* prog_) :
    mProg(prog_)
  {
    REQUIREDMSG(prog_ != 0, ieStrParamValuesDBCViolation);
  } // DwarfProgDIE::DwarfProgDIE
  virtual ~DwarfProgDIE() {}
  virtual DIEBase* Clone() const {return new DwarfProgDIE(*this);}
  IRProgram* GetProg() const {return mProg;}
private:
  IRProgram* mProg;
private:
  friend class Predicate;
  friend class Extract;
}; // class DwarfProgDIE

class DwarfModuleDIE : public DwarfDIEBase {
public:
  DwarfModuleDIE(const DwarfModuleDIE& moduleDIE_) :
    DwarfDIEBase(moduleDIE_)
  {
  } // DwarfModuleDIE::DwarfModuleDIE
  DwarfModuleDIE(IRModule* module_) :
    mModule(module_)
  {
    REQUIREDMSG(module_ != 0, ieStrParamValuesDBCViolation);
  } // DwarfModuleDIE::DwarfModuleDIE
  virtual ~DwarfModuleDIE() {}
  virtual DIEBase* Clone() const {return new DwarfModuleDIE(*this);}
  IRModule* GetModule() const {return mModule;}
private:
  IRModule* mModule;
private:
  friend class Predicate;
  friend class Extract;
}; // class DwarfModuleDIE

class DwarfFuncDIE : public DwarfDIEBase {
public:
  DwarfFuncDIE(const DwarfFuncDIE& funcDIE_) :
    DwarfDIEBase(funcDIE_),
    mFDE(0)
  {
  } // DwarfFuncDIE::DwarfFuncDIE
  DwarfFuncDIE(IRFunction* func_) :
    mFunc(func_),
    mFDE(0)
  {
    REQUIREDMSG(func_ != 0, ieStrParamValuesDBCViolation);
  } // DwarfFuncDIE::DwarfFuncDIE
  virtual ~DwarfFuncDIE() {}
  virtual DIEBase* Clone() const {return new DwarfFuncDIE(*this);}
  IRFunction* GetFunc() const {return mFunc;}
  void SetFDE(DF_FDE* fde_) {
    REQUIREDMSG(fde_ != 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(mFDE == 0, ieStrParamValuesDBCViolation);
    mFDE = fde_;
  } // DwarfFuncDIE::SetFDE
  DF_FDE* GetFDE() const {
    REQUIREDMSG(mFDE != 0, ieStrParamValuesDBCViolation);
    return mFDE;
  } // DwarfFuncDIE::GetFDE
  //! \pre expr_ may not be empty.
  void SetFrameBase(AddrDiff* range_, const vector<DW_LOC_OP*>& expr_) {
    REQUIREDMSG(!expr_.empty(), ieStrParamValuesDBCViolation);
    mFrameBase.first = range_;
    mFrameBase.second = expr_;
  } // DwarfFuncDIE::SetFrameBase
  //! \note if framebase is not set then location expression vector is empty.
  const pair<AddrDiff*, vector<DW_LOC_OP*> >& GetFrameBase() const {return mFrameBase;}
private:
  pair<AddrDiff*, vector<DW_LOC_OP*> > mFrameBase;
  DF_FDE* mFDE;
  IRFunction* mFunc;
private:
  friend class Predicate;
  friend class Extract;
}; // class DwarfFuncDIE

class DwarfBBDIE : public DwarfDIEBase {
public:
  DwarfBBDIE(const DwarfBBDIE& funcDIE_) :
    DwarfDIEBase(funcDIE_)
  {
  } // DwarfBBDIE::DwarfBBDIE
  DwarfBBDIE(IRBB* bb_) :
    mBB(bb_)
  {
    REQUIREDMSG(bb_ != 0, ieStrParamValuesDBCViolation);
  } // DwarfBBDIE::DwarfBBDIE
  virtual ~DwarfBBDIE() {}
  virtual DIEBase* Clone() const {return new DwarfBBDIE(*this);}
  IRBB* GetBB() const {return mBB;}
private:
  IRBB* mBB;
private:
  friend class Predicate;
  friend class Extract;
}; // class DwarfBBDIE

class DwarfStmtDIE : public DwarfDIEBase {
public:
  DwarfStmtDIE(const DwarfStmtDIE& funcDIE_) :
    DwarfDIEBase(funcDIE_)
  {
  } // DwarfStmtDIE::DwarfStmtDIE
  DwarfStmtDIE(IRStmt* stmt_) :
    mStmt(stmt_)
  {
    REQUIREDMSG(stmt_ != 0, ieStrParamValuesDBCViolation);
  } // DwarfStmtDIE::DwarfStmtDIE
  virtual ~DwarfStmtDIE() {}
  virtual DIEBase* Clone() const {return new DwarfStmtDIE(*this);}
  IRStmt* GetStmt() const {return mStmt;}
private:
  IRStmt* mStmt;
private:
  friend class Predicate;
  friend class Extract;
}; // class DwarfStmtDIE

class DwarfObjDIE : public DwarfDIEBase {
public:
  DwarfObjDIE(const DwarfObjDIE& objectDIE_) :
    DwarfDIEBase(objectDIE_)
  {
  } // DwarfObjDIE::DwarfObjDIE
  DwarfObjDIE(IRObject* obj_) :
    mObj(obj_)
  {
    REQUIREDMSG(obj_ != 0, ieStrParamValuesDBCViolation);
  } // DwarfObjDIE::DwarfObjDIE
  virtual ~DwarfObjDIE() {}
  virtual DIEBase* Clone() const {return new DwarfObjDIE(*this);}
  IRObject* GetObject() const {return mObj;}
  //! \brief The address of object that is confined to the lexical scope of objects. 
  //! \note You may still add with AddLifeRange, but this only makes
  //!       sense for global objects.
  //! \pre You may not call SetAddress twice.
  void SetAddress(const vector<DW_LOC_OP*>& locExpr_) {
    REQUIREDMSG(!locExpr_.empty(), ieStrParamValuesDBCViolation);
    REQUIREDMSG(mLifeRanges.empty() == true || mLifeRanges[0].first != 0, ieStrParamValuesDBCViolation);
    mLifeRanges.insert(mLifeRanges.begin(), make_pair((AddrDiff*)0, locExpr_));
  } // DwarfObjDIE::SetAddress
  //! \brief Adds an address range and an address.
  void AddLifeRange(AddrDiff* range_, const vector<DW_LOC_OP*>& addrExpr_) {
    REQUIREDMSG(range_ != 0 && !addrExpr_.empty(), ieStrParamValuesDBCViolation);
    mLifeRanges.push_back(make_pair(range_, addrExpr_));
  } // DwarfObjDIE::AddLifeRange 
  const vector<pair<AddrDiff*, vector<DW_LOC_OP*> > >& GetLifeRanges() const {return mLifeRanges;}
private:
  //! \brief Address ranges that this object is alive.
  vector<pair<AddrDiff*, vector<DW_LOC_OP*> > > mLifeRanges;
  IRObject* mObj;
private:
  friend class Predicate;
  friend class Extract;
}; // class DwarfObjDIE

class DwarfDIEBuilder : public DIEBuilder {
public:
  DwarfDIEBuilder() {}
  virtual ~DwarfDIEBuilder() {}
  virtual DIEBase* CreateProgDIE(IRProgram* prog_) {return new DwarfProgDIE(prog_);}
  virtual DIEBase* CreateModuleDIE(IRModule* module_) {return new DwarfModuleDIE(module_);}
  virtual DIEBase* CreateFuncDIE(IRFunction* func_) {return new DwarfFuncDIE(func_);}
  virtual DIEBase* CreateStmtDIE(IRStmt* stmt_) {return new DwarfStmtDIE(stmt_);}
  virtual DIEBase* CreateBBDIE(IRBB* bb_) {return new DwarfBBDIE(bb_);}
  virtual DIEBase* CreateObjDIE(IRObject* obj_) {return new DwarfObjDIE(obj_);}
}; // class DwarfDIEBuilder

string GetAbbrevComment(const ASMSection& asmSection_);

//! \brief Signed LEB128 representation.
//! \note LEB128 stands for "Little Endian Base 128".
//! \note LEB128 numbers are space saving when the numbers are small in
//!       magnitude. Which seems to be the case for dwarf numbers.
//! \todo M Design: With a common base we can reduce the class sizes of S/ULEB128.
class SLEB128 {
public:
  SLEB128(const hBigInt& value_);
  SLEB128(const hFSInt32& value_);
  SLEB128(const vector<hFUInt8>& value_);
  virtual ~SLEB128() {}
  const vector<hFUInt8>& GetBytes() const {return mBytes;}
  virtual void GetAsValues(vector<DW_Value*>& values_);
  hBigInt GetValue() const {return mValue;}
  //! \brief Returns the number of bytes the value occupies.
  hFUInt8 GetSize() const {return mBytes.size();}
  ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) const;
private:
  void decode();
  void encode();
  hBigInt mValue;
  vector<hFUInt8> mBytes; // Byte representation.
}; // class SLEB128

//! \brief Unsigned LEB128 representation.
class ULEB128 {
public:
  ULEB128(const hBigInt& value_);
  ULEB128(const hFUInt32& value_);
  ULEB128(const vector<hFUInt8>& value_);
  virtual ~ULEB128() {}
  hBigInt GetValue() const {return mValue;}
  const vector<hFUInt8>& GetBytes() const {return mBytes;}
  virtual void GetAsValues(vector<DW_Value*>& values_);
  //! \brief Returns the number of bytes the value occupies.
  hFUInt8 GetSize() const {return mBytes.size();}
  ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) const;
private:
  void decode();
  void encode();
  hBigInt mValue;
  vector<hFUInt8> mBytes; // Byte representation.
}; // class ULEB128

//! \brief Debug Information Entry for DWARF.
//!
//! Dwarf representation is a tree of DW_DIEs. DW_DIE is the base class of DW_TAG.
//! \sa DW_TAG DW_AT
class DW_DIE : public DIEBase {
public:
  DW_DIE();
  DW_DIE(const DW_DIE& die_);
  virtual ~DW_DIE() {}
  void AddChild(DW_DIE* die_) {mChildren.push_back(die_);}
  //! \param attributeCode_ Must be a Singleton of an attribute.
  void AddAttribute(DW_AT* attributeCode_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  void SetAddrTaken() {mAddrTaken = true;}
  bool IsAddrTaken() {return mAddrTaken == true;}
  const string& GetAddrLabel(const ASMDesc& asmDesc_);
  virtual const string& GetName() const;
  void SetOffset(hFUInt32 offset_) {mOffsetInCompUnit = offset_;}
  hFUInt32 GetOffset() const {return mOffsetInCompUnit;}
  virtual DIEBase* Clone() const = 0;
private:
  void emitDIE(ASMSection& debugInfo_, ASMSection& debugAbbrev_, const ASMDesc& asmDesc_);
  vector<DW_DIE*> mChildren;
  vector<DW_AT*> mAttributes;
  hFUInt32 mOffsetInCompUnit; //!< Offset of this DIE relative to its compilation unit.
  string mAddrLabel; //!< Must be empty if address taken is not set.
  bool mAddrTaken; //!< Set if there is a reference to this DIE, need to emit a label for this DIE.
  friend class DebugInfoDwarf2;
}; // class DW_DIE

class DW_DIE_null : public DW_DIE {
public:
  DW_DIE_null() {}
  DW_DIE_null(const DW_DIE_null& dieNull_) :
    DW_DIE(dieNull_)
  {
  } // DW_DIE_null::DW_DIE_null
  virtual ~DW_DIE_null() {}
  virtual DIEBase* Clone() const {return new DW_DIE_null(*this);}
}; // class DW_DIE_null

//! \brief Has the functions to construct C language specific DW_DIEs.
class Dwarf2CLangHelper : public IDebuggable {
public:
  Dwarf2CLangHelper() {}
  virtual ~Dwarf2CLangHelper() {}
  //! \brief Adds declaration location relatetd attributes to the specified tag.
  DW_DIE* AddBaseType(DW_DIE* compUnit_, const string& name_, eDW_ATE encoding_, hFUInt16 byteSize_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class Dwarf2CLangHelper

//! \brief Base of Call Frame Instructions.
class CFInst {
public:
  CFInst() {}
  virtual ~CFInst() {}
  virtual hFUInt32 GetCode() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // CFInst::GetCode
  virtual hFUInt32 GetByteSize() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // CFInst::GetByteSize
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return o;
  } // CFInst::GetDisassembly
}; // class CFInst

class DebugFrameEntry {
public:
  DebugFrameEntry() :
    mSizeOfInsts(0)
  {
  } // DebugFrameEntry::DebugFrameEntry
  virtual ~DebugFrameEntry() {}
  void AddInst(CFInst* inst_);
protected:
  vector<CFInst*> mInsts;
  hFUInt32 mSizeOfInsts; //!< Size of instruction lengths in bytes.
}; // class DebugFrameEntry

typedef DebugFrameEntry DFE;

//! \brief Common Information Entry.
//! \note CIE is target dependent. You should set this in your HWDescription derived
//!       OnProgStart or OnModuleStart, e.g. ARMHW::OnProgStart.
class DF_CIE : public DebugFrameEntry {
public:
  DF_CIE(hFUInt32 id_, hFUInt8 version_, const string& augmentation_, ULEB128 codeAlignFactor_,
    SLEB128 dataAlignFactor_, hFUInt8 retAddrRegNum_) :
    mID(id_),
    mVersion(version_),
    mAugmentation(augmentation_),
    mCodeAlignFactor(codeAlignFactor_),
    mDataAlignFactor(dataAlignFactor_),
    mRetAddrRegNum(retAddrRegNum_)
  {
  } // DF_CIE::DF_CIE
  virtual ~DF_CIE() {}
  void Emit(ASMSection& debugFrame_, const ASMDesc& asmDesc_);
  string GetAddrLabel() const {return mAddrLabel;}
private:
  hFUInt32 mID;
  hFUInt8 mVersion;
  string mAugmentation;
  ULEB128 mCodeAlignFactor;
  SLEB128 mDataAlignFactor;
  hFUInt8 mRetAddrRegNum;
  string mAddrLabel;
}; // class DF_CIE

//! \brief Frame Description Entry.
//! \note Usually you need one for each function.
//! In optimized code you may have functions split in hot/cold
//! regions in that case you would need for each split part.
class DF_FDE : public DebugFrameEntry {
public:
  DF_FDE(IRFunction* func_, DF_CIE* cie_) :
    mFunc(func_),
    mCIE(cie_)
  {
  } // DF_FDE::DF_FDE
  virtual ~DF_FDE() {}
  void Emit(ASMSection& debugFrame_, const ASMDesc& asmDesc_);
private:
  IRFunction* mFunc;
  DF_CIE* mCIE;
}; // class DF_FDE

//! \brief Interface of DWARF2 debug information generation.
//! \note We first create a tree of TAGs in memory. We annotate
//!       the IR that helps the debug information, for example,
//!       emitting a label for an address or start/end of functions, modules.
//!       At the end of assembly emission we emit the debug information
//!       sections by traversing the TAG tree.
//!       We try to reduce the calls made to the debug interface to keep the code
//!       quality high.
class DebugInfoDwarf2 : public IDebuggable {
public:
  DebugInfoDwarf2()
  {
  } // DebugInfoDwarf2::DebugInfoDwarf2
  virtual ~DebugInfoDwarf2() {}
  Dwarf2CLangHelper* GetCLangHelper() {return &Singleton<Dwarf2CLangHelper>::Obj();}
  virtual void OnInit();
  //! \brief We will emit the debug information in OnFinish.
  virtual void OnFinish(ostream& asmFile_, CGContext& cgContext_);
  virtual void OnFuncStart(IRFunction* func_, CGFuncContext& context_);
  virtual void OnFuncEnd(IRFunction* func_, CGFuncContext& context_);
  virtual void OnModuleStart(IRModule* module_);
  virtual void OnModuleEnd(IRModule* module_);
  virtual void OnProgStart(IRProgram* prog_, CGContext& context_);
  virtual void OnProgEnd(IRProgram* prog_, CGContext& context_);
  virtual void OnBBStart(IRBB* bb_, CGFuncContext& context_);
  virtual void OnStmtStart(IRStmt* stmt_, CGFuncContext& context_);
  virtual void OnStmtEnd(IRStmt* stmt_, CGFuncContext& context_);
  virtual void OnBlockScopeStart(DIEBase* block_);
  virtual void OnBlockScopeEnd(DIEBase* block_);
  virtual DW_TAG* AddModule(const string& name_, const string& producer_, eDW_LANG lang_, const string& compDir_);
  virtual DW_TAG* AddFunction(DIEBase* module_, bool external_, const string& name_, DIEBase* returnType_, const SrcLoc& srcLoc_);
  virtual DW_TAG* AddVariable(DIEBase* parent_, const string& name_, DIEBase* type_, bool external_,
    bool parameter_, const SrcLoc& srcLoc_);
  virtual DW_DIE* AddPointerType(DIEBase* parent_, hFUInt32 byteSize_, DIEBase* refType_);
  virtual DW_DIE* GetNull() {return &Singleton<DW_DIE_null>::Obj();}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \note It also stores the CIE for emission in .debug_frame section.
  void SetCurrentCIE(DF_CIE* cie_) {
    REQUIREDMSG(cie_ != 0, ieStrParamValuesDBCViolation);
    mCurrCIE = cie_;
    mCIEs.push_back(cie_);
  } // DebugInfoDwarf2::SetCurrentCIE
private:
  //! \brief Sets the DW_AT_location attribute of given tag and emits the
  //!        .debug_loc section contents.
  //! \attention .debug_info section must be emitted after .debug_loc because
  //!            we calculate and set the offsets of location lists in this
  //!            function.
  void emitLocationAttributes(ASMSection& lDebugLoc, ASMDesc& asmDesc_, DW_TAG* tag_, 
    const vector<pair<AddrDiff*, vector<DW_LOC_OP*> > >& location_, eDW_AT attribute_);
  void addHighLowPC(DW_TAG* tag_, const AddrLTConst& lowPC_, const AddrLTConst& highPC_);
  void addDeclAttributes(DW_DIE* tag_, hFUInt16 column_, const string& fileName_, hFUInt32 line_);
  void addDeclAttributes(DW_DIE* tag_, const SrcLoc& srcLoc_) {
    addDeclAttributes(tag_, srcLoc_.GetColNum(), srcLoc_.GetFileName(), srcLoc_.GetLineNum());
  } // DebugInfoDwarf2::addDeclAttributes
  DW_DIE* mRoot;
  vector<DW_DIE*> mCompileUnits;
  vector<DF_CIE*> mCIEs;
  vector<DF_FDE*> mFDEs;
  //! \brief Objects are collected during OnFuncEnd.
  vector<IRObject*> mObjects;
  vector<IRFunction*> mFuncs;
  DF_CIE* mCurrCIE;
private:
  // Below streams contains the assembly output of the sections created for debug information.
  ASMSection mDebugAbbrev; //!< ".debug_abbrev"
  ASMSection mDebugInfo; //!< ".debug_info"
  ASMSection mDebugARanges; //!< ".debug_aranges"
  ASMSection mDebugPubNames; //!< ".debug_pubnames"
  ASMSection mDebugLine; //!< ".debug_line"
  ASMSection mDebugFrame; //!< ".debug_frame"
  ASMSection mDebugLoc; //!< ".debug_loc"
}; // class DebugInfoDwarf2

class DW_Value {
public:
  DW_Value() {}
  virtual ~DW_Value() {}
  //! \brief Must return the numeric value of the form code of this value.
  //! For example, eDW_FORM_data1
  //! \sa DW_FORM
  virtual hFSInt32 GetFormCode() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return -1;
  } // DW_Value::GetFormCode
  virtual hFUInt32 GetByteSize() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DW_Value::GetByteSize
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return o;
  } // DW_Value::GetDisassembly
}; // class DW_Value

class DWC_Constant : public DW_Value {
public:
  DWC_Constant() {}
  virtual ~DWC_Constant() {}
}; // class DWC_Constant

//! \brief Variable length unsigned data, represented by ULEB128.
class DWC_UData : public DWC_Constant {
public:
  DWC_UData(const ULEB128& value_) :
    mValue(value_)
  {
  } // DWC_UData::DWC_UData
  virtual ~DWC_UData() {}
  const ULEB128& GetValue() const {return mValue;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_udata;}
  virtual hFUInt32 GetByteSize() const {return mValue.GetSize();}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    return mValue.GetDisassembly(o, asmDesc_, comment_);
  } // DWC_SData::GetDisassembly
private:
  ULEB128 mValue;
}; // class DWC_UData

//! \brief Variable length signed data, represented by SLEB128.
class DWC_SData : public DWC_Constant {
public:
  DWC_SData(const SLEB128& value_) :
    mValue(value_)
  {
  } // DWC_SData::DWC_SData
  virtual ~DWC_SData() {}
  const SLEB128& GetValue() const {return mValue;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_sdata;}
  virtual hFUInt32 GetByteSize() const {return mValue.GetSize();}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    return mValue.GetDisassembly(o, asmDesc_, comment_);
  } // DWC_SData::GetDisassembly
private:
  SLEB128 mValue;
}; // class DWC_SData

class DWC_Data1 : public DWC_Constant {
public:
  DWC_Data1(hFUInt8 value_) :
    mValue(value_)
  {
  } // DWC_Data1::DWC_Data1
  virtual ~DWC_Data1() {}
  hFUInt8 GetByte() const {return mValue;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_data1;}
  virtual hFUInt32 GetByteSize() const {return 1;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    string lComment;
    if (!comment_.empty()) {
      lComment = ", " + comment_;
    } // if
    asmDesc_.PutData1(o, mValue, GetAbbrevComment(o) + lComment).Endl();
    return o;
  } // DWC_Data1::GetDisassembly
private:
  hFUInt8 mValue;
}; // class DWC_Data1

class DWC_Data2 : public DWC_Constant {
public:
  DWC_Data2(hFUInt16 value_) :
    mValue(value_)
  {
  } // DWC_Data2::DWC_Data2
  virtual ~DWC_Data2() {}
  hFUInt16 GetByte() const {return mValue;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_data2;}
  virtual hFUInt32 GetByteSize() const {return 2;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    string lComment;
    if (!comment_.empty()) {
      lComment = ", " + comment_;
    } // if
    asmDesc_.PutData2(o, mValue, GetAbbrevComment(o) + lComment).Endl();
    return o;
  } // DWC_Data2::GetDisassembly
private:
  hFUInt16 mValue;
}; // class DWC_Data2

class DWC_Data4 : public DWC_Constant {
public:
  DWC_Data4(hFUInt32 value_) :
    mValue(value_)
  {
  } // DWC_Data4::DWC_Data4
  virtual ~DWC_Data4() {}
  hFUInt32 GetByte() const {return mValue;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_data4;}
  virtual hFUInt32 GetByteSize() const {return 4;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    string lComment;
    if (!comment_.empty()) {
      lComment = ", " + comment_;
    } // if
    asmDesc_.PutData4(o, mValue, GetAbbrevComment(o) + lComment).Endl();
    return o;
  } // DWC_Data4::GetDisassembly
private:
  hFUInt32 mValue;
}; // class DWC_Data4

class DWC_Data8 : public DWC_Constant {
public:
  DWC_Data8(hFUInt64 value_) :
    mValue(value_)
  {
  } // DWC_Data8::DWC_Data8
  virtual ~DWC_Data8() {}
  hFUInt64 GetByte() const {return mValue;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_data8;}
  virtual hFUInt32 GetByteSize() const {return 8;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string());
private:
  hFUInt64 mValue;
}; // class DWC_Data8

class DWC_Reference : public DW_Value {
public:
  DWC_Reference() {}
  virtual ~DWC_Reference() {}
private:
}; // class DWC_Reference

class DWC_RefAddr : public DWC_Reference {
public:
  DWC_RefAddr(DIEBase* die_);
  DWC_RefAddr(const string& addrLabel_) :
    mAddrLabel(addrLabel_),
    mDIE(0)
  {
  } // DWC_RefAddr::DWC_RefAddr
  const string& GetAddr() const {return mAddrLabel;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_ref_addr;}
  //! \todo M Design: use the size of addresses in the machine.
  virtual hFUInt32 GetByteSize() const {return 4;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string());
protected:
  string mAddrLabel;
  DW_TAG* mDIE;
}; // class DWC_RefAddr

class DWC_RefOffset : public DWC_RefAddr {
public:
  DWC_RefOffset(DIEBase* die_);
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_ref4;}
  virtual hFUInt32 GetByteSize() const {return 4;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string());
}; // class DWC_RefOffset

class DWC_RefN : public DWC_Reference {
public:
  //! \param byteCountSize_ may be 1,2,4,8.
  DWC_RefN(hFUInt8 byteCountSize_, hFUInt64 offset_) :
    mByteCountSize(byteCountSize_),
    mOffset(offset_)
  {
  } // DWC_RefN::DWC_RefN
  hFUInt8 GetByteCount() const {return mByteCountSize;}
  hFUInt64 GetOffset() const {return mOffset;}
  virtual hFUInt32 GetByteSize() const {return mByteCountSize;}
  virtual hFSInt32 GetFormCode() const {
    return mByteCountSize == 1 ? eDW_FORM_ref1 :
      (mByteCountSize == 2 ? eDW_FORM_ref2 :
        (mByteCountSize == 4 ? eDW_FORM_ref4 : eDW_FORM_ref8));
  } // DWC_RefN::GetFormCode
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    ASSERTMSG(0, ieStrNotImplemented);
    NOTIMPLEMENTED();
    return o;
  } // DWC_RefN::GetDisassembly
private:
  hFUInt8 mByteCountSize; //!< May be 1,2,4,8.
  hFUInt64 mOffset; //! Offset of DW_DIE relative to compilation unit.
}; // class DWC_RefN

class DWC_RefULEB128 : public DWC_Reference {
public:
  DWC_RefULEB128(const ULEB128& offset_) :
    mOffset(offset_)
  {
  } // DWC_RefULEB128::DWC_RefULEB128
  const ULEB128& GetOffset() const {return mOffset;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_ref_udata;}
  virtual hFUInt32 GetByteSize() const {return mOffset.GetSize();}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    ASSERTMSG(0, ieStrNotImplemented);
    NOTIMPLEMENTED();
    return o;
  } // DWC_RefULEB128::GetDisassembly
private:
  ULEB128 mOffset; //! Offset of DW_DIE relative to compilation unit.
}; // class DWC_RefULEB128

//! \brief Base class for all block values.
//! \note This class does not represent DW_Form_block,
//!       see DWC_BlockULEB128 for DW_Form_block.
class DWC_Block : public DW_Value {
public:
  DWC_Block() :
    mLength(0)
  {
  } // DWC_Block::DWC_Block
  virtual ~DWC_Block() {}
  virtual hFSInt32 GetFormCode() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DWC_Block::GetFormCode
  void AddValues(const vector<DW_Value*>& values_) {
    for (hFUInt32 c(0); c < values_.size(); ++ c) {
      AddValue(values_[c]);
    } // for
  } // DWC_Block::AddValues
  void AddValue(DW_Value* val_) {
    mValues.push_back(val_);
    mLength += val_->GetByteSize();
  } // DWC_Block::AddValue
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    for (hFUInt32 c(0); c < mValues.size(); ++ c) {
      mValues[c]->GetDisassembly(o, asmDesc_);
    } // for
    return o;
  } // DWC_Block::GetDisassembly
protected:
  hFUInt32 mLength; //!< Number of bytes the mValues have.
  vector<DW_Value*> mValues;
  template<class T> friend class Singleton;
}; // class DWC_Block

class DWC_BlockN : public DWC_Block {
private:
  //! \brief Should only be used by Singleton.
  DWC_BlockN() {}
public:
  //! \param byteCountSize_ can be 1,2, or 4, must be big enough to hold length_.
  DWC_BlockN(hFUInt8 byteCountSize_) :
    mByteCountSize(byteCountSize_)
  {
  } // DWC_BlockN::DWC_BlockN
  virtual ~DWC_BlockN() {}
  virtual hFUInt32 GetByteSize() const {return mLength+mByteCountSize;}
  virtual hFSInt32 GetFormCode() const {
    return mByteCountSize == 1 ? eDW_FORM_block1 :
      (mByteCountSize == 2 ? eDW_FORM_block2 : eDW_FORM_block4);
  } // DWC_BlockN::GetFormCode
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    if (mByteCountSize == 1) {
      ASSERTMSG(mLength < 0xff, ieStrInconsistentInternalStructure);
      asmDesc_.PutData1(o, mLength, comment_ + " length").Endl();
    } else if (mByteCountSize == 2) {
      ASSERTMSG(mLength < 0xffff, ieStrInconsistentInternalStructure);
      asmDesc_.PutData2(o, mLength, comment_ + " length").Endl();
    } else if (mByteCountSize == 4) {
      ASSERTMSG(mLength < 0xffffffff, ieStrInconsistentInternalStructure);
      asmDesc_.PutData4(o, mLength, comment_ + " length").Endl();
    } else {
      ASSERTMSG(0, ieStrPCShouldNotReachHere);
    } // if
    DWC_Block::GetDisassembly(o, asmDesc_);
    return o;
  } // DWC_BlockN::GetDisassembly
private:
  hFUInt8 mByteCountSize; //!< 1,2,4 length size in bytes.
  template<class T> friend class Singleton;
}; // class DWC_BlockN

class DWC_BlockULEB128 : public DWC_Block {
public:
  DWC_BlockULEB128()
  {
  } // DWC_BlockULEB128::DWC_BlockULEB128
  virtual ~DWC_BlockULEB128() {}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_block;}
  virtual hFUInt32 GetByteSize() const {
    ULEB128 mULEB128Length(mLength);
    return mLength + mULEB128Length.GetSize();
  } // DWC_BlockULEB128::GetByteSize
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    ULEB128 mULEB128Length(mLength);
    mULEB128Length.GetDisassembly(o, asmDesc_, comment_);
    DWC_Block::GetDisassembly(o, asmDesc_);
    return o;
  } // DWC_BlockULEB128::GetDisassembly
}; // class DWC_BlockULEB128

//! \brief Constructs a value for an LT const address.
class DWC_Address : public DW_Value {
private:
  //! \brief Should only be used by Singleton.
  DWC_Address() {}
public:
  DWC_Address(const string& addrLabel_) :
    mAddrLabel(addrLabel_)
  {
  } // DWC_Address::DWC_Address
  virtual ~DWC_Address() {}
  const string& GetAddress() const {return mAddrLabel;}
  //! \todo M Design: Adapt the size of address.
  virtual hFUInt32 GetByteSize() const {return 4;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_addr;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    asmDesc_.PutRelocAddr(o, mAddrLabel, comment_).Endl();
    return o;
  } // DWC_Address::GetDisassembly
private:
  string mAddrLabel; //!< Address is represented by an assembly label.
  template<class T> friend class Singleton;
}; // class DWC_Address

//! \note When when emitting the value dwarf expects a non-zero value for true
//!       and zero for false case.
class DWC_Flag : public DW_Value {
private:
  //! \brief Should only be used by Singleton.
  DWC_Flag() {}
public:
  DWC_Flag(bool flag_) :
    mFlag(flag_)
  {
  } // DWC_Flag::DWC_Flag
  virtual ~DWC_Flag() {}
  bool GetFlag() const {return mFlag;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_flag;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string());
private:
  bool mFlag;
  template<class T> friend class Singleton;
}; // class DWC_Flag

//! \brief Null terminated string.
class DWC_StringBase : public DW_Value {
public:
  DWC_StringBase() {}
  virtual ~DWC_StringBase() {}
}; // class DWC_StringBase

class DWC_String : public DWC_StringBase {
private:
  //! \brief Should only be used by Singleton.
  DWC_String() {}
public:
  DWC_String(const string& string_) :
    mString(string_)
  {
  } // DWC_String::DWC_String
  const string& GetString() const {return mString;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_string;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    string lComment;
    if (!comment_.empty()) {
      lComment = ", " + comment_;
    } // if
    asmDesc_.PutDataString(o, mString, GetAbbrevComment(o) + lComment).Endl();
    return o;
  } // DWC_String::GetDisassembly
private:
  string mString;
  template<class T> friend class Singleton;
}; // class DWC_String

class DWC_Strp : public DWC_StringBase {
private:
  //! \brief Should only be used by Singleton.
  DWC_Strp() {}
public:
  DWC_Strp(const hFUInt32& offset_) :
    mOffset(offset_)
  {
  } // DWC_Strp::DWC_Strp
  hFUInt32 GetOffset() const {return mOffset;}
  virtual hFSInt32 GetFormCode() const {return eDW_FORM_strp;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    ASSERTMSG(0, ieStrNotImplemented);
    NOTIMPLEMENTED();
    return o;
  } // DWC_Strp::GetDisassembly
private:
  hFUInt32 mOffset; //!< Offset in ".debug_str" section that contains the string.
  template<class T> friend class Singleton;
}; // class DWC_Strp

//! \brief Base class for DWARF attributes.
class DW_AT : public IDebuggable {
protected:
  // present for Singleton use
  DW_AT() {}
public:
  DW_AT(DW_Value* value_) :
    mValue(value_)
  {
  } // DW_AT::DW_AT
  virtual ~DW_AT() {}
  virtual hFUInt32 GetAttCode() const {
    ASSERTMSG(0, ieStrFuncShouldBeOverridden);
    return -1;
  } // DW_AT::GetAttCode
  void SetValue(DW_Value* value_) {mValue = value_;}
  DW_Value* GetValue() {return mValue;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual const string& GetName() const {
    static string retVal("");
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return retVal;
  } // DW_AT::GetName
private:
  DW_Value* mValue;
  template<class T> friend class Singleton;
}; // class DW_AT

#define NEWAT(name__, value__, valid_value_classes__) \
class name__ : public DW_AT { \
private: \
  name__() {} \
public: \
  name__(DW_Value* value_) : DW_AT(value_) {} \
  virtual ~name__() {} \
  virtual hFUInt32 GetAttCode() const {return value__;} \
  virtual void GetValidValues(vector<DW_Value*>& valids_) { \
    valid_value_classes__; \
  } \
  virtual const string& GetName() const { \
    static string retVal(#name__); \
    return retVal; \
  } \
private: \
  template<class T> friend class Singleton; \
};

#define VALUECLASS0()
#define VALUECLASS1(valClass) valids_.push_back(&Singleton<valClass>::Obj());
#define VALUECLASS2(vc1, vc2) VALUECLASS1(vc1) VALUECLASS1(vc2)
#define VALUECLASS3(vc1, vc2, vc3) VALUECLASS2(vc1, vc2) VALUECLASS1(vc3)

// attribute encodings, part 1

NEWAT(DW_AT_sibling, eDW_AT_sibling, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_location, eDW_AT_location, VALUECLASS2(DWC_Block, DWC_Constant));
NEWAT(DW_AT_name, eDW_AT_name, VALUECLASS1(DWC_StringBase));
NEWAT(DW_AT_ordering, eDW_AT_ordering, VALUECLASS0());
NEWAT(DW_AT_byte_size, eDW_AT_byte_size, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_bit_offset, eDW_AT_bit_offset, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_bit_size, eDW_AT_bit_size, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_stmt_list, eDW_AT_stmt_list, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_low_pc, eDW_AT_low_pc, VALUECLASS1(DWC_Address));
NEWAT(DW_AT_high_pc, eDW_AT_high_pc, VALUECLASS1(DWC_Address));
NEWAT(DW_AT_language, eDW_AT_language, VALUECLASS0());
NEWAT(DW_AT_discr, eDW_AT_discr, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_discr_value, eDW_AT_discr_value, VALUECLASS0());
NEWAT(DW_AT_visibility, eDW_AT_visibility, VALUECLASS0());
NEWAT(DW_AT_import, eDW_AT_import, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_string_length, eDW_AT_string_length, VALUECLASS2(DWC_Block, DWC_Constant));
NEWAT(DW_AT_common_reference, eDW_AT_common_reference, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_comp_dir, eDW_AT_comp_dir, VALUECLASS1(DWC_StringBase));
NEWAT(DW_AT_const_value, eDW_AT_const_value, VALUECLASS3(DWC_StringBase, DWC_Constant, DWC_Block));
NEWAT(DW_AT_containing_type, eDW_AT_containing_type, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_default_value, eDW_AT_default_value, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_inline, eDW_AT_inline, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_is_optional, eDW_AT_is_optional, VALUECLASS1(DWC_Flag));
NEWAT(DW_AT_lower_bound, eDW_AT_lower_bound, VALUECLASS2(DWC_Constant, DWC_Reference));
NEWAT(DW_AT_producer, eDW_AT_producer, VALUECLASS2(DWC_Constant, DWC_Reference));
NEWAT(DW_AT_prototyped, eDW_AT_prototyped, VALUECLASS1(DWC_Flag));
NEWAT(DW_AT_return_addr, eDW_AT_return_addr, VALUECLASS2(DWC_Block, DWC_Constant));
NEWAT(DW_AT_start_scope, eDW_AT_start_scope, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_stride_size, eDW_AT_stride_size, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_upper_bound, eDW_AT_upper_bound, VALUECLASS2(DWC_Constant, DWC_Reference));
// attribute encodings, 2
NEWAT(DW_AT_abstract_origin, eDW_AT_abstract_origin, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_accessibility, eDW_AT_accessibility, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_address_class, eDW_AT_address_class, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_artificial, eDW_AT_artificial, VALUECLASS1(DWC_Flag));
NEWAT(DW_AT_base_types, eDW_AT_base_types, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_calling_convention, eDW_AT_calling_convention, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_count, eDW_AT_count, VALUECLASS2(DWC_Constant, DWC_Reference));
NEWAT(DW_AT_data_member_location, eDW_AT_data_member_location, VALUECLASS2(DWC_Block, DWC_Reference));
NEWAT(DW_AT_decl_column, eDW_AT_decl_column, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_decl_file, eDW_AT_decl_file, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_decl_line, eDW_AT_decl_line, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_declaration, eDW_AT_declaration, VALUECLASS1(DWC_Flag));
NEWAT(DW_AT_discr_list, eDW_AT_discr_list, VALUECLASS1(DWC_Block));
NEWAT(DW_AT_encoding, eDW_AT_encoding, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_external, eDW_AT_external, VALUECLASS1(DWC_Flag));
NEWAT(DW_AT_frame_base, eDW_AT_frame_base, VALUECLASS2(DWC_Block, DWC_Constant));
NEWAT(DW_AT_friend, eDW_AT_friend, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_identifier_case, eDW_AT_identifier_case, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_macro_info, eDW_AT_macro_info, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_namelist_item, eDW_AT_namelist_item, VALUECLASS1(DWC_Block));
NEWAT(DW_AT_priority, eDW_AT_priority, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_segment, eDW_AT_segment, VALUECLASS2(DWC_Block, DWC_Constant));
NEWAT(DW_AT_specification, eDW_AT_specification, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_static_link, eDW_AT_static_link, VALUECLASS2(DWC_Block, DWC_Constant));
NEWAT(DW_AT_type, eDW_AT_type, VALUECLASS1(DWC_Reference));
NEWAT(DW_AT_use_location, eDW_AT_use_location, VALUECLASS2(DWC_Block, DWC_Constant));
NEWAT(DW_AT_variable_parameter, eDW_AT_variable_parameter, VALUECLASS1(DWC_Flag));
NEWAT(DW_AT_virtuality, eDW_AT_virtuality, VALUECLASS1(DWC_Constant));
NEWAT(DW_AT_vtable_elem_location, eDW_AT_vtable_elem_location, VALUECLASS2(DWC_Block, DWC_Reference));
NEWAT(DW_AT_lo_user, eDW_AT_lo_user, VALUECLASS0());
NEWAT(DW_AT_hi_user, eDW_AT_hi_user, VALUECLASS0());

class DW_TAG : public DW_DIE {
public:
  DW_TAG(DIEBase* parent_);
  DW_TAG(const DW_TAG& tag_) :
    DW_DIE(tag_),
    mParent(tag_.mParent)
  {
  } // DW_TAG::DW_TAG
  virtual ~DW_TAG() {}
  virtual hFUInt32 GetValueCode() const {
    ASSERTMSG(0, ieStrFuncShouldBeOverridden);
    return -1;
  } // DW_TAG::GetValueCode
  virtual const string& GetName() const {
    static string retVal("");
    ASSERTMSG(0, ieStrFuncShouldBeOverridden);
    return retVal;
  } // DW_TAG::GetName
  DW_DIE* GetParent() const {return mParent;}
  DW_TAG* GetCompUnit() const;
  virtual DIEBase* Clone() const {return new DW_TAG(*this);}
private:
  virtual void GetValidAttributes(vector<DW_AT*>& attributes_) const {
    ASSERTMSG(0, ieStrFuncShouldBeOverridden);
    return;
  } // DW_TAG::GetApplicableAttributes
  DW_DIE* mParent;
}; // class DW_TAG

#define NEWTAG(name, code, valid_attributes) \
class name : public DW_TAG { \
public: \
  name(DIEBase* parent_) : DW_TAG(parent_) {} \
  name(const name& tag_) : DW_TAG(tag_) {} \
  virtual ~name() {} \
  virtual hFUInt32 GetValueCode() const {return code;} \
  virtual void GetValidAttributes(vector<DW_AT*>& valids_) const { \
    valid_attributes; \
  } \
  virtual const string& GetName() const { \
    static string retVal(#name); \
    return retVal; \
  } \
  virtual DIEBase* Clone() const {return new name(*this);} \
};

#define VALIDATS()
#define VALIDAT(at) valids_.push_back(&Singleton<at>::Obj());
#define VALIDATS_DECLS VALIDAT(DW_AT_decl_column) \
  VALIDAT(DW_AT_decl_file) VALIDAT(DW_AT_decl_line)
#define VALIDATS_DECL1(at) VALIDATS_DECLS VALIDAT(at)
#define VALIDATS_DECL2(at1, at2) VALIDATS_DECL1(at1) VALIDAT(at2)
#define VALIDATS_DECL3(at1, at2, at3) VALIDATS_DECL2(at1, at2) VALIDAT(at3)
#define VALIDATS_DECL4(at1, at2, at3, at4) VALIDATS_DECL3(at1, at2, at3) VALIDAT(at4)
#define VALIDATS_DECL5(at1, at2, at3, at4, at5) VALIDATS_DECL4(at1, at2, at3, at4) VALIDAT(at5)
#define VALIDATS_DECL6(at1, at2, at3, at4, at5, at6) VALIDATS_DECL5(at1, at2, at3, at4, at5) VALIDAT(at6)
#define VALIDATS_DECL7(at1, at2, at3, at4, at5, at6, at7) VALIDATS_DECL6(at1, at2, at3, at4, at5, at6) VALIDAT(at7)
#define VALIDATS_DECL8(at1, at2, at3, at4, at5, at6, at7, at8) VALIDATS_DECL7(at1, at2, at3, at4, at5, at6, at7) VALIDAT(at8)
#define VALIDATS_DECL9(at1, at2, at3, at4, at5, at6, at7, at8, at9) VALIDATS_DECL8(at1, at2, at3, at4, at5, at6, at7, at8) VALIDAT(at9)
#define VALIDATS_DECL10(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10) VALIDATS_DECL9(at1, at2, at3, at4, at5, at6, at7, at8, at9) VALIDAT(at10)
#define VALIDATS_DECL11(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11) VALIDATS_DECL10(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10) VALIDAT(at11)
#define VALIDATS_DECL12(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12) VALIDATS_DECL11(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11) VALIDAT(at12)
#define VALIDATS_DECL13(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13) VALIDATS_DECL12(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12) VALIDAT(at13)
#define VALIDATS_DECL14(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14) VALIDATS_DECL13(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13) VALIDAT(at14)
#define VALIDATS_DECL15(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15) VALIDATS_DECL14(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14) VALIDAT(at15)
#define VALIDATS_DECL16(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16) VALIDATS_DECL15(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15) VALIDAT(at16)
#define VALIDATS_DECL17(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17) VALIDATS_DECL16(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16) VALIDAT(at17)
#define VALIDATS_DECL18(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18) VALIDATS_DECL17(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17) VALIDAT(at18)
#define VALIDATS_DECL19(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19) VALIDATS_DECL18(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18) VALIDAT(at19)
#define VALIDATS_DECL20(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20) VALIDATS_DECL19(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19) VALIDAT(at20)
#define VALIDATS_DECL21(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21) VALIDATS_DECL20(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20) VALIDAT(at21)
#define VALIDATS_DECL22(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21, at22) VALIDATS_DECL21(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21) VALIDAT(at22)
#define VALIDATS_DECL23(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21, at22, at23) VALIDATS_DECL22(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21, at22) VALIDAT(at23)
// no decls.
#define VALIDATS1(at) VALIDAT(at)
#define VALIDATS2(at1, at2) VALIDATS1(at1) VALIDAT(at2)
#define VALIDATS3(at1, at2, at3) VALIDATS2(at1, at2) VALIDAT(at3)
#define VALIDATS4(at1, at2, at3, at4) VALIDATS3(at1, at2, at3) VALIDAT(at4)
#define VALIDATS5(at1, at2, at3, at4, at5) VALIDATS4(at1, at2, at3, at4) VALIDAT(at5)
#define VALIDATS6(at1, at2, at3, at4, at5, at6) VALIDATS5(at1, at2, at3, at4, at5) VALIDAT(at6)
#define VALIDATS7(at1, at2, at3, at4, at5, at6, at7) VALIDATS6(at1, at2, at3, at4, at5, at6) VALIDAT(at7)
#define VALIDATS8(at1, at2, at3, at4, at5, at6, at7, at8) VALIDATS7(at1, at2, at3, at4, at5, at6, at7) VALIDAT(at8)
#define VALIDATS9(at1, at2, at3, at4, at5, at6, at7, at8, at9) VALIDATS8(at1, at2, at3, at4, at5, at6, at7, at8) VALIDAT(at9)
#define VALIDATS10(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10) VALIDATS9(at1, at2, at3, at4, at5, at6, at7, at8, at9) VALIDAT(at10)
#define VALIDATS11(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11) VALIDATS10(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10) VALIDAT(at11)
#define VALIDATS12(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12) VALIDATS11(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11) VALIDAT(at12)
#define VALIDATS13(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13) VALIDATS12(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12) VALIDAT(at13)
#define VALIDATS14(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14) VALIDATS13(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13) VALIDAT(at14)
#define VALIDATS15(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15) VALIDATS14(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14) VALIDAT(at15)
#define VALIDATS16(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16) VALIDATS15(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15) VALIDAT(at16)
#define VALIDATS17(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17) VALIDATS16(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16) VALIDAT(at17)
#define VALIDATS18(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18) VALIDATS17(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17) VALIDAT(at18)
#define VALIDATS19(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19) VALIDATS18(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18) VALIDAT(at19)
#define VALIDATS20(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20) VALIDATS19(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19) VALIDAT(at20)
#define VALIDATS21(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21) VALIDATS20(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20) VALIDAT(at21)
#define VALIDATS22(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21, at22) VALIDATS21(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21) VALIDAT(at22)
#define VALIDATS23(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21, at22, at23) VALIDATS22(at1, at2, at3, at4, at5, at6, at7, at8, at9, at10, at11, at12, at13, at14, at15, at16, at17, at18, at19, at20, at21, at22) VALIDAT(at23)

NEWTAG(DW_TAG_array_type, eDW_TAG_array_type, VALIDATS_DECL11(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_ordering,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_stride_size,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_class_type, eDW_TAG_class_type, VALIDATS_DECL8(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_visibility));
NEWTAG(DW_TAG_entry_point, eDW_TAG_entry_point, VALIDATS8(
  DW_AT_address_class,
  DW_AT_low_pc,
  DW_AT_name,
  DW_AT_return_addr,
  DW_AT_segment,
  DW_AT_sibling,
  DW_AT_static_link,
  DW_AT_type));
NEWTAG(DW_TAG_enumeration_type, eDW_TAG_enumeration_type, VALIDATS_DECL8(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_visibility));
NEWTAG(DW_TAG_formal_parameter, eDW_TAG_formal_parameter, VALIDATS_DECL10(
  DW_AT_abstract_origin,
  DW_AT_artificial,
  DW_AT_default_value,
  DW_AT_is_optional,
  DW_AT_location,
  DW_AT_name,
  DW_AT_segment,
  DW_AT_sibling,
  DW_AT_type,
  DW_AT_variable_parameter));
NEWTAG(DW_TAG_imported_declaration, eDW_TAG_imported_declaration, VALIDATS_DECL5(
  DW_AT_accessibility,
  DW_AT_import,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope));
NEWTAG(DW_TAG_label, eDW_TAG_label, VALIDATS6(
  DW_AT_abstract_origin,
  DW_AT_low_pc,
  DW_AT_name,
  DW_AT_segment,
  DW_AT_start_scope,
  DW_AT_sibling));
NEWTAG(DW_TAG_lexical_block, eDW_TAG_lexical_block, VALIDATS6(
  DW_AT_abstract_origin,
  DW_AT_high_pc,
  DW_AT_low_pc,
  DW_AT_name,
  DW_AT_segment,
  DW_AT_sibling));
NEWTAG(DW_TAG_member, eDW_TAG_member, VALIDATS_DECL10(
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_bit_offset,
  DW_AT_bit_size,
  DW_AT_data_member_location,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_pointer_type, eDW_TAG_pointer_type, VALIDATS3(
  DW_AT_address_class,
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_reference_type, eDW_TAG_reference_type, VALIDATS3(
  DW_AT_address_class,
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_compile_unit, eDW_TAG_compile_unit, VALIDATS11(
  DW_AT_base_types,
  DW_AT_comp_dir,
  DW_AT_identifier_case,
  DW_AT_high_pc,
  DW_AT_language,
  DW_AT_low_pc,
  DW_AT_macro_info,
  DW_AT_name,
  DW_AT_producer,
  DW_AT_sibling,
  DW_AT_stmt_list));
NEWTAG(DW_TAG_string_type, eDW_TAG_string_type, VALIDATS_DECL9(
  DW_AT_accessibility,
  DW_AT_abstract_origin,
  DW_AT_byte_size,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_string_length,
  DW_AT_visibility));
NEWTAG(DW_TAG_structure_type, eDW_TAG_structure_type, VALIDATS_DECL8(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_visibility));
NEWTAG(DW_TAG_subroutine_type, eDW_TAG_subroutine_type, VALIDATS_DECL10(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_address_class,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_prototyped,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_typedef, eDW_TAG_typedef, VALIDATS_DECL8(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_union_type, eDW_TAG_union_type, VALIDATS_DECL9(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_declaration,
  DW_AT_friend,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_visibility));
NEWTAG(DW_TAG_unspecified_parameters, eDW_TAG_unspecified_parameters, VALIDATS_DECL3(
  DW_AT_abstract_origin,
  DW_AT_artificial,
  DW_AT_sibling));
NEWTAG(DW_TAG_variant, eDW_TAG_variant, VALIDATS_DECL6(
  DW_AT_accessibility,
  DW_AT_abstract_origin,
  DW_AT_declaration,
  DW_AT_discr_list,
  DW_AT_discr_value,
  DW_AT_sibling));
NEWTAG(DW_TAG_common_block, eDW_TAG_common_block, VALIDATS_DECL5(
  DW_AT_declaration,
  DW_AT_location,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_visibility));
NEWTAG(DW_TAG_common_inclusion, eDW_TAG_common_inclusion, VALIDATS_DECL4(
  DW_AT_common_reference,
  DW_AT_declaration,
  DW_AT_sibling,
  DW_AT_visibility));
NEWTAG(DW_TAG_inheritance, eDW_TAG_inheritance, VALIDATS_DECL5(
  DW_AT_accessibility,
  DW_AT_data_member_location,
  DW_AT_sibling,
  DW_AT_type,
  DW_AT_virtuality));
NEWTAG(DW_TAG_inlined_subroutine, eDW_TAG_inlined_subroutine, VALIDATS_DECL7(
  DW_AT_abstract_origin,
  DW_AT_high_pc,
  DW_AT_low_pc,
  DW_AT_segment,
  DW_AT_sibling,
  DW_AT_return_addr,
  DW_AT_start_scope));
NEWTAG(DW_TAG_module, eDW_TAG_module, VALIDATS_DECL9(
  DW_AT_accessibility,
  DW_AT_declaration,
  DW_AT_high_pc,
  DW_AT_low_pc,
  DW_AT_name,
  DW_AT_priority,
  DW_AT_segment,
  DW_AT_sibling,
  DW_AT_visibility));
NEWTAG(DW_TAG_ptr_to_member_type, eDW_TAG_ptr_to_member_type, VALIDATS_DECL9(
  DW_AT_abstract_origin,
  DW_AT_address_class,
  DW_AT_containing_type,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_type,
  DW_AT_use_location,
  DW_AT_visibility));
NEWTAG(DW_TAG_set_type, eDW_TAG_set_type, VALIDATS_DECL9(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_declaration,
  DW_AT_name,
  DW_AT_start_scope,
  DW_AT_sibling,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_subrange_type, eDW_TAG_subrange_type, VALIDATS_DECL11(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_byte_size,
  DW_AT_count,
  DW_AT_declaration,
  DW_AT_lower_bound,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_type,
  DW_AT_upper_bound,
  DW_AT_visibility));
NEWTAG(DW_TAG_with_stmt, eDW_TAG_with_stmt, VALIDATS10(
  DW_AT_accessibility,
  DW_AT_address_class,
  DW_AT_declaration,
  DW_AT_high_pc,
  DW_AT_location,
  DW_AT_low_pc,
  DW_AT_segment,
  DW_AT_sibling,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_access_declaration, eDW_TAG_access_declaration, VALIDATS_DECL3(
  DW_AT_accessibility,
  DW_AT_name,
  DW_AT_sibling));
NEWTAG(DW_TAG_base_type, eDW_TAG_base_type, VALIDATS6(
  DW_AT_bit_offset,
  DW_AT_bit_size,
  DW_AT_byte_size,
  DW_AT_encoding,
  DW_AT_name,
  DW_AT_sibling));
NEWTAG(DW_TAG_catch_block, eDW_TAG_catch_block, VALIDATS5(
  DW_AT_abstract_origin,
  DW_AT_high_pc,
  DW_AT_low_pc,
  DW_AT_segment,
  DW_AT_sibling));
NEWTAG(DW_TAG_const_type, eDW_TAG_const_type, VALIDATS2(
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_constant, eDW_TAG_constant, VALIDATS_DECL9(
  DW_AT_accessibility,
  DW_AT_const_value,
  DW_AT_declaration,
  DW_AT_external,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_enumerator, eDW_TAG_enumerator, VALIDATS_DECL3(
  DW_AT_const_value,
  DW_AT_name,
  DW_AT_sibling));
NEWTAG(DW_TAG_file_type, eDW_TAG_file_type, VALIDATS_DECL7(
  DW_AT_abstract_origin,
  DW_AT_byte_size,
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_start_scope,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_friend, eDW_TAG_friend, VALIDATS_DECL3(
  DW_AT_abstract_origin,
  DW_AT_friend,
  DW_AT_sibling));
NEWTAG(DW_TAG_namelist, eDW_TAG_namelist, VALIDATS_DECL5(
  DW_AT_accessibility,
  DW_AT_abstract_origin,
  DW_AT_declaration,
  DW_AT_sibling,
  DW_AT_visibility));
NEWTAG(DW_TAG_namelist_item, eDW_TAG_namelist_item, VALIDATS_DECL2(
  DW_AT_namelist_item,
  DW_AT_sibling));
NEWTAG(DW_TAG_packed_type, eDW_TAG_packed_type, VALIDATS2(
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_subprogram, eDW_TAG_subprogram, VALIDATS_DECL23(
  DW_AT_abstract_origin,
  DW_AT_accessibility,
  DW_AT_address_class,
  DW_AT_artificial,
  DW_AT_calling_convention,
  DW_AT_declaration,
  DW_AT_external,
  DW_AT_frame_base,
  DW_AT_high_pc,
  DW_AT_inline,
  DW_AT_low_pc,
  DW_AT_name,
  DW_AT_prototyped,
  DW_AT_return_addr,
  DW_AT_segment,
  DW_AT_sibling,
  DW_AT_specification,
  DW_AT_start_scope,
  DW_AT_static_link,
  DW_AT_type,
  DW_AT_visibility,
  DW_AT_virtuality,
  DW_AT_vtable_elem_location));
NEWTAG(DW_TAG_template_type_param, eDW_TAG_template_type_param, VALIDATS_DECL3(
  DW_AT_name,
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_template_value_param, eDW_TAG_template_value_param, VALIDATS_DECL4(
  DW_AT_name,
  DW_AT_const_value,
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_thrown_type, eDW_TAG_thrown_type, VALIDATS_DECL2(
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_try_block, eDW_TAG_try_block, VALIDATS5(
  DW_AT_abstract_origin,
  DW_AT_high_pc,
  DW_AT_low_pc,
  DW_AT_segment,
  DW_AT_sibling));
NEWTAG(DW_TAG_variant_part, eDW_TAG_variant_part, VALIDATS_DECL6(
  DW_AT_accessibility,
  DW_AT_abstract_origin,
  DW_AT_declaration,
  DW_AT_discr,
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_variable, eDW_TAG_variable, VALIDATS12(
  DW_AT_accessibility,
  DW_AT_const_value,
  DW_AT_declaration,
  DW_AT_external,
  DW_AT_location,
  DW_AT_name,
  DW_AT_segment,
  DW_AT_sibling,
  DW_AT_specification,
  DW_AT_start_scope,
  DW_AT_type,
  DW_AT_visibility));
NEWTAG(DW_TAG_volatile_type, eDW_TAG_volatile_type, VALIDATS2(
  DW_AT_sibling,
  DW_AT_type));
NEWTAG(DW_TAG_lo_user, eDW_TAG_lo_user, VALIDATS(/* LIE */));
NEWTAG(DW_TAG_hi_user, eDW_TAG_hi_user, VALIDATS(/* LIE */));

extern DebugInfoDwarf2 dwarf;

// ///////////////////////
// CALL FRAME INSTRUCTIONS
// ///////////////////////
#define DW_CFA_inst_op0(name__, code__, size__) \
class name__ : public CFInst { \
public: \
  name__() {} \
  virtual ~name__() {} \
  virtual hFUInt32 GetCode() const {return code__;} \
  virtual hFUInt32 GetByteSize() const {return size__;} \
  ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) const; \
};

#define DW_CFA_inst_op1(name__, type__, code__, size__) \
class name__ : public CFInst { \
public: \
  name__(type__ op0_) : mOp0(op0_) {} \
  virtual ~name__() {} \
  virtual hFUInt32 GetCode() const {return code__;} \
  virtual hFUInt32 GetByteSize() const {return size__;} \
  ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) const; \
private: \
  type__ mOp0; \
};

#define DW_CFA_inst_op2(name__, type0__, type1__, code__, size__) \
class name__ : public CFInst { \
public: \
  name__(const type0__& op0_, const type1__& op1_) : mOp0(op0_), mOp1(op1_) {} \
  virtual ~name__() {} \
  virtual hFUInt32 GetCode() const {return code__;} \
  virtual hFUInt32 GetByteSize() const {return size__;} \
  ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) const; \
private: \
  type0__ mOp0; \
  type1__ mOp1; \
};

DW_CFA_inst_op1(DW_CFA_advance_loc, hFUInt8, (0x40 | mOp0), 1);
DW_CFA_inst_op1(DW_CFA_advance_loc1, hFUInt8, eDW_CFA_advance_loc1, 2);
DW_CFA_inst_op1(DW_CFA_advance_loc2, hFUInt16, eDW_CFA_advance_loc2, 3);
DW_CFA_inst_op1(DW_CFA_advance_loc4, hFUInt32, eDW_CFA_advance_loc4, 5);
DW_CFA_inst_op1(DW_CFA_advance_loc4_diff, AddrLTConstDiff, eDW_CFA_advance_loc4, 5);
// reg, offset
DW_CFA_inst_op2(DW_CFA_offset, hFUInt8, ULEB128, (0x80|mOp0), (1+mOp1.GetSize()));
DW_CFA_inst_op2(DW_CFA_offset_extended, ULEB128, ULEB128, eDW_CFA_offset_extended, (1+mOp0.GetSize()+mOp1.GetSize()));
DW_CFA_inst_op1(DW_CFA_restore, hFUInt8, (0xc0|mOp0), 1);
DW_CFA_inst_op1(DW_CFA_restore_extended, ULEB128, eDW_CFA_restore_extended, 1+mOp0.GetSize());
DW_CFA_inst_op1(DW_CFA_undefined, ULEB128, eDW_CFA_undefined, 1+mOp0.GetSize());
DW_CFA_inst_op1(DW_CFA_same_value, ULEB128, eDW_CFA_same_value, 1+mOp0.GetSize());
DW_CFA_inst_op1(DW_CFA_def_cfa_register, ULEB128, eDW_CFA_def_cfa_register, 1+mOp0.GetSize());
// new reg, old reg
DW_CFA_inst_op2(DW_CFA_register, ULEB128, ULEB128, eDW_CFA_register, (1+mOp0.GetSize()+mOp1.GetSize()));
DW_CFA_inst_op1(DW_CFA_def_cfa_offset, ULEB128, eDW_CFA_def_cfa_offset, 1+mOp0.GetSize());
DW_CFA_inst_op2(DW_CFA_def_cfa, ULEB128, ULEB128, eDW_CFA_def_cfa, (1+mOp0.GetSize()+mOp1.GetSize()));
DW_CFA_inst_op0(DW_CFA_remember_state, eDW_CFA_remember_state, 1);
DW_CFA_inst_op0(DW_CFA_nop, eDW_CFA_nop, 1);
DW_CFA_inst_op0(DW_CFA_restore_state, eDW_CFA_restore_state, 1);
DW_CFA_inst_op1(DW_CFA_set_loc, AddrLTConst, eDW_CFA_set_loc, 1+mOp0.GetSize());

// Dwarf operations

class DW_LOC_OP {
public:
  DW_LOC_OP() {}
  virtual ~DW_LOC_OP() {}
  virtual hFUInt16 GetSize() const {
    ASSERTMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DW_LOC_OP::GetSize
  virtual hFUInt32 GetCode() const {
    ASSERTMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DW_LOC_OP::GetCode
  virtual void GetAsValues(vector<DW_Value*>& values_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::GetDisassembly");
  } // DW_LOC_OP::GetAsValues
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::GetDisassembly");
    return o;
  } // DW_LOC_OP::GetDisassembly
}; // class DW_LOC_OP

template<eDW_OP C>
class DW_OP_OP0 : public DW_LOC_OP {
public:
  DW_OP_OP0() {}
  virtual ~DW_OP_OP0() {}
  virtual hFUInt32 GetCode() const {return C;}
  virtual hFUInt16 GetSize() const {return 1;}
  virtual void GetAsValues(vector<DW_Value*>& values_) {
    values_.push_back(new DWC_Data1(GetCode()));
  } // DW_OP_OP0::GetAsValues
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    asmDesc_.PutData1(o, GetCode(), toStr(C)).Endl();
    return o;
  } // DW_OP_OP0::GetDisassembly
}; // class DW_OP_OP0

#define DW_OP_ZERO_OP(name__) \
struct name__ : public DW_OP_OP0<e##name__> { \
  name__() {} \
  virtual ~name__() {} \
};

template<class T, eDW_OP C>
class DW_OP_OP1OBJ : public DW_LOC_OP {
public:
  DW_OP_OP1OBJ(const T& op0_) : mOp0(op0_) {}
  virtual ~DW_OP_OP1OBJ() {}
  virtual hFUInt32 GetCode() const {return C;}
  virtual hFUInt16 GetSize() const {return 1+mOp0.GetSize();}
  virtual void GetAsValues(vector<DW_Value*>& values_) {
    values_.push_back(new DWC_Data1(GetCode()));
    mOp0.GetAsValues(values_);
  }
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    asmDesc_.PutData1(o, GetCode(), toStr(C)).Endl();
    mOp0.GetDisassembly(o, asmDesc_);
    return o;
  }
private:
  T mOp0;
}; // class DW_OP_OP1OBJ

template<class T>
class DW_OP_OP1OBJ<T, eDW_OP_addr> : public DW_LOC_OP {
public:
  DW_OP_OP1OBJ(const T& op0_) : mOp0(op0_) {}
  virtual ~DW_OP_OP1OBJ() {}
  virtual hFUInt32 GetCode() const {return eDW_OP_addr;}
  virtual hFUInt16 GetSize() const {return 1+mOp0.GetSize();}
  virtual void GetAsValues(vector<DW_Value*>& values_) {
    values_.push_back(new DWC_Data1(GetCode()));
    values_.push_back(new DWC_Address(mOp0));
  } // DW_OP_OP1OBJ<T, eDW_OP_addr>::GetAsValues
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    asmDesc_.PutData1(o, GetCode(), toStr(eDW_OP_addr)).Endl();
    asmDesc_.PutRelocAddr(o, mOp0).Endl();
    return o;
  } // DW_OP_OP1OBJ<T, eDW_OP_addr>::GetDisassembly
private:
  T mOp0;
}; // class DW_OP_OP1OBJ<T, eDW_OP_addr>


#define DW_OP_OP1OBJ(name__, type__) \
struct name__ : public DW_OP_OP1OBJ<type__, e##name__> { \
  name__(const type__& op0_) : DW_OP_OP1OBJ<type__, e##name__>(op0_) {} \
  virtual ~name__() {} \
};

template<class T, eDW_OP C>
class DW_OP_OP1 : public DW_LOC_OP {
public:
  DW_OP_OP1(const T& op0_) :
    mOp0(op0_)
  {
  } // DW_OP_OP1::DW_OP_OP1
  virtual ~DW_OP_OP1() {}
  virtual hFUInt32 GetCode() const {return C;}
  virtual hFUInt16 GetSize() const {return 1+sizeof(T);}
  virtual void GetAsValues(vector<DW_Value*>& values_) {
    values_.push_back(new DWC_Data1(GetCode()));
    if (sizeof(T) == 1) {
      values_.push_back(new DWC_Data1(mOp0));
    } else if (sizeof(T) == 2) {
      values_.push_back(new DWC_Data2(mOp0));
    } else if (sizeof(T) == 4) {
      values_.push_back(new DWC_Data4(mOp0));
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
  } // DW_OP_OP1::GetAsValues
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    asmDesc_.PutData1(o, GetCode(), toStr(C)).Endl();
    if (sizeof(T) == 1) {
      asmDesc_.PutData1(o, mOp0).Endl();
    } else if (sizeof(T) == 2) {
      asmDesc_.PutData2(o, mOp0).Endl();
    } else if (sizeof(T) == 4) {
      asmDesc_.PutData4(o, mOp0).Endl();
    } else {
      ASSERTMSG(0, ieStrNotImplemented);
    } // if
    return o;
  } // DW_OP_OP1::GetDisassembly
private:
  T mOp0;
}; // class DW_OP_OP1

#define DW_OP_OP1(name__, type__) \
struct name__ : public DW_OP_OP1<type__, e##name__> { \
  name__(const type__& op0_) : DW_OP_OP1<type__, e##name__>(op0_) {} \
  virtual ~name__() {} \
};

template<class T0, class T1, eDW_OP C>
class DW_OP_OP2 : public DW_LOC_OP {
public:
  DW_OP_OP2(const T0& op0_, const T1& op1_) :
    mOp0(op0_),
    mOp1(op1_)
  {
  } // DW_OP_OP2::DW_OP_OP2
  virtual hFUInt32 GetCode() const {return C;}
  virtual hFUInt16 GetSize() const {return 1+mOp0.GetSize()+mOp1.GetSize();}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_ = string()) {
    asmDesc_.PutData1(o, GetCode(), toStr(C)).Endl();
    mOp0.GetDisassembly(o, asmDesc_);
    mOp1.GetDisassembly(o, asmDesc_);
    return o;
  } // DW_OP_OP2::GetDisassembly
private:
  T0 mOp0;
  T1 mOp1;
}; // class DW_OP_OP2

#define DW_OP_OP2OBJOBJ(name__, type0__, type1__) \
struct name__ : public DW_OP_OP2<type0__, type1__, e##name__> { \
  name__(const type0__& op0_, const type1__& op1__) : DW_OP_OP2<type0__, type1__, e##name__>(op0_, op1__) {} \
  virtual ~name__() {} \
};

DW_OP_ZERO_OP(DW_OP_lit0); DW_OP_ZERO_OP(DW_OP_lit1); DW_OP_ZERO_OP(DW_OP_lit2); 
DW_OP_ZERO_OP(DW_OP_lit3); DW_OP_ZERO_OP(DW_OP_lit4); DW_OP_ZERO_OP(DW_OP_lit5); 
DW_OP_ZERO_OP(DW_OP_lit6); DW_OP_ZERO_OP(DW_OP_lit7); DW_OP_ZERO_OP(DW_OP_lit8); 
DW_OP_ZERO_OP(DW_OP_lit9); DW_OP_ZERO_OP(DW_OP_lit10); DW_OP_ZERO_OP(DW_OP_lit11); 
DW_OP_ZERO_OP(DW_OP_lit12); DW_OP_ZERO_OP(DW_OP_lit13); DW_OP_ZERO_OP(DW_OP_lit14); 
DW_OP_ZERO_OP(DW_OP_lit15); DW_OP_ZERO_OP(DW_OP_lit16); DW_OP_ZERO_OP(DW_OP_lit17); 
DW_OP_ZERO_OP(DW_OP_lit18); DW_OP_ZERO_OP(DW_OP_lit19); DW_OP_ZERO_OP(DW_OP_lit20); 
DW_OP_ZERO_OP(DW_OP_lit21); DW_OP_ZERO_OP(DW_OP_lit22); DW_OP_ZERO_OP(DW_OP_lit23); 
DW_OP_ZERO_OP(DW_OP_lit24); DW_OP_ZERO_OP(DW_OP_lit25); DW_OP_ZERO_OP(DW_OP_lit26); 
DW_OP_ZERO_OP(DW_OP_lit27); DW_OP_ZERO_OP(DW_OP_lit28); DW_OP_ZERO_OP(DW_OP_lit29); 
DW_OP_ZERO_OP(DW_OP_lit30); DW_OP_ZERO_OP(DW_OP_lit31);

DW_OP_ZERO_OP(DW_OP_reg0); DW_OP_ZERO_OP(DW_OP_reg1); DW_OP_ZERO_OP(DW_OP_reg2); 
DW_OP_ZERO_OP(DW_OP_reg3); DW_OP_ZERO_OP(DW_OP_reg4); DW_OP_ZERO_OP(DW_OP_reg5); 
DW_OP_ZERO_OP(DW_OP_reg6); DW_OP_ZERO_OP(DW_OP_reg7); DW_OP_ZERO_OP(DW_OP_reg8); 
DW_OP_ZERO_OP(DW_OP_reg9); DW_OP_ZERO_OP(DW_OP_reg10); DW_OP_ZERO_OP(DW_OP_reg11); 
DW_OP_ZERO_OP(DW_OP_reg12); DW_OP_ZERO_OP(DW_OP_reg13); DW_OP_ZERO_OP(DW_OP_reg14); 
DW_OP_ZERO_OP(DW_OP_reg15); DW_OP_ZERO_OP(DW_OP_reg16); DW_OP_ZERO_OP(DW_OP_reg17); 
DW_OP_ZERO_OP(DW_OP_reg18); DW_OP_ZERO_OP(DW_OP_reg19); DW_OP_ZERO_OP(DW_OP_reg20); 
DW_OP_ZERO_OP(DW_OP_reg21); DW_OP_ZERO_OP(DW_OP_reg22); DW_OP_ZERO_OP(DW_OP_reg23); 
DW_OP_ZERO_OP(DW_OP_reg24); DW_OP_ZERO_OP(DW_OP_reg25); DW_OP_ZERO_OP(DW_OP_reg26); 
DW_OP_ZERO_OP(DW_OP_reg27); DW_OP_ZERO_OP(DW_OP_reg28); DW_OP_ZERO_OP(DW_OP_reg29); 
DW_OP_ZERO_OP(DW_OP_reg30); DW_OP_ZERO_OP(DW_OP_reg31); 

DW_OP_OP1OBJ(DW_OP_regx, ULEB128);

//! \todo M Design: DW_OP_addr should actually have tBigAddr type.
DW_OP_OP1OBJ(DW_OP_addr, AddrLTConst);
DW_OP_OP1(DW_OP_const1u, hFUInt8);
DW_OP_OP1(DW_OP_const1s, hFSInt8);
DW_OP_OP1(DW_OP_const2u, hFUInt16);
DW_OP_OP1(DW_OP_const2s, hFSInt16);
DW_OP_OP1(DW_OP_const4u, hFUInt32);
DW_OP_OP1(DW_OP_const4s, hFSInt32);
DW_OP_OP1(DW_OP_const8u, hFUInt64);
DW_OP_OP1(DW_OP_const8s, hFSInt64);
DW_OP_OP1OBJ(DW_OP_constu, ULEB128);
DW_OP_OP1OBJ(DW_OP_consts, SLEB128);
DW_OP_OP1OBJ(DW_OP_fbreg, SLEB128);

DW_OP_OP1OBJ(DW_OP_breg0, SLEB128); DW_OP_OP1OBJ(DW_OP_breg1, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg2, SLEB128); DW_OP_OP1OBJ(DW_OP_breg3, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg4, SLEB128); DW_OP_OP1OBJ(DW_OP_breg5, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg6, SLEB128); DW_OP_OP1OBJ(DW_OP_breg7, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg8, SLEB128); DW_OP_OP1OBJ(DW_OP_breg9, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg10, SLEB128); DW_OP_OP1OBJ(DW_OP_breg11, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg12, SLEB128); DW_OP_OP1OBJ(DW_OP_breg13, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg14, SLEB128); DW_OP_OP1OBJ(DW_OP_breg15, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg16, SLEB128); DW_OP_OP1OBJ(DW_OP_breg17, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg18, SLEB128); DW_OP_OP1OBJ(DW_OP_breg19, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg20, SLEB128); DW_OP_OP1OBJ(DW_OP_breg21, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg22, SLEB128); DW_OP_OP1OBJ(DW_OP_breg23, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg24, SLEB128); DW_OP_OP1OBJ(DW_OP_breg25, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg26, SLEB128); DW_OP_OP1OBJ(DW_OP_breg27, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg28, SLEB128); DW_OP_OP1OBJ(DW_OP_breg29, SLEB128);
DW_OP_OP1OBJ(DW_OP_breg30, SLEB128); DW_OP_OP1OBJ(DW_OP_breg31, SLEB128);

DW_OP_OP2OBJOBJ(DW_OP_bregx, ULEB128, SLEB128);

DW_OP_ZERO_OP(DW_OP_dup);
DW_OP_ZERO_OP(DW_OP_drop);
DW_OP_OP1(DW_OP_pick, hFUInt8);
DW_OP_ZERO_OP(DW_OP_over);
DW_OP_ZERO_OP(DW_OP_swap);
DW_OP_ZERO_OP(DW_OP_rot);
DW_OP_ZERO_OP(DW_OP_deref);
DW_OP_OP1(DW_OP_deref_size, hFUInt8);
DW_OP_ZERO_OP(DW_OP_xderef);
DW_OP_OP1(DW_OP_xderef_size, hFUInt8);

DW_OP_ZERO_OP(DW_OP_abs); DW_OP_ZERO_OP(DW_OP_div); DW_OP_ZERO_OP(DW_OP_and); 
DW_OP_ZERO_OP(DW_OP_minus); DW_OP_ZERO_OP(DW_OP_mod); DW_OP_ZERO_OP(DW_OP_mul); 
DW_OP_ZERO_OP(DW_OP_neg); DW_OP_ZERO_OP(DW_OP_or); DW_OP_ZERO_OP(DW_OP_not); 
DW_OP_ZERO_OP(DW_OP_plus); DW_OP_ZERO_OP(DW_OP_shl); DW_OP_ZERO_OP(DW_OP_shr); 
DW_OP_ZERO_OP(DW_OP_shra); DW_OP_ZERO_OP(DW_OP_xor);
  
DW_OP_OP1OBJ(DW_OP_plus_uconst, ULEB128);

DW_OP_ZERO_OP(DW_OP_le); DW_OP_ZERO_OP(DW_OP_lt); DW_OP_ZERO_OP(DW_OP_ge); 
DW_OP_ZERO_OP(DW_OP_gt); DW_OP_ZERO_OP(DW_OP_eq); DW_OP_ZERO_OP(DW_OP_ne); 

DW_OP_OP1(DW_OP_skip, hFSInt16);
DW_OP_OP1OBJ(DW_OP_piece, ULEB128);
DW_OP_ZERO_OP(DW_OP_nop);


#endif // __DEBUGINFO_DWARF_HPP__



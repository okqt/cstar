// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __DEBUGINFO_DWARF_HPP__
#include "debuginfo_dwarf.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif

DebugInfoDwarf2 dwarf;

DW_DIE* Dwarf2CLangHelper::
AddBaseType(DW_DIE* compUnit_, const string& name_, eDW_ATE encoding_, hFUInt16 byteSize_) {
  BMDEBUG5("Dwarf2CLangHelper::AddBaseType", compUnit_, name_, encoding_, byteSize_);
  REQUIREDMSG(dynamic_cast<DW_TAG_compile_unit*>(compUnit_) != 0, ieStrParamValuesDBCViolation);
  DW_DIE* retVal(new DW_TAG_base_type(compUnit_));
  retVal->AddAttribute(new DW_AT_name(new DWC_String(name_)));
  retVal->AddAttribute(new DW_AT_encoding(new DWC_Data1(encoding_)));
  retVal->AddAttribute(new DW_AT_byte_size(new DWC_Data1(byteSize_)));
  EMDEBUG1(retVal);
  return retVal;
} // Dwarf2CLangHelper::AddBaseType

DW_TAG* DebugInfoDwarf2::
AddVariable(DIEBase* parent_, const string& name_, DIEBase* type_, bool external_, bool parameter_, const SrcLoc& srcLoc_) {
  BMDEBUG7("DebugInfoDwarf2::AddVariable", parent_, name_, type_, external_, parameter_, &srcLoc_);
  REQUIREDMSG(dynamic_cast<DW_DIE*>(parent_) != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<DW_DIE*>(type_) != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(external_ == false || parameter_ == false, ieStrParamValuesDBCViolation);
  DW_TAG* retVal(0);
  if (parameter_ == true) {
    retVal = new DW_TAG_formal_parameter(parent_);
  } else {
    retVal = new DW_TAG_variable(parent_);
  } // if
  retVal->AddAttribute(new DW_AT_name(new DWC_String(name_)));
  retVal->AddAttribute(new DW_AT_external(new DWC_Flag(external_)));
  retVal->AddAttribute(new DW_AT_type(new DWC_RefOffset(type_)));
  addDeclAttributes(retVal, srcLoc_);
  EMDEBUG1(retVal);
  return retVal;
} //  DebugInfoDwarf2::AddVariable

DW_TAG* DebugInfoDwarf2::
AddFunction(DIEBase* module_, bool external_, const string& name_, DIEBase* type_, const SrcLoc& srcLoc_) {
  BMDEBUG6("DebugInfoDwarf2::AddFunction", module_, external_, name_, type_, &srcLoc_);
  REQUIREDMSG(dynamic_cast<DW_TAG_compile_unit*>(module_) != 0, ieStrParamValuesDBCViolation);
  DW_TAG* retVal(new DW_TAG_subprogram(module_));
  retVal->AddAttribute(new DW_AT_external(new DWC_Flag(external_)));
  retVal->AddAttribute(new DW_AT_name(new DWC_String(name_)));
  retVal->AddAttribute(new DW_AT_type(new DWC_RefOffset(type_)));
  addDeclAttributes(retVal, srcLoc_);
  EMDEBUG1(retVal);
  return retVal;
} // DebugInfoDwarf2::AddFunction

DW_TAG::
DW_TAG(DIEBase* parent_) :
  mParent(static_cast<DW_DIE*>(parent_))
{
  BMDEBUG2("DW_TAG::DW_TAG", parent_);
  REQUIREDMSG(parent_ == 0 || dynamic_cast<DW_DIE*>(parent_) != 0, ieStrParamValuesDBCViolation);
  if (dynamic_cast<DW_TAG_compile_unit*>(this) != 0) {
    // LIE: compile units do not have a parent.
  } else if (!mParent) {
    ASSERTMSG(0, ieStrNotImplemented);
  } else {
    mParent->AddChild(this);
  } // if
  EMDEBUG0();
} // DW_TAG::DW_TAG

void DW_DIE::
AddAttribute(DW_AT* attributeCode_) {
  BMDEBUG2("DW_DIE::AddAttribute", attributeCode_);
  mAttributes.push_back(attributeCode_);
  EMDEBUG0();
  return;
} // DW_DIE::AddAttribute

DW_TAG* DebugInfoDwarf2::
AddModule(const string& name_, const string& producer_, eDW_LANG language_, const string& compDir_) {
  BMDEBUG5("DebugInfoDwarf2::AddModule", name_, producer_, language_, compDir_);
  DW_TAG* retVal(new DW_TAG_compile_unit(new DW_DIE_null));
  retVal->AddAttribute(new DW_AT_name(new DWC_String(name_)));
  retVal->AddAttribute(new DW_AT_producer(new DWC_String(producer_)));
  retVal->AddAttribute(new DW_AT_language(new DWC_Data1(language_)));
  retVal->AddAttribute(new DW_AT_comp_dir(new DWC_String(compDir_)));
  mCompileUnits.push_back(retVal);
  EMDEBUG1(retVal);
  return retVal;
} // DebugInfoDwarf2::AddModule

DW_DIE* DebugInfoDwarf2::
AddPointerType(DIEBase* parent_, hFUInt32 byteSize_, DIEBase* refType_) {
  BMDEBUG4("DebugInfoDwarf2::AddPointerType", parent_, byteSize_, refType_);
  REQUIREDMSG(parent_ != 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(dynamic_cast<DW_DIE*>(refType_) != 0, ieStrParamValuesDBCViolation);
  DW_DIE* retVal(new DW_TAG_pointer_type(parent_));
  retVal->AddAttribute(new DW_AT_byte_size(new DWC_Data1(byteSize_)));
  //! \todo M Design: refType, retVal and compilation unit must be n the same
  //!       hierarchy, add an assertion.
  retVal->AddAttribute(new DW_AT_type(new DWC_RefOffset(static_cast<DW_DIE*>(refType_))));
  EMDEBUG1(retVal);
  return retVal;
} // DebugInfoDwarf2::AddPointerType

void DF_CIE::
Emit(ASMSection& debugFrame_, const ASMDesc& asmDesc_) {
  // Length: uword, not including the length field itself. length % addr_size must be zero.
  // Id: uword
  // Version: ubyte
  // String augmentation or zero byte.
  // Code alignment factor: ULEB128
  // Data alignment factor: SLEB128
  // Return address register: ubyte
  // initial instructions
  // padding to make length % addr_size 0
  hFUInt32 lLength(6);
  if (!mAugmentation.empty()) {
    lLength += mAugmentation.size() + 1;
  } else {
    ++ lLength;
  } // if
  lLength += mCodeAlignFactor.GetSize() + mDataAlignFactor.GetSize();
  lLength += mSizeOfInsts;
  //! \todo M Design: Get the target address size from somewhere.
  hFUInt32 lPaddingSize(lLength % 4);
  lLength = util.uGetUpAligned(lLength, 4);
  {
    static hFUInt32 lIndex(0);
    mAddrLabel = asmDesc_.GetLocalLabel("CIE", lIndex ++);
  } // block
  asmDesc_.PutLabel(debugFrame_, mAddrLabel, "CIE").Endl();
  asmDesc_.PutData4(debugFrame_, lLength, "CIE length").Endl();
  asmDesc_.PutData4(debugFrame_, mID, "CIE id").Endl();
  asmDesc_.PutData1(debugFrame_, mVersion, "CIE version").Endl();
  if (!mAugmentation.empty()) {
    asmDesc_.PutDataString(debugFrame_, mAugmentation, "CIE augmentation").Endl();
  } else {
    asmDesc_.PutData1(debugFrame_, 0, "CIE augmentation").Endl();
  } // if
  mCodeAlignFactor.GetDisassembly(debugFrame_, asmDesc_, "CIE code alignment");
  mDataAlignFactor.GetDisassembly(debugFrame_, asmDesc_, "CIE data alignment");
  asmDesc_.PutData1(debugFrame_, mRetAddrRegNum, "CIE return address register number").Endl();
  for (hFUInt32 c(0); c < mInsts.size(); ++ c) {
    mInsts[c]->GetDisassembly(debugFrame_, asmDesc_, "CIE inits");
  } // for
  if (lPaddingSize != 0) {
    asmDesc_.PutSpace(debugFrame_, lPaddingSize, "Length padding").Endl();
  } // if
} // DF_CIE::Emit

DW_TAG* DW_TAG::
GetCompUnit() const {
  BMDEBUG1("DW_DIE::GetCompUnit");
  DW_TAG* retVal(0);
  if (dynamic_cast<DW_DIE_null*>(GetParent()) != 0) {
    ASSERTMSG(dynamic_cast<const DW_TAG_compile_unit*>(this) != 0, ieStrInconsistentInternalStructure);
    retVal = const_cast<DW_TAG*>(this);
  } else {
    DW_TAG* lParent(static_cast<DW_TAG*>(static_cast<const DW_TAG*>(this)->GetParent()));
    retVal = lParent->GetCompUnit();
  } // if
  ENSUREMSG(retVal != 0, ieStrMustEnsureNonNull);
  EMDEBUG1(retVal);
  return retVal;
} // DW_DIE::GetCompUnit

void DebugInfoDwarf2::
addDeclAttributes(DW_DIE* tag_, hFUInt16 column_, const string& fileName_, hFUInt32 line_) {
  BMDEBUG4("DebugInfoDwarf2::addDeclAttributes", column_, fileName_, line_);
  REQUIREDMSG(tag_ != 0, ieStrParamValuesDBCViolation);
  //! \todo M Design: Do the check if tag_ accepts the attributes.
  //! \todo M Design: Data2 should be lowered if column is a small number.
  tag_->AddAttribute(new DW_AT_decl_column(new DWC_Data2(column_)));
  //! \todo M Design: convert fileName_ in to an index.
  tag_->AddAttribute(new DW_AT_decl_file(new DWC_Data1(1)));
  tag_->AddAttribute(new DW_AT_decl_line(new DWC_Data2(line_)));
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::addDeclAttributes

void SLEB128::
encode() {
  bool lMore(true);
  bool lNegative(mValue < 0);
  hBigInt lTmpValue(mValue);
  while (lMore == true) {
    hFUInt8 lByte((lTmpValue & 0x7F).get_hFUInt32());
    lTmpValue >>= 7;
    if ((!lTmpValue && !(lByte & 0x40)) ||
        (lTmpValue == -1 && (lByte & 0x40))) {
      lMore = false;
    } else {
      lByte |= 0x80;
    } // if
    mBytes.push_back(lByte);
  } // while
} // SLEB128::encode

SLEB128::
SLEB128(const hBigInt& value_) :
  mValue(value_)
{
  encode();
} // SLEB128::SLEB128

void SLEB128::
decode() {
  mValue = 0;
  hFUInt16 lShift(0);
  hFUInt8 lByteIndex(0);
  hFUInt8 lByte;
  while (true) {
    lByte = mBytes[lByteIndex ++];
    mValue |= hBigInt(lByte & 0x7F) << lShift;
    lShift += 7;
    if (!(lByte & 0x80)) {
      break;
    } // if
  } // while
  if (lByte & 0x40) {
    // sign extend
    mValue |= - (hBigInt(1) << lShift);
  } // if
} // SLEB128::decode

SLEB128::
SLEB128(const vector<hFUInt8>& value_) :
  mBytes(value_)
{
  decode();
} // SLEB128::SLEB128

void ULEB128::
encode() {
  // Calculate the bytes, see dwarf2 standard appendix 4.
  hBigInt lTmpValue(mValue);
  do {
    hFUInt8 lByte((lTmpValue & 0x7F).get_hFUInt32());
    lTmpValue >>= 7;
    if (lTmpValue != 0) {
      lByte |= 0x80;
    } // if
    mBytes.push_back(lByte);
  } while (lTmpValue != 0);
} // ULEB128::encode

ULEB128::
ULEB128(const hBigInt& value_) :
  mValue(value_)
{
  encode();
} // ULEB128::ULEB128

void ULEB128::
decode() {
  mValue = 0;
  hFUInt16 lShift(0);
  hFUInt16 lByteIndex(0);
  while (true) {
    hFUInt8 lByte(mBytes[lByteIndex ++]);
    mValue |= hBigInt(lByte & 0x7F) << lShift;
    if (!(lByte & 0x80)) {
      break;
    } // if
    lShift += 7;
  } // while
} // ULEB128::decode

ULEB128::
ULEB128(const hFUInt32& value_) :
  mValue(value_)
{
  encode();
} // ULEB128::ULEB128

SLEB128::
SLEB128(const hFSInt32& value_) :
  mValue(value_)
{
  encode();
} // SLEB128::SLEB128

ULEB128::
ULEB128(const vector<hFUInt8>& value_) :
  mBytes(value_)
{
  decode();
} // ULEB128::ULEB128

void DebugInfoDwarf2::
OnInit() {
  BMDEBUG1("DebugInfoDwarf2::OnInit");
  EMDEBUG0();
} // DebugInfoDwarf2::OnInit

class AbbrevKey {
  public:
    AbbrevKey(hFUInt32 tagCode_, const vector<DW_AT*>& attributes_) :
      mTagCode(tagCode_)
    {
      for (hFUInt32 c(0); c < attributes_.size(); ++ c) {
        union {
          hFUInt64 mKey;
          struct {
            hFUInt32 mAttCode;
            hFUInt32 mFormCode;
          } mIn;
        } lPairVal;
        lPairVal.mIn.mAttCode = attributes_[c]->GetAttCode();
        lPairVal.mIn.mFormCode = attributes_[c]->GetValue()->GetFormCode();
        mAttFormPairs.push_back(lPairVal.mKey);
      } // for
    } // AbbrevKey::AbbrevKey
    AbbrevKey() {}
    AbbrevKey(const AbbrevKey& ak_) :
      mTagCode(ak_.mTagCode),
      mAttFormPairs(ak_.mAttFormPairs),
      mAbbrevId(ak_.mAbbrevId)
    {
    } // AbbrevKey::AbbrevKey
    AbbrevKey& operator = (const AbbrevKey& ak_) {
      mTagCode = ak_.mTagCode;
      mAttFormPairs = ak_.mAttFormPairs;
      mAbbrevId = ak_.mAbbrevId;
      return *this;
    } // AbbrevKey::operator =
    bool operator < (const AbbrevKey& ak_) const {
      bool retVal(false);
      if (mTagCode < ak_.mTagCode) {
        retVal = true;
      } else if (mTagCode == ak_.mTagCode) {
        if (mAttFormPairs.size() < ak_.mAttFormPairs.size()) {
          retVal = true;
        } else if (mAttFormPairs.size() == ak_.mAttFormPairs.size()) {
          for (hFUInt32 c(0); c < mAttFormPairs.size(); ++ c) {
            if (mAttFormPairs[c] < ak_.mAttFormPairs[c]) {
              retVal = true;
              break;
            } else if (mAttFormPairs[c] > ak_.mAttFormPairs[c]) {
              break;
            } // if
          } // for
        } // if
      } // if
      return retVal;
    } // AbbrevKey::operator <
    hFUInt32 mTagCode; //!< Tag code.
    vector<hFUInt64> mAttFormPairs;
    hFUInt32 mAbbrevId; //!< Id of this abbreviation, 0 is reserved.
    static hFUInt32 mAbbrevIdCount; //!< keep track of used Ids.
}; // class AbbrevKey

hFUInt32 AbbrevKey::mAbbrevIdCount(1);

void DF_FDE::
Emit(ASMSection& debugFrame_, const ASMDesc& asmDesc_) {
  // Emit the followings in order:
  // - Length of FDE: uword, not including the length field itself, i.e. sum of all below.
  // - CIE_Pointer: uword, offset from .debug_frame.
  // - Initial location: e.g. start of function.
  // - Range of addresses: e.g. length of function.
  // - Instructions.
  //!  \todo M Design: Need to consider the size of address in the current target.
  hFUInt32 lLength(12 + mSizeOfInsts);
  //! \todo M Design: adapt 4 to addressing size.
  hFUInt32 lPaddingSize(lLength % 4);
  lLength = util.uGetUpAligned(lLength, 4);
  asmDesc_.PutData4(debugFrame_, lLength, "FDE length").Endl();
  asmDesc_.PutRelocAddrDiff(debugFrame_, mCIE->GetAddrLabel(), ".debug_frame", "CIE offset").Endl();
  asmDesc_.PutRelocAddr(debugFrame_, IRBuilder::smirbGetAttachedLabel(mFunc, "beg"), "Init location").Endl();
  asmDesc_.PutRelocAddrDiff(debugFrame_, IRBuilder::smirbGetAttachedLabel(mFunc, "end"),
    IRBuilder::smirbGetAttachedLabel(mFunc, "beg"), "Addr range").Endl();
  for (hFUInt32 c(0); c < mInsts.size(); ++ c) {
    mInsts[c]->GetDisassembly(debugFrame_, asmDesc_, "FDE");
  } // for
  if (lPaddingSize != 0) {
    asmDesc_.PutSpace(debugFrame_, lPaddingSize, "Length padding").Endl();
  } // if
  return;
} // DF_FDE::Emit

void DW_DIE::
emitDIE(ASMSection& debugInfo_, ASMSection& debugAbbrev_, const ASMDesc& asmDesc_) {
  BMDEBUG1("DW_DIE::emitDIE");
  // First check the abbreviation entry if we have already emitted the same?
  static set<AbbrevKey> slAbbrevs;
  AbbrevKey lCurrKey(static_cast<DW_TAG*>(this)->GetValueCode(), mAttributes);
  set<AbbrevKey>::iterator lIt(slAbbrevs.find(lCurrKey));
  hFUInt32 lAbbrevId;
  if (lIt != slAbbrevs.end() && mChildren.empty() == true) {
    lAbbrevId = lIt->mAbbrevId;
    // We already have an abbreviation table entry, just use that.
  } else {
    lCurrKey.mAbbrevId = AbbrevKey::mAbbrevIdCount ++;
    lAbbrevId = lCurrKey.mAbbrevId;
    slAbbrevs.insert(lCurrKey);
    // Add the tag/atts in to the abbreviation table:
    //   - mAbbrevId in ULEB128 format,
    //   - tag code ULEB128,
    //   - has child? 1 byte DW_CHILDREN
    //   - N times <ULEB128, ULEB128> DW_AT code, DW_FORM code.
    {
      ULEB128 lAbbrevIdULEB(lAbbrevId);
      ULEB128 lTagCode(lCurrKey.mTagCode);
      eDW_CHILDREN lHasChild(!mChildren.empty() ? eDW_CHILDREN_yes : eDW_CHILDREN_no);
      lAbbrevIdULEB.GetDisassembly(debugAbbrev_, asmDesc_);
      lTagCode.GetDisassembly(debugAbbrev_, asmDesc_);
      asmDesc_.PutData1(debugAbbrev_, lHasChild, GetAbbrevComment(debugAbbrev_)).Endl();
      for (hFUInt16 c(0); c < mAttributes.size(); ++ c) {
        ULEB128 lAttCode(mAttributes[c]->GetAttCode());
        ULEB128 lFormCode(mAttributes[c]->GetValue()->GetFormCode());
        lAttCode.GetDisassembly(debugAbbrev_, asmDesc_);
        lFormCode.GetDisassembly(debugAbbrev_, asmDesc_);
      } // for
      asmDesc_.PutData1(debugAbbrev_, 0, GetAbbrevComment(debugAbbrev_) + " end of attributes").Endl();
      asmDesc_.PutData1(debugAbbrev_, 0, GetAbbrevComment(debugAbbrev_) + " end of attributes").Endl();
    } // block
  } // if
  { // Now write the part in .debug_info section.
    if (IsAddrTaken() == true) {
      asmDesc_.PutLabel(debugInfo_, GetAddrLabel(asmDesc_)).Endl();
    } // if
    // Offset of die is the current size of debug_info section plus 4, which is the length field.
    SetOffset(debugInfo_.GetSize() + 4);
    ULEB128 lAbbrevIdULEB(lAbbrevId);
    lAbbrevIdULEB.GetDisassembly(debugInfo_, asmDesc_, GetName());
    for (hFUInt16 c(0); c < mAttributes.size(); ++ c) {
      mAttributes[c]->GetValue()->GetDisassembly(debugInfo_, asmDesc_, mAttributes[c]->GetName());
    } // for
  } // block
  for (hFUInt16 c(0); c < mChildren.size(); ++ c) {
    mChildren[c]->emitDIE(debugInfo_, debugAbbrev_, asmDesc_);
  } // for
  if (!mChildren.empty()) {
    asmDesc_.PutData1(debugInfo_, 0, GetAbbrevComment(debugInfo_) + " end of children").Endl();
  } // if
  EMDEBUG0();
  return;
} // DW_DIE::emitDIE

void DebugInfoDwarf2::
OnFinish(ostream& asmFile_, CGContext& cgContext_) {
  BMDEBUG1("DebugInfoDwarf2::OnFinish");
  // Create the sections' data
  ASSERTMSG(mCompileUnits.size() == 1, ieStrNotImplemented);
  // First emit the compilation unit data into a logical section to calculate
  // its exact size.
  ASMSection lASMFile(&asmFile_);
  ASMSection lDebugInfo;
  ASMSection lDebugLoc;
  ASMSection lDebugLine;
  ASMSection lDebugAbbrev;
  ASMDesc& lASMDesc(*cgContext_.GetASMDesc());
  { // First process the location expressions of objects.
    for (hFUInt32 c(0); c < mObjects.size(); ++ c) {
      if (pred.pHasDebugInfo(mObjects[c]) == true) {
        DwarfObjDIE* lDIE(extr.eGetDwarfDIE(mObjects[c]));
        emitLocationAttributes(lDebugLoc, lASMDesc, lDIE->GetTag(), lDIE->GetLifeRanges(), eDW_AT_location);
      } // if
    } // for
  } // block
  { // Set the framebase of functions.
    for (hFUInt32 c(0); c < mFuncs.size(); ++ c) {
      IRFunction* lFunc(mFuncs[c]);
      DwarfFuncDIE* lFuncDIE(extr.eGetDwarfDIE(lFunc));
      const pair<AddrDiff*, vector<DW_LOC_OP*> >& lFrameBase(lFuncDIE->GetFrameBase());
      if (!lFrameBase.second.empty()) {
        vector<pair<AddrDiff*, vector<DW_LOC_OP*> > > lLocList;
        lLocList.push_back(lFrameBase);
        emitLocationAttributes(lDebugLoc, lASMDesc, lFuncDIE->GetTag(), lLocList, eDW_AT_frame_base);
      } // if
    } // for
  } // block
  string lAbbrevBegLabel(lASMDesc.GetLocalLabel("abbr", 0));
  lASMDesc.PutData2(lDebugInfo, 2, GetAbbrevComment(lDebugInfo) + " Dwarf version").Endl(); // Dwarf version
  lASMDesc.PutRelocAddr(lDebugInfo, lAbbrevBegLabel,
    GetAbbrevComment(lDebugInfo) + " offset in debug_abbrev").Endl(); // offset in to the .debug_abbrev section
  lASMDesc.PutData1(lDebugInfo, cgContext_.GetTarget()->GetAddressSize(),
    GetAbbrevComment(lDebugInfo) + " size of pointers").Endl(); // size of pointers in HW
  mCompileUnits[0]->emitDIE(lDebugInfo, lDebugAbbrev, lASMDesc);
  { // put the end of .debug_info zero abbreviation code.
    lASMDesc.PutData1(lDebugAbbrev, 0, GetAbbrevComment(lDebugAbbrev) + " end of .debug_abbrev").Endl();
  } // block
  // Logical section is completed.
  { // Emit .debug_info section
    lASMDesc.PutSection(lASMFile.getAsm(), ASTCustom | ASTData, ".debug_info") << endl;
    lASMDesc.PutData4(lASMFile, lDebugInfo.GetSize()).Endl(); // Emit length of compilation unit-4
    lDebugInfo.getAsm() << ends;
    lASMFile << lDebugInfo.str() << endl;
  } // block
  { // Emit .debug_abbrev section
    lASMDesc.PutSection(lASMFile, ASTCustom|ASTData, ".debug_abbrev") << endl;
    lASMDesc.PutLabel(lASMFile, lAbbrevBegLabel).Endl();
    lDebugAbbrev.getAsm() << ends;
    lASMFile << lDebugAbbrev.str() << endl;
  } // block
  { // Emit the .debug_frame section
    ASMSection lDebugFrame;
    // Dump the CIEs.
    for(hFUInt32 c(0); c < mCIEs.size(); ++ c) {
      mCIEs[c]->Emit(lDebugFrame, lASMDesc);
    } // for
    for(hFUInt32 d(0); d < mFDEs.size(); ++ d) {
      mFDEs[d]->Emit(lDebugFrame, lASMDesc);
    } // for
    lASMDesc.PutSection(lASMFile.getAsm(), ASTCustom | ASTData, ".debug_frame") << endl;
    lDebugFrame.getAsm() << ends;
    lASMFile << lDebugFrame.str() << endl;
  } // block
  { // Emit .debug_loc section
    lASMDesc.PutSection(lASMFile, ASTCustom|ASTData, ".debug_loc") << endl;
    lDebugLoc.getAsm() << ends;
    lASMFile << lDebugLoc.str() << endl;
  } // block
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnFinish

void DebugInfoDwarf2::
OnFuncStart(IRFunction* func_, CGFuncContext& context_) {
  BMDEBUG3("DebugInfoDwarf2::OnFuncStart", func_, &context_);
  REQUIREDMSG(func_ != 0, ieStrParamValuesDBCViolation);
  DF_FDE* lFDE(new DF_FDE(func_, mCurrCIE));
  extr.eGetDwarfDIE(func_)->SetFDE(lFDE);
  mFDEs.push_back(lFDE);
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnFuncStart

void DebugInfoDwarf2::
OnFuncEnd(IRFunction* func_, CGFuncContext& context_) {
  BMDEBUG3("DebugInfoDwarf2::OnFuncEnd", func_, &context_);
  REQUIREDMSG(func_ != 0, ieStrParamValuesDBCViolation);
  { // Collect locals and parameters of function.
    const vector<IROParameter*>& lParams(func_->GetDeclaration()->GetParams());
    for (hFUInt32 c(0); c < lParams.size(); ++ c) {
      mObjects.push_back(lParams[c]);
    } // if
    const vector<IROLocal*>& lLocals(func_->GetLocals());
    for (hFUInt32 d(0); d < lLocals.size(); ++ d) {
      if (!pred.pIsTmp(lLocals[d])) {
        mObjects.push_back(lLocals[d]);
      } // if
    } // if
  } // block
  addHighLowPC(extr.eGetDwarfDIE(func_)->GetTag(),
    IRBuilder::smirbGetAttachedLabel(func_, "beg"),
    IRBuilder::smirbGetAttachedLabel(func_, "end"));
  mFuncs.push_back(func_);
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnFuncEnd

void DebugInfoDwarf2::
OnModuleStart(IRModule* module_) {
  BMDEBUG1("DebugInfoDwarf2::OnModuleStart");
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnModuleStart

void DebugInfoDwarf2::
OnModuleEnd(IRModule* module_) {
  BMDEBUG1("DebugInfoDwarf2::OnModuleEnd");
  DwarfModuleDIE* lDIE(extr.eGetDwarfDIE(module_));
  { // Collect globals of module.
    const vector<IROGlobal*>& lGlobals(module_->GetGlobals());
    for (hFUInt32 c(0); c < lGlobals.size(); ++ c) {
      mObjects.push_back(lGlobals[c]);
    } // if
  } // block
  addHighLowPC(lDIE->GetTag()->GetCompUnit(),
    IRBuilder::smirbGetAttachedLabel(module_, "beg"),
    IRBuilder::smirbGetAttachedLabel(module_, "end"));
  { // Set the line number statement list attribute.
    lDIE->GetTag()->GetCompUnit()->AddAttribute(new DW_AT_stmt_list(
      new DWC_Address(IRBuilder::smirbGetAttachedLabel(0, "stmt_list"))));
  } // block
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnModuleEnd

void DebugInfoDwarf2::
OnBBStart(IRBB* bb_, CGFuncContext& context_) {
  BMDEBUG1("DebugInfoDwarf2::OnBBStart");
  const SrcLoc& lSrcLoc(extr.eGetSrcLoc(bb_));
  if (pred.pIsSet(lSrcLoc) == true) {
    ASMSection o;
    context_.GetASMDesc()->PutDirLoc(o, lSrcLoc, DLNSD_basic_block);
    o.getAsm() << ends;
    context_.AddInst(new AsmSeqDirInfo(o.str()));
  } // if
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnBBStart

void DebugInfoDwarf2::
OnProgStart(IRProgram* prog_, CGContext& context_) {
  BMDEBUG1("DebugInfoDwarf2::OnProgStart");
  const map<string, hFUInt32>& lFileNames(extr.eGetFileNames(Singleton<SrcLoc>::Obj()));
  map<string, hFUInt32>::const_iterator lIt(lFileNames.begin());
  while (lIt != lFileNames.end()) {
    ASMSection o;
    o.getAsm() << "\t.file " << lIt->second << " ";
    util.uEmitString(o.getAsm(), lIt->first) << ends;
    context_.AddInst("ProgBeg", "dir", new AsmSeqDirInfo(o.str()));
    ++ lIt;
  } // while
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnProgStart

void DebugInfoDwarf2::
OnProgEnd(IRProgram* prog_, CGContext& context_) {
  BMDEBUG1("DebugInfoDwarf2::OnProgEnd");
  // LIE
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnProgEnd

void DebugInfoDwarf2::
OnStmtStart(IRStmt* stmt_, CGFuncContext& context_) {
  BMDEBUG2("DebugInfoDwarf2::OnStmtStart", stmt_);
  const SrcLoc& lSrcLoc(extr.eGetSrcLoc(stmt_));
  if (pred.pIsSet(lSrcLoc) == true) {
    PDEBUG("DebugInfo", "detail", "Stmt " << refcxt(stmt_) << " has srcloc: " << progcxt(&lSrcLoc));
    ASMSection o;
    context_.GetASMDesc()->PutDirLoc(o, lSrcLoc, DLNSD_is_stmt, 1);
    o.getAsm() << ends;
    context_.AddInst(new AsmSeqDirInfo(o.str()));
  } // if
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnStmtStart

void DebugInfoDwarf2::
OnStmtEnd(IRStmt* stmt_, CGFuncContext& context_) {
  BMDEBUG2("DebugInfoDwarf2::OnStmtEnd", stmt_);
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnStmtEnd

void DebugInfoDwarf2::
OnBlockScopeStart(DIEBase* block_) {
  BMDEBUG1("DebugInfoDwarf2::OnBlockScopeStart");
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnBlockScopeStart

void DebugInfoDwarf2::
OnBlockScopeEnd(DIEBase* block_) {
  BMDEBUG1("DebugInfoDwarf2::OnBlockScopeEnd");
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::OnBlockScopeEnd

DWC_RefOffset::
DWC_RefOffset(DIEBase* die_) :
  DWC_RefAddr(static_cast<DW_DIE*>(die_))
{
  REQUIREDMSG(dynamic_cast<DW_DIE*>(die_) != 0, ieStrParamValuesDBCViolation);
  ASSERTMSG(dynamic_cast<DW_TAG*>(die_) != 0, ieStrParamValuesDBCViolation);
} // DWC_RefOffset::DWC_RefOffset

DWC_RefAddr::
DWC_RefAddr(DIEBase* die_) :
  mDIE(static_cast<DW_TAG*>(die_)),
  mAddrLabel("")
{
  REQUIREDMSG(dynamic_cast<DW_TAG*>(die_) != 0, ieStrParamValuesDBCViolation);
  mDIE->SetAddrTaken();
} // DWC_RefAddr::DWC_RefAddr

ASMSection& DWC_RefAddr::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) {
  REQUIREDMSG(!mAddrLabel.empty() || mDIE != 0, ieStrParamValuesDBCViolation);
  if (!mDIE) {
    asmDesc_.PutDataString(o, mAddrLabel, GetAbbrevComment(o)).Endl();
  } else {
    asmDesc_.PutDataString(o, mDIE->GetAddrLabel(asmDesc_), GetAbbrevComment(o)).Endl();
  } // if
  return o;
} // DWC_RefAddr::GetDisassembly

ASMSection& DWC_RefOffset::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) {
  REQUIREDMSG(mDIE != 0, ieStrParamValuesDBCViolation);
  string lComment;
  if (!comment_.empty()) {
    lComment = ", " + comment_;
  } // if
  asmDesc_.PutData4(o, mDIE->GetOffset(), GetAbbrevComment(o) + lComment).Endl();
  return o;
} // DWC_RefOffset::GetDisassembly

string
GetAbbrevComment(const ASMSection& asmSection_) {
  string retVal("");
  if (!asmSection_.IsSizeUnknown()) {
    retVal = FORMATSTR(" offset=" << asmSection_.GetSize());
  } else {
    retVal = " offset=<unknown>";
  } // if
  return retVal;
} // GetAbbrevComment

ASMSection& SLEB128::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  string lComment;
  if (!comment_.empty()) {
    lComment = ", " + comment_;
  } // if
  if (asmDesc_.IsLittleEndian()) {
    for (hFUInt8 c(0); c < mBytes.size(); ++ c) {
      asmDesc_.PutData1(o, mBytes[c], FORMATSTR(GetAbbrevComment(o) << lComment << " sleb" << c)).Endl();
    } // for
  } else {
    for (hFSInt8 c(mBytes.size()-1); c >= 0; -- c) {
      asmDesc_.PutData1(o, mBytes[c], FORMATSTR(GetAbbrevComment(o) << lComment << " sleb" << c)).Endl();
    } // for
  } // if
  return o;
} // SLEB128::GetDisassembly

ASMSection& ULEB128::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  string lComment;
  if (!comment_.empty()) {
    lComment = ", " + comment_;
  } // if
  if (asmDesc_.IsLittleEndian()) {
    for (hFUInt8 c(0); c < mBytes.size(); ++ c) {
      asmDesc_.PutData1(o, mBytes[c], FORMATSTR(GetAbbrevComment(o) << lComment << " uleb" << c)).Endl();
    } // for
  } else {
    for (hFSInt8 c(mBytes.size()-1); c >= 0; -- c) {
      asmDesc_.PutData1(o, mBytes[c], FORMATSTR(GetAbbrevComment(o) << lComment << " uleb" << c)).Endl();
    } // for
  } // if
  return o;
} // ULEB128::GetDisassembly

ASMSection& DWC_Data8::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) {
  string lComment;
  if (!comment_.empty()) {
    lComment = ", " + comment_;
  } // if
  if (asmDesc_.IsLittleEndian() == true) {
    asmDesc_.PutData4(o, hFUInt32(mValue >> 32)).Endl();
    asmDesc_.PutData4(o, hFUInt32((mValue << 32) >> 32), GetAbbrevComment(o) + lComment).Endl();
  } else {
    asmDesc_.PutData4(o, hFUInt32((mValue << 32) >> 32)).Endl();
    asmDesc_.PutData4(o, hFUInt32(mValue >> 32), GetAbbrevComment(o) + lComment).Endl();
  } // if
  return o;
} // DWC_Data8::GetDisassembly

ASMSection& DWC_Flag::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) {
  string lComment;
  if (!comment_.empty()) {
    lComment = ", " + comment_;
  } // if
  asmDesc_.PutData1(o, !mFlag ? 0 : 1, GetAbbrevComment(o) + lComment).Endl();
  return o;
} // DWC_Flag::GetDisassembly

DW_DIE::
DW_DIE() :
  mAddrTaken(false)
{
  BMDEBUG1("DW_DIE::DW_DIE");
  EMDEBUG0();
} // DW_DIE::DW_DIE

DW_DIE::
DW_DIE(const DW_DIE& die_)  :
  mAddrTaken(die_.mAddrTaken),
  mOffsetInCompUnit(die_.mOffsetInCompUnit),
  mChildren(die_.mChildren),
  mAddrLabel(die_.mAddrLabel),
  mAttributes(die_.mAttributes)
{
  BMDEBUG1("DW_DIE::DW_DIE");
  EMDEBUG0();
} // DW_DIE::DW_DIE

const string& DW_DIE::
GetAddrLabel(const ASMDesc& asmDesc_) {
  static hFUInt32 slId;
  if (mAddrLabel.empty()) {
    mAddrLabel = asmDesc_.GetLocalLabel("DW", slId ++);
  } // if
  return mAddrLabel;
} // DW_DIE::GetAddrLabel

const string& DW_DIE::
GetName() const {
  static string retVal;
  REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
  return retVal;
} // DW_DIE::GetName

void DebugFrameEntry::
AddInst(CFInst* inst_) {
  REQUIREDMSG(inst_ != 0, ieStrParamValuesDBCViolation);
  mInsts.push_back(inst_);
  mSizeOfInsts += inst_->GetByteSize();
  return;
} // DebugFrameEntry::AddInst

ASMSection& DW_CFA_advance_loc::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " advance_loc").Endl();
  return o;
} // DW_CFA_advance_loc::GetDisassembly

ASMSection& DW_CFA_advance_loc1::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " advance_loc1").Endl();
  asmDesc_.PutData1(o, mOp0, "\tdelta").Endl();
  return o;
} // DW_CFA_advance_loc1::GetDisassembly

ASMSection& DW_CFA_advance_loc2::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " advance_loc2").Endl();
  asmDesc_.PutData2(o, mOp0, "\tdelta").Endl();
  return o;
} // DW_CFA_advance_loc2::GetDisassembly

ASMSection& DW_CFA_offset::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " offset").Endl();
  mOp1.GetDisassembly(o, asmDesc_, "\toffset");
  return o;
} // DW_CFA_offset::GetDisassembly

ASMSection& DW_CFA_offset_extended::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " offset_extended").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\treg");
  mOp1.GetDisassembly(o, asmDesc_, "\toffset");
  return o;
} // DW_CFA_offset_extended::GetDisassembly

ASMSection& DW_CFA_restore::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " restore").Endl();
  return o;
} // DW_CFA_restore::GetDisassembly

ASMSection& DW_CFA_restore_extended::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " restore_extended").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\tregister");
  return o;
} // DW_CFA_restore_extended::GetDisassembly

ASMSection& DW_CFA_undefined::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " undefined").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\treg");
  return o;
} // DW_CFA_undefined::GetDisassembly

ASMSection& DW_CFA_same_value::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " same_value").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\treg");
  return o;
} // DW_CFA_same_value::GetDisassembly

ASMSection& DW_CFA_def_cfa_register::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " def_cfa_register").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\treg");
  return o;
} // DW_CFA_def_cfa_register::GetDisassembly

ASMSection& DW_CFA_register::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " register").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\treg new");
  mOp1.GetDisassembly(o, asmDesc_, "\treg old");
  return o;
} // DW_CFA_register::GetDisassembly

ASMSection& DW_CFA_def_cfa_offset::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + "def_cfa_offset").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\toffset");
  return o;
} // DW_CFA_def_cfa_offset::GetDisassembly

ASMSection& DW_CFA_def_cfa::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " def_cfa").Endl();
  mOp0.GetDisassembly(o, asmDesc_, "\treg");
  mOp1.GetDisassembly(o, asmDesc_, "\toffset");
  return o;
} // DW_CFA_def_cfa::GetDisassembly

ASMSection& DW_CFA_remember_state::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " remember_state").Endl();
  return o;
} // DW_CFA_remember_state::GetDisassembly

ASMSection& DW_CFA_nop::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " nop").Endl();
  return o;
} // DW_CFA_nop::GetDisassembly

ASMSection& DW_CFA_restore_state::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " restore_state").Endl();
  return o;
} // DW_CFA_restore_state::GetDisassembly

ASMSection& DW_CFA_set_loc::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " set_loc").Endl();
  asmDesc_.PutRelocAddr(o, mOp0, "\taddr").Endl();
  return o;
} // DW_CFA_set_loc::GetDisassembly

ASMSection& DW_CFA_advance_loc4::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " advance_loc4").Endl();
  asmDesc_.PutData4(o, mOp0, "\tdelta").Endl();
  return o;
} // DW_CFA_advance_loc4::GetDisassembly

ASMSection& DW_CFA_advance_loc4_diff::
GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_, const string& comment_) const {
  asmDesc_.PutData1(o, GetCode(), comment_ + " advance_loc4").Endl();
  asmDesc_.PutRelocAddrDiff(o, mOp0, "\tdelta").Endl();
  return o;
} // DW_CFA_advance_loc4_diff::GetDisassembly

void DebugInfoDwarf2::
addHighLowPC(DW_TAG* tag_, const AddrLTConst& lowPC_, const AddrLTConst& highPC_) {
  BMDEBUG4("DebugInfoDwarf2::addHighLowPC", tag_, &lowPC_, &highPC_);
  REQUIREDMSG(dynamic_cast<DW_TAG_subprogram*>(tag_) != 0 ||
    dynamic_cast<DW_TAG_compile_unit*>(tag_) != 0, ieStrParamValuesDBCViolation);
  tag_->AddAttribute(new DW_AT_low_pc(new DWC_Address(lowPC_)));
  tag_->AddAttribute(new DW_AT_high_pc(new DWC_Address(highPC_)));
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::addHighLowPC

void DebugInfoDwarf2::
emitLocationAttributes(ASMSection& debugLoc_, ASMDesc& asmDesc_, DW_TAG* tag_,
  const vector<pair<AddrDiff*, vector<DW_LOC_OP*> > >& location_, eDW_AT attribute_) {
  BMDEBUG3("DebugInfoDwarf2::emitLocationAttributes", tag_, attribute_);
  if (!location_.empty()) {
    if (location_[0].first == 0) {
      PDEBUG("DebugInfo", "detail", "Block expression for location");
      hFUInt16 lLengthOfExpr(0);
      {
        for (hFUInt32 d(0); d < location_[0].second.size(); ++ d) {
          lLengthOfExpr += location_[0].second[d]->GetSize();
        } // for
      } // block
      DWC_Block* lBlock(new DWC_BlockN(util.uGetMinNumBytes(lLengthOfExpr)));
      vector<DW_Value*> lValues;
      for (hFUInt32 d(0); d < location_[0].second.size(); ++ d) {
        location_[0].second[d]->GetAsValues(lValues);
      } // for
      lBlock->AddValues(lValues);
      // We must emit it as location expression i.s.o location list.
      if (attribute_ == eDW_AT_location) {
        tag_->AddAttribute(new DW_AT_location(lBlock));
      } else if (attribute_ == eDW_AT_frame_base) {
        tag_->AddAttribute(new DW_AT_frame_base(lBlock));
      } else {
        ASSERTMSG(0, ieStrNotImplemented);
      } // if
    } else {
      PDEBUG("DebugInfo", "detail", "Location list for location");
      // This is a location list specification. Emit the location list in
      // .debug_loc and set the offset in attributes.
      if (attribute_ == eDW_AT_location) {
        tag_->AddAttribute(new DW_AT_location(new DWC_Data4(debugLoc_.GetSize())));
      } else if (attribute_ == eDW_AT_frame_base) {
        tag_->AddAttribute(new DW_AT_frame_base(new DWC_Data4(debugLoc_.GetSize())));
      } else {
        ASSERTMSG(0, ieStrNotImplemented);
      } // if
      for (hFUInt32 c(0); c < location_.size(); ++ c) {
        AddrLTConstDiff* lRange(dynamic_cast<AddrLTConstDiff*>(location_[c].first));
        ASSERTMSG(lRange != 0, ieStrInconsistentInternalStructure);
        asmDesc_.PutRelocAddr(debugLoc_, lRange->GetLowAddr(), "beg").Endl();
        asmDesc_.PutRelocAddr(debugLoc_, lRange->GetHighAddr(), "end").Endl();
        hFUInt16 lLengthOfExpr(0);
        {
          for (hFUInt32 d(0); d < location_[c].second.size(); ++ d) {
            lLengthOfExpr += location_[c].second[d]->GetSize();
          } // for
        } // block
        asmDesc_.PutData2(debugLoc_, lLengthOfExpr, "length").Endl();
        for (hFUInt32 d(0); d < location_[c].second.size(); ++ d) {
          location_[c].second[d]->GetDisassembly(debugLoc_, asmDesc_);
        } // for
      } // for
      asmDesc_.PutAddrConst(debugLoc_, 0, "loclist end").Endl();
      asmDesc_.PutAddrConst(debugLoc_, 0, "loclist end").Endl();
    } // if
  } else {
    PDEBUG("DebugInfo", "detail", "Location list for optimized away location");
    // The variable is optimized away.
    //! \todo M Design: We should actually create a location list, i.s.o block.
    tag_->AddAttribute(new DW_AT_location(new DWC_BlockN(1)));
  } // if
  EMDEBUG0();
  return;
} // DebugInfoDwarf2::emitLocationAttributes

void ULEB128::
GetAsValues(vector<DW_Value*>& values_) {
  for (hFUInt32 c(0); c < mBytes.size(); ++ c) {
    values_.push_back(new DWC_Data1(mBytes[c]));
  } // for
} // ULEB128::GetAsValues

void SLEB128::
GetAsValues(vector<DW_Value*>& values_) {
  for (hFUInt32 c(0); c < mBytes.size(); ++ c) {
    values_.push_back(new DWC_Data1(mBytes[c]));
  } // for
} // SLEB128::GetAsValues


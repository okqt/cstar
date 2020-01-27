// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __TARGET_HPP__
#define __TARGET_HPP__

#ifndef __HOSTTARGETTYPES_HPP__
#include "hosttargettypes.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif

//! \brief Object that represents a logical assembly section.
//!
//! All the assembly code is written to some section. Sections
//! keep track of their sizes in actualy bytes in object file.
//!
//! \note Some assembly constructs may make it impossible to
//! keep track of the size. For example, a pseudo instruction whose
//! size may be 4 or 8 bytes can arise such cases. Pseudo
//! instructions are frequently emitted by compilers to left the
//! dirty job of handling details by assembler. This however
//! actually may result in much dirtier assembly output due to
//! unknown section sizes.
class ASMSection {
public:
  ASMSection() :
    mSize(0),
    mSizeUnknown(false),
    mAttachedStream(0),
    mLastEndLine(0)
  {
  } // ASMSection::ASMSection
  ASMSection(ostream* attachedStream_) :
    mSize(0),
    mSizeUnknown(true),
    mAttachedStream(attachedStream_),
    mLastEndLine(0)
  {
  } // ASMSection::ASMSection
  //! \pre IsSizeUnknown must be false.
  hFUInt32 GetSize() const {
    REQUIREDMSG(IsSizeUnknown() == false, ieStrParamValuesDBCViolation);
    return mSize;
  } // ASMSection::GetSize
  operator ostream& () {
    //! \todo M Design: This is quite inefficient, find another solution.
    ostream* retVal(0);
    if (mAttachedStream) {
      retVal = mAttachedStream;
    } else {
      retVal = &mAsmStream;
    } // if
    return *retVal;
  } // ASMSection::operator ostream
  ostream& getAsm() {
    //! \todo M Design: This is quite inefficient, find another solution.
    ostream* retVal(0);
    if (mAttachedStream) {
      retVal = mAttachedStream;
    } else {
      retVal = &mAsmStream;
    } // if
    return *retVal;
  } // ASMSection::getAsm
  void SetSizeUnknown() {mSizeUnknown = true;}
  bool IsSizeUnknown() const {return mSizeUnknown == true;}
  //! \attention You should use Endl to get the column numbers correct for the stream.
  ASMSection& Endl() {
    getAsm() << endl;
    mLastEndLine = getAsm().tellp();
    return *this;
  } // ASMSection::Endl
  string str() {
    ostrstream o;
    o << getAsm().rdbuf();
    return o.str();
  } // ASMSection::str
  ostream& operator << (const string& string_) {
    ostream& retVal(getAsm());
    retVal << string_;
    return retVal;
  } // operator <<
  hFUInt32 GetColNum() {return getAsm().tellp() - mLastEndLine;}
private:
  void increaseSize(hFUInt32 amount_) {mSize += amount_;}
  bool mSizeUnknown;
  ostrstream mAsmStream;
  ostream* mAttachedStream;
  hFUInt32 mSize;
  streampos mLastEndLine; //!< This is used in formatting the output.
  friend class ASMDesc;
}; // class ASMSection

struct ASMSectionToken {
  ASMSectionToken(ASMSection& sec_) : mSec(sec_) {}
  ASMSection& mSec;
}; // class ASMSectionToken

//! \brief Helper for ASMSection end line emission.
//! \note You can use it as in 'o << "test" << Endl(o);'
ASMSectionToken Endl(ASMSection& o);

ostream& operator << (ostream& o, const ASMSectionToken& tok_);

#define ASMSECTION(stream) (ASMSection&)(ASMSection(&stream))

//! \brief Base class for assembly syntax of targets.
//! \note A target may have different assemblers with different syntaxes.
//!       This class is used for common assembly constructs.
class ASMDesc : public IDebuggable {
protected:
  ASMDesc(Invalid<ASMDesc>* inv_) {}
public:
  ASMDesc(const string& name_) :
    mName(name_),
    mDebugModeDump(true),
    mEndian(LBEInvalid)
  {
  } // ASMDesc::ASMDesc
  virtual ~ASMDesc() {}
  string GetName() const {return mName;}
  //! \brief Must place the comment starting string.
  virtual ASMSection& PutCommentStr(ASMSection& o) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutCommentStr");
    return o;
  } // ASMDesc::PutCommentStr
  //! \brief Must place the function prolog directives.
  //!
  //! These may include, text/code section, function name exporting,
  //! function start alignment requirement directives, function name label, etc.
  virtual ASMSection& PutFuncProlog(ASMSection& o, const IRFunction* func_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutFuncProlog");
    return o;
  } // ASMDesc::PutFuncProlog
  virtual ASMSection& PutFuncEpilog(ASMSection& o, const IRFunction* func_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutFuncEpilog");
    return o;
  } // ASMDesc::PutFuncEpilog
  virtual ostream& PutSection(ostream& o, AsmSectionType sectionType_, const string& customName_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutSection");
    return o;
  } // ASMDesc::PutSection
  //! \brief Must emit the global object.
  virtual ASMSection& PutGlobal(ASMSection& o, const IROGlobal* globalObj_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutGlobal");
    return o;
  } // ASMDesc::PutGlobal
  virtual ostream& PutDirFileName(ostream& o, const string& inputFileName_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutDirFileName");
    return o;
  } // ASMDesc::PutDirFileName
  //! \brief Must place the version, date, time, and the compiler name.
  virtual ostream& PutIdentificationString(ostream& o) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutIdentificationString");
    return o;
  } // ASMDesc::PutIdentificationString
  virtual ASMSection& PutAlign(ASMSection& o, const tBigInt& alignInBytes_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutAlign");
    return o;
  } // ASMDesc::PutAlign
  //! \brief Should place a data null-terminated string item.
  virtual ASMSection& PutDataString(ASMSection& o, const string& string_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutDataString");
    return o;
  } // ASMDesc::PutDataString
  virtual ASMSection& PutLabel(ASMSection& o, const string& label_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutLabel");
    return o;
  } // ASMDesc::PutLabel
  //! \brief Must place the relocation expression as data.
  //! The length of data in bytes is the size of type of \p relocExpr_.
  //! \todo H Design: the name PutReloc is misleading we need a name
  //! PutData(Reloc)? or something similar.
  virtual ASMSection& PutReloc(ASMSection& o, const IRRelocExpr* relocExpr_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutReloc");
    return o;
  } // ASMDesc::PutReloc
  virtual ASMSection& PutAddrConst(ASMSection& o, const tBigAddr& addr_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutAddrConst");
    return o;
  } // ASMDesc::PutAddrConst
  virtual ASMSection& PutRelocAddr(ASMSection& o, const string& address_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutRelocAddr");
    return o;
  } // ASMDesc::PutRelocAddr
  virtual ASMSection& PutRelocAddrDiff(ASMSection& o, const string& addressHigh_,
    const string& addressLow_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutRelocAddrDiff");
    return o;
  } // ASMDesc::PutRelocAddrDiff
  virtual ASMSection& PutRelocAddrDiff(ASMSection& o, const AddrLTConstDiff& addrDiff_, const string& comment_ = string()) const;
  virtual ASMSection& PutData1(ASMSection& o, hFUInt8 data_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutData1");
    return o;
  } // ASMDesc::PutData1
  virtual ASMSection& PutData2(ASMSection& o, hFUInt16 data_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutData2");
    return o;
  } // ASMDesc::PutData2
  virtual ASMSection& PutData4(ASMSection& o, hFUInt32 data_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutData4");
    return o;
  } // ASMDesc::PutData4
  virtual ASMSection& PutSpace(ASMSection& o, const tBigInt& sizeInBytes_, const string& comment_ = string()) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutSpace");
    return o;
  } // ASMDesc::PutSpace
  //! \brief Must return a local label corresponding to the given id.
  virtual string GetLocalLabel(const string& prefix_, hFUInt32 labelId_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::GetLocalLabel");
    return "invalid label";
  } // ASMDesc::GetLocalLabel
  virtual void PutInstRegSet(ostream& o, const RegSet& regSet_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutRegSet");
    return;
  } // ASMDesc::PutInstRegSet
  virtual void PutInstCondCode(ostream& o, CNTType condType_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutRegSet");
    return;
  } // ASMDesc::PutInstCondCode
  virtual void PutInstShiftCode(ostream& o, ShiftType shiftType_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutRegSet");
    return;
  } // ASMDesc::PutInstShiftCode
  // Debug Information Generation directives.
  //! \brief Must emit a directive that next instruction has the specified properties.
  virtual ASMSection& PutDirLoc(ASMSection& o, hFUInt32 fileNo_, hFUInt32 lineNo_, hFUInt32 colNo_,
    DWARF_LNSDir lnsDir_, hFUInt32 value_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutDirLoc");
    return o;
  } // ASMDesc::PutDirLoc
  virtual ASMSection& PutDirLoc(ASMSection& o, hFUInt32 fileNo_, hFUInt32 lineNo_, hFUInt32 colNo_, DWARF_LNSDir lnsDir_) const;
  virtual ASMSection& PutDirLoc(ASMSection& o, const SrcLoc& srcLoc_, DWARF_LNSDir lnsDir_, hFUInt32 value_) const;
  virtual ASMSection& PutDirLoc(ASMSection& o, const SrcLoc& srcLoc_, DWARF_LNSDir lnsDir_) const;
  //! \brief Must emit a directive that given file number is associated with given file name.
  virtual ASMSection& PutDirFileLoc(ASMSection& o, hFUInt32 fileNo_, const string& fileName_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name() << "::PutFileLoc");
    return o;
  } // ASMDesc::PutDirFileLoc
  void SetDebugModeDump() {mDebugModeDump = true;}
  bool IsDebugModeDump() const {return mDebugModeDump;}
  void SetReleaseModeDump() {mDebugModeDump = false;}
  bool IsReleaseModeDump() const {return !mDebugModeDump;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  bool IsLittleEndian() const {return mEndian == LBELittleEndian;}
  bool IsBigEndian() const {return mEndian == LBEBigEndian;}
  void SetEndianness(LittleBigEndian lbe_) {mEndian = lbe_;}
protected:
  void IncreaseSectionSize(ASMSection& section_, hFUInt32 amount_) const {
    section_.increaseSize(amount_);
  } // IncreaseSectionSize
private:
  string mName; //!< Descriptive name for this assembly syntax.
  bool mDebugModeDump; //!< Should be set to true if debug mode is set.
  LittleBigEndian mEndian;
}; // class ASMDesc

//! \file bcf_target.hpp
//! \brief "target.hpp" contains the base class of all targets with common functionality and
//! standard targets supported by framework.
//! Compiler framework supports all targets at the same time with command line options.
//! However, it can only produce executable for one target per compilation, i.e., same
//! source file can be compiled twice with two different targets.
//! \todo M Design: How will we do the bytes to bits conversion, assuming 1 byte == 8bits is not safe?
class Target : public IDebuggable {
public:
  Target(const string& name_, hFInt8 byteSize_) :
    mSinglePrecision(0),
    mDoublePrecision(0),
    mName(name_),
    mByteSize(byteSize_)
  {
    BMDEBUG1("Target::Target");
    EMDEBUG0();
  } // Target::Target
  virtual ~Target() {
    BMDEBUG1("Target::~Target");
    EMDEBUG0();
  } // Target::~Target
  //! \brief This function shall return the specified integer type
  //!  that is available in the target.
  //! \param numberOfBits minimum number of bits that returned type should contain.
  //! \param alignment maximum alignment in bits that returned type should have. '0' means any alignment is okay.
  //! \param sign it shall return the type that has the specified signedness. IRSDoNotCare means any signedness.
  //! \return It shall return a pointer to target type or 0.
  //! \retval 0 The specified type is not present in the target.
  //! \note It shall return the minimum sized type.
  //! \todo L Design: Should we add an option if we can use an emulated type for the HW by using libraries?
  //! \pre \p sign must be valid.
  //! \todo M Design: I do not like the return type of this function and the whole mechanism. Before any anatrop
  //! is written think on it.
  virtual tInt* GetIntType(hUInt16 numberOfBits_, hUInt16 alignment_, IRSignedness sign_);
  //! \brief This function shall return the specified floating point type
  //! that is available in the target.
  //! \todo M Design: Do we really need to specify alignment? Handle alignment argument.
  //! \param mantissa minimum number of significant bits that returned type should contain.
  //! \param exponent minimum number of exponent bits the returned type should contain.
  //! \param alignment maximum alignment in bits of the returned type. You may provide
  //! 0 (zero) if alignment is not important.
  virtual tReal* GetRealType(hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_);
  //! \brief This function must return single precision real type of target.
  //! \pre Single precision floating point type for target must have been set.
  virtual tReal* GetRealSingle() {
    REQUIRED_VALID(mSinglePrecision);
    return mSinglePrecision;
  } // Target::GetRealSingle
  virtual tReal* GetRealDouble() {
    REQUIRED_VALID(mDoublePrecision);
    return mDoublePrecision;
  } // Target::GetRealDouble
  //! \brief Returns the fastest integer type with the minimum number of bits specified and the signedness.
  //! \param numberOfBits number of bits the type should have.
  //! \param sign_ sign of the type.
  //! \pre \p sign_ must be valid.
  virtual tInt* GetFastIntType(hUInt16 numberOfBits_, IRSignedness sign_);
  //! \brief It shall return an unsigned integer type, that is
  //!        wide enough for the return value of sizeof a type in bytes.
  virtual tInt Get_size_t() const = 0;
  //! \brief It shall return a signed integer type, that is wide
  //!        enough to hold the value of result of difference of two pointers.
  virtual tInt Get_ptrdiff_t() const = 0;
  virtual void InstallValidTargetTypes() = 0;
  virtual hFInt32 GetMinStackAlignment() = 0;
  virtual hFInt32 GetMinStackSizeAlignment() = 0;
  //! \brief Function should return the number of bits per byte.
  virtual hFInt8 GetByteSize() const {return mByteSize;}
  virtual bool IsValid() const;
  //! \brief Sets the target for compilation.
  //! \return Returns the same parameter \p target.
  //! \pre Can only be called once.
  //! \post Ensures a valid \p target.
  static Target* SetTarget(Target* target_);
  virtual hFUInt8 GetAddressSize() const = 0;
  //! \brief Returns the target for the compilation.
  //! \pre A previous call to SetTarget.
  static Target* GetTarget() {
    REQUIREDMSG(smCompilationTarget != 0, ieStrNonNullParam);
    return smCompilationTarget;
  } // Target::GetTarget
  //! \brief DebugTrace interface.
  //! \note Implementation is in the debugtrace.cpp.
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
//! \todo M Design: Put proper interfaces to make this private:
protected:
  static Target* smCompilationTarget; //!< The target for the compilation.
  vector<tInt> mValidIntTypes; //!< Set of valid integers for the target, it is sorted by PredicateSortHelper .
  vector<tInt> mFastIntTypes; //!< Ordered list of fast integers for the target, index 0 is the fastest (user sorted).
  vector<tInt> mFastUIntTypes; //!< Ordered list of fast unsigned integers (user sorted).
  vector<tInt> mFastSIntTypes; //!< Ordered list of fast signed integers (user sorted).
  vector<tReal> mValidFloatTypes; //!< Set of valid floating point types for the target.
  tReal* mSinglePrecision; //!< Single precision floating point type of target, e.g. float in C.
  tReal* mDoublePrecision; //!< Double precision floating point type of target, e.g. double in C.
  string mName;
  hFInt8 mByteSize; //!< byte size in bits.
}; // class Target

//! \brief Definition of a test target.
class TestTarget : public Target {
public:
  TestTarget() :
    Target("TestTarget", INBITS(8)),
    mSize_t(INBITS(32), IRSUnsigned, INBITS(32)),
    mPtrdiff_t(INBITS(32), IRSSigned, INBITS(32))
  {
    BMDEBUG1("TestTarget::TestTarget");
    EMDEBUG0();
  } // TestTarget::TestTarget
  void InstallValidTargetTypes();
  virtual hFInt32 GetMinStackAlignment() {return 4;}
  //! \note Must be greater or equal to GetMinStackAlignment.
  virtual hFInt32 GetMinStackSizeAlignment() {
    return 8;
  } // TestTarget::GetMinStackSizeAlignment
  virtual tInt Get_size_t() const {return mSize_t;}
  virtual tInt Get_ptrdiff_t() const {return mPtrdiff_t;}
  //! \brief DebugTrace interface.
  //! \note Implementation is in the debugtrace.cpp.
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual hFUInt8 GetAddressSize() const {return 4;}
private:
  tInt mSize_t;
  tInt mPtrdiff_t;
}; // TestTarget

#endif // __TARGET_HPP__


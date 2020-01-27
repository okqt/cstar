// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __BCF_TARGET_HPP__
#include "bcf_target.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif

Target* Target::smCompilationTarget;

Target* Target::
SetTarget(Target* target_) {
  BDEBUG2("Target::SetTarget", target_);
  REQUIRED(smCompilationTarget == 0);
  target_->InstallValidTargetTypes();
  sort(target_->mValidIntTypes.begin(), target_->mValidIntTypes.end(), PredicateSortHelper());
  EDEBUG0();
  ENSURE_VALID(target_);
  return smCompilationTarget = target_;
} // Target::SetCompilationTarget

tInt* Target::
GetIntType(hUInt16 numberOfBits_, hUInt16 alignment_, IRSignedness sign_) {
  BMDEBUG4("Target::GetIntType", numberOfBits_, alignment_, sign_);
  REQUIREDMSG(!mValidIntTypes.empty(), ieStrInconsistentInternalStructure);
  tInt* retVal(0);
  for (hFUInt16 c(0); c < mValidIntTypes.size(); ++ c) {
    tInt& lCurrInt(mValidIntTypes[c]);
    if ((lCurrInt.GetSize() >= numberOfBits_) &&
        ((alignment_ == 0) || (lCurrInt.GetAlignment() <= alignment_))) {
      if ((sign_ == IRSDoNotCare) || (lCurrInt.GetSign() == sign_)) {
        retVal = &lCurrInt;
        break;
      } // if
    } // if
  } // for
  EMDEBUG0();
  ENSURE_VALID(retVal);
  return retVal;
} // Target::GetIntType

tReal* Target::
GetRealType(hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_) {
  BMDEBUG4("Target::GetRealType", mantissa_, exponent_, alignment_);
  tReal* retVal(0);
  for (hFUInt16 c(0); c < mValidFloatTypes.size(); ++ c) {
    if ((mValidFloatTypes[c].GetMantissa() >= mantissa_) &&
        (mValidFloatTypes[c].GetExponent() >= exponent_) &&
        ((alignment_ == 0) || (alignment_ >= mValidFloatTypes[c].GetAlignment()))) {
      retVal = &mValidFloatTypes[c];
      break;
    } // if
  } // for
  EMDEBUG0();
  return retVal;
} // Target::GetRealType

tInt* Target::
GetFastIntType(hUInt16 numberOfBits_, IRSignedness sign_) {
  BMDEBUG3("Target::GetFastIntType", numberOfBits_, sign_);
  REQUIRED(sign_ == IRSSigned || sign_ == IRSUnsigned || sign_ == IRSDoNotCare);
  tInt* retVal(0);
  if (sign_ == IRSSigned) {
    for (hFUInt16 c(0); c < mFastSIntTypes.size(); ++ c) {
      if (mFastSIntTypes[c].GetSize() >= numberOfBits_) {
        retVal = &mFastSIntTypes[c];
        break;
      } // if
    } // for
  } else if (sign_ == IRSUnsigned) {
    for (hFUInt16 c(0); c < mFastUIntTypes.size(); ++ c) {
      if (mFastUIntTypes[c].GetSize() >= numberOfBits_) {
        retVal = &mFastUIntTypes[c];
        break;
      } // if
    } // for
  } else if (sign_ == IRSDoNotCare) {
    for (hFUInt16 c(0); c < mFastIntTypes.size(); ++ c) {
      if (mFastIntTypes[c].GetSize() >= numberOfBits_) {
        retVal = &mFastIntTypes[c];
        break;
      } // if
    } // for
  } // if
  EMDEBUG0();
  ENSURE_VALID(retVal);
  return retVal;
} // Target::GetFastIntType

bool Target::
IsValid() const {
  BMDEBUG1("Target::IsValid");
  REQUIRED(!mValidIntTypes.empty());
  //! \todo check if this is too restrictive?
  REQUIRED(!mValidFloatTypes.empty());
  REQUIRED(!mFastIntTypes.empty());
  REQUIRED(!mFastUIntTypes.empty());
  REQUIRED(!mFastSIntTypes.empty());
  REQUIRED(Get_size_t().GetSign() == IRSUnsigned);
  REQUIRED(Get_ptrdiff_t().GetSign() == IRSSigned);
  //! \todo check that size_t & ptrdiff_t are in valid int types.
  EMDEBUG0();
  return true;
} // Target::IsValid

void TestTarget::
InstallValidTargetTypes() {
  BMDEBUG1("TestTarget::InstallValidTargetTypes");
  // Install valid types
  mValidIntTypes.push_back(tInt(INBITS(8), IRSUnsigned, INBITS(8)));
  mValidIntTypes.push_back(tInt(INBITS(8), IRSSigned, INBITS(8)));
  mValidIntTypes.push_back(tInt(INBITS(16), IRSUnsigned, INBITS(16)));
  mValidIntTypes.push_back(tInt(INBITS(16), IRSSigned, INBITS(16)));
  mValidIntTypes.push_back(tInt(INBITS(32), IRSUnsigned, INBITS(32)));
  mValidIntTypes.push_back(tInt(INBITS(32), IRSSigned, INBITS(32)));
  mValidIntTypes.push_back(tInt(INBITS(64), IRSUnsigned, INBITS(64)));
  mValidIntTypes.push_back(tInt(INBITS(64), IRSSigned, INBITS(64)));
  // Install fast types
  mFastIntTypes.push_back(tInt(INBITS(32), IRSSigned, INBITS(32)));
  mFastUIntTypes.push_back(tInt(INBITS(32), IRSUnsigned, INBITS(32)));
  mFastSIntTypes.push_back(tInt(INBITS(32), IRSSigned, INBITS(32)));
  // Install floating point types, (size, man, exp, align)
  mValidFloatTypes.push_back(tReal(INBITS(32), INBITS(24), INBITS(8), INBITS(32))); // float
  mValidFloatTypes.push_back(tReal(INBITS(64), INBITS(53), INBITS(11), INBITS(64))); // double
  mSinglePrecision = &mValidFloatTypes[0];
  mDoublePrecision = &mValidFloatTypes[1];
  EMDEBUG0();
} // TestTarget::InstallValidTargetTypes

ASMSection& ASMDesc::
PutDirLoc(ASMSection& o, const SrcLoc& srcLoc_, DWARF_LNSDir lnsDir_, hFUInt32 value_) const {
  BMDEBUG4("ASMDesc::PutDirLoc", &srcLoc_, toStr(lnsDir_), value_);
  REQUIREDMSG(pred.pIsSet(srcLoc_) == true, ieStrParamValuesDBCViolation);
  PutDirLoc(o, srcLoc_.GetFileNum(), srcLoc_.GetLineNum(), srcLoc_.GetColNum(), lnsDir_, value_);
  EMDEBUG0();
  return o;
} // ASMDesc::PutDirLoc

ASMSection& ASMDesc::
PutDirLoc(ASMSection& o, const SrcLoc& srcLoc_, DWARF_LNSDir lnsDir_) const {
  BMDEBUG3("ASMDesc::PutDirLoc", &srcLoc_, toStr(lnsDir_));
  REQUIREDMSG(lnsDir_ != DLNSD_is_stmt && lnsDir_ != DLNSD_isa, ieStrParamValuesDBCViolation);
  REQUIREDMSG(pred.pIsSet(srcLoc_) == true, ieStrParamValuesDBCViolation);
  PutDirLoc(o, srcLoc_.GetFileNum(), srcLoc_.GetLineNum(), srcLoc_.GetColNum(), lnsDir_);
  EMDEBUG0();
  return o;
} // ASMDesc::PutDirLoc

ASMSection& ASMDesc::
PutDirLoc(ASMSection& o, hFUInt32 fileNo_, hFUInt32 lineNo_, hFUInt32 colNo_, DWARF_LNSDir lnsDir_) const {
  BMDEBUG5("ASMDesc::PutDirLoc", fileNo_, lineNo_, colNo_, toStr(lnsDir_));
  REQUIREDMSG(fileNo_ >= 0, ieStrParamValuesDBCViolation);
  REQUIREDMSG(lnsDir_ != DLNSD_is_stmt && lnsDir_ != DLNSD_isa, ieStrParamValuesDBCViolation);
  PutDirLoc(o, fileNo_, lineNo_, colNo_, lnsDir_, hFUInt32_ignore);
  EMDEBUG0();
  return o;
} // ASMDesc::PutDirLoc

ASMSection& ASMDesc::
PutRelocAddrDiff(ASMSection& o, const AddrLTConstDiff& addrDiff_, const string& comment_) const {
  BMDEBUG3("ASMDesc::PutRelocAddrDiff", &addrDiff_, comment_);
  ASMSection& retVal(PutRelocAddrDiff(o, addrDiff_.GetHighAddr(), addrDiff_.GetLowAddr(), comment_));
  EMDEBUG0();
  return retVal;
} // ASMDesc::PutRelocAddrDiff

ostream& 
operator << (ostream& o, const ASMSectionToken& tok_) {
  tok_.mSec.Endl();
  return o; 
} // operator <<

ASMSectionToken 
Endl(ASMSection& o) {
  return ASMSectionToken(o);
} // Endl



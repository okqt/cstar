// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __HOSTTARGETTYPES_HPP__
#include "hosttargettypes.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif

hFUInt32 hFUInt32_ignore = 0xcdcdcdcd;
hFSInt32 hFSInt32_ignore = 0xcdcdcdcd;
hFUInt16 hFUInt16_ignore = 0xcdcd;
hFSInt16 hFSInt16_ignore = 0xcdcd;
hFUInt8 hFUInt8_ignore = 0xcd;
hFSInt8 hFSInt8_ignore = 0xcd;

BigInt::
BigInt() :
  mValue(0)
{
  BMDEBUG1("BigInt::BigInt");
  EMDEBUG0();
} // BigInt::BigInt

BigInt::
BigInt(const BigInt& bi_) :
  mValue(bi_.mValue)
{
  BMDEBUG1("BigInt::BigInt");
  EMDEBUG0();
} // BigInt::BigInt

BigReal::
BigReal() :
  mValue(0)
{
  BMDEBUG1("BigReal::BigReal");
  EMDEBUG0();
} // BigReal::BigReal

BigReal::
BigReal(const BigReal& bi_) :
  mValue(bi_.mValue)
{
  BMDEBUG1("BigReal::BigReal");
  EMDEBUG0();
} // BigReal::BigReal

BigReal::
BigReal(double value_) :
  mValue(value_)
{
  BMDEBUG1("BigReal::BigReal");
  EMDEBUG0();
} // BigReal::BigReal

bool tReal::
IsValid() const {
  BMDEBUG1("tReal::IsValid");
  REQUIREDMSG(mSize != 0 && mMantissa != 0 && mExponent != 0, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // tReal::IsValid

bool tInt::
IsValid() const {
  BMDEBUG1("tInt::IsValid");
  REQUIREDMSG(mSign == IRSSigned || mSign == IRSUnsigned, ieStrObjectNotValid);
  REQUIREDMSG(mSize != 0, ieStrObjectNotValid);
  EMDEBUG0();
  return true;
} // tInt::IsValid

tInt::
tInt(hUInt16 size_, IRSignedness sign_, hUInt16 alignment_) :
  mSign(sign_),
  mSize(size_),
  mAlignment(alignment_)
{
  BMDEBUG1("tInt::tInt");
  ENSURE_VALID(this);
  EMDEBUG0();
} // tInt::tInt

tInt::
tInt(const tInt& anInt_) :
  mSign(anInt_.mSign),
  mSize(anInt_.mSize),
  mAlignment(anInt_.mAlignment)
{
  BMDEBUG1("tInt::tInt");
  ENSURE_VALID(this);
  EMDEBUG0();
} // tInt::tInt

tInt& tInt::
operator = (const tInt& anInt_) {
  mSign = anInt_.mSign;
  mSize = anInt_.mSize;
  mAlignment = anInt_.mAlignment;
  ENSURE_VALID(this);
  return *this;
} // tInt::operator =

tReal::
tReal(hUInt16 size_, hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_) :
  mSize(size_),
  mMantissa(mantissa_),
  mExponent(exponent_),
  mAlignment(alignment_)
{
  BMDEBUG1("tReal::tReal");
  ENSURE_VALID(this);
  EMDEBUG0();
} // tReal::tReal

tReal::
tReal(const tReal& aReal_) :
  mSize(aReal_.mSize),
  mMantissa(aReal_.mMantissa),
  mExponent(aReal_.mExponent),
  mAlignment(aReal_.mAlignment)
{
  BMDEBUG1("tReal::tReal");
  ENSURE_VALID(this);
  EMDEBUG0();
} // tReal::tReal

tReal& tReal::
operator = (const tReal& aReal_) {
  mSize = aReal_.mSize;
  mMantissa = aReal_.mMantissa;
  mExponent = aReal_.mExponent;
  mAlignment = aReal_.mAlignment;
  ENSURE_VALID(this);
  return *this;
} // tReal::operator =

// //////////////////////////
// BigInt related friend functions
// //////////////////////////

BigInt operator - (const BigInt& bi_) {return BigInt(-bi_.mValue);}
BigInt operator + (const BigInt& l, const BigInt& r) {return BigInt(l.mValue + r.mValue);}
BigInt operator + (int l, const BigInt& r) {return BigInt(l + r.mValue);}
BigInt operator + (const BigInt& l, int r) {return BigInt(l.mValue + r);}
BigInt operator | (int l, const BigInt& r) {return BigInt(l | r.mValue);}
BigInt operator | (const BigInt& l, const BigInt& r) {return BigInt(l.mValue | r.mValue);}
BigInt operator | (const BigInt& l, int r) {return BigInt(l.mValue | r);}
BigInt operator & (int l, const BigInt& r) {return BigInt(l & r.mValue);}
BigInt operator & (const BigInt& l, const BigInt& r) {return BigInt(l.mValue & r.mValue);}
BigInt operator & (const BigInt& l, int r) {return BigInt(l.mValue & r);}
BigInt operator - (int l, const BigInt& r) {return BigInt(l - r.mValue);}
BigInt operator - (const BigInt& l, int r) {return BigInt(l.mValue - r);}
BigInt operator - (const BigInt& l, const BigInt& r) {return BigInt(l.mValue - r.mValue);}
BigInt operator * (int l, const BigInt& r) {return BigInt(l * r.mValue);}
BigInt operator * (const BigInt& l, int r) {return BigInt(l.mValue * r);}
BigInt operator * (const BigInt& l, const BigInt& r) {return BigInt(l.mValue * r.mValue);}
BigInt operator / (const BigInt& l, const BigInt& r) {return BigInt(l.mValue / r.mValue);}
BigInt operator / (const BigInt& l, int r) {return BigInt(l.mValue / r);}
BigInt operator % (const BigInt& l, const BigInt& r) {return BigInt(l.mValue % r.mValue);}
BigInt operator << (const BigInt& l, const BigInt& r) {return BigInt(l.mValue << r.mValue);}
BigInt operator >> (const BigInt& l, const BigInt& r) {return BigInt(l.mValue >> r.mValue);}
BigInt operator << (const BigInt& l, int r) {return BigInt(l.mValue << r);}
BigInt operator >> (const BigInt& l, int r) {return BigInt(l.mValue >> r);}
int operator % (const BigInt& l, int r) {return l.mValue % r;}
bool operator < (const BigInt& l, const BigInt& r) {return l.mValue < r.mValue;}
bool operator <= (const BigInt& l, const BigInt& r) {return l.mValue <= r.mValue;}
bool operator == (const BigInt& l, const BigInt& r) {return l.mValue == r.mValue;}
bool operator != (const BigInt& l, const BigInt& r) {return l.mValue != r.mValue;}
bool operator > (const BigInt& l, const BigInt& r) {return l.mValue > r.mValue;}
bool operator >= (const BigInt& l, const BigInt& r) {return l.mValue >= r.mValue;}
ostream& operator<<(ostream& o, const BigInt& bi_) {return o << bi_.mValue;}

// //////////////////////////
// BigReal related friend functions
// //////////////////////////
BigReal operator - (const BigReal& br_) {return BigReal(-br_.mValue);}
BigReal operator + (const BigReal& l, const BigReal& r) {return BigReal(l.mValue + r.mValue);}
BigReal operator + (double l, const BigReal& r) {return (l + r.mValue);}
BigReal operator + (const BigReal& l, double r) {return (l.mValue + r);}
BigReal operator - (double l, const BigReal& r) {return BigReal(l - r.mValue);}
BigReal operator - (const BigReal& l, double r) {return BigReal(l.mValue - r);}
BigReal operator - (const BigReal& l, const BigReal& r) {return BigReal(l.mValue - r.mValue);}
BigReal operator * (double l, const BigReal& r) {return BigReal(l * r.mValue);}
BigReal operator * (const BigReal& l, double r) {return BigReal(l.mValue * r);}
BigReal operator * (const BigReal& l, const BigReal& r) {return BigReal(l.mValue * r.mValue);}
BigReal operator / (const BigReal& l, const BigReal& r) {return BigReal(l.mValue / r.mValue);}
BigReal operator / (const BigReal& l, double r) {return BigReal(l.mValue / r);}
bool operator < (const BigReal& l, const BigReal& r) {return l.mValue < r.mValue;}
bool operator <= (const BigReal& l, const BigReal& r) {return l.mValue <= r.mValue;}
bool operator == (const BigReal& l, const BigReal& r) {return l.mValue == r.mValue;}
bool operator != (const BigReal& l, const BigReal& r) {return l.mValue != r.mValue;}
bool operator > (const BigReal& l, const BigReal& r) {return l.mValue > r.mValue;}
bool operator >= (const BigReal& l, const BigReal& r) {return l.mValue >= r.mValue;}
ostream& operator<<(ostream& o, const BigReal& br_) {return o << br_.mValue;}


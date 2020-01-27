// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __HOSTTARGETTYPES_HPP__
#define __HOSTTARGETTYPES_HPP__

#ifndef __ENUMS_HPP__
#include "enums.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __HOSTTYPES_HPP__
#include "hosttypes.hpp"
#endif

//! \file hosttargettypes.hpp
//! \brief This file contains target and host types for int, float, and various others.
//! \todo M Doc: Give examples of when to use host types, when to use taget types and when to use
//! big number types.

// //////////////////////////
// Host types documentation
// //////////////////////////

/*! \defgroup hosttypes Host Types
    \brief Host types are typedefs to host types with exact number of bits and signedness.
    Detailed information about host types can be found in \ref hosttypespage.
    \note If you need fast integers use one of fasthosttypes.
    \sa bignumtypes targettypes fasthosttypes

    \page hosttypespage Detailed information about host types
    Example usage:
    \code
    hUInt8 arr[100000]; // allocate exactly 8 bits of 100.000 elements.
    \endcode
    \note If host does not have exact number of bits then host type with minimum but
    bigger than specified number of bits is used.
*/

// //////////////////////////
// BigNum Types documentation
// //////////////////////////

//! \defgroup bignumtypes Big Number types
//! \brief Detailed information about big number types.
//!
//! \note Big number types are potentially slow. Therefore, avoid big number
//! types.
//! \warning Do not use big number types directly, they should be
//! used as placeholders for big host and big target types.
//! \sa BigInt BigReal tBigInt tBigReal hBigInt hBigReal hosttypes

// //////////////////////////
// Target types documentation
// //////////////////////////

//! \defgroup targettypes Target Types
//! \brief Detailed information about target types.
//!
//! \note Target types should only be used in expressions that involves
//! target values, e.g. in IR expressions. Use host types if the computation
//! is made at compiler.

//! \ingroup targettypes
typedef BigInt tBigInt;
//! \ingroup targettypes
typedef BigInt tBigAddr;
//! \ingroup targettypes
typedef BigReal tBigReal;
//! \ingroup hosttypes
typedef BigInt hBigInt;
//! \ingroup hosttypes
typedef BigInt hBigAddr;
//! \ingroup hosttypes
typedef BigReal hBigReal;

// //////////////////////////
// Big Number Types
// //////////////////////////

//! \ingroup bignumtypes
//! \brief A big integer class that can hold any int value.
//! \note BigInt is always signed.
//! \sa hosttypes, targettypes.
class BigInt : public IDebuggable {
public:
  BigInt();
  BigInt(const BigInt& bi_);
  BigInt(int value_) : mValue(value_) {}
  BigInt(unsigned int value_) : mValue(value_) {}
  BigInt(unsigned long long value_) : mValue(value_) {}
  BigInt(long long value_) : mValue(value_) {}
  BigInt(unsigned long value_) : mValue(value_) {}
  BigInt(long value_) : mValue(value_) {}
  BigInt& operator = (const BigInt& bi_) {mValue = bi_.mValue; return *this;}
  BigInt& operator = (int i) {mValue = i; return *this;}
  bool operator ! () {return mValue == 0;}
  BigInt& operator ++ () {mValue ++; return *this;}
  BigInt& operator -- () {mValue --; return *this;}
  BigInt& operator += (int i) {mValue += i; return *this;}
  BigInt& operator += (const BigInt& bi_){mValue += bi_.mValue; return *this;}
  BigInt& operator -= (int i)  {mValue -= i; return *this;}
  BigInt& operator -= (const BigInt& bi_) {mValue -= bi_.mValue; return *this;}
  BigInt& operator *= (int i)  {mValue *= i; return *this;}
  BigInt& operator *= (const BigInt& bi_) {mValue *= bi_.mValue; return *this;}
  //! \todo L Design: check for zero
  BigInt& operator /= (int i)  {mValue /= i; return *this;}
  //! \todo L Design: check for zero
  BigInt& operator /= (const BigInt& bi_) {mValue /= bi_.mValue; return *this;}
  //! \todo L Design: check for zero
  BigInt& operator %= (int i)  {mValue %= i; return *this;}
  //! \todo L Design: check for zero
  BigInt& operator %= (const BigInt& bi_) {mValue %= bi_.mValue; return *this;}
  BigInt& operator >>= (int i) {mValue >>= i; return *this;}
  BigInt& operator |= (int i) {mValue |= i; return *this;}
  BigInt& operator &= (int i) {mValue &= i; return *this;}
  BigInt& operator |= (const BigInt& bi_) {mValue |= bi_.mValue; return *this;}
  BigInt& operator &= (const BigInt& bi_) {mValue &= bi_.mValue; return *this;}
  // ////////////////////////
  // Friend operators
  // ////////////////////////
  //! \brief Negation operator.
  friend BigInt operator - (const BigInt& bi);
  friend BigInt operator + (const BigInt& l, const BigInt& r);
  friend BigInt operator + (int l, const BigInt& r);
  friend BigInt operator + (const BigInt& l, int r);
  friend BigInt operator | (int l, const BigInt& r);
  friend BigInt operator | (const BigInt& l, int r);
  friend BigInt operator | (const BigInt& l, const BigInt& r);
  friend BigInt operator & (int l, const BigInt& r);
  friend BigInt operator & (const BigInt& l, int r);
  friend BigInt operator & (const BigInt& l, const BigInt& r);
  friend BigInt operator - (int l, const BigInt& r);
  friend BigInt operator - (const BigInt& l, int r);
  friend BigInt operator - (const BigInt& l, const BigInt& r);
  friend BigInt operator * (int l, const BigInt& r);
  friend BigInt operator * (const BigInt& l, int r);
  friend BigInt operator * (const BigInt& l, const BigInt& r);
  friend BigInt operator / (const BigInt& l, const BigInt& r);
  friend BigInt operator / (const BigInt& l, int r);
  friend BigInt operator % (const BigInt& l, const BigInt& r);
  friend int operator % (const BigInt& l, int r);
  friend BigInt operator << (const BigInt& l, int r);
  friend BigInt operator << (const BigInt& l, const BigInt& r);
  friend BigInt operator >> (const BigInt& l, const BigInt& r);
  friend BigInt operator >> (const BigInt& l, int r);
  friend bool operator < (const BigInt& l, const BigInt& r);
  friend bool operator <= (const BigInt& l, const BigInt& r);
  friend bool operator == (const BigInt& l, const BigInt& r);
  friend bool operator != (const BigInt& l, const BigInt& r);
  friend bool operator > (const BigInt& l, const BigInt& r);
  friend bool operator >= (const BigInt& l, const BigInt& r);
  friend ostream& operator<<(ostream&, const BigInt&);
  virtual char* debug(const DTContext& context, ostream& toStr) const;
  string toStr() const {
    ostrstream o;
    o << mValue << ends;
    return o.str();
  } // BigInt::toStr
  // Provide some conversion operators to host types.
  // These conversion operators performs DBC checks for lossless operation.
  hFUInt32 get_hFUInt32() const {
    return mValue;
  } // BigInt::get_hFUInt32
  static BigInt getTop() {
    BigInt retVal;
    retVal.mValue = ~(hFSInt64)(0);
  } // BigInt::getTop
  hFUInt32 getNumOfBits() const {
    hFUInt32 retVal(0);
    BigInt lVal(mValue);
    while (lVal != 0) {
      ++ retVal;
      lVal >>= 1;
    } // while
    return retVal;
  } // BigInt::getNumOfBits
private:
  //! \brief Representation of big value.
  //! \todo H Design: We need to change this representation to support much bigger integers.
  hFSInt64 mValue;
}; // class BigInt

//! \ingroup bignumtypes
class BigReal : public IDebuggable {
public:
  BigReal();
  BigReal(const BigReal& bi_);
  BigReal(double value_);
  BigReal& operator = (const BigReal& bi_) {mValue = bi_.mValue; return *this;}
  BigReal& operator = (double d_) {mValue = d_; return *this;}
  BigReal& operator ++ () {mValue ++; return *this;}
  BigReal& operator -- () {mValue --; return *this;}
  BigReal& operator += (double d_) {mValue += d_; return *this;}
  BigReal& operator += (const BigReal& br_){mValue += br_.mValue; return *this;}
  BigReal& operator -= (double d_)  {mValue -= d_; return *this;}
  BigReal& operator -= (const BigReal& br_) {mValue -= br_.mValue; return *this;}
  BigReal& operator *= (double d_)  {mValue *= d_; return *this;}
  BigReal& operator *= (const BigReal& br_) {mValue *= br_.mValue; return *this;}
  //! \todo L Design: check for zero
  BigReal& operator /= (double d_)  {mValue /= d_; return *this;}
  //! \todo L Design: check for zero
  BigReal& operator /= (const BigReal& br_) {mValue /= br_.mValue; return *this;}
  // ////////////////////////
  // Friend operators
  // ////////////////////////
  //! \brief Negation operator.
  friend BigReal operator - (const BigReal& bi);
  friend BigReal operator + (const BigReal& l, const BigReal& r);
  friend BigReal operator + (double l, const BigReal& r);
  friend BigReal operator + (const BigReal& l, double r);
  friend BigReal operator - (double l, const BigReal& r);
  friend BigReal operator - (const BigReal& l, double r);
  friend BigReal operator - (const BigReal& l, const BigReal& r);
  friend BigReal operator * (double l, const BigReal& r);
  friend BigReal operator * (const BigReal& l, double r);
  friend BigReal operator * (const BigReal& l, const BigReal& r);
  friend BigReal operator / (const BigReal& l, const BigReal& r);
  friend BigReal operator / (const BigReal& l, double r);
  friend bool operator < (const BigReal& l, const BigReal& r);
  friend bool operator <= (const BigReal& l, const BigReal& r);
  friend bool operator == (const BigReal& l, const BigReal& r);
  friend bool operator != (const BigReal& l, const BigReal& r);
  friend bool operator > (const BigReal& l, const BigReal& r);
  friend bool operator >= (const BigReal& l, const BigReal& r);
  friend ostream& operator<<(ostream&, const BigReal&);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  hReal32 get_hReal32() const {return mValue;}
  hReal64 get_hReal64() const {return mValue;}
private:
  //! \brief Representation of big value.
  //! \todo H Design: We need to change this representation to support much bigger real numbers.
  double mValue;
}; // class BigReal

// //////////////////////////
// TARGET TYPES
// //////////////////////////
//! \ingroup targettypes
//! \brief Integer target type.
//! \note If you like to create a target type from C/C++ type use IRBuilder.
//! \sa IRBuilder Target
//! \todo M Design: not sure if members should be target type of host type???
class tInt : public IDebuggable {
public:
  //! \brief Creates a target type with specified number of bits, signedness and alignment.
  //! \param size_ Number of bits the type has, including the sign bit.
  //! \param sign_ Must be one of IRSSigned, or IRSUnsigned.
  //! \param alignment_ Minimum alignment of type in number of bits. Can be zero if it can be aligned
  //! to any address.
  tInt(hUInt16 size_, IRSignedness sign_, hUInt16 alignment_);
  tInt(const tInt& anInt_);
  tInt& operator = (const tInt& anInt_);
  IRSignedness GetSign() const {return mSign;}
  //! \brief Returns the size in bits.
  hUInt16 GetSize() const {return mSize;}
  //! \brief Returns the alignment in bits.
  hUInt16 GetAlignment() const {return mAlignment;}
  virtual bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  IRSignedness mSign;
  hUInt16 mSize;
  hUInt16 mAlignment;
}; // class tInt

//! \ingroup targettypes
//! \brief Floating point target type.
//! \par Which types are present in the targets?
//! IEEE floating point standard has 4 floating point types.
//! Single - 32 bits,
//! Double - 64 bits,
//! Extended - 80 bits,
//! Quad - 128 bits.
//! Not all architectures support these types.
//! \todo M Design: not sure if members should be target type or host type???
//! \todo M Design: Should target check for IEEE floating point.
class tReal : public IDebuggable {
public:
  //! \brief Constructs a target floating point type.
  //! \param size_ total size of the type, sizeof type.
  //! \param mantissa_ mantissa or significant part of the floating point number in bits.
  //! \param exponent_ number of bits for exponent.
  //! \param alignment_ minimum alignment of type in bits.
  tReal(hUInt16 size_, hUInt16 mantissa_, hUInt16 exponent_, hUInt16 alignment_);
  tReal(const tReal& real_);
  tReal& operator = (const tReal& real_);
  //! \brief Returns the size in bits.
  hUInt16 GetSize() const {return mSize;}
  //! \brief return the number of mantissa bits.
  hUInt16 GetMantissa() const {return mMantissa;}
  //! \brief return the number of exponent bits.
  hUInt16 GetExponent() const {return mExponent;}
  //! \brief Returns the alignment in bits.
  hUInt16 GetAlignment() const {return mAlignment;}
  bool IsValid() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  hUInt16 mSize; //!< number of total bits this type have, sizeof size in bits.
  hUInt16 mMantissa; //!< number of mantissa bits this type have.
  hUInt16 mExponent; //!< number of exponent bits this type have.
  //! \brief minimum alignment this type should have, can be zero
  //! if it can be aligned to any address.
  hUInt16 mAlignment;
}; // class tReal

#endif


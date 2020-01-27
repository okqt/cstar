// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#ifndef __HOSTTYPES_HPP__
#include "hosttypes.hpp"
#endif

#include <set>
using namespace std;

//! \file utils.hpp Definitions related to defensive programming,
//! design by contract, design patterns and others are in this file.

//! \attention In order to save space call a function rather than typing the code for the macros.

//! \todo H Design: make a double check on how DBC macros are implemented.

// -----------------------
// Constants
// -----------------------

//! \brief Message text to be used when a function which is called is not supposed to be called.
//!
//! Is either default implementation of a base class or not yet implemented function.
const char* const ieStrFuncShouldNotBeCalled = "Internal Error: Function should not be called: ";
//! \brief The message is displayed in functions that would be pure virtuals otherwise.
const char* const ieStrFuncShouldBeOverridden = "Internal Error: Function should be overriden in the derived class: ";
//! \brief The message test to be used when the program counter reaches to an unexpected location.
//!
//! \todo M Design: provide more detailed examples.
//! Situation frequently occurs in enums ...
const char* const ieStrPCShouldNotReachHere = "Internal Error: PC should not reach here: ";
//! \brief The message text to be use for unimplemented code.
const char* const ieStrNotImplemented = "Internal Error: PC reached to an unimplemented code: ";
//! \brief The message is displayed when the incoming parameter values are not
//!        conforming to the DBC requirements of the function.
const char* const ieStrParamValuesDBCViolation = "DBC: Param values do not "
  "conform to specified DBC requirements of the function: ";
//! \brief The message text to be use when class invariance is wrong.
//!
//! This message can be used in constructors required valid(this) case.
const char* const ieStrClassInvariance = "Internal Error: Class invariance is violated: ";
//! \brief The message text to be used when an object is not valid
//! \todo M Design: not sure if we need this.
const char* const ieStrObjectNotValid = "Internal Error: Object is not valid: ";
const char* const ieStrCopyCtorNeedsValid = "Internal Error: The copy constructor needs a valid incoming object: ";
const char* const ieStrAssignNeedsValid = "Internal Error: The assignment operator needs a valid incoming object: ";
//! \brief Indicates a problem in cross checking the values of internal
//!        variables for consistency, e.g. pred and extr results.
//! It can be used for all kinds of in-function expectations (especially in ASSERT), DBC.
const char* const ieStrInconsistentInternalStructure = "Internal Error: Data structures are inconsistent: ";
//! \brief To be used in REQUIRED for non-null parameter values.
const char* const ieStrNonNullParam = "Internal Error: Non-null parameter value required: ";
//! \brief An expression may only be used once in statements, you must do a
//!        recursive copy to avoid the case.
const char* const ieStrExprUsedBefore = "Internal Error: An expression may only be used once in statements: ";
//! \brief In the function non-null return or other side-effects must be ensured.
const char* const ieStrMustEnsureNonNull = "Internal Error: Non-null value must be ensured: ";
//! \brief In the function valid value must be ensured.
const char* const ieStrMustEnsureValid = "Internal Error: Valid value must be ensured: ";
//! \brief Try to avoid using this, use more specific messages.
const char* const ieStrDBCViolation = "Internal Error: DBC violation: ";
//! \brief Should be used in ENSURE that violates the DBC.
const char* const ieStrDBCEnsureViolation = "Internal Error: DBC Ensure violation: ";

// //////////////////////////
// Unit Macros
// //////////////////////////
//! \brief When a value is in bits wrap it with this macro.
//! Use it to emphasize the units of a value. For example, tInt(8, IRSSigned, 8) ->
//! tInt(INBITS(8), IRSSigned, INBITS(8)). You may use it for variables that are not
//! clearly in bits or bytes. Though, you are expected to use xxxInBits notation for
//! identifiers.
//! \sa INBYTES
#define INBITS(x) x
//! \brief When a value is in bytes wrap it with this macro.
//! \sa INBITS
#define INBYTES(x) x

// //////////////////////////
// Design by contract macros
// //////////////////////////
typedef enum {
  DBCTRequired,
  DBCTAssert,
  DBCTEnsure,
  DBCTValid
} DBCType;

class Consts {
public:
  Consts() {}
  static const bool cAddrTaken = true;
  static const bool cAddrNotTaken = false;
  //! \brief Constant when code should be generated for a given IR.
  static const bool cNeedNotMatch = false;
  //! \brief Constant when code need not be generated for a given IR.
  static const bool cMustMatch = true;
  static const bool cStackDownwards = true;
  static const bool cStackUpwards = false;
  static const hReal32 cInfiniteCost = FLT_MAX;
  static const bool cConvRetVal = true;
  static const bool cDoNotConvRetVal = false;
  static const bool cIgnoreDefsAndUses = true;
  static const bool cDoNotIgnoreDefsAndUses = false;
  static const bool cDFAIRLevel = true;
  static const bool cDFAInstLevel = false;
  //! \brief To be used when the value does not matter for a boolean.
  //!
  //! You may use it when passing arguments to bool parametered functions.
  //! \note Do not make any assumptions about the value of this boolean.
  static const bool cBoolDoNotCare = false;
  // These should be used to signify if a recursion is top level call or not.
  static const bool cRecNotFirstCall = false;
  static const bool cRecFirstCall = true;
  static const bool cDFAInitBottom = false;
  static const bool cDFAInitTop = true;
  static const bool cHasNoEllipses = false;
  static const bool cHasEllipses = true;
  static const bool cIRBNoOptims = false; //!< IRBuilder no optims, constant folding etc.
  static const bool cIRBDoOptims = true;
  static const bool cExecuteTriggers = true;
  static const bool cDoNotExecuteTriggers = false;
  // is killed is frequently used so, provide named constants for them.
  static const bool cIsKilledNo = false;
  static const bool cIsKilledYes = true;
  // Options class related
  static const bool cDefaultIsFalse = false; //!< To be used in Options::addBoolOption
  static const bool cDefaultIsTrue = true; //!< To be used in Options::addBoolOption
  static const bool cISATemplateIsOR = true;
  static const bool cISATemplateIsNotOR = false;
  static const bool cDefOfReg = true;
  static const bool cUseOfReg = false;
}; // class Consts

extern Consts consts;
extern Statistics* stats;

const char* toStr(DBCType dbcType_);
bool dtAssert(bool exprResult, const string& fileName, hFSInt32 lineNum, const string& exprStr, const string& msg);
bool DBC_VALIDFunc(DBCType dbcType_, const IDebuggable* obj_, const string& objStr_, const char* file_, hFInt32 line_, const string& msg_);
bool DBCFunc(DBCType dbcType_, bool expr_, const string& exprStr_, const char* file_, hFInt32 line_, const string& msg_);
//! \brief this is the object to be used in debugging DebugTrace, DTConfig, and DTContext classes.
extern ofstream dtLog;
extern bool dtLogEnabled;
extern bool dtEnabled;
extern bool dtAssertMsgEnabled;

#define FORMATSTR(msg) static_cast<const ostringstream&>(ostringstream().flush() << msg).str() 

/*! \brief This macro converts a stream in to a string.
  \note it is quite useful in providing formatted string as argument:
  \code
    string r = DTFMTTOSTR__("an example " << 12 << " also some other values can be inserted")
    // much better use case is using it as a temporary in passing arguments:
    callthis(DTFMTTOSTR__("show: " << 123));
    void callthis(const string& str); // callthis must have const reference type.
  \endcode
  \attention Use it only in this file in the macros.
*/
#define DTFMTTOSTR__(msg) (dtAssertMsgEnabled == true ? ( \
  static_cast<const ostringstream&>(ostringstream().flush() << msg).str()) : "")

//! \brief Assert macro that should be used within Debug/Trace utilities (DTConfig, DTContext, DebugTrace).
#if RELEASEBUILD
#define DTASSERT(expr_, msg_)
#define DTLOG(expr_)
#else
#define DTASSERT(expr_, msg_) {bool lRes__ = expr_; dtAssert(lRes__, __FILE__, __LINE__, #expr_, lRes__ ? "" : DTFMTTOSTR__(msg_));}
#define DTLOG(expr_) if (dtLogEnabled) { dtLog << expr_;}
#endif

//! \defgroup designbycontract Design by Contract
//! \brief Detailed information about 'design by contract'.
//!
//! \warning Be extra careful when you are changing ENSURE, REQUIRED
//! usage in functions. You may need to consider the overall design.
//! \note DBC macros should not be used in expression context.

//! \ingroup designbycontract
//! \brief ENSURE is used as post-condition at the end of functions.
//!
//! ENSURE must use variables that may be changed in the function.
//! If the argument \a expr is not true it will abort or exit the application.
//! \param expr Expression that needs to be \a true.
//! \warning Do not use these in the expression \a expr: assignment,
//! pre/post increment/decrement
//! operators, as well as statements with side effects, i.e., function calls.
//! \attention In the documentation please use "\post ..." in plain
//! English.
//! \attention Please use "\post" for the documentation of ENSURE use.
#if RELEASEBUILD
#define ENSURE(expr_)
#define ENSUREMSG(expr_, msg_)
#else
#define ENSURE(expr_) if (DEBUGBUILD) {DBCFunc(DBCTEnsure, expr_, #expr_, __FILE__, __LINE__, "");}
#define ENSUREMSG(expr_, msg_) if (DEBUGBUILD) {bool lRes__ = expr_; DBCFunc(DBCTEnsure, lRes__, #expr_, __FILE__, __LINE__, lRes__ ? "" : DTFMTTOSTR__(msg_));}
#endif

//! \ingroup designbycontract
//! \brief REQUIRED is used as pre-condition at the beginning of functions.
//!
//! \note Typically expression involves function parameters,
//! member and global variables, i.e. all variables except
//! from auto locals.
//! \param expr Expression that needs to be \a true.
//! \warning Do not use these in the expression \a expr: assignment,
//! pre/post increment/decrement
//! operators, as well as statements with side effects, i.e., function calls.
//! \attention In the documentation please use "\pre ..." in plain
//! English.
//! \attention Please use "\pre" for the documentation of REQUIRED use.
#if RELEASEBUILD
#define REQUIRED(expr_)
#define REQUIREDMSG(expr_, msg_)
#define REQUIREDCALLONCE()
#else
#define REQUIRED(expr_) if (DEBUGBUILD) {DBCFunc(DBCTRequired, expr_, #expr_, __FILE__, __LINE__, "");}
//! \note lRes__ is used to prevent multiple calls of expr.
#define REQUIREDMSG(expr_, msg_) if (DEBUGBUILD) {bool lRes__= expr_; DBCFunc(DBCTRequired, lRes__, #expr_, __FILE__, __LINE__, lRes__ ? "" : DTFMTTOSTR__(msg_));}
#define REQUIREDCALLONCE() {static bool lsFirstCall(true); REQUIREDMSG(lsFirstCall == true, ieStrParamValuesDBCViolation); if (lsFirstCall) {lsFirstCall = false;}}
#endif

//! \todo M Design: use a proper value instead of 1.
#define REQUIREDBLOCK() if (DEBUGBUILD)
#define PDEBUGBLOCK() if (DEBUGBUILD)
#define ASSERTBLOCK() if (DEBUGBUILD)
#define ENSUREBLOCK() if (DEBUGBUILD)

//! \ingroup designbycontract
//! \brief ISVALID is used before a use of an object that may be in invalid state.
//!
//! \note ISVALID makes use of isValid member function of \a obj.
//! \a obj must implement isValid function.
//! \sa MISSING_NULL
//! \note ISVALID should be used on an object that had MISSING_NULL argument
//! in its constructor.
//! \return It returns a bool true if the \a obj is valid. If isValid is
//! false program aborts.
#define ISVALID(obj_) if (DEBUGBUILG) {DBC_VALIDFunc(DBCTValid, obj_, #obj_, __FILE__, __LINE__, "");}
#define ISVALIDMSG(obj_, msg_) if (DEBUGBUILD) {DBC_VALIDFunc(DBCTValid, obj_, #obj_, __FILE__, __LINE__, DTFMTTOSTR__(msg_));}

//! \ingroup designbycontract
//! \brief ASSERT is used when semantically one of the ENSURE, REQUIRED,
//! ISVALID does not fit in catching problems.
//! \param expr Expression that needs to be \a true.
//! \warning Do not use these in the expression \a expr: assignment,
//! pre/post increment/decrement
//! operators, as well as statements with side effects, i.e., function calls.
#if RELEASEBUILD
#define ASSERT(expr_)
#define ASSERTMSG(expr_, msg_)
#else
#define ASSERT(expr_) if (DEBUGBUILD) {DBCFunc(DBCTAssert, expr_, #expr_, __FILE__, __LINE__, "");}
#define ASSERTMSG(expr_, msg_) if (DEBUGBUILD) {bool lRes__ = expr_; DBCFunc(DBCTAssert, lRes__, #expr_, __FILE__, __LINE__, lRes__ ? "" : DTFMTTOSTR__(msg_));}
#endif

//! todo M Debug: Conditionalize STATRECORD to debug mode.
#define STATRECORDMSG(what_, dbgmsg) if (STATBUILD) {stats->Record(what_); PDEBUG("STAT", "detail", what_ << " " << dbgmsg);}
#define STATRECORD(what_) if (STATBUILD) {stats->Record(what_);}
#define STATRESET(what_) if (STATBUILD) {stats->Reset(what_);}
#define STATBLOCK() if (STATBUILD) 

//! \brief The provided statement is only executed in debug mode.
#define DEBUGMODE(stmt) if (DEBUGBUILD) {stmt;}

//! \ingroup designbycontract
//! \brief ENSURE_VALID is composition of ENSURE and ISVALID macros.
//! \note It is a good practice to place ENSURE_VALID(this) in the
//! constructors, assignment operators, if object should be in valid state.
//! \attention Please use "\post" for the documentation of ENSURE use.
#if RELEASEBUILD
#define ENSURE_VALID(obj_)
#define ENSURE_VALIDMSG(obj_, msg_)
#else
#define ENSURE_VALID(obj_) if (DEBUGBUILD) {DBC_VALIDFunc(DBCTEnsure, obj_, #obj_, __FILE__, __LINE__, "");}
#define ENSURE_VALIDMSG(obj_, msg_) if (DEBUGBUILD) {DBC_VALIDFunc(DBCTEnsure, obj_, #obj_, __FILE__, __LINE__, DTFMTTOSTR__(msg_));}
#endif

//! \ingroup designbycontract
//! \brief ENSURE_VALID is composition of ENSURE and ISVALID macros.
//! \note It is a good practice to place ENSURE_INVALID(this) in the
//! destructors and removal functions, if object should be in invalid state.
//! \attention Please use "\post" for the documentation of ENSURE use.
#define ENSURE_INVALID(obj_) ENSURE(ISVALID(obj_) == false)

//! \ingroup designbycontract
//! \attention Please use "\pre" for the documentation of REQUIRED use.
#define REQUIRED_INVALID(obj_) REQUIRED(ISVALID(obj_) == false)

//! \ingroup designbycontract
//! \brief REQUIRED_VALID is composition of REQUIRED and ISVALID macros.
//! \note It is a good practice to use REQUIRED_VALID for parameter objects.
//! \attention Please use "\pre" for the documentation of REQUIRED use.
#if RELEASEBUILD
#define REQUIRED_VALID(obj_)
#define REQUIRED_VALIDMSG(obj_, msg_)
#else
#define REQUIRED_VALID(obj_) if (DEBUGBUILD) {DBC_VALIDFunc(DBCTRequired, obj_, #obj_, __FILE__, __LINE__, "");}
#define REQUIRED_VALIDMSG(obj_, msg_) if (DEBUGBUILD) {DBC_VALIDFunc(DBCTRequired, obj_, #obj_, __FILE__, __LINE__, DTFMTTOSTR__(msg_));}
#endif

//! \brief Must be used when the implementation of code is not yet complete.
//! \param X you may use HLM, High/Low/Medium as the priority of implementation.
#if RELEASEBUILD
#define NOTIMPLEMENTED(X)
#else
#define NOTIMPLEMENTED(X) if(DEBUGBUILD && dtEnabled && Singleton<DebugTrace>::Obj().inDebugTraceCall() == false) {\
  Singleton<DebugTrace>::Obj().pdebug("WARNING", "", DTFMTTOSTR__("NOTIMPLEMENTED-" << #X << \
     __FILE__ << " : " << __LINE__));}
#endif

//! \brief MISSING_NULL is used in the constructor arguments of parts of a composition.
/*! \code
    void composite() {
      part1* p1 = new part1(MISSING_NULL); // indicates that argument is missing and must be
                                        // provided to ensure valid object state.
      composite c(*p1);
      p1->SetParent(&c); // now make it a valid object.
      // ...
      ISVALID(p1);
    }
    \endcode
*/
//! \warning MISSING_NULL should not be used for any other purpose than the
//! above described one.
#define MISSING_NULL 0

//! \brief A place holder macro for *nothing*. Mainly put here for ctags like
//! utilities to jump in to its defition.
//! \note You may place this macro inside a comment. This is much readable.
//! \note Use it when return value of a function call is not used.
//! \note Use it when an optional syntax is left empty, i.e. \a for statement init, cond, iteration parts.
#define LIE /* left intentionally empty */

//! \brief Singleton template class.
//!
//! \note Do not use this template for objects that are frequently accessed.
//! For example, \a Predicate utility. Since singleton notation reduces
//! readability of code (it is too long). Instead use a global object, provide 
//! documentation and run-time checks to ensure singleton-ness.
template<typename S>
class Singleton {
public:
  static S& Obj() {
    static S obj;
    return obj;
  } // Singleton::Obj
}; // class Singleton

//! \brief All object must be derived from this class, if they are to be debugged and traced.
class IDebuggable {
public:
  IDebuggable() {
    mObjId = mObjIds ++;
  } // IDebuggable::IDebuggable
  virtual ~IDebuggable() {}
  //! \attention Since debugTrace is called within constructors you should
  //! provide the implementation of debugTrace in every base class (it is
  //! not sufficient to have it only in derived classes).
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  hFUInt32 GetObjId() const {return mObjId;}
  virtual bool IsValid() const;
private:
  //! \brief Each object has an integer identifier, this makes the object
  //!        identification easier than "this" ptr.
  //!
  //! Object identifiers restarts for each class.
  //! \todo M Design: implement class based object ids.
  hFUInt32 mObjId;
  static hFUInt32 mObjIds;
  friend class DebugTrace;
  template<class T> friend class Invalid;
  //! \brief The type is used to have a compile time check for T of Invalid<T>.
  //! \note Do not use the typedef name 'mustBeIDebuggable' in another class.
  typedef void* mustBeIDebuggable;
}; // class IDebuggable

class fBitVector {
private:
  typedef hFUInt8 bitContainerType;
public:
  fBitVector(hFSInt32 size_) {
    REQUIREDMSG(size_ >= 0, ieStrParamValuesDBCViolation);
    Resize(size_);
  } // fBitVector::fBitVector
  fBitVector(const fBitVector& bv_) :
    mBits(0),
    mSize(bv_.mSize),
    mElemCount(bv_.mElemCount)
  {
    mBits = new bitContainerType[mElemCount];
    memcpy(mBits, bv_.mBits, mElemCount*scContainerSizeInBits/8);
  } // fBitVector::fBitVector
  fBitVector() :
    mBits(0),
    mSize(0),
    mElemCount(0)
  {
  } // fBitVector::fBitVector
  fBitVector& operator = (const fBitVector& bv_) {
    mBits = 0;
    mSize = bv_.mSize;
    mElemCount = bv_.mElemCount;
    mBits = new bitContainerType[mElemCount];
    memcpy(mBits, bv_.mBits, mElemCount*scContainerSizeInBits/8);
    return *this;
  } // fBitVector::operator =
public:
  bool operator != (const fBitVector& bv_) {
    REQUIREDMSG(mSize == 0 || (mBits != 0 && bv_.mBits != 0), ieStrParamValuesDBCViolation);
    REQUIREDMSG(mSize == bv_.mSize, ieStrParamValuesDBCViolation);
    bool retVal(false);
    for (hFUInt32 c(0); c < mElemCount; ++ c) {
      if (mBits[c] != bv_.mBits[c]) {
        retVal = true;
        break;
      } // if
    } // for
    return retVal;
  } // fBitVector::operator !=
  void operator |= (const fBitVector& bv_) {
    REQUIREDMSG(mSize == 0 || (mBits != 0 && bv_.mBits != 0), ieStrParamValuesDBCViolation);
    REQUIREDMSG(mSize == bv_.mSize, ieStrParamValuesDBCViolation);
    for (hFUInt32 c(0); c < mElemCount; ++ c) {
      mBits[c] |= bv_.mBits[c];
    } // for
  } // fBitVector::operator |=
  void operator &= (const fBitVector& bv_) {
    REQUIREDMSG(mSize == 0 || (mBits != 0 && bv_.mBits != 0), ieStrParamValuesDBCViolation);
    REQUIREDMSG(mSize == bv_.mSize, ieStrParamValuesDBCViolation);
    for (hFUInt32 c(0); c < mElemCount; ++ c) {
      mBits[c] &= bv_.mBits[c];
    } // for
  } // fBitVector::operator &=
  void Resize(hFSInt32 size_) {
    REQUIREDMSG(size_ >= 0, ieStrParamValuesDBCViolation);
    mSize = size_;
    mElemCount = (mSize / scContainerSizeInBits)+1;
    mBits = new hFUInt8[mElemCount];
    return;
  } // fBitVector::Resize
  void SetAllOne() {
    REQUIREDMSG(mSize == 0 || mBits != 0, ieStrParamValuesDBCViolation);
    memset(mBits, 0xff, mElemCount*scContainerSizeInBits/8);
  } // fBitVector::SetAllOne
  void Reset() {
    REQUIREDMSG(mSize == 0 || mBits != 0, ieStrParamValuesDBCViolation);
    memset(mBits, 0, mElemCount*scContainerSizeInBits/8);
    return;
  } // fBitVector::Reset
  hFUInt32 GetSize() const {return mSize;}
  void MakeTop(hFSInt32 elemIndex_) {SetBit(elemIndex_, 1);}
  void MakeBottom(hFSInt32 elemIndex_) {SetBit(elemIndex_, 0);}
  bool IsTop(hFSInt32 elemIndex_) const {return GetBit(elemIndex_) == 1;}
  bool IsBottom(hFSInt32 elemIndex_) const {return GetBit(elemIndex_) == 0;}
  void SetBit(hFSInt32 elemIndex_, hFUInt8 value_) const {
    REQUIREDMSG(elemIndex_ >= 0 && elemIndex_ < mSize, ieStrParamValuesDBCViolation);
    REQUIREDMSG(value_ == 0 || value_ == 1, ieStrParamValuesDBCViolation);
    hFUInt32 lArrIndex(elemIndex_ / scContainerSizeInBits);
    if (value_) {
      mBits[lArrIndex] |= 1 << (elemIndex_ - lArrIndex*scContainerSizeInBits);
    } else {
      mBits[lArrIndex] &= ~(1 << (elemIndex_ - lArrIndex*scContainerSizeInBits));
    } // if
    return;
  } // fBitVector::SetBit
  hFUInt8 GetBit(hFSInt32 elemIndex_) const {
    REQUIREDMSG(elemIndex_ >= 0 && elemIndex_ < mSize, ieStrParamValuesDBCViolation);
    hFUInt32 lArrIndex(elemIndex_ / scContainerSizeInBits);
    hFUInt8 retVal((mBits[lArrIndex] >> (elemIndex_ - lArrIndex*scContainerSizeInBits)) & 0x1);
    return retVal;
  } // fBitVector::GetBit
private:
  static const hFUInt8 scContainerSizeInBits = sizeof(hFUInt8)*8;
private:
  hFUInt32 mSize;
  hFUInt8* mBits;
  hFUInt32 mElemCount;
private:
  friend fBitVector operator | (const fBitVector& leftVector_, const fBitVector& rightVector_);
  friend fBitVector operator & (const fBitVector& leftVector_, const fBitVector& rightVector_);
  friend fBitVector operator - (const fBitVector& leftVector_, const fBitVector& rightVector_);
}; // class fBitVector

//! \brief DFA program point class.
//! \note this part of bitvector is stored in basic blocks.
//! We only need the detailed information in one place, namely
//! top vector of a DFA analysis.
class DFAProgPnt : public IDebuggable {
public:
  DFAProgPnt(const DFAProgPnt& progPnt_) :
    mBitIndex(progPnt_.mBitIndex),
    mBottom(progPnt_.mBottom)
  {
  } // DFAProgPnt::DFAProgPnt
  DFAProgPnt& operator = (const DFAProgPnt& pp_) {
    mBitIndex = pp_.mBitIndex;
    mBottom = pp_.mBottom;
    return *this;
  } // DFAProgPnt::operator =
  DFAProgPnt(hFSInt32 bitIndex_) :
    mBitIndex(bitIndex_),
    mBottom(false)
  {
  } // DFAProgPnt::DFAProgPnt
  //! \brief Return the index of this program point in mTopVector.
  hFSInt32 GetIndex() const {
    REQUIREDMSG(mBitIndex >= 0, ieStrParamValuesDBCViolation);
    return mBitIndex;
  } // DFAProgPnt::GetIndex
  bool IsBottom() const {return mBottom == true;}
  bool IsTop() const {return mBottom == false;}
  void MakeBottom() {mBottom = true;}
  void MakeTop() {mBottom = false;}
  bool operator == (const Instruction* inst_) {
    return isProgPoint(inst_);
  } // DFAProgPnt::operator ==
  bool operator == (const IRStmt* stmt_) {
    return isProgPoint(stmt_);
  } // DFAProgPnt::operator ==
  bool operator != (const DFAProgPnt& dfaProgPnt_) const {
    return IsBottom() ? !dfaProgPnt_.IsBottom() : dfaProgPnt_.IsBottom();
  } // DFAProgPnt::operator !=
  virtual char* debug(const DTContext& context, ostream& toStr) const;
protected:
  virtual bool isProgPoint(const IRStmt* stmt_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return false;
  } // DFAProgPnt::isProgPoint
  virtual bool isProgPoint(const Instruction* inst_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return false;
  } // DFAProgPnt::isProgPoint
private:
  //! \brief Index of this program point in the top vector.
  //! In the literature bit-vectors are used for DFA, although
  //! we do not use bit-vectors we can use bit index name.
  hFSInt32 mBitIndex;
  bool mBottom;
}; // class DFAProgPnt

typedef vector<DFAProgPnt> BitVector;

//! \note If you define a tuple member in a class you may want to
//!       add named getter helper functions for the members of tuple:
//!       typedef tuple3<IRExpr*, IRExpr*, hFUInt8> TTT;
//!       TTT mXYZ;
//!       IRExpr*& getExpr(TTT& t) {return t.mT0;}
//!       IRExpr*& getChild(TTT& t) {return t.mT1;}
//!       hFUInt8& getCount(TTT& t) {return t.mT2;}
//!       You can now use it like:
//!       getExpr(mXYZ) = getChild(mXYZ);
template<class T0, class T1, class T2>
struct tuple3 {
  tuple3(T0 t0_, T1 t1_, T2 t2_) : 
    mT0(t0_), mT1(t1_), mT2(t2_) 
  {}
  T0 mT0; T1 mT1; T2 mT2; 
}; // struct tuple3

//! \sa tuple3
template<class T0, class T1, class T2, class T3>
struct tuple4 {
  tuple4(T0 t0_, T1 t1_, T2 t2_, T3 t3_) : 
    mT0(t0_), mT1(t1_), mT2(t2_), mT3(t3_) 
  {}
  T0 mT0; T1 mT1; T2 mT2; T3 mT3; 
}; // struct tuple4

//! \sa tuple3
template<class T0, class T1, class T2, class T3, class T4>
struct tuple5 {
  tuple5(T0 t0_, T1 t1_, T2 t2_, T3 t3_, T4 t4_) : 
    mT0(t0_), mT1(t1_), mT2(t2_), mT3(t3_), mT4(t4_) 
  {}
  T0 mT0; T1 mT1; T2 mT2; T3 mT3; T4 mT4; 
}; // struct tuple5

//! \sa tuple3
template<class T0, class T1, class T2, class T3, class T4, class T5>
struct tuple6 {
  tuple6(T0 t0_, T1 t1_, T2 t2_, T3 t3_, T4 t4_, T5 t5_) : 
    mT0(t0_), mT1(t1_), mT2(t2_), mT3(t3_), mT4(t4_), mT5(t5_) 
  {}
  T0 mT0; T1 mT1; T2 mT2; T3 mT3; T4 mT4; T5 mT5;
}; // struct tuple6

//! \sa tuple3
template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
struct tuple7 {
  tuple7(T0 t0_, T1 t1_, T2 t2_, T3 t3_, T4 t4_, T5 t5_, T6 t6_) : 
    mT0(t0_), mT1(t1_), mT2(t2_), mT3(t3_), mT4(t4_), mT5(t5_), mT6(t6_) 
  {}
  T0 mT0; T1 mT1; T2 mT2; T3 mT3; T4 mT4; T5 mT5; T6 mT6;
}; // struct tuple6

//! \sa tuple3
template<class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct tuple8 {
  tuple8(T0 t0_, T1 t1_, T2 t2_, T3 t3_, T4 t4_, T5 t5_, T6 t6_, T7 t7_) : 
    mT0(t0_), mT1(t1_), mT2(t2_), mT3(t3_), mT4(t4_), mT5(t5_), mT6(t6_), mT7(t7_)
  {}
  T0 mT0; T1 mT1; T2 mT2; T3 mT3; T4 mT4; T5 mT5; T6 mT6; T7 mT7;
}; // struct tuple6

class Utils : public IDebuggable {
public:
  template<class tcTN> tcTN 
  uMax(const tcTN& arg0_, const tcTN& arg1_) {
    return arg0_ > arg1_ ? arg0_ : arg1_;
  } // Utils::uMax
  template<class tcTN> tcTN
  uMin(const tcTN& arg0_, const tcTN& arg1_) {
    return arg0_ < arg1_ ? arg0_ : arg1_;
  } // Utils::uMin
  //! \brief Returns 32 bits wide variable with specified range of bits set to 1.
  //! \pre start_ and end_ must be >= 0 and < 32 and start_ <= end_.
  hUInt32 uGetMask_32(hFSInt8 start_, hFSInt8 end_) const;
  //! \brief Returns 16 bits wide variable with specified range of bits set to 1.
  //! \pre start_ and end_ must be >= 0 and < 16 and start_ <= end_.
  hUInt16 uGetMask_16(hFSInt8 start_, hFSInt8 end_) const;
  //! \brief Returns 8 bits wide variable with specified range of bits set to 1.
  //! \pre start_ and end_ must be >= 0 and < 8 and start_ <= end_.
  hUInt8 uGetMask_8(hFSInt8 start_, hFSInt8 end_) const;
  template<typename T, typename V> bool
  uIsOneOf(const T& var_, const V& val0_, const V& val1_) {
    return var_ == val0_ || var_ == val1_;
  } // Utils::uIsOneOf
  template<typename T, typename V> bool
  uIsOneOf(const T& var_, const V& val0_, const V& val1_, const V& val2_) {
    return uIsOneOf(var_, val0_, val1_) || var_ == val2_;
  } // Utils::uIsOneOf
  template<typename T, typename V> bool
  uIsOneOf(const T& var_, const V& val0_, const V& val1_, const V& val2_, const V& val3_) {
    return uIsOneOf(var_, val0_, val1_, val2_) || var_ == val3_;
  } // Utils::uIsOneOf
  template<typename T, typename V> bool
  uIsOneOf(const T& var_, const V& val0_, const V& val1_, const V& val2_, const V& val3_, const V& val4_) {
    return uIsOneOf(var_, val0_, val1_, val2_, val3_) || var_ == val4_;
  } // Utils::uIsOneOf
  template<typename T, typename V> bool
  uIsOneOf(const T& var_, const V& val0_, const V& val1_, const V& val2_, const V& val3_, const V& val4_, const V& val5_) {
    return uIsOneOf(var_, val0_, val1_, val2_, val3_, val4_) || var_ == val5_;
  } // Utils::uIsOneOf
  template<typename T>
  set<T> intersect(const set<T>& s1, const set<T>& s2) {
    set<T> retVal;
    typename set<T>::const_iterator lIt(s1.begin());
    while (lIt != s1.end()) {
      if (s2.find(*lIt) != s2.end()) {
        retVal.insert(*lIt);
      } // if
      ++ lIt;
    } // while
    return retVal;
  } // Utils::intersect
  template<typename tC> 
  void uSwap(tC& l_, tC& r_) {
    tC tmp(r_);
    r_ = l_;
    l_ = tmp;
  } // Utils::uSwap
  template<typename tcDst, typename tcSrc> tcDst
  uBitConvert(tcSrc src_) {
    union {
      tcDst dst;
      tcSrc src;
    } convert;
    convert.src = src_;
    return convert.dst;
  } // Utils::uBitConvert
  //! \brief Returns true the biggest of provided values is exact multiple of the other.
  //! \pre Parameters may not be null.
  bool uIsMultiples(const tBigInt& val0_, const tBigInt& val1_) const;
  //! \brief Returns the minimum number of bytes a value can be represented.
  //! \todo M Design: Adapt the function to operate with negative values.
  hFUInt8 uGetMinNumBytes(const tBigInt& value_) const;
  hFUInt32 uGetUpAligned(hFUInt32 value_, hFUInt32 alignment_) const;
  BigInt uGetUpAligned(const BigInt& value_, hFUInt32 alignment_) const;
  ostream& uEmitString(ostream& o, const string& string_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  bool uInRange(hFSInt32 value_, hFSInt32 low_, hFSInt32 high_) const;
  //! \brief If power of 2 returns bit position if not -1.
  hFSInt32 uIsPowOf2(const BigInt& value_) const;
private:
  static hFSInt8 smRefCount;
}; // class Utils

extern Utils util;

class SrcLoc : public IDebuggable {
public:
  SrcLoc();
  SrcLoc(const SrcLoc& srcLoc_) :
    mFile(srcLoc_.mFile),
    mLine(srcLoc_.mLine),
    mColumn(srcLoc_.mColumn),
    mFileNum(srcLoc_.mFileNum)
  {
  } // SrcLoc::SrcLoc
  SrcLoc(const string& file_, hFSInt32 line_, hFSInt32 column_);
  hFSInt32 GetFileNum() const {return mFileNum;}
  string GetFileName() const {return mFile;}
  hFSInt32 GetLineNum() const {return mLine;}
  hFSInt32 GetColNum() const {return mColumn;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  string mFile;
  hFSInt32 mLine;
  hFSInt32 mColumn;
  hFSInt32 mFileNum;
  static map<string, hFUInt32> smFileNums;
private:
  friend class Predicate;
  friend class Extract;
}; // class SrcLoc

ostream& operator << (ostream& o, const SrcLoc& srcLoc_);

// ///////////////////////////////
// Debug Trace utility log helpers
// ///////////////////////////////
ostream& dtlEndl(ostream& o);
ostream& dtlIndent(ostream& o);
ostream& dtlUnindent(ostream& o);

//! \todo M Design: Add enable/disable skip functions and child calls configuration variables.
typedef enum {
  DTCVInvalid              = -1,
  DTCVNotSet               = 0,
  DTCVSkipFuncs            = 1 << 0, //!< Skips the functions listed.
  DTCVSkipChildCalls       = 1 << 1, //!< Skips the child function calls of given execution point.
  DTCVDumpProg             = 1 << 2, //!< dumps the program at the execution point.
  DTCVShowOnly             = 1 << 3, //!< Show only the execution points listed, note that this resembles the breakpoints in a debugger.
  DTCVDumpProgAtEnd        = 1 << 4, //!< Dumps the program at the end of execution point.
  DTCVMaxCallDepth         = 1 << 5, //!< Aborts the program if call depth of functions are reach to this number
  DTCVShowThis             = 1 << 6, //!< In the member function dumps prints the value of this pointer.
  DTCVUseObjIds            = 1 << 7, //!< Instead of pointer values displays the object ids.
  DTCVShowFileLoc          = 1 << 8, //!< Dumps file locations in the outputs.
  DTCVShowDBC              = 1 << 9, //!< Dumps the DBC related outputs.
  DTCVStmtExprFmtC         = 1 << 10, //!< Dump statement and expressions in C/C++ format (no detailed stmt&expr information).
  DTCVFuncFmtC             = 1 << 11, //!< Dump func in C/C++ format (no detailed BB and CFG information).
  DTCVDefEPSet             = 1 << 12, //!< Start the definition of an execution point set.
  DTCVDisableLog           = 1 << 13, //!< Disables the debug log to dtLog.
  DTCVDisableMsg           = 1 << 14, //!< Disables the message output in asserts, exe runs faster when true.
  DTCVDisableDT            = 1 << 15, //!< Disables the debug/trace outputs.
  DTCVDumpHWDesc           = 1 << 16, //!< Dumps the HWDescription at execution point
  DTCVDumpToConsole        = 1 << 17, //!< Dumps the DebugTrace output to console instead of file.
  DTCVStmtExprFmtMultiLine = 1 << 18, //!< Dumps statements and expressions in multi line.
  DTCVDumpCFGGraphs        = 1 << 19, //!< Dumps program cfgs as graphs that can be read by post processors.
  DTCVExpandTypes          = 1 << 20, //!< print full types instead of references.
  DTCVDumpStats            = 1 << 21, //!< dumps the statistics at the execution point.
} DTConfigVar;

//! \brief Object that holds the debug trace config.
class DTConfig {
public:
  DTConfig();
  hFUInt16 GetMaxCallDepth() const {return mMaxCallDepth;}
  bool LoadFromFile(const string& fileName_);
  DTConfig& operator -= (DTConfigVar cv_) {
    mConfigVars = (DTConfigVar)(mConfigVars & ~cv_);
    return *this;
  } // DTConfig::operator -=
  DTConfig& operator += (DTConfigVar cv_) {
    mConfigVars = (DTConfigVar)(mConfigVars | cv_);
    return *this;
  } // DTConfig::operator +=
  bool operator != (DTConfigVar cv_) const {
    return !(mConfigVars & cv_);
  } // DTConfig::operator !=
  bool operator == (DTConfigVar cv_) const {
    return mConfigVars & cv_;
  } // DTConfig::operator ==
  set<string>& operator [] (DTConfigVar cv_) {
    DTASSERT(mVarType[cv_] == "strset", "operator[] can only be used for 'set of string' type of vars");
    return mStrSetVars[cv_];
  } // DTConfig::operator []
  bool HasShowOnly() const {
    bool retVal(false);
    map<DTConfigVar, set<string> >::const_iterator lIt(mStrSetVars.find(DTCVShowOnly));
    if (lIt != mStrSetVars.end()) {
      retVal = !lIt->second.empty();
    } // if
    return retVal;
  } // DTConfig::HasShowOnly
  bool HasSkipFuncs() const {
    bool retVal(false);
    map<DTConfigVar, set<string> >::const_iterator lIt(mStrSetVars.find(DTCVSkipFuncs));
    if (lIt != mStrSetVars.end()) {
      retVal = !lIt->second.empty();
    } // if
    return retVal;
  } // DTConfig::HasSkipFuncs
  bool HasSkipChildCalls() const {
    bool retVal(false);
    map<DTConfigVar, set<string> >::const_iterator lIt(mStrSetVars.find(DTCVSkipChildCalls));
    if (lIt != mStrSetVars.end()) {
      retVal = !lIt->second.empty();
    } // if
    return retVal;
  } // DTConfig::HasSkipChildCalls
private:
  static hFSInt8 smRefCount;
  DTConfigVar mConfigVars;
  //! \todo M Design: I wished that we could use host types here but it needs splitting of hosttargettypes.hpp.
  hFUInt16 mMaxCallDepth;
  //! \todo M Design: Perhaps it is better to create commands for execution points.
  map<DTConfigVar, set<string> > mStrSetVars; //! string sets for config vars.
  map<string, set<string> > mEPSets; //!< Definition of set of execution points.
  //! \todo M Design: change the second string to an enum.
  map<DTConfigVar, string> mVarType; //!< holds the type of config variable.
  friend class DebugTrace;
}; // class DTConfig

extern DTConfig dtConfig;

typedef enum {
  DTCCInvalid       = -1,
  DTCCNotSet        = 0,       //!< Means there is no command set.
  DTCCAbort         = 1 << 0,  //!< When set, aborts the program, called by DBC asserts.
  DTCCDumpProg      = 1 << 1,  //!< When set, IR program is dumped.
  DTCCDumpRef       = 1 << 2,  //!< The format of dump should be reference.
  DTCCIndent        = 1 << 3,  //!< Indentation command.
  DTCCEndLine       = 1 << 4,  //!< End of line command.
  DTCCSEGV          = 1 << 5,  //!< Segmentation violation is received.
  DTCCDumpExpr      = 1 << 6,  //!< We are in a dump expression context.
  DTCCDBC           = 1 << 7,  //!< We are in a DBC macro calls dump, stacks.
  DTCCDumpUseExpr   = 1 << 8,  //!< We are in a dump use expression context.
  DTCCDumpHWDesc    = 1 << 9,  //!< We are in a dump hw description context.
  DTCCDumpDFA       = 1 << 10, //!< We are in a DFA dump context.
  DTCCDumpCGCxt     = 1 << 11, //!< We are in a CGContext dump context.
  DTCCIncIndent     = 1 << 12, //!< Increment indentation command.
  DTCCDecIndent     = 1 << 13, //!< Decrement indentation command.
  DTCCDumpMatchInfo = 1 << 14, //!< We are in a dump match info context.
  DTCCDumpParam     = 1 << 15, //!< We are in dump as parameter context.
  DTCCDumpThis      = 1 << 16, //!< We are in dump as this context.
  DTCCDumpStats     = 1 << 17, //!< When set, statistics are dumped.
} DTCCommand;

//! \brief Debug/Trace context.
//! \note This is a run-time singleton object.
class DTContext {
public:
  DTContext();
  DTContext(const DTContext& dtContext_);
  DTContext(DTCCommand command_);
  DTContext(DTCCommand command_, const string& file_, hFSInt32 line_);
  //! \brief Removes the beginning and end of function context.
  //! \note Should be typically called after processing DTCBELEndOfEnd, DTCBELEndOfBeg.
  DTConfig& GetConfig() {return dtConfig;}
  DTContext operator + (DTCCommand command_) const {
    DTContext retVal(*this);
    retVal.mCommand = (DTCCommand)(mCommand | command_);
    return retVal;
  } // DTContext::operator +
  bool operator != (DTCCommand command_) const {
    return !(mCommand & command_);
  } // DTContext::operator !=
  //! \brief Checks if the command context bit is set.
  bool operator == (DTCCommand command_) const {
    return mCommand & command_;
  } // DTContext::operator ==
  bool IsInDumpContext() const {
    //! \todo M Design: Populate the list as new dump contexts are added.
    return mCommand & DTCCDumpProg || mCommand & DTCCDumpHWDesc || mCommand & DTCCDumpStats;
  } // DTContext::IsInDumpContext
  const string& GetFile() const {return mFile;}
  hFSInt32 GetLine() const {return mLine;}
  static const IDebuggable* GetCurrObj(const string& name_) {
    map<string, const IDebuggable*>::const_iterator lIt(mContextObjs.find(name_));
    const IDebuggable* retVal(0);
    if (lIt != mContextObjs.end()) {
      retVal = lIt->second;
    } // if
    return retVal;
  } // DTContext::GetCurrObj
  static void SetCurrObj(const string& name_, const IDebuggable* currObj_) {
    mContextObjs[name_] = currObj_;
  } // DTContext::SetCurrObj
  static void SetCFGGraph(ostream* o_) {mCFGGraph = o_;}
  static ostream& GetCFGGraph() {return *mCFGGraph;}
private:
  static map<string, const IDebuggable*> mContextObjs;
  DTCCommand mCommand; //!< Command send to the DebugTrace utility.
  string mFile; //!< File name of currently set execution point, valid only when begEndLine == DTCBELExePoint.
  hFSInt32 mLine; //!< Line number of currently set execution point, valid only when begEndLine == DTCBELExePoint.
  //! \brief Stream that is used to dump the CFG in a graph format, e.g. graphviz.
  static ostream* mCFGGraph;
  friend const char* dtToStr(const DTContext&);
}; // class DTContext

extern DTContext* dtContext;

//! \brief tristate values.
typedef enum {
  TV_Invalid=-1,
  TV_False=0,
  TV_True=1,
  TV_Unknown=2
} TristateValue;

//! \brief tristate class is an enhanced version of bool with extra value \a unknown.
//!
//! Default state is \a unknown.
//!
//! \par It is quite common to use a temporary boolean value for setting the
//! return value of a bool returning function. Since bool can have either true or false,
//! the return value of this return variable is initialized to true or false.
//! This causes a problem when implementation could have forgotten to set a
//! value for the return variable. If tristate had been used it was possible
//! to use ENSURE(!retVal.isUnknown()) and implementer could live in peace of mind.
//!
//! \note tristate is typically encountered in Predicate functions.
class tristate : public IDebuggable {
public:
  //! \brief Default value of tristate is \a unknown.
  tristate();
  //! \brief You may construct tristate from a \a bool.
  tristate(bool val_);
  //! \brief Checks if the state is \a true.
  bool isTrue() const;
  //! \brief Checks if the state is \a false.
  bool isFalse() const;
  //! \brief Checks if the state is \a unknown.
  bool isUnknown() const;
  bool operator != (bool val_) const;
  bool operator == (bool val_) const;
  //! \brief Only way to set the value to a boolean.
  void operator = (bool val_);
  bool IsValid() const;
  //! \brief Sets the state to \a unknown.
  void setUnknown();
  bool operator!();
  //! \brief Provides type conversion to bool.
  //! \pre State is not \a unknown.
  operator bool() const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  TristateValue mValue;
}; // class tristate

template<class T>
class tree {
public:
  tree<T>(const T& data_) : 
    mParent(0),
    mData(data_)
  {
  } // tree::tree
  //! \return Returns the index of the child.
  void AddChild(const T& child_) {
    mChildren.push_back(new tree<T>(child_));
    mChildren.back().mParent = this;
  } // tree::AddChild
  void Remove(const T& data_) {
    mChildren.erase(find(mChildren.begin(), mChildren.end(), data_));
  } // tree::Remove
  bool HasChild(const T& data_) {
    bool retVal(!mChildren.empty());
    retVal = retVal == true && (find(mChildren.begin(), mChildren.end(), data_) != mChildren.end());
    return retVal;
  } // tree::HasChild
  tree<T>* GetParent() const {return mParent;}
  tree<T>* GetRecCopy() const {
    ASSERTMSG(0, ieStrNotImplemented);
    tree<T>* retVal();
    return retVal;
  } // tree::GetRecCopy
  const vector<tree<T> >& GetChildren() const {return mChildren;}
  hFUInt32 GetNumChildren() const {return mChildren.size();}
private:
  tree<T>* mParent;
  T mData;
  vector<tree<T> > mChildren;
}; // class tree

#endif // __UTILS_HPP__


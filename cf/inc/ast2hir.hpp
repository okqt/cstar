// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __AST2HIR_HPP__
#define __AST2HIR_HPP__

#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __IREXPRBUILDER_HPP__
#include "irexprbuilder.hpp"
#endif

//! \file ast2hir.hpp
//! \brief This file contains the interface classes for Abstract Syntax Tree
//! to HIR conversion. Since there may be multiple front-ends we have a base
//! class IAST2HIR. Each front-end shall have its own derived class.

//! \brief Interface base class for abstract syntax tree to HIR conversions.
class IAST2HIR : public IDebuggable {
public:
  IAST2HIR(CGContext* cgContext_) :
    mCGContext(cgContext_)
  {
    BMDEBUG1("IAST2HIR::IAST2HIR");
    EMDEBUG0();
  } // IAST2HIR::IAST2HIR
  virtual ~IAST2HIR() {}
  virtual void ConvertMain(void* ast) {
    // Do not call this function.
    REQUIRED(0);
  } // IAST2HIR::ConvertMain
  virtual char* debug(const DTContext& context, ostream& toStr) const;
  CGContext* GetCGContext() const {
    return mCGContext;
  } // IAST2HIR::GetCGContext
private:
  CGContext* mCGContext;
}; // class IAST2HIR

//! \brief Begin(or entry) and end(exit) statement pairs of a code region.
//!
//! This is a helper class used in generating statements.
//! \note The program execution must start with begin statement and end
//!       or pass thru the exit statement.
//! \note Statement regions may never be empty. So, you must create
//!       at least one -null- statement for each code region. Although,
//!       this is redundant and inefficient, it leads to a much readable
//!       and understandable code in statement conversions, e.g. if/while/for.
//!       However, note that we did not do the same for expression regions.
//!       Because empty expression regions are so much that adding null
//!       statements cause ununderstandable HIR code.
class StmtBEPair : public IDebuggable {
public: // Constructors
  StmtBEPair(const StmtBEPair& stmtBEPair_) :
    mInitIC(stmtBEPair_.mInitIC),
    mBegin(stmtBEPair_.mBegin),
    mEnd(stmtBEPair_.mEnd),
    mBegIC(ICInvalid),
    mEndIC(ICInvalid)
  {
    BMDEBUG2("StmtBEPair::StmtBEPair", &stmtBEPair_);
    EMDEBUG1(this);
  } // StmtBEPair::StmtBEPair
  StmtBEPair(const ICStmt& ic_) :
    mInitIC(&Invalid<ICStmt>::Obj()),
    mBegin(&Invalid<IRStmt>::Obj()),
    mEnd(&Invalid<IRStmt>::Obj()),
    mBegIC(ICInvalid),
    mEndIC(ICInvalid)
  {
    BMDEBUG1("StmtBEPair::StmtBEPair");
    if (!pred.pIsInvalid(&ic_)) {
      mInitIC = new ICStmt(ic_);
    } // if
    EMDEBUG1(this);
  } // StmtBEPair::StmtBEPair
  //! \brief Constructs a single statement region.
  //! \pre \p beginEnd must not be null.
  StmtBEPair(IRStmt* beginEnd_) :
    mInitIC(&Invalid<ICStmt>::Obj()),
    mBegin(beginEnd_),
    mEnd(beginEnd_),
    mBegIC(ICInvalid),
    mEndIC(ICInvalid)
  {
    BMDEBUG2("StmtBEPair::StmtBEPair", beginEnd_);
    REQUIREDMSG(beginEnd_ != 0, ieStrNonNullParam);
    EMDEBUG1(this);
  } // StmtBEPair::StmtBEPair
  //! \note Begin and end can be the same.
  //! \pre \p begin and \p end must not be null.
  StmtBEPair(IRStmt* begin_, IRStmt* end_) :
    mInitIC(&Invalid<ICStmt>::Obj()),
    mBegin(begin_),
    mEnd(end_),
    mBegIC(ICInvalid),
    mEndIC(ICInvalid)
  {
    BMDEBUG3("StmtBEPair::StmtBEPair", mBegin, mEnd);
    REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(begin_, end_) == true, ieStrNonNullParam);
    EMDEBUG1(this);
  } // StmtBEPair::StmtBEPair
public: // Member functions
  //! \todo M Design: Update documentation to reflect that stmt region can be
  //!       empty.
  virtual bool pIsEmpty() const {
    bool retVal(mBegin == &Invalid<IRStmt>::Obj());
    return retVal;
  } // StmtBEPair::pIsEmpty
  virtual const ICStmt& eGetBegIC() const {
    if (pred.pIsInvalid(mInitIC) == true) {
      mBegIC = ICStmt(mBegin, ICBefore);
    } else {
      mBegIC = *mInitIC;
    } // if
    return mBegIC;
  } // StmtBEPair::eGetBegIC
  virtual const ICStmt& eGetEndIC() const {
    if (pred.pIsInvalid(mInitIC) == true) {
      mEndIC = ICStmt(mEnd, ICAfter);
    } else {
      mEndIC = *mInitIC;
    } // if
    return mEndIC;
  } // StmtBEPair::eGetEndIC
  virtual IRStmt* eGetLastStmt() const {return mEnd;}
  virtual IRStmt* eGetFirstStmt() const {return mBegin;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  bool IsValid() const {
    NOTIMPLEMENTED(M);
    return true;
  } // StmtBEPair::IsValid
public: // Operators
  StmtBEPair& operator += (IRStmt* stmt_);
  StmtBEPair& operator += (const StmtBEPair& stmt_);
  StmtBEPair& operator += (const ExprBEPair& stmt_);
  operator const ICStmt&() {return eGetEndIC();}
  StmtBEPair& operator = (const StmtBEPair& stmtBEPair_) {
    mInitIC = stmtBEPair_.mInitIC;
    mBegin = stmtBEPair_.mBegin;
    mEnd = stmtBEPair_.mEnd;
    return *this;
  } // StmtBEPair::operator =
protected:
  ICStmt* mInitIC;
  IRStmt* mBegin;
  IRStmt* mEnd;
  mutable ICStmt mBegIC;
  mutable ICStmt mEndIC;
}; // class StmtBEPair

//! \brief Expression begin and end statement pair.
//!
//! Expression may have empty begin/end statement pairs.
//! However, this representation is not that useful for
//! insert context conversions of region begin and ends.
//! So, we require the previous insert context for empty
//! regions, which must have ICAfter place. You may not
//! convert empty expression regions to statement regions.
//! However, you may get begin and end context of them,
//! it is basically the insert context provided to the
//! empty region.
//! \note Most of the helper member functions are needed to
//!       achieve a cleaner statement construction.
class ExprBEPair : public StmtBEPair {
public: // Constructors
  ExprBEPair(const ExprBEPair& exprBEPair_) :
    StmtBEPair(exprBEPair_.mBegin, exprBEPair_.mEnd),
    mPrevIC(exprBEPair_.mPrevIC)
  {
    BMDEBUG2("ExprBEPair::ExprBEPair", &exprBEPair_);
    EMDEBUG1(this);
  } // ExprBEPair::ExprBEPair
  ExprBEPair(IRExprAnyRef expr_, const ExprBEPair& exprBEPair_) :
    StmtBEPair(exprBEPair_.mBegin, exprBEPair_.mEnd),
    mExpr(expr_),
    mPrevIC(exprBEPair_.mPrevIC)
  {
    BMDEBUG3("ExprBEPair::ExprBEPair", expr_.GetExpr(), &exprBEPair_);
    EMDEBUG1(this);
  } // ExprBEPair::ExprBEPair
  ExprBEPair(IRExprAnyRef expr_, const StmtBEPair& stmtBEPair_) :
    StmtBEPair(stmtBEPair_),
    mExpr(expr_),
    mPrevIC(&Invalid<IRStmt>::Obj())
  {
    BMDEBUG3("ExprBEPair::ExprBEPair", expr_.GetExpr(), &stmtBEPair_);
    EMDEBUG1(this);
  } // ExprBEPair::ExprBEPair
  ExprBEPair(IRExprAnyRef expr_, IRStmt* begin_, IRStmt* end_) :
    StmtBEPair(begin_, end_),
    mExpr(expr_),
    mPrevIC(&Invalid<IRStmt>::Obj())
  {
    BMDEBUG5("ExprBEPair::ExprBEPair", expr_.GetExpr(), begin_, end_, mPrevIC);
    REQUIREDMSG(pred.GetDBCHelper()->pIsAllNonNull(begin_, end_) == true,
      "When you know they are null use the other constructor.");
    EMDEBUG1(this);
  } // ExprBEPair::ExprBEPair
  ExprBEPair(const ICStmt& prevIC_) :
    StmtBEPair(prevIC_.GetStmt(), prevIC_.GetStmt()),
    mPrevIC(prevIC_.GetStmt())
  {
    BMDEBUG2("ExprBEPair::ExprBEPair", &prevIC_);
    REQUIREDMSG(prevIC_.GetPlace() == ICAfter, "Previous IC must have ICAfter context");
    EMDEBUG1(this);
  } // ExprBEPair::ExprBEPair
  ExprBEPair(IRExprAnyRef expr_, const ICStmt& prevIC_) :
    StmtBEPair(prevIC_.GetStmt(), prevIC_.GetStmt()),
    mExpr(expr_),
    mPrevIC(prevIC_.GetStmt())
  {
    BMDEBUG3("ExprBEPair::ExprBEPair", expr_.GetExpr(), &prevIC_);
    REQUIREDMSG(prevIC_.GetPlace() == ICAfter, "Previous IC must have ICAfter context");
    EMDEBUG1(this);
  } // ExprBEPair::ExprBEPair
public: // Member functions
  virtual const ICStmt& eGetBegIC() const {
    mBegIC = StmtBEPair::eGetBegIC();
    if (mPrevIC != &Invalid<IRStmt>::Obj()) {
      mBegIC = ICStmt(mPrevIC, ICAfter);
    } // if
    return mBegIC;
  } // ExprBEPair::eGetBegIC
  virtual const ICStmt& eGetEndIC() const {
    mEndIC = StmtBEPair::eGetEndIC();
    if (mPrevIC != &Invalid<IRStmt>::Obj()) {
      mEndIC = ICStmt(mPrevIC, ICAfter);
    } // if
    return mEndIC;
  } // ExprBEPair::eGetEndIC
  virtual IRStmt* eGetFirstStmt() const {
    REQUIREDMSG(pIsEmpty() == false, "Must be non-empty region");
    return StmtBEPair::eGetFirstStmt();
  } // ExprBEPair::eGetFirstStmt
  virtual IRStmt* eGetLastStmt() const {
    REQUIREDMSG(pIsEmpty() == false, "Must be non-empty region");
    return StmtBEPair::eGetLastStmt();
  } // ExprBEPair::eGetLastStmt
  const IRExprAnyRef& GetExpr() const {return mExpr;}
  IRExpr* GetRecCopy() const {return mExpr.GetRecCopy().GetExpr();}
  IRExprAnyRef GetRecCopyRef() const {return mExpr.GetRecCopy();}
  //! \todo M Design: Could it be better to rename this in to IsEmpty, let p
  //!       be belonging to predicate?
  virtual bool pIsEmpty() const {
    bool retVal(mPrevIC != &Invalid<IRStmt>::Obj());
    return retVal;
  } // ExprBEPair::pIsEmpty
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  bool IsValid() const {
    NOTIMPLEMENTED(M);
    return true;
  } // ExprBEPair::IsValid
public: // Operators
  operator const ICStmt&() {return eGetEndIC();}
  operator StmtBEPair() const {
    REQUIREDMSG(!pIsEmpty(), "You cannot convert empty expression region to StmtBEPair");
    return StmtBEPair(mBegin, mEnd);
  } // ExprBEPair::operator StmtBEPair
  operator IRExprAnyRef() const {return mExpr;}
  operator IRExpr*() const {return mExpr.GetExpr();}
  ExprBEPair& operator = (const ExprBEPair& exprBEPair_) {
    mBegin = exprBEPair_.mBegin;
    mEnd = exprBEPair_.mEnd;
    mExpr = exprBEPair_.mExpr;
    mPrevIC = exprBEPair_.mPrevIC;
    return *this;
  } // ExprBEPair::operator =
  ExprBEPair& operator = (const StmtBEPair& stmtBEPair_) {
    mBegin = stmtBEPair_.eGetFirstStmt();
    mEnd = stmtBEPair_.eGetLastStmt();
    setNotEmpty();
    return *this;
  } // ExprBEPair::operator =
  ExprBEPair& operator = (const IRExprAnyRef& expr_) {
    mExpr = expr_;
    return *this;
  } // ExprBEPair::operator =
private: // Member functions
  //! \brief When expression region becomes not empty use this function.
  void setNotEmpty() {
    mPrevIC = &Invalid<IRStmt>::Obj();
  } // ExprBEPair::setNotEmpty
  //! \brief When expression region becomes empty use this function.
  void setEmpty(IRStmt* prevIC_) {
    mPrevIC = prevIC_;
  } // ExprBEPair::setEmpty
private: // Member data
  IRExprAnyRef mExpr;
  //! \brief Previous insert context, non-null if expression region is empty.
  IRStmt* mPrevIC;
}; // class ExprBEPair

// When expression region is empty it is tedious to extract begin/end
// context, first/last statements. Therefore, provide an easy way to
// make non-empty expression regions, provide + operator.
ExprBEPair operator + (IRStmt* stmt_, const ExprBEPair& exprBEPair_);
StmtBEPair operator + (IRStmt* stmt_, const StmtBEPair& stmtBEPair_);
StmtBEPair operator + (const StmtBEPair& stmtBEPair_, IRStmt* stmt_);
ExprBEPair operator + (const ExprBEPair& exprBEPair_, IRStmt* stmt_);
ExprBEPair operator + (const ExprBEPair& exprBEPairBeg_, const ExprBEPair& exprBEPairEnd_);
StmtBEPair operator + (const StmtBEPair& stmtBEPair_, const ExprBEPair& exprBEPair_);
StmtBEPair operator + (const ExprBEPair& exprBEPair_, const StmtBEPair& stmtBEPair_);
StmtBEPair operator + (const StmtBEPair& stmtBEPairBeg_, const StmtBEPair& stmtBEPairEnd_);


#endif


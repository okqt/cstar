// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __AST2HIR_HPP__
#include "ast2hir.hpp"
#endif

char* IAST2HIR::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IAST2HIR::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  DTASSERT(context_ != DTCCDumpExpr, "Internal Error");
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IAST2HIR::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IAST2HIR::debug

char* IRExprBuilder::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("IRExprBuilder::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // IRExprBuilder::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // IRExprBuilder::debug

char* ExprBEPair::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("ExprBEPair::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    const char* isEmpty = !pIsEmpty() ? "" : "E, ";
    toStr_ << "{" << isEmpty << refcxt(mExpr.GetExpr()) << ", " << refcxt(mBegin) << ", " << refcxt(mEnd) << "}";
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // ExprBEPair::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // ExprBEPair::debug

char* StmtBEPair::
debug(const DTContext& context_, ostream& toStr_) const {
  BDEBUGCALL();
  DTLOG("StmtBEPair::debug(" << dtToStr(context_) << ") {" << dtlIndent << dtlEndl);
  if (context_ == DTCCDumpRef) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    toStr_ << "{";
    if (!pred.pIsInvalid(mBegin)) {
      toStr_ << refcxt(mBegin);
    } // if
    toStr_ << ",";
    if (!pred.pIsInvalid(mEnd)) {
      toStr_ << refcxt(mEnd);
    } // if
    toStr_ << ",";
    if (!pred.pIsInvalid(mInitIC)) {
      toStr_ << "<" << refcxt(mInitIC->GetStmt()) << ", " << mInitIC->GetPlace() << ">";
    } // if
    toStr_ << "}";
  } else if (context_ == DTCCDumpProg) {
    debugTrace->dumpObjRef(this, context_, toStr_);
    NOTIMPLEMENTED();
  } else {
    toStr_ << refcxt(this);
  } // if
  DTLOG(dtlUnindent << dtlEndl << "} // StmtBEPair::debug" << dtlEndl);
  EDEBUGCALL();
  return "";
} // StmtBEPair::debug

// //////////////////////////
// Region expansion operators
// //////////////////////////
// See the header file for the documentation of regions.
ExprBEPair
operator + (IRStmt* stmt_, const ExprBEPair& exprBEPair_) {
  BDEBUG3("operator+BEPair", stmt_, &exprBEPair_);
  IRStmt* lEndStmt(!exprBEPair_.pIsEmpty() ? exprBEPair_.eGetLastStmt() : stmt_);
  ExprBEPair retVal(exprBEPair_.GetExpr(), stmt_, lEndStmt);
  EDEBUG1(&retVal);
  return retVal;
} // operator +

ExprBEPair
operator + (const ExprBEPair& exprBEPair_, IRStmt* stmt_) {
  BDEBUG3("operator+BEPair", &exprBEPair_, stmt_);
  IRStmt* lBegStmt(!exprBEPair_.pIsEmpty() ? exprBEPair_.eGetFirstStmt() : stmt_);
  ExprBEPair retVal(exprBEPair_.GetExpr(), lBegStmt, stmt_);
  EDEBUG1(&retVal);
  return retVal;
} // operator +

StmtBEPair
operator + (IRStmt* stmt_, const StmtBEPair& stmtBEPair_) {
  BDEBUG3("operator+BEPair", stmt_, &stmtBEPair_);
  StmtBEPair retVal(stmt_, stmtBEPair_.eGetLastStmt());
  if (stmtBEPair_.pIsEmpty() == true) {
    retVal = stmt_;
  } // if
  EDEBUG1(&retVal);
  return retVal;
} // operator +

StmtBEPair
operator + (const StmtBEPair& stmtBEPair_, IRStmt* stmt_) {
  BDEBUG3("operator+BEPair", &stmtBEPair_, stmt_);
  StmtBEPair retVal(stmtBEPair_.eGetFirstStmt(), stmt_);
  if (stmtBEPair_.pIsEmpty() == true) {
    retVal = stmt_;
  } // if
  EDEBUG1(&retVal);
  return retVal;
} // operator +

ExprBEPair
operator + (const ExprBEPair& exprBEPairBeg_, const ExprBEPair& exprBEPairEnd_) {
  BDEBUG3("operator+BEPair", &exprBEPairBeg_, &exprBEPairEnd_);
  // Assume result is an empty region
  ExprBEPair retVal(exprBEPairEnd_.eGetEndIC());
  if (!exprBEPairBeg_.pIsEmpty() || !exprBEPairEnd_.pIsEmpty()) {
    IRStmt* lBegStmt(!exprBEPairBeg_.pIsEmpty() ? exprBEPairBeg_.eGetFirstStmt() : exprBEPairEnd_.eGetFirstStmt());
    IRStmt* lEndStmt(!exprBEPairEnd_.pIsEmpty() ? exprBEPairEnd_.eGetLastStmt() : exprBEPairBeg_.eGetLastStmt());
    retVal = StmtBEPair(lBegStmt, lEndStmt);
  } // if
  EDEBUG1(&retVal);
  return retVal;
} // operator +

StmtBEPair
operator + (const StmtBEPair& stmtBEPair_, const ExprBEPair& exprBEPair_) {
  BDEBUG3("operator+BEPair", &stmtBEPair_, &exprBEPair_);
  IRStmt* lEndStmt(!exprBEPair_.pIsEmpty() ? exprBEPair_.eGetLastStmt() : stmtBEPair_.eGetLastStmt());
  StmtBEPair retVal(stmtBEPair_.eGetFirstStmt(), lEndStmt);
  EDEBUG1(&retVal);
  return retVal;
} // operator +

StmtBEPair
operator + (const ExprBEPair& exprBEPair_, const StmtBEPair& stmtBEPair_) {
  BDEBUG3("operator+BEPair", &exprBEPair_, &stmtBEPair_);
  IRStmt* lBegStmt(!exprBEPair_.pIsEmpty() ? exprBEPair_.eGetFirstStmt() : stmtBEPair_.eGetFirstStmt());
  StmtBEPair retVal(lBegStmt, stmtBEPair_.eGetLastStmt());
  EDEBUG1(&retVal);
  return retVal;
} // operator +

StmtBEPair
operator + (const StmtBEPair& stmtBEPairBeg_, const StmtBEPair& stmtBEPairEnd_) {
  BDEBUG3("operator+BEPair", &stmtBEPairBeg_, &stmtBEPairEnd_);
  StmtBEPair retVal(stmtBEPairBeg_);
  if (stmtBEPairBeg_.pIsEmpty() == true) {
    retVal = stmtBEPairEnd_;
  } else if (stmtBEPairEnd_.pIsEmpty() == true) {
    /* LIE: set at initialization */
  } else {
    retVal = StmtBEPair(stmtBEPairBeg_.eGetFirstStmt(), stmtBEPairEnd_.eGetLastStmt());
  } // if
  EDEBUG1(&retVal);
  return retVal;
} // operator +

StmtBEPair& StmtBEPair::
operator += (IRStmt* stmt_) {
  *this = *this + stmt_;
  return *this;
} // StmtBEPair:: operator +=

StmtBEPair& StmtBEPair::
operator += (const StmtBEPair& stmt_) {
  *this = *this + stmt_;
  return *this;
} // StmtBEPair:: operator +=

StmtBEPair& StmtBEPair::
operator += (const ExprBEPair& expr_) {
  *this = *this + expr_;
  return *this;
} // StmtBEPair:: operator +=



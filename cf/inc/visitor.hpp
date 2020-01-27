// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __VISITOR_HPP__
#define __VISITOR_HPP__

#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif

struct StmtVisitorOptions {
  StmtVisitorOptions() :
    mNoPreheader(true),
    mMarkStmts(false)
  {
  } // StmtVisitorOptions
  StmtVisitorOptions& SetPreheaderNo() {mNoPreheader = true; return *this;}
  StmtVisitorOptions& SetPreheaderYes() {mNoPreheader = false; return *this;}
  StmtVisitorOptions& SetMark2False(eIRTreeMark markWhat_ = eIRTMGeneric) {mMarkStmts = true; mMarkWhat = markWhat_; mMarkValue = false; return *this;}
  StmtVisitorOptions& SetMark2True(eIRTreeMark markWhat_ = eIRTMGeneric) {mMarkStmts = true; mMarkWhat = markWhat_; mMarkValue = true; return *this;}
  bool IsPreheaderNo() const {return mNoPreheader;}
  bool IsShouldMark() const {return mMarkStmts == true;}
  bool GetMark() const {return mMarkValue;}
  eIRTreeMark GetMarkWhat() const {return mMarkWhat;}
private:
  //! \brief Do not collect preheader statements, default does not collect them.
  bool mNoPreheader;
  bool mMarkStmts; //!< If true set the mIsMarked of statements to mMarkValue;
  eIRTreeMark mMarkWhat;
  bool mMarkValue; 
  friend class StmtVisitor;
}; // struct StmtVisitorOptions

class StmtVisitor : public IDebuggable {
public:
  StmtVisitor(bool mustVisit_, const StmtVisitorOptions& opt_) : 
    mOptions(opt_),
    mMustVisit(mustVisit_) 
  {
  } // StmtVisitor::StmtVisitor
  virtual ~StmtVisitor() {}
  virtual bool Visit(IRStmt* stmt_);
  virtual bool Visit(IRSAssign* stmt_);
  virtual bool Visit(IRSCall* stmt_);
  virtual bool Visit(IRSFCall* stmt_);
  virtual bool Visit(IRSPCall* stmt_);
  virtual bool Visit(IRSReturn* stmt_);
  virtual bool Visit(IRSIf* stmt_);
  virtual bool Visit(IRSJump* stmt_);
  virtual bool Visit(IRSDynJump* stmt_);
  virtual bool Visit(IRSProlog* stmt_);
  virtual bool Visit(IRSEpilog* stmt_);
  virtual bool Visit(IRSEval* stmt_);
  virtual bool Visit(IRSLabel* stmt_);
  virtual bool Visit(IRSNull* stmt_);
  virtual bool Visit(IRSSwitch* stmt_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  const StmtVisitorOptions& GetOptions() const {return mOptions;}
private:
  bool mMustVisit;
  StmtVisitorOptions mOptions;
}; // class StmtVisitor

struct ExprVisitorOptions {
  ExprVisitorOptions() :
    mNoLHS(false),
    mNoPreheader(true),
    mMarkExprs(false)
  {
  } // ExprVisitorOptions
  ExprVisitorOptions& SetPreheaderNo() {mNoPreheader = true; return *this;}
  ExprVisitorOptions& SetPreheaderYes() {mNoPreheader = false; return *this;}
  ExprVisitorOptions& SetLHSNo() {mNoLHS = true; return *this;}
  ExprVisitorOptions& SetLHSYes() {mNoLHS = false; return *this;}
  ExprVisitorOptions& SetMark2False(eIRTreeMark markWhat_) {mMarkExprs = true; mMarkWhat = markWhat_; mMarkValue = false; return *this;}
  ExprVisitorOptions& SetMark2True(eIRTreeMark markWhat_) {mMarkExprs = true; mMarkWhat = markWhat_; mMarkValue = true; return *this;}
  bool IsLHSNo() const {return mNoLHS;}
  bool IsPreheaderNo() const {return mNoPreheader;}
  bool IsShouldMark() const {return mMarkExprs == true;}
  bool GetMark() const {return mMarkValue;}
  eIRTreeMark GetMarkWhat() const {return mMarkWhat;}
private:
  //! \brief Do not collect LHS expressions, default collects LHS expressions.
  //! It does not collect lhs of asignments and return in objects of fcalls.
  bool mNoLHS;
  //! \brief Do not collect preheader expressions, default does not collect them.
  bool mNoPreheader;
  eIRTreeMark mMarkWhat;
  bool mMarkExprs; //!< If true set the mIsMarked of expressions to mMarkValue;
  bool mMarkValue; 
  friend class ExprVisitor;
  friend class CExprVisitor;
}; // struct ExprVisitorOptions

class ExprVisitor : public IDebuggable {
public:
  ExprVisitor(bool mustVisit_, ExprVisitOrderType order_, const ExprVisitorOptions& opt_) : 
    mMustVisit(mustVisit_),
    mOrder(order_),
    mOptions(opt_)
  {
  } // ExprVisitor::ExprVisitor
  virtual ~ExprVisitor() {}
  ExprVisitOrderType GetOrderType() const {return mOrder;}
  virtual bool Visit(IREAdd* expr_);
  virtual bool Visit(IREAddrConst* expr_);
  virtual bool Visit(IREAddrOf* expr_);
  virtual bool Visit(IREANeg* expr_);
  virtual bool Visit(IREAShiftRight* expr_);
  virtual bool Visit(IREBAnd* expr_);
  virtual bool Visit(IREBinary* expr_);
  virtual bool Visit(IREConst* expr_);
  virtual bool Visit(IREBNeg* expr_);
  virtual bool Visit(IREBoolConst* expr_);
  virtual bool Visit(IREBOr* expr_);
  virtual bool Visit(IREBXOr* expr_);
  virtual bool Visit(IRECast* expr_);
  virtual bool Visit(IRECmp* expr_);
  virtual bool Visit(IREDeref* expr_);
  virtual bool Visit(IREDiv* expr_);
  virtual bool Visit(IREEq* expr_);
  virtual bool Visit(IREGe* expr_);
  virtual bool Visit(IREGt* expr_);
  virtual bool Visit(IREIntConst* expr_);
  virtual bool Visit(IRELAnd* expr_);
  virtual bool Visit(IRELe* expr_);
  virtual bool Visit(IRELOr* expr_);
  virtual bool Visit(IRELShiftRight* expr_);
  virtual bool Visit(IRELt* expr_);
  virtual bool Visit(IREMember* expr_);
  virtual bool Visit(IREMod* expr_);
  virtual bool Visit(IREMul* expr_);
  virtual bool Visit(IRENe* expr_);
  virtual bool Visit(IRENot* expr_);
  virtual bool Visit(IRENull* expr_);
  virtual bool Visit(IREPtrAdd* expr_);
  virtual bool Visit(IREPtrSub* expr_);
  virtual bool Visit(IREQuo* expr_);
  virtual bool Visit(IRERealConst* expr_);
  virtual bool Visit(IRERem* expr_);
  virtual bool Visit(IREShiftLeft* expr_);
  virtual bool Visit(IREStrConst* expr_);
  virtual bool Visit(IRESub* expr_);
  virtual bool Visit(IRESubscript* expr_);
  virtual bool Visit(IREUnary* expr_);
  virtual bool Visit(IRExpr* expr_);
  virtual bool Visit(IRExprList* expr_);
  virtual bool Visit(IRObject* obj_);
  virtual bool Visit(IROField* obj_);
  virtual bool Visit(IROBitField* obj_);
  virtual bool Visit(IROLocal* obj_);
  virtual bool Visit(IROGlobal* obj_);
  virtual bool Visit(IROTmpGlobal* obj_);
  virtual bool Visit(IROParameter* obj_);
  virtual bool Visit(IRORelocSymbol* obj_);
  virtual bool Visit(IROTmp* obj_);
  // NonTerminals
  virtual bool Visit(NonTerminal* nt_);
  virtual bool Visit(ConditionNT* nt_);
  virtual bool Visit(NullNT* nt_);
  virtual bool Visit(AnyNT* nt_);
  virtual bool Visit(RegisterNT* nt_);
  virtual bool Visit(IgnoreNT* nt_);
  virtual bool Visit(IntConstNT* nt_);
  virtual bool Visit(RealConstNT* nt_);
  virtual bool Visit(MemoryNT* nt_);
  virtual bool Visit(ShiftOpNT* nt_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public: // Member data
  const ExprVisitorOptions& GetOptions() const {return mOptions;}
  ExprVisitorOptions mOptions;
public:
  static const bool cMayNotVisit = false;
  static const bool cMustVisit = true;
private:
  //! \brief When set to true at ctor time all the member visit functions
  //!        must be overridden in the drived class.
  bool mMustVisit;
  ExprVisitOrderType mOrder;
}; // class ExprVisitor

//! \brief Constant visitor that does not change the elements visited.
class CExprVisitor : public IDebuggable {
public:
  CExprVisitor(bool mustVisit_, ExprVisitOrderType order_) :
    mMustVisit(mustVisit_),
    mOrder(order_)
  {
  } // CExprVisitor::CExprVisitor
  ExprVisitOrderType GetOrderType() const {return mOrder;}
  virtual ~CExprVisitor() {}
  virtual bool Visit(const IREAdd* expr_) const;
  virtual bool Visit(const IREAddrConst* expr_) const;
  virtual bool Visit(const IREAddrOf* expr_) const;
  virtual bool Visit(const IREANeg* expr_) const;
  virtual bool Visit(const IREAShiftRight* expr_) const;
  virtual bool Visit(const IREBAnd* expr_) const;
  virtual bool Visit(const IREBinary* expr_) const;
  virtual bool Visit(const IREConst* expr_) const;
  virtual bool Visit(const IREBNeg* expr_) const;
  virtual bool Visit(const IREBoolConst* expr_) const;
  virtual bool Visit(const IREBOr* expr_) const;
  virtual bool Visit(const IREBXOr* expr_) const;
  virtual bool Visit(const IRECast* expr_) const;
  virtual bool Visit(const IRECmp* expr_) const;
  virtual bool Visit(const IREDeref* expr_) const;
  virtual bool Visit(const IREDiv* expr_) const;
  virtual bool Visit(const IREEq* expr_) const;
  virtual bool Visit(const IREGe* expr_) const;
  virtual bool Visit(const IREGt* expr_) const;
  virtual bool Visit(const IREIntConst* expr_) const;
  virtual bool Visit(const IRELAnd* expr_) const;
  virtual bool Visit(const IRELe* expr_) const;
  virtual bool Visit(const IRELOr* expr_) const;
  virtual bool Visit(const IRELShiftRight* expr_) const;
  virtual bool Visit(const IRELt* expr_) const;
  virtual bool Visit(const IREMember* expr_) const;
  virtual bool Visit(const IREMod* expr_) const;
  virtual bool Visit(const IREMul* expr_) const;
  virtual bool Visit(const IRENe* expr_) const;
  virtual bool Visit(const IRENot* expr_) const;
  virtual bool Visit(const IRENull* expr_) const;
  virtual bool Visit(const IREPtrAdd* expr_) const;
  virtual bool Visit(const IREPtrSub* expr_) const;
  virtual bool Visit(const IREQuo* expr_) const;
  virtual bool Visit(const IRERealConst* expr_) const;
  virtual bool Visit(const IRERem* expr_) const;
  virtual bool Visit(const IREShiftLeft* expr_) const;
  virtual bool Visit(const IREStrConst* expr_) const;
  virtual bool Visit(const IRESub* expr_) const;
  virtual bool Visit(const IRESubscript* expr_) const;
  virtual bool Visit(const IREUnary* expr_) const;
  virtual bool Visit(const IRExpr* expr_) const;
  virtual bool Visit(const IRExprList* expr_) const;
  virtual bool Visit(const IRObject* obj_) const;
  virtual bool Visit(const IROField* obj_) const;
  virtual bool Visit(const IROBitField* obj_) const;
  virtual bool Visit(const IROLocal* obj_) const;
  virtual bool Visit(const IROGlobal* obj_) const;
  virtual bool Visit(const IROTmpGlobal* obj_) const;
  virtual bool Visit(const IROParameter* obj_) const;
  virtual bool Visit(const IRORelocSymbol* obj_) const;
  virtual bool Visit(const IROTmp* obj_) const;
  // NonTerminals
  virtual bool Visit(const NonTerminal* nt_) const;
  virtual bool Visit(const ConditionNT* nt_) const;
  virtual bool Visit(const NullNT* nt_) const;
  virtual bool Visit(const AnyNT* nt_) const;
  virtual bool Visit(const RegisterNT* nt_) const;
  virtual bool Visit(const IgnoreNT* nt_) const;
  virtual bool Visit(const IntConstNT* nt_) const;
  virtual bool Visit(const RealConstNT* nt_) const;
  virtual bool Visit(const MemoryNT* nt_) const;
  virtual bool Visit(const ShiftOpNT* nt_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public: // Member data
  const ExprVisitorOptions& GetOptions() const {return mOptions;}
  ExprVisitorOptions mOptions;
private:
  //! \brief When set to true at ctor time all the member visit functions
  //!        must be overridden in the drived class.
  bool mMustVisit;
  ExprVisitOrderType mOrder;
}; // class CExprVisitor

// ----------------------------
// Collectors
// ----------------------------
template<typename tcN, typename A=IRBB>
class StmtCollector : public StmtVisitor {
public:
  StmtCollector(const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
  } // StmtCollector::StmtCollector
  StmtCollector(A* accept_, const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
    accept_->Accept(*this);
  } // StmtCollector::StmtCollector
  virtual bool Visit(tcN* stmt_) {
    BMDEBUG2("StmtCollector::Visit", stmt_);
    mCollected0.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector::Visit
  hFUInt32 size() const {return mCollected0.size();}
  bool empty() const {return size() == 0;}
  tcN*& operator[](hFUInt32 index_) {return mCollected0[index_];}
public: // Member data
  vector<tcN*> mCollected0;
}; // class StmtCollector

template<typename tcN0, typename tcN1, typename A=IRBB>
class StmtCollector2 : public StmtVisitor {
public:
  StmtCollector2(const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
  } // StmtCollector2::StmtCollector2
  StmtCollector2(A* accept_, const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
    accept_->Accept(*this);
  } // StmtCollector2::StmtCollector2
  virtual bool Visit(tcN0* stmt_) {
    BMDEBUG2("StmtCollector2::Visit", stmt_);
    mCollected0.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector2::Visit
  virtual bool Visit(tcN1* stmt_) {
    BMDEBUG2("StmtCollector2::Visit", stmt_);
    mCollected1.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector2::Visit
  hFUInt32 size() const {return mCollected0.size() + mCollected1.size();}
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
}; // class StmtCollector2

template<typename tcN0, typename tcN1, typename tcN2, typename A=IRBB>
class StmtCollector3 : public StmtVisitor {
public:
  StmtCollector3(const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
  } // StmtCollector3::StmtCollector3
  StmtCollector3(A* accept_, const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
    accept_->Accept(*this);
  } // StmtCollector3::StmtCollector3
  virtual bool Visit(tcN0* stmt_) {
    BMDEBUG2("StmtCollector3::Visit", stmt_);
    mCollected0.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector3::Visit
  virtual bool Visit(tcN1* stmt_) {
    BMDEBUG2("StmtCollector3::Visit", stmt_);
    mCollected1.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector3::Visit
  virtual bool Visit(tcN2* stmt_) {
    BMDEBUG2("StmtCollector3::Visit", stmt_);
    mCollected2.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected2.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector3::Visit
  hFUInt32 size() const {return mCollected0.size() + mCollected1.size() + mCollected2.size();}
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
  vector<tcN2*> mCollected2;
}; // class StmtCollector3

template<typename tcN0, typename tcN1, typename tcN2, typename tcN3, typename A=IRBB>
class StmtCollector4 : public StmtVisitor {
public:
  StmtCollector4(const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
  } // StmtCollector4::StmtCollector4
  StmtCollector4(A* accept_, const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
    accept_->Accept(*this);
  } // StmtCollector4::StmtCollector4
  virtual bool Visit(tcN0* stmt_) {
    BMDEBUG2("StmtCollector4::Visit", stmt_);
    mCollected0.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector4::Visit
  virtual bool Visit(tcN1* stmt_) {
    BMDEBUG2("StmtCollector4::Visit", stmt_);
    mCollected1.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector4::Visit
  virtual bool Visit(tcN2* stmt_) {
    BMDEBUG2("StmtCollector4::Visit", stmt_);
    mCollected2.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected2.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector4::Visit
  virtual bool Visit(tcN3* stmt_) {
    BMDEBUG2("StmtCollector4::Visit", stmt_);
    mCollected3.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected3.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector4::Visit
  hFUInt32 size() const {return mCollected0.size() + mCollected1.size() + mCollected2.size() + mCollected3.size();}
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
  vector<tcN2*> mCollected2;
  vector<tcN3*> mCollected3;
}; // class StmtCollector4

template<typename tcN0, typename tcN1, typename tcN2, typename tcN3, typename tcN4, typename A=IRBB>
class StmtCollector5 : public StmtVisitor {
public:
  StmtCollector5(const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
  } // StmtCollector5::StmtCollector5
  StmtCollector5(A* accept_, const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
    accept_->Accept(*this);
  } // StmtCollector5::StmtCollector5
  virtual bool Visit(tcN0* stmt_) {
    BMDEBUG2("StmtCollector5::Visit", stmt_);
    mCollected0.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector5::Visit
  virtual bool Visit(tcN1* stmt_) {
    BMDEBUG2("StmtCollector5::Visit", stmt_);
    mCollected1.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector5::Visit
  virtual bool Visit(tcN2* stmt_) {
    BMDEBUG2("StmtCollector5::Visit", stmt_);
    mCollected2.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected2.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector5::Visit
  virtual bool Visit(tcN3* stmt_) {
    BMDEBUG2("StmtCollector5::Visit", stmt_);
    mCollected3.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected3.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector5::Visit
  virtual bool Visit(tcN4* stmt_) {
    BMDEBUG2("StmtCollector5::Visit", stmt_);
    mCollected4.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected4.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector5::Visit
  hFUInt32 size() const {
    return mCollected0.size() + mCollected1.size() + mCollected2.size() +
      mCollected3.size()+mCollected4.size();
  }
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
  vector<tcN2*> mCollected2;
  vector<tcN3*> mCollected3;
  vector<tcN4*> mCollected4;
}; // class StmtCollector5

template<typename tcN0, typename tcN1, typename tcN2, typename tcN3, typename tcN4, typename tcN5, typename A=IRBB>
class StmtCollector6 : public StmtVisitor {
public:
  StmtCollector6(const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
  } // StmtCollector6::StmtCollector6
  StmtCollector6(A* accept_, const StmtVisitorOptions& opt_ = StmtVisitorOptions()) : 
    StmtVisitor(ExprVisitor::cMayNotVisit, opt_) 
  {
    accept_->Accept(*this);
  } // StmtCollector6::StmtCollector6
  virtual bool Visit(tcN0* stmt_) {
    BMDEBUG2("StmtCollector6::Visit", stmt_);
    mCollected0.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector6::Visit
  virtual bool Visit(tcN1* stmt_) {
    BMDEBUG2("StmtCollector6::Visit", stmt_);
    mCollected1.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector6::Visit
  virtual bool Visit(tcN2* stmt_) {
    BMDEBUG2("StmtCollector6::Visit", stmt_);
    mCollected2.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected2.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector6::Visit
  virtual bool Visit(tcN3* stmt_) {
    BMDEBUG2("StmtCollector6::Visit", stmt_);
    mCollected3.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected3.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector6::Visit
  virtual bool Visit(tcN4* stmt_) {
    BMDEBUG2("StmtCollector6::Visit", stmt_);
    mCollected4.push_back(stmt_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected4.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // StmtCollector6::Visit
  hFUInt32 size() const {
    return mCollected0.size() + mCollected1.size() + mCollected2.size() +
      mCollected3.size()+mCollected4.size()+mCollected5.size();
  }
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
  vector<tcN2*> mCollected2;
  vector<tcN3*> mCollected3;
  vector<tcN4*> mCollected4;
  vector<tcN5*> mCollected5;
}; // class StmtCollector6


//! \todo M Design: provide a mechanism to call the Accept of a given template parameter in the ctor.
template<typename tcN, typename T=IRStmt>
class ExprCollector : public ExprVisitor {
public:
  ExprCollector(ExprVisitOrderType evot_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
  } // ExprCollector::ExprCollector
  ExprCollector(ExprVisitOrderType evot_, T* accept_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
    BMDEBUG2("ExprCollector::ExprCollector", accept_);
    accept_->Accept(*this);
    EMDEBUG1(size());
  } // ExprCollector::ExprCollector
  virtual bool Visit(tcN* expr_) {
    BMDEBUG2("ExprCollector::Visit", expr_);
    mCollected0.push_back(expr_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  } // ExprCollector::Visit
  hFUInt32 size() const {return mCollected0.size();}
  bool empty() const {return size() == 0;}
  tcN*& operator[](hFUInt32 index_) {return mCollected0[index_];}
public: // Member data
  vector<tcN*> mCollected0;
}; // class ExprCollector

template<typename tcN0, typename tcN1, typename T=IRStmt>
class ExprCollector2 : public ExprVisitor {
public:
  ExprCollector2(ExprVisitOrderType evot_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
  } // ExprCollector2::ExprCollector2
  ExprCollector2(ExprVisitOrderType evot_, T* accept_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
    BMDEBUG2("ExprCollector2::ExprCollector2", accept_);
    accept_->Accept(*this);
    EMDEBUG1(size());
  } // ExprCollector2::ExprCollector2
  virtual bool Visit(tcN0* expr_) {
    BMDEBUG2("ExprCollector2::Visit", expr_);
    mCollected0.push_back(expr_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  }
  virtual bool Visit(tcN1* expr_) {
    BMDEBUG2("ExprCollector2::Visit", expr_);
    mCollected1.push_back(expr_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  }
  hFUInt32 size() const {return mCollected0.size() + mCollected1.size();}
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
}; // class ExprCollector2

template<typename tcN0, typename tcN1, typename tcN2, typename T=IRStmt>
class ExprCollector3 : public ExprVisitor {
public:
  ExprCollector3(ExprVisitOrderType evot_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
  } // ExprCollector3::ExprCollector3
  ExprCollector3(ExprVisitOrderType evot_, T* accept_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
    BMDEBUG2("ExprCollector3::ExprCollector3", accept_);
    accept_->Accept(*this);
    EMDEBUG1(size());
  } // ExprCollector3::ExprCollector3
  virtual bool Visit(tcN0* expr_) {
    BMDEBUG2("ExprCollector3::Visit", expr_);
    mCollected0.push_back(expr_);
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true);
    return true;
  }
  virtual bool Visit(tcN1* expr_) {
    BMDEBUG2("ExprCollector3::Visit", expr_); 
    mCollected1.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN2* expr_) {
    BMDEBUG2("ExprCollector3::Visit", expr_); 
    mCollected2.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected2.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  hFUInt32 size() const {return mCollected0.size() + mCollected1.size() + mCollected2.size();}
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
  vector<tcN2*> mCollected2;
}; // class ExprCollector3

template<typename tcN0, typename tcN1, typename tcN2, typename tcN3, typename T=IRStmt>
class ExprCollector4 : public ExprVisitor {
public:
  ExprCollector4(ExprVisitOrderType evot_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
  } // ExprCollector4::ExprCollector4
  ExprCollector4(ExprVisitOrderType evot_, T* accept_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
    BMDEBUG2("ExprCollector4::ExprCollector4", accept_);
    accept_->Accept(*this);
    EMDEBUG1(size());
  } // ExprCollector4::ExprCollector4
  virtual bool Visit(tcN0* expr_) {
    BMDEBUG2("ExprCollector4::Visit", expr_); 
    mCollected0.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN1* expr_) {
    BMDEBUG2("ExprCollector4::Visit", expr_); 
    mCollected1.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN2* expr_) {
    BMDEBUG2("ExprCollector4::Visit", expr_); 
    mCollected2.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected2.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN3* expr_) {
    BMDEBUG2("ExprCollector4::Visit", expr_); 
    mCollected3.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected3.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  hFUInt32 size() const {return mCollected0.size() + mCollected1.size() + mCollected2.size() + mCollected3.size();}
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
  vector<tcN2*> mCollected2;
  vector<tcN3*> mCollected3;
}; // class ExprCollector4

template<typename tcN0, typename tcN1, typename tcN2, typename tcN3, typename tcN4, typename T=IRStmt>
class ExprCollector5 : public ExprVisitor {
public:
  ExprCollector5(ExprVisitOrderType evot_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
  } // ExprCollector5::ExprCollector5
  ExprCollector5(ExprVisitOrderType evot_, T* accept_, const ExprVisitorOptions& opt_ = ExprVisitorOptions()) : 
    ExprVisitor(ExprVisitor::cMayNotVisit, evot_, opt_) 
  {
    BMDEBUG2("ExprCollector5::ExprCollector5", accept_);
    accept_->Accept(*this);
    EMDEBUG1(size());
  } // ExprCollector5::ExprCollector5
  virtual bool Visit(tcN0* expr_) {
    BMDEBUG2("ExprCollector5::Visit", expr_); 
    mCollected0.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected0.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN1* expr_) {
    BMDEBUG2("ExprCollector5::Visit", expr_); 
    mCollected1.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected1.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN2* expr_) {
    BMDEBUG2("ExprCollector5::Visit", expr_); 
    mCollected2.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected2.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN3* expr_) {
    BMDEBUG2("ExprCollector5::Visit", expr_); 
    mCollected3.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected3.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  virtual bool Visit(tcN4* expr_) {
    BMDEBUG2("ExprCollector5::Visit", expr_); 
    mCollected4.push_back(expr_); 
    if (GetOptions().IsShouldMark() == true) {
      mCollected4.back()->SetMark2(!GetOptions().GetMark(), GetOptions().GetMarkWhat());
    } // if
    EMDEBUG1(true); 
    return true;
  }
  hFUInt32 size() const {return mCollected0.size() + mCollected1.size() + mCollected2.size() + mCollected3.size() + mCollected4.size();}
  bool empty() const {return size() == 0;}
public: // Member data
  vector<tcN0*> mCollected0;
  vector<tcN1*> mCollected1;
  vector<tcN2*> mCollected2;
  vector<tcN3*> mCollected3;
  vector<tcN4*> mCollected4;
}; // class ExprCollector5

#endif


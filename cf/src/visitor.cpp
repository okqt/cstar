// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __VISITOR_HPP__
#include "visitor.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

bool ExprVisitor::Visit(IREAdd* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREAddrConst* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREConst*>(expr_));}
bool ExprVisitor::Visit(IREAddrOf* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(expr_));}
bool ExprVisitor::Visit(IREANeg* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREUnary*>(expr_));}
bool ExprVisitor::Visit(IREAShiftRight* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREBAnd* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREBinary* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(expr_));}
bool ExprVisitor::Visit(IREBNeg* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREUnary*>(expr_));}
bool ExprVisitor::Visit(IREBoolConst* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREConst*>(expr_));}
bool ExprVisitor::Visit(IREBOr* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREBXOr* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IRECast* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREUnary*>(expr_));}
bool ExprVisitor::Visit(IRECmp* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREDeref* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREUnary*>(expr_));}
bool ExprVisitor::Visit(IREDiv* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREEq* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRECmp*>(expr_));}
bool ExprVisitor::Visit(IREGe* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRECmp*>(expr_));}
bool ExprVisitor::Visit(IREGt* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRECmp*>(expr_));}
bool ExprVisitor::Visit(IREIntConst* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREConst*>(expr_));}
bool ExprVisitor::Visit(IRELAnd* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IRELe* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRECmp*>(expr_));}
bool ExprVisitor::Visit(IRELOr* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IRELShiftRight* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IRELt* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRECmp*>(expr_));}
bool ExprVisitor::Visit(IREMember* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREMod* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREMul* expr_)
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IRENe* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRECmp*>(expr_));}
bool ExprVisitor::Visit(IRENot* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREUnary*>(expr_));}
bool ExprVisitor::Visit(IRENull* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(expr_));}
bool ExprVisitor::Visit(IREPtrSub* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREPtrAdd* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREQuo* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IRERealConst* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREConst*>(expr_));}
bool ExprVisitor::Visit(IRERem* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREShiftLeft* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREStrConst* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREConst*>(expr_));}
bool ExprVisitor::Visit(IRESub* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IRESubscript* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IREBinary*>(expr_));}
bool ExprVisitor::Visit(IREConst* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(expr_));}
bool ExprVisitor::Visit(IREUnary* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(expr_));}
bool ExprVisitor::Visit(IRExpr* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return true;}
bool ExprVisitor::Visit(IRExprList* expr_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(expr_));}
bool ExprVisitor::Visit(IRObject* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(obj_));}
bool ExprVisitor::Visit(IROBitField* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(IROLocal* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(IROGlobal* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(IROTmpGlobal* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(IROParameter* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(IRORelocSymbol* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(IROTmp* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(IROField* obj_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRObject*>(obj_));}
bool ExprVisitor::Visit(NonTerminal* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRExpr*>(nt_));}
bool ExprVisitor::Visit(ConditionNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(NullNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(AnyNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(RegisterNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(IgnoreNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(IntConstNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(RealConstNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(MemoryNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}
bool ExprVisitor::Visit(ShiftOpNT* nt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<NonTerminal*>(nt_));}

bool CExprVisitor::Visit(const IREAdd* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREAddrConst* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREConst*>(expr_));}
bool CExprVisitor::Visit(const IREAddrOf* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(expr_));}
bool CExprVisitor::Visit(const IREANeg* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREUnary*>(expr_));}
bool CExprVisitor::Visit(const IREAShiftRight* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREBAnd* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREConst* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(expr_));}
bool CExprVisitor::Visit(const IREBinary* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(expr_));}
bool CExprVisitor::Visit(const IREBNeg* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREUnary*>(expr_));}
bool CExprVisitor::Visit(const IREBoolConst* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREConst*>(expr_));}
bool CExprVisitor::Visit(const IREBOr* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREBXOr* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IRECast* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREUnary*>(expr_));}
bool CExprVisitor::Visit(const IRECmp* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREDeref* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREUnary*>(expr_));}
bool CExprVisitor::Visit(const IREDiv* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREEq* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRECmp*>(expr_));}
bool CExprVisitor::Visit(const IREGe* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRECmp*>(expr_));}
bool CExprVisitor::Visit(const IREGt* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRECmp*>(expr_));}
bool CExprVisitor::Visit(const IREIntConst* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREConst*>(expr_));}
bool CExprVisitor::Visit(const IRELAnd* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IRELe* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRECmp*>(expr_));}
bool CExprVisitor::Visit(const IRELOr* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IRELShiftRight* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IRELt* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRECmp*>(expr_));}
bool CExprVisitor::Visit(const IREMember* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREMod* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREMul* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IRENe* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRECmp*>(expr_));}
bool CExprVisitor::Visit(const IRENot* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREUnary*>(expr_));}
bool CExprVisitor::Visit(const IRENull* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(expr_));}
bool CExprVisitor::Visit(const IREPtrSub* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREPtrAdd* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREQuo* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IRERealConst* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREConst*>(expr_));}
bool CExprVisitor::Visit(const IRERem* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREShiftLeft* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREStrConst* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREConst*>(expr_));}
bool CExprVisitor::Visit(const IRESub* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IRESubscript* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IREBinary*>(expr_));}
bool CExprVisitor::Visit(const IREUnary* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(expr_));}
bool CExprVisitor::Visit(const IRExpr* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return true;}
bool CExprVisitor::Visit(const IRExprList* expr_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(expr_));}
bool CExprVisitor::Visit(const IRObject* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(obj_));}
bool CExprVisitor::Visit(const IROBitField* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const IROLocal* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const IROGlobal* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const IROTmpGlobal* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const IROParameter* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const IRORelocSymbol* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const IROTmp* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const IROField* obj_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRObject*>(obj_));}
bool CExprVisitor::Visit(const NonTerminal* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const IRExpr*>(nt_));}
bool CExprVisitor::Visit(const ConditionNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const NullNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const AnyNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const RegisterNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const IgnoreNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const IntConstNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const RealConstNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const MemoryNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}
bool CExprVisitor::Visit(const ShiftOpNT* nt_) const 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<const NonTerminal*>(nt_));}

bool StmtVisitor::Visit(IRStmt* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return true;}
bool StmtVisitor::Visit(IRSAssign* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSCall* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSFCall* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRSCall*>(stmt_));}
bool StmtVisitor::Visit(IRSPCall* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRSCall*>(stmt_));}
bool StmtVisitor::Visit(IRSReturn* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSIf* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSJump* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSDynJump* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSProlog* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSEpilog* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSEval* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSLabel* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSNull* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}
bool StmtVisitor::Visit(IRSSwitch* stmt_) 
  {REQUIREDMSG(mMustVisit == false, ieStrFuncShouldBeOverridden); return Visit(static_cast<IRStmt*>(stmt_));}


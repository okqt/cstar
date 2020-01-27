// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_LOWER_HPP__
#include "ato_lower.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

//! \todo M Design: Need to create a lock mechanism for visitor, when visit in
//!       effect nodes may not be modified.
class ExprLowerer : public ExprVisitor {
public:
  ExprLowerer(ATOLowerHIR* parent_, const vector<LowerExprInfo*>& funcs_) :
    ExprVisitor(ExprVisitor::cMayNotVisit, EVOTPost, ExprVisitorOptions()),
    mFuncs(funcs_),
    mParent(parent_)
  {
    BMDEBUG2("ExprLowerer::ExprLowerer", parent_);
    REQUIREDMSG(parent_ != 0, ieStrNonNullParam);
    EMDEBUG0();
  } // ExprLowerer::ExprLowerer
  virtual ~ExprLowerer() {}
  virtual bool Visit(IRExpr* expr_) {
    BMDEBUG2("ExprLowerer::Visit", expr_);
    for (hFInt32 c(0); c < mFuncs.size(); ++ c) {
      if (typeid(*mFuncs[c]->mPatExpr) == typeid(*expr_)) {
        PDEBUG("ExprLowerer", "detail", " expressions match.");
        IRFunctionDecl* lFunc(mFuncs[c]->mFuncReplacement);
        // Return and operand types must match.
        if (mFuncs[c]->mConvRetVal == true || (expr_->GetType() == lFunc->GetType()->GetReturnType())) {
          PDEBUG("ExprLowerer", "detail", " return types match.");
          vector<IRExpr*> lChildren;
          expr_->GetChildren(lChildren);
          bool lOperandTypesOk(true);
          //! \todo M Design: Could there be an expr having different types for
          //!       children that is of interest to be lowered? May be we just
          //!       need to check the first child?
          for (hFInt32 d(0); d < lChildren.size(); ++ d) {
            if (lChildren[d]->GetType() != lFunc->GetType()->GetParamType(d)) {
              lOperandTypesOk = false;
              break;
            } // if
          } // for
          if (lOperandTypesOk == true) {
            mExprs.push_back(new LowerExprInfo(expr_, mFuncs[c]->mFuncReplacement, mFuncs[c]->mConvRetVal));
            PDEBUG("ExprLowerer", "detail", "Expr to lower: " <<
              progcxt(expr_) << endl << " funcdecl: " << progcxt(mFuncs[c]->mFuncReplacement));
          } // if
        } // if
      } // if
    } // for
    EMDEBUG1(true);
    return true;
  } // ExprLowerer::Visit
  vector<LowerExprInfo*> mExprs;
private:
  const vector<LowerExprInfo*>& mFuncs;
  ATOLowerHIR* mParent;
}; // class ExprLowerer

hFInt32 ATOLowerHIR::
AnatropDo(IRStmt* stmt_) {
  BMDEBUG2("ATOLowerHIR::AnatropDo(IRStmt*)", stmt_);
  hFInt32 retVal(0);
  if (IRSAssign* lAssign = dynamic_cast<IRSAssign*>(stmt_)) {
    retVal += LowerStrAssign(lAssign);
  } else if (IRSSwitch* lSwitch = dynamic_cast<IRSSwitch*>(stmt_)) {
    retVal += LowerSwitch(lSwitch);
  } // if
  {
    ExprLowerer lExprLowerer(this, extr.eGetCGContext(stmt_)->GetHWDesc()->GetExprsToBeLowered());
    stmt_->Accept(lExprLowerer);
    for (hFUInt32 c(0); c < lExprLowerer.mExprs.size(); ++ c) {
      retVal += LowerExprToCall(lExprLowerer.mExprs[c]->mPatExpr, lExprLowerer.mExprs[c]->mFuncReplacement,
        lExprLowerer.mExprs[c]->mConvRetVal);
    } // for
  } // block
  { // Process member and subscript statements, plus logical 'and' and 'or'.
    {
      ExprCollector<IREMember> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerMember(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRESubscript> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerSubscript(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRELOr> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerLOr(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRELAnd> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerLAnd(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRERealConst> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerRealConsts(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IREPtrSub> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerPtrSub(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRENot> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerNot(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRECast> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += InsertCasts(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRECmp> lExprCollector(EVOTPost, stmt_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerCmp(lExprCollector.mCollected0[c]);
      } // for
    } // block
  } // block
  EMDEBUG1(retVal);
  return retVal;
} // ATOLowerHIR::AnatropDo

//! \todo M Design: I think we can traverse all stmts and invoke do of stmt,
//!       i.s.o repeating them in two AnatropDo funcs.
hFInt32 ATOLowerHIR::
AnatropDo(IRModule* module_) {
  BMDEBUG2("ATOLowerHIR::AnatropDo(IRModule*)", module_);
  hFInt32 retVal(0);
  { // Process switch and assignment statements.
    //! \todo M Design: Actually what we need here is to provide a filter
    //!       that will skip the assignments that are not structure typed.
    //!       provide a predicate functional object, per type or per collector?
    StmtCollector2<IRSSwitch, IRSAssign> lStmtCollector;
    module_->Accept(lStmtCollector);
    for (hFUInt32 c(0); c < lStmtCollector.mCollected0.size(); ++ c) {
      retVal += LowerSwitch(lStmtCollector.mCollected0[c]);
    } // for
    for (hFUInt32 d(0); d < lStmtCollector.mCollected1.size(); ++ d) {
      retVal += LowerStrAssign(lStmtCollector.mCollected1[d]);
    } // for
  } // block
  {
    ListIterator<IRFunction*> lFuncIter(module_->GetFuncIter());
    for (lFuncIter.First(); !lFuncIter.IsDone(); lFuncIter.Next()) {
      ExprLowerer lExprLowerer(this, lFuncIter->GetCGContext()->GetHWDesc()->GetExprsToBeLowered());
      lFuncIter->Accept(lExprLowerer);
      for (hFUInt32 c(0); c < lExprLowerer.mExprs.size(); ++ c) {
        retVal += LowerExprToCall(lExprLowerer.mExprs[c]->mPatExpr, lExprLowerer.mExprs[c]->mFuncReplacement,
            lExprLowerer.mExprs[c]->mConvRetVal);
      } // for
    } // for 
  } // block
  { // Process member and subscript statements, plus logical 'and' and 'or'.
    {
      ExprCollector<IREMember, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerMember(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRERealConst, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerRealConsts(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRESubscript, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerSubscript(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRELOr, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerLOr(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IREPtrSub, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerPtrSub(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRELAnd, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerLAnd(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRENot, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerNot(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRECast, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += InsertCasts(lExprCollector.mCollected0[c]);
      } // for
    } // block
    {
      ExprCollector<IRECmp, IRModule> lExprCollector(EVOTPost, module_);
      for (hFUInt32 c(0); c < lExprCollector.mCollected0.size(); ++ c) {
        retVal += LowerCmp(lExprCollector.mCollected0[c]);
      } // for
    } // block
  } // block
  EMDEBUG1(retVal);
  return retVal;
} // ATOLowerHIR::AnatropDo

hFInt32 ATOLowerHIR::
LowerExprToCall(IRExpr* expr_, IRFunctionDecl* func_, bool convRetVal_) {
  BMDEBUG4("ATOLowerHIR::LowerExprToCall", expr_, func_, convRetVal_);
  IRStmt* lStmt(extr.eGetStmt(expr_));
  IRBuilder lIRBuilder(extr.eGetFunc(expr_), consts.cIRBDoOptims);
  IRBuilderHelper* lIRHBuilder(lIRBuilder.GetHelper());
  IROTmp* lTmp(lIRBuilder.irboTmp(expr_->GetType(), "lowexpr"));
  IRExprList* lArgs(new IRExprList());
  //! \todo M Design: accessing to children is ugly, how can we improve it?
  //!       Perhaps iterators can help? Need to find a way.
  //! \attention The order of children is important in this code,
  //!            e.g. for Sub left must be the first child and right the
  //!            second. How can we assert this automatically?
  vector<IRExpr*> lChildren;
  expr_->GetChildren(lChildren);
  for (hFInt32 c(0); c < lChildren.size(); ++ c) {
    lArgs->AddExpr(lChildren[c]->GetRecCopyExpr());
  } // for
  // Find the function typed global object and use it otherwise create the function object.
  IRModule* lModule(extr.eGetModule(lStmt));
  IROGlobal* lFuncObj(lModule->FindGlobal(func_->GetName()));
  if (pred.pIsInvalid(lFuncObj) == true) {
    lFuncObj = lIRBuilder.irboGlobal(func_->GetName(), func_->GetType(), IRLStatic);
  } // if
  if (convRetVal_ == true) {
    IROTmp* lFuncTmp(lIRBuilder.irboTmp(func_->GetType()->GetReturnType(), "lowexpr"));
    IRSFCall* lCall(lIRBuilder.irbsFCall(lFuncTmp, lIRBuilder.irbeAddrOf(lFuncObj), lArgs,
      ICStmt(lStmt, ICBefore), SrcLoc()));
    irhBuilder->irbsAssign(lTmp, lIRBuilder.irbeCast(expr_->GetType(),
      irhBuilder->irbeUseOf(lFuncTmp)), ICStmt(lCall, ICAfter), SrcLoc());
  } else {
    /* LIE */ lIRBuilder.irbsFCall(lTmp, lIRBuilder.irbeAddrOf(lFuncObj), lArgs,
      ICStmt(lStmt, ICBefore), SrcLoc());
  } // if
  /* LIE */ expr_->ReplaceWith(irhBuilder->irbeUseOf(lTmp));
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerExprToCall

hFInt32 ATOLowerHIR::
InsertCasts(IRECast* cast_) {
  BMDEBUG2("ATOLowerHIR::InsertCast", cast_);
  const vector<InsertCastInfo*>& lCastsToBeInserted(extr.eGetCGContext(cast_)->GetHWDesc()->GetCastsToBeInserted());
  for (hFInt32 c(0); c < lCastsToBeInserted.size(); ++ c) {
    if (lCastsToBeInserted[c]->mFrom == cast_->GetExpr()->GetType()) {
      if (lCastsToBeInserted[c]->mTo == cast_->GetType()) {
        IRExpr* lInsertedCast(irBuilder->irbeCast(lCastsToBeInserted[c]->mInsert, cast_->GetExpr()->GetRecCopyExpr()));
        /* LIE */ cast_->GetExpr()->ReplaceWith(lInsertedCast);
        anatropManager->Trigger("HIRLower", extr.eGetStmt(cast_));
        break;
      } // if
    } // if
  } // for
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::InsertCasts

hFInt32 ATOLowerHIR::
LowerSwitch(IRSSwitch* switch_) {
  BMDEBUG2("ATOLowerHIR::LowerSwitch", switch_);
  IRExpr* lSwitchExpr(switch_->GetExpr());
  vector<pair<IRExprList*, IRBB*> > lCases;
  switch_->GetCases(lCases);
  ASSERTMSG(lCases.back().first->IsEmpty() == true,
    ieStrInconsistentInternalStructure << "Last case (default label) must have empty list");
  IRBB* lDefaultBB(lCases.back().second);
  IRStmt* lPrevStmt(0);
  for (hFUInt32 c(0); c+1 < lCases.size(); ++ c) {
    pair<IRExprList*, IRBB*> lCurrCase(lCases[c]);
    vector<IRExpr*> lCaseConsts;
    lCurrCase.first->GetChildren(lCaseConsts);
    IRExpr* lCondExpr(irBuilder->irbeEq(lSwitchExpr->GetRecCopyExpr(), lCaseConsts[0]->GetRecCopyExpr()));
    for (hFUInt32 d(1); d < lCaseConsts.size(); ++ d) {
      lCondExpr = irBuilder->irbeLOr(lCondExpr, irBuilder->irbeEq(lSwitchExpr->GetRecCopyExpr(), lCaseConsts[d]->GetRecCopyExpr()));
    } // for
    if (!lPrevStmt) {
      if (lCurrCase.second != lDefaultBB) {
        lPrevStmt = irBuilder->irbsIf(lCondExpr, lCurrCase.second, lDefaultBB, ICStmt(switch_, ICBefore), SrcLoc());
      } else {
        lPrevStmt = irBuilder->irbsJump(lDefaultBB, ICStmt(switch_, ICBefore), SrcLoc());
      } // if
    } else {
      lPrevStmt = irhBuilder->irbsIf(lCondExpr, lCurrCase.second->GetFirstStmt(),
        lPrevStmt, ICStmt(lPrevStmt, ICBefore), SrcLoc());
    } // if
  } // for
  if (lCases.size() == 1) {
    /* LIE */ irBuilder->irbsJump(lDefaultBB, ICStmt(switch_, ICBefore), SrcLoc());
  } // if
  irBuilder->irbRemoveStmt(switch_);
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerSwitch

// We do the lowering of relational operators in HW machines that has a single
// test result register.
// As a result of relational lowering we would like to get 'if' statements
// with a single relational operator as top node of the if statement
// conditional expressions, e.g. if (left relop right).
// We want to have this form since it makes the CG rule writing easy.
hFInt32 ATOLowerHIR::
LowerCmp(IRECmp* cmp_) {
  BMDEBUG2("ATOLowerHIR::LowerCmp", cmp_);
  REQUIREDMSG(!pred.pIsDetached(cmp_), ieStrParamValuesDBCViolation);
  hFInt32 retVal(0);
  if (!dynamic_cast<IRSIf*>(cmp_->GetParent())) {
    // stmt = ... (left ?relop? right) ... =>
    // if (left ?relop? right) {tmp = true} else {tmp = false}
    // stmt = ... (tmp) ...
    IRStmt* lStmt(extr.eGetStmt(cmp_));
    IRBuilder irBuilder(extr.eGetFunc(lStmt), consts.cIRBDoOptims);
    IRBuilderHelper* irhBuilder(irBuilder.GetHelper());
    IROTmp* lTmp(irBuilder.irboTmp(IREBoolConst::boolConstBoolType, "lowcmp"));
    IRSAssign* lTmpTrue(irhBuilder->irbsAssign(lTmp, irBuilder.irbeBoolConst(true), ICStmt(lStmt, ICBefore), SrcLoc()));
    IRSAssign* lTmpFalse(irhBuilder->irbsAssign(lTmp, irBuilder.irbeBoolConst(false), ICStmt(lStmt, ICBefore), SrcLoc()));
    /* LIE */ irhBuilder->irbsJump(lStmt, ICStmt(lTmpTrue, ICAfter), SrcLoc());
    /* LIE */ irhBuilder->irbsJump(lStmt, ICStmt(lTmpFalse, ICAfter), SrcLoc());
    IRSIf* lIf(irhBuilder->irbsIf(cmp_->GetRecCopyExpr(), lTmpTrue, lTmpFalse, ICStmt(lTmpTrue, ICBefore), SrcLoc()));
    /* LIE */ cmp_->ReplaceWith(irhBuilder->irbeUseOf(lTmp));
    anatropManager->Trigger("HIRLower", lIf);
    retVal = 1;
  } else {
    PDEBUG(GetName(), "detail", "parent of " << refcxt(cmp_) << " is 'if' statement so 'cmp' is not lowered.");
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOLowerHIR::LowerCmp

hFInt32 ATOLowerHIR::
LowerPtrSub(IREPtrSub* ptrSub_) {
  BMDEBUG2("ATOLowerHIR::LowerPtrSub", ptrSub_);
  hFInt32 retVal(0);
  // ptrLeft - ptrRight ==> lowered in to:
  // ptrLeft - ptrRight / sizeof(ptrLeft[0]) or sizeof(elemType of ptrLeft).
  IRType* lElemType(extr.eGetElemType(ptrSub_->GetLeftExpr()));
  IRTInt* lIntType(irBuilder->irbtGetInt(Target::GetTarget()->Get_ptrdiff_t()));
  IREIntConst* lIntConst(irBuilder->irbeIntConst(Target::GetTarget()->Get_ptrdiff_t(),
    lElemType->GetSize()/8));
  IRExpr* lSub(irBuilder->irbeSub(irBuilder->irbeCast(lIntType, ptrSub_->GetLeftExpr()->GetRecCopyExpr()),
    irBuilder->irbeCast(lIntType, ptrSub_->GetRightExpr()->GetRecCopyExpr())));
  /* LIE */ ptrSub_->ReplaceWith(irBuilder->irbeDiv(lSub, lIntConst));
  EMDEBUG1(retVal);
  return retVal;
} // ATOLowerHIR::LowerPtrSub

hFInt32 ATOLowerHIR::
LowerStrAssign(IRSAssign* assign_) {
  BMDEBUG2("ATOLowerHIR::LowerStrAssign", assign_);
  IRBuilder irBuilder(extr.eGetFunc(assign_), consts.cIRBDoOptims);
  hFInt32 retVal(0);
  if (IREDeref* lDeref = dynamic_cast<IREDeref*>(assign_->GetRHS())) {
    if (pred.pIsStructUnion(lDeref->GetType()) == true) {
      IRExpr* lLength(irBuilder.irbeIntConst(Target::GetTarget()->Get_size_t(),
        lDeref->GetType()->GetSize()/8));
      IRExpr* lDst(assign_->GetLHS()->GetRecCopyExpr());
      IRExpr* lSrc(lDeref->GetExpr()->GetRecCopyExpr());
      IRSCall* lMemcpy(irBuilder.irbsBIPCall(new IRExprList(lDst, lSrc, lLength), IRBIRmemcpy,
        ICStmt(assign_, ICBefore), extr.eGetSrcLoc(assign_)));
      anatropManager->Trigger("CallConv", lMemcpy);
      irBuilder.irbRemoveStmt(assign_);
      retVal = 1;
    } // if
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOLowerHIR::LowerStrAssign

hFInt32 ATOLowerHIR::
LowerLOr(IRELOr* lor_) {
  BMDEBUG2("ATOLowerHIR::LowerLOr", lor_);
  REQUIREDMSG(!pred.pIsDetached(lor_), ieStrParamValuesDBCViolation);
  // stmt = ... (left || right) ... =>
  // if (left) {tmp = true} else if (right) {tmp = true} else {tmp = false}
  // stmt = ... (tmp) ...
  IRStmt* lStmt(extr.eGetStmt(lor_));
  IRBuilder irBuilder(extr.eGetFunc(lStmt), consts.cIRBDoOptims);
  IRBuilderHelper* irhBuilder(irBuilder.GetHelper());
  IROTmp* lTmp(irBuilder.irboTmp(IREBoolConst::boolConstBoolType, "lowlor"));
  // if (left) {
  //   jump truecase;
  // } else if (right) {
  //   jump truecase;
  // } else {
  //   jump falsecase;
  // } // if
  // truecase:
  //   tmp = true;
  //   jump endif;
  // falsecase:
  //   tmp = false;
  //   jump endif;
  // endif:
  //   stmt = ...
  IRExpr* lLeft(lor_->GetLeftExpr());
  IRExpr* lRight(lor_->GetRightExpr());
  IRSAssign* lTmpTrue(irhBuilder->irbsAssign(lTmp, irBuilder.irbeBoolConst(true), ICStmt(lStmt, ICBefore), SrcLoc()));
  IRSAssign* lTmpFalse(irhBuilder->irbsAssign(lTmp, irBuilder.irbeBoolConst(false), ICStmt(lStmt, ICBefore), SrcLoc()));
  /* LIE */ irhBuilder->irbsJump(lStmt, ICStmt(lTmpTrue, ICAfter), SrcLoc());
  /* LIE */ irhBuilder->irbsJump(lStmt, ICStmt(lTmpFalse, ICAfter), SrcLoc());
  IRSIf* lRightCase(0);
  if (!pred.pIsCmp(lRight)) {
    lRightCase = irhBuilder->irbsIf(irhBuilder->irbeIsTrue(lRight->GetRecCopyExpr()),
      lTmpTrue, lTmpFalse, ICStmt(lTmpTrue, ICBefore), SrcLoc());
  } else {
    lRightCase = irhBuilder->irbsIf(lRight->GetRecCopyExpr(), lTmpTrue,
      lTmpFalse, ICStmt(lTmpTrue, ICBefore), SrcLoc());
  } // if
  if (!pred.pIsCmp(lLeft)) {
    IRSIf* lLeftCase(irhBuilder->irbsIf(irhBuilder->irbeIsTrue(lLeft->GetRecCopyExpr()),
      lTmpTrue, lRightCase, ICStmt(lRightCase, ICBefore), SrcLoc()));
  } else {
    IRSIf* lLeftCase(irhBuilder->irbsIf(lLeft->GetRecCopyExpr(), lTmpTrue, lRightCase,
      ICStmt(lRightCase, ICBefore), SrcLoc()));
  } // if
  /* LIE */ lor_->ReplaceWith(irhBuilder->irbeUseOf(lTmp));
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerLOr

hFInt32 ATOLowerHIR::
LowerLAnd(IRELAnd* land_) {
  BMDEBUG2("ATOLowerHIR::LowerLAnd", land_);
  REQUIREDMSG(!pred.pIsDetached(land_), ieStrParamValuesDBCViolation);
  // stmt = ... (left && right) ... =>
  // if (!left) {tmp = false} else if (!right) {tmp = false} else {tmp = true}
  // stmt = ... (tmp) ...
  IRStmt* lStmt(extr.eGetStmt(land_));
  IRBuilder irBuilder(extr.eGetFunc(lStmt), consts.cIRBDoOptims);
  IRBuilderHelper* irhBuilder(irBuilder.GetHelper());
  IROTmp* lTmp(irBuilder.irboTmp(IREBoolConst::boolConstBoolType, "lowand"));
  // if (!left) {
  //   jump falsecase;
  // } else if (!right) {
  //   jump falsecase;
  // } else {
  //   jump truecase;
  // } // if
  // truecase:
  //   tmp = true;
  //   jump endif;
  // falsecase:
  //   tmp = false;
  //   jump endif;
  // endif:
  //   stmt = ...
  IRExpr* lLeft(land_->GetLeftExpr());
  IRExpr* lRight(land_->GetRightExpr());
  IRSAssign* lTmpTrue(irhBuilder->irbsAssign(lTmp, irBuilder.irbeBoolConst(true), ICStmt(lStmt, ICBefore), SrcLoc()));
  IRSAssign* lTmpFalse(irhBuilder->irbsAssign(lTmp, irBuilder.irbeBoolConst(false), ICStmt(lStmt, ICBefore), SrcLoc()));
  /* LIE */ irhBuilder->irbsJump(lStmt, ICStmt(lTmpTrue, ICAfter), SrcLoc());
  /* LIE */ irhBuilder->irbsJump(lStmt, ICStmt(lTmpFalse, ICAfter), SrcLoc());
  IRSIf* lRightCase(0);
  if (!pred.pIsCmp(lRight)) {
    lRightCase = irhBuilder->irbsIf(irhBuilder->irbeIsFalse(lRight->GetRecCopyExpr()),
      lTmpFalse, lTmpTrue, ICStmt(lTmpTrue, ICBefore), SrcLoc());
  } else {
    lRightCase = irhBuilder->irbsIf(irBuilder.irbeInverseCmp(static_cast<IRECmp*>(lRight)),
      lTmpFalse, lTmpTrue, ICStmt(lTmpTrue, ICBefore), SrcLoc());
  } // if
  if (!pred.pIsCmp(lLeft)) {
    /* LIE */ irhBuilder->irbsIf(irhBuilder->irbeIsFalse(lLeft->GetRecCopyExpr()),
      lTmpFalse, lRightCase, ICStmt(lRightCase, ICBefore), SrcLoc());
  } else {
    /* LIE */ irhBuilder->irbsIf(irBuilder.irbeInverseCmp(static_cast<IRECmp*>(lLeft)),
      lTmpFalse, lRightCase, ICStmt(lRightCase, ICBefore), SrcLoc());
  } // if
  /* LIE */ land_->ReplaceWith(irhBuilder->irbeUseOf(lTmp));
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerLAnd

hFInt32 ATOLowerHIR::
LowerMember(IREMember* member_) {
  BMDEBUG2("ATOLowerHIR::LowerMember", member_);
  REQUIREDMSG(!pred.pIsDetached(member_), ieStrParamValuesDBCViolation);
  if (!pred.pIsBitField(member_->GetMember())) {
    // member_->GetBase(), member_->GetMember()
    //  -> cast_to<type of member>(ptradd(base, offset of member))
    //! \todo M Design: Use the byte size of the target instead of 8.
    IREIntConst* lIntConst(irBuilder->irbeIntConst(*Target::GetTarget()->GetIntType(32, 0, IRSUnsigned),
      member_->GetMember()->GetOffset() / 8));
    IREPtrAdd* lPtrAdd(irBuilder->irbePtrAdd(member_->GetBase()->GetRecCopyExpr(), lIntConst));
    IRExpr* lCast(irBuilder->irbeCast(irBuilder->irbtGetPtr(member_->GetMember()->GetType()), lPtrAdd));
    /* LIE */ member_->ReplaceWith(irBuilder->irbeDeref(lCast));
  } else {
    ASSERTMSG(pred.pIsAddrOf(dynamic_cast<IRExpr*>(member_->GetParent())) == true,
      ieStrInconsistentInternalStructure);
    if (!pred.pIsDeref(dynamic_cast<IRExpr*>(member_->GetParent()->GetParent()))) {
      ASSERTMSG(pred.pIsAssign(dynamic_cast<IRStmt*>(member_->GetParent()->GetParent())) == true,
        ieStrInconsistentInternalStructure <<
          "You must use address of bit-fields on the LHS of an assignment");
      // &member = rhs case, or write in to bit field case.
      extr.eGetCGContext(member_)->GetCallConv()->processWriteBitField(static_cast<IRSAssign*>(extr.eGetStmt(member_)));
    } else {
      ASSERTMSG(pred.pIsDeref(dynamic_cast<IRExpr*>(member_->GetParent()->GetParent())) == true,
        ieStrInconsistentInternalStructure << "You must use deref of addr of bit-fields on RHS expressions.");
      // *&member, extract the value of bit-field case.
      extr.eGetCGContext(member_)->GetCallConv()->processReadBitField(static_cast<IREDeref*>(member_->GetParent()->GetParent()));
    } // if
  } // if
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerMember

hFInt32 ATOLowerHIR::
LowerNot(IRENot* not_) {
  BMDEBUG2("ATOLowerHIR::LowerNot", not_);
  REQUIREDMSG(!pred.pIsDetached(not_), ieStrParamValuesDBCViolation);
  // You may not need to lower Not but then you have to implement a rule in
  // the code generator, handling it in IR would be a general way to lessen
  // the time required to write a code generator.
  //! \todo M Design: try to find more lowerings that will reduce the amount
  //!       of effort required to write a code generator, in expense of code quality.
  REQUIREDMSG(pred.pIsBool(not_->GetExpr()->GetType()) == true,
    ieStrInconsistentInternalStructure << " : IRENot may only have boolean typed operand.");
  // !(expr) => (expr == false)
  IRExpr* lNewExpr(irBuilder->irbeEq(not_->GetExpr()->GetRecCopyExpr(), irBuilder->irbeBoolConst(false)));
  /* LIE */ not_->ReplaceWith(lNewExpr);
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerNot

hFInt32 ATOLowerHIR::
LowerRealConsts(IRERealConst* realConst_) {
  BMDEBUG2("ATOLowerHIR::LowerRealConsts", realConst_);
  //! \todo M Design: Add options to select size of real type to be lowered.
  if (realConst_->GetType()->GetSize() == 64) {
    IRBuilder irBuilder(extr.eGetFunc(realConst_), consts.cIRBDoOptims);
    IROGlobal* lTmpGlobal(irBuilder.irboTmpGlobal(realConst_->GetType()));
    lTmpGlobal->SetAddress(AddrLTConst());
    irBuilder.irbSetInits(lTmpGlobal, realConst_->GetRecCopyExpr());
    /* LIE */ realConst_->ReplaceWith(irBuilder.GetHelper()->irbeUseOf(lTmpGlobal));
  } // if
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerRealConsts

hFInt32 ATOLowerHIR::
LowerSubscript(IRESubscript* subscript_) {
  BMDEBUG2("ATOLowerHIR::LowerSubscript", subscript_);
  REQUIREDMSG(!pred.pIsDetached(subscript_), ieStrParamValuesDBCViolation);
  PDEBUG(GetName(), "detail", "Stmt: " << progcxt(extr.eGetStmt(subscript_)));
  IRExpr* lBase(subscript_->GetLeftExpr());
  IRExpr* lIndex(subscript_->GetRightExpr());
  IRType* lElemType(extr.eGetElemType(lBase));
  //! \todo M Design: convert 8 to byte size of target.
  if (pred.pIsObj(lBase) == true) {
    lBase = irBuilder->irbeAddrOf(lBase);
  } // if
  IREPtrAdd* lPtrAdd(irBuilder->irbePtrAdd(lBase->GetRecCopyExpr(), irBuilder->irbeMul(lIndex->GetRecCopyExpr(),
    irBuilder->irbeIntConst(Target::GetTarget()->Get_size_t(), lElemType->GetSize() / 8))));
  /* LIE */ subscript_->ReplaceWith(irBuilder->irbeCast(lElemType, lPtrAdd));
  PDEBUG(GetName(), "detail", "ElemType: " << progcxt(lElemType));
  EMDEBUG1(1);
  return 1;
} // ATOLowerHIR::LowerSubscript



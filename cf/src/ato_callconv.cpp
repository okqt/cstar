// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __ATO_CALLCONV_HPP__
#include "ato_callconv.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ITERATORS_HPP__
#include "iterators.hpp"
#endif
#ifndef __EXTRACT_HPP__
#include "extract.hpp"
#endif
#ifndef __PREDICATES_HPP__
#include "predicates.hpp"
#endif
#ifndef __CGT_BASE_HPP__
#include "cgt_base.hpp"
#endif

hFInt32 ATOCallConv::
AnatropDo(IRStmt* stmt_) {
  BMDEBUG2("ATOCallConv::AnatropDo(IRStmt*)", stmt_);
  hFInt32 retVal(0);
  CGContext* lCGContext(extr.eGetCGContext(stmt_));
  CallingConvention* lCallConv(lCGContext->GetCallConv());
  if (IRSCall* lCall = dynamic_cast<IRSCall*>(stmt_)) {
    lCallConv->processCall(lCall, lCGContext);
    retVal ++;
  } else if (IRSReturn* lRet = dynamic_cast<IRSReturn*>(stmt_)) {
    lCallConv->processReturn(lRet, lCGContext);
    retVal ++;
  } // if
  EMDEBUG1(retVal);
  return retVal;
} // ATOCallConv::AnatropDo

hFInt32 ATOCallConv::
AnatropDo(IRModule* module_) {
  BMDEBUG2("ATOCallConv::AnatropDo(IRModule*)", module_);
  ListIterator<IRFunction*> lFuncIter(module_->GetFuncIter());
  hFInt32 retVal(0);
  for (lFuncIter.First(); !lFuncIter.IsDone(); lFuncIter.Next()) {
    CGContext* lCGContext(lFuncIter->GetCGContext());
    CallingConvention* lCallConv(lCGContext->GetCallConv());
    lCallConv->processParams(*lFuncIter, lCGContext);
    retVal ++;
    // We need to traverse all the call statements too.
    ListIterator<IRStmt*> lStmtIter(extr.eGetStmtIter(*lFuncIter));
    for (lStmtIter.First(); !lStmtIter.IsDone(); lStmtIter.Next()) {
      if (pred.pIsCall(*lStmtIter) == true) {
        lCallConv->processCall(static_cast<IRSCall*>(*lStmtIter), lCGContext);
        retVal ++;
      } else if (pred.pIsReturn(*lStmtIter) == true) {
        lCallConv->processReturn(static_cast<IRSReturn*>(*lStmtIter), lCGContext);
        retVal ++;
      } // if
    } // for
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOCallConv::AnatropDo



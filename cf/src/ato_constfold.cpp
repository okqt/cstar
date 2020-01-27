// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __ATO_CONSTFOLD_HPP__
#include "ato_constfold.hpp"
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

hFInt32 ATOConstFold::
AnatropDo(IRStmt* stmt_) {
  BMDEBUG2("ATOConstFold::AnatropDo(IRStmt*)", stmt_);
  hFUInt32 retVal(0);
  ExprCollector<IREConst> lConstExprs(EVOTPost, stmt_);
  for (hFUInt32 c(0); c < lConstExprs.size(); ++ c) {
    if (pred.pIsAttached(lConstExprs[c]) == true) {
      IRExpr* lToBeFolded(extr.eGetParentExpr(lConstExprs[c]));
      if (!lToBeFolded) {
        IRStmt* lParentStmt(lConstExprs[c]->GetParentStmt());
        if (pred.pIsIf(lParentStmt) == true) {
          anatropManager->Trigger("IfSimplifications", stmt_);
        } // if
        //! \todo M Design: we can also trigger procedure cloning if args are consts.
      } else {
        IRExpr* lFold;
        while (lToBeFolded != 0 && lToBeFolded->constFold(lFold) == true) {
          STATRECORD("constfold: count");
          lToBeFolded->ReplaceWith(lFold);
          lToBeFolded = extr.eGetParentExpr(lFold);
          ++ retVal;
        } // while
      } // if
    } // if
  } // for
  EMDEBUG1(retVal);
  return retVal;
} // ATOConstFold::AnatropDo


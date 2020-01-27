// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __DEBUGINFO_HPP__
#define __DEBUGINFO_HPP__

#ifndef __HOSTTARGETTYPES_HPP__
#include "hosttargettypes.hpp"
#endif

//! \file debuginfo.hpp
//! \brief This file contains the generic debug information generation interface class definitions.

//! \brief Base class for program items that has debug information entry.
//! IR constructs has a DIEBase typed variable attached to them.
//! Usually it is not possible to complete debug information in one
//! pass. Types must be created during AST2HIR, low level information
//! must be completed after code generation.
class DIEBase : public IDebuggable {
protected:
  DIEBase(Invalid<DIEBase>* inv_) {}
public:
  DIEBase() {}
  DIEBase(const SrcLoc& srcLoc_) :
    mSrcLoc(srcLoc_)
  {
  } // DIEBase::DIEBase
  DIEBase(const DIEBase& base_) :
    mSrcLoc(base_.mSrcLoc)
  {
  } // DIEBase::DIEBase
  virtual ~DIEBase() {}
  virtual DIEBase* Clone() const {return new DIEBase(*this);}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  void SetSrcLoc(const SrcLoc& srcLoc_) {mSrcLoc = srcLoc_;}
  const SrcLoc& GetSrcLoc() const {return mSrcLoc;}
private:
  SrcLoc mSrcLoc;
}; // class DIEBase

class DIEBuilder : public IDebuggable {
public:
  DIEBuilder() {}
  virtual ~DIEBuilder() {}
  virtual DIEBase* CreateProgDIE(IRProgram* prog_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DIEBuilder::CreateProgDIE
  virtual DIEBase* CreateModuleDIE(IRModule* module_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DIEBuilder::CreateModuleDIE
  virtual DIEBase* CreateFuncDIE(IRFunction* func_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DIEBuilder::CreateFuncDIE
  virtual DIEBase* CreateStmtDIE(IRStmt* stmt_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DIEBuilder::CreateStmtDIE
  virtual DIEBase* CreateBBDIE(IRBB* bb_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DIEBuilder::CreateBBDIE
  virtual DIEBase* CreateObjDIE(IRObject* obj_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
    return 0;
  } // DIEBuilder::CreateObjDIE
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class DIEBuilder

#endif // __DEBUGINFO_HPP__


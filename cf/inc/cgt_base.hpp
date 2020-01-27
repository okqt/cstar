// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __CGT_BASE_HPP__
#define __CGT_BASE_HPP__

//! \file cgt_base.hpp
//! \brief Code generator base class and utilities header file.

//! \todo H Design: I think we do not need NTIntConst and NTRealConst non-terminal types.
//! \todo M Design: Check that every class has an InvalidObj invalid version.

#include <float.h>

#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif
#ifndef __IRS_HPP__
#include "irs.hpp"
#endif
#ifndef __IRBUILDER_HPP__
#include "irbuilder.hpp"
#endif

ostream& operator << (ostream& o, ShiftType st_);
ostream& operator << (ostream& o, const RegSet& regSet_);
ostream& operator << (ostream& o, const VirtReg& virtReg_);
ostream& operator << (ostream& o, const HWReg& hwReg_);

//! \brief Node information for pattern trees.
struct NodeInfo {
  //! \param [in] irExpr_ May be null.
  NodeInfo(IRExpr* irExpr_, NonTerminal* nt_);
public: // Member data
  IRExpr* mIRExpr;     //!< Matched IR expression node, may be null.
  NonTerminal* mNT;    //!< Non-terminal at the node.
}; // struct NodeInfo

//! \brief HW register base class.
//! \todo H Design: Should we make registers pointer only identifiable
//!       or make them have unique ids? It would be possible to give different
//!       names to registers in different contexts, this is quite minor
//!       improvement however we could copy/assign registers objects, store
//!       them in standard containers, etc. The draw back is it might be
//!       slower?
//! \todo M Design: Have a look at the debug/trace dump.
class Register : public IDebuggable {
protected:
  Register(const Invalid<Register>* inv_) {}
  Register(const string& name_, hFUInt32 id_);
public:
  Register(const string& name_);
  Register(const Register& reg_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const {
    NOTIMPLEMENTED();
    return "";
  } // Register::debug
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) {
    o.getAsm() << GetName().c_str();
    return o;
  } // Register::GetDisassembly
  virtual string GetName() const {
    ENSUREMSG(mName != "", ieStrParamValuesDBCViolation);
    return mName;
  } // Register::GetName
  //! \brief Registers can be identified by this value.
  hFUInt32 GetId() const {return mId;}
  bool operator == (const Register& reg_) const {
    return mId == reg_.mId;
  } // Register::operator ==
private: // Static data members
  static hFUInt32 sIdIndex;
private: // Data members
  string mName; //!< Same register may have different names.
  hFUInt32 mId; //!< Identifies this register.
  //! \brief Instructions that 'defines' this register.
  //! Set of instructions that stores a value in this register.
  //! This variable is used to reduce the time-complexity of
  //! interference graph building.
  set<Instruction*> mDefInsts;
private:
  template<class P, class D> friend class DFA;
  friend class Predicate;
}; // class Register

class RegSet : public IDebuggable {
public:
  RegSet();
  RegSet(Register* reg_);
  RegSet(const RegSet& regSet_);
  RegSet(const vector<HWReg*>& regs_);
  RegSet(const vector<VirtReg*>& regs_);
  RegSet(const vector<Register*>& regs_);
  RegSet& Insert(Register* reg_);
  RegSet& Insert(const RegSet& regSet_);
  const set<Register*>& GetRegs() const {return mRegs;}
  void GetRegs(set<Register*>& regs_) const {regs_ = mRegs;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  bool IsEmpty() const {return mRegs.empty() == true;}
  void Clear() {mRegs.clear();}
  //! \brief Returns one of the registers in the set.
  //! \pre IsEmpty must be false.
  Register* GetAny() const {
    REQUIREDMSG(IsEmpty() == false, ieStrParamValuesDBCViolation);
    return *mRegs.begin();
  } // RegSet::GetAny
  hFUInt32 GetSize() const {return mRegs.size();}
  void Remove(const RegSet& regSet_);
  bool HasReg(const Register* reg_) const {return mRegs.find(const_cast<Register*>(reg_)) != mRegs.end();}
  //! \pre \p reg_ must be present in the set.
  RegSet& operator -= (const Register* reg_);
  RegSet& operator |= (Register* reg_);
public: // Member data
  set<Register*> mRegs;
private:
  friend class Extract;
  friend class Predicate;
}; // class RegSet

// --------------------------
// Operators
// --------------------------
RegSet operator | (Register& left_, Register& right_);
RegSet operator | (Register& left_, const RegSet& right_);
RegSet operator | (const RegSet& left_, Register& right_);
RegSet operator | (const RegSet& left_, const RegSet& right_);

//! \brief Code generator basic block class.
class CGBB : public IRBB {
protected:
  CGBB(const Invalid<CGBB>* inv_) : IRBB(&Invalid<IRBB>::Obj()) {}
public:
  CGBB(CFG* parent_) :
    IRBB(parent_)
  {
  } // CGBB::CGBB
  void AddInst(AsmSequenceItem* inst_);
  void GetInsts(vector<AsmSequenceItem*>& insts_) const {
    insts_ = mInsts;
  } // CGBB::GetInsts
private:
  vector<AsmSequenceItem*> mInsts; //!< Instructions attached to this basic block.
private:
  friend class IRBB;
  template<class P, class D> friend class DFA;
};// class CGBB

class HWReg : public Register {
protected:
  HWReg(const Invalid<HWReg>* inv_) : Register(&Invalid<Register>::Obj()) {}
private:
  HWReg(const string& name_, hFUInt32 id_) :
    Register(name_, id_)
  {
  } // HWReg::HWReg
public:
  HWReg(const string& name_) :
    Register(name_)
  {
  } // HWReg::HWReg
  //! \brief Creates a clone of this hardware register with a different name.
  HWReg* Clone(const string& name_) const {
    return new HWReg(name_, GetId());
  } // HWReg::Clone
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  friend class Predicate;
}; // class HWReg

class VirtReg : public Register {
protected:
  VirtReg(const Invalid<VirtReg>* inv_) : Register(&Invalid<Register>::Obj()) {}
public:
  VirtReg(const VirtReg& vReg_);
  VirtReg();
  //! \brief Creates a virtual register with the given name.
  VirtReg(const string& name_);
  //! \brief Creates a virtual register with a pre assigned HW register.
  VirtReg(HWReg* preSetHWReg_);
public:
  virtual string GetName() const;
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_);
  //! \pre \p hwReg_ May not be null. But it may be Invalid.
  void SetHWReg(HWReg* hwReg_);
  //! \return May return Invalid HWReg.
  HWReg* GetHWReg() const;
  const vector<RegAllocRule*>& GetRegAllocRules() const {return mRegAllocRules;}
  virtual VirtReg* Clone() const;
  void ReplaceRegAllocRuleReg(const VirtReg* vRegOld_, VirtReg* vRegNew_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  HWReg* mHWReg; //!< Get assigned hw register, it is Invalid if not assigned.
  //! \brief Assigned adjacency matrix index, assigned during register allocation.
  //! This value may change during the life time of this object.
  hFSInt32 mAdjMatrixIndex;
  //! \note Every virtual register that is involved in a register allocation rule
  //!       has this list of rules. This means same rules may be present in
  //!       different virtual registers.
  vector<RegAllocRule*> mRegAllocRules;
private:
  friend class RegAllocRule;
  friend class Predicate;
  friend class RegisterAllocator;
  template<class P, class D> friend class DFA;
}; // class VirtReg

class GenInstOp : public IDebuggable {
public:
  GenInstOp(const char* what_);
  GenInstOp(const string& what_);
  GenInstOp(const RegSet& regSet_);
  GenInstOp(Register* reg_);
  GenInstOp(const ConditionNT& condNT_);
  GenInstOp(const RegisterNT& regNT_);
  GenInstOp(CNTType condType_);
  GenInstOp(ShiftType shiftType_);
  GenInstOp(const tBigInt& int_);
  GenInstOp(const GenInstOp& genOp_) : 
    mKind(genOp_.mKind),
    mValue(genOp_.mValue)
  {
  } // GenInstOp::GenInstOp
public:
  void operator = (const GenInstOp& genOp_) {
    mKind = genOp_.mKind;
    mValue = genOp_.mValue;
  } // GenInstOp::operator =
public:
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  bool IsShift() const {return mKind == GIO_Shift;}
  bool IsCondition() const {return mKind == GIO_Cond;}
  bool IsString() const {return mKind == GIO_String;}
  bool IsRegSet() const {return mKind == GIO_RegSet;}
  bool IsReg() const {return mKind == GIO_Register;}
  bool IsInt() const {return mKind == GIO_Int;}
  ShiftType GetShift() const {
    REQUIREDMSG(IsShift() == true, ieStrParamValuesDBCViolation);
    return mValue.mShift;
  } // GenInstOp::GetShift
  CNTType GetCondition() const {
    REQUIREDMSG(IsCondition() == true, ieStrParamValuesDBCViolation);
    return mValue.mCondition;
  } // GenInstOp::GetCondition
  const string& GetString() const {
    REQUIREDMSG(IsString() == true, ieStrParamValuesDBCViolation);
    return *mValue.mString;
  } // GenInstOp::GetString
  const tBigInt& GetInt() const {
    REQUIREDMSG(IsInt() == true, ieStrParamValuesDBCViolation);
    return *mValue.mInt;
  } // GenInstOp::GetInt
  Register* GetReg() const {
    REQUIREDMSG(IsReg() == true, ieStrParamValuesDBCViolation);
    return mValue.mReg;
  } // GenInstOp::GetReg
  const RegSet& GetRegSet() const {
    REQUIREDMSG(IsRegSet() == true, ieStrParamValuesDBCViolation);
    return *mValue.mRegSet;
  } // GenInstOp::GetRegSet
  string GetDisassembly(const ASMDesc& asmDesc_) const;
private:
  enum {GIO_Invalid, GIO_Register, GIO_String, GIO_Shift, GIO_Cond, GIO_RegSet, GIO_Int} mKind;
  union {
    tBigInt* mInt;
    RegSet* mRegSet;
    Register* mReg;
    string* mString; //!< This may be a register name or a flag name.
    CNTType mCondition;
    ShiftType mShift;
  } mValue;
private:
  friend class InstructionSet::InstructionSetDefinition;
}; // class GenInstOp

// --------------------------
// NonTerminals
// --------------------------

//! \brief Base class for nonterminals.
//! \todo M Design: Having a ruleId field in the non terminal would
//!       speed up the compilation time. The issue is, when non-terminals
//!       are shared by leaf/result merge the ruleId is confusing.
//!       resultNT ids are always zero, pattern tree ids are not. Still
//!       should be done this way to speed up.
class NonTerminal : public IRPatExpr {
protected:
  NonTerminal(const Invalid<NonTerminal>* inv_) : IRPatExpr(&Invalid<IRPatExpr>::Obj()) {}
public:
  NonTerminal(IRType* type_ = ptrToVoidType) :
    IRExpr(type_)
  {
  } // NonTerminal::NonTerminal
  virtual ~NonTerminal() {}
  virtual NonTerminal* Clone() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // NonTerminal::Clone
  virtual NTType GetNTType() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return NTTInvalid;
  } // NonTerminal::GetNTType
  //! \brief Must return all the virtual registers of the nonterminal.
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // NonTerminal::GetVRegs
  //! \brief Should merge the HWRegs used in \p nt in to this NonTerminal.
  //! \note It is possible that there are conflicting cases, i.e.
  //!       both have an assigned HWReg for a given virtual register.
  //!       In this case function must return false. (In this case
  //!       a copy statement must be inserted, by the caller.)
  //! \attention type of \p nt must match to the type of this class.
  //! \param unmerged_ must be empty.
  virtual bool MergeHWRegs(const NonTerminal& nt_, vector<pair<VirtReg*, VirtReg*> >& unmerged_);
  NonTerminal& operator = (const NonTerminal& nt) {
    return *this;
  } // NonTerminal::operator =
  virtual bool Accept(ExprVisitor& visitor_) {
    BMDEBUG1("NonTerminal::Accept");
    bool retVal(visitor_.Visit(this));
    EMDEBUG1(retVal);
    return retVal;
  } // NonTerminal::Accept
public:  // IRExpr interfaces
  virtual bool GetChildren(vector<IRPatExpr*>& children_) const {
    REQUIREDMSG(children_.empty() == true, ieStrParamValuesDBCViolation);
    return false;
  } // NonTerminal::GetChildren
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual IRExpr* GetRecCopy() const {
    return Clone();
  } // NonTerminal::GetRecCopy
  virtual NonTerminal* SetNewVirtRegs() {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // NonTerminal::SetNewVirtRegs
private:
  static IRTPtr* ptrToVoidType;
private:
  friend class HWDescription;
}; // class NonTerminal

//! \brief Causes expression trees that corresponds to ignore NT nodes are not generated.
class IgnoreNT : public NonTerminal {
public:
  IgnoreNT() {}
  virtual ~IgnoreNT() {}
  virtual NonTerminal* Clone() const {
    return new IgnoreNT();
  } // IgnoreNT::Clone
  virtual NTType GetNTType() const {return NTTIgnore;}
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual NonTerminal* SetNewVirtRegs() {return this;}
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) {/* LIE */}
}; // class IgnoreNT

//! \brief Register nonterminal base class.
class RegisterNT : public NonTerminal {
public:
  RegisterNT();
  RegisterNT(VirtReg* reg_);
  virtual ~RegisterNT() {}
public:
  virtual NonTerminal* Clone() const {
    return new RegisterNT(mReg);
  } // RegisterNT::Clone
  virtual NTType GetNTType() const {return NTTRegister;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  operator VirtReg*() const {return mReg;}
  Register* GetReg() const {return mReg;}
  VirtReg* GetVReg() const {
    REQUIREDMSG(dynamic_cast<VirtReg*>(mReg) != 0, ieStrInconsistentInternalStructure);
    return static_cast<VirtReg*>(mReg);
  } // RegisterNT::GetVReg
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {
    mReg = mReg->Clone();
    return this;
  } // RegisterNT::SetNewVirtRegs
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {
    vRegs_.push_back(mReg);
    return;
  } // RegisterNT::GetVRegs
private:
  VirtReg* mReg;
}; // class RegisterNT

//! \brief Shift operation nonterminal base class.
class ShiftOpNT : public NonTerminal {
public:
  ShiftOpNT(const ShiftOpNT& shiftOpNT_);
  ShiftOpNT();
  ShiftOpNT(VirtReg* reg_, tBigInt shiftAmount_, ShiftType shiftType_);
  ShiftOpNT(VirtReg* reg_, VirtReg* shiftAmount_, ShiftType shiftType_);
  virtual ~ShiftOpNT() {}
  virtual NonTerminal* Clone() const {
    return new ShiftOpNT(*this);
  } // ShiftOpNT::Clone
  ShiftOpNT& operator = (const ShiftOpNT& nt_);
  virtual NTType GetNTType() const {return NTTShiftOp;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  Register* GetReg() const {return mReg;}
  Register* GetShiftReg() const {return mRegShiftAmount;}
  tBigInt GetShiftAmount() const {return mIntShiftAmount;}
  ShiftType GetShiftType() const {return mShiftType;}
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {
    mReg = mReg->Clone();
    mRegShiftAmount = mRegShiftAmount->Clone();
    return this;
  } // ShiftOpNT::SetNewVirtRegs
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {
    vRegs_.push_back(mReg);
    vRegs_.push_back(mRegShiftAmount);
    return;
  } // ShiftOpNT::GetVRegs
private:
  VirtReg* mReg;
  tBigInt mIntShiftAmount; //!< Has no special value but use -1 for non-invalid regShiftAmount.
  VirtReg* mRegShiftAmount; //!< valid/invalid specifies if amount is an int or reg shift.
  ShiftType mShiftType;
}; // class ShiftOpNT

//! \brief Memory nonterminal base class.
//! A Non-Terminal with register and offset fields, used to address memory.
class MemoryNT : public NonTerminal {
public:
  MemoryNT();
  MemoryNT(VirtReg* reg_, tBigInt offset_);
  virtual ~MemoryNT() {}
  virtual NonTerminal* Clone() const {
    return new MemoryNT(mReg, mOffset);
  } // MemoryNT::Clone
  virtual NTType GetNTType() const {return NTTMemory;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {
    mReg = mReg->Clone();
    return this;
  } // MemoryNT::SetNewVirtRegs
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {
    vRegs_.push_back(mReg);
    return;
  } // MemoryNT::GetVRegs
private:
  VirtReg* mReg;
  tBigInt mOffset;
}; // class MemoryNT

class RealConstNT : public NonTerminal {
public:
  RealConstNT();
  RealConstNT(tBigReal value_);
  virtual ~RealConstNT() {}
  virtual NonTerminal* Clone() const {
    return new RealConstNT(value);
  } // RealConstNT::Clone
  virtual NTType GetNTType() const {return NTTRealConst;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {/* LIE */ return this;}
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {/* LIE */}
private:
  tBigReal value;
}; // class RealConstNT

class IntConstNT : public NonTerminal {
public:
  IntConstNT();
  IntConstNT(tBigInt value_);
  virtual ~IntConstNT() {}
  virtual NonTerminal* Clone() const {
    return new IntConstNT(value);
  } // IntConstNT::Clone
  virtual NTType GetNTType() const {return NTTIntConst;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {/* LIE */ return this;}
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {/* LIE */}
private:
  tBigInt value;
}; // class IntConstNT

//! \brief Represents any non-terminal.
//! It is basically used to get an expression value in to any location, e.g. IRSEval.
class AnyNT : public NonTerminal {
public:
  AnyNT() :
    NonTerminal()
  {
  } // AnyNT::AnyNT
  virtual ~AnyNT() {}
  virtual NonTerminal* Clone() const {
    return new AnyNT();
  } // AnyNT::Clone
  virtual NTType GetNTType() const {return NTTAny;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {/* LIE */ return this;}
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {/* LIE */}
}; // class AnyNT

//! \brief Represents IRENull match.
class NullNT : public NonTerminal {
public:
  NullNT() :
    NonTerminal()
  {
  } // NullNT::NullNT
  virtual ~NullNT() {}
  virtual NonTerminal* Clone() const {
    return new NullNT();
  } // NullNT::Clone
  virtual NTType GetNTType() const {return NTTNull;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {/* LIE */ return this;}
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {/* LIE */}
}; // class NullNT

class ConditionNT : public NonTerminal {
public:
  ConditionNT(const ConditionNT& nt_) :
    NonTerminal(nt_),
    mCondition(nt_.mCondition)
  {
  } // ConditionNT::ConditionNT
  ConditionNT();
  ConditionNT(CNTType condition_);
  ConditionNT& operator = (const ConditionNT& condNT_);
  virtual ~ConditionNT() {}
  static CNTType GetCondition(const IRECmp* cmp_);
  virtual NonTerminal* Clone() const {return new ConditionNT(*this);}
  virtual NTType GetNTType() const {return NTTCondition;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  CNTType GetCondition() const {return mCondition;}
  ConditionNT GetInverted() const;
  virtual bool Accept(ExprVisitor& visitor_) {return visitor_.Visit(this);}
  virtual NonTerminal* SetNewVirtRegs() {/* LIE */ return this;}
  virtual void GetVRegs(vector<VirtReg*>& vRegs_) const {/* LIE */}
private:
  CNTType mCondition;
}; // class ConditionNT

// --------------------------
// Instructions
// --------------------------
//! \todo H Design: Instruction data structures are designed by looking in to
//!       only ARM architecture, we need to generalize them by considering more archs.

//! \brief Base class for items that can be generated during OnCover.
//! \note The offset of item is intentionally not put in to the interface
//!       since offsets may not be efficiently handled. Offset should be
//!       handled by Extract.
class AsmSequenceItem : public IDebuggable {
public:
  AsmSequenceItem(const AsmSequenceItem& asmSeqItem_) :
    mLength(asmSeqItem_.mLength),
    mParent(&Invalid<IRBB>::Obj())
  {
  } // AsmSequenceItem::AsmSequenceItem
  AsmSequenceItem(const tBigInt& length_) :
    mLength(length_),
    mParent(&Invalid<IRBB>::Obj())
  {
  } // AsmSequenceItem::AsmSequenceItem
  virtual char* debug(const DTContext& context_, ostream& toStr_) const = 0;
  virtual bool IsValid() const = 0;
  //! \brief Emits the attached comment of this item.
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const;
  //! \brief Length of this assembly sequence item in bytes.
  tBigInt GetLength() const {return mLength;}
  void AppendComment(const string& comment_) {mComment += comment_;}
  void SetComment(const string& comment_) {mComment = comment_;}
  const string& GetComment() const {return mComment;}
  IRBB* GetBB() const {return mParent;}
  const AddrLTConst& GetAddr(CGFuncContext& context_);
  const AddrLTConst& GetAddr() const {return mAddr;}
private:
  tBigInt mLength;
  //! \brief Any string that can be attached as comment to this item.
  string mComment;
  AddrLTConst mAddr;
  IRBB* mParent;
private:
  friend class CGBB;
}; // class AsmSequenceItem

//! \brief A block of data items with a label.
//! \todo Add string literal support.
//! Data items can be compile time constants or relocations.
//! \par What is a relocation?
//! A relocation is an expression whose numeric value is determined by
//! linker/loader at link time. Sometimes compilers may find out the value of
//! relocations, e.g. difference of two local labels (but still they are
//! emitted as relocations). Relocations makes
//! assembly code generation easier for compilers by using the names of
//! objects, labels -symbols.
//! \li symbol, a simple relocation is a label, typically for a global variable
//!     symbol.
//! \li symbol+offset, is another typical relocation, used in literal pools.
//! \li symbol-symbol, is used to represent code size, data size. You can
//!     evaluate the size of functions with this relocation.
//! Relocations are massively used when debug information is generated.
//!
//! \par How the data value is represented?
//! The value is an expression that is made up of relocation
//! symbols and constants. While these arithmetic expressions must
//! conform to the types of involved operands, the whole expression
//! must be type casted to the desired data width. The casting is
//! necessary if top level type is not already have the desired type size.
class AsmSeqData : public AsmSequenceItem {
public:
  //! \brief Constructs an empty data list.
  AsmSeqData(AsmSeqLabel* label_);
  //! \brief Constructs and adds the relocation value to the data list.
  AsmSeqData(AsmSeqLabel* label_, IRRelocExpr* value_);
  //! \brief Constructs and adds the relocation value to the data list.
  AsmSeqData(AsmSeqLabel* label_, IRRelocExpr* value_, const tBigInt& length_);
  //! \brief Adds the data to the list, uses the type size of supplied expression.
  //! \pre \p value_ must be non-null.
  AsmSeqData& Add(IRRelocExpr* value_);
  //! \brief Adds the data to the list but also inserts a type cast
  //!        with a type that is specified bytes long.
  //! \param value_ The value of data.
  //! \param length_ Size in bytes of the value, a type cast is created
  //!        by using this parameter.
  //! \pre \p value_ must be non-null.
  //! \pre \p length_ must be greater than zero.
  AsmSeqData& Add(IRRelocExpr* value_, const tBigInt& length_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual bool IsValid() const {return true;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const;
  AsmSeqLabel* GetLabel() const {return mLabel;}
private:
  vector<IRRelocExpr*> mValue;
  AsmSeqLabel* mLabel;
}; // class AsmSeqData

class AsmSeqDirective : public AsmSequenceItem {
public:
  AsmSeqDirective() : AsmSequenceItem(0) {}
  virtual ~AsmSeqDirective() {}
  virtual bool IsValid() const {return true;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class AsmSeqDirective

class AsmSeqDirSection : public AsmSeqDirective {
public:
  AsmSeqDirSection(AsmSectionType sectionType_, const string& customName_=string()) :
    mSectionType(sectionType_),
    mCustomName(customName_)
  {
    REQUIREDMSG(sectionType_ != ASTCustom || !customName_.empty(), ieStrParamValuesDBCViolation);
  } // AsmSeqDirSection::AsmSeqDirSection
  AsmSectionType GetSectionType() const {return mSectionType;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
    asmDesc_.PutSection(o, mSectionType, mCustomName);
    return o;
  } // AsmSeqDirSection::GetDisassembly
private:
  AsmSectionType mSectionType;
  string mCustomName;
}; // class AsmSeqDirSection

class AsmSeqDirAlign : public AsmSeqDirective {
public:
  AsmSeqDirAlign(const tBigInt& alignBytes_) :
    mAlignBytes(alignBytes_)
  {
    BMDEBUG2("AsmSeqDirAlign::AsmSeqDirAlign", &alignBytes_);
    EMDEBUG0();
  } // AsmSeqDirAlign::AsmSeqDirAlign
  tBigInt GetAlignment() const {return mAlignBytes;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
    BMDEBUG1("AsmSeqDirAlign::GetDisassembly");
    asmDesc_.PutAlign(o, mAlignBytes);
    EMDEBUG0();
    return o;
  } // AsmSeqDirAlign::GetDisassembly
private:
  tBigInt mAlignBytes;
}; // class AsmSeqDirAlign

//! \brief Emits the provided text as it is.
//! \note The directive must have no compiler related effect.
class AsmSeqDirInfo : public AsmSeqDirective {
public:
  AsmSeqDirInfo(const string& asmText_) :
    mAsmText(asmText_)
  {
    BMDEBUG2("AsmSeqDirInfo::AsmSeqDirInfo", asmText_);
    EMDEBUG0();
  } // AsmSeqDirInfo::AsmSeqDirInfo
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
    o.getAsm() << mAsmText;
    return o;
  } // AsmSeqDirInfo
private:
  string mAsmText;
}; // class AsmSeqDirInfo

class AsmSeqLabel : public AsmSequenceItem {
public:
  AsmSeqLabel(const string& label_) :
    AsmSequenceItem(0),
    mLabel(label_)
  {
    BMDEBUG2("AsmSeqLabel::AsmSeqLabel", label_);
    EMDEBUG0();
  } // AsmSeqLabel::AsmSeqLabel
  virtual bool IsValid() const {return true;}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  string GetLabel() const {return mLabel;}
  operator string() const {return mLabel;}
private:
  string mLabel;
}; // class AsmSeqLabel

class Instruction : public AsmSequenceItem {
protected:
  Instruction(const tBigInt& length_, const string& disassembly_, const DefRegSet& defs_, const UseRegSet& uses_, bool ignoreDefsAndUses_);
  Instruction(const tBigInt& length_, const string& disassembly_, const DefRegSet& defs_, const UseRegSet& uses_, 
    const vector<GenInstOp>& operands_, bool ignoreDefsAndUses_);
  Instruction(const Instruction& inst_);
  virtual ~Instruction() {/* LIE */}
public:
  const GenInstOp& GetOperand(hFSInt32 opIndex_) const {
    REQUIREDMSG(opIndex_ >= 0 && opIndex_ < mOperands.size(), ieStrParamValuesDBCViolation);
    return mOperands[opIndex_];
  } // Instruction::GetOperand
  //! \brief Clears out the uses and defs of this instruction.
  //! You can use this to mark instructions that need not be considered in
  //! register allocation.
  void IgnoreDefsAndUses() {mUses.Clear(); mDefs.Clear();}
  virtual bool IsValid() const {return true;}
public: // AsmSequenceItem interface
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  Instruction* AddUses(const RegSet& regSet_) {mUses.Insert(regSet_); return this;}
  Instruction* AddDefs(const RegSet& regSet_) {mDefs.Insert(regSet_); return this;}
  const RegSet& GetDefs() const {return mDefs;}
  const RegSet& GetUses() const {return mUses;}
  const string& GetFormatStr() const {return mDisassembly;}
protected:
  vector<GenInstOp> mOperands;
  //! \brief Definitions of this instruction.
  RegSet mDefs;
  //! \brief Uses of this instruction.
  RegSet mUses;
  string mDisassembly; //!< Provided to constructors.
private:
  friend class InstructionSet::InstructionSetDefinition;
}; // class Instruction

//! \brief Holds the defined registers for functions with parameters that comes
//! in registers.
class InstFuncEnter : public Instruction {
public:
  InstFuncEnter(const RegSet& argDefs_) : Instruction(0, "\t", argDefs_, UseRegSet(), false) {/* LIE */}
  virtual ASMSection& GetDisassembly(ASMSection& o, const ASMDesc& asmDesc_) const {
    asmDesc_.PutCommentStr(o);
    o.getAsm() << "func_enter";
    Instruction::GetDisassembly(o, asmDesc_);
    return o;
  } // InstFuncEnter::GetDisassembly
  virtual ~InstFuncEnter() {/* LIE */}
  virtual bool IsValid() const {return true;}
}; // class InstFuncEnter

//! \brief Disassembles a given relocation expression.
//! Relocation expressions can only use a subset of IR expressions.
//! Therefore, just define the visitor functions that are valid
//! in a relocation expression.
class RelocExprDisassemblerVisitor : public CExprVisitor {
public:
  RelocExprDisassemblerVisitor(ostream& o, const ASMDesc& asmDesc_) :
    CExprVisitor(ExprVisitor::cMayNotVisit, EVOTPre),
    mOStream(o),
    mASMDesc(asmDesc_)
  {
  } // RelocExprDisassemblerVisitor::RelocExprDisassemblerVisitor
  virtual ~RelocExprDisassemblerVisitor() {}
  virtual bool Visit(const IREAdd* expr_) const;
  virtual bool Visit(const IREANeg* expr_) const;
  virtual bool Visit(const IRECast* expr_) const;
  virtual bool Visit(const IREIntConst* expr_) const;
  virtual bool Visit(const IRORelocSymbol* obj_) const;
  virtual bool Visit(const IRERealConst* expr_) const;
  virtual bool Visit(const IRESub* expr_) const;
  virtual bool Visit(const IREStrConst* expr_) const;
  virtual bool Visit(const IREAddrConst* expr_) const;
private:
  ostream& mOStream;
  const ASMDesc& mASMDesc;
}; // class RelocExprDisassemblerVisitor

//! \attention RegAllocRule's must insert them selves in to the virtual
//!            registers involved in the rule.
class RegAllocRule : public IDebuggable {
public:
  RegAllocRule() {}
  virtual ~RegAllocRule() {}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Must return true if given HWReg can be allocated to the specified virtual register.
  //! \note It is guaranteed that the provided vReg_ is involved in this rule.
  virtual bool CanHWRegBeAllocated(HWReg* hwReg_, const VirtReg* vReg_, const RegSet& availHWRegs_) const = 0;
  //! \brief Must change the specification by replacing the old vreg with the new one.
  virtual void ReplaceVRegs(const VirtReg* vRegOld_, VirtReg* vRegNew_) = 0;
  //! \brief Replaces the virtual register with the given name with a new register.
  virtual void ReplaceVReg(const string& name_, Register* regNew_) = 0;
  virtual RegAllocRule* Clone() const = 0;
protected:
  void AddRegAllocRule(const vector<VirtReg*>& vRegs_);
  void AddRegAllocRule(const RegSet& vRegs_);
  void AddRegAllocRule(VirtReg* vReg_);
}; // class RegAllocRule

//! \brief Specifices that all the provided virtual registers
//!        must be assigned to different hardware registers.
class RARMustNotBeSame : public RegAllocRule {
public:
  RARMustNotBeSame(const RARMustNotBeSame& rule_) :
    mRegs(rule_.mRegs)
  {
  } // RARMustNotBeSame::RARMustNotBeSame
  RARMustNotBeSame(const RegSet& regs_);
  virtual ~RARMustNotBeSame() {}
public:
  const RegSet& GetRegs() const {return mRegs;}
  virtual void ReplaceVRegs(const VirtReg* vRegOld_, VirtReg* vRegNew_);
  virtual void ReplaceVReg(const string& name_, Register* regNew_);
  virtual bool CanHWRegBeAllocated(HWReg* hwReg_, const VirtReg* vReg_, const RegSet& availHWRegs_) const;
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual RegAllocRule* Clone() const {return new RARMustNotBeSame(*this);}
private:
  RegSet mRegs;
}; // class RARMustNotBeSame

//! \brief For a specified vector of vregs, one of given list of
//!        hwreg vectors can be allocated.
//! \note Since for some machines list of registers are in small amounts
//!       it is not expensive to go thru a list of possible assignments.
//!       For example, for ARM HW double precision real values must be
//!       in reven-rodd pairs or {r0-r1,r2-r3,r4-r5,r6-r7,r8-r9,r10-r11,r12-r13}.
//!       So the given virtual pair can only be assigned to these HW regs.
class RARMustBeInList : public RegAllocRule {
public:
  RARMustBeInList(const RARMustBeInList& raRule_) :
    mVRegs(raRule_.mVRegs),
    mValidVectors(raRule_.mValidVectors)
  {
  } // RARMustBeInList::RARMustBeInList
  RARMustBeInList(const vector<VirtReg*>& vregs_, const vector<vector<HWReg*> >* validVectors_);
  virtual bool CanHWRegBeAllocated(HWReg* hwReg_, const VirtReg* vReg_, const RegSet& availHWRegs_) const;
  virtual void ReplaceVRegs(const VirtReg* vRegOld_, VirtReg* vRegNew_);
  virtual void ReplaceVReg(const string& name_, Register* regNew_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  virtual RegAllocRule* Clone() const {return new RARMustBeInList(*this);}
private:
  vector<VirtReg*> mVRegs;
  const vector<vector<HWReg*> >* mValidVectors;
}; // class RARMustBeInList

class RARBuilder {
public:
  RARBuilder(InstructionSetDefinition* isaDef_) :
    mISADef(isaDef_)
  {
    REQUIREDMSG(isaDef_ != 0, ieStrParamValuesDBCViolation);
  } // RARBuilder::RARBuilder
  RARMustNotBeSame* GetRARMustNotBeSame(const char* reg0_, ...);
  virtual ~RARBuilder() {}
private:
  InstructionSetDefinition* mISADef;
}; // class RARBuilder

// --------------------------
// Rule classes
// --------------------------

//! \brief Base class for rules.
class Rule : public IDebuggable {
protected:
  Rule(const Invalid<Rule>* inv_) {}
  static const hFInt32 cMaxPatternNTs = 16;
public:
  Rule(const Rule& rule_);
  Rule(const string& name_, HWDescription* hwDescription_);
  //! \brief The cost of this rule.
  virtual hReal32 GetCost() {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return -1.0;
  } // Rule::GetCost
  HWDescription* GetHWDesc() const {return mHWDescription;}
  const string& GetName() const {return mName;}
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  void ReplaceNT(NonTerminal* old_, NonTerminal* new_);
  hFInt32 GetNTIndex(const NonTerminal* nt_) const;
  void AddInst(CGFuncContext& context_, AsmSequenceItem* inst_);
public: // Instruction creation functions
  Instruction* AddInst(CGFuncContext& context_, const string& instId_);
  Instruction* AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_);
  Instruction* AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, const GenInstOp& op1_);
  Instruction* AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_);
  Instruction* AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_);
  Instruction* AddInst(CGFuncContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_, const GenInstOp& op4_);
  static Instruction* CreateInst(CGContext& context_, const string& instId_);
  static Instruction* CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_);
  static Instruction* CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, const GenInstOp& op1_);
  static Instruction* CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_);
  static Instruction* CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_);
  static Instruction* CreateInst(CGContext& context_, const string& instId_, const GenInstOp& op0_, 
    const GenInstOp& op1_, const GenInstOp& op2_, const GenInstOp& op3_, const GenInstOp& op4_);
public: // Expression pattern building helpers
  static IRPatExpr* ireBAnd(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireNot(IRPatExpr* expr_);
  static IRPatExpr* ireCast(IRPatExpr* expr_);
  static IRPatExpr* ireBNeg(IRPatExpr* expr_);
  static IRPatExpr* ireANeg(IRPatExpr* expr_);
  static IRPatExpr* ireBXor(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireBOr(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireLShiftRight(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireAShiftRight(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireShiftLeft(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* irePtrAdd(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireAdd(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireMul(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireSub(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireDiv(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireMod(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireRem(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireQuo(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireCmp(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireGe(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireGt(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireLe(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireLt(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireEq(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireNe(IRPatExpr* left_, IRPatExpr* right_);
  static IRPatExpr* ireIntConst();
  static IRPatExpr* ireAddrConst();
  static IRPatExpr* ireBoolConst();
  static IRPatExpr* ireStrConst();
  static IRPatExpr* ireNull();
  static IRPatExpr* ireRealConst();
  static IRPatExpr* ireDeref(IRPatExpr* expr_);
  static IRPatExpr* ireAddrOf(IRPatExpr* expr_);
  static IRPatExpr* iroObject();
  static IRPatExpr* iroGlobal();
  static IRPatExpr* iroTmp();
  static IRPatExpr* iroLocal();
  static IRPatExpr* iroParam();
  // NonTerminals
  //! \brief Creates a register non-terminal.
  IgnoreNT* ignoreNT(hFInt32 ntIdInRule_);
  RegisterNT* regNT(hFInt32 ntIdInRule_);
  RegisterNT* regNT(hFInt32 ntIdInRule_, VirtReg* virtReg_);
  ShiftOpNT* shiftOpNT(hFInt32 ntIdInRule_);
  MemoryNT* memNT(hFInt32 ntIdInRule_);
  IntConstNT* intNT(hFInt32 ntIdInRule_);
  IntConstNT* intNT(hFInt32 ntIdInRule_, const tBigInt& value_);
  RealConstNT* realNT(hFInt32 ntIdInRule_);
  RealConstNT* realNT(hFInt32 ntIdInRule_, const tBigReal& value_);
  NullNT* nullNT(hFInt32 ntIdInRule_);
  ConditionNT* condNT(hFInt32 ntIdInRule_);
  ConditionNT* condNT(hFInt32 ntIdInRule_, CNTType cond_);
  AnyNT* anyNT(hFInt32 ntIdInRule_);
  RegSet getUseVRegSet(NonTerminal* nt0_) {
    vector<VirtReg*> lVRegs;
    nt0_->GetVRegs(lVRegs);
    return RegSet(lVRegs);
  }
  RegSet getUseVRegSet(NonTerminal* nt0_, NonTerminal* nt1_) {
    return getUseVRegSet(nt0_) | getUseVRegSet(nt1_);
  }
  RegSet getUseVRegSet(NonTerminal* nt0_, NonTerminal* nt1_, NonTerminal* nt2_) {
    return getUseVRegSet(nt0_) | getUseVRegSet(nt1_, nt2_);
  }
  RegSet getUseVRegSet(NonTerminal* nt0_, NonTerminal* nt1_, NonTerminal* nt2_, NonTerminal* nt3_) {
    return getUseVRegSet(nt0_) | getUseVRegSet(nt1_, nt2_, nt3_);
  }
protected: // Member data
  static IRBuilder* smIRBuilder;
protected: // Member functions
  //! \brief Returns the non-terminal specified in the pattern tree.
  //! \pre \p ntIndex must be positive and less than or equal to the
  //!      biggest index used in the rule's pattern tree.
  //! \pre Pattern type specified by index \p ntIndex must be RegisterNT type.
  RegisterNT* GetRegNT(hFInt32 ntIndex_);
  ShiftOpNT* GetShiftOpNT(hFInt32 ntIndex_);
  ConditionNT* GetCondNT(hFInt32 ntIndex_);
  AnyNT* GetAnyNT(hFInt32 ntIndex_);
  //! \brief Sets the non-terminal at given index to a new non-terminal.
  //! \pre NT at ntIndex_ must be of type AnyNT.
  //! \note OnPatternMatch is a common place to set concrete non-terminals for AnyNTs.
  void SetNT(hFInt32 ntIndex_, NonTerminal* newNT_);
private: // Member functions
  //! \brief This function is called during the cover, to merge the
  //!        result non-terminals with pattern non-terminals.
  //! In most cases result non-terminal of one rule is leaf non-terminal rule of another
  //! rule. The result non-terminal therefore should be the same as the leaf
  //! non-terminals. In static rule-pattern definitions of rules this is not
  //! the case naturally. We can merge the non-terminals when minimal cost
  //! matches are calculated.
  void setPatternNT(hFUInt32 patternIndex_, NonTerminal* newNT_);
protected:
  //! \brief List of non-terminals specified in the pattern tree.
  //! Ids that are not used are null pointers.
  vector<NonTerminal*> mPatternNTs;
private:
  string mName;
  HWDescription* mHWDescription;
private:
  friend class HWDescription;
}; // class Rule

class ExprRule : public Rule {
protected:
  ExprRule(const Invalid<ExprRule>* inv_) : Rule(&Invalid<Rule>::Obj()) {}
public:
  //! \param name_ A short name for the rule.
  //! \param hwDesc_ \todo doc.
  ExprRule(const string& name_, HWDescription* hwDesc_);
  ExprRule(const ExprRule& aRule_);
  //! \pre Result pattern non-terminal id must always be 0.
  //! \pre exprPattern_ must start at 1, and must be consecutive.
  void SetExprPattern(NonTerminal* result_, IRPatExpr* exprPattern_);
  //! \brief Returns the top level IRPatExpr pattern node.
  IRPatExpr* GetPatRoot() const {return mExprPattern;}
  bool IsExprPatternMatch(IRExpr* candidIRExpr_);
  //! \brief Extra condition on top of pattern match.
  //! \return You should return false if you do not want a match even pattern
  //!         of the rule matches.
  //! \attention You may only make use of \p matchedExpr during the evaluation
  //!            of condition. You should not use the non-terminals in the
  //!            pattern and result.
  virtual bool CheckCondition(const IRExpr* matchedExpr_) {return true;}
  //! \brief Called when rule pattern is matched to an IR expression and
  //!        CheckCondition() is true.
  //! \note In this function you may -typically- want to decorate the rule according to the
  //!       matched IR expression. For example, if \a result non-terminal
  //!       is of type IntConstNT we would set the value from the matched IREIntConst.
  //! \attention Member \p irExpr is not yet valid.
  //! \post Member \p result non-terminal must be in valid state.
  //! \attention \p matchedExpr is made on purpose constant. You are not
  //!            expected to modify the matched IRExpr, however you can modify
  //!            the rule's result non-terminal data. Actually this is the
  //!            only place that you should modify the result non-terminal.
  //!            Because the result non-terminal of this rule will be
  //!            propagated to and used in the leaves of the parent node.
  //!            The result non-terminal will be propagated to the parent
  //!            leaves once the rule is calculated to be the minimal match.
  //! \todo H Design: Do we really need to wait for the minimal match for
  //!       propagation of result NT? Can we do the propagation during the match, so
  //!       that the parent node's CheckCondition can make use of the leaf
  //!       nonterminals? -> NO, we cannot propagate it during match, i.e.
  //!       CheckCondition may not access to leaves. This would increase the
  //!       computational complexity of the matcher, we need to call checkcondition
  //!       for each leaf and for all rules of that leaf. We can provide such
  //!       a matcher algorithm later. However, I do not think that having such a
  //!       condition check would improve anything.
  virtual void OnPatternMatch(const IRExpr* matchedExpr_) {
    /* LIE */
    return;
  } // ExprRule::OnPatternMatch
  IRExpr* GetMatchedExpr() const {return mIRExpr;}
  NonTerminal* GetResultNT() const {return mResultNT;}
  void SetMatchedExpr(IRExpr* matchedIRExpr_) {mIRExpr = matchedIRExpr_;}
  //! \brief Returns the leaf ir nodes of the rule corresponding to a given IR node.
  //!
  //! If the rule has a binary tree pattern it returns the operands of
  //! \p irTopNode (which must have exactly two children nodes).
  //! \return Returns true if rule has a leaf node.
  //! \pre \p leaves must be empty.
  //! \param [in] irTopNode_ May be null.
  //! \param leaves_ \todo doc.
  bool GetIRLeaves(IRExpr* irTopNode_, vector<NodeInfo>& leaves_) const;
  //! \param [in] irNode_ may be null.
  //! \param patNode_ \todo doc.
  //! \param leaves_ \todo doc.
  static void sGetIRLeaves(IRExpr* irNode_, IRPatExpr* patNode_, vector<NodeInfo>& leaves_);
  //! \brief Returns true if a pattern expression node matches to a given ir expression node.
  static bool sIsExprPatternMatch(IRExpr* irExpr_, IRPatExpr* patExpr_);
  //! \brief OnCover is called when the match is okay and the rule has the minimum cost.
  //! \param [in] irExpr_ The matched expression.
  //! \param [out] funcContext_ Generated instructions, may be none.
  //! You should start generating the code for the rule in this function.
  //! \pre Incoming \p insts may not be empty, you should insert new
  //!      instructions at the end of incoming \p insts.
  virtual void OnCover(IRExpr* irExpr_, CGFuncContext& funcContext_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
  } // ExprRule::OnCover
  virtual ExprRule* Clone() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // ExprRule::Clone
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
protected:
private:
  static bool smatchExprNode(IRExpr* irExpr_, IRPatExpr* patExpr_);
  //! \brief Pattern to be matched against IR expressions.
  IRPatExpr* mExprPattern;
  //! \brief IR expression that is matched to pattern exprPattern.
  //! \todo M Design: Rename ExprRule::irExpr to matchedExpr.
  IRExpr* mIRExpr;
  NonTerminal* mResultNT; //!< Result non-terminal, valid after OnPatternMatch is called.
  //! \todo H Design: Use preprocessor conditionals for dbc... variables in
  //!       classes.
  bool dbcIsResultNTValid; //!< When true \p resultNT is valid.
  bool dbcIsIRExprValid; //!< When true \p irExpr is valid.
}; // class ExprRule

//! \note TroubleShooting: Check that your derived class copy constructor is handling
//!       All the member data.
class StmtRule : public Rule {
protected:
  StmtRule(const Invalid<StmtRule>* inv_) : Rule(&Invalid<Rule>::Obj()) {}
public:
  StmtRule(const StmtRule& aStmtRule_);
  StmtRule(const string& name_, HWDescription* hwDesc_);
  void SetStmtPattern(IRStmt* stmtPattern_);
  IRStmt* GetStmtPattern() const;
  IRStmt* GetMatchedStmt() const;
  void SetMatchedStmt(IRStmt* matchedStmt_) {mIRStmt = matchedStmt_;}
  bool IsStmtPatternMatch(IRStmt* candidIRStmt_);
  //! \brief Extra condition on top of pattern match.
  //! \return You should return false if you do not want a match even pattern
  //!         of the rule matches.
  virtual bool CheckCondition(const IRStmt* matchedStmt_) {return true;}
  //! \brief Statements have default cost of 10.
  virtual hReal32 GetCost() {
    return 10.0;
  } // StmtRule::GetCost
  //! \brief Called when rule pattern is matched to an IR and CheckCondition() is true.
  //! \note In most cases you do nothing for statement rules.
  //! \attention Member \p irStmt is not yet valid.
  //! \todo H Design: Should this be called after cost is also taken in to account?
  virtual void OnPatternMatch(IRStmt* matchedIR_) {
    /* LIE */
    return;
  } // StmtRule::OnPatternMatch
  //! \param [in] irTopNode_ may be null.
  //! \param leaves_ \todo doc.
  bool GetIRLeaves(IRStmt* irTopNode_, vector<NodeInfo>& leaves_) const;
  //! \brief OnCover is called when there match is okay and the rule has the minimum cost.
  //! You should start generating the code for the rule in this function.
  virtual void OnCover(IRStmt* irStmt_, CGFuncContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
  } // StmtRule::OnCover
  virtual StmtRule* Clone() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return 0;
  } // StmtRule::Clone
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  static IRSIf* irsIf(ConditionNT* cond_);
  static IRSEval* irsEval(IRPatExpr* expr_);
  static IRSJump* irsJump();
  static IRSNull* irsNull();
  static IRSLabel* irsLabel();
  static IRSPCall* irsPCall(IRPatExpr* routine_);
  static IRSFCall* irsFCall(IRPatExpr* routine_);
  //! \brief Returns a builtin routine call pattern.
  static IRSBuiltInCall* irsBICall(IRPatExpr* routine_);
  static IRSAssign* irsAssign(IRPatExpr* left_, IRPatExpr* right_);
  static IRSReturn* irsReturn(IRPatExpr* ret_);
  static IRSProlog* irsProlog();
  static IRSEpilog* irsEpilog();
  void diAddCallFrameInst(IRFunction* func_, CFInst* inst_);
private:
  IRStmt* mStmtPattern; //!< IR pattern to be matched against IR statements.
  //! \brief Pointer to the matched IR statement.
  //!
  //! Points to a valid matched statement, or to IRStmtInvalid if there is no match.
  //! \todo M Design: Rename StmtRule::irStmt to matchedStmt.
  IRStmt* mIRStmt;
}; // class StmtRule

class StmtPrologEpilogRule : public StmtRule {
public:
  StmtPrologEpilogRule(const StmtPrologEpilogRule& stmtRule_);
  StmtPrologEpilogRule(const string& name_, HWDescription* hwDesc_);
  //! \brief Prolog and Epilog rules often needs calling conventions, so
  //!        captured OnCover is translated it to this function.
  virtual void OnCover(IRStmt* irStmt_, CGFuncContext& context_, const CallingConvention* callConv_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
  } // StmtPrologEpilogRule::OnCover
private:
  //! \brief Translates OnCover in to a call with additional calling convention parameter.
  //! \pre Do not override this function in the derived rule classes.
  virtual void OnCover(IRStmt* irStmt_, CGFuncContext& context_);
}; // class StmtPrologEpilogRule

//! \brief Matcher information per IRExpr node.
//! \todo H Design: Chain conversion rules are not yet supported.
struct IRExprNodeMatcherInfo {
  IRExprNodeMatcherInfo();
  //! \brief Cost of rule for each non-terminal type.
  //! \note Each element of \a costVector is the cost of the rule plus cost of converting the
  //!       resultNT in to each non-terminal type plus the costs of the operand rules.
  //! Example: costVector[1] corresponds to having the result in a RegisterNT,
  //!       cost[2] corresponds to having the result in a MemoryNT, and so on.
  //!       If resultNT is of type MemoryNT, we need to add the cost of converting a
  //!       RegisterNT in to a MemoryNT, so cost[1] += Rule(RegisterNT->MemoryNT).
  //!       For cost[2], we do not need any conversion.
  //! \note costVector[NTTLast] is the minimum cost element.
  pair<hReal32, ExprRule*> mCostVector[NTTLast+1];
  //! \brief Rules that convert the result non-terminal of matchedRules[n]
  //!        in to the non-terminal type at index \a n (one of the NTType with value \a n).
  //! ntConvRule[n] may be null if matchedRules[n] result NT is already
  //! of NTType type with value \a n.
  //! \todo M Design: Find a much different name than HWDescription::mNTConvRules.
  ExprRule* mNTConvRule[NTTLast+1];
  //! \brief List of all rules that matches to the IRExpr node.
  vector<ExprRule*> mMatchedRules;
public:
  ExprRule* GetConvRule(NTType ntType_) const {
    return mNTConvRule[ntType_];
  } // IRExprNodeMatcherInfo::GetConvRule
  ExprRule* GetExprRule(NTType ntType_) const {
    return mCostVector[ntType_].second;
  } // IRExprNodeMatcherInfo::GetExprRule
  hReal32 GetCost(NTType ntType_) const {
    return mCostVector[ntType_].first;
  } // IRExprNodeMatcherInfo::GetCost
  void SetCostInfo(NTType ntType_, hReal32 cost_, ExprRule* exprRule_, ExprRule* convRule_);
public:
  //! \return May return \a null if conversion was not needed.
  ExprRule* GetMinCostConvRule(NTType ntType_) const {
    return mNTConvRule[ntType_];
  } // IRExprNodeMatcherInfo::GetMinCostConvRule
  ExprRule* GetMinCostRule(NTType ntType_) const {
    return mCostVector[ntType_].second;
  } // IRExprNodeMatcherInfo::GetMinCostRule
public:
  //! \brief Nonterminal prototypes
  static NonTerminal* mNTPrototypes[NTTLast];
}; // struct IRExprNodeMatcherInfo

//! \todo M Design: I am not sure if it is a good idea to initialize Invalid
//!       to -1, all bits set?
typedef enum {
  CGFInvalid    = -1,       //!< Invalid
  CGFLocalRA    = 1 << 0,   //!< Generate code so that local register allocator is sufficient.
} CodeGenFlags;

//! \brief Context for code genereation, it holds in/output information.
//! \note Do not place the calling convention object in to this class.
//!       Because, calling convention is related to only prolog/epilog.
class CGContext : public IDebuggable {
protected:
  CGContext(const Invalid<CGContext>* inv_) {}
public: // Member functions
  CGContext(HWDescription* hwDesc_, CallingConvention* callConv_, RegisterAllocator* defaultRA_, ASMDesc* asmDesc_);
  CGContext(const CGContext& context_);
  virtual ~CGContext() {}
public: // Member function
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  HWDescription* GetHWDesc() const {return mHWDesc;}
  Target* GetTarget() const;
  ASMDesc* GetASMDesc() const {return mASMDesc;}
  CallingConvention* GetCallConv() const {return mCallConv;}
  //! \todo M Design: perhaps we do not need this function.
  hFUInt32 GetNextLabelIndex(const string& prefix_ = string()) {
    static map<string, hFInt32> slIndex;
    return slIndex[prefix_] ++;
  } // CGContext::GetNextLabelIndex
  RegisterAllocator* GetDefaultRegAllocator() const {return mDefaultRegAllocator;}
  string GetALocalLabel(const string& prefix_ = string()) {return mASMDesc->GetLocalLabel(prefix_, GetNextLabelIndex(prefix_));}
  AsmSeqLabel* GenAsmSeqLabel() {
    return new AsmSeqLabel(mASMDesc->GetLocalLabel("", GetNextLabelIndex()));
  } // CGContext::GenAsmSeqLabel
  //! \param [in] where_ Can be one of {"ProgBeg", "ModuleBeg", "ProgEnd", "ModuleEnd"}.
  //! \param [in] section_ Can be "data" or "text", "dir". "dir" is used for
  //!             directives.
  //! \param inst_ \todo doc.
  //! \note Instructions are emitted in order: ProgBeg, ModuleBeg, ModuleEnd, ProgEnd.
  //! \note This function is to be used in IRProgram, IRModule generation contexts.
  CGContext& AddInst(const string& where_, const string& section_, AsmSequenceItem* inst_, const string& comment_=string());
  //! \brief Adds a separately generated function context.
  //! \note If you add a function context you must add all the
  //!       functions. If no function is added GenerateModule,
  //!       shall call GenerateFunc and add the function contexts.
  CGContext& AddFuncContext(CGFuncContext* context_) {
    mFuncContexts.push_back(context_);
    return *this;
  } // CGContext::AddFuncContext
  CodeGenFlags GetCGFlags() const {return mCGFlags;}
  void SetDefFuncContext(CGFuncContext* defFuncContext_) {
    REQUIREDMSG(defFuncContext_ != 0, ieStrNonNullParam);
    mDefFuncContext = defFuncContext_;
    return;
  } // CGContext::SetDefFuncContext
  CGFuncContext* GetDefFuncContext() const {
    return mDefFuncContext;
  } // CGContext::GetDefFuncContext
private: // Member data
  //! \brief keeps the block of instructions that makes the whole program.
  map<string, vector<AsmSequenceItem*> > mInsts;
  vector<CGFuncContext*> mFuncContexts;
  CGFuncContext* mDefFuncContext;
  //! \brief Keeps the labeling index for assembly level labels.
  hFUInt32 mASMLabelIndex;
  //! \brief The hardware for which this code will be generated.
  HWDescription* mHWDesc;
  CallingConvention* mCallConv;
  RegisterAllocator* mDefaultRegAllocator; //!< Default register allocator for this context.
  ASMDesc* mASMDesc;
  CodeGenFlags mCGFlags;
private:
  friend class HWDescription;
}; // class CGContext

//! \brief Context to be used for function generation.
class CGFuncContext : public CGContext {
protected:
  CGFuncContext(const Invalid<CGFuncContext>* inv_) : CGContext(&Invalid<CGContext>::Obj()) {}
public: // Member functions
  CGFuncContext(const CGContext& cgContext_, IRFunction* func_, RegisterAllocator* regAllocator_, bool mustMatch_,
    bool stackGrowsDownwards_);
  CGFuncContext(const CGContext& cgContext_, RegisterAllocator* regAllocator_, bool mustMatch_, bool stackGrowsDownwards_);
  CGFuncContext(const CGFuncContext& cgContext_, IRFunction* func_);
  CGFuncContext(const CGFuncContext& cgContext_);
  virtual ~CGFuncContext() {}
public:
  CGFuncContext& AddInst(AsmSequenceItem* inst_, const string& comment_);
  CGFuncContext& AddInst(AsmSequenceItem* inst_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  bool IsMustMatch() const {return mMustMatch;}
  bool IsStackGrowsDownwards() const {return mStackGrowsDownwards;}
  HWRegSet GetAvailableRegs() const {return mAvailableRegs;}
public: // Member functions
  void GetInstructions(vector<AsmSequenceItem*>& insts_) {insts_ = mInsts;}
  IRFunction* GetFunc() const {return mFunc;}
  AsmSeqLabel* GetLabelOf(const IRBB* bb_);
  RegisterAllocator* GetRegisterAllocator() const {return mRegAllocator;}
  //! \param numInsts_ Returns a previous instruction. If zero returns the
  //!        last instruction.
  //! \pre numInsts_ must be 0 or one less than the size of number of
  //!        instructions.
  AsmSequenceItem* GetPrevInst(hFSInt32 numInsts_) const {
    REQUIREDMSG(util.uInRange(numInsts_, 0, mInsts.size()-1), ieStrParamValuesDBCViolation);
    return mInsts[mInsts.size() - 1 - numInsts_];
  } // CGFuncContext::GetPrevInst
private: // Member data
  //! \brief Contains the instructions generated.
  vector<AsmSequenceItem*> mInsts;
  RegisterAllocator* mRegAllocator;
  //! \brief Registers that are available for register allocation.
  //! Register allocators use this variable as scratch variable, they may
  //! insert and remove registers.
  //! \note It is initialized at ctor to all available regs obtained from HWDescription.
  RegSet mAvailableRegs;
  map<const IRBB*, AsmSeqLabel*> mBBLabels;
  //! \brief When set to \a true, any code generation failure stops the compilation.
  bool mMustMatch;
  //! \brief Set to \a true when one or more IR constructs could not be generated.
  //! \note \p mustMatch should be \a false for \p isFailed to be meaningful.
  //!       \p isFailed is still set to \a true, when \p mustMatch is \a true.
  bool mIsFailed;
  //! \brief Indicates that stack grows downwards.
  bool mStackGrowsDownwards;
  IRFunction* mFunc;
  //! \brief Map that holds the match information of all IRExpr.
  map<IRExpr*, IRExprNodeMatcherInfo*> mIRNodeMatchInfo;
private:
  friend class HWDescription;
  friend class CGContext;
  friend class RegisterAllocator;
  friend const char* dumpMatchInfo(const DTContext& context_, const IRExpr* expr_);
}; // class CGFuncContext

//! \brief Base class of calling conventions.
//! \note All derived class calling conventions will be Singletons.
//!
//! Calling convention object is used to assign hw addresses
//! to parameters and return values of a given function or call
//! statement.
//!
//! Calling convention is the property of function type. Think of
//! a call through function pointers. Call statement would only
//! know the type of the function called.
//!
//! \note Note that calling conventions are independent from
//! optimizations and debug information.
//!
//! \par Handling return statement in rules and in calling convention, why?
//! Calling convention conceptually specifies where in the HW the
//! parameters and return values should be stored. Therefore,
//! we will do the handling of values passed in calling convention
//! functions. In the rules, we will generate instructions as much
//! uniformly as possible. We could have handled calling convention in the
//! rules also. But this is inferior to handling them separately
//! in a calling convention class. In the rules, we will just
//! emit the instructions, regardless of the calling convention
//! knowledge.
//!
//! \par Example handling of return statement.
//! In ARM architecture we need to place return value in r0 if
//! it is fundamental data type (not a strut, float, int, etc.)
//! and less than or equal to 4 bytes. As one way of implementing it,
//! in the calling convention function, we will create a local variable
//! with name "$ret" (this will allow us to debug what is returned in
//! the debuggers) and type equal to the function return type. For "$ret"
//! in the calling convention we will set the Address as HWReg r0.
//! And create an assignment "$ret = return expression". In the
//! IRSReturn rule we would only need to process return instruction:
//! which is either a jump to epilog or direct epilog emission.
//! In other word in the rule we do not need to handle the return
//! expression, return expression would be handled by an assign rule.
class CallingConvention : public IDebuggable {
public:
  CallingConvention(const string name_) :
    mName(name_)
  {
  } // CallingConvention::CallingConvention
  virtual ~CallingConvention() {}
public:
  //! \brief Must assign \a Address'es to parameters of \p func.
  virtual void processParams(IRFunction* func_, CGContext* cgContext_) const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  } // CallingConvention::processParams
  //! \brief Must assign Address'es to the arguments passed, and to the
  //!        returned in variable.
  virtual void processCall(IRSCall* callStmt_, CGContext* cgContext_) const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  } // CallingConvention::processCall
  //! \brief Must assign the Address of return value.
  virtual void processReturn(IRSReturn* retStmt_, CGContext* cgContext_) const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  } // CallingConvention::processReturn
  //! \note assign_ => addr of (IREMember) = RHS.
  virtual void processWriteBitField(IRSAssign* assign_) const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  } // CallingConvention::processReadBitField
  //! \note deref_ => deref(addr of(IREMember))
  virtual void processReadBitField(IREDeref* deref_) const {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  } // CallingConvention::processReadBitField
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private: // Member data
  string mName;
}; // class CallingConvention

//! \brief Specified that the calling convention is the default one.
class CallConvDefault : public CallingConvention {
public:
  CallConvDefault() : CallingConvention("ccDefault") {}
  virtual ~CallConvDefault() {}
}; // class CallConvDefault

class LowerExprInfo {
public:
  LowerExprInfo(const LowerExprInfo& lowerExprInfo_) :
    mPatExpr(lowerExprInfo_.mPatExpr),
    mFuncReplacement(lowerExprInfo_.mFuncReplacement),
    mConvRetVal(lowerExprInfo_.mConvRetVal)
  {
  } // LowerExprInfo::LowerExprInfo
  LowerExprInfo(IRPatExpr* patExpr_, IRFunctionDecl* funcReplacement_, bool convRetVal_) :
    mPatExpr(patExpr_),
    mFuncReplacement(funcReplacement_),
    mConvRetVal(convRetVal_)
  {
  } // LowerExprInfo::LowerExprInfo
public:
  IRPatExpr* mPatExpr;
  IRFunctionDecl* mFuncReplacement;
  bool mConvRetVal;
}; // class LowerExprInfo

//! \brief Holds the information of cast that should be inserted between a cast.
//! Example: 'char'->'double' is a cast, from 'char' to 'double'. When
//! insert cast is 'int', a new cast is added 'char'->'int', 'int'->'double',
//! old one is modified.
//!
//! It is quite wasteful to define cast from all types in to all types, e.g.
//! N->M requires N*M casting hardware description rules. An intermediate
//! cast would reduce this, N->1, 1->M, N + M rules are required. This
//! intermediate cast is 'int' in C/C++.
//! Front-ends usually handle this intermediate cast. However, in the back-end
//! optimizations and other transformations may not consider it, so at the end
//! of HIR, just before code generation we insert these casts.
class InsertCastInfo {
public:
  InsertCastInfo(const InsertCastInfo& ici_) :
    mFrom(ici_.mFrom),
    mTo(ici_.mTo),
    mInsert(ici_.mInsert)
  {
  } // InsertCastInfo::InsertCastInfo
  InsertCastInfo(IRType* from_, IRType* to_, IRType* insert_) :
    mFrom(from_),
    mTo(to_),
    mInsert(insert_)
  {
  } // InsertCastInfo::InsertCastInfo
public:
  IRType* mFrom;
  IRType* mTo;
  IRType* mInsert;
}; // class InsertCastInfo

//! \todo M Design: mustMatch is not fully utilized, e.g. coverTree.
//! \attention All HWDescription derived classes should be Singletons.
class HWDescription : public IDebuggable {
public:
  HWDescription(const string& name_, Target* target_);
public:
  void AddRule(Rule* aRule_);
  //! \brief The final code emission, it emits the generated instructions.
  //! \param [in] o Output stream where the disassembly will be put.
  void Emit(ostream& o, CGContext& context_);
  bool GenerateProg(IRProgram* prog_, CGContext& context_);
  bool GenerateModule(IRModule* module_, CGContext& context_);
  //! \brief Generates the function IR statements.
  //! \return May return \a false if statements may not be generated due to unmatched IR.
  bool GenerateFunc(IRFunction* func_, CGFuncContext& context_);
  //! \brief This function is called during the code generation.
  //! The basic blocks are generated in physical order. When
  //! a new basic block is encountered this function is called.
  //!
  //! HWDescription::OnBBStart puts the label of basic block as
  //! the default action. You may override this function.
  //! \note Basic blocks are targets of control flow statements,
  //!       therefore you need to emit a label at the beginning of basic
  //!       blocks. You may do this label emitting in this function.
  //! \note Sets the mCurrBB.
  virtual void OnBBStart(IRBB* bb_, CGFuncContext& context_);
  //! \brief Must generate the asm sequence directive for the definition of a global.
  virtual void OnGlobal(IROGlobal* global_, CGContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnGlobal
  //! \brief Must generate the directives at the beginning of program start.
  virtual void OnProgStart(IRProgram* prog_, CGContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnProgStart
  virtual void OnProgEnd(IRProgram* prog_, CGContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnProgEnd
  virtual void OnStmtStart(IRStmt* stmt_, CGFuncContext& context_);
  virtual void OnStmtEnd(IRStmt* stmt_, CGFuncContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnStmtEnd
  virtual void OnModuleStart(IRModule* module_, CGContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnModuleStart
  virtual void OnModuleEnd(IRModule* module_, CGContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnModuleEnd
  //! \brief Called at the beginning of function emission.
  //! \attention You should call this function within your derived class implementation.
  //! \note This function calls the OnParam and the OnLocal.
  virtual void OnFuncStart(IRFunction* func_, CGFuncContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnFuncStart
  virtual void OnLocal(IRFunction* func_, IROLocal* local_, CGFuncContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnLocal
  virtual void OnParam(IRFunction* func_, IROParameter* param_, CGFuncContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return;
  } // HWDescription::OnParam
  virtual void OnFuncEnd(IRFunction* func_, CGFuncContext& context_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Must return the frame pointer register of HW.
  virtual HWReg* GetFPReg() const = 0;
  //! \brief Must return the Link register of HW (where the return address of calls are stored)
  virtual HWReg* GetLRReg() const = 0;
  //! \brief Must return the stack pointer of HW.
  virtual HWReg* GetSPReg() const = 0;
  //! \brief Must return the program counter of HW.
  virtual HWReg* GetPCReg() const = 0;
  virtual void InitializeRules() = 0;
  void Initialize() {
    BMDEBUG1("HWDescription::Initialize");
    AddHWRegs();
    InitializeRules();
    EMDEBUG0();
    return;
  } // HWDescription::Initialize
  const string& GetName() const {return mName;}
  //! \brief Must return the registers that are available for register allocation.
  virtual const RegSet& GetRegsForRegAlloc() const = 0;
  virtual void AssignLTConstAddresses(const IRModule* module_, CGContext& context_) = 0;
  //! \brief Must add the hw registers.
  virtual void AddHWRegs() = 0;
  HWReg* GetHWReg(const string& name_) const {
    REQUIREDMSG(mHWRegs.empty() == false, ieStrParamValuesDBCViolation);
    REQUIREDMSG(mHWRegs.find(name_) != mHWRegs.end(), ieStrParamValuesDBCViolation << " : " << name_);
    map<string, HWReg*>::const_iterator lIt(mHWRegs.find(name_));
    ENSUREMSG(lIt->second != 0, ieStrMustEnsureNonNull);
    return lIt->second;
  } // HWDescription::GetHWReg
  InstructionSetDefinition* GetISADef() const {
    REQUIREDMSG(mISADef != 0, ieStrParamValuesDBCViolation);
    return mISADef;
  } // HWDescription::GetISADef
  void SetInstSetDefinition(InstructionSetDefinition* isaDef_) {
    REQUIREDMSG(isaDef_ != 0, ieStrParamValuesDBCViolation);
    mISADef = isaDef_;
  } // HWDescription::SetInstSetDefinition
private: // Member functions
  void mergeLeafAndResultNTs(Rule* rule_, const NodeInfo& nodeInfo_, CGFuncContext& context_);
  //! \brief Generates the statements in a basic block.
  //! \return Returns false if there is any unmatched IR.
  bool generateBB(IRBB* bb_, CGFuncContext& context_);
  bool generateStmt(IRStmt* stmt_, CGFuncContext& context_);
  //! \brief Returns statement rules that matches to the \p stmt.
  //! \param [in] stmt_ IR Statement to be matched.
  //! \param [in,out] results_ StmtRules that matches to the \p stmt.
  //! \return May return false if there is no matching statement rule to \p stmt.
  //! \pre \p stmtRules must be empty.
  //! \pre \p stmt must be non-null.
  bool matchStmt(IRStmt* stmt_, vector<StmtRule*>& results_);
  bool coverStmt(IRStmt* matchedStmt_, StmtRule* matchedRule_, CGFuncContext& context_);
  //! \brief Returns the rules that matches to irExpr among the given rule list.
  //! \return May return false if there is no matching expression rule to \p irExpr.
  bool matchExpr(IRExpr* irExpr_, const vector<ExprRule*>& rules_, vector<ExprRule*>& results_);
  bool matchTree(IRExpr* irExpr_, CGFuncContext& context_);
  bool coverTree(const NodeInfo& irExpr_, CGFuncContext& context_, bool isRecFirstCall_);
  //! \brief Calculates and updates the cost of a rule given matched rule.
  //! \return Returns \a true if the cost vector is updated (this means \p
  //!         exprRule is a better match).
  //! \note \p exprRule has the matched IRExpr node.
  //! \pre leafInfo must have the cost information of all child nodes of IR
  //!      expr that the exprRule has matched.
  bool updateCostVectorOf(ExprRule* exprRule_, CGFuncContext& context_);
public: // Member functions
  CGBB* GetCurrBB() {return mCurrBB;}
  Target* GetTarget() const {return mTarget;}
  //! \sa ATOLowerHIR
  const vector<LowerExprInfo*>& GetExprsToBeLowered() const {return mExprsToBeLowered;}
  //! \sa ATOLowerHIR
  const vector<InsertCastInfo*>& GetCastsToBeInserted() const {return mCastsToBeInserted;}
  bool IsAHWReg(const string& regName_) const {return mHWRegs.find(regName_) != mHWRegs.end();}
protected: // Member functions
  void AddInsertCast(const pair<IRType*, IRType*>& cast_, IRType* insert_) {
    BMDEBUG4("HWDescription::AddInsertCast", cast_.first, cast_.second, insert_);
    mCastsToBeInserted.push_back(new InsertCastInfo(cast_.first, cast_.second, insert_));
    EMDEBUG0();
    return;
  } // HWDescription::AddInsertCast
  void AddToBeLowered(IRPatExpr* pattern_, IRFunctionDecl* funcReplacement_, bool convRetVal_) {
    BMDEBUG4("HWDescription::AddToBeLowered", pattern_, funcReplacement_, convRetVal_);
    mExprsToBeLowered.push_back(new LowerExprInfo(pattern_, funcReplacement_, convRetVal_));
    EMDEBUG0();
    return;
  } // HWDescription::AddToBeLowered
protected:
  map<string, HWReg*> mHWRegs;
private: // Member data
  CGBB* mCurrBB; //!< Current basic block being generated.
  vector<StmtRule*> mStmtRules;
  vector<ExprRule*> mExprRules;
  vector<InsertCastInfo*> mCastsToBeInserted;
  vector<LowerExprInfo*> mExprsToBeLowered;
  //! \brief Holds the rules that convert one nonterminal in to another one.
  vector<ExprRule*> mNTConvRules;
  string mName;
  Target* mTarget;
  InstructionSetDefinition* mISADef;
}; // class HWDescription

class RegisterAllocator : public IDebuggable {
public:
  //! \param name_ Name of the register allocator, e.g. name of the algorithm used.
  RegisterAllocator(const string& name_) :
    mFuncContext(&Invalid<CGFuncContext>::Obj()),
    mName(name_)
  {
  } // RegisterAllocator::RegisterAllocator
  virtual ~RegisterAllocator() {}
  //! \return Returns true on successful allocation.
  virtual bool Start(CGFuncContext& context_) {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden << ":" << typeid(*this).name());
    return false;
  } // RegisterAllocator::Start
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  //! \brief Must return a HWReg in the available regs set of function context.
  //! The default implementation returns a register randomly.
  //! You may provide a different scheme in a derived class.
  //! \post You need to do register allocation rule checks, RegisterAllocator implements these.
  //!       So in derived class just do call RegisterAllocator::GetAnAvailableReg.
  virtual HWReg* GetAnAvailableReg(CGFuncContext& context_, VirtReg* vReg_);
  //! \brief Removes the given HW register from available regs set.
  void RemoveFromAvailables(CGFuncContext& context_, HWReg* reg_) {
    REQUIREDMSG(reg_ != 0, ieStrNonNullParam);
    //! \todo M Design: Have a look at more in detail here...
    if (context_.mAvailableRegs.mRegs.find(reg_) != context_.mAvailableRegs.mRegs.end()) {
      context_.mAvailableRegs -= reg_;
    } // if
  } // RegisterAllocator::RemoveFromAvailables
  void RemoveFromAvailables(CGFuncContext& context_, const HWRegSet& regs_) {
    const set<Register*> lRegs(regs_.GetRegs());
    for (set<Register*>::const_iterator lRit(lRegs.begin()); lRit != lRegs.end(); ++ lRit) {
      //! \todo M Design: Perhaps it is faster to use a set subtraction.
      RemoveFromAvailables(context_, static_cast<HWReg*>(*lRit));
    } // for
  } // RegisterAllocator::RemoveFromAvailables
  //! \brief Adds a HWReg back to available's set.
  void AddToAvailableRegs(CGFuncContext& context_, HWReg* reg_);
protected:
  void BuildInterferenceGraph(CGFuncContext& context_);
  bool isInterfere(DFAInstLiveness* dfaLiveness_, VirtReg* vReg0_, VirtReg* vReg1_);
  //! \brief Start member function must call this for its context.
  void SetFuncContext(CGFuncContext& context_) {
    mFuncContext = &context_;
  } // RegisterAllocator::SetFuncContext
private:
  CGFuncContext* mFuncContext;
  string mName; //!< Name of the register allocator.
  vector<vector<bool> > mAdjacencyMatrix;
}; // class RegisterAllocator

//! \brief Local register allocator, allocates registers
//!        for virtual registers only live within a basic block.
class RALocal : public RegisterAllocator {
public:
  RALocal() :
    RegisterAllocator("Local register allocator")
  {
  } // RALocal::RALocal
  virtual ~RALocal() {}
  virtual bool Start(CGFuncContext& context_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const {
    return RegisterAllocator::debug(context_, toStr_);
  } // RALocal::debug
}; // class RALocal

class RAGreedy : public RegisterAllocator {
public:
  RAGreedy() :
    RegisterAllocator("Greedy Register Allocator")
  {
  } // RAGreedy::RAGreedy
  virtual ~RAGreedy() {}
  virtual bool Start(CGFuncContext& context_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const {
    return RegisterAllocator::debug(context_, toStr_);
  } // RAGreedy::debug
}; // class RAGreedy

namespace InstructionSet {
class InstSetDef_Op : public IDebuggable {
public:
  InstSetDef_Op(const InstSetDef_Template* parent_) :
    mParent(parent_)
  {
  } // InstSetDef_Op::InstSetDef_Op
  virtual ~InstSetDef_Op() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  //! \brief Must return true if a value is set for this operand.
  virtual bool IsValueSet() const {
    REQUIREDMSG(0, ieStrFuncShouldBeOverridden);
  } // InstSetDef_Op::IsValueSet
protected:
  const InstSetDef_Template* mParent;
}; // class InstSetDef_Op

class InstSetDef_Regs : public IDebuggable {
public:
  InstSetDef_Regs(InstructionSetDefinition* parent_, const string& name_) :
    mName(name_),
    mParent(parent_)
  {
  } // InstSetDef_Regs::InstSetDef_Regs
  virtual ~InstSetDef_Regs() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  //! \return Returns this.
  InstSetDef_Regs* Regs(const char* reg0_, ...);
  bool IsAReg(const string& name_) {return mRegNames.find(name_) != mRegNames.end();}
  const string& GetName() const {return mName;}
private:
  string mName;
  RegSet mRegs;
  set<string> mRegNames;
  InstructionSetDefinition* mParent;
}; // class InstSetDef_Regs

class InstSetDef_OpRegSet : public InstSetDef_Op {
public:
  InstSetDef_OpRegSet(InstSetDef_Regs* regSetType_, const InstSetDef_Template* parent_) : 
    InstSetDef_Op(parent_),
    mRegSetType(regSetType_) 
  {
  } // InstSetDef_OpReg::InstSetDef_OpReg
  virtual ~InstSetDef_OpRegSet() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  virtual bool IsValueSet() const {return false;}
  const RegSet& GetRegs() const {return mValue;}
  InstSetDef_Regs* GetRegsType() const {return mRegSetType;} 
private:
  InstSetDef_Regs* mRegSetType;
  RegSet mValue;
}; // class InstSetDef_OpRegSet

class InstSetDef_OpReg : public InstSetDef_Op {
public:
  InstSetDef_OpReg(const string& id_, InstSetDef_Regs* regsType_, InstSetDef_Template* parent_, bool isDef_) : 
    InstSetDef_Op(parent_),
    mRegsType(regsType_),
    mIsDef(isDef_),
    mID(id_),
    mValue(0)
  {
  } // InstSetDef_OpReg::InstSetDef_OpReg
  virtual ~InstSetDef_OpReg() {}
public:
  InstSetDef_Regs* GetRegsType() const {return mRegsType;} 
  Register* GetReg() const {return mValue;}
  bool IsDef() const {return mIsDef;}
  const string& GetID() const {return mID;}
  virtual bool IsValueSet() const {return mValue != 0;}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  InstSetDef_Regs* mRegsType;
  Register* mValue;
  //! \brief A name given to this register operand.
  //! \note All register operands of an instruction must have different ids.
  //! Ids are, for example, used in register allocation rules.
  string mID;
  bool mIsDef; //!< If false this is a use of register.
}; // class InstSetDef_OpReg

class InstSetDef_Flag : public IDebuggable {
public:
  InstSetDef_Flag(InstructionSetDefinition* parent_, const string& name_) :
    mName(name_),
    mParent(parent_),
    mOptional(false)
  {
  } // InstSetDef_Flag::InstSetDef_Flag
  virtual ~InstSetDef_Flag() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  //! \return Returns this.
  InstSetDef_Flag* Flags(const char* flag1_, ...);
  bool IsAFlag(const string& flag_) const {return mFlags.find(flag_) != mFlags.end();}
  bool IsOptional() const {return mOptional;}
  const string& GetName() const {return mName;}
private:
  string mName;
  bool mOptional;
  set<string> mFlags;
  InstructionSetDefinition* mParent;
}; // class InstSetDef_Flag

class InstSetDef_OpFlag : public InstSetDef_Op {
public:
  InstSetDef_OpFlag(const InstSetDef_Flag* flagType_, const InstSetDef_Template* parent_) : 
    InstSetDef_Op(parent_),
    mFlagType(flagType_),
    mValue("")
  {
    REQUIREDMSG(flagType_ != 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(parent_ != 0, ieStrParamValuesDBCViolation);
  } // InstSetDef_OpFlag::InstSetDef_OpFlag
  InstSetDef_OpFlag(const InstSetDef_Flag* flagType_, const string& value_, const InstSetDef_Template* parent_) : 
    InstSetDef_Op(parent_),
    mFlagType(flagType_),
    mValue(value_)
  {
    REQUIREDMSG(value_ != "", ieStrParamValuesDBCViolation);
    REQUIREDMSG(flagType_ != 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(parent_ != 0, ieStrParamValuesDBCViolation);
  } // InstSetDef_OpFlag::InstSetDef_OpFlag
  virtual ~InstSetDef_OpFlag() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  virtual bool IsValueSet() const {return mValue != "";}
  const InstSetDef_Flag* GetFlag() const {return mFlagType;}
  void SetValue(const string& value_) {
    REQUIREDMSG(value_ != "", ieStrParamValuesDBCViolation); 
    mValue = value_;
    return;
  } // InstSetDef_OpFlag::SetValue
  const string& GetValue() const {return mValue;}
private:
  const InstSetDef_Flag* mFlagType;
  string mValue;
}; // class InstSetDef_OpFlag

class InstSetDef_OpTemplate : public InstSetDef_Op {
public:
  InstSetDef_OpTemplate(const string& name_, const InstSetDef_Template* parent_) : 
    InstSetDef_Op(parent_),
    mName(name_)
  {
  } // InstSetDef_OpTemplate::InstSetDef_OpTemplate
  virtual ~InstSetDef_OpTemplate() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  const string& GetName() const {return mName;}
  virtual bool IsValueSet() const {return false;}
private:
  string mName;
  InstSetDef_Template* mValue;
}; // class InstSetDef_OpTemplate

class InstSetDef_OpLabel : public InstSetDef_Op {
public:
  InstSetDef_OpLabel(const InstSetDef_Template* parent_) : 
    InstSetDef_Op(parent_) 
  {
  } // InstSetDef_OpLabel::InstSetDef_OpLabel
  virtual ~InstSetDef_OpLabel() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  void SetValue(const string& value_) {
    REQUIREDMSG(value_ != "", ieStrParamValuesDBCViolation);
    mValue = value_;
    return;
  } // InstSetDef_OpLabel::SetValue
  const string& GetValue() const {return mValue;}
  virtual bool IsValueSet() const {return mValue != "";}
private:
  string mValue;
}; // class InstSetDef_OpLabel

class InstSetDef_OpInt : public InstSetDef_Op {
public:
  InstSetDef_OpInt(const InstSetDef_Template* parent_) : 
    InstSetDef_Op(parent_),
    mValue(0)
  {
  } // InstSetDef_OpInt::InstSetDef_OpInt
  virtual ~InstSetDef_OpInt() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  virtual bool IsValueSet() const {return mValue != 0;}
  void SetValue(tBigInt value_) {
    mValue = new tBigInt(value_);
  } // InstSetDef_OpInt::SetValue
  const tBigInt& GetValue() const {return *mValue;}
private:
  tBigInt *mValue;
}; // class InstSetDef_OpInt

class InstSetDef_Template : public IDebuggable {
public:
  InstSetDef_Template(InstructionSetDefinition* parent_, const string& name_, bool isOR_ = false) :
    mName(name_),
    mParent(parent_),
    mFormatStr(0),
    mIsOR(isOR_)
  {
    if (isOR_ == true) {
      mFormatStr = "@0";
    } // if
  } // InstSetDef_Template::ISDefinition_Template
  virtual ~InstSetDef_Template() {}
public: // Member functions: IDebuggable
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public: // Member functions
  InstSetDef_Template* AddOp(const string& name_);
  InstSetDef_Template* AddOpFlag(const string& name_);
  InstSetDef_Template* AddOpFlag(const string& name_, const string& mustBeValue_);
  InstSetDef_Template* AddOpRegDef(const string& regsType_, const string& id_);
  InstSetDef_Template* AddOpRegUse(const string& regsType_, const string& id_);
  //! \note The name_ parameter is not currently used.
  InstSetDef_Template* AddOpRegSetUse(const string& name_);
  InstSetDef_Template* AddOpLabel();
  InstSetDef_Template* AddOpInt();
  //! \pre For OR templates do not call FormatStr they have "@0" as format string.
  //! \pre You may call it only once per object.
  InstSetDef_Template* FormatStr(const char* formatStr_);
  const vector<InstSetDef_Op*>& GetOperands() const {return mOperands;}
  const string& GetName() const {return mName;}
  //! \brief Returns true if operands represents an OR relation instead of sequence.
  bool IsOR() const {return mIsOR;}
private: // Member data
  string mName;
  bool mIsOR;
  const char* mFormatStr;
  vector<InstSetDef_Op*> mOperands;
  InstructionSetDefinition* mParent;
private:
  friend class InstructionSetDefinition;
}; // class InstSetDef_Template

class InstSetDef_Inst : public IDebuggable {
public:
  InstSetDef_Inst(InstructionSetDefinition* parent_, const string& name_, hFSInt32 sizeInBytes_) : 
    mName(name_),
    mParent(parent_),
    mTemplate(""),
    mSizeInBytes(sizeInBytes_)
  {
    REQUIREDMSG(sizeInBytes_ >= -1, ieStrParamValuesDBCViolation);
  } // InstSetDef_Inst::InstSetDef_Inst
  virtual ~InstSetDef_Inst() {}
public:
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
public:
  InstSetDef_Inst* UseTemplate(const string& name_);
  InstSetDef_Inst* AddRARule(RegAllocRule* rule_);
  InstSetDef_Inst* AddDef(const string& regName_);
  InstSetDef_Inst* AddDefs(const char* regName_, ...);
  const string& GetTemplateName() const {
    REQUIREDMSG(mTemplate != "", ieStrParamValuesDBCViolation << 
      " You must call UseTemplate before getting a template, every instruction must have a template.");
    return mTemplate;
  } // InstSetDef_Inst::GetTemplate
  hFUInt32 GetSize() const {return mSizeInBytes;}
  const RegSet& GetDefs() const {return mDefs;}
  const RegSet& GetUses() const {return mUses;}
  const vector<RegAllocRule*>& GetRARules() const {return mRARules;}
private:
  InstructionSetDefinition* mParent;
  string mTemplate;
  string mFormatStr;
  string mName;
  vector<RegAllocRule*> mRARules;
  RegSet mDefs;
  RegSet mUses;
  hFSInt32 mSizeInBytes;
private:
  friend class InstructionSetDefinition;
}; // class InstSetDef_Inst

class InstructionSetDefinition : public IDebuggable {
public:
  InstructionSetDefinition(HWDescription* hwDesc_, const string& name_, ASMDesc* asmDesc_) :
    mName(name_),
    mHWDesc(hwDesc_),
    mASMDesc(asmDesc_)
  {
    REQUIREDMSG(name_.size() != 0, ieStrParamValuesDBCViolation);
    REQUIREDMSG(hwDesc_ != 0, ieStrParamValuesDBCViolation);
  } // InstructionSetDefinition::InstructionSetDefinition
  virtual ~InstructionSetDefinition() {}
public:
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  HWDescription* GetHWDesc() const {return mHWDesc;}
  ASMDesc* GetASMDesc() const {return mASMDesc;}
  //! \param sizeInBytes_ Size of instruction in bytes, it can be 0 for labels
  //! or -1 if size can only be known at assembler, otherwise it is positive.
  InstSetDef_Inst* DefineInst(const string& opCode_, hFSInt32 sizeInBytes_) {
    InstSetDef_Inst* retVal(new InstSetDef_Inst(this, opCode_, sizeInBytes_));
    mInsts.insert(make_pair(opCode_, retVal));
    return retVal;
  } // InstructionSetDefinition::DefineInst
  InstSetDef_Regs* DefineRegs(const string& newRegs_) {
    InstSetDef_Regs* retVal(new InstSetDef_Regs(this, newRegs_));
    mRegs[newRegs_] = retVal;
    return retVal;
  } // InstructionSetDefinition::DefineRegs
  InstSetDef_Flag* DefineFlag(const string& newFlag_) {
    InstSetDef_Flag* retVal(new InstSetDef_Flag(this, newFlag_));
    mFlags[newFlag_] = retVal;
    return retVal;
  } // InstructionSetDefinition::DefineFlag
  InstSetDef_Template* DefineTemplate(const string& name_) {
    InstSetDef_Template* retVal(new InstSetDef_Template(this, name_, consts.cISATemplateIsNotOR));
    mTemplates.insert(make_pair(name_, retVal));
    return retVal;
  } // InstructionSetDefinition::DefineTemplate
  InstSetDef_Template* DefineTemplates(const string& name_) {
    InstSetDef_Template* retVal(new InstSetDef_Template(this, name_, consts.cISATemplateIsOR));
    mTemplates.insert(make_pair(name_, retVal));
    return retVal;
  } // InstructionSetDefinition::DefineTemplates
  InstSetDef_Template* DefineInstTemplate(const string& name_) {
    InstSetDef_Template* retVal(new InstSetDef_Template(this, name_, consts.cISATemplateIsNotOR));
    mTemplates.insert(make_pair(name_, retVal));
    return retVal;
  } // InstructionSetDefinition::DefineInstTemplate
  //! \pre Instruction definition must exists.
  void GetInstDefs(const string& name_, vector<InstSetDef_Inst*>& instDefs_) const {
    BMDEBUG2("InstructionSetDefinition::GetInstDefs", name_);
    REQUIREDMSG(mInsts.find(name_) != mInsts.end(), ieStrParamValuesDBCViolation);
    typedef multimap<string, InstSetDef_Inst*>::const_iterator I;
    pair<I, I> lRange(mInsts.equal_range(name_));
    for (I lIt(lRange.first); lIt != lRange.second; ++ lIt) {
      instDefs_.push_back(lIt->second);
    } // for
    EMDEBUG1(instDefs_.size());
    return;
  } // InstructionSetDefinition::GetInstDef
  //! \pre A template must exist for a given name.
  void GetTemplates(const string& name_, vector<InstSetDef_Template*>& templates_) const {
    BMDEBUG2("InstructionSetDefinition::GetTemplates", name_);
    REQUIREDMSG(mTemplates.find(name_) != mTemplates.end(), ieStrParamValuesDBCViolation);
    typedef multimap<string, InstSetDef_Template*>::const_iterator I;
    pair<I, I> lRange(mTemplates.equal_range(name_));
    for (I lIt(lRange.first); lIt != lRange.second; ++ lIt) {
      templates_.push_back(lIt->second);
    } // for
    EMDEBUG1(templates_.size());
    return;
  } // InstructionSetDefinition::GetTemplates
  bool IsFlagType(const string& name_) {
    map<string, InstSetDef_Flag*>::const_iterator lIt(mFlags.find(name_));
    bool retVal(lIt != mFlags.end());
    return retVal;
  } // InstructionSetDefinition::IsFlagType
  InstSetDef_Regs* GetRegs(const string& name_) {
    REQUIREDMSG(IsRegType(name_) == true, ieStrParamValuesDBCViolation);
    return mRegs[name_];
  } // InstructionSetDefinition::GetRegs
  InstSetDef_Flag* GetFlag(const string& name_) {
    REQUIREDMSG(IsFlagType(name_) == true, ieStrParamValuesDBCViolation);
    return mFlags[name_];
  } // InstructionSetDefinition::GetFlag
  //! \return Returns true if name_ is defined by DefineRegs call.
  bool IsRegType(const string& name_) {
    map<string, InstSetDef_Regs*>::const_iterator lIt(mRegs.find(name_));
    bool retVal(lIt != mRegs.end());
    return retVal;
  } // InstructionSetDefinition::IsRegType
  Instruction* CreateInst(const string& opCode_, hFSInt32 numOfOps_, ...);
  InstructionSetDefinition* DefineShift(ShiftType shiftType_, const string& flag_) {
    mShiftFlags[shiftType_] = flag_;
    return this;
  } // InstructionSetDefinition::DefineShift
  InstructionSetDefinition* DefineCond(CNTType condType_, const string& flag_) {
    mCondFlags[condType_] = flag_;
    return this;
  } // InstructionSetDefinition::DefineShift
  //! \brief Add the given offset to the position numbers that are equal or
  //!        greater than the given start position.
  static void smRenumberFormatStr(string& formatStr_, hFSInt32 startPos_, hFSInt32 offset_);
  //! \brief places a string at the given position in a format string.
  //! \return Returns true if any replacement took place.
  static bool smReplaceFormatStr(string& formatStr_, hFSInt32 pos_, const string& str_);
  static hFSInt32 smMaxPosInFormatStr(const string& formatStr_);
private: // Member functions
  //! \param [out] operands_ returns newly created operands for the instruction.
  //! \pre "..." must be all "GenInstOp*" typed.
  //! \return Returns true if it could parse the given operands for the instruction template.
  Instruction* parseOps(const string& opCode_, const InstSetDef_Inst* instDef_, const vector<GenInstOp*>& ops_) const;
  //! \return Returns the template that matches to the given operands otherwise returns null.
  InstSetDef_Template* parseOps(const string& templateName_, const vector<GenInstOp*>& ops_, 
    hFSInt32& opsArgIndex_, vector<InstSetDef_Op*>& outOps_, string& formatStr_, bool isFirstCall_) const;
  bool parseOps(const InstSetDef_Template* template_, const vector<GenInstOp*>& ops_, hFSInt32& opsArgIndex_, 
    vector<InstSetDef_Op*>& outOps_, string& formatStr_, bool isFirstCall_) const;
  bool parseDoOpsMatch(InstSetDef_Op* op_, GenInstOp* genOp_) const;
  bool isOptionalFlag(const InstSetDef_Op* op_) const {
    BMDEBUG2("InstructionSetDefinition::isOptionalFlag", op_);
    const InstSetDef_OpFlag* lFlagOp(dynamic_cast<const InstSetDef_OpFlag*>(op_));
    bool retVal(lFlagOp != 0 && lFlagOp->GetFlag()->IsOptional() == true);
    EMDEBUG1(retVal);
    return retVal;
  } // InstructionSetDefinition::isOptionalFlag
private: // Member data
  string mName;
  HWDescription* mHWDesc;
  ASMDesc* mASMDesc;
  map<string, InstSetDef_Flag*> mFlags;
  map<string, InstSetDef_Regs*> mRegs;
  multimap<string, InstSetDef_Inst*> mInsts;
  multimap<string, InstSetDef_Template*> mTemplates;
  string mShiftFlags[STLast];
  string mCondFlags[CNTLast];
}; // class InstructionSetDefinition

} // namespace InstructionSet
using namespace InstructionSet;

#endif


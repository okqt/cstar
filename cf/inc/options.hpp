// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif

//! \brief All the compiler options are handled with this class.
//! \todo M Design: implement is valid, check if enums has a default value.
class Options : public IDebuggable {
public:
  Options() : 
    mIsInited(false)
  {
  } // Options::Options
  virtual ~Options() {}
  //! \brief Sets the options to the ones in given generic options.
  //! \pre All option names in generic options must be present in 'this' options.
  void Set(const GenericOptions& genericOptions_);
  //! \pre optionName_ must be initialized.
  virtual bool obGet(const string& optionName_) {
    REQUIREDMSG(mBoolOptions.find(optionName_) != mBoolOptions.end(), ieStrParamValuesDBCViolation);
    return mBoolOptions[optionName_];
  } // Options::obGet
  virtual Options& obSet(const string& optionName_, bool value_) {
    REQUIREDMSG(mBoolOptions.find(optionName_) != mBoolOptions.end(), ieStrParamValuesDBCViolation);
    mBoolOptions[optionName_] = value_;
    return *this;
  } // Options::obSet
  virtual Options& oeSet(const string& optionName_, const string& optionValue_) {
    REQUIREDMSG(mEnumOptions.find(optionName_) != mEnumOptions.end(), ieStrParamValuesDBCViolation << 
      " option name is not found: " << optionName_);
    REQUIREDMSG(mValidEnums.find(make_pair(optionName_, optionValue_)) != mValidEnums.end(), 
      ieStrParamValuesDBCViolation << " option value is not found: " << optionValue_);
    mEnumOptions[optionName_] = optionValue_;
    return *this;
  } // Options::oeSet
  //! \pre optionName_ must be initialized.
  virtual const string& oeGet(const string& optionName_) {
    REQUIREDMSG(mEnumOptions.find(optionName_) != mEnumOptions.end(), ieStrParamValuesDBCViolation);
    return mEnumOptions[optionName_];
  } // Options::oeGet
  //! \brief Returns true if options are initialized, i.e. any option is added.
  bool IsInited() const {return mIsInited == true;}
public:
  static const bool scDefault = true;
protected:
  bool hasBoolOption(const string& name_) const {
    bool retVal(mBoolOptions.find(name_) != mBoolOptions.end());
    return retVal;
  } // Options::hasBoolOption
  bool hasEnumOption(const string& optionName_) const {
    bool retVal(mEnumOptions.find(optionName_) != mEnumOptions.end());
    return retVal;
  } // Options::hasEnumOption
  bool hasEnumOption(const string& optionName_, const string& optionValue_) const {
    bool retVal(mValidEnums.find(make_pair(optionName_, optionValue_)) != mValidEnums.end());
    return retVal;
  } // Options::hasEnumOption
  void addBoolOption(const string& optionName_, bool defaultValue_) {
    REQUIREDMSG(hasEnumOption(optionName_) == false, ieStrParamValuesDBCViolation);
    mBoolOptions[optionName_] = defaultValue_;
    mIsInited = true;
  } // Options::addBoolOption
  void addEnumOption(const string& optionName_, const string& value_, bool isDefault_ = false) {
    REQUIREDMSG(hasBoolOption(optionName_) == false, ieStrParamValuesDBCViolation);
    mValidEnums.insert(make_pair(optionName_, value_));
    if (isDefault_ == scDefault) {
      mEnumOptions[optionName_] = value_;
    } // if
    mIsInited = true;
  } // Options::addEnumOption
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
private:
  map<string, bool> mBoolOptions;
  map<string, string> mEnumOptions;
  set<pair<string, string> > mValidEnums;
  bool mIsInited; //!< True if an option has been added.
private:
  friend class Anatrop;
  friend void back_end();
  friend void lcc_progbeg(int argc, char* argv_[]);
}; // class Options

//! \brief A generic options class, i.e. you may start using options without installing them.
//! \note Options that are used first becomes valid option and further uses of
//!       the same option must be consistent with the first one. For example,
//!       if you use an option name in bool context first time you may not
//!       use it in enumeration context afterwards.
//! \sa Options
class GenericOptions : public Options {
public:
  GenericOptions() {}
  virtual ~GenericOptions() {}
  virtual GenericOptions& obSet(const string& optionName_, bool value_) {
    if (hasBoolOption(optionName_) == false) {
      addBoolOption(optionName_, value_);
    } // if
    return static_cast<GenericOptions&>(Options::obSet(optionName_, value_));
  } // GenericOptions::obSet
  virtual GenericOptions& oeSet(const string& optionName_, const string& optionValue_) {
    if (hasEnumOption(optionName_, optionValue_) == false) {
      addEnumOption(optionName_, optionValue_);
    } // if
    return static_cast<GenericOptions&>(Options::oeSet(optionName_, optionValue_));
  } // GenericOptions::oeSet
  virtual const string& oeGet(const string& optionName_) {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled << 
      " You should first construct an Option object from this GenericOption and call the oeGet of that Option.");
    static string retVal("<erropt>");
    return retVal;
  } // GenericOptions::oeGet
  virtual bool obGet(const string& optionName_) {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled << 
      " You should first construct an Option object from this GenericOption and call the obGet of that Option.");
    return false;
  } // GenericOptions::obGet
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
}; // class GenericOptions

extern Options options;

#endif // __OPTIONS_HPP__


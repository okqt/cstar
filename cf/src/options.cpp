// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __OPTIONS_HPP__
#include "options.hpp"
#endif

Options options;

void Options::
Set(const GenericOptions& genericOptions_) {
  { // set bool options
    map<string, bool>::const_iterator lBoolIt(genericOptions_.mBoolOptions.begin());
    while (lBoolIt != genericOptions_.mBoolOptions.end()) {
      ASSERTMSG(hasBoolOption(lBoolIt->first) == true, ieStrParamValuesDBCViolation << 
          " all options mentioned in the parameter must be present: " <<
          lBoolIt->first << " is not an option");
      obSet(lBoolIt->first, lBoolIt->second);
      ++ lBoolIt;
    } // while
  } // block
  { // set enumeration options
    map<string, string>::const_iterator lEnumIt(genericOptions_.mEnumOptions.begin());
    while (lEnumIt != genericOptions_.mEnumOptions.end()) {
      ASSERTMSG(hasEnumOption(lEnumIt->first, lEnumIt->second) == true, ieStrParamValuesDBCViolation << 
          " all options mentioned in the parameter must be present.");
      oeSet(lEnumIt->first, lEnumIt->second);
      ++ lEnumIt;
    } // while
  } // block
  return;
} // Options::Set


// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __STATS_HPP__
#define __STATS_HPP__

#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif
#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif

class Statistics : public IDebuggable {
public:
  Statistics();
  virtual ~Statistics() {}
  void Record(const string& what_);
  void Reset(const string& what_);
  virtual char* debug(const DTContext& context_, ostream& toStr_) const;
  void OnProgEnd(const string& fileNamePrefix_);
private:
  set<string> mValidStats;
  map<string, hFSInt32> mStats;
}; // class Statistics

extern Statistics* stat;

#endif


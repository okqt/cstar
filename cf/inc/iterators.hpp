// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.

#ifndef __ITERATORS_HPP__
#define __ITERATORS_HPP__

#ifndef __DEBUGTRACE_HPP__
#include "debugtrace.hpp"
#endif
#ifndef __UTILS_HPP__
#include "utils.hpp"
#endif

//! \file iterators.hpp
//! \brief Iterators have well defined semantics for iterating over
//!        collections.
//! Although there may be other ways to traverse collections when
//! intention is to 'traverse' a collection, iterators are expected to be used.

//! \brief List iteration interface for a collection.
//! \note currently only std::list is provided, but std::vector,
//! or other collections can also be used.
//! \todo M Design: Can we use iterator classes in the iterator? It sounds
//!       like a much better approach.
//! \sa FwdListIterator, RevListIterator
template<typename T>
class ListIterator {
public:
  //! \brief Iterators cannot be assigned.
  void operator = (const ListIterator& it_) {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  } // operator =
  ~ListIterator() {}
public:
  ListIterator() :
    mCurrListIndex(-1),
    mIsInited(false)
  {
    // LIE
  } // ListIterator::ListIterator
  ListIterator(const ListIterator<T>& dlit_) :
    mCurrListIndex(0),
    mRefList(dlit_.mRefList),
    mIsInited(false)
  {
    REQUIREDMSG(dlit_.mIsInited == false, "You can only copy an unoperated ListIterator");
  } // ListIterator::ListIterator
  ListIterator(list<T>& refList_) :
    mCurrListIndex(0),
    mIsInited(false)
  {
    mRefList.push_back(&refList_);
  } // ListIterator::ListIterator
  void First() {
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    mIsInited = true;
    mCurrListIndex = 0;
    mIt = mRefList[0]->begin();
  } // ListIterator::First
  bool IsDone() {
    REQUIREDMSG(mIsInited == true, "First() must be called before IsDone()");
    return (mCurrListIndex+1 == (hFInt32)mRefList.size()) && mIt == mRefList[mCurrListIndex]->end();
  } // ListIterator::IsDone
  void Next() {
    REQUIREDMSG(mIsInited == true, "First() must be called before Next()");
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    REQUIREDMSG((mCurrListIndex+1 != (hFInt32)mRefList.size()) ||
      mIt != mRefList[mCurrListIndex]->end(), "Next() should not be called, past the last element.");
    ++ mIt;
    if (mIt == mRefList[mCurrListIndex]->end() && mCurrListIndex != mRefList.size()-1) {
      mIt = mRefList[++ mCurrListIndex]->begin();
    } // if
  } // ListIterator::Next
  void Add(ListIterator<T>& listIter_) {
    REQUIREDMSG(mIsInited == false, "You may only add a list before using the list");
    for (hFUInt32 c = 0; c < listIter_.mRefList.size(); ++ c) {
      mRefList.push_back(listIter_.mRefList[c]);
    } // for
    mCurrListIndex = 0;
  } // ListIterator::Add
  //! \brief Returns pointer to the current item.
  T& operator->() {
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    REQUIREDMSG(mIsInited == true &&
      ((mCurrListIndex+1 != (hFInt32)mRefList.size()) || mIt != mRefList[mCurrListIndex]->end()),
      "You may not access an element before calling First, or when IsDone() is true");
    return *mIt;
  } // ListIterator::operator->
  //! \brief Returns the current item in the list.
  T& operator*() {
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    REQUIREDMSG(mIsInited == true &&
      ((mCurrListIndex+1 != (hFInt32)mRefList.size()) || mIt != mRefList[mCurrListIndex]->end()),
      "You may not access an element before calling First, or when IsDone() is true");
    return *mIt;
  } // ListIterator::operator*
private:
  hFSInt32 mCurrListIndex;
  vector<list<T>*> mRefList; //!< The internal representation of the collection.
  typename list<T>::iterator mIt;
  bool mIsInited; //!< State variable keeping track of initializaton, i.e., call of the First() function.
}; // class ListIterator

//! \brief You should use FwdListIterator if the traversal order is important.
//! \sa ListIterator, RevListIterator
#define FwdListIterator ListIterator

//! \sa ListIterator,FwdListIterator
template<typename T>
class RevListIterator {
public:
  //! \brief Iterators cannot be assigned.
  void operator = (const RevListIterator& it_) {
    REQUIREDMSG(0, ieStrFuncShouldNotBeCalled);
  } // operator =
public:
  RevListIterator() :
    mCurrListIndex(-1),
    mIsInited(false)
  {
    // LIE
  } // RevListIterator::RevListIterator
  RevListIterator(const RevListIterator<T>& dlit_) :
    mCurrListIndex(0),
    mRefList(dlit_.mRefList),
    mIsInited(false)
  {
    REQUIREDMSG(dlit_.mIsInited == false, "You can only copy an unoperated RevListIterator");
  } // RevListIterator::RevListIterator
  RevListIterator(list<T>& refList_) :
    mCurrListIndex(0),
    mIsInited(false)
  {
    mRefList.push_back(&refList_);
  } // RevListIterator::RevListIterator
  void First() {
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    mIsInited = true;
    mCurrListIndex = 0;
    mIt = mRefList[0]->rbegin();
  } // RevListIterator::First
  bool IsDone() {
    REQUIREDMSG(mIsInited == true, "First() must be called before IsDone()");
    return (mCurrListIndex+1 == (hFInt32)mRefList.size()) && mIt == mRefList[mCurrListIndex]->rend();
  } // RevListIterator::IsDone
  void Next() {
    REQUIREDMSG(mIsInited == true, "First() must be called before Next()");
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    REQUIREDMSG((mCurrListIndex+1 != (hFInt32)mRefList.size()) ||
      mIt != mRefList[mCurrListIndex]->rend(), "Next() should not be called, past the last element.");
    ++ mIt;
    if (mIt == mRefList[mCurrListIndex]->rend() && mCurrListIndex != mRefList.size() - 1) {
      mIt = mRefList[++ mCurrListIndex]->rbegin();
    } // if
  } // RevListIterator::Next
  void Add(RevListIterator<T>& listIter_) {
    REQUIREDMSG(mIsInited == false, "You may only add a list before using the list");
    for (hFUInt32 c = 0; c < listIter_.mRefList.size(); ++ c) {
      mRefList.insert(0, listIter_.mRefList[c]);
    } // for
    mCurrListIndex = 0;
  } // RevListIterator::Add
  //! \brief Returns pointer to the current item.
  T& operator->() {
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    REQUIREDMSG(mIsInited == true &&
      ((mCurrListIndex+1 != (hFInt32)mRefList.size()) || mIt != mRefList[mCurrListIndex]->rend()),
      "You may not access an element before calling First, or when IsDone() is true");
    return *mIt;
  } // RevListIterator::operator->
  //! \brief Returns the current item in the list.
  T& operator*() {
    REQUIREDMSG(mCurrListIndex != -1, "First add a list");
    REQUIREDMSG(mIsInited == true &&
      ((mCurrListIndex+1 != (hFInt32)mRefList.size()) || mIt != mRefList[mCurrListIndex]->rend()),
      "You may not access an element before calling First, or when IsDone() is true");
    return *mIt;
  } // RevListIterator::operator*
private:
  hFSInt32 mCurrListIndex;
  vector<list<T>*> mRefList; //!< The internal representation of the collection.
  typename list<T>::reverse_iterator mIt;
  bool mIsInited; //!< State variable keeping track of initializaton, i.e., call of the First() function.
}; // class RevListIterator

#endif // __ITERATORS_HPP__


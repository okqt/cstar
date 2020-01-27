// The information contained herein is confidential information.
// Any reproduction or disclosure of this file without the written
// permission is unlawful.
// Copyright (c) 2006-2007.
#ifndef __HOSTTYPES_HPP__
#define __HOSTTYPES_HPP__

//! \file hosttypes.hpp
//! \brief All host type definitions are here.

// //////////////////////////
// HOST TYPES
// //////////////////////////
// Compiler developer must ensure that C/C++ types are
// as wide as the number of bits in hU/SInt#.
// If there is no C/C++ type available use BigInt.
//! \ingroup hosttypes
//! \brief Provide an unsigned and at least 8bit host type.
typedef unsigned char hUInt8;
//! \ingroup hosttypes
//! \brief Provide a signed and at least 8bit host type.
typedef signed char hSInt8;
//! \ingroup hosttypes
//! \brief Provide an unsigned and at least 16bit host type.
typedef unsigned short hUInt16;
//! \ingroup hosttypes
//! \brief Provide a signed and at least 16bit host type.
typedef signed short hSInt16;
//! \ingroup hosttypes
//! \brief Provide an unsigned and at least 32bit host type.
typedef unsigned int hUInt32;
//! \ingroup hosttypes
//! \brief Provide an unsigned and at least 64bit host type.
typedef unsigned long long hUInt64;
//! \ingroup hosttypes
//! \brief Provide a signed and at least 32bit host type.
typedef signed int hSInt32;
//! \ingroup hosttypes
//! \brief Provide a signed and at least 64bit host type.
typedef signed long long hSInt64;
// You may add more integer host types, e.g. hU/SInt64,128,256,512,1024,...
// But do not forget to update the IRBuilder::IsValid for checking the
// validity of the types.

/*! \defgroup fasthosttypes Fast Host Types
    \brief Fast host types are mapped to the fastest integers of host with given minimum number of bits.

    \par Why fast host types?
    The reason fast host types are introduced is that some of the hardware performs
    faster operations if the data type is signed, however, some HW is faster with unsigned.
    At the same time, some hardware is faster with wider data types. So
    when you are developing some compiler algorithms in the framework
    choose the data type accordingly, i.e. for memory efficiecy use hosttypes,
    for speed use fasthosttypes.

    \par Requirements
    The host typedef should be the fastest integer type
    and at the same time it should be at least as big as the number of bits specified.
    For example:
    \code
    for (hFUInt8 c = 0; c < length; c ++) {
      ...
    } // for
    \endcode
    The variable \a c must be an unsigned host type and it must have at least 8 bits.

    \par Comparison of normal and fast host types
         Compared to non-fast host types fast host types are suitable for temporary
         host variables that should be fast but need not be memory space efficient.
    \note There are also fast types without signedness specification, e.g. hFInt16
          this can be signed or an unsigned host type.
    \note Avoid using fast types in parameter types, it is more
          important to have signedness in the parameter types.
    \sa targettypes hosttypes
*/
//! \ingroup fasthosttypes
//! \brief You can change the "unsigned int" depending on your platform.
typedef unsigned int hFUInt8;
//! \ingroup fasthosttypes
//! \brief You can change the "signed int" depending on your platform.
typedef signed int hFSInt8;
//! \ingroup fasthosttypes
//! \brief You can change the "unsigned int" depending on your platform.
typedef unsigned int hFUInt16;
//! \ingroup fasthosttypes
//! \brief You can change the "signed int" depending on your platform.
typedef signed int hFSInt16;
//! \ingroup fasthosttypes
//! \brief You can change the "unsigned int" depending on your platform.
typedef unsigned int hFUInt32;
//! \ingroup fasthosttypes
//! \brief You can change the "unsigned long long" depending on your platform.
typedef unsigned long long hFUInt64;
//! \ingroup fasthosttypes
//! \brief You can change the "signed long long" depending on your platform.
typedef signed long long hFSInt64;
//! \ingroup fasthosttypes
//! \brief You can change the "signed int" depending on your platform.
typedef signed int hFSInt32;
//! \ingroup fasthosttypes
//! \brief You can change the "int" depending on your platform.
typedef int hFInt8;
//! \ingroup fasthosttypes
//! \brief You can change the "int" depending on your platform.
typedef int hFInt16;
//! \ingroup fasthosttypes
//! \brief You can change the "int" depending on your platform.
typedef int hFInt32;

//! \ingroup hosttypes
typedef float hReal32;
//! \ingroup hosttypes
typedef double hReal64;
//! \ingroup hosttypes
typedef long double hReal80;

extern hFUInt32 hFUInt32_ignore;
extern hFSInt32 hFSInt32_ignore;
extern hFUInt16 hFUInt16_ignore;
extern hFSInt16 hFSInt16_ignore;
extern hFUInt8 hFUInt8_ignore;
extern hFSInt8 hFSInt8_ignore;

#endif // __HOSTTYPES_HPP__


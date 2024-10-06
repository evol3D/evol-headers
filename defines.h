#ifndef EV_HEADERS_DEFINES_H
#define EV_HEADERS_DEFINES_H

#ifndef EV_OS_WINDOWS
# define EV_OS_WINDOWS 0
#endif
#ifndef EV_OS_LINUX
# define EV_OS_LINUX 0
#endif
#ifndef EV_OS_IOS
# define EV_OS_IOS 0
#endif
#ifndef EV_OS_MACOS
# define EV_OS_MACOS 0
#endif
#ifndef EV_OS_ANDROID
# define EV_OS_ANDROID 0
#endif
#if (EV_OS_WINDOWS || EV_OS_LINUX || EV_OS_IOS || EV_OS_MACOS || EV_OS_ANDROID)
# define EV_OS_DEFINED 1
#else
# define EV_OS_DEFINED 0
#endif

#ifndef EV_CC_MSVC
# define EV_CC_MSVC 0
#endif
#ifndef EV_CC_GCC
# define EV_CC_GCC 0
#endif
#ifndef EV_CC_CLANG
# define EV_CC_CLANG 0
#endif
#if (EV_CC_MSVC || EV_CC_GCC || EV_CC_CLANG)
# define EV_CC_DEFINED 1
#else
# define EV_CC_DEFINED 0
#endif

// Operating System Detection
#if !EV_OS_DEFINED
# if defined(_WIN32)
#  undef EV_OS_WINDOWS
#  define EV_OS_WINDOWS 1
# elif defined(__linux__)
#  undef EV_OS_LINUX
#  define EV_OS_LINUX 1
# elif defined(__APPLE__)
#  include "TargetConditionals.h"
#  if defined(TARGET_OS_IPHONE)
#   undef EV_OS_IOS
#   define EV_OS_IOS 1
#  elif defined(TARGET_OS_MAC)
#   undef EV_OS_MACOS
#   define EV_OS_MACOS 1
#  elif defined(__ANDROID__)
#   undef EV_OS_ANDROID
#   define EV_OS_ANDROID 1
#  endif
# else
#  error EV_OS_UNKNOWN
# endif
#endif

// Compiler Detection
#if !EV_CC_DEFINED
# if defined(_MSC_VER)
#  undef EV_CC_MSVC
#  define EV_CC_MSVC 1
# elif defined(__GNUC__)
#  undef EV_CC_GCC
#  define EV_CC_GCC 1
# elif defined(__clang)
#  undef EV_CC_CLANG
#  define EV_CC_CLANG 1
# else
#  error EV_CC_UNKNOWN
# endif
#endif

#if !defined(EV_BUILDTYPE_DEBUG) && !defined(EV_BUILDTYPE_DEBUG_OPT) && !defined(EV_BUILDTYPE_RELEASE)
#define EV_BUILDTYPE_RELEASE 1
#endif

#ifndef EV_BUILDTYPE_DEBUG
# define EV_BUILDTYPE_DEBUG 0
#endif
#ifndef EV_BUILDTYPE_DEBUGOPT
# define EV_BUILDTYPE_DEBUGOPT 0
#endif
#ifndef EV_BUILDTYPE_RELEASE
# define EV_BUILDTYPE_RELEASE 0
#endif
#if (EV_BUILDTYPE_DEBUG || EV_BUILDTYPE_DEBUGOPT || EV_BUILDTYPE_RELEASE)
# define EV_BUILDTYPE_DEFINED 1
#else
# define EV_BUILDTYPE_DEFINED 0
#endif

#if !EV_BUILDTYPE_DEFINED
# error "Buildtype not defined. Please define one of `EV_BUILDTYPE_{DEBUG,DEBUGOPT,RELEASE}`"
#endif

#endif // EV_HEADERS_DEFINES_H

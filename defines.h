#pragma once

// Operating System Detection
#if defined(_WIN32)
# define EV_OS_WINDOWS
#elif defined(__linux__)
# define EV_OS_LINUX
#elif defined(__APPLE__)
# include "TargetConditionals.h"
# if defined(TARGET_OS_IPHONE)
#  define EV_OS_IOS
# elif defined(TARGET_OS_MAC)
#  define EV_OS_MACOS
# elif defined(__ANDROID__)
#  define EV_OS_ANDROID
# endif
#else
# error EV_OS_UNKNOWN
#endif

// Compiler Detection
#if defined(_MSC_VER)
# define EV_CC_MSVC
#elif defined(__GNUC__)
# define EV_CC_GCC
#elif defined(__clang)
# define EV_CC_CLANG
#else
# error EV_CC_UNKNOWN
#endif

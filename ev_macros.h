#pragma once

#include "defines.h"

/*!
 * \brief Macro to get a type's alignment
 */
#define EV_ALIGNOF _Alignof

/*!
 * \brief Attribute to specify the minimum alignment of a variable or structure
 */
#define EV_ALIGNAS _Alignas

/*!
 * \brief A simple decorator with no purpose other than to make it known that
 * a function parameter is not guaranteed to be aligned.
 */
#define EV_UNALIGNED

#if defined(EV_CC_MSVC)
# define EV_EXPORT __declspec(dllexport)
# define EV_IMPORT __declspec(dllimport)
# define EV_UNUSED
#elif defined(EV_CC_GCC) || defined(EV_CC_CLANG)
# define EV_EXPORT __attribute__((visibility("default")))
# define EV_IMPORT
# define EV_UNUSED __attribute__((unused))
#else
# error "Unknown Compiler"
#endif

#if defined(EV_CC_MSVC)
# define _EV_BREAK_IF(cond) cond ? __debugbreak():0
#elif defined(EV_CC_GCC) || defined(EV_CC_CLANG)
# include <signal.h>
# define _EV_BREAK_IF(cond) cond ? raise(SIGTRAP):0
#else
# error "Unknown Compiler"
/*!
 * \brief Breaks in the debugger if `cond` evaluates to `true`
 */
# define _EV_BREAK_IF(cond)
#endif

/*!
 * \brief Macro to get the size of a compile-time array.
 */
#define EV_ARRSIZE(...) (sizeof(__VA_ARGS__)/sizeof((__VA_ARGS__)[0]))

/*!
 * \brief Macro to concatenate two tokens together.
 */
#define EV_CAT(a, ...) EV_CAT_IMPL(a, __VA_ARGS__)
#define EV_CAT_IMPL(a, ...) a##__VA_ARGS__

/*!
 * \brief Macro to wrap tokens in double quotations. (")
 */
#define EV_STRINGIZE(a) EV_STRINGIZE_IMPL(a)
#define EV_STRINGIZE_IMPL(a) #a

// Used for the removal of parenthesis around tokens
#define EV_EXPAND(...) __VA_ARGS__

#define EV_EMPTY()
#define EV_DEFER(id) id EV_EMPTY()
#define EV_OBSTRUCT(...) __VA_ARGS__ EV_DEFER(EV_EMPTY)()

/*!
 * \brief Macro that returns the number of arguments passed to it.
 */
#define EV_VA_ARGS_NARG(...) EV_VA_ARGS_NARG_IMPL(__VA_ARGS__, EV_VA_ARGS_RSEQ_N())
#define EV_VA_ARGS_NARG_IMPL(...) EV_VA_ARGS_ARG_N(__VA_ARGS__)
#define EV_VA_ARGS_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, N, ...) N
#define EV_VA_ARGS_RSEQ_N() 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/*!
 * \brief Calls the passed macro for each element.
 * \note Maximum number of elements to iterate over is 64 elements.
 */
#define EV_FOREACH(OP, ...) EV_CAT(__EV_INTERNAL_FOREACH, EV_VA_ARGS_NARG(__VA_ARGS__))(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH1( OP, a     ) OP(a)
#define __EV_INTERNAL_FOREACH2( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH1( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH3( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH2( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH4( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH3( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH5( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH4( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH6( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH5( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH7( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH6( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH8( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH7( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH9( OP, a, ...) OP(a) __EV_INTERNAL_FOREACH8( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH10(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH9( OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH11(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH10(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH12(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH11(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH13(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH12(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH14(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH13(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH15(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH14(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH16(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH15(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH17(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH16(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH18(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH17(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH19(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH18(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH20(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH19(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH21(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH20(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH22(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH21(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH23(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH22(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH24(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH23(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH25(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH24(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH26(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH25(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH27(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH26(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH28(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH27(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH29(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH28(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH30(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH29(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH31(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH30(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH32(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH31(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH33(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH32(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH34(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH33(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH35(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH34(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH36(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH35(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH37(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH36(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH38(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH37(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH39(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH38(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH40(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH39(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH41(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH40(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH42(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH41(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH43(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH42(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH44(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH43(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH45(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH44(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH46(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH45(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH47(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH46(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH48(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH47(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH49(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH48(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH50(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH49(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH51(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH50(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH52(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH51(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH53(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH52(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH54(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH53(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH55(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH54(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH56(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH55(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH57(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH56(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH58(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH57(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH59(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH58(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH60(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH59(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH61(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH60(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH62(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH61(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH63(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH62(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH64(OP, a, ...) OP(a) __EV_INTERNAL_FOREACH63(OP, __VA_ARGS__)

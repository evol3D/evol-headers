#ifndef EV_HEADERS_MACROS_H
#define EV_HEADERS_MACROS_H

#include "defines.h"

// Internal Usage
#define __EV_MACRO_IF_ELSE(cond) EV_CAT(__EV_MACRO_IF_,cond)
#define __EV_MACRO_IF_1(...) __VA_ARGS__ __EV_MACRO_IF_1_ELSE
#define __EV_MACRO_IF_0(...)             __EV_MACRO_IF_0_ELSE
#define __EV_MACRO_IF_1_ELSE(...)
#define __EV_MACRO_IF_0_ELSE(...) __VA_ARGS__

#define __EV_MACRO_SECOND(a,b,...) b
#define __EV_MACRO_SECOND_INDIRECT() __EV_MACRO_SECOND

#define __EV_MACRO_IS_PROBE(...) __EV_MACRO_SECOND(__VA_ARGS__,0)
#define __EV_MACRO_PROBE() ~,1

#define __EV_MACRO_NOT(x) __EV_MACRO_IS_PROBE(EV_CAT(__EV_MACRO_NOT_,x))
#define __EV_MACRO_NOT_0 __EV_MACRO_PROBE()

#define __EV_MACRO_BOOL(x) __EV_MACRO_NOT(__EV_MACRO_NOT(x))

#define __EV_MACRO_NOP(...)
#define __EV_MACRO_NOP_INDIRECT() __EV_MACRO_NOP 
// End Of: Internal Usage

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

#define EV_PRAGMA(...) _Pragma(EV_STRINGIZE(__VA_ARGS__))

#if ( EV_CC_MSVC )
# define EV_WARNING_DISABLE_MSVC(w) EV_PRAGMA(warning(disable:w))
# define EV_WARNING_DISABLE_CLANG(...)
# define EV_WARNING_DISABLE_GCC(...)
# define EV_WARNING_PUSH() EV_PRAGMA(warning(push))
# define EV_WARNING_POP() EV_PRAGMA(warning(pop))
# define EV_EXPORT __declspec(dllexport)
# define EV_IMPORT __declspec(dllimport)
# define EV_UNUSED
# define EV_FORCEINLINE __forceinline
# define EV_ALIGN(x) __declspec(align(x))
#elif ( EV_CC_GCC || EV_CC_CLANG )
# define EV_EXPORT __attribute__((visibility("default")))
# define EV_IMPORT
# define EV_UNUSED __attribute__((unused))
# define EV_FORCEINLINE inline __attribute__((always_inline))
# define EV_ALIGN(x) __attribute__((aligned(x)))
# if ( EV_CC_GCC )
#  define EV_PRAGMA_CC_NAME GCC
#  define EV_WARNING_DISABLE_GCC(w) EV_PRAGMA(EV_PRAGMA_CC_NAME diagnostic ignored "-W"w)
#  define EV_WARNING_DISABLE_CLANG(...)
# elif ( EV_CC_CLANG )
#  define EV_PRAGMA_CC_NAME clang
#  define EV_WARNING_DISABLE_CLANG(w) EV_PRAGMA(EV_PRAGMA_CC_NAME diagnostic ignored "-W"w)
#  define EV_WARNING_DISABLE_GCC(...)
# endif
# define EV_WARNING_PUSH()     EV_PRAGMA(EV_PRAGMA_CC_NAME diagnostic push)
# define EV_WARNING_POP()      EV_PRAGMA(EV_PRAGMA_CC_NAME diagnostic pop)
# define EV_WARNING_DISABLE_MSVC(...)
#else
# error "Unknown Compiler"
#endif

#if ( EV_CC_MSVC )
# define _EV_BREAK_IF(cond) cond ? __debugbreak():0
#elif ( EV_CC_GCC || EV_CC_CLANG )
# include <signal.h>
# define _EV_BREAK_IF(cond) cond ? raise(SIGTRAP):0
#else
# error "Unknown Compiler"
/*!
 * \brief Breaks in the debugger if `cond` evaluates to `true`
 */
# define _EV_BREAK_IF(cond)
#endif

#if ( EV_BUILDTYPE_DEBUG || EV_BUILDTYPE_DEBUGOPT )
# define EV_DBGBREAK_IF(...) _EV_BREAK_IF(__VA_ARGS__)
# define EV_DEBUG(...) __VA_ARGS__
#include <assert.h>
# define EV_UNIMPLEMENTED() assert(!"Hit an unimplemented path")
#else
# define EV_DBGBREAK_IF(...)
# define EV_DEBUG(...)
# define EV_UNIMPLEMENTED()
#endif

/*!
 * \brief Macro to get the size of a compile-time array.
 */
#define EV_ARRSIZE(...) (sizeof(__VA_ARGS__)/sizeof((__VA_ARGS__)[0]))
#define EV_ARRSIZE_LITERAL(...) EV_EXPAND(EV_DEFER(EV_VA_ARGS_NARG)(EV_FILTER_EMPTY(__VA_ARGS__)))

/*!
 * \brief Macro to concatenate two tokens together.
 */
#define EV_CAT(a, ...) EV_CAT_IMPL(a, __VA_ARGS__)
#define EV_CAT_IMPL(a, ...) a##__VA_ARGS__

/*!
 * \brief Macro to concatenate multiple tokens together
 */
#define EV_CATN(...) EV_REDUCE(EV_CAT, __VA_ARGS__)

/*!
 * \brief Macro to wrap tokens in double quotations. (")
 */
#define EV_STRINGIZE(...) EV_STRINGIZE_IMPL(__VA_ARGS__)
#define EV_STRINGIZE_IMPL(...) #__VA_ARGS__

/*!
 * \brief Returns head of list (a,b,c) -> a
 */
#define EV_HEAD(h,...) h

/*!
 * \brief Returns tail of list (a,b,c) -> b,c
 */
#define EV_TAIL(h,...) __VA_ARGS__

// Used for the removal of parenthesis around tokens
#define EV_EXPAND(...) __VA_ARGS__

#define EV_EMPTY()
#define EV_DEFER(id) id EV_EMPTY()
#define EV_OBSTRUCT(...) __VA_ARGS__ EV_DEFER(EV_EMPTY)()

/*!
 * \brief Macro that returns the number of arguments passed to it.
 */
#define EV_VA_ARGS_NARG(...) EV_VA_ARGS_NARG_IMPL(_, ## __VA_ARGS__, EV_VA_ARGS_RSEQ_N())
#define EV_VA_ARGS_NARG_IMPL(...) EV_VA_ARGS_ARG_N(__VA_ARGS__)
#define EV_VA_ARGS_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, N, ...) N
#define EV_VA_ARGS_RSEQ_N() 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define EV_EVAL(...)             __EV_INTERNAL_EVAL1(__EV_INTERNAL_EVAL1(__EV_INTERNAL_EVAL1(__EV_INTERNAL_EVAL1(__VA_ARGS__))))
#define __EV_INTERNAL_EVAL1(...) __EV_INTERNAL_EVAL2(__EV_INTERNAL_EVAL2(__EV_INTERNAL_EVAL2(__EV_INTERNAL_EVAL2(__VA_ARGS__))))
#define __EV_INTERNAL_EVAL2(...) __EV_INTERNAL_EVAL3(__EV_INTERNAL_EVAL3(__EV_INTERNAL_EVAL3(__EV_INTERNAL_EVAL3(__VA_ARGS__))))
#define __EV_INTERNAL_EVAL3(...) __EV_INTERNAL_EVAL4(__EV_INTERNAL_EVAL4(__EV_INTERNAL_EVAL4(__EV_INTERNAL_EVAL4(__VA_ARGS__))))
#define __EV_INTERNAL_EVAL4(...) __VA_ARGS__
// #define __EV_INTERNAL_EVAL4(...) __EV_INTERNAL_EVAL5(__EV_INTERNAL_EVAL5(__EV_INTERNAL_EVAL5(__EV_INTERNAL_EVAL5(__VA_ARGS__))))
// #define __EV_INTERNAL_EVAL5(...) __EV_INTERNAL_EVAL6(__EV_INTERNAL_EVAL6(__EV_INTERNAL_EVAL6(__EV_INTERNAL_EVAL6(__VA_ARGS__))))
// #define __EV_INTERNAL_EVAL6(...) __VA_ARGS__

#define EV_INDIRECT_OP_ELSE_NOP(OP_INDIRECT,...) \
  EV_DEFER                                       \
  (                                              \
    EV_VA_OPT_ELSE(__VA_ARGS__)                  \
    (                                            \
      OP_INDIRECT                                \
    )                                            \
    (                                            \
      __EV_MACRO_NOP_INDIRECT                    \
    )                                            \
  )()

/*!
 * \brief Similar functionality to `__VA_OPT__`
 * Used as follows:
 * ```c
 * #define TEST(...) EV_VA_OPT(__VA_ARGS__)("Not ")"Empty"
 * ```
 */
#define EV_VA_OPT(...) __EV_MACRO_IF_ELSE(__EV_MACRO_NOT(EV_VA_ARGS_NARG(__VA_ARGS__)))()
#define EV_VA_OPT_ELSE(...) __EV_MACRO_IF_ELSE(__EV_MACRO_BOOL(EV_VA_ARGS_NARG(__VA_ARGS__)))

/*!
 * \brief Calls the passed macro for each element.
 */
#define EV_FOREACH(OP, ...) \
  EV_VA_OPT(__VA_ARGS__)(EV_EVAL(__EV_INTERNAL_FOREACH_IMPL(OP, __VA_ARGS__)))
#define __EV_INTERNAL_FOREACH_IMPL(OP, a, ...) \
  OP(a) EV_INDIRECT_OP_ELSE_NOP(__EV_INTERNAL_FOREACH_IMPL_INDIRECT, __VA_ARGS__)(OP, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH_IMPL_INDIRECT() __EV_INTERNAL_FOREACH_IMPL

/*!
 * \brief Same as EV_FOREACH but allows passing a UDATA token
 */
#define EV_FOREACH_UDATA(OP, UDATA,...) \
  EV_VA_OPT(__VA_ARGS__)(EV_EVAL(__EV_INTERNAL_FOREACH_UDATA_IMPL(OP, UDATA, __VA_ARGS__)))
#define __EV_INTERNAL_FOREACH_UDATA_IMPL(OP, UDATA, a, ...) \
  OP(UDATA, a) EV_INDIRECT_OP_ELSE_NOP(__EV_INTERNAL_FOREACH_UDATA_IMPL_INDIRECT, __VA_ARGS__)(OP, UDATA, __VA_ARGS__)
#define __EV_INTERNAL_FOREACH_UDATA_IMPL_INDIRECT() __EV_INTERNAL_FOREACH_UDATA_IMPL

#define EV_JOIN(...) \
  EV_JOIN_W_DELIM((,),__VA_ARGS__)

#define EV_JOIN_W_DELIM(delim, ...) \
  EV_VA_OPT(__VA_ARGS__)(EV_EVAL(__EV_INTERNAL_JOIN_W_DELIM_IMPL(delim, __VA_ARGS__)))
#define __EV_INTERNAL_JOIN_W_DELIM_IMPL(delim, a, ...) \
  a EV_INDIRECT_OP_ELSE_NOP(EV_EXPAND delim __EV_INTERNAL_JOIN_W_DELIM_IMPL_INDIRECT, __VA_ARGS__)(delim, __VA_ARGS__)
#define __EV_INTERNAL_JOIN_W_DELIM_IMPL_INDIRECT() __EV_INTERNAL_JOIN_W_DELIM_IMPL

#define DEFAULT_ZIP_OP(a, b) (a, b)
#define EV_ZIP(list1, list2) \
  EV_ZIP_W_OP(DEFAULT_ZIP_OP, list1, list2)

#define EV_ZIP_W_OP(OP, list1, list2) \
  EV_VA_OPT(EV_EXPAND list1)(EV_EVAL(__EV_INTERNAL_ZIP_W_OP_IMPL(OP, list1, list2)))
#define __EV_INTERNAL_ZIP_W_OP_IMPL(OP, list1, list2) \
  OP(EV_HEAD list1, EV_HEAD list2) EV_INDIRECT_OP_ELSE_NOP(__EV_INTERNAL_ZIP_W_OP_IMPL_INDIRECT, EV_TAIL list1)(OP, (EV_TAIL list1), (EV_TAIL list2))
#define __EV_INTERNAL_ZIP_W_OP_IMPL_INDIRECT() , __EV_INTERNAL_ZIP_W_OP_IMPL

#define EV_REDUCE(OP, ...) \
  EV_VA_OPT(__VA_ARGS__)(EV_EVAL(EV_INDIRECT_OP_ELSE_NOP(__EV_INTERNAL_REDUCE_IMPL_INDIRECT, __VA_ARGS__)(OP, EV_HEAD(__VA_ARGS__), EV_TAIL(__VA_ARGS__))))
#define __EV_INTERNAL_REDUCE_IMPL(OP, last, ...) \
  EV_DEFER(EV_VA_OPT_ELSE(__VA_ARGS__)(__EV_INTERNAL_REDUCE_IMPL_INDIRECT)(__EV_MACRO_SECOND_INDIRECT))()(OP, EV_VA_OPT_ELSE(__VA_ARGS__)(OP(last, EV_HEAD(__VA_ARGS__)))(last) EV_VA_OPT(__VA_ARGS__)(,EV_TAIL(__VA_ARGS__)))
#define __EV_INTERNAL_REDUCE_IMPL_INDIRECT() __EV_INTERNAL_REDUCE_IMPL

/*!
 * \brief Macro to get a the hex value of a 4-number as a character
 */
#define EV_TOHEX_CHAR(c) (("0123456789abcdef")[c])

#define __EV_FILTER_EMPTY_OP(x) EV_VA_OPT_ELSE(x)(,x)()
#define EV_FILTER_EMPTY(x,...) x EV_FOREACH(__EV_FILTER_EMPTY_OP, __VA_ARGS__)

#endif // EV_HEADERS_MACROS_H

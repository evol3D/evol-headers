#ifndef EV_HEADERS_TYPES_H
#define EV_HEADERS_TYPES_H

#include "ev_macros.h"
#include "ev_internal.h"
#include "ev_hash.h"

typedef void(*ev_copy_fn)(void *dst, void *src);
typedef void(*ev_free_fn)(void *self);
typedef u64(*ev_hash_fn)(void *self, u64 seed);
typedef bool(*ev_equal_fn)(void *self, void *other);

typedef void(*ev_tostr_fn)(void *self, char *out);
typedef u32(*ev_tostrlen_fn)();

typedef struct {
  EV_DEBUG(const char *name;)

  u32 size;
  u32 alignment;

  ev_copy_fn  copy_fn;
  ev_free_fn  free_fn;
  ev_hash_fn  hash_fn;
  ev_equal_fn equal_fn;
  ev_tostr_fn tostr_fn;
  ev_tostrlen_fn tostrlen_fn;

  void *default_val;
  void *invalid_val;
} EvTypeData;

#define COPY_FUNCTION(T,name)     EV_CAT(EV_CAT(EV_CAT(EV_COPY_FUNCTION_,T),_),name)
#define FREE_FUNCTION(T,name)     EV_CAT(EV_CAT(EV_CAT(EV_FREE_FUNCTION_,T),_),name)
#define HASH_FUNCTION(T,name)     EV_CAT(EV_CAT(EV_CAT(EV_HASH_FUNCTION_,T),_),name)
#define EQUAL_FUNCTION(T,name)    EV_CAT(EV_CAT(EV_CAT(EV_EQUAL_FUNCTION_,T),_),name)
#define TOSTR_FUNCTION(T,name)    EV_CAT(EV_CAT(EV_CAT(EV_TOSTR_FUNCTION_,T),_),name)
#define TOSTRLEN_FUNCTION(T,name) EV_CAT(EV_CAT(EV_CAT(EV_TOSTRLEN_FUNCTION_,T),_),name)

#define DEFINE_COPY_FUNCTION(T,name) static inline void COPY_FUNCTION(T,name)(T *dst, T *src)
#define DEFINE_DEFAULT_COPY_FUNCTION(T) \
  DEFINE_COPY_FUNCTION(T,DEFAULT) { *dst = *src; }

#define DEFINE_FREE_FUNCTION(T,name) static inline void FREE_FUNCTION(T,name)(T *self)
#define DEFINE_DEFAULT_FREE_FUNCTION(T) \
  DEFINE_FREE_FUNCTION(T,DEFAULT) { (void)self; }

#define DEFINE_HASH_FUNCTION(T,name) static inline void HASH_FUNCTION(T,name)(T *self, u64 seed)
#define DEFINE_DEFAULT_HASH_FUNCTION(T) \
  DEFINE_HASH_FUNCTION(T,DEFAULT) { ev_hash_murmur3(self, sizeof(T), seed); }

#define DEFINE_EQUAL_FUNCTION(T,name) static inline bool EQUAL_FUNCTION(T,name)(T *self, T *other)
// NOTE: This shouldn't be used for non-arithmetic types.
#define DEFINE_DEFAULT_EQUAL_FUNCTION(T) \
  DEFINE_EQUAL_FUNCTION(T,DEFAULT) { return memcmp(self, other, sizeof(T)); }

#define DEFINE_TOSTR_FUNCTION(T,name) static inline void TOSTR_FUNCTION(T,name)(T *self, char* out)
#define DEFINE_DEFAULT_TOSTR_FUNCTION(T) \
  DEFINE_TOSTR_FUNCTION(T,DEFAULT) \
  {  \
    for(int i = 0; i < sizeof(T); i++) \
    { \
      u8 byte = ((u8*)self)[i];\
      out[i*2] = EV_TOHEX_CHAR((u32)(byte >> 4) & 0xf); \
      out[i*2+1] = EV_TOHEX_CHAR((u32)byte & 0xf); \
    } \
  }

#define DEFINE_TOSTRLEN_FUNCTION(T,name) static inline u32 TOSTRLEN_FUNCTION(T,name)()
#define DEFINE_DEFAULT_TOSTRLEN_FUNCTION(T) \
  DEFINE_TOSTRLEN_FUNCTION(T,DEFAULT) { return sizeof(T) * 2; }

#define DECLARE_COPY_FUNCTION(T,name)     DEFINE_COPY_FUNCTION(T,name);
#define DECLARE_FREE_FUNCTION(T,name)     DEFINE_FREE_FUNCTION(T,name);
#define DECLARE_HASH_FUNCTION(T,name)     DEFINE_HASH_FUNCTION(T,name);
#define DECLARE_EQUAL_FUNCTION(T,name)    DEFINE_EQUAL_FUNCTION(T,name);
#define DECLARE_TOSTR_FUNCTION(T,name)    DEFINE_TOSTR_FUNCTION(T,name);
#define DECLARE_TOSTRLEN_FUNCTION(T,name) DEFINE_TOSTRLEN_FUNCTION(T,name);

#define EV_OVERRIDE_VAR(T) EV_CAT(__ev_internal_override_var_,T)
#define TypeData(T) EV_CAT(EV_TYPEDATA_,T)
#define TYPEDATA_GEN(T, ...) \
  EV_WARNING_PUSH(); \
  EV_WARNING_DISABLE_GCC("override-init"); \
  EV_WARNING_DISABLE_CLANG("override-init"); \
  EV_UNUSED static const EvTypeData TypeData(T) = { \
    EV_DEBUG(.name = EV_STRINGIZE(T),) \
    .size = sizeof(T), \
    .alignment = EV_ALIGNOF(T), \
    .default_val = (void*)&(T){0}, \
    .invalid_val = (void*)&(T){0}, \
    EV_VA_OPT(__VA_ARGS__)(EV_FOREACH_UDATA(__EV_STRUCT_METHOD_DEF, T, __VA_ARGS__)) \
  }; \
  EV_WARNING_POP(); \
  EV_UNUSED static T EV_OVERRIDE_VAR(T)

#define __EV_STRUCT_METHOD_DEF(T, ...) EV_CAT(__EV_,EV_CAT(EV_HEAD __VA_ARGS__,_FN))(T, EV_TAIL __VA_ARGS__)

#define COPY(...)     (COPY     , __VA_ARGS__)
#define FREE(...)     (FREE     , __VA_ARGS__)
#define HASH(...)     (HASH     , __VA_ARGS__)
#define EQUAL(...)    (EQUAL    , __VA_ARGS__)
#define TOSTR(...)    (TOSTR    , __VA_ARGS__)
#define TOSTRLEN(...) (TOSTRLEN , __VA_ARGS__)
#define DEFAULT(...)  (DEFAULT  , __VA_ARGS__)
#define INVALID(...)  (INVALID  , __VA_ARGS__)

#define __EV_COPY_FN(T,name)     .copy_fn     = (ev_copy_fn)     COPY_FUNCTION(T,name),
#define __EV_FREE_FN(T,name)     .free_fn     = (ev_free_fn)     FREE_FUNCTION(T,name),
#define __EV_HASH_FN(T,name)     .hash_fn     = (ev_hash_fn)     HASH_FUNCTION(T,name),
#define __EV_EQUAL_FN(T,name)    .equal_fn    = (ev_equal_fn)    EQUAL_FUNCTION(T,name),
#define __EV_TOSTR_FN(T,name)    .tostr_fn    = (ev_tostr_fn)    TOSTR_FUNCTION(T,name),
#define __EV_TOSTRLEN_FN(T,name) .tostrlen_fn = (ev_tostrlen_fn) TOSTRLEN_FUNCTION(T,name),
#define __EV_DEFAULT_FN(T, ...)  .default_val = (void*)&(T){ __VA_ARGS__ },
#define __EV_INVALID_FN(T, ...)  .invalid_val = (void*)&(T){ __VA_ARGS__ },

static void nop() {}
#define METHOD_CHECK(T, ...) (__VA_ARGS__ EV_DEBUG(?__VA_ARGS__:(assert(!EV_STRINGIZE(__VA_ARGS__)"not defined"),(T)nop)))

#define EV_COPY(T)     METHOD_CHECK(ev_copy_fn,     TypeData(T).copy_fn)
#define EV_FREE(T)     METHOD_CHECK(ev_free_fn,     TypeData(T).free_fn)
#define EV_HASH(T)     METHOD_CHECK(ev_hash_fn,     TypeData(T).hash_fn)
#define EV_EQUAL(T)    METHOD_CHECK(ev_equal_fn,    TypeData(T).equal_fn)
#define EV_TOSTR(T)    METHOD_CHECK(ev_tostr_fn,    TypeData(T).tostr_fn)
#define EV_TOSTRLEN(T) METHOD_CHECK(ev_tostrlen_fn, TypeData(T).tostrlen_fn)
#define __EV_OVERRIDE_DEFAULT(T, ...) EV_OVERRIDE_VAR(T).__VA_ARGS__,
#define __EV_DEFAULT_INTERNAL(T) (*(T*)TypeData(T).default_val)
#define EV_DEFAULT(T, ...) EV_VA_OPT_ELSE(__VA_ARGS__) \
                              ((EV_OVERRIDE_VAR(T)=__EV_DEFAULT_INTERNAL(T), \
                                EV_FOREACH_UDATA(__EV_OVERRIDE_DEFAULT, T, __VA_ARGS__) \
                                EV_OVERRIDE_VAR(T))) \
                              (__EV_DEFAULT_INTERNAL(T))
#define EV_INVALID(T) (*(T*)TypeData(T).invalid_val)

#if defined(EV_BUILDTYPE_DEBUG) || defined(EV_BUILDTYPE_DEBUGOPT)
#include <stdio.h>
#endif
DEFINE_TOSTR_FUNCTION(EvTypeData, EvTypeDataStringize)
{
  EV_DEBUG(
      sprintf(out,
              "TypeData:\n"
     EV_DEBUG("\t name: %s\n")
              "\t size: %u\n"
              "\t alig: %u\n",
     EV_DEBUG(self->name),
              self->size,
              self->alignment
      );
  )
}

TYPEDATA_GEN(EvTypeData,
    TOSTR(EvTypeDataStringize));

#endif // EV_HEADERS_TYPES_H

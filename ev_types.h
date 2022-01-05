#ifndef EV_HEADERS_TYPES_H
#define EV_HEADERS_TYPES_H

#include "ev_macros.h"
#include "ev_internal.h"
#include "ev_hash.h"

typedef void(*ev_copy_fn)(void *dst, void *src);
typedef void(*ev_free_fn)(void *self);
typedef __ev_uint64_t(*ev_hash_fn)(void *self, __ev_uint64_t seed);
typedef __ev_bool(*ev_equal_fn)(void *self, void *other);

typedef struct {
  EV_DEBUG(const char *name;)

  __ev_uint32_t size;
  __ev_uint32_t alignment;

  ev_copy_fn  copy_fn;
  ev_free_fn  free_fn;
  ev_hash_fn  hash_fn;
  ev_equal_fn equal_fn;

  void *default_val;
  void *invalid_val;
} EvTypeData;

#define COPY_FUNCTION(T,name)  EV_CAT(EV_CAT(EV_CAT(EV_COPY_FUNCTION_,T),_),name)
#define FREE_FUNCTION(T,name)  EV_CAT(EV_CAT(EV_CAT(EV_FREE_FUNCTION_,T),_),name)
#define HASH_FUNCTION(T,name)  EV_CAT(EV_CAT(EV_CAT(EV_HASH_FUNCTION_,T),_),name)
#define EQUAL_FUNCTION(T,name) EV_CAT(EV_CAT(EV_CAT(EV_EQUAL_FUNCTION_,T),_),name)

#define DEFINE_COPY_FUNCTION(T,name) void COPY_FUNCTION(T,name)(T *dst, T *src)
#define DEFINE_DEFAULT_COPY_FUNCTION(T) \
  DEFINE_COPY_FUNCTION(T,DEFAULT) { *dst = *src; }

#define DEFINE_FREE_FUNCTION(T,name) void FREE_FUNCTION(T,name)(T *self)
#define DEFINE_DEFAULT_FREE_FUNCTION(T) \
  DEFINE_FREE_FUNCTION(T,DEFAULT) { (void)self; }

#define DEFINE_HASH_FUNCTION(T,name) void HASH_FUNCTION(T,name)(T *self, u64 seed)
#define DEFINE_DEFAULT_HASH_FUNCTION(T) \
  DEFINE_HASH_FUNCTION(T,DEFAULT) { ev_hash_murmur3(self, sizeof(T), seed); }

#define DEFINE_EQUAL_FUNCTION(T,name)  __ev_bool EQUAL_FUNCTION(T,name)(T *self, T *other)
#define DEFINE_DEFAULT_EQUAL_FUNCTION(T) \
  DEFINE_EQUAL_FUNCTION(T,DEFAULT) { return *self == *other; }

#define DECLARE_COPY_FUNCTION(T,name)  DEFINE_COPY_FUNCTION(T,name);
#define DECLARE_FREE_FUNCTION(T,name)  DEFINE_FREE_FUNCTION(T,name);
#define DECLARE_HASH_FUNCTION(T,name)  DEFINE_HASH_FUNCTION(T,name);
#define DECLARE_EQUAL_FUNCTION(T,name) DEFINE_EQUAL_FUNCTION(T,name);

#define EV_REGISTER_TYPE(T) \
  DEFINE_DEFAULT_COPY_FUNCTION(T) \
  DEFINE_DEFAULT_FREE_FUNCTION(T) \
  DEFINE_DEFAULT_HASH_FUNCTION(T) \
  DEFINE_DEFAULT_EQUAL_FUNCTION(T)

#define EV_TYPEDEF(T, ...)        \
  typedef __VA_ARGS__ T;          \
  EV_REGISTER_TYPE(T)

#define TypeData(T) EV_CAT(EV_TYPEDATA_,T)
#define TYPEDATA_GEN(T, ...) \
  static const EvTypeData TypeData(T) = { \
    EV_DEBUG(.name = EV_STRINGIZE(T),) \
    .size = sizeof(T), \
    .alignment = EV_ALIGNOF(T), \
    .copy_fn  = (ev_copy_fn)COPY_FUNCTION(T, DEFAULT),   \
    .hash_fn  = (ev_hash_fn)HASH_FUNCTION(T, DEFAULT),   \
    .free_fn  = (ev_free_fn)FREE_FUNCTION(T, DEFAULT),   \
    .equal_fn = (ev_equal_fn)EQUAL_FUNCTION(T, DEFAULT), \
    .default_val = (void*)&(T){0}, \
    .invalid_val = (void*)&(T){0}, \
    EV_VA_OPT(__VA_ARGS__)(EV_FOREACH_UDATA(__EV_STRUCT_METHOD_DEF, T, __VA_ARGS__)) \
  }

#define __EV_STRUCT_METHOD_DEF(T, ...) EV_CAT(__EV_,EV_CAT(EV_HEAD __VA_ARGS__,_FN))(T, EV_TAIL __VA_ARGS__)

#define COPY(...)    (COPY   , __VA_ARGS__)
#define FREE(...)    (FREE   , __VA_ARGS__)
#define HASH(...)    (HASH   , __VA_ARGS__)
#define EQUAL(...)   (EQUAL  , __VA_ARGS__)
#define DEFAULT(...) (DEFAULT, __VA_ARGS__)
#define INVALID(...) (INVALID, __VA_ARGS__)

#define __EV_COPY_FN(T,name)  .copy_fn  = (ev_copy_fn) COPY_FUNCTION(T,name),
#define __EV_FREE_FN(T,name)  .free_fn  = (ev_free_fn) FREE_FUNCTION(T,name),
#define __EV_HASH_FN(T,name)  .hash_fn  = (ev_hash_fn) HASH_FUNCTION(T,name),
#define __EV_EQUAL_FN(T,name) .equal_fn = (ev_equal_fn)EQUAL_FUNCTION(T,name),
#define __EV_DEFAULT_FN(T, ...)  .default_val = (void*)&(T){ __VA_ARGS__ },
#define __EV_INVALID_FN(T, ...)  .invalid_val = (void*)&(T){ __VA_ARGS__ },

#define EV_COPY(T) TypeData(T).copy_fn
#define EV_FREE(T) TypeData(T).free_fn
#define EV_HASH(T) TypeData(T).hash_fn
#define EV_EQUAL(T) TypeData(T).equal_fn
#define EV_DEFAULT(T) *TypeData(T).default_val
#define EV_INVALID(T) *TypeData(T).invalid_val

#endif // EV_HEADERS_TYPES_H

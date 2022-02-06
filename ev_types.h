#ifndef EV_HEADERS_TYPES_H
#define EV_HEADERS_TYPES_H

#include "ev_macros.h"
#include "ev_internal.h"
#include "ev_hash.h"

typedef void(*ev_copy_fn)(void *dst, void *src);
typedef void(*ev_free_fn)(void *self);
typedef u64(*ev_hash_fn)(void *self, u64 seed);
typedef i32(*ev_cmp_fn)(void *self, void *other);

typedef struct {
  EV_DEBUG(const char *name;)

  u32 size;
  u32 alignment;

  ev_copy_fn copy_fn;
  ev_free_fn free_fn;
  ev_hash_fn hash_fn;
  ev_cmp_fn  cmp_fn;

  void *default_val;
  void *invalid_val;
} EvTypeData;

#define COPY_FUNCTION(T,name)  EV_CAT(EV_CAT(EV_CAT(EV_COPY_FUNCTION_,T),_),name)
#define FREE_FUNCTION(T,name)  EV_CAT(EV_CAT(EV_CAT(EV_FREE_FUNCTION_,T),_),name)
#define HASH_FUNCTION(T,name)  EV_CAT(EV_CAT(EV_CAT(EV_HASH_FUNCTION_,T),_),name)
#define CMP_FUNCTION(T,name)   EV_CAT(EV_CAT(EV_CAT(EV_CMP_FUNCTION_ ,T),_),name)

#include <string.h>
#define DEFINE_COPY_FUNCTION(T,name) void COPY_FUNCTION(T,name)(T *dst, T *src)
#define DEFINE_DEFAULT_INTERNAL_COPY_FUNCTION(T) \
  static inline DEFINE_COPY_FUNCTION(T,__EV_INTERNAL) { memcpy(dst, src, sizeof(T)); }

#define DEFINE_FREE_FUNCTION(T,name) void FREE_FUNCTION(T,name)(T *self)
#define DEFINE_DEFAULT_INTERNAL_FREE_FUNCTION(T) \
  static inline DEFINE_FREE_FUNCTION(T,__EV_INTERNAL) { (void)self; }

#define DEFINE_HASH_FUNCTION(T,name) u64 HASH_FUNCTION(T,name)(T *self, u64 seed)
#define DEFINE_DEFAULT_INTERNAL_HASH_FUNCTION(T) \
  static inline DEFINE_HASH_FUNCTION(T,__EV_INTERNAL) { return ev_hash_murmur3(self, sizeof(T), seed); }

#define DEFINE_CMP_FUNCTION(T,name) i32 CMP_FUNCTION(T,name)(T *self, T *other)
#define DEFINE_DEFAULT_INTERNAL_CMP_FUNCTION(T) \
  static inline DEFINE_CMP_FUNCTION(T,__EV_INTERNAL) { return memcmp(self, other, sizeof(T)); }

#define DECLARE_COPY_FUNCTION(T,name)  DEFINE_COPY_FUNCTION(T,name);
#define DECLARE_FREE_FUNCTION(T,name)  DEFINE_FREE_FUNCTION(T,name);
#define DECLARE_HASH_FUNCTION(T,name)  DEFINE_HASH_FUNCTION(T,name);
#define DECLARE_CMP_FUNCTION(T,name)   DEFINE_CMP_FUNCTION(T,name);

#define ACTIVE_COPY_FUNCTION(T)  __EV_TYPE_INTERNAL_##T##_ACTIVE_COPY_FN
#define ACTIVE_FREE_FUNCTION(T)  __EV_TYPE_INTERNAL_##T##_ACTIVE_FREE_FN
#define ACTIVE_HASH_FUNCTION(T)  __EV_TYPE_INTERNAL_##T##_ACTIVE_HASH_FN
#define ACTIVE_CMP_FUNCTION(T)   __EV_TYPE_INTERNAL_##T##_ACTIVE_CMP_FN

#define COPY_FUNCTION_TYPE(T) __EV_TYPE_INTERNAL_##T##_COPY_FN_TYPE
#define FREE_FUNCTION_TYPE(T) __EV_TYPE_INTERNAL_##T##_FREE_FN_TYPE
#define HASH_FUNCTION_TYPE(T) __EV_TYPE_INTERNAL_##T##_HASH_FN_TYPE
#define CMP_FUNCTION_TYPE(T)  __EV_TYPE_INTERNAL_##T##_CMP_FN_TYPE

#define DEFINE_FUNCTION_TYPES(T)                                                         \
  typedef void(*COPY_FUNCTION_TYPE(T))(T*,T*);                                           \
  typedef void(*FREE_FUNCTION_TYPE(T))(T*);                                              \
  typedef  u64(*HASH_FUNCTION_TYPE(T))(T*,u64);                                          \
  typedef  i32(*CMP_FUNCTION_TYPE(T))(T*,T*)

#define DEFINE_DEFAULT_INTERNAL_FUNCTIONS(T)                                             \
  DEFINE_DEFAULT_INTERNAL_COPY_FUNCTION(T)                                               \
  DEFINE_DEFAULT_INTERNAL_FREE_FUNCTION(T)                                               \
  DEFINE_DEFAULT_INTERNAL_HASH_FUNCTION(T)                                               \
  DEFINE_DEFAULT_INTERNAL_CMP_FUNCTION(T)

#define EV_OVERRIDE_VAR(T) EV_CAT(__ev_internal_override_var_,T)
#define TypeData(T) EV_CAT(EV_TYPEDATA_,T)
#define TYPEDATA_GEN(T, ...) \
  DEFINE_FUNCTION_TYPES(T);                                                              \
  DEFINE_DEFAULT_INTERNAL_FUNCTIONS(T);                                                  \
  static const COPY_FUNCTION_TYPE(T) ACTIVE_COPY_FUNCTION(T) = COPY_FUNCTION(T,__EV_INTERNAL); \
  static const FREE_FUNCTION_TYPE(T) ACTIVE_FREE_FUNCTION(T) = FREE_FUNCTION(T,__EV_INTERNAL); \
  static const HASH_FUNCTION_TYPE(T) ACTIVE_HASH_FUNCTION(T) = HASH_FUNCTION(T,__EV_INTERNAL); \
  static const CMP_FUNCTION_TYPE(T)  ACTIVE_CMP_FUNCTION(T)  = CMP_FUNCTION(T,__EV_INTERNAL);  \
  EV_VA_OPT(__VA_ARGS__)(EV_FOREACH_UDATA(__EV_STRUCT_METHOD_DEF, T, __VA_ARGS__)) \
  EV_WARNING_PUSH(); \
  EV_WARNING_DISABLE_GCC("override-init"); \
  EV_WARNING_DISABLE_CLANG("override-init"); \
  EV_UNUSED static const EvTypeData TypeData(T) = { \
    EV_DEBUG(.name = EV_STRINGIZE(T),) \
    .size = sizeof(T), \
    .alignment = EV_ALIGNOF(T), \
    .default_val = (void*)&(T){0}, \
    .invalid_val = (void*)&(T){0}, \
    .copy_fn = (ev_copy_fn)ACTIVE_COPY_FUNCTION(T), \
    .free_fn = (ev_free_fn)ACTIVE_FREE_FUNCTION(T), \
    .hash_fn = (ev_hash_fn)ACTIVE_HASH_FUNCTION(T), \
    .cmp_fn  = (ev_cmp_fn) ACTIVE_CMP_FUNCTION(T)   \
  }; \
  EV_WARNING_POP(); \
  EV_UNUSED static T EV_OVERRIDE_VAR(T)


#define COPY(...)    (COPY   , __VA_ARGS__)
#define FREE(...)    (FREE   , __VA_ARGS__)
#define HASH(...)    (HASH   , __VA_ARGS__)
#define CMP(...)     (CMP  , __VA_ARGS__)
#define DEFAULT(...) (DEFAULT, __VA_ARGS__)
#define INVALID(...) (INVALID, __VA_ARGS__)

#define __EV_STRUCT_METHOD_DEF(T, ...) EV_CAT(__EV_,EV_CAT(EV_HEAD __VA_ARGS__,_FN))(T, EV_TAIL __VA_ARGS__)
#define __EV_COPY_FN(T,name)  ACTIVE_COPY_FUNCTION(T) = (COPY_FUNCTION_TYPE(T)) COPY_FUNCTION(T,name);
#define __EV_FREE_FN(T,name)  ACTIVE_FREE_FUNCTION(T) = (FREE_FUNCTION_TYPE(T)) FREE_FUNCTION(T,name);
#define __EV_HASH_FN(T,name)  ACTIVE_HASH_FUNCTION(T) = (HASH_FUNCTION_TYPE(T)) HASH_FUNCTION(T,name);
#define __EV_CMP_FN(T,name)   ACTIVE_CMP_FUNCTION(T)  = (CMP_FUNCTION_TYPE(T))  CMP_FUNCTION(T,name);
#define __EV_DEFAULT_FN(T, ...)  .default_val = (void*)&(T){ __VA_ARGS__ },
#define __EV_INVALID_FN(T, ...)  .invalid_val = (void*)&(T){ __VA_ARGS__ },

#define EV_COPY(T) TypeData(T).copy_fn
#define EV_FREE(T) TypeData(T).free_fn
#define EV_HASH(T) TypeData(T).hash_fn
#define EV_CMP(T)  TypeData(T).cmp_fn
#define __EV_OVERRIDE_DEFAULT(T, ...) EV_OVERRIDE_VAR(T).__VA_ARGS__,
#define __EV_DEFAULT_INTERNAL(T) (*(T*)TypeData(T).default_val)
#define EV_DEFAULT(T, ...) EV_VA_OPT_ELSE(__VA_ARGS__) \
                              ((EV_OVERRIDE_VAR(T)=__EV_DEFAULT_INTERNAL(T), \
                                EV_FOREACH_UDATA(__EV_OVERRIDE_DEFAULT, T, __VA_ARGS__) \
                                EV_OVERRIDE_VAR(T))) \
                              (__EV_DEFAULT_INTERNAL(T))
#define EV_INVALID(T) (*(T*)TypeData(T).invalid_val)

#endif // EV_HEADERS_TYPES_H

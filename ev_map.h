/*!
 * \file ev_map.h
 */
#ifndef EV_MAP_HEADER
#define EV_MAP_HEADER
#include "ev_internal.h"
#include "ev_types.h"
#include "ev_numeric.h"
#include "ev_vec.h"

#define EV_MAP_MAGIC (0x65765F6D61705F74)

#if defined(EV_MAP_SHARED)
# if defined (EV_MAP_IMPL)
#  define EV_MAP_API EV_EXPORT
# else
#  define EV_MAP_API EV_IMPORT
# endif
#else
# define EV_MAP_API
#endif

#ifndef EV_MAP_INIT_CAP
/*!
 * \brief Initial capacity that is first reserved when a map is initialized
 */
#define EV_MAP_INIT_CAP 32
#endif

#ifndef EV_MAP_GROWTH_RATE
/*!
 * \brief Rate at which a map grows whenever a resize is needed
 */
#define EV_MAP_GROWTH_RATE 3 / 2
#endif

typedef struct {
  u64 _magic;
  u64 length;
  u64 capacity;
  u64 seed;
  EvTypeData keyTypeData;
  EvTypeData valTypeData;
  ev_vec_t keys;
  ev_vec_t vals;
} ev_map_t;

typedef enum {
  EV_MAP_ERR_NONE = 0,
  EV_MAP_ERR_OOM = 1
} ev_map_error_t;
TYPEDATA_GEN(ev_map_error_t, DEFAULT(EV_MAP_ERR_NONE));

#if defined(EV_MAP_SHORTNAMES)
# define map_t ev_map_t

# define map_error_t ev_map_error_t

# define map(K,V)  ev_map(K,V)

# define map_init        ev_map_init
/* # define vec_iter_begin  ev_vec_iter_begin */
/* # define vec_iter_end    ev_vec_iter_end */
/* # define vec_iter_next   ev_vec_iter_next */
/* # define vec_fini        ev_vec_fini */
/* # define vec_push        ev_vec_push */
/* # define vec_append      ev_vec_append */
/* # define vec_last        ev_vec_last */
/* # define vec_len         ev_vec_len */
/* # define vec_capacity    ev_vec_capacity */
/* # define vec_clear       ev_vec_clear */
/* # define vec_setlen      ev_vec_setlen */
/* # define vec_setcapacity ev_vec_setcapacity */
/* # define vec_grow        ev_vec_grow */
#endif

/*!
 * \brief For the sake of readability
 * \details Sample usage:
 * ```
 * ev_map(int,int) v = ev_map_init(int,int);
 * ```
 */
#define ev_map(K,V) struct { \
  u64 _magic;                \
  u64 length;                \
  u64 capacity;              \
  u64 seed;                  \
  EvTypeData keyTypeData;    \
  EvTypeData valTypeData;    \
  ev_vec(K) keys;            \
  ev_vec(V) vals;            \
}

/*!
 * \param typeData The EvTypeData for the element that the vector will contain
 *
 * \returns A vector object
 */
EV_MAP_API ev_map_t*
ev_map_init_impl(
  EvTypeData keyTypeData,
  EvTypeData valTypeData);

/*!
 * \brief Syntactic sugar for `ev_map_init_impl()`
 * \details Sample usage:
 * ```
 * ev_map_init(i32,i64);                   // ev_map_init_impl(TypeData(i32),TypeData(i64));
 * ```
 */
#define ev_map_init(K,V) (void*)ev_map_init_impl(TypeData(K), TypeData(V))

/*!
 * \brief Syntactic sugar for `ev_vec_push_impl()` that allows multiple pushed in the same statement.
 * \details Sample usage:
 * ```
 * ev_vec_t v;
 * i32 x = 1;
 * i32 y = 2;
 * ev_vec_push(&v, &x);                   // ev_vec_push_impl(&v, &x);
 * ev_vec_push(&v, &x, &y);               // ev_vec_push_impl(&v, &x); ev_vec_push_impl(&v, &y);
 * ```
 *
 * *Note* This is possibly replaceable with a variadic function.
 */
#define ev_map_add(m, ...) \
  EV_FOREACH_UDATA(__ev_map_internal_add, m, __VA_ARGS__);
#define __ev_map_internal_add(m, entry) ev_map_add_impl((ev_map_t*)m, EV_HEAD entry, EV_TAIL entry);


/*!
 * \brief A function that destroys a vector object. If the element type has a 
 * destructor function, then this function is called on every element before 
 * all reserved memory is freed.
 *
 * *Note*: For stack-allocated vectors (`svec`), destructors are called for 
 * elements but no memory is freed.
 *
 * \param v A pointer to the vector that is being destroyed
 */
EV_MAP_API void
ev_map_fini(
  ev_map_t* m);

EV_MAP_API void
ev_map_add_impl(
  ev_map_t* m,
  void* key,
  void* val
);

/*!
 * \brief Calls the free operation (if exists) on every entry, then sets
 * the length to 0.
 *
 * \param v A pointer to the map object
 *
 * \returns 0 on success
 */
EV_MAP_API u32
ev_map_clear(
  ev_map_t* m);

TYPEDATA_GEN(
    ev_map_t,
    INVALID(
      ._magic = ~0ULL
    ),
);

#define EV_MAP_IMPLEMENTATION

#ifdef EV_MAP_IMPLEMENTATION
#undef EV_MAP_IMPLEMENTATION

#ifdef EV_MAP_API_CHECK
#define EV_VEC_CHECK(x) do { x; } while (0)
#else
#define EV_MAP_CHECK(x)
#endif

#include <stdlib.h>
#include <string.h>

ev_map_t*
ev_map_init_impl(
  EvTypeData keyTypeData,
  EvTypeData valTypeData)
{
  ev_map_t* m = malloc(sizeof(ev_map_t));
  if(!m) return &EV_INVALID(ev_map_t);

  m->_magic = EV_MAP_MAGIC;
  m->keyTypeData = keyTypeData;
  m->valTypeData = valTypeData;
  m->keys = ev_vec_init_impl(keyTypeData, EV_DEFAULT(ev_vec_overrides_t));
  m->vals = ev_vec_init_impl(valTypeData, EV_DEFAULT(ev_vec_overrides_t));

  m->length = 0;

  assert(ev_vec_capacity(&m->keys) == ev_vec_capacity(&m->vals));

  m->capacity = ev_vec_capacity(&m->keys);

  return m;
}

void 
ev_map_fini(
  ev_map_t* m)
{
  ev_vec_fini(&m->keys);
  ev_vec_fini(&m->vals);
  free(m);
}

u64
_ev_map_hash(
  ev_map_t* m,
  void* key)
{
  u64 clipMask = 0xFFFFFFFFFFFF;
  if(m->keyTypeData.hash_fn != NULL)
    return m->keyTypeData.hash_fn(key, m->seed) & clipMask;
  return ev_hash_murmur3(key, m->keyTypeData.size, m->seed) & clipMask;
}

void 
ev_map_add_impl(
  ev_map_t* m,
  void* key,
  void* val)
{
  u64 hash = _ev_map_hash(m, key);
}

u32 
ev_map_clear(
  ev_map_t* m)
{
  m->length = 0;
  return ev_vec_clear(m->keys) | ev_vec_clear(m->vals);
}

#endif

#endif

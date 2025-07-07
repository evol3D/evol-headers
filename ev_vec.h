/*!
 * \file ev_vec.h
 */
#ifndef EV_VEC_HEADER
#define EV_VEC_HEADER
#include "ev_types.h"
#include "ev_numeric.h"

#if !EV_OS_WINDOWS
#include <string.h>
#endif

#if defined(EV_VEC_SHARED)
# if defined (EV_VEC_IMPL)
#  define EV_VEC_API EV_EXPORT
# else
#  define EV_VEC_API EV_IMPORT
# endif
#else
# define EV_VEC_API
#endif

#ifndef EV_VEC_INIT_CAP
/*!
 * \brief Initial capacity that is first reserved when a vector is initialized
 */
#define EV_VEC_INIT_CAP 32
#endif

#ifndef EV_VEC_GROWTH_RATE
/*!
 * \brief Rate at which a vector grows whenever a resize is needed
 */
#define EV_VEC_GROWTH_RATE 3 / 2
#endif

#if EV_CC_MSVC
# define __EV_VEC_EMPTY_ARRAY { 0 }
#else
# define __EV_VEC_EMPTY_ARRAY {   }
#endif

typedef void *ev_vec_t;
typedef void *ev_svec_t;

typedef enum {
  EV_VEC_ERR_NONE = 0,
  EV_VEC_ERR_OOM = 1
} ev_vec_error_t;
TYPEDATA_GEN(ev_vec_error_t, DEFAULT(EV_VEC_ERR_NONE));

typedef struct {
  ev_copy_fn copy;
  ev_equal_fn equal;
  ev_free_fn free;
  ev_tostr_fn tostr;
} ev_vec_overrides_t;
TYPEDATA_GEN(ev_vec_overrides_t);

#if defined(EV_VEC_SHORTNAMES)
# define vec_t  ev_vec_t
# define svec_t ev_svec_t

# define vec_error_t ev_vec_error_t

# define vec(T)  ev_vec(T)
# define svec(T) ev_svec(T)

# define vec_init        ev_vec_init
# define svec_init       ev_svec_init
# define svec_init_w_cap ev_svec_init_w_cap
# define svec_init_w_len ev_svec_init_w_len
# define vec_iter_begin  ev_vec_iter_begin
# define vec_iter_end    ev_vec_iter_end
# define vec_iter_next   ev_vec_iter_next
# define vec_fini        ev_vec_fini
# define vec_push        ev_vec_push
# define vec_append      ev_vec_append
# define vec_last        ev_vec_last
# define vec_len         ev_vec_len
# define vec_capacity    ev_vec_capacity
# define vec_clear       ev_vec_clear
# define vec_setlen      ev_vec_setlen
# define vec_setcapacity ev_vec_setcapacity
# define vec_grow        ev_vec_grow
#endif

/*!
 * \brief For the sake of readability
 * \details Sample usage:
 * ```
 * ev_vec(int) v = ev_vec_init(int);
 * ```
 */
#define ev_vec(T) T*

/*!
 * \brief For the sake of readability
 * \details Sample usage:
 * ```
 * ev_svec(int) v = ev_svec_init(int, { 0, 0 });
 * ```
 */
#define ev_svec(T) T*

#define EV_VEC_MAGIC (0x65765F7665635F74)

//! Metadata that is stored with a vector. Unique to each vector.
struct ev_vec_meta_t {
  u64 _magic;

  //! The number of elements in the vector.
  u64 length;
  //! The maximum length of the vector before it needs to be resized.
  u64 capacity;

  //! The type data of the elements
  EvTypeData typeData;

  enum {
      EV_VEC_ALLOCATION_TYPE_STACK,
      EV_VEC_ALLOCATION_TYPE_HEAP
  } allocationType;
};

/*!
 * \param typeData The EvTypeData for the element that the vector will contain
 *
 * \returns A vector object
 */
EV_VEC_API ev_vec_t
ev_vec_init_impl(
  EvTypeData typeData,
  ev_vec_overrides_t overrides);

/*!
 * \brief Syntactic sugar for `ev_vec_init_impl()`
 * \details Sample usage:
 * ```
 * ev_vec_init(i32);                   // ev_vec_init_impl(TypeData(i32));
 * ```
 */
#define ev_vec_init(T, ...) ev_vec_init_impl(TypeData(T), EV_DEFAULT(ev_vec_overrides_t,__VA_ARGS__))

#define ev_svec_init(T, ...) __ev_svec_init_impl(T, EV_ARRSIZE((T[])__VA_ARGS__), EV_ARRSIZE((T[])__VA_ARGS__), __VA_ARGS__)
#define ev_svec_init_w_cap(T, cap) __ev_svec_init_impl(T, 0, cap)
#define ev_svec_init_w_len(T, len) __ev_svec_init_impl(T, len, len)

static struct ev_vec_meta_t *__svec_interm_md;
#define __ev_svec_init_impl(T, len, cap, ...) (                                                       \
    EV_WARNING_PUSH()                                                                                 \
    EV_WARNING_DISABLE_CLANG("unsequenced")                                                           \
    __svec_interm_md = (void*)(u8[sizeof(T)*cap + sizeof(struct ev_vec_meta_t)]){},                   \
    *__svec_interm_md = (struct ev_vec_meta_t){                                                       \
      ._magic = EV_VEC_MAGIC,                                                                         \
      .length = len,                                                                                  \
      .capacity = cap,                                                                                \
      .typeData = TypeData(T),                                                                        \
      .allocationType = EV_VEC_ALLOCATION_TYPE_STACK,                                                 \
    },                                                                                                \
    EV_VA_OPT(__VA_ARGS__)(memcpy(&__svec_interm_md[1], (T[])__VA_ARGS__, sizeof((T[])__VA_ARGS__)),) \
    (void*)&(__svec_interm_md[1])                                                                     \
    EV_WARNING_POP()                                                                                  \
  )

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
#define ev_vec_push(v, ...) \
  EV_FOREACH_UDATA(__ev_vec_internal_push, v, __VA_ARGS__);
#define __ev_vec_internal_push(v, var) ev_vec_push_impl((ev_vec_t*)v, var);

/*!
 * \param vec_p A pointer to the vector that we want an iterator for
 *
 * \returns A pointer to the first element in a vector
 */
EV_VEC_API void *
ev_vec_iter_begin(
  const void* vec_p);

/*!
 * \param vec_p A pointer to the vector that we want an iterator for
 *
 * \returns A pointer to the memory block right after the last element in the vector
 */
EV_VEC_API void *
ev_vec_iter_end(
  const void* vec_p);

/*!
 * \brief A function that increments an iterator to make it point to the next
 * element in the vector
 *
 * \param vec_p A pointer to the vector that is being iterated over
 * \param iter Reference to the iterator that is being incremented
 */
EV_VEC_API void
ev_vec_iter_next(
  const void* vec_p, 
  void **iter);

/*!
 * \brief A function that looks for the index of `val` in `v`
 *
 * \param vec_p A pointer to the vector that is being iterated over
 * \param val A pointer to the object that will be compared with vector elements
 *
 * \returns If found, index of element in vector. Otherwise, -1.
 */
EV_VEC_API i32
ev_vec_find(
    const void* vec_p,
    void* val);

/*!
 * \brief A function that destroys a vector object. If the element type has a 
 * destructor function, then this function is called on every element before 
 * all reserved memory is freed.
 *
 * *Note*: For stack-allocated vectors (`svec`), destructors are called for 
 * elements but no memory is freed.
 *
 * \param vec_p A pointer to the vector that is being destroyed
 */
EV_VEC_API void
ev_vec_fini(
  void* vec_p);

/*!
 * \brief A function that copies a value to the end of a vector. If the element
 * type has a copy function, then this function is called to copy the new element 
 * into the vector. Otherwise, memcpy is used with a the element type size as a
 * copy length. If a resize is needed but fails due to 'OOM' issues, then the 
 * vector is left unchanged and VEC_ERR_OOM is returned.
 *
 * For `svec`, as long as the capacity is more than the current size, a push
 * operation is permitted. Otherwise, the operation is treated as an OOM.
 *
 * \param vec_p Reference to the vector object
 * \param val A pointer to the element that is to be copied to the end of the
 * vector
 *
 * \returns The index of the element that was just pushed. If the operation
 * failed, a non-zero (vec_error_t) value is returned.
 */
EV_VEC_API int
ev_vec_push_impl(
  void* vec_p,
  void *val);

/*!
 * \brief A function that appends the elements of an array to the end of a
 * vector.  If a resize is needed but fails due to 'OOM' issues, then the
 * vector is left unchanged and a VEC_ERR_OOM is returned.
 *
 * For `svec`, as long as the capacity is more than the current size by the
 * desired amount, an append operation is permitted. Otherwise, the operation 
 * is treated as an OOM.
 *
 * *NOTE* The type's copy function is not used; this is merely a memcpy
 * operation. If a deep copy is needed, individually pushing the elements of
 * the array is the way to go.
 *
 * \param vec_p Reference to the vector object
 * \param arr A pointer to the array that is to be copied to the end of the
 * vector
 * \param size Number of elements in the array
 *
 * \returns The index of the first element that was appended to the vector. If 
 * the operation failed, a non-zero (vec_error_t) value is returned.
 */
EV_VEC_API u32
ev_vec_append(
  void* vec_p,
  void **arr,
  u64 size);

/*!
 * \brief A function that copies the value at the end of a vector and removes
 * it from the vector. If a copy function was passed while initializing the 
 * vector, then this function is used. Otherwise, memcpy is used with a length 
 * of `vec_meta.elemsize`
 *
 * \param vec_p Reference to the vector object
 * \param out A pointer to the memory block at which the popped element will be
 * copied. If NULL is passed, then the element is destructed. Otherwise, the
 * element is copied to `out` and the receiving code is responsible for its
 * destruction.
 * 
 * \returns An error code. If the operation was successful, then `VEC_ERR_NONE` 
 * is returned.
 */
EV_VEC_API ev_vec_error_t
ev_vec_pop(
    void* vec_p, 
    void *out);

/*!
 * \brief A function that returns the last element in the vector.
 *
 * \param vec_p A pointer to the vector object
 * 
 * \returns Pointer to the last element in the vector. NULL if the vector is
 * empty.
 */
EV_VEC_API void *
ev_vec_last(
    const void* v);

/*!
 * \brief A function that returns the length of a vector
 *
 * \param vec_p A pointer to the vector object
 *
 * \returns Current length of the vector
 */
EV_VEC_API u64
ev_vec_len(
  void* vec_p);

/*!
 * \brief A function that returns the capacity of a vector
 *
 * \param vec_p A pointer to the vector object
 *
 * \returns Current capacity of the vector
 */
EV_VEC_API u64
ev_vec_capacity(
  const void* vec_p);

/*!
 * \brief Calls the free operation (if exists) on every element, then sets
 * the length to 0.
 *
 * \param vec_p A pointer to the vector object
 *
 * \returns 0 on success
 */
EV_VEC_API u32
ev_vec_clear(
  const void* vec_p);

/*!
 * \brief Sets the length of the vector to `len`.
 *
 * \details If `len` is less than `v`'s current length, then `v`'s length is
 * amended. Otherwise, the capacity is checked to make sure that there is enough
 * space for the new len.
 *
 * For `svec`, if the `len` is more than the already allocated capacity, it is
 * treated as an OOM.
 *
 * \param vec_p Reference to the vector object
 * \param len The desired new length
 *
 * \returns `VEC_ERR_NONE` on success
 */
EV_VEC_API ev_vec_error_t
ev_vec_setlen(
  void* vec_p, 
  u64 len);

/*!
 * \brief Sets the capacity of the vector to `cap`.
 *
 * \param vec_p Reference to the vector object
 * \param cap The desired new capacity
 *
 * For stack-allocated vectors, `VEC_ERR_OOM` is returned
 *
 * \returns `VEC_ERR_NONE` on success, `VEC_ERR_OOM` on OOM
 */
EV_VEC_API ev_vec_error_t
ev_vec_setcapacity(
  void* vec_p, 
  u64 cap);

/*!
 * \brief Grows the vector's capacity by a factor of `VEC_GROWTH_RATE`
 *
 * \param vec_p Reference to the vector object
 *
 * \returns `VEC_ERR_NONE` on success
 */
EV_VEC_API ev_vec_error_t
ev_vec_grow(
  void* vec_p);

static const ev_vec_t EV_VEC_EMPTY = 
  (ev_vec(i32))&((struct {
    struct ev_vec_meta_t meta;
    EV_ALIGNAS(EV_ALIGNOF(i32)) i32 data[0];
    }) {
      .meta._magic = EV_VEC_MAGIC,
      .meta.length = 0,
      .meta.capacity = 0,
      .meta.typeData = TypeData(i32),
      .meta.allocationType = EV_VEC_ALLOCATION_TYPE_STACK,
      .data = __EV_VEC_EMPTY_ARRAY
    }).data;

TYPEDATA_GEN(
    ev_vec_t,
    INVALID(EV_VEC_EMPTY)
);

TYPEDATA_GEN(
    ev_svec_t,
    INVALID(EV_VEC_EMPTY)
);


#ifdef EV_VEC_IMPLEMENTATION
#undef EV_VEC_IMPLEMENTATION


#ifdef EV_VEC_API_CHECK
#define EV_VEC_CHECK(x) do { x; } while (0)
#else
#define EV_VEC_CHECK(x)
#endif

#include <stdlib.h>
#include <string.h>

#define ev_vec_meta(v) \
  ((struct ev_vec_meta_t *)v) - 1

#define __ev_vec_getmeta(v) \
  struct ev_vec_meta_t *metadata = ((struct ev_vec_meta_t *)(v)) - 1; \
  assert(metadata->_magic == EV_VEC_MAGIC);

#define __ev_vec_syncmeta(v) \
  metadata = ((struct ev_vec_meta_t *)(v)) - 1;

ev_vec_t
ev_vec_init_impl(
  EvTypeData typeData,
  ev_vec_overrides_t overrides)
{
  void *v = malloc(sizeof(struct ev_vec_meta_t) + (EV_VEC_INIT_CAP * typeData.size));
  if (!v)
    return NULL;

  if(overrides.copy)
    typeData.copy_fn = overrides.copy;
  if(overrides.equal)
    typeData.equal_fn = overrides.equal;
  if(overrides.free)
    typeData.free_fn = overrides.free;
  if(overrides.tostr)
    typeData.tostr_fn = overrides.tostr;

  struct ev_vec_meta_t *metadata = (struct ev_vec_meta_t *)v;
  *metadata = (struct ev_vec_meta_t){
    ._magic = EV_VEC_MAGIC,
    .length   = 0,
    .capacity = EV_VEC_INIT_CAP,
    .allocationType = EV_VEC_ALLOCATION_TYPE_HEAP,
    .typeData = typeData
  };

  return metadata + 1;
}

i32
ev_vec_find(
    const void* vec_p,
    void *val)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)
  if(metadata->typeData.equal_fn) {
    for (void *elem = ev_vec_iter_begin(v); elem != ev_vec_iter_end(v); ev_vec_iter_next(v, &elem)) {
      if(metadata->typeData.equal_fn(elem, val))
      {
        return (elem - *v) / metadata->typeData.size;
      }
    }
  }
  else {
    for (void *elem = ev_vec_iter_begin(v); elem != ev_vec_iter_end(v); ev_vec_iter_next(v, &elem)) {
      if(memcmp(elem, val, metadata->typeData.size) == 0)
      {
        return (elem - *v) / metadata->typeData.size;
      }
    }
  }
  return -1;
}

void
ev_vec_fini(
    void* vec_p)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  if (metadata->typeData.free_fn) {
    for (void *elem = ev_vec_iter_begin(v); elem != ev_vec_iter_end(v);
         ev_vec_iter_next(v, &elem)) {
      metadata->typeData.free_fn(elem);
    }
  }
  if(metadata->allocationType == EV_VEC_ALLOCATION_TYPE_HEAP) {
    free(metadata);
  }

  *v = EV_INVALID(ev_vec_t);
}

int
ev_vec_push_impl(
    void* vec_p,
    void *val)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  if (metadata->length == metadata->capacity) {
    ev_vec_error_t grow_err = ev_vec_grow(v);
    if(grow_err) {
      return grow_err;
    } else {
      __ev_vec_syncmeta(*v)
    }
  }

  void *dst = ((char *)*v) + (metadata->length * metadata->typeData.size);
  if (metadata->typeData.copy_fn) {
    metadata->typeData.copy_fn(dst, val);
  } else {
    memcpy(dst, val, metadata->typeData.size);
  }

  return (int)metadata->length++;
}

void *
ev_vec_iter_begin(
    const void* vec_p)
{
  return *(void**)vec_p;
}

void *
ev_vec_iter_end(
    const void* vec_p)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  return ((char *)*v) + (metadata->typeData.size * metadata->length);
}

void
ev_vec_iter_next(
    const void* vec_p,
    void **iter)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)
  *iter = ((char*)*iter) + metadata->typeData.size;
}

EV_VEC_API u32
ev_vec_append(
  void* vec_p,
  void **arr,
  u64 size)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)
  size_t old_len = metadata->length;
  size_t req_len = old_len + size;

  ev_vec_error_t setlen_err = ev_vec_setlen(v, req_len);
  if(setlen_err) {
    return setlen_err;
  }
  __ev_vec_syncmeta(*v)

  void *dst = ((char *)*v) + (old_len * metadata->typeData.size);
  memcpy(dst, *arr, metadata->typeData.size * size);

  return (int)old_len;
}

ev_vec_error_t
ev_vec_pop(
    void* vec_p, 
    void *out)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  if(out != NULL) {
    void *src = ((char *)*v) + ((metadata->length-1) * metadata->typeData.size);
    if (metadata->typeData.copy_fn) {
      metadata->typeData.copy_fn(out, src);
    } else {
      memcpy(out, src, metadata->typeData.size);
    }
  } else {
    void *elem = ((char *)*v) + ((metadata->length-1) * metadata->typeData.size);
    if (metadata->typeData.free_fn) {
      metadata->typeData.free_fn(elem);
    }
  }

  metadata->length--;

  return EV_VEC_ERR_NONE;
}

void *
ev_vec_last(
    const void* vec_p)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  if(metadata->length == 0) {
    return NULL;
  }

  return ((char *)*v) + ((metadata->length-1) * metadata->typeData.size);
}

u64
ev_vec_len(
    void* vec_p)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)
  return metadata->length;
}

u64
ev_vec_capacity(
    const void* vec_p)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)
  return metadata->capacity;
}

u32
ev_vec_clear(
    const void* vec_p)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  if (metadata->typeData.free_fn) {
    for (void *elem = ev_vec_iter_begin(v); elem != ev_vec_iter_end(v);
         ev_vec_iter_next(v, &elem)) {
      metadata->typeData.free_fn(elem);
    }
  }

  metadata->length = 0;
  return 0;
}

ev_vec_error_t
ev_vec_setlen(
    void* vec_p, 
    u64 len)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  while(len > metadata->capacity) {
    ev_vec_error_t grow_err = ev_vec_grow(v);
    if(grow_err) {
      return grow_err;
    }
    __ev_vec_syncmeta(*v)
  }

  metadata->length = len;
  return EV_VEC_ERR_NONE;
}

ev_vec_error_t
ev_vec_setcapacity(
    void* vec_p, 
    u64 cap)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)

  if(metadata->allocationType == EV_VEC_ALLOCATION_TYPE_STACK) {
    return EV_VEC_ERR_OOM;
  }

  if(metadata->capacity == cap) {
    return EV_VEC_ERR_NONE;
  }

  void *buf = ((char *)(*v) - sizeof(struct ev_vec_meta_t));
  void *tmp = realloc(buf, sizeof(struct ev_vec_meta_t) + (cap * metadata->typeData.size));

  if (!tmp) {
    return EV_VEC_ERR_OOM;
  }

  if(buf != tmp) {
    buf = tmp;
    metadata           = (struct ev_vec_meta_t *)buf;
    *v = (char *)buf + sizeof(struct ev_vec_meta_t);
  }

  metadata->capacity = cap;
  return EV_VEC_ERR_NONE;
}

ev_vec_error_t
ev_vec_grow(
    void* vec_p)
{
  ev_vec_t* v = (ev_vec_t*)vec_p;
  __ev_vec_getmeta(*v)
  return ev_vec_setcapacity(v, metadata->capacity * EV_VEC_GROWTH_RATE);
}

#endif

#endif

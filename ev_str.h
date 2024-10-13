/*!
 * \file ev_str.h
 */
#ifndef EV_STR_HEADER
#define EV_STR_HEADER

#include "ev_types.h"
#include "ev_numeric.h"

#include <stddef.h>
#include <stdarg.h>

#define EV_STR_evstring_MAGIC (0x65767374)

#ifdef EV_STR_SHARED
    #if defined (EV_STR_IMPL)
        #define EV_STR_API EV_EXPORT
    #else
        #define EV_STR_API EV_IMPORT
    #endif
#else
    #define EV_STR_API
#endif

#if !defined(ev_str_malloc) && !defined(ev_str_free) && !defined(ev_str_realloc)
    #include <stdlib.h>
    #ifndef ev_str_malloc
        #define ev_str_malloc malloc
    #endif
    #ifndef ev_str_free
        #define ev_str_free free
    #endif
    #ifndef ev_str_realloc
        #define ev_str_realloc realloc
    #endif
#endif


#ifndef EV_STR_GROWTH_FACTOR
/*!
 * \brief Rate at which an evstring grows whenever a resize is needed
 */
#define EV_STR_GROWTH_FACTOR 3 / 2
#endif

typedef char *evstring;

typedef enum {
    EV_STR_ERR_NONE = 0,
    EV_STR_ERR_OOM = -1,
} evstring_error_t;
TYPEDATA_GEN(evstring_error_t, DEFAULT(EV_STR_ERR_NONE));

struct evstr_meta_t {
    EV_DEBUG(u64 magic;)
    u64 length;
    u64 size;
    enum {
        EV_STR_ALLOCATION_TYPE_STACK,
        EV_STR_ALLOCATION_TYPE_HEAP
    } allocationType;
};

#define __ev_strlen_const sizeof
#define evstr(str) __evstr_impl(str, __ev_strlen_const(str))
#define __evstr_impl(str, len) \
    (( struct { struct evstr_meta_t meta; char data[len]; } ) { \
        EV_DEBUG(.meta.magic = EV_STR_evstring_MAGIC,) \
        .meta.length = len-1, \
        .meta.size = len, \
        .meta.allocationType = EV_STR_ALLOCATION_TYPE_STACK, \
        .data = str \
    }).data

typedef struct evstring_view {
  evstring data;
  u64 offset;
  u64 len;
} evstring_view;

#define evstring_newGeneric(str) _Generic((str), \
        evstring_view: evstring_newFromView, \
        default: evstring_newFromStr\
        )(str)

#define evstring_new(str, ...) EV_VA_OPT_ELSE(__VA_ARGS__)(evstring_newFmt(str, __VA_ARGS__))(evstring_newGeneric(str))

#define evstring_pushGeneric(str, push) _Generic((push), \
        char: evstring_pushChar, \
        evstring_view: evstring_pushView, \
        default: evstring_pushStr \
        )(str, push)

#define evstring_push(str, push, ...) \
    EV_VA_OPT_ELSE(__VA_ARGS__)(evstring_pushFmt(str, push, __VA_ARGS__))(evstring_pushGeneric(str, push))

EV_STR_API evstring 
evstring_newFromStr(
    const char *str);

EV_STR_API evstring
evstring_newFromView(
    evstring_view v);

EV_STR_API void
evstring_free(
    evstring s);

EV_STR_API u64
evstring_getLength(
    const evstring s);

EV_STR_API evstring_error_t
evstring_setLength(
    evstring *s,
    size_t newLength);

EV_STR_API i32
evstring_cmp(
    const evstring s1,
    const evstring s2);

EV_STR_API evstring_error_t
evstring_pushChar(
    evstring *s,
    char c);

EV_STR_API evstring_error_t
evstring_pushStr(
    evstring *s,
    const char *data);

EV_STR_API evstring_error_t
evstring_pushFmt(
    evstring *s,
    const char *fmt,
    ...);

EV_STR_API evstring
evstring_newFromView(
    evstring_view view);

EV_STR_API evstring_view
evstring_slice(
    const evstring s,
    i64 begin,
    i64 end);

EV_STR_API i32
evstring_pushView(
    evstring *s,
    evstring_view ref);

EV_STR_API void
evstring_clear(
    evstring *s);

EV_STR_API evstring
evstring_newFmt(
    const char *fmt,
    ...);

EV_STR_API u64
evstring_findAll(
    const evstring text,
    const evstring query,
    evstring_view *results);

EV_STR_API evstring_view
evstring_findFirst(
    const evstring text,
    const evstring query);

evstring_view
__evstring_findFirst_impl(
    evstring_view text,
    evstring_view query);

EV_STR_API evstring
evstring_replaceFirst(
    const evstring text,
    const evstring query,
    const evstring replacement);

EV_STR_API i64
evstring_findFirstChar(
    const evstring text,
    const char c);

EV_STR_API i64
evstring_findLastChar(
    const evstring text,
    const char c);

DEFINE_EQUAL_FUNCTION(evstring, Default)
{
  return evstring_cmp(*(evstring*)self, *(evstring*)other) == 0;
}

TYPEDATA_GEN(evstring,
    EQUAL(Default)
);


#if defined(EV_STR_IMPLEMENTATION)

#if EV_OS_WINDOWS
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define META(s) (((struct evstr_meta_t *)(s)) - 1)

#if EV_BUILDTYPE_DEBUG || EV_BUILDTYPE_DEBUGOPT
#define evstr_asserttype(str) \
    assert(META(str)->magic == EV_STR_evstring_MAGIC)
#else
#define evstr_asserttype(str)
#endif

evstring_error_t
evstring_addSpace(
    evstring *s,
    u64 space);

evstring_error_t
evstring_pushFmt_v(
    evstring *s,
    const char *fmt,
    va_list args);

evstring
evstring_new_impl(
    const char *data,
    u64 len)
{
    u64 size = sizeof(struct evstr_meta_t) + len + 1;

    void *p = ev_str_malloc(size);
    assert(p); // Raised if malloc fails

    struct evstr_meta_t *meta = (struct evstr_meta_t *)p;
EV_DEBUG
(
    meta->magic = EV_STR_evstring_MAGIC;
)
    meta->length = len;
    meta->size = size;
    meta->allocationType = EV_STR_ALLOCATION_TYPE_HEAP;

    evstring s = (evstring)(meta + 1);
    if(len > 0) {
        memcpy(s, data, len);
    }
    s[len] = '\0';

    return s;
}

#include <stdio.h>

evstring
evstring_newFmt_v(
    const char *fmt,
    va_list args)
{
    va_list test;
    va_copy(test, args);
    i32 len = vsnprintf(NULL, 0, fmt, test);
    if(len < 0) {
        return EV_INVALID(evstring);
    }
    evstring res = evstring_new_impl(NULL, 0);
    evstring_setLength(&res, len);
    vsnprintf(res, len + 1, fmt, args);

    va_end(test);

    return res;
}

evstring
evstring_newFmt(
    const char *fmt,
    ...)
{
    va_list ap;
    va_start(ap, fmt);

    evstring res = evstring_newFmt_v(fmt, ap);

    va_end(ap);

    return res;
}

evstring
evstring_newFromStr(
    const char *str)
{
    u64 len = strlen(str);
    return evstring_new_impl(str, len);
}

evstring
evstring_newFromView(
    evstring_view v)
{
    return evstring_new_impl(v.data + v.offset, v.len);
}

void
evstring_free(
    evstring s)
{
    evstr_asserttype(s);
    if(META(s)->allocationType == EV_STR_ALLOCATION_TYPE_HEAP) {
        free(META(s));
    }
}

u64
evstring_getLength(
    const evstring s)
{
  evstr_asserttype(s);
  return META(s)->length;
}

evstring_error_t
evstring_setSize(
    evstring *s,
    size_t newsize)
{
    evstr_asserttype(*s);
    struct evstr_meta_t *meta = META(*s);
    if(meta->allocationType == EV_STR_ALLOCATION_TYPE_STACK) {
        return EV_STR_ERR_OOM;
    }

    if(meta->size == newsize) {
        return EV_STR_ERR_NONE;
    }

    void *buf = (void*)meta;
    void *tmp = ev_str_realloc(buf, sizeof(struct evstr_meta_t) + newsize);

    if (!tmp) {
        return EV_STR_ERR_OOM;
    }

    if(buf != tmp) { // Reallocation caused memory to be moved
        buf = tmp;
        meta = (struct evstr_meta_t *)buf;
        *s = (evstring)(meta+1);
    }

    meta->size = newsize;
    return EV_STR_ERR_NONE;
}

evstring_error_t
evstring_grow(
    evstring *s)
{
    evstr_asserttype(*s);
    return evstring_setSize(s, META(*s)->size * EV_STR_GROWTH_FACTOR);
}

evstring_error_t
evstring_setLength(
    evstring *s,
    size_t newlen)
{
    evstr_asserttype(*s);
    struct evstr_meta_t *meta = META(*s);
    if(newlen == meta->length) {
        return EV_STR_ERR_NONE;
    }

    u64 required_size = sizeof(struct evstr_meta_t) + newlen + 1;
    while(required_size > meta->size) {
        evstring_error_t grow_err = evstring_grow(s);
        if(grow_err) {
            return grow_err;
        }
        meta = META(*s);
    }
    meta->length = newlen;

    return EV_STR_ERR_NONE;
}

void
evstring_clear(
    evstring *s)
{
    evstr_asserttype(*s);
    evstring_setLength(s, 0);
}

i32
evstring_cmp(
    const evstring s1,
    const evstring s2)
{
    evstr_asserttype(s1);
    evstr_asserttype(s2);
    u64 len1 = evstring_getLength(s1);
    u64 len2 = evstring_getLength(s2);
    if(len1 != len2) {
        return 1;
    }
    return memcmp(s1, s2, len1);
}


evstring_error_t
evstring_push_impl(
    evstring *s,
    u64 sz,
    const char *data)
{
    evstr_asserttype(*s);
    struct evstr_meta_t *meta = META(*s);

    // TODO Find a more efficient approach?
    u64 required_size = sizeof(struct evstr_meta_t) + meta->length + sz + 1;
    while(required_size > meta->size) { // `<=` because of the null terminator
        evstring_error_t grow_err = evstring_grow(s);
        if(grow_err != EV_STR_ERR_NONE) {
            return grow_err;
        }
        meta = META(*s);
    }

    memcpy((*s) + meta->length, data, sz);
    // printf("Memcpy: dst = (*s {%p}) + meta->length {%llu}, src = data {%p}, size = sz {%llu}\n", *s, meta->length, data, sz);
    meta->length += sz;

    (*s)[meta->length] = '\0';
    return EV_STR_ERR_NONE;
}

evstring_error_t
evstring_pushChar(
    evstring *s,
    char c)
{
    evstr_asserttype(*s);
    return evstring_push_impl(s, 1, &c);
}

evstring_error_t
evstring_pushStr(
    evstring *s,
    const char *data)
{
    evstr_asserttype(*s);
    // TODO: Check that data is not within the range of *s
    return evstring_push_impl(s,strlen(data),data);
}

evstring_error_t
evstring_pushView(
    evstring *s,
    evstring_view v)
{
    evstr_asserttype(*s);
    assert(*s != v.data && " *s might be realloc'ed in a push operation. This would lead to the view pointing to a free'd block of memory.");
    return evstring_push_impl(s,v.len,v.data + v.offset);
}

evstring_view
evstring_slice(
    const evstring s,
    i64 begin,
    i64 end)
{
    evstr_asserttype(s);

    u64 string_len = evstring_getLength(s);
    u64 wrapped_begin = begin < 0 ? string_len + 1 + begin : begin;
    u64 wrapped_end   = end   < 0 ? string_len + 1 + end   : end;

    assert(wrapped_begin >= 0 && wrapped_begin < string_len);
    assert(wrapped_end > 0 && wrapped_end <= string_len);

    assert(wrapped_begin < wrapped_end);

    return (evstring_view) {
        .data = s,
        .offset = wrapped_begin, 
        .len = wrapped_end - wrapped_begin
    };
}

u64
evstring_getSpace(
    const evstring s)
{
    evstr_asserttype(s);
    struct evstr_meta_t *meta = META(s);
    return meta->size - meta->length - 1 - sizeof(struct evstr_meta_t);
}

evstring_error_t
evstring_addSpace(
    evstring *s,
    u64 space)
{
    evstr_asserttype(*s);
    return evstring_setSize(s, META(*s)->size + space);
}

evstring_view
__evstring_findFirst_impl(
    evstring_view text,
    evstring_view query)
{
    u64 found_progress = 0;

    evstring_view result = {
        .data = text.data,
        .len = 0,
        .offset = ~0ull
    };

    for(u64 i = text.offset; i < text.offset + text.len; i++) {
        if(text.data[i] == query.data[query.offset + found_progress]) {
            found_progress++;
        }
        if(found_progress == query.len) {
            result.offset = (i+1) - query.len;
            result.len = query.len;
            break;
        }
    }
    return result;
}

evstring_view
evstring_findFirst(
    const evstring text,
    const evstring query)
{
    evstr_asserttype(text);
    evstr_asserttype(query);
    return __evstring_findFirst_impl(evstring_slice(text, 0, -1), evstring_slice(query, 0, -1));
}

evstring
evstring_replaceFirst(
    const evstring text,
    const evstring query,
    const evstring replacement)
{
    evstr_asserttype(text);
    evstr_asserttype(query);
    evstr_asserttype(replacement);
    evstring result = NULL;

    evstring_view query_slice = evstring_findFirst(text, query);

    // If the query doesn't actually exist, then we're returning a clone of
    // the original string.
    if(query_slice.len == 0) {
        result = evstring_new_impl(text, evstring_getLength(text));
    } else {
        result = evstring_new_impl(NULL,0);

        // If the query doesn't match at the beginning of the string,
        // then we need to copy the data before it first.
        if(query_slice.offset != 0) {
            evstring_push(&result, evstring_slice(text, 0, query_slice.offset));
        }

        // Then, we simply push the replacement
        evstring_push_impl(&result, evstring_getLength(replacement), replacement);

        // Followed by the rest of the string
        evstring_push(&result,
            evstring_slice(text, query_slice.offset + query_slice.len, -1));
    }
    return result;
}

evstring_error_t
evstring_pushFmt(
    evstring *s,
    const char *fmt,
    ...)
{
    evstr_asserttype(*s);
    va_list ap;
    va_start(ap, fmt);
    evstring_error_t res = evstring_pushFmt_v(s, fmt, ap);
    va_end(ap);
    return res;
}

evstring_error_t
evstring_pushFmt_v(
    evstring *s,
    const char *fmt,
    va_list args)
{
    evstr_asserttype(*s);
    va_list test;
    va_copy(test, args);
    int fmt_len = vsnprintf(NULL, 0, fmt, test);
    size_t old_len = evstring_getLength(*s);
    evstring_error_t res = evstring_setLength(s, old_len + fmt_len);
    if(res == EV_STR_ERR_NONE) {
        int write_res = vsnprintf((*s) + old_len, fmt_len+1, fmt, args);
        assert(write_res > 0);
        assert(write_res == fmt_len);
    }
    va_end(test);
    return res;
}

i64
evstring_findFirstChar(
    const evstring text,
    const char c)
{
    evstr_asserttype(text);
    struct evstr_meta_t *meta = META(text);
    for(int i = 0; i < meta->length; i++) {
        if(text[i] == c) {
            return i;
        }
    }
    return -1;
}

i64
evstring_findLastChar(
    const evstring text,
    const char c)
{
    evstr_asserttype(text);
    struct evstr_meta_t *meta = META(text);
    int i;
    for(i = meta->length - 1; i >= 0; i--) {
        if(text[i] == c) {
            break;
        }
    }
    return i; 
}

u64
evstring_findAll(
    const evstring text,
    const evstring query,
    evstring_view *results)
{
    evstr_asserttype(text);
    evstr_asserttype(query);

    u64 text_len = evstring_getLength(text);
    u64 query_len = evstring_getLength(query);
    if(text_len == 0 || query_len == 0 || query_len > text_len) {
        return 0;
    }
    bool check_run = (results == NULL);
    u64 count = 0;
    for(evstring_view v = evstring_findFirst(text, query); 
            v.len != 0; 
            v = __evstring_findFirst_impl(
                evstring_slice(text, v.offset + v.len, -1), 
                evstring_slice(query, 0, -1))) {
        if(!check_run) {
            results[count++] = v;
        }
    }
    return count;
}

#endif

#endif

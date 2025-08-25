// ### BEGIN_FILE_INCLUDE: algorithm.h
#ifndef STC_ALGORITHM_H_INCLUDED
#define STC_ALGORITHM_H_INCLUDED

// IWYU pragma: begin_exports
// ### BEGIN_FILE_INCLUDE: crange.h
// IWYU pragma: private, include "stc/algorithm.h"
#ifndef STC_CRANGE_H_INCLUDED
#define STC_CRANGE_H_INCLUDED

// ### BEGIN_FILE_INCLUDE: linkage.h
#undef STC_API
#undef STC_DEF

#if !defined i_static && !defined STC_STATIC && (defined i_header || defined STC_HEADER  || \
                                                 defined i_implement || defined STC_IMPLEMENT)
  #define STC_API extern
  #define STC_DEF
#else
  #define i_implement
  #if defined __GNUC__ || defined __clang__ || defined __INTEL_LLVM_COMPILER
    #define STC_API static __attribute__((unused))
  #else
    #define STC_API static inline
  #endif
  #define STC_DEF static
#endif
#if defined STC_IMPLEMENT || defined i_import
  #define i_implement
#endif

#if defined i_aux && defined i_allocator
  #define _i_aux_alloc
#endif
#ifndef i_allocator
  #define i_allocator c
#endif
#ifndef i_free
  #define i_malloc c_JOIN(i_allocator, _malloc)
  #define i_calloc c_JOIN(i_allocator, _calloc)
  #define i_realloc c_JOIN(i_allocator, _realloc)
  #define i_free c_JOIN(i_allocator, _free)
#endif

#if defined __clang__ && !defined __cplusplus
  #pragma clang diagnostic push
  #pragma clang diagnostic warning "-Wall"
  #pragma clang diagnostic warning "-Wextra"
  #pragma clang diagnostic warning "-Wpedantic"
  #pragma clang diagnostic warning "-Wconversion"
  #pragma clang diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma clang diagnostic ignored "-Wmissing-field-initializers"
#elif defined __GNUC__ && !defined __cplusplus
  #pragma GCC diagnostic push
  #pragma GCC diagnostic warning "-Wall"
  #pragma GCC diagnostic warning "-Wextra"
  #pragma GCC diagnostic warning "-Wpedantic"
  #pragma GCC diagnostic warning "-Wconversion"
  #pragma GCC diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma GCC diagnostic ignored "-Wclobbered"
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough=3"
  #pragma GCC diagnostic ignored "-Wstringop-overflow="
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
// ### END_FILE_INCLUDE: linkage.h
// ### BEGIN_FILE_INCLUDE: common.h
#ifndef STC_COMMON_H_INCLUDED
#define STC_COMMON_H_INCLUDED

#ifdef _MSC_VER
    #pragma warning(disable: 4116 4996) // unnamed type definition in parentheses
#endif
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef ptrdiff_t       isize;
#ifndef STC_NO_INT_DEFS
    typedef int8_t      int8;
    typedef uint8_t     uint8;
    typedef int16_t     int16;
    typedef uint16_t    uint16;
    typedef int32_t     int32;
    typedef uint32_t    uint32;
    typedef int64_t     int64;
    typedef uint64_t    uint64;
#endif
#if !defined STC_HAS_TYPEOF && (_MSC_FULL_VER >= 193933428 || \
    defined __GNUC__ || defined __clang__ || defined __TINYC__)
    #define STC_HAS_TYPEOF 1
#endif
#if defined __GNUC__
  #define c_GNUATTR(...) __attribute__((__VA_ARGS__))
#else
  #define c_GNUATTR(...)
#endif
#define STC_INLINE static inline c_GNUATTR(unused)
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR
#define c_NPOS INTPTR_MAX

// Macro overloading feature support
#define c_MACRO_OVERLOAD(name, ...) \
    c_JOIN(name ## _,c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_JOIN0(a, b) a ## b
#define c_JOIN(a, b) c_JOIN0(a, b)
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) _c_ARG_N args
#define _c_RSEQ_N 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
#define _c_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) N

// Saturated overloading
// #define foo(...) foo_I(__VA_ARGS__, c_COMMA_N(foo_3), c_COMMA_N(foo_2), c_COMMA_N(foo_1),)(__VA_ARGS__)
// #define foo_I(a,b,c, n, ...) c_TUPLE_AT_1(n, foo_n,)
#define c_TUPLE_AT_1(x,y,...) y
#define c_COMMA_N(x) ,x
#define c_EXPAND(...) __VA_ARGS__

// Select arg, e.g. for #define i_type A,B then c_GETARG(2, i_type) is B
#define c_GETARG(N, ...) c_ARG_##N(__VA_ARGS__,)
#define c_ARG_1(a, ...) a
#define c_ARG_2(a, b, ...) b
#define c_ARG_3(a, b, c, ...) c
#define c_ARG_4(a, b, c, d, ...) d

#define _i_new_n(T, n) ((T*)i_malloc((n)*c_sizeof(T)))
#define _i_new_zeros(T, n) ((T*)i_calloc(n, c_sizeof(T)))
#define _i_realloc_n(ptr, old_n, n) i_realloc(ptr, (old_n)*c_sizeof *(ptr), (n)*c_sizeof *(ptr))
#define _i_free_n(ptr, n) i_free(ptr, (n)*c_sizeof *(ptr))

#ifndef __cplusplus
    #define c_new(T, ...) ((T*)c_safe_memcpy(c_malloc(c_sizeof(T)), ((T[]){__VA_ARGS__}), c_sizeof(T)))
    #define c_literal(T) (T)
    #define c_make_array(T, ...) ((T[])__VA_ARGS__)
    #define c_make_array2d(T, N, ...) ((T[][N])__VA_ARGS__)
#else
    #include <new>
    #define c_new(T, ...) new (c_malloc(c_sizeof(T))) T(__VA_ARGS__)
    #define c_literal(T) T
    template<typename T, int M, int N> struct _c_Array { T data[M][N]; };
    #define c_make_array(T, ...) (_c_Array<T, 1, sizeof((T[])__VA_ARGS__)/sizeof(T)>{{__VA_ARGS__}}.data[0])
    #define c_make_array2d(T, N, ...) (_c_Array<T, sizeof((T[][N])__VA_ARGS__)/sizeof(T[N]), N>{__VA_ARGS__}.data)
#endif

#ifdef STC_ALLOCATOR
    #define c_malloc c_JOIN(STC_ALLOCATOR, _malloc)
    #define c_calloc c_JOIN(STC_ALLOCATOR, _calloc)
    #define c_realloc c_JOIN(STC_ALLOCATOR, _realloc)
    #define c_free c_JOIN(STC_ALLOCATOR, _free)
#else
    #define c_malloc(sz) malloc(c_i2u_size(sz))
    #define c_calloc(n, sz) calloc(c_i2u_size(n), c_i2u_size(sz))
    #define c_realloc(ptr, old_sz, sz) realloc(ptr, c_i2u_size(1 ? (sz) : (old_sz)))
    #define c_free(ptr, sz) ((void)(sz), free(ptr))
#endif

#define c_new_n(T, n) ((T*)c_malloc((n)*c_sizeof(T)))
#define c_free_n(ptr, n) c_free(ptr, (n)*c_sizeof *(ptr))
#define c_realloc_n(ptr, old_n, n) c_realloc(ptr, (old_n)*c_sizeof *(ptr), (n)*c_sizeof *(ptr))
#define c_delete_n(T, ptr, n) do { \
    T* _tp = ptr; isize _n = n, _i = _n; \
    while (_i--) T##_drop((_tp + _i)); \
    c_free(_tp, _n*c_sizeof(T)); \
} while (0)

#define c_static_assert(expr)   (void)sizeof(int[(expr) ? 1 : -1])
#if defined STC_NDEBUG || defined NDEBUG
    #define c_assert(expr)      (void)sizeof(expr)
#else
    #define c_assert(expr)      assert(expr)
#endif
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(Tp, p)     ((Tp)(1 ? (p) : (Tp)0))
#define c_litstrlen(literal)    (c_sizeof("" literal) - 1)
#define c_countof(a)            (isize)(sizeof(a)/sizeof 0[a])
#define c_arraylen(a)           c_countof(a) // [deprecated]?

// expect signed ints to/from these (use with gcc -Wconversion)
#define c_sizeof                (isize)sizeof
#define c_strlen(s)             (isize)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u_size(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u_size(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u_size(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u_size(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u_size(ilen))
// library internal, but may be useful in user code:
#define c_u2i_size(u)           (isize)(1 ? (u) : (size_t)1) // warns if u is signed
#define c_i2u_size(i)           (size_t)(1 ? (i) : -1)       // warns if i is unsigned
#define c_uless(a, b)           ((size_t)(a) < (size_t)(b))
#define c_safe_cast(T, From, x) ((T)(1 ? (x) : (From){0}))

// x, y are i_keyraw* type, which defaults to i_key*. vp is i_key* type.
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(vp)      c_hash_n(vp, sizeof *(vp))
#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

// non-owning char pointer
typedef const char* cstr_raw;
#define cstr_raw_cmp(x, y)      strcmp(*(x), *(y))
#define cstr_raw_eq(x, y)       (cstr_raw_cmp(x, y) == 0)
#define cstr_raw_hash(vp)       c_hash_str(*(vp))
#define cstr_raw_clone(v)       (v)
#define cstr_raw_drop(vp)       ((void)vp)

// Control block macros

// [deprecated]:
#define c_init(...) c_make(__VA_ARGS__)
#define c_forlist(...) for (c_items(_VA_ARGS__))
#define c_foritems(...) for (c_items(__VA_ARGS__))
#define c_foreach(...) for (c_each(__VA_ARGS__))
#define c_foreach_n(...) for (c_each_n(__VA_ARGS__))
#define c_foreach_kv(...) for (c_each_kv(__VA_ARGS__))
#define c_foreach_reverse(...) for (c_each_reverse(__VA_ARGS__))
#define c_forrange(...) for (c_range(__VA_ARGS__))
#define c_forrange32(...) for (c_range32(__VA_ARGS__))

// New:
#define c_each(...) c_MACRO_OVERLOAD(c_each, __VA_ARGS__)
#define c_each_3(it, C, cnt) \
    C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it)
#define c_each_4(it, C, start, end) \
    _c_each(it, C, start, (end).ref, _)

#define c_each_n(...) c_MACRO_OVERLOAD(c_each_n, __VA_ARGS__)
#define c_each_n_3(it, C, cnt) c_each_n_4(it, C, cnt, INTPTR_MAX)
#define c_each_n_4(it, C, cnt, n) \
    struct {C##_iter iter; C##_value* ref; isize size, index;} \
    it = {.iter=C##_begin(&cnt), .size=n}; (it.ref = it.iter.ref) && it.index < it.size; C##_next(&it.iter), ++it.index

#define c_each_reverse(...) c_MACRO_OVERLOAD(c_each_reverse, __VA_ARGS__)
#define c_each_reverse_3(it, C, cnt) /* works for stack, vec, queue, deque */ \
    C##_iter it = C##_rbegin(&cnt); it.ref; C##_rnext(&it)
#define c_each_reverse_4(it, C, start, end) \
    _c_each(it, C, start, (end).ref, _r)

#define _c_each(it, C, start, endref, rev) /* private */ \
    C##_iter it = (start), *_endref_##it = c_safe_cast(C##_iter*, C##_value*, endref) \
         ; it.ref != (C##_value*)_endref_##it; C##rev##next(&it)

#define c_each_kv(...) c_MACRO_OVERLOAD(c_each_kv, __VA_ARGS__)
#define c_each_kv_4(key, val, C, cnt) /* structured binding for maps */ \
    _c_each_kv(key, val, C, C##_begin(&cnt), NULL)
#define c_each_kv_5(key, val, C, start, end) \
    _c_each_kv(key, val, C, start, (end).ref)

#define _c_each_kv(key, val, C, start, endref) /* private */ \
    const C##_key *key = (const C##_key*)&key; key; ) \
    for (C##_mapped *val; key; key = NULL) \
    for (C##_iter _it_##key = start, *_endref_##key = c_safe_cast(C##_iter*, C##_value*, endref); \
         _it_##key.ref != (C##_value*)_endref_##key && (key = &_it_##key.ref->first, val = &_it_##key.ref->second); \
         C##_next(&_it_##key)

#define c_items(it, T, ...) \
    struct {T* ref; int size, index;} \
    it = {.ref=c_make_array(T, __VA_ARGS__), .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
    ; it.index < it.size ; ++it.ref, ++it.index

// c_range, c_range32: python-like int range iteration
#define c_range_t(...) c_MACRO_OVERLOAD(c_range_t, __VA_ARGS__)
#define c_range_t_3(T, i, stop) c_range_t_4(T, i, 0, stop)
#define c_range_t_4(T, i, start, stop) \
    T i=start, _c_end_##i=stop; i < _c_end_##i; ++i
#define c_range_t_5(T, i, start, stop, step) \
    T i=start, _c_inc_##i=step, _c_end_##i=(stop) - (_c_inc_##i > 0) \
    ; (_c_inc_##i > 0) == (i <= _c_end_##i) ; i += _c_inc_##i

#define c_range(...) c_MACRO_OVERLOAD(c_range, __VA_ARGS__)
#define c_range_1(stop) c_range_t_4(isize, _c_i1, 0, stop)
#define c_range_2(i, stop) c_range_t_4(isize, i, 0, stop)
#define c_range_3(i, start, stop) c_range_t_4(isize, i, start, stop)
#define c_range_4(i, start, stop, step) c_range_t_5(isize, i, start, stop, step)

#define c_range32(...) c_MACRO_OVERLOAD(c_range32, __VA_ARGS__)
#define c_range32_2(i, stop) c_range_t_4(int32_t, i, 0, stop)
#define c_range32_3(i, start, stop) c_range_t_4(int32_t, i, start, stop)
#define c_range32_4(i, start, stop, step) c_range_t_5(int32_t, i, start, stop, step)

// make container from a literal list
#define c_make(C, ...) \
    C##_from_n(c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// put multiple raw-type elements from a literal list into a container
#define c_put_items(C, cnt, ...) \
    C##_put_n(cnt, c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// drop multiple containers of same type
#define c_drop(C, ...) \
    do { for (c_items(_c_i2, C*, {__VA_ARGS__})) C##_drop(*_c_i2.ref); } while(0)

// RAII scopes
#define c_defer(...) \
    for (int _c_i3 = 0; _c_i3++ == 0; __VA_ARGS__)

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(init, deinit) \
    for (int _c_i4 = 0; _c_i4 == 0; ) for (init; _c_i4++ == 0; deinit)
#define c_with_3(init, condition, deinit) \
    for (int _c_i5 = 0; _c_i5 == 0; ) for (init; _c_i5++ == 0 && (condition); deinit)

// General functions

STC_INLINE void* c_safe_memcpy(void* dst, const void* src, isize size)
    { return dst ? memcpy(dst, src, (size_t)size) : NULL; }

#if INTPTR_MAX == INT64_MAX
    #define FNV_BASIS 0xcbf29ce484222325
    #define FNV_PRIME 0x00000100000001b3
#else
    #define FNV_BASIS 0x811c9dc5
    #define FNV_PRIME 0x01000193
#endif

STC_INLINE size_t c_basehash_n(const void* key, isize len) {
    const uint8_t* msg = (const uint8_t*)key;
    size_t h = FNV_BASIS, block = 0;

    while (len >= c_sizeof h) {
        memcpy(&block, msg, sizeof h);
        h ^= block;
        h *= FNV_PRIME;
        msg += c_sizeof h;
        len -= c_sizeof h;
    }
    while (len--) {
        h ^= *(msg++);
        h *= FNV_PRIME;
    }
    return h;
}

STC_INLINE size_t c_hash_n(const void* key, isize len) {
    uint64_t b8; uint32_t b4;
    switch (len) {
        case 8: memcpy(&b8, key, 8); return (size_t)(b8 * 0xc6a4a7935bd1e99d);
        case 4: memcpy(&b4, key, 4); return b4 * FNV_BASIS;
        default: return c_basehash_n(key, len);
    }
}

STC_INLINE size_t c_hash_str(const char *str) {
    const uint8_t* msg = (const uint8_t*)str;
    size_t h = FNV_BASIS;
    while (*msg) {
        h ^= *(msg++);
        h *= FNV_PRIME;
    }
    return h;
}

#define c_hash_mix(...) /* non-commutative hash combine */ \
    c_hash_mix_n(c_make_array(size_t, {__VA_ARGS__}), c_sizeof((size_t[]){__VA_ARGS__})/c_sizeof(size_t))

STC_INLINE size_t c_hash_mix_n(size_t h[], isize n) {
    for (isize i = 1; i < n; ++i) h[0] += h[0] ^ h[i];
    return h[0];
}

// generic typesafe swap
#define c_swap(xp, yp) do { \
    (void)sizeof((xp) == (yp)); \
    char _tv[sizeof *(xp)]; \
    void *_xp = xp, *_yp = yp; \
    memcpy(_tv, _xp, sizeof _tv); \
    memcpy(_xp, _yp, sizeof _tv); \
    memcpy(_yp, _tv, sizeof _tv); \
} while (0)

// get next power of two
STC_INLINE isize c_next_pow2(isize n) {
    n--;
    n |= n >> 1, n |= n >> 2;
    n |= n >> 4, n |= n >> 8;
    n |= n >> 16;
    #if INTPTR_MAX == INT64_MAX
    n |= n >> 32;
    #endif
    return n + 1;
}

STC_INLINE char* c_strnstrn(const char *str, isize slen, const char *needle, isize nlen) {
    if (nlen == 0) return (char *)str;
    if (nlen > slen) return NULL;
    slen -= nlen;
    do {
        if (*str == *needle && !c_memcmp(str, needle, nlen))
            return (char *)str;
        ++str;
    } while (slen--);
    return NULL;
}
#endif // STC_COMMON_H_INCLUDED
// ### END_FILE_INCLUDE: common.h

// crange: isize range -----

typedef isize crange_value;
typedef struct { crange_value start, end, step, value; } crange;
typedef struct { crange_value *ref, end, step; } crange_iter;

STC_INLINE crange crange_make_3(crange_value start, crange_value stop, crange_value step)
    { crange r = {start, stop - (step > 0), step}; return r; }

#define crange_make(...) c_MACRO_OVERLOAD(crange_make, __VA_ARGS__)
#define crange_make_1(stop) crange_make_3(0, stop, 1) // NB! arg is stop
#define crange_make_2(start, stop) crange_make_3(start, stop, 1)

STC_INLINE crange_iter crange_begin(crange* self) {
    self->value = self->start;
    crange_iter it = {&self->value, self->end, self->step};
    return it;
}

STC_INLINE void crange_next(crange_iter* it) {
    if ((it->step > 0) == ((*it->ref += it->step) > it->end))
        it->ref = NULL;
}

STC_INLINE crange_iter crange_advance(crange_iter it, size_t n) {
    if ((it.step > 0) == ((*it.ref += it.step*(isize)n) > it.end))
        it.ref = NULL;
    return it;
}

// iota: c++-like std::iota, use in iterations on-the-fly -----
// Note: c_iota() does not compile with c++, crange does.
#define c_iota(...) c_MACRO_OVERLOAD(c_iota, __VA_ARGS__)
#define c_iota_1(start) c_iota_3(start, INTPTR_MAX, 1) // NB! arg is start.
#define c_iota_2(start, stop) c_iota_3(start, stop, 1)
#define c_iota_3(start, stop, step) ((crange[]){crange_make_3(start, stop, step)})[0]


// crange32 -----

typedef int32_t crange32_value;
typedef struct { crange32_value start, end, step, value; } crange32;
typedef struct { crange32_value *ref, end, step; } crange32_iter;

STC_INLINE crange32 crange32_make_3(crange32_value start, crange32_value stop, crange32_value step)
    { crange32 r = {start, stop - (step > 0), step}; return r; }

#define crange32_make(...) c_MACRO_OVERLOAD(crange32_make, __VA_ARGS__)
#define crange32_make_1(stop) crange32_make_3(0, stop, 1) // NB! arg is stop
#define crange32_make_2(start, stop) crange32_make_3(start, stop, 1)

STC_INLINE crange32_iter crange32_begin(crange32* self) {
    self->value = self->start;
    crange32_iter it = {&self->value, self->end, self->step};
    return it;
}

STC_INLINE void crange32_next(crange32_iter* it) {
    if ((it->step > 0) == ((*it->ref += it->step) > it->end))
        it->ref = NULL;
}

STC_INLINE crange32_iter crange32_advance(crange32_iter it, uint32_t n) {
    if ((it.step > 0) == ((*it.ref += it.step*(int32_t)n) > it.end))
        it.ref = NULL;
    return it;
}

// ### BEGIN_FILE_INCLUDE: linkage2.h

#undef i_aux
#undef _i_aux_alloc

#undef i_allocator
#undef i_malloc
#undef i_calloc
#undef i_realloc
#undef i_free

#undef i_static
#undef i_header
#undef i_implement
#undef i_import

#if defined __clang__ && !defined __cplusplus
  #pragma clang diagnostic pop
#elif defined __GNUC__ && !defined __cplusplus
  #pragma GCC diagnostic pop
#endif
// ### END_FILE_INCLUDE: linkage2.h
#endif // STC_CRANGE_H_INCLUDE
// ### END_FILE_INCLUDE: crange.h
// ### BEGIN_FILE_INCLUDE: filter.h
// IWYU pragma: private, include "stc/algorithm.h"
#ifndef STC_FILTER_H_INCLUDED
#define STC_FILTER_H_INCLUDED


// ------- c_filter --------
#define c_flt_take(n) _flt_take(&fltbase, n)
#define c_flt_skip(n) (c_flt_counter() > (n))
#define c_flt_takewhile(pred) _flt_takewhile(&fltbase, pred)
#define c_flt_skipwhile(pred) (fltbase.sb[fltbase.sb_top++] |= !(pred))
#define c_flt_counter() (++fltbase.sn[++fltbase.sn_top])
#define c_flt_getcount() (fltbase.sn[fltbase.sn_top])
#define c_flt_map(expr) (_mapped = (expr), value = &_mapped)
#define c_flt_src _it.ref

#define c_filter(C, cnt, pred) \
    _c_filter(C, C##_begin(&cnt), _, pred)

#define c_filter_from(C, start, pred) \
    _c_filter(C, start, _, pred)

#define c_filter_reverse(C, cnt, pred) \
    _c_filter(C, C##_rbegin(&cnt), _r, pred)

#define c_filter_reverse_from(C, start, pred) \
    _c_filter(C, start, _r, pred)

#define _c_filter(C, start, rev, pred) do { \
    struct _flt_base fltbase = {0}; \
    C##_iter _it = start; \
    C##_value *value = _it.ref, _mapped = {0}; \
    for ((void)_mapped ; !fltbase.done & (_it.ref != NULL) ; \
         C##rev##next(&_it), value = _it.ref, fltbase.sn_top=0, fltbase.sb_top=0) \
      (void)(pred); \
} while (0)

// ------- c_filter_zip --------
#define c_filter_zip(...) c_MACRO_OVERLOAD(c_filter_zip, __VA_ARGS__)
#define c_filter_zip_4(C, cnt1, cnt2, pred) \
    c_filter_zip_5(C, cnt1, C, cnt2, pred)
#define c_filter_zip_5(C1, cnt1, C2, cnt2, pred) \
    _c_filter_zip(C1, C1##_begin(&cnt1), C2, C2##_begin(&cnt2), _, pred)

#define c_filter_reverse_zip(...) c_MACRO_OVERLOAD(c_filter_reverse_zip, __VA_ARGS__)
#define c_filter_reverse_zip_4(C, cnt1, cnt2, pred) \
    c_filter_reverse_zip_5(C, cnt1, C, cnt2, pred)
#define c_filter_reverse_zip_5(C1, cnt1, C2, cnt2, pred) \
    _c_filter_zip(C1, C1##_rbegin(&cnt1), C2, C2##_rbegin(&cnt2), _r, pred)

#define c_filter_pairwise(C, cnt, pred) \
    _c_filter_zip(C, C##_begin(&cnt), C, C##_advance(_it1, 1), _, pred)

#define c_flt_map1(expr) (_mapped1 = (expr), value1 = &_mapped1)
#define c_flt_map2(expr) (_mapped2 = (expr), value2 = &_mapped2)
#define c_flt_src1 _it1.ref
#define c_flt_src2 _it2.ref

#define _c_filter_zip(C1, start1, C2, start2, rev, pred) do { \
    struct _flt_base fltbase = {0}; \
    C1##_iter _it1 = start1; \
    C2##_iter _it2 = start2; \
    C1##_value* value1 = _it1.ref, _mapped1; (void)_mapped1; \
    C2##_value* value2 = _it2.ref, _mapped2; (void)_mapped2; \
    for (; !fltbase.done & (_it1.ref != NULL) & (_it2.ref != NULL); \
         C1##rev##next(&_it1), value1 = _it1.ref, C2##rev##next(&_it2), value2 = _it2.ref, \
         fltbase.sn_top=0, fltbase.sb_top=0) \
      (void)(pred); \
} while (0)

// ------- c_ffilter --------
// c_ffilter allows to execute imperative statements for each element
// in a for-loop, e.g., calling nested generic statements instead
// of defining a function/expression for it:
#define c_fflt_take(i, n) _flt_take(&i.base, n)
#define c_fflt_skip(i, n) (c_fflt_counter(i) > (n))
#define c_fflt_takewhile(i, pred) _flt_takewhile(&i.base, pred)
#define c_fflt_skipwhile(i, pred) (i.base.sb[i.base.sb_top++] |= !(pred))
#define c_fflt_counter(i) (++i.base.sn[++i.base.sn_top])
#define c_fflt_getcount(i) (i.base.sn[i.base.sn_top])
#define c_fflt_map(i, expr) (i.mapped = (expr), i.ref = &i.mapped)
#define c_fflt_src(i) i.iter.ref

#define c_forfilter(...) for (c_ffilter(__VA_ARGS__))
#define c_forfilter_from(...) for (c_ffilter_from(__VA_ARGS__))
#define c_forfilter_reverse(...) for (c_ffilter_reverse(__VA_ARGS__))
#define c_forfilter_reverse_from(...) for (c_ffilter_reverse_from(__VA_ARGS__))

#define c_ffilter(i, C, cnt, pred) \
    _c_ffilter(i, C, C##_begin(&cnt), _, pred)

#define c_ffilter_from(i, C, start, pred) \
    _c_ffilter(i, C, start, _, pred)

#define c_ffilter_reverse(i, C, cnt,pred) \
    _c_ffilter(i, C, C##_rbegin(&cnt), _r, pred)

#define c_ffilter_reverse_from(i, C, start, pred) \
    _c_ffilter(i, C, start, _r, pred)

#define _c_ffilter(i, C, start, rev, pred) \
    struct {C##_iter iter; C##_value *ref, mapped; struct _flt_base base;} \
    i = {.iter=start, .ref=i.iter.ref} ; !i.base.done & (i.iter.ref != NULL) ; \
    C##rev##next(&i.iter), i.ref = i.iter.ref, i.base.sn_top=0, i.base.sb_top=0) \
      if (!(pred)) ; else if (1

// ------------------------ private -------------------------
#ifndef c_NFILTERS
#define c_NFILTERS 20
#endif

struct _flt_base {
    uint8_t sn_top, sb_top;
    bool done, sb[c_NFILTERS];
    uint32_t sn[c_NFILTERS];
};

static inline bool _flt_take(struct _flt_base* base, uint32_t n) {
    uint32_t k = ++base->sn[++base->sn_top];
    base->done |= (k >= n);
    return n > 0;
}

static inline bool _flt_takewhile(struct _flt_base* base, bool pred) {
    bool skip = (base->sb[base->sb_top++] |= !pred);
    base->done |= skip;
    return !skip;
}

#endif // STC_FILTER_H_INCLUDED
// ### END_FILE_INCLUDE: filter.h
// ### BEGIN_FILE_INCLUDE: utility.h
// IWYU pragma: private, include "stc/algorithm.h"
#ifndef STC_UTILITY_H_INCLUDED
#define STC_UTILITY_H_INCLUDED

// --------------------------------
// c_find_if, c_find_reverse_if
// --------------------------------

#define c_find_if(...) c_MACRO_OVERLOAD(c_find_if, __VA_ARGS__)
#define c_find_if_4(C, cnt, outit_ptr, pred) \
    _c_find(C, C##_begin(&cnt), NULL, _, outit_ptr, pred)

#define c_find_if_5(C, start, finish, outit_ptr, pred) \
    _c_find(C, start, (finish).ref, _, outit_ptr, pred)

#define c_find_reverse_if(...) c_MACRO_OVERLOAD(c_find_reverse_if, __VA_ARGS__)
#define c_find_reverse_if_4(C, cnt, outit_ptr, pred) \
    _c_find(C, C##_rbegin(&cnt), NULL, _r, outit_ptr, pred)

#define c_find_reverse_if_5(C, rstart, rfinish, outit_ptr, pred) \
    _c_find(C, rstart, (rfinish).ref, _r, outit_ptr, pred)

// private
#define _c_find(C, start, endref, rev, outit_ptr, pred) do { \
    C##_iter* _out = outit_ptr; \
    const C##_value *value, *_endref = endref; \
    for (*_out = start; (value = _out->ref) != _endref; C##rev##next(_out)) \
        if (pred) goto c_JOIN(findif_, __LINE__); \
    _out->ref = NULL; c_JOIN(findif_, __LINE__):; \
} while (0)

// --------------------------------
// c_reverse
// --------------------------------

#define c_reverse_array(array, n) do { \
    typedef struct { char d[sizeof 0[array]]; } _etype; \
    _etype* _arr = (_etype *)(array); \
    for (isize _i = 0, _j = (n) - 1; _i < _j; ++_i, --_j) \
        c_swap(_arr + _i, _arr + _j); \
} while (0)

// Compiles with vec, stack, and deque, and cspan container types:
#define c_reverse(CntType, self) do { \
    CntType* _self = self; \
    for (isize _i = 0, _j = CntType##_size(_self) - 1; _i < _j; ++_i, --_j) \
        c_swap(CntType##_at_mut(_self, _i), CntType##_at_mut(_self, _j)); \
} while (0)

// --------------------------------
// c_erase_if
// --------------------------------

// Use with: list, hashmap, hashset, sortedmap, sortedset:
#define c_erase_if(C, cnt_ptr, pred) do { \
    C* _cnt = cnt_ptr; \
    const C##_value* value; \
    for (C##_iter _it = C##_begin(_cnt); (value = _it.ref); ) { \
        if (pred) _it = C##_erase_at(_cnt, _it); \
        else C##_next(&_it); \
    } \
} while (0)

// --------------------------------
// c_eraseremove_if
// --------------------------------

// Use with: stack, vec, deque, queue:
#define c_eraseremove_if(C, cnt_ptr, pred) do { \
    C* _cnt = cnt_ptr; \
    isize _n = 0; \
    const C##_value* value; \
    C##_iter _i, _it = C##_begin(_cnt); \
    while ((value = _it.ref) && !(pred)) \
        C##_next(&_it); \
    for (_i = _it; (value = _it.ref); C##_next(&_it)) { \
        if (pred) C##_value_drop(_cnt, _it.ref), ++_n; \
        else *_i.ref = *_it.ref, C##_next(&_i); \
    } \
    C##_adjust_end_(_cnt, -_n); \
} while (0)

// --------------------------------
// c_copy_to, c_copy_if
// --------------------------------

#define c_copy_to(...) c_MACRO_OVERLOAD(c_copy_to, __VA_ARGS__)
#define c_copy_to_3(C, outcnt_ptr, cnt) \
    _c_copy_if(C, outcnt_ptr, _, C, cnt, true)

#define c_copy_to_4(C_out, outcnt_ptr, C, cnt) \
    _c_copy_if(C_out, outcnt_ptr, _, C, cnt, true)

#define c_copy_if(...) c_MACRO_OVERLOAD(c_copy_if, __VA_ARGS__)
#define c_copy_if_4(C, outcnt_ptr, cnt, pred) \
    _c_copy_if(C, outcnt_ptr, _, C, cnt, pred)

#define c_copy_if_5(C_out, outcnt_ptr, C, cnt, pred) \
    _c_copy_if(C_out, outcnt_ptr, _, C, cnt, pred)

// private
#define _c_copy_if(C_out, outcnt_ptr, rev, C, cnt, pred) do { \
    C_out *_out = outcnt_ptr; \
    C _cnt = cnt; \
    const C##_value* value; \
    for (C##_iter _it = C##rev##begin(&_cnt); (value = _it.ref); C##rev##next(&_it)) \
        if (pred) C_out##_push(_out, C_out##_value_clone(_out, *_it.ref)); \
} while (0)

// --------------------------------
// c_all_of, c_any_of, c_none_of
// --------------------------------

#define c_all_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, !(pred)); \
    *(outbool_ptr) = _it.ref == NULL; \
} while (0)

#define c_any_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, pred); \
    *(outbool_ptr) = _it.ref != NULL; \
} while (0)

#define c_none_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, pred); \
    *(outbool_ptr) = _it.ref == NULL; \
} while (0)

// --------------------------------
// c_min, c_max, c_min_n, c_max_n
// --------------------------------
#define _c_minmax_call(fn, T, ...) \
   fn(c_make_array(T, {__VA_ARGS__}), c_sizeof((T[]){__VA_ARGS__})/c_sizeof(T))

#define c_min(...) _c_minmax_call(c_min_n, isize, __VA_ARGS__)
#define c_umin(...) _c_minmax_call(c_umin_n, size_t, __VA_ARGS__)
#define c_min32(...) _c_minmax_call(c_min32_n, int32_t, __VA_ARGS__)
#define c_fmin(...) _c_minmax_call(c_fmin_n, float, __VA_ARGS__)
#define c_dmin(...) _c_minmax_call(c_dmin_n, double, __VA_ARGS__)
#define c_max(...) _c_minmax_call(c_max_n, isize, __VA_ARGS__)
#define c_umax(...) _c_minmax_call(c_umax_n, size_t, __VA_ARGS__)
#define c_max32(...) _c_minmax_call(c_max32_n, int32_t, __VA_ARGS__)
#define c_fmax(...) _c_minmax_call(c_fmax_n, float, __VA_ARGS__)
#define c_dmax(...) _c_minmax_call(c_dmax_n, double, __VA_ARGS__)

#define _c_minmax_def(fn, T, opr) \
    static inline T fn(const T a[], isize n) { \
        T x = a[0]; \
        for (isize i = 1; i < n; ++i) if (a[i] opr x) x = a[i]; \
        return x; \
    }
_c_minmax_def(c_min32_n, int32_t, <)
_c_minmax_def(c_min_n, isize, <)
_c_minmax_def(c_umin_n, size_t, <)
_c_minmax_def(c_fmin_n, float, <)
_c_minmax_def(c_dmin_n, double, <)
_c_minmax_def(c_max32_n, int32_t, >)
_c_minmax_def(c_max_n, isize, >)
_c_minmax_def(c_umax_n, size_t, >)
_c_minmax_def(c_fmax_n, float, >)
_c_minmax_def(c_dmax_n, double, >)

#endif // STC_UTILITY_H_INCLUDED
// ### END_FILE_INCLUDE: utility.h
// ### BEGIN_FILE_INCLUDE: sumtype.h
#ifndef STC_SUMTYPE_H_INCLUDED
#define STC_SUMTYPE_H_INCLUDED


#define _c_EMPTY()
#define _c_LOOP_INDIRECTION() c_LOOP
#define _c_LOOP_END_1 ,_c_LOOP1
#define _c_LOOP0(f,T,x,...) f c_EXPAND((T, c_EXPAND x)) _c_LOOP_INDIRECTION _c_EMPTY()()(f,T,__VA_ARGS__)
#define _c_LOOP1(...)
#define _c_CHECK(x,...) c_TUPLE_AT_1(__VA_ARGS__,x,)
#define _c_E0(...) __VA_ARGS__
#define _c_E1(...) _c_E0(_c_E0(_c_E0(_c_E0(_c_E0(_c_E0(__VA_ARGS__))))))
#define _c_E2(...) _c_E1(_c_E1(_c_E1(_c_E1(_c_E1(_c_E1(__VA_ARGS__))))))
#define c_EVAL(...) _c_E2(_c_E2(_c_E2(__VA_ARGS__))) // currently supports up to 130 variants
#define c_LOOP(f,T,x,...) _c_CHECK(_c_LOOP0, c_JOIN(_c_LOOP_END_, c_NUMARGS(c_EXPAND x)))(f,T,x,__VA_ARGS__)


#define _c_enum_1(x,...) (x=__LINE__*1000, __VA_ARGS__)
#define _c_vartuple_tag(T, Tag, ...) Tag,
#define _c_vartuple_type(T, Tag, ...) typedef __VA_ARGS__ Tag##_type; typedef T Tag##_sumtype;
#define _c_vartuple_var(T, Tag, ...) struct { enum enum_##T tag; Tag##_type get; } Tag;

#define c_union(T, ...) \
    typedef union T T; \
    enum enum_##T { c_EVAL(c_LOOP(_c_vartuple_tag, T, _c_enum_1 __VA_ARGS__, (0),)) }; \
    c_EVAL(c_LOOP(_c_vartuple_type, T,  __VA_ARGS__, (0),)) \
    union T { \
        struct { enum enum_##T tag; } _any_; \
        c_EVAL(c_LOOP(_c_vartuple_var, T, __VA_ARGS__, (0),)) \
    }
#define c_sumtype c_union

#if defined STC_HAS_TYPEOF && STC_HAS_TYPEOF
    #define c_when(varptr) \
        for (__typeof__(varptr) _vp1 = (varptr); _vp1; _vp1 = NULL) \
        switch (_vp1->_any_.tag)

    #define c_is_2(Tag, x) \
        break; case Tag: \
        for (__typeof__(_vp1->Tag.get)* x = &_vp1->Tag.get; x; x = NULL)

    #define c_is_3(varptr, Tag, x) \
        false) ; else for (__typeof__(varptr) _vp2 = (varptr); _vp2; _vp2 = NULL) \
            if (c_is_variant(_vp2, Tag)) \
                for (__typeof__(_vp2->Tag.get) *x = &_vp2->Tag.get; x; x = NULL
#else
    typedef union { struct { int tag; } _any_; } _c_any_variant;
    #define c_when(varptr) \
        for (_c_any_variant* _vp1 = (_c_any_variant *)(varptr); \
             _vp1; _vp1 = NULL, (void)sizeof((varptr)->_any_.tag)) \
            switch (_vp1->_any_.tag)

    #define c_is_2(Tag, x) \
        break; case Tag: \
        for (Tag##_type *x = &((Tag##_sumtype *)_vp1)->Tag.get; x; x = NULL)

    #define c_is_3(varptr, Tag, x) \
        false) ; else for (Tag##_sumtype* _vp2 = c_const_cast(Tag##_sumtype*, varptr); _vp2; _vp2 = NULL) \
            if (c_is_variant(_vp2, Tag)) \
                for (Tag##_type *x = &_vp2->Tag.get; x; x = NULL
#endif

// Handling multiple tags with different payloads:
#define c_is(...) c_MACRO_OVERLOAD(c_is, __VA_ARGS__)
#define c_is_1(Tag) \
    break; case Tag:

#define c_or_is(Tag) \
    ; case Tag:

// Type checked multiple tags with same payload:
#define c_is_same(...) c_MACRO_OVERLOAD(c_is_same, __VA_ARGS__)
#define _c_chk(Tag1, Tag2) \
    case 1 ? Tag1 : sizeof((Tag1##_type*)0 == (Tag2##_type*)0):
#define c_is_same_2(Tag1, Tag2) \
    break; _c_chk(Tag1, Tag2) case Tag2:
#define c_is_same_3(Tag1, Tag2, Tag3) \
    break; _c_chk(Tag1, Tag2) _c_chk(Tag2, Tag3) case Tag3:
#define c_is_same_4(Tag1, Tag2, Tag3, Tag4) \
    break; _c_chk(Tag1, Tag2) _c_chk(Tag2, Tag3) _c_chk(Tag3, Tag4) case Tag4:

#define c_otherwise \
    break; default:

#define c_variant(Tag, ...) \
    (c_literal(Tag##_sumtype){.Tag={.tag=Tag, .get=__VA_ARGS__}})

#define c_is_variant(varptr, Tag) \
    ((varptr)->Tag.tag == Tag)

#define c_get_if(varptr, Tag) \
    (c_is_variant(varptr, Tag) ? &(varptr)->Tag.get : NULL)

#define c_variant_id(varptr) \
    ((int)(varptr)->_any_.tag)

#endif // STC_SUMTYPE_H_INCLUDED
// ### END_FILE_INCLUDE: sumtype.h
// IWYU pragma: end_exports

#endif // STC_ALGORITHM_H_INCLUDED
// ### END_FILE_INCLUDE: algorithm.h


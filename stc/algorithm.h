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

#if !defined i_static  && !defined STC_STATIC  && (defined i_header || defined STC_HEADER  || \
                                                   defined i_implement || defined STC_IMPLEMENT)
  #define STC_API extern
  #define STC_DEF
#else
  #define i_static
  #if defined __GNUC__ || defined __clang__
    #define STC_API static __attribute__((unused))
  #else
    #define STC_API static inline
  #endif
  #define STC_DEF static
#endif
#if defined STC_IMPLEMENT || defined i_import
  #define i_implement
#endif

#if defined STC_ALLOCATOR && !defined i_allocator
  #define i_allocator STC_ALLOCATOR
#elif !defined i_allocator
  #define i_allocator c
#endif
#ifndef i_malloc
  #define i_malloc c_JOIN(i_allocator, _malloc)
  #define i_calloc c_JOIN(i_allocator, _calloc)
  #define i_realloc c_JOIN(i_allocator, _realloc)
  #define i_free c_JOIN(i_allocator, _free)
#endif

#ifdef i_aux
  #define _i_aux_struct struct { i_aux } aux;
#else
  #define _i_aux_struct
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
  #pragma GCC diagnostic ignored "-Wuninitialized"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
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
#if defined __GNUC__ || defined __clang__
    #define STC_INLINE static inline __attribute((unused))
#else
    #define STC_INLINE static inline
#endif
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR
#define c_NPOS INTPTR_MAX

/* Macro overloading feature support based on: https://rextester.com/ONP80107 */
#define c_MACRO_OVERLOAD(name, ...) \
    c_JOIN(c_JOIN0(name,_),c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_JOIN0(a, b) a ## b
#define c_JOIN(a, b) c_JOIN0(a, b)
#define c_EXPAND(...) __VA_ARGS__
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) c_EXPAND(_c_ARG_N args)
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                 _14, _15, _16, N, ...) N

// Select, e.g. for #define i_type A,B then c_SELECT(_c_SEL22, i_type) is B
#define c_SELECT(X, ...) c_EXPAND(X(__VA_ARGS__)) // need c_EXPAND for MSVC
#define _c_SEL21(a, b) a
#define _c_SEL22(a, b) b
#define _c_SEL31(a, b, c) a
#define _c_SEL32(a, b, c) b
#define _c_SEL33(a, b, c) c

#define _i_malloc(T, n)     ((T*)i_malloc((n)*c_sizeof(T)))
#define _i_calloc(T, n)     ((T*)i_calloc((n), c_sizeof(T)))
#ifndef __cplusplus
    #define c_new(T, ...)       ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_literal(T)        (T)
    #define c_make_array(T, ...) ((T[])__VA_ARGS__)
    #define c_make_array2d(T, N, ...) ((T[][N])__VA_ARGS__)
#else
    #include <new>
    #define c_new(T, ...)       new (malloc(sizeof(T))) T(__VA_ARGS__)
    #define c_literal(T)        T
    template<typename T, int M, int N> struct _c_Array { T data[M][N]; };
    #define c_make_array(T, ...) (_c_Array<T, 1, sizeof((T[])__VA_ARGS__)/sizeof(T)>{{__VA_ARGS__}}.data[0])
    #define c_make_array2d(T, N, ...) (_c_Array<T, sizeof((T[][N])__VA_ARGS__)/sizeof(T[N]), N>{__VA_ARGS__}.data)
#endif
#ifndef c_malloc
    #define c_malloc(sz)        malloc(c_i2u_size(sz))
    #define c_calloc(n, sz)     calloc(c_i2u_size(n), c_i2u_size(sz))
    #define c_realloc(ptr, old_sz, sz) realloc(ptr, c_i2u_size(1 ? (sz) : (old_sz)))
    #define c_free(ptr, sz)     do { (void)(sz); free(ptr); } while(0)
#endif
#define c_new_n(T, n)           ((T*)c_calloc(n, c_sizeof(T)))
#define c_delete(T, ptr)        do { T* _tp = ptr; T##_drop(_tp); c_free(_tp, c_sizeof(T)); } while (0)
#define c_delete_n(T, ptr, n)   do { T* _tp = ptr; isize _n = n, _m = _n; \
                                     while (_n--) T##_drop((_tp + _n)); \
                                     c_free(_tp, _m*c_sizeof(T)); } while (0)

#define c_static_assert(expr)   (1 ? 0 : (int)sizeof(int[(expr) ? 1 : -1]))
#if defined STC_NDEBUG || defined NDEBUG
    #define c_assert(expr)      ((void)0)
#else
    #define c_assert(expr)      assert(expr)
#endif
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(Tp, p)     ((Tp)(1 ? (p) : (Tp)0))
#define c_litstrlen(literal)    (c_sizeof("" literal) - 1)
#define c_arraylen(a)           (isize)(sizeof(a)/sizeof 0[a])

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

// x, y are i_keyraw* type, which defaults to i_key*:
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(p)       c_hash_n(p, sizeof *(p))
#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

// non-owning c-string "class"
typedef const char* cstr_raw;
#define cstr_raw_cmp(xp, yp) strcmp(*(xp), *(yp))
#define cstr_raw_eq(xp, yp) (cstr_raw_cmp(xp, yp) == 0)
#define cstr_raw_hash(p) c_hash_str(*(p))
#define cstr_raw_clone(s) (s)
#define cstr_raw_drop(p) ((void)p)

// Control block macros

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it))
#define c_foreach_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _)

#define c_foreach_n(it, C, cnt, n) \
    for (isize it##_index=0, _c_n=n; _c_n; _c_n=0) \
    for (C##_iter it = C##_begin(&cnt); it.ref && it##_index < _c_n; C##_next(&it), ++it##_index)

#define c_foreach_reverse(...) c_MACRO_OVERLOAD(c_foreach_reverse, __VA_ARGS__)
#define c_foreach_reverse_3(it, C, cnt) /* works for stack, vec, queue, deque */ \
    for (C##_iter it = C##_rbegin(&cnt); it.ref; C##_rnext(&it))
#define c_foreach_reverse_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _r)

#define _c_foreach(it, C, start, endref, rev) /* private */ \
    for (C##_iter it = (start), *_endref = c_safe_cast(C##_iter*, C##_value*, endref) \
         ; it.ref != (C##_value*)_endref; C##rev##next(&it))

#define c_foreach_kv(...) c_MACRO_OVERLOAD(c_foreach_kv, __VA_ARGS__)
#define c_foreach_kv_4(key, val, C, cnt) /* structured binding for maps */ \
    _c_foreach_kv(key, val, C, C##_begin(&cnt), NULL)
#define c_foreach_kv_5(key, val, C, start, end) \
    _c_foreach_kv(key, val, C, start, (end).ref)

#define _c_foreach_kv(key, val, C, start, endref) /* private */ \
    for (const C##_key *key, **_c_k = &key; _c_k; ) \
    for (C##_mapped *val; _c_k; _c_k = NULL) \
    for (C##_iter _it = start, *_endref = c_safe_cast(C##_iter*, C##_value*, endref) ; \
         _it.ref != (C##_value*)_endref && (key = &_it.ref->first, val = &_it.ref->second); \
         C##_next(&_it))

#define c_foritems(it, T, ...) \
    for (struct {T* ref; int size, index;} \
         it = {.ref=c_make_array(T, __VA_ARGS__), .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
         ; it.index < it.size; ++it.ref, ++it.index)

// c_forrange, c_forrange32: python-like int range iteration
#define c_forrange_t(...) c_MACRO_OVERLOAD(c_forrange_t, __VA_ARGS__)
#define c_forrange_t_3(T, i, stop) c_forrange_t_4(T, i, 0, stop)
#define c_forrange_t_4(T, i, start, stop) \
    for (T i=start, _c_end=stop; i < _c_end; ++i)
#define c_forrange_t_5(T, i, start, stop, step) \
    for (T i=start, _c_inc=step, _c_end=(stop) - (_c_inc > 0) \
         ; (_c_inc > 0) == (i <= _c_end); i += _c_inc)

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_t_4(isize, _c_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_t_4(isize, i, 0, stop)
#define c_forrange_3(i, start, stop) c_forrange_t_4(isize, i, start, stop)
#define c_forrange_4(i, start, stop, step) c_forrange_t_5(isize, i, start, stop, step)

#define c_forrange32(...) c_MACRO_OVERLOAD(c_forrange32, __VA_ARGS__)
#define c_forrange32_2(i, stop) c_forrange_t_4(int32_t, i, 0, stop)
#define c_forrange32_3(i, start, stop) c_forrange_t_4(int32_t, i, start, stop)
#define c_forrange32_4(i, start, stop, step) c_forrange_t_5(int32_t, i, start, stop, step)

// deprecated/removed:
#define c_init(C, ...) c_make(C, __VA_ARGS__)                      // [deprecated]
#define c_forlist(...) c_foritems(_VA_ARGS__)                      // [deprecated]
#define c_forpair(...) 'c_forpair not_supported. Use c_foreach_kv' // [removed]

// make container from a literal list, and drop multiple containers of same type
#define c_make(C, ...) \
    C##_with_n(c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// push multiple elements from a literal list into a container
#define c_push(C, cnt, ...) \
    C##_put_n(cnt, c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// drop multiple containers of same type
#define c_drop(C, ...) \
    do { c_foritems (_c_i, C*, {__VA_ARGS__}) C##_drop(*_c_i.ref); } while(0)

// define function with "on-the-fly" defined return type (e.g. variant, optional)
#define c_func(name, args, RIGHTARROW, ...) \
    typedef __VA_ARGS__ name##_result; name##_result name args

// RAII scopes
#define c_defer(...) \
    for (int _c_i = 0; _c_i++ == 0; __VA_ARGS__)

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(init, deinit) \
    for (int _c_i = 0; _c_i == 0; ) for (init; _c_i++ == 0; deinit)
#define c_with_3(init, condition, deinit) \
    for (int _c_i = 0; _c_i == 0; ) for (init; _c_i++ == 0 && (condition); deinit)

// General functions

// substring in substring?
char* c_strnstrn(const char *str, isize slen, const char *needle, isize nlen);

// hashing
size_t c_basehash_n(const void* key, isize len);

STC_INLINE size_t c_hash_n(const void* key, isize len) {
    uint64_t b8; uint32_t b4;
    switch (len) {
        case 8: memcpy(&b8, key, 8); return (size_t)(b8 * 0xc6a4a7935bd1e99d);
        case 4: memcpy(&b4, key, 4); return b4 * (size_t)0xa2ffeb2f01000193;
        default: return c_basehash_n(key, len);
    }
}

STC_INLINE size_t c_hash_str(const char *str)
    { return c_basehash_n(str, c_strlen(str)); }

STC_INLINE size_t _chash_mix(size_t h[], int n) {
    for (int i = 1; i < n; ++i) h[0] += h[0] ^ h[i];
    return h[0];
}
#define c_hash_mix(...) /* non-commutative hash combine! */ \
    _chash_mix(c_make_array(size_t, {__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))

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
#endif // STC_COMMON_H_INCLUDED

#if !defined STC_COMMON_C_INCLUDED && defined STC_IMPLEMENT
#define STC_COMMON_C_INCLUDED

char* c_strnstrn(const char *str, isize slen, const char *needle, isize nlen) {
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

size_t c_basehash_n(const void* key, isize len) {
    size_t block = 0, hash = 0x811c9dc5;
    const uint8_t* msg = (const uint8_t*)key;
    while (len >= c_sizeof(size_t)) {
        memcpy(&block, msg, sizeof(size_t));
        hash = (hash ^ block) * (size_t)0x89bb179901000193;
        msg += c_sizeof(size_t);
        len -= c_sizeof(size_t);
    }
    c_memcpy(&block, msg, len);
    hash = (hash ^ block) * (size_t)0xb0340f4501000193;
    return hash ^ (hash >> 3);
}
#endif // STC_COMMON_C_INCLUDED
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

#undef i_allocator
#undef i_malloc
#undef i_calloc
#undef i_realloc
#undef i_free
#undef i_aux
#undef _i_aux_struct

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
#define c_flt_take(n) _flt_take(&_base, n)
#define c_flt_skip(n) (c_flt_counter() > (n))
#define c_flt_takewhile(pred) _flt_takewhile(&_base, pred)
#define c_flt_skipwhile(pred) (_base.sb[_base.sb_top++] |= !(pred))
#define c_flt_counter() (++_base.sn[++_base.sn_top])
#define c_flt_getcount() (_base.sn[_base.sn_top])
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
    struct _flt_base _base = {0}; \
    C##_iter _it = start; \
    C##_value *value = _it.ref, _mapped = {0}; \
    for ((void)_mapped ; !_base.done & (_it.ref != NULL) ; \
         C##rev##next(&_it), value = _it.ref, _base.sn_top=0, _base.sb_top=0) \
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
    struct _flt_base _base = {0}; \
    C1##_iter _it1 = start1; \
    C2##_iter _it2 = start2; \
    C1##_value* value1 = _it1.ref, _mapped1; (void)_mapped1; \
    C2##_value* value2 = _it2.ref, _mapped2; (void)_mapped2; \
    for (; !_base.done & (_it1.ref != NULL) & (_it2.ref != NULL); \
         C1##rev##next(&_it1), value1 = _it1.ref, C2##rev##next(&_it2), value2 = _it2.ref, \
         _base.sn_top=0, _base.sb_top=0) \
      (void)(pred); \
} while (0)

// ------- c_forfilter --------
// c_forfilter allows to execute imperative statements for each element
// as it is a for-loop, e.g., calling nested generic statements instead
// of defining a function/expression for it:
#define c_fflt_take(i, n) _flt_take(&i.base, n)
#define c_fflt_skip(i, n) (c_fflt_counter(i) > (n))
#define c_fflt_takewhile(i, pred) _flt_takewhile(&i.base, pred)
#define c_fflt_skipwhile(i, pred) (i.base.sb[i.base.sb_top++] |= !(pred))
#define c_fflt_counter(i) (++i.base.sn[++i.base.sn_top])
#define c_fflt_getcount(i) (i.base.sn[i.base.sn_top])
#define c_fflt_map(i, expr) (i.mapped = (expr), i.ref = &i.mapped)
#define c_fflt_src(i) i.iter.ref

#define c_forfilter(i, C, cnt, pred) \
    _c_forfilter(i, C, C##_begin(&cnt), _, pred)

#define c_forfilter_reverse(i, C, cnt,pred) \
    _c_forfilter(i, C, C##_rbegin(&cnt), _r, pred)

#define c_forfilter_from(i, C, start, pred) \
    _c_forfilter(i, C, start, _, pred)

#define c_forfilter_reverse_from(i, C, start, pred) \
    _c_forfilter(i, C, start, _r, pred)

#define _c_forfilter(i, C, start, rev, pred) \
    for (struct {C##_iter iter; C##_value *ref, mapped; struct _flt_base base;} \
         i = {.iter=start, .ref=i.iter.ref} ; !i.base.done & (i.iter.ref != NULL) ; \
         C##rev##next(&i.iter), i.ref = i.iter.ref, i.base.sn_top=0, i.base.sb_top=0) \
      if (!(pred)) ; else

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
// c_erase_if
// --------------------------------

// Use with: list, hmap, hset, smap, sset:
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
        if (pred) C##_value_drop(_it.ref), ++_n; \
        else *_i.ref = *_it.ref, C##_next(&_i); \
    } \
    C##_adjust_end_(_cnt, -_n); \
} while (0)

// ------------------------------------
// c_copy, c_copy_if, c_copy_reverse_if
// ------------------------------------

#define c_copy(...) c_MACRO_OVERLOAD(c_copy, __VA_ARGS__)
#define c_copy_3(C, cnt, outcnt_ptr) \
    _c_copy_if(C, cnt, _, C, outcnt_ptr, true)

#define c_copy_4(C, cnt, C_out, outcnt_ptr) \
    _c_copy_if(C, cnt, _, C_out, outcnt_ptr, true)

#define c_copy_reverse(...) c_MACRO_OVERLOAD(c_copy_reverse, __VA_ARGS__)
#define c_copy_reverse_3(C, cnt, outcnt_ptr) \
    _c_copy_if(C, cnt, _r, C, outcnt_ptr, true)

#define c_copy_reverse_4(C, cnt, C_out, outcnt_ptr) \
    _c_copy_if(C, cnt, _r, C_out, outcnt_ptr, true)


#define c_copy_if(...) c_MACRO_OVERLOAD(c_copy_if, __VA_ARGS__)
#define c_copy_if_4(C, cnt, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _, C, outcnt_ptr, pred)

#define c_copy_if_5(C, cnt, C_out, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _, C_out, outcnt_ptr, pred)

#define c_copy_reverse_if(...) c_MACRO_OVERLOAD(c_copy_reverse_if, __VA_ARGS__)
#define c_copy_reverse_if_4(C, cnt, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _r, C, outcnt_ptr, pred)

#define c_copy_reverse_if_5(C, cnt, C_out, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _r, C_out, outcnt_ptr, pred)

// private
#define _c_copy_if(C, cnt, rev, C_out, outcnt_ptr, pred) do { \
    C _cnt = cnt; \
    C_out *_out = outcnt_ptr; \
    const C##_value* value; \
    for (C##_iter _it = C##rev##begin(&_cnt); (value = _it.ref); C##rev##next(&_it)) \
        if (pred) C_out##_push(_out, C##_value_clone(*_it.ref)); \
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

#endif // STC_UTILITY_H_INCLUDED
// ### END_FILE_INCLUDE: utility.h
// ### BEGIN_FILE_INCLUDE: variant.h
#ifndef STC_VARIANT_H_INCLUDED
#define STC_VARIANT_H_INCLUDED


#define c_STRIP_PARENS(X) _c_STRIP_PARENS( _c_E1 X )
#define c_CALL(f, ...) f(__VA_ARGS__)
#define _c_STRIP_PARENS(X) X
#define _c_EMPTY()
#define _c_TUPLE_AT_1(x,y,...) y
#define _c_CHECK(x,...) _c_TUPLE_AT_1(__VA_ARGS__,x,)
#define _c_LOOP_END_1 ,_c_LOOP1
#define _c_LOOP_INDIRECTION() c_LOOP
#define _c_LOOP0(T,f,x,...) c_CALL(f, T, c_STRIP_PARENS(x)) _c_LOOP_INDIRECTION _c_EMPTY()() (T,f,__VA_ARGS__)
#define _c_LOOP1(...)
#define _c_E1(...) __VA_ARGS__
#define _c_E2(...) _c_E1(_c_E1(_c_E1(_c_E1(_c_E1(_c_E1(__VA_ARGS__))))))
#define c_EVAL(...) _c_E2(_c_E2(_c_E2(_c_E2(_c_E2(_c_E2(__VA_ARGS__))))))
#define c_LOOP(T,f,x,...) _c_CHECK(_c_LOOP0, c_JOIN(_c_LOOP_END_, c_NUMARGS x))(T,f,x,__VA_ARGS__)

#define _c_vartuple_tag(T, Value, type) Value##_vartag,
#define _c_vartuple_type(T, Value, type) typedef type Value##_vartype; typedef union T Value##_variant;
#define _c_vartuple_var(T, Value, type) struct { uint8_t tag; Value##_vartype var; } Value;

#define c_variant_type(T, ...) \
    typedef union T T; \
    c_EVAL(c_LOOP(T, _c_vartuple_type, __VA_ARGS__, (0))) \
    enum { T##_dummytag, c_EVAL(c_LOOP(T, _c_vartuple_tag, __VA_ARGS__, (0))) }; \
    union T { \
        struct { uint8_t tag; } _dummy; \
        c_EVAL(c_LOOP(T, _c_vartuple_var, __VA_ARGS__, (0))) \
    }

#define c_match(variant) \
    for (void *_match = (void *)(variant); _match != NULL; _match = NULL) \
    switch ((variant)->_dummy.tag)

#define c_of(Value, x) \
    break; case Value##_vartag: \
    for (Value##_vartype *x = &((Value##_variant *)_match)->Value.var; x != NULL; x = NULL)

#define c_otherwise \
    break; default:

#define c_variant(Value, ...) \
    ((Value##_variant){.Value={.tag=Value##_vartag, .var=__VA_ARGS__}})

#endif // STC_VARIANT_H_INCLUDED
// ### END_FILE_INCLUDE: variant.h
// IWYU pragma: end_exports

#endif // STC_ALGORITHM_H_INCLUDED
// ### END_FILE_INCLUDE: algorithm.h


// ### BEGIN_FILE_INCLUDE: random.h
#define i_header // external linkage of normal_dist by default.
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

#ifndef STC_RANDOM_H_INCLUDED
#define STC_RANDOM_H_INCLUDED

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

// Macro overloading feature support based on: https://rextester.com/ONP80107
#define c_MACRO_OVERLOAD(name, ...) \
    c_JOIN(c_JOIN0(name,_),c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_JOIN0(a, b) a ## b
#define c_JOIN(a, b) c_JOIN0(a, b)
#define c_EXPAND(...) __VA_ARGS__
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) _c_ARG_N args  // wrap c_EXPAND(..) for MSVC without /std:c11
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
#define _c_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,N,...) N

// Saturated overloading
// #define foo(...) foo_I(__VA_ARGS__, c_COMMA_N(foo_3), c_COMMA_N(foo_2), c_COMMA_N(foo_1),)(__VA_ARGS__)
// #define foo_I(a,b,c, n, ...) c_TUPLE_AT_1(n, foo_n,)
#define c_TUPLE_AT_1(x,y,...) y
#define c_COMMA_N(x) ,x

// Select arg, e.g. for #define i_type A,B then c_GETARG(2, i_type) is B
#define c_GETARG(N, ...) c_ARG_##N(__VA_ARGS__,) // wrap c_EXPAND(..) for MSVC without /std:c11
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

#define c_each_n(it, C, cnt, n) \
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

STC_INLINE size_t c_basehash_n(const void* key, isize len) {
    size_t block = 0, hash = 0x811c9dc5;
    const uint8_t* msg = (const uint8_t*)key;
    while (len > c_sizeof(size_t)) {
        memcpy(&block, msg, sizeof(size_t));
        hash = (hash ^ block) * (size_t)0x89bb179901000193;
        msg += c_sizeof(size_t);
        len -= c_sizeof(size_t);
    }
    c_memcpy(&block, msg, len);
    hash = (hash ^ block) * (size_t)0xb0340f4501000193;
    return hash ^ (hash >> 3);
}

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

#define c_hash_mix(...) /* non-commutative hash combine! */ \
    _chash_mix(c_make_array(size_t, {__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))

STC_INLINE size_t _chash_mix(size_t h[], int n) {
    for (int i = 1; i < n; ++i) h[0] += h[0] ^ h[i];
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

// ===== crand64 ===================================

typedef struct {
    uint64_t data[4];
} crand64;

typedef struct {
    double mean, stddev;
    double _next;
    int _has_next;
} crand64_normal_dist;

STC_API double crand64_normal(crand64_normal_dist* d);
STC_API double crand64_normal_r(crand64* rng, uint64_t stream, crand64_normal_dist* d);

#if INTPTR_MAX == INT64_MAX
  #define crandWS crand64
#else
  #define crandWS crand32
#endif

#define c_shuffle_seed(s) \
    c_JOIN(crandWS, _seed)(s)

#define c_shuffle_array(array, n) do { \
    typedef struct { char d[sizeof 0[array]]; } _etype; \
    _etype* _arr = (_etype *)(array); \
    for (isize _i = (n) - 1; _i > 0; --_i) { \
        isize _j = (isize)(c_JOIN(crandWS, _uint)() % (_i + 1)); \
        c_swap(_arr + _i, _arr + _j); \
    } \
} while (0)

// Compiles with vec, stack, and deque container types:
#define c_shuffle(CntType, self) do { \
    CntType* _self = self; \
    for (isize _i = CntType##_size(_self) - 1; _i > 0; --_i) { \
        isize _j = (isize)(c_JOIN(crandWS, _uint)() % (_i + 1)); \
        c_swap(CntType##_at_mut(_self, _i), CntType##_at_mut(_self, _j)); \
    } \
} while (0)

STC_INLINE void crand64_seed_r(crand64* rng, uint64_t seed) {
    uint64_t* s = rng->data;
    s[0] = seed*0x9e3779b97f4a7c15; s[0] ^= s[0] >> 30;
    s[1] = s[0]*0xbf58476d1ce4e5b9; s[1] ^= s[1] >> 27;
    s[2] = s[1]*0x94d049bb133111eb; s[2] ^= s[2] >> 31;
    s[3] = seed;
}

// Minimum period length 2^64 per stream. 2^63 streams (odd numbers only)
STC_INLINE uint64_t crand64_uint_r(crand64* rng, uint64_t stream) {
    uint64_t* s = rng->data;
    const uint64_t result = (s[0] ^ (s[3] += stream)) + s[1];
    s[0] = s[1] ^ (s[1] >> 11);
    s[1] = s[2] + (s[2] << 3);
    s[2] = ((s[2] << 24) | (s[2] >> 40)) + result;
    return result;
}

STC_INLINE double crand64_real_r(crand64* rng, uint64_t stream)
    { return (double)(crand64_uint_r(rng, stream) >> 11) * 0x1.0p-53; }

STC_INLINE crand64* _stc64(void) {
    static crand64 rng = {{0x9e3779bb07979af0,0x6f682616bae3641a,0xe220a8397b1dcdaf,0x1}};
    return &rng;
}

STC_INLINE void crand64_seed(uint64_t seed)
    { crand64_seed_r(_stc64(), seed); }

STC_INLINE crand64 crand64_from(uint64_t seed)
    { crand64 rng; crand64_seed_r(&rng, seed); return rng; }

STC_INLINE uint64_t crand64_uint(void)
    { return crand64_uint_r(_stc64(), 1); }

STC_INLINE double crand64_real(void)
    { return crand64_real_r(_stc64(), 1); }

// --- crand64_uniform ---

typedef struct {
    int64_t low;
    uint64_t range, threshold;
} crand64_uniform_dist;

STC_INLINE crand64_uniform_dist
crand64_make_uniform(int64_t low, int64_t high) {
    crand64_uniform_dist d = {low, (uint64_t)(high - low + 1)};
    d.threshold = (uint64_t)(0 - d.range) % d.range;
    return d;
}

// 128-bit multiplication
#if defined(__SIZEOF_INT128__)
    #define c_umul128(a, b, lo, hi) \
        do { __uint128_t _z = (__uint128_t)(a)*(b); \
             *(lo) = (uint64_t)_z, *(hi) = (uint64_t)(_z >> 64U); } while(0)
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    #define c_umul128(a, b, lo, hi) ((void)(*(lo) = _umul128(a, b, hi)))
#elif defined(__x86_64__)
    #define c_umul128(a, b, lo, hi) \
        asm("mulq %3" : "=a"(*(lo)), "=d"(*(hi)) : "a"(a), "rm"(b))
#endif

STC_INLINE int64_t
crand64_uniform_r(crand64* rng, uint64_t stream, crand64_uniform_dist* d) {
    uint64_t lo, hi;
    #ifdef c_umul128
        do { c_umul128(crand64_uint_r(rng, stream), d->range, &lo, &hi); } while (lo < d->threshold);
    #else
        do { lo = crand64_uint_r(rng, stream); hi = lo % d->range; } while (lo - hi > -d->range);
    #endif
    return d->low + (int64_t)hi;
}

STC_INLINE int64_t crand64_uniform(crand64_uniform_dist* d)
    { return crand64_uniform_r(_stc64(), 1, d); }

// ===== crand32 ===================================

typedef struct { uint32_t data[4]; } crand32;

STC_INLINE void crand32_seed_r(crand32* rng, uint32_t seed) {
    uint32_t* s = rng->data;
    s[0] = seed*0x9e3779b9; s[0] ^= s[0] >> 16;
    s[1] = s[0]*0x21f0aaad; s[1] ^= s[1] >> 15;
    s[2] = s[1]*0x735a2d97; s[2] ^= s[2] >> 15;
    s[3] = seed;
}

// Minimum period length 2^32 per stream. 2^31 streams (odd numbers only)
STC_INLINE uint32_t crand32_uint_r(crand32* rng, uint32_t stream) {
    uint32_t* s = rng->data;
    const uint32_t result = (s[0] ^ (s[3] += stream)) + s[1];
    s[0] = s[1] ^ (s[1] >> 9);
    s[1] = s[2] + (s[2] << 3);
    s[2] = ((s[2] << 21) | (s[2] >> 11)) + result;
    return result;
}

STC_INLINE double crand32_real_r(crand32* rng, uint32_t stream)
    { return crand32_uint_r(rng, stream) * 0x1.0p-32; }

STC_INLINE crand32* _stc32(void) {
    static crand32 rng = {{0x9e37e78e,0x6eab1ba1,0x64625032,0x1}};
    return &rng;
}

STC_INLINE void crand32_seed(uint32_t seed)
    { crand32_seed_r(_stc32(), seed); }

STC_INLINE crand32 crand32_from(uint32_t seed)
    { crand32 rng; crand32_seed_r(&rng, seed); return rng; }

STC_INLINE uint32_t crand32_uint(void)
    { return crand32_uint_r(_stc32(), 1); }

STC_INLINE double crand32_real(void)
    { return crand32_real_r(_stc32(), 1); }

// --- crand32_uniform ---

typedef struct {
    int32_t low;
    uint32_t range, threshold;
} crand32_uniform_dist;

STC_INLINE crand32_uniform_dist
crand32_make_uniform(int32_t low, int32_t high) {
    crand32_uniform_dist d = {low, (uint32_t)(high - low + 1)};
    d.threshold = (uint32_t)(0 - d.range) % d.range;
    return d;
}

STC_INLINE int32_t
crand32_uniform_r(crand32* rng, uint32_t stream, crand32_uniform_dist* d) {
    uint64_t r;
    do {
        r = crand32_uint_r(rng, stream) * (uint64_t)d->range;
    } while ((uint32_t)r < d->threshold);
    return d->low + (int32_t)(r >> 32);
}

STC_INLINE int64_t crand32_uniform(crand32_uniform_dist* d)
    { return crand32_uniform_r(_stc32(), 1, d); }

#endif // STC_RANDOM_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

#ifndef STC_RANDOM_C_INCLUDED
#define STC_RANDOM_C_INCLUDED
#include <math.h>

STC_DEF double
crand64_normal_r(crand64* rng, uint64_t stream, crand64_normal_dist* d) {
    double v1, v2, sq, rt;
    if (d->_has_next++ & 1)
        return d->_next*d->stddev + d->mean;
    do {
        // range (-1.0, 1.0):
        v1 = (double)((int64_t)crand64_uint_r(rng, stream) >> 11) * 0x1.0p-52;
        v2 = (double)((int64_t)crand64_uint_r(rng, stream) >> 11) * 0x1.0p-52;

        sq = v1*v1 + v2*v2;
    } while (sq >= 1.0 || sq == 0.0);
    rt = sqrt(-2.0 * log(sq) / sq);
    d->_next = v2*rt;
    return (v1*rt)*d->stddev + d->mean;
}

STC_DEF double crand64_normal(crand64_normal_dist* d)
    { return crand64_normal_r(_stc64(), 1, d); }

#endif // STC_RANDOM_C_INCLUDED
#endif // i_implement
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
// ### END_FILE_INCLUDE: random.h


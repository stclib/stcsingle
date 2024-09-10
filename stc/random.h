// ### BEGIN_FILE_INCLUDE: random.h
#define i_header // external linkage of normal_dist by default.
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

#define c_NPOS INTPTR_MAX
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR

#if defined __GNUC__ || defined __clang__
    #define STC_INLINE static inline __attribute((unused))
#else
    #define STC_INLINE static inline
#endif

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

#define _c_SEL21(a, b) a
#define _c_SEL22(a, b) b
#define _c_SEL31(a, b, c) a
#define _c_SEL32(a, b, c) b
#define _c_SEL33(a, b, c) c
#define c_SELECT(S, ...) c_EXPAND(S(__VA_ARGS__)) // c_EXPAND for MSVC

#ifndef __cplusplus
    #define _i_malloc(T, n)     ((T*)i_malloc((n)*c_sizeof(T)))
    #define _i_calloc(T, n)     ((T*)i_calloc(n, c_sizeof(T)))
    #define c_new(T, ...)       ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_literal(T)        (T)
#else
    #include <new>
    #define _i_malloc(T, n)     static_cast<T*>(i_malloc((n)*c_sizeof(T)))
    #define _i_calloc(T, n)     static_cast<T*>(i_calloc(n, c_sizeof(T)))
    #define c_new(T, ...)       new (malloc(sizeof(T))) T(__VA_ARGS__)
    #define c_literal(T)        T
#endif
#define c_new_n(T, n)           ((T*)malloc(sizeof(T)*c_i2u_size(n)))
#define c_malloc(sz)            malloc(c_i2u_size(sz))
#define c_calloc(n, sz)         calloc(c_i2u_size(n), c_i2u_size(sz))
#define c_realloc(p, old_sz, sz) realloc(p, c_i2u_size(1 ? (sz) : (old_sz)))
#define c_free(p, sz)           do { (void)(sz); free(p); } while(0)
#define c_delete(T, ptr)        do { T *_tp = ptr; T##_drop(_tp); free(_tp); } while (0)

#define c_static_assert(expr)   (1 ? 0 : (int)sizeof(int[(expr) ? 1 : -1]))
#if defined STC_NDEBUG || defined NDEBUG
    #define c_assert(expr)      ((void)0)
#else
    #define c_assert(expr)      assert(expr)
#endif
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(Tp, p)     ((Tp)(1 ? (p) : (Tp)0))

#define c_swap(xp, yp) do { \
    (void)sizeof((xp) == (yp)); \
    char _tv[sizeof *(xp)]; \
    void *_xp = xp, *_yp = yp; \
    memcpy(_tv, _xp, sizeof _tv); \
    memcpy(_xp, _yp, sizeof _tv); \
    memcpy(_yp, _tv, sizeof _tv); \
} while (0)

// use with gcc -Wconversion
typedef ptrdiff_t               isize;
#define c_sizeof                (isize)sizeof
#define c_strlen(s)             (isize)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u_size(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u_size(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u_size(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u_size(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u_size(ilen))
// Mostly library internal, but may be useful in user code:
#define c_u2i_size(u)           (isize)(1 ? (u) : (size_t)1) // warns if u is signed
#define c_i2u_size(i)           (size_t)(1 ? (i) : -1)       // warns if i is unsigned
#define c_uless(a, b)           ((size_t)(a) < (size_t)(b))
#define c_safe_cast(T, From, x) ((T)(1 ? (x) : (From){0}))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(p)       chash_n(p, sizeof *(p))

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

/* Function macros and others */

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (isize)(sizeof(a)/sizeof 0[a])

// Non-owning c-string "class"
typedef const char* cstr_raw;
#define cstr_raw_cmp(xp, yp) strcmp(*(xp), *(yp))
#define cstr_raw_eq(xp, yp) (cstr_raw_cmp(xp, yp) == 0)
#define cstr_raw_hash(p) chash_str(*(p))
#define cstr_raw_clone(s) (s)
#define cstr_raw_drop(p) ((void)p)

#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t chash_n(const void* key, isize len) {
    uint32_t u4; uint64_t u8;
    switch (len) {
        case 8: memcpy(&u8, key, 8); return u8*0xc6a4a7935bd1e99d;
        case 4: memcpy(&u4, key, 4); return u4*0xc6a4a7935bd1e99d;
        case 0: return 1;
    }
    const uint8_t *x = (const uint8_t*)key;
    uint64_t h = (uint64_t)*x << 7, n = (uint64_t)len >> 3;
    len &= 7;
    while (n--) {
        memcpy(&u8, x, 8), x += 8;
        h = (h ^ u8)*0xc6a4a7935bd1e99d;
    }
    while (len--) h = (h ^ *x++)*0x100000001b3;
    return h ^ c_ROTL(h, 26);
}

STC_INLINE uint64_t chash_str(const char *str)
    { return chash_n(str, c_strlen(str)); }

STC_INLINE uint64_t _chash_mix(uint64_t h[], int n) { // n > 0
    for (int i = 1; i < n; ++i) h[0] += h[0] ^ h[i]; // non-commutative!
    return h[0];
}

STC_INLINE char* cstrnstrn(const char *str, isize slen,
                            const char *needle, isize nlen) {
    if (!nlen) return (char *)str;
    if (nlen > slen) return NULL;
    slen -= nlen;
    do {
        if (*str == *needle && !c_memcmp(str, needle, nlen))
            return (char *)str;
        ++str;
    } while (slen--);
    return NULL;
}

STC_INLINE isize cnextpow2(isize n) {
    n--;
    n |= n >> 1, n |= n >> 2;
    n |= n >> 4, n |= n >> 8;
    n |= n >> 16;
    #if INTPTR_MAX == INT64_MAX
    n |= n >> 32;
    #endif
    return n + 1;
}
/* Control block macros */

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it))
#define c_foreach_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _)

#define c_foreach_index(k, it, C, cnt) \
    for (isize k = 0, _i=1; _i; _i=0) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it), ++k)

#define c_foreach_reverse(...) c_MACRO_OVERLOAD(c_foreach_reverse, __VA_ARGS__)
#define c_foreach_reverse_3(it, C, cnt) /* works for stack, vec, queue, deque */ \
    for (C##_iter it = C##_rbegin(&cnt); it.ref; C##_rnext(&it))
#define c_foreach_reverse_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _r)

#define _c_foreach(it, C, start, endref, rev) /* private */ \
    for (C##_iter it = (start), *_endref = c_safe_cast(C##_iter*, C##_value*, endref) \
         ; it.ref != (C##_value*)_endref; C##rev##next(&it))

#define c_foreach_kv(...) c_MACRO_OVERLOAD(c_foreach_kv, __VA_ARGS__)
#define _c_foreach_kv(key, val, C, start, endref) /* structured binding for maps */ \
    for (const C##_key *key, **_k = &key; _k; ) \
    for (C##_mapped *val; _k; _k = NULL) \
    for (C##_iter _it = start, *_endref = c_safe_cast(C##_iter*, C##_value*, endref) ; \
         _it.ref != (C##_value*)_endref && (key = &_it.ref->first, val = &_it.ref->second); \
         C##_next(&_it))

#define c_foreach_kv_4(key, val, C, cnt) \
        _c_foreach_kv(key, val, C, C##_begin(&cnt), NULL)
#define c_foreach_kv_5(key, val, C, start, end) \
        _c_foreach_kv(key, val, C, start, (end).ref)

#define c_forlist(...) 'c_forlist not_supported. Use c_foritems'   // [removed]
#define c_forpair(...) 'c_forpair not_supported. Use c_foreach_kv' // [removed]

// c_forrange: python-like indexed iteration
#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (isize i=start, _end=stop; i < _end; ++i)
#define c_forrange_4(i, start, stop, step) \
    for (isize i=start, _inc=step, _end=(isize)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
    #define c_init(C, ...) \
        C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))
    #define c_foritems(it, T, ...) \
        for (struct {T* ref; int size, index;} \
             it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define chash_mix(...) \
        _chash_mix((uint64_t[]){__VA_ARGS__}, c_NUMARGS(__VA_ARGS__))
#else
    #include <initializer_list>
    #include <array>
    template <class C, class T>
    inline C _from_n(C (*func)(const T[], isize), std::initializer_list<T> il)
        { return func(&*il.begin(), il.size()); }
    #define c_init(C, ...) _from_n<C,C##_raw>(C##_from_n, __VA_ARGS__)
    #define c_foritems(it, T, ...) \
        for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator ref; size_t size, index;} \
             it = {._il=__VA_ARGS__, .ref=it._il.begin(), .size=it._il.size()} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define chash_mix(...) \
        _chash_mix(std::array<uint64_t, c_NUMARGS(__VA_ARGS__)>{__VA_ARGS__}.data(), c_NUMARGS(__VA_ARGS__))
#endif

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(init, deinit) \
    for (int _i = 1; _i; ) for (init; _i; _i = 0, deinit) // thanks, tstanisl
#define c_with_3(init, condition, deinit) \
    for (int _i = 1; _i; ) for (init; _i && (condition); _i = 0, deinit)

#define c_defer(...) \
    for (int _i = 1; _i; _i = 0, __VA_ARGS__)

#define c_drop(C, ...) \
    do { c_foritems (_i, C*, {__VA_ARGS__}) C##_drop(*_i.ref); } while(0)

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
crand64_uniform_from(int64_t low, int64_t high) {
    crand64_uniform_dist d = {low, (uint64_t)(high - low + 1)};
    d.threshold = (uint64_t)(0 - d.range) % d.range;
    return d;
}

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
crand32_uniform_from(int32_t low, int32_t high) {
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
#if defined i_implement || defined i_static

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
// ### END_FILE_INCLUDE: random.h


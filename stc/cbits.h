// ### BEGIN_FILE_INCLUDE: cbits.h
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

#if defined __clang__ && !defined __cplusplus
  #pragma clang diagnostic push
  #pragma clang diagnostic warning "-Wall"
  #pragma clang diagnostic warning "-Wextra"
  #pragma clang diagnostic warning "-Wpedantic"
  #pragma clang diagnostic warning "-Wconversion"
  #pragma clang diagnostic warning "-Wdouble-promotion"
  #pragma clang diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma clang diagnostic ignored "-Wmissing-field-initializers"
#elif defined __GNUC__ && !defined __cplusplus
  #pragma GCC diagnostic push
  #pragma GCC diagnostic warning "-Wall"
  #pragma GCC diagnostic warning "-Wextra"
  #pragma GCC diagnostic warning "-Wpedantic"
  #pragma GCC diagnostic warning "-Wconversion"
  #pragma GCC diagnostic warning "-Wdouble-promotion"
  #pragma GCC diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
// ### END_FILE_INCLUDE: linkage.h
#ifndef STC_CBITS_H_INCLUDED
#define STC_CBITS_H_INCLUDED
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
#define _c_SEL(S, ...) S(__VA_ARGS__)

#ifndef __cplusplus
    #define _i_alloc(T)         ((T*)i_malloc(c_sizeof(T)))
    #define _i_new(T, ...)      ((T*)memcpy(_i_alloc(T), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_new(T, ...)       ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_LITERAL(T)        (T)
#else
    #include <new>
    #define _i_alloc(T)         static_cast<T*>(i_malloc(c_sizeof(T)))
    #define _i_new(T, ...)      new (_i_alloc(T)) T(__VA_ARGS__)
    #define c_new(T, ...)       new (malloc(sizeof(T))) T(__VA_ARGS__)
    #define c_LITERAL(T)        T
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
#define c_safe_cast(T, F, x)    ((T)(1 ? (x) : (F){0}))
#define c_swap(T, xp, yp)       do { T *_xp = xp, *_yp = yp, \
                                    _tv = *_xp; *_xp = *_yp; *_yp = _tv; } while (0)
// use with gcc -Wconversion
#define c_sizeof                (intptr_t)sizeof
#define c_strlen(s)             (intptr_t)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u_size(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u_size(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u_size(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u_size(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u_size(ilen))
#define c_u2i_size(u)           (intptr_t)(1 ? (u) : (size_t)1)
#define c_i2u_size(i)           (size_t)(1 ? (i) : -1)
#define c_uless(a, b)           ((size_t)(a) < (size_t)(b))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(d)       c_hash(d)
#define c_hash(d)               c_hash_n(d, sizeof *(d))

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

/* Function macros and others */

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (intptr_t)(sizeof(a)/sizeof 0[a])

// Non-owning c-string "class"
typedef const char* ccharptr;
#define ccharptr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define ccharptr_eq(xp, yp) (ccharptr_cmp(xp, yp) == 0)
#define ccharptr_hash(p) c_hash_str(*(p))
#define ccharptr_clone(s) (s)
#define ccharptr_drop(p) ((void)p)

#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t c_hash_n(const void* key, intptr_t len) {
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

STC_INLINE uint64_t c_hash_str(const char *str)
    { return c_hash_n(str, c_strlen(str)); }

STC_INLINE uint64_t _c_hash_mix(uint64_t h[], int n) { // n > 0
    for (int i = 1; i < n; ++i) h[0] ^= h[0] + h[i]; // non-commutative!
    return h[0];
}

STC_INLINE char* c_strnstrn(const char *str, intptr_t slen,
                              const char *needle, intptr_t nlen) {
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

STC_INLINE intptr_t c_next_pow2(intptr_t n) {
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
#define c_foreach_4(it, C, start, finish) \
    _c_foreach(it, C, start, (finish).ref, _)

#define c_foreach_reverse(...) c_MACRO_OVERLOAD(c_foreach_reverse, __VA_ARGS__)
#define c_foreach_reverse_3(it, C, cnt) /* works for stack, vec, queue, deq */ \
    for (C##_iter it = C##_rbegin(&cnt); it.ref; C##_rnext(&it))
#define c_foreach_reverse_4(it, C, start, finish) \
    _c_foreach(it, C, start, (finish).ref, _r)

#define _c_foreach(it, C, start, endref, rev) /* private */ \
    for (C##_iter it = (start), *_endref = c_safe_cast(C##_iter*, C##_value*, endref) \
         ; it.ref != (C##_value*)_endref; C##rev##next(&it))

#define c_foreach_n(it, C, cnt, N) /* iterate up to N items */ \
    for (struct {C##_iter iter; C##_value* ref; intptr_t index, n;} it = {.iter=C##_begin(&cnt), .n=N} \
         ; (it.ref = it.iter.ref) && it.index < it.n; C##_next(&it.iter), ++it.index)

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter iter; const C##_key* key; C##_mapped* val;} _ = {.iter=C##_begin(&cnt)} \
         ; _.iter.ref && (_.key = &_.iter.ref->first, _.val = &_.iter.ref->second) \
         ; C##_next(&_.iter))

// c_forrange: python-like indexed iteration
#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (intptr_t i=start, _end=stop; i < _end; ++i)
#define c_forrange_4(i, start, stop, step) \
    for (intptr_t i=start, _inc=step, _end=(intptr_t)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
    #define c_init(C, ...) \
        C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))
    #define c_forlist(it, T, ...) \
        for (struct {T* ref; int size, index;} \
             it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define c_hash_mix(...) \
        _c_hash_mix((uint64_t[]){__VA_ARGS__}, c_NUMARGS(__VA_ARGS__))
#else
    #include <initializer_list>
    #include <array>
    template <class C, class T>
    inline C _from_n(C (*func)(const T[], intptr_t), std::initializer_list<T> il)
        { return func(&*il.begin(), il.size()); }
    #define c_init(C, ...) _from_n<C,C##_raw>(C##_from_n, __VA_ARGS__)
    #define c_forlist(it, T, ...) \
        for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator ref; size_t size, index;} \
             it = {._il=__VA_ARGS__, .ref=it._il.begin(), .size=it._il.size()} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define c_hash_mix(...) \
        _c_hash_mix(std::array<uint64_t, c_NUMARGS(__VA_ARGS__)>{__VA_ARGS__}.data(), c_NUMARGS(__VA_ARGS__))
#endif

#define c_defer(...) \
    for (int _i = 1; _i; _i = 0, __VA_ARGS__)

#define c_scoped(...) c_MACRO_OVERLOAD(c_scoped, __VA_ARGS__)
#define c_scoped_2(declvar, drop) \
    for (declvar, *_i, **_ip = &_i; _ip; _ip = 0, drop)
#define c_scoped_3(declvar, pred, drop) \
    for (declvar, *_i, **_ip = &_i; _ip && (pred); _ip = 0, drop)
#define c_with c_scoped // [deprecated]

#define c_scope(...) c_MACRO_OVERLOAD(c_scope, __VA_ARGS__)
#define c_scope_2(init, drop) \
    for (int _i = (init, 1); _i; _i = 0, drop)
#define c_scope_3(init, pred, drop) \
    for (int _i = (init, 1); _i && (pred); _i = 0, drop)

#define c_drop(C, ...) \
    do { c_forlist (_i, C*, {__VA_ARGS__}) C##_drop(*_i.ref); } while(0)

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
#include <stdlib.h>
#include <string.h>

#if INTPTR_MAX == INT64_MAX
#define _gnu_popc(x) __builtin_popcountll(x)
#define _msc_popc(x) (int)__popcnt64(x)
#else
#define _gnu_popc(x) __builtin_popcount(x)
#define _msc_popc(x) (int)__popcnt(x)
#endif
#define _cbits_BN (8*c_sizeof(uintptr_t))
#define _cbits_bit(i) ((uintptr_t)1 << ((i) & (_cbits_BN - 1)))
#define _cbits_words(n) (intptr_t)(((n) + (_cbits_BN - 1))/_cbits_BN)
#define _cbits_bytes(n) (_cbits_words(n)*c_sizeof(uintptr_t))

#if defined _MSC_VER
  #include <intrin.h>
  STC_INLINE int c_popcount(uintptr_t x) { return _msc_popc(x); }
#elif defined __GNUC__ || defined __clang__
  STC_INLINE int c_popcount(uintptr_t x) { return _gnu_popc(x); }
#else
  STC_INLINE int c_popcount(uintptr_t x) { /* http://en.wikipedia.org/wiki/Hamming_weight */
    x -= (x >> 1) & (uintptr_t)0x5555555555555555;
    x = (x & (uintptr_t)0x3333333333333333) + ((x >> 2) & (uintptr_t)0x3333333333333333);
    x = (x + (x >> 4)) & (uintptr_t)0x0f0f0f0f0f0f0f0f;
    return (int)((x*(uintptr_t)0x0101010101010101) >> (_cbits_BN - 8));
  }
#endif
#if defined __GNUC__ && !defined __clang__ && !defined __cplusplus
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=" // gcc 11.4
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"     // gcc 11.4
#endif

STC_INLINE intptr_t _cbits_count(const uintptr_t* set, const intptr_t sz) {
    const intptr_t n = sz/_cbits_BN;
    intptr_t count = 0;
    for (intptr_t i = 0; i < n; ++i)
        count += c_popcount(set[i]);
    if (sz & (_cbits_BN - 1))
        count += c_popcount(set[n] & (_cbits_bit(sz) - 1));
    return count;
}

STC_INLINE char* _cbits_to_str(const uintptr_t* set, const intptr_t sz,
                               char* out, intptr_t start, intptr_t stop) {
    if (stop > sz) stop = sz;
    c_assert(start <= stop);

    c_memset(out, '0', stop - start);
    for (intptr_t i = start; i < stop; ++i)
        if ((set[i/_cbits_BN] & _cbits_bit(i)) != 0)
            out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}

#define _cbits_OPR(OPR, VAL) \
    const intptr_t n = sz/_cbits_BN; \
    for (intptr_t i = 0; i < n; ++i) \
        if ((set[i] OPR other[i]) != VAL) \
            return false; \
    if (!(sz & (_cbits_BN - 1))) \
        return true; \
    const uintptr_t i = (uintptr_t)n, m = _cbits_bit(sz) - 1; \
    return ((set[i] OPR other[i]) & m) == (VAL & m)

STC_INLINE bool _cbits_subset_of(const uintptr_t* set, const uintptr_t* other, const intptr_t sz)
    { _cbits_OPR(|, set[i]); }

STC_INLINE bool _cbits_disjoint(const uintptr_t* set, const uintptr_t* other, const intptr_t sz)
    { _cbits_OPR(&, 0); }

#endif // STC_CBITS_H_INCLUDED

#define _i_memb(name) c_JOIN(i_type, name)

#if !defined i_capacity // DYNAMIC SIZE BITARRAY

#define _i_assert(x) c_assert(x)
#define i_type cbits

typedef struct { uintptr_t *buffer; intptr_t _size; } i_type;

STC_INLINE cbits cbits_init(void) { return c_LITERAL(cbits){NULL}; }
STC_INLINE void cbits_drop(cbits* self) { i_free(self->buffer, _cbits_bytes(self->_size)); }
STC_INLINE intptr_t cbits_size(const cbits* self) { return self->_size; }

STC_INLINE cbits* cbits_take(cbits* self, cbits other) {
    if (self->buffer != other.buffer) {
        cbits_drop(self);
        *self = other;
    }
    return self;
}

STC_INLINE cbits cbits_clone(cbits other) {
    const intptr_t bytes = _cbits_bytes(other._size);
    cbits set = {(uintptr_t *)c_memcpy(i_malloc(bytes), other.buffer, bytes), other._size};
    return set;
}

STC_INLINE cbits* cbits_copy(cbits* self, const cbits* other) {
    if (self->buffer == other->buffer)
        return self;
    if (self->_size != other->_size)
        return cbits_take(self, cbits_clone(*other));
    c_memcpy(self->buffer, other->buffer, _cbits_bytes(other->_size));
    return self;
}

STC_INLINE void cbits_resize(cbits* self, const intptr_t size, const bool value) {
    const intptr_t new_n = _cbits_words(size), osize = self->_size, old_n = _cbits_words(osize);
    self->buffer = (uintptr_t *)i_realloc(self->buffer, old_n*8, new_n*8);
    self->_size = size;
    if (new_n >= old_n) {
        c_memset(self->buffer + old_n, -(int)value, (new_n - old_n)*8);
        if (old_n > 0) {
            uintptr_t mask = _cbits_bit(osize) - 1;
            if (value) self->buffer[old_n - 1] |= ~mask;
            else       self->buffer[old_n - 1] &= mask;
        }
    }
}

STC_INLINE void cbits_set_all(cbits *self, const bool value);
STC_INLINE void cbits_set_pattern(cbits *self, const uintptr_t pattern);

STC_INLINE cbits cbits_move(cbits* self) {
    cbits tmp = *self;
    self->buffer = NULL, self->_size = 0;
    return tmp;
}

STC_INLINE cbits cbits_with_size(const intptr_t size, const bool value) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_all(&set, value);
    return set;
}

STC_INLINE cbits cbits_with_pattern(const intptr_t size, const uintptr_t pattern) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_pattern(&set, pattern);
    return set;
}

#else // i_capacity: FIXED SIZE BITARRAY

#define _i_assert(x) (void)0
#ifndef i_type
#define i_type c_JOIN(cbits, i_capacity)
#endif

typedef struct { uintptr_t buffer[(i_capacity - 1)/64 + 1]; } i_type;

STC_INLINE void     _i_memb(_init)(i_type* self) { memset(self->buffer, 0, i_capacity*8); }
STC_INLINE void     _i_memb(_drop)(i_type* self) { (void)self; }
STC_INLINE intptr_t _i_memb(_size)(const i_type* self) { (void)self; return i_capacity; }
STC_INLINE i_type   _i_memb(_move)(i_type* self) { return *self; }

STC_INLINE i_type*  _i_memb(_take)(i_type* self, i_type other)
    { *self = other; return self; }

STC_INLINE i_type _i_memb(_clone)(i_type other)
    { return other; }

STC_INLINE i_type* _i_memb(_copy)(i_type* self, const i_type* other)
    { *self = *other; return self; }

STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value);
STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uintptr_t pattern);

STC_INLINE i_type _i_memb(_with_size)(const intptr_t size, const bool value) {
    c_assert(size <= i_capacity);
    i_type set; _i_memb(_set_all)(&set, value);
    return set;
}

STC_INLINE i_type _i_memb(_with_pattern)(const intptr_t size, const uintptr_t pattern) {
    c_assert(size <= i_capacity);
    i_type set; _i_memb(_set_pattern)(&set, pattern);
    return set;
}
#endif // i_capacity

// COMMON:

STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value)
    { c_memset(self->buffer, value? ~0 : 0, _cbits_bytes(_i_memb(_size)(self))); }

STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uintptr_t pattern) {
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] = pattern;
}

STC_INLINE bool _i_memb(_test)(const i_type* self, const intptr_t i)
    { return (self->buffer[i/_cbits_BN] & _cbits_bit(i)) != 0; }

STC_INLINE bool _i_memb(_at)(const i_type* self, const intptr_t i)
    { return (self->buffer[i/_cbits_BN] & _cbits_bit(i)) != 0; }

STC_INLINE void _i_memb(_set)(i_type *self, const intptr_t i)
    { self->buffer[i/_cbits_BN] |= _cbits_bit(i); }

STC_INLINE void _i_memb(_reset)(i_type *self, const intptr_t i)
    { self->buffer[i/_cbits_BN] &= ~_cbits_bit(i); }

STC_INLINE void _i_memb(_set_value)(i_type *self, const intptr_t i, const bool b) {
    self->buffer[i/_cbits_BN] ^= ((uintptr_t)-(int)b ^ self->buffer[i/_cbits_BN]) & _cbits_bit(i);
}

STC_INLINE void _i_memb(_flip)(i_type *self, const intptr_t i)
    { self->buffer[i/_cbits_BN] ^= _cbits_bit(i); }

STC_INLINE void _i_memb(_flip_all)(i_type *self) {
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] ^= ~(uintptr_t)0;
}

STC_INLINE i_type _i_memb(_from)(const char* str) {
    intptr_t n = c_strlen(str);
    i_type set = _i_memb(_with_size)(n, false);
    while (n--) if (str[n] == '1') _i_memb(_set)(&set, n);
    return set;
}

/* Intersection */
STC_INLINE void _i_memb(_intersect)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] &= other->buffer[n];
}
/* Union */
STC_INLINE void _i_memb(_union)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] |= other->buffer[n];
}
/* Exclusive disjunction */
STC_INLINE void _i_memb(_xor)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    intptr_t n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->buffer[n] ^= other->buffer[n];
}

STC_INLINE intptr_t _i_memb(_count)(const i_type* self)
    { return _cbits_count(self->buffer, _i_memb(_size)(self)); }

STC_INLINE char* _i_memb(_to_str)(const i_type* self, char* out, intptr_t start, intptr_t stop)
    { return _cbits_to_str(self->buffer, _i_memb(_size)(self), out, start, stop); }

STC_INLINE bool _i_memb(_subset_of)(const i_type* self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_subset_of(self->buffer, other->buffer, _i_memb(_size)(self));
}

STC_INLINE bool _i_memb(_disjoint)(const i_type* self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_disjoint(self->buffer, other->buffer, _i_memb(_size)(self));
}

// ### BEGIN_FILE_INCLUDE: linkage2.h

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
#undef _i_memb
#undef _i_assert
#undef i_capacity
#undef i_type
// ### END_FILE_INCLUDE: cbits.h


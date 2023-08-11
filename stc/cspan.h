// ### BEGIN_FILE_INCLUDE: cspan.h
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

// ### BEGIN_FILE_INCLUDE: linkage.h
#undef STC_API
#undef STC_DEF

#ifdef i_extern // [deprecated]
#  define i_import
#endif
#if !defined(i_static) && !defined(STC_STATIC) && (defined(i_header) || defined(STC_HEADER) || \
                                                   defined(i_implement) || defined(STC_IMPLEMENT))
  #define STC_API extern
  #define STC_DEF
#else
  #define i_static
  #define STC_API static inline
  #define STC_DEF static inline
#endif
#if defined(STC_IMPLEMENT) || defined(i_import)
  #define i_implement
#endif
// ### END_FILE_INCLUDE: linkage.h
// ### BEGIN_FILE_INCLUDE: ccommon.h
#ifndef CCOMMON_H_INCLUDED
#define CCOMMON_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef long long _llong;
#define c_NPOS INTPTR_MAX
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR

#if defined(_MSC_VER)
  #pragma warning(disable: 4116 4996) // unnamed type definition in parentheses
  #define STC_FORCE_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
  #define STC_FORCE_INLINE static inline __attribute((always_inline))
#else
  #define STC_FORCE_INLINE static inline
#endif
#define STC_INLINE static inline

/* Macro overloading feature support based on: https://rextester.com/ONP80107 */
#define c_MACRO_OVERLOAD(name, ...) \
    c_PASTE(c_CONCAT(name,_), c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_CONCAT(a, b) a ## b
#define c_PASTE(a, b) c_CONCAT(a, b)
#define c_EXPAND(...) __VA_ARGS__
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) c_EXPAND(_c_ARG_N args)
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                 _14, _15, _16, N, ...) N

#ifdef __cplusplus 
  #include <new>
  #define _i_alloc(T)           static_cast<T*>(i_malloc(c_sizeof(T)))
  #define _i_new(T, ...)        new (_i_alloc(T)) T(__VA_ARGS__)
  #define c_new(T, ...)         new (malloc(sizeof(T))) T(__VA_ARGS__)
  #define c_LITERAL(T)          T
#else
  #define _i_alloc(T)           ((T*)i_malloc(c_sizeof(T)))
  #define _i_new(T, ...)        ((T*)memcpy(_i_alloc(T), ((T[]){__VA_ARGS__}), sizeof(T)))
  #define c_new(T, ...)         ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
  #define c_LITERAL(T)          (T)
#endif
#define c_new_n(T, n)           ((T*)malloc(sizeof(T)*(size_t)(n)))
#define c_malloc(sz)            malloc(c_i2u(sz))
#define c_calloc(n, sz)         calloc(c_i2u(n), c_i2u(sz))
#define c_realloc(p, sz)        realloc(p, c_i2u(sz))
#define c_free(p)               free(p)
#define c_delete(T, ptr)        do { T *_tp = ptr; T##_drop(_tp); free(_tp); } while (0)

#define c_static_assert(...)    c_MACRO_OVERLOAD(c_static_assert, __VA_ARGS__)
#define c_static_assert_1(b)    ((int)(0*sizeof(int[(b) ? 1 : -1])))
#define c_static_assert_2(b, m) c_static_assert_1(b)
#if defined STC_NDEBUG || defined NDEBUG
  #define c_assert(expr)        ((void)0)
#else
  #define c_assert(expr)        assert(expr)
#endif
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(T, p)      ((T)(1 ? (p) : (T)0))
#define c_swap(T, xp, yp)       do { T *_xp = xp, *_yp = yp, \
                                    _tv = *_xp; *_xp = *_yp; *_yp = _tv; } while (0)
#define c_sizeof                (intptr_t)sizeof
#define c_strlen(s)             (intptr_t)strlen(s)

#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u(ilen))
#define c_u2i(u)                ((intptr_t)(1 ? (u) : (size_t)1))
#define c_i2u(i)                ((size_t)(1 ? (i) : (intptr_t)1))
#define c_LTu(a, b)             ((size_t)(a) < (size_t)(b))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_hash(x)       cfasthash(x, c_sizeof(*(x)))

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

#define c_option(flag)          ((i_opt) & (flag))
#define c_is_forward            (1<<0)
#define c_no_atomic             (1<<1)
#define c_no_clone              (1<<2)
#define c_no_emplace            (1<<3)
#define c_no_hash               (1<<4)
#define c_use_cmp               (1<<5)
/* Function macros and others */

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (intptr_t)(sizeof(a)/sizeof 0[a])

// Non-owning c-string "class"
typedef const char* ccharptr;
#define ccharptr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define ccharptr_hash(p) cstrhash(*(p))
#define ccharptr_clone(s) (s)
#define ccharptr_drop(p) ((void)p)

#define c_sv(...) c_MACRO_OVERLOAD(c_sv, __VA_ARGS__)
#define c_sv_1(lit) c_sv_2(lit, c_litstrlen(lit))
#define c_sv_2(str, n) (c_LITERAL(csview){str, n})

#define c_SV(sv) (int)(sv).size, (sv).str // print csview: use format "%.*s"
#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t cfasthash(const void* key, intptr_t len) {
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

STC_INLINE uint64_t cstrhash(const char *str)
    { return cfasthash(str, c_strlen(str)); }

STC_INLINE char* cstrnstrn(const char *str, const char *needle,
                           intptr_t slen, const intptr_t nlen) {
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

STC_INLINE intptr_t cnextpow2(intptr_t n) {
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
    for (C##_iter it = start, *_endref = (C##_iter*)(finish).ref \
         ; it.ref != (C##_value*)_endref; C##_next(&it))

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter it; const C##_key* key; C##_mapped* val;} _ = {.it=C##_begin(&cnt)} \
         ; _.it.ref && (_.key = &_.it.ref->first, _.val = &_.it.ref->second) \
         ; C##_next(&_.it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_c_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (_llong i=start, _end=(_llong)(stop); i < _end; ++i)
#define c_forrange_4(i, start, stop, step) \
    for (_llong i=start, _inc=step, _end=(_llong)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
  #define c_init(C, ...) \
      C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))
  #define c_forlist(it, T, ...) \
      for (struct {T* ref; int size, index;} \
           it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
           ; it.index < it.size; ++it.ref, ++it.index)
#else
    #include <initializer_list>
    template <class C, class T>
    inline C _from_n(C (*func)(const T[], intptr_t), std::initializer_list<T> il)
        { return func(&*il.begin(), il.size()); }

    #define c_init(C, ...) _from_n<C,C##_raw>(C##_from_n, __VA_ARGS__)
    #define c_forlist(it, T, ...) \
        for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator ref; size_t size, index;} \
             it = {._il=__VA_ARGS__, .ref=it._il.begin(), .size=it._il.size()} \
             ; it.index < it.size; ++it.ref, ++it.index)
#endif

#define c_defer(...) \
    for (int _i = 1; _i; _i = 0, __VA_ARGS__)
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

#endif // CCOMMON_H_INCLUDED
// ### END_FILE_INCLUDE: ccommon.h

#define using_cspan(...) c_MACRO_OVERLOAD(using_cspan, __VA_ARGS__)
#define using_cspan_2(Self, T) \
    using_cspan_3(Self, T, 1); \
    STC_INLINE Self Self##_from_n(Self##_raw* raw, const intptr_t n) { \
        return (Self){.data=raw, .shape={(int32_t)n}, .stride={.d={1}}}; \
    } \
    struct stc_nostruct

#define using_cspan_3(Self, T, RANK) \
    typedef T Self##_value; typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        int32_t shape[RANK]; \
        cspan_tuple##RANK stride; \
    } Self; \
    \
    typedef struct { Self##_value *ref; int32_t pos[RANK]; const Self *_s; } Self##_iter; \
    \
    STC_INLINE Self Self##_slice_(Self##_value* d, const int32_t shape[], const int32_t stri[], \
                                  const int rank, const int32_t a[][2]) { \
        Self s; int outrank; \
        s.data = d + _cspan_slice(s.shape, s.stride.d, &outrank, shape, stri, rank, a); \
        c_assert(outrank == RANK); \
        return s; \
    } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {.ref=self->data, ._s=self}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {0}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) { \
        static const struct { int8_t i, j, inc; } t[2] = {{RANK - 1, 0, -1}, {0, RANK - 1, 1}}; \
        const int order = (it->_s->stride.d[0] < it->_s->stride.d[RANK - 1]); /* 0='C', 1='F' */ \
        it->ref += _cspan_next##RANK(it->pos, it->_s->shape, it->_s->stride.d, RANK, t[order].i, t[order].inc); \
        if (it->pos[t[order].j] == it->_s->shape[t[order].j]) \
            it->ref = NULL; \
    } \
    struct stc_nostruct

#define using_cspan2(Self, T) using_cspan_2(Self, T); using_cspan_3(Self##2, T, 2)
#define using_cspan3(Self, T) using_cspan2(Self, T); using_cspan_3(Self##3, T, 3)
#define using_cspan4(Self, T) using_cspan3(Self, T); using_cspan_3(Self##4, T, 4)
#define using_cspan_tuple(N) typedef struct { int32_t d[N]; } cspan_tuple##N
using_cspan_tuple(1); using_cspan_tuple(2);
using_cspan_tuple(3); using_cspan_tuple(4);
using_cspan_tuple(5); using_cspan_tuple(6);
using_cspan_tuple(7); using_cspan_tuple(8);

#define c_END -1
#define c_ALL 0,c_END

/* Use cspan_init() for static initialization only. c_init() for non-static init. */
#define cspan_init(SpanType, ...) \
    {.data=(SpanType##_value[])__VA_ARGS__, .shape={sizeof((SpanType##_value[])__VA_ARGS__)/sizeof(SpanType##_value)}, .stride={.d={1}}}

/* create a cspan from a cvec, cstack, or cpque (heap) */
#define cspan_from(container) \
    {.data=(container)->data, .shape={(int32_t)(container)->_len}, .stride={.d={1}}}

#define cspan_from_n(ptr, n) \
    {.data=(ptr), .shape={n}, .stride={.d={1}}}

#define cspan_from_array(array) \
    cspan_from_n(array, c_arraylen(array))

#define cspan_size(self) _cspan_size((self)->shape, cspan_rank(self))
#define cspan_rank(self) c_arraylen((self)->shape)
#define cspan_is_order_F(self) ((self)->stride.d[0] < (self)->stride.d[cspan_rank(self) - 1])
#define cspan_index(self, ...) c_PASTE(cspan_idx_, c_NUMARGS(__VA_ARGS__))(self, __VA_ARGS__)
#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)

// cspan_subspanX: (X <= 3) optimized. Similar to cspan_slice(Span3, &ms3, {off,off+count}, {c_ALL}, {c_ALL});
#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), .shape={count}, .stride=(self)->stride}
#define cspan_subspan2(self, offset, count) \
    {.data=cspan_at(self, offset, 0), .shape={count, (self)->shape[1]}, .stride=(self)->stride}
#define cspan_subspan3(self, offset, count) \
    {.data=cspan_at(self, offset, 0, 0), .shape={count, (self)->shape[1], (self)->shape[2]}, .stride=(self)->stride}

// cspan_submd(): Reduce rank (N <= 4) Optimized, same as e.g. cspan_slice(Span2, &ms4, {x}, {y}, {c_ALL}, {c_ALL});
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), .shape={(self)->shape[1]}, .stride=(cspan_tuple1){.d={(self)->stride.d[1]}}}
#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), .shape={(self)->shape[1], (self)->shape[2]}, \
                                    .stride=(cspan_tuple2){.d={(self)->stride.d[1], (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), .shape={(self)->shape[2]}, .stride=(cspan_tuple1){.d={(self)->stride.d[2]}}}
#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), .shape={(self)->shape[1], (self)->shape[2], (self)->shape[3]}, \
                                       .stride=(cspan_tuple3){.d={(self)->stride.d[1], (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), .shape={(self)->shape[2], (self)->shape[3]}, \
                                       .stride=(cspan_tuple2){.d={(self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), .shape={(self)->shape[3]}, .stride=(cspan_tuple1){.d={(self)->stride.d[3]}}}

#define cspan_md(array, ...) cspan_md_order('C', array, __VA_ARGS__)
#define cspan_md_order(order, array, ...) /* order='C' or 'F' */ \
    {.data=array, .shape={__VA_ARGS__}, \
     .stride=*(c_PASTE(cspan_tuple, c_NUMARGS(__VA_ARGS__))*)_cspan_shape2stride(order, ((int32_t[]){__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))}

#define cspan_transpose(self) \
    _cspan_transpose((self)->shape, (self)->stride.d, cspan_rank(self))

// General slicing function;
#define cspan_slice(OutSpan, parent, ...) \
    OutSpan##_slice_((parent)->data, (parent)->shape, (parent)->stride.d, cspan_rank(parent) + \
                     c_static_assert(cspan_rank(parent) == sizeof((int32_t[][2]){__VA_ARGS__})/sizeof(int32_t[2])), \
                     (const int32_t[][2]){__VA_ARGS__})

/* ------------------- PRIVAT DEFINITIONS ------------------- */

// cspan_index() helpers:
#define cspan_idx_1 cspan_idx_3
#define cspan_idx_2 cspan_idx_3
#define cspan_idx_3(self, ...) \
    c_PASTE(_cspan_idx, c_NUMARGS(__VA_ARGS__))((self)->shape, (self)->stride, __VA_ARGS__) // small/fast
#define cspan_idx_4(self, ...) \
    (_cspan_idxN(c_NUMARGS(__VA_ARGS__), (self)->shape, (self)->stride.d, (int32_t[]){__VA_ARGS__}) + \
     c_static_assert(cspan_rank(self) == c_NUMARGS(__VA_ARGS__))) // general
#define cspan_idx_5 cspan_idx_4
#define cspan_idx_6 cspan_idx_4

STC_INLINE intptr_t _cspan_size(const int32_t shape[], int rank) {
    intptr_t sz = shape[0];
    while (--rank > 0) sz *= shape[rank];
    return sz;
}

STC_INLINE void _cspan_transpose(int32_t shape[], int32_t stride[], int rank) {
    for (int i = 0; i < --rank; ++i) {
        c_swap(int32_t, shape + i, shape + rank);
        c_swap(int32_t, stride + i, stride + rank);
    }
}

STC_INLINE intptr_t _cspan_idx1(const int32_t shape[1], const cspan_tuple1 stri, int32_t x)
    { c_assert(c_LTu(x, shape[0])); return (intptr_t)stri.d[0]*x; }

STC_INLINE intptr_t _cspan_idx2(const int32_t shape[2], const cspan_tuple2 stri, int32_t x, int32_t y)
    { c_assert(c_LTu(x, shape[0]) && c_LTu(y, shape[1])); return (intptr_t)stri.d[0]*x + stri.d[1]*y; }

STC_INLINE intptr_t _cspan_idx3(const int32_t shape[3], const cspan_tuple3 stri, int32_t x, int32_t y, int32_t z) {
    c_assert(c_LTu(x, shape[0]) && c_LTu(y, shape[1]) && c_LTu(z, shape[2]));
    return (intptr_t)stri.d[0]*x + stri.d[1]*y + stri.d[2]*z;
}
STC_INLINE intptr_t _cspan_idxN(int rank, const int32_t shape[], const int32_t stride[], const int32_t a[]) {
    intptr_t off = 0;
    while (rank--) {
        c_assert(c_LTu(a[rank], shape[rank]));
        off += stride[rank]*a[rank];
    }
    return off;
}

STC_INLINE intptr_t _cspan_next2(int32_t pos[], const int32_t shape[], const int32_t stride[], int rank, int i, int inc) {
    intptr_t off = stride[i];
    ++pos[i];
    for (; --rank && pos[i] == shape[i]; i += inc) {
        pos[i] = 0; ++pos[i + inc];
        off += stride[i + inc] - stride[i]*shape[i];
    }
    return off;
}
#define _cspan_next1(pos, shape, stride, rank, i, inc) (++pos[0], stride[0])
#define _cspan_next3 _cspan_next2
#define _cspan_next4 _cspan_next2
#define _cspan_next5 _cspan_next2
#define _cspan_next6 _cspan_next2
#define _cspan_next7 _cspan_next2
#define _cspan_next8 _cspan_next2

STC_API intptr_t _cspan_slice(int32_t oshape[], int32_t ostride[], int* orank,
                              const int32_t shape[], const int32_t stride[],
                              int rank, const int32_t a[][2]);

STC_API int32_t* _cspan_shape2stride(char order, int32_t shape[], int rank);
#endif // STC_CSPAN_H_INCLUDED

/* --------------------- IMPLEMENTATION --------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF int32_t* _cspan_shape2stride(char order, int32_t shape[], int rank) {
    int32_t k = 1, i, j, inc, s1, s2;
    if (order == 'F') i = 0, j = rank, inc = 1; 
    else  /* 'C' */   i = rank - 1, j = -1, inc = -1;
    s1 = shape[i]; shape[i] = 1;

    for (i += inc; i != j; i += inc) {
        s2 = shape[i];
        shape[i] = (k *= s1);
        s1 = s2;
    }
    return shape;
}

STC_DEF intptr_t _cspan_slice(int32_t oshape[], int32_t ostride[], int* orank, 
                              const int32_t shape[], const int32_t stride[], 
                              int rank, const int32_t a[][2]) {
    intptr_t off = 0;
    int i = 0, oi = 0;
    int32_t end;
    for (; i < rank; ++i) {
        off += stride[i]*a[i][0];
        switch (a[i][1]) {
            case 0: c_assert(c_LTu(a[i][0], shape[i])); continue;
            case -1: end = shape[i]; break;
            default: end = a[i][1];
        }
        oshape[oi] = end - a[i][0];
        ostride[oi] = stride[i];
        c_assert(c_LTu(0, oshape[oi]) & !c_LTu(shape[i], end));
        ++oi;
    }
    *orank = oi;
    return off;
}

#endif
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_import
// ### END_FILE_INCLUDE: cspan.h


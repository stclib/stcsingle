// ### BEGIN_FILE_INCLUDE: cspan.h
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

// ### BEGIN_FILE_INCLUDE: ccommon.h
#ifndef CCOMMON_H_INCLUDED
#define CCOMMON_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
// ### BEGIN_FILE_INCLUDE: altnames.h
#define c_FORLIST c_forlist
#define c_FORRANGE c_forrange
#define c_FOREACH c_foreach
#define c_FORPAIR c_forpair
#define c_FORFILTER c_forfilter
#define c_FORMATCH c_formatch
#define c_FORTOKEN c_fortoken
#define c_FORTOKEN_SV c_fortoken_sv
#define c_AUTO c_auto
#define c_WITH c_with
#define c_SCOPE c_scope
#define c_DEFER c_defer
// ### END_FILE_INCLUDE: altnames.h
// ### BEGIN_FILE_INCLUDE: raii.h
#define c_defer(...) \
    for (int _i = 1; _i; _i = 0, __VA_ARGS__)

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(declvar, drop) \
    for (declvar, *_i, **_ip = &_i; _ip; _ip = 0, drop)
#define c_with_3(declvar, pred, drop) \
    for (declvar, *_i, **_ip = &_i; _ip && (pred); _ip = 0, drop)

#define c_scope(...) c_MACRO_OVERLOAD(c_scope, __VA_ARGS__)
#define c_scope_2(init, drop) \
    for (int _i = (init, 1); _i; _i = 0, drop)
#define c_scope_3(init, pred, drop) \
    for (int _i = (init, 1); _i && (pred); _i = 0, drop)

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto_2(C, a) \
    c_with_2(C a = C##_init(), C##_drop(&a))
#define c_auto_3(C, a, b) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init()), \
            (C##_drop(&b), C##_drop(&a)))
#define c_auto_4(C, a, b, c) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init()), \
            (C##_drop(&c), C##_drop(&b), C##_drop(&a)))
#define c_auto_5(C, a, b, c, d) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init(), d = C##_init()), \
            (C##_drop(&d), C##_drop(&c), C##_drop(&b), C##_drop(&a)))
// ### END_FILE_INCLUDE: raii.h

#define c_NPOS INTPTR_MAX
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR
#if defined STC_NDEBUG || defined NDEBUG
  #define c_ASSERT(expr) (void)(0)
#else
  #define c_ASSERT(expr) assert(expr)
#endif

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
  #define _i_new(T, ...)        ((T*)memcpy(_i_alloc(T), (T[]){__VA_ARGS__}, sizeof(T)))
  #define c_new(T, ...)         ((T*)memcpy(malloc(sizeof(T)), (T[]){__VA_ARGS__}, sizeof(T)))
  #define c_LITERAL(T)          (T)
#endif
#define c_malloc(sz)            malloc(c_i2u(sz))
#define c_calloc(n, sz)         calloc(c_i2u(n), c_i2u(sz))
#define c_realloc(p, sz)        realloc(p, c_i2u(sz))
#define c_free(p)               free(p)
#define c_delete(T, ptr)        do { T *_tp = ptr; T##_drop(_tp); free(_tp); } while (0)

#define c_static_assert(b)      ((int)(0*sizeof(int[(b) ? 1 : -1])))
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(T, p)      ((T)(p) + 0*sizeof((T)0 == (p)))
#define c_swap(T, xp, yp)       do { T *_xp = xp, *_yp = yp, \
                                    _tv = *_xp; *_xp = *_yp; *_yp = _tv; } while (0)
#define c_sizeof                (intptr_t)sizeof
#define c_strlen(s)             (intptr_t)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u(ilen))

#define c_u2i(u)                ((intptr_t)((u) + 0*sizeof((u) == 1U)))
#define c_i2u(i)                ((size_t)(i) + 0*sizeof((i) == 1))
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
#define c_no_cmp                (1<<4)
#define c_no_hash               (1<<5)

/* Function macros and others */

#define c_make(C, ...) \
  C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (intptr_t)(sizeof(a)/sizeof 0[a])

// Non-owning c-string
typedef const char* crawstr;
#define crawstr_clone(s) (s)
#define crawstr_drop(p) ((void)p)
#define crawstr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define crawstr_hash(p) cstrhash(*(p))

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
    uint64_t h = *x, n = (uint64_t)len >> 3;
    len &= 7;
    while (n--) {
        memcpy(&u8, x, 8), x += 8;
        h += u8*0xc6a4a7935bd1e99d;
    }
    while (len--) h = (h << 10) - h - *x++;
    return c_ROTL(h, 26) ^ h;
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

/* Control block macros */

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it))
#define c_foreach_4(it, C, start, finish) \
    for (C##_iter it = start, *_endref = (C##_iter*)(finish).ref \
         ; it.ref != (C##_value*)_endref; C##_next(&it))

#define c_foreach_rv(it, C, cnt) \
    for (C##_iter it = {.ref=C##_end(&cnt).end - 1, .end=(cnt).data - 1} \
         ; it.ref != it.end; --it.ref)

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter it; const C##_key* key; C##_mapped* val;} _ = {.it=C##_begin(&cnt)} \
         ; _.it.ref && (_.key = &_.it.ref->first, _.val = &_.it.ref->second) \
         ; C##_next(&_.it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_c_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (long long i=start, _end=(long long)(stop); i < _end; ++i)
#define c_forrange_4(i, start, stop, step) \
    for (long long i=start, _inc=step, _end=(long long)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
  #define c_forlist(it, T, ...) \
    for (struct {T* ref; int size, index;} \
         it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
         ; it.index < it.size; ++it.ref, ++it.index)
#else
  #include <initializer_list>
  #define c_forlist(it, T, ...) \
    for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator data, ref; size_t size, index;} \
         it = {._il=__VA_ARGS__, .data=it._il.begin(), .ref=it.data, .size=it._il.size()} \
         ; it.index < it.size; ++it.ref, ++it.index)
#endif

#define c_drop(C, ...) \
    do { c_forlist (_i, C*, {__VA_ARGS__}) C##_drop(*_i.ref); } while(0)

#endif // CCOMMON_H_INCLUDED

#undef STC_API
#undef STC_DEF

#if !defined(i_static) && !defined(STC_STATIC) && (defined(i_header) || defined(STC_HEADER) || \
                                                   defined(i_implement) || defined(STC_IMPLEMENT))
  #define STC_API extern
  #define STC_DEF
#else
  #define i_static
  #define STC_API static inline
  #define STC_DEF static inline
#endif
#if defined(STC_EXTERN)
  #define i_extern
#endif
#if defined(i_static) || defined(STC_IMPLEMENT)
  #define i_implement
#endif
// ### END_FILE_INCLUDE: ccommon.h

#define using_cspan(...) c_MACRO_OVERLOAD(using_cspan, __VA_ARGS__)
#define using_cspan_2(Self, T) \
    using_cspan_3(Self, T, 1)

#define using_cspan_3(Self, T, RANK) \
    typedef T Self##_value; typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        int32_t shape[RANK]; \
        cspan_idx##RANK stride; \
    } Self; \
    \
    typedef struct { Self##_value *ref; int32_t pos[RANK]; const Self *_s; } Self##_iter; \
    \
    STC_INLINE Self Self##_from_n(Self##_raw* raw, const intptr_t n) { \
        return (Self){.data=raw, .shape={(int32_t)n}}; \
    } \
    STC_INLINE Self Self##_slice_(Self##_value* v, const int32_t shape[], const int32_t stri[], \
                                     const int rank, const int32_t a[][2]) { \
        Self s = {.data=v}; int outrank; \
        s.data += _cspan_slice(s.shape, s.stride.d, &outrank, shape, stri, rank, a); \
        c_ASSERT(outrank == RANK); \
        return s; \
    } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {.ref=self->data, .pos={0}, ._s=self}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {.ref=NULL}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) { \
        it->ref += _cspan_next##RANK(RANK, it->pos, it->_s->shape, it->_s->stride.d); \
        if (it->pos[0] == it->_s->shape[0]) it->ref = NULL; \
    } \
    struct stc_nostruct

#define using_cspan2(Self, T) using_cspan_3(Self, T, 1); using_cspan_3(Self##2, T, 2)
#define using_cspan3(Self, T) using_cspan2(Self, T); using_cspan_3(Self##3, T, 3)
#define using_cspan4(Self, T) using_cspan3(Self, T); using_cspan_3(Self##4, T, 4)
typedef struct { int32_t d[1]; } cspan_idx1;
typedef struct { int32_t d[2]; } cspan_idx2;
typedef struct { int32_t d[3]; } cspan_idx3;
typedef struct { int32_t d[4]; } cspan_idx4;
typedef struct { int32_t d[5]; } cspan_idx5;
typedef struct { int32_t d[6]; } cspan_idx6;
#define c_END -1
#define c_ALL 0,-1

#define cspan_md(array, ...) \
    {.data=array, .shape={__VA_ARGS__}, .stride={.d={__VA_ARGS__}}}

/* For static initialization, use cspan_make(). c_make() for non-static only. */
#define cspan_make(SpanType, ...) \
    {.data=(SpanType##_value[])__VA_ARGS__, .shape={sizeof((SpanType##_value[])__VA_ARGS__)/sizeof(SpanType##_value)}}

#define cspan_slice(OutSpan, parent, ...) \
    OutSpan##_slice_((parent)->data, (parent)->shape, (parent)->stride.d, cspan_rank(parent) + \
                     c_static_assert(cspan_rank(parent) == sizeof((int32_t[][2]){__VA_ARGS__})/sizeof(int32_t[2])), \
                     (const int32_t[][2]){__VA_ARGS__})
     
/* create a cspan from a cvec, cstack, cdeq, cqueue, or cpque (heap) */
#define cspan_from(container) \
    {.data=(container)->data, .shape={(int32_t)(container)->_len}}

#define cspan_from_array(array) \
    {.data=(array) + c_static_assert(sizeof(array) != sizeof(void*)), .shape={c_arraylen(array)}}

#define cspan_size(self) _cspan_size((self)->shape, cspan_rank(self))
#define cspan_rank(self) c_arraylen((self)->shape)

#define cspan_index(self, ...) c_PASTE(cspan_idx_, c_NUMARGS(__VA_ARGS__))(self, __VA_ARGS__)
#define cspan_idx_1 cspan_idx_4
#define cspan_idx_2 cspan_idx_4
#define cspan_idx_3 cspan_idx_4
#define cspan_idx_4(self, ...) \
    c_PASTE(_cspan_idx, c_NUMARGS(__VA_ARGS__))((self)->shape, (self)->stride, __VA_ARGS__) // small/fast
#define cspan_idx_5(self, ...) \
    (_cspan_idxN(c_NUMARGS(__VA_ARGS__), (self)->shape, (self)->stride.d, (int32_t[]){__VA_ARGS__}) + \
     c_static_assert(cspan_rank(self) == c_NUMARGS(__VA_ARGS__))) // general
#define cspan_idx_6 cspan_idx_5

#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)

// cspan_subspanN. (N<4) Optimized, same as e.g. cspan_slice(Span3, &ms3, {off,off+count}, {c_ALL}, {c_ALL});
#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), .shape={count}}
#define cspan_subspan2(self, offset, count) \
    {.data=cspan_at(self, offset, 0), .shape={count, (self)->shape[1]}, .stride={(self)->stride}}
#define cspan_subspan3(self, offset, count) \
    {.data=cspan_at(self, offset, 0, 0), .shape={count, (self)->shape[1], (self)->shape[2]}, .stride={(self)->stride}}

// cspan_submdN: reduce rank (N<5) Optimized, same as e.g. cspan_slice(Span2, &ms4, {x}, {y}, {c_ALL}, {c_ALL});
#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), .shape={(self)->shape[1]}}
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), .shape={(self)->shape[1], (self)->shape[2]}, \
                                    .stride={.d={0, (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), .shape={(self)->shape[2]}}
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), .shape={(self)->shape[1], (self)->shape[2], (self)->shape[3]}, \
                                       .stride={.d={0, (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), .shape={(self)->shape[2], (self)->shape[3]}, .stride={.d={0, (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), .shape={(self)->shape[3]}}

// private definitions:

STC_INLINE intptr_t _cspan_size(const int32_t shape[], int rank) {
    intptr_t sz = shape[0];
    while (rank-- > 1) sz *= shape[rank];
    return sz;
}

STC_INLINE intptr_t _cspan_idx1(const int32_t shape[1], const cspan_idx1 stri, int32_t x)
    { c_ASSERT(c_LTu(x, shape[0])); return x; }

STC_INLINE intptr_t _cspan_idx2(const int32_t shape[2], const cspan_idx2 stri, int32_t x, int32_t y)
    { c_ASSERT(c_LTu(x, shape[0]) && c_LTu(y, shape[1])); return (intptr_t)stri.d[1]*x + y; }

STC_INLINE intptr_t _cspan_idx3(const int32_t shape[3], const cspan_idx3 stri, int32_t x, int32_t y, int32_t z) {
    c_ASSERT(c_LTu(x, shape[0]) && c_LTu(y, shape[1]) && c_LTu(z, shape[2]));
    return (intptr_t)stri.d[2]*(stri.d[1]*x + y) + z;
}
STC_INLINE intptr_t _cspan_idx4(const int32_t shape[4], const cspan_idx4 stri, int32_t x, int32_t y,
                                                                               int32_t z, int32_t w) {
    c_ASSERT(c_LTu(x, shape[0]) && c_LTu(y, shape[1]) && c_LTu(z, shape[2]) && c_LTu(w, shape[3]));
    return (intptr_t)stri.d[3]*(stri.d[2]*(stri.d[1]*x + y) + z) + w;
}

STC_API intptr_t _cspan_idxN(int rank, const int32_t shape[], const int32_t stri[], const int32_t a[]);
STC_API intptr_t _cspan_next2(int rank, int32_t pos[], const int32_t shape[], const int32_t stride[]);
#define _cspan_next1(r, pos, d, s) (++pos[0], 1)
#define _cspan_next3 _cspan_next2
#define _cspan_next4 _cspan_next2
#define _cspan_next5 _cspan_next2
#define _cspan_next6 _cspan_next2

STC_API intptr_t _cspan_slice(int32_t odim[], int32_t ostri[], int* orank, 
                              const int32_t shape[], const int32_t stri[], 
                              int rank, const int32_t a[][2]);

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

STC_DEF intptr_t _cspan_idxN(int rank, const int32_t shape[], const int32_t stri[], const int32_t a[]) {
    intptr_t off = a[0];
    c_ASSERT(c_LTu(a[0], shape[0]));
    for (int i = 1; i < rank; ++i) {
        off *= stri[i];
        off += a[i];
        c_ASSERT(c_LTu(a[i], shape[i]));
    }
    return off;
}

STC_DEF intptr_t _cspan_next2(int rank, int32_t pos[], const int32_t shape[], const int32_t stride[]) {
    intptr_t off = 1, rs = 1;
    ++pos[rank - 1];
    while (--rank && pos[rank] == shape[rank]) {
        pos[rank] = 0, ++pos[rank - 1];
        const intptr_t ds = rs*shape[rank];
        rs *= stride[rank];
        off += rs - ds;
    }
    return off;
}

STC_DEF intptr_t _cspan_slice(int32_t odim[], int32_t ostri[], int* orank, 
                              const int32_t shape[], const int32_t stri[], 
                              int rank, const int32_t a[][2]) {
    intptr_t off = 0;
    int i = 0, j = 0;
    int32_t t, s = 1;
    for (; i < rank; ++i) {
        off *= stri[i];
        off += a[i][0];
        switch (a[i][1]) { 
            case 0: s *= stri[i]; c_ASSERT(c_LTu(a[i][0], shape[i])); continue;
            case -1: t = shape[i]; break;
            default: t = a[i][1]; break; 
        }
        odim[j] = t - a[i][0];
        ostri[j] = s*stri[i];
        c_ASSERT(c_LTu(0, odim[j]) & !c_LTu(shape[i], t));
        s = 1; ++j;
    }
    *orank = j;
    return off;
}
#endif
#endif
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_extern
// ### END_FILE_INCLUDE: cspan.h


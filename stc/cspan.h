// ### BEGIN_FILE_INCLUDE: cspan.h
#ifndef STC_CSPAN_INDEX_TYPE
  #define STC_CSPAN_INDEX_TYPE int32_t
#endif

#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

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
    #define STC_API static
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

typedef long long _llong;
#define c_NPOS INTPTR_MAX
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR

#if defined __GNUC__ // includes __clang__
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
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_hash          stc_hash_1

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

/* Function macros and others */

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (intptr_t)(sizeof(a)/sizeof 0[a])

// Non-owning c-string "class"
typedef const char* ccharptr;
#define ccharptr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define ccharptr_hash(p) stc_strhash(*(p))
#define ccharptr_clone(s) (s)
#define ccharptr_drop(p) ((void)p)

#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

#define stc_hash(...) c_MACRO_OVERLOAD(stc_hash, __VA_ARGS__)
#define stc_hash_1(x) stc_hash_2(x, c_sizeof(*(x)))

STC_INLINE uint64_t stc_hash_2(const void* key, intptr_t len) {
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

STC_INLINE uint64_t stc_strhash(const char *str)
    { return stc_hash_2(str, c_strlen(str)); }

STC_INLINE uint64_t _stc_hash_mix(uint64_t h[], int n) { // n > 0
    for (int i = 1; i < n; ++i) h[0] ^= h[0] + h[i]; // non-commutative!
    return h[0];
}

STC_INLINE char* stc_strnstrn(const char *str, intptr_t slen,
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

STC_INLINE intptr_t stc_nextpow2(intptr_t n) {
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
    for (C##_iter it = (start), *_endref = c_safe_cast(C##_iter*, C##_value*, (finish).ref) \
         ; it.ref != (C##_value*)_endref; C##_next(&it))

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter iter; const C##_key* key; C##_mapped* val;} _ = {.iter=C##_begin(&cnt)} \
         ; _.iter.ref && (_.key = &_.iter.ref->first, _.val = &_.iter.ref->second) \
         ; C##_next(&_.iter))

#define c_forindexed(it, C, cnt) \
    for (struct {C##_iter iter; C##_value* ref; intptr_t index;} it = {.iter=C##_begin(&cnt)} \
         ; (it.ref = it.iter.ref) ; C##_next(&it.iter), ++it.index)

#define c_foriter(existing_iter, C, cnt) \
    for (existing_iter = C##_begin(&cnt); (existing_iter).ref; C##_next(&existing_iter))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (_llong i=start, _end=stop; i < _end; ++i)
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
    #define stc_hash_mix(...) \
        _stc_hash_mix((uint64_t[]){__VA_ARGS__}, c_NUMARGS(__VA_ARGS__))
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
    #define stc_hash_mix(...) \
        _stc_hash_mix(std::array<uint64_t, c_NUMARGS(__VA_ARGS__)>{__VA_ARGS__}.data(), c_NUMARGS(__VA_ARGS__))
#endif

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
typedef STC_CSPAN_INDEX_TYPE cextent_t, cstride_t;

#define using_cspan(...) c_MACRO_OVERLOAD(using_cspan, __VA_ARGS__)
#define using_cspan_2(Self, T) \
    using_cspan_3(Self, T, 1); \
    STC_INLINE Self Self##_from_n(Self##_value* values, intptr_t n) { \
        return (Self)cspan_from_n(values, n); \
    } \
    struct stc_nostruct

#define using_cspan_3(Self, T, RANK) \
    typedef T Self##_value; \
    typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        cextent_t shape[RANK]; \
        cspan_tuple##RANK stride; \
    } Self; \
    \
    typedef struct { \
        Self##_value *ref; \
        const Self *_s; \
        cextent_t pos[RANK]; \
    } Self##_iter; \
    \
    STC_INLINE Self Self##_slice_(Self##_value* d, const cextent_t shape[], const cstride_t stri[], \
                                  const intptr_t args[][3], const int rank) { \
        Self s; int outrank; \
        s.data = d + _cspan_slice(s.shape, s.stride.d, &outrank, shape, stri, args, rank); \
        c_assert(outrank == RANK); \
        return s; \
    } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        return (Self##_iter){.ref=self->data, ._s=self}; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        (void)self; \
        return (Self##_iter){0}; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) { \
        int done; \
        it->ref += _cspan_next##RANK(it->pos, it->_s->shape, it->_s->stride.d, RANK, &done); \
        if (done) it->ref = NULL; \
    } \
    struct stc_nostruct

#define using_cspan2(Self, T) using_cspan_2(Self, T); using_cspan_3(Self##2, T, 2)
#define using_cspan3(Self, T) using_cspan2(Self, T); using_cspan_3(Self##3, T, 3)
#define using_cspan4(Self, T) using_cspan3(Self, T); using_cspan_3(Self##4, T, 4)
#define using_cspan_tuple(N) typedef struct { cstride_t d[N]; } cspan_tuple##N
using_cspan_tuple(1); using_cspan_tuple(2);
using_cspan_tuple(3); using_cspan_tuple(4);
using_cspan_tuple(5); using_cspan_tuple(6);
using_cspan_tuple(7); using_cspan_tuple(8);

// cspan_init: static construction from initialization list
//
#define cspan_init(Span, ...) \
    ((Span){.data=(Span##_value[])__VA_ARGS__, \
            .shape={sizeof((Span##_value[])__VA_ARGS__)/sizeof(Span##_value)}, \
            .stride=(cspan_tuple1){.d={1}}})

// cspan_from* a pointer+size, c-array, or a cvec/cstack container
//
#define cspan_from_n(ptr, n) \
    {.data=(ptr), \
     .shape={(cextent_t)(n)}, \
     .stride=(cspan_tuple1){.d={1}}}

#define cspan_from_array(array) \
    cspan_from_n(array, c_arraylen(array))

#define cspan_from(container) \
    cspan_from_n((container)->data, (container)->_len)

// cspan_subspan on 1d spans
//
#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), \
     .shape={(cextent_t)(count)}, \
     .stride=(self)->stride}

// Accessors
//
#define cspan_size(self) _cspan_size((self)->shape, cspan_rank(self))
#define cspan_rank(self) c_arraylen((self)->shape) // constexpr
#define cspan_is_colmajor(self) ((self)->stride.d[0] < (self)->stride.d[cspan_rank(self) - 1])
#define cspan_is_rowmajor(self) (!cspan_is_colmajor(self))
#define cspan_get_layout(self) (cspan_is_colmajor(self) ? c_COLMAJOR : c_ROWMAJOR)
#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)
#define cspan_index(self, ...) \
    _cspan_index((self)->shape, (self)->stride.d, ((const intptr_t[]){__VA_ARGS__}), \
                 cspan_rank(self) + c_static_assert(cspan_rank(self) == c_NUMARGS(__VA_ARGS__)))

// Multi-dimensional span constructors
//
typedef enum {c_ROWMAJOR, c_COLMAJOR} cspan_layout;

#define cspan_md(array, ...) \
    cspan_md_layout(c_ROWMAJOR, array, __VA_ARGS__)

#define cspan_md_layout(layout, array, ...) \
    {.data=array, \
     .shape={__VA_ARGS__}, \
     .stride=*(c_JOIN(cspan_tuple,c_NUMARGS(__VA_ARGS__))*) \
             _cspan_shape2stride(layout, ((cstride_t[]){__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))}

// Transpose and swap axes
//
#define cspan_transposed2(self) \
    {.data=(self)->data + c_static_assert(cspan_rank(self) == 2), \
     .shape={(self)->shape[1], (self)->shape[0]}, \
     .stride=(cspan_tuple2){.d={(self)->stride.d[1], (self)->stride.d[0]}}}

#define cspan_transpose(self) \
    _cspan_transpose((self)->shape, (self)->stride.d, cspan_rank(self))

#define cspan_swap_axes(self, ax1, ax2) \
    _cspan_swap_axes((self)->shape, (self)->stride.d, ax1, ax2, cspan_rank(self))

// General slicing function.
//
#define c_END (cextent_t)(((size_t)1 << (sizeof(cextent_t)*8 - 1)) - 1)
#define c_ALL 0,c_END

#define cspan_slice(OutSpan, self, ...) \
    OutSpan##_slice_((self)->data, (self)->shape, (self)->stride.d, \
                     ((const intptr_t[][3]){__VA_ARGS__}), cspan_rank(self) + \
                     c_static_assert(cspan_rank(self) == sizeof((intptr_t[][3]){__VA_ARGS__})/sizeof(intptr_t[3])))

// submd#(): # <= 4 optimized. Reduce rank, like e.g. cspan_slice(Span2, &ms3, {x}, {c_ALL}, {c_ALL});
//
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), \
     .shape={(self)->shape[1]}, \
     .stride=(cspan_tuple1){.d={(self)->stride.d[1]}}}

#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), \
     .shape={(self)->shape[1], (self)->shape[2]}, \
     .stride=(cspan_tuple2){.d={(self)->stride.d[1], (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), \
     .shape={(self)->shape[2]}, \
     .stride=(cspan_tuple1){.d={(self)->stride.d[2]}}}

#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), \
     .shape={(self)->shape[1], (self)->shape[2], (self)->shape[3]}, \
     .stride=(cspan_tuple3){.d={(self)->stride.d[1], (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), \
     .shape={(self)->shape[2], (self)->shape[3]}, \
     .stride=(cspan_tuple2){.d={(self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), \
     .shape={(self)->shape[3]}, \
     .stride=(cspan_tuple1){.d={(self)->stride.d[3]}}}

#define cspan_print(...) c_MACRO_OVERLOAD(cspan_print, __VA_ARGS__)
#define cspan_print_2(Span, span) /* c11 */ \
    cspan_print_3(Span, span, _Generic(*(span).data, \
        float:"%g", double:"%g", \
        uint8_t:"%d", int8_t:"%d", int16_t:"%d", \
        int32_t:"%" PRIi32, int64_t:"%" PRIi64))
#define cspan_print_3(Span, span, fmt) \
    cspan_print_4(Span, span, fmt, stdout)
#define cspan_print_4(Span, span, fmt, fp) \
    cspan_print_5(Span, span, fmt, fp, "[]")
#define cspan_print_5(Span, span, fmt, fp, brackets) \
    cspan_print_6(Span, span, fmt, fp, brackets, c_EXPAND)
#define cspan_print_complex(Span, span, prec) \
    cspan_print_6(Span, span, "%." #prec "f%+." #prec "fi", stdout, "[]", cspan_CMPLX_FLD)
#define cspan_CMPLX_FLD(x) creal(x), cimag(x)

#define cspan_print_6(Span, span, fmt, fp, brackets, field) do { \
    const Span _s = span; \
    const char *_f = fmt, *_b = brackets; \
    FILE* _fp = fp; \
    int _w, _max = 0; \
    char _res[2][16], _fld[128]; \
    c_foreach_3 (_it, Span, _s) { \
        _w = snprintf(NULL, 0ULL, _f, field(_it.ref[0])); \
        if (_w > _max) _max = _w; \
    } \
    c_foreach_3 (_it, Span, _s) { \
        _cspan_print_assist(_it.pos, _s.shape, cspan_rank(&_s), _res, _b); \
        _w = _max + (_it.pos[cspan_rank(&_s) - 1] > 0); \
        sprintf(_fld, _f, field(_it.ref[0])); \
        fprintf(_fp, "%s%*s%s", _res[0], _w, _fld, _res[1]); \
    } \
} while (0)

/* ------------------- PRIVAT DEFINITIONS ------------------- */

STC_INLINE intptr_t _cspan_size(const cextent_t shape[], int rank) {
    intptr_t size = shape[0];
    while (--rank) size *= shape[rank];
    return size;
}

STC_INLINE void _cspan_swap_axes(cextent_t shape[], cstride_t stride[], int i, int j, int rank) {
    (void)rank;
    c_assert(c_uless(i, rank) & c_uless(j, rank));
    c_swap(cextent_t, shape + i, shape + j);
    c_swap(cstride_t, stride + i, stride + j);
}

STC_INLINE void _cspan_transpose(cextent_t shape[], cstride_t stride[], int rank) {
    for (int i = 0; i < --rank; ++i) {
        c_swap(cextent_t, shape + i, shape + rank);
        c_swap(cstride_t, stride + i, stride + rank);
    }
}

STC_INLINE intptr_t _cspan_index(const cextent_t shape[], const cstride_t stride[],
                                 const intptr_t args[], int rank) {
    intptr_t off = 0;
    (void)shape;
    while (rank--) {
        c_assert(c_uless(args[rank], shape[rank]));
        off += args[rank]*stride[rank];
    }
    return off;
}

STC_API void _cspan_print_assist(cextent_t pos[], const cextent_t shape[], const int rank,
                                 char result[2][16], const char* brackets);

STC_API intptr_t _cspan_next2(cextent_t pos[], const cextent_t shape[], const cstride_t stride[],
                              int rank, int* done);
#define _cspan_next1(pos, shape, stride, rank, done) (*done = ++pos[0]==shape[0], stride[0])
#define _cspan_next3 _cspan_next2
#define _cspan_next4 _cspan_next2
#define _cspan_next5 _cspan_next2
#define _cspan_next6 _cspan_next2
#define _cspan_next7 _cspan_next2
#define _cspan_next8 _cspan_next2

STC_API intptr_t _cspan_slice(cextent_t oshape[], cstride_t ostride[], int* orank,
                              const cextent_t shape[], const cstride_t stride[],
                              const intptr_t args[][3], int rank);

STC_API cstride_t* _cspan_shape2stride(cspan_layout layout, cstride_t shape[], int rank);
#endif // STC_CSPAN_H_INCLUDED

/* --------------------- IMPLEMENTATION --------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF void _cspan_print_assist(cextent_t pos[], const cextent_t shape[], const int rank,
                                 char result[2][16], const char* brackets) {
    int n = 0, j = 0, r = rank - 1;
    memset(result, 0, 32);

    while (n <= r && pos[r - n] == 0) ++n;
    if (n) for (; j < rank; ++j)
        result[0][j] = j < rank - n ? ' ' : brackets[0];
    for (j = 0; r >= 0 && pos[r] + 1 == shape[r]; --r, ++j)
        result[1][j] = brackets[1];

    n = (j > 0) + ((j > 1) & (j < rank)); // newlines
    if (brackets[2] && j < rank) result[1][j++] = brackets[2]; // comma
    while (n--) result[1][j++] = '\n';
}

STC_DEF intptr_t _cspan_next2(cextent_t pos[], const cextent_t shape[], const cstride_t stride[],
                              int r, int* done) {
    intptr_t off = stride[--r];
    ++pos[r];

    for (; r && pos[r] == shape[r]; --r) {
        pos[r] = 0; ++pos[r - 1];
        off += stride[r - 1] - (intptr_t)shape[r]*stride[r];
    }
    *done = pos[r] == shape[r];
    return off;
}

STC_DEF cstride_t* _cspan_shape2stride(cspan_layout layout, cstride_t shpstri[], int rank) {
    int i, inc;
    if (layout == c_COLMAJOR) i = 0, inc = 1;
    else i = rank - 1, inc = -1;
    cstride_t k = 1, s1 = shpstri[i], s2;

    shpstri[i] = 1;
    while (--rank) {
        i += inc;
        s2 = shpstri[i];
        shpstri[i] = (k *= s1);
        s1 = s2;
    }
    return shpstri;
}

STC_DEF intptr_t _cspan_slice(cextent_t oshape[], cstride_t ostride[], int* orank,
                              const cextent_t shape[], const cstride_t stride[],
                              const intptr_t args[][3], int rank) {
    intptr_t end, off = 0;
    int i = 0, oi = 0;

    for (; i < rank; ++i) {
        off += args[i][0]*stride[i];
        switch (args[i][1]) {
            case 0: c_assert(c_uless(args[i][0], shape[i])); continue;
            case c_END: end = shape[i]; break;
            default: end = args[i][1];
        }
        oshape[oi] = (cextent_t)(end - args[i][0]);
        ostride[oi] = stride[i];
        c_assert((oshape[oi] > 0) & !c_uless(shape[i], end));
        if (args[i][2] > 0) {
            ostride[oi] *= (cextent_t)args[i][2];
            oshape[oi] = (oshape[oi] - 1)/(cextent_t)args[i][2] + 1;
        }
        ++oi;
    }
    *orank = oi;
    return off;
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
#endif
// ### END_FILE_INCLUDE: cspan.h


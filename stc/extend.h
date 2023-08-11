// ### BEGIN_FILE_INCLUDE: extend.h
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
// ### BEGIN_FILE_INCLUDE: forward.h
#ifndef STC_FORWARD_H_INCLUDED
#define STC_FORWARD_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define forward_carc(CX, VAL) _c_carc_types(CX, VAL)
#define forward_cbox(CX, VAL) _c_cbox_types(CX, VAL)
#define forward_cdeq(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_clist(CX, VAL) _c_clist_types(CX, VAL)
#define forward_cmap(CX, KEY, VAL) _c_chash_types(CX, KEY, VAL, c_true, c_false)
#define forward_cset(CX, KEY) _c_chash_types(CX, cset, KEY, KEY, c_false, c_true)
#define forward_csmap(CX, KEY, VAL) _c_aatree_types(CX, KEY, VAL, c_true, c_false)
#define forward_csset(CX, KEY) _c_aatree_types(CX, KEY, KEY, c_false, c_true)
#define forward_cstack(CX, VAL) _c_cstack_types(CX, VAL)
#define forward_cpque(CX, VAL) _c_cpque_types(CX, VAL)
#define forward_cqueue(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_cvec(CX, VAL) _c_cvec_types(CX, VAL)

// csview
typedef const char csview_value;
typedef struct csview { 
    csview_value* str; 
    intptr_t size;
} csview;

typedef union { 
    csview_value* ref; 
    struct { csview chr; csview_value* end; } u8;
} csview_iter;

// cstr
typedef char cstr_value;
typedef struct { cstr_value* data; intptr_t size, cap; } cstr_buf;
typedef union cstr {
    struct { cstr_value data[sizeof(cstr_buf) - 1]; unsigned char size; } sml;
    struct { cstr_value* data; size_t size, ncap; } lon;
} cstr;

typedef union { 
    cstr_value* ref; 
    struct { csview chr; } u8;
} cstr_iter;

#define c_true(...) __VA_ARGS__
#define c_false(...)

#define _c_carc_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
        catomic_long* use_count; \
    } SELF

#define _c_cbox_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
    } SELF

#define _c_cdeq_types(SELF, VAL) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value *data; \
        intptr_t start, end, capmask; \
    } SELF; \
\
    typedef struct { \
        SELF##_value *ref; \
        intptr_t pos; \
        const SELF* _s; \
    } SELF##_iter

#define _c_clist_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef struct { \
        SELF##_value *ref; \
        SELF##_node *const *_last, *prev; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_node *last; \
    } SELF

#define _c_chash_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
\
    typedef struct { \
        SELF##_value *ref; \
        bool inserted; \
        uint8_t hashx; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref, *_end; \
        struct chash_slot* sref; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value* data; \
        struct chash_slot* slot; \
        intptr_t size, bucket_count; \
    } SELF

#define _c_aatree_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
\
    typedef struct { \
        SELF##_value *ref; \
        bool inserted; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref; \
        SELF##_node *_d; \
        int _top; \
        int32_t _tn, _st[36]; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_node *nodes; \
        int32_t root, disp, head, size, cap; \
    } SELF

#define _c_cstack_fixed(SELF, VAL, CAP) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value data[CAP]; intptr_t _len; } SELF

#define _c_cstack_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value* data; intptr_t _len, _cap; } SELF

#define _c_cvec_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value *data; intptr_t _len, _cap; } SELF

#define _c_cpque_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { SELF##_value* data; intptr_t _len, _cap; } SELF

#endif // STC_FORWARD_H_INCLUDED
// ### END_FILE_INCLUDE: forward.h

#ifdef i_key_str
  #define _i_key cstr
#elif defined i_keyclass
  #define _i_key i_keyclass
#elif defined i_keyboxed
  #define _i_key i_keyboxed
#elif defined i_key
  #define _i_key i_key
#endif

#ifdef i_val_str
  #define _i_val cstr
#elif defined i_valclass
  #define _i_val i_valclass
#elif defined i_valboxed
  #define _i_val i_valboxed
#elif defined i_val
  #define _i_val i_val
#endif

#if defined _i_key && defined _i_val
  c_PASTE(forward_, i_base)(i_type, _i_key, _i_val);
#elif defined _i_key
  c_PASTE(forward_, i_base)(i_type, _i_key);
#else
  c_PASTE(forward_, i_base)(i_type, _i_val);
#endif

typedef struct {
    i_extend
    i_type get;
} c_PASTE(i_type, _ext);

#define c_extend() c_container_of(self, _cx_MEMB(_ext), get)
// Note: i_less: c_extend() accessible for cpque types
//       i_cmp: c_extend() accessible for csmap and csset types
//       i_hash/i_eq: c_extend() accessible for cmap and cset types

#define i_is_forward
#define _i_inc <stc/i_base.h>
#include _i_inc
#undef _i_inc
#undef _i_key
#undef _i_val
#undef i_base
#undef i_extend
// ### END_FILE_INCLUDE: extend.h


// ### BEGIN_FILE_INCLUDE: carc.h

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

#ifndef CARC_H_INCLUDED
#define CARC_H_INCLUDED
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
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
    typedef long catomic_long;
    #define c_atomic_inc(v) (void)__atomic_add_fetch(v, 1, __ATOMIC_SEQ_CST)
    #define c_atomic_dec_and_test(v) !__atomic_sub_fetch(v, 1, __ATOMIC_SEQ_CST)
#elif defined(_MSC_VER)
    #include <intrin.h>
    typedef long catomic_long;
    #define c_atomic_inc(v) (void)_InterlockedIncrement(v)
    #define c_atomic_dec_and_test(v) !_InterlockedDecrement(v)
#else
    #include <stdatomic.h>
    typedef _Atomic long catomic_long;
    #define c_atomic_inc(v) (void)atomic_fetch_add(v, 1)
    #define c_atomic_dec_and_test(v) (atomic_fetch_sub(v, 1) == 1)
#endif

#define carc_null {0}
#endif // CARC_H_INCLUDED

#define _i_prefix carc_
#define _i_carc
// ### BEGIN_FILE_INCLUDE: template.h
#ifndef _i_template
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define _cx_Self i_type
  #define _cx_MEMB(name) c_PASTE(_cx_Self, name)
  #define _cx_DEFTYPES(macro, SELF, ...) c_EXPAND(macro(SELF, __VA_ARGS__))
  #define _cx_value _cx_MEMB(_value)
  #define _cx_key _cx_MEMB(_key)
  #define _cx_mapped _cx_MEMB(_mapped)
  #define _cx_raw _cx_MEMB(_raw)
  #define _cx_keyraw _cx_MEMB(_keyraw)
  #define _cx_iter _cx_MEMB(_iter)
  #define _cx_result _cx_MEMB(_result)
  #define _cx_node _cx_MEMB(_node)
#endif

#ifndef i_type
  #define i_type c_PASTE(_i_prefix, i_tag)
#endif

#ifndef i_allocator
  #define i_allocator c
#endif
#ifndef i_malloc
  #define i_malloc c_PASTE(i_allocator, _malloc)
  #define i_calloc c_PASTE(i_allocator, _calloc)
  #define i_realloc c_PASTE(i_allocator, _realloc)
  #define i_free c_PASTE(i_allocator, _free)
#endif

#if !(defined i_key || defined i_key_str || defined i_key_ssv || \
      defined i_keyclass || defined i_keyboxed)
  #if defined _i_ismap
    #error "i_key* must be defined for maps"
  #endif

  #if defined i_val_str
    #define i_key_str i_val_str
  #endif
  #if defined i_val_ssv
    #define i_key_ssv i_val_ssv
  #endif  
  #if defined i_valboxed
    #define i_keyboxed i_valboxed
  #endif
  #if defined i_valclass
    #define i_keyclass i_valclass
  #endif
  #if defined i_val
    #define i_key i_val
  #endif
  #if defined i_valraw
    #define i_keyraw i_valraw
  #endif
  #if defined i_valclone
    #define i_keyclone i_valclone
  #endif
  #if defined i_valfrom
    #define i_keyfrom i_valfrom
  #endif
  #if defined i_valto
    #define i_keyto i_valto
  #endif
  #if defined i_valdrop
    #define i_keydrop i_valdrop
  #endif
#endif

#if c_option(c_is_forward)
  #define i_is_forward
#endif
#if c_option(c_no_hash)
  #define i_no_hash
#endif
#if c_option(c_no_emplace)
  #define i_no_emplace
#endif
#if c_option(c_use_cmp) || defined _i_ismap || defined _i_isset || defined _i_ispque
  #define i_use_cmp
#endif
#if c_option(c_no_clone) || defined _i_carc
  #define i_no_clone
#endif

#if defined i_key_str
  #define i_keyclass cstr
  #define i_rawclass ccharptr
  #ifndef i_tag
    #define i_tag str
  #endif
#elif defined i_key_ssv
  #define i_keyclass cstr
  #define i_rawclass csview
  #define i_keyfrom cstr_from_sv
  #define i_keyto cstr_sv
  #define i_eq csview_eq
  #ifndef i_tag
    #define i_tag ssv
  #endif
#elif defined i_keyboxed
  #define i_keyclass i_keyboxed
  #define i_rawclass c_PASTE(i_keyboxed, _raw)
  #if defined i_use_cmp
    #define i_eq c_PASTE(i_keyboxed, _raw_eq)
  #endif
#endif

#if defined i_rawclass
  #define i_keyraw i_rawclass
#elif defined i_keyclass && !defined i_keyraw
  #define i_rawclass i_key
#endif

#if defined i_keyclass
  #define i_key i_keyclass
  #ifndef i_keyclone
    #define i_keyclone c_PASTE(i_key, _clone)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_PASTE(i_key, _drop)
  #endif
  #if !defined i_keyfrom && defined i_keyraw
    #define i_keyfrom c_PASTE(i_key, _from)
  #endif
  #if !defined i_keyto && defined i_keyraw
    #define i_keyto c_PASTE(i_key, _toraw)
  #endif
  #if !defined i_keyraw && (defined i_cmp || defined i_less || defined i_eq || defined i_hash)
    #define i_use_cmp
  #endif
#endif

#if defined i_rawclass && defined i_use_cmp
  #if !(defined i_cmp || defined i_less)
    #define i_cmp c_PASTE(i_keyraw, _cmp)
  #endif
  #if !(defined i_hash || defined i_no_hash)
    #define i_hash c_PASTE(i_keyraw, _hash)
  #endif
#endif

#if defined i_cmp || defined i_less || defined i_use_cmp
  #define _i_has_cmp
#endif
#if defined i_eq || defined i_use_cmp
  #define _i_has_eq
#endif
#if !(defined i_hash || defined i_no_hash)
  #define i_hash c_default_hash
#endif

#if !defined i_key
  #error "No i_key or i_val defined"
#elif defined i_keyraw ^ defined i_keyto
  #error "Both i_keyraw/i_valraw and i_keyto/i_valto must be defined, if any"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone/i_valclone and i_keydrop/i_valdrop must be defined, if any (unless i_no_clone defined)."
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Define i_keyfrom/i_valfrom and/or i_keydrop/i_valdrop instead"
#elif defined i_keyraw && defined _i_ishash && !(defined i_hash && (defined _i_has_cmp || defined i_eq))
  #error "For cmap/cset, both i_hash and i_eq (or i_less or i_cmp) must be defined when i_keyraw is defined."
#elif defined i_keyraw && defined i_use_cmp && !defined _i_has_cmp
  #error "For csmap/csset/cpque, i_cmp or i_less must be defined when i_keyraw is defined."
#endif

#ifndef i_tag
  #define i_tag i_key
#endif
#ifndef i_keyraw
  #define i_keyraw i_key
#endif
#ifndef i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #define i_has_emplace
#endif
#ifndef i_keyto
  #define i_keyto c_default_toraw
#endif
#ifndef i_keyclone
  #define i_keyclone c_default_clone
#endif
#ifndef i_keydrop
  #define i_keydrop c_default_drop
#endif

// i_eq, i_less, i_cmp
#if !defined i_eq && (defined i_cmp || defined i_less)
  #define i_eq(x, y) !(i_cmp(x, y))
#elif !defined i_eq
  #define i_eq(x, y) *x == *y
#endif
#if defined i_cmp && defined i_less
  #error "Only one of i_cmp and i_less may be defined"
#elif defined i_cmp
  #define i_less(x, y) (i_cmp(x, y)) < 0
#elif !defined i_less
  #define i_less(x, y) *x < *y
#endif
#ifndef i_cmp
  #define i_cmp(x, y) (i_less(y, x)) - (i_less(x, y))
#endif

#if defined _i_ismap // ---- process cmap/csmap value i_val, ... ----

#ifdef i_val_str
  #define i_valclass cstr
  #define i_valraw const char*
#elif defined i_val_ssv
  #define i_valclass cstr
  #define i_valraw csview
  #define i_valfrom cstr_from_sv
  #define i_valto cstr_sv
#elif defined i_valboxed
  #define i_valclass i_valboxed
  #define i_valraw c_PASTE(i_valboxed, _raw)
#endif

#ifdef i_valclass
  #define i_val i_valclass
  #ifndef i_valclone
    #define i_valclone c_PASTE(i_val, _clone)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_PASTE(i_val, _drop)
  #endif
  #if !defined i_valfrom && defined i_valraw
    #define i_valfrom c_PASTE(i_val, _from)
  #endif
  #if !defined i_valto && defined i_valraw
    #define i_valto c_PASTE(i_val, _toraw)
  #endif
#endif

#ifndef i_val
  #error "i_val* must be defined for maps"
#elif defined i_valraw ^ defined i_valto
  #error "Both i_valraw and i_valto must be defined, if any"
#elif !defined i_no_clone && (defined i_valclone ^ defined i_valdrop)
  #error "Both i_valclone and i_valdrop must be defined, if any"
#endif

#ifndef i_valraw
  #define i_valraw i_val
#endif
#ifndef i_valfrom
  #define i_valfrom c_default_clone
#else
  #define i_has_emplace
#endif
#ifndef i_valto
  #define i_valto c_default_toraw
#endif
#ifndef i_valclone
  #define i_valclone c_default_clone
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#endif

#endif // !_i_ismap

#ifndef i_val
  #define i_val i_key
#endif
#ifndef i_valraw
  #define i_valraw i_keyraw
#endif
#ifndef i_has_emplace
  #define i_no_emplace
#endif
#endif
// ### END_FILE_INCLUDE: template.h
typedef i_keyraw _cx_raw;

#if c_option(c_no_atomic)
  #define i_no_atomic
#endif
#if !defined i_no_atomic
  #define _i_atomic_inc(v)          c_atomic_inc(v)
  #define _i_atomic_dec_and_test(v) c_atomic_dec_and_test(v)
#else
  #define _i_atomic_inc(v)          (void)(++*(v))
  #define _i_atomic_dec_and_test(v) !(--*(v))
#endif
#ifndef i_is_forward
_cx_DEFTYPES(_c_carc_types, _cx_Self, i_key);
#endif
struct _cx_MEMB(_rep_) { catomic_long counter; i_key value; };

STC_INLINE _cx_Self _cx_MEMB(_init)(void) 
    { return c_LITERAL(_cx_Self){NULL, NULL}; }

STC_INLINE long _cx_MEMB(_use_count)(const _cx_Self* self)
    { return self->use_count ? *self->use_count : 0; }

STC_INLINE _cx_Self _cx_MEMB(_from_ptr)(_cx_value* p) {
    _cx_Self ptr = {p};
    if (p) 
        *(ptr.use_count = _i_alloc(catomic_long)) = 1;
    return ptr;
}

// c++: std::make_shared<_cx_value>(val)
STC_INLINE _cx_Self _cx_MEMB(_make)(_cx_value val) {
    _cx_Self ptr;
    struct _cx_MEMB(_rep_)* rep = _i_alloc(struct _cx_MEMB(_rep_));
    *(ptr.use_count = &rep->counter) = 1;
    *(ptr.get = &rep->value) = val;
    return ptr;
}

STC_INLINE _cx_raw _cx_MEMB(_toraw)(const _cx_Self* self)
    { return i_keyto(self->get); }

STC_INLINE _cx_Self _cx_MEMB(_move)(_cx_Self* self) {
    _cx_Self ptr = *self;
    self->get = NULL, self->use_count = NULL;
    return ptr;
}

STC_INLINE void _cx_MEMB(_drop)(_cx_Self* self) {
    if (self->use_count && _i_atomic_dec_and_test(self->use_count)) {
        i_keydrop(self->get);
        if ((char *)self->get != (char *)self->use_count + offsetof(struct _cx_MEMB(_rep_), value))
            i_free(self->get);
        i_free((long*)self->use_count);
    }
}

STC_INLINE void _cx_MEMB(_reset)(_cx_Self* self) {
    _cx_MEMB(_drop)(self);
    self->use_count = NULL, self->get = NULL;
}

STC_INLINE void _cx_MEMB(_reset_to)(_cx_Self* self, _cx_value* p) {
    _cx_MEMB(_drop)(self);
    *self = _cx_MEMB(_from_ptr)(p);
}

#ifndef i_no_emplace
STC_INLINE _cx_Self _cx_MEMB(_from)(_cx_raw raw)
    { return _cx_MEMB(_make)(i_keyfrom(raw)); }
#else
STC_INLINE _cx_Self _cx_MEMB(_from)(_cx_value val)
    { return _cx_MEMB(_make)(val); }
#endif    

// does not use i_keyclone, so OK to always define.
STC_INLINE _cx_Self _cx_MEMB(_clone)(_cx_Self ptr) {
    if (ptr.use_count)
        _i_atomic_inc(ptr.use_count);
    return ptr;
}

// take ownership of unowned
STC_INLINE void _cx_MEMB(_take)(_cx_Self* self, _cx_Self unowned) {
    _cx_MEMB(_drop)(self);
    *self = unowned;
}
// share ownership with ptr
STC_INLINE void _cx_MEMB(_assign)(_cx_Self* self, _cx_Self ptr) {
    if (ptr.use_count)
        _i_atomic_inc(ptr.use_count);
    _cx_MEMB(_drop)(self);
    *self = ptr;
}

#if defined i_use_cmp
    STC_INLINE int _cx_MEMB(_raw_cmp)(const _cx_raw* rx, const _cx_raw* ry)
        { return i_cmp(rx, ry); }

    STC_INLINE int _cx_MEMB(_cmp)(const _cx_Self* self, const _cx_Self* other) {
        _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_cmp((&rx), (&ry));
    }

    STC_INLINE bool _cx_MEMB(_raw_eq)(const _cx_raw* rx, const _cx_raw* ry)
        { return i_eq(rx, ry); }

    STC_INLINE bool _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
        _cx_raw rx = i_keyto(self->get), ry = i_keyto(other->get);
        return i_eq((&rx), (&ry));
    }

    #ifndef i_no_hash
    STC_INLINE uint64_t _cx_MEMB(_raw_hash)(const _cx_raw* rx)
        { return i_hash(rx); }

    STC_INLINE uint64_t _cx_MEMB(_hash)(const _cx_Self* self)
        { _cx_raw rx = i_keyto(self->get); return i_hash(&rx); }
    #endif // i_no_hash

#else

    STC_INLINE int _cx_MEMB(_cmp)(const _cx_Self* self, const _cx_Self* other) {
        return c_default_cmp(&self->get, &other->get);
    }
    STC_INLINE bool _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
        return self->get == other->get;
    }
    STC_INLINE uint64_t _cx_MEMB(_hash)(const _cx_Self* self)
        { return c_default_hash(&self->get); }
#endif // i_use_cmp

// ### BEGIN_FILE_INCLUDE: template2.h
#ifdef i_more
#undef i_more
#else
#undef i_type
#undef i_tag
#undef i_imp
#undef i_opt
#undef i_less
#undef i_cmp
#undef i_eq
#undef i_hash
#undef i_rawclass
#undef i_capacity

#undef i_val
#undef i_val_str
#undef i_val_ssv
#undef i_valboxed
#undef i_valclass
#undef i_valraw
#undef i_valclone
#undef i_valfrom
#undef i_valto
#undef i_valdrop

#undef i_key
#undef i_key_str
#undef i_key_ssv
#undef i_keyboxed
#undef i_keyclass
#undef i_keyraw
#undef i_keyclone
#undef i_keyfrom
#undef i_keyto
#undef i_keydrop

#undef i_header
#undef i_implement
#undef i_static
#undef i_import

#undef i_allocator
#undef i_malloc
#undef i_calloc
#undef i_realloc
#undef i_free

#undef i_use_cmp
#undef i_no_hash
#undef i_no_clone
#undef i_no_emplace
#undef i_is_forward
#undef i_has_emplace

#undef _i_has_cmp
#undef _i_has_eq
#undef _i_prefix
#undef _i_expandby
#undef _i_template
#endif
// ### END_FILE_INCLUDE: template2.h
#undef i_no_atomic
#undef _i_atomic_inc
#undef _i_atomic_dec_and_test
#undef _i_carc
// ### END_FILE_INCLUDE: carc.h


// ### BEGIN_FILE_INCLUDE: csset.h

// Sorted set - implemented as an AA-tree (balanced binary tree).

#define _i_prefix csset_
// ### BEGIN_FILE_INCLUDE: csmap.h

// Sorted/Ordered set and map - implemented as an AA-tree.
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

#ifndef CSMAP_H_INCLUDED
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
#define c_no_cmp                (1<<4)
#define c_native_cmp            (1<<5)
#define c_no_hash               (1<<6)
/* Function macros and others */

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

typedef struct chash_slot chash_slot;

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
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref, *_end; \
        chash_slot* sref; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value* data; \
        chash_slot* slot; \
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
#include <string.h>

#endif // CSMAP_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix csmap_
  #define _i_ismap
  #define _i_MAP_ONLY c_true
  #define _i_SET_ONLY c_false
  #define _i_keyref(vp) (&(vp)->first)
#else
  #define _i_isset
  #define _i_MAP_ONLY c_false
  #define _i_SET_ONLY c_true
  #define _i_keyref(vp) (vp)
#endif
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
#if c_option(c_no_cmp)
  #define i_no_cmp
#endif
#if c_option(c_no_hash)
  #define i_no_hash
#endif
#if c_option(c_no_emplace)
  #define i_no_emplace
#endif
#if c_option(c_native_cmp)
  #define i_cmp_native
#endif
#if c_option(c_no_clone) || defined _i_carc
  #define i_no_clone
#endif

#if defined i_key_str
  #define i_keyclass cstr
  #define i_rawclass crawstr
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
  #ifndef i_no_cmp
    #define i_eq c_PASTE(i_keyboxed, _raw_eq)
  #endif
#endif

#if defined i_rawclass
  #define i_keyraw i_rawclass
#elif defined i_keyclass && !defined i_keyraw
  #define i_rawclass i_key
#endif

#ifdef i_keyclass
  #define i_key i_keyclass
  #ifndef i_keyclone
    #define i_keyclone c_PASTE(i_key, _clone)
  #endif
  #if !defined i_keyto && defined i_keyraw
    #define i_keyto c_PASTE(i_key, _toraw)
  #endif
  #if !defined i_keyfrom && defined i_keyraw
    #define i_keyfrom c_PASTE(i_key, _from)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_PASTE(i_key, _drop)
  #endif
#endif

#ifdef i_rawclass
  #if !(defined i_cmp || defined i_no_cmp)
    #define i_cmp c_PASTE(i_keyraw, _cmp)
  #endif
  #if !(defined i_hash || defined i_no_hash || defined i_no_cmp)
    #define i_hash c_PASTE(i_keyraw, _hash)
  #endif
#endif

#if !defined i_keyraw && !defined i_no_clone
  #if !defined i_keyfrom && defined i_keyclone
    #define i_keyfrom i_keyclone
  #elif !defined i_keyclone && defined i_keyfrom
    #define i_keyclone i_keyfrom
  #endif
#endif

#if !defined i_key
  #error "No i_key or i_val defined"
#elif defined i_keyraw ^ defined i_keyto
  #error "Both i_keyraw/i_valraw and i_keyto/i_valto must be defined, if any"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone/i_valclone and i_keydrop/i_valdrop must be defined, if any"
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Define i_keyfrom/i_valfrom and/or i_keydrop/i_valdrop instead"
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

#ifndef i_no_cmp
  #if defined i_cmp || defined i_less || defined i_cmp_native
    #define _i_has_cmp
  #endif
  #if defined i_eq || defined i_cmp_native
    #define _i_has_eq
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
#endif

#if !defined i_hash && (!(defined _i_cbox || defined _i_carc) || defined i_cmp_native)
  #define i_hash c_default_hash
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
  #if !defined i_valto && defined i_valraw
    #define i_valto c_PASTE(i_val, _toraw)
  #endif
  #if !defined i_valfrom && defined i_valraw
    #define i_valfrom c_PASTE(i_val, _from)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_PASTE(i_val, _drop)
  #endif
#endif

#if !defined i_valraw && !defined i_no_clone
  #if !defined i_valfrom && defined i_valclone
    #define i_valfrom i_valclone
  #elif !defined i_valclone && defined i_valfrom
    #define i_valclone i_valfrom
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
#ifndef i_is_forward
  _cx_DEFTYPES(_c_aatree_types, _cx_Self, i_key, i_val, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _cx_value {
    _cx_key first;
    _cx_mapped second;
}; )
struct _cx_node {
    int32_t link[2];
    int8_t level;
    _cx_value value;
};

typedef i_keyraw _cx_keyraw;
typedef i_valraw _cx_MEMB(_rmapped);
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { i_keyraw first; i_valraw second; } )
        _cx_raw;

#if !defined i_no_emplace
STC_API _cx_result      _cx_MEMB(_emplace)(_cx_Self* self, _cx_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped));
#endif // !i_no_emplace
#if !defined i_no_clone
STC_API _cx_Self        _cx_MEMB(_clone)(_cx_Self tree);
#endif // !i_no_clone
STC_API void            _cx_MEMB(_drop)(_cx_Self* self);
STC_API bool            _cx_MEMB(_reserve)(_cx_Self* self, intptr_t cap);
STC_API _cx_value*      _cx_MEMB(_find_it)(const _cx_Self* self, _cx_keyraw rkey, _cx_iter* out);
STC_API _cx_iter        _cx_MEMB(_lower_bound)(const _cx_Self* self, _cx_keyraw rkey);
STC_API _cx_value*      _cx_MEMB(_front)(const _cx_Self* self);
STC_API _cx_value*      _cx_MEMB(_back)(const _cx_Self* self);
STC_API int             _cx_MEMB(_erase)(_cx_Self* self, _cx_keyraw rkey);
STC_API _cx_iter        _cx_MEMB(_erase_at)(_cx_Self* self, _cx_iter it);
STC_API _cx_iter        _cx_MEMB(_erase_range)(_cx_Self* self, _cx_iter it1, _cx_iter it2);
STC_API _cx_iter        _cx_MEMB(_begin)(const _cx_Self* self);
STC_API void            _cx_MEMB(_next)(_cx_iter* it);

STC_INLINE _cx_Self     _cx_MEMB(_init)(void) { _cx_Self tree = {0}; return tree; }
STC_INLINE bool         _cx_MEMB(_empty)(const _cx_Self* cx) { return cx->size == 0; }
STC_INLINE intptr_t     _cx_MEMB(_size)(const _cx_Self* cx) { return cx->size; }
STC_INLINE intptr_t     _cx_MEMB(_capacity)(const _cx_Self* cx) { return cx->cap; }
STC_INLINE _cx_iter     _cx_MEMB(_find)(const _cx_Self* self, _cx_keyraw rkey)
                            { _cx_iter it; _cx_MEMB(_find_it)(self, rkey, &it); return it; }
STC_INLINE bool         _cx_MEMB(_contains)(const _cx_Self* self, _cx_keyraw rkey)
                            { _cx_iter it; return _cx_MEMB(_find_it)(self, rkey, &it) != NULL; }
STC_INLINE const _cx_value* _cx_MEMB(_get)(const _cx_Self* self, _cx_keyraw rkey)
                            { _cx_iter it; return _cx_MEMB(_find_it)(self, rkey, &it); }
STC_INLINE _cx_value*   _cx_MEMB(_get_mut)(_cx_Self* self, _cx_keyraw rkey)
                            { _cx_iter it; return _cx_MEMB(_find_it)(self, rkey, &it); }

STC_INLINE _cx_Self
_cx_MEMB(_with_capacity)(const intptr_t cap) {
    _cx_Self tree = _cx_MEMB(_init)();
    _cx_MEMB(_reserve)(&tree, cap);
    return tree;
}

STC_INLINE void
_cx_MEMB(_clear)(_cx_Self* self) 
    { _cx_MEMB(_drop)(self); *self = _cx_MEMB(_init)(); }

STC_INLINE _cx_raw
_cx_MEMB(_value_toraw)(const _cx_value* val) {
    return _i_SET_ONLY( i_keyto(val) )
           _i_MAP_ONLY( c_LITERAL(_cx_raw){i_keyto((&val->first)), 
                                        i_valto((&val->second))} );
}

STC_INLINE void
_cx_MEMB(_value_drop)(_cx_value* val) {
    i_keydrop(_i_keyref(val));
    _i_MAP_ONLY( i_valdrop((&val->second)); )
}

#if !defined i_no_clone
STC_INLINE _cx_value
_cx_MEMB(_value_clone)(_cx_value _val) {
    *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
    _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
    return _val;
}

STC_INLINE void
_cx_MEMB(_copy)(_cx_Self *self, const _cx_Self* other) {
    if (self->nodes == other->nodes)
        return;
    _cx_MEMB(_drop)(self);
    *self = _cx_MEMB(_clone)(*other);
}

STC_INLINE void
_cx_MEMB(_shrink_to_fit)(_cx_Self *self) {
    _cx_Self tmp = _cx_MEMB(_clone)(*self);
    _cx_MEMB(_drop)(self); *self = tmp;
}
#endif // !i_no_clone

#ifndef _i_isset
    STC_API _cx_result _cx_MEMB(_insert_or_assign)(_cx_Self* self, i_key key, i_val mapped);
    #if !defined i_no_emplace
        STC_API _cx_result  _cx_MEMB(_emplace_or_assign)(_cx_Self* self, _cx_keyraw rkey, i_valraw rmapped);
    #endif

    STC_INLINE const _cx_mapped*
    _cx_MEMB(_at)(const _cx_Self* self, _cx_keyraw rkey)
        { _cx_iter it; return &_cx_MEMB(_find_it)(self, rkey, &it)->second; }
    STC_INLINE _cx_mapped*
    _cx_MEMB(_at_mut)(_cx_Self* self, _cx_keyraw rkey)
        { _cx_iter it; return &_cx_MEMB(_find_it)(self, rkey, &it)->second; }
#endif // !_i_isset

STC_INLINE _cx_iter
_cx_MEMB(_end)(const _cx_Self* self) {
    (void)self;
    _cx_iter it; it.ref = NULL, it._top = 0, it._tn = 0;
    return it;
}

STC_INLINE _cx_iter
_cx_MEMB(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref)
        _cx_MEMB(_next)(&it);
    return it;
}

STC_INLINE bool
_cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
    if (_cx_MEMB(_size)(self) != _cx_MEMB(_size)(other)) return false;
    _cx_iter i = _cx_MEMB(_begin)(self), j = _cx_MEMB(_begin)(other);
    for (; i.ref; _cx_MEMB(_next)(&i), _cx_MEMB(_next)(&j)) {
        const _cx_keyraw _rx = i_keyto(_i_keyref(i.ref)), _ry = i_keyto(_i_keyref(j.ref));
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}

static _cx_result _cx_MEMB(_insert_entry_)(_cx_Self* self, _cx_keyraw rkey);

STC_INLINE _cx_result
_cx_MEMB(_insert)(_cx_Self* self, i_key _key _i_MAP_ONLY(, i_val _mapped)) {
    _cx_result _res = _cx_MEMB(_insert_entry_)(self, i_keyto((&_key)));
    if (_res.inserted)
        { *_i_keyref(_res.ref) = _key; _i_MAP_ONLY( _res.ref->second = _mapped; )}
    else 
        { i_keydrop((&_key)); _i_MAP_ONLY( i_valdrop((&_mapped)); )}
    return _res;
}

STC_INLINE _cx_value*
_cx_MEMB(_push)(_cx_Self* self, _cx_value _val) {
    _cx_result _res = _cx_MEMB(_insert_entry_)(self, i_keyto(_i_keyref(&_val)));
    if (_res.inserted)
        *_res.ref = _val;
    else
        _cx_MEMB(_value_drop)(&_val);
    return _res.ref;
}

STC_INLINE void
_cx_MEMB(_put_n)(_cx_Self* self, const _cx_raw* raw, intptr_t n) {
    while (n--) 
#if defined _i_isset && defined i_no_emplace
        _cx_MEMB(_insert)(self, *raw++);
#elif defined _i_isset
        _cx_MEMB(_emplace)(self, *raw++);
#elif defined i_no_emplace
        _cx_MEMB(_insert_or_assign)(self, raw->first, raw->second), ++raw;
#else
        _cx_MEMB(_emplace_or_assign)(self, raw->first, raw->second), ++raw;
#endif
}

STC_INLINE _cx_Self
_cx_MEMB(_from_n)(const _cx_raw* raw, intptr_t n)
    { _cx_Self cx = {0}; _cx_MEMB(_put_n)(&cx, raw, n); return cx; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF void
_cx_MEMB(_next)(_cx_iter *it) {
    int32_t tn = it->_tn;
    if (it->_top || tn) {
        while (tn) {
            it->_st[it->_top++] = tn;
            tn = it->_d[tn].link[0];
        }
        tn = it->_st[--it->_top];
        it->_tn = it->_d[tn].link[1];
        it->ref = &it->_d[tn].value;
    } else
        it->ref = NULL;
}

STC_DEF _cx_iter
_cx_MEMB(_begin)(const _cx_Self* self) {
    _cx_iter it;
    it.ref = NULL;
    it._d = self->nodes, it._top = 0;
    it._tn = self->root;
    if (it._tn)
        _cx_MEMB(_next)(&it);
    return it;
}

STC_DEF bool
_cx_MEMB(_reserve)(_cx_Self* self, const intptr_t cap) {
    if (cap <= self->cap)
        return false;
    _cx_node* nodes = (_cx_node*)i_realloc(self->nodes, (cap + 1)*c_sizeof(_cx_node));
    if (!nodes)
        return false;
    nodes[0] = c_LITERAL(_cx_node){0};
    self->nodes = nodes;
    self->cap = (int32_t)cap;
    return true;
}

STC_DEF _cx_value*
_cx_MEMB(_front)(const _cx_Self* self) {
    _cx_node *d = self->nodes;
    int32_t tn = self->root;
    while (d[tn].link[0])
        tn = d[tn].link[0];
    return &d[tn].value;
}

STC_DEF _cx_value*
_cx_MEMB(_back)(const _cx_Self* self) {
    _cx_node *d = self->nodes;
    int32_t tn = self->root;
    while (d[tn].link[1])
        tn = d[tn].link[1];
    return &d[tn].value;
}

static int32_t
_cx_MEMB(_new_node_)(_cx_Self* self, int level) {
    int32_t tn;
    if (self->disp) {
        tn = self->disp;
        self->disp = self->nodes[tn].link[1];
    } else {
        if (self->head == self->cap)
            if (!_cx_MEMB(_reserve)(self, self->head*3/2 + 4))
                return 0;
        tn = ++self->head; /* start with 1, 0 is nullnode. */
    }
    _cx_node* dn = &self->nodes[tn];
    dn->link[0] = dn->link[1] = 0; dn->level = (int8_t)level;
    return tn;
}

#ifndef _i_isset
    STC_DEF _cx_result
    _cx_MEMB(_insert_or_assign)(_cx_Self* self, i_key _key, i_val _mapped) {
        _cx_result _res = _cx_MEMB(_insert_entry_)(self, i_keyto((&_key)));
        _cx_mapped* _mp = _res.ref ? &_res.ref->second : &_mapped;
        if (_res.inserted)
            _res.ref->first = _key;
        else
            { i_keydrop((&_key)); i_valdrop(_mp); }
        *_mp = _mapped;
        return _res;
    }

    #if !defined i_no_emplace
    STC_DEF _cx_result
    _cx_MEMB(_emplace_or_assign)(_cx_Self* self, _cx_keyraw rkey, i_valraw rmapped) {
        _cx_result _res = _cx_MEMB(_insert_entry_)(self, rkey);
        if (_res.inserted)
            _res.ref->first = i_keyfrom(rkey);
        else {
            if (!_res.ref) return _res;
            i_valdrop((&_res.ref->second));
        }
        _res.ref->second = i_valfrom(rmapped);
        return _res;
    }
    #endif // !i_no_emplace
#endif // !_i_isset

STC_DEF _cx_value*
_cx_MEMB(_find_it)(const _cx_Self* self, _cx_keyraw rkey, _cx_iter* out) {
    int32_t tn = self->root;
    _cx_node *d = out->_d = self->nodes;
    out->_top = 0;
    while (tn) {
        int c; const _cx_keyraw _raw = i_keyto(_i_keyref(&d[tn].value));
        if ((c = i_cmp((&_raw), (&rkey))) < 0)
            tn = d[tn].link[1];
        else if (c > 0)
            { out->_st[out->_top++] = tn; tn = d[tn].link[0]; }
        else
            { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); }
    }
    return (out->ref = NULL);
}

STC_DEF _cx_iter
_cx_MEMB(_lower_bound)(const _cx_Self* self, _cx_keyraw rkey) {
    _cx_iter it;
    _cx_MEMB(_find_it)(self, rkey, &it);
    if (!it.ref && it._top) {
        int32_t tn = it._st[--it._top];
        it._tn = it._d[tn].link[1];
        it.ref = &it._d[tn].value;
    }
    return it;
}

STC_DEF int32_t
_cx_MEMB(_skew_)(_cx_node *d, int32_t tn) {
    if (tn && d[d[tn].link[0]].level == d[tn].level) {
        int32_t tmp = d[tn].link[0];
        d[tn].link[0] = d[tmp].link[1];
        d[tmp].link[1] = tn;
        tn = tmp;
    }
    return tn;
}

STC_DEF int32_t
_cx_MEMB(_split_)(_cx_node *d, int32_t tn) {
    if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) {
        int32_t tmp = d[tn].link[1];
        d[tn].link[1] = d[tmp].link[0];
        d[tmp].link[0] = tn;
        tn = tmp;
        ++d[tn].level;
    }
    return tn;
}

static int32_t
_cx_MEMB(_insert_entry_i_)(_cx_Self* self, int32_t tn, const _cx_keyraw* rkey, _cx_result* _res) {
    int32_t up[64], tx = tn;
    _cx_node* d = self->nodes;
    int c, top = 0, dir = 0;
    while (tx) {
        up[top++] = tx;
        const _cx_keyraw _raw = i_keyto(_i_keyref(&d[tx].value));
        if (!(c = i_cmp((&_raw), rkey)))
            { _res->ref = &d[tx].value; return tn; }
        dir = (c < 0);
        tx = d[tx].link[dir];
    }
    if ((tx = _cx_MEMB(_new_node_)(self, 1)) == 0)
        return 0;
    d = self->nodes;
    _res->ref = &d[tx].value;
    _res->inserted = true;
    if (top == 0)
        return tx;
    d[up[top - 1]].link[dir] = tx;
    while (top--) {
        if (top)
            dir = (d[up[top - 1]].link[1] == up[top]);
        up[top] = _cx_MEMB(_skew_)(d, up[top]);
        up[top] = _cx_MEMB(_split_)(d, up[top]);
        if (top)
            d[up[top - 1]].link[dir] = up[top];
    }
    return up[0];
}

static _cx_result
_cx_MEMB(_insert_entry_)(_cx_Self* self, _cx_keyraw rkey) {
    _cx_result res = {NULL};
    int32_t tn = _cx_MEMB(_insert_entry_i_)(self, self->root, &rkey, &res);
    self->root = tn;
    self->size += res.inserted;
    return res;
}

static int32_t
_cx_MEMB(_erase_r_)(_cx_Self *self, int32_t tn, const _cx_keyraw* rkey, int *erased) {
    _cx_node *d = self->nodes;
    if (tn == 0)
        return 0;
    _cx_keyraw raw = i_keyto(_i_keyref(&d[tn].value));
    int32_t tx; int c = i_cmp((&raw), rkey);
    if (c != 0)
        d[tn].link[c < 0] = _cx_MEMB(_erase_r_)(self, d[tn].link[c < 0], rkey, erased);
    else {
        if (!(*erased)++)
            _cx_MEMB(_value_drop)(&d[tn].value);
        if (d[tn].link[0] && d[tn].link[1]) {
            tx = d[tn].link[0];
            while (d[tx].link[1])
                tx = d[tx].link[1];
            d[tn].value = d[tx].value; /* move */
            raw = i_keyto(_i_keyref(&d[tn].value));
            d[tn].link[0] = _cx_MEMB(_erase_r_)(self, d[tn].link[0], &raw, erased);
        } else { /* unlink node */
            tx = tn;
            tn = d[tn].link[ d[tn].link[0] == 0 ];
            /* move it to disposed nodes list */
            d[tx].link[1] = self->disp;
            self->disp = tx;
        }
    }
    tx = d[tn].link[1];
    if (d[d[tn].link[0]].level < d[tn].level - 1 || d[tx].level < d[tn].level - 1) {
        if (d[tx].level > --d[tn].level)
            d[tx].level = d[tn].level;
                       tn = _cx_MEMB(_skew_)(d, tn);
       tx = d[tn].link[1] = _cx_MEMB(_skew_)(d, d[tn].link[1]);
            d[tx].link[1] = _cx_MEMB(_skew_)(d, d[tx].link[1]);
                       tn = _cx_MEMB(_split_)(d, tn);
            d[tn].link[1] = _cx_MEMB(_split_)(d, d[tn].link[1]);
    }
    return tn;
}

STC_DEF int
_cx_MEMB(_erase)(_cx_Self* self, _cx_keyraw rkey) {
    int erased = 0;
    int32_t root = _cx_MEMB(_erase_r_)(self, self->root, &rkey, &erased);
    if (!erased)
        return 0;
    self->root = root;
    --self->size;
    return 1;
}

STC_DEF _cx_iter
_cx_MEMB(_erase_at)(_cx_Self* self, _cx_iter it) {
    _cx_keyraw raw = i_keyto(_i_keyref(it.ref));
    _cx_MEMB(_next)(&it);
    if (it.ref) {
        _cx_keyraw nxt = i_keyto(_i_keyref(it.ref));
        _cx_MEMB(_erase)(self, raw);
        _cx_MEMB(_find_it)(self, nxt, &it);
    } else
        _cx_MEMB(_erase)(self, raw);
    return it;
}

STC_DEF _cx_iter
_cx_MEMB(_erase_range)(_cx_Self* self, _cx_iter it1, _cx_iter it2) {
    if (!it2.ref) { 
        while (it1.ref)
            it1 = _cx_MEMB(_erase_at)(self, it1);
        return it1;
    }
    _cx_key k1 = *_i_keyref(it1.ref), k2 = *_i_keyref(it2.ref);
    _cx_keyraw r1 = i_keyto((&k1));
    for (;;) {
        if (memcmp(&k1, &k2, sizeof k1) == 0)
            return it1;
        _cx_MEMB(_next)(&it1);
        k1 = *_i_keyref(it1.ref);
        _cx_MEMB(_erase)(self, r1);
        r1 = i_keyto((&k1));
        _cx_MEMB(_find_it)(self, r1, &it1);
    }
}

#if !defined i_no_clone
static int32_t
_cx_MEMB(_clone_r_)(_cx_Self* self, _cx_node* src, int32_t sn) {
    if (sn == 0)
        return 0;
    int32_t tx, tn = _cx_MEMB(_new_node_)(self, src[sn].level);
    self->nodes[tn].value = _cx_MEMB(_value_clone)(src[sn].value);
    tx = _cx_MEMB(_clone_r_)(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx;
    tx = _cx_MEMB(_clone_r_)(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx;
    return tn;
}

STC_DEF _cx_Self
_cx_MEMB(_clone)(_cx_Self tree) {
    _cx_Self clone = _cx_MEMB(_with_capacity)(tree.size);
    int32_t root = _cx_MEMB(_clone_r_)(&clone, tree.nodes, tree.root);
    clone.root = root;
    clone.size = tree.size;
    return clone;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _cx_result
_cx_MEMB(_emplace)(_cx_Self* self, _cx_keyraw rkey _i_MAP_ONLY(, i_valraw rmapped)) {
    _cx_result res = _cx_MEMB(_insert_entry_)(self, rkey);
    if (res.inserted) {
        *_i_keyref(res.ref) = i_keyfrom(rkey);
        _i_MAP_ONLY(res.ref->second = i_valfrom(rmapped);)
    }
    return res;
}
#endif // i_no_emplace

static void
_cx_MEMB(_drop_r_)(_cx_node* d, int32_t tn) {
    if (tn) {
        _cx_MEMB(_drop_r_)(d, d[tn].link[0]);
        _cx_MEMB(_drop_r_)(d, d[tn].link[1]);
        _cx_MEMB(_value_drop)(&d[tn].value);
    }
}

STC_DEF void
_cx_MEMB(_drop)(_cx_Self* self) {
    if (self->cap) {
        _cx_MEMB(_drop_r_)(self->nodes, self->root);
        i_free(self->nodes);
    }
}

#endif // i_implement
#undef _i_isset
#undef _i_ismap
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
#define CSMAP_H_INCLUDED
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

#undef i_cmp_native
#undef i_no_cmp
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
// ### END_FILE_INCLUDE: csmap.h
// ### END_FILE_INCLUDE: csset.h


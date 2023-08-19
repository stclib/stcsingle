// ### BEGIN_FILE_INCLUDE: cvec.h

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

#ifndef CVEC_H_INCLUDED
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
// use with gcc -Wsign-conversion
#define c_sizeof                (intptr_t)sizeof
#define c_strlen(s)             (intptr_t)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u(ilen))
#define c_u2i(u)                (intptr_t)(1 ? (u) : (size_t)1)
#define c_i2u(i)                (size_t)(1 ? (i) : -1)
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
#define c_sv_1(literal) c_sv_2(literal, c_litstrlen(literal))
#define c_sv_2(str, n) (c_LITERAL(csview){str, n})
#define c_SV(sv) (int)(sv).size, (sv).buf // printf("%.*s\n", c_SV(sv));

#define c_rs(literal) c_rs_2(literal, c_litstrlen(literal))
#define c_rs_2(str, n) (c_LITERAL(crawstr){str, n})

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

// csview : non-null terminated string view
typedef const char csview_value;
typedef struct csview { 
    csview_value* buf; 
    intptr_t size;
} csview;

typedef union { 
    csview_value* ref; 
    csview chr;
    struct { csview chr; csview_value* end; } u8;
} csview_iter;


// crawstr : null-terminated string view
typedef csview_value crawstr_value;
typedef struct crawstr { 
    crawstr_value* str; 
    intptr_t size;
} crawstr;

typedef union { 
    crawstr_value* ref; 
    csview chr;
    struct { csview chr; } u8; // [deprecated]
} crawstr_iter;


// cstr : null-terminated string (short string optimized - sso)
typedef char cstr_value;
typedef struct { cstr_value* data; intptr_t size, cap; } cstr_buf;
typedef union cstr {
    struct { cstr_value data[sizeof(cstr_buf) - 1]; unsigned char size; } sml;
    struct { cstr_value* data; size_t size, ncap; } lon;
} cstr;

typedef union { 
    cstr_value* ref; 
    csview chr;
    struct { csview chr; } u8; // [deprecated]
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
#include <string.h>

#define _it2_ptr(it1, it2) (it1.ref && !it2.ref ? it2.end : it2.ref)
#define _it_ptr(it) (it.ref ? it.ref : it.end)
#endif // CVEC_H_INCLUDED

#define _i_prefix cvec_
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

#ifndef i_is_forward
   _cx_DEFTYPES(_c_cvec_types, _cx_Self, i_key);
#endif
typedef i_keyraw _cx_raw;
STC_API _cx_Self        _cx_MEMB(_init)(void);
STC_API void            _cx_MEMB(_drop)(_cx_Self* self);
STC_API void            _cx_MEMB(_clear)(_cx_Self* self);
STC_API bool            _cx_MEMB(_reserve)(_cx_Self* self, intptr_t cap);
STC_API bool            _cx_MEMB(_resize)(_cx_Self* self, intptr_t size, i_key null);
STC_API _cx_iter        _cx_MEMB(_erase_n)(_cx_Self* self, intptr_t idx, intptr_t n);
STC_API _cx_iter        _cx_MEMB(_insert_uninit)(_cx_Self* self, intptr_t idx, intptr_t n);
#if defined _i_has_eq || defined _i_has_cmp
STC_API _cx_iter        _cx_MEMB(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw raw);
#endif
#if defined _i_has_cmp
STC_API int             _cx_MEMB(_value_cmp)(const _cx_value* x, const _cx_value* y);
STC_API _cx_iter        _cx_MEMB(_binary_search_in)(_cx_iter it1, _cx_iter it2, _cx_raw raw, _cx_iter* lower_bound);
#endif
STC_INLINE void         _cx_MEMB(_value_drop)(_cx_value* val) { i_keydrop(val); }

STC_INLINE _cx_value*   _cx_MEMB(_push)(_cx_Self* self, i_key value) {
                            if (self->_len == self->_cap)
                            if (!_cx_MEMB(_reserve)(self, self->_len*2 + 4))
                                return NULL;
                            _cx_value *v = self->data + self->_len++;
                            *v = value;
                            return v;
                        }

#if !defined i_no_emplace
STC_API _cx_iter
_cx_MEMB(_emplace_n)(_cx_Self* self, intptr_t idx, const _cx_raw raw[], intptr_t n);

STC_INLINE _cx_value* _cx_MEMB(_emplace)(_cx_Self* self, _cx_raw raw) {
    return _cx_MEMB(_push)(self, i_keyfrom(raw));
}
STC_INLINE _cx_value* _cx_MEMB(_emplace_back)(_cx_Self* self, _cx_raw raw) {
     return _cx_MEMB(_push)(self, i_keyfrom(raw));
}
STC_INLINE _cx_iter _cx_MEMB(_emplace_at)(_cx_Self* self, _cx_iter it, _cx_raw raw) {
    return _cx_MEMB(_emplace_n)(self, _it_ptr(it) - self->data, &raw, 1);
}
#endif // !i_no_emplace

#if !defined i_no_clone
STC_API _cx_Self        _cx_MEMB(_clone)(_cx_Self cx);
STC_API _cx_iter        _cx_MEMB(_copy_n)(_cx_Self* self, intptr_t idx, const _cx_value arr[], intptr_t n);
STC_INLINE void         _cx_MEMB(_put_n)(_cx_Self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_MEMB(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_Self     _cx_MEMB(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_Self cx = {0}; _cx_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE i_key        _cx_MEMB(_value_clone)(_cx_value val)
                            { return i_keyclone(val); }
STC_INLINE void         _cx_MEMB(_copy)(_cx_Self* self, const _cx_Self* other) {
                            if (self->data == other->data) return;
                            _cx_MEMB(_clear)(self);
                            _cx_MEMB(_copy_n)(self, 0, other->data, other->_len);
                        }
#endif // !i_no_clone

STC_INLINE intptr_t     _cx_MEMB(_size)(const _cx_Self* self) { return self->_len; }
STC_INLINE intptr_t     _cx_MEMB(_capacity)(const _cx_Self* self) { return self->_cap; }
STC_INLINE bool         _cx_MEMB(_empty)(const _cx_Self* self) { return !self->_len; }
STC_INLINE _cx_raw      _cx_MEMB(_value_toraw)(const _cx_value* val) { return i_keyto(val); }
STC_INLINE _cx_value*   _cx_MEMB(_front)(const _cx_Self* self) { return self->data; }
STC_INLINE _cx_value*   _cx_MEMB(_back)(const _cx_Self* self)
                            { return self->data + self->_len - 1; }
STC_INLINE void         _cx_MEMB(_pop)(_cx_Self* self)
                            { c_assert(self->_len); _cx_value* p = &self->data[--self->_len]; i_keydrop(p); }
STC_INLINE _cx_value    _cx_MEMB(_pull)(_cx_Self* self)
                            { c_assert(self->_len); return self->data[--self->_len]; }
STC_INLINE _cx_value*   _cx_MEMB(_push_back)(_cx_Self* self, i_key value)
                            { return _cx_MEMB(_push)(self, value); }
STC_INLINE void         _cx_MEMB(_pop_back)(_cx_Self* self) { _cx_MEMB(_pop)(self); }

STC_INLINE _cx_Self
_cx_MEMB(_with_size)(const intptr_t size, i_key null) {
    _cx_Self cx = _cx_MEMB(_init)();
    _cx_MEMB(_resize)(&cx, size, null);
    return cx;
}

STC_INLINE _cx_Self
_cx_MEMB(_with_capacity)(const intptr_t cap) {
    _cx_Self cx = _cx_MEMB(_init)();
    _cx_MEMB(_reserve)(&cx, cap);
    return cx;
}

STC_INLINE void
_cx_MEMB(_shrink_to_fit)(_cx_Self* self) {
    _cx_MEMB(_reserve)(self, _cx_MEMB(_size)(self));
}

STC_INLINE _cx_iter
_cx_MEMB(_insert_n)(_cx_Self* self, const intptr_t idx, const _cx_value arr[], const intptr_t n) {
    _cx_iter it = _cx_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        c_memcpy(it.ref, arr, n*c_sizeof *arr);
    return it;
}
STC_INLINE _cx_iter
_cx_MEMB(_insert_at)(_cx_Self* self, _cx_iter it, const _cx_value value) {
    return _cx_MEMB(_insert_n)(self, _it_ptr(it) - self->data, &value, 1);
}

STC_INLINE _cx_iter
_cx_MEMB(_erase_at)(_cx_Self* self, _cx_iter it) {
    return _cx_MEMB(_erase_n)(self, it.ref - self->data, 1);
}
STC_INLINE _cx_iter
_cx_MEMB(_erase_range)(_cx_Self* self, _cx_iter i1, _cx_iter i2) {
    return _cx_MEMB(_erase_n)(self, i1.ref - self->data, _it2_ptr(i1, i2) - i1.ref);
}

STC_INLINE const _cx_value*
_cx_MEMB(_at)(const _cx_Self* self, const intptr_t idx) {
    c_assert(idx < self->_len); return self->data + idx;
}
STC_INLINE _cx_value*
_cx_MEMB(_at_mut)(_cx_Self* self, const intptr_t idx) {
    c_assert(idx < self->_len); return self->data + idx;
}


STC_INLINE _cx_iter _cx_MEMB(_begin)(const _cx_Self* self) { 
    intptr_t n = self->_len; 
    return c_LITERAL(_cx_iter){n ? self->data : NULL, self->data + n};
}

STC_INLINE _cx_iter _cx_MEMB(_end)(const _cx_Self* self) 
    { return c_LITERAL(_cx_iter){NULL, self->data + self->_len}; }

STC_INLINE void _cx_MEMB(_next)(_cx_iter* it) 
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _cx_iter _cx_MEMB(_advance)(_cx_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL; return it; }

STC_INLINE intptr_t _cx_MEMB(_index)(const _cx_Self* self, _cx_iter it) 
    { return (it.ref - self->data); }

STC_INLINE void _cx_MEMB(_adjust_end_)(_cx_Self* self, intptr_t n)
    { self->_len += n; }

#if defined _i_has_eq || defined _i_has_cmp

STC_INLINE _cx_iter
_cx_MEMB(_find)(const _cx_Self* self, _cx_raw raw) {
    return _cx_MEMB(_find_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), raw);
}

STC_INLINE const _cx_value*
_cx_MEMB(_get)(const _cx_Self* self, _cx_raw raw) {
    return _cx_MEMB(_find)(self, raw).ref;
}

STC_INLINE _cx_value*
_cx_MEMB(_get_mut)(const _cx_Self* self, _cx_raw raw)
    { return (_cx_value*) _cx_MEMB(_get)(self, raw); }

STC_INLINE bool
_cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
    if (self->_len != other->_len) return false;
    for (intptr_t i = 0; i < self->_len; ++i) {
        const _cx_raw _rx = i_keyto(self->data+i), _ry = i_keyto(other->data+i);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif
#if defined _i_has_cmp

STC_INLINE _cx_iter
_cx_MEMB(_binary_search)(const _cx_Self* self, _cx_raw raw) {
    _cx_iter lower;
    return _cx_MEMB(_binary_search_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), raw, &lower);
}

STC_INLINE _cx_iter
_cx_MEMB(_lower_bound)(const _cx_Self* self, _cx_raw raw) {
    _cx_iter lower;
    _cx_MEMB(_binary_search_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), raw, &lower);
    return lower;
}

STC_INLINE void
_cx_MEMB(_sort_range)(_cx_iter i1, _cx_iter i2, int(*cmp)(const _cx_value*, const _cx_value*)) {
    qsort(i1.ref, (size_t)(_it2_ptr(i1, i2) - i1.ref), sizeof(_cx_value),
          (int(*)(const void*, const void*)) cmp);
}

STC_INLINE void
_cx_MEMB(_sort)(_cx_Self* self) {
    _cx_MEMB(_sort_range)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), _cx_MEMB(_value_cmp));
}
#endif // _i_has_cmp
/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF _cx_Self
_cx_MEMB(_init)(void) {
    return c_LITERAL(_cx_Self){NULL};
}

STC_DEF void
_cx_MEMB(_clear)(_cx_Self* self) {
    if (self->_cap) {
        for (_cx_value *p = self->data, *q = p + self->_len; p != q; ) {
            --q; i_keydrop(q);
        }
        self->_len = 0;
    }
}

STC_DEF void
_cx_MEMB(_drop)(_cx_Self* self) {
    if (self->_cap == 0)
        return;
    _cx_MEMB(_clear)(self);
    i_free(self->data);
}

STC_DEF bool
_cx_MEMB(_reserve)(_cx_Self* self, const intptr_t cap) {
    if (cap > self->_cap || (cap && cap == self->_len)) {
        _cx_value* d = (_cx_value*)i_realloc(self->data, cap*c_sizeof(i_key));
        if (!d)
            return false;
        self->data = d;
        self->_cap = cap;
    }
    return true;
}

STC_DEF bool
_cx_MEMB(_resize)(_cx_Self* self, const intptr_t len, i_key null) {
    if (!_cx_MEMB(_reserve)(self, len))
        return false;
    const intptr_t n = self->_len;
    for (intptr_t i = len; i < n; ++i)
        { i_keydrop((self->data + i)); }
    for (intptr_t i = n; i < len; ++i)
        self->data[i] = null;
    self->_len = len;
    return true;
}

STC_DEF _cx_iter
_cx_MEMB(_insert_uninit)(_cx_Self* self, const intptr_t idx, const intptr_t n) {
    if (self->_len + n > self->_cap)
        if (!_cx_MEMB(_reserve)(self, self->_len*5/4 + n))
            return _cx_MEMB(_end)(self);

    _cx_value* pos = self->data + idx;
    c_memmove(pos + n, pos, (self->_len - idx)*c_sizeof *pos);
    self->_len += n;
    return c_LITERAL(_cx_iter){pos, self->data + self->_len};
}

STC_DEF _cx_iter
_cx_MEMB(_erase_n)(_cx_Self* self, const intptr_t idx, const intptr_t len) {
    _cx_value* d = self->data + idx, *p = d, *end = self->data + self->_len;
    for (intptr_t i = 0; i < len; ++i, ++p)
        { i_keydrop(p); }
    c_memmove(d, p, (end - p)*c_sizeof *d);
    self->_len -= len;
    return c_LITERAL(_cx_iter){p == end ? NULL : d, end - len};
}

#if !defined i_no_clone
STC_DEF _cx_Self
_cx_MEMB(_clone)(_cx_Self cx) {
    _cx_Self out = _cx_MEMB(_init)();
    _cx_MEMB(_copy_n)(&out, 0, cx.data, cx._len);
    return out;
}

STC_DEF _cx_iter
_cx_MEMB(_copy_n)(_cx_Self* self, const intptr_t idx,
                  const _cx_value arr[], const intptr_t n) {
    _cx_iter it = _cx_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        for (_cx_value* p = it.ref, *q = p + n; p != q; ++arr)
            *p++ = i_keyclone((*arr));
    return it;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _cx_iter
_cx_MEMB(_emplace_n)(_cx_Self* self, const intptr_t idx, const _cx_raw raw[], intptr_t n) {
    _cx_iter it = _cx_MEMB(_insert_uninit)(self, idx, n);
    if (it.ref)
        for (_cx_value* p = it.ref; n--; ++raw, ++p)
            *p = i_keyfrom((*raw));
    return it;
}
#endif // !i_no_emplace
#if defined _i_has_eq || defined _i_has_cmp

STC_DEF _cx_iter
_cx_MEMB(_find_in)(_cx_iter i1, _cx_iter i2, _cx_raw raw) {
    const _cx_value* p2 = _it2_ptr(i1, i2);
    for (; i1.ref != p2; ++i1.ref) {
        const _cx_raw r = i_keyto(i1.ref);
        if (i_eq((&raw), (&r)))
            return i1;
    }
    i2.ref = NULL;
    return i2;
}
#endif
#if defined _i_has_cmp

STC_DEF _cx_iter
_cx_MEMB(_binary_search_in)(_cx_iter i1, _cx_iter i2, const _cx_raw raw,
                            _cx_iter* lower_bound) {
    _cx_value* w[2] = {i1.ref, _it2_ptr(i1, i2)};
    _cx_iter mid = i1;
    while (w[0] != w[1]) {
        mid.ref = w[0] + (w[1] - w[0])/2;
        const _cx_raw m = i_keyto(mid.ref);
        const int c = i_cmp((&raw), (&m));

        if (!c) return *lower_bound = mid;
        w[c < 0] = mid.ref + (c > 0);
    }
    i1.ref = w[0] == i2.end ? NULL : w[0];
    *lower_bound = i1;
    i1.ref = NULL; return i1;
}

STC_DEF int _cx_MEMB(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw rx = i_keyto(x);
    const _cx_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // _i_has_cmp
#endif // i_implement
#define CVEC_H_INCLUDED
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
// ### END_FILE_INCLUDE: cvec.h


// ### BEGIN_FILE_INCLUDE: cqueue.h
// STC queue

#ifndef _i_prefix
#define _i_prefix cqueue_
#endif
#define _i_queue
#define _pop_front _pop

// ### BEGIN_FILE_INCLUDE: cdeq.h
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

#ifndef CDEQ_H_INCLUDED
// ### BEGIN_FILE_INCLUDE: forward.h
#ifndef STC_FORWARD_H_INCLUDED
#define STC_FORWARD_H_INCLUDED

#include <stdint.h>

#define forward_carc(CX, VAL) _c_carc_types(CX, VAL)
#define forward_cbox(CX, VAL) _c_cbox_types(CX, VAL)
#define forward_cdeq(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_clist(CX, VAL) _c_clist_types(CX, VAL)
#define forward_cmap(CX, KEY, VAL) _c_chash_types(CX, KEY, VAL, int32_t, c_true, c_false)
#define forward_cmap64(CX, KEY, VAL) _c_chash_types(CX, KEY, VAL, int64_t, c_true, c_false)
#define forward_cset(CX, KEY) _c_chash_types(CX, cset, KEY, KEY, int32_t, c_false, c_true)
#define forward_cset64(CX, KEY) _c_chash_types(CX, cset, KEY, KEY, int64_t, c_false, c_true)
#define forward_csmap(CX, KEY, VAL) _c_aatree_types(CX, KEY, VAL, int32_t, c_true, c_false)
#define forward_csset(CX, KEY) _c_aatree_types(CX, KEY, KEY, int32_t, c_false, c_true)
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
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value *_base, *data; \
        intptr_t _len, _cap; \
    } SELF

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

#define _c_chash_types(SELF, KEY, VAL, SZ, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef SZ SELF##_ssize; \
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
        uint8_t* _hx; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value* table; \
        uint8_t* _hashx; \
        SELF##_ssize size, bucket_count; \
    } SELF

#define _c_aatree_types(SELF, KEY, VAL, SZ, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef SZ SELF##_ssize; \
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
        SELF##_ssize _tn, _st[36]; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_node *nodes; \
        SELF##_ssize root, disp, head, size, cap; \
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
#endif // CDEQ_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix cdeq_
#endif
// ### BEGIN_FILE_INCLUDE: template.h
#ifdef _i_template
  #error template.h already included
#endif
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define _cx_self i_type
  #define _cx_memb(name) c_PASTE(_cx_self, name)
  #define _cx_deftypes(macro, SELF, ...) c_EXPAND(macro(SELF, __VA_ARGS__))
  #define _cx_value _cx_memb(_value)
  #define _cx_key _cx_memb(_key)
  #define _cx_mapped _cx_memb(_mapped)
  #define _cx_raw _cx_memb(_raw)
  #define _cx_rawkey _cx_memb(_rawkey)
  #define _cx_iter _cx_memb(_iter)
  #define _cx_result _cx_memb(_result)
  #define _cx_node _cx_memb(_node)
#endif

#ifndef i_type
  #define i_type c_PASTE(_i_prefix, i_tag)
#endif

#ifndef i_ssize
  #define i_ssize intptr_t
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
#ifdef i_eq
  #define _i_has_eq
#endif

#if defined i_key_str
  #define i_keyclass cstr
  #define i_rawclass crawstr
  #define i_keyfrom cstr_from
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
  #if !defined i_cmp && !defined i_no_cmp
    #define i_cmp c_PASTE(i_keyraw, _cmp)
  #endif
  #if !defined i_hash && !defined i_no_hash
    #define i_hash c_PASTE(i_keyraw, _hash)
  #endif
#endif

#if !defined i_key
  #error "No i_key or i_val defined"
#elif defined i_keyraw ^ defined i_keyto
  #error "Both i_keyraw/valraw and i_keyto/valto must be defined, if any"
#elif defined i_keyfrom && !defined i_keyraw
  #error "i_keyfrom/valfrom defined without i_keyraw/valraw"
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Define i_keyfrom/i_valfrom and/or i_keydrop/i_valdrop instead"
#endif

#ifndef i_tag
  #define i_tag i_key
#endif
#if c_option(c_no_clone)
  #define i_no_clone
#elif !(defined i_keyclone || defined i_no_clone) && (defined i_keydrop || defined i_keyraw)
  #error i_keyclone/valclone should be defined when i_keydrop/valdrop or i_keyraw/valraw is defined
#endif
#ifndef i_keyraw
  #define i_keyraw i_key
#endif
#ifndef i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #define _i_has_from
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
  #define i_eq c_default_eq
#endif
#if defined i_less && defined i_cmp
  #error "Only one of i_less and i_cmp may be defined"
#elif !defined i_less && !defined i_cmp
  #define i_less c_default_less
#elif !defined i_less
  #define i_less(x, y) (i_cmp(x, y)) < 0
#endif
#ifndef i_cmp
  #define i_cmp(x, y) (i_less(y, x)) - (i_less(x, y))
#endif

#ifndef i_hash
  #define i_hash c_default_hash
#endif

#if defined _i_ismap // ---- process cmap/csmap value i_val, ... ----

#ifdef i_val_str
  #define i_valclass cstr
  #define i_valraw crawstr
  #define i_valfrom cstr_from
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

#ifndef i_val
  #error "i_val* must be defined for maps"
#endif

#if !(defined i_valclone || defined i_no_clone) && (defined i_valdrop || defined i_valraw)
  #error i_valclone should be defined when i_valdrop or i_valraw is defined
#endif
#ifndef i_valraw
  #define i_valraw i_val
#endif
#ifndef i_valfrom
  #define i_valfrom c_default_clone
#else
  #define _i_has_from
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
#ifndef _i_has_from
  #define i_no_emplace
#endif
// ### END_FILE_INCLUDE: template.h

#ifndef i_is_forward
_cx_deftypes(_c_cdeq_types, _cx_self, i_key);
#endif
typedef i_keyraw _cx_raw;

STC_API _cx_self        _cx_memb(_init)(void);
STC_API _cx_self        _cx_memb(_with_capacity)(const intptr_t n);
STC_API bool            _cx_memb(_reserve)(_cx_self* self, const intptr_t n);
STC_API void            _cx_memb(_clear)(_cx_self* self);
STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API _cx_value*      _cx_memb(_push)(_cx_self* self, i_key value);
STC_API void            _cx_memb(_shrink_to_fit)(_cx_self *self);
STC_INLINE void         _cx_memb(_put_n)(_cx_self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_memb(_push)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_self     _cx_memb(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_self cx = {0}; _cx_memb(_put_n)(&cx, raw, n); return cx; }
STC_INLINE void         _cx_memb(_value_drop)(_cx_value* val) { i_keydrop(val); }
#if !defined _i_queue
#if !defined i_no_emplace
STC_API _cx_iter        _cx_memb(_emplace_range)(_cx_self* self, _cx_value* pos,
                                                 const _cx_raw* p1, const _cx_raw* p2);
#endif // i_no_emplace

#if !defined i_no_cmp || defined _i_has_eq
STC_API _cx_iter        _cx_memb(_find_in)(_cx_iter p1, _cx_iter p2, _cx_raw raw);
#endif
#ifndef i_no_cmp
STC_API int             _cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y);
#endif
STC_API _cx_value*      _cx_memb(_push_front)(_cx_self* self, i_key value);
STC_API _cx_iter        _cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2);
STC_API _cx_iter        _cx_memb(_insert_range)(_cx_self* self, _cx_value* pos,
                                                const _cx_value* p1, const _cx_value* p2);
#endif // !_i_queue

#if !defined i_no_emplace
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push)(self, i_keyfrom(raw)); }
#endif

#if !defined i_no_clone
#if !defined _i_queue
STC_API _cx_iter        _cx_memb(_copy_range)(_cx_self* self, _cx_value* pos,
                                                const _cx_value* p1, const _cx_value* p2);

STC_INLINE void         _cx_memb(_copy)(_cx_self *self, const _cx_self* other) {
                            if (self->data == other->data) return;
                            _cx_memb(_clear)(self);
                            _cx_memb(_copy_range)(self, self->data,
                                                  other->data, other->data + other->_len);
                        }
#endif // !_i_queue
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_INLINE i_key        _cx_memb(_value_clone)(i_key val)
                            { return i_keyclone(val); }
#endif // !i_no_clone
STC_INLINE intptr_t     _cx_memb(_size)(const _cx_self* self) { return self->_len; }
STC_INLINE intptr_t     _cx_memb(_capacity)(const _cx_self* self) { return self->_cap; }
STC_INLINE bool         _cx_memb(_empty)(const _cx_self* self) { return !self->_len; }
STC_INLINE _cx_raw      _cx_memb(_value_toraw)(const _cx_value* pval) { return i_keyto(pval); }
STC_INLINE _cx_value*   _cx_memb(_front)(const _cx_self* self) { return self->data; }
STC_INLINE _cx_value*   _cx_memb(_back)(const _cx_self* self)
                            { return self->data + self->_len - 1; }
STC_INLINE void         _cx_memb(_pop_front)(_cx_self* self) // == _pop() when _i_queue
                            { i_keydrop(self->data); ++self->data; --self->_len; }

STC_INLINE _cx_iter _cx_memb(_begin)(const _cx_self* self) {
    intptr_t n = self->_len;
    return c_LITERAL(_cx_iter){n ? self->data : NULL, self->data + n};
}

STC_INLINE _cx_iter _cx_memb(_end)(const _cx_self* self)
    { return c_LITERAL(_cx_iter){NULL, self->data + self->_len}; }

STC_INLINE void _cx_memb(_next)(_cx_iter* it)
    { if (++it->ref == it->end) it->ref = NULL; }

STC_INLINE _cx_iter _cx_memb(_advance)(_cx_iter it, size_t n)
    { if ((it.ref += n) >= it.end) it.ref = NULL; return it; }

#if !defined _i_queue

STC_INLINE intptr_t _cx_memb(_index)(const _cx_self* self, _cx_iter it)
    { return (it.ref - self->data); }
STC_INLINE void _cx_memb(_pop_back)(_cx_self* self)
    { _cx_value* p = &self->data[--self->_len]; i_keydrop(p); }

STC_INLINE const _cx_value* _cx_memb(_at)(const _cx_self* self, const intptr_t idx) {
    assert(idx < self->_len); return self->data + idx;
}
STC_INLINE _cx_value* _cx_memb(_at_mut)(_cx_self* self, const intptr_t idx) {
    assert(idx < self->_len); return self->data + idx;
}

STC_INLINE _cx_value* _cx_memb(_push_back)(_cx_self* self, i_key value) {
    return _cx_memb(_push)(self, value);
}
STC_INLINE _cx_iter
_cx_memb(_insert)(_cx_self* self, const intptr_t idx, i_key value) {
    return _cx_memb(_insert_range)(self, self->data + idx, &value, &value + 1);
}
STC_INLINE _cx_iter
_cx_memb(_insert_n)(_cx_self* self, const intptr_t idx, const _cx_value arr[], const intptr_t n) {
    return _cx_memb(_insert_range)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value) {
    return _cx_memb(_insert_range)(self, _it_ptr(it), &value, &value + 1);
}

STC_INLINE _cx_iter
_cx_memb(_erase_n)(_cx_self* self, const intptr_t idx, const intptr_t n) {
    return _cx_memb(_erase_range_p)(self, self->data + idx, self->data + idx + n);
}
STC_INLINE _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    return _cx_memb(_erase_range_p)(self, it.ref, it.ref + 1);
}
STC_INLINE _cx_iter
_cx_memb(_erase_range)(_cx_self* self, _cx_iter i1, _cx_iter i2) {
    return _cx_memb(_erase_range_p)(self, i1.ref, _it2_ptr(i1, i2));
}

#if !defined i_no_emplace
STC_INLINE _cx_value*
_cx_memb(_emplace_front)(_cx_self* self, _cx_raw raw) {
    return _cx_memb(_push_front)(self, i_keyfrom(raw));
}

STC_INLINE _cx_value* _cx_memb(_emplace_back)(_cx_self* self, _cx_raw raw) {
    return _cx_memb(_push)(self, i_keyfrom(raw));
}

STC_INLINE _cx_iter
_cx_memb(_emplace_n)(_cx_self* self, const intptr_t idx, const _cx_raw arr[], const intptr_t n) {
    return _cx_memb(_emplace_range)(self, self->data + idx, arr, arr + n);
}
STC_INLINE _cx_iter
_cx_memb(_emplace_at)(_cx_self* self, _cx_iter it, _cx_raw raw) {
    return _cx_memb(_emplace_range)(self, _it_ptr(it), &raw, &raw + 1);
}
#endif // !i_no_emplace

#if !defined i_no_cmp || defined _i_has_eq

STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, _cx_raw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, _cx_raw raw) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), raw).ref;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(_cx_self* self, _cx_raw raw)
    { return (_cx_value *) _cx_memb(_get)(self, raw); }

STC_INLINE bool
_cx_memb(_eq)(const _cx_self* self, const _cx_self* other) {
    if (self->_len != other->_len) return false;
    for (intptr_t i = 0; i < self->_len; ++i) {
        const _cx_raw _rx = i_keyto(self->data+i), _ry = i_keyto(other->data+i);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif
#ifndef i_no_cmp

STC_INLINE void
_cx_memb(_sort_range)(_cx_iter i1, _cx_iter i2, int(*cmp)(const _cx_value*, const _cx_value*)) {
    qsort(i1.ref, (size_t)(_it2_ptr(i1, i2) - i1.ref), sizeof *i1.ref,
          (int(*)(const void*, const void*)) cmp);
}

STC_INLINE void
_cx_memb(_sort)(_cx_self* self) {
    _cx_memb(_sort_range)(_cx_memb(_begin)(self), _cx_memb(_end)(self), _cx_memb(_value_cmp));
}
#endif // !c_no_cmp
#endif // _i_queue

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)
#define _cdeq_nfront(self) ((self)->data - (self)->_base)

STC_DEF _cx_self
_cx_memb(_init)(void) {
    _cx_self cx = {NULL};
    return cx;
}

STC_DEF void
_cx_memb(_clear)(_cx_self* self) {
    if (self->_cap) {
        for (_cx_value *p = self->data, *q = p + self->_len; p != q; )
            { --q; i_keydrop(q); }
        self->_len = 0;
        self->data = self->_base;
    }
}

STC_DEF void
_cx_memb(_shrink_to_fit)(_cx_self *self) {
    if (self->_len != self->_cap) {
        c_memmove(self->_base, self->data, self->_len*c_sizeof(i_key));
        _cx_value* d = (_cx_value*)i_realloc(self->_base, self->_len*c_sizeof(i_key));
        if (d) {
            self->_base = d;
            self->_cap = self->_len;
        }
	self->data = self->_base;
    }
}

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    if (self->_base) {
        _cx_memb(_clear)(self);
        i_free(self->_base);
    }
}

static intptr_t
_cx_memb(_realloc_)(_cx_self* self, const intptr_t n) {
    const intptr_t cap = (intptr_t)((float)self->_len*1.7f) + n + 7;
    const intptr_t nfront = _cdeq_nfront(self);
    _cx_value* d = (_cx_value*)i_realloc(self->_base, cap*c_sizeof(i_key));
    if (!d)
        return 0; 
    self->_cap = cap;
    self->_base = d;
    self->data = d + nfront;
    return cap;
}

static bool
_cx_memb(_expand_right_half_)(_cx_self* self, const intptr_t idx, const intptr_t n) {
    const intptr_t sz = self->_len, cap = self->_cap;
    const intptr_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nback >= n || (intptr_t)((float)sz*1.3f) + n > cap) {
        if (!_cx_memb(_realloc_)(self, n))
            return false;
        c_memmove(self->data + idx + n, self->data + idx, (sz - idx)*c_sizeof(i_key));
    } else {
#if !defined _i_queue
        const intptr_t unused = cap - (sz + n);
        const intptr_t pos = (nfront*2 < unused) ? nfront : unused/2;
#else
        const intptr_t pos = 0;
#endif
        c_memmove(self->_base + pos, self->data, idx*c_sizeof(i_key));
        c_memmove(self->data + pos + idx + n, self->data + idx, (sz - idx)*c_sizeof(i_key));
        self->data = self->_base + pos;
    }
    return true;
}

STC_DEF _cx_self
_cx_memb(_with_capacity)(const intptr_t n) {
    _cx_self cx = _cx_memb(_init)();
    _cx_memb(_expand_right_half_)(&cx, 0, n);
    return cx;
}

STC_DEF bool
_cx_memb(_reserve)(_cx_self* self, const intptr_t n) {
    const intptr_t sz = self->_len;
    return n <= sz || _cx_memb(_expand_right_half_)(self, sz, n - sz);
}

STC_DEF _cx_value*
_cx_memb(_push)(_cx_self* self, i_key value) {
    if (_cdeq_nfront(self) + self->_len == self->_cap)
        _cx_memb(_expand_right_half_)(self, self->_len, 1);
    _cx_value *v = self->data + self->_len++;
    *v = value;
    return v;
}

#if !defined i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    _cx_self out = _cx_memb(_with_capacity)(cx._len);
    if (out._base)
        for (intptr_t i = 0; i < cx._len; ++i) 
            out.data[i] = i_keyclone(cx.data[i]);
    return out;
}
#endif

#if !defined _i_queue

static void
_cx_memb(_expand_left_half_)(_cx_self* self, const intptr_t idx, const intptr_t n) {
    intptr_t cap = self->_cap;
    const intptr_t sz = self->_len;
    const intptr_t nfront = _cdeq_nfront(self), nback = cap - sz - nfront;
    if (nfront >= n) {
        self->data = (_cx_value *)c_memmove(self->data - n, self->data, idx*c_sizeof(i_key));
    } else {
        if ((intptr_t)((float)sz*1.3f) + n > cap)
            cap = _cx_memb(_realloc_)(self, n);
        const intptr_t unused = cap - (sz + n);
        const intptr_t pos = (nback*2 < unused) ? unused - nback : unused/2;
        c_memmove(self->_base + pos + idx + n, self->data + idx, (sz - idx)*c_sizeof(i_key));
        self->data = (_cx_value *)c_memmove(self->_base + pos, self->data, idx*c_sizeof(i_key));
    }
}

static _cx_iter
_cx_memb(_insert_uninit)(_cx_self* self, _cx_value* pos, const intptr_t n) {
    if (n) {
        if (!pos) pos = self->data + self->_len;
        const intptr_t idx = (pos - self->data);
        if (idx*2 < self->_len)
            _cx_memb(_expand_left_half_)(self, idx, n);
        else
            _cx_memb(_expand_right_half_)(self, idx, n);
        self->_len += n;
        pos = self->data + idx;
    }
    return c_LITERAL(_cx_iter){pos, self->data + self->_len};
}

STC_DEF _cx_value*
_cx_memb(_push_front)(_cx_self* self, i_key value) {
    if (self->data == self->_base)
        _cx_memb(_expand_left_half_)(self, 0, 1);
    else
        --self->data;
    ++self->_len;
    *self->data = value;
    return self->data;
}

STC_DEF _cx_iter
_cx_memb(_insert_range)(_cx_self* self, _cx_value* pos,
                          const _cx_value* p1, const _cx_value* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, (p2 - p1));
    if (it.ref)
        c_memcpy(it.ref, p1, (p2 - p1)*c_sizeof *p1);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range_p)(_cx_self* self, _cx_value* p1, _cx_value* p2) {
    assert(p1 && p2);
    intptr_t len = p2 - p1;
    _cx_value* p = p1, *end = self->data + self->_len;
    for (; p != p2; ++p)
        { i_keydrop(p); }
    c_memmove(p1, p2, (end - p2)*c_sizeof *p1);
    self->_len -= len;
    return c_LITERAL(_cx_iter){p2 == end ? NULL : p1, end - len};
}

#if !defined i_no_clone
STC_DEF _cx_iter
_cx_memb(_copy_range)(_cx_self* self, _cx_value* pos,
                      const _cx_value* p1, const _cx_value* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, (p2 - p1));
    if (it.ref)
        for (_cx_value* p = it.ref; p1 != p2; ++p1)
            *p++ = i_keyclone((*p1));
    return it;
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_DEF _cx_iter
_cx_memb(_emplace_range)(_cx_self* self, _cx_value* pos,
                         const _cx_raw* p1, const _cx_raw* p2) {
    _cx_iter it = _cx_memb(_insert_uninit)(self, pos, (p2 - p1));
    if (it.ref)
        for (_cx_value* p = it.ref; p1 != p2; ++p1)
            *p++ = i_keyfrom((*p1));
    return it;
}
#endif // !i_no_emplace

#if !defined i_no_cmp || defined _i_has_eq
STC_DEF _cx_iter
_cx_memb(_find_in)(_cx_iter i1, _cx_iter i2, _cx_raw raw) {
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
#ifndef i_no_cmp
STC_DEF int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw rx = i_keyto(x);
    const _cx_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // !c_no_cmp
#endif // !_i_queue
#endif // IMPLEMENTATION
#define CDEQ_H_INCLUDED
// ### BEGIN_FILE_INCLUDE: template2.h
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
#undef i_ssize

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
#undef i_extern

#undef i_allocator
#undef i_malloc
#undef i_calloc
#undef i_realloc
#undef i_free

#undef i_no_cmp
#undef i_no_hash
#undef i_no_clone
#undef i_no_emplace
#undef i_is_forward

#undef _i_prefix
#undef _i_expandby
#undef _i_has_from
#undef _i_has_eq
#undef _i_template
// ### END_FILE_INCLUDE: template2.h
// ### END_FILE_INCLUDE: cdeq.h

#undef _pop_front
#undef _i_queue
// ### END_FILE_INCLUDE: cqueue.h


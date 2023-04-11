// ### BEGIN_FILE_INCLUDE: clist.h

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

#ifndef CLIST_H_INCLUDED
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

#define _c_clist_complete_types(SELF, dummy) \
    struct SELF##_node { \
        struct SELF##_node *next; \
        SELF##_value value; \
    }

#define _clist_tonode(vp) c_container_of(vp, _cx_node, value)
    
#define _c_clist_insert_entry_after(ref, val) \
    _cx_node *entry = (_cx_node *)i_malloc(c_sizeof *entry); entry->value = val; \
    _c_clist_insert_after_node(ref, entry)

#define _c_clist_insert_after_node(ref, entry) \
    if (ref) entry->next = ref->next, ref->next = entry; \
    else     entry->next = entry
    // +: set self->last based on node 

#endif // CLIST_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix clist_
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
  #define _cx_keyraw _cx_memb(_keyraw)
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
  _cx_deftypes(_c_clist_types, _cx_self, i_key);
#endif
_cx_deftypes(_c_clist_complete_types, _cx_self, dummy);
typedef i_keyraw _cx_raw;

STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API _cx_value*      _cx_memb(_push_back)(_cx_self* self, i_key value);
STC_API _cx_value*      _cx_memb(_push_front)(_cx_self* self, i_key value);
STC_API _cx_iter        _cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value);
STC_API _cx_iter        _cx_memb(_erase_at)(_cx_self* self, _cx_iter it);
STC_API _cx_iter        _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2);
#if !defined i_no_cmp || defined _i_has_eq
STC_API _cx_iter        _cx_memb(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw val);
STC_API intptr_t        _cx_memb(_remove)(_cx_self* self, _cx_raw val);
#endif
#ifndef i_no_cmp
STC_API bool            _cx_memb(_sort_with)(_cx_self* self, int(*cmp)(const _cx_value*, const _cx_value*));
STC_API int             _cx_memb(_sort_cmp_)(const _cx_value*, const _cx_value*);
STC_INLINE bool         _cx_memb(_sort)(_cx_self* self)
                            { return _cx_memb(_sort_with)(self, _cx_memb(_sort_cmp_)); }
#endif
STC_API void            _cx_memb(_reverse)(_cx_self* self);
STC_API _cx_iter        _cx_memb(_splice)(_cx_self* self, _cx_iter it, _cx_self* other);
STC_API _cx_self        _cx_memb(_split_off)(_cx_self* self, _cx_iter it1, _cx_iter it2);
STC_API _cx_value*      _cx_memb(_push_back_node)(_cx_self* self, _cx_node* node);
STC_API _cx_value*      _cx_memb(_insert_after_node)(_cx_self* self, _cx_node* ref, _cx_node* node);
STC_API _cx_node*       _cx_memb(_unlink_after_node)(_cx_self* self, _cx_node* ref);
STC_API void            _cx_memb(_erase_after_node)(_cx_self* self, _cx_node* ref);
STC_INLINE _cx_node*    _cx_memb(_get_node)(_cx_value* vp) { return _clist_tonode(vp); }

#if !defined i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_INLINE i_key        _cx_memb(_value_clone)(i_key val) { return i_keyclone(val); }
STC_INLINE void
_cx_memb(_copy)(_cx_self *self, const _cx_self* other) {
    if (self->last == other->last) return;
    _cx_memb(_drop)(self); *self = _cx_memb(_clone)(*other);
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE _cx_value*   _cx_memb(_emplace_back)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push_back)(self, i_keyfrom(raw)); }
STC_INLINE _cx_value*   _cx_memb(_emplace_front)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push_front)(self, i_keyfrom(raw)); }
STC_INLINE _cx_iter     _cx_memb(_emplace_at)(_cx_self* self, _cx_iter it, _cx_raw raw)
                            { return _cx_memb(_insert_at)(self, it, i_keyfrom(raw)); }
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push_back)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

STC_INLINE _cx_self     _cx_memb(_init)(void) { return c_LITERAL(_cx_self){NULL}; }
STC_INLINE void         _cx_memb(_put_n)(_cx_self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_memb(_push_back)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_self     _cx_memb(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_self cx = {0}; _cx_memb(_put_n)(&cx, raw, n); return cx; }
STC_INLINE bool         _cx_memb(_reserve)(_cx_self* self, intptr_t n) { (void)(self + n); return true; }
STC_INLINE bool         _cx_memb(_empty)(const _cx_self* self) { return self->last == NULL; }
STC_INLINE void         _cx_memb(_clear)(_cx_self* self) { _cx_memb(_drop)(self); }
STC_INLINE _cx_value*   _cx_memb(_push)(_cx_self* self, i_key value)
                            { return _cx_memb(_push_back)(self, value); }
STC_INLINE void         _cx_memb(_pop_front)(_cx_self* self)
                            { assert(!_cx_memb(_empty)(self)); _cx_memb(_erase_after_node)(self, self->last); }
STC_INLINE _cx_node*    _cx_memb(_unlink_node_front)(_cx_self* self)
                            { return _cx_memb(_unlink_after_node)(self, self->last); }
STC_INLINE _cx_value*   _cx_memb(_front)(const _cx_self* self) { return &self->last->next->value; }
STC_INLINE _cx_value*   _cx_memb(_back)(const _cx_self* self) { return &self->last->value; }

STC_INLINE intptr_t
_cx_memb(_count)(const _cx_self* self) {
    intptr_t n = 1; const _cx_node *node = self->last;
    if (!node) return 0;
    while ((node = node->next) != self->last) ++n;
    return n;
}

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    _cx_value* head = self->last ? &self->last->next->value : NULL;
    return c_LITERAL(_cx_iter){head, &self->last, self->last};
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self)
    { return c_LITERAL(_cx_iter){NULL}; }

STC_INLINE void
_cx_memb(_next)(_cx_iter* it) {
    _cx_node* node = it->prev = _clist_tonode(it->ref);
    it->ref = (node == *it->_last ? NULL : &node->next->value);
}

STC_INLINE _cx_iter
_cx_memb(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref) _cx_memb(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_splice_range)(_cx_self* self, _cx_iter it,
                        _cx_self* other, _cx_iter it1, _cx_iter it2) {
    _cx_self tmp = _cx_memb(_split_off)(other, it1, it2);
    return _cx_memb(_splice)(self, it, &tmp);
}

#if !defined i_no_cmp || defined _i_has_eq
STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, _cx_raw val) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), val);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, _cx_raw val) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), val).ref;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(_cx_self* self, _cx_raw val) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), val).ref;
}

STC_INLINE bool _cx_memb(_eq)(const _cx_self* self, const _cx_self* other) {
    _cx_iter i = _cx_memb(_begin)(self), j = _cx_memb(_begin)(other);
    for (; i.ref && j.ref; _cx_memb(_next)(&i), _cx_memb(_next)(&j)) {
        const _cx_raw _rx = i_keyto(i.ref), _ry = i_keyto(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return !(i.ref || j.ref);
}
#endif

// -------------------------- IMPLEMENTATION -------------------------
#if defined(i_implement)

#if !defined i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    _cx_self out = _cx_memb(_init)();
    c_foreach (it, _cx_self, cx)
        _cx_memb(_push_back)(&out, i_keyclone((*it.ref)));
    return out;
}
#endif

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    while (self->last) _cx_memb(_erase_after_node)(self, self->last);
}

STC_DEF _cx_value*
_cx_memb(_push_back)(_cx_self* self, i_key value) {
    _c_clist_insert_entry_after(self->last, value);
    self->last = entry;
    return &entry->value;
}

STC_DEF _cx_value*
_cx_memb(_push_front)(_cx_self* self, i_key value) {
    _c_clist_insert_entry_after(self->last, value);
    if (!self->last)
        self->last = entry;
    return &entry->value;
}

STC_DEF _cx_value*
_cx_memb(_push_back_node)(_cx_self* self, _cx_node* node) {
    _c_clist_insert_after_node(self->last, node);
    self->last = node;
    return &node->value;
}

STC_DEF _cx_value*
_cx_memb(_insert_after_node)(_cx_self* self, _cx_node* ref, _cx_node* node) {
    _c_clist_insert_after_node(ref, node);
    if (!self->last)
        self->last = node;
    return &node->value;
}

STC_DEF _cx_iter
_cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value) {
    _cx_node* node = it.ref ? it.prev : self->last;
    _c_clist_insert_entry_after(node, value);
    if (!self->last || !it.ref) {
        it.prev = self->last ? self->last : entry;
        self->last = entry;
    }
    it.ref = &entry->value;
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    _cx_node *node = _clist_tonode(it.ref);
    it.ref = (node == self->last) ? NULL : &node->next->value;
    _cx_memb(_erase_after_node)(self, it.prev);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2) {
    _cx_node *end = it2.ref ? _clist_tonode(it2.ref) : self->last->next;
    if (it1.ref != it2.ref) do {
        _cx_memb(_erase_after_node)(self, it1.prev);
        if (!self->last) break;
    } while (it1.prev->next != end);
    return it2;
}

STC_DEF void
_cx_memb(_erase_after_node)(_cx_self* self, _cx_node* ref) {
    _cx_node* node = _cx_memb(_unlink_after_node)(self, ref);
    i_keydrop((&node->value));
    i_free(node);
}

STC_DEF _cx_node*
_cx_memb(_unlink_after_node)(_cx_self* self, _cx_node* ref) {
    _cx_node* node = ref->next, *next = node->next;
    ref->next = next;
    if (node == next)
        self->last = NULL;
    else if (node == self->last)
        self->last = ref;
    return node;
}

STC_DEF void
_cx_memb(_reverse)(_cx_self* self) {
    _cx_self rev = {NULL};
    while (self->last) {
        _cx_node* node = _cx_memb(_unlink_after_node)(self, self->last);
        _cx_memb(_insert_after_node)(&rev, rev.last, node);
    }
    *self = rev;
}

STC_DEF _cx_iter
_cx_memb(_splice)(_cx_self* self, _cx_iter it, _cx_self* other) {
    if (!self->last)
        self->last = other->last;
    else if (other->last) {
        _cx_node *p = it.ref ? it.prev : self->last, *next = p->next;
        it.prev = other->last;
        p->next = it.prev->next;
        it.prev->next = next;
        if (!it.ref) self->last = it.prev;
    }
    other->last = NULL;
    return it;
}

STC_DEF _cx_self
_cx_memb(_split_off)(_cx_self* self, _cx_iter it1, _cx_iter it2) {
    _cx_self lst = {NULL};
    if (it1.ref == it2.ref)
        return lst;
    _cx_node *p1 = it1.prev,
             *p2 = it2.ref ? it2.prev : self->last;
    p1->next = p2->next;
    p2->next = _clist_tonode(it1.ref);
    if (self->last == p2)
        self->last = (p1 == p2) ? NULL : p1;
    lst.last = p2;
    return lst;
}

#if !defined i_no_cmp || defined _i_has_eq

STC_DEF _cx_iter
_cx_memb(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw val) {
    c_foreach (it, _cx_self, it1, it2) {
        _cx_raw r = i_keyto(it.ref);
        if (i_eq((&r), (&val)))
            return it;
    }
    it2.ref = NULL; return it2;
}

STC_DEF intptr_t
_cx_memb(_remove)(_cx_self* self, _cx_raw val) {
    intptr_t n = 0;
    _cx_node *prev = self->last, *node;
    if (prev) do {
        node = prev->next;
        _cx_raw r = i_keyto((&node->value));
        if (i_eq((&r), (&val))) {
            _cx_memb(_erase_after_node)(self, prev), ++n;
            if (!self->last) break;
        } else
            prev = node;
    } while (node != self->last);
    return n;
}
#endif

#ifndef i_no_cmp
STC_DEF int _cx_memb(_sort_cmp_)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw a = i_keyto(x), b = i_keyto(y);
    return i_cmp((&a), (&b));
}

STC_DEF bool _cx_memb(_sort_with)(_cx_self* self, int(*cmp)(const _cx_value*, const _cx_value*)) {
    size_t len = 0, cap = 0;
    _cx_value *a = NULL, *p = NULL;
    _cx_iter i;
    for (i = _cx_memb(_begin)(self); i.ref; _cx_memb(_next)(&i)) {
        if (len == cap) {
            if ((p = (_cx_value *)i_realloc(a, (cap += cap/2 + 4)*sizeof *a))) a = p; 
            else { i_free(a); return false; }
        }
        a[len++] = *i.ref;
    }
    qsort(a, len, sizeof *a, (int(*)(const void*, const void*))cmp);
    for (i = _cx_memb(_begin)(self); i.ref; _cx_memb(_next)(&i), ++p) 
        *i.ref = *p;
    i_free(a); return true;
}
#endif // !c_no_cmp
#endif // i_implement
#define CLIST_H_INCLUDED
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
// ### END_FILE_INCLUDE: clist.h


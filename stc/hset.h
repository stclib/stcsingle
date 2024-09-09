// ### BEGIN_FILE_INCLUDE: hset.h

// Unordered set - implemented with the robin-hood hashing scheme.

#define _i_prefix hset_
#define _i_is_set
// ### BEGIN_FILE_INCLUDE: hmap.h

// Unordered set/map - implemented with the robin-hood hashing scheme.
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

#ifdef i_aux
  #define _i_aux_struct struct { i_aux } aux;
#else
  #define _i_aux_struct
#endif

#if defined __clang__ && !defined __cplusplus
  #pragma clang diagnostic push
  #pragma clang diagnostic warning "-Wall"
  #pragma clang diagnostic warning "-Wextra"
  #pragma clang diagnostic warning "-Wpedantic"
  #pragma clang diagnostic warning "-Wconversion"
  #pragma clang diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma clang diagnostic ignored "-Wmissing-field-initializers"
#elif defined __GNUC__ && !defined __cplusplus
  #pragma GCC diagnostic push
  #pragma GCC diagnostic warning "-Wall"
  #pragma GCC diagnostic warning "-Wextra"
  #pragma GCC diagnostic warning "-Wpedantic"
  #pragma GCC diagnostic warning "-Wconversion"
  #pragma GCC diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma GCC diagnostic ignored "-Wuninitialized"
  #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
// ### END_FILE_INCLUDE: linkage.h

#ifndef STC_HMAP_H_INCLUDED
#define STC_HMAP_H_INCLUDED
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
#define c_SELECT(S, ...) c_EXPAND(S(__VA_ARGS__)) // c_EXPAND for MSVC

#ifndef __cplusplus
    #define _i_malloc(T, n)     ((T*)i_malloc((n)*c_sizeof(T)))
    #define _i_calloc(T, n)     ((T*)i_calloc(n, c_sizeof(T)))
    #define c_new(T, ...)       ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_literal(T)        (T)
#else
    #include <new>
    #define _i_malloc(T, n)     static_cast<T*>(i_malloc((n)*c_sizeof(T)))
    #define _i_calloc(T, n)     static_cast<T*>(i_calloc(n, c_sizeof(T)))
    #define c_new(T, ...)       new (malloc(sizeof(T))) T(__VA_ARGS__)
    #define c_literal(T)        T
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

#define c_swap(xp, yp) do { \
    (void)sizeof((xp) == (yp)); \
    char _tv[sizeof *(xp)]; \
    void *_xp = xp, *_yp = yp; \
    memcpy(_tv, _xp, sizeof _tv); \
    memcpy(_xp, _yp, sizeof _tv); \
    memcpy(_yp, _tv, sizeof _tv); \
} while (0)

// use with gcc -Wconversion
typedef ptrdiff_t               isize;
#define c_sizeof                (isize)sizeof
#define c_strlen(s)             (isize)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u_size(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u_size(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u_size(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u_size(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u_size(ilen))
// Mostly library internal, but may be useful in user code:
#define c_u2i_size(u)           (isize)(1 ? (u) : (size_t)1) // warns if u is signed
#define c_i2u_size(i)           (size_t)(1 ? (i) : -1)       // warns if i is unsigned
#define c_uless(a, b)           ((size_t)(a) < (size_t)(b))
#define c_safe_cast(T, From, x) ((T)(1 ? (x) : (From){0}))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(p)       chash_n(p, sizeof *(p))

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

/* Function macros and others */

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (isize)(sizeof(a)/sizeof 0[a])

// Non-owning c-string "class"
typedef const char* cstr_raw;
#define cstr_raw_cmp(xp, yp) strcmp(*(xp), *(yp))
#define cstr_raw_eq(xp, yp) (cstr_raw_cmp(xp, yp) == 0)
#define cstr_raw_hash(p) chash_str(*(p))
#define cstr_raw_clone(s) (s)
#define cstr_raw_drop(p) ((void)p)

#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t chash_n(const void* key, isize len) {
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

STC_INLINE uint64_t chash_str(const char *str)
    { return chash_n(str, c_strlen(str)); }

STC_INLINE uint64_t _chash_mix(uint64_t h[], int n) { // n > 0
    for (int i = 1; i < n; ++i) h[0] += h[0] ^ h[i]; // non-commutative!
    return h[0];
}

STC_INLINE char* cstrnstrn(const char *str, isize slen,
                            const char *needle, isize nlen) {
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

STC_INLINE isize cnextpow2(isize n) {
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
#define c_foreach_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _)

#define c_foreach_reverse(...) c_MACRO_OVERLOAD(c_foreach_reverse, __VA_ARGS__)
#define c_foreach_reverse_3(it, C, cnt) /* works for stack, vec, queue, deque */ \
    for (C##_iter it = C##_rbegin(&cnt); it.ref; C##_rnext(&it))
#define c_foreach_reverse_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _r)

#define _c_foreach(it, C, start, endref, rev) /* private */ \
    for (C##_iter it = (start), *_endref = c_safe_cast(C##_iter*, C##_value*, endref) \
         ; it.ref != (C##_value*)_endref; C##rev##next(&it))

#define c_foreach_kv(...) c_MACRO_OVERLOAD(c_foreach_kv, __VA_ARGS__)
#define _c_foreach_kv(key, val, C, start, endref) /* structured binding for maps */ \
    for (const C##_key *key, **_k = &key; _k; ) \
    for (C##_mapped *val; _k; _k = NULL) \
    for (C##_iter _it = start, *_endref = c_safe_cast(C##_iter*, C##_value*, endref) ; \
         _it.ref != (C##_value*)_endref && (key = &_it.ref->first, val = &_it.ref->second); \
         C##_next(&_it))

#define c_foreach_kv_4(key, val, C, cnt) \
        _c_foreach_kv(key, val, C, C##_begin(&cnt), NULL)
#define c_foreach_kv_5(key, val, C, start, end) \
        _c_foreach_kv(key, val, C, start, (end).ref)

#define c_forlist(...) 'c_forlist not_supported. Use c_foritems'   // [removed]
#define c_forpair(...) 'c_forpair not_supported. Use c_foreach_kv' // [removed]

// c_forrange: python-like indexed iteration
#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (isize i=start, _end=stop; i < _end; ++i)
#define c_forrange_4(i, start, stop, step) \
    for (isize i=start, _inc=step, _end=(isize)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
    #define c_init(C, ...) \
        C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))
    #define c_foritems(it, T, ...) \
        for (struct {T* ref; int size, index;} \
             it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define chash_mix(...) \
        _chash_mix((uint64_t[]){__VA_ARGS__}, c_NUMARGS(__VA_ARGS__))
#else
    #include <initializer_list>
    #include <array>
    template <class C, class T>
    inline C _from_n(C (*func)(const T[], isize), std::initializer_list<T> il)
        { return func(&*il.begin(), il.size()); }
    #define c_init(C, ...) _from_n<C,C##_raw>(C##_from_n, __VA_ARGS__)
    #define c_foritems(it, T, ...) \
        for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator ref; size_t size, index;} \
             it = {._il=__VA_ARGS__, .ref=it._il.begin(), .size=it._il.size()} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define chash_mix(...) \
        _chash_mix(std::array<uint64_t, c_NUMARGS(__VA_ARGS__)>{__VA_ARGS__}.data(), c_NUMARGS(__VA_ARGS__))
#endif

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(init, deinit) \
    for (int _i = 1; _i; ) for (init; _i; _i = 0, deinit) // thanks, tstanisl
#define c_with_3(init, condition, deinit) \
    for (int _i = 1; _i; ) for (init; _i && (condition); _i = 0, deinit)

#define c_deferred(...) \
    for (int _i = 1; _i; _i = 0, __VA_ARGS__)

#define c_drop(C, ...) \
    do { c_foritems (_i, C*, {__VA_ARGS__}) C##_drop(*_i.ref); } while(0)

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
// ### BEGIN_FILE_INCLUDE: types.h

#ifdef i_aux
  #define _i_aux_struct struct { i_aux } aux;
#else
  #define _i_aux_struct
#endif

#ifndef STC_TYPES_H_INCLUDED
#define STC_TYPES_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define forward_arc(C, VAL) _c_arc_types(C, VAL)
#define forward_box(C, VAL) _c_box_types(C, VAL)
#define forward_deq(C, VAL) _c_deque_types(C, VAL)
#define forward_list(C, VAL) _c_list_types(C, VAL)
#define forward_hmap(C, KEY, VAL) _c_htable_types(C, KEY, VAL, c_true, c_false)
#define forward_hset(C, KEY) _c_htable_types(C, cset, KEY, KEY, c_false, c_true)
#define forward_smap(C, KEY, VAL) _c_aatree_types(C, KEY, VAL, c_true, c_false)
#define forward_sset(C, KEY) _c_aatree_types(C, KEY, KEY, c_false, c_true)
#define forward_stack(C, VAL) _c_stack_types(C, VAL)
#define forward_pqueue(C, VAL) _c_pqueue_types(C, VAL)
#define forward_queue(C, VAL) _c_deque_types(C, VAL)
#define forward_vec(C, VAL) _c_vec_types(C, VAL)

// csview : non-null terminated string view
typedef const char csview_value;
typedef struct csview {
    csview_value* buf;
    ptrdiff_t size;
} csview;

typedef union {
    csview_value* ref;
    csview chr;
    struct { csview chr; csview_value* end; } u8;
} csview_iter;

#define c_sv(...) c_MACRO_OVERLOAD(c_sv, __VA_ARGS__)
#define c_sv_1(literal) c_sv_2(literal, c_litstrlen(literal))
#define c_sv_2(str, n) (c_literal(csview){str, n})
#define c_svfmt "%.*s"
#define c_svarg(sv) (int)(sv).size, (sv).buf // printf(c_svfmt "\n", c_svarg(sv));
#define c_SVARG(sv) c_svarg(sv) // [deprecated]
#define c_SV(sv) c_svarg(sv) // [deprecated]

// zsview : zero-terminated string view
typedef csview_value zsview_value;
typedef struct zsview {
    zsview_value* str;
    ptrdiff_t size;
} zsview;

typedef union {
    zsview_value* ref;
    csview chr;
} zsview_iter;

#define c_zv(literal) (c_literal(zsview){literal, c_litstrlen(literal)})

// cstr : zero-terminated owning string (short string optimized - sso)
typedef char cstr_value;
typedef struct { cstr_value* data; ptrdiff_t size, cap; } cstr_buf;
typedef union cstr {
    struct { cstr_value data[ sizeof(cstr_buf) ]; } sml;
    struct { cstr_value* data; size_t size, ncap; } lon;
} cstr;

typedef union {
    cstr_value* ref;
    csview chr; // utf8 character/codepoint
} cstr_iter;


#if defined __GNUC__ || defined __clang__ || defined _MSC_VER
    typedef long catomic_long;
#else
    typedef _Atomic(long) catomic_long;
#endif

#define c_true(...) __VA_ARGS__
#define c_false(...)

#define _c_arc_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
        catomic_long* use_count; \
    } SELF

#define _c_box_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
    } SELF

#define _c_deque_types(SELF, VAL) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value *cbuf; \
        ptrdiff_t start, end, capmask; \
        _i_aux_struct \
    } SELF; \
\
    typedef struct { \
        SELF##_value *ref; \
        ptrdiff_t pos; \
        const SELF* _s; \
    } SELF##_iter

#define _c_list_types(SELF, VAL) \
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
        _i_aux_struct \
    } SELF

#define _c_htable_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
\
    typedef struct { \
        SELF##_value *ref; \
        size_t idx; \
        bool inserted; \
        uint8_t hashx; \
        uint16_t dist; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref, *_end; \
        struct hmap_meta *_mref; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value* table; \
        struct hmap_meta* meta; \
        ptrdiff_t size, bucket_count; \
        _i_aux_struct \
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
        int32_t root, disp, head, size, capacity; \
        _i_aux_struct \
    } SELF

#define _c_stack_fixed(SELF, VAL, CAP) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value data[CAP]; ptrdiff_t size; } SELF

#define _c_stack_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value* data; ptrdiff_t size, capacity; _i_aux_struct } SELF

#define _c_vec_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value *data; ptrdiff_t size, capacity; _i_aux_struct } SELF

#define _c_pqueue_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { SELF##_value* data; ptrdiff_t size, capacity; _i_aux_struct } SELF

#endif // STC_TYPES_H_INCLUDED
// ### END_FILE_INCLUDE: types.h
#include <stdlib.h>
#include <string.h>
#define _hashmask 0x3fU
#define _distmask 0x3ffU
struct hmap_meta { uint16_t hashx:6, dist:10; }; // dist: 0=empty, 1=PSL 0, 2=PSL 1, ...
#endif // STC_HMAP_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix hmap_
#endif
#ifndef _i_is_set
  #define _i_is_map
  #define _i_MAP_ONLY c_true
  #define _i_SET_ONLY c_false
  #define _i_keyref(vp) (&(vp)->first)
#else
  #define _i_MAP_ONLY c_false
  #define _i_SET_ONLY c_true
  #define _i_keyref(vp) (vp)
#endif
#define _i_is_hash
// ### BEGIN_FILE_INCLUDE: template.h
#ifndef _i_template
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define c_option(flag)  ((i_opt) & (flag))
  #define c_is_forward    (1<<0)
  #define c_no_atomic     (1<<1)
  #define c_no_clone      (1<<2)
  #define c_no_hash       (1<<4)
  #define c_use_cmp       (1<<5)
  #define c_use_eq        (1<<6)

  #define _c_MEMB(name) c_JOIN(Self, name)
  #define _c_DEFTYPES(macro, SELF, ...) c_EXPAND(macro(SELF, __VA_ARGS__))
  #define _m_value _c_MEMB(_value)
  #define _m_key _c_MEMB(_key)
  #define _m_mapped _c_MEMB(_mapped)
  #define _m_rmapped _c_MEMB(_rmapped)
  #define _m_raw _c_MEMB(_raw)
  #define _m_keyraw _c_MEMB(_keyraw)
  #define _m_iter _c_MEMB(_iter)
  #define _m_result _c_MEMB(_result)
  #define _m_node _c_MEMB(_node)
#endif

#if defined i_key_arc
  #define i_key_arcbox i_key_arc
#elif defined i_key_box
  #define i_key_arcbox i_key_box
#elif defined i_key_str // [deprecated]
  #define i_key_cstr
  #define i_tag str
#endif

#if defined i_val_arc
  #define i_val_arcbox i_val_arc
#elif defined i_val_box
  #define i_val_arcbox i_val_box
#elif defined i_val_str // [deprecated]
  #define i_val_cstr
  #define i_tag str
#endif

#ifdef i_TYPE // [deprecated]
  #define i_type i_TYPE
#endif

#if defined i_cmpclass
  #define i_use_cmp
  #define i_use_eq
#endif

#ifdef i_class
  #define Self c_SELECT(_c_SEL21, i_class)
  #define i_keyclass c_SELECT(_c_SEL22, i_class)
#elif defined i_type && !(defined i_key || defined i_keyclass || \
                          defined i_key_cstr || defined i_key_arcbox)
  #if defined i_cmpclass
    #define Self i_type
    #define i_key i_cmpclass
    #define i_keytoraw c_default_toraw
  #elif defined _i_is_map
    #define Self c_SELECT(_c_SEL31, i_type)
    #define i_key c_SELECT(_c_SEL32, i_type)
    #define i_val c_SELECT(_c_SEL33, i_type)
  #else
    #define Self c_SELECT(_c_SEL21, i_type)
    #define i_key c_SELECT(_c_SEL22, i_type)
  #endif
#elif !defined Self && defined i_type
  #define Self i_type
#elif !defined Self
  #define Self c_JOIN(_i_prefix, i_tag)
#endif

#define i_no_emplace

#if c_option(c_is_forward)
  #define i_is_forward
#endif
#if c_option(c_no_hash)
  #define i_no_hash
#endif
#if c_option(c_use_cmp)
  #define i_use_cmp
#endif
#if c_option(c_use_eq)
  #define i_use_eq
#endif
#if c_option(c_no_clone) || defined _i_is_arc
  #define i_no_clone
#endif

// Handle predefined element-types with lookup convertion types:
// cstr_from(const char*) and arc_T_from(T) / box_T_from(T)
#if defined i_key_cstr
  #define i_keyclass cstr
  #define i_cmpclass cstr_raw
#elif defined i_key_arcbox
  #define i_keyclass i_key_arcbox
  #define i_cmpclass c_JOIN(i_key_arcbox, _raw)
#endif

// Check for i_keyclass and i_cmpclass, and fill in missing defs.
#if defined i_cmpclass
  #define i_keyraw i_cmpclass
#elif defined i_keyclass && !defined i_keyraw
  #define i_cmpclass i_key
#endif

// Bind to i_key "class members": _clone, _drop, _from and _toraw (when conditions are met).
#if defined i_keyclass
  #define i_key i_keyclass
  #if !defined i_keyclone && !defined i_no_clone
    #define i_keyclone c_JOIN(i_keyclass, _clone)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_JOIN(i_keyclass, _drop)
  #endif
  #if !defined i_keyfrom && defined i_keyraw
    #define i_keyfrom c_JOIN(i_keyclass, _from)
  #elif !defined i_keyfrom && !defined i_no_clone
    #define i_keyfrom c_JOIN(i_keyclass, _clone)
  #endif
  #if !defined i_keytoraw && defined i_keyraw
    #define i_keytoraw c_JOIN(i_keyclass, _toraw)
  #endif
#endif

// Define when container has support for sorting (cmp) and linear search (eq)
#if defined i_use_cmp || defined i_cmp || defined i_less
  #define _i_has_cmp
#endif
#if defined i_use_cmp || defined i_cmp || defined i_use_eq || defined i_eq
  #define _i_has_eq
#endif

// Bind to i_keyraw "class members": _cmp, _eq and _hash (when conditions are met).
#if defined i_cmpclass // => i_keyraw
  #if !(defined i_cmp || defined i_less) && (defined i_use_cmp || defined _i_sorted || defined _i_is_pqueue)
    #define i_cmp c_JOIN(i_cmpclass, _cmp)
  #endif
  #if !defined i_eq && (defined i_use_eq || defined i_hash || defined _i_is_hash)
    #define i_eq c_JOIN(i_cmpclass, _eq)
  #endif
  #if !(defined i_hash || defined i_no_hash)
    #define i_hash c_JOIN(i_cmpclass, _hash)
  #endif
#endif

#if !defined i_key
  #error "No i_key defined"
#elif defined i_keyraw ^ defined i_keytoraw
  #error "Both i_keyraw/i_valraw and i_keytoraw/i_valtoraw must be defined, if any"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone/i_valclone and i_keydrop/i_valdrop must be defined, if any (unless i_no_clone defined)."
#elif defined i_keyboxed || defined i_valboxed
  #error "i_keyboxed / i_valboxed not supported. " \
         "Use: i_key_box/i_key_arc ; i_val_box/i_val_arc."
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Use i_keyfrom/i_keydrop ; i_valfrom/i_valdrop"
#elif defined i_keyto || defined i_valto
  #error i_keyto/i_valto not supported. Use i_keytoraw/i_valtoraw
#elif defined i_keyraw && defined i_use_cmp && !defined _i_has_cmp
  #error "For smap/sset/pqueue, i_cmp or i_less must be defined when i_keyraw is defined."
#endif

// Fill in missing i_eq, i_less, i_cmp functions with defaults.
#if !defined i_eq && defined i_cmp
  #define i_eq(x, y) (i_cmp(x, y)) == 0
#elif !defined i_eq
  #define i_eq(x, y) *x == *y // works for integral types
#endif
#if !defined i_less && defined i_cmp
  #define i_less(x, y) (i_cmp(x, y)) < 0
#elif !defined i_less
  #define i_less(x, y) *x < *y // works for integral types
#endif
#if !defined i_cmp && defined i_less
  #define i_cmp(x, y) (i_less(y, x)) - (i_less(x, y))
#endif
#if !(defined i_hash || defined i_no_hash)
  #define i_hash c_default_hash
#endif

#ifndef i_tag
  #define i_tag i_key
#endif
#if !defined i_keyfrom && defined i_keyclone && !defined i_keyraw
  #define i_keyfrom i_keyclone
#elif !defined i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #undef i_no_emplace
#endif
#ifndef i_keyraw
  #define i_keyraw i_key
  #define i_keytoraw c_default_toraw
#endif
#ifndef i_keyclone
  #define i_keyclone c_default_clone
#endif
#ifndef i_keydrop
  #define i_keydrop c_default_drop
#endif

#if defined _i_is_map // ---- process hmap/smap value i_val, ... ----

#ifdef i_val_cstr
  #define i_valclass cstr
  #define i_valraw const char*
#elif defined i_val_arcbox
  #define i_valclass i_val_arcbox
  #define i_valraw c_JOIN(i_val_arcbox, _raw)
#endif

#ifdef i_valclass
  #define i_val i_valclass
  #if !defined i_valclone && !defined i_no_clone
    #define i_valclone c_JOIN(i_valclass, _clone)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_JOIN(i_valclass, _drop)
  #endif
  #if !defined i_valfrom && defined i_valraw
    #define i_valfrom c_JOIN(i_valclass, _from)
  #elif !defined i_valfrom && !defined i_no_clone
    #define i_valfrom c_JOIN(i_valclass, _clone)
  #endif
  #if !defined i_valtoraw && defined i_valraw
    #define i_valtoraw c_JOIN(i_valclass, _toraw)
  #endif
#endif

#ifndef i_val
  #error "i_val* must be defined for maps"
#elif defined i_valraw ^ defined i_valtoraw
  #error "Both i_valraw and i_valtoraw must be defined, if any"
#elif !defined i_no_clone && (defined i_valclone ^ defined i_valdrop)
  #error "Both i_valclone and i_valdrop must be defined, if any"
#endif

#if !defined i_valfrom && defined i_valclone && !defined i_valraw
  #define i_valfrom i_valclone
#elif !defined i_valfrom
  #define i_valfrom c_default_clone
#else
  #undef i_no_emplace
#endif
#ifndef i_valraw
  #define i_valraw i_val
  #define i_valtoraw c_default_toraw
#endif
#ifndef i_valclone
  #define i_valclone c_default_clone
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#endif

#endif // !_i_is_map

#ifndef i_val
  #define i_val i_key
#endif
#ifndef i_valraw
  #define i_valraw i_keyraw
#endif
#endif // STC_TEMPLATE_H_INCLUDED
// ### END_FILE_INCLUDE: template.h
#ifndef i_is_forward
  _c_DEFTYPES(_c_htable_types, Self, i_key, i_val, _i_MAP_ONLY, _i_SET_ONLY);
#endif

_i_MAP_ONLY( struct _m_value {
    _m_key first;
    _m_mapped second;
}; )

typedef i_keyraw _m_keyraw;
typedef i_valraw _m_rmapped;
typedef _i_SET_ONLY( i_keyraw )
        _i_MAP_ONLY( struct { _m_keyraw first;
                              _m_rmapped second; } )
_m_raw;

STC_API Self            _c_MEMB(_with_capacity)(isize cap);
#if !defined i_no_clone
STC_API Self            _c_MEMB(_clone)(Self map);
#endif
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API bool            _c_MEMB(_reserve)(Self* self, isize capacity);
STC_API void            _c_MEMB(_erase_entry)(Self* self, _m_value* val);
STC_API float           _c_MEMB(_max_load_factor)(const Self* self);
STC_API isize           _c_MEMB(_capacity)(const Self* map);
static _m_result        _c_MEMB(_bucket_lookup_)(const Self* self, const _m_keyraw* rkeyptr);
static _m_result        _c_MEMB(_bucket_insert_)(const Self* self, const _m_keyraw* rkeyptr);

STC_INLINE Self         _c_MEMB(_init)(void) { Self map = {0}; return map; }
STC_INLINE void         _c_MEMB(_shrink_to_fit)(Self* self) { _c_MEMB(_reserve)(self, (isize)self->size); }
STC_INLINE bool         _c_MEMB(_is_empty)(const Self* map) { return !map->size; }
STC_INLINE isize        _c_MEMB(_size)(const Self* map) { return (isize)map->size; }
STC_INLINE isize        _c_MEMB(_bucket_count)(Self* map) { return map->bucket_count; }
STC_INLINE bool         _c_MEMB(_contains)(const Self* self, _m_keyraw rkey)
                            { return self->size && _c_MEMB(_bucket_lookup_)(self, &rkey).ref; }

#ifndef i_max_load_factor
  #define i_max_load_factor 0.80f
#endif

STC_INLINE _m_result
_c_MEMB(_insert_entry_)(Self* self, _m_keyraw rkey) {
    if (self->size >= (isize)((float)self->bucket_count * (i_max_load_factor)))
        if (!_c_MEMB(_reserve)(self, (isize)(self->size*3/2 + 2)))
            return c_literal(_m_result){0};

    _m_result res = _c_MEMB(_bucket_insert_)(self, &rkey);
    self->size += res.inserted;
    return res;
}

#ifdef _i_is_map
    STC_API _m_result _c_MEMB(_insert_or_assign)(Self* self, _m_key key, _m_mapped mapped);
    #if !defined i_no_emplace
    STC_API _m_result _c_MEMB(_emplace_or_assign)(Self* self, _m_keyraw rkey, _m_rmapped rmapped);
    #endif

    STC_INLINE const _m_mapped* _c_MEMB(_at)(const Self* self, _m_keyraw rkey) {
        _m_result res = _c_MEMB(_bucket_lookup_)(self, &rkey);
        c_assert(res.ref);
        return &res.ref->second;
    }

    STC_INLINE _m_mapped* _c_MEMB(_at_mut)(Self* self, _m_keyraw rkey)
        { return (_m_mapped*)_c_MEMB(_at)(self, rkey); }
#endif // _i_is_map

#if !defined i_no_clone
    STC_INLINE void _c_MEMB(_copy)(Self *self, const Self* other) {
        if (self->table == other->table)
            return;
        _c_MEMB(_drop)(self);
        *self = _c_MEMB(_clone)(*other);
    }

    STC_INLINE _m_value _c_MEMB(_value_clone)(_m_value _val) {
        *_i_keyref(&_val) = i_keyclone((*_i_keyref(&_val)));
        _i_MAP_ONLY( _val.second = i_valclone(_val.second); )
        return _val;
    }
#endif // !i_no_clone

#if !defined i_no_emplace
    STC_INLINE _m_result
    _c_MEMB(_emplace)(Self* self, _m_keyraw rkey _i_MAP_ONLY(, _m_rmapped rmapped)) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, rkey);
        if (_res.inserted) {
            *_i_keyref(_res.ref) = i_keyfrom(rkey);
            _i_MAP_ONLY( _res.ref->second = i_valfrom(rmapped); )
        }
        return _res;
    }

    #ifdef _i_is_map
    STC_INLINE _m_result _c_MEMB(_emplace_key)(Self* self, _m_keyraw rkey) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, rkey);
        if (_res.inserted)
            _res.ref->first = i_keyfrom(rkey);
        return _res;
    }
    #endif // _i_is_map
#endif // !i_no_emplace

STC_INLINE _m_raw _c_MEMB(_value_toraw)(const _m_value* val) {
    return _i_SET_ONLY( i_keytoraw(val) )
           _i_MAP_ONLY( c_literal(_m_raw){i_keytoraw((&val->first)), i_valtoraw((&val->second))} );
}

STC_INLINE void _c_MEMB(_value_drop)(_m_value* _val) {
    i_keydrop(_i_keyref(_val));
    _i_MAP_ONLY( i_valdrop((&_val->second)); )
}

STC_INLINE _m_result
_c_MEMB(_insert)(Self* self, _m_key _key _i_MAP_ONLY(, _m_mapped _mapped)) {
    _m_result _res = _c_MEMB(_insert_entry_)(self, i_keytoraw((&_key)));
    if (_res.inserted)
        { *_i_keyref(_res.ref) = _key; _i_MAP_ONLY( _res.ref->second = _mapped; )}
    else
        { i_keydrop((&_key)); _i_MAP_ONLY( i_valdrop((&_mapped)); )}
    return _res;
}

STC_INLINE _m_value* _c_MEMB(_push)(Self* self, _m_value _val) {
    _m_result _res = _c_MEMB(_insert_entry_)(self, i_keytoraw(_i_keyref(&_val)));
    if (_res.inserted)
        *_res.ref = _val;
    else
        _c_MEMB(_value_drop)(&_val);
    return _res.ref;
}

#ifdef _i_is_map
STC_INLINE _m_result _c_MEMB(_put)(Self* self, _m_keyraw rkey, _m_rmapped rmapped) {
    #ifdef i_no_emplace
        return _c_MEMB(_insert_or_assign)(self, rkey, rmapped);
    #else
        return _c_MEMB(_emplace_or_assign)(self, rkey, rmapped);
    #endif
}
#endif

STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n) {
    while (n--)
        #if defined _i_is_set && defined i_no_emplace
            _c_MEMB(_insert)(self, *raw++);
        #elif defined _i_is_set
            _c_MEMB(_emplace)(self, *raw++);
        #else
            _c_MEMB(_put)(self, raw->first, raw->second), ++raw;
        #endif
}

STC_INLINE Self _c_MEMB(_from_n)(const _m_raw* raw, isize n)
    { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_API _m_iter _c_MEMB(_begin)(const Self* self);

STC_INLINE _m_iter _c_MEMB(_end)(const Self* self)
    { (void)self; return c_literal(_m_iter){0}; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it) {
    while ((++it->ref, (++it->_mref)->dist == 0)) ;
    if (it->ref == it->_end) it->ref = NULL;
}

STC_INLINE _m_iter _c_MEMB(_advance)(_m_iter it, size_t n) {
    while (n-- && it.ref) _c_MEMB(_next)(&it);
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_find)(const Self* self, _m_keyraw rkey) {
    _m_value* ref;
    if (self->size && (ref = _c_MEMB(_bucket_lookup_)(self, &rkey).ref))
        return c_literal(_m_iter){ref,
                                  &self->table[self->bucket_count],
                                  &self->meta[ref - self->table]};
    return _c_MEMB(_end)(self);
}

STC_INLINE const _m_value*
_c_MEMB(_get)(const Self* self, _m_keyraw rkey) {
    return self->size ? _c_MEMB(_bucket_lookup_)(self, &rkey).ref : NULL;
}

STC_INLINE _m_value*
_c_MEMB(_get_mut)(Self* self, _m_keyraw rkey)
    { return (_m_value*)_c_MEMB(_get)(self, rkey); }

STC_INLINE int
_c_MEMB(_erase)(Self* self, _m_keyraw rkey) {
    _m_value* ref;
    if (self->size && (ref = _c_MEMB(_bucket_lookup_)(self, &rkey).ref))
        { _c_MEMB(_erase_entry)(self, ref); return 1; }
    return 0;
}

STC_INLINE _m_iter
_c_MEMB(_erase_at)(Self* self, _m_iter it) {
    _c_MEMB(_erase_entry)(self, it.ref);
    if (it._mref->dist == 0)
        _c_MEMB(_next)(&it);
    return it;
}

STC_INLINE bool
_c_MEMB(_eq)(const Self* self, const Self* other) {
    if (_c_MEMB(_size)(self) != _c_MEMB(_size)(other)) return false;
    for (_m_iter i = _c_MEMB(_begin)(self); i.ref; _c_MEMB(_next)(&i)) {
        const _m_keyraw _raw = i_keytoraw(_i_keyref(i.ref));
        if (!_c_MEMB(_contains)(other, _raw)) return false;
    }
    return true;
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF _m_iter _c_MEMB(_begin)(const Self* self) {
    _m_iter it = {self->table, self->table+self->bucket_count, self->meta};
    if (it._mref)
        while (it._mref->dist == 0)
            ++it.ref, ++it._mref;
    if (it.ref == it._end) it.ref = NULL;
    return it;
}

STC_DEF float _c_MEMB(_max_load_factor)(const Self* self) {
    (void)self; return (float)(i_max_load_factor);
}

STC_DEF isize _c_MEMB(_capacity)(const Self* map) {
    return (isize)((float)map->bucket_count * (i_max_load_factor));
}

STC_DEF Self _c_MEMB(_with_capacity)(const isize cap) {
    Self map = {0};
    _c_MEMB(_reserve)(&map, cap);
    return map;
}

static void _c_MEMB(_wipe_)(Self* self) {
    if (self->size == 0)
        return;
    _m_value* d = self->table, *_end = &d[self->bucket_count];
    struct hmap_meta* m = self->meta;
    for (; d != _end; ++d)
        if ((m++)->dist)
            _c_MEMB(_value_drop)(d);
}

STC_DEF void _c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    if (self->bucket_count > 0) {
        _c_MEMB(_wipe_)(self);
        i_free(self->meta, (self->bucket_count + 1)*c_sizeof *self->meta);
        i_free(self->table, self->bucket_count*c_sizeof *self->table);
    }
}

STC_DEF void _c_MEMB(_clear)(Self* self) {
    _c_MEMB(_wipe_)(self);
    self->size = 0;
    c_memset(self->meta, 0, c_sizeof(struct hmap_meta)*self->bucket_count);
}

#ifdef _i_is_map
    STC_DEF _m_result
    _c_MEMB(_insert_or_assign)(Self* self, _m_key _key, _m_mapped _mapped) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, i_keytoraw((&_key)));
        _m_mapped* _mp = _res.ref ? &_res.ref->second : &_mapped;
        if (_res.inserted)
            _res.ref->first = _key;
        else
            { i_keydrop((&_key)); i_valdrop(_mp); }
        *_mp = _mapped;
        return _res;
    }

    #if !defined i_no_emplace
    STC_DEF _m_result
    _c_MEMB(_emplace_or_assign)(Self* self, _m_keyraw rkey, _m_rmapped rmapped) {
        _m_result _res = _c_MEMB(_insert_entry_)(self, rkey);
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
#endif // _i_is_map

static _m_result
_c_MEMB(_bucket_lookup_)(const Self* self, const _m_keyraw* rkeyptr) {
    const uint64_t _hash = i_hash(rkeyptr);
    const size_t _idxmask = (size_t)self->bucket_count - 1;
    _m_result _res = {.idx=_hash & _idxmask, .hashx=(uint8_t)((_hash >> 24) & _hashmask), .dist=1};

    while (_res.dist <= self->meta[_res.idx].dist) {
        if (self->meta[_res.idx].hashx == _res.hashx) {
            const _m_keyraw _raw = i_keytoraw(_i_keyref(&self->table[_res.idx]));
            if (i_eq((&_raw), rkeyptr)) {
                _res.ref = &self->table[_res.idx];
                break;
            }
        }
        _res.idx = (_res.idx + 1) & _idxmask;
        ++_res.dist;
    }
    return _res;
}

static _m_result
_c_MEMB(_bucket_insert_)(const Self* self, const _m_keyraw* rkeyptr) {
    _m_result res = _c_MEMB(_bucket_lookup_)(self, rkeyptr);
    if (res.ref) // bucket exists
        return res;
    res.ref = &self->table[res.idx];
    res.inserted = true;
    struct hmap_meta snew = {.hashx=(uint16_t)(res.hashx & _hashmask),
                             .dist=(uint16_t)(res.dist & _distmask)};
    struct hmap_meta scur = self->meta[res.idx];
    self->meta[res.idx] = snew;

    if (scur.dist != 0) { // collision, reorder buckets
        struct hmap_meta *meta = self->meta;
        size_t mask = (size_t)self->bucket_count - 1;
        _m_value dcur = *res.ref;
        for (;;) {
            res.idx = (res.idx + 1) & mask;
            ++scur.dist;
            if (meta[res.idx].dist == 0)
                break;
            if (meta[res.idx].dist < scur.dist) {
                c_swap(&scur, &meta[res.idx]);
                c_swap(&dcur, &self->table[res.idx]);
            }
        }
        meta[res.idx] = scur;
        self->table[res.idx] = dcur;
    }
    return res;
}


#if !defined i_no_clone
    STC_DEF Self
    _c_MEMB(_clone)(Self map) {
        if (map.bucket_count) {
            _m_value *d = _i_malloc(_m_value, map.bucket_count);
            const isize _mbytes = (map.bucket_count + 1)*c_sizeof *map.meta;
            struct hmap_meta *m = (struct hmap_meta *)i_malloc(_mbytes);
            if (d && m) {
                c_memcpy(m, map.meta, _mbytes);
                _m_value *_dst = d, *_end = map.table + map.bucket_count;
                for (; map.table != _end; ++map.table, ++map.meta, ++_dst)
                    if (map.meta->dist)
                        *_dst = _c_MEMB(_value_clone)(*map.table);
            } else {
                if (d) i_free(d, map.bucket_count*c_sizeof *d);
                if (m) i_free(m, _mbytes);
                d = 0, m = 0, map.bucket_count = 0;
            }
            map.table = d, map.meta = m;
        }
        return map;
    }
#endif

STC_DEF bool
_c_MEMB(_reserve)(Self* self, const isize _newcap) {
    const isize _oldbucks = self->bucket_count;
    if (_newcap != self->size && _newcap <= _oldbucks)
        return true;
    isize _newbucks = (isize)((float)_newcap / (i_max_load_factor)) + 4;
    _newbucks = cnextpow2(_newbucks);

    Self map = {
        _i_malloc(_m_value, _newbucks),
        _i_calloc(struct hmap_meta, _newbucks + 1),
        self->size, _newbucks
    };

    bool ok = map.table && map.meta;
    if (ok) {  // Rehash:
        map.meta[_newbucks].dist = _distmask; // end-mark for iter
        const _m_value* d = self->table;
        const struct hmap_meta* m = self->meta;

        for (isize i = 0; i < _oldbucks; ++i, ++d) if ((m++)->dist) {
            _m_keyraw r = i_keytoraw(_i_keyref(d));
            _m_result _res = _c_MEMB(_bucket_insert_)(&map, &r);
            *_res.ref = *d; // move
        }
        c_swap(self, &map);
    }
    i_free(map.meta, (map.bucket_count + (int)(map.meta != NULL))*c_sizeof *map.meta);
    i_free(map.table, map.bucket_count*c_sizeof *map.table);
    return ok;
}

STC_DEF void
_c_MEMB(_erase_entry)(Self* self, _m_value* _val) {
    _m_value* d = self->table;
    struct hmap_meta *m = self->meta;
    size_t i = (size_t)(_val - d), j = i;
    size_t mask = (size_t)self->bucket_count - 1;

    _c_MEMB(_value_drop)(_val);
    for (;;) {
        j = (j + 1) & mask;
        if (m[j].dist < 2) // 0 => empty, 1 => PSL 0
            break;
        d[i] = d[j];
        m[i] = m[j];
        --m[i].dist;
        i = j;
    }
    m[i].dist = 0;
    --self->size;
}

#endif // i_implement
#undef i_max_load_factor
#undef _i_is_set
#undef _i_is_map
#undef _i_is_hash
#undef _i_keyref
#undef _i_MAP_ONLY
#undef _i_SET_ONLY
// ### BEGIN_FILE_INCLUDE: linkage2.h

#undef i_allocator
#undef i_malloc
#undef i_calloc
#undef i_realloc
#undef i_free
#undef i_aux
#undef _i_aux_struct

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
// ### BEGIN_FILE_INCLUDE: template2.h
#undef i_TYPE
#undef i_type
#undef i_class
#undef i_tag
#undef i_opt
#undef i_capacity

#undef i_key
#undef i_key_cstr
#undef i_key_str    // [deprecated]
#undef i_key_arc
#undef i_key_box
#undef i_key_arcbox // [deprecated]
#undef i_keyclass
#undef i_cmpclass   // define i_keyraw, and bind i_cmp, i_eq, i_hash "members"
#undef i_keyclone
#undef i_keydrop
#undef i_keyraw
#undef i_keyfrom
#undef i_keytoraw
#undef i_cmp
#undef i_less
#undef i_eq
#undef i_hash

#undef i_val
#undef i_val_cstr
#undef i_val_str    // [deprecated]
#undef i_val_arc
#undef i_val_box
#undef i_val_arcbox // [deprecated]
#undef i_valclass
#undef i_valclone
#undef i_valdrop
#undef i_valraw
#undef i_valfrom
#undef i_valtoraw

#undef i_use_cmp
#undef i_use_eq
#undef i_no_hash
#undef i_no_clone
#undef i_no_emplace
#undef i_is_forward

#undef _i_has_cmp
#undef _i_has_eq
#undef _i_prefix
#undef _i_template
#undef Self
// ### END_FILE_INCLUDE: template2.h
// ### END_FILE_INCLUDE: hmap.h
// ### END_FILE_INCLUDE: hset.h


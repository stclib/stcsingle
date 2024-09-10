// ### BEGIN_FILE_INCLUDE: deque.h
// Deque - double ended queue. Implemented as a ring buffer, extension of queue.

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

#ifndef STC_DEQUE_H_INCLUDED
#define STC_DEQUE_H_INCLUDED
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

#define c_foreach_index(k, it, C, cnt) \
    for (isize k = 0, _i=1; _i; _i=0) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it), ++k)

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

#define c_defer(...) \
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
    SELF##_value, SELF##_entry; \
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
    SELF##_value, SELF##_entry; \
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
#endif // STC_DEQUE_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix deque_
#endif
#define _pop _pop_front
#define _pull _pull_front
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
// ### BEGIN_FILE_INCLUDE: queue_prv.h

#ifndef i_is_forward
_c_DEFTYPES(_c_deque_types, Self, i_key);
#endif
typedef i_keyraw _m_raw;

STC_API Self            _c_MEMB(_with_capacity)(const isize n);
STC_API bool            _c_MEMB(_reserve)(Self* self, const isize n);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API _m_value*       _c_MEMB(_push)(Self* self, _m_value value); // push_back
STC_API void            _c_MEMB(_shrink_to_fit)(Self *self);
STC_API _m_iter         _c_MEMB(_advance)(_m_iter it, isize n);

#define _cbuf_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cbuf_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

STC_INLINE Self         _c_MEMB(_init)(void)
                            { Self cx = {0}; return cx; }

STC_INLINE void         _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
                            { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }

STC_INLINE Self         _c_MEMB(_from_n)(const _m_raw* raw, isize n)
                            { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }

STC_INLINE void         _c_MEMB(_value_drop)(_m_value* val) { i_keydrop(val); }

#if !defined i_no_emplace
STC_INLINE _m_value*    _c_MEMB(_emplace)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API bool            _c_MEMB(_eq)(const Self* self, const Self* other);
#endif

#if !defined i_no_clone
STC_API Self            _c_MEMB(_clone)(Self cx);
STC_INLINE _m_value     _c_MEMB(_value_clone)(_m_value val)
                            { return i_keyclone(val); }

STC_INLINE void         _c_MEMB(_copy)(Self* self, const Self* other) {
                            if (self->cbuf == other->cbuf) return;
                            _c_MEMB(_drop)(self);
                            *self = _c_MEMB(_clone)(*other);
                        }
#endif // !i_no_clone
STC_INLINE isize        _c_MEMB(_size)(const Self* self)
                            { return _cbuf_toidx(self, self->end); }
STC_INLINE isize        _c_MEMB(_capacity)(const Self* self)
                            { return self->capmask; }
STC_INLINE bool         _c_MEMB(_is_empty)(const Self* self)
                            { return self->start == self->end; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* pval)
                            { return i_keytoraw(pval); }

STC_INLINE _m_value*    _c_MEMB(_front)(const Self* self)
                            { return self->cbuf + self->start; }

STC_INLINE _m_value*    _c_MEMB(_back)(const Self* self)
                            { return self->cbuf + ((self->end - 1) & self->capmask); }

STC_INLINE void _c_MEMB(_pop)(Self* self) { // pop_front
    c_assert(!_c_MEMB(_is_empty)(self));
    i_keydrop((self->cbuf + self->start));
    self->start = (self->start + 1) & self->capmask;
}

STC_INLINE _m_value _c_MEMB(_pull)(Self* self) { // move front out of queue
    c_assert(!_c_MEMB(_is_empty)(self));
    isize s = self->start;
    self->start = (s + 1) & self->capmask;
    return self->cbuf[s];
}

STC_INLINE _m_iter _c_MEMB(_begin)(const Self* self) {
    return c_literal(_m_iter){
        .ref=_c_MEMB(_is_empty)(self) ? NULL : self->cbuf + self->start,
        .pos=self->start, ._s=self};
}

STC_INLINE _m_iter _c_MEMB(_rbegin)(const Self* self) {
    isize pos = (self->end - 1) & self->capmask;
    return c_literal(_m_iter){
        .ref=_c_MEMB(_is_empty)(self) ? NULL : self->cbuf + pos,
        .pos=pos, ._s=self};
}

STC_INLINE _m_iter _c_MEMB(_end)(const Self* self)
    { (void)self; return c_literal(_m_iter){0}; }

STC_INLINE _m_iter _c_MEMB(_rend)(const Self* self)
    { (void)self; return c_literal(_m_iter){0}; }

STC_INLINE void _c_MEMB(_next)(_m_iter* it) {
    if (it->pos != it->_s->capmask) { ++it->ref; ++it->pos; }
    else { it->ref -= it->pos; it->pos = 0; }
    if (it->pos == it->_s->end) it->ref = NULL;
}

STC_INLINE void _c_MEMB(_rnext)(_m_iter* it) {
    if (it->pos == it->_s->start) it->ref = NULL;
    else if (it->pos != 0) { --it->ref; --it->pos; }
    else it->ref += (it->pos = it->_s->capmask);
}

STC_INLINE isize _c_MEMB(_index)(const Self* self, _m_iter it)
    { return _cbuf_toidx(self, it.pos); }

STC_INLINE void _c_MEMB(_adjust_end_)(Self* self, isize n)
    { self->end = (self->end + n) & self->capmask; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement || defined i_static

STC_DEF _m_iter _c_MEMB(_advance)(_m_iter it, isize n) {
    isize len = _c_MEMB(_size)(it._s);
    isize pos = it.pos, idx = _cbuf_toidx(it._s, pos);
    it.pos = (pos + n) & it._s->capmask;
    it.ref += it.pos - pos;
    if (!c_uless(idx + n, len)) it.ref = NULL;
    return it;
}

STC_DEF void
_c_MEMB(_clear)(Self* self) {
    c_foreach (i, Self, *self)
        { i_keydrop(i.ref); }
    self->start = 0, self->end = 0;
}

STC_DEF void
_c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    _c_MEMB(_clear)(self);
    i_free(self->cbuf, (self->capmask + 1)*c_sizeof(*self->cbuf));
}

STC_DEF Self
_c_MEMB(_with_capacity)(const isize n) {
    Self cx = {0};
    _c_MEMB(_reserve)(&cx, n);
    return cx;
}

STC_DEF bool
_c_MEMB(_reserve)(Self* self, const isize n) {
    if (n <= self->capmask)
        return true;
    isize oldpow2 = self->capmask + 1, newpow2 = cnextpow2(n + 1);
    _m_value* d = (_m_value *)i_realloc(self->cbuf, oldpow2*c_sizeof *d, newpow2*c_sizeof *d);
    if (!d)
        return false;
    isize head = oldpow2 - self->start;
    if (self->start <= self->end)
        ;
    else if (head < self->end) {
        self->start = newpow2 - head;
        c_memmove(d + self->start, d + oldpow2 - head, head*c_sizeof *d);
    } else {
        c_memmove(d + oldpow2, d, self->end*c_sizeof *d);
        self->end += oldpow2;
    }
    self->capmask = newpow2 - 1;
    self->cbuf = d;
    return true;
}

STC_DEF _m_value*
_c_MEMB(_push)(Self* self, _m_value value) { // push_back
    isize end = (self->end + 1) & self->capmask;
    if (end == self->start) { // full
        _c_MEMB(_reserve)(self, self->capmask + 3); // => 2x expand
        end = (self->end + 1) & self->capmask;
    }
    _m_value *v = self->cbuf + self->end;
    self->end = end;
    *v = value;
    return v;
}

STC_DEF void
_c_MEMB(_shrink_to_fit)(Self *self) {
    isize sz = _c_MEMB(_size)(self), j = 0;
    if (sz > self->capmask/2)
        return;
    Self out = _c_MEMB(_with_capacity)(sz);
    if (!out.cbuf)
        return;
    c_foreach (i, Self, *self)
        out.cbuf[j++] = *i.ref;
    out.end = sz;
    i_free(self->cbuf, (self->capmask + 1)*c_sizeof(*self->cbuf));
    *self = out;
}

#if !defined i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self q) {
    isize sz = _c_MEMB(_size)(&q), j = 0;
    Self tmp = _c_MEMB(_with_capacity)(sz);
    if (tmp.cbuf)
        c_foreach (i, Self, q)
            tmp.cbuf[j++] = i_keyclone((*i.ref));
    q.cbuf = tmp.cbuf;
    q.capmask = tmp.capmask;
    q.start = 0;
    q.end = sz;
    return q;
}
#endif // i_no_clone

#if defined _i_has_eq
STC_DEF bool
_c_MEMB(_eq)(const Self* self, const Self* other) {
    if (_c_MEMB(_size)(self) != _c_MEMB(_size)(other)) return false;
    for (_m_iter i = _c_MEMB(_begin)(self), j = _c_MEMB(_begin)(other);
         i.ref; _c_MEMB(_next)(&i), _c_MEMB(_next)(&j))
    {
        const _m_raw _rx = i_keytoraw(i.ref), _ry = i_keytoraw(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return true;
}
#endif // _i_has_eq
#endif // IMPLEMENTATION
// ### END_FILE_INCLUDE: queue_prv.h
#undef _pop
#undef _pull

STC_API _m_value*   _c_MEMB(_push_front)(Self* self, _m_value value);
STC_API _m_iter     _c_MEMB(_insert_n)(Self* self, isize idx, const _m_value* arr, isize n);
STC_API _m_iter     _c_MEMB(_insert_uninit)(Self* self, isize idx, isize n);
STC_API void        _c_MEMB(_erase_n)(Self* self, isize idx, isize n);

STC_INLINE const _m_value*
_c_MEMB(_at)(const Self* self, isize idx)
    { return self->cbuf + _cbuf_topos(self, idx); }

STC_INLINE _m_value*
_c_MEMB(_at_mut)(Self* self, isize idx)
    { return self->cbuf + _cbuf_topos(self, idx); }

STC_INLINE _m_value*
_c_MEMB(_push_back)(Self* self, _m_value val)
    { return  _c_MEMB(_push)(self, val); }

STC_INLINE void
_c_MEMB(_pop_back)(Self* self) {
    c_assert(!_c_MEMB(_is_empty)(self));
    self->end = (self->end - 1) & self->capmask;
    i_keydrop((self->cbuf + self->end));
}

STC_INLINE _m_value _c_MEMB(_pull_back)(Self* self) { // move back out of deque
    c_assert(!_c_MEMB(_is_empty)(self));
    self->end = (self->end - 1) & self->capmask;
    return self->cbuf[self->end];
}

STC_INLINE _m_iter
_c_MEMB(_insert_at)(Self* self, _m_iter it, const _m_value val) {
    isize idx = _cbuf_toidx(self, it.pos);
    return _c_MEMB(_insert_n)(self, idx, &val, 1);
}

STC_INLINE _m_iter
_c_MEMB(_erase_at)(Self* self, _m_iter it) {
    _c_MEMB(_erase_n)(self, _cbuf_toidx(self, it.pos), 1);
    if (it.pos == self->end) it.ref = NULL;
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_erase_range)(Self* self, _m_iter it1, _m_iter it2) {
    isize idx1 = _cbuf_toidx(self, it1.pos);
    isize idx2 = _cbuf_toidx(self, it2.pos);
    _c_MEMB(_erase_n)(self, idx1, idx2 - idx1);
    if (it1.pos == self->end) it1.ref = NULL;
    return it1;
}

#if !defined i_no_emplace
STC_API _m_iter
_c_MEMB(_emplace_n)(Self* self, isize idx, const _m_raw* raw, isize n);

STC_INLINE _m_value*
_c_MEMB(_emplace_front)(Self* self, const _m_raw raw)
    { return _c_MEMB(_push_front)(self, i_keyfrom(raw)); }

STC_INLINE _m_value*
_c_MEMB(_emplace_back)(Self* self, const _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }

STC_INLINE _m_iter
_c_MEMB(_emplace_at)(Self* self, _m_iter it, const _m_raw raw)
    { return _c_MEMB(_insert_at)(self, it, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API _m_iter _c_MEMB(_find_in)(_m_iter p1, _m_iter p2, _m_raw raw);

STC_INLINE _m_iter
_c_MEMB(_find)(const Self* self, _m_raw raw) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw);
}

STC_INLINE const _m_value*
_c_MEMB(_get)(const Self* self, _m_raw raw) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw).ref;
}

STC_INLINE _m_value*
_c_MEMB(_get_mut)(Self* self, _m_raw raw)
    { return (_m_value *) _c_MEMB(_get)(self, raw); }
#endif // _i_has_eq

#if defined _i_has_cmp
// ### BEGIN_FILE_INCLUDE: sort_prv.h

#ifdef _i_is_list
  #define i_at(self, idx) (&((_m_value *)(self)->last)[idx])
  #define i_at_mut i_at
#elif !defined i_at
  #define i_at(self, idx) _c_MEMB(_at)(self, idx)
  #define i_at_mut(self, idx) _c_MEMB(_at_mut)(self, idx)
#endif

STC_API void _c_MEMB(_sort_lowhigh)(Self* self, isize lo, isize hi);

#ifdef _i_is_array
STC_API isize _c_MEMB(_lower_bound_range)(const Self* self, const _m_raw raw, isize start, isize end);
STC_API isize _c_MEMB(_binary_search_range)(const Self* self, const _m_raw raw, isize start, isize end);

static inline void _c_MEMB(_sort)(Self* arr, isize n)
    { _c_MEMB(_sort_lowhigh)(arr, 0, n - 1); }

static inline isize // -1 = not found
_c_MEMB(_lower_bound)(const Self* arr, const _m_raw raw, isize n)
    { return _c_MEMB(_lower_bound_range)(arr, raw, 0, n); }

static inline isize // -1 = not found
_c_MEMB(_binary_search)(const Self* arr, const _m_raw raw, isize n)
    { return _c_MEMB(_binary_search_range)(arr, raw, 0, n); }

#elif !defined _i_is_list
STC_API isize _c_MEMB(_lower_bound_range)(const Self* self, const _m_raw raw, isize start, isize end);
STC_API isize _c_MEMB(_binary_search_range)(const Self* self, const _m_raw raw, isize start, isize end);

static inline void _c_MEMB(_sort)(Self* self)
    { _c_MEMB(_sort_lowhigh)(self, 0, _c_MEMB(_size)(self) - 1); }

static inline isize // -1 = not found
_c_MEMB(_lower_bound)(const Self* self, const _m_raw raw)
    { return _c_MEMB(_lower_bound_range)(self, raw, 0, _c_MEMB(_size)(self)); }

static inline isize // -1 = not found
_c_MEMB(_binary_search)(const Self* self, const _m_raw raw)
    { return _c_MEMB(_binary_search_range)(self, raw, 0, _c_MEMB(_size)(self)); }
#endif

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement || defined i_static

static void _c_MEMB(_insertsort_lowhigh)(Self* self, isize lo, isize hi) {
    for (isize j = lo, i = lo + 1; i <= hi; j = i, ++i) {
        _m_value x = *i_at(self, i);
        _m_raw rx = i_keytoraw((&x));
        while (j >= 0) {
            _m_raw ry = i_keytoraw(i_at(self, j));
            if (!(i_less((&rx), (&ry)))) break;
            *i_at_mut(self, j + 1) = *i_at(self, j);
            --j;
        }
        *i_at_mut(self, j + 1) = x;
    }
}

STC_DEF void _c_MEMB(_sort_lowhigh)(Self* self, isize lo, isize hi) {
    isize i = lo, j;
    while (lo < hi) {
        _m_raw pivot = i_keytoraw(i_at(self, (isize)(lo + (hi - lo)*7LL/16))), rx;
        j = hi;
        do {
            do { rx = i_keytoraw(i_at(self, i)); } while ((i_less((&rx), (&pivot))) && ++i);
            do { rx = i_keytoraw(i_at(self, j)); } while ((i_less((&pivot), (&rx))) && --j);
            if (i > j) break;
            c_swap(i_at_mut(self, i), i_at_mut(self, j));
            ++i; --j;
        } while (i <= j);

        if (j - lo > hi - i) {
            c_swap(&lo, &i);
            c_swap(&hi, &j);
        }
        if (j - lo > 64) _c_MEMB(_sort_lowhigh)(self, lo, j);
        else if (j > lo) _c_MEMB(_insertsort_lowhigh)(self, lo, j);
        lo = i;
    }
}

#ifndef _i_is_list
STC_DEF isize // -1 = not found
_c_MEMB(_lower_bound_range)(const Self* self, const _m_raw raw, isize start, isize end) {
    isize count = end - start, step = count/2;

    while (count > 0) {
        isize it = start;
        it += step;

        const _m_raw rx = i_keytoraw(i_at(self, it));
        if (i_less((&rx), (&raw))) {
            start = ++it;
            count -= step + 1;
            step = count*3/4;
        } else {
            count = step;
            step = count/4;
        }
    }
    return start == end ? -1 : start;
}

STC_DEF isize // -1 = not found
_c_MEMB(_binary_search_range)(const Self* self, const _m_raw raw, isize start, isize end) {
    isize res = _c_MEMB(_lower_bound_range)(self, raw, start, end);
    if (res != -1) {
        const _m_raw rx = i_keytoraw(i_at(self, res));
        if (i_less((&raw), (&rx))) res = -1;
    }
    return res;
}
#endif // !_i_is_list
#endif // IMPLEMENTATION
#undef i_at
#undef i_at_mut
// ### END_FILE_INCLUDE: sort_prv.h
#endif // _i_has_cmp

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_static)

STC_DEF _m_value*
_c_MEMB(_push_front)(Self* self, _m_value value) {
    isize start = (self->start - 1) & self->capmask;
    if (start == self->end) { // full
        _c_MEMB(_reserve)(self, self->capmask + 3); // => 2x expand
        start = (self->start - 1) & self->capmask;
    }
    _m_value *v = self->cbuf + start;
    self->start = start;
    *v = value;
    return v;
}

STC_DEF void
_c_MEMB(_erase_n)(Self* self, const isize idx, const isize n) {
    const isize len = _c_MEMB(_size)(self);
    for (isize i = idx + n - 1; i >= idx; --i)
        i_keydrop(_c_MEMB(_at_mut)(self, i));
    for (isize i = idx, j = i + n; j < len; ++i, ++j)
        *_c_MEMB(_at_mut)(self, i) = *_c_MEMB(_at)(self, j);
    self->end = (self->end - n) & self->capmask;
}

STC_DEF _m_iter
_c_MEMB(_insert_uninit)(Self* self, const isize idx, const isize n) {
    const isize len = _c_MEMB(_size)(self);
    _m_iter it = {._s=self};
    if (len + n > self->capmask)
        if (!_c_MEMB(_reserve)(self, len + n + 3)) // minimum 2x expand
            return it;
    it.pos = _cbuf_topos(self, idx);
    it.ref = self->cbuf + it.pos;
    self->end = (self->end + n) & self->capmask;

    if (it.pos < self->end) // common case because of reserve policy
        c_memmove(it.ref + n, it.ref, (len - idx)*c_sizeof *it.ref);
    else for (isize i = len - 1, j = i + n; i >= idx; --i, --j)
        *_c_MEMB(_at_mut)(self, j) = *_c_MEMB(_at)(self, i);
    return it;
}

STC_DEF _m_iter
_c_MEMB(_insert_n)(Self* self, const isize idx, const _m_value* arr, const isize n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    for (isize i = idx, j = 0; j < n; ++i, ++j)
        *_c_MEMB(_at_mut)(self, i) = arr[j];
    return it;
}

#if !defined i_no_emplace
STC_DEF _m_iter
_c_MEMB(_emplace_n)(Self* self, const isize idx, const _m_raw* raw, const isize n) {
    _m_iter it = _c_MEMB(_insert_uninit)(self, idx, n);
    for (isize i = idx, j = 0; j < n; ++i, ++j)
        *_c_MEMB(_at_mut)(self, i) = i_keyfrom(raw[j]);
    return it;
}
#endif

#if defined _i_has_eq
STC_DEF _m_iter
_c_MEMB(_find_in)(_m_iter i1, _m_iter i2, _m_raw raw) {
    for (; i1.pos != i2.pos; _c_MEMB(_next)(&i1)) {
        const _m_raw r = i_keytoraw(i1.ref);
        if (i_eq((&raw), (&r)))
            break;
    }
    return i1;
}
#endif
#endif // IMPLEMENTATION
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
// ### END_FILE_INCLUDE: deque.h


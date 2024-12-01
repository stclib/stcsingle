// ### BEGIN_FILE_INCLUDE: list.h

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

#ifndef STC_LIST_H_INCLUDED
#define STC_LIST_H_INCLUDED
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

typedef ptrdiff_t       isize;
#ifndef STC_NO_INT_DEFS
    typedef int8_t      int8;
    typedef uint8_t     uint8;
    typedef int16_t     int16;
    typedef uint16_t    uint16;
    typedef int32_t     int32;
    typedef uint32_t    uint32;
    typedef int64_t     int64;
    typedef uint64_t    uint64;
#endif
#if defined __GNUC__ || defined __clang__
    #define STC_INLINE static inline __attribute((unused))
#else
    #define STC_INLINE static inline
#endif
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR
#define c_NPOS INTPTR_MAX

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

// Select, e.g. for #define i_type A,B then c_SELECT(_c_SEL22, i_type) is B
#define c_SELECT(X, ...) c_EXPAND(X(__VA_ARGS__)) // need c_EXPAND for MSVC
#define _c_SEL21(a, b) a
#define _c_SEL22(a, b) b
#define _c_SEL31(a, b, c) a
#define _c_SEL32(a, b, c) b
#define _c_SEL33(a, b, c) c

#define _i_malloc(T, n)     ((T*)i_malloc((n)*c_sizeof(T)))
#define _i_calloc(T, n)     ((T*)i_calloc((n), c_sizeof(T)))
#ifndef __cplusplus
    #define c_new(T, ...)       ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_literal(T)        (T)
    #define c_make_array(T, ...) ((T[])__VA_ARGS__)
    #define c_make_array2d(T, N, ...) ((T[][N])__VA_ARGS__)
#else
    #include <new>
    #define c_new(T, ...)       new (malloc(sizeof(T))) T(__VA_ARGS__)
    #define c_literal(T)        T
    template<typename T, int M, int N> struct _c_Array { T data[M][N]; };
    #define c_make_array(T, ...) (_c_Array<T, 1, sizeof((T[])__VA_ARGS__)/sizeof(T)>{{__VA_ARGS__}}.data[0])
    #define c_make_array2d(T, N, ...) (_c_Array<T, sizeof((T[][N])__VA_ARGS__)/sizeof(T[N]), N>{__VA_ARGS__}.data)
#endif
#ifndef c_malloc
    #define c_malloc(sz)        malloc(c_i2u_size(sz))
    #define c_calloc(n, sz)     calloc(c_i2u_size(n), c_i2u_size(sz))
    #define c_realloc(ptr, old_sz, sz) realloc(ptr, c_i2u_size(1 ? (sz) : (old_sz)))
    #define c_free(ptr, sz)     do { (void)(sz); free(ptr); } while(0)
#endif
#define c_new_n(T, n)           ((T*)c_calloc(n, c_sizeof(T)))
#define c_delete(T, ptr)        do { T* _tp = ptr; T##_drop(_tp); c_free(_tp, c_sizeof(T)); } while (0)
#define c_delete_n(T, ptr, n)   do { T* _tp = ptr; isize _n = n, _m = _n; \
                                     while (_n--) T##_drop((_tp + _n)); \
                                     c_free(_tp, _m*c_sizeof(T)); } while (0)

#define c_static_assert(expr)   (1 ? 0 : (int)sizeof(int[(expr) ? 1 : -1]))
#if defined STC_NDEBUG || defined NDEBUG
    #define c_assert(expr)      ((void)0)
#else
    #define c_assert(expr)      assert(expr)
#endif
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(Tp, p)     ((Tp)(1 ? (p) : (Tp)0))
#define c_litstrlen(literal)    (c_sizeof("" literal) - 1)
#define c_arraylen(a)           (isize)(sizeof(a)/sizeof 0[a])

// expect signed ints to/from these (use with gcc -Wconversion)
#define c_sizeof                (isize)sizeof
#define c_strlen(s)             (isize)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u_size(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u_size(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u_size(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u_size(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u_size(ilen))
// library internal, but may be useful in user code:
#define c_u2i_size(u)           (isize)(1 ? (u) : (size_t)1) // warns if u is signed
#define c_i2u_size(i)           (size_t)(1 ? (i) : -1)       // warns if i is unsigned
#define c_uless(a, b)           ((size_t)(a) < (size_t)(b))
#define c_safe_cast(T, From, x) ((T)(1 ? (x) : (From){0}))

// x, y are i_keyraw* type, which defaults to i_key*:
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(p)       c_hash_n(p, sizeof *(p))
#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

// non-owning c-string "class"
typedef const char* cstr_raw;
#define cstr_raw_cmp(xp, yp) strcmp(*(xp), *(yp))
#define cstr_raw_eq(xp, yp) (cstr_raw_cmp(xp, yp) == 0)
#define cstr_raw_hash(p) c_hash_str(*(p))
#define cstr_raw_clone(s) (s)
#define cstr_raw_drop(p) ((void)p)

// Control block macros

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it))
#define c_foreach_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _)

#define c_foreach_n(it, C, cnt, n) \
    for (isize it##_index=0, _c_n=n; _c_n; _c_n=0) \
    for (C##_iter it = C##_begin(&cnt); it.ref && it##_index < _c_n; C##_next(&it), ++it##_index)

#define c_foreach_reverse(...) c_MACRO_OVERLOAD(c_foreach_reverse, __VA_ARGS__)
#define c_foreach_reverse_3(it, C, cnt) /* works for stack, vec, queue, deque */ \
    for (C##_iter it = C##_rbegin(&cnt); it.ref; C##_rnext(&it))
#define c_foreach_reverse_4(it, C, start, end) \
    _c_foreach(it, C, start, (end).ref, _r)

#define _c_foreach(it, C, start, endref, rev) /* private */ \
    for (C##_iter it = (start), *_endref = c_safe_cast(C##_iter*, C##_value*, endref) \
         ; it.ref != (C##_value*)_endref; C##rev##next(&it))

#define c_foreach_kv(...) c_MACRO_OVERLOAD(c_foreach_kv, __VA_ARGS__)
#define c_foreach_kv_4(key, val, C, cnt) /* structured binding for maps */ \
    _c_foreach_kv(key, val, C, C##_begin(&cnt), NULL)
#define c_foreach_kv_5(key, val, C, start, end) \
    _c_foreach_kv(key, val, C, start, (end).ref)

#define _c_foreach_kv(key, val, C, start, endref) /* private */ \
    for (const C##_key *key, **_c_k = &key; _c_k; ) \
    for (C##_mapped *val; _c_k; _c_k = NULL) \
    for (C##_iter _it = start, *_endref = c_safe_cast(C##_iter*, C##_value*, endref) ; \
         _it.ref != (C##_value*)_endref && (key = &_it.ref->first, val = &_it.ref->second); \
         C##_next(&_it))

#define c_foritems(it, T, ...) \
    for (struct {T* ref; int size, index;} \
         it = {.ref=c_make_array(T, __VA_ARGS__), .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
         ; it.index < it.size; ++it.ref, ++it.index)

// c_forrange, c_forrange32: python-like int range iteration
#define c_forrange_t(...) c_MACRO_OVERLOAD(c_forrange_t, __VA_ARGS__)
#define c_forrange_t_3(T, i, stop) c_forrange_t_4(T, i, 0, stop)
#define c_forrange_t_4(T, i, start, stop) \
    for (T i=start, _c_end=stop; i < _c_end; ++i)
#define c_forrange_t_5(T, i, start, stop, step) \
    for (T i=start, _c_inc=step, _c_end=(stop) - (_c_inc > 0) \
         ; (_c_inc > 0) == (i <= _c_end); i += _c_inc)

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_t_4(isize, _c_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_t_4(isize, i, 0, stop)
#define c_forrange_3(i, start, stop) c_forrange_t_4(isize, i, start, stop)
#define c_forrange_4(i, start, stop, step) c_forrange_t_5(isize, i, start, stop, step)

#define c_forrange32(...) c_MACRO_OVERLOAD(c_forrange32, __VA_ARGS__)
#define c_forrange32_2(i, stop) c_forrange_t_4(int32_t, i, 0, stop)
#define c_forrange32_3(i, start, stop) c_forrange_t_4(int32_t, i, start, stop)
#define c_forrange32_4(i, start, stop, step) c_forrange_t_5(int32_t, i, start, stop, step)

// deprecated/removed:
#define c_init(C, ...) c_make(C, __VA_ARGS__)                      // [deprecated]
#define c_forlist(...) c_foritems(_VA_ARGS__)                      // [deprecated]
#define c_forpair(...) 'c_forpair not_supported. Use c_foreach_kv' // [removed]

// make container from a literal list, and drop multiple containers of same type
#define c_make(C, ...) \
    C##_with_n(c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// push multiple elements from a literal list into a container
#define c_push(C, cnt, ...) \
    C##_put_n(cnt, c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// drop multiple containers of same type
#define c_drop(C, ...) \
    do { c_foritems (_c_i, C*, {__VA_ARGS__}) C##_drop(*_c_i.ref); } while(0)

// define function with "on-the-fly" defined return type (e.g. variant, optional)
#define c_func(name, args, RIGHTARROW, ...) \
    typedef __VA_ARGS__ name##_result; name##_result name args

// RAII scopes
#define c_defer(...) \
    for (int _c_i = 0; _c_i++ == 0; __VA_ARGS__)

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(init, deinit) \
    for (int _c_i = 0; _c_i == 0; ) for (init; _c_i++ == 0; deinit)
#define c_with_3(init, condition, deinit) \
    for (int _c_i = 0; _c_i == 0; ) for (init; _c_i++ == 0 && (condition); deinit)

// General functions

// substring in substring?
char* c_strnstrn(const char *str, isize slen, const char *needle, isize nlen);

// hashing
size_t c_basehash_n(const void* key, isize len);

STC_INLINE size_t c_hash_n(const void* key, isize len) {
    uint64_t b8; uint32_t b4;
    switch (len) {
        case 8: memcpy(&b8, key, 8); return (size_t)(b8 * 0xc6a4a7935bd1e99d);
        case 4: memcpy(&b4, key, 4); return b4 * (size_t)0xa2ffeb2f01000193;
        default: return c_basehash_n(key, len);
    }
}

STC_INLINE size_t c_hash_str(const char *str)
    { return c_basehash_n(str, c_strlen(str)); }

STC_INLINE size_t _chash_mix(size_t h[], int n) {
    for (int i = 1; i < n; ++i) h[0] += h[0] ^ h[i];
    return h[0];
}
#define c_hash_mix(...) /* non-commutative hash combine! */ \
    _chash_mix(c_make_array(size_t, {__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))

// generic typesafe swap
#define c_swap(xp, yp) do { \
    (void)sizeof((xp) == (yp)); \
    char _tv[sizeof *(xp)]; \
    void *_xp = xp, *_yp = yp; \
    memcpy(_tv, _xp, sizeof _tv); \
    memcpy(_xp, _yp, sizeof _tv); \
    memcpy(_yp, _tv, sizeof _tv); \
} while (0)

// get next power of two
STC_INLINE isize c_next_pow2(isize n) {
    n--;
    n |= n >> 1, n |= n >> 2;
    n |= n >> 4, n |= n >> 8;
    n |= n >> 16;
    #if INTPTR_MAX == INT64_MAX
    n |= n >> 32;
    #endif
    return n + 1;
}
#endif // STC_COMMON_H_INCLUDED

#if !defined STC_COMMON_C_INCLUDED && defined STC_IMPLEMENT
#define STC_COMMON_C_INCLUDED

char* c_strnstrn(const char *str, isize slen, const char *needle, isize nlen) {
    if (nlen == 0) return (char *)str;
    if (nlen > slen) return NULL;
    slen -= nlen;
    do {
        if (*str == *needle && !c_memcmp(str, needle, nlen))
            return (char *)str;
        ++str;
    } while (slen--);
    return NULL;
}

size_t c_basehash_n(const void* key, isize len) {
    size_t block = 0, hash = 0x811c9dc5;
    const uint8_t* msg = (const uint8_t*)key;
    while (len >= c_sizeof(size_t)) {
        memcpy(&block, msg, sizeof(size_t));
        hash = (hash ^ block) * (size_t)0x89bb179901000193;
        msg += c_sizeof(size_t);
        len -= c_sizeof(size_t);
    }
    c_memcpy(&block, msg, len);
    hash = (hash ^ block) * (size_t)0xb0340f4501000193;
    return hash ^ (hash >> 3);
}
#endif // STC_COMMON_C_INCLUDED
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
typedef struct { cstr_value* data; intptr_t size, cap; } cstr_view;
typedef union cstr {
    struct { cstr_value data[ sizeof(cstr_view) ]; } sml;
    struct { cstr_value* data; uintptr_t size, ncap; } lon;
} cstr;

typedef union {
    const cstr_value* ref;
    csview chr; // utf8 character/codepoint
} cstr_iter;

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

#define _c_list_complete_types(SELF, dummy) \
    struct SELF##_node { \
        SELF##_value value; /* must be first! */ \
        struct SELF##_node *next; \
    }

#define _clist_tonode(vp) c_safe_cast(_m_node*, _m_value*, vp)

#define _c_list_insert_entry_after(ref, val) \
    _m_node *entry = _i_malloc(_m_node, 1); entry->value = val; \
    _c_list_insert_after_node(ref, entry)

#define _c_list_insert_after_node(ref, entry) \
    if (ref) entry->next = ref->next, ref->next = entry; \
    else     entry->next = entry
    // +: set self->last based on node

#endif // STC_LIST_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix list_
#endif
// ### BEGIN_FILE_INCLUDE: template.h
// IWYU pragma: private
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

#if defined i_key_arcbox // [deprecated]
  #define i_keypro i_key_arcbox
#elif defined i_key_cstr // [deprecated]
  #define i_keypro cstr
#elif defined i_key_str  // [deprecated]
  #define i_keypro cstr
  #define i_tag str
#endif
#if defined i_val_arcbox // [deprecated]
  #define i_valpro i_val_arcbox
#elif defined i_val_cstr // [deprecated]
  #define i_valpro cstr
#elif defined i_val_str  // [deprecated]
  #define i_valpro cstr
  #define i_tag str
#endif
#ifdef i_TYPE            // [deprecated]
  #define i_type i_TYPE
#endif

#if defined i_type && !(defined i_key || defined i_keyclass || \
                        defined i_keypro || defined i_rawclass)
  #if defined _i_is_map && !defined i_val
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

#if defined i_rawclass
  #if defined _i_is_arc || defined _i_is_box
    #define i_use_cmp
    #define i_use_eq
  #endif
  #if !(defined i_key || defined i_keyclass)
    #define i_key i_rawclass
    #define i_keytoraw c_default_toraw
  #endif
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
#if defined i_keypro
  #define i_keyclass i_keypro
  #define i_rawclass c_JOIN(i_keypro, _raw)
#endif

// Check for keyclass and rawclass, and fill in missing defs.
#if defined i_rawclass
  #define i_keyraw i_rawclass
#elif defined i_keyclass && !defined i_keyraw
  #define i_rawclass i_key
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
#if defined i_rawclass // => i_keyraw
  #if !(defined i_cmp || defined i_less) && (defined i_use_cmp || defined _i_sorted || defined _i_is_pqueue)
    #define i_cmp c_JOIN(i_rawclass, _cmp)
  #endif
  #if !defined i_eq && (defined i_use_eq || defined i_hash || defined _i_is_hash)
    #define i_eq c_JOIN(i_rawclass, _eq)
  #endif
  #if !(defined i_hash || defined i_no_hash)
    #define i_hash c_JOIN(i_rawclass, _hash)
  #endif
#endif

#if !defined i_key
  #error "No i_key defined"
#elif defined i_keyraw && !defined i_keytoraw
  #error "If i_keyraw/i_valraw is defined, i_keytoraw/i_valtoraw must be defined too"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone/i_valclone and i_keydrop/i_valdrop must be defined, if any (unless i_no_clone defined)."
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
#endif
#ifndef i_keytoraw
  #define i_keytoraw c_default_toraw
#endif
#ifndef i_keyclone
  #define i_keyclone c_default_clone
#endif
#ifndef i_keydrop
  #define i_keydrop c_default_drop
#endif

#if defined _i_is_map // ---- process hmap/smap value i_val, ... ----

#if defined i_valpro
  #define i_valclass i_valpro
  #define i_valraw c_JOIN(i_valpro, _raw)
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
#elif defined i_valraw && !defined i_valtoraw
  #error "If i_valraw is defined, i_valtoraw must be defined too"
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
#endif
#ifndef i_valtoraw
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

#define _i_is_list
#ifndef i_is_forward
  _c_DEFTYPES(_c_list_types, Self, i_key);
#endif
_c_DEFTYPES(_c_list_complete_types, Self, dummy);
typedef i_keyraw _m_raw;

STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API _m_value*       _c_MEMB(_push_back)(Self* self, _m_value value);
STC_API _m_value*       _c_MEMB(_push_front)(Self* self, _m_value value);
STC_API _m_iter         _c_MEMB(_insert_at)(Self* self, _m_iter it, _m_value value);
STC_API _m_iter         _c_MEMB(_erase_at)(Self* self, _m_iter it);
STC_API _m_iter         _c_MEMB(_erase_range)(Self* self, _m_iter it1, _m_iter it2);
#if defined _i_has_eq
STC_API _m_iter         _c_MEMB(_find_in)(_m_iter it1, _m_iter it2, _m_raw val);
STC_API isize           _c_MEMB(_remove)(Self* self, _m_raw val);
#endif
#if defined _i_has_cmp
STC_API bool            _c_MEMB(_sort)(Self* self);
#endif
STC_API void            _c_MEMB(_reverse)(Self* self);
STC_API _m_iter         _c_MEMB(_splice)(Self* self, _m_iter it, Self* other);
STC_API Self            _c_MEMB(_split_off)(Self* self, _m_iter it1, _m_iter it2);
STC_API _m_value*       _c_MEMB(_push_back_node)(Self* self, _m_node* node);
STC_API _m_value*       _c_MEMB(_insert_after_node)(Self* self, _m_node* ref, _m_node* node);
STC_API _m_node*        _c_MEMB(_unlink_after_node)(Self* self, _m_node* ref);
STC_API void            _c_MEMB(_erase_after_node)(Self* self, _m_node* ref);
STC_INLINE _m_node*     _c_MEMB(_get_node)(_m_value* pval) { return _clist_tonode(pval); }
STC_INLINE _m_node*     _c_MEMB(_unlink_front_node)(Self* self)
                            { return _c_MEMB(_unlink_after_node)(self, self->last); }
#if !defined i_no_clone
STC_API Self            _c_MEMB(_clone)(Self cx);
STC_INLINE _m_value     _c_MEMB(_value_clone)(_m_value val) { return i_keyclone(val); }

STC_INLINE void
_c_MEMB(_copy)(Self *self, const Self* other) {
    if (self->last == other->last) return;
    _c_MEMB(_drop)(self); *self = _c_MEMB(_clone)(*other);
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE _m_value*    _c_MEMB(_emplace_back)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push_back)(self, i_keyfrom(raw)); }
STC_INLINE _m_value*    _c_MEMB(_emplace_front)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push_front)(self, i_keyfrom(raw)); }
STC_INLINE _m_iter      _c_MEMB(_emplace_at)(Self* self, _m_iter it, _m_raw raw)
                            { return _c_MEMB(_insert_at)(self, it, i_keyfrom(raw)); }
STC_INLINE _m_value*    _c_MEMB(_emplace)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push_back)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

STC_INLINE Self         _c_MEMB(_init)(void) { return c_literal(Self){NULL}; }
STC_INLINE void         _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
                            { while (n--) _c_MEMB(_push_back)(self, i_keyfrom(*raw++)); }
STC_INLINE Self         _c_MEMB(_with_n)(const _m_raw* raw, isize n)
                            { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE bool         _c_MEMB(_reserve)(Self* self, isize n) { (void)(self + n); return true; }
STC_INLINE bool         _c_MEMB(_is_empty)(const Self* self) { return self->last == NULL; }
STC_INLINE void         _c_MEMB(_clear)(Self* self) { _c_MEMB(_drop)(self); }
STC_INLINE _m_value*    _c_MEMB(_push)(Self* self, _m_value value)
                            { return _c_MEMB(_push_back)(self, value); }
STC_INLINE void         _c_MEMB(_pop_front)(Self* self)
                            { c_assert(!_c_MEMB(_is_empty)(self)); _c_MEMB(_erase_after_node)(self, self->last); }
STC_INLINE const _m_value* _c_MEMB(_front)(const Self* self) { return &self->last->next->value; }
STC_INLINE _m_value*       _c_MEMB(_front_mut)(Self* self) { return &self->last->next->value; }
STC_INLINE const _m_value* _c_MEMB(_back)(const Self* self) { return &self->last->value; }
STC_INLINE _m_value*       _c_MEMB(_back_mut)(Self* self) { return &self->last->value; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* pval) { return i_keytoraw(pval); }
STC_INLINE void         _c_MEMB(_value_drop)(_m_value* pval) { i_keydrop(pval); }

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    self->last = NULL;
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

STC_INLINE isize
_c_MEMB(_count)(const Self* self) {
    isize n = 1; const _m_node *node = self->last;
    if (node == NULL) return 0;
    while ((node = node->next) != self->last) ++n;
    return n;
}

STC_INLINE _m_iter
_c_MEMB(_begin)(const Self* self) {
    _m_value* head = self->last ? &self->last->next->value : NULL;
    return c_literal(_m_iter){head, &self->last, self->last};
}

STC_INLINE _m_iter
_c_MEMB(_end)(const Self* self)
    { (void)self; return c_literal(_m_iter){NULL}; }

STC_INLINE void
_c_MEMB(_next)(_m_iter* it) {
    _m_node* node = it->prev = _clist_tonode(it->ref);
    it->ref = (node == *it->_last ? NULL : &node->next->value);
}

STC_INLINE _m_iter
_c_MEMB(_advance)(_m_iter it, size_t n) {
    while (n-- && it.ref) _c_MEMB(_next)(&it);
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_splice_range)(Self* self, _m_iter it,
                       Self* other, _m_iter it1, _m_iter it2) {
    Self tmp = _c_MEMB(_split_off)(other, it1, it2);
    return _c_MEMB(_splice)(self, it, &tmp);
}

#if defined _i_has_eq
STC_INLINE _m_iter
_c_MEMB(_find)(const Self* self, _m_raw val) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), val);
}

STC_INLINE const _m_value*
_c_MEMB(_get)(const Self* self, _m_raw val) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), val).ref;
}

STC_INLINE _m_value*
_c_MEMB(_get_mut)(Self* self, _m_raw val) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), val).ref;
}

STC_INLINE bool _c_MEMB(_eq)(const Self* self, const Self* other) {
    _m_iter i = _c_MEMB(_begin)(self), j = _c_MEMB(_begin)(other);
    for (; i.ref && j.ref; _c_MEMB(_next)(&i), _c_MEMB(_next)(&j)) {
        const _m_raw _rx = i_keytoraw(i.ref), _ry = i_keytoraw(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return !(i.ref || j.ref);
}
#endif

// -------------------------- IMPLEMENTATION -------------------------
#if defined(i_implement) || defined(i_static)

#if !defined i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self lst) {
    Self tmp = {0};
    c_foreach (it, Self, lst)
        _c_MEMB(_push_back)(&tmp, i_keyclone((*it.ref)));
    lst.last = tmp.last;
    return lst;
}
#endif

STC_DEF void
_c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    while (self->last) _c_MEMB(_erase_after_node)(self, self->last);
}

STC_DEF _m_value*
_c_MEMB(_push_back)(Self* self, _m_value value) {
    _c_list_insert_entry_after(self->last, value);
    self->last = entry;
    return &entry->value;
}

STC_DEF _m_value*
_c_MEMB(_push_front)(Self* self, _m_value value) {
    _c_list_insert_entry_after(self->last, value);
    if (self->last == NULL)
        self->last = entry;
    return &entry->value;
}

STC_DEF _m_value*
_c_MEMB(_push_back_node)(Self* self, _m_node* node) {
    _c_list_insert_after_node(self->last, node);
    self->last = node;
    return &node->value;
}

STC_DEF _m_value*
_c_MEMB(_insert_after_node)(Self* self, _m_node* ref, _m_node* node) {
    _c_list_insert_after_node(ref, node);
    if (self->last == NULL)
        self->last = node;
    return &node->value;
}

STC_DEF _m_iter
_c_MEMB(_insert_at)(Self* self, _m_iter it, _m_value value) {
    _m_node* node = it.ref ? it.prev : self->last;
    _c_list_insert_entry_after(node, value);
    if (self->last == NULL || it.ref == NULL) {
        it.prev = self->last ? self->last : entry;
        self->last = entry;
    }
    it.ref = &entry->value;
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_at)(Self* self, _m_iter it) {
    _m_node *node = _clist_tonode(it.ref);
    it.ref = (node == self->last) ? NULL : &node->next->value;
    _c_MEMB(_erase_after_node)(self, it.prev);
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_range)(Self* self, _m_iter it1, _m_iter it2) {
    _m_node *end = it2.ref ? _clist_tonode(it2.ref) : self->last->next;
    if (it1.ref != it2.ref) do {
        _c_MEMB(_erase_after_node)(self, it1.prev);
        if (self->last == NULL) break;
    } while (it1.prev->next != end);
    return it2;
}

STC_DEF void
_c_MEMB(_erase_after_node)(Self* self, _m_node* ref) {
    _m_node* node = _c_MEMB(_unlink_after_node)(self, ref);
    i_keydrop((&node->value));
    i_free(node, c_sizeof *node);
}

STC_DEF _m_node*
_c_MEMB(_unlink_after_node)(Self* self, _m_node* ref) {
    _m_node* node = ref->next, *next = node->next;
    ref->next = next;
    if (node == next)
        self->last = NULL;
    else if (node == self->last)
        self->last = ref;
    return node;
}

STC_DEF void
_c_MEMB(_reverse)(Self* self) {
    Self rev = {NULL};
    while (self->last) {
        _m_node* node = _c_MEMB(_unlink_after_node)(self, self->last);
        _c_MEMB(_insert_after_node)(&rev, rev.last, node);
    }
    *self = rev;
}

STC_DEF _m_iter
_c_MEMB(_splice)(Self* self, _m_iter it, Self* other) {
    if (self->last == NULL)
        self->last = other->last;
    else if (other->last) {
        _m_node *p = it.ref ? it.prev : self->last, *next = p->next;
        it.prev = other->last;
        p->next = it.prev->next;
        it.prev->next = next;
        if (it.ref == NULL) self->last = it.prev;
    }
    other->last = NULL;
    return it;
}

STC_DEF Self
_c_MEMB(_split_off)(Self* self, _m_iter it1, _m_iter it2) {
    Self lst = {NULL};
    if (it1.ref == it2.ref)
        return lst;
    _m_node *p1 = it1.prev,
            *p2 = it2.ref ? it2.prev : self->last;
    p1->next = p2->next;
    p2->next = _clist_tonode(it1.ref);
    if (self->last == p2)
        self->last = (p1 == p2) ? NULL : p1;
    lst.last = p2;
    return lst;
}

#if defined _i_has_eq
STC_DEF _m_iter
_c_MEMB(_find_in)(_m_iter it1, _m_iter it2, _m_raw val) {
    c_foreach (it, Self, it1, it2) {
        _m_raw r = i_keytoraw(it.ref);
        if (i_eq((&r), (&val)))
            return it;
    }
    it2.ref = NULL; return it2;
}

STC_DEF isize
_c_MEMB(_remove)(Self* self, _m_raw val) {
    isize n = 0;
    _m_node *prev = self->last, *node;
    if (prev) do {
        node = prev->next;
        _m_raw r = i_keytoraw((&node->value));
        if (i_eq((&r), (&val))) {
            _c_MEMB(_erase_after_node)(self, prev), ++n;
            if (self->last == NULL) break;
        } else
            prev = node;
    } while (node != self->last);
    return n;
}
#endif

#if defined _i_has_cmp
// ### BEGIN_FILE_INCLUDE: sort_prv.h

// IWYU pragma: private
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

static inline isize // c_NPOS = not found
_c_MEMB(_lower_bound)(const Self* arr, const _m_raw raw, isize n)
    { return _c_MEMB(_lower_bound_range)(arr, raw, 0, n); }

static inline isize // c_NPOS = not found
_c_MEMB(_binary_search)(const Self* arr, const _m_raw raw, isize n)
    { return _c_MEMB(_binary_search_range)(arr, raw, 0, n); }

#elif !defined _i_is_list
STC_API isize _c_MEMB(_lower_bound_range)(const Self* self, const _m_raw raw, isize start, isize end);
STC_API isize _c_MEMB(_binary_search_range)(const Self* self, const _m_raw raw, isize start, isize end);

static inline void _c_MEMB(_sort)(Self* self)
    { _c_MEMB(_sort_lowhigh)(self, 0, _c_MEMB(_size)(self) - 1); }

static inline isize // c_NPOS = not found
_c_MEMB(_lower_bound)(const Self* self, const _m_raw raw)
    { return _c_MEMB(_lower_bound_range)(self, raw, 0, _c_MEMB(_size)(self)); }

static inline isize // c_NPOS = not found
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
STC_DEF isize // c_NPOS = not found
_c_MEMB(_lower_bound_range)(const Self* self, const _m_raw raw, isize start, isize end) {
    isize count = end - start, step = count/2;
    while (count > 0) {
        const _m_raw rx = i_keytoraw(i_at(self, start + step));
        if (i_less((&rx), (&raw))) {
            start += step + 1;
            count -= step + 1;
            step = count*7/8;
        } else {
            count = step;
            step = count/8;
        }
    }
    return start == end ? c_NPOS : start;
}

STC_DEF isize // c_NPOS = not found
_c_MEMB(_binary_search_range)(const Self* self, const _m_raw raw, isize start, isize end) {
    isize res = _c_MEMB(_lower_bound_range)(self, raw, start, end);
    if (res != c_NPOS) {
        const _m_raw rx = i_keytoraw(i_at(self, res));
        if (i_less((&raw), (&rx))) res = c_NPOS;
    }
    return res;
}
#endif // !_i_is_list
#endif // IMPLEMENTATION
#undef i_at
#undef i_at_mut
// ### END_FILE_INCLUDE: sort_prv.h

STC_DEF bool _c_MEMB(_sort)(Self* self) {
    isize len = 0, cap = 0;
    _m_value *arr = NULL, *p = NULL;
    _m_node* keep;
    c_foreach (i, Self, *self) {
        if (len == cap) {
            isize cap_n = cap + cap/2 + 8;
            if ((p = (_m_value *)i_realloc(arr, cap*c_sizeof *p, cap_n*c_sizeof *p)) == NULL)
                goto done;
            arr = p, cap = cap_n;
        }
        arr[len++] = *i.ref;
    }
    keep = self->last;
    self->last = (_m_node *)arr;
    _c_MEMB(_sort_lowhigh)(self, 0, len - 1);
    self->last = keep;
    c_foreach (i, Self, *self)
        *i.ref = *p++;
    done: i_free(arr, cap*c_sizeof *arr);
    return p != NULL;
}
#endif // _i_has_cmp
#endif // i_implement
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
// IWYU pragma: private
#undef i_TYPE       // [deprecated]
#undef i_type
#undef i_class
#undef i_tag
#undef i_opt
#undef i_capacity

#undef i_key
#undef i_keypro     // Replaces the next 3
#undef i_key_str    // [deprecated]
#undef i_key_cstr   // [deprecated]
#undef i_key_arcbox // [deprecated]
#undef i_keyclass
#undef i_rawclass   // define i_keyraw, and bind i_cmp, i_eq, i_hash "class members"
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
#undef i_valpro
#undef i_val_str    // [deprecated]
#undef i_val_cstr   // [deprecated]
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
#undef _i_is_list
// ### END_FILE_INCLUDE: list.h


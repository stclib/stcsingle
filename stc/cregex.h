// ### BEGIN_FILE_INCLUDE: cregex.h
#ifndef STC_CREGEX_H_INCLUDED
#define STC_CREGEX_H_INCLUDED
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

// Macro overloading feature support based on: https://rextester.com/ONP80107
#define c_MACRO_OVERLOAD(name, ...) \
    c_JOIN(c_JOIN0(name,_),c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_JOIN0(a, b) a ## b
#define c_JOIN(a, b) c_JOIN0(a, b)
#define c_EXPAND(...) __VA_ARGS__
// This is the way to make c_NUMARGS work also for MSVC++ and MSVC pre -std:c11
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) c_EXPAND(_c_ARG_N args)
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
#define _c_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,N,...) N

// Select arg, e.g. for #define i_type A,B then c_SELECT(c_ARG_2, i_type) is B
#define c_SELECT(X, ...) c_EXPAND(X(__VA_ARGS__,,)) // need c_EXPAND for MSVC
#define c_ARG_1(a, ...) a
#define c_ARG_2(a, b, ...) b
#define c_ARG_3(a, b, c, ...) c

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

#define declare_arc(C, VAL) _c_arc_types(C, VAL)
#define declare_box(C, VAL) _c_box_types(C, VAL)
#define declare_deq(C, VAL) _c_deque_types(C, VAL)
#define declare_list(C, VAL) _c_list_types(C, VAL)
#define declare_hmap(C, KEY, VAL) _c_htable_types(C, KEY, VAL, c_true, c_false)
#define declare_hset(C, KEY) _c_htable_types(C, cset, KEY, KEY, c_false, c_true)
#define declare_smap(C, KEY, VAL) _c_aatree_types(C, KEY, VAL, c_true, c_false)
#define declare_sset(C, KEY) _c_aatree_types(C, KEY, KEY, c_false, c_true)
#define declare_stack(C, VAL) _c_stack_types(C, VAL)
#define declare_pqueue(C, VAL) _c_pqueue_types(C, VAL)
#define declare_queue(C, VAL) _c_deque_types(C, VAL)
#define declare_vec(C, VAL) _c_vec_types(C, VAL)

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
    csview chr; // utf8 character/codepoint
    const cstr_value* ref;
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

enum {
    CREG_DEFAULT = 0,

    /* compile-flags */
    CREG_DOTALL = 1<<0,    /* dot matches newline too */
    CREG_ICASE = 1<<1,     /* ignore case */

    /* match-flags */
    CREG_FULLMATCH = 1<<2, /* like start-, end-of-line anchors were in pattern: "^ ... $" */
    CREG_NEXT = 1<<3,      /* use end of previous match[0] as start of input */
    CREG_STARTEND = 1<<4,  /* use match[0] as start+end of input */

    /* replace-flags */
    CREG_STRIP = 1<<5,     /* only keep the matched strings, strip rest */

    /* limits */
    CREG_MAX_CLASSES = 16,
    CREG_MAX_CAPTURES = 32,
};

typedef enum {
    CREG_OK = 0,
    CREG_NOMATCH = -1,
    CREG_MATCHERROR = -2,
    CREG_OUTOFMEMORY = -3,
    CREG_UNMATCHEDLEFTPARENTHESIS = -4,
    CREG_UNMATCHEDRIGHTPARENTHESIS = -5,
    CREG_TOOMANYSUBEXPRESSIONS = -6,
    CREG_TOOMANYCHARACTERCLASSES = -7,
    CREG_MALFORMEDCHARACTERCLASS = -8,
    CREG_MISSINGOPERAND = -9,
    CREG_UNKNOWNOPERATOR = -10,
    CREG_OPERANDSTACKOVERFLOW = -11,
    CREG_OPERATORSTACKOVERFLOW = -12,
    CREG_OPERATORSTACKUNDERFLOW = -13,
} cregex_result;

typedef struct {
    struct _Reprog* prog;
    int error;
} cregex;

typedef struct {
    const cregex* regex;
    csview input;
    csview match[CREG_MAX_CAPTURES];
} cregex_iter;

#define c_formatch(it, re, str) \
    for (cregex_iter it = {.regex=re, .input={str}, .match={{0}}}; \
         cregex_match_next(it.regex, it.input.buf, it.match) == CREG_OK && it.match[0].size; )

#define c_formatch_sv(it, re, strview) \
    for (cregex_iter it = {.regex=re, .input=strview, .match={{0}}}; \
         cregex_match_next_sv(it.regex, it.input, it.match) == CREG_OK && it.match[0].size; )

/* compile a regex from a pattern. return CREG_OK, or negative error code on failure. */
int cregex_compile_pro(cregex *re, const char* pattern, int cflags);

STC_INLINE int cregex_compile(cregex* re, const char* pattern)
    { return cregex_compile_pro(re, pattern, CREG_DEFAULT); }

/* construct and return a regex from a pattern. return CREG_OK, or negative error code on failure. */
STC_INLINE cregex cregex_make(const char* pattern, int cflags) {
    cregex re = {0};
    cregex_compile_pro(&re, pattern, cflags);
    return re;
}
STC_INLINE cregex cregex_from(const char* pattern)
    { return cregex_make(pattern, CREG_DEFAULT); }


/* number of capture groups in a regex pattern, excluding the full match capture (0) */
int cregex_captures(const cregex* re);


/* return CREG_OK, CREG_NOMATCH or CREG_MATCHERROR. */
int cregex_match_pro(const cregex* re, const char* input, csview match[], int mflags);

STC_INLINE int cregex_match_sv(const cregex* re, csview input, csview match[]) {
    match[0] = input;
    return cregex_match_pro(re, input.buf, match, CREG_STARTEND);
}

STC_INLINE int cregex_match(const cregex* re, const char* input, csview match[])
    { return cregex_match_pro(re, input, match, CREG_DEFAULT); }

STC_INLINE bool cregex_is_match(const cregex* re, const char* input)
    { return cregex_match_pro(re, input, NULL, CREG_DEFAULT) == CREG_OK; }

STC_INLINE int cregex_match_next_sv(const cregex* re, csview input, csview match[]) {
    if (match[0].buf) {
        match[0].buf += match[0].size;
        match[0].size = input.buf + input.size - match[0].buf;
    }
    return cregex_match_pro(re, input.buf, match, CREG_STARTEND);
}

STC_INLINE int cregex_match_next(const cregex* re, const char* input, csview match[]) {
    return cregex_match_pro(re, input, match, CREG_NEXT);
}

/* match + compile RE pattern */
int cregex_match_aio(const char* pattern, const char* input, csview match[]);


/* replace csview input with replace using regular expression pattern */
cstr cregex_replace_pro(const cregex* re, csview input, const char* replace, int count,
                        bool(*transform)(int group, csview match, cstr* result), int rflags);

/* replace const char* input with replace using regular expression pattern */
STC_INLINE cstr cregex_replace(const cregex* re, const char* input, const char* replace) {
    csview sv = {input, c_strlen(input)};
    return cregex_replace_pro(re, sv, replace, INT32_MAX, NULL, CREG_DEFAULT);
}


/* replace + compile RE pattern, and extra arguments */
cstr cregex_replace_aio_pro(const char* pattern, csview input, const char* replace, int count,
                            bool(*transform)(int group, csview match, cstr* result), int crflags);

STC_INLINE cstr cregex_replace_aio(const char* pattern, const char* input, const char* replace) {
    csview sv = {input, c_strlen(input)};
    return cregex_replace_aio_pro(pattern, sv, replace, INT32_MAX, NULL, CREG_DEFAULT);
}

/* destroy regex */
void cregex_drop(cregex* re);

#endif // STC_CREGEX_H_INCLUDED

#if defined STC_IMPLEMENT || defined i_implement || defined i_import
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
  #pragma GCC diagnostic ignored "-Wclobbered"
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough=3"
  #pragma GCC diagnostic ignored "-Wstringop-overflow="
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
// ### END_FILE_INCLUDE: linkage.h
// ### BEGIN_FILE_INCLUDE: cregex_prv.c
#ifndef STC_CREGEX_PRV_C_INCLUDED
#define STC_CREGEX_PRV_C_INCLUDED

#include <setjmp.h>
// ### BEGIN_FILE_INCLUDE: utf8_prv.h
// IWYU pragma: private, include "stc/utf8.h"
#ifndef STC_UTF8_PRV_H_INCLUDED
#define STC_UTF8_PRV_H_INCLUDED

// The following functions assume valid utf8 strings:

/* number of bytes in the utf8 codepoint from s */
STC_INLINE int utf8_chr_size(const char *s) {
    unsigned b = (uint8_t)*s;
    if (b < 0x80) return 1;
    /*if (b < 0xC2) return 0;*/
    if (b < 0xE0) return 2;
    if (b < 0xF0) return 3;
    /*if (b < 0xF5)*/ return 4;
    /*return 0;*/
}

/* number of codepoints in the utf8 string s */
STC_INLINE isize utf8_count(const char *s) {
    isize size = 0;
    while (*s)
        size += (*++s & 0xC0) != 0x80;
    return size;
}

STC_INLINE isize utf8_count_n(const char *s, isize nbytes) {
    isize size = 0;
    while ((nbytes-- != 0) & (*s != 0)) {
        size += (*++s & 0xC0) != 0x80;
    }
    return size;
}

STC_INLINE const char* utf8_at(const char *s, isize u8pos) {
    while ((u8pos > 0) & (*s != 0))
        u8pos -= (*++s & 0xC0) != 0x80;
    return s;
}

STC_INLINE const char* utf8_offset(const char* s, isize u8pos) {
    int inc = 1;
    if (u8pos < 0) u8pos = -u8pos, inc = -1;
    while (u8pos && *s)
        u8pos -= (*(s += inc) & 0xC0) != 0x80;
    return s;
}

STC_INLINE isize utf8_to_index(const char* s, isize u8pos)
    { return utf8_at(s, u8pos) - s; }

STC_INLINE csview utf8_subview(const char *s, isize u8pos, isize u8len) {
    csview span;
    span.buf = utf8_at(s, u8pos);
    span.size = utf8_to_index(span.buf, u8len);
    return span;
}

// ------------------------------------------------------
// The following requires linking with utf8 symbols.
// To call them, either define i_import before including
// one of cstr, csview, zsview, or link with src/libstc.o.

extern bool     utf8_valid_n(const char* s, isize nbytes);
extern int      utf8_encode(char *out, uint32_t c);
extern int      utf8_icompare(const csview s1, const csview s2);
extern uint32_t utf8_peek_at(const char* s, isize u8offset);
extern uint32_t utf8_casefold(uint32_t c);
extern uint32_t utf8_tolower(uint32_t c);
extern uint32_t utf8_toupper(uint32_t c);

STC_INLINE bool utf8_isupper(uint32_t c)
    { return utf8_tolower(c) != c; }

STC_INLINE bool utf8_islower(uint32_t c)
    { return utf8_toupper(c) != c; }


/* decode next utf8 codepoint. https://bjoern.hoehrmann.de/utf-8/decoder/dfa */
typedef struct { uint32_t state, codep; } utf8_decode_t;
extern const uint8_t utf8_dtab[]; /* utf8code.c */

STC_INLINE uint32_t utf8_decode(utf8_decode_t* d, const uint32_t byte) {
    const uint32_t type = utf8_dtab[byte];
    d->codep = d->state ? (byte & 0x3fu) | (d->codep << 6)
                        : (0xffU >> type) & byte;
    return d->state = utf8_dtab[256 + d->state + type];
}

STC_INLINE uint32_t utf8_peek(const char* s) {
    utf8_decode_t d = {.state=0};
    do { utf8_decode(&d, (uint8_t)*s++); } while (d.state);
    return d.codep;
}

/* case-insensitive utf8 string comparison */
STC_INLINE int utf8_icmp(const char* s1, const char* s2) {
    return utf8_icompare(c_sv(s1, INTPTR_MAX), c_sv(s2, INTPTR_MAX));
}

STC_INLINE bool utf8_valid(const char* s) {
    return utf8_valid_n(s, INTPTR_MAX);
}

#endif // STC_UTF8_PRV_H_INCLUDED
// ### END_FILE_INCLUDE: utf8_prv.h
// ### BEGIN_FILE_INCLUDE: cstr_prv.h
// IWYU pragma: private, include "stc/cstr.h"
#ifndef STC_CSTR_PRV_H_INCLUDED
#define STC_CSTR_PRV_H_INCLUDED

#include <stdio.h> /* FILE*, vsnprintf */
#include <stdlib.h> /* malloc */
#include <stddef.h> /* size_t */
/**************************** PRIVATE API **********************************/

#if defined __GNUC__ && !defined __clang__
  // linkage.h already does diagnostic push
  // Warns wrongfully on -O3 on cstr_assign(&str, "literal longer than 23 ...");
  #pragma GCC diagnostic ignored "-Warray-bounds"
#endif

enum  { cstr_s_last = sizeof(cstr_view) - 1,
        cstr_s_cap = cstr_s_last - 1 };
#define cstr_s_size(s)          ((isize)(s)->sml.data[cstr_s_last])
#define cstr_s_set_size(s, len) ((s)->sml.data[len] = 0, (s)->sml.data[cstr_s_last] = (char)(len))
#define cstr_s_data(s)          (s)->sml.data

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define byte_rotl_(x, b)       ((x) << (b)*8 | (x) >> (sizeof(x) - (b))*8)
    #define cstr_l_cap(s)          (isize)(~byte_rotl_((s)->lon.ncap, sizeof((s)->lon.ncap) - 1))
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~byte_rotl_((size_t)(cap), 1))
#else
    #define cstr_l_cap(s)          (isize)(~(s)->lon.ncap)
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~(size_t)(cap))
#endif
#define cstr_l_size(s)          (isize)((s)->lon.size)
#define cstr_l_set_size(s, len) ((s)->lon.data[(s)->lon.size = (size_t)(len)] = 0)
#define cstr_l_data(s)          (s)->lon.data
#define cstr_l_drop(s)          i_free((s)->lon.data, cstr_l_cap(s) + 1)

#define cstr_is_long(s)         (((s)->sml.data[cstr_s_last] & 128) != 0)
extern  char* _cstr_init(cstr* self, isize len, isize cap);
extern  char* _cstr_internal_move(cstr* self, isize pos1, isize pos2);

/**************************** PUBLIC API **********************************/

#define             cstr_init() (c_literal(cstr){0})
#define             cstr_lit(literal) cstr_with_n(literal, c_litstrlen(literal))
#define             cstr_join_vec(sep, vecp) \
                        cstr_join_array_s(sep, (vecp)->data, (vecp)->size)
#define             cstr_join_s(sep, ...) \
                        cstr_join_array_s(sep, c_make_array(cstr, __VA_ARGS__), c_NUMARGS(__VA_ARGS__))

extern  cstr        cstr_from_replace(csview sv, csview search, csview repl, int32_t count);
extern  cstr        cstr_from_fmt(const char* fmt, ...);

extern  char*       cstr_reserve(cstr* self, isize cap);
extern  void        cstr_shrink_to_fit(cstr* self);
extern  char*       cstr_resize(cstr* self, isize size, char value);
extern  isize       cstr_find_at(const cstr* self, isize pos, const char* search);
extern  isize       cstr_find_sv(const cstr* self, csview search);
extern  char*       cstr_assign_n(cstr* self, const char* str, isize len);
STC_INLINE char*    cstr_append(cstr* self, const char* str);
STC_INLINE char*    cstr_append_s(cstr* self, cstr s);
extern  char*       cstr_append_n(cstr* self, const char* str, isize len);
extern  isize       cstr_append_fmt(cstr* self, const char* fmt, ...);
extern  char*       cstr_append_uninit(cstr *self, isize len);
extern  bool        cstr_getdelim(cstr *self, int delim, FILE *fp);
extern  void        cstr_erase(cstr* self, isize pos, isize len);
extern  isize       cstr_printf(cstr* self, const char* fmt, ...);
extern  size_t      cstr_hash(const cstr *self);
extern  bool        cstr_u8_valid(const cstr* self);
extern  void        cstr_u8_erase(cstr* self, isize u8pos, isize u8len);

STC_INLINE cstr_view cstr_getview(cstr* s) {
    return cstr_is_long(s) ? c_literal(cstr_view){s->lon.data, cstr_l_size(s), cstr_l_cap(s)}
                           : c_literal(cstr_view){s->sml.data, cstr_s_size(s), cstr_s_cap};
}
STC_INLINE zsview cstr_zv(const cstr* s) {
    return cstr_is_long(s) ? c_literal(zsview){s->lon.data, cstr_l_size(s)}
                           : c_literal(zsview){s->sml.data, cstr_s_size(s)};
}
STC_INLINE csview cstr_sv(const cstr* s) {
    return cstr_is_long(s) ? c_literal(csview){s->lon.data, cstr_l_size(s)}
                           : c_literal(csview){s->sml.data, cstr_s_size(s)};
}

STC_INLINE cstr cstr_with_n(const char* str, const isize len) {
    cstr s;
    c_memcpy(_cstr_init(&s, len, len), str, len);
    return s;
}

STC_INLINE cstr cstr_from(const char* str)
    { return cstr_with_n(str, c_strlen(str)); }

STC_INLINE cstr cstr_join_array(const char* sep, const char* arr[], isize n) {
    cstr s = {0}; const char* _sep = "";
    while (n--) { cstr_append(&s, _sep); cstr_append(&s, *arr++); _sep = sep; }
    return s;
}
STC_INLINE cstr cstr_join_array_s(const char* sep, cstr arr[], isize n) {
    cstr s = {0}; const char* _sep = "";
    while (n--) { cstr_append(&s, _sep); cstr_append_s(&s, *arr++); _sep = sep; }
    return s;
}

STC_INLINE cstr cstr_from_sv(csview sv)
    { return cstr_with_n(sv.buf, sv.size); }

STC_INLINE cstr cstr_from_zv(zsview zv)
    { return cstr_with_n(zv.str, zv.size); }

STC_INLINE cstr cstr_with_size(const isize size, const char value) {
    cstr s;
    c_memset(_cstr_init(&s, size, size), value, size);
    return s;
}

STC_INLINE cstr cstr_with_capacity(const isize cap) {
    cstr s;
    _cstr_init(&s, 0, cap);
    return s;
}

STC_INLINE cstr* cstr_take(cstr* self, const cstr s) {
    if (cstr_is_long(self) && self->lon.data != s.lon.data)
        cstr_l_drop(self);
    *self = s;
    return self;
}

STC_INLINE cstr cstr_move(cstr* self) {
    cstr tmp = *self;
    *self = cstr_init();
    return tmp;
}

STC_INLINE cstr cstr_clone(cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_with_n(sv.buf, sv.size);
}

STC_INLINE void cstr_drop(cstr* self) {
    if (cstr_is_long(self))
        cstr_l_drop(self);
}

#define SSO_CALL(s, call) (cstr_is_long(s) ? cstr_l_##call : cstr_s_##call)

STC_INLINE void _cstr_set_size(cstr* self, isize len)
    { SSO_CALL(self, set_size(self, len)); }

STC_INLINE void cstr_clear(cstr* self)
    { _cstr_set_size(self, 0); }

STC_INLINE char* cstr_data(cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_str(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_toraw(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE isize cstr_size(const cstr* self)
    { return SSO_CALL(self, size(self)); }

STC_INLINE bool cstr_is_empty(const cstr* self)
    { return cstr_size(self) == 0; }

STC_INLINE isize cstr_capacity(const cstr* self)
    { return cstr_is_long(self) ? cstr_l_cap(self) : (isize)cstr_s_cap; }

STC_INLINE isize cstr_to_index(const cstr* self, cstr_iter it)
    { return it.ref - cstr_str(self); }

STC_INLINE cstr cstr_from_s(cstr s, isize pos, isize len)
    { return cstr_with_n(cstr_str(&s) + pos, len); }

STC_INLINE csview cstr_subview(const cstr* self, isize pos, isize len) {
    csview sv = cstr_sv(self);
    c_assert(((size_t)pos <= (size_t)sv.size) & (len >= 0));
    if (pos + len > sv.size) len = sv.size - pos;
    return c_literal(csview){sv.buf + pos, len};
}

STC_INLINE zsview cstr_tail(const cstr* self, isize len) {
    c_assert(len >= 0);
    csview sv = cstr_sv(self);
    if (len > sv.size) len = sv.size;
    return c_literal(zsview){&sv.buf[sv.size - len], len};
}

// BEGIN utf8 functions =====

STC_INLINE cstr cstr_u8_from(const char* str, isize u8pos, isize u8len)
    { str = utf8_at(str, u8pos); return cstr_with_n(str, utf8_to_index(str, u8len)); }

STC_INLINE isize cstr_u8_size(const cstr* self)
    { return utf8_count(cstr_str(self)); }

STC_INLINE isize cstr_u8_to_index(const cstr* self, isize u8pos)
    { return utf8_to_index(cstr_str(self), u8pos); }

STC_INLINE zsview cstr_u8_tail(const cstr* self, isize u8len) {
    csview sv = cstr_sv(self);
    const char* p = &sv.buf[sv.size];
    while (u8len && p != sv.buf)
        u8len -= (*--p & 0xC0) != 0x80;
    return c_literal(zsview){p, sv.size - (p - sv.buf)};
}

STC_INLINE csview cstr_u8_subview(const cstr* self, isize u8pos, isize u8len)
    { return utf8_subview(cstr_str(self), u8pos, u8len); }

STC_INLINE cstr_iter cstr_u8_at(const cstr* self, isize u8pos) {
    csview sv;
    sv.buf = utf8_at(cstr_str(self), u8pos);
    sv.size = utf8_chr_size(sv.buf);
    c_assert(sv.size);
    return c_literal(cstr_iter){.chr = sv};
}

// utf8 iterator

STC_INLINE cstr_iter cstr_begin(const cstr* self) {
    csview sv = cstr_sv(self);
    cstr_iter it = {.chr = {sv.buf, utf8_chr_size(sv.buf)}};
    return it;
}
STC_INLINE cstr_iter cstr_end(const cstr* self) {
    (void)self; cstr_iter it = {0}; return it;
}
STC_INLINE void cstr_next(cstr_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (*it->ref == '\0') it->ref = NULL;
}

STC_INLINE cstr_iter cstr_advance(cstr_iter it, isize u8pos) {
    it.ref = utf8_offset(it.ref, u8pos);
    it.chr.size = utf8_chr_size(it.ref);
    if (*it.ref == '\0') it.ref = NULL;
    return it;
}

// utf8 case conversion: requires `#define i_import` before including cstr.h in one TU.
extern  cstr cstr_tocase_sv(csview sv, int k);

STC_INLINE cstr cstr_casefold_sv(csview sv)
    { return cstr_tocase_sv(sv, 0); }

STC_INLINE cstr cstr_tolower_sv(csview sv)
    { return cstr_tocase_sv(sv, 1); }

STC_INLINE cstr cstr_toupper_sv(csview sv)
    { return cstr_tocase_sv(sv, 2); }

STC_INLINE cstr cstr_tolower(const char* str)
    { return cstr_tolower_sv(c_sv(str, c_strlen(str))); }

STC_INLINE cstr cstr_toupper(const char* str)
    { return cstr_toupper_sv(c_sv(str, c_strlen(str))); }

STC_INLINE void cstr_lowercase(cstr* self)
    { cstr_take(self, cstr_tolower_sv(cstr_sv(self))); }

STC_INLINE void cstr_uppercase(cstr* self)
    { cstr_take(self, cstr_toupper_sv(cstr_sv(self))); }

STC_INLINE bool cstr_istarts_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self);
    isize len = c_strlen(sub);
    return len <= sv.size && !utf8_icompare(sv, c_sv(sub, len));
}

STC_INLINE bool cstr_iends_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self);
    isize n = c_strlen(sub);
    return n <= sv.size && !utf8_icmp(sv.buf + sv.size - n, sub);
}

STC_INLINE int cstr_icmp(const cstr* s1, const cstr* s2)
    { return utf8_icmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE bool cstr_ieq(const cstr* s1, const cstr* s2) {
    csview x = cstr_sv(s1), y = cstr_sv(s2);
    return x.size == y.size && !utf8_icompare(x, y);
}

STC_INLINE bool cstr_iequals(const cstr* self, const char* str)
    { return !utf8_icmp(cstr_str(self), str); }

// END utf8 =====

STC_INLINE int cstr_cmp(const cstr* s1, const cstr* s2)
    { return strcmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE bool cstr_eq(const cstr* s1, const cstr* s2) {
    csview x = cstr_sv(s1), y = cstr_sv(s2);
    return x.size == y.size && !c_memcmp(x.buf, y.buf, x.size);
}

STC_INLINE bool cstr_equals(const cstr* self, const char* str)
    { return !strcmp(cstr_str(self), str); }

STC_INLINE bool cstr_equals_sv(const cstr* self, csview sv)
    { return sv.size == cstr_size(self) && !c_memcmp(cstr_str(self), sv.buf, sv.size); }

STC_INLINE isize cstr_find(const cstr* self, const char* search) {
    const char *str = cstr_str(self), *res = strstr((char*)str, search);
    return res ? (res - str) : c_NPOS;
}

STC_INLINE bool cstr_contains(const cstr* self, const char* search)
    { return strstr((char*)cstr_str(self), search) != NULL; }

STC_INLINE bool cstr_contains_sv(const cstr* self, csview search)
    { return cstr_find_sv(self, search) != c_NPOS; }


STC_INLINE bool cstr_starts_with_sv(const cstr* self, csview sub) {
    if (sub.size > cstr_size(self)) return false;
    return !c_memcmp(cstr_str(self), sub.buf, sub.size);
}

STC_INLINE bool cstr_starts_with(const cstr* self, const char* sub) {
    const char* str = cstr_str(self);
    while (*sub && *str == *sub) ++str, ++sub;
    return !*sub;
}

STC_INLINE bool cstr_ends_with_sv(const cstr* self, csview sub) {
    csview sv = cstr_sv(self);
    if (sub.size > sv.size) return false;
    return !c_memcmp(sv.buf + sv.size - sub.size, sub.buf, sub.size);
}

STC_INLINE bool cstr_ends_with(const cstr* self, const char* sub)
    { return cstr_ends_with_sv(self, c_sv(sub, c_strlen(sub))); }

STC_INLINE char* cstr_assign(cstr* self, const char* str)
    { return cstr_assign_n(self, str, c_strlen(str)); }

STC_INLINE char* cstr_assign_sv(cstr* self, csview sv)
    { return cstr_assign_n(self, sv.buf, sv.size); }

STC_INLINE char* cstr_copy(cstr* self, cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_assign_n(self, sv.buf, sv.size);
}


STC_INLINE char* cstr_push(cstr* self, const char* chr)
    { return cstr_append_n(self, chr, utf8_chr_size(chr)); }

STC_INLINE void cstr_pop(cstr* self) {
    csview sv = cstr_sv(self);
    const char* s = sv.buf + sv.size;
    while ((*--s & 0xC0) == 0x80) ;
    _cstr_set_size(self, (s - sv.buf));
}

STC_INLINE char* cstr_append(cstr* self, const char* str)
    { return cstr_append_n(self, str, c_strlen(str)); }

STC_INLINE char* cstr_append_sv(cstr* self, csview sv)
    { return cstr_append_n(self, sv.buf, sv.size); }

STC_INLINE char* cstr_append_s(cstr* self, cstr s)
    { return cstr_append_sv(self, cstr_sv(&s)); }


STC_INLINE void cstr_replace_sv(cstr* self, csview search, csview repl, int32_t count)
    { cstr_take(self, cstr_from_replace(cstr_sv(self), search, repl, count)); }

STC_INLINE void cstr_replace_nfirst(cstr* self, const char* search, const char* repl, int32_t count)
    { cstr_replace_sv(self, c_sv(search, c_strlen(search)), c_sv(repl, c_strlen(repl)), count); }

STC_INLINE void cstr_replace(cstr* self, const char* search, const char* repl)
    { cstr_replace_nfirst(self, search, repl, INT32_MAX); }


STC_INLINE void cstr_replace_at_sv(cstr* self, isize pos, isize len, const csview repl) {
    char* d = _cstr_internal_move(self, pos + len, pos + repl.size);
    c_memcpy(d + pos, repl.buf, repl.size);
}
STC_INLINE void cstr_replace_at(cstr* self, isize pos, isize len, const char* repl)
    { cstr_replace_at_sv(self, pos, len, c_sv(repl, c_strlen(repl))); }

STC_INLINE void cstr_u8_replace(cstr* self, isize u8pos, isize u8len, const char* repl) {
    const char* s = cstr_str(self); csview span = utf8_subview(s, u8pos, u8len);
    cstr_replace_at(self, span.buf - s, span.size, repl);
}


STC_INLINE void cstr_insert_sv(cstr* self, isize pos, csview sv)
    { cstr_replace_at_sv(self, pos, 0, sv); }

STC_INLINE void cstr_insert(cstr* self, isize pos, const char* str)
    { cstr_replace_at_sv(self, pos, 0, c_sv(str, c_strlen(str))); }

STC_INLINE void cstr_u8_insert(cstr* self, isize u8pos, const char* str)
    { cstr_insert(self, utf8_to_index(cstr_str(self), u8pos), str); }

STC_INLINE bool cstr_getline(cstr *self, FILE *fp)
    { return cstr_getdelim(self, '\n', fp); }

#endif // STC_CSTR_PRV_H_INCLUDED
// ### END_FILE_INCLUDE: cstr_prv.h
// ### BEGIN_FILE_INCLUDE: ucd_prv.c
#ifndef STC_UCD_PRV_C_INCLUDED
#define STC_UCD_PRV_C_INCLUDED

#include <ctype.h>

// ------------------------------------------------------
// The following requires linking with utf8 symbols.
// To call them, either define i_import before including
// one of cstr, csview, zsview, or link with src/libstc.o.

enum {
    U8G_Cc, U8G_Lt, U8G_Nd, U8G_Nl,
    U8G_Pc, U8G_Pd, U8G_Pf, U8G_Pi,
    U8G_Sc, U8G_Zl, U8G_Zp, U8G_Zs,
    U8G_Arabic, U8G_Bengali, U8G_Cyrillic,
    U8G_Devanagari, U8G_Georgian, U8G_Greek,
    U8G_Han, U8G_Hiragana, U8G_Katakana,
    U8G_Latin, U8G_Thai,
    U8G_SIZE
};

static bool utf8_isgroup(int group, uint32_t c);

static bool utf8_isalpha(uint32_t c) {
    static int16_t groups[] = {U8G_Latin, U8G_Nl, U8G_Cyrillic, U8G_Han, U8G_Devanagari,
                               U8G_Arabic, U8G_Bengali, U8G_Hiragana, U8G_Katakana,
                               U8G_Thai, U8G_Greek, U8G_Georgian};
    if (c < 128) return isalpha((int)c) != 0;
    for (int j=0; j < (int)(sizeof groups/sizeof groups[0]); ++j)
        if (utf8_isgroup(groups[j], c))
            return true;
    return false;
}

static bool utf8_iscased(uint32_t c) {
    if (c < 128) return isalpha((int)c) != 0;
    return utf8_islower(c) || utf8_isupper(c) ||
           utf8_isgroup(U8G_Lt, c);
}

static bool utf8_isalnum(uint32_t c) {
    if (c < 128) return isalnum((int)c) != 0;
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c);
}

static bool utf8_isword(uint32_t c) {
    if (c < 128) return (isalnum((int)c) != 0) | (c == '_');
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c) ||
           utf8_isgroup(U8G_Pc, c);
}

static bool utf8_isblank(uint32_t c) {
    if (c < 128) return (c == ' ') | (c == '\t');
    return utf8_isgroup(U8G_Zs, c);
}

static bool utf8_isspace(uint32_t c) {
    if (c < 128) return isspace((int)c) != 0;
    return ((c == 8232) | (c == 8233)) || utf8_isgroup(U8G_Zs, c);
}

/* The tables below are extracted from the RE2 library */
typedef struct {
  uint16_t lo;
  uint16_t hi;
} URange16;

static const URange16 Cc_range16[] = { // Control
    { 0, 31 },
    { 127, 159 },
};

static const URange16 Lt_range16[] = { // Title case
    { 453, 453 },
    { 456, 456 },
    { 459, 459 },
    { 498, 498 },
    { 8072, 8079 },
    { 8088, 8095 },
    { 8104, 8111 },
    { 8124, 8124 },
    { 8140, 8140 },
    { 8188, 8188 },
};

static const URange16 Nd_range16[] = { // Decimal number
    { 48, 57 },
    { 1632, 1641 },
    { 1776, 1785 },
    { 1984, 1993 },
    { 2406, 2415 },
    { 2534, 2543 },
    { 2662, 2671 },
    { 2790, 2799 },
    { 2918, 2927 },
    { 3046, 3055 },
    { 3174, 3183 },
    { 3302, 3311 },
    { 3430, 3439 },
    { 3558, 3567 },
    { 3664, 3673 },
    { 3792, 3801 },
    { 3872, 3881 },
    { 4160, 4169 },
    { 4240, 4249 },
    { 6112, 6121 },
    { 6160, 6169 },
    { 6470, 6479 },
    { 6608, 6617 },
    { 6784, 6793 },
    { 6800, 6809 },
    { 6992, 7001 },
    { 7088, 7097 },
    { 7232, 7241 },
    { 7248, 7257 },
    { 42528, 42537 },
    { 43216, 43225 },
    { 43264, 43273 },
    { 43472, 43481 },
    { 43504, 43513 },
    { 43600, 43609 },
    { 44016, 44025 },
    { 65296, 65305 },
};

static const URange16 Nl_range16[] = { // Number letter
    { 5870, 5872 },
    { 8544, 8578 },
    { 8581, 8584 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12346 },
    { 42726, 42735 },
};

static const URange16 Pc_range16[] = { // Connector punctuation
    { 95, 95 },
    { 8255, 8256 },
    { 8276, 8276 },
    { 65075, 65076 },
    { 65101, 65103 },
    { 65343, 65343 },
};

static const URange16 Pd_range16[] = { // Dash punctuation
    { 45, 45 },
    { 1418, 1418 },
    { 1470, 1470 },
    { 5120, 5120 },
    { 6150, 6150 },
    { 8208, 8213 },
    { 11799, 11799 },
    { 11802, 11802 },
    { 11834, 11835 },
    { 11840, 11840 },
    { 11869, 11869 },
    { 12316, 12316 },
    { 12336, 12336 },
    { 12448, 12448 },
    { 65073, 65074 },
    { 65112, 65112 },
    { 65123, 65123 },
    { 65293, 65293 },
};

static const URange16 Pf_range16[] = { // Final punctuation
    { 187, 187 },
    { 8217, 8217 },
    { 8221, 8221 },
    { 8250, 8250 },
    { 11779, 11779 },
    { 11781, 11781 },
    { 11786, 11786 },
    { 11789, 11789 },
    { 11805, 11805 },
    { 11809, 11809 },
};

static const URange16 Pi_range16[] = { // Initial punctuation
    { 171, 171 },
    { 8216, 8216 },
    { 8219, 8220 },
    { 8223, 8223 },
    { 8249, 8249 },
    { 11778, 11778 },
    { 11780, 11780 },
    { 11785, 11785 },
    { 11788, 11788 },
    { 11804, 11804 },
    { 11808, 11808 },
};

static const URange16 Sc_range16[] = { // Currency symbol
    { 36, 36 },
    { 162, 165 },
    { 1423, 1423 },
    { 1547, 1547 },
    { 2046, 2047 },
    { 2546, 2547 },
    { 2555, 2555 },
    { 2801, 2801 },
    { 3065, 3065 },
    { 3647, 3647 },
    { 6107, 6107 },
    { 8352, 8384 },
    { 43064, 43064 },
    { 65020, 65020 },
    { 65129, 65129 },
    { 65284, 65284 },
    { 65504, 65505 },
    { 65509, 65510 },
};

static const URange16 Zl_range16[] = { // Line separator
    { 8232, 8232 },
};

static const URange16 Zp_range16[] = { // Paragraph separator
    { 8233, 8233 },
};

static const URange16 Zs_range16[] = { // Space separator
    { 32, 32 },
    { 160, 160 },
    { 5760, 5760 },
    { 8192, 8202 },
    { 8239, 8239 },
    { 8287, 8287 },
    { 12288, 12288 },
};

static const URange16 Arabic_range16[] = {
    { 1536, 1540 },
    { 1542, 1547 },
    { 1549, 1562 },
    { 1564, 1566 },
    { 1568, 1599 },
    { 1601, 1610 },
    { 1622, 1647 },
    { 1649, 1756 },
    { 1758, 1791 },
    { 1872, 1919 },
    { 2160, 2190 },
    { 2192, 2193 },
    { 2200, 2273 },
    { 2275, 2303 },
    { 64336, 64450 },
    { 64467, 64829 },
    { 64832, 64911 },
    { 64914, 64967 },
    { 64975, 64975 },
    { 65008, 65023 },
    { 65136, 65140 },
    { 65142, 65276 },
};

static const URange16 Bengali_range16[] = {
    { 2432, 2435 },
    { 2437, 2444 },
    { 2447, 2448 },
    { 2451, 2472 },
    { 2474, 2480 },
    { 2482, 2482 },
    { 2486, 2489 },
    { 2492, 2500 },
    { 2503, 2504 },
    { 2507, 2510 },
    { 2519, 2519 },
    { 2524, 2525 },
    { 2527, 2531 },
    { 2534, 2558 },
};

static const URange16 Cyrillic_range16[] = {
    { 1024, 1156 },
    { 1159, 1327 },
    { 7296, 7304 },
    { 7467, 7467 },
    { 7544, 7544 },
    { 11744, 11775 },
    { 42560, 42655 },
    { 65070, 65071 },
};

static const URange16 Devanagari_range16[] = {
    { 2304, 2384 },
    { 2389, 2403 },
    { 2406, 2431 },
    { 43232, 43263 },
};

static const URange16 Georgian_range16[] = {
    { 4256, 4293 },
    { 4295, 4295 },
    { 4301, 4301 },
    { 4304, 4346 },
    { 4348, 4351 },
    { 7312, 7354 },
    { 7357, 7359 },
    { 11520, 11557 },
    { 11559, 11559 },
    { 11565, 11565 },
};

static const URange16 Greek_range16[] = {
    { 880, 883 },
    { 885, 887 },
    { 890, 893 },
    { 895, 895 },
    { 900, 900 },
    { 902, 902 },
    { 904, 906 },
    { 908, 908 },
    { 910, 929 },
    { 931, 993 },
    { 1008, 1023 },
    { 7462, 7466 },
    { 7517, 7521 },
    { 7526, 7530 },
    { 7615, 7615 },
    { 7936, 7957 },
    { 7960, 7965 },
    { 7968, 8005 },
    { 8008, 8013 },
    { 8016, 8023 },
    { 8025, 8025 },
    { 8027, 8027 },
    { 8029, 8029 },
    { 8031, 8061 },
    { 8064, 8116 },
    { 8118, 8132 },
    { 8134, 8147 },
    { 8150, 8155 },
    { 8157, 8175 },
    { 8178, 8180 },
    { 8182, 8190 },
    { 8486, 8486 },
    { 43877, 43877 },
};

static const URange16 Han_range16[] = {
    { 11904, 11929 },
    { 11931, 12019 },
    { 12032, 12245 },
    { 12293, 12293 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12347 },
    { 13312, 19903 },
    { 19968, 40959 },
    { 63744, 64109 },
    { 64112, 64217 },
};

static const URange16 Hiragana_range16[] = {
	{ 12353, 12438 },
	{ 12445, 12447 },
};

static const URange16 Katakana_range16[] = {
	{ 12449, 12538 },
	{ 12541, 12543 },
	{ 12784, 12799 },
	{ 13008, 13054 },
	{ 13056, 13143 },
	{ 65382, 65391 },
	{ 65393, 65437 },
};

static const URange16 Latin_range16[] = {
    { 65, 90 },
    { 97, 122 },
    { 170, 170 },
    { 186, 186 },
    { 192, 214 },
    { 216, 246 },
    { 248, 696 },
    { 736, 740 },
    { 7424, 7461 },
    { 7468, 7516 },
    { 7522, 7525 },
    { 7531, 7543 },
    { 7545, 7614 },
    { 7680, 7935 },
    { 8305, 8305 },
    { 8319, 8319 },
    { 8336, 8348 },
    { 8490, 8491 },
    { 8498, 8498 },
    { 8526, 8526 },
    { 8544, 8584 },
    { 11360, 11391 },
    { 42786, 42887 },
    { 42891, 42954 },
    { 42960, 42961 },
    { 42963, 42963 },
    { 42965, 42969 },
    { 42994, 43007 },
    { 43824, 43866 },
    { 43868, 43876 },
    { 43878, 43881 },
    { 64256, 64262 },
    { 65313, 65338 },
    { 65345, 65370 },
};

static const URange16 Thai_range16[] = {
    { 3585, 3642 },
    { 3648, 3675 },
};

#ifdef __cplusplus
    #define _e_arg(k, v) v
#else
    #define _e_arg(k, v) [k] = v
#endif
#define UNI_ENTRY(Code) { Code##_range16, sizeof(Code##_range16)/sizeof(URange16) }

typedef struct {
  const URange16 *r16;
  int nr16;
} UGroup;

static const UGroup _utf8_unicode_groups[U8G_SIZE] = {
    _e_arg(U8G_Cc, UNI_ENTRY(Cc)),
    _e_arg(U8G_Lt, UNI_ENTRY(Lt)),
    _e_arg(U8G_Nd, UNI_ENTRY(Nd)),
    _e_arg(U8G_Nl, UNI_ENTRY(Nl)),
    _e_arg(U8G_Pc, UNI_ENTRY(Pc)),
    _e_arg(U8G_Pd, UNI_ENTRY(Pd)),
    _e_arg(U8G_Pf, UNI_ENTRY(Pf)),
    _e_arg(U8G_Pi, UNI_ENTRY(Pi)),
    _e_arg(U8G_Sc, UNI_ENTRY(Sc)),
    _e_arg(U8G_Zl, UNI_ENTRY(Zl)),
    _e_arg(U8G_Zp, UNI_ENTRY(Zp)),
    _e_arg(U8G_Zs, UNI_ENTRY(Zs)),
    _e_arg(U8G_Arabic, UNI_ENTRY(Arabic)),
    _e_arg(U8G_Bengali, UNI_ENTRY(Bengali)),
    _e_arg(U8G_Cyrillic, UNI_ENTRY(Cyrillic)),
    _e_arg(U8G_Devanagari, UNI_ENTRY(Devanagari)),
    _e_arg(U8G_Georgian, UNI_ENTRY(Georgian)),
    _e_arg(U8G_Greek, UNI_ENTRY(Greek)),
    _e_arg(U8G_Han, UNI_ENTRY(Han)),
    _e_arg(U8G_Hiragana, UNI_ENTRY(Hiragana)),
    _e_arg(U8G_Katakana, UNI_ENTRY(Katakana)),
    _e_arg(U8G_Latin, UNI_ENTRY(Latin)),
    _e_arg(U8G_Thai, UNI_ENTRY(Thai)),
};

static bool utf8_isgroup(int group, uint32_t c) {
    for (int j=0; j<_utf8_unicode_groups[group].nr16; ++j) {
        if (c < _utf8_unicode_groups[group].r16[j].lo)
            return false;
        if (c <= _utf8_unicode_groups[group].r16[j].hi)
            return true;
    }
    return false;
}

#endif // STC_UCD_PRV_C_INCLUDED
// ### END_FILE_INCLUDE: ucd_prv.c

typedef uint32_t _Rune; /* Utf8 code point */
typedef int32_t _Token;
/* max character classes per program */
#define _NCLASS CREG_MAX_CLASSES
/* max subexpressions */
#define _NSUBEXP CREG_MAX_CAPTURES
/* max rune ranges per character class */
#define _NCCRUNE (_NSUBEXP * 2)

typedef struct
{
    _Rune *end;
    _Rune spans[_NCCRUNE];
} _Reclass;

typedef struct _Reinst
{
    _Token type;
    union {
        _Reclass *classp;        /* class pointer */
        _Rune    rune;           /* character */
        int     subid;           /* sub-expression id for TOK_RBRA and TOK_LBRA */
        struct _Reinst *right;   /* right child of TOK_OR */
    } r;
    union {    /* regexp relies on these two being in the same union */
        struct _Reinst *left;    /* left child of TOK_OR */
        struct _Reinst *next;    /* next instruction for TOK_CAT & TOK_LBRA */
    } l;
} _Reinst;

typedef struct {
    bool icase;
    bool dotall;
} _Reflags;

typedef struct _Reprog
{
    _Reinst  *startinst;     /* start pc */
    _Reflags flags;
    int nsubids;
    isize allocsize;
    _Reclass cclass[_NCLASS]; /* .data */
    _Reinst  firstinst[];    /* .text : originally 5 elements? */
} _Reprog;

typedef csview _Resub;

typedef struct _Resublist
{
    _Resub m[_NSUBEXP];
} _Resublist;

enum {
    TOK_MASK    = 0xFF00000,
    TOK_OPERATOR = 0x8000000,   /* Bitmask of all operators */
    TOK_START   = 0x8000001,    /* Start, used for marker on stack */
    TOK_RBRA    ,               /* Right bracket, ) */
    TOK_LBRA    ,               /* Left bracket, ( */
    TOK_OR      ,               /* Alternation, | */
    TOK_CAT     ,               /* Concatentation, implicit operator */
    TOK_STAR    ,               /* Closure, * */
    TOK_PLUS    ,               /* a+ == aa* */
    TOK_QUEST   ,               /* a? == a|nothing, i.e. 0 or 1 a's */
    TOK_RUNE    = 0x8100000,
    TOK_IRUNE   ,
    ASC_an      , ASC_AN,       /* alphanum */
    ASC_al      , ASC_AL,       /* alpha */
    ASC_as      , ASC_AS,       /* ascii */
    ASC_bl      , ASC_BL,       /* blank */
    ASC_ct      , ASC_CT,       /* ctrl */
    ASC_d       , ASC_D,        /* digit */
    ASC_s       , ASC_S,        /* space */
    ASC_w       , ASC_W,        /* word */
    ASC_gr      , ASC_GR,       /* graphic */
    ASC_pr      , ASC_PR,       /* print */
    ASC_pu      , ASC_PU,       /* punct */
    ASC_lo      , ASC_LO,       /* lower */
    ASC_up      , ASC_UP,       /* upper */
    ASC_xd      , ASC_XD,       /* hex */
    UTF_al      , UTF_AL,       /* utf8 alpha */
    UTF_an      , UTF_AN,       /* utf8 alphanumeric */
    UTF_bl      , UTF_BL,       /* utf8 blank */
    UTF_lc      , UTF_LC,       /* utf8 letter cased */
    UTF_ll      , UTF_LL,       /* utf8 letter lowercase */
    UTF_lu      , UTF_LU,       /* utf8 letter uppercase */
    UTF_sp      , UTF_SP,       /* utf8 space */
    UTF_wr      , UTF_WR,       /* utf8 word */
    UTF_GRP = 0x8150000,
    UTF_cc = UTF_GRP+2*U8G_Cc, UTF_CC, /* utf8 control char */
    UTF_lt = UTF_GRP+2*U8G_Lt, UTF_LT, /* utf8 letter titlecase */
    UTF_nd = UTF_GRP+2*U8G_Nd, UTF_ND, /* utf8 number decimal */
    UTF_nl = UTF_GRP+2*U8G_Nl, UTF_NL, /* utf8 number letter */
    UTF_pc = UTF_GRP+2*U8G_Pc, UTF_PC, /* utf8 punct connector */
    UTF_pd = UTF_GRP+2*U8G_Pd, UTF_PD, /* utf8 punct dash */
    UTF_pf = UTF_GRP+2*U8G_Pf, UTF_PF, /* utf8 punct final */
    UTF_pi = UTF_GRP+2*U8G_Pi, UTF_PI, /* utf8 punct initial */
    UTF_sc = UTF_GRP+2*U8G_Sc, UTF_SC, /* utf8 symbol currency */
    UTF_zl = UTF_GRP+2*U8G_Zl, UTF_ZL, /* utf8 separator line */
    UTF_zp = UTF_GRP+2*U8G_Zp, UTF_ZP, /* utf8 separator paragraph */
    UTF_zs = UTF_GRP+2*U8G_Zs, UTF_ZS, /* utf8 separator space */
    UTF_arabic = UTF_GRP+2*U8G_Arabic, UTF_ARABIC,
    UTF_bengali = UTF_GRP+2*U8G_Bengali, UTF_BENGALI,
    UTF_cyrillic = UTF_GRP+2*U8G_Cyrillic, UTF_CYRILLIC,
    UTF_devanagari = UTF_GRP+2*U8G_Devanagari, UTF_DEVANAGARI,
    UTF_georgian = UTF_GRP+2*U8G_Georgian, UTF_GEORGIAN,
    UTF_greek = UTF_GRP+2*U8G_Greek, UTF_GREEK,
    UTF_han = UTF_GRP+2*U8G_Han, UTF_HAN,
    UTF_hiragana = UTF_GRP+2*U8G_Hiragana, UTF_HIRAGANA,
    UTF_katakana = UTF_GRP+2*U8G_Katakana, UTF_KATAKANA,
    UTF_latin = UTF_GRP+2*U8G_Latin, UTF_LATIN,
    UTF_thai = UTF_GRP+2*U8G_Thai, UTF_THAI,
    TOK_ANY     = 0x8200000,    /* Any character except newline, . */
    TOK_ANYNL   ,               /* Any character including newline, . */
    TOK_NOP     ,               /* No operation, internal use only */
    TOK_BOL     , TOK_BOS,      /* Beginning of line / string, ^ */
    TOK_EOL     , TOK_EOS,      /* End of line / string, $ */
    TOK_EOZ     ,               /* End of line with optional NL */
    TOK_CCLASS  ,               /* Character class, [] */
    TOK_NCCLASS ,               /* Negated character class, [] */
    TOK_WBOUND  ,               /* Non-word boundary, not consuming meta char */
    TOK_NWBOUND ,               /* Word boundary, not consuming meta char */
    TOK_CASED   ,               /* (?-i) */
    TOK_ICASE   ,               /* (?i) */
    TOK_END     = 0x82FFFFF,    /* Terminate: match found */
};

#define _LISTSIZE    10
#define _BIGLISTSIZE (10*_LISTSIZE)

typedef struct _Relist
{
    _Reinst*    inst;       /* Reinstruction of the thread */
    _Resublist  se;         /* matched subexpressions in this thread */
} _Relist;

typedef struct _Reljunk
{
    _Relist*    relist[2];
    _Relist*    reliste[2];
    int         starttype;
    _Rune       startchar;
    const char* starts;
    const char* eol;
} _Reljunk;


static int
chartorune(_Rune *rune, const char *s)
{
    utf8_decode_t ctx = {.state=0};
    const uint8_t *b = (const uint8_t*)s;
    do { utf8_decode(&ctx, *b++); } while (ctx.state);
    *rune = ctx.codep;
    return (int)((const char*)b - s);
}

static const char*
utfrune(const char *s, _Rune c)
{
    if (c < 128)        /* ascii */
        return strchr((char *)s, (int)c);
    int n;
    for (_Rune r = (uint32_t)*s; r; s += n, r = *(unsigned char*)s) {
        if (r < 128) { n = 1; continue; }
        n = chartorune(&r, s);
        if (r == c) return s;
    }
    return NULL;
}

static const char*
utfruneicase(const char *s, _Rune c)
{
    _Rune r = (uint32_t)*s;
    int n;
    if (c < 128) for (c = (_Rune)tolower((int)c); r; ++s, r = *(unsigned char*)s) {
        if (r < 128 && (_Rune)tolower((int)r) == c) return s;
    }
    else for (c = utf8_casefold(c); r; s += n, r = *(unsigned char*)s) {
        if (r < 128) { n = 1; continue; }
        n = chartorune(&r, s);
        if (utf8_casefold(r) == c) return s;
    }
    return NULL;
}


static void
_renewmatch(_Resub *mp, int ms, _Resublist *sp, int nsubids)
{
    if (mp==NULL || ms==0)
        return;
    if (mp[0].buf == NULL || sp->m[0].buf < mp[0].buf ||
       (sp->m[0].buf == mp[0].buf && sp->m[0].size > mp[0].size)) {
        for (int i=0; i<ms && i<=nsubids; i++)
            mp[i] = sp->m[i];
    }
}

static _Relist*
_renewthread(_Relist *lp,  /* _relist to add to */
    _Reinst *ip,           /* instruction to add */
    int ms,
    _Resublist *sep)       /* pointers to subexpressions */
{
    _Relist *p;

    for (p=lp; p->inst; p++) {
        if (p->inst == ip) {
            if (sep->m[0].buf < p->se.m[0].buf) {
                if (ms > 1)
                    p->se = *sep;
                else
                    p->se.m[0] = sep->m[0];
            }
            return 0;
        }
    }
    p->inst = ip;
    if (ms > 1)
        p->se = *sep;
    else
        p->se.m[0] = sep->m[0];
    (++p)->inst = NULL;
    return p;
}

static _Relist*
_renewemptythread(_Relist *lp,   /* _relist to add to */
    _Reinst *ip,                 /* instruction to add */
    int ms,
    const char *sp)             /* pointers to subexpressions */
{
    _Relist *p;

    for (p=lp; p->inst; p++) {
        if (p->inst == ip) {
            if (sp < p->se.m[0].buf) {
                if (ms > 1)
                    memset(&p->se, 0, sizeof(p->se));
                p->se.m[0].buf = sp;
            }
            return 0;
        }
    }
    p->inst = ip;
    if (ms > 1)
        memset(&p->se, 0, sizeof(p->se));
    p->se.m[0].buf = sp;
    (++p)->inst = NULL;
    return p;
}

typedef struct _Node
{
    _Reinst*    first;
    _Reinst*    last;
} _Node;

#define _NSTACK 20
typedef struct _Parser
{
    const char* exprp;   /* pointer to next character in source expression */
    _Node andstack[_NSTACK];
    _Node* andp;
    _Token atorstack[_NSTACK];
    _Token* atorp;
    short subidstack[_NSTACK]; /* parallel to atorstack */
    short* subidp;
    short cursubid;      /* id of current subexpression */
    int error;
    _Reflags flags;
    int dot_type;
    int rune_type;
    bool litmode;
    bool lastwasand;     /* Last token was _operand */
    short nbra;
    short nclass;
    isize instcap;
    _Rune yyrune;         /* last lex'd rune */
    _Reclass *yyclassp;   /* last lex'd class */
    _Reclass* classp;
    _Reinst* freep;
    jmp_buf regkaboom;
} _Parser;

/* predeclared crap */
static void _operator(_Parser *par, _Token type);
static void _pushand(_Parser *par, _Reinst *first, _Reinst *last);
static void _pushator(_Parser *par, _Token type);
static void _evaluntil(_Parser *par, _Token type);
static int  _bldcclass(_Parser *par);

static void
_rcerror(_Parser *par, cregex_result err)
{
    par->error = err;
    longjmp(par->regkaboom, 1);
}

static _Reinst*
_newinst(_Parser *par, _Token t)
{
    par->freep->type = t;
    par->freep->l.left = 0;
    par->freep->r.right = 0;
    return par->freep++;
}

static void
_operand(_Parser *par, _Token t)
{
    _Reinst *i;

    if (par->lastwasand)
        _operator(par, TOK_CAT);    /* catenate is implicit */
    i = _newinst(par, t);
    switch (t) {
    case TOK_CCLASS: case TOK_NCCLASS:
        i->r.classp = par->yyclassp; break;
    case TOK_RUNE:
        i->r.rune = par->yyrune; break;
    case TOK_IRUNE:
        i->r.rune = utf8_casefold(par->yyrune);
    }
    _pushand(par, i, i);
    par->lastwasand = true;
}

static void
_operator(_Parser *par, _Token t)
{
    if (t==TOK_RBRA && --par->nbra<0)
        _rcerror(par, CREG_UNMATCHEDRIGHTPARENTHESIS);
    if (t==TOK_LBRA) {
        if (++par->cursubid >= _NSUBEXP)
            _rcerror(par, CREG_TOOMANYSUBEXPRESSIONS);
        par->nbra++;
        if (par->lastwasand)
            _operator(par, TOK_CAT);
    } else
        _evaluntil(par, t);
    if (t != TOK_RBRA)
        _pushator(par, t);
    par->lastwasand = 0;
    if (t==TOK_STAR || t==TOK_QUEST || t==TOK_PLUS || t==TOK_RBRA)
        par->lastwasand = true;    /* these look like operands */
}

static void
_pushand(_Parser *par, _Reinst *f, _Reinst *l)
{
    if (par->andp >= &par->andstack[_NSTACK])
        _rcerror(par, CREG_OPERANDSTACKOVERFLOW);
    par->andp->first = f;
    par->andp->last = l;
    par->andp++;
}

static void
_pushator(_Parser *par, _Token t)
{
    if (par->atorp >= &par->atorstack[_NSTACK])
        _rcerror(par, CREG_OPERATORSTACKOVERFLOW);
    *par->atorp++ = t;
    *par->subidp++ = par->cursubid;
}

static _Node*
_popand(_Parser *par, _Token op)
{
    (void)op;
    _Reinst *inst;

    if (par->andp <= &par->andstack[0]) {
        _rcerror(par, CREG_MISSINGOPERAND);
        inst = _newinst(par, TOK_NOP);
        _pushand(par, inst, inst);
    }
    return --par->andp;
}

static _Token
_popator(_Parser *par)
{
    if (par->atorp <= &par->atorstack[0])
        _rcerror(par, CREG_OPERATORSTACKUNDERFLOW);
    --par->subidp;
    return *--par->atorp;
}


static void
_evaluntil(_Parser *par, _Token pri)
{
    _Node *op1, *op2;
    _Reinst *inst1, *inst2;

    while (pri==TOK_RBRA || par->atorp[-1]>=pri) {
        switch (_popator(par)) {
        default:
            _rcerror(par, CREG_UNKNOWNOPERATOR);
            break;
        case TOK_LBRA:        /* must have been TOK_RBRA */
            op1 = _popand(par, '(');
            inst2 = _newinst(par, TOK_RBRA);
            inst2->r.subid = *par->subidp;
            op1->last->l.next = inst2;
            inst1 = _newinst(par, TOK_LBRA);
            inst1->r.subid = *par->subidp;
            inst1->l.next = op1->first;
            _pushand(par, inst1, inst2);
            return;
        case TOK_OR:
            op2 = _popand(par, '|');
            op1 = _popand(par, '|');
            inst2 = _newinst(par, TOK_NOP);
            op2->last->l.next = inst2;
            op1->last->l.next = inst2;
            inst1 = _newinst(par, TOK_OR);
            inst1->r.right = op1->first;
            inst1->l.left = op2->first;
            _pushand(par, inst1, inst2);
            break;
        case TOK_CAT:
            op2 = _popand(par, 0);
            op1 = _popand(par, 0);
            op1->last->l.next = op2->first;
            _pushand(par, op1->first, op2->last);
            break;
        case TOK_STAR:
            op2 = _popand(par, '*');
            inst1 = _newinst(par, TOK_OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            _pushand(par, inst1, inst1);
            break;
        case TOK_PLUS:
            op2 = _popand(par, '+');
            inst1 = _newinst(par, TOK_OR);
            op2->last->l.next = inst1;
            inst1->r.right = op2->first;
            _pushand(par, op2->first, inst1);
            break;
        case TOK_QUEST:
            op2 = _popand(par, '?');
            inst1 = _newinst(par, TOK_OR);
            inst2 = _newinst(par, TOK_NOP);
            inst1->l.left = inst2;
            inst1->r.right = op2->first;
            op2->last->l.next = inst2;
            _pushand(par, inst1, inst2);
            break;
        }
    }
}


static _Reprog*
_optimize(_Parser *par, _Reprog *pp)
{
    _Reinst *inst, *target;
    _Reclass *cl;

    for (inst = pp->firstinst; inst->type != TOK_END; inst++) {
        target = inst->l.next;
        while (target->type == TOK_NOP)
            target = target->l.next;
        inst->l.next = target;
    }

    if ((par->freep - pp->firstinst)*2 > par->instcap)
        return pp;

    isize ipp = (isize)pp; // convert pointer to isize!
    isize new_allocsize = c_sizeof(_Reprog) + (par->freep - pp->firstinst)*c_sizeof(_Reinst);
    _Reprog *npp = (_Reprog *)i_realloc(pp, pp->allocsize, new_allocsize);
    ptrdiff_t diff = (isize)npp - ipp;

    if ((npp == NULL) | (diff == 0))
        return (_Reprog *)ipp;
    npp->allocsize = new_allocsize;
    par->freep = (_Reinst *)((char *)par->freep + diff);

    for (inst = npp->firstinst; inst < par->freep; inst++) {
        switch (inst->type) {
        case TOK_OR:
        case TOK_STAR:
        case TOK_PLUS:
        case TOK_QUEST:
            inst->r.right = (_Reinst *)((char*)inst->r.right + diff);
            break;
        case TOK_CCLASS:
        case TOK_NCCLASS:
            inst->r.right = (_Reinst *)((char*)inst->r.right + diff);
            cl = inst->r.classp;
            cl->end = (_Rune *)((char*)cl->end + diff);
            break;
        }
        if (inst->l.left)
            inst->l.left = (_Reinst *)((char*)inst->l.left + diff);
    }
    npp->startinst = (_Reinst *)((char*)npp->startinst + diff);
    return npp;
}


static _Reclass*
_newclass(_Parser *par)
{
    if (par->nclass >= _NCLASS)
        _rcerror(par, CREG_TOOMANYCHARACTERCLASSES);
    return &(par->classp[par->nclass++]);
}


static int /* quoted */
_nextc(_Parser *par, _Rune *rp)
{
    int ret;
    for (;;) {
        ret = par->litmode;
        par->exprp += chartorune(rp, par->exprp);

        if (*rp == '\\') {
            if (par->litmode) {
                if (*par->exprp != 'E')
                    break;
                par->exprp += 1;
                par->litmode = false;
                continue;
            }
            par->exprp += chartorune(rp, par->exprp);
            if (*rp == 'Q') {
                par->litmode = true;
                continue;
            }
            ret = 1;
        }
        break;
    }
    return ret;
}


static void
_lexasciiclass(_Parser *par, _Rune *rp) /* assume *rp == '[' and *par->exprp == ':' */
{
    static struct { const char* c; int n, r; } cls[] = {
        {"alnum:]", 7, ASC_an}, {"alpha:]", 7, ASC_al}, {"ascii:]", 7, ASC_as},
        {"blank:]", 7, ASC_bl}, {"cntrl:]", 7, ASC_ct}, {"digit:]", 7, ASC_d},
        {"graph:]", 7, ASC_gr}, {"lower:]", 7, ASC_lo}, {"print:]", 7, ASC_pr},
        {"punct:]", 7, ASC_pu}, {"space:]", 7, ASC_s}, {"upper:]", 7, ASC_up},
        {"xdigit:]", 8, ASC_xd}, {"word:]", 6, ASC_w},
    };
    int inv = par->exprp[1] == '^', off = 1 + inv;
    for (unsigned i = 0; i < (sizeof cls/sizeof *cls); ++i)
        if (strncmp(par->exprp + off, cls[i].c, (size_t)cls[i].n) == 0) {
            *rp = (_Rune)cls[i].r;
            par->exprp += off + cls[i].n;
            break;
        }
    if (par->rune_type == TOK_IRUNE && (*rp == ASC_lo || *rp == ASC_up))
        *rp = (_Rune)ASC_al;
    if (inv && *rp != '[')
        *rp += 1;
}


static void
_lexutfclass(_Parser *par, _Rune *rp)
{
    static struct { const char* c; uint32_t n, r; } cls[] = {
        {"{Alpha}", 7, UTF_al}, {"{L&}", 4, UTF_lc},
        {"{Digit}", 7, UTF_nd}, {"{Nd}", 4, UTF_nd},
        {"{Lower}", 7, UTF_ll}, {"{Ll}", 4, UTF_ll},
        {"{Upper}", 7, UTF_lu}, {"{Lu}", 4, UTF_lu},
        {"{Cntrl}", 7, UTF_cc}, {"{Cc}", 4, UTF_cc},
        {"{Alnum}", 7, UTF_an}, {"{Blank}", 7, UTF_bl},
        {"{Space}", 7, UTF_sp}, {"{Word}", 6, UTF_wr},
        {"{XDigit}", 8, ASC_xd},
        {"{Lt}", 4, UTF_lt}, {"{Nl}", 4, UTF_nl},
        {"{Pc}", 4, UTF_pc}, {"{Pd}", 4, UTF_pd},
        {"{Pf}", 4, UTF_pf}, {"{Pi}", 4, UTF_pi},
        {"{Zl}", 4, UTF_zl}, {"{Zp}", 4, UTF_zp},
        {"{Zs}", 4, UTF_zs}, {"{Sc}", 4, UTF_sc},
        {"{Arabic}", 8, UTF_arabic},
        {"{Bengali}", 9, UTF_bengali},
        {"{Cyrillic}", 10, UTF_cyrillic},
        {"{Devanagari}", 12, UTF_devanagari},
        {"{Georgian}", 10, UTF_georgian},
        {"{Greek}", 7, UTF_greek},
        {"{Han}", 5, UTF_han},
        {"{Hiragana}", 10, UTF_hiragana},
        {"{Katakana}", 10, UTF_katakana},
        {"{Latin}", 7, UTF_latin},
        {"{Thai}", 6, UTF_thai},
    };
    unsigned inv = (*rp == 'P');
    for (unsigned i = 0; i < (sizeof cls/sizeof *cls); ++i) {
        if (strncmp(par->exprp, cls[i].c, (size_t)cls[i].n) == 0) {
            if (par->rune_type == TOK_IRUNE && (cls[i].r == UTF_ll || cls[i].r == UTF_lu))
                *rp = (_Rune)(UTF_lc + inv);
            else
                *rp = (_Rune)(cls[i].r + inv);
            par->exprp += cls[i].n;
            break;
        }
    }
}

#define CASE_RUNE_MAPPINGS(rune) \
    case 't': rune = '\t'; break; \
    case 'n': rune = '\n'; break; \
    case 'r': rune = '\r'; break; \
    case 'v': rune = '\v'; break; \
    case 'f': rune = '\f'; break; \
    case 'a': rune = '\a'; break; \
    case 'd': rune = UTF_nd; break; \
    case 'D': rune = UTF_ND; break; \
    case 's': rune = UTF_sp; break; \
    case 'S': rune = UTF_SP; break; \
    case 'w': rune = UTF_wr; break; \
    case 'W': rune = UTF_WR; break


static _Token
_lex(_Parser *par)
{
    bool quoted = _nextc(par, &par->yyrune);

    if (quoted) {
        if (par->litmode)
            return par->rune_type;

        switch (par->yyrune) {
        CASE_RUNE_MAPPINGS(par->yyrune);
        case 'b': return TOK_WBOUND;
        case 'B': return TOK_NWBOUND;
        case 'A': return TOK_BOS;
        case 'z': return TOK_EOS;
        case 'Z': return TOK_EOZ;
        case 'x': /* hex number rune */
            if (*par->exprp != '{') break;
            sscanf(++par->exprp, "%x", &par->yyrune);
            while (*par->exprp) if (*(par->exprp++) == '}') break;
            if (par->exprp[-1] != '}')
                _rcerror(par, CREG_UNMATCHEDRIGHTPARENTHESIS);
            if (par->yyrune == 0) return TOK_END;
            break;
        case 'p': case 'P':
            _lexutfclass(par, &par->yyrune);
            break;
        }
        return par->rune_type;
    }

    switch (par->yyrune) {
    case  0 : return TOK_END;
    case '*': return TOK_STAR;
    case '?': return TOK_QUEST;
    case '+': return TOK_PLUS;
    case '|': return TOK_OR;
    case '^': return TOK_BOL;
    case '$': return TOK_EOL;
    case '.': return par->dot_type;
    case '[': return _bldcclass(par);
    case '(':
        if (par->exprp[0] == '?') { /* override global flags */
            for (int k = 1, enable = 1; ; ++k) switch (par->exprp[k]) {
                case  0 : par->exprp += k; return TOK_END;
                case ')': par->exprp += k + 1;
                          return TOK_CASED + (par->rune_type == TOK_IRUNE);
                case '-': enable = 0; break;
                case 's': par->dot_type = TOK_ANY + enable; break;
                case 'i': par->rune_type = TOK_RUNE + enable; break;
                default: _rcerror(par, CREG_UNKNOWNOPERATOR); return 0;
            }
        }
        return TOK_LBRA;
    case ')': return TOK_RBRA;
    }
    return par->rune_type;
}


static _Token
_bldcclass(_Parser *par)
{
    _Token type;
    _Rune r[_NCCRUNE];
    _Rune *p, *ep, *np;
    _Rune rune;
    int quoted;

    /* we have already seen the '[' */
    type = TOK_CCLASS;
    par->yyclassp = _newclass(par);

    /* look ahead for negation */
    /* SPECIAL CASE!!! negated classes don't match \n */
    ep = r;
    quoted = _nextc(par, &rune);
    if (!quoted && rune == '^') {
        type = TOK_NCCLASS;
        quoted = _nextc(par, &rune);
        ep[0] = ep[1] = '\n';
        ep += 2;
    }

    /* parse class into a set of spans */
    for (; ep < &r[_NCCRUNE]; quoted = _nextc(par, &rune)) {
        if (rune == 0) {
            _rcerror(par, CREG_MALFORMEDCHARACTERCLASS);
            return 0;
        }
        if (!quoted) {
            if (rune == ']')
                break;
            if (rune == '-') {
                if (ep != r && *par->exprp != ']') {
                    quoted = _nextc(par, &rune);
                    if (rune == 0) {
                        _rcerror(par, CREG_MALFORMEDCHARACTERCLASS);
                        return 0;
                    }
                    ep[-1] = par->rune_type == TOK_IRUNE ? utf8_casefold(rune) : rune;
                    continue;
                }
            }
            if (rune == '[' && *par->exprp == ':')
                _lexasciiclass(par, &rune);
        } else switch (rune) {
            CASE_RUNE_MAPPINGS(rune);
            case 'p': case 'P':
                _lexutfclass(par, &rune);
                break;
        }
        ep[0] = ep[1] = par->rune_type == TOK_IRUNE ? utf8_casefold(rune) : rune;
        ep += 2;
    }

    /* sort on span start */
    for (p = r; p < ep; p += 2)
        for (np = p; np < ep; np += 2)
            if (*np < *p) {
                rune = np[0]; np[0] = p[0]; p[0] = rune;
                rune = np[1]; np[1] = p[1]; p[1] = rune;
            }

    /* merge spans */
    np = par->yyclassp->spans;
    p = r;
    if (r == ep)
        par->yyclassp->end = np;
    else {
        np[0] = *p++;
        np[1] = *p++;
        for (; p < ep; p += 2)
            if (p[0] <= np[1]) {
                if (p[1] > np[1])
                    np[1] = p[1];
            } else {
                np += 2;
                np[0] = p[0];
                np[1] = p[1];
            }
        par->yyclassp->end = np+2;
    }

    return type;
}


static _Reprog*
_regcomp1(_Reprog *pp, _Parser *par, const char *s, int cflags)
{
    _Token token;

    /* get memory for the program. estimated max usage */
    isize instcap = 5 + 6*c_strlen(s);
    isize new_allocsize = c_sizeof(_Reprog) + instcap*c_sizeof(_Reinst);
    pp = (_Reprog *)i_realloc(pp, pp ? pp->allocsize : 0, new_allocsize);
    if (pp == NULL) {
        par->error = CREG_OUTOFMEMORY;
        return NULL;
    }
    pp->allocsize = new_allocsize;
    pp->flags.icase = (cflags & CREG_ICASE) != 0;
    pp->flags.dotall = (cflags & CREG_DOTALL) != 0;
    par->instcap = instcap;
    par->freep = pp->firstinst;
    par->classp = pp->cclass;
    par->error = 0;

    if (setjmp(par->regkaboom))
        goto out;

    /* go compile the sucker */
    par->flags = pp->flags;
    par->rune_type = pp->flags.icase ? TOK_IRUNE : TOK_RUNE;
    par->dot_type = pp->flags.dotall ? TOK_ANYNL : TOK_ANY;
    par->litmode = false;
    par->exprp = s;
    par->nclass = 0;
    par->nbra = 0;
    par->atorp = par->atorstack;
    par->andp = par->andstack;
    par->subidp = par->subidstack;
    par->lastwasand = false;
    par->cursubid = 0;

    /* Start with a low priority operator to prime parser */
    _pushator(par, TOK_START-1);
    while ((token = _lex(par)) != TOK_END) {
        if ((token & TOK_MASK) == TOK_OPERATOR)
            _operator(par, token);
        else
            _operand(par, token);
    }

    /* Close with a low priority operator */
    _evaluntil(par, TOK_START);

    /* Force TOK_END */
    _operand(par, TOK_END);
    _evaluntil(par, TOK_START);

    if (par->nbra)
        _rcerror(par, CREG_UNMATCHEDLEFTPARENTHESIS);
    --par->andp;    /* points to first and only _operand */
    pp->startinst = par->andp->first;

    pp = _optimize(par, pp);
    pp->nsubids = par->cursubid;
out:
    if (par->error) {
        i_free(pp, pp->allocsize);
        pp = NULL;
    }
    return pp;
}

#if defined __clang__
  #pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#elif defined __GNUC__
  #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

static int
_runematch(_Rune s, _Rune r)
{
    int inv = 0, n;
    switch (s) {
    case ASC_D: inv = 1; case ASC_d: return inv ^ (isdigit((int)r) != 0);
    case ASC_S: inv = 1; case ASC_s: return inv ^ (isspace((int)r) != 0);
    case ASC_W: inv = 1; case ASC_w: return inv ^ ((isalnum((int)r) != 0) | (r == '_'));
    case ASC_AL: inv = 1; case ASC_al: return inv ^ (isalpha((int)r) != 0);
    case ASC_AN: inv = 1; case ASC_an: return inv ^ (isalnum((int)r) != 0);
    case ASC_AS: return (r >= 128); case ASC_as: return (r < 128);
    case ASC_BL: inv = 1; case ASC_bl: return inv ^ ((r == ' ') | (r == '\t'));
    case ASC_CT: inv = 1; case ASC_ct: return inv ^ (iscntrl((int)r) != 0);
    case ASC_GR: inv = 1; case ASC_gr: return inv ^ (isgraph((int)r) != 0);
    case ASC_PR: inv = 1; case ASC_pr: return inv ^ (isprint((int)r) != 0);
    case ASC_PU: inv = 1; case ASC_pu: return inv ^ (ispunct((int)r) != 0);
    case ASC_LO: inv = 1; case ASC_lo: return inv ^ (islower((int)r) != 0);
    case ASC_UP: inv = 1; case ASC_up: return inv ^ (isupper((int)r) != 0);
    case ASC_XD: inv = 1; case ASC_xd: return inv ^ (isxdigit((int)r) != 0);
    case UTF_AN: inv = 1; case UTF_an: return inv ^ (int)utf8_isalnum(r);
    case UTF_BL: inv = 1; case UTF_bl: return inv ^ (int)utf8_isblank(r);
    case UTF_SP: inv = 1; case UTF_sp: return inv ^ (int)utf8_isspace(r);
    case UTF_LL: inv = 1; case UTF_ll: return inv ^ (int)utf8_islower(r);
    case UTF_LU: inv = 1; case UTF_lu: return inv ^ (int)utf8_isupper(r);
    case UTF_LC: inv = 1; case UTF_lc: return inv ^ (int)utf8_iscased(r);
    case UTF_AL: inv = 1; case UTF_al: return inv ^ (int)utf8_isalpha(r);
    case UTF_WR: inv = 1; case UTF_wr: return inv ^ (int)utf8_isword(r);
    case UTF_cc: case UTF_CC:
    case UTF_lt: case UTF_LT:
    case UTF_nd: case UTF_ND:
    case UTF_nl: case UTF_NL:
    case UTF_pc: case UTF_PC:
    case UTF_pd: case UTF_PD:
    case UTF_pf: case UTF_PF:
    case UTF_pi: case UTF_PI:
    case UTF_sc: case UTF_SC:
    case UTF_zl: case UTF_ZL:
    case UTF_zp: case UTF_ZP:
    case UTF_zs: case UTF_ZS:
    case UTF_arabic: case UTF_ARABIC:
    case UTF_bengali: case UTF_BENGALI:
    case UTF_cyrillic: case UTF_CYRILLIC:
    case UTF_devanagari: case UTF_DEVANAGARI:
    case UTF_georgian: case UTF_GEORGIAN:
    case UTF_greek: case UTF_GREEK:
    case UTF_han: case UTF_HAN:
    case UTF_hiragana: case UTF_HIRAGANA:
    case UTF_katakana: case UTF_KATAKANA:
    case UTF_latin: case UTF_LATIN:
    case UTF_thai: case UTF_THAI:
        n = (int)s - UTF_GRP;
        inv = n & 1;
        return inv ^ (int)utf8_isgroup(n / 2, r);
    }
    return s == r;
}

static int
_regexec1(const _Reprog *progp,  /* program to run */
    const char *bol,    /* string to run machine on */
    _Resub *mp,         /* subexpression elements */
    int ms,        /* number of elements at mp */
    _Reljunk *j,
    int mflags
)
{
    int flag=0;
    _Reinst *inst;
    _Relist *tlp;
    _Relist *tl, *nl;    /* This list, next list */
    _Relist *tle, *nle;  /* Ends of this and next list */
    const char *s, *p;
    _Rune r, *rp, *ep;
    int n, checkstart, match = 0;
    int i;

    bool icase = progp->flags.icase;
    checkstart = j->starttype;
    if (mp)
        for (i=0; i<ms; i++) {
            mp[i].buf = NULL;
            mp[i].size = 0;
        }
    j->relist[0][0].inst = NULL;
    j->relist[1][0].inst = NULL;

    /* Execute machine once for each character, including terminal NUL */
    s = j->starts;
    do {
        /* fast check for first char */
        if (checkstart) {
            switch (j->starttype) {
            case TOK_IRUNE:
                p = utfruneicase(s, j->startchar);
                goto next1;
            case TOK_RUNE:
                p = utfrune(s, j->startchar);
                next1:
                if (p == NULL || s == j->eol)
                    return match;
                s = p;
                break;
            case TOK_BOL:
                if (s == bol)
                    break;
                p = utfrune(s, '\n');
                if (p == NULL || s == j->eol)
                    return match;
                s = p+1;
                break;
            }
        }
        r = *(unsigned char*)s;
        n = r < 128 ? 1 : chartorune(&r, s);

        /* switch run lists */
        tl = j->relist[flag];
        tle = j->reliste[flag];
        nl = j->relist[flag^=1];
        nle = j->reliste[flag];
        nl->inst = NULL;

        /* Add first instruction to current list */
        if (match == 0)
            _renewemptythread(tl, progp->startinst, ms, s);

        /* Execute machine until current list is empty */
        for (tlp=tl; tlp->inst; tlp++) {    /* assignment = */
            for (inst = tlp->inst; ; inst = inst->l.next) {
                int ok = false;

                switch (inst->type) {
                case TOK_IRUNE:
                    r = utf8_casefold(r); /* FALLTHRU */
                case TOK_RUNE:
                    ok = _runematch(inst->r.rune, r);
                    break;
                case TOK_CASED: case TOK_ICASE:
                    icase = inst->type == TOK_ICASE;
                    continue;
                case TOK_LBRA:
                    tlp->se.m[inst->r.subid].buf = s;
                    continue;
                case TOK_RBRA:
                    tlp->se.m[inst->r.subid].size = (s - tlp->se.m[inst->r.subid].buf);
                    continue;
                case TOK_ANY:
                    ok = (r != '\n');
                    break;
                case TOK_ANYNL:
                    ok = true;
                    break;
                case TOK_BOL:
                    if (s == bol || s[-1] == '\n') continue;
                    break;
                case TOK_BOS:
                    if (s == bol) continue;
                    break;
                case TOK_EOL:
                    if (r == '\n') continue; /* FALLTHRU */
                case TOK_EOS:
                    if (s == j->eol || r == 0) continue;
                    break;
                case TOK_EOZ:
                    if (s == j->eol || r == 0 || (r == '\n' && s[1] == 0)) continue;
                    break;
                case TOK_NWBOUND:
                    ok = true; /* FALLTHRU */
                case TOK_WBOUND:
                    if (ok ^ (s == bol || s == j->eol || (utf8_isword(utf8_peek_at(s, -1))
                                                        ^ utf8_isword(utf8_peek(s)))))
                        continue;
                    break;
                case TOK_NCCLASS:
                    ok = true; /* FALLTHRU */
                case TOK_CCLASS:
                    ep = inst->r.classp->end;
                    if (icase) r = utf8_casefold(r);
                    for (rp = inst->r.classp->spans; rp < ep; rp += 2) {
                        if ((r >= rp[0] && r <= rp[1]) || (rp[0] == rp[1] && _runematch(rp[0], r)))
                            break;
                    }
                    ok ^= (rp < ep);
                    break;
                case TOK_OR:
                    /* evaluate right choice later */
                    if (_renewthread(tlp, inst->r.right, ms, &tlp->se) == tle)
                        return -1;
                    /* efficiency: advance and re-evaluate */
                    continue;
                case TOK_END:    /* Match! */
                    match = !(mflags & CREG_FULLMATCH) ||
                            ((s == j->eol || r == 0 || r == '\n') &&
                            (tlp->se.m[0].buf == bol || tlp->se.m[0].buf[-1] == '\n'));
                    tlp->se.m[0].size = (s - tlp->se.m[0].buf);
                    if (mp != NULL)
                        _renewmatch(mp, ms, &tlp->se, progp->nsubids);
                    break;
                }

                if (ok && _renewthread(nl, inst->l.next, ms, &tlp->se) == nle)
                    return -1;
                break;
            }
        }
        if (s == j->eol)
            break;
        checkstart = j->starttype && nl->inst==NULL;
        s += n;
    } while (r);
    return match;
}


static int
_regexec2(const _Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    _Resub *mp,         /* subexpression elements */
    int ms,             /* number of elements at mp */
    _Reljunk *j,
    int mflags
)
{
    int rv;
    _Relist *relists;

    /* mark space */
    isize sz = 2 * _BIGLISTSIZE*c_sizeof(_Relist);
    relists = (_Relist *)i_malloc(sz);
    if (relists == NULL)
        return -1;

    j->relist[0] = relists;
    j->relist[1] = relists + _BIGLISTSIZE;
    j->reliste[0] = relists + _BIGLISTSIZE - 2;
    j->reliste[1] = relists + 2*_BIGLISTSIZE - 2;

    rv = _regexec1(progp, bol, mp, ms, j, mflags);
    i_free(relists, sz);
    return rv;
}

static int
_regexec(const _Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    int ms,             /* number of elements at mp */
    _Resub mp[],        /* subexpression elements */
    int mflags)
{
    _Reljunk j;
    _Relist relist0[_LISTSIZE], relist1[_LISTSIZE];
    int rv;

    j.starts = bol;
    j.eol = NULL;

    if (mp && mp[0].buf) {
        if (mflags & CREG_STARTEND)
            j.starts = mp[0].buf, j.eol = mp[0].buf + mp[0].size;
        else if (mflags & CREG_NEXT)
            j.starts = mp[0].buf + mp[0].size;
    }

    j.starttype = 0;
    j.startchar = 0;
    int rune_type = progp->flags.icase ? TOK_IRUNE : TOK_RUNE;
    if (progp->startinst->type == rune_type && progp->startinst->r.rune < 128) {
        j.starttype = rune_type;
        j.startchar = progp->startinst->r.rune;
    }
    if (progp->startinst->type == TOK_BOL)
        j.starttype = TOK_BOL;

    /* mark space */
    j.relist[0] = relist0;
    j.relist[1] = relist1;
    j.reliste[0] = relist0 + _LISTSIZE - 2;
    j.reliste[1] = relist1 + _LISTSIZE - 2;

    rv = _regexec1(progp, bol, mp, ms, &j, mflags);
    if (rv >= 0)
        return rv;
    rv = _regexec2(progp, bol, mp, ms, &j, mflags);
    return rv;
}


static void
_build_subst(const char* replace, int nmatch, const csview match[],
             bool(*transform)(int, csview, cstr*), cstr* subst) {
    cstr_view buf = cstr_getview(subst);
    isize len = 0, cap = buf.cap;
    char* dst = buf.data;
    cstr mstr = {0};

    while (*replace != '\0') {
        if (*replace == '$') {
            const int arg = *++replace;
            int g;
            switch (arg) {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                g = arg - '0';
                if (replace[1] >= '0' && replace[1] <= '9' && replace[2] == ';')
                    { g = g*10 + (replace[1] - '0'); replace += 2; }
                if (g < nmatch) {
                    csview m = transform && transform(g, match[g], &mstr) ? cstr_sv(&mstr) : match[g];
                    if (len + m.size > cap)
                        dst = cstr_reserve(subst, cap += cap/2 + m.size);
                    for (int i = 0; i < m.size; ++i)
                        dst[len++] = m.buf[i];
                }
                ++replace;
            case '\0':
                continue;
            }
        }
        if (len == cap)
            dst = cstr_reserve(subst, cap += cap/2 + 4);
        dst[len++] = *replace++;
    }
    cstr_drop(&mstr);
    _cstr_set_size(subst, len);
}



int
cregex_compile_pro(cregex *self, const char* pattern, int cflags) {
    _Parser par;
    self->prog = _regcomp1(self->prog, &par, pattern, cflags);
    return self->error = par.error;
}

int
cregex_captures(const cregex* self) {
    return self->prog ? self->prog->nsubids : 0;
}

int
cregex_match_pro(const cregex* re, const char* input, csview match[], int mflags) {
    int res = _regexec(re->prog, input, cregex_captures(re) + 1, match, mflags);
    switch (res) {
        case 1: return CREG_OK;
        case 0: return CREG_NOMATCH;
        default: return CREG_MATCHERROR;
    }
}

int
cregex_match_aio(const char* pattern, const char* input, csview match[]) {
    cregex re = cregex_make(pattern, CREG_DEFAULT);
    if (re.error != CREG_OK) return re.error;
    int res = cregex_match_pro(&re, input, match, CREG_DEFAULT);
    cregex_drop(&re);
    return res;
}

cstr
cregex_replace_pro(const cregex* re, csview input, const char* replace,
                  int count, bool(*transform)(int, csview, cstr*), int rflags) {
    cstr out = {0};
    cstr subst = {0};
    csview match[CREG_MAX_CAPTURES];
    int nmatch = cregex_captures(re) + 1;
    bool copy = !(rflags & CREG_STRIP);

    while (count-- && cregex_match_sv(re, input, match) == CREG_OK) {
        _build_subst(replace, nmatch, match, transform, &subst);
        const isize mpos = (match[0].buf - input.buf);
        if (copy & (mpos > 0)) cstr_append_n(&out, input.buf, mpos);
        cstr_append_s(&out, subst);
        input.buf = match[0].buf + match[0].size;
        input.size -= mpos + match[0].size;
    }
    if (copy) cstr_append_sv(&out, input);
    cstr_drop(&subst);
    return out;
}

cstr
cregex_replace_aio_pro(const char* pattern, csview input, const char* replace,
                          int count, bool(*transform)(int, csview, cstr*), int crflags) {
    cregex re = {0};
    if (cregex_compile_pro(&re, pattern, crflags) != CREG_OK)
        assert(0);
    cstr out = cregex_replace_pro(&re, input, replace, count, transform, crflags);
    cregex_drop(&re);
    return out;
}

void
cregex_drop(cregex* self) {
    i_free(self->prog, self->prog->allocsize);
}

#endif // STC_CREGEX_PRV_C_INCLUDED
// ### END_FILE_INCLUDE: cregex_prv.c
  #if defined i_import
// ### BEGIN_FILE_INCLUDE: utf8_prv.c
#ifndef STC_UTF8_PRV_C_INCLUDED
#define STC_UTF8_PRV_C_INCLUDED

// ### BEGIN_FILE_INCLUDE: utf8_tab.c

struct CaseMapping { uint16_t c1, c2, m2; };

static struct CaseMapping casemappings[] = {
    {0x0041, 0x005A, 0x007A}, // A a (26) LATIN CAPITAL LETTER A
    {0x00B5, 0x00B5, 0x03BC}, // µ μ ( 1) MICRO SIGN
    {0x00C0, 0x00D6, 0x00F6}, // À à (23) LATIN CAPITAL LETTER A WITH GRAVE
    {0x00D8, 0x00DE, 0x00FE}, // Ø ø ( 7) LATIN CAPITAL LETTER O WITH STROKE
    {0x0100, 0x012E, 0x012F}, // Ā ā (24) LATIN CAPITAL LETTER A WITH MACRON
    {0x0132, 0x0136, 0x0137}, // Ĳ ĳ ( 3) LATIN CAPITAL LIGATURE IJ
    {0x0139, 0x0147, 0x0148}, // Ĺ ĺ ( 8) LATIN CAPITAL LETTER L WITH ACUTE
    {0x014A, 0x0176, 0x0177}, // Ŋ ŋ (23) LATIN CAPITAL LETTER ENG
    {0x0178, 0x0178, 0x00FF}, // Ÿ ÿ ( 1) LATIN CAPITAL LETTER Y WITH DIAERESIS
    {0x0179, 0x017D, 0x017E}, // Ź ź ( 3) LATIN CAPITAL LETTER Z WITH ACUTE
    {0x017F, 0x017F, 0x0073}, // ſ s ( 1) LATIN SMALL LETTER LONG S
    {0x0181, 0x0181, 0x0253}, // Ɓ ɓ ( 1) LATIN CAPITAL LETTER B WITH HOOK
    {0x0182, 0x0184, 0x0185}, // Ƃ ƃ ( 2) LATIN CAPITAL LETTER B WITH TOPBAR
    {0x0186, 0x0186, 0x0254}, // Ɔ ɔ ( 1) LATIN CAPITAL LETTER OPEN O
    {0x0187, 0x0187, 0x0188}, // Ƈ ƈ ( 1) LATIN CAPITAL LETTER C WITH HOOK
    {0x0189, 0x018A, 0x0257}, // Ɖ ɖ ( 2) LATIN CAPITAL LETTER AFRICAN D
    {0x018B, 0x018B, 0x018C}, // Ƌ ƌ ( 1) LATIN CAPITAL LETTER D WITH TOPBAR
    {0x018E, 0x018E, 0x01DD}, // Ǝ ǝ ( 1) LATIN CAPITAL LETTER REVERSED E
    {0x018F, 0x018F, 0x0259}, // Ə ə ( 1) LATIN CAPITAL LETTER SCHWA
    {0x0190, 0x0190, 0x025B}, // Ɛ ɛ ( 1) LATIN CAPITAL LETTER OPEN E
    {0x0191, 0x0191, 0x0192}, // Ƒ ƒ ( 1) LATIN CAPITAL LETTER F WITH HOOK
    {0x0193, 0x0193, 0x0260}, // Ɠ ɠ ( 1) LATIN CAPITAL LETTER G WITH HOOK
    {0x0194, 0x0194, 0x0263}, // Ɣ ɣ ( 1) LATIN CAPITAL LETTER GAMMA
    {0x0196, 0x0196, 0x0269}, // Ɩ ɩ ( 1) LATIN CAPITAL LETTER IOTA
    {0x0197, 0x0197, 0x0268}, // Ɨ ɨ ( 1) LATIN CAPITAL LETTER I WITH STROKE
    {0x0198, 0x0198, 0x0199}, // Ƙ ƙ ( 1) LATIN CAPITAL LETTER K WITH HOOK
    {0x019C, 0x019C, 0x026F}, // Ɯ ɯ ( 1) LATIN CAPITAL LETTER TURNED M
    {0x019D, 0x019D, 0x0272}, // Ɲ ɲ ( 1) LATIN CAPITAL LETTER N WITH LEFT HOOK
    {0x019F, 0x019F, 0x0275}, // Ɵ ɵ ( 1) LATIN CAPITAL LETTER O WITH MIDDLE TILDE
    {0x01A0, 0x01A4, 0x01A5}, // Ơ ơ ( 3) LATIN CAPITAL LETTER O WITH HORN
    {0x01A6, 0x01A6, 0x0280}, // Ʀ ʀ ( 1) LATIN LETTER YR
    {0x01A7, 0x01A7, 0x01A8}, // Ƨ ƨ ( 1) LATIN CAPITAL LETTER TONE TWO
    {0x01A9, 0x01A9, 0x0283}, // Ʃ ʃ ( 1) LATIN CAPITAL LETTER ESH
    {0x01AC, 0x01AC, 0x01AD}, // Ƭ ƭ ( 1) LATIN CAPITAL LETTER T WITH HOOK
    {0x01AE, 0x01AE, 0x0288}, // Ʈ ʈ ( 1) LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
    {0x01AF, 0x01AF, 0x01B0}, // Ư ư ( 1) LATIN CAPITAL LETTER U WITH HORN
    {0x01B1, 0x01B2, 0x028B}, // Ʊ ʊ ( 2) LATIN CAPITAL LETTER UPSILON
    {0x01B3, 0x01B5, 0x01B6}, // Ƴ ƴ ( 2) LATIN CAPITAL LETTER Y WITH HOOK
    {0x01B7, 0x01B7, 0x0292}, // Ʒ ʒ ( 1) LATIN CAPITAL LETTER EZH
    {0x01B8, 0x01B8, 0x01B9}, // Ƹ ƹ ( 1) LATIN CAPITAL LETTER EZH REVERSED
    {0x01BC, 0x01BC, 0x01BD}, // Ƽ ƽ ( 1) LATIN CAPITAL LETTER TONE FIVE
    {0x01C4, 0x01C4, 0x01C6}, // Ǆ ǆ ( 1) LATIN CAPITAL LETTER DZ WITH CARON
    {0x01C5, 0x01C5, 0x01C6}, // ǅ ǆ ( 1) LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
    {0x01C7, 0x01C7, 0x01C9}, // Ǉ ǉ ( 1) LATIN CAPITAL LETTER LJ
    {0x01C8, 0x01C8, 0x01C9}, // ǈ ǉ ( 1) LATIN CAPITAL LETTER L WITH SMALL LETTER J
    {0x01CA, 0x01CA, 0x01CC}, // Ǌ ǌ ( 1) LATIN CAPITAL LETTER NJ
    {0x01CB, 0x01DB, 0x01DC}, // ǋ ǌ ( 9) LATIN CAPITAL LETTER N WITH SMALL LETTER J
    {0x01DE, 0x01EE, 0x01EF}, // Ǟ ǟ ( 9) LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    {0x01F1, 0x01F1, 0x01F3}, // Ǳ ǳ ( 1) LATIN CAPITAL LETTER DZ
    {0x01F2, 0x01F4, 0x01F5}, // ǲ ǳ ( 2) LATIN CAPITAL LETTER D WITH SMALL LETTER Z
    {0x01F6, 0x01F6, 0x0195}, // Ƕ ƕ ( 1) LATIN CAPITAL LETTER HWAIR
    {0x01F7, 0x01F7, 0x01BF}, // Ƿ ƿ ( 1) LATIN CAPITAL LETTER WYNN
    {0x01F8, 0x021E, 0x021F}, // Ǹ ǹ (20) LATIN CAPITAL LETTER N WITH GRAVE
    {0x0220, 0x0220, 0x019E}, // Ƞ ƞ ( 1) LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
    {0x0222, 0x0232, 0x0233}, // Ȣ ȣ ( 9) LATIN CAPITAL LETTER OU
    {0x023A, 0x023A, 0x2C65}, // Ⱥ ⱥ ( 1) LATIN CAPITAL LETTER A WITH STROKE
    {0x023B, 0x023B, 0x023C}, // Ȼ ȼ ( 1) LATIN CAPITAL LETTER C WITH STROKE
    {0x023D, 0x023D, 0x019A}, // Ƚ ƚ ( 1) LATIN CAPITAL LETTER L WITH BAR
    {0x023E, 0x023E, 0x2C66}, // Ⱦ ⱦ ( 1) LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
    {0x0241, 0x0241, 0x0242}, // Ɂ ɂ ( 1) LATIN CAPITAL LETTER GLOTTAL STOP
    {0x0243, 0x0243, 0x0180}, // Ƀ ƀ ( 1) LATIN CAPITAL LETTER B WITH STROKE
    {0x0244, 0x0244, 0x0289}, // Ʉ ʉ ( 1) LATIN CAPITAL LETTER U BAR
    {0x0245, 0x0245, 0x028C}, // Ʌ ʌ ( 1) LATIN CAPITAL LETTER TURNED V
    {0x0246, 0x024E, 0x024F}, // Ɇ ɇ ( 5) LATIN CAPITAL LETTER E WITH STROKE
    {0x0345, 0x0345, 0x03B9}, // ͅ ι ( 1) COMBINING GREEK YPOGEGRAMMENI
    {0x0370, 0x0372, 0x0373}, // Ͱ ͱ ( 2) GREEK CAPITAL LETTER HETA
    {0x0376, 0x0376, 0x0377}, // Ͷ ͷ ( 1) GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA
    {0x037F, 0x037F, 0x03F3}, // Ϳ ϳ ( 1) GREEK CAPITAL LETTER YOT
    {0x0386, 0x0386, 0x03AC}, // Ά ά ( 1) GREEK CAPITAL LETTER ALPHA WITH TONOS
    {0x0388, 0x038A, 0x03AF}, // Έ έ ( 3) GREEK CAPITAL LETTER EPSILON WITH TONOS
    {0x038C, 0x038C, 0x03CC}, // Ό ό ( 1) GREEK CAPITAL LETTER OMICRON WITH TONOS
    {0x038E, 0x038F, 0x03CE}, // Ύ ύ ( 2) GREEK CAPITAL LETTER UPSILON WITH TONOS
    {0x0391, 0x03A1, 0x03C1}, // Α α (17) GREEK CAPITAL LETTER ALPHA
    {0x03A3, 0x03AB, 0x03CB}, // Σ σ ( 9) GREEK CAPITAL LETTER SIGMA
    {0x03C2, 0x03C2, 0x03C3}, // ς σ ( 1) GREEK SMALL LETTER FINAL SIGMA
    {0x03CF, 0x03CF, 0x03D7}, // Ϗ ϗ ( 1) GREEK CAPITAL KAI SYMBOL
    {0x03D0, 0x03D0, 0x03B2}, // ϐ β ( 1) GREEK BETA SYMBOL
    {0x03D1, 0x03D1, 0x03B8}, // ϑ θ ( 1) GREEK THETA SYMBOL
    {0x03D5, 0x03D5, 0x03C6}, // ϕ φ ( 1) GREEK PHI SYMBOL
    {0x03D6, 0x03D6, 0x03C0}, // ϖ π ( 1) GREEK PI SYMBOL
    {0x03D8, 0x03EE, 0x03EF}, // Ϙ ϙ (12) GREEK LETTER ARCHAIC KOPPA
    {0x03F0, 0x03F0, 0x03BA}, // ϰ κ ( 1) GREEK KAPPA SYMBOL
    {0x03F1, 0x03F1, 0x03C1}, // ϱ ρ ( 1) GREEK RHO SYMBOL
    {0x03F4, 0x03F4, 0x03B8}, // ϴ θ ( 1) GREEK CAPITAL THETA SYMBOL
    {0x03F5, 0x03F5, 0x03B5}, // ϵ ε ( 1) GREEK LUNATE EPSILON SYMBOL
    {0x03F7, 0x03F7, 0x03F8}, // Ϸ ϸ ( 1) GREEK CAPITAL LETTER SHO
    {0x03F9, 0x03F9, 0x03F2}, // Ϲ ϲ ( 1) GREEK CAPITAL LUNATE SIGMA SYMBOL
    {0x03FA, 0x03FA, 0x03FB}, // Ϻ ϻ ( 1) GREEK CAPITAL LETTER SAN
    {0x03FD, 0x03FF, 0x037D}, // Ͻ ͻ ( 3) GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL
    {0x0400, 0x040F, 0x045F}, // Ѐ ѐ (16) CYRILLIC CAPITAL LETTER IE WITH GRAVE
    {0x0410, 0x042F, 0x044F}, // А а (32) CYRILLIC CAPITAL LETTER A
    {0x0460, 0x0480, 0x0481}, // Ѡ ѡ (17) CYRILLIC CAPITAL LETTER OMEGA
    {0x048A, 0x04BE, 0x04BF}, // Ҋ ҋ (27) CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
    {0x04C0, 0x04C0, 0x04CF}, // Ӏ ӏ ( 1) CYRILLIC LETTER PALOCHKA
    {0x04C1, 0x04CD, 0x04CE}, // Ӂ ӂ ( 7) CYRILLIC CAPITAL LETTER ZHE WITH BREVE
    {0x04D0, 0x052E, 0x052F}, // Ӑ ӑ (48) CYRILLIC CAPITAL LETTER A WITH BREVE
    {0x0531, 0x0556, 0x0586}, // Ա ա (38) ARMENIAN CAPITAL LETTER AYB
    {0x10A0, 0x10C5, 0x2D25}, // Ⴀ ⴀ (38) GEORGIAN CAPITAL LETTER AN
    {0x10C7, 0x10C7, 0x2D27}, // Ⴧ ⴧ ( 1) GEORGIAN CAPITAL LETTER YN
    {0x10CD, 0x10CD, 0x2D2D}, // Ⴭ ⴭ ( 1) GEORGIAN CAPITAL LETTER AEN
    {0x13F8, 0x13FD, 0x13F5}, // ᏸ Ᏸ ( 6) CHEROKEE SMALL LETTER YE
    {0x1C80, 0x1C80, 0x0432}, // ᲀ в ( 1) CYRILLIC SMALL LETTER ROUNDED VE
    {0x1C81, 0x1C81, 0x0434}, // ᲁ д ( 1) CYRILLIC SMALL LETTER LONG-LEGGED DE
    {0x1C82, 0x1C82, 0x043E}, // ᲂ о ( 1) CYRILLIC SMALL LETTER NARROW O
    {0x1C83, 0x1C84, 0x0442}, // ᲃ с ( 2) CYRILLIC SMALL LETTER WIDE ES
    {0x1C85, 0x1C85, 0x0442}, // ᲅ т ( 1) CYRILLIC SMALL LETTER THREE-LEGGED TE
    {0x1C86, 0x1C86, 0x044A}, // ᲆ ъ ( 1) CYRILLIC SMALL LETTER TALL HARD SIGN
    {0x1C87, 0x1C87, 0x0463}, // ᲇ ѣ ( 1) CYRILLIC SMALL LETTER TALL YAT
    {0x1C88, 0x1C88, 0xA64B}, // ᲈ ꙋ ( 1) CYRILLIC SMALL LETTER UNBLENDED UK
    {0x1C90, 0x1CBA, 0x10FA}, // Ა ა (43) GEORGIAN MTAVRULI CAPITAL LETTER AN
    {0x1CBD, 0x1CBF, 0x10FF}, // Ჽ ჽ ( 3) GEORGIAN MTAVRULI CAPITAL LETTER AEN
    {0x1E00, 0x1E94, 0x1E95}, // Ḁ ḁ (75) LATIN CAPITAL LETTER A WITH RING BELOW
    {0x1E9B, 0x1E9B, 0x1E61}, // ẛ ṡ ( 1) LATIN SMALL LETTER LONG S WITH DOT ABOVE
    {0x1E9E, 0x1E9E, 0x00DF}, // ẞ ß ( 1) LATIN CAPITAL LETTER SHARP S
    {0x1EA0, 0x1EFE, 0x1EFF}, // Ạ ạ (48) LATIN CAPITAL LETTER A WITH DOT BELOW
    {0x1F08, 0x1F0F, 0x1F07}, // Ἀ ἀ ( 8) GREEK CAPITAL LETTER ALPHA WITH PSILI
    {0x1F18, 0x1F1D, 0x1F15}, // Ἐ ἐ ( 6) GREEK CAPITAL LETTER EPSILON WITH PSILI
    {0x1F28, 0x1F2F, 0x1F27}, // Ἠ ἠ ( 8) GREEK CAPITAL LETTER ETA WITH PSILI
    {0x1F38, 0x1F3F, 0x1F37}, // Ἰ ἰ ( 8) GREEK CAPITAL LETTER IOTA WITH PSILI
    {0x1F48, 0x1F4D, 0x1F45}, // Ὀ ὀ ( 6) GREEK CAPITAL LETTER OMICRON WITH PSILI
    {0x1F59, 0x1F5F, 0x1F57}, // Ὑ ὑ ( 7) GREEK CAPITAL LETTER UPSILON WITH DASIA
    {0x1F68, 0x1F6F, 0x1F67}, // Ὠ ὠ ( 8) GREEK CAPITAL LETTER OMEGA WITH PSILI
    {0x1F88, 0x1F8F, 0x1F87}, // ᾈ ᾀ ( 8) GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    {0x1F98, 0x1F9F, 0x1F97}, // ᾘ ᾐ ( 8) GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    {0x1FA8, 0x1FAF, 0x1FA7}, // ᾨ ᾠ ( 8) GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    {0x1FB8, 0x1FB9, 0x1FB1}, // Ᾰ ᾰ ( 2) GREEK CAPITAL LETTER ALPHA WITH VRACHY
    {0x1FBA, 0x1FBB, 0x1F71}, // Ὰ ὰ ( 2) GREEK CAPITAL LETTER ALPHA WITH VARIA
    {0x1FBC, 0x1FBC, 0x1FB3}, // ᾼ ᾳ ( 1) GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
    {0x1FBE, 0x1FBE, 0x03B9}, // ι ι ( 1) GREEK PROSGEGRAMMENI
    {0x1FC8, 0x1FCB, 0x1F75}, // Ὲ ὲ ( 4) GREEK CAPITAL LETTER EPSILON WITH VARIA
    {0x1FCC, 0x1FCC, 0x1FC3}, // ῌ ῃ ( 1) GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
    {0x1FD8, 0x1FD9, 0x1FD1}, // Ῐ ῐ ( 2) GREEK CAPITAL LETTER IOTA WITH VRACHY
    {0x1FDA, 0x1FDB, 0x1F77}, // Ὶ ὶ ( 2) GREEK CAPITAL LETTER IOTA WITH VARIA
    {0x1FE8, 0x1FE9, 0x1FE1}, // Ῠ ῠ ( 2) GREEK CAPITAL LETTER UPSILON WITH VRACHY
    {0x1FEA, 0x1FEB, 0x1F7B}, // Ὺ ὺ ( 2) GREEK CAPITAL LETTER UPSILON WITH VARIA
    {0x1FEC, 0x1FEC, 0x1FE5}, // Ῥ ῥ ( 1) GREEK CAPITAL LETTER RHO WITH DASIA
    {0x1FF8, 0x1FF9, 0x1F79}, // Ὸ ὸ ( 2) GREEK CAPITAL LETTER OMICRON WITH VARIA
    {0x1FFA, 0x1FFB, 0x1F7D}, // Ὼ ὼ ( 2) GREEK CAPITAL LETTER OMEGA WITH VARIA
    {0x1FFC, 0x1FFC, 0x1FF3}, // ῼ ῳ ( 1) GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
    {0x2126, 0x2126, 0x03C9}, // Ω ω ( 1) OHM SIGN
    {0x212A, 0x212A, 0x006B}, // K k ( 1) KELVIN SIGN
    {0x212B, 0x212B, 0x00E5}, // Å å ( 1) ANGSTROM SIGN
    {0x2132, 0x2132, 0x214E}, // Ⅎ ⅎ ( 1) TURNED CAPITAL F
    {0x2160, 0x216F, 0x217F}, // Ⅰ ⅰ (16) ROMAN NUMERAL ONE
    {0x2183, 0x2183, 0x2184}, // Ↄ ↄ ( 1) ROMAN NUMERAL REVERSED ONE HUNDRED
    {0x24B6, 0x24CF, 0x24E9}, // Ⓐ ⓐ (26) CIRCLED LATIN CAPITAL LETTER A
    {0x2C00, 0x2C2F, 0x2C5F}, // Ⰰ ⰰ (48) GLAGOLITIC CAPITAL LETTER AZU
    {0x2C60, 0x2C60, 0x2C61}, // Ⱡ ⱡ ( 1) LATIN CAPITAL LETTER L WITH DOUBLE BAR
    {0x2C62, 0x2C62, 0x026B}, // Ɫ ɫ ( 1) LATIN CAPITAL LETTER L WITH MIDDLE TILDE
    {0x2C63, 0x2C63, 0x1D7D}, // Ᵽ ᵽ ( 1) LATIN CAPITAL LETTER P WITH STROKE
    {0x2C64, 0x2C64, 0x027D}, // Ɽ ɽ ( 1) LATIN CAPITAL LETTER R WITH TAIL
    {0x2C67, 0x2C6B, 0x2C6C}, // Ⱨ ⱨ ( 3) LATIN CAPITAL LETTER H WITH DESCENDER
    {0x2C6D, 0x2C6D, 0x0251}, // Ɑ ɑ ( 1) LATIN CAPITAL LETTER ALPHA
    {0x2C6E, 0x2C6E, 0x0271}, // Ɱ ɱ ( 1) LATIN CAPITAL LETTER M WITH HOOK
    {0x2C6F, 0x2C6F, 0x0250}, // Ɐ ɐ ( 1) LATIN CAPITAL LETTER TURNED A
    {0x2C70, 0x2C70, 0x0252}, // Ɒ ɒ ( 1) LATIN CAPITAL LETTER TURNED ALPHA
    {0x2C72, 0x2C72, 0x2C73}, // Ⱳ ⱳ ( 1) LATIN CAPITAL LETTER W WITH HOOK
    {0x2C75, 0x2C75, 0x2C76}, // Ⱶ ⱶ ( 1) LATIN CAPITAL LETTER HALF H
    {0x2C7E, 0x2C7F, 0x0240}, // Ȿ ȿ ( 2) LATIN CAPITAL LETTER S WITH SWASH TAIL
    {0x2C80, 0x2CE2, 0x2CE3}, // Ⲁ ⲁ (50) COPTIC CAPITAL LETTER ALFA
    {0x2CEB, 0x2CED, 0x2CEE}, // Ⳬ ⳬ ( 2) COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI
    {0x2CF2, 0x2CF2, 0x2CF3}, // Ⳳ ⳳ ( 1) COPTIC CAPITAL LETTER BOHAIRIC KHEI
    {0xA640, 0xA66C, 0xA66D}, // Ꙁ ꙁ (23) CYRILLIC CAPITAL LETTER ZEMLYA
    {0xA680, 0xA69A, 0xA69B}, // Ꚁ ꚁ (14) CYRILLIC CAPITAL LETTER DWE
    {0xA722, 0xA72E, 0xA72F}, // Ꜣ ꜣ ( 7) LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF
    {0xA732, 0xA76E, 0xA76F}, // Ꜳ ꜳ (31) LATIN CAPITAL LETTER AA
    {0xA779, 0xA77B, 0xA77C}, // Ꝺ ꝺ ( 2) LATIN CAPITAL LETTER INSULAR D
    {0xA77D, 0xA77D, 0x1D79}, // Ᵹ ᵹ ( 1) LATIN CAPITAL LETTER INSULAR G
    {0xA77E, 0xA786, 0xA787}, // Ꝿ ꝿ ( 5) LATIN CAPITAL LETTER TURNED INSULAR G
    {0xA78B, 0xA78B, 0xA78C}, // Ꞌ ꞌ ( 1) LATIN CAPITAL LETTER SALTILLO
    {0xA78D, 0xA78D, 0x0265}, // Ɥ ɥ ( 1) LATIN CAPITAL LETTER TURNED H
    {0xA790, 0xA792, 0xA793}, // Ꞑ ꞑ ( 2) LATIN CAPITAL LETTER N WITH DESCENDER
    {0xA796, 0xA7A8, 0xA7A9}, // Ꞗ ꞗ (10) LATIN CAPITAL LETTER B WITH FLOURISH
    {0xA7AA, 0xA7AA, 0x0266}, // Ɦ ɦ ( 1) LATIN CAPITAL LETTER H WITH HOOK
    {0xA7AB, 0xA7AB, 0x025C}, // Ɜ ɜ ( 1) LATIN CAPITAL LETTER REVERSED OPEN E
    {0xA7AC, 0xA7AC, 0x0261}, // Ɡ ɡ ( 1) LATIN CAPITAL LETTER SCRIPT G
    {0xA7AD, 0xA7AD, 0x026C}, // Ɬ ɬ ( 1) LATIN CAPITAL LETTER L WITH BELT
    {0xA7AE, 0xA7AE, 0x026A}, // Ɪ ɪ ( 1) LATIN CAPITAL LETTER SMALL CAPITAL I
    {0xA7B0, 0xA7B0, 0x029E}, // Ʞ ʞ ( 1) LATIN CAPITAL LETTER TURNED K
    {0xA7B1, 0xA7B1, 0x0287}, // Ʇ ʇ ( 1) LATIN CAPITAL LETTER TURNED T
    {0xA7B2, 0xA7B2, 0x029D}, // Ʝ ʝ ( 1) LATIN CAPITAL LETTER J WITH CROSSED-TAIL
    {0xA7B3, 0xA7B3, 0xAB53}, // Ꭓ ꭓ ( 1) LATIN CAPITAL LETTER CHI
    {0xA7B4, 0xA7C2, 0xA7C3}, // Ꞵ ꞵ ( 8) LATIN CAPITAL LETTER BETA
    {0xA7C4, 0xA7C4, 0xA794}, // Ꞔ ꞔ ( 1) LATIN CAPITAL LETTER C WITH PALATAL HOOK
    {0xA7C5, 0xA7C5, 0x0282}, // Ʂ ʂ ( 1) LATIN CAPITAL LETTER S WITH HOOK
    {0xA7C6, 0xA7C6, 0x1D8E}, // Ᶎ ᶎ ( 1) LATIN CAPITAL LETTER Z WITH PALATAL HOOK
    {0xA7C7, 0xA7C9, 0xA7CA}, // Ꟈ ꟈ ( 2) LATIN CAPITAL LETTER D WITH SHORT STROKE OVERLAY
    {0xA7D0, 0xA7D0, 0xA7D1}, // Ꟑ ꟑ ( 1) LATIN CAPITAL LETTER CLOSED INSULAR G
    {0xA7D6, 0xA7D8, 0xA7D9}, // Ꟗ ꟗ ( 2) LATIN CAPITAL LETTER MIDDLE SCOTS S
    {0xA7F5, 0xA7F5, 0xA7F6}, // Ꟶ ꟶ ( 1) LATIN CAPITAL LETTER REVERSED HALF H
    {0xAB70, 0xABBF, 0x13EF}, // ꭰ Ꭰ (80) CHEROKEE SMALL LETTER A
    {0xFF21, 0xFF3A, 0xFF5A}, // Ａ ａ (26) FULLWIDTH LATIN CAPITAL LETTER A
    {0x0130, 0x0130, 0x0069}, // İ i ( 1) LATIN CAPITAL LETTER I WITH DOT ABOVE
    {0x01CD, 0x01DB, 0x01DC}, // Ǎ ǎ ( 8) LATIN CAPITAL LETTER A WITH CARON
    {0x01F4, 0x01F4, 0x01F5}, // Ǵ ǵ ( 1) LATIN CAPITAL LETTER G WITH ACUTE
    {0x13A0, 0x13EF, 0xABBF}, // Ꭰ ꭰ (80) CHEROKEE LETTER A
    {0x13F0, 0x13F5, 0x13FD}, // Ᏸ ᏸ ( 6) CHEROKEE LETTER YE
    {0x039C, 0x039C, 0x00B5}, // Μ µ ( 1) 
    {0x0049, 0x0049, 0x0131}, // I ı ( 1) 
    {0x0053, 0x0053, 0x017F}, // S ſ ( 1) 
    {0x03A3, 0x03A3, 0x03C2}, // Σ ς ( 1) 
    {0x0392, 0x0392, 0x03D0}, // Β ϐ ( 1) 
    {0x0398, 0x0398, 0x03D1}, // Θ ϑ ( 1) 
    {0x03A6, 0x03A6, 0x03D5}, // Φ ϕ ( 1) 
    {0x03A0, 0x03A0, 0x03D6}, // Π ϖ ( 1) 
    {0x039A, 0x039A, 0x03F0}, // Κ ϰ ( 1) 
    {0x03A1, 0x03A1, 0x03F1}, // Ρ ϱ ( 1) 
    {0x0395, 0x0395, 0x03F5}, // Ε ϵ ( 1) 
    {0x0412, 0x0412, 0x1C80}, // В ᲀ ( 1) 
    {0x0414, 0x0414, 0x1C81}, // Д ᲁ ( 1) 
    {0x041E, 0x041E, 0x1C82}, // О ᲂ ( 1) 
    {0x0421, 0x0422, 0x1C84}, // С ᲃ ( 2) 
    {0x0422, 0x0422, 0x1C85}, // Т ᲅ ( 1) 
    {0x042A, 0x042A, 0x1C86}, // Ъ ᲆ ( 1) 
    {0x0462, 0x0462, 0x1C87}, // Ѣ ᲇ ( 1) 
    {0xA64A, 0xA64A, 0x1C88}, // Ꙋ ᲈ ( 1) 
    {0x1E60, 0x1E60, 0x1E9B}, // Ṡ ẛ ( 1) 
    {0x0399, 0x0399, 0x1FBE}, // Ι ι ( 1) 
}; // 218

enum { casefold_len = 192 };

static uint8_t upcase_ind[162] = {
    0, 2, 3, 4, 192, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 43, 45, 193, 47, 48, 194, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
    63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 75, 80, 83, 85, 86, 87, 88, 89, 90, 91,
    92, 93, 94, 95, 96, 97, 98, 99, 195, 196, 109, 110, 111, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 125, 126, 129, 131, 132, 133, 134, 135, 136, 137, 139, 140, 141, 142, 144, 146, 147, 148,
    149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
    169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
    189, 191,
};

static uint8_t lowcase_ind[184] = {
    0, 197, 113, 2, 3, 8, 4, 198, 5, 6, 7, 9, 199, 60, 12, 14, 16, 20, 50, 25,
    57, 53, 29, 31, 33, 35, 37, 39, 40, 51, 41, 43, 45, 193, 17, 47, 48, 194, 52, 54,
    56, 158, 59, 63, 154, 152, 155, 11, 13, 15, 18, 19, 174, 21, 175, 22, 170, 173, 24, 23,
    177, 148, 176, 26, 153, 27, 28, 150, 30, 184, 32, 179, 34, 61, 36, 62, 38, 180, 178, 65,
    66, 88, 68, 69, 72, 200, 73, 70, 71, 201, 202, 203, 204, 75, 80, 205, 206, 86, 67, 207,
    85, 87, 90, 89, 91, 92, 94, 93, 95, 96, 109, 110, 196, 208, 209, 210, 211, 212, 213, 214,
    215, 167, 149, 185, 111, 216, 114, 115, 116, 117, 118, 119, 120, 121, 126, 129, 132, 136, 134, 137,
    122, 123, 124, 125, 127, 217, 130, 131, 133, 135, 138, 142, 144, 146, 147, 55, 58, 151, 156, 157,
    159, 160, 161, 97, 98, 99, 162, 163, 164, 165, 166, 168, 169, 171, 183, 172, 182, 186, 187, 188,
    189, 181, 195, 191,
};
// ### END_FILE_INCLUDE: utf8_tab.c

const uint8_t utf8_dtab[] = {
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12,
};

int utf8_encode(char *out, uint32_t c) {
    if (c < 0x80U) {
        out[0] = (char) c;
        return 1;
    } else if (c < 0x0800U) {
        out[0] = (char) ((c>>6  & 0x1F) | 0xC0);
        out[1] = (char) ((c     & 0x3F) | 0x80);
        return 2;
    } else if (c < 0x010000U) {
        if ((c < 0xD800U) | (c >= 0xE000U)) {
            out[0] = (char) ((c>>12 & 0x0F) | 0xE0);
            out[1] = (char) ((c>>6  & 0x3F) | 0x80);
            out[2] = (char) ((c     & 0x3F) | 0x80);
            return 3;
        }
    } else if (c < 0x110000U) {
        out[0] = (char) ((c>>18 & 0x07) | 0xF0);
        out[1] = (char) ((c>>12 & 0x3F) | 0x80);
        out[2] = (char) ((c>>6  & 0x3F) | 0x80);
        out[3] = (char) ((c     & 0x3F) | 0x80);
        return 4;
    }
    return 0;
}

uint32_t utf8_peek_at(const char* s, isize offset)
    { return utf8_peek(utf8_offset(s, offset)); }

bool utf8_valid_n(const char* s, isize nbytes) {
    utf8_decode_t d = {.state=0};
    while ((nbytes-- != 0) & (*s != 0))
        utf8_decode(&d, (uint8_t)*s++);
    return d.state == 0;
}

uint32_t utf8_casefold(uint32_t c) {
    for (int i=0; i < casefold_len; ++i) {
        const struct CaseMapping entry = casemappings[i];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1) == (c & 1));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

uint32_t utf8_tolower(uint32_t c) {
    for (int i=0; i < (int)(sizeof upcase_ind/sizeof *upcase_ind); ++i) {
        const struct CaseMapping entry = casemappings[upcase_ind[i]];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1) == (c & 1));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

uint32_t utf8_toupper(uint32_t c) {
    for (int i=0; i < (int)(sizeof lowcase_ind/sizeof *lowcase_ind); ++i) {
        const struct CaseMapping entry = casemappings[lowcase_ind[i]];
        if (c <= entry.m2) {
            int d = entry.m2 - entry.c2;
            if (c < (uint32_t)(entry.c1 + d)) return c;
            if (d == 1) return c - ((entry.m2 & 1) == (c & 1));
            return (uint32_t)((int)c - d);
        }
    }
    return c;
}

int utf8_icompare(const csview s1, const csview s2) {
    utf8_decode_t d1 = {.state=0}, d2 = {.state=0};
    isize j1 = 0, j2 = 0;
    while ((j1 < s1.size) & (j2 < s2.size)) {
        do { utf8_decode(&d1, (uint8_t)s1.buf[j1++]); } while (d1.state);
        do { utf8_decode(&d2, (uint8_t)s2.buf[j2++]); } while (d2.state);
        int32_t c = (int32_t)utf8_casefold(d1.codep) - (int32_t)utf8_casefold(d2.codep);
        if (c || !s2.buf[j2 - 1]) // OK if s1.size and s2.size are npos
            return (int)c;
    }
    return (int)(s1.size - s2.size);
}

#endif // STC_UTF8_PRV_C_INCLUDED
// ### END_FILE_INCLUDE: utf8_prv.c
// ### BEGIN_FILE_INCLUDE: cstr_prv.c
// ------------------- STC_CSTR_CORE --------------------
#if !defined STC_CSTR_CORE_C_INCLUDED && \
    (defined i_implement || defined STC_CSTR_CORE)
#define STC_CSTR_CORE_C_INCLUDED

size_t cstr_hash(const cstr *self) {
    csview sv = cstr_sv(self);
    return c_basehash_n(sv.buf, sv.size);
}

isize cstr_find_sv(const cstr* self, csview search) {
    csview sv = cstr_sv(self);
    char* res = c_strnstrn(sv.buf, sv.size, search.buf, search.size);
    return res ? (res - sv.buf) : c_NPOS;
}

char* _cstr_internal_move(cstr* self, const isize pos1, const isize pos2) {
    cstr_view r = cstr_getview(self);
    if (pos1 != pos2) {
        const isize newlen = (r.size + pos2 - pos1);
        if (newlen > r.cap)
            r.data = cstr_reserve(self, r.size*3/2 + pos2 - pos1);
        c_memmove(&r.data[pos2], &r.data[pos1], r.size - pos1);
        _cstr_set_size(self, newlen);
    }
    return r.data;
}

char* _cstr_init(cstr* self, const isize len, const isize cap) {
    if (cap > cstr_s_cap) {
        self->lon.data = (char *)i_malloc(cap + 1);
        cstr_l_set_size(self, len);
        cstr_l_set_cap(self, cap);
        return self->lon.data;
    }
    cstr_s_set_size(self, len);
    return self->sml.data;
}

char* cstr_reserve(cstr* self, const isize cap) {
    if (cstr_is_long(self)) {
        if (cap > cstr_l_cap(self)) {
            self->lon.data = (char *)i_realloc(self->lon.data, cstr_l_cap(self) + 1, cap + 1);
            cstr_l_set_cap(self, cap);
        }
        return self->lon.data;
    }
    /* from short to long: */
    if (cap > cstr_s_cap) {
        char* data = (char *)i_malloc(cap + 1);
        const isize len = cstr_s_size(self);
        /* copy full short buffer to emulate realloc() */
        c_memcpy(data, self->sml.data, c_sizeof self->sml);
        self->lon.data = data;
        self->lon.size = (size_t)len;
        cstr_l_set_cap(self, cap);
        return data;
    }
    return self->sml.data;
}

char* cstr_resize(cstr* self, const isize size, const char value) {
    cstr_view r = cstr_getview(self);
    if (size > r.size) {
        if (size > r.cap && (r.data = cstr_reserve(self, size)) == NULL)
            return NULL;
        c_memset(r.data + r.size, value, size - r.size);
    }
    _cstr_set_size(self, size);
    return r.data;
}

isize cstr_find_at(const cstr* self, const isize pos, const char* search) {
    csview sv = cstr_sv(self);
    if (pos > sv.size) return c_NPOS;
    const char* res = strstr((char*)sv.buf + pos, search);
    return res ? (res - sv.buf) : c_NPOS;
}

char* cstr_assign_n(cstr* self, const char* str, const isize len) {
    char* d = cstr_reserve(self, len);
    if (d) { _cstr_set_size(self, len); c_memmove(d, str, len); }
    return d;
}

char* cstr_append_n(cstr* self, const char* str, const isize len) {
    cstr_view r = cstr_getview(self);
    if (r.size + len > r.cap) {
        const size_t off = (size_t)(str - r.data);
        r.data = cstr_reserve(self, r.size*3/2 + len);
        if (r.data == NULL) return NULL;
        if (off <= (size_t)r.size) str = r.data + off; /* handle self append */
    }
    c_memcpy(r.data + r.size, str, len);
    _cstr_set_size(self, r.size + len);
    return r.data;
}

cstr cstr_from_replace(csview in, csview search, csview repl, int32_t count) {
    cstr out = cstr_init();
    isize from = 0; char* res;
    if (count == 0) count = INT32_MAX;
    if (search.size)
        while (count-- && (res = c_strnstrn(in.buf + from, in.size - from, search.buf, search.size))) {
            const isize pos = (res - in.buf);
            cstr_append_n(&out, in.buf + from, pos - from);
            cstr_append_n(&out, repl.buf, repl.size);
            from = pos + search.size;
        }
    cstr_append_n(&out, in.buf + from, in.size - from);
    return out;
}

void cstr_erase(cstr* self, const isize pos, isize len) {
    cstr_view r = cstr_getview(self);
    if (len > r.size - pos) len = r.size - pos;
    c_memmove(&r.data[pos], &r.data[pos + len], r.size - (pos + len));
    _cstr_set_size(self, r.size - len);
}

void cstr_shrink_to_fit(cstr* self) {
    cstr_view r = cstr_getview(self);
    if (r.size == r.cap)
        return;
    if (r.size > cstr_s_cap) {
        self->lon.data = (char *)i_realloc(self->lon.data, cstr_l_cap(self) + 1, r.size + 1);
        cstr_l_set_cap(self, r.size);
    } else if (r.cap > cstr_s_cap) {
        c_memcpy(self->sml.data, r.data, r.size + 1);
        cstr_s_set_size(self, r.size);
        i_free(r.data, r.cap + 1);
    }
}
#endif // STC_CSTR_CORE_C_INCLUDED

// ------------------- STC_CSTR_IO --------------------
#if !defined STC_CSTR_IO_C_INCLUDED && \
    (defined i_import || defined STC_CSTR_IO)
#define STC_CSTR_IO_C_INCLUDED

#include <stdarg.h>

char* cstr_append_uninit(cstr *self, isize len) {
    cstr_view r = cstr_getview(self);
    if (r.size + len > r.cap && (r.data = cstr_reserve(self, r.size*3/2 + len)) == NULL)
        return NULL;
    _cstr_set_size(self, r.size + len);
    return r.data + r.size;
}

bool cstr_getdelim(cstr *self, const int delim, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    isize pos = 0;
    cstr_view r = cstr_getview(self);
    for (;;) {
        if (c == delim || c == EOF) {
            _cstr_set_size(self, pos);
            return true;
        }
        if (pos == r.cap) {
            _cstr_set_size(self, pos);
            r.data = cstr_reserve(self, (r.cap = r.cap*3/2 + 16));
        }
        r.data[pos++] = (char) c;
        c = fgetc(fp);
    }
}

static isize cstr_vfmt(cstr* self, isize start, const char* fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);
    const int n = vsnprintf(NULL, 0ULL, fmt, args);
    vsnprintf(cstr_reserve(self, start + n) + start, (size_t)n+1, fmt, args2);
    va_end(args2);
    _cstr_set_size(self, start + n);
    return n;
}

cstr cstr_from_fmt(const char* fmt, ...) {
    cstr s = cstr_init();
    va_list args;
    va_start(args, fmt);
    cstr_vfmt(&s, 0, fmt, args);
    va_end(args);
    return s;
}

isize cstr_append_fmt(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const isize n = cstr_vfmt(self, cstr_size(self), fmt, args);
    va_end(args);
    return n;
}

/* NB! self-data in args is UB */
isize cstr_printf(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const isize n = cstr_vfmt(self, 0, fmt, args);
    va_end(args);
    return n;
}
#endif // STC_CSTR_IO_C_INCLUDED

// ------------------- STC_CSTR_UTF8 --------------------
#if !defined STC_CSTR_UTF8_C_INCLUDED && \
    (defined i_import || defined STC_CSTR_UTF8 || defined STC_UTF8_PRV_C_INCLUDED)
#define STC_CSTR_UTF8_C_INCLUDED

#include <ctype.h>

void cstr_u8_erase(cstr* self, const isize u8pos, const isize u8len) {
    csview r = cstr_sv(self);
    csview span = utf8_subview(r.buf, u8pos, u8len);
    c_memmove((void *)&span.buf[0], &span.buf[span.size], r.size - span.size - (span.buf - r.buf));
    _cstr_set_size(self, r.size - span.size);
}

bool cstr_u8_valid(const cstr* self)
    { return utf8_valid(cstr_str(self)); }

static struct {
    int      (*conv_asc)(int);
    uint32_t (*conv_utf)(uint32_t);
}
fn_tocase[] = {{tolower, utf8_casefold},
               {tolower, utf8_tolower},
               {toupper, utf8_toupper}};

cstr cstr_tocase_sv(csview sv, int k) {
    cstr out = {0};
    char *buf = cstr_reserve(&out, sv.size*3/2);
    const char *end = sv.buf + sv.size;
    uint32_t cp; isize sz = 0;
    utf8_decode_t d = {.state=0};

    while (sv.buf < end) {
        do { utf8_decode(&d, (uint8_t)*sv.buf++); } while (d.state);
        if (d.codep < 128)
            buf[sz++] = (char)fn_tocase[k].conv_asc((int)d.codep);
        else {
            cp = fn_tocase[k].conv_utf(d.codep);
            sz += utf8_encode(buf + sz, cp);
        }
    }
    _cstr_set_size(&out, sz);
    cstr_shrink_to_fit(&out);
    return out;
}
#endif // i_import STC_CSTR_UTF8_C_INCLUDED
// ### END_FILE_INCLUDE: cstr_prv.c
  #endif
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
#endif
// ### END_FILE_INCLUDE: cregex.h


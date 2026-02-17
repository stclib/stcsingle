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

#ifndef ISIZE_MAX
    typedef ptrdiff_t   isize_t;
    typedef isize_t     isize; // [deprecated]
    #define ISIZE_MIN   PTRDIFF_MIN
    #define ISIZE_MAX   PTRDIFF_MAX
#endif
#if !defined STC_HAS_TYPEOF && (_MSC_FULL_VER >= 193933428 || \
    defined __GNUC__ || defined __clang__ || defined __TINYC__)
    #define STC_HAS_TYPEOF 1
#endif
#if defined __GNUC__
  #define c_GNUATTR(...) __attribute__((__VA_ARGS__))
#else
  #define c_GNUATTR(...)
#endif
#define STC_INLINE static inline c_GNUATTR(unused)
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR
#define c_NPOS INTPTR_MAX

// Macro overloading feature support
#define c_MACRO_OVERLOAD(name, ...) \
    c_JOIN(name ## _,c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_JOIN0(a, b) a ## b
#define c_JOIN(a, b) c_JOIN0(a, b)
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) _c_ARG_N args
#define _c_RSEQ_N 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
#define _c_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) N

// Saturated overloading
// #define foo(...) foo_I(__VA_ARGS__, c_COMMA_N(foo_3), c_COMMA_N(foo_2), c_COMMA_N(foo_1),)(__VA_ARGS__)
// #define foo_I(a,b,c, n, ...) c_TUPLE_AT_1(n, foo_n,)
#define c_TUPLE_AT_1(x,y,...) y
#define c_COMMA_N(x) ,x
#define c_EXPAND(...) __VA_ARGS__

// Select arg, e.g. for #define i_type A,B then c_GETARG(2, i_type) is B
#define c_GETARG(N, ...) c_ARG_##N(__VA_ARGS__,)
#define c_ARG_1(a, ...) a
#define c_ARG_2(a, b, ...) b
#define c_ARG_3(a, b, c, ...) c
#define c_ARG_4(a, b, c, d, ...) d

#define _i_new_n(T, n) ((T*)i_malloc((n)*c_sizeof(T)))
#define _i_new_zeros(T, n) ((T*)i_calloc(n, c_sizeof(T)))
#define _i_realloc_n(ptr, old_n, n) i_realloc(ptr, (old_n)*c_sizeof *(ptr), (n)*c_sizeof *(ptr))
#define _i_free_n(ptr, n) i_free(ptr, (n)*c_sizeof *(ptr))

#ifndef __cplusplus
    #define c_new(T, ...) ((T*)c_safe_memcpy(c_malloc(c_sizeof(T)), ((T[]){__VA_ARGS__}), c_sizeof(T)))
    #define c_literal(T) (T)
    #define c_make_array(T, ...) ((T[])__VA_ARGS__)
    #define c_make_array2d(T, N, ...) ((T[][N])__VA_ARGS__)
#else
    #include <new>
    #define c_new(T, ...) new (c_malloc(c_sizeof(T))) T(__VA_ARGS__)
    #define c_literal(T) T
    template<typename T, int M, int N> struct _c_Array { T data[M][N]; };
    #define c_make_array(T, ...) (_c_Array<T, 1, sizeof((T[])__VA_ARGS__)/sizeof(T)>{{__VA_ARGS__}}.data[0])
    #define c_make_array2d(T, N, ...) (_c_Array<T, sizeof((T[][N])__VA_ARGS__)/sizeof(T[N]), N>{__VA_ARGS__}.data)
#endif

#ifdef STC_ALLOCATOR
    #define c_malloc c_JOIN(STC_ALLOCATOR, _malloc)
    #define c_calloc c_JOIN(STC_ALLOCATOR, _calloc)
    #define c_realloc c_JOIN(STC_ALLOCATOR, _realloc)
    #define c_free c_JOIN(STC_ALLOCATOR, _free)
#else
    #define c_malloc(sz) malloc(c_i2u_size(sz))
    #define c_calloc(n, sz) calloc(c_i2u_size(n), c_i2u_size(sz))
    #define c_realloc(ptr, old_sz, sz) realloc(ptr, c_i2u_size(1 ? (sz) : (old_sz)))
    #define c_free(ptr, sz) ((void)(sz), free(ptr))
#endif

#define c_new_n(T, n) ((T*)c_malloc((n)*c_sizeof(T)))
#define c_free_n(ptr, n) c_free(ptr, (n)*c_sizeof *(ptr))
#define c_realloc_n(ptr, old_n, n) c_realloc(ptr, (old_n)*c_sizeof *(ptr), (n)*c_sizeof *(ptr))
#define c_delete_n(T, ptr, n) do { \
    T* _tp = ptr; isize_t _n = n, _i = _n; \
    while (_i--) T##_drop((_tp + _i)); \
    c_free(_tp, _n*c_sizeof(T)); \
} while (0)

#define c_static_assert(expr)   (void)sizeof(int[(expr) ? 1 : -1])
#if defined STC_NDEBUG || defined NDEBUG
    #define c_assert(expr)      (void)sizeof(expr)
#else
    #define c_assert(expr)      assert(expr)
#endif
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(Tp, p)     ((Tp)(1 ? (p) : (Tp)0))
#define c_litstrlen(literal)    (c_sizeof("" literal) - 1)
#define c_countof(a)            (isize_t)(sizeof(a)/sizeof 0[a])
#define c_arraylen(a)           c_countof(a) // [deprecated]?

// expect signed ints to/from these (use with gcc -Wconversion)
#define c_sizeof                (isize_t)sizeof
#define c_strlen(s)             (isize_t)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u_size(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u_size(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u_size(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u_size(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u_size(ilen))
// library internal, but may be useful in user code:
#define c_u2i_size(u)           (isize_t)(1 ? (u) : (size_t)1) // warns if u is signed
#define c_i2u_size(i)           (size_t)(1 ? (i) : -1)       // warns if i is unsigned
#define c_uless(a, b)           ((size_t)(a) < (size_t)(b))
#define c_safe_cast(T, From, x) ((T)(1 ? (x) : (From){0}))

// x, y are i_keyraw* type, which defaults to i_key*. vp is i_key* type.
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(vp)      c_hash_n(vp, sizeof *(vp))
#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

// Control block macros

// [deprecated]:
#define c_init(...) c_make(__VA_ARGS__)
#define c_forlist(...) for (c_items(_VA_ARGS__))
#define c_foritems(...) for (c_items(__VA_ARGS__))
#define c_foreach(...) for (c_each(__VA_ARGS__))
#define c_foreach_n(...) for (c_each_n(__VA_ARGS__))
#define c_foreach_kv(...) for (c_each_kv(__VA_ARGS__))
#define c_foreach_reverse(...) for (c_each_reverse(__VA_ARGS__))
#define c_forrange(...) for (c_range(__VA_ARGS__))
#define c_forrange32(...) for (c_range32(__VA_ARGS__))

// New:
#define c_each(...) c_MACRO_OVERLOAD(c_each, __VA_ARGS__)
#define c_each_3(it, C, cnt) \
    C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it)
#define c_each_4(it, C, start, end) \
    _c_each(it, C, start, (end).ref, _)

#define c_each_ref(v, C, cnt) \
    C##_value* v = (C##_value*)&v; v; ) \
    for (C##_iter v##_itr_ = C##_begin(&cnt); (v = v##_itr_.ref); C##_next(&v##_itr_)
#define c_each_item(...) c_each_ref(__VA_ARGS__) // [deprecated]

#define c_each_n(...) c_MACRO_OVERLOAD(c_each_n, __VA_ARGS__)
#define c_each_n_3(it, C, cnt) c_each_n_4(it, C, cnt, INTPTR_MAX)
#define c_each_n_4(it, C, cnt, n) \
    struct {C##_iter iter; C##_value* ref; isize_t size, index;} \
    it = {.iter=C##_begin(&cnt), .size=n}; (it.ref = it.iter.ref) && it.index < it.size; C##_next(&it.iter), ++it.index

#define c_each_reverse(...) c_MACRO_OVERLOAD(c_each_reverse, __VA_ARGS__)
#define c_each_reverse_3(it, C, cnt) /* works for stack, vec, queue, deque */ \
    C##_iter it = C##_rbegin(&cnt); it.ref; C##_rnext(&it)
#define c_each_reverse_4(it, C, start, end) \
    _c_each(it, C, start, (end).ref, _r)

#define _c_each(it, C, start, endref, rev) /* private */ \
    C##_iter it = (start), *_endref_##it = c_safe_cast(C##_iter*, C##_value*, endref) \
         ; it.ref != (C##_value*)_endref_##it; C##rev##next(&it)

#define c_each_kv(...) c_MACRO_OVERLOAD(c_each_kv, __VA_ARGS__)
#define c_each_kv_4(key, val, C, cnt) /* structured binding for maps */ \
    _c_each_kv(key, val, C, C##_begin(&cnt), NULL)
#define c_each_kv_5(key, val, C, start, end) \
    _c_each_kv(key, val, C, start, (end).ref)

#define _c_each_kv(key, val, C, start, endref) /* private */ \
    const C##_key *key = (const C##_key*)&key; key; ) \
    for (C##_mapped *val; key; key = NULL) \
    for (C##_iter _it_##key = start, *_endref_##key = c_safe_cast(C##_iter*, C##_value*, endref); \
         _it_##key.ref != (C##_value*)_endref_##key && (key = &_it_##key.ref->first, val = &_it_##key.ref->second); \
         C##_next(&_it_##key)

#define c_items(it, T, ...) \
    struct {T* ref; int size, index;} \
    it = {.ref=c_make_array(T, __VA_ARGS__), .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
    ; it.index < it.size ; ++it.ref, ++it.index

// c_range, c_range32: python-like int range iteration
#define c_range_t(...) c_MACRO_OVERLOAD(c_range_t, __VA_ARGS__)
#define c_range_t_3(T, i, stop) c_range_t_4(T, i, 0, stop)
#define c_range_t_4(T, i, start, stop) \
    T i=start, _c_end_##i=stop; i < _c_end_##i; ++i
#define c_range_t_5(T, i, start, stop, step) \
    T i=start, _c_inc_##i=step, _c_end_##i=(stop) - (_c_inc_##i > 0) \
    ; (_c_inc_##i > 0) == (i <= _c_end_##i) ; i += _c_inc_##i

#define c_range(...) c_MACRO_OVERLOAD(c_range, __VA_ARGS__)
#define c_range_1(stop) c_range_t_4(isize_t, _c_i1, 0, stop)
#define c_range_2(i, stop) c_range_t_4(isize_t, i, 0, stop)
#define c_range_3(i, start, stop) c_range_t_4(isize_t, i, start, stop)
#define c_range_4(i, start, stop, step) c_range_t_5(isize_t, i, start, stop, step)

#define c_range32(...) c_MACRO_OVERLOAD(c_range32, __VA_ARGS__)
#define c_range32_2(i, stop) c_range_t_4(int32_t, i, 0, stop)
#define c_range32_3(i, start, stop) c_range_t_4(int32_t, i, start, stop)
#define c_range32_4(i, start, stop, step) c_range_t_5(int32_t, i, start, stop, step)

// make container from a literal list
#define c_make(C, ...) \
    C##_from_n(c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// put multiple raw-type elements from a literal list into a container
#define c_put_items(C, cnt, ...) \
    C##_put_n(cnt, c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

// drop multiple containers of same type
#define c_drop(C, ...) \
    do { for (c_items(_c_i2, C*, {__VA_ARGS__})) C##_drop(*_c_i2.ref); } while(0)

// RAII scopes
#define c_defer(...) \
    for (int _c_i3 = 0; _c_i3++ == 0; __VA_ARGS__)

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(init, deinit) \
    for (int _c_i4 = 0; _c_i4 == 0; ) for (init; _c_i4++ == 0; deinit)
#define c_with_3(init, condition, deinit) \
    for (int _c_i5 = 0; _c_i5 == 0; ) for (init; _c_i5++ == 0 && (condition); deinit)

// General functions

STC_INLINE void* c_safe_memcpy(void* dst, const void* src, isize_t size)
    { return dst ? memcpy(dst, src, (size_t)size) : NULL; }

#if INTPTR_MAX == INT64_MAX
    #define FNV_BASIS 0xcbf29ce484222325
    #define FNV_PRIME 0x00000100000001b3
#else
    #define FNV_BASIS 0x811c9dc5
    #define FNV_PRIME 0x01000193
#endif

STC_INLINE size_t c_basehash_n(const void* key, isize_t len) {
    const uint8_t* msg = (const uint8_t*)key;
    size_t h = FNV_BASIS, block = 0;

    while (len >= c_sizeof h) {
        memcpy(&block, msg, sizeof h);
        h ^= block;
        h *= FNV_PRIME;
        msg += c_sizeof h;
        len -= c_sizeof h;
    }
    while (len--) {
        h ^= *(msg++);
        h *= FNV_PRIME;
    }
    return h;
}

STC_INLINE size_t c_hash_n(const void* key, isize_t len) {
    uint64_t b8; uint32_t b4;
    switch (len) {
        case 8: memcpy(&b8, key, 8); return (size_t)(b8 * 0xc6a4a7935bd1e99d);
        case 4: memcpy(&b4, key, 4); return b4 * FNV_BASIS;
        default: return c_basehash_n(key, len);
    }
}

STC_INLINE size_t c_hash_str(const char *str) {
    const uint8_t* msg = (const uint8_t*)str;
    size_t h = FNV_BASIS;
    while (*msg) {
        h ^= *(msg++);
        h *= FNV_PRIME;
    }
    return h;
}

#define c_hash_mix(...) /* non-commutative hash combine */ \
    c_hash_mix_n(c_make_array(size_t, {__VA_ARGS__}), c_sizeof((size_t[]){__VA_ARGS__})/c_sizeof(size_t))

STC_INLINE size_t c_hash_mix_n(size_t h[], isize_t n) {
    for (isize_t i = 1; i < n; ++i) h[0] += h[0] ^ h[i];
    return h[0];
}

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
STC_INLINE isize_t c_next_pow2(isize_t n) {
    n--;
    n |= n >> 1, n |= n >> 2;
    n |= n >> 4, n |= n >> 8;
    n |= n >> 16;
    #if INTPTR_MAX == INT64_MAX
    n |= n >> 32;
    #endif
    return n + 1;
}

STC_INLINE char* c_strnstrn(const char *str, isize_t slen, const char *needle, isize_t nlen) {
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
#endif // STC_COMMON_H_INCLUDED
// ### END_FILE_INCLUDE: common.h
// ### BEGIN_FILE_INCLUDE: types.h

#ifndef STC_TYPES_H_INCLUDED
#define STC_TYPES_H_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define declare_rc(C, KEY) declare_arc(C, KEY)
#define declare_list(C, KEY) _declare_list(C, KEY,)
#define declare_stack(C, KEY) _declare_stack(C, KEY,)
#define declare_vec(C, KEY) _declare_stack(C, KEY,)
#define declare_pqueue(C, KEY) _declare_stack(C, KEY,)
#define declare_queue(C, KEY) _declare_queue(C, KEY,)
#define declare_deque(C, KEY) _declare_queue(C, KEY,)
#define declare_hashmap(C, KEY, VAL) _declare_htable(C, KEY, VAL, c_true, c_false,)
#define declare_hashset(C, KEY) _declare_htable(C, KEY, KEY, c_false, c_true,)
#define declare_sortedmap(C, KEY, VAL) _declare_aatree(C, KEY, VAL, c_true, c_false,)
#define declare_sortedset(C, KEY) _declare_aatree(C, KEY, KEY, c_false, c_true,)

#define declare_list_aux(C, KEY, AUX) _declare_list(C, KEY, AUX aux;)
#define declare_stack_aux(C, KEY, AUX) _declare_stack(C, KEY, AUX aux;)
#define declare_vec_aux(C, KEY, AUX) _declare_stack(C, KEY, AUX aux;)
#define declare_pqueue_aux(C, KEY, AUX) _declare_stack(C, KEY, AUX aux;)
#define declare_queue_aux(C, KEY, AUX) _declare_queue(C, KEY, AUX aux;)
#define declare_deque_aux(C, KEY, AUX) _declare_queue(C, KEY, AUX aux;)
#define declare_hashmap_aux(C, KEY, VAL, AUX) _declare_htable(C, KEY, VAL, c_true, c_false, AUX aux;)
#define declare_hashset_aux(C, KEY, AUX) _declare_htable(C, KEY, KEY, c_false, c_true, AUX aux;)
#define declare_sortedmap_aux(C, KEY, VAL, AUX) _declare_aatree(C, KEY, VAL, c_true, c_false, AUX aux;)
#define declare_sortedset_aux(C, KEY, AUX) _declare_aatree(C, KEY, KEY, c_false, c_true, AUX aux;)

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
typedef struct { cstr_value* data; intptr_t size, cap; } cstr_buf;
typedef union cstr {
    struct { cstr_buf *a, *b, *c; } _dummy;
    struct { cstr_value* data; uintptr_t size; uintptr_t ncap; } lon;
    struct { cstr_value data[ sizeof(cstr_buf) - 1 ]; uint8_t size; } sml;
} cstr;

typedef union {
    csview chr; // utf8 character/codepoint
    const cstr_value* ref;
} cstr_iter;

#define c_true(...) __VA_ARGS__
#define c_false(...)

#define declare_arc(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF##_ctrl SELF##_ctrl; \
\
    typedef union SELF { \
        SELF##_value* get; \
        SELF##_ctrl* ctrl; \
    } SELF

#define declare_arc2(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF##_ctrl SELF##_ctrl; \
\
    typedef struct SELF { \
        SELF##_value* get; \
        SELF##_ctrl* ctrl2; \
    } SELF

#define declare_box(SELF, VAL) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value* get; \
    } SELF

#define _declare_queue(SELF, VAL, AUXDEF) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value *cbuf; \
        ptrdiff_t start, end, capmask; \
        AUXDEF \
    } SELF; \
\
    typedef struct { \
        SELF##_value *ref; \
        ptrdiff_t pos; \
        const SELF* _s; \
    } SELF##_iter

#define _declare_list(SELF, VAL, AUXDEF) \
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
        AUXDEF \
    } SELF

#define _declare_htable(SELF, KEY, VAL, MAP_ONLY, SET_ONLY, AUXDEF) \
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
        AUXDEF \
    } SELF

#define _declare_aatree(SELF, KEY, VAL, MAP_ONLY, SET_ONLY, AUXDEF) \
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
        AUXDEF \
    } SELF

#define _declare_inplace_stack(SELF, VAL, CAP, AUXDEF) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { ptrdiff_t size; SELF##_value data[CAP]; AUXDEF } SELF

#define _declare_stack(SELF, VAL, AUXDEF) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value *data; ptrdiff_t size, capacity; AUXDEF } SELF

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

#define c_match(it, re, str) \
    cregex_iter it = {.regex=re, .input={str}, .match={{0}}}; \
    cregex_match(it.regex, it.input.buf, it.match, CREG_NEXT) == CREG_OK && it.match[0].size;

#define c_match_sv(it, re, strview) \
    cregex_iter it = {.regex=re, .input=strview, .match={{0}}}; \
    cregex_match_sv(it.regex, it.input, it.match, CREG_NEXT) == CREG_OK && it.match[0].size;

/* compile a regex from a pattern. return CREG_OK, or negative error code on failure. */
extern int cregex_compile_pro(cregex *re, const char* pattern, int cflags);

#define cregex_compile(...) \
    c_ARG_4(__VA_ARGS__, cregex_compile_pro(__VA_ARGS__), cregex_compile_pro(__VA_ARGS__, CREG_DEFAULT), _too_few_args_)

/* construct and return a regex from a pattern. return CREG_OK, or negative error code on failure. */
STC_INLINE cregex cregex_make(const char* pattern, int cflags) {
    cregex re = {0};
    cregex_compile_pro(&re, pattern, cflags);
    return re;
}
STC_INLINE cregex cregex_from(const char* pattern)
    { return cregex_make(pattern, CREG_DEFAULT); }

/* destroy regex */
extern void cregex_drop(cregex* re);

/* number of capture groups in a regex pattern, excluding the full match capture (0) */
extern int cregex_captures(const cregex* re);

/* ----- Private ----- */

struct cregex_match_opt { csview* match; int flags; int _dummy; };
struct cregex_replace_opt { int count; bool(*xform)(int group, csview match, cstr* out); int flags; int _dummy; };

extern int cregex_match_opt(const cregex* re, const char* input, const char* input_end, struct cregex_match_opt opt);
extern int cregex_match_aio_opt(const char* pattern, const char* input, const char* input_end, struct cregex_match_opt opt);
extern cstr cregex_replace_opt(const cregex* re, const char* input, const char* input_end, const char* replace, struct cregex_replace_opt opt);
extern cstr cregex_replace_aio_opt(const char* pattern, const char* input, const char* input_end, const char* replace, struct cregex_replace_opt opt);

static inline int cregex_match_sv_opt(const cregex* re, csview sv, struct cregex_match_opt opt)
    { return cregex_match_opt(re, sv.buf, sv.buf+sv.size, opt); }
static inline int cregex_match_aio_sv_opt(const char* pattern, csview sv, struct cregex_match_opt opt)
    { return cregex_match_aio_opt(pattern, sv.buf, sv.buf+sv.size, opt); }
static inline cstr cregex_replace_sv_opt(const cregex* re, csview sv, const char* replace, struct cregex_replace_opt opt)
    { return cregex_replace_opt(re, sv.buf, sv.buf+sv.size, replace, opt); }
static inline cstr cregex_replace_aio_sv_opt(const char* pattern, csview sv, const char* replace, struct cregex_replace_opt opt)
    { return cregex_replace_aio_opt(pattern, sv.buf, sv.buf+sv.size, replace, opt); }

/* match: return CREG_OK, CREG_NOMATCH or CREG_MATCHERROR. */
#define _cregex_match(re, str, ...) cregex_match_opt(re, str, NULL, (struct cregex_match_opt){__VA_ARGS__})
#define _cregex_match_sv(re, sv, ...) cregex_match_sv_opt(re, sv, (struct cregex_match_opt){__VA_ARGS__})
/* all-in-one: compile RE pattern + match + free */
#define _cregex_match_aio(pattern, str, ...) cregex_match_aio_opt(pattern, str, NULL, (struct cregex_match_opt){__VA_ARGS__})
#define _cregex_match_aio_sv(pattern, sv, ...) cregex_match_aio_sv_opt(pattern, sv, (struct cregex_match_opt){__VA_ARGS__})

/* replace input with a string using regular expression */
#define _cregex_replace(re, str, replace, ...) cregex_replace_opt(re, str, NULL, replace, (struct cregex_replace_opt){__VA_ARGS__})
#define _cregex_replace_sv(re, sv, replace, ...) cregex_replace_sv_opt(re, sv, replace, (struct cregex_replace_opt){__VA_ARGS__})
/* all-in-one: compile RE string pattern + match + replace + free */
#define _cregex_replace_aio(pattern, str, replace, ...) cregex_replace_aio_opt(pattern, str, NULL, replace, (struct cregex_replace_opt){__VA_ARGS__})
#define _cregex_replace_aio_sv(pattern, sv, replace, ...) cregex_replace_aio_sv_opt(pattern, sv, replace, (struct cregex_replace_opt){__VA_ARGS__})

/* ----- API functions ---- */

#define cregex_match(...) _cregex_match(__VA_ARGS__, ._dummy=0)
#define cregex_match_sv(...) _cregex_match_sv(__VA_ARGS__, ._dummy=0)
#define cregex_match_aio(...) _cregex_match_aio(__VA_ARGS__, ._dummy=0)
#define cregex_match_aio_sv(...) _cregex_match_aio_sv(__VA_ARGS__, ._dummy=0)
#define cregex_is_match(re, str) (_cregex_match(re, str, 0) == CREG_OK)

#define cregex_replace(...) _cregex_replace(__VA_ARGS__, ._dummy=0)
#define cregex_replace_sv(...) _cregex_replace_sv(__VA_ARGS__, ._dummy=0)
#define cregex_replace_aio(...) _cregex_replace_aio(__VA_ARGS__, ._dummy=0)
#define cregex_replace_aio_sv(...) _cregex_replace_aio_sv(__VA_ARGS__, ._dummy=0)

#endif // STC_CREGEX_H_INCLUDED

#if defined STC_IMPLEMENT || defined i_implement || defined i_import
// ### BEGIN_FILE_INCLUDE: linkage.h
#undef STC_API
#undef STC_DEF

#if !defined i_static && !defined STC_STATIC && (defined i_header || defined STC_HEADER  || \
                                                 defined i_implement || defined STC_IMPLEMENT)
  #define STC_API extern
  #define STC_DEF
#else
  #define i_implement
  #if defined __GNUC__ || defined __clang__ || defined __INTEL_LLVM_COMPILER
    #define STC_API static __attribute__((unused))
  #else
    #define STC_API static inline
  #endif
  #define STC_DEF static
#endif
#if defined STC_IMPLEMENT || defined i_import
  #define i_implement
#endif

#if defined i_aux && defined i_allocator
  #define _i_aux_alloc
#endif
#ifndef i_allocator
  #define i_allocator c
#endif
#ifndef i_free
  #define i_malloc c_JOIN(i_allocator, _malloc)
  #define i_calloc c_JOIN(i_allocator, _calloc)
  #define i_realloc c_JOIN(i_allocator, _realloc)
  #define i_free c_JOIN(i_allocator, _free)
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

#include <ctype.h>

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
STC_INLINE isize_t utf8_count(const char *s) {
    isize_t size = 0;
    while (*s)
        size += (*++s & 0xC0) != 0x80;
    return size;
}

STC_INLINE isize_t utf8_count_n(const char *s, isize_t nbytes) {
    isize_t size = 0;
    while ((nbytes-- != 0) & (*s != 0)) {
        size += (*++s & 0xC0) != 0x80;
    }
    return size;
}

STC_INLINE const char* utf8_at(const char *s, isize_t u8pos) {
    while ((u8pos > 0) & (*s != 0))
        u8pos -= (*++s & 0xC0) != 0x80;
    return s;
}

STC_INLINE const char* utf8_offset(const char* s, isize_t u8pos) {
    int inc = 1;
    if (u8pos < 0) u8pos = -u8pos, inc = -1;
    while (u8pos && *s)
        u8pos -= (*(s += inc) & 0xC0) != 0x80;
    return s;
}

STC_INLINE isize_t utf8_to_index(const char* s, isize_t u8pos)
    { return utf8_at(s, u8pos) - s; }

STC_INLINE csview utf8_subview(const char *s, isize_t u8pos, isize_t u8len) {
    csview span;
    span.buf = utf8_at(s, u8pos);
    span.size = utf8_to_index(span.buf, u8len);
    return span;
}

// ------------------------------------------------------
// Functions below must be linked with utf8_prv.c content
// To call them, either define i_import before including
// one of cstr, csview, zsview, or link with src/libstc.a

/* decode next utf8 codepoint. https://bjoern.hoehrmann.de/utf-8/decoder/dfa */
typedef struct { uint32_t state, codep; } utf8_decode_t;
extern const uint8_t utf8_dtab[]; /* utf8code.c */
#define utf8_ACCEPT 0
#define utf8_REJECT 12

extern bool     utf8_valid(const char* s);
extern bool     utf8_valid_n(const char* s, isize_t nbytes);
extern int      utf8_encode(char *out, uint32_t c);
extern int      utf8_decode_codepoint(utf8_decode_t* d, const char* s, const char* end);
extern int      utf8_icompare(const csview s1, const csview s2);
extern uint32_t utf8_peek_at(const char* s, isize_t u8offset);
extern uint32_t utf8_casefold(uint32_t c);
extern uint32_t utf8_tolower(uint32_t c);
extern uint32_t utf8_toupper(uint32_t c);

STC_INLINE bool utf8_isupper(uint32_t c)
    { return c < 128 ? (c >= 'A') & (c <= 'Z') : utf8_tolower(c) != c; }

STC_INLINE bool utf8_islower(uint32_t c)
    { return c < 128 ? (c >= 'a') & (c <= 'z') : utf8_toupper(c) != c; }

STC_INLINE uint32_t utf8_decode(utf8_decode_t* d, const uint32_t byte) {
    const uint32_t type = utf8_dtab[byte];
    d->codep = d->state ? (byte & 0x3fu) | (d->codep << 6)
                        : (0xffU >> type) & byte;
    return d->state = utf8_dtab[256 + d->state + type];
}

STC_INLINE uint32_t utf8_peek(const char* s) {
    utf8_decode_t d = {.state=0};
    do {
        utf8_decode(&d, (uint8_t)*s++);
    } while (d.state > utf8_REJECT);
    return d.state == utf8_ACCEPT ? d.codep : 0xFFFD;
}

/* case-insensitive utf8 string comparison */
STC_INLINE int utf8_icmp(const char* s1, const char* s2) {
    return utf8_icompare(c_sv(s1, INTPTR_MAX), c_sv(s2, INTPTR_MAX));
}

// ------------------------------------------------------
// Functions below must be linked with ucd_prv.c content

enum utf8_group {
    U8G_Cc, U8G_L, U8G_Lm, U8G_Lt, U8G_Nd, U8G_Nl, U8G_No,
    U8G_P, U8G_Pc, U8G_Pd, U8G_Pe, U8G_Pf, U8G_Pi, U8G_Ps,
    U8G_Sc, U8G_Sk, U8G_Sm, U8G_Zl, U8G_Zp, U8G_Zs,
    U8G_Arabic, U8G_Bengali, U8G_Cyrillic,
    U8G_Devanagari, U8G_Georgian, U8G_Greek,
    U8G_Han, U8G_Hiragana, U8G_Katakana,
    U8G_Latin, U8G_Thai,
    U8G_SIZE
};

extern bool utf8_isgroup(int group, uint32_t c);

STC_INLINE bool utf8_isdigit(uint32_t c)
    { return c < 128 ? (c >= '0') & (c <= '9') : utf8_isgroup(U8G_Nd, c); }

STC_INLINE bool utf8_isalpha(uint32_t c)
    { return (c < 128 ? isalpha((int)c) != 0 : utf8_isgroup(U8G_L, c)); }

STC_INLINE bool utf8_iscased(uint32_t c) {
    if (c < 128) return isalpha((int)c) != 0;
    return utf8_toupper(c) != c || utf8_tolower(c) != c || utf8_isgroup(U8G_Lt, c);
}

STC_INLINE bool utf8_isalnum(uint32_t c) {
    if (c < 128) return isalnum((int)c) != 0;
    return utf8_isgroup(U8G_L, c) || utf8_isgroup(U8G_Nd, c);
}

STC_INLINE bool utf8_isword(uint32_t c) {
    if (c < 128) return (isalnum((int)c) != 0) | (c == '_');
    return utf8_isgroup(U8G_L, c) || utf8_isgroup(U8G_Nd, c) || utf8_isgroup(U8G_Pc, c);
}

STC_INLINE bool utf8_isblank(uint32_t c) {
    if (c < 128) return (c == ' ') | (c == '\t');
    return utf8_isgroup(U8G_Zs, c);
}

STC_INLINE bool utf8_isspace(uint32_t c) {
    if (c < 128) return isspace((int)c) != 0;
    return ((c == 8232) | (c == 8233)) || utf8_isgroup(U8G_Zs, c);
}

#define c_lowerbound(T, c, at, less, N, ret) do { \
    int _n = N, _i = 0, _mid = _n/2; \
    T _c = c; \
    while (_n > 0) { \
        if (less(at((_i + _mid)), &_c)) { \
            _i += _mid + 1; \
            _n -= _mid + 1; \
            _mid = _n*7/8; \
        } else { \
            _n = _mid; \
            _mid = _n/8; \
        } \
    } \
    *(ret) = _i; \
} while (0)

#endif // STC_UTF8_PRV_H_INCLUDED
// ### END_FILE_INCLUDE: utf8_prv.h
// ### BEGIN_FILE_INCLUDE: cstr_prv.h
// IWYU pragma: private, include "stc/cstr.h"
#ifndef STC_CSTR_PRV_H_INCLUDED
#define STC_CSTR_PRV_H_INCLUDED

#include <stdio.h> /* FILE*, vsnprintf */
#include <stdlib.h> /* malloc */
#include <stddef.h> /* size_t */
#include <stdarg.h> /* cstr_vfmt() */
/**************************** PRIVATE API **********************************/

#if defined __GNUC__ && !defined __clang__
  // linkage.h already does diagnostic push
  // Warns wrongfully on -O3 on cstr_assign(&str, "literal longer than 23 ...");
  #pragma GCC diagnostic ignored "-Warray-bounds"
#endif

enum  { cstr_s_cap = sizeof(cstr_buf) - 2 };
#define cstr_s_size(s)          ((isize_t)(s)->sml.size)
#define cstr_s_set_size(s, len) ((s)->sml.data[(s)->sml.size = (uint8_t)(len)] = 0)
#define cstr_s_data(s)          (s)->sml.data

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define byte_rotl_(x, b)       ((x) << (b)*8 | (x) >> (sizeof(x) - (b))*8)
    #define cstr_l_cap(s)          (isize_t)(~byte_rotl_((s)->lon.ncap, sizeof((s)->lon.ncap) - 1))
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~byte_rotl_((uintptr_t)(cap), 1))
#else
    #define cstr_l_cap(s)          (isize_t)(~(s)->lon.ncap)
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~(uintptr_t)(cap))
#endif
#define cstr_l_size(s)          (isize_t)((s)->lon.size)
#define cstr_l_set_size(s, len) ((s)->lon.data[(s)->lon.size = (uintptr_t)(len)] = 0)
#define cstr_l_data(s)          (s)->lon.data
#define cstr_l_drop(s)          c_free((s)->lon.data, cstr_l_cap(s) + 1)

#define cstr_is_long(s)         ((s)->sml.size >= 128)
extern  char* _cstr_init(cstr* self, isize_t len, isize_t cap);
extern  char* _cstr_internal_move(cstr* self, isize_t pos1, isize_t pos2);

/**************************** PUBLIC API **********************************/

// non-owning char pointer
typedef             const char* cstr_raw;
#define             cstr_raw_cmp(x, y)      strcmp(*(x), *(y))
#define             cstr_raw_eq(x, y)       (cstr_raw_cmp(x, y) == 0)
#define             cstr_raw_hash(vp)       c_hash_str(*(vp))

#define             cstr_init() (c_literal(cstr){0})
#define             cstr_lit(literal) cstr_from_n(literal, c_litstrlen(literal))

extern  cstr        cstr_from_replace(csview sv, csview search, csview repl, int32_t count);
extern  cstr        cstr_from_fmt(const char* fmt, ...) c_GNUATTR(format(printf, 1, 2));

extern  void        cstr_drop(const cstr* self);
extern  cstr*       cstr_take(cstr* self, const cstr s);
extern  char*       cstr_reserve(cstr* self, isize_t cap);
extern  void        cstr_shrink_to_fit(cstr* self);
extern  char*       cstr_resize(cstr* self, isize_t size, char value);
extern  isize_t     cstr_find_at(const cstr* self, isize_t pos, const char* search);
extern  isize_t     cstr_find_sv(const cstr* self, csview search);
extern  char*       cstr_assign_n(cstr* self, const char* str, isize_t len);
extern  char*       cstr_append_n(cstr* self, const char* str, isize_t len);
extern  isize_t     cstr_append_fmt(cstr* self, const char* fmt, ...) c_GNUATTR(format(printf, 2, 3));
extern  char*       cstr_append_uninit(cstr *self, isize_t len);

extern  bool        cstr_getdelim(cstr *self, int delim, FILE *fp);
extern  void        cstr_erase(cstr* self, isize_t pos, isize_t len);
extern  isize_t     cstr_printf(cstr* self, const char* fmt, ...) c_GNUATTR(format(printf, 2, 3));
extern  isize_t     cstr_vfmt(cstr* self, isize_t start, const char* fmt, va_list args);
extern  size_t      cstr_hash(const cstr *self);
extern  bool        cstr_u8_valid(const cstr* self);
extern  void        cstr_u8_erase(cstr* self, isize_t u8pos, isize_t u8len);

STC_INLINE cstr_buf cstr_getbuf(cstr* s) {
    return cstr_is_long(s) ? c_literal(cstr_buf){s->lon.data, cstr_l_size(s), cstr_l_cap(s)}
                           : c_literal(cstr_buf){s->sml.data, cstr_s_size(s), cstr_s_cap};
}
STC_INLINE zsview cstr_zv(const cstr* s) {
    return cstr_is_long(s) ? c_literal(zsview){s->lon.data, cstr_l_size(s)}
                           : c_literal(zsview){s->sml.data, cstr_s_size(s)};
}
STC_INLINE csview cstr_sv(const cstr* s) {
    return cstr_is_long(s) ? c_literal(csview){s->lon.data, cstr_l_size(s)}
                           : c_literal(csview){s->sml.data, cstr_s_size(s)};
}

STC_INLINE cstr cstr_from_n(const char* str, const isize_t len) {
    cstr s;
    c_memcpy(_cstr_init(&s, len, len), str, len);
    return s;
}

STC_INLINE cstr cstr_from(const char* str)
    { return cstr_from_n(str, c_strlen(str)); }

STC_INLINE cstr cstr_from_sv(csview sv)
    { return cstr_from_n(sv.buf, sv.size); }

STC_INLINE cstr cstr_from_zv(zsview zv)
    { return cstr_from_n(zv.str, zv.size); }

STC_INLINE cstr cstr_with_size(const isize_t size, const char value) {
    cstr s;
    c_memset(_cstr_init(&s, size, size), value, size);
    return s;
}

STC_INLINE cstr cstr_with_capacity(const isize_t cap) {
    cstr s;
    _cstr_init(&s, 0, cap);
    return s;
}

STC_INLINE cstr cstr_move(cstr* self) {
    cstr tmp = *self;
    *self = cstr_init();
    return tmp;
}

STC_INLINE cstr cstr_clone(cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_from_n(sv.buf, sv.size);
}

#define SSO_CALL(s, call) (cstr_is_long(s) ? cstr_l_##call : cstr_s_##call)

STC_INLINE void _cstr_set_size(cstr* self, isize_t len)
    { SSO_CALL(self, set_size(self, len)); }

STC_INLINE void cstr_clear(cstr* self)
    { _cstr_set_size(self, 0); }

STC_INLINE char* cstr_data(cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_str(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_toraw(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE isize_t cstr_size(const cstr* self)
    { return SSO_CALL(self, size(self)); }

STC_INLINE bool cstr_is_empty(const cstr* self)
    { return cstr_size(self) == 0; }

STC_INLINE isize_t cstr_capacity(const cstr* self)
    { return cstr_is_long(self) ? cstr_l_cap(self) : cstr_s_cap; }

STC_INLINE isize_t cstr_to_index(const cstr* self, cstr_iter it)
    { return it.ref - cstr_str(self); }

STC_INLINE cstr cstr_from_s(cstr s, isize_t pos, isize_t len)
    { return cstr_from_n(cstr_str(&s) + pos, len); }

STC_INLINE csview cstr_subview(const cstr* self, isize_t pos, isize_t len) {
    csview sv = cstr_sv(self);
    c_assert(((size_t)pos <= (size_t)sv.size) & (len >= 0));
    if (pos + len > sv.size) len = sv.size - pos;
    return c_literal(csview){sv.buf + pos, len};
}

STC_INLINE zsview cstr_tail(const cstr* self, isize_t len) {
    c_assert(len >= 0);
    csview sv = cstr_sv(self);
    if (len > sv.size) len = sv.size;
    return c_literal(zsview){&sv.buf[sv.size - len], len};
}

// BEGIN utf8 functions =====

STC_INLINE cstr cstr_u8_from(const char* str, isize_t u8pos, isize_t u8len)
    { str = utf8_at(str, u8pos); return cstr_from_n(str, utf8_to_index(str, u8len)); }

STC_INLINE isize_t cstr_u8_size(const cstr* self)
    { return utf8_count(cstr_str(self)); }

STC_INLINE isize_t cstr_u8_to_index(const cstr* self, isize_t u8pos)
    { return utf8_to_index(cstr_str(self), u8pos); }

STC_INLINE zsview cstr_u8_tail(const cstr* self, isize_t u8len) {
    csview sv = cstr_sv(self);
    const char* p = &sv.buf[sv.size];
    while (u8len && p != sv.buf)
        u8len -= (*--p & 0xC0) != 0x80;
    return c_literal(zsview){p, sv.size - (p - sv.buf)};
}

STC_INLINE csview cstr_u8_subview(const cstr* self, isize_t u8pos, isize_t u8len)
    { return utf8_subview(cstr_str(self), u8pos, u8len); }

STC_INLINE cstr_iter cstr_u8_at(const cstr* self, isize_t u8pos) {
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

STC_INLINE cstr_iter cstr_advance(cstr_iter it, isize_t u8pos) {
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
    isize_t len = c_strlen(sub);
    return len <= sv.size && !utf8_icompare((sv.size = len, sv), c_sv(sub, len));
}

STC_INLINE bool cstr_iends_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self);
    isize_t len = c_strlen(sub);
    return len <= sv.size && !utf8_icmp(sv.buf + sv.size - len, sub);
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

STC_INLINE isize_t cstr_find(const cstr* self, const char* search) {
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

#define cstr_join(self, sep, vec) do { \
    struct _vec_s { cstr* data; ptrdiff_t size; } \
          *_vec = (struct _vec_s*)&(vec); \
    (void)sizeof((vec).data == _vec->data && &(vec).size == &_vec->size); \
    cstr_join_sn(self, sep, _vec->data, _vec->size); \
} while (0);

#define cstr_join_items(self, sep, ...) \
    cstr_join_n(self, sep, c_make_array(const char*, __VA_ARGS__), c_sizeof((const char*[])__VA_ARGS__)/c_sizeof(char*))

STC_INLINE void cstr_join_n(cstr* self, const char* sep, const char* arr[], isize_t n) {
    const char* _sep = cstr_is_empty(self) ? "" : sep;
    while (n--) { cstr_append(self, _sep); cstr_append(self, *arr++); _sep = sep; }
}
STC_INLINE void cstr_join_sn(cstr* self, const char* sep, const cstr arr[], isize_t n) {
    const char* _sep = cstr_is_empty(self) ? "" : sep;
    while (n--) { cstr_append(self, _sep); cstr_append_s(self, *arr++); _sep = sep; }
}


STC_INLINE void cstr_replace_sv(cstr* self, csview search, csview repl, int32_t count)
    { cstr_take(self, cstr_from_replace(cstr_sv(self), search, repl, count)); }

STC_INLINE void cstr_replace_nfirst(cstr* self, const char* search, const char* repl, int32_t count)
    { cstr_replace_sv(self, c_sv(search, c_strlen(search)), c_sv(repl, c_strlen(repl)), count); }

STC_INLINE void cstr_replace(cstr* self, const char* search, const char* repl)
    { cstr_replace_nfirst(self, search, repl, INT32_MAX); }


STC_INLINE void cstr_replace_at_sv(cstr* self, isize_t pos, isize_t len, const csview repl) {
    char* d = _cstr_internal_move(self, pos + len, pos + repl.size);
    c_memcpy(d + pos, repl.buf, repl.size);
}
STC_INLINE void cstr_replace_at(cstr* self, isize_t pos, isize_t len, const char* repl)
    { cstr_replace_at_sv(self, pos, len, c_sv(repl, c_strlen(repl))); }

STC_INLINE void cstr_u8_replace(cstr* self, isize_t u8pos, isize_t u8len, const char* repl) {
    const char* s = cstr_str(self); csview span = utf8_subview(s, u8pos, u8len);
    cstr_replace_at(self, span.buf - s, span.size, repl);
}


STC_INLINE void cstr_insert_sv(cstr* self, isize_t pos, csview sv)
    { cstr_replace_at_sv(self, pos, 0, sv); }

STC_INLINE void cstr_insert(cstr* self, isize_t pos, const char* str)
    { cstr_replace_at_sv(self, pos, 0, c_sv(str, c_strlen(str))); }

STC_INLINE void cstr_u8_insert(cstr* self, isize_t u8pos, const char* str)
    { cstr_insert(self, utf8_to_index(cstr_str(self), u8pos), str); }

STC_INLINE bool cstr_getline(cstr *self, FILE *fp)
    { return cstr_getdelim(self, '\n', fp); }

#endif // STC_CSTR_PRV_H_INCLUDED
// ### END_FILE_INCLUDE: cstr_prv.h
// ### BEGIN_FILE_INCLUDE: ucd_prv.c
#ifndef STC_UCD_PRV_C_INCLUDED
#define STC_UCD_PRV_C_INCLUDED

// ------------------------------------------------------
// The following requires linking with utf8 symbols.
// To call them, either define i_import before including
// one of cstr, csview, zsview, or link with src/libstc.o.

/* The tables below are extracted from the RE2 library */

typedef struct {
  uint16_t lo;
  uint16_t hi;
} URange16;

static const URange16 Cc_range16[] = { // Control
    { 0, 31 },
    { 127, 159 },
};

static const URange16 L_range16[] = { // Letter
    { 65, 90 },
    { 97, 122 },
    { 170, 170 },
    { 181, 181 },
    { 186, 186 },
    { 192, 214 },
    { 216, 246 },
    { 248, 705 },
    { 710, 721 },
    { 736, 740 },
    { 748, 748 },
    { 750, 750 },
    { 880, 884 },
    { 886, 887 },
    { 890, 893 },
    { 895, 895 },
    { 902, 902 },
    { 904, 906 },
    { 908, 908 },
    { 910, 929 },
    { 931, 1013 },
    { 1015, 1153 },
    { 1162, 1327 },
    { 1329, 1366 },
    { 1369, 1369 },
    { 1376, 1416 },
    { 1488, 1514 },
    { 1519, 1522 },
    { 1568, 1610 },
    { 1646, 1647 },
    { 1649, 1747 },
    { 1749, 1749 },
    { 1765, 1766 },
    { 1774, 1775 },
    { 1786, 1788 },
    { 1791, 1791 },
    { 1808, 1808 },
    { 1810, 1839 },
    { 1869, 1957 },
    { 1969, 1969 },
    { 1994, 2026 },
    { 2036, 2037 },
    { 2042, 2042 },
    { 2048, 2069 },
    { 2074, 2074 },
    { 2084, 2084 },
    { 2088, 2088 },
    { 2112, 2136 },
    { 2144, 2154 },
    { 2160, 2183 },
    { 2185, 2190 },
    { 2208, 2249 },
    { 2308, 2361 },
    { 2365, 2365 },
    { 2384, 2384 },
    { 2392, 2401 },
    { 2417, 2432 },
    { 2437, 2444 },
    { 2447, 2448 },
    { 2451, 2472 },
    { 2474, 2480 },
    { 2482, 2482 },
    { 2486, 2489 },
    { 2493, 2493 },
    { 2510, 2510 },
    { 2524, 2525 },
    { 2527, 2529 },
    { 2544, 2545 },
    { 2556, 2556 },
    { 2565, 2570 },
    { 2575, 2576 },
    { 2579, 2600 },
    { 2602, 2608 },
    { 2610, 2611 },
    { 2613, 2614 },
    { 2616, 2617 },
    { 2649, 2652 },
    { 2654, 2654 },
    { 2674, 2676 },
    { 2693, 2701 },
    { 2703, 2705 },
    { 2707, 2728 },
    { 2730, 2736 },
    { 2738, 2739 },
    { 2741, 2745 },
    { 2749, 2749 },
    { 2768, 2768 },
    { 2784, 2785 },
    { 2809, 2809 },
    { 2821, 2828 },
    { 2831, 2832 },
    { 2835, 2856 },
    { 2858, 2864 },
    { 2866, 2867 },
    { 2869, 2873 },
    { 2877, 2877 },
    { 2908, 2909 },
    { 2911, 2913 },
    { 2929, 2929 },
    { 2947, 2947 },
    { 2949, 2954 },
    { 2958, 2960 },
    { 2962, 2965 },
    { 2969, 2970 },
    { 2972, 2972 },
    { 2974, 2975 },
    { 2979, 2980 },
    { 2984, 2986 },
    { 2990, 3001 },
    { 3024, 3024 },
    { 3077, 3084 },
    { 3086, 3088 },
    { 3090, 3112 },
    { 3114, 3129 },
    { 3133, 3133 },
    { 3160, 3162 },
    { 3165, 3165 },
    { 3168, 3169 },
    { 3200, 3200 },
    { 3205, 3212 },
    { 3214, 3216 },
    { 3218, 3240 },
    { 3242, 3251 },
    { 3253, 3257 },
    { 3261, 3261 },
    { 3293, 3294 },
    { 3296, 3297 },
    { 3313, 3314 },
    { 3332, 3340 },
    { 3342, 3344 },
    { 3346, 3386 },
    { 3389, 3389 },
    { 3406, 3406 },
    { 3412, 3414 },
    { 3423, 3425 },
    { 3450, 3455 },
    { 3461, 3478 },
    { 3482, 3505 },
    { 3507, 3515 },
    { 3517, 3517 },
    { 3520, 3526 },
    { 3585, 3632 },
    { 3634, 3635 },
    { 3648, 3654 },
    { 3713, 3714 },
    { 3716, 3716 },
    { 3718, 3722 },
    { 3724, 3747 },
    { 3749, 3749 },
    { 3751, 3760 },
    { 3762, 3763 },
    { 3773, 3773 },
    { 3776, 3780 },
    { 3782, 3782 },
    { 3804, 3807 },
    { 3840, 3840 },
    { 3904, 3911 },
    { 3913, 3948 },
    { 3976, 3980 },
    { 4096, 4138 },
    { 4159, 4159 },
    { 4176, 4181 },
    { 4186, 4189 },
    { 4193, 4193 },
    { 4197, 4198 },
    { 4206, 4208 },
    { 4213, 4225 },
    { 4238, 4238 },
    { 4256, 4293 },
    { 4295, 4295 },
    { 4301, 4301 },
    { 4304, 4346 },
    { 4348, 4680 },
    { 4682, 4685 },
    { 4688, 4694 },
    { 4696, 4696 },
    { 4698, 4701 },
    { 4704, 4744 },
    { 4746, 4749 },
    { 4752, 4784 },
    { 4786, 4789 },
    { 4792, 4798 },
    { 4800, 4800 },
    { 4802, 4805 },
    { 4808, 4822 },
    { 4824, 4880 },
    { 4882, 4885 },
    { 4888, 4954 },
    { 4992, 5007 },
    { 5024, 5109 },
    { 5112, 5117 },
    { 5121, 5740 },
    { 5743, 5759 },
    { 5761, 5786 },
    { 5792, 5866 },
    { 5873, 5880 },
    { 5888, 5905 },
    { 5919, 5937 },
    { 5952, 5969 },
    { 5984, 5996 },
    { 5998, 6000 },
    { 6016, 6067 },
    { 6103, 6103 },
    { 6108, 6108 },
    { 6176, 6264 },
    { 6272, 6276 },
    { 6279, 6312 },
    { 6314, 6314 },
    { 6320, 6389 },
    { 6400, 6430 },
    { 6480, 6509 },
    { 6512, 6516 },
    { 6528, 6571 },
    { 6576, 6601 },
    { 6656, 6678 },
    { 6688, 6740 },
    { 6823, 6823 },
    { 6917, 6963 },
    { 6981, 6988 },
    { 7043, 7072 },
    { 7086, 7087 },
    { 7098, 7141 },
    { 7168, 7203 },
    { 7245, 7247 },
    { 7258, 7293 },
    { 7296, 7304 },
    { 7312, 7354 },
    { 7357, 7359 },
    { 7401, 7404 },
    { 7406, 7411 },
    { 7413, 7414 },
    { 7418, 7418 },
    { 7424, 7615 },
    { 7680, 7957 },
    { 7960, 7965 },
    { 7968, 8005 },
    { 8008, 8013 },
    { 8016, 8023 },
    { 8025, 8025 },
    { 8027, 8027 },
    { 8029, 8029 },
    { 8031, 8061 },
    { 8064, 8116 },
    { 8118, 8124 },
    { 8126, 8126 },
    { 8130, 8132 },
    { 8134, 8140 },
    { 8144, 8147 },
    { 8150, 8155 },
    { 8160, 8172 },
    { 8178, 8180 },
    { 8182, 8188 },
    { 8305, 8305 },
    { 8319, 8319 },
    { 8336, 8348 },
    { 8450, 8450 },
    { 8455, 8455 },
    { 8458, 8467 },
    { 8469, 8469 },
    { 8473, 8477 },
    { 8484, 8484 },
    { 8486, 8486 },
    { 8488, 8488 },
    { 8490, 8493 },
    { 8495, 8505 },
    { 8508, 8511 },
    { 8517, 8521 },
    { 8526, 8526 },
    { 8579, 8580 },
    { 11264, 11492 },
    { 11499, 11502 },
    { 11506, 11507 },
    { 11520, 11557 },
    { 11559, 11559 },
    { 11565, 11565 },
    { 11568, 11623 },
    { 11631, 11631 },
    { 11648, 11670 },
    { 11680, 11686 },
    { 11688, 11694 },
    { 11696, 11702 },
    { 11704, 11710 },
    { 11712, 11718 },
    { 11720, 11726 },
    { 11728, 11734 },
    { 11736, 11742 },
    { 11823, 11823 },
    { 12293, 12294 },
    { 12337, 12341 },
    { 12347, 12348 },
    { 12353, 12438 },
    { 12445, 12447 },
    { 12449, 12538 },
    { 12540, 12543 },
    { 12549, 12591 },
    { 12593, 12686 },
    { 12704, 12735 },
    { 12784, 12799 },
    { 13312, 19903 },
    { 19968, 42124 },
    { 42192, 42237 },
    { 42240, 42508 },
    { 42512, 42527 },
    { 42538, 42539 },
    { 42560, 42606 },
    { 42623, 42653 },
    { 42656, 42725 },
    { 42775, 42783 },
    { 42786, 42888 },
    { 42891, 42954 },
    { 42960, 42961 },
    { 42963, 42963 },
    { 42965, 42969 },
    { 42994, 43009 },
    { 43011, 43013 },
    { 43015, 43018 },
    { 43020, 43042 },
    { 43072, 43123 },
    { 43138, 43187 },
    { 43250, 43255 },
    { 43259, 43259 },
    { 43261, 43262 },
    { 43274, 43301 },
    { 43312, 43334 },
    { 43360, 43388 },
    { 43396, 43442 },
    { 43471, 43471 },
    { 43488, 43492 },
    { 43494, 43503 },
    { 43514, 43518 },
    { 43520, 43560 },
    { 43584, 43586 },
    { 43588, 43595 },
    { 43616, 43638 },
    { 43642, 43642 },
    { 43646, 43695 },
    { 43697, 43697 },
    { 43701, 43702 },
    { 43705, 43709 },
    { 43712, 43712 },
    { 43714, 43714 },
    { 43739, 43741 },
    { 43744, 43754 },
    { 43762, 43764 },
    { 43777, 43782 },
    { 43785, 43790 },
    { 43793, 43798 },
    { 43808, 43814 },
    { 43816, 43822 },
    { 43824, 43866 },
    { 43868, 43881 },
    { 43888, 44002 },
    { 44032, 55203 },
    { 55216, 55238 },
    { 55243, 55291 },
    { 63744, 64109 },
    { 64112, 64217 },
    { 64256, 64262 },
    { 64275, 64279 },
    { 64285, 64285 },
    { 64287, 64296 },
    { 64298, 64310 },
    { 64312, 64316 },
    { 64318, 64318 },
    { 64320, 64321 },
    { 64323, 64324 },
    { 64326, 64433 },
    { 64467, 64829 },
    { 64848, 64911 },
    { 64914, 64967 },
    { 65008, 65019 },
    { 65136, 65140 },
    { 65142, 65276 },
    { 65313, 65338 },
    { 65345, 65370 },
    { 65382, 65470 },
    { 65474, 65479 },
    { 65482, 65487 },
    { 65490, 65495 },
    { 65498, 65500 },
};

static const URange16 Lm_range16[] = { // Modifier letter
    { 688, 705 },
    { 710, 721 },
    { 736, 740 },
    { 748, 748 },
    { 750, 750 },
    { 884, 884 },
    { 890, 890 },
    { 1369, 1369 },
    { 1600, 1600 },
    { 1765, 1766 },
    { 2036, 2037 },
    { 2042, 2042 },
    { 2074, 2074 },
    { 2084, 2084 },
    { 2088, 2088 },
    { 2249, 2249 },
    { 2417, 2417 },
    { 3654, 3654 },
    { 3782, 3782 },
    { 4348, 4348 },
    { 6103, 6103 },
    { 6211, 6211 },
    { 6823, 6823 },
    { 7288, 7293 },
    { 7468, 7530 },
    { 7544, 7544 },
    { 7579, 7615 },
    { 8305, 8305 },
    { 8319, 8319 },
    { 8336, 8348 },
    { 11388, 11389 },
    { 11631, 11631 },
    { 11823, 11823 },
    { 12293, 12293 },
    { 12337, 12341 },
    { 12347, 12347 },
    { 12445, 12446 },
    { 12540, 12542 },
    { 40981, 40981 },
    { 42232, 42237 },
    { 42508, 42508 },
    { 42623, 42623 },
    { 42652, 42653 },
    { 42775, 42783 },
    { 42864, 42864 },
    { 42888, 42888 },
    { 42994, 42996 },
    { 43000, 43001 },
    { 43471, 43471 },
    { 43494, 43494 },
    { 43632, 43632 },
    { 43741, 43741 },
    { 43763, 43764 },
    { 43868, 43871 },
    { 43881, 43881 },
    { 65392, 65392 },
    { 65438, 65439 },
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

static const URange16 No_range16[] = { // Other number
    { 178, 179 },
    { 185, 185 },
    { 188, 190 },
    { 2548, 2553 },
    { 2930, 2935 },
    { 3056, 3058 },
    { 3192, 3198 },
    { 3416, 3422 },
    { 3440, 3448 },
    { 3882, 3891 },
    { 4969, 4988 },
    { 6128, 6137 },
    { 6618, 6618 },
    { 8304, 8304 },
    { 8308, 8313 },
    { 8320, 8329 },
    { 8528, 8543 },
    { 8585, 8585 },
    { 9312, 9371 },
    { 9450, 9471 },
    { 10102, 10131 },
    { 11517, 11517 },
    { 12690, 12693 },
    { 12832, 12841 },
    { 12872, 12879 },
    { 12881, 12895 },
    { 12928, 12937 },
    { 12977, 12991 },
    { 43056, 43061 },
};

static const URange16 P_range16[] = { // Punctuation
    { 33, 35 },
    { 37, 42 },
    { 44, 47 },
    { 58, 59 },
    { 63, 64 },
    { 91, 93 },
    { 95, 95 },
    { 123, 123 },
    { 125, 125 },
    { 161, 161 },
    { 167, 167 },
    { 171, 171 },
    { 182, 183 },
    { 187, 187 },
    { 191, 191 },
    { 894, 894 },
    { 903, 903 },
    { 1370, 1375 },
    { 1417, 1418 },
    { 1470, 1470 },
    { 1472, 1472 },
    { 1475, 1475 },
    { 1478, 1478 },
    { 1523, 1524 },
    { 1545, 1546 },
    { 1548, 1549 },
    { 1563, 1563 },
    { 1565, 1567 },
    { 1642, 1645 },
    { 1748, 1748 },
    { 1792, 1805 },
    { 2039, 2041 },
    { 2096, 2110 },
    { 2142, 2142 },
    { 2404, 2405 },
    { 2416, 2416 },
    { 2557, 2557 },
    { 2678, 2678 },
    { 2800, 2800 },
    { 3191, 3191 },
    { 3204, 3204 },
    { 3572, 3572 },
    { 3663, 3663 },
    { 3674, 3675 },
    { 3844, 3858 },
    { 3860, 3860 },
    { 3898, 3901 },
    { 3973, 3973 },
    { 4048, 4052 },
    { 4057, 4058 },
    { 4170, 4175 },
    { 4347, 4347 },
    { 4960, 4968 },
    { 5120, 5120 },
    { 5742, 5742 },
    { 5787, 5788 },
    { 5867, 5869 },
    { 5941, 5942 },
    { 6100, 6102 },
    { 6104, 6106 },
    { 6144, 6154 },
    { 6468, 6469 },
    { 6686, 6687 },
    { 6816, 6822 },
    { 6824, 6829 },
    { 7002, 7008 },
    { 7037, 7038 },
    { 7164, 7167 },
    { 7227, 7231 },
    { 7294, 7295 },
    { 7360, 7367 },
    { 7379, 7379 },
    { 8208, 8231 },
    { 8240, 8259 },
    { 8261, 8273 },
    { 8275, 8286 },
    { 8317, 8318 },
    { 8333, 8334 },
    { 8968, 8971 },
    { 9001, 9002 },
    { 10088, 10101 },
    { 10181, 10182 },
    { 10214, 10223 },
    { 10627, 10648 },
    { 10712, 10715 },
    { 10748, 10749 },
    { 11513, 11516 },
    { 11518, 11519 },
    { 11632, 11632 },
    { 11776, 11822 },
    { 11824, 11855 },
    { 11858, 11869 },
    { 12289, 12291 },
    { 12296, 12305 },
    { 12308, 12319 },
    { 12336, 12336 },
    { 12349, 12349 },
    { 12448, 12448 },
    { 12539, 12539 },
    { 42238, 42239 },
    { 42509, 42511 },
    { 42611, 42611 },
    { 42622, 42622 },
    { 42738, 42743 },
    { 43124, 43127 },
    { 43214, 43215 },
    { 43256, 43258 },
    { 43260, 43260 },
    { 43310, 43311 },
    { 43359, 43359 },
    { 43457, 43469 },
    { 43486, 43487 },
    { 43612, 43615 },
    { 43742, 43743 },
    { 43760, 43761 },
    { 44011, 44011 },
    { 64830, 64831 },
    { 65040, 65049 },
    { 65072, 65106 },
    { 65108, 65121 },
    { 65123, 65123 },
    { 65128, 65128 },
    { 65130, 65131 },
    { 65281, 65283 },
    { 65285, 65290 },
    { 65292, 65295 },
    { 65306, 65307 },
    { 65311, 65312 },
    { 65339, 65341 },
    { 65343, 65343 },
    { 65371, 65371 },
    { 65373, 65373 },
    { 65375, 65381 },
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

static const URange16 Pe_range16[] = { // End/close punctuation
    { 41, 41 },
    { 93, 93 },
    { 125, 125 },
    { 3899, 3899 },
    { 3901, 3901 },
    { 5788, 5788 },
    { 8262, 8262 },
    { 8318, 8318 },
    { 8334, 8334 },
    { 8969, 8969 },
    { 8971, 8971 },
    { 9002, 9002 },
    { 10089, 10089 },
    { 10091, 10091 },
    { 10093, 10093 },
    { 10095, 10095 },
    { 10097, 10097 },
    { 10099, 10099 },
    { 10101, 10101 },
    { 10182, 10182 },
    { 10215, 10215 },
    { 10217, 10217 },
    { 10219, 10219 },
    { 10221, 10221 },
    { 10223, 10223 },
    { 10628, 10628 },
    { 10630, 10630 },
    { 10632, 10632 },
    { 10634, 10634 },
    { 10636, 10636 },
    { 10638, 10638 },
    { 10640, 10640 },
    { 10642, 10642 },
    { 10644, 10644 },
    { 10646, 10646 },
    { 10648, 10648 },
    { 10713, 10713 },
    { 10715, 10715 },
    { 10749, 10749 },
    { 11811, 11811 },
    { 11813, 11813 },
    { 11815, 11815 },
    { 11817, 11817 },
    { 11862, 11862 },
    { 11864, 11864 },
    { 11866, 11866 },
    { 11868, 11868 },
    { 12297, 12297 },
    { 12299, 12299 },
    { 12301, 12301 },
    { 12303, 12303 },
    { 12305, 12305 },
    { 12309, 12309 },
    { 12311, 12311 },
    { 12313, 12313 },
    { 12315, 12315 },
    { 12318, 12319 },
    { 64830, 64830 },
    { 65048, 65048 },
    { 65078, 65078 },
    { 65080, 65080 },
    { 65082, 65082 },
    { 65084, 65084 },
    { 65086, 65086 },
    { 65088, 65088 },
    { 65090, 65090 },
    { 65092, 65092 },
    { 65096, 65096 },
    { 65114, 65114 },
    { 65116, 65116 },
    { 65118, 65118 },
    { 65289, 65289 },
    { 65341, 65341 },
    { 65373, 65373 },
    { 65376, 65376 },
    { 65379, 65379 },
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

static const URange16 Ps_range16[] = { // Start/open punctuation
    { 40, 40 },
    { 91, 91 },
    { 123, 123 },
    { 3898, 3898 },
    { 3900, 3900 },
    { 5787, 5787 },
    { 8218, 8218 },
    { 8222, 8222 },
    { 8261, 8261 },
    { 8317, 8317 },
    { 8333, 8333 },
    { 8968, 8968 },
    { 8970, 8970 },
    { 9001, 9001 },
    { 10088, 10088 },
    { 10090, 10090 },
    { 10092, 10092 },
    { 10094, 10094 },
    { 10096, 10096 },
    { 10098, 10098 },
    { 10100, 10100 },
    { 10181, 10181 },
    { 10214, 10214 },
    { 10216, 10216 },
    { 10218, 10218 },
    { 10220, 10220 },
    { 10222, 10222 },
    { 10627, 10627 },
    { 10629, 10629 },
    { 10631, 10631 },
    { 10633, 10633 },
    { 10635, 10635 },
    { 10637, 10637 },
    { 10639, 10639 },
    { 10641, 10641 },
    { 10643, 10643 },
    { 10645, 10645 },
    { 10647, 10647 },
    { 10712, 10712 },
    { 10714, 10714 },
    { 10748, 10748 },
    { 11810, 11810 },
    { 11812, 11812 },
    { 11814, 11814 },
    { 11816, 11816 },
    { 11842, 11842 },
    { 11861, 11861 },
    { 11863, 11863 },
    { 11865, 11865 },
    { 11867, 11867 },
    { 12296, 12296 },
    { 12298, 12298 },
    { 12300, 12300 },
    { 12302, 12302 },
    { 12304, 12304 },
    { 12308, 12308 },
    { 12310, 12310 },
    { 12312, 12312 },
    { 12314, 12314 },
    { 12317, 12317 },
    { 64831, 64831 },
    { 65047, 65047 },
    { 65077, 65077 },
    { 65079, 65079 },
    { 65081, 65081 },
    { 65083, 65083 },
    { 65085, 65085 },
    { 65087, 65087 },
    { 65089, 65089 },
    { 65091, 65091 },
    { 65095, 65095 },
    { 65113, 65113 },
    { 65115, 65115 },
    { 65117, 65117 },
    { 65288, 65288 },
    { 65339, 65339 },
    { 65371, 65371 },
    { 65375, 65375 },
    { 65378, 65378 },
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

static const URange16 Sk_range16[] = { // Modifier symbol
    { 94, 94 },
    { 96, 96 },
    { 168, 168 },
    { 175, 175 },
    { 180, 180 },
    { 184, 184 },
    { 706, 709 },
    { 722, 735 },
    { 741, 747 },
    { 749, 749 },
    { 751, 767 },
    { 885, 885 },
    { 900, 901 },
    { 2184, 2184 },
    { 8125, 8125 },
    { 8127, 8129 },
    { 8141, 8143 },
    { 8157, 8159 },
    { 8173, 8175 },
    { 8189, 8190 },
    { 12443, 12444 },
    { 42752, 42774 },
    { 42784, 42785 },
    { 42889, 42890 },
    { 43867, 43867 },
    { 43882, 43883 },
    { 64434, 64450 },
    { 65342, 65342 },
    { 65344, 65344 },
    { 65507, 65507 },
};

static const URange16 Sm_range16[] = { // Math symbol
    { 43, 43 },
    { 60, 62 },
    { 124, 124 },
    { 126, 126 },
    { 172, 172 },
    { 177, 177 },
    { 215, 215 },
    { 247, 247 },
    { 1014, 1014 },
    { 1542, 1544 },
    { 8260, 8260 },
    { 8274, 8274 },
    { 8314, 8316 },
    { 8330, 8332 },
    { 8472, 8472 },
    { 8512, 8516 },
    { 8523, 8523 },
    { 8592, 8596 },
    { 8602, 8603 },
    { 8608, 8608 },
    { 8611, 8611 },
    { 8614, 8614 },
    { 8622, 8622 },
    { 8654, 8655 },
    { 8658, 8658 },
    { 8660, 8660 },
    { 8692, 8959 },
    { 8992, 8993 },
    { 9084, 9084 },
    { 9115, 9139 },
    { 9180, 9185 },
    { 9655, 9655 },
    { 9665, 9665 },
    { 9720, 9727 },
    { 9839, 9839 },
    { 10176, 10180 },
    { 10183, 10213 },
    { 10224, 10239 },
    { 10496, 10626 },
    { 10649, 10711 },
    { 10716, 10747 },
    { 10750, 11007 },
    { 11056, 11076 },
    { 11079, 11084 },
    { 64297, 64297 },
    { 65122, 65122 },
    { 65124, 65126 },
    { 65291, 65291 },
    { 65308, 65310 },
    { 65372, 65372 },
    { 65374, 65374 },
    { 65506, 65506 },
    { 65513, 65516 },
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
    _e_arg(U8G_L, UNI_ENTRY(L)),
    _e_arg(U8G_Lm, UNI_ENTRY(Lm)),
    _e_arg(U8G_Lt, UNI_ENTRY(Lt)),
    _e_arg(U8G_Nd, UNI_ENTRY(Nd)),
    _e_arg(U8G_Nl, UNI_ENTRY(Nl)),
    _e_arg(U8G_No, UNI_ENTRY(No)),
    _e_arg(U8G_P, UNI_ENTRY(P)),
    _e_arg(U8G_Pc, UNI_ENTRY(Pc)),
    _e_arg(U8G_Pd, UNI_ENTRY(Pd)),
    _e_arg(U8G_Pe, UNI_ENTRY(Pe)),
    _e_arg(U8G_Pf, UNI_ENTRY(Pf)),
    _e_arg(U8G_Pi, UNI_ENTRY(Pi)),
    _e_arg(U8G_Ps, UNI_ENTRY(Ps)),
    _e_arg(U8G_Sc, UNI_ENTRY(Sc)),
    _e_arg(U8G_Sk, UNI_ENTRY(Sk)),
    _e_arg(U8G_Sm, UNI_ENTRY(Sm)),
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

bool utf8_isgroup(int group, uint32_t c) {
    #define _at_group(idx) &_utf8_unicode_groups[group].r16[idx].hi
    int i, n = _utf8_unicode_groups[group].nr16;
    c_lowerbound(uint32_t, c, _at_group, c_default_less, n, &i);
    return (i < n && c >= _utf8_unicode_groups[group].r16[i].lo);
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
    isize_t allocsize;
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
    UTF_an      , UTF_AN,       /* utf8 alphanumeric */
    UTF_bl      , UTF_BL,       /* utf8 blank */
    UTF_lc      , UTF_LC,       /* utf8 letter cased */
    UTF_ll      , UTF_LL,       /* utf8 letter lowercase */
    UTF_lu      , UTF_LU,       /* utf8 letter uppercase */
    UTF_sp      , UTF_SP,       /* utf8 space */
    UTF_wr      , UTF_WR,       /* utf8 word */
    UTF_GRP = 0x8150000, // odd enums = inverse:
    UTF_cc = UTF_GRP+2*U8G_Cc, UTF_CC, /* utf8 control char */
    UTF_l  = UTF_GRP+2*U8G_L,  UTF_L,  /* utf8 letter */
    UTF_lm = UTF_GRP+2*U8G_Lm, UTF_LM, /* utf8 letter modifier */
    UTF_lt = UTF_GRP+2*U8G_Lt, UTF_LT, /* utf8 letter titlecase */
    UTF_nd = UTF_GRP+2*U8G_Nd, UTF_ND, /* utf8 number decimal */
    UTF_nl = UTF_GRP+2*U8G_Nl, UTF_NL, /* utf8 number letter */
    UTF_no = UTF_GRP+2*U8G_No, UTF_NO, /* utf8 number other */
    UTF_p  = UTF_GRP+2*U8G_P,  UTF_P,  /* utf8 punctuation */
    UTF_pc = UTF_GRP+2*U8G_Pc, UTF_PC, /* utf8 punct connector */
    UTF_pd = UTF_GRP+2*U8G_Pd, UTF_PD, /* utf8 punct dash */
    UTF_pe = UTF_GRP+2*U8G_Pe, UTF_PE, /* utf8 punct close */
    UTF_pf = UTF_GRP+2*U8G_Pf, UTF_PF, /* utf8 punct final */
    UTF_pi = UTF_GRP+2*U8G_Pi, UTF_PI, /* utf8 punct initial */
    UTF_ps = UTF_GRP+2*U8G_Ps, UTF_PS, /* utf8 punct open */
    UTF_sc = UTF_GRP+2*U8G_Sc, UTF_SC, /* utf8 symbol currency */
    UTF_sk = UTF_GRP+2*U8G_Sk, UTF_SK, /* utf8 symbol modifier */
    UTF_sm = UTF_GRP+2*U8G_Sm, UTF_SM, /* utf8 symbol math */
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


static inline int
chartorune(_Rune *rune, const char *s)
{
    utf8_decode_t d = {.state=0};
    int n = utf8_decode_codepoint(&d, s, NULL);
    *rune = d.codep;
    return n;
}

static const char*
utfrune(const char *s, _Rune c) // search
{
    if (c < 0x80)        /* ascii */
        return strchr((char *)s, (int)c);

    utf8_decode_t d = {.state=0};
    while (*s != 0) {
        int n = utf8_decode_codepoint(&d, s, NULL);
        if (d.codep == c) return s;
        s += n;
    }
    return NULL;
}

static const char*
utfruneicase(const char *s, _Rune c) {
    if (c < 0x80) {
        for (int low = tolower((int)c); *s != 0; ++s)
            if (tolower(*s) == low)
                return s;
    } else {
        utf8_decode_t d = {.state=0};
        c = utf8_casefold(c);
        while (*s != 0) {
            int n = utf8_decode_codepoint(&d, s, NULL);
            if (utf8_casefold(d.codep) == c)
                return s;
            s += n;
        }
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
    isize_t instcap;
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

    intptr_t ipp = (intptr_t)pp; // convert pointer to integer!
    isize_t new_allocsize = c_sizeof(_Reprog) + (par->freep - pp->firstinst)*c_sizeof(_Reinst);
    _Reprog *npp = (_Reprog *)c_realloc(pp, pp->allocsize, new_allocsize);
    isize_t diff = (intptr_t)npp - ipp;

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
            if (*rp == 'x' && *par->exprp == '{') {
                *rp = (_Rune)strtol(par->exprp + 1, (char **)&par->exprp, 16);
                if (*par->exprp != '}')
                    _rcerror(par, CREG_UNMATCHEDRIGHTPARENTHESIS);
                par->exprp++;
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
        {"{Alpha}", 7, UTF_l}, {"{L&}", 4, UTF_lc},
        {"{Digit}", 7, UTF_nd}, {"{Nd}", 4, UTF_nd},
        {"{Lower}", 7, UTF_ll}, {"{Ll}", 4, UTF_ll},
        {"{Upper}", 7, UTF_lu}, {"{Lu}", 4, UTF_lu},
        {"{Cntrl}", 7, UTF_cc}, {"{Cc}", 4, UTF_cc},
        {"{Alnum}", 7, UTF_an}, {"{Blank}", 7, UTF_bl},
        {"{Space}", 7, UTF_sp}, {"{Word}", 6, UTF_wr},
        {"{XDigit}", 8, ASC_xd},
        {"{L}",  3, UTF_l},  {"{Lm}", 4, UTF_lm},
        {"{Lt}", 4, UTF_lt},
        {"{Nl}", 4, UTF_nl}, {"{No}", 4, UTF_no},
        {"{P}",  3, UTF_p},  {"{Pc}", 4, UTF_pc},
        {"{Pd}", 4, UTF_pd}, {"{Pe}", 4, UTF_pe},
        {"{Pf}", 4, UTF_pf}, {"{Pi}", 4, UTF_pi},
        {"{Ps}", 4, UTF_ps},
        {"{Zl}", 4, UTF_zl}, {"{Zp}", 4, UTF_zp},
        {"{Zs}", 4, UTF_zs}, {"{Sc}", 4, UTF_sc},
        {"{Sk}", 4, UTF_sk}, {"{Sm}", 4, UTF_sm},
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
    isize_t instcap = 5 + 6*c_strlen(s);
    isize_t new_allocsize = c_sizeof(_Reprog) + instcap*c_sizeof(_Reinst);
    pp = (_Reprog *)c_realloc(pp, pp ? pp->allocsize : 0, new_allocsize);
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
        c_free(pp, pp->allocsize);
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
    case UTF_WR: inv = 1; case UTF_wr: return inv ^ (int)utf8_isword(r);
    case UTF_L:  inv = 1; case UTF_l:  return inv ^ (int)utf8_isalpha(r);
    case UTF_ND: inv = 1; case UTF_nd: return inv ^ (int)utf8_isdigit(r);
/* isgroup: */
    case UTF_cc: case UTF_CC:
    case UTF_lm: case UTF_LM:
    case UTF_lt: case UTF_LT:
    case UTF_nl: case UTF_NL:
    case UTF_no: case UTF_NO:
    case UTF_p: case UTF_P:
    case UTF_pc: case UTF_PC:
    case UTF_pd: case UTF_PD:
    case UTF_pe: case UTF_PE:
    case UTF_pf: case UTF_PF:
    case UTF_pi: case UTF_PI:
    case UTF_ps: case UTF_PS:
    case UTF_sc: case UTF_SC:
    case UTF_sk: case UTF_SK:
    case UTF_sm: case UTF_SM:
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
    int ms,             /* number of elements at mp */
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

    bool icase = progp->flags.icase;
    checkstart = j->starttype;
    if (mp) memset(mp, 0, (unsigned)ms*sizeof *mp);
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
        r = *(uint8_t*)s;
        n = r < 0x80 ? 1 : chartorune(&r, s);

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
                    if (ok ^ (r == 0 || s == bol || s == j->eol ||
                              (utf8_isword(utf8_peek_at(s, -1)) ^
                               utf8_isword(utf8_peek(s)))))
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
    isize_t sz = 2 * _BIGLISTSIZE*c_sizeof(_Relist);
    relists = (_Relist *)c_malloc(sz);
    if (relists == NULL)
        return -1;

    j->relist[0] = relists;
    j->relist[1] = relists + _BIGLISTSIZE;
    j->reliste[0] = relists + _BIGLISTSIZE - 2;
    j->reliste[1] = relists + 2*_BIGLISTSIZE - 2;

    rv = _regexec1(progp, bol, mp, ms, j, mflags);
    c_free(relists, sz);
    return rv;
}

static int
_regexec(const _Reprog *progp,    /* program to run */
    const char *bol,    /* string to run machine on */
    const char *bol_end,/* end of string (or NULL for null-termination) */
    int ms,             /* number of elements at mp */
    _Resub mp[],        /* subexpression elements */
    int mflags)
{
    _Reljunk j;
    _Relist relist0[_LISTSIZE], relist1[_LISTSIZE];
    int rv;

    j.starts = bol;
    j.eol = bol_end;

    if ((mflags & CREG_NEXT) && mp[0].buf)
        j.starts = mp[0].buf + mp[0].size;
    if (j.eol && j.starts > j.eol)
        return 0; // no match

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
_build_substitution(const char* replace, int nmatch, const csview match[],
                    bool(*transform)(int, csview, cstr*), cstr* subst) {
    cstr_buf mbuf = cstr_getbuf(subst);
    isize_t len = 0, cap = mbuf.cap;
    char* dst = mbuf.data;
    cstr tr_str = {0};

    while (*replace != '\0') {
        if (*replace == '$') {
            int arg = replace[1];
            if (arg >= '0' && arg <= '9') {
                arg -= '0';
                if (replace[2] >= '0' && replace[2] <= '9' && replace[3] == ';')
                    { arg = arg*10 + (replace[2] - '0'); replace += 2; }
                replace += 2;
                if (arg < nmatch) {
                    csview tr_sv = transform && transform(arg, match[arg], &tr_str)
                                 ? cstr_sv(&tr_str) : match[arg];
                    if (len + tr_sv.size > cap)
                        dst = cstr_reserve(subst, cap += cap/2 + tr_sv.size);
                    for (int i = 0; i < tr_sv.size; ++i)
                        dst[len++] = tr_sv.buf[i];
                }
                continue;
            }
            if (arg == '$') // allow e.g. "$$3" => "$3"
                ++replace;
        }
        if (len == cap)
            dst = cstr_reserve(subst, cap += cap/2 + 4);
        dst[len++] = *replace++;
    }
    cstr_drop(&tr_str);
    _cstr_set_size(subst, len);
}



int cregex_compile_pro(cregex *self, const char* pattern, int cflags) {
    _Parser par;
    self->prog = _regcomp1(self->prog, &par, pattern, cflags);
    return self->error = par.error;
}

int cregex_captures(const cregex* self) {
    return self->prog ? self->prog->nsubids : 0;
}

void cregex_drop(cregex* self) {
    c_free(self->prog, self->prog->allocsize);
}

int cregex_match_opt(const cregex* re, const char* input, const char* input_end, struct cregex_match_opt opt) {
    int res = _regexec(re->prog, input, input_end, cregex_captures(re) + 1, opt.match, opt.flags);
    switch (res) {
        case 1: return CREG_OK;
        case 0: return CREG_NOMATCH;
        default: return CREG_MATCHERROR;
    }
}

int cregex_match_aio_opt(const char* pattern, const char* input, const char* input_end, struct cregex_match_opt opt) {
    cregex re = cregex_make(pattern, opt.flags);
    if (re.error != CREG_OK) return re.error;
    int res = cregex_match_opt(&re, input, input_end, opt);
    cregex_drop(&re);
    return res;
}

cstr cregex_replace_opt(const cregex* re, const char* input, const char* input_end, const char* replace, struct cregex_replace_opt opt) {
    cstr out = {0};
    cstr subst = {0};
    csview match[CREG_MAX_CAPTURES];
    int nmatch = cregex_captures(re) + 1;
    bool copy = !(opt.flags & CREG_STRIP);
    struct cregex_match_opt mopt = {match};
    opt.count += (opt.count != 0);

    while (--opt.count && cregex_match_opt(re, input, input_end, mopt) == CREG_OK) {
        _build_substitution(replace, nmatch, match, opt.xform, &subst);
        const isize_t mpos = (match[0].buf - input);
        if (copy & (mpos > 0))
            cstr_append_n(&out, input, mpos);
        cstr_append_s(&out, subst);
        input = match[0].buf + match[0].size;
    }
    if (copy) {
        isize_t len = input_end ? input_end - input : c_strlen(input);
        cstr_append_sv(&out, c_sv(input, len));
    }
    cstr_drop(&subst);
    return out;
}

cstr cregex_replace_aio_opt(const char* pattern, const char* input, const char* input_end, const char* replace, struct cregex_replace_opt opt) {
    cregex re = {0};
    if (cregex_compile_pro(&re, pattern, opt.flags) != CREG_OK)
        assert(0);
    cstr out = cregex_replace_opt(&re, input, input_end, replace, opt);
    cregex_drop(&re);
    return out;
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

uint32_t utf8_peek_at(const char* s, isize_t offset) {
    return utf8_peek(utf8_offset(s, offset));
}

bool utf8_valid(const char* s) {
    utf8_decode_t d = {.state=0};
    while ((utf8_decode(&d, (uint8_t)*s) != utf8_REJECT) & (*s != '\0'))
        ++s;
    return d.state == utf8_ACCEPT;
}

bool utf8_valid_n(const char* s, isize_t nbytes) {
    utf8_decode_t d = {.state=0};
    for (; nbytes-- != 0; ++s)
        if ((utf8_decode(&d, (uint8_t)*s) == utf8_REJECT) | (*s == '\0'))
            break;
    return d.state == utf8_ACCEPT;
}

uint32_t utf8_casefold(uint32_t c) {
    #define _at_fold(idx) &casemappings[idx].c2
    int i;
    c_lowerbound(uint32_t, c, _at_fold, c_default_less, casefold_len, &i);
    if (i < casefold_len && casemappings[i].c1 <= c) {
        const struct CaseMapping entry = casemappings[i];
        int d = entry.m2 - entry.c2;
        if (d == 1) return c + ((entry.c2 & 1U) == (c & 1U));
        return (uint32_t)((int)c + d);
    }
    return c;
}

uint32_t utf8_tolower(uint32_t c) {
    #define _at_upper(idx) &casemappings[upcase_ind[idx]].c2
    int i, n = c_countof(upcase_ind);
    c_lowerbound(uint32_t, c, _at_upper, c_default_less, n, &i);
    if (i < n) {
        const struct CaseMapping entry = casemappings[upcase_ind[i]];
        if (entry.c1 <= c) {
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1U) == (c & 1U));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

uint32_t utf8_toupper(uint32_t c) {
    #define _at_lower(idx) &casemappings[lowcase_ind[idx]].m2
    int i, n = c_countof(lowcase_ind);
    c_lowerbound(uint32_t, c, _at_lower, c_default_less, n, &i);
    if (i < n) {
        const struct CaseMapping entry = casemappings[lowcase_ind[i]];
        int d = entry.m2 - entry.c2;
        if (entry.c1 + (uint32_t)d <= c) {
            if (d == 1) return c - ((entry.m2 & 1U) == (c & 1U));
            return (uint32_t)((int)c - d);
        }
    }
    return c;
}

int utf8_decode_codepoint(utf8_decode_t* d, const char* s, const char* end) { // s < end
    const char* start = s;
    do switch (utf8_decode(d, (uint8_t)*s++)) {
        case utf8_ACCEPT: return (int)(s - start);
        case utf8_REJECT: goto recover;
    } while (s != end);

    recover: // non-complete utf8 is also treated as utf8_REJECT
    d->state = utf8_ACCEPT;
    d->codep = 0xFFFD;
    //return 1;
    int n = (int)(s - start);
    return n > 2 ? n - 1 : 1;
}

int utf8_icompare(const csview s1, const csview s2) {
    utf8_decode_t d1 = {.state=0}, d2 = {.state=0};
    const char *e1 = s1.buf + s1.size, *e2 = s2.buf + s2.size;
    isize_t j1 = 0, j2 = 0;
    while ((j1 < s1.size) & (j2 < s2.size)) {
        if (s2.buf[j2] == '\0') return s1.buf[j1];

        j1 += utf8_decode_codepoint(&d1, s1.buf + j1, e1);
        j2 += utf8_decode_codepoint(&d2, s2.buf + j2, e2);

        int32_t c = (int32_t)utf8_casefold(d1.codep) - (int32_t)utf8_casefold(d2.codep);
        if (c != 0) return (int)c;
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

void cstr_drop(const cstr* self) {
    if (cstr_is_long(self))
        cstr_l_drop(self);
}

cstr* cstr_take(cstr* self, const cstr s) {
    if (cstr_is_long(self) && self->lon.data != s.lon.data)
        cstr_l_drop(self);
    *self = s;
    return self;
}

size_t cstr_hash(const cstr *self) {
    csview sv = cstr_sv(self);
    return c_hash_str(sv.buf);
}

isize_t cstr_find_sv(const cstr* self, csview search) {
    csview sv = cstr_sv(self);
    char* res = c_strnstrn(sv.buf, sv.size, search.buf, search.size);
    return res ? (res - sv.buf) : c_NPOS;
}

char* _cstr_init(cstr* self, const isize_t len, const isize_t cap) {
    if (cap > cstr_s_cap) {
        self->lon.data = (char *)c_malloc(cap + 1);
        cstr_l_set_size(self, len);
        cstr_l_set_cap(self, cap);
        return self->lon.data;
    }
    cstr_s_set_size(self, len);
    return self->sml.data;
}

char* cstr_reserve(cstr* self, const isize_t cap) {
    if (cstr_is_long(self)) {
        if (cap > cstr_l_cap(self)) {
            self->lon.data = (char *)c_realloc(self->lon.data, cstr_l_cap(self) + 1, cap + 1);
            cstr_l_set_cap(self, cap);
        }
        return self->lon.data;
    }
    /* from short to long: */
    if (cap > cstr_s_cap) {
        char* data = (char *)c_malloc(cap + 1);
        const isize_t len = cstr_s_size(self);
        /* copy full short buffer to emulate realloc() */
        c_memcpy(data, self->sml.data, c_sizeof self->sml);
        self->lon.data = data;
        self->lon.size = (size_t)len;
        cstr_l_set_cap(self, cap);
        return data;
    }
    return self->sml.data;
}

char* cstr_resize(cstr* self, const isize_t size, const char value) {
    cstr_buf b = cstr_getbuf(self);
    if (size > b.size) {
        if (size > b.cap && (b.data = cstr_reserve(self, size)) == NULL)
            return NULL;
        c_memset(b.data + b.size, value, size - b.size);
    }
    _cstr_set_size(self, size);
    return b.data;
}

isize_t cstr_find_at(const cstr* self, const isize_t pos, const char* search) {
    csview sv = cstr_sv(self);
    if (pos > sv.size) return c_NPOS;
    const char* res = strstr((char*)sv.buf + pos, search);
    return res ? (res - sv.buf) : c_NPOS;
}

char* cstr_assign_n(cstr* self, const char* str, const isize_t len) {
    char* d = cstr_reserve(self, len);
    if (d) { _cstr_set_size(self, len); c_memmove(d, str, len); }
    return d;
}

char* cstr_append_n(cstr* self, const char* str, const isize_t len) {
    cstr_buf b = cstr_getbuf(self);
    if (b.size + len > b.cap) {
        const size_t off = (size_t)(str - b.data);
        b.data = cstr_reserve(self, b.size*3/2 + len);
        if (b.data == NULL) return NULL;
        if (off <= (size_t)b.size) str = b.data + off; /* handle self append */
    }
    c_memcpy(b.data + b.size, str, len);
    _cstr_set_size(self, b.size + len);
    return b.data;
}

void cstr_erase(cstr* self, const isize_t pos, isize_t len) {
    cstr_buf b = cstr_getbuf(self);
    if (len > b.size - pos) len = b.size - pos;
    c_memmove(&b.data[pos], &b.data[pos + len], b.size - (pos + len));
    _cstr_set_size(self, b.size - len);
}
#endif // STC_CSTR_CORE_C_INCLUDED


// ------------------- STC_CSTR_MISC --------------------
#if !defined STC_CSTR_MISC_C_INCLUDED && \
    (defined i_implement || defined STC_CSTR_MISC)
#define STC_CSTR_MISC_C_INCLUDED

void cstr_shrink_to_fit(cstr* self) {
    cstr_buf b = cstr_getbuf(self);
    if (b.size == b.cap)
        return;
    if (b.size > cstr_s_cap) {
        self->lon.data = (char *)c_realloc(self->lon.data, cstr_l_cap(self) + 1, b.size + 1);
        cstr_l_set_cap(self, b.size);
    } else if (b.cap > cstr_s_cap) {
        c_memcpy(self->sml.data, b.data, b.size + 1);
        cstr_s_set_size(self, b.size);
        c_free(b.data, b.cap + 1);
    }
}

char* _cstr_internal_move(cstr* self, const isize_t pos1, const isize_t pos2) {
    cstr_buf b = cstr_getbuf(self);
    if (pos1 != pos2) {
        const isize_t newlen = (b.size + pos2 - pos1);
        if (newlen > b.cap)
            b.data = cstr_reserve(self, b.size*3/2 + pos2 - pos1);
        c_memmove(&b.data[pos2], &b.data[pos1], b.size - pos1);
        _cstr_set_size(self, newlen);
    }
    return b.data;
}

cstr cstr_from_replace(csview in, csview search, csview repl, int32_t count) {
    cstr out = cstr_init();
    isize_t from = 0; char* res;
    if (count == 0) count = INT32_MAX;
    if (search.size)
        while (count-- && (res = c_strnstrn(in.buf + from, in.size - from, search.buf, search.size))) {
            const isize_t pos = (res - in.buf);
            cstr_append_n(&out, in.buf + from, pos - from);
            cstr_append_n(&out, repl.buf, repl.size);
            from = pos + search.size;
        }
    cstr_append_n(&out, in.buf + from, in.size - from);
    return out;
}
#endif // STC_CSTR_MISC_C_INCLUDED


// ------------------- STC_CSTR_IO --------------------
#if !defined STC_CSTR_IO_C_INCLUDED && \
    (defined i_import || defined STC_CSTR_IO)
#define STC_CSTR_IO_C_INCLUDED

char* cstr_append_uninit(cstr *self, isize_t len) {
    cstr_buf b = cstr_getbuf(self);
    if (b.size + len > b.cap && (b.data = cstr_reserve(self, b.size*3/2 + len)) == NULL)
        return NULL;
    _cstr_set_size(self, b.size + len);
    return b.data + b.size;
}

bool cstr_getdelim(cstr *self, const int delim, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    isize_t pos = 0;
    cstr_buf b = cstr_getbuf(self);
    for (;;) {
        if (c == delim || c == EOF) {
            _cstr_set_size(self, pos);
            return true;
        }
        if (pos == b.cap) {
            _cstr_set_size(self, pos);
            char* data = cstr_reserve(self, (b.cap = b.cap*3/2 + 16));
            b.data = data;
        }
        b.data[pos++] = (char) c;
        c = fgetc(fp);
    }
}

isize_t cstr_vfmt(cstr* self, isize_t start, const char* fmt, va_list args) {
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

isize_t cstr_append_fmt(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const isize_t n = cstr_vfmt(self, cstr_size(self), fmt, args);
    va_end(args);
    return n;
}

/* NB! self-data in args is UB */
isize_t cstr_printf(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const isize_t n = cstr_vfmt(self, 0, fmt, args);
    va_end(args);
    return n;
}
#endif // STC_CSTR_IO_C_INCLUDED

// ------------------- STC_CSTR_UTF8 --------------------
#if !defined STC_CSTR_UTF8_C_INCLUDED && \
    (defined i_import || defined STC_CSTR_UTF8 || defined STC_UTF8_PRV_C_INCLUDED)
#define STC_CSTR_UTF8_C_INCLUDED

#include <ctype.h>

void cstr_u8_erase(cstr* self, const isize_t u8pos, const isize_t u8len) {
    csview b = cstr_sv(self);
    csview span = utf8_subview(b.buf, u8pos, u8len);
    c_memmove((void *)&span.buf[0], &span.buf[span.size], b.size - span.size - (span.buf - b.buf));
    _cstr_set_size(self, b.size - span.size);
}

bool cstr_u8_valid(const cstr* self)
    { return utf8_valid(cstr_str(self)); }

static int toLower(int c)
    { return c >= 'A' && c <= 'Z' ? c + 32 : c; }
static int toUpper(int c)
    { return c >= 'a' && c <= 'z' ? c - 32 : c; }
static struct {
    int      (*conv_asc)(int);
    uint32_t (*conv_utf)(uint32_t);
}
fn_tocase[] = {{toLower, utf8_casefold},
               {toLower, utf8_tolower},
               {toUpper, utf8_toupper}};

cstr cstr_tocase_sv(csview sv, int k) {
    cstr out = {0};
    char *buf = cstr_reserve(&out, sv.size*3/2);
    isize_t sz = 0;
    utf8_decode_t d = {.state=0};
    const char* end = sv.buf + sv.size;

    while (sv.buf < end) {
        sv.buf += utf8_decode_codepoint(&d, sv.buf, end);

        if (d.codep < 0x80)
            buf[sz++] = (char)fn_tocase[k].conv_asc((int)d.codep);
        else {
            uint32_t cp = fn_tocase[k].conv_utf(d.codep);
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

#undef i_aux
#undef _i_aux_alloc

#undef i_allocator
#undef i_malloc
#undef i_calloc
#undef i_realloc
#undef i_free

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


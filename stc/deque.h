// ### BEGIN_FILE_INCLUDE: deque.h
// Deque - double ended queue. Implemented as a ring buffer, extension of queue.

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
    struct { uintptr_t size; cstr_value* data; uintptr_t ncap; } lon;
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
        SELF##_ctrl* ctrl1; \
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

#define declare_stack_fixed(SELF, VAL, CAP) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value data[CAP]; ptrdiff_t size; } SELF

#define _declare_stack(SELF, VAL, AUXDEF) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value *data; ptrdiff_t size, capacity; AUXDEF } SELF

#endif // STC_TYPES_H_INCLUDED
// ### END_FILE_INCLUDE: types.h

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

// Macro overloading feature support based on: https://rextester.com/ONP80107
#define c_MACRO_OVERLOAD(name, ...) \
    c_JOIN(c_JOIN0(name,_),c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_JOIN0(a, b) a ## b
#define c_JOIN(a, b) c_JOIN0(a, b)
#define c_EXPAND(...) __VA_ARGS__
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) _c_ARG_N args  // wrap c_EXPAND(..) for MSVC without /std:c11
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1,
#define _c_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,N,...) N

// Saturated overloading
// #define foo(...) foo_I(__VA_ARGS__, c_COMMA_N(foo_3), c_COMMA_N(foo_2), c_COMMA_N(foo_1),)(__VA_ARGS__)
// #define foo_I(a,b,c, n, ...) c_TUPLE_AT_1(n, foo_n,)
#define c_TUPLE_AT_1(x,y,...) y
#define c_COMMA_N(x) ,x

// Select arg, e.g. for #define i_type A,B then c_GETARG(2, i_type) is B
#define c_GETARG(N, ...) c_ARG_##N(__VA_ARGS__,) // wrap c_EXPAND(..) for MSVC without /std:c11
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
    T* _tp = ptr; isize _n = n, _i = _n; \
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
#define c_countof(a)            (isize)(sizeof(a)/sizeof 0[a])
#define c_arraylen(a)           c_countof(a) // [deprecated]?

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

// x, y are i_keyraw* type, which defaults to i_key*. vp is i_key* type.
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_hash(vp)      c_hash_n(vp, sizeof *(vp))
#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

// non-owning char pointer
typedef const char* cstr_raw;
#define cstr_raw_cmp(x, y)      strcmp(*(x), *(y))
#define cstr_raw_eq(x, y)       (cstr_raw_cmp(x, y) == 0)
#define cstr_raw_hash(vp)       c_hash_str(*(vp))
#define cstr_raw_clone(v)       (v)
#define cstr_raw_drop(vp)       ((void)vp)

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

#define c_each_n(it, C, cnt, n) \
    struct {C##_iter iter; C##_value* ref; isize size, index;} \
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
#define c_range_1(stop) c_range_t_4(isize, _c_i1, 0, stop)
#define c_range_2(i, stop) c_range_t_4(isize, i, 0, stop)
#define c_range_3(i, start, stop) c_range_t_4(isize, i, start, stop)
#define c_range_4(i, start, stop, step) c_range_t_5(isize, i, start, stop, step)

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

STC_INLINE void* c_safe_memcpy(void* dst, const void* src, isize size)
    { return dst ? memcpy(dst, src, (size_t)size) : NULL; }

STC_INLINE size_t c_basehash_n(const void* key, isize len) {
    size_t block = 0, hash = 0x811c9dc5;
    const uint8_t* msg = (const uint8_t*)key;
    while (len > c_sizeof(size_t)) {
        memcpy(&block, msg, sizeof(size_t));
        hash = (hash ^ block) * (size_t)0x89bb179901000193;
        msg += c_sizeof(size_t);
        len -= c_sizeof(size_t);
    }
    c_memcpy(&block, msg, len);
    hash = (hash ^ block) * (size_t)0xb0340f4501000193;
    return hash ^ (hash >> 3);
}

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

#define c_hash_mix(...) /* non-commutative hash combine! */ \
    _chash_mix(c_make_array(size_t, {__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))

STC_INLINE size_t _chash_mix(size_t h[], int n) {
    for (int i = 1; i < n; ++i) h[0] += h[0] ^ h[i];
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

STC_INLINE char* c_strnstrn(const char *str, isize slen, const char *needle, isize nlen) {
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
#include <stdlib.h>
#endif // STC_DEQUE_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix deque_
#endif
#define _pop _pop_front
#define _pull _pull_front
// ### BEGIN_FILE_INCLUDE: template.h
// IWYU pragma: private
#ifndef _i_template
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED

  #define _c_MEMB(name) c_JOIN(Self, name)
  #define _c_DEFTYPES(macro, SELF, ...) macro(SELF, __VA_ARGS__)
  #define _m_value _c_MEMB(_value)
  #define _m_key _c_MEMB(_key)
  #define _m_mapped _c_MEMB(_mapped)
  #define _m_rmapped _c_MEMB(_rmapped)
  #define _m_raw _c_MEMB(_raw)
  #define _m_keyraw _c_MEMB(_keyraw)
  #define _m_iter _c_MEMB(_iter)
  #define _m_result _c_MEMB(_result)
  #define _m_node _c_MEMB(_node)

  #define c_OPTION(flag)  ((i_opt) & (flag))
  #define c_declared      (1<<0)
  #define c_no_atomic     (1<<1)
  #define c_arc2          (1<<2)
  #define c_no_clone      (1<<3)
  #define c_no_hash       (1<<4)
  #define c_use_cmp       (1<<5)
  #define c_use_eq        (1<<6)
  #define c_cmpclass      (1<<7)
  #define c_keyclass      (1<<8)
  #define c_valclass      (1<<9)
  #define c_keypro        (1<<10)
  #define c_valpro        (1<<11)
#endif

#if defined i_rawclass   // [deprecated]
  #define i_cmpclass i_rawclass
#endif

#if defined T && !defined i_type
  #define i_type T
#endif
#if defined i_type && c_NUMARGS(i_type) > 1
  #define Self c_GETARG(1, i_type)
  #define i_key c_GETARG(2, i_type)
  #if c_NUMARGS(i_type) == 3
    #if defined _i_is_map
      #define i_val c_GETARG(3, i_type)
    #else
      #define i_opt c_GETARG(3, i_type)
    #endif
  #elif c_NUMARGS(i_type) == 4
    #define i_val c_GETARG(3, i_type)
    #define i_opt c_GETARG(4, i_type)
  #endif
#elif !defined Self && defined i_type
  #define Self i_type
#elif !defined Self
  #define Self c_JOIN(_i_prefix, i_tag)
#endif

#if defined i_aux && c_NUMARGS(i_aux) == 2
  #define _i_aux_def c_GETARG(1, i_aux) aux;
  #undef i_allocator // override:
  #define i_allocator c_GETARG(2, i_aux)
#elif defined i_aux
  #define _i_aux_def i_aux aux;
#else
  #define _i_aux_def
#endif

#if c_OPTION(c_declared)
  #define i_declared
#endif
#if c_OPTION(c_no_hash)
  #define i_no_hash
#endif
#if c_OPTION(c_use_cmp)
  #define i_use_cmp
#endif
#if c_OPTION(c_use_eq)
  #define i_use_eq
#endif
#if c_OPTION(c_no_clone) || defined _i_is_arc
  #define i_no_clone
#endif
#if c_OPTION(c_keyclass)
  #define i_keyclass i_key
#endif
#if c_OPTION(c_valclass)
  #define i_valclass i_val
#endif
#if c_OPTION(c_cmpclass)
  #define i_cmpclass i_key
  #define i_use_cmp
#endif
#if c_OPTION(c_keypro)
  #define i_keypro i_key
#endif
#if c_OPTION(c_valpro)
  #define i_valpro i_val
#endif

#if defined i_keypro
  #define i_keyclass i_keypro
  #define i_cmpclass c_JOIN(i_keypro, _raw)
#endif

#if defined i_cmpclass
  #define i_keyraw i_cmpclass
  #if !(defined i_key || defined i_keyclass)
    #define i_key i_cmpclass
  #endif
#elif defined i_keyclass && !defined i_keyraw
  // Special: When only i_keyclass is defined, also define i_cmpclass to the same.
  // Do not define i_keyraw here, otherwise _from() / _toraw() is expected to exist.
  #define i_cmpclass i_key
#endif

// Bind to i_key "class members": _clone, _drop, _from and _toraw (when conditions are met).
#if defined i_keyclass
  #ifndef i_key
    #define i_key i_keyclass
  #endif
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

// Bind to i_cmpclass "class members": _cmp, _eq and _hash (when conditions are met).
#if defined i_cmpclass
  #if !(defined i_cmp || defined i_less) && (defined i_use_cmp || defined _i_sorted)
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
#elif defined i_keyraw && !(c_OPTION(c_cmpclass) || defined i_keytoraw)
  #error "If i_cmpclass / i_keyraw is defined, i_keytoraw must be defined too"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone and i_keydrop must be defined, if any (unless i_no_clone defined)."
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Use i_keyfrom/i_keydrop"
#elif defined i_keyto || defined i_valto
  #error i_keyto / i_valto not supported. Use i_keytoraw / i_valtoraw
#elif defined i_keyraw && defined i_use_cmp && !defined _i_has_cmp
  #error "For smap / sset / pqueue, i_cmp or i_less must be defined when i_keyraw is defined."
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

#define i_no_emplace

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

#if defined _i_is_map // ---- process hashmap/sortedmap value i_val, ... ----

#if defined i_valpro
  #define i_valclass i_valpro
  #define i_valraw c_JOIN(i_valpro, _raw)
#endif

#ifdef i_valclass
  #ifndef i_val
    #define i_val i_valclass
  #endif
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
// ### BEGIN_FILE_INCLUDE: queue_prv.h

// IWYU pragma: private
#ifndef i_declared
_c_DEFTYPES(_declare_queue, Self, i_key, _i_aux_def);
#endif
typedef i_keyraw _m_raw;

STC_API bool            _c_MEMB(_reserve)(Self* self, const isize cap);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API _m_value*       _c_MEMB(_push)(Self* self, _m_value value); // push_back
STC_API void            _c_MEMB(_shrink_to_fit)(Self *self);
STC_API _m_iter         _c_MEMB(_advance)(_m_iter it, isize n);

#define _cbuf_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cbuf_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

STC_INLINE void         _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
                            { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }

STC_INLINE void         _c_MEMB(_value_drop)(const Self* self, _m_value* val)
                            { (void)self; i_keydrop(val); }

#ifndef _i_aux_alloc
STC_INLINE Self         _c_MEMB(_init)(void)
                            { Self cx = {0}; return cx; }

STC_INLINE Self         _c_MEMB(_with_capacity)(const isize cap)
                            { Self cx = {0}; _c_MEMB(_reserve)(&cx, cap); return cx; }

STC_INLINE Self         _c_MEMB(_from_n)(const _m_raw* raw, isize n)
                            { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }
#endif

#if !defined i_no_emplace
STC_INLINE _m_value*    _c_MEMB(_emplace)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API bool            _c_MEMB(_eq)(const Self* self, const Self* other);
#endif

#if !defined i_no_clone
STC_API Self            _c_MEMB(_clone)(Self q);
STC_INLINE _m_value     _c_MEMB(_value_clone)(const Self* self, _m_value val)
                            { (void)self; return i_keyclone(val); }

STC_INLINE void         _c_MEMB(_copy)(Self* self, const Self* other) {
                            if (self == other) return;
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

STC_INLINE const _m_value* _c_MEMB(_front)(const Self* self)
                            { return self->cbuf + self->start; }
STC_INLINE _m_value*    _c_MEMB(_front_mut)(Self* self)
                            { return self->cbuf + self->start; }

STC_INLINE const _m_value* _c_MEMB(_back)(const Self* self)
                            { return self->cbuf + ((self->end - 1) & self->capmask); }
STC_INLINE _m_value*    _c_MEMB(_back_mut)(Self* self)
                            { return (_m_value*)_c_MEMB(_back)(self); }

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    memset(self, 0, sizeof *self);
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

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
#if defined i_implement

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
    for (c_each(i, Self, *self))
        { i_keydrop(i.ref); }
    self->start = 0, self->end = 0;
}

STC_DEF void
_c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    _c_MEMB(_clear)(self);
    _i_free_n(self->cbuf, self->capmask + 1);
}

STC_DEF bool
_c_MEMB(_reserve)(Self* self, const isize cap) {
    isize oldpow2 = self->capmask + (self->capmask & 1); // handle capmask = 0
    isize newpow2 = c_next_pow2(cap + 1);
    if (newpow2 <= oldpow2)
        return self->cbuf != NULL;
    _m_value* d = (_m_value *)_i_realloc_n(self->cbuf, oldpow2, newpow2);
    if (d == NULL)
        return false;
    isize head = oldpow2 - self->start;
    if (self->start <= self->end)       // [..S########E....|................]
        ;
    else if (head < self->end) {        // [#######E.....S##|.............s!!]
        c_memcpy(d + newpow2 - head, d + self->start, head*c_sizeof *d);
        self->start = newpow2 - head;
    } else {                            // [##E.....S#######|!!e.............]
        c_memcpy(d + oldpow2, d, self->end*c_sizeof *d);
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
        if (!_c_MEMB(_reserve)(self, self->capmask + 3)) // => 2x expand
            return NULL;
        end = (self->end + 1) & self->capmask;
    }
    _m_value *v = self->cbuf + self->end;
    self->end = end;
    *v = value;
    return v;
}

STC_DEF void
_c_MEMB(_shrink_to_fit)(Self *self) {
    isize sz = _c_MEMB(_size)(self);
    isize newpow2 = c_next_pow2(sz + 1);
    if (newpow2 > self->capmask)
        return;
    if (self->start <= self->end) {
        c_memmove(self->cbuf, self->cbuf + self->start, sz*c_sizeof *self->cbuf);
        self->start = 0, self->end = sz;
    } else {
        isize n = self->capmask - self->start + 1;
        c_memmove(self->cbuf + (newpow2 - n), self->cbuf + self->start, n*c_sizeof *self->cbuf);
        self->start = newpow2 - n;
    }
    self->cbuf = (_m_value *)_i_realloc_n(self->cbuf, self->capmask + 1, newpow2);
    self->capmask = newpow2 - 1;
}

#if !defined i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self q) {
    Self out = q, *self = &out; (void)self; // may be used by _i_new_n/i_keyclone via i_aux.
    out.start = 0; out.end = _c_MEMB(_size)(&q);
    out.capmask = c_next_pow2(out.end + 1) - 1;
    out.cbuf = _i_new_n(_m_value, out.capmask + 1);
    isize i = 0;
    if (out.cbuf)
        for (c_each(it, Self, q))
            out.cbuf[i++] = i_keyclone((*it.ref));
    return out;
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
_c_MEMB(_at)(const Self* self, isize idx) {
    c_assert(c_uless(idx, _c_MEMB(_size)(self)));
    return self->cbuf + _cbuf_topos(self, idx);
}

STC_INLINE _m_value*
_c_MEMB(_at_mut)(Self* self, isize idx)
    { return (_m_value*)_c_MEMB(_at)(self, idx); }

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
STC_API _m_iter _c_MEMB(_find_in)(const Self* self, _m_iter p1, _m_iter p2, _m_raw raw);

STC_INLINE _m_iter
_c_MEMB(_find)(const Self* self, _m_raw raw) {
    return _c_MEMB(_find_in)(self, _c_MEMB(_begin)(self), _c_MEMB(_end)(self), raw);
}
#endif // _i_has_eq

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
#if defined i_implement

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
#endif // _i_has_cmp

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

STC_DEF _m_value*
_c_MEMB(_push_front)(Self* self, _m_value value) {
    isize start = (self->start - 1) & self->capmask;
    if (start == self->end) { // full
        if (!_c_MEMB(_reserve)(self, self->capmask + 3)) // => 2x expand
            return NULL;
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
    c_assert(idx + n <= len);
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
    if (len + n >= self->capmask)
        if (!_c_MEMB(_reserve)(self, len + n)) // minimum 2x expand
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
_c_MEMB(_find_in)(const Self* self, _m_iter i1, _m_iter i2, _m_raw raw) {
    (void)self;
    for (; i1.ref != i2.ref; _c_MEMB(_next)(&i1)) {
        const _m_raw r = i_keytoraw(i1.ref);
        if (i_eq((&raw), (&r)))
            break;
    }
    return i1;
}
#endif
#endif // IMPLEMENTATION
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
// ### BEGIN_FILE_INCLUDE: template2.h
// IWYU pragma: private
#undef T            // alias for i_type
#undef i_type
#undef i_class
#undef i_tag
#undef i_opt
#undef i_capacity

#undef i_key
#undef i_keypro     // Replaces next two
#undef i_key_str    // [deprecated]
#undef i_key_arcbox // [deprecated]
#undef i_keyclass
#undef i_cmpclass   // define i_keyraw, and bind i_cmp, i_eq, i_hash "class members"
#undef i_rawclass   // [deprecated] for i_cmpclass
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
#undef i_valpro     // Replaces next two
#undef i_val_str    // [deprecated]
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
#undef i_declared

#undef _i_aux_def
#undef _i_has_cmp
#undef _i_has_eq
#undef _i_prefix
#undef _i_template
#undef Self
// ### END_FILE_INCLUDE: template2.h
// ### END_FILE_INCLUDE: deque.h


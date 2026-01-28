// ### BEGIN_FILE_INCLUDE: queue.h

// Queue. Implemented as a ring buffer.
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

#ifndef STC_QUEUE_H_INCLUDED
#define STC_QUEUE_H_INCLUDED
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
#include <stdlib.h>
#endif // STC_QUEUE_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix queue_
#endif
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
  #define c_use_arc2      (1<<2)
  #define c_no_clone      (1<<3)
  #define c_use_cmp       (1<<5)
  #define c_use_eq        (1<<6)
  #define c_use_compare   (c_use_cmp | c_use_eq)
  #define c_compare_key   (1<<7)
  #define c_class_key     (1<<8)
  #define c_class_val     (1<<9)
  #define c_pro_key       (1<<10)
  #define c_pro_val       (1<<11)

  #define c_use_comp c_use_compare // [deprecated]
  #define c_comp_key c_compare_key // [deprecated]
  #define c_keycomp  c_compare_key // [deprecated]
  #define c_cmpclass c_compare_key // [deprecated]
  #define c_keyclass c_class_key   // [deprecated]
  #define c_valclass c_class_val   // [deprecated]
  #define c_keypro   c_pro_key     // [deprecated]
  #define c_valpro   c_pro_val     // [deprecated]
#endif
#ifdef i_keycomp                   // [deprecated]
  #define i_compare_key i_keycomp
#elif defined i_comp_key           // [deprecated]
  #define i_compare_key i_comp_key
#elif defined i_keyclass           // [deprecated]
  #define i_class_key i_keyclass
#elif defined i_keypro             // [deprecated]
  #define i_pro_key i_keypro
#endif
#if defined i_valclass             // [deprecated]
  #define i_class_val i_valclass
#elif defined i_valpro             // [deprecated]
  #define i_pro_val i_valpro
#endif

#if defined T && !defined i_type
  #define i_type T
#endif
#if defined i_type && c_NUMARGS(i_type) > 1
  #define Self c_GETARG(1, i_type)
  #define i_key c_GETARG(2, i_type)
  #ifdef _i_is_map
    #define i_val c_GETARG(3, i_type)
    #if c_NUMARGS(i_type) == 4
      #define i_opt c_GETARG(4, i_type)+0
    #endif
  #elif c_NUMARGS(i_type) >= 3
    #define i_opt c_GETARG(3, i_type)+0
  #endif
#elif !defined Self && defined i_type
  #define Self i_type
#elif !defined Self
  #define Self c_JOIN(_i_prefix, i_tag)
#endif

#if defined i_aux && c_NUMARGS(i_aux) == 2
  // shorthand for defining i_aux AND i_allocator as a one-liner combo.
  #define _i_aux_alloc
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
#if c_OPTION(c_use_cmp)
  #define i_use_cmp
#endif
#if c_OPTION(c_use_eq)
  #define i_use_eq
#endif
#if c_OPTION(c_no_clone) || defined _i_is_arc
  #define i_no_clone
#endif
#if c_OPTION(c_class_key)
  #define i_class_key i_key
#endif
#if c_OPTION(c_class_val)
  #define i_class_val i_val
#endif
#if c_OPTION(c_compare_key)
  #define i_compare_key i_key
  #define i_use_cmp
  #define i_use_eq
#endif
#if c_OPTION(c_pro_key)
  #define i_pro_key i_key
#endif
#if c_OPTION(c_pro_val)
  #define i_pro_val i_val
#endif

#if defined i_pro_key
  #define i_class_key i_pro_key
  #define i_compare_key c_JOIN(i_pro_key, _raw)
#endif

#if defined i_compare_key
  #define i_keyraw i_compare_key
#elif defined i_class_key && !defined i_keyraw
  // Also bind comparisons functions when c_class_key is specified.
  #define i_compare_key i_key
#elif defined i_keyraw && !defined i_keyfrom
  // Define _i_no_put when i_keyfrom is not explicitly defined and i_keyraw is.
  // In this case, i_keytoraw needs to be defined (may be done later in this file).
  #define _i_no_put
#endif

// Bind to i_key "class members": _clone, _drop, _from and _toraw (when conditions are met).
#if defined i_class_key
  #ifndef i_key
    #define i_key i_class_key
  #endif
  #if !defined i_keyclone && !defined i_no_clone
    #define i_keyclone c_JOIN(i_class_key, _clone)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_JOIN(i_class_key, _drop)
  #endif
  #if !defined i_keyfrom && defined i_keyraw
    #define i_keyfrom c_JOIN(i_class_key, _from)
  #endif
  #if !defined i_keytoraw && defined i_keyraw
    #define i_keytoraw c_JOIN(i_class_key, _toraw)
  #endif
#endif

// Define when container has support for sorting (cmp) and linear search (eq)
#if defined i_use_cmp || defined i_cmp || defined i_less || defined _i_sorted
  #define _i_has_cmp
#endif
#if defined i_use_eq || defined i_eq || defined i_hash || defined _i_hasher
  #define _i_has_eq
#endif

// Bind to i_compare_key "class members": _cmp, _eq and _hash (when conditions are met).
#if defined i_compare_key
  #if !(defined i_cmp || defined i_less) && defined _i_has_cmp
    #define i_cmp c_JOIN(i_compare_key, _cmp)
  #endif
  #if !defined i_eq && defined _i_has_eq
    #define i_eq c_JOIN(i_compare_key, _eq)
  #endif
  #if !(defined i_hash || defined i_no_hash)
    #define i_hash c_JOIN(i_compare_key, _hash)
  #endif
#endif

#if !defined i_key
  #error "No i_key defined"
#elif defined i_keyraw && !(c_OPTION(c_compare_key) || defined i_keytoraw)
  #error "If i_compare_key / i_keyraw is defined, i_keytoraw must be defined too"
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
#if !defined i_eq
  #define i_eq(x, y) *x == *y // works for integral types
  #define _i_has_default_eq
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

#define _i_no_emplace
#define _i_is_trivial

#ifndef i_tag
  #define i_tag i_key
#endif
#ifndef i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #undef _i_no_emplace
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
#else
  #undef _i_is_trivial
#endif

#if defined _i_is_map // ---- process hashmap/sortedmap value i_val, ... ----

#if defined i_pro_val
  #define i_class_val i_pro_val
  #define i_valraw c_JOIN(i_pro_val, _raw)
#endif

#ifdef i_class_val
  #ifndef i_val
    #define i_val i_class_val
  #endif
  #if !defined i_valclone && !defined i_no_clone
    #define i_valclone c_JOIN(i_class_val, _clone)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_JOIN(i_class_val, _drop)
  #endif
  #if !defined i_valfrom && defined i_valraw
    #define i_valfrom c_JOIN(i_class_val, _from)
  #endif
  #if !defined i_valtoraw && defined i_valraw
    #define i_valtoraw c_JOIN(i_class_val, _toraw)
  #endif
#endif

#ifndef i_val
  #error "i_val* must be defined for maps"
#elif defined i_valraw && !defined i_valtoraw
  #error "If i_valraw is defined, i_valtoraw must be defined too"
#elif !defined i_no_clone && (defined i_valclone ^ defined i_valdrop)
  #error "Both i_valclone and i_valdrop must be defined, if any"
#endif

#ifndef i_valfrom
  #define i_valfrom c_default_clone
  #ifdef i_valraw
    #define _i_no_put
  #endif
#else
  #undef _i_no_emplace
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
#else
  #undef _i_is_trivial
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

STC_API bool            _c_MEMB(_reserve)(Self* self, const isize_t cap);
STC_API void            _c_MEMB(_clear)(Self* self);
STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API _m_value*       _c_MEMB(_push)(Self* self, _m_value value); // push_back
STC_API void            _c_MEMB(_shrink_to_fit)(Self *self);
STC_API _m_iter         _c_MEMB(_advance)(_m_iter it, isize_t n);

#define _cbuf_toidx(self, pos) (((pos) - (self)->start) & (self)->capmask)
#define _cbuf_topos(self, idx) (((self)->start + (idx)) & (self)->capmask)

#ifndef _i_no_put
STC_INLINE void _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize_t n)
    { while (n--) _c_MEMB(_push)(self, i_keyfrom((*raw))), ++raw; }
#endif

STC_INLINE void _c_MEMB(_value_drop)(const Self* self, _m_value* val)
    { (void)self; i_keydrop(val); }

#ifndef _i_aux_alloc
    STC_INLINE Self _c_MEMB(_init)(void)
        { Self out = {0}; return out; }

    STC_INLINE Self _c_MEMB(_with_capacity)(isize_t cap) {
        cap = c_next_pow2(cap + 1);
        Self out = {_i_new_n(_m_value, cap), 0, 0, cap - 1};
        return out;
    }

    STC_INLINE Self _c_MEMB(_with_size_uninit)(isize_t size)
        { Self out = _c_MEMB(_with_capacity)(size); out.end = size; return out; }

    STC_INLINE Self _c_MEMB(_with_size)(isize_t size, _m_raw default_raw) {
        Self out = _c_MEMB(_with_capacity)(size);
        while (out.end < size) out.cbuf[out.end++] = i_keyfrom(default_raw);
        return out;
    }

    #ifndef _i_no_put
    STC_INLINE Self _c_MEMB(_from_n)(const _m_raw* raw, isize_t n) {
        Self out = _c_MEMB(_with_capacity)(n);
        _c_MEMB(_put_n)(&out, raw, n); return out;
    }
    #endif
#endif

#ifndef _i_no_emplace
STC_INLINE _m_value* _c_MEMB(_emplace)(Self* self, _m_raw raw)
    { return _c_MEMB(_push)(self, i_keyfrom(raw)); }
#endif

#if defined _i_has_eq
STC_API bool _c_MEMB(_eq)(const Self* self, const Self* other);
#endif

#ifndef i_no_clone
STC_API Self _c_MEMB(_clone)(Self q);

STC_INLINE _m_value _c_MEMB(_value_clone)(const Self* self, _m_value val)
    { (void)self; return i_keyclone(val); }

STC_INLINE void _c_MEMB(_copy)(Self* self, const Self* other) {
    if (self == other) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}
#endif // !i_no_clone

STC_INLINE isize_t _c_MEMB(_size)(const Self* self)
    { return _cbuf_toidx(self, self->end); }

STC_INLINE isize_t _c_MEMB(_capacity)(const Self* self)
    { return self->capmask; }

STC_INLINE bool _c_MEMB(_is_empty)(const Self* self)
    { return self->start == self->end; }

STC_INLINE _m_raw _c_MEMB(_value_toraw)(const _m_value* pval)
    { return i_keytoraw(pval); }

STC_INLINE const _m_value* _c_MEMB(_front)(const Self* self)
    { return self->cbuf + self->start; }

STC_INLINE _m_value* _c_MEMB(_front_mut)(Self* self)
    { return self->cbuf + self->start; }

STC_INLINE const _m_value* _c_MEMB(_back)(const Self* self)
    { return self->cbuf + ((self->end - 1) & self->capmask); }

STC_INLINE _m_value* _c_MEMB(_back_mut)(Self* self)
    { return (_m_value*)_c_MEMB(_back)(self); }

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    self->capmask = self->start = self->end = 0;
    self->cbuf = NULL;
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned)
    { _c_MEMB(_drop)(self); *self = unowned; }

STC_INLINE void _c_MEMB(_pop)(Self* self) { // pop_front
    c_assert(!_c_MEMB(_is_empty)(self));
    i_keydrop((self->cbuf + self->start));
    self->start = (self->start + 1) & self->capmask;
}

STC_INLINE _m_value _c_MEMB(_pull)(Self* self) { // move front out of queue
    c_assert(!_c_MEMB(_is_empty)(self));
    isize_t s = self->start;
    self->start = (s + 1) & self->capmask;
    return self->cbuf[s];
}

STC_INLINE _m_iter _c_MEMB(_begin)(const Self* self) {
    return c_literal(_m_iter){
        .ref=_c_MEMB(_is_empty)(self) ? NULL : self->cbuf + self->start,
        .pos=self->start, ._s=self};
}

STC_INLINE _m_iter _c_MEMB(_rbegin)(const Self* self) {
    isize_t pos = (self->end - 1) & self->capmask;
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

STC_INLINE isize_t _c_MEMB(_index)(const Self* self, _m_iter it)
    { return _cbuf_toidx(self, it.pos); }

STC_INLINE void _c_MEMB(_adjust_end_)(Self* self, isize_t n)
    { self->end = (self->end + n) & self->capmask; }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement

STC_DEF _m_iter _c_MEMB(_advance)(_m_iter it, isize_t n) {
    isize_t len = _c_MEMB(_size)(it._s);
    isize_t pos = it.pos, idx = _cbuf_toidx(it._s, pos);
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
_c_MEMB(_reserve)(Self* self, const isize_t cap) {
    isize_t oldpow2 = self->capmask + (self->capmask & 1); // handle capmask = 0
    isize_t newpow2 = c_next_pow2(cap + 1);
    if (newpow2 <= oldpow2)
        return self->cbuf != NULL;
    _m_value* d = (_m_value *)_i_realloc_n(self->cbuf, oldpow2, newpow2);
    if (d == NULL)
        return false;
    isize_t head = oldpow2 - self->start;
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
    isize_t end = (self->end + 1) & self->capmask;
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
    isize_t sz = _c_MEMB(_size)(self);
    isize_t newpow2 = c_next_pow2(sz + 1);
    if (newpow2 > self->capmask)
        return;
    if (self->start <= self->end) {
        c_memmove(self->cbuf, self->cbuf + self->start, sz*c_sizeof *self->cbuf);
        self->start = 0, self->end = sz;
    } else {
        isize_t n = self->capmask - self->start + 1;
        c_memmove(self->cbuf + (newpow2 - n), self->cbuf + self->start, n*c_sizeof *self->cbuf);
        self->start = newpow2 - n;
    }
    self->cbuf = (_m_value *)_i_realloc_n(self->cbuf, self->capmask + 1, newpow2);
    self->capmask = newpow2 - 1;
}

#ifndef i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self q) {
    Self out = q, *self = &out; (void)self; // may be used by _i_new_n/i_keyclone via i_aux.
    out.start = 0; out.end = _c_MEMB(_size)(&q);
    out.capmask = c_next_pow2(out.end + 1) - 1;
    out.cbuf = _i_new_n(_m_value, out.capmask + 1);
    isize_t i = 0;
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
// ### BEGIN_FILE_INCLUDE: finalize.h
#ifndef i_extend
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

#undef i_compare_key  // define i_keyraw, and bind i_cmp, i_eq, i_hash "class members"
#undef i_class_key
#undef i_pro_key
#undef i_cmpclass  // [deprecated]
#undef i_keycomp   // [deprecated]
#undef i_keyclass  // [deprecated]
#undef i_keypro    // [deprecated]

#undef i_key
#undef i_keyclone
#undef i_keydrop
#undef i_keyraw
#undef i_keyfrom
#undef i_keytoraw
#undef i_cmp
#undef i_less
#undef i_eq
#undef i_hash

#undef i_class_val
#undef i_pro_val
#undef i_valclass  // [deprecated]
#undef i_valpro    // [deprecated]

#undef i_val
#undef i_valclone
#undef i_valdrop
#undef i_valraw
#undef i_valfrom
#undef i_valtoraw

#undef i_use_cmp
#undef i_use_eq
#undef i_no_hash
#undef i_no_clone
#undef i_declared

#undef _i_no_put
#undef _i_no_emplace
#undef _i_is_trivial
#undef _i_aux_def
#undef _i_has_cmp
#undef _i_has_eq
#undef _i_has_default_eq
#undef _i_prefix
#undef _i_template
#undef Self
// ### END_FILE_INCLUDE: template2.h
#endif
#undef i_extend
// ### END_FILE_INCLUDE: finalize.h
// ### END_FILE_INCLUDE: queue.h


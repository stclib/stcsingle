// ### BEGIN_FILE_INCLUDE: cspan.h
#define i_header
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

#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED
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
typedef int32_t cspan_istride, _istride;

#define using_cspan use_cspan                   // [deprecated]
#define using_cspan2 use_cspan2                 // [deprecated]
#define using_cspan3 use_cspan3                 // [deprecated]
#define using_cspan2_with_eq use_cspan2_with_eq // [deprecated]
#define using_cspan3_with_eq use_cspan3_with_eq // [deprecated]

#define use_cspan(...) c_MACRO_OVERLOAD(use_cspan, __VA_ARGS__)
#define use_cspan_2(Self, T) \
    use_cspan_3(Self, T, 1); \
    STC_INLINE Self Self##_from_n(Self##_value* dataptr, isize n) \
        { return (Self)cspan_from_n(dataptr, n); } \
    STC_INLINE const Self##_value* Self##_at(const Self* self, isize idx) \
        { return cspan_at(self, idx); } \
    STC_INLINE Self##_value* Self##_at_mut(Self* self, isize idx) \
        { return cspan_at(self, idx); } \
    struct stc_nostruct

#define use_cspan_with_eq(...) c_MACRO_OVERLOAD(use_cspan_with_eq, __VA_ARGS__)
#define use_cspan_with_eq_3(Self, T, i_eq) \
    use_cspan_with_eq_4(Self, T, i_eq, 1); \
    STC_INLINE Self Self##_from_n(Self##_value* dataptr, isize n) \
        { return (Self)cspan_from_n(dataptr, n); } \
    struct stc_nostruct

#define use_cspan_3(Self, T, RANK) \
    typedef T Self##_value; \
    typedef T Self##_raw; \
    typedef struct { \
        Self##_value *data; \
        _istride shape[RANK]; \
        cspan_tuple##RANK stride; \
    } Self; \
    \
    typedef struct { \
        Self##_value *ref; \
        const Self *_s; \
        _istride pos[RANK]; \
    } Self##_iter; \
    \
    STC_INLINE Self Self##_slice_(Self##_value* d, const _istride shape[], const _istride stri[], \
                                  const isize args[][3], const int rank) { \
        Self s; int outrank; \
        s.data = d + _cspan_slice(s.shape, s.stride.d, &outrank, shape, stri, args, rank); \
        c_assert(outrank == RANK); \
        return s; \
    } \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        return c_literal(Self##_iter){ \
            .ref=RANK==1 && self->shape[0]==0 ? NULL : self->data, ._s=self}; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        (void)self; \
        return c_literal(Self##_iter){0}; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) { \
        isize off = it->_s->stride.d[RANK - 1]; \
        bool done = _cspan_next##RANK(it->pos, it->_s->shape, it->_s->stride.d, RANK, &off); \
        if (done) it->ref = NULL; else it->ref += off; \
    } \
    STC_INLINE isize Self##_size(const Self* self) \
        { return cspan_size(self); } \
    STC_INLINE Self Self##_transposed(Self sp) \
        { _cspan_transpose(sp.shape, sp.stride.d, cspan_rank(&sp)); return sp; } \
    STC_INLINE Self Self##_swapped_axes(Self sp, int ax1, int ax2) \
        { _cspan_swap_axes(sp.shape, sp.stride.d, cspan_rank(&sp), ax1, ax2); return sp; } \
    struct stc_nostruct

#define use_cspan_with_eq_4(Self, T, i_eq, RANK) \
    use_cspan_3(Self, T, RANK); \
    STC_INLINE bool Self##_eq(const Self* x, const Self* y) { \
        if (memcmp(x->shape, y->shape, sizeof x->shape) != 0) \
            return false; \
        for (Self##_iter _i = Self##_begin(x), _j = Self##_begin(y); \
             _i.ref != NULL; Self##_next(&_i), Self##_next(&_j)) \
            { if (!(i_eq(_i.ref, _j.ref))) return false; } \
        return true; \
    } \
    STC_INLINE bool Self##_equals(Self sp1, Self sp2) \
        { return Self##_eq(&sp1, &sp2); } \
    struct stc_nostruct

#define use_cspan2(Self, T) use_cspan_2(Self, T); use_cspan_3(Self##2, T, 2)
#define use_cspan3(Self, T) use_cspan2(Self, T); use_cspan_3(Self##3, T, 3)
#define use_cspan2_with_eq(Self, T, eq) use_cspan_with_eq_3(Self, T, eq); \
                                        use_cspan_with_eq_4(Self##2, T, eq, 2)
#define use_cspan3_with_eq(Self, T, eq) use_cspan2_with_eq(Self, T, eq); \
                                        use_cspan_with_eq_4(Self##3, T, eq, 3)
#define use_cspan_tuple(N) typedef struct { _istride d[N]; } cspan_tuple##N
use_cspan_tuple(1); use_cspan_tuple(2);
use_cspan_tuple(3); use_cspan_tuple(4);
use_cspan_tuple(5); use_cspan_tuple(6);
use_cspan_tuple(7); use_cspan_tuple(8);


// cspan_from_n a pointer+size
#define cspan_from_n(dataptr, n) \
    {.data=dataptr, \
     .shape={(_istride)(n)}, \
     .stride=c_literal(cspan_tuple1){.d={1}}}

// Make a fixed size, zeroed out 1d-span (in the local lexical scope).
#define cspan_zeros(Span, FIXED_N) \
    ((Span)cspan_from_n((Span##_value[FIXED_N]){0}, FIXED_N))

// May make a global scope 1d-span from initializer list, else like c_make(Span, ...).
#define cspan_make(Span, ...) \
    ((Span)cspan_from_n(c_make_array(Span##_value, __VA_ARGS__), \
                        sizeof((Span##_value[])__VA_ARGS__)/sizeof(Span##_value)))

// Make 1d-span from a c-array.
#define cspan_from_array(array) \
    cspan_from_n(array, c_arraylen(array))

// Make 1d-span from a vec or stack container.
#define cspan_from_vec(container) \
    cspan_from_n((container)->data, (container)->size)

// Make a 1d-sub-span from a 1d-span
#define cspan_subspan(self, offset, count) \
    {.data=cspan_at(self, offset), \
     .shape={(_istride)(count)}, \
     .stride=(self)->stride}

// Accessors
//
#define cspan_size(self) _cspan_size((self)->shape, cspan_rank(self))
#define cspan_rank(self) c_arraylen((self)->shape) // constexpr
#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))
#define cspan_front(self) ((self)->data)
#define cspan_back(self) ((self)->data + cspan_size(self) - 1)

#define cspan_index(...) cspan_index_fn(__VA_ARGS__, c_COMMA_N(cspan_index_3d), c_COMMA_N(cspan_index_2d), \
                                                     c_COMMA_N(cspan_index_1d),)(__VA_ARGS__)
#define cspan_index_fn(self, i,j,k,n, ...) c_TUPLE_AT_1(n, cspan_index_nd,)
#define cspan_index_1d(self, i)     (c_static_assert(cspan_rank(self) == 1), \
                                     c_assert((i) < (self)->shape[0]), \
                                     (i)*(self)->stride.d[0])
#define cspan_index_2d(self, i,j)   (c_static_assert(cspan_rank(self) == 2), \
                                     c_assert((i) < (self)->shape[0] && (j) < (self)->shape[1]), \
                                     (i)*(self)->stride.d[0] + (j)*(self)->stride.d[1])
#define cspan_index_3d(self, i,j,k) (c_static_assert(cspan_rank(self) == 3), \
                                     c_assert((i) < (self)->shape[0] && (j) < (self)->shape[1] && (k) < (self)->shape[2]), \
                                     (i)*(self)->stride.d[0] + (j)*(self)->stride.d[1] + (k)*(self)->stride.d[2])
#define cspan_index_nd(self, ...) _cspan_index((self)->shape, (self)->stride.d, c_make_array(isize, {__VA_ARGS__}), \
                                               (c_static_assert(cspan_rank(self) == c_NUMARGS(__VA_ARGS__)), cspan_rank(self)))


// Multi-dimensional span constructors
//
typedef enum {c_ROWMAJOR, c_COLMAJOR, c_STRIDED} cspan_layout;

#define cspan_is_colmajor(self) \
    _cspan_is_layout(c_COLMAJOR, (self)->shape, (self)->stride.d, cspan_rank(self))
#define cspan_is_rowmajor(self) \
    _cspan_is_layout(c_ROWMAJOR, (self)->shape, (self)->stride.d, cspan_rank(self))
#define cspan_get_layout(self) \
    (cspan_is_rowmajor(self) ? c_ROWMAJOR : cspan_is_colmajor(self) ? c_COLMAJOR : c_STRIDED)

#define cspan_md(dataptr, ...) \
    cspan_md_layout(c_ROWMAJOR, dataptr, __VA_ARGS__)

// Span2 sp1 = cspan_md(data, 30, 50);
// Span2 sp2 = {data, cspan_shape(15, 25), cspan_strides(50*2, 2)}; // every second in each dim
#define cspan_shape(...) {__VA_ARGS__}
#define cspan_strides(...) {.d={__VA_ARGS__}}

#define cspan_md_layout(layout, dataptr, ...) \
    {.data=dataptr, \
     .shape={__VA_ARGS__}, \
     .stride=*(c_JOIN(cspan_tuple,c_NUMARGS(__VA_ARGS__))*) \
             _cspan_shape2stride(layout, c_make_array(_istride, {__VA_ARGS__}), c_NUMARGS(__VA_ARGS__))}

// Transpose matrix
#define cspan_transpose(self) \
    _cspan_transpose((self)->shape, (self)->stride.d, cspan_rank(self))

// Swap two matrix axes
#define cspan_swap_axes(self, ax1, ax2) \
    _cspan_swap_axes((self)->shape, (self)->stride.d, cspan_rank(self), ax1, ax2)

// Set all span elements to value.
#define cspan_set_all(Span, self, value) do { \
    Span##_value _v = value; \
    for (c_each_3(_it, Span, *(self))) *_it.ref = _v; \
} while (0)

// General slicing function.
//
#define c_END (_istride)(((size_t)1 << (sizeof(_istride)*8 - 1)) - 1)
#define c_ALL 0,c_END

#define cspan_slice(self, Outspan, ...) \
    Outspan##_slice_((self)->data, (self)->shape, (self)->stride.d, \
                     c_make_array2d(const isize, 3, {__VA_ARGS__}), \
                     (c_static_assert(cspan_rank(self) == sizeof((isize[][3]){__VA_ARGS__})/sizeof(isize[3])), cspan_rank(self)))

// submd#(): Reduces rank, fully typesafe + range checked by default
//           int ms3[N1][N2][N3];
//           int (*ms2)[N3] = ms3[1]; // traditional, lose range test/info. VLA.
//           Span3 ms3 = cspan_md(data, N1,N2,N3); // Uses cspan_md instead.
//           *cspan_at(&ms3, 1,1,1) = 42;
//           Span2 ms2 = cspan_slice(&ms3, Span2, {1}, {c_ALL}, {c_ALL});
//           Span2 ms2 = cspan_submd3(&ms3, 1); // Same as line above, optimized.
#define cspan_submd2(self, x) \
    {.data=cspan_at(self, x, 0), \
     .shape={(self)->shape[1]}, \
     .stride=c_literal(cspan_tuple1){.d={(self)->stride.d[1]}}}

#define cspan_submd3(...) c_MACRO_OVERLOAD(cspan_submd3, __VA_ARGS__)
#define cspan_submd3_2(self, x) \
    {.data=cspan_at(self, x, 0, 0), \
     .shape={(self)->shape[1], (self)->shape[2]}, \
     .stride=c_literal(cspan_tuple2){.d={(self)->stride.d[1], (self)->stride.d[2]}}}
#define cspan_submd3_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0), \
     .shape={(self)->shape[2]}, \
     .stride=c_literal(cspan_tuple1){.d={(self)->stride.d[2]}}}

#define cspan_submd4(...) c_MACRO_OVERLOAD(cspan_submd4, __VA_ARGS__)
#define cspan_submd4_2(self, x) \
    {.data=cspan_at(self, x, 0, 0, 0), \
     .shape={(self)->shape[1], (self)->shape[2], (self)->shape[3]}, \
     .stride=c_literal(cspan_tuple3){.d={(self)->stride.d[1], (self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_3(self, x, y) \
    {.data=cspan_at(self, x, y, 0, 0), \
     .shape={(self)->shape[2], (self)->shape[3]}, \
     .stride=c_literal(cspan_tuple2){.d={(self)->stride.d[2], (self)->stride.d[3]}}}
#define cspan_submd4_4(self, x, y, z) \
    {.data=cspan_at(self, x, y, z, 0), \
     .shape={(self)->shape[3]}, \
     .stride=c_literal(cspan_tuple1){.d={(self)->stride.d[3]}}}

#define cspan_print(...) c_MACRO_OVERLOAD(cspan_print, __VA_ARGS__)
#define cspan_print_3(Span, fmt, span) \
    cspan_print_4(Span, fmt, span, stdout)
#define cspan_print_4(Span, fmt, span, fp) \
    cspan_print_5(Span, fmt, span, fp, "[]")
#define cspan_print_5(Span, fmt, span, fp, brackets) \
    cspan_print_6(Span, fmt, span, fp, brackets, c_EXPAND)
#define cspan_print_complex(Span, prec, span, fp) \
    cspan_print_6(Span, "%." #prec "f%+." #prec "fi", span, fp, "[]", cspan_CMPLX_FLD)
#define cspan_CMPLX_FLD(x) creal(x), cimag(x)

#define cspan_print_6(Span, fmt, span, fp, brackets, field) do { \
    const Span _s = span; \
    const char *_f = fmt, *_b = brackets; \
    FILE* _fp = fp; \
    int _w, _max = 0; \
    char _res[2][20], _fld[64]; \
    for (c_each_3(_it, Span, _s)) { \
        _w = snprintf(NULL, 0ULL, _f, field(_it.ref[0])); \
        if (_w > _max) _max = _w; \
    } \
    for (c_each_3(_it, Span, _s)) { \
        _cspan_print_assist(_it.pos, _s.shape, cspan_rank(&_s), _b, _res); \
        _w = _max + (_it.pos[cspan_rank(&_s) - 1] > 0); \
        snprintf(_fld, sizeof _fld, _f, field(_it.ref[0])); \
        fprintf(_fp, "%s%*s%s", _res[0], _w, _fld, _res[1]); \
    } \
} while (0)

/* ----- PRIVATE ----- */

STC_INLINE isize _cspan_size(const _istride shape[], int rank) {
    isize size = shape[0];
    while (--rank) size *= shape[rank];
    return size;
}

STC_INLINE void _cspan_swap_axes(_istride shape[], _istride stride[],
                                 int rank, int ax1, int ax2) {
    (void)rank;
    c_assert(c_uless(ax1, rank) & c_uless(ax2, rank));
    c_swap(shape + ax1, shape + ax2);
    c_swap(stride + ax1, stride + ax2);
}

STC_INLINE void _cspan_transpose(_istride shape[], _istride stride[], int rank) {
    for (int i = 0; i < --rank; ++i) {
        c_swap(shape + i, shape + rank);
        c_swap(stride + i, stride + rank);
    }
}

STC_INLINE isize _cspan_index(const _istride shape[], const _istride stride[],
                              const isize args[], int rank) {
    isize off = 0;
    (void)shape;
    while (rank-- != 0) {
        c_assert(args[rank] < shape[rank]);
        off += args[rank]*stride[rank];
    }
    return off;
}

STC_API void _cspan_print_assist(_istride pos[], const _istride shape[], const int rank,
                                 const char* brackets, char result[2][20]);

STC_API bool _cspan_nextN(_istride pos[], const _istride shape[], const _istride stride[],
                           int rank, isize* off);
#define _cspan_next1(pos, shape, stride, rank, off)            (++pos[0] == shape[0])
#define _cspan_next2(pos, shape, stride, rank, off)            (++pos[1] == shape[1] && \
    (pos[1] = 0, *off += stride[0] - (isize)shape[1]*stride[1], ++pos[0] == shape[0]))
#define _cspan_next3(pos, shape, stride, rank, off)            (++pos[2] == shape[2] && \
    (pos[2] = 0, *off += stride[1] - (isize)shape[2]*stride[2], ++pos[1] == shape[1]) && \
    (pos[1] = 0, *off += stride[0] - (isize)shape[1]*stride[1], ++pos[0] == shape[0]))
#define _cspan_next4 _cspan_nextN
#define _cspan_next5 _cspan_nextN
#define _cspan_next6 _cspan_nextN
#define _cspan_next7 _cspan_nextN
#define _cspan_next8 _cspan_nextN

STC_API isize _cspan_slice(_istride oshape[], _istride ostride[], int* orank,
                           const _istride shape[], const _istride stride[],
                           const isize args[][3], int rank);
STC_API _istride* _cspan_shape2stride(cspan_layout layout, _istride shape[], int rank);
STC_API bool _cspan_is_layout(cspan_layout layout, const _istride shape[], const _istride strides[], int rank);

#endif // STC_CSPAN_H_INCLUDED

/* --------------------- IMPLEMENTATION --------------------- */
#if defined i_implement

STC_DEF bool _cspan_is_layout(cspan_layout layout, const _istride shape[], const _istride strides[], int rank) {
    _istride tmpshape[16]; // 16 = "max" rank
    size_t sz = (size_t)rank*sizeof(_istride);
    memcpy(tmpshape, shape, sz);
    return memcmp(strides, _cspan_shape2stride(layout, tmpshape, rank), sz) == 0;
}

STC_DEF void _cspan_print_assist(_istride pos[], const _istride shape[], const int rank,
                                 const char* brackets, char result[2][20]) {
    int n = 0, j = 0, r = rank - 1;
    memset(result, 0, 32);

    // left braces:
    while (n <= r && pos[r - n] == 0)
        ++n;
    if (n) for (; j < rank; ++j)
        result[0][j] = j < rank - n ? ' ' : brackets[0];

    // right braces:
    for (j = 0; r >= 0 && pos[r] + 1 == shape[r]; --r, ++j)
        result[1][j] = brackets[1];

    // comma and newlines:
    n = (j > 0) + ((j > 1) & (j < rank));
    if (brackets[2] && j < rank)
        result[1][j++] = brackets[2]; // comma
    while (n--)
        result[1][j++] = '\n';
}

STC_DEF bool _cspan_nextN(_istride pos[], const _istride shape[], const _istride stride[],
                          int rank, isize* off) {
    ++pos[--rank];
    for (; rank && pos[rank] == shape[rank]; --rank) {
        pos[rank] = 0; ++pos[rank - 1];
        *off += stride[rank - 1] - (isize)shape[rank]*stride[rank];
    }
    return pos[rank] == shape[rank];
}

STC_DEF _istride* _cspan_shape2stride(cspan_layout layout, _istride shpstri[], int rank) {
    int i, inc;
    if (layout == c_COLMAJOR) i = 0, inc = 1;
    else i = rank - 1, inc = -1;
    _istride k = 1, s1 = shpstri[i], s2;

    shpstri[i] = 1;
    while (--rank) {
        i += inc;
        s2 = shpstri[i];
        shpstri[i] = (k *= s1);
        s1 = s2;
    }
    return shpstri;
}

STC_DEF isize _cspan_slice(_istride oshape[], _istride ostride[], int* orank,
                           const _istride shape[], const _istride stride[],
                           const isize args[][3], int rank) {
    isize end, off = 0;
    int i = 0, oi = 0;

    for (; i < rank; ++i) {
        off += args[i][0]*stride[i];
        switch (args[i][1]) {
            case 0: c_assert(c_uless(args[i][0], shape[i])); continue;
            case c_END: end = shape[i]; break;
            default: end = args[i][1];
        }
        oshape[oi] = (_istride)(end - args[i][0]);
        ostride[oi] = stride[i];
        c_assert((oshape[oi] > 0) & !c_uless(shape[i], end));
        if (args[i][2] > 0) {
            ostride[oi] *= (_istride)args[i][2];
            oshape[oi] = (oshape[oi] - 1)/(_istride)args[i][2] + 1;
        }
        ++oi;
    }
    *orank = oi;
    return off;
}
#endif // IMPLEMENT
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
// ### END_FILE_INCLUDE: cspan.h


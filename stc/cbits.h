// ### BEGIN_FILE_INCLUDE: cbits.h
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
#ifndef STC_CBITS_H_INCLUDED
#define STC_CBITS_H_INCLUDED
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

#if INTPTR_MAX == INT64_MAX
#define _gnu_popc(x) __builtin_popcountll(x)
#define _msc_popc(x) (int)__popcnt64(x)
#else
#define _gnu_popc(x) __builtin_popcount(x)
#define _msc_popc(x) (int)__popcnt(x)
#endif
#define _cbits_WS c_sizeof(uintptr_t)
#define _cbits_WB (8*_cbits_WS)
#define _cbits_bit(i) ((uintptr_t)1 << ((i) & (_cbits_WB - 1)))
#define _cbits_words(n) (isize)(((n) + (_cbits_WB - 1))/_cbits_WB)
#define _cbits_bytes(n) (_cbits_words(n)*_cbits_WS)

#if defined _MSC_VER
  #include <intrin.h>
  STC_INLINE int c_popcount(uintptr_t x) { return _msc_popc(x); }
#elif defined __GNUC__ || defined __clang__
  STC_INLINE int c_popcount(uintptr_t x) { return _gnu_popc(x); }
#else
  STC_INLINE int c_popcount(uintptr_t x) { /* http://en.wikipedia.org/wiki/Hamming_weight */
    x -= (x >> 1) & (uintptr_t)0x5555555555555555;
    x = (x & (uintptr_t)0x3333333333333333) + ((x >> 2) & (uintptr_t)0x3333333333333333);
    x = (x + (x >> 4)) & (uintptr_t)0x0f0f0f0f0f0f0f0f;
    return (int)((x*(uintptr_t)0x0101010101010101) >> (_cbits_WB - 8));
  }
#endif
#if defined __GNUC__ && !defined __clang__ && !defined __cplusplus
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=" // gcc 11.4
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"     // gcc 11.4
#endif

#define cbits_print(...) c_MACRO_OVERLOAD(cbits_print, __VA_ARGS__)
#define cbits_print_1(self) cbits_print_4(self, stdout, 0, -1)
#define cbits_print_2(self, stream) cbits_print_4(self, stream, 0, -1)
#define cbits_print_4(self, stream, start, end) cbits_print_5(cbits, self, stream, start, end)
#define cbits_print_3(SetType, self, stream) cbits_print_5(SetType, self, stream, 0, -1)
#define cbits_print_5(SetType, self, stream, start, end) do { \
    const SetType* _cb_set = self; \
    isize _cb_start = start, _cb_end = end; \
    if (_cb_end == -1) _cb_end = SetType##_size(_cb_set); \
    for (c_range_3(_cb_i, _cb_start, _cb_end)) \
        fputc(SetType##_test(_cb_set, _cb_i) ? '1' : '0', stream); \
} while (0)

STC_INLINE isize _cbits_count(const uintptr_t* set, const isize sz) {
    const isize n = sz/_cbits_WB;
    isize count = 0;
    for (isize i = 0; i < n; ++i)
        count += c_popcount(set[i]);
    if (sz & (_cbits_WB - 1))
        count += c_popcount(set[n] & (_cbits_bit(sz) - 1));
    return count;
}

STC_INLINE char* _cbits_to_str(const uintptr_t* set, const isize sz,
                               char* out, isize start, isize stop) {
    if (stop > sz) stop = sz;
    c_assert(start <= stop);

    c_memset(out, '0', stop - start);
    for (isize i = start; i < stop; ++i)
        if ((set[i/_cbits_WB] & _cbits_bit(i)) != 0)
            out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}

#define _cbits_OPR(OPR, VAL) \
    const isize n = sz/_cbits_WB; \
    for (isize i = 0; i < n; ++i) \
        if ((set[i] OPR other[i]) != VAL) \
            return false; \
    if ((sz & (_cbits_WB - 1)) == 0) \
        return true; \
    const uintptr_t i = (uintptr_t)n, m = _cbits_bit(sz) - 1; \
    return ((set[i] OPR other[i]) & m) == (VAL & m)

STC_INLINE bool _cbits_subset_of(const uintptr_t* set, const uintptr_t* other, const isize sz)
    { _cbits_OPR(|, set[i]); }

STC_INLINE bool _cbits_disjoint(const uintptr_t* set, const uintptr_t* other, const isize sz)
    { _cbits_OPR(&, 0); }

#endif // STC_CBITS_H_INCLUDED

#if defined T && !defined i_type
  #define i_type T
#endif
#if defined i_type
  #define Self c_GETARG(1, i_type)
  #define _i_length c_GETARG(2, i_type)
#else
  #define Self cbits
#endif
#ifndef i_allocator
  #define i_allocator c
#endif
#define _i_MEMB(name) c_JOIN(Self, name)


#if !defined _i_length // DYNAMIC SIZE BITARRAY

typedef struct { uintptr_t *buffer; isize _size; } Self;
#define _i_assert(x) c_assert(x)

STC_INLINE void cbits_drop(cbits* self) { i_free(self->buffer, _cbits_bytes(self->_size)); }
STC_INLINE isize cbits_size(const cbits* self) { return self->_size; }

STC_INLINE cbits* cbits_take(cbits* self, cbits other) {
    if (self->buffer != other.buffer) {
        cbits_drop(self);
        *self = other;
    }
    return self;
}

STC_INLINE cbits cbits_clone(cbits other) {
    cbits set = other;
    const isize bytes = _cbits_bytes(other._size);
    set.buffer = (uintptr_t *)c_safe_memcpy(i_malloc(bytes), other.buffer, bytes);
    return set;
}

STC_INLINE cbits* cbits_copy(cbits* self, const cbits* other) {
    if (self->buffer == other->buffer)
        return self;
    if (self->_size != other->_size)
        return cbits_take(self, cbits_clone(*other));
    c_memcpy(self->buffer, other->buffer, _cbits_bytes(other->_size));
    return self;
}

STC_INLINE bool cbits_resize(cbits* self, const isize size, const bool value) {
    const isize new_w = _cbits_words(size), osize = self->_size, old_w = _cbits_words(osize);
    uintptr_t* b = (uintptr_t *)i_realloc(self->buffer, old_w*_cbits_WS, new_w*_cbits_WS);
    if (b == NULL) return false;
    self->buffer = b; self->_size = size;
    if (size > osize) {
        c_memset(self->buffer + old_w, -(int)value, (new_w - old_w)*_cbits_WS);
        if (osize & (_cbits_WB - 1)) {
            uintptr_t mask = _cbits_bit(osize) - 1;
            if (value) self->buffer[old_w - 1] |= ~mask;
            else       self->buffer[old_w - 1] &= mask;
        }
    }
    return true;
}

STC_INLINE void cbits_set_all(cbits *self, const bool value);
STC_INLINE void cbits_set_pattern(cbits *self, const uintptr_t pattern);

STC_INLINE cbits cbits_move(cbits* self) {
    cbits tmp = *self;
    self->buffer = NULL, self->_size = 0;
    return tmp;
}

STC_INLINE cbits cbits_with_size(const isize size, const bool value) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_all(&set, value);
    return set;
}

STC_INLINE cbits cbits_with_pattern(const isize size, const uintptr_t pattern) {
    cbits set = {(uintptr_t *)i_malloc(_cbits_bytes(size)), size};
    cbits_set_pattern(&set, pattern);
    return set;
}

#else // _i_length: FIXED SIZE BITARRAY

#define _i_assert(x) (void)0

typedef struct { uintptr_t buffer[(_i_length - 1)/_cbits_WB + 1]; } Self;

STC_INLINE void     _i_MEMB(_drop)(Self* self) { (void)self; }
STC_INLINE isize    _i_MEMB(_size)(const Self* self) { (void)self; return _i_length; }
STC_INLINE Self     _i_MEMB(_move)(Self* self) { return *self; }
STC_INLINE Self*    _i_MEMB(_take)(Self* self, Self other) { *self = other; return self; }
STC_INLINE Self     _i_MEMB(_clone)(Self other) { return other; }
STC_INLINE void     _i_MEMB(_copy)(Self* self, const Self* other) { *self = *other; }
STC_INLINE void     _i_MEMB(_set_all)(Self *self, const bool value);
STC_INLINE void     _i_MEMB(_set_pattern)(Self *self, const uintptr_t pattern);

STC_INLINE Self _i_MEMB(_with_size)(const isize size, const bool value) {
    c_assert(size <= _i_length);
    Self set; _i_MEMB(_set_all)(&set, value);
    return set;
}

STC_INLINE Self _i_MEMB(_with_pattern)(const isize size, const uintptr_t pattern) {
    c_assert(size <= _i_length);
    Self set; _i_MEMB(_set_pattern)(&set, pattern);
    return set;
}
#endif // _i_length

// COMMON:

STC_INLINE void _i_MEMB(_set_all)(Self *self, const bool value)
    { c_memset(self->buffer, -(int)value, _cbits_bytes(_i_MEMB(_size)(self))); }

STC_INLINE void _i_MEMB(_set_pattern)(Self *self, const uintptr_t pattern) {
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] = pattern;
}

STC_INLINE bool _i_MEMB(_test)(const Self* self, const isize i)
    { return (self->buffer[i/_cbits_WB] & _cbits_bit(i)) != 0; }

STC_INLINE bool _i_MEMB(_at)(const Self* self, const isize i)
    { c_assert(c_uless(i, _i_MEMB(_size)(self))); return _i_MEMB(_test)(self, i); }

STC_INLINE void _i_MEMB(_set)(Self *self, const isize i)
    { self->buffer[i/_cbits_WB] |= _cbits_bit(i); }

STC_INLINE void _i_MEMB(_reset)(Self *self, const isize i)
    { self->buffer[i/_cbits_WB] &= ~_cbits_bit(i); }

STC_INLINE void _i_MEMB(_set_value)(Self *self, const isize i, const bool b) {
    self->buffer[i/_cbits_WB] ^= ((uintptr_t)-(int)b ^ self->buffer[i/_cbits_WB]) & _cbits_bit(i);
}

STC_INLINE void _i_MEMB(_flip)(Self *self, const isize i)
    { self->buffer[i/_cbits_WB] ^= _cbits_bit(i); }

STC_INLINE void _i_MEMB(_flip_all)(Self *self) {
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] ^= ~(uintptr_t)0;
}

STC_INLINE Self _i_MEMB(_from)(const char* str) {
    isize n = c_strlen(str);
    Self set = _i_MEMB(_with_size)(n, false);
    while (n--) if (str[n] == '1') _i_MEMB(_set)(&set, n);
    return set;
}

/* Intersection */
STC_INLINE void _i_MEMB(_intersect)(Self *self, const Self* other) {
    _i_assert(self->_size == other->_size);
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] &= other->buffer[n];
}
/* Union */
STC_INLINE void _i_MEMB(_union)(Self *self, const Self* other) {
    _i_assert(self->_size == other->_size);
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] |= other->buffer[n];
}
/* Exclusive disjunction */
STC_INLINE void _i_MEMB(_xor)(Self *self, const Self* other) {
    _i_assert(self->_size == other->_size);
    isize n = _cbits_words(_i_MEMB(_size)(self));
    while (n--) self->buffer[n] ^= other->buffer[n];
}

STC_INLINE isize _i_MEMB(_count)(const Self* self)
    { return _cbits_count(self->buffer, _i_MEMB(_size)(self)); }

STC_INLINE char* _i_MEMB(_to_str)(const Self* self, char* out, isize start, isize stop)
    { return _cbits_to_str(self->buffer, _i_MEMB(_size)(self), out, start, stop); }

STC_INLINE bool _i_MEMB(_subset_of)(const Self* self, const Self* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_subset_of(self->buffer, other->buffer, _i_MEMB(_size)(self));
}

STC_INLINE bool _i_MEMB(_disjoint)(const Self* self, const Self* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_disjoint(self->buffer, other->buffer, _i_MEMB(_size)(self));
}

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
#undef i_type
#undef _i_length
#undef _i_MEMB
#undef _i_assert
#undef Self
// ### END_FILE_INCLUDE: cbits.h


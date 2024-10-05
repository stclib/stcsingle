// ### BEGIN_FILE_INCLUDE: sort.h
#ifndef _i_template
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

#ifndef __cplusplus
    #define _i_malloc(T, n)     ((T*)i_malloc((n)*c_sizeof(T)))
    #define _i_calloc(T, n)     ((T*)i_calloc(n, c_sizeof(T)))
    #define c_new(T, ...)       ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_literal(T)        (T)
    #define c_make_array(T, ...) ((T[])__VA_ARGS__)
    #define c_make_array2d(T, N, ...) ((T[][N])__VA_ARGS__)
#else
    #include <new>
    #define _i_malloc(T, n)     static_cast<T*>(i_malloc((n)*c_sizeof(T)))
    #define _i_calloc(T, n)     static_cast<T*>(i_calloc(n, c_sizeof(T)))
    #define c_new(T, ...)       new (malloc(sizeof(T))) T(__VA_ARGS__)
    #define c_literal(T)        T
    template<typename T, int M, int N> struct _c_Array { T data[M][N]; };
    #define c_make_array(T, ...) (_c_Array<T, 1, sizeof((T[])__VA_ARGS__)/sizeof(T)>{{__VA_ARGS__}}.data[0])
    #define c_make_array2d(T, N, ...) (_c_Array<T, sizeof((T[][N])__VA_ARGS__)/sizeof(T[N]), N>{__VA_ARGS__}.data)
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

#ifndef __cplusplus
    #define c_foritems(it, T, ...) \
        for (struct {T* ref; int size, index;} \
             it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
             ; it.index < it.size; ++it.ref, ++it.index)
#else
    #include <initializer_list>
    #define c_foritems(it, T, ...) \
        for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator ref; size_t size, index;} \
             it = {._il=__VA_ARGS__, .ref=it._il.begin(), .size=it._il.size()} \
             ; it.index < it.size; ++it.ref, ++it.index)
#endif
#define c_forlist(...) c_foritems(_VA_ARGS__) // [deprecated]
#define c_forpair(...) 'c_forpair not_supported. Use c_foreach_kv' // [removed]

// c_forrange, c_forrange32: python-like int range iteration
#define c_forrange_ex(...) c_MACRO_OVERLOAD(c_forrange_ex, __VA_ARGS__)
#define c_forrange_ex_3(T, i, stop) c_forrange_ex_4(T, i, 0, stop)
#define c_forrange_ex_4(T, i, start, stop) \
    for (T i=start, _c_end=stop; i < _c_end; ++i)
#define c_forrange_ex_5(T, i, start, stop, step) \
    for (T i=start, _c_inc=step, _c_end=(stop) - (_c_inc > 0) \
         ; (_c_inc > 0) == (i <= _c_end); i += _c_inc)

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_ex_4(isize, _c_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_ex_4(isize, i, 0, stop)
#define c_forrange_3(i, start, stop) c_forrange_ex_4(isize, i, start, stop)
#define c_forrange_4(i, start, stop, step) c_forrange_ex_5(isize, i, start, stop, step)

#define c_forrange32(...) c_MACRO_OVERLOAD(c_forrange32, __VA_ARGS__)
#define c_forrange32_2(i, stop) c_forrange_ex_4(int32_t, i, 0, stop)
#define c_forrange32_3(i, start, stop) c_forrange_ex_4(int32_t, i, start, stop)
#define c_forrange32_4(i, start, stop, step) c_forrange_ex_5(int32_t, i, start, stop, step)

// init container with literal list, and drop multiple containers of same type
#define c_init(C, ...) \
    C##_from_n(c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

#define c_push(C, cnt, ...) \
    C##_put_n(cnt, c_make_array(C##_raw, __VA_ARGS__), c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

#define c_drop(C, ...) \
    do { c_foritems (_c_i, C*, {__VA_ARGS__}) C##_drop(*_c_i.ref); } while(0)

#define c_func(name, args, RIGHTARROW, ...) \
    typedef __VA_ARGS__ name##_result; name##_result name args

// RAII scopes
#define c_defer(...) \
    for (int _c_i = 1; _c_i; _c_i = 0, __VA_ARGS__)

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(init, deinit) \
    for (int _c_i = 1; _c_i; ) for (init; _c_i; _c_i = 0, deinit) // thanks, tstanisl
#define c_with_3(init, condition, deinit) \
    for (int _c_i = 1; _c_i; ) for (init; _c_i && (condition); _c_i = 0, deinit)

// General functions

// hashing
STC_INLINE size_t c_hash_n(const void* key, isize len) {
    union { size_t block; uint64_t b8; uint32_t b4; } u;
    switch (len) {
        case 8: memcpy(&u.b8, key, 8); return (size_t)(u.b8 * 0xc6a4a7935bd1e99d);
        case 4: memcpy(&u.b4, key, 4); return u.b4 * (size_t)0xa2ffeb2f01000193;
        case 0: return 0x811c9dc5;
    }
    size_t hash = 0x811c9dc5;
    const uint8_t* msg = (const uint8_t*)key;
    while (len >= c_sizeof(size_t)) {
        memcpy(&u.block, msg, sizeof(size_t));
        hash = (hash ^ u.block) * (size_t)0x89bb179901000193;
        msg += c_sizeof(size_t);
        len -= c_sizeof(size_t);
    }
    while (len--) hash = (hash ^ *msg++) * (size_t)0xb0340f4501000193;
    return hash ^ (hash >> 3);
}

STC_INLINE size_t c_hash_str(const char *str)
    { return c_hash_n(str, c_strlen(str)); }

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

// substring in substring?
STC_INLINE char* c_strnstrn(const char *str, isize slen,
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

// 128-bit multiplication
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

  #define _i_is_array
  #if defined i_type && !defined i_key
    #define Self c_SELECT(_c_SEL21, i_type)
    #define i_key c_SELECT(_c_SEL22, i_type)
  #elif defined i_type
    #define Self i_type
  #else
    #define Self c_JOIN(i_key, s)
  #endif

  typedef i_key Self;
  typedef Self c_JOIN(Self, _value), c_JOIN(Self, _raw);
  #define i_at(arr, idx) (&(arr)[idx])
  #define i_at_mut i_at
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
#elif defined i_keyraw && !defined i_keytoraw
  #error "If i_keyraw/i_valraw is defined, i_keytoraw/i_valtoraw must be defined too"
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
#endif

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

#ifdef _i_is_array
  #undef _i_is_array
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
#endif
#undef i_at
#undef i_at_mut
// ### END_FILE_INCLUDE: sort.h


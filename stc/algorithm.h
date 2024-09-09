// ### BEGIN_FILE_INCLUDE: algorithm.h
#ifndef STC_ALGORITHM_H_INCLUDED
#define STC_ALGORITHM_H_INCLUDED

// ### BEGIN_FILE_INCLUDE: crange.h
#ifndef STC_CRANGE_H_INCLUDED
#define STC_CRANGE_H_INCLUDED

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

typedef isize crange_value;
typedef struct { crange_value start, end, step, value; } crange;
typedef struct { crange_value *ref, end, step; } crange_iter;

#define crange_from(...) c_MACRO_OVERLOAD(crange_from, __VA_ARGS__)
#define crange_from_1(start) crange_from_3(start, INTPTR_MAX, 1)
#define crange_from_2(start, stop) crange_from_3(start, stop, 1)

#define c_iota(...) c_MACRO_OVERLOAD(c_iota, __VA_ARGS__)
#define c_iota_1(start) c_iota_3(start, INTPTR_MAX, 1)
#define c_iota_2(start, stop) c_iota_3(start, stop, 1)
#define c_iota_3(start, stop, step) ((crange[]){crange_from_3(start, stop, step)})[0]

STC_INLINE crange crange_from_3(crange_value start, crange_value stop, crange_value step)
    { crange r = {start, stop - (step > 0), step}; return r; }

STC_INLINE crange_iter crange_begin(crange* self) {
    self->value = self->start;
    crange_iter it = {&self->value, self->end, self->step};
    return it;
}

STC_INLINE void crange_next(crange_iter* it) {
    if ((it->step > 0) == ((*it->ref += it->step) > it->end))
        it->ref = NULL;
}

#endif // STC_CRANGE_H_INCLUDE
// ### END_FILE_INCLUDE: crange.h
// ### BEGIN_FILE_INCLUDE: filter.h
#ifndef STC_FILTER_H_INCLUDED
#define STC_FILTER_H_INCLUDED


// ------- c_filter --------
#define c_flt_take(n) _flt_take(&_base, n)
#define c_flt_skip(n) (c_flt_counter() > (n))
#define c_flt_takewhile(pred) _flt_takewhile(&_base, pred)
#define c_flt_skipwhile(pred) (_base.sb[_base.sb_top++] |= !(pred))
#define c_flt_counter() (++_base.sn[++_base.sn_top])
#define c_flt_getcount() (_base.sn[_base.sn_top])
#define c_flt_map(expr) (_mapped = (expr), value = &_mapped)
#define c_flt_src _it.ref

#define c_filter(C, cnt, pred) \
    _c_filter(C, C##_begin(&cnt), _, pred)

#define c_filter_from(C, start, pred) \
    _c_filter(C, start, _, pred)

#define c_filter_reverse(C, cnt, pred) \
    _c_filter(C, C##_rbegin(&cnt), _r, pred)

#define c_filter_reverse_from(C, start, pred) \
    _c_filter(C, start, _r, pred)

#define _c_filter(C, start, rev, pred) do { \
    struct _flt_base _base = {0}; \
    C##_iter _it = start; \
    C##_value *value = _it.ref, _mapped; \
    for ((void)_mapped ; !_base.done & (_it.ref != NULL) ; \
         C##rev##next(&_it), value = _it.ref, _base.sn_top=0, _base.sb_top=0) \
      (void)(pred); \
} while (0)

// ------- c_filter_zip --------
#define c_filter_zip(...) c_MACRO_OVERLOAD(c_filter_zip, __VA_ARGS__)
#define c_filter_zip_4(C, cnt1, cnt2, pred) \
    c_filter_zip_5(C, cnt1, C, cnt2, pred)
#define c_filter_zip_5(C1, cnt1, C2, cnt2, pred) \
    _c_filter_zip(C1, C1##_begin(&cnt1), C2, C2##_begin(&cnt2), _, pred)

#define c_filter_reverse_zip(...) c_MACRO_OVERLOAD(c_filter_reverse_zip, __VA_ARGS__)
#define c_filter_reverse_zip_4(C, cnt1, cnt2, pred) \
    c_filter_reverse_zip_5(C, cnt1, C, cnt2, pred)
#define c_filter_reverse_zip_5(C1, cnt1, C2, cnt2, pred) \
    _c_filter_zip(C1, C1##_rbegin(&cnt1), C2, C2##_rbegin(&cnt2), _r, pred)

#define c_filter_pairwise(C, cnt, pred) \
    _c_filter_zip(C, C##_begin(&cnt), C, C##_advance(_it1, 1), _, pred)

#define c_flt_map1(expr) (_mapped1 = (expr), value1 = &_mapped1)
#define c_flt_map2(expr) (_mapped2 = (expr), value2 = &_mapped2)
#define c_flt_src1 _it1.ref
#define c_flt_src2 _it2.ref

#define _c_filter_zip(C1, start1, C2, start2, rev, pred) do { \
    struct _flt_base _base = {0}; \
    C1##_iter _it1 = start1; \
    C2##_iter _it2 = start2; \
    C1##_value* value1 = _it1.ref, _mapped1; (void)_mapped1; \
    C2##_value* value2 = _it2.ref, _mapped2; (void)_mapped2; \
    for (; !_base.done & (_it1.ref != NULL) & (_it2.ref != NULL); \
         C1##rev##next(&_it1), value1 = _it1.ref, C2##rev##next(&_it2), value2 = _it2.ref, \
         _base.sn_top=0, _base.sb_top=0) \
      (void)(pred); \
} while (0)

// ------- c_forfilter --------
// c_forfilter allows to execute imperative statements for each element
// as it is a for-loop, e.g., calling nested generic statements instead
// of defining a function/expression for it:
#define c_fflt_take(i, n) _flt_take(&i.base, n)
#define c_fflt_skip(i, n) (c_fflt_counter(i) > (n))
#define c_fflt_takewhile(i, pred) _flt_takewhile(&i.base, pred)
#define c_fflt_skipwhile(i, pred) (i.base.sb[i.base.sb_top++] |= !(pred))
#define c_fflt_counter(i) (++i.base.sn[++i.base.sn_top])
#define c_fflt_getcount(i) (i.base.sn[i.base.sn_top])
#define c_fflt_map(i, expr) (i.mapped = (expr), i.ref = &i.mapped)
#define c_fflt_src(i) i.iter.ref

#define c_forfilter(i, C, cnt, pred) \
    _c_forfilter(i, C, C##_begin(&cnt), _, pred)

#define c_forfilter_reverse(i, C, cnt,pred) \
    _c_forfilter(i, C, C##_rbegin(&cnt), _r, pred)

#define c_forfilter_from(i, C, start, pred) \
    _c_forfilter(i, C, start, _, pred)

#define c_forfilter_reverse_from(i, C, start, pred) \
    _c_forfilter(i, C, start, _r, pred)

#define _c_forfilter(i, C, start, rev, pred) \
    for (struct {C##_iter iter; C##_value *ref, mapped; struct _flt_base base;} \
         i = {.iter=start, .ref=i.iter.ref} ; !i.base.done & (i.iter.ref != NULL) ; \
         C##rev##next(&i.iter), i.ref = i.iter.ref, i.base.sn_top=0, i.base.sb_top=0) \
      if (!(pred)) ; else

// ------------------------ private -------------------------
#ifndef c_NFILTERS
#define c_NFILTERS 20
#endif

struct _flt_base {
    uint8_t sn_top, sb_top;
    bool done, sb[c_NFILTERS];
    uint32_t sn[c_NFILTERS];
};

static inline bool _flt_take(struct _flt_base* base, uint32_t n) {
    uint32_t k = ++base->sn[++base->sn_top];
    base->done |= (k >= n);
    return n > 0;
}

static inline bool _flt_takewhile(struct _flt_base* base, bool pred) {
    bool skip = (base->sb[base->sb_top++] |= !pred);
    base->done |= skip;
    return !skip;
}

#endif // STC_FILTER_H_INCLUDED
// ### END_FILE_INCLUDE: filter.h
// ### BEGIN_FILE_INCLUDE: utility.h
#ifndef STC_UTILITY_H_INCLUDED
#define STC_UTILITY_H_INCLUDED

// --------------------------------
// c_find_if, c_find_reverse_if
// --------------------------------

#define c_find_if(...) c_MACRO_OVERLOAD(c_find_if, __VA_ARGS__)
#define c_find_if_4(C, cnt, outit_ptr, pred) \
    _c_find(C, C##_begin(&cnt), NULL, _, outit_ptr, pred)

#define c_find_if_5(C, start, finish, outit_ptr, pred) \
    _c_find(C, start, (finish).ref, _, outit_ptr, pred)

#define c_find_reverse_if(...) c_MACRO_OVERLOAD(c_find_reverse_if, __VA_ARGS__)
#define c_find_reverse_if_4(C, cnt, outit_ptr, pred) \
    _c_find(C, C##_rbegin(&cnt), NULL, _r, outit_ptr, pred)

#define c_find_reverse_if_5(C, rstart, rfinish, outit_ptr, pred) \
    _c_find(C, rstart, (rfinish).ref, _r, outit_ptr, pred)

// private
#define _c_find(C, start, endref, rev, outit_ptr, pred) do { \
    C##_iter* _out = outit_ptr; \
    const C##_value *value, *_endref = endref; \
    for (*_out = start; (value = _out->ref) != _endref; C##rev##next(_out)) \
        if (pred) goto c_JOIN(findif_, __LINE__); \
    _out->ref = NULL; c_JOIN(findif_, __LINE__):; \
} while (0)

// --------------------------------
// c_erase_if
// --------------------------------

// Use with: list, hmap, hset, smap, sset:
#define c_erase_if(C, cnt_ptr, pred) do { \
    C* _cnt = cnt_ptr; \
    const C##_value* value; \
    for (C##_iter _it = C##_begin(_cnt); (value = _it.ref); ) { \
        if (pred) _it = C##_erase_at(_cnt, _it); \
        else C##_next(&_it); \
    } \
} while (0)

// --------------------------------
// c_eraseremove_if
// --------------------------------

// Use with: stack, vec, deque, queue:
#define c_eraseremove_if(C, cnt_ptr, pred) do { \
    C* _cnt = cnt_ptr; \
    isize _n = 0; \
    const C##_value* value; \
    C##_iter _i, _it = C##_begin(_cnt); \
    while ((value = _it.ref) && !(pred)) \
        C##_next(&_it); \
    for (_i = _it; (value = _it.ref); C##_next(&_it)) { \
        if (pred) C##_value_drop(_it.ref), ++_n; \
        else *_i.ref = *_it.ref, C##_next(&_i); \
    } \
    C##_adjust_end_(_cnt, -_n); \
} while (0)

// ------------------------------------
// c_copy, c_copy_if, c_copy_reverse_if
// ------------------------------------

#define c_copy(...) c_MACRO_OVERLOAD(c_copy, __VA_ARGS__)
#define c_copy_3(C, cnt, outcnt_ptr) \
    _c_copy_if(C, cnt, _, C, outcnt_ptr, true)

#define c_copy_4(C, cnt, C_out, outcnt_ptr) \
    _c_copy_if(C, cnt, _, C_out, outcnt_ptr, true)

#define c_copy_reverse(...) c_MACRO_OVERLOAD(c_copy_reverse, __VA_ARGS__)
#define c_copy_reverse_3(C, cnt, outcnt_ptr) \
    _c_copy_if(C, cnt, _r, C, outcnt_ptr, true)

#define c_copy_reverse_4(C, cnt, C_out, outcnt_ptr) \
    _c_copy_if(C, cnt, _r, C_out, outcnt_ptr, true)


#define c_copy_if(...) c_MACRO_OVERLOAD(c_copy_if, __VA_ARGS__)
#define c_copy_if_4(C, cnt, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _, C, outcnt_ptr, pred)

#define c_copy_if_5(C, cnt, C_out, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _, C_out, outcnt_ptr, pred)

#define c_copy_reverse_if(...) c_MACRO_OVERLOAD(c_copy_reverse_if, __VA_ARGS__)
#define c_copy_reverse_if_4(C, cnt, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _r, C, outcnt_ptr, pred)

#define c_copy_reverse_if_5(C, cnt, C_out, outcnt_ptr, pred) \
    _c_copy_if(C, cnt, _r, C_out, outcnt_ptr, pred)

// private
#define _c_copy_if(C, cnt, rev, C_out, outcnt_ptr, pred) do { \
    C _cnt = cnt; \
    C_out *_out = outcnt_ptr; \
    const C##_value* value; \
    for (C##_iter _it = C##rev##begin(&_cnt); (value = _it.ref); C##rev##next(&_it)) \
        if (pred) C_out##_push(_out, C##_value_clone(*_it.ref)); \
} while (0)

// --------------------------------
// c_all_of, c_any_of, c_none_of
// --------------------------------

#define c_all_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, !(pred)); \
    *(outbool_ptr) = _it.ref == NULL; \
} while (0)

#define c_any_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, pred); \
    *(outbool_ptr) = _it.ref != NULL; \
} while (0)

#define c_none_of(C, cnt, outbool_ptr, pred) do { \
    C##_iter _it; \
    c_find_if_4(C, cnt, &_it, pred); \
    *(outbool_ptr) = _it.ref == NULL; \
} while (0)

#endif // STC_UTILITY_H_INCLUDED
// ### END_FILE_INCLUDE: utility.h

#endif // STC_ALGORITHM_H_INCLUDED
// ### END_FILE_INCLUDE: algorithm.h


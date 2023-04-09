// ### BEGIN_FILE_INCLUDE: calgo.h
#ifndef STC_CALGO_INCLUDED
#define STC_CALGO_INCLUDED

// ### BEGIN_FILE_INCLUDE: crange.h
#ifndef STC_CRANGE_H_INCLUDED
#define STC_CRANGE_H_INCLUDED

// ### BEGIN_FILE_INCLUDE: ccommon.h
#ifndef CCOMMON_H_INCLUDED
#define CCOMMON_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
// ### BEGIN_FILE_INCLUDE: altnames.h
#define c_FORLIST c_forlist
#define c_FORRANGE c_forrange
#define c_FOREACH c_foreach
#define c_FORPAIR c_forpair
#define c_FORFILTER c_forfilter
#define c_FORMATCH c_formatch
#define c_FORTOKEN c_fortoken
#define c_FORTOKEN_SV c_fortoken_sv
#define c_AUTO c_auto
#define c_WITH c_with
#define c_SCOPE c_scope
#define c_DEFER c_defer
// ### END_FILE_INCLUDE: altnames.h
// ### BEGIN_FILE_INCLUDE: raii.h
#define c_defer(...) \
    for (int _i = 1; _i; _i = 0, __VA_ARGS__)

#define c_with(...) c_MACRO_OVERLOAD(c_with, __VA_ARGS__)
#define c_with_2(declvar, drop) \
    for (declvar, *_i, **_ip = &_i; _ip; _ip = 0, drop)
#define c_with_3(declvar, pred, drop) \
    for (declvar, *_i, **_ip = &_i; _ip && (pred); _ip = 0, drop)

#define c_scope(...) c_MACRO_OVERLOAD(c_scope, __VA_ARGS__)
#define c_scope_2(init, drop) \
    for (int _i = (init, 1); _i; _i = 0, drop)
#define c_scope_3(init, pred, drop) \
    for (int _i = (init, 1); _i && (pred); _i = 0, drop)

#define c_auto(...) c_MACRO_OVERLOAD(c_auto, __VA_ARGS__)
#define c_auto_2(C, a) \
    c_with_2(C a = C##_init(), C##_drop(&a))
#define c_auto_3(C, a, b) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init()), \
            (C##_drop(&b), C##_drop(&a)))
#define c_auto_4(C, a, b, c) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init()), \
            (C##_drop(&c), C##_drop(&b), C##_drop(&a)))
#define c_auto_5(C, a, b, c, d) \
    c_with_2(c_EXPAND(C a = C##_init(), b = C##_init(), c = C##_init(), d = C##_init()), \
            (C##_drop(&d), C##_drop(&c), C##_drop(&b), C##_drop(&a)))
// ### END_FILE_INCLUDE: raii.h

#define c_NPOS INTPTR_MAX
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR
#if defined STC_NDEBUG || defined NDEBUG
  #define c_ASSERT(expr) (void)(0)
#else
  #define c_ASSERT(expr) assert(expr)
#endif

#if defined(_MSC_VER)
  #pragma warning(disable: 4116 4996) // unnamed type definition in parentheses
  #define STC_FORCE_INLINE static __forceinline
#elif defined(__GNUC__) || defined(__clang__)
  #define STC_FORCE_INLINE static inline __attribute((always_inline))
#else
  #define STC_FORCE_INLINE static inline
#endif
#define STC_INLINE static inline

/* Macro overloading feature support based on: https://rextester.com/ONP80107 */
#define c_MACRO_OVERLOAD(name, ...) \
    c_PASTE(c_CONCAT(name,_), c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_CONCAT(a, b) a ## b
#define c_PASTE(a, b) c_CONCAT(a, b)
#define c_EXPAND(...) __VA_ARGS__
#define c_NUMARGS(...) _c_APPLY_ARG_N((__VA_ARGS__, _c_RSEQ_N))
#define _c_APPLY_ARG_N(args) c_EXPAND(_c_ARG_N args)
#define _c_RSEQ_N 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define _c_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                 _14, _15, _16, N, ...) N

#ifdef __cplusplus
  #include <new>
  #define _i_alloc(T)           static_cast<T*>(i_malloc(c_sizeof(T)))
  #define _i_new(T, ...)        new (_i_alloc(T)) T(__VA_ARGS__)
  #define c_new(T, ...)         new (malloc(sizeof(T))) T(__VA_ARGS__)
  #define c_LITERAL(T)          T
#else
  #define _i_alloc(T)           ((T*)i_malloc(c_sizeof(T)))
  #define _i_new(T, ...)        ((T*)memcpy(_i_alloc(T), (T[]){__VA_ARGS__}, sizeof(T)))
  #define c_new(T, ...)         ((T*)memcpy(malloc(sizeof(T)), (T[]){__VA_ARGS__}, sizeof(T)))
  #define c_LITERAL(T)          (T)
#endif
#define c_malloc(sz)            malloc(c_i2u(sz))
#define c_calloc(n, sz)         calloc(c_i2u(n), c_i2u(sz))
#define c_realloc(p, sz)        realloc(p, c_i2u(sz))
#define c_free(p)               free(p)
#define c_delete(T, ptr)        do { T *_tp = ptr; T##_drop(_tp); free(_tp); } while (0)

#define c_static_assert(b)      ((int)(0*sizeof(int[(b) ? 1 : -1])))
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(T, p)      ((T)(p) + 0*sizeof((T)0 == (p)))
#define c_swap(T, xp, yp)       do { T *_xp = xp, *_yp = yp, \
                                    _tv = *_xp; *_xp = *_yp; *_yp = _tv; } while (0)
#define c_sizeof                (intptr_t)sizeof
#define c_strlen(s)             (intptr_t)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u(ilen))

#define c_u2i(u)                ((intptr_t)((u) + 0*sizeof((u) == 1U)))
#define c_i2u(i)                ((size_t)(i) + 0*sizeof((i) == 1))
#define c_LTu(a, b)             ((size_t)(a) < (size_t)(b))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_hash(x)       cfasthash(x, c_sizeof(*(x)))

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

#define c_option(flag)          ((i_opt) & (flag))
#define c_is_forward            (1<<0)
#define c_no_atomic             (1<<1)
#define c_no_clone              (1<<2)
#define c_no_emplace            (1<<3)
#define c_no_cmp                (1<<4)
#define c_no_hash               (1<<5)

/* Function macros and others */

#define c_make(C, ...) \
  C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (intptr_t)(sizeof(a)/sizeof 0[a])

// Non-owning c-string
typedef const char* crawstr;
#define crawstr_clone(s) (s)
#define crawstr_drop(p) ((void)p)
#define crawstr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define crawstr_hash(p) cstrhash(*(p))

#define c_sv(...) c_MACRO_OVERLOAD(c_sv, __VA_ARGS__)
#define c_sv_1(lit) c_sv_2(lit, c_litstrlen(lit))
#define c_sv_2(str, n) (c_LITERAL(csview){str, n})

#define c_SV(sv) (int)(sv).size, (sv).str // print csview: use format "%.*s"
#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t cfasthash(const void* key, intptr_t len) {
    uint32_t u4; uint64_t u8;
    switch (len) {
        case 8: memcpy(&u8, key, 8); return u8*0xc6a4a7935bd1e99d;
        case 4: memcpy(&u4, key, 4); return u4*0xc6a4a7935bd1e99d;
        case 0: return 1;
    }
    const uint8_t *x = (const uint8_t*)key;
    uint64_t h = *x, n = (uint64_t)len >> 3;
    len &= 7;
    while (n--) {
        memcpy(&u8, x, 8), x += 8;
        h += u8*0xc6a4a7935bd1e99d;
    }
    while (len--) h = (h << 10) - h - *x++;
    return c_ROTL(h, 26) ^ h;
}

STC_INLINE uint64_t cstrhash(const char *str)
    { return cfasthash(str, c_strlen(str)); }

STC_INLINE char* cstrnstrn(const char *str, const char *needle,
                           intptr_t slen, const intptr_t nlen) {
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

/* Control block macros */

#define c_foreach(...) c_MACRO_OVERLOAD(c_foreach, __VA_ARGS__)
#define c_foreach_3(it, C, cnt) \
    for (C##_iter it = C##_begin(&cnt); it.ref; C##_next(&it))
#define c_foreach_4(it, C, start, finish) \
    for (C##_iter it = start, *_endref = (C##_iter*)(finish).ref \
         ; it.ref != (C##_value*)_endref; C##_next(&it))

#define c_foreach_rv(it, C, cnt) \
    for (C##_iter it = {.ref=C##_end(&cnt).end - 1, .end=(cnt).data - 1} \
         ; it.ref != it.end; --it.ref)

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter it; const C##_key* key; C##_mapped* val;} _ = {.it=C##_begin(&cnt)} \
         ; _.it.ref && (_.key = &_.it.ref->first, _.val = &_.it.ref->second) \
         ; C##_next(&_.it))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_c_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (long long i=start, _end=(long long)(stop); i < _end; ++i)
#define c_forrange_4(i, start, stop, step) \
    for (long long i=start, _inc=step, _end=(long long)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
  #define c_forlist(it, T, ...) \
    for (struct {T* ref; int size, index;} \
         it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
         ; it.index < it.size; ++it.ref, ++it.index)
#else
  #include <initializer_list>
  #define c_forlist(it, T, ...) \
    for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator data, ref; size_t size, index;} \
         it = {._il=__VA_ARGS__, .data=it._il.begin(), .ref=it.data, .size=it._il.size()} \
         ; it.index < it.size; ++it.ref, ++it.index)
#endif

#define c_drop(C, ...) \
    do { c_forlist (_i, C*, {__VA_ARGS__}) C##_drop(*_i.ref); } while(0)

#endif // CCOMMON_H_INCLUDED

#undef STC_API
#undef STC_DEF

#if !defined(i_static) && !defined(STC_STATIC) && (defined(i_header) || defined(STC_HEADER) || \
                                                   defined(i_implement) || defined(STC_IMPLEMENT))
  #define STC_API extern
  #define STC_DEF
#else
  #define i_static
  #define STC_API static inline
  #define STC_DEF static inline
#endif
#if defined(STC_EXTERN)
  #define i_extern
#endif
#if defined(i_static) || defined(STC_IMPLEMENT)
  #define i_implement
#endif
// ### END_FILE_INCLUDE: ccommon.h

#define crange_obj(...) \
    (*(crange[]){crange_make(__VA_ARGS__)})

typedef long long crange_value;
typedef struct { crange_value start, end, step, value; } crange;
typedef struct { crange_value *ref, end, step; } crange_iter;

#define crange_make(...) c_MACRO_OVERLOAD(crange_make, __VA_ARGS__)
#define crange_make_1(stop) crange_make_3(0, stop, 1)
#define crange_make_2(start, stop) crange_make_3(start, stop, 1)

STC_INLINE crange crange_make_3(crange_value start, crange_value stop, crange_value step)
    { crange r = {start, stop - (step > 0), step}; return r; }

STC_INLINE crange_iter crange_begin(crange* self)
    { self->value = self->start; crange_iter it = {&self->value, self->end, self->step}; return it; }

STC_INLINE crange_iter crange_end(crange* self) 
    { crange_iter it = {NULL}; return it; }

STC_INLINE void crange_next(crange_iter* it) 
    { *it->ref += it->step; if ((it->step > 0) == (*it->ref > it->end)) it->ref = NULL; }

#endif
// ### END_FILE_INCLUDE: crange.h
// ### BEGIN_FILE_INCLUDE: filter.h
#ifndef STC_FILTER_H_INCLUDED
#define STC_FILTER_H_INCLUDED


// c_forfilter:

#define c_flt_skip(i, n) (c_flt_counter(i) > (n))
#define c_flt_skipwhile(i, pred) ((i).b.s2[(i).b.s2top++] |= !(pred))
#define c_flt_take(i, n) _flt_take(&(i).b, n)
#define c_flt_takewhile(i, pred) _flt_takewhile(&(i).b, pred)
#define c_flt_counter(i) ++(i).b.s1[(i).b.s1top++]
#define c_flt_getcount(i) (i).b.s1[(i).b.s1top - 1]

#define c_forfilter(i, C, cnt, filter) \
    c_forfilter_it(i, C, C##_begin(&cnt), filter)

#define c_forfilter_it(i, C, start, filter) \
    for (struct {struct _flt_base b; C##_iter it; C##_value *ref;} \
         i = {.it=start, .ref=i.it.ref} ; !i.b.done & (i.it.ref != NULL) ; \
         C##_next(&i.it), i.ref = i.it.ref, i.b.s1top=0, i.b.s2top=0) \
      if (!(filter)) ; else


// c_find_if, c_erase_if, c_eraseremove_if:

#define c_find_if(...) c_MACRO_OVERLOAD(c_find_if, __VA_ARGS__)
#define c_find_if_4(it, C, cnt, pred) do { \
    intptr_t _index = 0; \
    for (it = C##_begin(&cnt); it.ref && !(pred); C##_next(&it)) \
        ++_index; \
} while (0)

#define c_find_if_5(it, C, start, end, pred) do { \
    intptr_t _index = 0; \
    const C##_value* _endref = (end).ref; \
    for (it = start; it.ref != _endref && !(pred); C##_next(&it)) \
        ++_index; \
    if (it.ref == _endref) it.ref = NULL; \
} while (0)


// Use with: clist, cmap, cset, csmap, csset:
#define c_erase_if(it, C, cnt, pred) do { \
    C* _cnt = &cnt; \
    intptr_t _index = 0; \
    for (C##_iter it = C##_begin(_cnt); it.ref; ++_index) { \
        if (pred) it = C##_erase_at(_cnt, it); \
        else C##_next(&it); \
    } \
} while (0)


// Use with: cstack, cvec, cdeq, cqueue:
#define c_eraseremove_if(it, C, cnt, pred) do { \
    C* _cnt = &cnt; \
    intptr_t _n = 0, _index = 0; \
    C##_iter it = C##_begin(_cnt), _i; \
    while (it.ref && !(pred)) \
        C##_next(&it), ++_index; \
    for (_i = it; it.ref; C##_next(&it), ++_index) \
        if (pred) C##_value_drop(it.ref), ++_n; \
        else *_i.ref = *it.ref, C##_next(&_i); \
    _cnt->_len -= _n; \
} while (0)

// ------------------------ private -------------------------
#ifndef c_NFILTERS
#define c_NFILTERS 32
#endif

struct _flt_base {
    uint32_t s1[c_NFILTERS];
    bool s2[c_NFILTERS], done;
    uint8_t s1top, s2top;
};

static inline bool _flt_take(struct _flt_base* b, uint32_t n) {
    uint32_t k = ++b->s1[b->s1top++];
    b->done |= (k >= n);
    return k <= n;
}

static inline bool _flt_takewhile(struct _flt_base* b, bool pred) {
    bool skip = (b->s2[b->s2top++] |= !pred);
    b->done |= skip;
    return !skip;
}

#endif
// ### END_FILE_INCLUDE: filter.h
// ### BEGIN_FILE_INCLUDE: coroutine.h
#ifndef STC_COROUTINE_INCLUDED
#define STC_COROUTINE_INCLUDED

enum {
    cco_state_final = -1,
    cco_state_done = -2,
};

#define cco_suspended(ctx) ((ctx)->cco_state > 0)
#define cco_alive(ctx) ((ctx)->cco_state != cco_state_done)

#define cco_begin(ctx) \
    int *_state = &(ctx)->cco_state; \
    switch (*_state) { \
        case 0:

#define cco_end(retval) \
        *_state = cco_state_done; break; \
        case -99: goto _cco_final_; \
    } \
    return retval

#define cco_yield(...) c_MACRO_OVERLOAD(cco_yield, __VA_ARGS__)
#define cco_yield_1(retval) \
    do { \
        *_state = __LINE__; return retval; \
        case __LINE__:; \
    } while (0)

#define cco_yield_2(corocall2, ctx2) \
    cco_yield_3(corocall2, ctx2, )

#define cco_yield_3(corocall2, ctx2, retval) \
    do { \
        *_state = __LINE__; \
        do { \
            corocall2; if (cco_suspended(ctx2)) return retval; \
            case __LINE__:; \
        } while (cco_alive(ctx2)); \
    } while (0)

#define cco_final \
    case cco_state_final: \
    _cco_final_

#define cco_return \
    goto _cco_final_

#define cco_stop(ctx) \
    do { \
        int* _state = &(ctx)->cco_state; \
        if (*_state > 0) *_state = cco_state_final; \
    } while (0)

#define cco_reset(ctx) \
    do { \
        int* _state = &(ctx)->cco_state; \
        if (*_state == cco_state_done) *_state = 0; \
    } while (0)

#endif
// ### END_FILE_INCLUDE: coroutine.h

#endif
// ### END_FILE_INCLUDE: calgo.h


// ### BEGIN_FILE_INCLUDE: coroutine.h
#ifndef STC_COROUTINE_H_INCLUDED
#define STC_COROUTINE_H_INCLUDED
#include <stdlib.h>
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

enum {
    cco_STATE_INIT = 0,
    cco_STATE_DONE = -1,
    cco_STATE_DROP = -2,
};
enum cco_status {
    cco_DONE    = 0,
    cco_YIELD   = 1<<12,
    cco_SUSPEND = 1<<13,
    cco_AWAIT   = 1<<14,
};
#define cco_CANCEL (1U<<30)
#define cco_CHILD_ERROR ((1U<<30) - 1)

enum cco_deprecated {
    CCO_STATE_INIT = cco_STATE_INIT, // [deprecated]
    CCO_STATE_DONE = cco_STATE_DONE, // [deprecated]
    CCO_STATE_DROP = cco_STATE_DROP, // [deprecated]
    CCO_DONE       = cco_DONE,       // [deprecated]
    CCO_YIELD      = cco_YIELD,      // [deprecated]
    CCO_SUSPEND    = cco_SUSPEND,    // [deprecated]
    CCO_AWAIT      = cco_AWAIT,      // [deprecated]      
};      
#define CCO_CANCEL cco_CANCEL                        // [deprecated]
#define cco_cancel_group cco_cancel_all              // [deprecated]      
#define cco_await_all_of cco_await_all               // [deprecated]
#define cco_await_cancel_group cco_await_cancel_all  // [deprecated]
#define cco_await_cancel_all_of cco_await_cancel_all // [deprecated]

typedef struct {
    int32_t await_count;
    int32_t spawn_count:24;
    uint8_t child_errors:8;
} cco_group;

#define cco_state_struct(Prefix) \
    struct Prefix##_state { \
        int32_t pos:24; \
        bool drop; \
        struct Prefix##_fiber* fib; \
        cco_group* group; \
    }

#define cco_is_initial(co) ((co)->base.state.pos == cco_STATE_INIT)
#define cco_is_done(co) ((co)->base.state.pos == cco_STATE_DONE)
#define cco_is_active(co) ((co)->base.state.pos != cco_STATE_DONE)

#if defined STC_HAS_TYPEOF && STC_HAS_TYPEOF
    #define _cco_state(co) __typeof__((co)->base.state)
    #define _cco_validate_task_struct(co) \
        c_static_assert(/* error: co->base not first member in task struct */ \
                        sizeof((co)->base) == sizeof(cco_base) || \
                        offsetof(__typeof__(*(co)), base) == 0)
#else
    #define _cco_state(co) cco_state
    #define _cco_validate_task_struct(co) (void)0
#endif

#define cco_async(co) \
    if (0) goto _resume_lbl; \
    else for (_cco_state(co)* _cco_st = (_cco_validate_task_struct(co), (_cco_state(co)*) &(co)->base.state) \
              ; _cco_st->pos != cco_STATE_DONE \
              ; _cco_st->pos = cco_STATE_DONE, \
                (void)(sizeof((co)->base) > sizeof(cco_base) && (_cco_st->group? --_cco_st->group->spawn_count : 0))) \
        _resume_lbl: switch (_cco_st->pos) case cco_STATE_INIT: // thanks, @liigo!

#define cco_finalize /* label */ \
    _cco_st->drop = true; /* FALLTHRU */ \
    case cco_STATE_DROP

#define cco_drop [fix: use cco_finalize:]
#define cco_cleanup [fix: use cco_finalize:]
#define cco_routine [fix: use cco_async]

#define cco_stop(co) \
    do { \
        cco_state* _st = (cco_state*)&(co)->base.state; \
        if (!_st->drop) { _st->pos = cco_STATE_DROP; _st->drop = true; } \
    } while (0)

#define cco_reset_state(co) \
    do { \
        cco_state* _st = (cco_state*)&(co)->base.state; \
        _st->pos = cco_STATE_INIT, _st->drop = false; \
    } while (0)

#define cco_return \
    do { \
        _cco_st->pos = (_cco_st->drop ? cco_STATE_DONE : cco_STATE_DROP); \
        _cco_st->drop = true; \
        goto _resume_lbl; \
    } while (0)

#define cco_exit() \
    do { \
        _cco_st->pos = cco_STATE_DONE; \
        goto _resume_lbl; \
    } while (0)

#define cco_yield_v(status) cco_yield_v_OFFSET(status, 0)
#define cco_yield_v_OFFSET(status, N) \
    do { \
        _cco_st->pos = __LINE__ + N; return status; \
        case __LINE__ + N:; \
    } while (0)

#define cco_yield \
    cco_yield_v(cco_YIELD)

#define cco_suspend \
    cco_yield_v(cco_SUSPEND)

#define cco_await(until) cco_await_OFFSET(until, 0)
#define cco_await_OFFSET(until, N) \
    do { \
        _cco_st->pos = __LINE__ + N; /* FALLTHRU */ \
        case __LINE__ + N: if (!(until)) return cco_AWAIT; \
    } while (0)
    
/* cco_await_coroutine(): assumes coroutine returns a status value (int) */
#define cco_await_coroutine(...) c_MACRO_OVERLOAD(cco_await_coroutine, __VA_ARGS__)
#define cco_await_coroutine_1(corocall) cco_await_coroutine_2(corocall, cco_DONE)
#define cco_await_coroutine_2(corocall, awaitbits) \
    do { \
        _cco_st->pos = __LINE__; /* FALLTHRU */ \
        case __LINE__: { \
            int _res = corocall; \
            if (_res & ~(awaitbits)) return _res; \
        } \
    } while (0)

/* cco_run_coroutine(): assumes coroutine returns a status value (int) */
#define cco_run_coroutine(corocall) \
    while ((1 ? (corocall) : -1) != cco_DONE)


struct cco_error {
    int32_t code, line;
    const char* file;
    intptr_t info;
};

#define cco_fiber_struct(Prefix, EnvPtr) \
    typedef int Prefix##_env_ptr_check[sizeof((EnvPtr)0 == (void*)0)]; \
    typedef EnvPtr Prefix##_env_ptr; \
    struct Prefix##_fiber { \
        struct cco_task* task; \
        Prefix##_env_ptr env; \
        cco_group* tmp_group; \
        struct cco_task* parent_task; \
        struct cco_task_fiber *next, *parent; \
        struct cco_task_state recover_state; \
        struct cco_error error; \
        int awaitbits, status; \
        cco_base base; /* is a coroutine object itself (but not a task) */ \
    }

/* Define a Task struct */
#define cco_task_struct(...) c_MACRO_OVERLOAD(cco_task_struct, __VA_ARGS__)
#define cco_task_struct_1(Task) \
    cco_task_struct_2(Task, void*)

#define cco_task_struct_2(Task, EnvPtr) \
    cco_fiber_struct(Task, EnvPtr); \
    cco_state_struct(Task); \
    _cco_task_struct(Task)

#define _cco_task_struct(Task) \
    struct Task; \
    typedef struct { \
        int (*func)(struct Task*); \
        int awaitbits; \
        struct Task##_state state; \
        struct cco_task* parent_task; \
        cco_group tsk_group; \
    } Task##_base; \
    struct Task

/* Base cco_state type */
typedef cco_state_struct(cco_task) cco_state;
typedef struct { cco_state state; } cco_base;

/* Base cco_task type */
cco_fiber_struct(cco_task, void*);
_cco_task_struct(cco_task) { cco_task_base base; };

typedef struct cco_task_fiber cco_fiber;
typedef struct cco_task cco_task;

#define _cco_taskbase() \
    c_container_of((cco_state*)_cco_st, cco_task_base, state)

#define cco_status() (_cco_st->fib->status + 0)
#define cco_error() (&_cco_st->fib->error)
#define cco_wg() (&_cco_taskbase()->tsk_group)
#define cco_spawner_wg() (_cco_st->group + 0)
#define cco_spawner_fiber() (_cco_taskbase()->state.fib->parent + 0)
#define cco_task_fiber() ((cco_fiber*)_cco_taskbase()->state.fib + 0)

#define cco_env(a_task) ((a_task)->base.state.fib->env)
#define cco_set_env(a_task, the_env) ((a_task)->base.state.fib->env = the_env)

#define cco_promote_child_errors(flag, wg) \
    (void)((wg)->child_errors = (flag))

#define cco_cast_task(...) \
    ((void)sizeof((__VA_ARGS__)->base.func(__VA_ARGS__)), (cco_task *)(__VA_ARGS__))

#define cco_cancel_fiber(a_fiber) \
    do { \
        cco_fiber* _fb1 = a_fiber; \
        _fb1->error.code = cco_CANCEL; \
        _fb1->error.line = __LINE__; \
        _fb1->error.file = __FILE__; \
        cco_stop(_fb1->task); \
    } while (0)

/* Cancel job/task and unwind await stack; MAY be stopped (recovered) in cco_finalize section */
#define cco_cancel_task(a_task) \
    do { \
        cco_task* _tsk1 = cco_cast_task(a_task); \
        cco_cancel_fiber(_tsk1->base.state.fib); \
        cco_stop(_tsk1); \
        /*if (_tsk1 == _cco_st->fib->task) goto _resume_lbl;*/ \
    } while (0)

#define cco_cancel_all(a_group) \
    _cco_cancel_all((cco_fiber*)_cco_st->fib, a_group)


/* Return with error and unwind await stack; must be recovered in cco_finalize section */
#define cco_throw(...) c_MACRO_OVERLOAD(cco_throw, __VA_ARGS__)
#define cco_throw_1(error_code) cco_throw_2(error_code, 0)
#define cco_throw_2(error_code, info_data) \
    do { \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        _fib->error = (struct cco_error){error_code, __LINE__, __FILE__, info_data}; \
        if (_cco_st->group && _cco_st->group->child_errors && _fib->parent) { \
            _fib->parent->error = _fib->error; \
            _fib->parent->error.code = cco_CHILD_ERROR; \
        } \
        cco_return; \
    } while (0)

/* Recover the thrown error; to be used in cco_finalize section upon handling cco_error()->code */
#define cco_recover \
    do { \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        c_assert(_fib->error.code); \
        _fib->task->base.state = _fib->recover_state; \
        _fib->error.code = 0; \
        goto _resume_lbl; \
    } while (0)

/* Asymmetric coroutine await/call */
#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_1(a_task) cco_await_task_2(a_task, cco_DONE)
#define cco_await_task_2(a_task, _awaitbits) do { \
    {   (void)sizeof(cco_env(a_task) == _cco_st->fib->env); \
        cco_task* _wt_task = cco_cast_task(a_task); \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        _wt_task->base.awaitbits = (_awaitbits); \
        _wt_task->base.parent_task = _fib->task; \
        _wt_task->base.state.fib = _fib; \
        _wt_task->base.state.group = &_cco_taskbase()->tsk_group; \
        _cco_taskbase()->tsk_group.spawn_count = 1; \
        _fib->task = _wt_task; \
    } \
    cco_suspend; \
} while (0)

/* Symmetric coroutine flow of control transfer */
#define cco_yield_to(a_task) do { \
    {   (void)sizeof(cco_env(a_task) == _cco_st->fib->env); \
        cco_task* _to_task = cco_cast_task(a_task); \
        cco_fiber* _fib = (cco_fiber*)_cco_st->fib; \
        _to_task->base.awaitbits = _fib->task->base.awaitbits; \
        _to_task->base.parent_task = NULL; \
        _to_task->base.state.fib = _fib; \
        _to_task->base.state.group = &_cco_taskbase()->tsk_group; \
        _cco_taskbase()->tsk_group.spawn_count = 1; \
        _fib->task = _to_task; \
    } \
    cco_suspend; \
} while (0)

#define cco_resume(a_task) \
    _cco_resume_task(cco_cast_task(a_task))

static inline int _cco_resume_task(cco_task* task)
    { return task->base.func(task); }

#define cco_new_fiber(...) c_MACRO_OVERLOAD(cco_new_fiber, __VA_ARGS__)
#define cco_new_fiber_1(a_task) \
    _cco_new_fiber(cco_cast_task(a_task), NULL)
#define cco_new_fiber_2(a_task, env) \
    _cco_new_fiber(cco_cast_task(a_task), ((void)sizeof((env) == cco_env(a_task)), env))

#define cco_reset_group(a_group) ((a_group)->spawn_count = 0)
#define cco_spawn(...) c_MACRO_OVERLOAD(cco_spawn, __VA_ARGS__)
#define cco_spawn_2(a_task, a_group) cco_spawn_4(a_task, a_group, NULL, _cco_st->fib)
#define cco_spawn_3(a_task, a_group, env) cco_spawn_4(a_task, a_group, env, _cco_st->fib)
#define cco_spawn_4(a_task, a_group, env, fiber) \
    _cco_spawn(cco_cast_task(a_task), a_group, ((void)sizeof((env) == cco_env(a_task)), env), \
               (cco_fiber*)((void)sizeof((fiber)->parent_task), fiber))


#define cco_await_cancel_task(a_task) do { \
    cco_cancel_task(a_task); \
    cco_await_task(a_task); \
} while (0)

#define cco_await_n(n, a_group) do { /* does not cancel remaining */ \
    _cco_st->fib->tmp_group = a_group; \
    _cco_st->fib->tmp_group->await_count = _cco_st->fib->tmp_group->spawn_count - (n); \
    cco_await(_cco_st->fib->tmp_group->spawn_count == _cco_st->fib->tmp_group->await_count); \
} while (0)

#define cco_await_all(a_group) do { \
    _cco_st->fib->tmp_group = a_group; \
    cco_await(_cco_st->fib->tmp_group->spawn_count == 0); \
} while (0)

#define cco_await_any(a_group) do { /* await 1; cancel remaining */ \
    cco_await_n_2(1, a_group); \
    cco_cancel_all(_cco_st->fib->tmp_group); \
    cco_await_OFFSET(_cco_st->fib->tmp_group->spawn_count == 0, 100000); /* await_all() */ \
} while (0)

#define cco_await_cancel_all(a_group) do { \
    cco_group* _grp = a_group; \
    cco_cancel_all(_grp); \
    cco_await_all(_grp); \
} while (0)


#define cco_run_fiber(...) c_MACRO_OVERLOAD(cco_run_fiber, __VA_ARGS__)
#define cco_run_fiber_1(fiber_ref) \
    for (cco_fiber** _it_ref = (cco_fiber**)((void)sizeof((fiber_ref)[0]->env), fiber_ref) \
        ; (*_it_ref = cco_execute_next(*_it_ref)) != NULL; )
#define cco_run_fiber_2(it, fiber) \
    for (cco_fiber* it = (cco_fiber*)((void)sizeof((fiber)->env), fiber) \
        ; (it = cco_execute_next(it)) != NULL; )

#define cco_run_task(...) c_MACRO_OVERLOAD(cco_run_task, __VA_ARGS__)
#define cco_run_task_1(a_task) cco_run_fiber_2(_fibit, cco_new_fiber_1(a_task))
#define cco_run_task_2(a_task, env) cco_run_fiber_2(_fibit, cco_new_fiber_2(a_task, env))
#define cco_run_task_3(it, a_task, env) cco_run_fiber_2(it, cco_new_fiber_2(a_task, env))

extern int        cco_execute(cco_fiber* fib); // is a coroutine itself
extern cco_fiber* cco_execute_next(cco_fiber* fib);  // resume the next fiber and return it

extern cco_fiber* _cco_new_fiber(cco_task* task, void* env);
extern cco_fiber* _cco_spawn(cco_task* task, cco_group* wg, void* env, cco_fiber* fib);
extern void       _cco_cancel_all(cco_fiber* fib, cco_group* wg);

#define cco_each(existing_it, C, cnt) \
    existing_it = C##_begin(&cnt); (existing_it).ref; C##_next(&existing_it)

#define cco_each_reverse(existing_it, C, cnt) \
    existing_it = C##_rbegin(&cnt); (existing_it).ref; C##_rnext(&existing_it)

#define cco_flt_take(n) \
    (c_flt_take(n), fltbase.done ? (_it.base.state.pos = cco_STATE_DROP, _it.base.state.drop = 1) : 1)

#define cco_flt_takewhile(pred) \
    (c_flt_takewhile(pred), fltbase.done ? (_it.base.state.pos = cco_STATE_DROP, _it.base.state.drop = 1) : 1)



typedef struct { ptrdiff_t acq_count; } cco_semaphore;

#define cco_make_semaphore(value) (c_literal(cco_semaphore){value})
#define cco_set_semaphore(sem, value) ((sem)->acq_count = value)
#define cco_acquire_semaphore(sem) (--(sem)->acq_count)
#define cco_release_semaphore(sem) (++(sem)->acq_count)

#define cco_await_semaphore(sem) \
    do { \
        cco_await((sem)->acq_count > 0); \
        cco_acquire_semaphore(sem); \
    } while (0)



#ifdef _WIN32
    #ifdef __cplusplus
      #define _c_LINKC extern "C" __declspec(dllimport)
    #else
      #define _c_LINKC __declspec(dllimport)
    #endif
    #ifndef _WINDOWS_ // windows.h
      typedef long long LARGE_INTEGER;
      _c_LINKC int __stdcall QueryPerformanceCounter(LARGE_INTEGER*);
      //_c_LINKC int __stdcall QueryPerformanceFrequency(LARGE_INTEGER*);
    #endif
    #define cco_timer_freq() 10000000LL /* 1/10th microseconds */
    //static inline long long cco_timer_freq(void) {
    //    long long quad;
    //    QueryPerformanceFrequency((LARGE_INTEGER*)&quad);
    //    return quad;
    //}

    static inline long long cco_timer_ticks(void) {
        long long quad;
        QueryPerformanceCounter((LARGE_INTEGER*)&quad);
        return quad;
    }
#else
    #include <sys/time.h>
    #define cco_timer_freq() 1000000LL

    static inline long long cco_timer_ticks(void) { /* microseconds */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec*cco_timer_freq() + tv.tv_usec;
    }
#endif

typedef struct { double duration; long long start_time; } cco_timer;

static inline cco_timer cco_make_timer(double sec) {
    cco_timer tm = {.duration=sec, .start_time=cco_timer_ticks()};
    return tm;
}

static inline void cco_start_timer(cco_timer* tm, double sec) {
    tm->duration = sec;
    tm->start_time = cco_timer_ticks();
}

static inline void cco_restart_timer(cco_timer* tm) {
    tm->start_time = cco_timer_ticks();
}

static inline double cco_timer_elapsed(cco_timer* tm) {
    return (double)(cco_timer_ticks() - tm->start_time)*(1.0/cco_timer_freq());
}

static inline bool cco_timer_expired(cco_timer* tm) {
    return cco_timer_elapsed(tm) >= tm->duration;
}

static inline double cco_timer_remaining(cco_timer* tm) {
    return tm->duration - cco_timer_elapsed(tm);
}

#define cco_await_timer(tm, sec) \
    do { \
        cco_start_timer(tm, sec); \
        cco_await(cco_timer_expired(tm)); \
    } while (0)

#endif // STC_COROUTINE_H_INCLUDED

/* -------------------------- IMPLEMENTATION ------------------------- */
#if (defined i_implement || defined STC_IMPLEMENT) && !defined STC_COROUTINE_IMPLEMENT
#define STC_COROUTINE_IMPLEMENT
#include <stdio.h>

cco_fiber* _cco_spawn(cco_task* task, cco_group* wg, void* env, cco_fiber* fib) {
    cco_fiber* new_fb = fib->next ? c_new(cco_fiber, {.next = fib->next, .parent = fib}) : fib;
    new_fb->task = task;
    new_fb->env = (env ? env : fib->env);
    task->base.state.fib = fib->next = new_fb;
    task->base.state.group = wg;
    if (wg) wg->spawn_count += 1;
    return new_fb;
}

cco_fiber* _cco_new_fiber(cco_task* task, void* env) {
    cco_fiber* new_fb = c_new(cco_fiber, {.task=task, .env=env});
    task->base.state.fib = new_fb->next = new_fb;
    return new_fb;
}

void _cco_cancel_all(cco_fiber* fib, cco_group* wg) {
    for (cco_fiber *fit = fib->next; fit != fib; fit = fit->next) {
        cco_task* cur = fit->task;
        do {
            if (cur->base.state.group == wg) {   
                fit->error.code = cco_CANCEL;
                break;
            }
            cur = cur->base.parent_task;
        } while (cur);
    }
}

cco_fiber* cco_execute_next(cco_fiber* fib) {
    cco_fiber *_next = fib->next, *unlinked;
    int ret = cco_execute(_next);

    if (ret == cco_DONE) {
        unlinked = _next;
        _next = (_next == fib ? NULL : _next->next);
        fib->next = _next;
        c_free_n(unlinked, 1);
    }
    return _next;
}

int cco_execute(cco_fiber* fib) {
    cco_async (fib) {
        while (1) {
            fib->parent_task = fib->task->base.parent_task;
            fib->awaitbits = fib->task->base.awaitbits;
            fib->status = cco_resume(fib->task);
            if (fib->error.code) {
                // Note: if fib->status == cco_DONE, fib->task may already be destructed.
                if (fib->status == cco_DONE) {
                    fib->task = fib->parent_task;
                    if (fib->task == NULL)
                        break;
                    fib->recover_state = fib->task->base.state;
                }
                cco_stop(fib->task);
                cco_suspend;
                continue;
            }
            if (!((fib->status & ~fib->awaitbits) || (fib->task = fib->parent_task) != NULL))
                break;
            cco_suspend;
        }
    }

    if ((uint32_t)fib->error.code & ~cco_CANCEL) { // Allow cco_CANCEL not to trigger error.
        fprintf(stderr, __FILE__ ": error: unhandled coroutine error '%d'\n"
                        "%s:%d: cco_throw(%d);\n",
                        fib->error.code, fib->error.file, fib->error.line, fib->error.code);
        exit(fib->error.code);
    }
    return cco_DONE;
}
#endif // IMPLEMENT
#undef i_implement
#undef i_static
#undef i_header
// ### END_FILE_INCLUDE: coroutine.h


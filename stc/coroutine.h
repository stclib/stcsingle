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

enum {
    CCO_STATE_INIT = 0,
    CCO_STATE_DONE = -1,
    CCO_STATE_DROP = -2,
};
typedef enum {
    CCO_DONE = 0,
    CCO_YIELD = 1<<12,
    CCO_AWAIT = 1<<13,
    CCO_NOOP = 1<<14,
} cco_result;
#define CCO_CANCEL (1U<<30)

typedef struct {
    int launch_count;
} cco_group; // waitgroup

typedef struct {
    struct cco_state {
        int32_t pos:24;
        bool drop;
        struct cco_fiber* fb;
        cco_group* wg;
    } state;
} cco_base;

#define cco_is_initial(co) ((co)->base.state.pos == CCO_STATE_INIT)
#define cco_is_done(co) ((co)->base.state.pos == CCO_STATE_DONE)
#define cco_is_active(co) ((co)->base.state.pos != CCO_STATE_DONE)

#if defined STC_HAS_TYPEOF && STC_HAS_TYPEOF
    #define _cco_validate_task_struct(co) \
        c_static_assert(/* error: co->base not first member in task struct */ \
                        sizeof((co)->base) == sizeof(cco_base) || \
                        offsetof(__typeof__(*(co)), base) == 0)
#else
    #define _cco_validate_task_struct(co) (void)0
#endif

#define cco_async(co) \
    if (0) goto _resume; \
    else for (struct cco_state *_state = (_cco_validate_task_struct(co), &(co)->base.state) \
              ; _state->pos != CCO_STATE_DONE \
              ; _state->pos = CCO_STATE_DONE, _state->wg ? --_state->wg->launch_count : 0) \
        _resume: switch (_state->pos) case CCO_STATE_INIT: // thanks, @liigo!

#define cco_drop /* label */ \
    _state->drop = true; /* FALLTHRU */ \
    case CCO_STATE_DROP
#define cco_cleanup [fix: use cco_drop:]
#define cco_finally [fix: use cco_drop:]
#define cco_routine [fix: use cco_async]

#define cco_stop(co) \
    do { \
        struct cco_state* _s = &(co)->base.state; \
        if (!_s->drop) { _s->pos = CCO_STATE_DROP; _s->drop = true; } \
    } while (0)

#define cco_reset_state(co) \
    do { \
        struct cco_state* _s = &(co)->base.state; \
        _s->pos = CCO_STATE_INIT, _s->drop = false; \
    } while (0)

#define cco_return \
    do { \
        _state->pos = (_state->drop ? CCO_STATE_DONE : CCO_STATE_DROP); \
        _state->drop = true; \
        goto _resume; \
    } while (0)

#define cco_exit() \
    do { \
        _state->pos = CCO_STATE_DONE; \
        goto _resume; \
    } while (0)

#define cco_yield \
    cco_yield_v(CCO_YIELD)

#define cco_yield_v(value) \
    do { \
        _state->pos = __LINE__; return value; \
        case __LINE__:; \
    } while (0)

#define cco_await(until) \
    do { \
        _state->pos = __LINE__; /* FALLTHRU */ \
        case __LINE__: if (!(until)) return CCO_AWAIT; \
    } while (0)

/* cco_await_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_await_coroutine(...) c_MACRO_OVERLOAD(cco_await_coroutine, __VA_ARGS__)
#define cco_await_coroutine_1(corocall) cco_await_coroutine_2(corocall, CCO_DONE)
#define cco_await_coroutine_2(corocall, awaitbits) \
    do { \
        _state->pos = __LINE__; /* FALLTHRU */ \
        case __LINE__: { \
            int _res = corocall; \
            if (_res & ~(awaitbits)) return _res; \
        } \
    } while (0)

/* cco_run_coroutine(): assumes coroutine returns a cco_result value (int) */
#define cco_run_coroutine(corocall) \
    while ((1 ? (corocall) : -1) != CCO_DONE)


typedef struct {
    int32_t code, line;
    const char* file;
} cco_error;

typedef struct cco_fiber {
    struct cco_task* task;
    void* env;
    struct cco_task* parent_task;
    struct cco_fiber* next;
    struct cco_state recover_state;
    cco_error err;
    int awaitbits, status;
    cco_base base; /* is a coroutine object itself */
} cco_fiber;

/* Define a Task struct */
#define cco_task_struct(Task) \
    struct Task; \
    typedef struct { \
        int (*func)(struct Task*); \
        int awaitbits; \
        struct cco_state state; \
        struct cco_task* parent_task; \
    } Task##_base; \
    struct Task

/* Base cco_task type */
cco_task_struct(cco_task) { cco_task_base base; };
typedef struct cco_task cco_task;

#define cco_env(Tp) ((Tp)_state->fb->env)
#define cco_err() (*(const cco_error*)&_state->fb->err)
#define cco_status() (_state->fb->status + 0)


#define cco_cast_task(...) \
    ((void)sizeof((__VA_ARGS__)->base.func(__VA_ARGS__)), (cco_task *)(__VA_ARGS__))

/* Throw an "exception"; can be catched up in the cco_await_task call tree */
#define cco_task_throw(error_code) \
    do { \
        cco_fiber* _fb = _state->fb; \
        _fb->err.code = error_code; \
        _fb->err.line = __LINE__; \
        _fb->err.file = __FILE__; \
        cco_return; \
    } while (0)

#define cco_cancel_task(a_task) \
    do { \
        cco_fiber* _fb = cco_cast_task(a_task)->base.state.fb; \
        _fb->err.code = CCO_CANCEL; \
        _fb->err.line = __LINE__; \
        _fb->err.file = __FILE__; \
        cco_stop(_fb->task); \
        if (_fb->task == _state->fb->task) goto _resume; \
    } while (0)

#define cco_recover_task() \
    do { \
        cco_fiber* _fb = _state->fb; \
        c_assert(_fb->err.code); \
        _fb->task->base.state = _fb->recover_state; \
        _fb->err.code = 0; \
        goto _resume; \
    } while (0)

/* Asymmetric coroutine await/call */
#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_1(a_task) cco_await_task_2(a_task, CCO_DONE)
#define cco_await_task_2(a_task, _awaitbits) do { \
    {   cco_task* _await_task = cco_cast_task(a_task); \
        cco_fiber* _fb = _state->fb; \
        _await_task->base.awaitbits = (_awaitbits); \
        _await_task->base.parent_task = _fb->task; \
        _fb->task = _await_task; \
        _await_task->base.state.fb = _fb; \
    } \
    cco_yield_v(CCO_NOOP); \
} while (0)

/* Symmetric coroutine flow of control transfer */
#define cco_yield_to(a_task) do { \
    {   cco_task* _to_task = cco_cast_task(a_task); \
        cco_fiber* _fb = _state->fb; \
        _to_task->base.awaitbits = _fb->task->base.awaitbits; \
        _to_task->base.parent_task = NULL; \
        _fb->task = _to_task; \
        _to_task->base.state.fb = _fb; \
    } \
    cco_yield_v(CCO_NOOP); \
} while (0)

#define cco_resume_task(a_task) \
    _cco_resume_task(cco_cast_task(a_task))

static inline int _cco_resume_task(cco_task* task)
    { return task->base.func(task); }

#define cco_new_fiber(...) c_MACRO_OVERLOAD(cco_new_fiber, __VA_ARGS__)
#define cco_new_fiber_1(task) cco_new_fiber_2(task, NULL)
#define cco_new_fiber_2(task, env) _cco_new_fiber(cco_cast_task(task), env, NULL)

#define cco_spawn(...) c_MACRO_OVERLOAD(cco_spawn, __VA_ARGS__)
#define cco_spawn_1(task) _cco_spawn(cco_cast_task(task), NULL, _state->fb, NULL)
#define cco_spawn_2(task, env) _cco_spawn(cco_cast_task(task), env, _state->fb, NULL)
#define cco_spawn_3(task, env, fiber) _cco_spawn(cco_cast_task(task), env, fiber, NULL)

#define cco_reset_group(waitgroup) ((waitgroup)->launch_count = 0)
#define cco_launch(...) c_MACRO_OVERLOAD(cco_launch, __VA_ARGS__)
#define cco_launch_2(task, waitgroup) cco_launch_3(task, waitgroup, NULL)
#define cco_launch_3(task, waitgroup, env) do { \
    cco_group* _wg = waitgroup; _wg->launch_count += 1; \
    _cco_spawn(cco_cast_task(task), env, _state->fb, _wg); \
} while (0)
#define cco_await_group(waitgroup) cco_await((waitgroup)->launch_count == 0)

#define cco_run_fiber(...) c_MACRO_OVERLOAD(cco_run_fiber, __VA_ARGS__)
#define cco_run_fiber_1(fiber_ref) \
    for (cco_fiber** _it_ref = fiber_ref; (*_it_ref = cco_resume_next(*_it_ref)) != NULL; )
#define cco_run_fiber_2(it, fiber) \
    for (cco_fiber* it = fiber; (it = cco_resume_next(it)) != NULL; )

#define cco_run_task(...) c_MACRO_OVERLOAD(cco_run_task, __VA_ARGS__)
#define cco_run_task_1(task) cco_run_task_2(task, NULL)
#define cco_run_task_2(task, env) cco_run_fiber_2(_it_fb, cco_new_fiber_2(task, env))
#define cco_run_task_3(it, task, env) cco_run_fiber_2(it, cco_new_fiber_2(task, env))

#define cco_joined() \
    (_state->fb == _state->fb->next)

extern cco_fiber* _cco_new_fiber(cco_task* task, void* env, cco_group* wg);
extern cco_fiber* _cco_spawn(cco_task* task, void* env, cco_fiber* fb, cco_group* wg);
extern cco_fiber* cco_resume_next(cco_fiber* prev);
extern int        cco_resume_current(cco_fiber* co); /* coroutine */

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined i_implement || defined STC_IMPLEMENT
#include <stdio.h>

int cco_resume_current(cco_fiber* fb) {
    cco_async (fb) {
        while (1) {
            fb->parent_task = fb->task->base.parent_task;
            fb->awaitbits = fb->task->base.awaitbits;
            fb->status = fb->task->base.func(fb->task); // resume
            // Note: if fb->status == CCO_DONE, fb->task may already be destructed.
            if (fb->err.code && (fb->status == CCO_DONE || !fb->task->base.state.drop)) {
                fb->task = fb->parent_task;
                if (fb->task == NULL)
                    break;
                fb->recover_state = fb->task->base.state;
                cco_stop(fb->task);
                continue;
            }
            if (!((fb->status & ~fb->awaitbits) || (fb->task = fb->parent_task) != NULL))
                break;
            cco_yield_v(CCO_NOOP);
        }
    }

    if ((uint32_t)fb->err.code & ~CCO_CANCEL) { // Allow CCO_CANCEL not to trigger error.
        fprintf(stderr, __FILE__ ": error: unhandled coroutine exception '%d'\n"
                        "%s:%d: unhandled: cco_task_throw(%d)\n",
                        fb->err.code, fb->err.file, fb->err.line, fb->err.code);
        exit(fb->err.code);
    }
    return CCO_DONE;
}

cco_fiber* cco_resume_next(cco_fiber* prev) {
    cco_fiber *curr = prev->next, *unlink;
    int ret = cco_resume_current(curr);

    if (ret == CCO_DONE) {
        unlink = curr;
        curr = (curr == prev ? NULL : curr->next);
        prev->next = curr;
        c_free_n(unlink, 1);
    }
    return curr;
}

cco_fiber* _cco_new_fiber(cco_task* _task, void* env, cco_group* wg) {
    cco_fiber* new_fb = c_new(cco_fiber, {.task=_task, .env=env});
    _task->base.state.fb = new_fb;
    _task->base.state.wg = wg;
    return (new_fb->next = new_fb);
}

cco_fiber* _cco_spawn(cco_task* _task, void* env, cco_fiber* fb, cco_group* wg) {
    cco_fiber* new_fb;
    new_fb = fb->next = (fb->next == NULL ? fb : c_new(cco_fiber, {.next=fb->next}));
    new_fb->task = _task;
    new_fb->env = (env == NULL ? fb->env : env);
    _task->base.state.fb = new_fb;
    _task->base.state.wg = wg;
    return new_fb;
}

#undef i_implement
#endif

#define cco_each(existing_it, C, cnt) \
    existing_it = C##_begin(&cnt); (existing_it).ref; C##_next(&existing_it)

#define cco_each_reverse(existing_it, C, cnt) \
    existing_it = C##_rbegin(&cnt); (existing_it).ref; C##_rnext(&existing_it)

#define cco_flt_take(n) \
    (c_flt_take(n), fltbase.done ? (_it.base.state.pos = CCO_STATE_DROP, _it.base.state.drop = 1) : 1)

#define cco_flt_takewhile(pred) \
    (c_flt_takewhile(pred), fltbase.done ? (_it.base.state.pos = CCO_STATE_DROP, _it.base.state.drop = 1) : 1)



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
    struct _FILETIME; struct _LARGE_INTEGER;
    _c_LINKC void __stdcall GetSystemTimePreciseAsFileTime(struct _FILETIME*);
    _c_LINKC void __stdcall Sleep(unsigned long);
    _c_LINKC int __stdcall QueryPerformanceCounter(struct _LARGE_INTEGER*);
    #define cco_timer_res 1.0E-7

    static inline double cco_time(void) { /* seconds since epoch */
        unsigned long long quad;
        /* 64-bit value representing 1/10th usecs since Jan 1 1601 */
        GetSystemTimePreciseAsFileTime((struct _FILETIME*)&quad);
        /* subtract time diff to Jan 1 1970 in 1/10th usecs */
        return (double)(quad - 116444736000000000ULL)*cco_timer_res;
    }

    static inline long long cco_ticks(void) { /* 1/10th microseconds */
        long long quad;
        QueryPerformanceCounter((struct _LARGE_INTEGER*)&quad);
        return quad;
    }

    static inline void cco_sleep(double sec) {
        Sleep((unsigned long)(sec*1000.0));
    }
#else
    #include <sys/time.h>
    #define cco_timer_freq 1000000LL
    #define cco_timer_res (1.0/cco_timer_freq)

    static inline double cco_time(void) { /* seconds since epoch */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (double)tv.tv_sec + (double)tv.tv_usec*cco_timer_res;
    }

    static inline long long cco_ticks(void) { /* microseconds */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec*cco_timer_freq + tv.tv_usec;
    }

    static inline void cco_sleep(double sec) {
        struct timeval tv;
        tv.tv_sec = (time_t)sec;
        tv.tv_usec = (suseconds_t)((sec - (double)(long)sec)*cco_timer_freq);
        select(0, NULL, NULL, NULL, &tv);
    }
#endif

typedef struct { double duration; long long start_time; } cco_timer;

static inline cco_timer cco_make_timer(double sec) {
    cco_timer tm = {.duration=sec, .start_time=cco_ticks()};
    return tm;
}

static inline void cco_start_timer(cco_timer* tm, double sec) {
    tm->duration = sec;
    tm->start_time = cco_ticks();
}

static inline void cco_restart_timer(cco_timer* tm) {
    tm->start_time = cco_ticks();
}

static inline double cco_timer_elapsed(cco_timer* tm) {
    return (double)(cco_ticks() - tm->start_time)*cco_timer_res;
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
// ### END_FILE_INCLUDE: coroutine.h


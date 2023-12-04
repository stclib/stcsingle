// ### BEGIN_FILE_INCLUDE: coroutine.h
#ifndef STC_COROUTINE_H_INCLUDED
#define STC_COROUTINE_H_INCLUDED
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

typedef long long _llong;
#define c_NPOS INTPTR_MAX
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR

#if defined __GNUC__ // includes __clang__
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
#define _c_SEL(S, ...) S(__VA_ARGS__)

#ifndef __cplusplus
    #define _i_alloc(T)         ((T*)i_malloc(c_sizeof(T)))
    #define _i_new(T, ...)      ((T*)memcpy(_i_alloc(T), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_new(T, ...)       ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
    #define c_LITERAL(T)        (T)
#else
    #include <new>
    #define _i_alloc(T)         static_cast<T*>(i_malloc(c_sizeof(T)))
    #define _i_new(T, ...)      new (_i_alloc(T)) T(__VA_ARGS__)
    #define c_new(T, ...)       new (malloc(sizeof(T))) T(__VA_ARGS__)
    #define c_LITERAL(T)        T
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
#define c_safe_cast(T, F, x)    ((T)(1 ? (x) : (F){0}))
#define c_swap(T, xp, yp)       do { T *_xp = xp, *_yp = yp, \
                                    _tv = *_xp; *_xp = *_yp; *_yp = _tv; } while (0)
// use with gcc -Wconversion
#define c_sizeof                (intptr_t)sizeof
#define c_strlen(s)             (intptr_t)strlen(s)
#define c_strncmp(a, b, ilen)   strncmp(a, b, c_i2u_size(ilen))
#define c_memcpy(d, s, ilen)    memcpy(d, s, c_i2u_size(ilen))
#define c_memmove(d, s, ilen)   memmove(d, s, c_i2u_size(ilen))
#define c_memset(d, val, ilen)  memset(d, val, c_i2u_size(ilen))
#define c_memcmp(a, b, ilen)    memcmp(a, b, c_i2u_size(ilen))
#define c_u2i_size(u)           (intptr_t)(1 ? (u) : (size_t)1)
#define c_i2u_size(i)           (size_t)(1 ? (i) : -1)
#define c_uless(a, b)           ((size_t)(a) < (size_t)(b))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_hash          stc_hash_1

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

/* Function macros and others */

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (intptr_t)(sizeof(a)/sizeof 0[a])

// Non-owning c-string "class"
typedef const char* ccharptr;
#define ccharptr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define ccharptr_hash(p) stc_strhash(*(p))
#define ccharptr_clone(s) (s)
#define ccharptr_drop(p) ((void)p)

#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

#define stc_hash(...) c_MACRO_OVERLOAD(stc_hash, __VA_ARGS__)
#define stc_hash_1(x) stc_hash_2(x, c_sizeof(*(x)))

STC_INLINE uint64_t stc_hash_2(const void* key, intptr_t len) {
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

STC_INLINE uint64_t stc_strhash(const char *str)
    { return stc_hash_2(str, c_strlen(str)); }

STC_INLINE uint64_t _stc_hash_mix(uint64_t h[], int n) { // n > 0
    for (int i = 1; i < n; ++i) h[0] ^= h[0] + h[i]; // non-commutative!
    return h[0];
}

STC_INLINE char* stc_strnstrn(const char *str, intptr_t slen,
                              const char *needle, intptr_t nlen) {
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

STC_INLINE intptr_t stc_nextpow2(intptr_t n) {
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
#define c_foreach_4(it, C, start, finish) \
    for (C##_iter it = (start), *_endref = c_safe_cast(C##_iter*, C##_value*, (finish).ref) \
         ; it.ref != (C##_value*)_endref; C##_next(&it))

#define c_forpair(key, val, C, cnt) /* structured binding */ \
    for (struct {C##_iter iter; const C##_key* key; C##_mapped* val;} _ = {.iter=C##_begin(&cnt)} \
         ; _.iter.ref && (_.key = &_.iter.ref->first, _.val = &_.iter.ref->second) \
         ; C##_next(&_.iter))

#define c_forindexed(it, C, cnt) \
    for (struct {C##_iter iter; C##_value* ref; intptr_t index;} it = {.iter=C##_begin(&cnt)} \
         ; (it.ref = it.iter.ref) ; C##_next(&it.iter), ++it.index)

#define c_foriter(existing_iter, C, cnt) \
    for (existing_iter = C##_begin(&cnt); (existing_iter).ref; C##_next(&existing_iter))

#define c_forrange(...) c_MACRO_OVERLOAD(c_forrange, __VA_ARGS__)
#define c_forrange_1(stop) c_forrange_3(_i, 0, stop)
#define c_forrange_2(i, stop) c_forrange_3(i, 0, stop)
#define c_forrange_3(i, start, stop) \
    for (_llong i=start, _end=stop; i < _end; ++i)
#define c_forrange_4(i, start, stop, step) \
    for (_llong i=start, _inc=step, _end=(_llong)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
    #define c_init(C, ...) \
        C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))
    #define c_forlist(it, T, ...) \
        for (struct {T* ref; int size, index;} \
             it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define stc_hash_mix(...) \
        _stc_hash_mix((uint64_t[]){__VA_ARGS__}, c_NUMARGS(__VA_ARGS__))
#else
    #include <initializer_list>
    #include <array>
    template <class C, class T>
    inline C _from_n(C (*func)(const T[], intptr_t), std::initializer_list<T> il)
        { return func(&*il.begin(), il.size()); }
    #define c_init(C, ...) _from_n<C,C##_raw>(C##_from_n, __VA_ARGS__)
    #define c_forlist(it, T, ...) \
        for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator ref; size_t size, index;} \
             it = {._il=__VA_ARGS__, .ref=it._il.begin(), .size=it._il.size()} \
             ; it.index < it.size; ++it.ref, ++it.index)
    #define stc_hash_mix(...) \
        _stc_hash_mix(std::array<uint64_t, c_NUMARGS(__VA_ARGS__)>{__VA_ARGS__}.data(), c_NUMARGS(__VA_ARGS__))
#endif

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

#define c_drop(C, ...) \
    do { c_forlist (_i, C*, {__VA_ARGS__}) C##_drop(*_i.ref); } while(0)

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

enum {
    CCO_STATE_FINAL = -1,
    CCO_STATE_DONE = -2,
};
typedef enum {
    CCO_DONE = 0,
    CCO_AWAIT = 1<<0,
    CCO_YIELD = 1<<1,
} cco_result;

#define cco_initial(co) ((co)->cco_state == 0)
#define cco_suspended(co) ((co)->cco_state > 0)
#define cco_done(co) ((co)->cco_state == CCO_STATE_DONE)

#define cco_routine(co) \
    for (int* _state = &(co)->cco_state; *_state != CCO_STATE_DONE; *_state = CCO_STATE_DONE) \
        _resume: switch (*_state) case 0: // thanks, @liigo!

#define cco_yield() cco_yield_v(CCO_YIELD)
#define cco_yield_v(ret) \
    do { \
        *_state = __LINE__; return ret; goto _resume; \
        case __LINE__:; \
    } while (0)

#define cco_await(promise) cco_await_and_return(promise, CCO_AWAIT)
#define cco_await_v(promise) cco_await_and_return(promise, )
#define cco_await_and_return(promise, ret) \
    do { \
        *_state = __LINE__; \
        /* fall through */ \
        case __LINE__: if (!(promise)) {return ret; goto _resume;} \
    } while (0)

/* cco_await_call(): assumes coroutine returns a cco_result value (int) */
#define cco_await_call(...) c_MACRO_OVERLOAD(cco_await_call, __VA_ARGS__)
#define cco_await_call_1(corocall) cco_await_call_2(corocall, CCO_DONE)
#define cco_await_call_2(corocall, awaitbits) \
    do { \
        *_state = __LINE__; \
        /* fall through */ \
        case __LINE__: { int _r = corocall; if (_r & ~(awaitbits)) {return _r; goto _resume;} } \
    } while (0)

/* cco_blocking_call(): assumes coroutine returns a cco_result value (int) */
#define cco_blocking_call(corocall) while ((corocall) != CCO_DONE)

#define cco_cleanup cco_final // [deprecated]
#define cco_final \
    *_state = CCO_STATE_FINAL; \
    /* fall through */ \
    case CCO_STATE_FINAL

#define cco_return \
    do { \
        *_state = *_state >= 0 ? CCO_STATE_FINAL : CCO_STATE_DONE; \
        goto _resume; \
    } while (0)

#define cco_cancel \
    do { *_state = CCO_STATE_DONE; goto _resume; } while (0)

#define cco_yield_final() cco_yield_final_v(CCO_YIELD)
#define cco_yield_final_v(value) \
    do { \
        *_state = *_state >= 0 ? CCO_STATE_FINAL : CCO_STATE_DONE; \
        return value; \
    } while (0)

#define cco_stop(co) \
    do { \
        int* _s = &(co)->cco_state; \
        if (*_s > 0) *_s = CCO_STATE_FINAL; \
        else if (*_s == 0) *_s = CCO_STATE_DONE; \
    } while (0)

#define cco_reset(co) \
    (void)((co)->cco_state = 0)


#define cco_iter_struct(Gen, ...) \
    typedef Gen Gen##_value; \
    typedef struct Gen##_iter { \
        Gen##_value* ref; \
        int cco_state; \
        __VA_ARGS__ \
    } Gen##_iter


struct cco_runtime;

#define cco_task_struct(Task, ...) \
    struct Task { \
        int (*cco_func)(struct Task*, struct cco_runtime*); \
        int cco_state, cco_expect; \
        __VA_ARGS__ \
    }

typedef cco_task_struct(cco_task, /**/) cco_task;

typedef struct cco_runtime {
    int result, top; cco_task* stack[];
} cco_runtime;

#define cco_cast_task(task) \
    ((cco_task *)(task) + 0*sizeof((task)->cco_func(task, (cco_runtime*)0) + ((int*)0 == &(task)->cco_state)))

#define cco_resume_task(task, rt) \
    (task)->cco_func(task, rt)

#define cco_await_task(...) c_MACRO_OVERLOAD(cco_await_task, __VA_ARGS__)
#define cco_await_task_2(task, rt) cco_await_task_3(task, rt, CCO_DONE)
#define cco_await_task_3(task, rt, awaitbits) \
    do { \
        ((rt)->stack[++(rt)->top] = cco_cast_task(task))->cco_expect = (awaitbits); \
        cco_yield_v(CCO_AWAIT); \
    } while (0)

#define cco_blocking_task(...) c_MACRO_OVERLOAD(cco_blocking_task, __VA_ARGS__)
#define cco_blocking_task_1(task) cco_blocking_task_3(task, _rt, 16)
#define cco_blocking_task_3(task, rt, STACKDEPTH) \
    for (struct { int result, top; cco_task* stack[STACKDEPTH]; } rt = {.stack={cco_cast_task(task)}}; \
         (((rt.result = cco_resume_task(rt.stack[rt.top], (cco_runtime*)&rt)) & \
           ~rt.stack[rt.top]->cco_expect) || --rt.top >= 0); )


typedef struct { intptr_t count; } cco_sem;

#define cco_await_sem(sem) cco_await_sem_and_return(sem, CCO_AWAIT)
#define cco_await_sem_v(sem) cco_await_sem_and_return(sem, )
#define cco_await_sem_and_return(sem, ret) \
    do { \
        cco_await_and_return((sem)->count > 0, ret); \
        --(sem)->count; \
    } while (0)

#define cco_sem_release(sem) ++(sem)->count
#define cco_sem_from(value) ((cco_sem){value})
#define cco_sem_set(sem, value) ((sem)->count = value)


#ifdef _WIN32
    #ifdef __cplusplus
      #define _c_LINKC extern "C" __declspec(dllimport)
    #else
      #define _c_LINKC __declspec(dllimport)
    #endif
    #if 1 // _WIN32_WINNT < _WIN32_WINNT_WIN8 || defined __TINYC__
      #define _c_getsystime GetSystemTimeAsFileTime
    #else
      #define _c_getsystime GetSystemTimePreciseAsFileTime
    #endif
    struct _FILETIME;
    _c_LINKC void _c_getsystime(struct _FILETIME*);
    _c_LINKC void Sleep(unsigned long);

    static inline double cco_time(void) { /* seconds since epoch */
        unsigned long long quad;          /* 64-bit value representing 1/10th usecs since Jan 1 1601, 00:00 UTC */
        _c_getsystime((struct _FILETIME*)&quad);
        return (double)(quad - 116444736000000000ULL)*1e-7;  /* time diff Jan 1 1601-Jan 1 1970 in 1/10th usecs */
    }

    static inline void cco_sleep(double sec) {
        Sleep((unsigned long)(sec*1000.0));
    }
#else
    #include <sys/time.h>
    static inline double cco_time(void) { /* seconds since epoch */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (double)tv.tv_sec + (double)tv.tv_usec*1e-6;
    }

    static inline void cco_sleep(double sec) {
        struct timeval tv;
        tv.tv_sec = (time_t)sec;
        tv.tv_usec = (suseconds_t)((sec - (double)(long)sec)*1e6);
        select(0, NULL, NULL, NULL, &tv);
    }
#endif

typedef struct { double interval, start; } cco_timer;

#define cco_await_timer(tm, sec) cco_await_timer_v_3(tm, sec, CCO_AWAIT)
#define cco_await_timer_v(...) c_MACRO_OVERLOAD(cco_await_timer_v, __VA_ARGS__)
#define cco_await_timer_v_2(tm, sec) cco_await_timer_v_3(tm, sec, )
#define cco_await_timer_v_3(tm, sec, ret) \
    do { \
        cco_timer_start(tm, sec); \
        cco_await_and_return(cco_timer_expired(tm), ret); \
    } while (0)

static inline void cco_timer_start(cco_timer* tm, double sec) {
    tm->interval = sec;
    tm->start = cco_time();
}

static inline cco_timer cco_timer_from(double sec) {
    cco_timer tm = {.interval=sec, .start=cco_time()};
    return tm;
}

static inline void cco_timer_restart(cco_timer* tm) {
    tm->start = cco_time();
}

static inline bool cco_timer_expired(cco_timer* tm) {
    return cco_time() - tm->start >= tm->interval;
}

static inline double cco_timer_elapsed(cco_timer* tm) {
    return cco_time() - tm->start;
}

static inline double cco_timer_remaining(cco_timer* tm) {
    return tm->start + tm->interval - cco_time();
}

#endif // STC_COROUTINE_H_INCLUDED
// ### END_FILE_INCLUDE: coroutine.h


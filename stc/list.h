// ### BEGIN_FILE_INCLUDE: list.h

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
    #define STC_API static
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

#if defined __clang__ && !defined __cplusplus
  #pragma clang diagnostic push
  #pragma clang diagnostic warning "-Wall"
  #pragma clang diagnostic warning "-Wextra"
  #pragma clang diagnostic warning "-Wpedantic"
  #pragma clang diagnostic warning "-Wconversion"
  #pragma clang diagnostic warning "-Wdouble-promotion"
  #pragma clang diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma clang diagnostic ignored "-Wmissing-field-initializers"
#elif defined __GNUC__ && !defined __cplusplus
  #pragma GCC diagnostic push
  #pragma GCC diagnostic warning "-Wall"
  #pragma GCC diagnostic warning "-Wextra"
  #pragma GCC diagnostic warning "-Wpedantic"
  #pragma GCC diagnostic warning "-Wconversion"
  #pragma GCC diagnostic warning "-Wdouble-promotion"
  #pragma GCC diagnostic warning "-Wwrite-strings"
  // ignored
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
// ### END_FILE_INCLUDE: linkage.h

#ifndef STC_LIST_H_INCLUDED
#define STC_LIST_H_INCLUDED
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
// ### BEGIN_FILE_INCLUDE: forward.h
#ifndef STC_FORWARD_H_INCLUDED
#define STC_FORWARD_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define forward_arc(C, VAL) _c_arc_types(C, VAL)
#define forward_box(C, VAL) _c_box_types(C, VAL)
#define forward_deq(C, VAL) _c_deq_types(C, VAL)
#define forward_list(C, VAL) _c_list_types(C, VAL)
#define forward_hmap(C, KEY, VAL) _c_htable_types(C, KEY, VAL, c_true, c_false)
#define forward_hset(C, KEY) _c_htable_types(C, cset, KEY, KEY, c_false, c_true)
#define forward_smap(C, KEY, VAL) _c_aatree_types(C, KEY, VAL, c_true, c_false)
#define forward_sset(C, KEY) _c_aatree_types(C, KEY, KEY, c_false, c_true)
#define forward_stack(C, VAL) _c_stack_types(C, VAL)
#define forward_pque(C, VAL) _c_pque_types(C, VAL)
#define forward_queue(C, VAL) _c_deq_types(C, VAL)
#define forward_vec(C, VAL) _c_vec_types(C, VAL)
// OLD deprecated names:
#define forward_carc forward_arc
#define forward_cbox forward_box
#define forward_cdeq forward_deq
#define forward_clist forward_list
#define forward_cmap forward_hmap
#define forward_cset forward_hset
#define forward_csmap forward_smap
#define forward_csset forward_sset
#define forward_cstack forward_stack
#define forward_cpque forward_pque
#define forward_cqueue forward_queue
#define forward_cvec forward_vec

// csview : non-null terminated string view
typedef const char csview_value;
typedef struct csview {
    csview_value* buf;
    intptr_t size;
} csview;

typedef union {
    csview_value* ref;
    csview chr;
    struct { csview chr; csview_value* end; } u8;
} csview_iter;

#define c_sv(...) c_MACRO_OVERLOAD(c_sv, __VA_ARGS__)
#define c_sv_1(literal) c_sv_2(literal, c_litstrlen(literal))
#define c_sv_2(str, n) (c_LITERAL(csview){str, n})
#define c_SV(sv) (int)(sv).size, (sv).buf // printf("%.*s\n", c_SV(sv));

// czview : null-terminated string view
typedef csview_value czview_value;
typedef struct czview {
    czview_value* str;
    intptr_t size;
} czview;

typedef union {
    czview_value* ref;
    csview chr;
} czview_iter;

#define c_zv(literal) c_zv_2(literal, c_litstrlen(literal))
#define c_zv_2(str, n) (c_LITERAL(czview){str, n})

// cstr : null-terminated owning string (short string optimized - sso)
typedef char cstr_value;
typedef struct { cstr_value* data; intptr_t size, cap; } cstr_buf;
typedef union cstr {
    struct { cstr_value data[ sizeof(cstr_buf) ]; } sml;
    struct { cstr_value* data; size_t size, ncap; } lon;
} cstr;

typedef union {
    cstr_value* ref;
    csview chr; // utf8 character/codepoint
} cstr_iter;


#if defined __GNUC__ || defined __clang__ || defined _MSC_VER
    typedef long catomic_long;
#else
    typedef _Atomic(long) catomic_long;
#endif

#define c_true(...) __VA_ARGS__
#define c_false(...)

#define _c_arc_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
        catomic_long* use_count; \
    } SELF

#define _c_box_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
    } SELF

#define _c_deq_types(SELF, VAL) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value *cbuf; \
        intptr_t start, end, capmask; \
    } SELF; \
\
    typedef struct { \
        SELF##_value *ref; \
        intptr_t pos; \
        const SELF* _s; \
    } SELF##_iter

#define _c_list_types(SELF, VAL) \
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
    } SELF

#define _c_htable_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
\
    typedef struct { \
        SELF##_value *ref; \
        bool inserted; \
        uint8_t hashx; \
    } SELF##_result; \
\
    typedef struct { \
        SELF##_value *ref, *_end; \
        struct hmap_slot *_sref; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value* table; \
        struct hmap_slot* slot; \
        intptr_t size, bucket_count; \
    } SELF

#define _c_aatree_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
    typedef KEY SELF##_key; \
    typedef VAL SELF##_mapped; \
    typedef struct SELF##_node SELF##_node; \
\
    typedef SET_ONLY( SELF##_key ) \
            MAP_ONLY( struct SELF##_value ) \
    SELF##_value; \
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
        int32_t root, disp, head, size, cap; \
    } SELF

#define _c_stack_fixed(SELF, VAL, CAP) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value data[CAP]; intptr_t _len; } SELF

#define _c_stack_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value* data; intptr_t _len, _cap; } SELF

#define _c_vec_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value *data; intptr_t _len, _cap; } SELF

#define _c_pque_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { SELF##_value* data; intptr_t _len, _cap; } SELF

#endif // STC_FORWARD_H_INCLUDED
// ### END_FILE_INCLUDE: forward.h
#include <stdlib.h>
#include <string.h>

#define _c_list_complete_types(SELF, dummy) \
    struct SELF##_node { \
        struct SELF##_node *next; \
        SELF##_value value; \
    }

#define _clist_tonode(vp) c_container_of(vp, _m_node, value)

#define _c_list_insert_entry_after(ref, val) \
    _m_node *entry = (_m_node *)i_malloc(c_sizeof *entry); entry->value = val; \
    _c_list_insert_after_node(ref, entry)

#define _c_list_insert_after_node(ref, entry) \
    if (ref) entry->next = ref->next, ref->next = entry; \
    else     entry->next = entry
    // +: set self->last based on node

#endif // STC_LIST_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix list_
#endif
// ### BEGIN_FILE_INCLUDE: template.h
#ifndef _i_template
#define _i_template

#ifndef STC_TEMPLATE_H_INCLUDED
#define STC_TEMPLATE_H_INCLUDED
  #define c_option(flag)  ((i_opt) & (flag))
  #define c_is_forward    (1<<0)
  #define c_no_atomic     (1<<1)
  #define c_no_clone      (1<<2)
  #define c_no_emplace    (1<<3)
  #define c_no_hash       (1<<4)
  #define c_use_cmp       (1<<5)
  #define c_more          (1<<6)

  #define _c_MEMB(name) c_JOIN(i_type, name)
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

#if defined i_TYPE && defined _i_ismap
  #define i_type _c_SEL(_c_SEL31, i_TYPE)
  #define i_key _c_SEL(_c_SEL32, i_TYPE)
  #define i_val _c_SEL(_c_SEL33, i_TYPE)
#elif defined i_TYPE
  #define i_type _c_SEL(_c_SEL21, i_TYPE)
  #define i_key _c_SEL(_c_SEL22, i_TYPE)
#endif
#ifndef i_type
  #define i_type c_JOIN(_i_prefix, i_tag)
#endif

#if defined i_keyclass || defined i_valclass || defined i_rawclass || \
    defined i_keyboxed || defined i_valboxed
  #error "i_keyclass, i_valclass, i_rawclass, i_keyboxed, i_valboxed is not supported. " \
         "Use: i_key_class, i_val_class, i_raw_class, i_key_arcbox, i_val_arcbox."
#endif

#if !(defined i_key || \
      defined i_key_str || defined i_key_ssv || \
      defined i_key_class || defined i_key_arcbox)
  #if defined _i_ismap
    #error "i_key* must be defined for maps"
  #endif

  #if defined i_val_str
    #define i_key_str i_val_str
  #endif
  #if defined i_val_ssv
    #define i_key_ssv i_val_ssv
  #endif
  #if defined i_val_arcbox
    #define i_key_arcbox i_val_arcbox
  #endif
  #if defined i_val_class
    #define i_key_class i_val_class
  #endif
  #if defined i_val
    #define i_key i_val
  #endif
  #if defined i_valraw
    #define i_keyraw i_valraw
  #endif
  #if defined i_valclone
    #define i_keyclone i_valclone
  #endif
  #if defined i_valfrom
    #define i_keyfrom i_valfrom
  #endif
  #if defined i_valto
    #define i_keyto i_valto
  #endif
  #if defined i_valdrop
    #define i_keydrop i_valdrop
  #endif
#endif

#if c_option(c_is_forward)
  #define i_is_forward
#endif
#if c_option(c_no_hash)
  #define i_no_hash
#endif
#if c_option(c_no_emplace)
  #define i_no_emplace
#endif
#if c_option(c_use_cmp) || defined i_cmp || defined i_less || \
                           defined _i_ismap || defined _i_isset || defined _i_ispque
  #define i_use_cmp
#endif
#if c_option(c_no_clone) || defined _i_carc
  #define i_no_clone
#endif
#if c_option(c_more)
  #define i_more
#endif

#if defined i_key_str
  #define i_key_class cstr
  #define i_raw_class ccharptr
  #ifndef i_tag
    #define i_tag str
  #endif
#elif defined i_key_ssv
  #define i_key_class cstr
  #define i_raw_class csview
  #define i_keyfrom cstr_from_sv
  #define i_keyto cstr_sv
  #ifndef i_tag
    #define i_tag ssv
  #endif
#elif defined i_key_arcbox
  #define i_key_class i_key_arcbox
  #define i_raw_class c_JOIN(i_key_arcbox, _raw)
  #if defined i_use_cmp
    #define i_eq c_JOIN(i_key_arcbox, _raw_eq)
  #endif
#endif

#if defined i_raw_class
  #define i_keyraw i_raw_class
#elif defined i_key_class && !defined i_keyraw
  #define i_raw_class i_key
#endif

#if defined i_key_class
  #define i_key i_key_class
  #ifndef i_keyclone
    #define i_keyclone c_JOIN(i_key, _clone)
  #endif
  #ifndef i_keydrop
    #define i_keydrop c_JOIN(i_key, _drop)
  #endif
  #if !defined i_keyfrom && defined i_keyraw
    #define i_keyfrom c_JOIN(i_key, _from)
  #endif
  #if !defined i_keyto && defined i_keyraw
    #define i_keyto c_JOIN(i_key, _toraw)
  #endif
#endif

#if defined i_raw_class
  #if !(defined i_cmp || defined i_less) && defined i_use_cmp
    #define i_cmp c_JOIN(i_keyraw, _cmp)
  #endif
  #if !(defined i_hash || defined i_no_hash)
    #define i_hash c_JOIN(i_keyraw, _hash)
  #endif
#endif

#if defined i_cmp || defined i_less || defined i_use_cmp
  #define _i_has_cmp
#endif
#if defined i_eq || defined i_use_cmp
  #define _i_has_eq
#endif
#if !(defined i_hash || defined i_no_hash)
  #define i_hash c_default_hash
#endif

#if !defined i_key
  #error "No i_key or i_val defined"
#elif defined i_keyraw ^ defined i_keyto
  #error "Both i_keyraw/i_valraw and i_keyto/i_valto must be defined, if any"
#elif !defined i_no_clone && (defined i_keyclone ^ defined i_keydrop)
  #error "Both i_keyclone/i_valclone and i_keydrop/i_valdrop must be defined, if any (unless i_no_clone defined)."
#elif defined i_from || defined i_drop
  #error "i_from / i_drop not supported. Define i_keyfrom/i_valfrom and/or i_keydrop/i_valdrop instead"
#elif defined i_keyraw && defined _i_ishash && !(defined i_hash && (defined _i_has_cmp || defined i_eq))
  #error "For cmap/cset, both i_hash and i_eq (or i_less or i_cmp) must be defined when i_keyraw is defined."
#elif defined i_keyraw && defined i_use_cmp && !defined _i_has_cmp
  #error "For csmap/csset/cpque, i_cmp or i_less must be defined when i_keyraw is defined."
#endif

// i_eq, i_less, i_cmp
#if !defined i_eq && defined i_cmp
  #define i_eq(x, y) !(i_cmp(x, y))
#elif !defined i_eq && !defined i_keyraw
  #define i_eq(x, y) *x == *y // for integral types, else define i_eq or i_cmp yourself
#endif
#if !defined i_less && defined i_cmp
  #define i_less(x, y) (i_cmp(x, y)) < 0
#elif !defined i_less && !defined i_keyraw
  #define i_less(x, y) *x < *y // for integral types, else define i_less or i_cmp yourself
#endif
#if !defined i_cmp && defined i_less
  #define i_cmp(x, y) (i_less(y, x)) - (i_less(x, y))
#endif

#ifndef i_tag
  #define i_tag i_key
#endif
#ifndef i_keyraw
  #define i_keyraw i_key
#endif
#ifndef i_keyfrom
  #define i_keyfrom c_default_clone
#else
  #define i_has_emplace
#endif
#ifndef i_keyto
  #define i_keyto c_default_toraw
#endif
#ifndef i_keyclone
  #define i_keyclone c_default_clone
#endif
#ifndef i_keydrop
  #define i_keydrop c_default_drop
#endif

#if defined _i_ismap // ---- process cmap/csmap value i_val, ... ----

#ifdef i_val_str
  #define i_val_class cstr
  #define i_valraw const char*
#elif defined i_val_ssv
  #define i_val_class cstr
  #define i_valraw csview
  #define i_valfrom cstr_from_sv
  #define i_valto cstr_sv
#elif defined i_val_arcbox
  #define i_val_class i_val_arcbox
  #define i_valraw c_JOIN(i_val_arcbox, _raw)
#endif

#ifdef i_val_class
  #define i_val i_val_class
  #ifndef i_valclone
    #define i_valclone c_JOIN(i_val, _clone)
  #endif
  #ifndef i_valdrop
    #define i_valdrop c_JOIN(i_val, _drop)
  #endif
  #if !defined i_valfrom && defined i_valraw
    #define i_valfrom c_JOIN(i_val, _from)
  #endif
  #if !defined i_valto && defined i_valraw
    #define i_valto c_JOIN(i_val, _toraw)
  #endif
#endif

#ifndef i_val
  #error "i_val* must be defined for maps"
#elif defined i_valraw ^ defined i_valto
  #error "Both i_valraw and i_valto must be defined, if any"
#elif !defined i_no_clone && (defined i_valclone ^ defined i_valdrop)
  #error "Both i_valclone and i_valdrop must be defined, if any"
#endif

#ifndef i_valraw
  #define i_valraw i_val
#endif
#ifndef i_valfrom
  #define i_valfrom c_default_clone
#else
  #define i_has_emplace
#endif
#ifndef i_valto
  #define i_valto c_default_toraw
#endif
#ifndef i_valclone
  #define i_valclone c_default_clone
#endif
#ifndef i_valdrop
  #define i_valdrop c_default_drop
#endif

#endif // !_i_ismap

#ifndef i_val
  #define i_val i_key
#endif
#ifndef i_valraw
  #define i_valraw i_keyraw
#endif
#ifndef i_has_emplace
  #define i_no_emplace
#endif // STC_TEMPLATE_H_INCLUDED
#endif
// ### END_FILE_INCLUDE: template.h

#ifndef i_is_forward
  _c_DEFTYPES(_c_list_types, i_type, i_key);
#endif
_c_DEFTYPES(_c_list_complete_types, i_type, dummy);
typedef i_keyraw _m_raw;

STC_API void            _c_MEMB(_drop)(i_type* self);
STC_API _m_value*       _c_MEMB(_push_back)(i_type* self, _m_value value);
STC_API _m_value*       _c_MEMB(_push_front)(i_type* self, _m_value value);
STC_API _m_iter         _c_MEMB(_insert_at)(i_type* self, _m_iter it, _m_value value);
STC_API _m_iter         _c_MEMB(_erase_at)(i_type* self, _m_iter it);
STC_API _m_iter         _c_MEMB(_erase_range)(i_type* self, _m_iter it1, _m_iter it2);
#if defined _i_has_eq || defined _i_has_cmp
STC_API _m_iter         _c_MEMB(_find_in)(_m_iter it1, _m_iter it2, _m_raw val);
STC_API intptr_t        _c_MEMB(_remove)(i_type* self, _m_raw val);
#endif
#if defined _i_has_cmp
STC_API bool            _c_MEMB(_sort_with)(i_type* self, int(*cmp)(const _m_value*, const _m_value*));
STC_API int             _c_MEMB(_sort_cmp_)(const _m_value*, const _m_value*);
STC_INLINE bool         _c_MEMB(_sort)(i_type* self)
                            { return _c_MEMB(_sort_with)(self, _c_MEMB(_sort_cmp_)); }
#endif
STC_API void            _c_MEMB(_reverse)(i_type* self);
STC_API _m_iter         _c_MEMB(_splice)(i_type* self, _m_iter it, i_type* other);
STC_API i_type          _c_MEMB(_split_off)(i_type* self, _m_iter it1, _m_iter it2);
STC_API _m_value*       _c_MEMB(_push_back_node)(i_type* self, _m_node* node);
STC_API _m_value*       _c_MEMB(_insert_after_node)(i_type* self, _m_node* ref, _m_node* node);
STC_API _m_node*        _c_MEMB(_unlink_after_node)(i_type* self, _m_node* ref);
STC_API void            _c_MEMB(_erase_after_node)(i_type* self, _m_node* ref);
STC_INLINE _m_node*     _c_MEMB(_get_node)(_m_value* pval) { return _clist_tonode(pval); }
STC_INLINE _m_node*     _c_MEMB(_unlink_front_node)(i_type* self)
                            { return _c_MEMB(_unlink_after_node)(self, self->last); }
#if !defined i_no_clone
STC_API i_type          _c_MEMB(_clone)(i_type cx);
STC_INLINE _m_value     _c_MEMB(_value_clone)(_m_value val) { return i_keyclone(val); }

STC_INLINE void
_c_MEMB(_copy)(i_type *self, const i_type* other) {
    if (self->last == other->last) return;
    _c_MEMB(_drop)(self); *self = _c_MEMB(_clone)(*other);
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE _m_value*    _c_MEMB(_emplace_back)(i_type* self, _m_raw raw)
                            { return _c_MEMB(_push_back)(self, i_keyfrom(raw)); }
STC_INLINE _m_value*    _c_MEMB(_emplace_front)(i_type* self, _m_raw raw)
                            { return _c_MEMB(_push_front)(self, i_keyfrom(raw)); }
STC_INLINE _m_iter      _c_MEMB(_emplace_at)(i_type* self, _m_iter it, _m_raw raw)
                            { return _c_MEMB(_insert_at)(self, it, i_keyfrom(raw)); }
STC_INLINE _m_value*    _c_MEMB(_emplace)(i_type* self, _m_raw raw)
                            { return _c_MEMB(_push_back)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

STC_INLINE i_type       _c_MEMB(_init)(void) { return c_LITERAL(i_type){NULL}; }
STC_INLINE void         _c_MEMB(_put_n)(i_type* self, const _m_raw* raw, intptr_t n)
                            { while (n--) _c_MEMB(_push_back)(self, i_keyfrom(*raw++)); }
STC_INLINE i_type       _c_MEMB(_from_n)(const _m_raw* raw, intptr_t n)
                            { i_type cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE bool         _c_MEMB(_reserve)(i_type* self, intptr_t n) { (void)(self + n); return true; }
STC_INLINE bool         _c_MEMB(_empty)(const i_type* self) { return self->last == NULL; }
STC_INLINE void         _c_MEMB(_clear)(i_type* self) { _c_MEMB(_drop)(self); }
STC_INLINE _m_value*    _c_MEMB(_push)(i_type* self, _m_value value)
                            { return _c_MEMB(_push_back)(self, value); }
STC_INLINE void         _c_MEMB(_pop_front)(i_type* self)
                            { c_assert(!_c_MEMB(_empty)(self)); _c_MEMB(_erase_after_node)(self, self->last); }
STC_INLINE _m_value*    _c_MEMB(_front)(const i_type* self) { return &self->last->next->value; }
STC_INLINE _m_value*    _c_MEMB(_back)(const i_type* self) { return &self->last->value; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* pval) { return i_keyto(pval); }
STC_INLINE void         _c_MEMB(_value_drop)(_m_value* pval) { i_keydrop(pval); }

STC_INLINE intptr_t
_c_MEMB(_count)(const i_type* self) {
    intptr_t n = 1; const _m_node *node = self->last;
    if (!node) return 0;
    while ((node = node->next) != self->last) ++n;
    return n;
}

STC_INLINE _m_iter
_c_MEMB(_begin)(const i_type* self) {
    _m_value* head = self->last ? &self->last->next->value : NULL;
    return c_LITERAL(_m_iter){head, &self->last, self->last};
}

STC_INLINE _m_iter
_c_MEMB(_end)(const i_type* self)
    { (void)self; return c_LITERAL(_m_iter){NULL}; }

STC_INLINE void
_c_MEMB(_next)(_m_iter* it) {
    _m_node* node = it->prev = _clist_tonode(it->ref);
    it->ref = (node == *it->_last ? NULL : &node->next->value);
}

STC_INLINE _m_iter
_c_MEMB(_advance)(_m_iter it, size_t n) {
    while (n-- && it.ref) _c_MEMB(_next)(&it);
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_splice_range)(i_type* self, _m_iter it,
                       i_type* other, _m_iter it1, _m_iter it2) {
    i_type tmp = _c_MEMB(_split_off)(other, it1, it2);
    return _c_MEMB(_splice)(self, it, &tmp);
}

#if defined _i_has_eq || defined _i_has_cmp
STC_INLINE _m_iter
_c_MEMB(_find)(const i_type* self, _m_raw val) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), val);
}

STC_INLINE const _m_value*
_c_MEMB(_get)(const i_type* self, _m_raw val) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), val).ref;
}

STC_INLINE _m_value*
_c_MEMB(_get_mut)(i_type* self, _m_raw val) {
    return _c_MEMB(_find_in)(_c_MEMB(_begin)(self), _c_MEMB(_end)(self), val).ref;
}

STC_INLINE bool _c_MEMB(_eq)(const i_type* self, const i_type* other) {
    _m_iter i = _c_MEMB(_begin)(self), j = _c_MEMB(_begin)(other);
    for (; i.ref && j.ref; _c_MEMB(_next)(&i), _c_MEMB(_next)(&j)) {
        const _m_raw _rx = i_keyto(i.ref), _ry = i_keyto(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return !(i.ref || j.ref);
}
#endif

// -------------------------- IMPLEMENTATION -------------------------
#if defined(i_implement) || defined(i_static)

#if !defined i_no_clone
STC_DEF i_type
_c_MEMB(_clone)(i_type cx) {
    i_type out = _c_MEMB(_init)();
    c_foreach (it, i_type, cx)
        _c_MEMB(_push_back)(&out, i_keyclone((*it.ref)));
    return out;
}
#endif

STC_DEF void
_c_MEMB(_drop)(i_type* self) {
    while (self->last) _c_MEMB(_erase_after_node)(self, self->last);
}

STC_DEF _m_value*
_c_MEMB(_push_back)(i_type* self, _m_value value) {
    _c_list_insert_entry_after(self->last, value);
    self->last = entry;
    return &entry->value;
}

STC_DEF _m_value*
_c_MEMB(_push_front)(i_type* self, _m_value value) {
    _c_list_insert_entry_after(self->last, value);
    if (!self->last)
        self->last = entry;
    return &entry->value;
}

STC_DEF _m_value*
_c_MEMB(_push_back_node)(i_type* self, _m_node* node) {
    _c_list_insert_after_node(self->last, node);
    self->last = node;
    return &node->value;
}

STC_DEF _m_value*
_c_MEMB(_insert_after_node)(i_type* self, _m_node* ref, _m_node* node) {
    _c_list_insert_after_node(ref, node);
    if (!self->last)
        self->last = node;
    return &node->value;
}

STC_DEF _m_iter
_c_MEMB(_insert_at)(i_type* self, _m_iter it, _m_value value) {
    _m_node* node = it.ref ? it.prev : self->last;
    _c_list_insert_entry_after(node, value);
    if (!self->last || !it.ref) {
        it.prev = self->last ? self->last : entry;
        self->last = entry;
    }
    it.ref = &entry->value;
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_at)(i_type* self, _m_iter it) {
    _m_node *node = _clist_tonode(it.ref);
    it.ref = (node == self->last) ? NULL : &node->next->value;
    _c_MEMB(_erase_after_node)(self, it.prev);
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_range)(i_type* self, _m_iter it1, _m_iter it2) {
    _m_node *end = it2.ref ? _clist_tonode(it2.ref) : self->last->next;
    if (it1.ref != it2.ref) do {
        _c_MEMB(_erase_after_node)(self, it1.prev);
        if (!self->last) break;
    } while (it1.prev->next != end);
    return it2;
}

STC_DEF void
_c_MEMB(_erase_after_node)(i_type* self, _m_node* ref) {
    _m_node* node = _c_MEMB(_unlink_after_node)(self, ref);
    i_keydrop((&node->value));
    i_free(node, c_sizeof *node);
}

STC_DEF _m_node*
_c_MEMB(_unlink_after_node)(i_type* self, _m_node* ref) {
    _m_node* node = ref->next, *next = node->next;
    ref->next = next;
    if (node == next)
        self->last = NULL;
    else if (node == self->last)
        self->last = ref;
    return node;
}

STC_DEF void
_c_MEMB(_reverse)(i_type* self) {
    i_type rev = {NULL};
    while (self->last) {
        _m_node* node = _c_MEMB(_unlink_after_node)(self, self->last);
        _c_MEMB(_insert_after_node)(&rev, rev.last, node);
    }
    *self = rev;
}

STC_DEF _m_iter
_c_MEMB(_splice)(i_type* self, _m_iter it, i_type* other) {
    if (!self->last)
        self->last = other->last;
    else if (other->last) {
        _m_node *p = it.ref ? it.prev : self->last, *next = p->next;
        it.prev = other->last;
        p->next = it.prev->next;
        it.prev->next = next;
        if (!it.ref) self->last = it.prev;
    }
    other->last = NULL;
    return it;
}

STC_DEF i_type
_c_MEMB(_split_off)(i_type* self, _m_iter it1, _m_iter it2) {
    i_type lst = {NULL};
    if (it1.ref == it2.ref)
        return lst;
    _m_node *p1 = it1.prev,
            *p2 = it2.ref ? it2.prev : self->last;
    p1->next = p2->next;
    p2->next = _clist_tonode(it1.ref);
    if (self->last == p2)
        self->last = (p1 == p2) ? NULL : p1;
    lst.last = p2;
    return lst;
}

#if defined _i_has_eq || defined _i_has_cmp
STC_DEF _m_iter
_c_MEMB(_find_in)(_m_iter it1, _m_iter it2, _m_raw val) {
    c_foreach (it, i_type, it1, it2) {
        _m_raw r = i_keyto(it.ref);
        if (i_eq((&r), (&val)))
            return it;
    }
    it2.ref = NULL; return it2;
}

STC_DEF intptr_t
_c_MEMB(_remove)(i_type* self, _m_raw val) {
    intptr_t n = 0;
    _m_node *prev = self->last, *node;
    if (prev) do {
        node = prev->next;
        _m_raw r = i_keyto((&node->value));
        if (i_eq((&r), (&val))) {
            _c_MEMB(_erase_after_node)(self, prev), ++n;
            if (!self->last) break;
        } else
            prev = node;
    } while (node != self->last);
    return n;
}
#endif

#if defined _i_has_cmp
STC_DEF int _c_MEMB(_sort_cmp_)(const _m_value* x, const _m_value* y) {
    const _m_raw a = i_keyto(x), b = i_keyto(y);
    return i_cmp((&a), (&b));
}

STC_DEF bool _c_MEMB(_sort_with)(i_type* self, int(*cmp)(const _m_value*, const _m_value*)) {
    intptr_t len = 0, cap = 0;
    _m_value *arr = NULL, *p = NULL;
    c_foreach (i, i_type, *self) {
        if (len == cap) {
            intptr_t cap_n = cap + cap/2 + 8;
            if (!(p = (_m_value *)i_realloc(arr, cap*c_sizeof *p, cap_n*c_sizeof *p)))
                goto done;
            arr = p, cap = cap_n;
        }
        arr[len++] = *i.ref;
    }
    qsort(arr, (size_t)len, sizeof *arr, (int(*)(const void*, const void*))cmp);
    c_foreach (i, i_type, *self)
        *i.ref = *p++;
    done: i_free(arr, cap*c_sizeof *arr);
    return p != NULL;
}
#endif // _i_has_cmp
#endif // i_implement
// ### BEGIN_FILE_INCLUDE: template2.h
#ifdef i_more
#undef i_more
#else
#undef i_TYPE
#undef i_type
#undef i_tag
#undef i_imp
#undef i_opt
#undef i_less
#undef i_cmp
#undef i_eq
#undef i_hash
#undef i_capacity
#undef i_raw_class

#undef i_val
#undef i_val_str
#undef i_val_ssv
#undef i_val_arcbox
#undef i_val_class
#undef i_valraw
#undef i_valclone
#undef i_valfrom
#undef i_valto
#undef i_valdrop

#undef i_key
#undef i_key_str
#undef i_key_ssv
#undef i_key_arcbox
#undef i_key_class
#undef i_keyraw
#undef i_keyclone
#undef i_keyfrom
#undef i_keyto
#undef i_keydrop

#undef i_use_cmp
#undef i_no_hash
#undef i_no_clone
#undef i_no_emplace
#undef i_is_forward
#undef i_has_emplace

#undef _i_has_cmp
#undef _i_has_eq
#undef _i_prefix
#undef _i_template

#undef i_keyclass // [deprecated]
#undef i_valclass // [deprecated]
#undef i_rawclass // [deprecated]
#undef i_keyboxed // [deprecated]
#undef i_valboxed // [deprecated]
#endif
// ### END_FILE_INCLUDE: template2.h
// ### BEGIN_FILE_INCLUDE: linkage2.h

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
// ### END_FILE_INCLUDE: list.h

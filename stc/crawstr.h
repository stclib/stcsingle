// ### BEGIN_FILE_INCLUDE: crawstr.h
#define _i_inc_utf8
// ### BEGIN_FILE_INCLUDE: utf8.h
// ### BEGIN_FILE_INCLUDE: linkage.h
#undef STC_API
#undef STC_DEF

#ifdef i_extern // [deprecated]
#  define i_import
#endif
#if !defined(i_static) && !defined(STC_STATIC) && (defined(i_header) || defined(STC_HEADER) || \
                                                   defined(i_implement) || defined(STC_IMPLEMENT))
  #define STC_API extern
  #define STC_DEF
#else
  #define i_static
  #define STC_API static
  #define STC_DEF STC_API
#endif
#if defined(STC_IMPLEMENT) || defined(i_import)
  #define i_implement
#endif
// ### END_FILE_INCLUDE: linkage.h

#ifndef UTF8_H_INCLUDED
#define UTF8_H_INCLUDED

// ### BEGIN_FILE_INCLUDE: ccommon.h
#ifndef CCOMMON_H_INCLUDED
#define CCOMMON_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef long long _llong;
#define c_NPOS INTPTR_MAX
#define c_ZI PRIiPTR
#define c_ZU PRIuPTR

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
    c_JOIN(c_JOIN0(name,_),c_NUMARGS(__VA_ARGS__))(__VA_ARGS__)
#define c_JOIN0(a, b) a ## b
#define c_JOIN(a, b) c_JOIN0(a, b)
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
  #define _i_new(T, ...)        ((T*)memcpy(_i_alloc(T), ((T[]){__VA_ARGS__}), sizeof(T)))
  #define c_new(T, ...)         ((T*)memcpy(malloc(sizeof(T)), ((T[]){__VA_ARGS__}), sizeof(T)))
  #define c_LITERAL(T)          (T)
#endif
#define c_new_n(T, n)           ((T*)malloc(sizeof(T)*c_i2u_size(n)))
#define c_malloc(sz)            malloc(c_i2u_size(sz))
#define c_calloc(n, sz)         calloc(c_i2u_size(n), c_i2u_size(sz))
#define c_realloc(p, sz)        realloc(p, c_i2u_size(sz))
#define c_free(p)               free(p)
#define c_delete(T, ptr)        do { T *_tp = ptr; T##_drop(_tp); free(_tp); } while (0)

#define c_static_assert(expr)   (1 ? 0 : (int)sizeof(int[(expr) ? 1 : -1]))
#if defined STC_NDEBUG || defined NDEBUG
  #define c_assert(expr)        (0)
#else
  #define c_assert(expr)        assert(expr)
#endif
#define c_container_of(p, C, m) ((C*)((char*)(1 ? (p) : &((C*)0)->m) - offsetof(C, m)))
#define c_const_cast(Tp, p)     ((Tp)(1 ? (p) : (Tp)0))
#define c_safe_cast(T, F, x)    ((T)(1 ? (x) : *(F*)0))
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
#define c_less_unsigned(a, b)   ((size_t)(a) < (size_t)(b))

// x and y are i_keyraw* type, defaults to i_key*:
#define c_default_cmp(x, y)     (c_default_less(y, x) - c_default_less(x, y))
#define c_default_less(x, y)    (*(x) < *(y))
#define c_default_eq(x, y)      (*(x) == *(y))
#define c_memcmp_eq(x, y)       (memcmp(x, y, sizeof *(x)) == 0)
#define c_default_hash(x)       stc_hash(x, c_sizeof(*(x)))

#define c_default_clone(v)      (v)
#define c_default_toraw(vp)     (*(vp))
#define c_default_drop(vp)      ((void) (vp))

#define c_option(flag)          ((i_opt) & (flag))
#define c_is_forward            (1<<0)
#define c_no_atomic             (1<<1)
#define c_no_clone              (1<<2)
#define c_no_emplace            (1<<3)
#define c_no_hash               (1<<4)
#define c_use_cmp               (1<<5)
/* Function macros and others */

#define c_litstrlen(literal) (c_sizeof("" literal) - 1)
#define c_arraylen(a) (intptr_t)(sizeof(a)/sizeof 0[a])

// Non-owning c-string "class"
typedef const char* ccharptr;
#define ccharptr_cmp(xp, yp) strcmp(*(xp), *(yp))
#define ccharptr_hash(p) stc_strhash(*(p))
#define ccharptr_clone(s) (s)
#define ccharptr_drop(p) ((void)p)

#define c_sv(...) c_MACRO_OVERLOAD(c_sv, __VA_ARGS__)
#define c_sv_1(literal) c_sv_2(literal, c_litstrlen(literal))
#define c_sv_2(str, n) (c_LITERAL(csview){str, n})
#define c_SV(sv) (int)(sv).size, (sv).buf // printf("%.*s\n", c_SV(sv));

#define c_rs(literal) c_rs_2(literal, c_litstrlen(literal))
#define c_rs_2(str, n) (c_LITERAL(crawstr){str, n})

#define c_ROTL(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

STC_INLINE uint64_t stc_hash(const void* key, intptr_t len) {
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
    { return stc_hash(str, c_strlen(str)); }

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
    for (struct {C##_iter it; const C##_key* key; C##_mapped* val;} _ = {.it=C##_begin(&cnt)} \
         ; _.it.ref && (_.key = &_.it.ref->first, _.val = &_.it.ref->second) \
         ; C##_next(&_.it))

#define c_forrange(...) c_for(long long, __VA_ARGS__)
#define c_for(...) c_MACRO_OVERLOAD(c_for, __VA_ARGS__)
#define c_for_2(T, stop) c_for_4(T, _c_i, 0, stop)
#define c_for_3(T, i, stop) c_for_4(T, i, 0, stop)
#define c_for_4(T, i, start, stop) \
    for (T i=start, _end=stop; i < _end; ++i)
#define c_for_5(T, i, start, stop, step) \
    for (T i=start, _inc=step, _end=(T)(stop) - (_inc > 0) \
         ; (_inc > 0) ^ (i > _end); i += _inc)

#ifndef __cplusplus
  #define c_init(C, ...) \
      C##_from_n((C##_raw[])__VA_ARGS__, c_sizeof((C##_raw[])__VA_ARGS__)/c_sizeof(C##_raw))
  #define c_forlist(it, T, ...) \
      for (struct {T* ref; int size, index;} \
           it = {.ref=(T[])__VA_ARGS__, .size=(int)(sizeof((T[])__VA_ARGS__)/sizeof(T))} \
           ; it.index < it.size; ++it.ref, ++it.index)
#else
    #include <initializer_list>
    template <class C, class T>
    inline C _from_n(C (*func)(const T[], intptr_t), std::initializer_list<T> il)
        { return func(&*il.begin(), il.size()); }

    #define c_init(C, ...) _from_n<C,C##_raw>(C##_from_n, __VA_ARGS__)
    #define c_forlist(it, T, ...) \
        for (struct {std::initializer_list<T> _il; std::initializer_list<T>::iterator ref; size_t size, index;} \
             it = {._il=__VA_ARGS__, .ref=it._il.begin(), .size=it._il.size()} \
             ; it.index < it.size; ++it.ref, ++it.index)
#endif

#define c_defer(...) \
    for (int _i = 1; _i; _i = 0, __VA_ARGS__)
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

#endif // CCOMMON_H_INCLUDED
// ### END_FILE_INCLUDE: ccommon.h
#include <ctype.h>
// ### BEGIN_FILE_INCLUDE: forward.h
#ifndef STC_FORWARD_H_INCLUDED
#define STC_FORWARD_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define forward_carc(CX, VAL) _c_carc_types(CX, VAL)
#define forward_cbox(CX, VAL) _c_cbox_types(CX, VAL)
#define forward_cdeq(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_clist(CX, VAL) _c_clist_types(CX, VAL)
#define forward_cmap(CX, KEY, VAL) _c_chash_types(CX, KEY, VAL, c_true, c_false)
#define forward_cset(CX, KEY) _c_chash_types(CX, cset, KEY, KEY, c_false, c_true)
#define forward_csmap(CX, KEY, VAL) _c_aatree_types(CX, KEY, VAL, c_true, c_false)
#define forward_csset(CX, KEY) _c_aatree_types(CX, KEY, KEY, c_false, c_true)
#define forward_cstack(CX, VAL) _c_cstack_types(CX, VAL)
#define forward_cpque(CX, VAL) _c_cpque_types(CX, VAL)
#define forward_cqueue(CX, VAL) _c_cdeq_types(CX, VAL)
#define forward_cvec(CX, VAL) _c_cvec_types(CX, VAL)

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


// crawstr : null-terminated string view
typedef csview_value crawstr_value;
typedef struct crawstr {
    crawstr_value* str;
    intptr_t size;
} crawstr;

typedef union {
    crawstr_value* ref;
    csview chr;
    struct { csview chr; } u8; // [deprecated]
} crawstr_iter;


// cstr : null-terminated string (short string optimized - sso)
typedef char cstr_value;
typedef struct { cstr_value* data; intptr_t size, cap; } cstr_buf;
typedef union cstr {
    struct { cstr_value data[sizeof(cstr_buf) - 1]; unsigned char size; } sml;
    struct { cstr_value* data; size_t size, ncap; } lon;
} cstr;

typedef union {
    cstr_value* ref;
    csview chr;
    struct { csview chr; } u8; // [deprecated]
} cstr_iter;


#define c_true(...) __VA_ARGS__
#define c_false(...)

#define _c_carc_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
        catomic_long* use_count; \
    } SELF

#define _c_cbox_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { \
        SELF##_value* get; \
    } SELF

#define _c_cdeq_types(SELF, VAL) \
    typedef VAL SELF##_value; \
\
    typedef struct SELF { \
        SELF##_value *data; \
        intptr_t start, end, capmask; \
    } SELF; \
\
    typedef struct { \
        SELF##_value *ref; \
        intptr_t pos; \
        const SELF* _s; \
    } SELF##_iter

#define _c_clist_types(SELF, VAL) \
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

#define _c_chash_types(SELF, KEY, VAL, MAP_ONLY, SET_ONLY) \
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
        struct chash_slot* sref; \
    } SELF##_iter; \
\
    typedef struct SELF { \
        SELF##_value* data; \
        struct chash_slot* slot; \
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

#define _c_cstack_fixed(SELF, VAL, CAP) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value data[CAP]; intptr_t _len; } SELF

#define _c_cstack_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value* data; intptr_t _len, _cap; } SELF

#define _c_cvec_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct { SELF##_value *ref, *end; } SELF##_iter; \
    typedef struct SELF { SELF##_value *data; intptr_t _len, _cap; } SELF

#define _c_cpque_types(SELF, VAL) \
    typedef VAL SELF##_value; \
    typedef struct SELF { SELF##_value* data; intptr_t _len, _cap; } SELF

#endif // STC_FORWARD_H_INCLUDED
// ### END_FILE_INCLUDE: forward.h

enum {
    U8G_Cc, U8G_Lt, U8G_Nd, U8G_Nl,
    U8G_Pc, U8G_Pd, U8G_Pf, U8G_Pi,
    U8G_Sc, U8G_Zl, U8G_Zp, U8G_Zs,
    U8G_Arabic, U8G_Cyrillic,
    U8G_Devanagari, U8G_Greek,
    U8G_Han, U8G_Latin,
    U8G_SIZE
};

// utf8 methods defined in src/utf8code.c:
extern bool     utf8_isgroup(int group, uint32_t c); 
extern bool     utf8_isalpha(uint32_t c);
extern uint32_t utf8_casefold(uint32_t c);
extern uint32_t utf8_tolower(uint32_t c);
extern uint32_t utf8_toupper(uint32_t c);
extern bool     utf8_iscased(uint32_t c);
extern bool     utf8_isword(uint32_t c);
extern bool     utf8_valid_n(const char* s, intptr_t nbytes);
extern int      utf8_icmp_sv(csview s1, csview s2);
extern int      utf8_encode(char *out, uint32_t c);
extern uint32_t utf8_peek_off(const char *s, int offset);

STC_INLINE bool utf8_isupper(uint32_t c) 
    { return utf8_tolower(c) != c; }

STC_INLINE bool utf8_islower(uint32_t c) 
    { return utf8_toupper(c) != c; }

STC_INLINE bool utf8_isalnum(uint32_t c) {
    if (c < 128) return isalnum((int)c) != 0;
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c);
}

STC_INLINE bool utf8_isblank(uint32_t c) {
    if (c < 128) return (c == ' ') | (c == '\t');
    return utf8_isgroup(U8G_Zs, c);
}

STC_INLINE bool utf8_isspace(uint32_t c) {
    if (c < 128) return isspace((int)c) != 0;
    return ((c == 8232) | (c == 8233)) || utf8_isgroup(U8G_Zs, c);
}

/* decode next utf8 codepoint. https://bjoern.hoehrmann.de/utf-8/decoder/dfa */
typedef struct { uint32_t state, codep; } utf8_decode_t;
extern const uint8_t utf8_dtab[]; /* utf8code.c */

STC_INLINE uint32_t utf8_decode(utf8_decode_t* d, const uint32_t byte) {
    const uint32_t type = utf8_dtab[byte];
    d->codep = d->state ? (byte & 0x3fu) | (d->codep << 6)
                        : (0xffU >> type) & byte;
    return d->state = utf8_dtab[256 + d->state + type];
}

STC_INLINE uint32_t utf8_peek(const char* s) {
    utf8_decode_t d = {.state=0};
    do { utf8_decode(&d, (uint8_t)*s++); } while (d.state);
    return d.codep;
}

/* case-insensitive utf8 string comparison */
STC_INLINE int utf8_icmp(const char* s1, const char* s2) {
    return utf8_icmp_sv(c_sv(s1, INTPTR_MAX), c_sv(s2, INTPTR_MAX));
}

STC_INLINE bool utf8_valid(const char* s) {
    return utf8_valid_n(s, INTPTR_MAX);
}

/* following functions are independent but assume valid utf8 strings: */

/* number of bytes in the utf8 codepoint from s */
STC_INLINE int utf8_chr_size(const char *s) {
    unsigned b = (uint8_t)*s;
    if (b < 0x80) return 1;
    /*if (b < 0xC2) return 0;*/
    if (b < 0xE0) return 2;
    if (b < 0xF0) return 3;
    /*if (b < 0xF5)*/ return 4;
    /*return 0;*/
}

/* number of codepoints in the utf8 string s */
STC_INLINE intptr_t utf8_size(const char *s) {
    intptr_t size = 0;
    while (*s)
        size += (*++s & 0xC0) != 0x80;
    return size;
}

STC_INLINE intptr_t utf8_size_n(const char *s, intptr_t nbytes) {
    intptr_t size = 0;
    while ((nbytes-- != 0) & (*s != 0)) {
        size += (*++s & 0xC0) != 0x80;
    }
    return size;
}

STC_INLINE const char* utf8_at(const char *s, intptr_t index) {
    while ((index > 0) & (*s != 0))
        index -= (*++s & 0xC0) != 0x80;
    return s;
}

STC_INLINE intptr_t utf8_pos(const char* s, intptr_t index)
    { return (intptr_t)(utf8_at(s, index) - s); }
#endif // UTF8_H_INCLUDED

#if defined i_import || (defined i_implement && !defined _i_inc_utf8)
// ### BEGIN_FILE_INCLUDE: utf8code.c
#ifndef UTF8_C_INCLUDED
#define UTF8_C_INCLUDED

#ifndef UTF8_H_INCLUDED
#endif
// ### BEGIN_FILE_INCLUDE: utf8tabs.inc
#include <stdint.h>

struct CaseMapping { uint16_t c1, c2, m2; };

static struct CaseMapping casemappings[] = {
    {0x0041, 0x005A, 0x007A}, // A a (26) LATIN CAPITAL LETTER A
    {0x00B5, 0x00B5, 0x03BC}, // µ μ ( 1) MICRO SIGN
    {0x00C0, 0x00D6, 0x00F6}, // À à (23) LATIN CAPITAL LETTER A WITH GRAVE
    {0x00D8, 0x00DE, 0x00FE}, // Ø ø ( 7) LATIN CAPITAL LETTER O WITH STROKE
    {0x0100, 0x012E, 0x012F}, // Ā ā (24) LATIN CAPITAL LETTER A WITH MACRON
    {0x0132, 0x0136, 0x0137}, // Ĳ ĳ ( 3) LATIN CAPITAL LIGATURE IJ
    {0x0139, 0x0147, 0x0148}, // Ĺ ĺ ( 8) LATIN CAPITAL LETTER L WITH ACUTE
    {0x014A, 0x0176, 0x0177}, // Ŋ ŋ (23) LATIN CAPITAL LETTER ENG
    {0x0178, 0x0178, 0x00FF}, // Ÿ ÿ ( 1) LATIN CAPITAL LETTER Y WITH DIAERESIS
    {0x0179, 0x017D, 0x017E}, // Ź ź ( 3) LATIN CAPITAL LETTER Z WITH ACUTE
    {0x017F, 0x017F, 0x0073}, // ſ s ( 1) LATIN SMALL LETTER LONG S
    {0x0181, 0x0181, 0x0253}, // Ɓ ɓ ( 1) LATIN CAPITAL LETTER B WITH HOOK
    {0x0182, 0x0184, 0x0185}, // Ƃ ƃ ( 2) LATIN CAPITAL LETTER B WITH TOPBAR
    {0x0186, 0x0186, 0x0254}, // Ɔ ɔ ( 1) LATIN CAPITAL LETTER OPEN O
    {0x0187, 0x0187, 0x0188}, // Ƈ ƈ ( 1) LATIN CAPITAL LETTER C WITH HOOK
    {0x0189, 0x018A, 0x0257}, // Ɖ ɖ ( 2) LATIN CAPITAL LETTER AFRICAN D
    {0x018B, 0x018B, 0x018C}, // Ƌ ƌ ( 1) LATIN CAPITAL LETTER D WITH TOPBAR
    {0x018E, 0x018E, 0x01DD}, // Ǝ ǝ ( 1) LATIN CAPITAL LETTER REVERSED E
    {0x018F, 0x018F, 0x0259}, // Ə ə ( 1) LATIN CAPITAL LETTER SCHWA
    {0x0190, 0x0190, 0x025B}, // Ɛ ɛ ( 1) LATIN CAPITAL LETTER OPEN E
    {0x0191, 0x0191, 0x0192}, // Ƒ ƒ ( 1) LATIN CAPITAL LETTER F WITH HOOK
    {0x0193, 0x0193, 0x0260}, // Ɠ ɠ ( 1) LATIN CAPITAL LETTER G WITH HOOK
    {0x0194, 0x0194, 0x0263}, // Ɣ ɣ ( 1) LATIN CAPITAL LETTER GAMMA
    {0x0196, 0x0196, 0x0269}, // Ɩ ɩ ( 1) LATIN CAPITAL LETTER IOTA
    {0x0197, 0x0197, 0x0268}, // Ɨ ɨ ( 1) LATIN CAPITAL LETTER I WITH STROKE
    {0x0198, 0x0198, 0x0199}, // Ƙ ƙ ( 1) LATIN CAPITAL LETTER K WITH HOOK
    {0x019C, 0x019C, 0x026F}, // Ɯ ɯ ( 1) LATIN CAPITAL LETTER TURNED M
    {0x019D, 0x019D, 0x0272}, // Ɲ ɲ ( 1) LATIN CAPITAL LETTER N WITH LEFT HOOK
    {0x019F, 0x019F, 0x0275}, // Ɵ ɵ ( 1) LATIN CAPITAL LETTER O WITH MIDDLE TILDE
    {0x01A0, 0x01A4, 0x01A5}, // Ơ ơ ( 3) LATIN CAPITAL LETTER O WITH HORN
    {0x01A6, 0x01A6, 0x0280}, // Ʀ ʀ ( 1) LATIN LETTER YR
    {0x01A7, 0x01A7, 0x01A8}, // Ƨ ƨ ( 1) LATIN CAPITAL LETTER TONE TWO
    {0x01A9, 0x01A9, 0x0283}, // Ʃ ʃ ( 1) LATIN CAPITAL LETTER ESH
    {0x01AC, 0x01AC, 0x01AD}, // Ƭ ƭ ( 1) LATIN CAPITAL LETTER T WITH HOOK
    {0x01AE, 0x01AE, 0x0288}, // Ʈ ʈ ( 1) LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
    {0x01AF, 0x01AF, 0x01B0}, // Ư ư ( 1) LATIN CAPITAL LETTER U WITH HORN
    {0x01B1, 0x01B2, 0x028B}, // Ʊ ʊ ( 2) LATIN CAPITAL LETTER UPSILON
    {0x01B3, 0x01B5, 0x01B6}, // Ƴ ƴ ( 2) LATIN CAPITAL LETTER Y WITH HOOK
    {0x01B7, 0x01B7, 0x0292}, // Ʒ ʒ ( 1) LATIN CAPITAL LETTER EZH
    {0x01B8, 0x01B8, 0x01B9}, // Ƹ ƹ ( 1) LATIN CAPITAL LETTER EZH REVERSED
    {0x01BC, 0x01BC, 0x01BD}, // Ƽ ƽ ( 1) LATIN CAPITAL LETTER TONE FIVE
    {0x01C4, 0x01C4, 0x01C6}, // Ǆ ǆ ( 1) LATIN CAPITAL LETTER DZ WITH CARON
    {0x01C5, 0x01C5, 0x01C6}, // ǅ ǆ ( 1) LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
    {0x01C7, 0x01C7, 0x01C9}, // Ǉ ǉ ( 1) LATIN CAPITAL LETTER LJ
    {0x01C8, 0x01C8, 0x01C9}, // ǈ ǉ ( 1) LATIN CAPITAL LETTER L WITH SMALL LETTER J
    {0x01CA, 0x01CA, 0x01CC}, // Ǌ ǌ ( 1) LATIN CAPITAL LETTER NJ
    {0x01CB, 0x01DB, 0x01DC}, // ǋ ǌ ( 9) LATIN CAPITAL LETTER N WITH SMALL LETTER J
    {0x01DE, 0x01EE, 0x01EF}, // Ǟ ǟ ( 9) LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
    {0x01F1, 0x01F1, 0x01F3}, // Ǳ ǳ ( 1) LATIN CAPITAL LETTER DZ
    {0x01F2, 0x01F4, 0x01F5}, // ǲ ǳ ( 2) LATIN CAPITAL LETTER D WITH SMALL LETTER Z
    {0x01F6, 0x01F6, 0x0195}, // Ƕ ƕ ( 1) LATIN CAPITAL LETTER HWAIR
    {0x01F7, 0x01F7, 0x01BF}, // Ƿ ƿ ( 1) LATIN CAPITAL LETTER WYNN
    {0x01F8, 0x021E, 0x021F}, // Ǹ ǹ (20) LATIN CAPITAL LETTER N WITH GRAVE
    {0x0220, 0x0220, 0x019E}, // Ƞ ƞ ( 1) LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
    {0x0222, 0x0232, 0x0233}, // Ȣ ȣ ( 9) LATIN CAPITAL LETTER OU
    {0x023A, 0x023A, 0x2C65}, // Ⱥ ⱥ ( 1) LATIN CAPITAL LETTER A WITH STROKE
    {0x023B, 0x023B, 0x023C}, // Ȼ ȼ ( 1) LATIN CAPITAL LETTER C WITH STROKE
    {0x023D, 0x023D, 0x019A}, // Ƚ ƚ ( 1) LATIN CAPITAL LETTER L WITH BAR
    {0x023E, 0x023E, 0x2C66}, // Ⱦ ⱦ ( 1) LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
    {0x0241, 0x0241, 0x0242}, // Ɂ ɂ ( 1) LATIN CAPITAL LETTER GLOTTAL STOP
    {0x0243, 0x0243, 0x0180}, // Ƀ ƀ ( 1) LATIN CAPITAL LETTER B WITH STROKE
    {0x0244, 0x0244, 0x0289}, // Ʉ ʉ ( 1) LATIN CAPITAL LETTER U BAR
    {0x0245, 0x0245, 0x028C}, // Ʌ ʌ ( 1) LATIN CAPITAL LETTER TURNED V
    {0x0246, 0x024E, 0x024F}, // Ɇ ɇ ( 5) LATIN CAPITAL LETTER E WITH STROKE
    {0x0345, 0x0345, 0x03B9}, // ͅ ι ( 1) COMBINING GREEK YPOGEGRAMMENI
    {0x0370, 0x0372, 0x0373}, // Ͱ ͱ ( 2) GREEK CAPITAL LETTER HETA
    {0x0376, 0x0376, 0x0377}, // Ͷ ͷ ( 1) GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA
    {0x037F, 0x037F, 0x03F3}, // Ϳ ϳ ( 1) GREEK CAPITAL LETTER YOT
    {0x0386, 0x0386, 0x03AC}, // Ά ά ( 1) GREEK CAPITAL LETTER ALPHA WITH TONOS
    {0x0388, 0x038A, 0x03AF}, // Έ έ ( 3) GREEK CAPITAL LETTER EPSILON WITH TONOS
    {0x038C, 0x038C, 0x03CC}, // Ό ό ( 1) GREEK CAPITAL LETTER OMICRON WITH TONOS
    {0x038E, 0x038F, 0x03CE}, // Ύ ύ ( 2) GREEK CAPITAL LETTER UPSILON WITH TONOS
    {0x0391, 0x03A1, 0x03C1}, // Α α (17) GREEK CAPITAL LETTER ALPHA
    {0x03A3, 0x03AB, 0x03CB}, // Σ σ ( 9) GREEK CAPITAL LETTER SIGMA
    {0x03C2, 0x03C2, 0x03C3}, // ς σ ( 1) GREEK SMALL LETTER FINAL SIGMA
    {0x03CF, 0x03CF, 0x03D7}, // Ϗ ϗ ( 1) GREEK CAPITAL KAI SYMBOL
    {0x03D0, 0x03D0, 0x03B2}, // ϐ β ( 1) GREEK BETA SYMBOL
    {0x03D1, 0x03D1, 0x03B8}, // ϑ θ ( 1) GREEK THETA SYMBOL
    {0x03D5, 0x03D5, 0x03C6}, // ϕ φ ( 1) GREEK PHI SYMBOL
    {0x03D6, 0x03D6, 0x03C0}, // ϖ π ( 1) GREEK PI SYMBOL
    {0x03D8, 0x03EE, 0x03EF}, // Ϙ ϙ (12) GREEK LETTER ARCHAIC KOPPA
    {0x03F0, 0x03F0, 0x03BA}, // ϰ κ ( 1) GREEK KAPPA SYMBOL
    {0x03F1, 0x03F1, 0x03C1}, // ϱ ρ ( 1) GREEK RHO SYMBOL
    {0x03F4, 0x03F4, 0x03B8}, // ϴ θ ( 1) GREEK CAPITAL THETA SYMBOL
    {0x03F5, 0x03F5, 0x03B5}, // ϵ ε ( 1) GREEK LUNATE EPSILON SYMBOL
    {0x03F7, 0x03F7, 0x03F8}, // Ϸ ϸ ( 1) GREEK CAPITAL LETTER SHO
    {0x03F9, 0x03F9, 0x03F2}, // Ϲ ϲ ( 1) GREEK CAPITAL LUNATE SIGMA SYMBOL
    {0x03FA, 0x03FA, 0x03FB}, // Ϻ ϻ ( 1) GREEK CAPITAL LETTER SAN
    {0x03FD, 0x03FF, 0x037D}, // Ͻ ͻ ( 3) GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL
    {0x0400, 0x040F, 0x045F}, // Ѐ ѐ (16) CYRILLIC CAPITAL LETTER IE WITH GRAVE
    {0x0410, 0x042F, 0x044F}, // А а (32) CYRILLIC CAPITAL LETTER A
    {0x0460, 0x0480, 0x0481}, // Ѡ ѡ (17) CYRILLIC CAPITAL LETTER OMEGA
    {0x048A, 0x04BE, 0x04BF}, // Ҋ ҋ (27) CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
    {0x04C0, 0x04C0, 0x04CF}, // Ӏ ӏ ( 1) CYRILLIC LETTER PALOCHKA
    {0x04C1, 0x04CD, 0x04CE}, // Ӂ ӂ ( 7) CYRILLIC CAPITAL LETTER ZHE WITH BREVE
    {0x04D0, 0x052E, 0x052F}, // Ӑ ӑ (48) CYRILLIC CAPITAL LETTER A WITH BREVE
    {0x0531, 0x0556, 0x0586}, // Ա ա (38) ARMENIAN CAPITAL LETTER AYB
    {0x10A0, 0x10C5, 0x2D25}, // Ⴀ ⴀ (38) GEORGIAN CAPITAL LETTER AN
    {0x10C7, 0x10C7, 0x2D27}, // Ⴧ ⴧ ( 1) GEORGIAN CAPITAL LETTER YN
    {0x10CD, 0x10CD, 0x2D2D}, // Ⴭ ⴭ ( 1) GEORGIAN CAPITAL LETTER AEN
    {0x13F8, 0x13FD, 0x13F5}, // ᏸ Ᏸ ( 6) CHEROKEE SMALL LETTER YE
    {0x1C80, 0x1C80, 0x0432}, // ᲀ в ( 1) CYRILLIC SMALL LETTER ROUNDED VE
    {0x1C81, 0x1C81, 0x0434}, // ᲁ д ( 1) CYRILLIC SMALL LETTER LONG-LEGGED DE
    {0x1C82, 0x1C82, 0x043E}, // ᲂ о ( 1) CYRILLIC SMALL LETTER NARROW O
    {0x1C83, 0x1C84, 0x0442}, // ᲃ с ( 2) CYRILLIC SMALL LETTER WIDE ES
    {0x1C85, 0x1C85, 0x0442}, // ᲅ т ( 1) CYRILLIC SMALL LETTER THREE-LEGGED TE
    {0x1C86, 0x1C86, 0x044A}, // ᲆ ъ ( 1) CYRILLIC SMALL LETTER TALL HARD SIGN
    {0x1C87, 0x1C87, 0x0463}, // ᲇ ѣ ( 1) CYRILLIC SMALL LETTER TALL YAT
    {0x1C88, 0x1C88, 0xA64B}, // ᲈ ꙋ ( 1) CYRILLIC SMALL LETTER UNBLENDED UK
    {0x1C90, 0x1CBA, 0x10FA}, // Ა ა (43) GEORGIAN MTAVRULI CAPITAL LETTER AN
    {0x1CBD, 0x1CBF, 0x10FF}, // Ჽ ჽ ( 3) GEORGIAN MTAVRULI CAPITAL LETTER AEN
    {0x1E00, 0x1E94, 0x1E95}, // Ḁ ḁ (75) LATIN CAPITAL LETTER A WITH RING BELOW
    {0x1E9B, 0x1E9B, 0x1E61}, // ẛ ṡ ( 1) LATIN SMALL LETTER LONG S WITH DOT ABOVE
    {0x1E9E, 0x1E9E, 0x00DF}, // ẞ ß ( 1) LATIN CAPITAL LETTER SHARP S
    {0x1EA0, 0x1EFE, 0x1EFF}, // Ạ ạ (48) LATIN CAPITAL LETTER A WITH DOT BELOW
    {0x1F08, 0x1F0F, 0x1F07}, // Ἀ ἀ ( 8) GREEK CAPITAL LETTER ALPHA WITH PSILI
    {0x1F18, 0x1F1D, 0x1F15}, // Ἐ ἐ ( 6) GREEK CAPITAL LETTER EPSILON WITH PSILI
    {0x1F28, 0x1F2F, 0x1F27}, // Ἠ ἠ ( 8) GREEK CAPITAL LETTER ETA WITH PSILI
    {0x1F38, 0x1F3F, 0x1F37}, // Ἰ ἰ ( 8) GREEK CAPITAL LETTER IOTA WITH PSILI
    {0x1F48, 0x1F4D, 0x1F45}, // Ὀ ὀ ( 6) GREEK CAPITAL LETTER OMICRON WITH PSILI
    {0x1F59, 0x1F5F, 0x1F57}, // Ὑ ὑ ( 7) GREEK CAPITAL LETTER UPSILON WITH DASIA
    {0x1F68, 0x1F6F, 0x1F67}, // Ὠ ὠ ( 8) GREEK CAPITAL LETTER OMEGA WITH PSILI
    {0x1F88, 0x1F8F, 0x1F87}, // ᾈ ᾀ ( 8) GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
    {0x1F98, 0x1F9F, 0x1F97}, // ᾘ ᾐ ( 8) GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
    {0x1FA8, 0x1FAF, 0x1FA7}, // ᾨ ᾠ ( 8) GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
    {0x1FB8, 0x1FB9, 0x1FB1}, // Ᾰ ᾰ ( 2) GREEK CAPITAL LETTER ALPHA WITH VRACHY
    {0x1FBA, 0x1FBB, 0x1F71}, // Ὰ ὰ ( 2) GREEK CAPITAL LETTER ALPHA WITH VARIA
    {0x1FBC, 0x1FBC, 0x1FB3}, // ᾼ ᾳ ( 1) GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
    {0x1FBE, 0x1FBE, 0x03B9}, // ι ι ( 1) GREEK PROSGEGRAMMENI
    {0x1FC8, 0x1FCB, 0x1F75}, // Ὲ ὲ ( 4) GREEK CAPITAL LETTER EPSILON WITH VARIA
    {0x1FCC, 0x1FCC, 0x1FC3}, // ῌ ῃ ( 1) GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
    {0x1FD8, 0x1FD9, 0x1FD1}, // Ῐ ῐ ( 2) GREEK CAPITAL LETTER IOTA WITH VRACHY
    {0x1FDA, 0x1FDB, 0x1F77}, // Ὶ ὶ ( 2) GREEK CAPITAL LETTER IOTA WITH VARIA
    {0x1FE8, 0x1FE9, 0x1FE1}, // Ῠ ῠ ( 2) GREEK CAPITAL LETTER UPSILON WITH VRACHY
    {0x1FEA, 0x1FEB, 0x1F7B}, // Ὺ ὺ ( 2) GREEK CAPITAL LETTER UPSILON WITH VARIA
    {0x1FEC, 0x1FEC, 0x1FE5}, // Ῥ ῥ ( 1) GREEK CAPITAL LETTER RHO WITH DASIA
    {0x1FF8, 0x1FF9, 0x1F79}, // Ὸ ὸ ( 2) GREEK CAPITAL LETTER OMICRON WITH VARIA
    {0x1FFA, 0x1FFB, 0x1F7D}, // Ὼ ὼ ( 2) GREEK CAPITAL LETTER OMEGA WITH VARIA
    {0x1FFC, 0x1FFC, 0x1FF3}, // ῼ ῳ ( 1) GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
    {0x2126, 0x2126, 0x03C9}, // Ω ω ( 1) OHM SIGN
    {0x212A, 0x212A, 0x006B}, // K k ( 1) KELVIN SIGN
    {0x212B, 0x212B, 0x00E5}, // Å å ( 1) ANGSTROM SIGN
    {0x2132, 0x2132, 0x214E}, // Ⅎ ⅎ ( 1) TURNED CAPITAL F
    {0x2160, 0x216F, 0x217F}, // Ⅰ ⅰ (16) ROMAN NUMERAL ONE
    {0x2183, 0x2183, 0x2184}, // Ↄ ↄ ( 1) ROMAN NUMERAL REVERSED ONE HUNDRED
    {0x24B6, 0x24CF, 0x24E9}, // Ⓐ ⓐ (26) CIRCLED LATIN CAPITAL LETTER A
    {0x2C00, 0x2C2F, 0x2C5F}, // Ⰰ ⰰ (48) GLAGOLITIC CAPITAL LETTER AZU
    {0x2C60, 0x2C60, 0x2C61}, // Ⱡ ⱡ ( 1) LATIN CAPITAL LETTER L WITH DOUBLE BAR
    {0x2C62, 0x2C62, 0x026B}, // Ɫ ɫ ( 1) LATIN CAPITAL LETTER L WITH MIDDLE TILDE
    {0x2C63, 0x2C63, 0x1D7D}, // Ᵽ ᵽ ( 1) LATIN CAPITAL LETTER P WITH STROKE
    {0x2C64, 0x2C64, 0x027D}, // Ɽ ɽ ( 1) LATIN CAPITAL LETTER R WITH TAIL
    {0x2C67, 0x2C6B, 0x2C6C}, // Ⱨ ⱨ ( 3) LATIN CAPITAL LETTER H WITH DESCENDER
    {0x2C6D, 0x2C6D, 0x0251}, // Ɑ ɑ ( 1) LATIN CAPITAL LETTER ALPHA
    {0x2C6E, 0x2C6E, 0x0271}, // Ɱ ɱ ( 1) LATIN CAPITAL LETTER M WITH HOOK
    {0x2C6F, 0x2C6F, 0x0250}, // Ɐ ɐ ( 1) LATIN CAPITAL LETTER TURNED A
    {0x2C70, 0x2C70, 0x0252}, // Ɒ ɒ ( 1) LATIN CAPITAL LETTER TURNED ALPHA
    {0x2C72, 0x2C72, 0x2C73}, // Ⱳ ⱳ ( 1) LATIN CAPITAL LETTER W WITH HOOK
    {0x2C75, 0x2C75, 0x2C76}, // Ⱶ ⱶ ( 1) LATIN CAPITAL LETTER HALF H
    {0x2C7E, 0x2C7F, 0x0240}, // Ȿ ȿ ( 2) LATIN CAPITAL LETTER S WITH SWASH TAIL
    {0x2C80, 0x2CE2, 0x2CE3}, // Ⲁ ⲁ (50) COPTIC CAPITAL LETTER ALFA
    {0x2CEB, 0x2CED, 0x2CEE}, // Ⳬ ⳬ ( 2) COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI
    {0x2CF2, 0x2CF2, 0x2CF3}, // Ⳳ ⳳ ( 1) COPTIC CAPITAL LETTER BOHAIRIC KHEI
    {0xA640, 0xA66C, 0xA66D}, // Ꙁ ꙁ (23) CYRILLIC CAPITAL LETTER ZEMLYA
    {0xA680, 0xA69A, 0xA69B}, // Ꚁ ꚁ (14) CYRILLIC CAPITAL LETTER DWE
    {0xA722, 0xA72E, 0xA72F}, // Ꜣ ꜣ ( 7) LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF
    {0xA732, 0xA76E, 0xA76F}, // Ꜳ ꜳ (31) LATIN CAPITAL LETTER AA
    {0xA779, 0xA77B, 0xA77C}, // Ꝺ ꝺ ( 2) LATIN CAPITAL LETTER INSULAR D
    {0xA77D, 0xA77D, 0x1D79}, // Ᵹ ᵹ ( 1) LATIN CAPITAL LETTER INSULAR G
    {0xA77E, 0xA786, 0xA787}, // Ꝿ ꝿ ( 5) LATIN CAPITAL LETTER TURNED INSULAR G
    {0xA78B, 0xA78B, 0xA78C}, // Ꞌ ꞌ ( 1) LATIN CAPITAL LETTER SALTILLO
    {0xA78D, 0xA78D, 0x0265}, // Ɥ ɥ ( 1) LATIN CAPITAL LETTER TURNED H
    {0xA790, 0xA792, 0xA793}, // Ꞑ ꞑ ( 2) LATIN CAPITAL LETTER N WITH DESCENDER
    {0xA796, 0xA7A8, 0xA7A9}, // Ꞗ ꞗ (10) LATIN CAPITAL LETTER B WITH FLOURISH
    {0xA7AA, 0xA7AA, 0x0266}, // Ɦ ɦ ( 1) LATIN CAPITAL LETTER H WITH HOOK
    {0xA7AB, 0xA7AB, 0x025C}, // Ɜ ɜ ( 1) LATIN CAPITAL LETTER REVERSED OPEN E
    {0xA7AC, 0xA7AC, 0x0261}, // Ɡ ɡ ( 1) LATIN CAPITAL LETTER SCRIPT G
    {0xA7AD, 0xA7AD, 0x026C}, // Ɬ ɬ ( 1) LATIN CAPITAL LETTER L WITH BELT
    {0xA7AE, 0xA7AE, 0x026A}, // Ɪ ɪ ( 1) LATIN CAPITAL LETTER SMALL CAPITAL I
    {0xA7B0, 0xA7B0, 0x029E}, // Ʞ ʞ ( 1) LATIN CAPITAL LETTER TURNED K
    {0xA7B1, 0xA7B1, 0x0287}, // Ʇ ʇ ( 1) LATIN CAPITAL LETTER TURNED T
    {0xA7B2, 0xA7B2, 0x029D}, // Ʝ ʝ ( 1) LATIN CAPITAL LETTER J WITH CROSSED-TAIL
    {0xA7B3, 0xA7B3, 0xAB53}, // Ꭓ ꭓ ( 1) LATIN CAPITAL LETTER CHI
    {0xA7B4, 0xA7C2, 0xA7C3}, // Ꞵ ꞵ ( 8) LATIN CAPITAL LETTER BETA
    {0xA7C4, 0xA7C4, 0xA794}, // Ꞔ ꞔ ( 1) LATIN CAPITAL LETTER C WITH PALATAL HOOK
    {0xA7C5, 0xA7C5, 0x0282}, // Ʂ ʂ ( 1) LATIN CAPITAL LETTER S WITH HOOK
    {0xA7C6, 0xA7C6, 0x1D8E}, // Ᶎ ᶎ ( 1) LATIN CAPITAL LETTER Z WITH PALATAL HOOK
    {0xA7C7, 0xA7C9, 0xA7CA}, // Ꟈ ꟈ ( 2) LATIN CAPITAL LETTER D WITH SHORT STROKE OVERLAY
    {0xA7D0, 0xA7D0, 0xA7D1}, // Ꟑ ꟑ ( 1) LATIN CAPITAL LETTER CLOSED INSULAR G
    {0xA7D6, 0xA7D8, 0xA7D9}, // Ꟗ ꟗ ( 2) LATIN CAPITAL LETTER MIDDLE SCOTS S
    {0xA7F5, 0xA7F5, 0xA7F6}, // Ꟶ ꟶ ( 1) LATIN CAPITAL LETTER REVERSED HALF H
    {0xAB70, 0xABBF, 0x13EF}, // ꭰ Ꭰ (80) CHEROKEE SMALL LETTER A
    {0xFF21, 0xFF3A, 0xFF5A}, // Ａ ａ (26) FULLWIDTH LATIN CAPITAL LETTER A
    {0x0130, 0x0130, 0x0069}, // İ i ( 1) LATIN CAPITAL LETTER I WITH DOT ABOVE
    {0x01CD, 0x01DB, 0x01DC}, // Ǎ ǎ ( 8) LATIN CAPITAL LETTER A WITH CARON
    {0x01F4, 0x01F4, 0x01F5}, // Ǵ ǵ ( 1) LATIN CAPITAL LETTER G WITH ACUTE
    {0x13A0, 0x13EF, 0xABBF}, // Ꭰ ꭰ (80) CHEROKEE LETTER A
    {0x13F0, 0x13F5, 0x13FD}, // Ᏸ ᏸ ( 6) CHEROKEE LETTER YE
    {0x039C, 0x039C, 0x00B5}, // Μ µ ( 1) 
    {0x0049, 0x0049, 0x0131}, // I ı ( 1) 
    {0x0053, 0x0053, 0x017F}, // S ſ ( 1) 
    {0x03A3, 0x03A3, 0x03C2}, // Σ ς ( 1) 
    {0x0392, 0x0392, 0x03D0}, // Β ϐ ( 1) 
    {0x0398, 0x0398, 0x03D1}, // Θ ϑ ( 1) 
    {0x03A6, 0x03A6, 0x03D5}, // Φ ϕ ( 1) 
    {0x03A0, 0x03A0, 0x03D6}, // Π ϖ ( 1) 
    {0x039A, 0x039A, 0x03F0}, // Κ ϰ ( 1) 
    {0x03A1, 0x03A1, 0x03F1}, // Ρ ϱ ( 1) 
    {0x0395, 0x0395, 0x03F5}, // Ε ϵ ( 1) 
    {0x0412, 0x0412, 0x1C80}, // В ᲀ ( 1) 
    {0x0414, 0x0414, 0x1C81}, // Д ᲁ ( 1) 
    {0x041E, 0x041E, 0x1C82}, // О ᲂ ( 1) 
    {0x0421, 0x0422, 0x1C84}, // С ᲃ ( 2) 
    {0x0422, 0x0422, 0x1C85}, // Т ᲅ ( 1) 
    {0x042A, 0x042A, 0x1C86}, // Ъ ᲆ ( 1) 
    {0x0462, 0x0462, 0x1C87}, // Ѣ ᲇ ( 1) 
    {0xA64A, 0xA64A, 0x1C88}, // Ꙋ ᲈ ( 1) 
    {0x1E60, 0x1E60, 0x1E9B}, // Ṡ ẛ ( 1) 
    {0x0399, 0x0399, 0x1FBE}, // Ι ι ( 1) 
}; // 218

enum { casefold_len = 192 };

static uint8_t upcase_ind[162] = {
    0, 2, 3, 4, 192, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 43, 45, 193, 47, 48, 194, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
    63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 75, 80, 83, 85, 86, 87, 88, 89, 90, 91,
    92, 93, 94, 95, 96, 97, 98, 99, 195, 196, 109, 110, 111, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 125, 126, 129, 131, 132, 133, 134, 135, 136, 137, 139, 140, 141, 142, 144, 146, 147, 148,
    149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
    169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188,
    189, 191,
};

static uint8_t lowcase_ind[184] = {
    0, 197, 113, 2, 3, 8, 4, 198, 5, 6, 7, 9, 199, 60, 12, 14, 16, 20, 50, 25,
    57, 53, 29, 31, 33, 35, 37, 39, 40, 51, 41, 43, 45, 193, 17, 47, 48, 194, 52, 54,
    56, 158, 59, 63, 154, 152, 155, 11, 13, 15, 18, 19, 174, 21, 175, 22, 170, 173, 24, 23,
    177, 148, 176, 26, 153, 27, 28, 150, 30, 184, 32, 179, 34, 61, 36, 62, 38, 180, 178, 65,
    66, 88, 68, 69, 72, 200, 73, 70, 71, 201, 202, 203, 204, 75, 80, 205, 206, 86, 67, 207,
    85, 87, 90, 89, 91, 92, 94, 93, 95, 96, 109, 110, 196, 208, 209, 210, 211, 212, 213, 214,
    215, 167, 149, 185, 111, 216, 114, 115, 116, 117, 118, 119, 120, 121, 126, 129, 132, 136, 134, 137,
    122, 123, 124, 125, 127, 217, 130, 131, 133, 135, 138, 142, 144, 146, 147, 55, 58, 151, 156, 157,
    159, 160, 161, 97, 98, 99, 162, 163, 164, 165, 166, 168, 169, 171, 183, 172, 182, 186, 187, 188,
    189, 181, 195, 191,
};
// ### END_FILE_INCLUDE: utf8tabs.inc

const uint8_t utf8_dtab[] = {
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12,
};

int utf8_encode(char *out, uint32_t c)
{
    if (c < 0x80U) {
        out[0] = (char) c;
        return 1;
    } else if (c < 0x0800U) {
        out[0] = (char) ((c>>6  & 0x1F) | 0xC0);
        out[1] = (char) ((c     & 0x3F) | 0x80);
        return 2;
    } else if (c < 0x010000U) {
        if ((c < 0xD800U) | (c >= 0xE000U)) {
            out[0] = (char) ((c>>12 & 0x0F) | 0xE0);
            out[1] = (char) ((c>>6  & 0x3F) | 0x80);
            out[2] = (char) ((c     & 0x3F) | 0x80);
            return 3;
        }
    } else if (c < 0x110000U) {
        out[0] = (char) ((c>>18 & 0x07) | 0xF0);
        out[1] = (char) ((c>>12 & 0x3F) | 0x80);
        out[2] = (char) ((c>>6  & 0x3F) | 0x80);
        out[3] = (char) ((c     & 0x3F) | 0x80);
        return 4;
    }
    return 0;
}

uint32_t utf8_peek_off(const char* s, int pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos) pos += (*(s += inc) & 0xC0) != 0x80;
    return utf8_peek(s);
}

bool utf8_valid_n(const char* s, intptr_t nbytes) {
    utf8_decode_t d = {.state=0};
    while ((nbytes-- != 0) & (*s != 0))
        utf8_decode(&d, (uint8_t)*s++);
    return d.state == 0;
}

uint32_t utf8_casefold(uint32_t c) {
    for (int i=0; i < casefold_len; ++i) {
        const struct CaseMapping entry = casemappings[i];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1) == (c & 1));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

uint32_t utf8_tolower(uint32_t c) {
    for (int i=0; i < (int)(sizeof upcase_ind/sizeof *upcase_ind); ++i) {
        const struct CaseMapping entry = casemappings[upcase_ind[i]];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1) == (c & 1));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

uint32_t utf8_toupper(uint32_t c) {
    for (int i=0; i < (int)(sizeof lowcase_ind/sizeof *lowcase_ind); ++i) {
        const struct CaseMapping entry = casemappings[lowcase_ind[i]];
        if (c <= entry.m2) {
            int d = entry.m2 - entry.c2;
            if (c < (uint32_t)(entry.c1 + d)) return c;
            if (d == 1) return c - ((entry.m2 & 1) == (c & 1));
            return (uint32_t)((int)c - d);
        }
    }
    return c;
}

int utf8_icmp_sv(const csview s1, const csview s2) {
    utf8_decode_t d1 = {.state=0}, d2 = {.state=0};
    intptr_t j1 = 0, j2 = 0;
    while ((j1 < s1.size) & (j2 < s2.size)) {
        do { utf8_decode(&d1, (uint8_t)s1.buf[j1++]); } while (d1.state);
        do { utf8_decode(&d2, (uint8_t)s2.buf[j2++]); } while (d2.state);
        int32_t c = (int32_t)utf8_casefold(d1.codep) - (int32_t)utf8_casefold(d2.codep);
        if (c || !s2.buf[j2 - 1]) // OK if s1.size and s2.size are npos
            return (int)c;
    }
    return (int)(s1.size - s2.size);
}

typedef struct {
  uint16_t lo;
  uint16_t hi;
} URange16;

typedef struct {
  const URange16 *r16;
  int nr16;
} UGroup;

#ifndef __cplusplus
static
#else
extern
#endif
const UGroup _utf8_unicode_groups[U8G_SIZE];

bool utf8_isgroup(int group, uint32_t c) {
    for (int j=0; j<_utf8_unicode_groups[group].nr16; ++j) {
        if (c < _utf8_unicode_groups[group].r16[j].lo)
            return false;
        if (c <= _utf8_unicode_groups[group].r16[j].hi)
            return true;
    }
    return false;
}

bool utf8_isalpha(uint32_t c) {
    static int16_t groups[] = {U8G_Latin, U8G_Nl, U8G_Greek, U8G_Cyrillic,
                               U8G_Han, U8G_Devanagari, U8G_Arabic};
    if (c < 128) return isalpha((int)c) != 0;
    for (int j=0; j < c_arraylen(groups); ++j)
        if (utf8_isgroup(groups[j], c))
            return true;
    return false;
}

bool utf8_iscased(uint32_t c) {
    if (c < 128) return isalpha((int)c) != 0;
    return utf8_islower(c) || utf8_isupper(c) || 
           utf8_isgroup(U8G_Lt, c);
}

bool utf8_isword(uint32_t c) {
    if (c < 128) return (isalnum((int)c) != 0) | (c == '_');
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c) ||
           utf8_isgroup(U8G_Pc, c);
}

/* The tables below are extracted from the RE2 library */

static const URange16 Cc_range16[] = { // Control
    { 0, 31 },
    { 127, 159 },
};

static const URange16 Lt_range16[] = { // Title case
    { 453, 453 },
    { 456, 456 },
    { 459, 459 },
    { 498, 498 },
    { 8072, 8079 },
    { 8088, 8095 },
    { 8104, 8111 },
    { 8124, 8124 },
    { 8140, 8140 },
    { 8188, 8188 },
};

static const URange16 Nd_range16[] = { // Decimal number
    { 48, 57 },
    { 1632, 1641 },
    { 1776, 1785 },
    { 1984, 1993 },
    { 2406, 2415 },
    { 2534, 2543 },
    { 2662, 2671 },
    { 2790, 2799 },
    { 2918, 2927 },
    { 3046, 3055 },
    { 3174, 3183 },
    { 3302, 3311 },
    { 3430, 3439 },
    { 3558, 3567 },
    { 3664, 3673 },
    { 3792, 3801 },
    { 3872, 3881 },
    { 4160, 4169 },
    { 4240, 4249 },
    { 6112, 6121 },
    { 6160, 6169 },
    { 6470, 6479 },
    { 6608, 6617 },
    { 6784, 6793 },
    { 6800, 6809 },
    { 6992, 7001 },
    { 7088, 7097 },
    { 7232, 7241 },
    { 7248, 7257 },
    { 42528, 42537 },
    { 43216, 43225 },
    { 43264, 43273 },
    { 43472, 43481 },
    { 43504, 43513 },
    { 43600, 43609 },
    { 44016, 44025 },
    { 65296, 65305 },
};

static const URange16 Nl_range16[] = { // Number letter
    { 5870, 5872 },
    { 8544, 8578 },
    { 8581, 8584 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12346 },
    { 42726, 42735 },
};

static const URange16 Pc_range16[] = { // Connector punctuation
    { 95, 95 },
    { 8255, 8256 },
    { 8276, 8276 },
    { 65075, 65076 },
    { 65101, 65103 },
    { 65343, 65343 },
};

static const URange16 Pd_range16[] = { // Dash punctuation
    { 45, 45 },
    { 1418, 1418 },
    { 1470, 1470 },
    { 5120, 5120 },
    { 6150, 6150 },
    { 8208, 8213 },
    { 11799, 11799 },
    { 11802, 11802 },
    { 11834, 11835 },
    { 11840, 11840 },
    { 11869, 11869 },
    { 12316, 12316 },
    { 12336, 12336 },
    { 12448, 12448 },
    { 65073, 65074 },
    { 65112, 65112 },
    { 65123, 65123 },
    { 65293, 65293 },
};

static const URange16 Pf_range16[] = { // Final punctuation
    { 187, 187 },
    { 8217, 8217 },
    { 8221, 8221 },
    { 8250, 8250 },
    { 11779, 11779 },
    { 11781, 11781 },
    { 11786, 11786 },
    { 11789, 11789 },
    { 11805, 11805 },
    { 11809, 11809 },
};

static const URange16 Pi_range16[] = { // Initial punctuation
    { 171, 171 },
    { 8216, 8216 },
    { 8219, 8220 },
    { 8223, 8223 },
    { 8249, 8249 },
    { 11778, 11778 },
    { 11780, 11780 },
    { 11785, 11785 },
    { 11788, 11788 },
    { 11804, 11804 },
    { 11808, 11808 },
};

static const URange16 Sc_range16[] = { // Currency symbol
    { 36, 36 },
    { 162, 165 },
    { 1423, 1423 },
    { 1547, 1547 },
    { 2046, 2047 },
    { 2546, 2547 },
    { 2555, 2555 },
    { 2801, 2801 },
    { 3065, 3065 },
    { 3647, 3647 },
    { 6107, 6107 },
    { 8352, 8384 },
    { 43064, 43064 },
    { 65020, 65020 },
    { 65129, 65129 },
    { 65284, 65284 },
    { 65504, 65505 },
    { 65509, 65510 },
};

static const URange16 Zl_range16[] = { // Line separator
    { 8232, 8232 },
};

static const URange16 Zp_range16[] = { // Paragraph separator
    { 8233, 8233 },
};

static const URange16 Zs_range16[] = { // Space separator
    { 32, 32 },
    { 160, 160 },
    { 5760, 5760 },
    { 8192, 8202 },
    { 8239, 8239 },
    { 8287, 8287 },
    { 12288, 12288 },
};

static const URange16 Arabic_range16[] = {
    { 1536, 1540 },
    { 1542, 1547 },
    { 1549, 1562 },
    { 1564, 1566 },
    { 1568, 1599 },
    { 1601, 1610 },
    { 1622, 1647 },
    { 1649, 1756 },
    { 1758, 1791 },
    { 1872, 1919 },
    { 2160, 2190 },
    { 2192, 2193 },
    { 2200, 2273 },
    { 2275, 2303 },
    { 64336, 64450 },
    { 64467, 64829 },
    { 64832, 64911 },
    { 64914, 64967 },
    { 64975, 64975 },
    { 65008, 65023 },
    { 65136, 65140 },
    { 65142, 65276 },
};

static const URange16 Cyrillic_range16[] = {
    { 1024, 1156 },
    { 1159, 1327 },
    { 7296, 7304 },
    { 7467, 7467 },
    { 7544, 7544 },
    { 11744, 11775 },
    { 42560, 42655 },
    { 65070, 65071 },
};

static const URange16 Devanagari_range16[] = {
    { 2304, 2384 },
    { 2389, 2403 },
    { 2406, 2431 },
    { 43232, 43263 },
};

static const URange16 Greek_range16[] = {
    { 880, 883 },
    { 885, 887 },
    { 890, 893 },
    { 895, 895 },
    { 900, 900 },
    { 902, 902 },
    { 904, 906 },
    { 908, 908 },
    { 910, 929 },
    { 931, 993 },
    { 1008, 1023 },
    { 7462, 7466 },
    { 7517, 7521 },
    { 7526, 7530 },
    { 7615, 7615 },
    { 7936, 7957 },
    { 7960, 7965 },
    { 7968, 8005 },
    { 8008, 8013 },
    { 8016, 8023 },
    { 8025, 8025 },
    { 8027, 8027 },
    { 8029, 8029 },
    { 8031, 8061 },
    { 8064, 8116 },
    { 8118, 8132 },
    { 8134, 8147 },
    { 8150, 8155 },
    { 8157, 8175 },
    { 8178, 8180 },
    { 8182, 8190 },
    { 8486, 8486 },
    { 43877, 43877 },
};

static const URange16 Han_range16[] = {
    { 11904, 11929 },
    { 11931, 12019 },
    { 12032, 12245 },
    { 12293, 12293 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12347 },
    { 13312, 19903 },
    { 19968, 40959 },
    { 63744, 64109 },
    { 64112, 64217 },
};

static const URange16 Latin_range16[] = {
    { 65, 90 },
    { 97, 122 },
    { 170, 170 },
    { 186, 186 },
    { 192, 214 },
    { 216, 246 },
    { 248, 696 },
    { 736, 740 },
    { 7424, 7461 },
    { 7468, 7516 },
    { 7522, 7525 },
    { 7531, 7543 },
    { 7545, 7614 },
    { 7680, 7935 },
    { 8305, 8305 },
    { 8319, 8319 },
    { 8336, 8348 },
    { 8490, 8491 },
    { 8498, 8498 },
    { 8526, 8526 },
    { 8544, 8584 },
    { 11360, 11391 },
    { 42786, 42887 },
    { 42891, 42954 },
    { 42960, 42961 },
    { 42963, 42963 },
    { 42965, 42969 },
    { 42994, 43007 },
    { 43824, 43866 },
    { 43868, 43876 },
    { 43878, 43881 },
    { 64256, 64262 },
    { 65313, 65338 },
    { 65345, 65370 },
};

#define UNI_ENTRY(Code) \
    { Code##_range16, sizeof(Code##_range16)/sizeof(URange16) }
#ifdef __cplusplus
#define _e_arg(k, v) v
#else
#define _e_arg(k, v) [k] = v
static
#endif
const UGroup _utf8_unicode_groups[U8G_SIZE] = {
    _e_arg(U8G_Cc, UNI_ENTRY(Cc)),
    _e_arg(U8G_Lt, UNI_ENTRY(Lt)),
    _e_arg(U8G_Nd, UNI_ENTRY(Nd)),
    _e_arg(U8G_Nl, UNI_ENTRY(Nl)),
    _e_arg(U8G_Pc, UNI_ENTRY(Pc)),
    _e_arg(U8G_Pd, UNI_ENTRY(Pd)),
    _e_arg(U8G_Pf, UNI_ENTRY(Pf)),
    _e_arg(U8G_Pi, UNI_ENTRY(Pi)),
    _e_arg(U8G_Sc, UNI_ENTRY(Sc)),
    _e_arg(U8G_Zl, UNI_ENTRY(Zl)),
    _e_arg(U8G_Zp, UNI_ENTRY(Zp)),
    _e_arg(U8G_Zs, UNI_ENTRY(Zs)),
    _e_arg(U8G_Arabic, UNI_ENTRY(Arabic)),
    _e_arg(U8G_Cyrillic, UNI_ENTRY(Cyrillic)),
    _e_arg(U8G_Devanagari, UNI_ENTRY(Devanagari)),
    _e_arg(U8G_Greek, UNI_ENTRY(Greek)),
    _e_arg(U8G_Han, UNI_ENTRY(Han)),
    _e_arg(U8G_Latin, UNI_ENTRY(Latin)),
};

#endif
// ### END_FILE_INCLUDE: utf8code.c
#endif
#ifndef _i_inc_utf8
#undef i_static
#undef i_header
#undef i_implement
#undef i_import
#undef i_opt
#endif
#undef _i_inc_utf8
// ### END_FILE_INCLUDE: utf8.h

#ifndef CRAWSTR_H_INCLUDED
#define CRAWSTR_H_INCLUDED

#define             crawstr_init() c_rs("")
#define             crawstr_clone(rs) c_default_clone(rs)
#define             crawstr_drop(self) c_default_drop(self)
#define             crawstr_toraw(self) (self)->str

STC_INLINE crawstr  crawstr_from(const char* str)
                        { return c_rs_2(str, c_strlen(str)); }
STC_INLINE void     crawstr_clear(crawstr* self) { *self = c_rs(""); }
STC_INLINE csview   crawstr_sv(crawstr rs) { return c_sv_2(rs.str, rs.size); }

STC_INLINE intptr_t crawstr_size(crawstr rs) { return rs.size; }
STC_INLINE bool     crawstr_empty(crawstr rs) { return rs.size == 0; }

STC_INLINE bool crawstr_equals(crawstr rs, const char* str) { 
    intptr_t n = c_strlen(str); 
    return rs.size == n && !c_memcmp(rs.str, str, n);
}

STC_INLINE intptr_t crawstr_find(crawstr rs, const char* search) {
    char* res = strstr(rs.str, search);
    return res ? (res - rs.str) : c_NPOS;
}

STC_INLINE bool crawstr_contains(crawstr rs, const char* str)
    { return crawstr_find(rs, str) != c_NPOS; }

STC_INLINE bool crawstr_starts_with(crawstr rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str, str, n);
}

STC_INLINE bool crawstr_ends_with(crawstr rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str + rs.size - n, str, n);
}

/* utf8 iterator */
STC_INLINE crawstr_iter crawstr_begin(const crawstr* self) { 
    if (!self->size) return c_LITERAL(crawstr_iter){.ref = NULL};
    return c_LITERAL(crawstr_iter){.u8 = {{self->str, utf8_chr_size(self->str)}}};
}
STC_INLINE crawstr_iter crawstr_end(const crawstr* self) {
    (void)self; return c_LITERAL(crawstr_iter){.ref = NULL};
}
STC_INLINE void crawstr_next(crawstr_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}
STC_INLINE crawstr_iter crawstr_advance(crawstr_iter it, intptr_t pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos && *it.ref) pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.chr.size = utf8_chr_size(it.ref);
    if (!*it.ref) it.ref = NULL;
    return it;
}

/* utf8 ignore case cmp: depends on src/utf8code.c */
STC_INLINE int crawstr_icmp(const crawstr* x, const crawstr* y)
    { return utf8_icmp_sv(c_sv_2(x->str, x->size), c_sv_2(y->str, y->size)); }

STC_INLINE int crawstr_cmp(const crawstr* x, const crawstr* y) {
    intptr_t n = x->size < y->size ? x->size : y->size;
    int c = c_memcmp(x->str, y->str, n);
    return c ? c : (int)(x->size - y->size);
}

STC_INLINE bool crawstr_eq(const crawstr* x, const crawstr* y)
    { return x->size == y->size && !c_memcmp(x->str, y->str, x->size); }

STC_INLINE uint64_t crawstr_hash(const crawstr *self)
    { return stc_hash(self->str, self->size); }

#endif // CRAWSTR_H_INCLUDED
#undef i_static
#undef i_header
#undef i_implement
#undef i_import
#undef i_opt
// ### END_FILE_INCLUDE: crawstr.h


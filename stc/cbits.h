// ### BEGIN_FILE_INCLUDE: cbits.h
#ifndef CBITS_H_INCLUDED
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
#include <stdlib.h>
#include <string.h>

#ifndef i_ssize
#define i_ssize intptr_t
#endif
#define _cbits_bit(i) ((uint64_t)1 << ((i) & 63))
#define _cbits_words(n) (i_ssize)(((n) + 63)>>6)
#define _cbits_bytes(n) (_cbits_words(n) * c_sizeof(uint64_t))

#if defined(__GNUC__)
    STC_INLINE int cpopcount64(uint64_t x) {return __builtin_popcountll(x);}
    #ifndef __clang__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow="
    #pragma GCC diagnostic ignored "-Walloc-size-larger-than="
    #endif
#elif defined(_MSC_VER) && defined(_WIN64)
    #include <intrin.h>
    STC_INLINE int cpopcount64(uint64_t x) {return (int)__popcnt64(x);}
#else
    STC_INLINE int cpopcount64(uint64_t x) { /* http://en.wikipedia.org/wiki/Hamming_weight */
        x -= (x >> 1) & 0x5555555555555555;
        x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
        x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
        return (int)((x * 0x0101010101010101) >> 56);
    }
#endif

STC_INLINE i_ssize _cbits_count(const uint64_t* set, const i_ssize sz) {
    const i_ssize n = sz>>6;
    i_ssize count = 0;
    for (i_ssize i = 0; i < n; ++i)
        count += cpopcount64(set[i]);
    if (sz & 63)
        count += cpopcount64(set[n] & (_cbits_bit(sz) - 1));
    return count;
}

STC_INLINE char* _cbits_to_str(const uint64_t* set, const i_ssize sz, 
                               char* out, i_ssize start, i_ssize stop) {
    if (stop > sz) stop = sz;
    assert(start <= stop);

    c_memset(out, '0', stop - start);
    for (i_ssize i = start; i < stop; ++i) 
        if ((set[i>>6] & _cbits_bit(i)) != 0)
            out[i - start] = '1';
    out[stop - start] = '\0';
    return out;
}

#define _cbits_OPR(OPR, VAL) \
    const i_ssize n = sz>>6; \
    for (i_ssize i = 0; i < n; ++i) \
        if ((set[i] OPR other[i]) != VAL) \
            return false; \
    if (!(sz & 63)) \
        return true; \
    const uint64_t i = (uint64_t)n, m = _cbits_bit(sz) - 1; \
    return ((set[i] OPR other[i]) & m) == (VAL & m)

STC_INLINE bool _cbits_subset_of(const uint64_t* set, const uint64_t* other, const i_ssize sz)
    { _cbits_OPR(|, set[i]); }

STC_INLINE bool _cbits_disjoint(const uint64_t* set, const uint64_t* other, const i_ssize sz)
    { _cbits_OPR(&, 0); }

#endif // CBITS_H_INCLUDED

#define _i_memb(name) c_PASTE(i_type, name)

#if !defined i_capacity // DYNAMIC SIZE BITARRAY

#define _i_assert(x) assert(x)
#define i_type cbits

struct { uint64_t *data64; i_ssize _size; } typedef i_type;

STC_INLINE cbits   cbits_init(void) { return c_LITERAL(cbits){NULL}; }
STC_INLINE void    cbits_create(cbits* self) { self->data64 = NULL; self->_size = 0; }
STC_INLINE void    cbits_drop(cbits* self) { c_free(self->data64); }
STC_INLINE i_ssize cbits_size(const cbits* self) { return self->_size; }

STC_INLINE cbits* cbits_take(cbits* self, cbits other) {
    if (self->data64 != other.data64) {
        cbits_drop(self);
        *self = other;
    }
    return self;
}

STC_INLINE cbits cbits_clone(cbits other) {
    const i_ssize bytes = _cbits_bytes(other._size);
    cbits set = {(uint64_t *)c_memcpy(c_malloc(bytes), other.data64, bytes), other._size};
    return set;
}

STC_INLINE cbits* cbits_copy(cbits* self, const cbits* other) {
    if (self->data64 == other->data64)
        return self;
    if (self->_size != other->_size)
        return cbits_take(self, cbits_clone(*other));
    c_memcpy(self->data64, other->data64, _cbits_bytes(other->_size));
    return self;
}

STC_INLINE void cbits_resize(cbits* self, const i_ssize size, const bool value) {
    const i_ssize new_n = _cbits_words(size), osize = self->_size, old_n = _cbits_words(osize);
    self->data64 = (uint64_t *)c_realloc(self->data64, new_n*8);
    self->_size = size;
    if (new_n >= old_n) {
        c_memset(self->data64 + old_n, -(int)value, (new_n - old_n)*8);
        if (old_n > 0) {
            uint64_t m = _cbits_bit(osize) - 1; /* mask */
            value ? (self->data64[old_n - 1] |= ~m) 
                  : (self->data64[old_n - 1] &= m);
        }
    }
}

STC_INLINE void cbits_set_all(cbits *self, const bool value);
STC_INLINE void cbits_set_pattern(cbits *self, const uint64_t pattern);

STC_INLINE cbits cbits_move(cbits* self) {
    cbits tmp = *self;
    self->data64 = NULL, self->_size = 0;
    return tmp;
}

STC_INLINE cbits cbits_with_size(const i_ssize size, const bool value) {
    cbits set = {(uint64_t *)c_malloc(_cbits_bytes(size)), size};
    cbits_set_all(&set, value);
    return set;
}

STC_INLINE cbits cbits_with_pattern(const i_ssize size, const uint64_t pattern) {
    cbits set = {(uint64_t *)c_malloc(_cbits_bytes(size)), size};
    cbits_set_pattern(&set, pattern);
    return set;
}

#else // i_capacity: FIXED SIZE BITARRAY

#define _i_assert(x) (void)0
#ifndef i_type
#define i_type c_PASTE(cbits, i_capacity)
#endif

struct { uint64_t data64[(i_capacity - 1)/64 + 1]; } typedef i_type;

STC_INLINE i_type   _i_memb(_init)(void) { return c_LITERAL(i_type){0}; }
STC_INLINE void     _i_memb(_create)(i_type* self) {}
STC_INLINE void     _i_memb(_drop)(i_type* self) {}
STC_INLINE i_ssize  _i_memb(_size)(const i_type* self) { return i_capacity; }
STC_INLINE i_type   _i_memb(_move)(i_type* self) { return *self; }

STC_INLINE i_type*  _i_memb(_take)(i_type* self, i_type other)
    { *self = other; return self; }

STC_INLINE i_type _i_memb(_clone)(i_type other)
    { return other; }

STC_INLINE i_type* _i_memb(_copy)(i_type* self, const i_type* other) 
    { *self = *other; return self; }
    
STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value);
STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uint64_t pattern);

STC_INLINE i_type _i_memb(_with_size)(const i_ssize size, const bool value) {
    assert(size <= i_capacity);
    i_type set; _i_memb(_set_all)(&set, value);
    return set;
}

STC_INLINE i_type _i_memb(_with_pattern)(const i_ssize size, const uint64_t pattern) {
    assert(size <= i_capacity);
    i_type set; _i_memb(_set_pattern)(&set, pattern);
    return set;
}
#endif // i_capacity

// COMMON:

STC_INLINE void _i_memb(_set_all)(i_type *self, const bool value)
    { c_memset(self->data64, value? ~0 : 0, _cbits_bytes(_i_memb(_size)(self))); }

STC_INLINE void _i_memb(_set_pattern)(i_type *self, const uint64_t pattern) {
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] = pattern;
}

STC_INLINE bool _i_memb(_test)(const i_type* self, const i_ssize i) 
    { return (self->data64[i>>6] & _cbits_bit(i)) != 0; }

STC_INLINE bool _i_memb(_at)(const i_type* self, const i_ssize i)
    { return (self->data64[i>>6] & _cbits_bit(i)) != 0; }

STC_INLINE void _i_memb(_set)(i_type *self, const i_ssize i)
    { self->data64[i>>6] |= _cbits_bit(i); }

STC_INLINE void _i_memb(_reset)(i_type *self, const i_ssize i)
    { self->data64[i>>6] &= ~_cbits_bit(i); }

STC_INLINE void _i_memb(_set_value)(i_type *self, const i_ssize i, const bool b) {
    self->data64[i>>6] ^= ((uint64_t)-(int)b ^ self->data64[i>>6]) & _cbits_bit(i);
}

STC_INLINE void _i_memb(_flip)(i_type *self, const i_ssize i)
    { self->data64[i>>6] ^= _cbits_bit(i); }

STC_INLINE void _i_memb(_flip_all)(i_type *self) {
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] ^= ~(uint64_t)0;
}

STC_INLINE i_type _i_memb(_from)(const char* str) {
    int64_t n = c_strlen(str);
    i_type set = _i_memb(_with_size)(n, false);
    while (n--) if (str[n] == '1') _i_memb(_set)(&set, n);
    return set;
}

/* Intersection */
STC_INLINE void _i_memb(_intersect)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] &= other->data64[n];
}
/* Union */
STC_INLINE void _i_memb(_union)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] |= other->data64[n];
}
/* Exclusive disjunction */
STC_INLINE void _i_memb(_xor)(i_type *self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    i_ssize n = _cbits_words(_i_memb(_size)(self));
    while (n--) self->data64[n] ^= other->data64[n];
}

STC_INLINE int64_t _i_memb(_count)(const i_type* self)
    { return _cbits_count(self->data64, _i_memb(_size)(self)); }

STC_INLINE char* _i_memb(_to_str)(const i_type* self, char* out, int64_t start, int64_t stop)
    { return _cbits_to_str(self->data64, _i_memb(_size)(self), out, start, stop); }

STC_INLINE bool _i_memb(_subset_of)(const i_type* self, const i_type* other) { 
    _i_assert(self->_size == other->_size);
    return _cbits_subset_of(self->data64, other->data64, _i_memb(_size)(self));
}

STC_INLINE bool _i_memb(_disjoint)(const i_type* self, const i_type* other) {
    _i_assert(self->_size == other->_size);
    return _cbits_disjoint(self->data64, other->data64, _i_memb(_size)(self));
}
#if defined __GNUC__ && !defined __clang__
#pragma GCC diagnostic pop
#endif
#define CBITS_H_INCLUDED
#undef _i_size
#undef _i_memb
#undef _i_assert
#undef i_capacity
#undef i_type
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_exterm
// ### END_FILE_INCLUDE: cbits.h


//
// MathLike.h
//
// MIT License
// Copyright (c) 2022 Ken Kocienda. All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef UU_MATH_LIKE_H
#define UU_MATH_LIKE_H

#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#include <UU/Compiler.h>

#define UUMinT(_type, _x, _y) ({ \
    _type __x = (_x); \
    _type __y = (_y); \
    ((__x) < (__y)) ? (__x) : (__y); \
})

#define UUMaxT(_type, _x, _y) ({ \
    _type __x = (_x); \
    _type __y = (_y); \
    ((__x) > (__y)) ? (__x) : (__y); \
})

#define UUMultiMinT(_type, _x, ...) ({ \
    _type __elems[] = { (_x), __VA_ARGS__ }; \
    unsigned __count = sizeof(__elems) / sizeof(_type); \
    _type __z = __elems[0]; \
    for (unsigned _i = 1; _i < __count; _i++) { \
        if (__elems[_i] < __z) { \
            __z = __elems[_i]; \
        } \
    } \
    __z; \
})

#define UUMultiMaxT(_type, _x, ...) ({ \
    _type __elems[] = { (_x), __VA_ARGS__ }; \
    unsigned __count = sizeof(__elems) / sizeof(_type); \
    _type __z = __elems[0]; \
    for (unsigned _i = 1; _i < __count; _i++) { \
        if (__elems[_i] > __z) { \
            __z = __elems[_i]; \
        } \
    } \
    __z; \
})

#define UUClampT(_type, _x, _lo, _hi) ({ \
    _type __x = (_x); \
    _type __lo = (_lo); \
    _type __hi = (_hi); \
    _type __z = ((__x) < (__lo)) ? (__lo) : ((__x) > (__hi)) ? (__hi) : (__x); \
    __z; \
})

#define UUClampUnitZeroToOne(_x) ({ \
    UUClampT(CGFloat, (_x), 0.0, 1.0); \
})

namespace UU {

template <class T> struct FP {
    STATIC_CONSTEXPR T Epsilon = 0.001;
    STATIC_CONSTEXPR T Zero = 0;
    STATIC_CONSTEXPR T One = 1;
};

template <class T> bool is_fuzzy_equal_with_epsilon(T fuzzy, T solid, T epsilon = FP<T>::Epsilon) {
    return fabs(fuzzy - solid) < epsilon;
}

template <class T> bool is_fuzzy_equal(T fuzzy, T solid)
{
    return is_fuzzy_equal_with_epsilon(fuzzy, solid, FP<T>::Epsilon);
}

template <class T> bool is_fuzzy_zero(T num)
{
    return is_fuzzy_equal(num, FP<T>::Zero);
}

template <class T> bool is_fuzzy_one(T num)
{
    return is_fuzzy_equal(num, FP<T>::One);
}

template <class T> T lerp(T a, T b, T f)
{
    return a + ((b - a) * f);
}

template <class T> T moving_average(T p, T n, T a)
{
    return (p * (1.0 - a)) + (n * a);
}

template <class T> T union_alpha(T b, T s)
{
    return b + s - (b * s);
}

UU_ALWAYS_INLINE
bool is_power_of_2(size_t x)
{
    return (x & (x - 1)) == 0;
}

UU_ALWAYS_INLINE
size_t next_power_of_2(size_t n)
{
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    n |= (n >> 32);
    return n + 1;
}


UU_ALWAYS_INLINE
size_t ceil_to_page_size(size_t length)
{
    size_t page_size = getpagesize();
    bool even = length % page_size == 0;
    return even ? length : ((length / page_size) + 1) * page_size;
}

}  // namespace UU

// =========================================================================================================================================
// Hash goodness cribbed from Wolfgang Brehm: https://stackoverflow.com/a/50978188

#include <limits>
#include <cstdint>

namespace UU {

template <class T>
T xorshift(const T &n, int i) {
    return n^(n>>i);
}

STATIC_INLINE uint64_t distribute(const uint64_t &n) {
    uint64_t p = 0x5555555555555555;     // pattern of alternating 0 and 1
    uint64_t c = 17316035218449499591ull;// random uneven integer constant;
    return c * xorshift(p * xorshift(n, 32), 32);
}

template <class T, class S>
typename std::enable_if_t<std::is_unsigned<T>::value, T>
constexpr rotl(const T n, const S i){
    const T m = (std::numeric_limits<T>::digits - 1);
    const T c = (T)i & m;
    return (n << c) | (n >> ((T(0) - c) & m));
}

template <class T>
inline size_t hash_combine(std::size_t &seed, const T &v)
{
    return rotl(seed, std::numeric_limits<size_t>::digits / 3) ^ distribute(std::hash<T>{}(v));
}

}  // namespace UU

#endif // UU_MATH_LIKE_H
//
// Bits.h
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

#ifndef UU_BITS_H
#define UU_BITS_H

#include <bit>

#include <UU/Assertions.h>
#include <UU/Platform.h>
#include <UU/Types.h>

#if COMPILER(MSVC)
#include <intrin.h>
#endif

namespace UU {

template <typename N> requires IsUnsignedIntegral<N>
UU_ALWAYS_INLINE constexpr int countr_one(N n) {
#if COMPILER(GCC_OR_CLANG)
    if constexpr (std::is_same_v<N, UInt8> ||
                  std::is_same_v<N, UInt16> ||
                  std::is_same_v<N, UInt32>) {
        unsigned int x = ~n;
        return __builtin_ffs(x) - 1;
    }
    else if constexpr (std::is_same_v<N, UInt64>) {
        if (n == UInt64Max) {
            return 0;
        }
        unsigned long long x = ~n;
        return __builtin_clzll(x);
    }
#elif COMPILER(MSVC)
    if constexpr (std::is_same_v<N, UInt32>) {
        if (n < UInt32Max) {
            unsigned long x = ~n;
            unsigned long index;
            _BitScanForward(&index, x);
            return index;
        }
    }    
    else if constexpr (std::is_same_v<N, UInt64>) {
        if (n < UInt64Max) {
            UInt64 x = ~n;
            unsigned long index;
            _BitScanForward64(&index, x);
            return index;
        }
    }
#endif
    return -1;
}

template <typename N> requires IsUnsignedIntegral<N>
UU_ALWAYS_INLINE constexpr int popcount(N n) {
#if COMPILER(GCC_OR_CLANG)
    if constexpr (std::is_same_v<N, UInt8> ||
                  std::is_same_v<N, UInt16> ||
                  std::is_same_v<N, UInt32>) {
        unsigned int x = n;
        return __builtin_popcount(x);
    }
    else if constexpr (std::is_same_v<N, UInt64>) {
        return __builtin_popcountll(n);
    }
#elif COMPILER(MSVC)
    if constexpr (std::is_same_v<N, UInt8> ||
                  std::is_same_v<N, UInt16> ||
                  std::is_same_v<N, UInt32>) {
        unsigned int x = n;
        return __popcnt(x);
    }    
    else if constexpr (std::is_same_v<N, UInt64>) {
        return __popcnt64(n);
    }
#endif
    return -1;
}

}  // namespace UU

#endif // UU_BITS_H

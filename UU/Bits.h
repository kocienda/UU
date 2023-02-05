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

namespace UU {

template <typename N> requires IsUnsignedIntegral<N>
constexpr int countr_one(N n) {
#if COMPILER(GCC_OR_CLANG)
    if constexpr (std::is_same_v<N, UInt8> ||
                  std::is_same_v<N, UInt16> ||
                  std::is_same_v<N, UInt32>) {
        unsigned int x = ~n;
        return __builtin_ffs(x) - 1;
    }
    else if constexpr (std::is_same_v<N, UInt64>) {
        unsigned long long x = ~n;
        return __builtin_ffsll(x) - 1;
    }
#elif COMPILER(MSVC)
    if constexpr (std::is_same_v<N, UInt32>) {
        if (LIKELY(n < UInt32Max)) {
            unsigned int x = ~n;
            return _tzcnt_u32(x);
        }
    }    
    else if constexpr (std::is_same_v<N, UInt64>) {
        if (LIKELY(n < UInt64Max)) {
            unsigned int x1 = ~(n & 0xffffffff);
            int p = _tzcnt_u32(x1);
            if (p < 32) {
                return p;
            }
            else {
                unsigned int x2 = ~(n >> 32);
                return _tzcnt_u32(x2) + 32;
            }
        }
    }
#endif
    return -1;
}

}  // namespace UU

#endif // UU_BITS_H

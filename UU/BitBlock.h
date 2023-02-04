//
// BitBlock.h
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

#ifndef UU_BIT_BLOCK_H
#define UU_BIT_BLOCK_H

#include <bit>

#include <UU/Assertions.h>
#include <UU/Stretch.h>
#include <UU/Types.h>

namespace UU {

class BitBlock {
public:
    constexpr BitBlock() : m_value(0) {}

    constexpr Size bits() const { return 64; }
    constexpr Size size() const { return bits(); }
    constexpr UInt64 mask_for(Size idx) { ASSERT(idx < bits()); return UInt64(1) << idx; }
    constexpr void set_all() { m_value = UInt64Max; }
    constexpr void set(Size idx) {  m_value |= mask_for(idx); }
    constexpr void set(const Stretch<Size> &s) { for (auto it : s) { set(it); } }
    constexpr void clear(Size idx) { m_value &= ~(mask_for(idx)); }
    constexpr bool get(Size idx) { return m_value & mask_for(idx); }
    constexpr void reset() { m_value = 0; }
    template <bool B = true> constexpr bool is_empty() const { return (m_value == 0) == B; }
    template <bool B = true> constexpr bool is_full() const { return (m_value == UInt64Max) == B; }
    constexpr Size count() const { return std::popcount(m_value); }
    constexpr Size peek() const { return std::countr_one(m_value); }
    constexpr Size take() { 
        ASSERT(is_full<false>());
        Size idx = peek();
        set(idx);
        return idx;
    }

private:
    UInt64 m_value;
};

}  // namespace UU

#endif // UU_BIT_BLOCK_H

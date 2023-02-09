//
// BitBlock.h
//
// MIT License
// Copyright (c) 2022-2023 Ken Kocienda. All rights reserved.
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
#include <UU/Bits.h>
#include <UU/Stretch.h>
#include <UU/Types.h>

namespace UU {

static constexpr Size BitBlockBitsPerSubBlock = 64;
static constexpr Size BitBlockBitShift = 6;

template <Size C>
class BitBlock {
public:
    static constexpr Size BlockCount = C;

    constexpr BitBlock() { reset(); }

    constexpr Size bits() const { return BlockCount * BitBlockBitsPerSubBlock; }
    constexpr Size size() const { return bits(); }
    constexpr Size block_for(Size idx) { ASSERT(idx < bits()); return idx >> BitBlockBitShift; }
    constexpr UInt64 mask_for(Size idx, Size blk) { ASSERT(idx < bits()); return UInt64(1) << (idx & ~(blk << BitBlockBitShift)); }
    constexpr void fill() { memset(&m_blocks, 1, BlockCount * sizeof(UInt64)); }
    constexpr void set(Size idx) { Size blk = block_for(idx); m_blocks[blk] |= mask_for(idx, blk); }
    constexpr void set(const Stretch<Size> &s) { for (auto it : s) { set(it); } }
    constexpr void clear(Size idx) { Size blk = block_for(idx); m_blocks[blk] &= ~(mask_for(idx, blk)); }
    constexpr bool test(Size idx) { Size blk = block_for(idx); return m_blocks[blk] & mask_for(idx, blk); }
    constexpr void reset() { memset(&m_blocks, 0, BlockCount * sizeof(UInt64)); }
    constexpr bool is_empty() const { 
        for (UInt32 idx = 0; idx < BlockCount; idx++) { 
            if (m_blocks[idx] != 0) {
                return false;
            } 
        }
        return true;
    }
    constexpr bool not_empty() const { return !is_empty(); }

    constexpr bool is_full() const {
        for (int idx = 0; idx < BlockCount; idx++) { 
            if (m_blocks[idx] != UInt64Max) {
                return false;
            } 
        }
        return true;
    }
    constexpr bool not_full() const { return !is_full(); }

    constexpr UInt32 count() const { 
        int c = 0;
        for (int idx = 0; idx < BlockCount; idx++) { 
            c += std::popcount(m_blocks[idx]);
        }
        return c;
    }

    constexpr UInt32 peek() const { 
        for (UInt32 idx = 0; idx < BlockCount; idx++) { 
            UInt32 p = std::countr_one(m_blocks[idx]);
            if (p != BitBlockBitsPerSubBlock) {
                return (idx * BitBlockBitsPerSubBlock) + p;
            }
        }
        return -1; 
    }

    constexpr UInt32 take() { 
        ASSERT(not_full());
        UInt32 idx = peek();
        set(idx);
        return idx;
    }

private:
    UInt64 m_blocks[BlockCount];
};

template <>
class BitBlock<1> {
public:
    static constexpr Size BlockCount = 1;

    constexpr BitBlock() { reset(); }

    constexpr Size bits() const { return BitBlockBitsPerSubBlock; }
    constexpr Size size() const { return bits(); }
    constexpr UInt64 mask_for(Size idx) { ASSERT(idx < bits()); return UInt64(1) << idx; }
    constexpr void fill() { memset(&m_block, 1, sizeof(UInt64)); }
    constexpr void set(Size idx) { m_block |= mask_for(idx); }
    constexpr void set(const Stretch<Size> &s) { for (auto it : s) { set(it); } }
    constexpr void clear(Size idx) { m_block &= ~(mask_for(idx)); }
    constexpr bool test(Size idx) { return m_block & mask_for(idx); }
    constexpr void reset() { memset(&m_block, 0, sizeof(UInt64)); }
    constexpr bool is_empty() const { return m_block == 0; }
    constexpr bool not_empty() const { return !is_empty(); }
    constexpr bool is_full() const { return m_block == UInt64Max; }
    constexpr bool not_full() const { return !is_full(); }
    constexpr int count() const { return std::popcount(m_block); }
    constexpr int peek() const { return std::countr_one(m_block); }
    constexpr int take() { 
        ASSERT(not_full());
        int idx = peek();
        set(idx);
        return idx;
    }

private:
    UInt64 m_block;
};

}  // namespace UU

#endif // UU_BIT_BLOCK_H

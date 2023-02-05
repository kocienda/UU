//
// Allocator.cpp
//
// MIT License
// Copyright (c) 2022 Ken Kocienda. All rights reserved.
// 
// Permission is hereby granted, dealloc of charge, to any person obtaining a copy
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

#ifndef UU_ALLOCATOR_H
#define UU_ALLOCATOR_H

#include <format>

#include <UU/Assertions.h>
#include <UU/BitBlock.h>
#include <UU/Types.h>
#include <UU/UUString.h>

namespace UU {

// helpers ========================================================================================

UU_ALWAYS_INLINE 
Byte *byte_ptr(void *ptr) { return reinterpret_cast<Byte *>(ptr); }

UU_ALWAYS_INLINE 
constexpr Size align_up(Size size) { 
    constexpr Size void_size = std::alignment_of_v<void *>;
    Size rem = size % void_size;
    return rem == 0 ? size : size + (void_size - rem); 
}

// Memory =========================================================================================

struct Memory {
    constexpr Memory() {}
    constexpr Memory(void *ptr_, Size length_, int special_ = 0) : 
        ptr(ptr_), length(length_), special(special_) {}
    void *ptr = nullptr;
    Size length = 0;
    int special = 0;
};

// MemoryBlock ====================================================================================

template <Size Length, Size Count> requires IsMutipleOf64<Count>
struct MemoryBlock {
    static constexpr Size Capacity = Length * Count;
    static constexpr Size BlockCount = Count / 64;
    constexpr MemoryBlock() {}
    
    constexpr bool is_full() const { return bits.is_full(); }
    constexpr bool not_full() const { return !is_full(); }
    constexpr void ensure() { 
        if (UNLIKELY(base == nullptr)) {
            base = malloc(Capacity);
        }
    }
    constexpr Memory take() {
        ensure();
        ASSERT(not_full());
        int idx = bits.peek();
        bits.set(idx);
        return { byte_ptr(base) + (idx * Length), Length, idx };
    }
    constexpr void put(const Memory &mem) { 
        ASSERT(bits.test(mem.special));
        bits.clear(mem.special);
    }
    constexpr void reset() { 
        bits.reset();
    }
    constexpr void release() { 
        free(base);
        base = nullptr;
    }
    bool contains(const Memory &mem) const { 
        return mem.ptr >= base && mem.ptr <= byte_ptr(base) + Capacity;
    }    

    void *base = nullptr;
    BitBlock<BlockCount> bits;
};

// Freelist =======================================================================================

template <typename Allocator, Size Length, Size Capacity = SizeMax>
class Freelist
{
public:
    Memory alloc(Size length) {
        if (!root) {
            return parent.alloc(length);
        }
        Size elength = align_up(length);
        if (elength > Length) {
            return parent.alloc(length);
        }
        ASSERT(capacity > 0);
        Byte *ptr = (Byte *)root;
        LOG(Memory, "Returning from freelist: %lu : %p", Length, ptr);
        Memory mem(ptr, Length);
        root = root->next;
        capacity--;
        return mem;
    }

    void dealloc(Memory mem) {
        Size elength = align_up(mem.length);
        if (elength < Length || capacity >= Capacity) {
            LOG(Memory, "Freelist freeing from parent: %lu : %p", mem.length, mem.ptr);
            parent.dealloc(mem);
            return;
        }
        LOG(Memory, "Storing in freelist: %lu : %p", mem.length, mem.ptr);
        auto node = (Node *)mem.ptr;
        node->next = root;
        root = node;
        capacity++;
    }

    bool owns(const Memory &mem) const { 
        Size elength = align_up(mem.length);
        return elength == Length || parent.owns(mem);
    }    

private:
    struct Node { 
        Node *next = nullptr;
    };

    Allocator parent;
    Node *root = nullptr;
    Size capacity = 0;
};

// NullAllocator ==================================================================================

class NullAllocator
{
public:
    Memory alloc(Size length) {
        return Memory();
    }

    void dealloc(Memory mem) {
        ASSERT(mem.ptr == nullptr);
    }

    bool owns(const Memory &mem) const { return mem.ptr == nullptr; }    
};


// Segregator =====================================================================================

template <Size Threshold, typename First, typename Second>
class Segregator
{
public:
    Memory alloc(Size length) {
        Memory mem;
        if (length <= Threshold) {
            mem = first.alloc(length);
        }
        else {
            mem = second.alloc(length);
        }
        return mem;
    }

    void dealloc(Memory mem) {
        if (mem.length <= Threshold && first.owns(mem)) {
            first.dealloc(mem);
        }
        else {
            second.dealloc(mem);
        }
    }

    bool owns(const Memory &mem) const { return first.owns(mem) || second.owns(mem); }

private:
    First first; 
    Second second; 
};

// FallbackAllocator ==============================================================================

template <typename First, typename Second>
class FallbackAllocator
{
public:
    Memory alloc(Size length) {
        Memory mem = first.alloc(length);
        if (!mem.ptr) {
            mem = second.alloc(length);
        }
        return mem;
    }

    void dealloc(Memory mem) {
        if (first.owns(mem)) {
            first.dealloc(mem);
        }
        else {
            second.dealloc(mem);
        }
    }

    bool owns(const Memory &mem) const { return first.owns(mem) || second.owns(mem); }

private:
    First first; 
    Second second; 
};

// StackAllocator =================================================================================

template <Size S, Size Chunk = 64> requires IsGreaterThan<S, Chunk>
class StackAllocator
{
public:
    static constexpr Size Capacity = S;
    static constexpr Size EChunk = align_up(Chunk);

    constexpr StackAllocator() : ptr(base()) {}

    Memory alloc(Size length) {
        Size elength = std::max(align_up(length), EChunk);
        Memory mem;
        if (LIKELY(elength <= remaining())) {
            LOG(Memory, "StackAllocator alloc: %lu : %p", elength, ptr);
            mem = Memory(ptr, elength);
            ptr += elength;
        }
        return mem;
    }

    void dealloc(Memory mem) {
        Byte *mem_ptr = byte_ptr(mem.ptr);
        if (ptr == mem_ptr + align_up(mem.length)) {
            LOG(Memory, "StackAllocator dealloc: %lu : %p", mem.length, mem_ptr);
            ptr = mem_ptr;
        }
    }

    void free_all() {
        ptr = base();
    }

    bool owns(const Memory &mem) const { 
        return mem.ptr >= base() && mem.ptr < base() + Capacity; 
    }    

private:
    UU_ALWAYS_INLINE 
    Byte *base() const { return reinterpret_cast<Byte *>(const_cast<Byte *>(bytes)); }
    
    UU_ALWAYS_INLINE 
    Size remaining() const { return Capacity - (ptr - base()); }

    Byte bytes[Capacity];
    Byte *ptr;
};

// BlockAllocator =================================================================================

template <Size Count, Size LoFit, Size HiFit = LoFit> requires 
    IsMutipleOf64<Count> && IsLessThanOrEqual<LoFit, HiFit>
class BlockAllocator
{
public:
    using Block = MemoryBlock<HiFit, Count>;
    static constexpr Size Capacity = Block::Capacity;
    static constexpr Size BlockCount = Block::BlockCount;

    constexpr BlockAllocator() {}

    constexpr bool fits(Size length) const { return length >= LoFit && length <= HiFit; }

    Memory alloc(Size length) {
        Size elength = align_up(length);
        Memory mem;
        if (fits(elength) && m_block.not_full()) {
            mem = m_block.take();
            LOG(Memory, "BlockAllocator alloc: %lu : %p : %d", mem.length, mem.ptr, mem.special);
        }
        return mem;
    }

    void dealloc(Memory mem) {
        if (owns(mem)) {
            LOG(Memory, "BlockAllocator dealloc: %lu : %p : %d", mem.length, mem.ptr, mem.special);
            m_block.put(mem);
        }
    }

    void free_all() {
        m_block.reset();
        m_block.release();
    }

    bool owns(const Memory &mem) const { 
        return m_block.contains(mem);
    }    

private:
    Block m_block;
};

// StatsAllocator =================================================================================

template <typename Allocator>
class StatsAllocator : private Allocator
{
public:
    Memory alloc(Size length) {
        Memory mem = Allocator::alloc(length);
        allocs++;
        bytes_allocated += mem.length;
        bytes_allocated_now += mem.length;
        bytes_allocated_highwater = std::max(bytes_allocated_highwater, bytes_allocated_now);
        return mem;
    }

    void dealloc(Memory mem) {
        if (!owns(mem)) {
            return;
        }
        deallocs++;
        bytes_deallocated += mem.length;
        bytes_allocated_now -= mem.length;
        Allocator::dealloc(mem);
    }

    bool owns(const Memory &mem) const { return Allocator::owns(mem); }    

    String stats() const {
        Size num_digits = number_of_digits(bytes_allocated);
        std::string num_fmt = "{0:" + integer_to_string(num_digits) + "}";
        String result;
        std::string allocs_fmt =                    std::format("allocs:                    {0}\n", num_fmt);
        std::string deallocs_fmt =                  std::format("deallocs:                  {0}\n", num_fmt);
        std::string bytes_allocated_fmt =           std::format("bytes allocated:           {0}\n", num_fmt);
        std::string bytes_deallocated_fmt =         std::format("bytes deallocated:         {0}\n", num_fmt);
        std::string bytes_allocated_now_fmt =       std::format("bytes allocated now:       {0}\n", num_fmt);
        std::string bytes_allocated_highwater_fmt = std::format("bytes allocated highwater: {0}\n", num_fmt);
        result += "============================================================\n";
        result += "Allocator stats\n";
        result += "------------------------------------------------------------\n";
        result += std::vformat(allocs_fmt, std::make_format_args(integer_to_string<Size>(allocs)));
        result += std::vformat(deallocs_fmt, std::make_format_args(integer_to_string<Size>(deallocs)));
        result += std::vformat(bytes_allocated_fmt, std::make_format_args(integer_to_string<Size>(bytes_allocated)));
        result += std::vformat(bytes_deallocated_fmt, std::make_format_args(integer_to_string<Size>(bytes_deallocated)));
        result += std::vformat(bytes_allocated_now_fmt, std::make_format_args(integer_to_string<Size>(bytes_allocated_now)));
        result += std::vformat(bytes_allocated_highwater_fmt, std::make_format_args(integer_to_string<Size>(bytes_allocated_highwater)));
        return result;
    }    

private:
    Size allocs = 0;
    Size deallocs = 0;
    Size bytes_allocated = 0;
    Size bytes_deallocated = 0;
    Size bytes_allocated_now = 0;
    Size bytes_allocated_highwater = 0;
};

// Mallocator =====================================================================================

class Mallocator
{
public:
    Memory alloc(Size length) {
        Memory mem = Memory(malloc(length), length);
        LOG(Memory, "Mallocator alloc: %lu : %p", mem.length, mem.ptr);
        return mem;
    }

    void dealloc(Memory mem) {
        LOG(Memory, "Mallocator dealloc: %lu : %p", mem.length, mem.ptr);
        ::free(mem.ptr);
    }

    bool owns(const Memory &mem) const { return true; }
};


}  // namespace UU

#endif  // UU_ALLOCATOR_H

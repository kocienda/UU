//
// Allocator.h
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

#ifndef UU_ALLOCATOR_H
#define UU_ALLOCATOR_H

#include <format>
#include <string>

#include <UU/Assertions.h>
#include <UU/BitBlock.h>
#include <UU/MathLike.h>
#include <UU/Types.h>

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
    constexpr Memory(void *ptr_, Size capacity_) : ptr(ptr_), capacity(capacity_) {}
    constexpr bool is_empty() const { return ptr == nullptr; }
    constexpr bool not_empty() const { return !is_empty(); }
    void *ptr = nullptr;
    Size capacity = 0;
};

// MemoryBlock ====================================================================================

template <Size Capacity, Size Count> requires IsMutipleOf64<Count>
struct MemoryBlock {
    constexpr MemoryBlock() {}
    
    constexpr bool is_empty() const { return bits.is_empty(); }
    constexpr bool not_empty() const { return !is_empty(); }
    constexpr bool is_full() const { return bits.is_full(); }
    constexpr bool not_full() const { return !is_full(); }

    UU_ALWAYS_INLINE
    constexpr void reserve() { 
        if (UNLIKELY(base == nullptr)) {
            base = malloc(Capacity * Count);
        }
    }
    constexpr Memory take() {
        reserve();
        ASSERT(not_full());
        UInt32 idx = bits.peek();
        bits.set(idx);
        LOG(Memory, "MemoryBlock take: %d of %d", idx, Count);
        return { byte_ptr(base) + (idx * Capacity), Capacity };
    }
    constexpr void put(const Memory &mem) { 
        Size idx = (byte_ptr(mem.ptr) - byte_ptr(base)) / Capacity;
        LOG(Memory, "MemoryBlock put: %d", idx);
        ASSERT(idx < Count);
        bits.clear(idx);
    }
    constexpr void reset() { 
        bits.reset();
    }
    constexpr void release() { 
        LOG(Memory, "MemoryBlock free: %p", base);
        free(base);
        base = nullptr;
    }
    bool contains(const Memory &mem) const {
        return mem.ptr >= base && mem.ptr < (byte_ptr(base) + (Capacity * Count));
    }    

    void *base = nullptr;
    BitBlock<Count / BitBlockBitsPerSubBlock> bits;
};

// NullAllocator ==================================================================================

class NullAllocator
{
public:
    Memory alloc(Size capacity) {
        return Memory();
    }

    bool dealloc(Memory mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory mem) {
        ASSERT(mem.ptr == nullptr);
    }

    bool owns(const Memory &mem) const { return mem.ptr == nullptr; }
};

// Freelist =======================================================================================

template <typename Alloc, Size Length, Size Count = SizeMax>
class Freelist
{
public:
    Memory alloc(Size capacity) {
        if (!root) {
            return parent.alloc(capacity);
        }
        Size ecap = align_up(capacity);
        if (ecap > Length) {
            return parent.alloc(capacity);
        }
        ASSERT(count > 0);
        Byte *ptr = (Byte *)root;
        LOG(Memory, "Returning from freelist: %lu : %p", Length, ptr);
        Memory mem(ptr, Length);
        root = root->next;
        count--;
        return mem;
    }

    bool dealloc(Memory mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory mem) {
        Size ecap = align_up(mem.capacity);
        if (ecap < Length || count >= Count) {
            LOG(Memory, "Freelist freeing from parent: %lu : %p", mem.capacity, mem.ptr);
            parent.free(mem);
            return;
        }
        LOG(Memory, "Storing in freelist: %lu : %p", mem.capacity, mem.ptr);
        auto node = (Node *)mem.ptr;
        node->next = root;
        root = node;
        count++;
    }

    bool owns(const Memory &mem) const { 
        Size ecap = align_up(mem.capacity);
        return ecap == Length || parent.owns(mem);
    }    

private:
    struct Node { 
        Node *next = nullptr;
    };

    Alloc parent;
    Node *root = nullptr;
    Size count = 0;
};

// Segregator =====================================================================================

template <Size Threshold, typename FirstAlloc, typename SecondAlloc>
class Segregator
{
public:
    Memory alloc(Size capacity) {
        Memory mem;
        if (capacity <= Threshold) {
            mem = first.alloc(capacity);
        }
        else {
            mem = second.alloc(capacity);
        }
        return mem;
    }

    bool dealloc(Memory mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory mem) {
        if (mem.capacity <= Threshold && first.owns(mem)) {
            first.free(mem);
        }
        else {
            second.free(mem);
        }
    }

    bool owns(const Memory &mem) const { return first.owns(mem) || second.owns(mem); }

private:
    FirstAlloc first;
    SecondAlloc second;
};

// FallbackAllocator ==============================================================================

template <typename FirstAlloc, typename SecondAlloc>
class FallbackAllocator : private FirstAlloc, private SecondAlloc
{
public:
    Memory alloc(Size capacity) {
        Memory mem = FirstAlloc::alloc(capacity);
        if (!mem.ptr) {
            mem = SecondAlloc::alloc(capacity);
        }
        return mem;
    }

    bool dealloc(Memory mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory mem) {
        if (FirstAlloc::owns(mem)) {
            FirstAlloc::free(mem);
        }
        else {
            SecondAlloc::free(mem);
        }
    }

    bool owns(const Memory &mem) const { return FirstAlloc::owns(mem) || SecondAlloc::owns(mem); }
};

// StackAllocator =================================================================================

template <Size S, Size Chunk = 64> requires IsGreaterThan<S, Chunk>
class StackAllocator
{
public:
    static constexpr Size Count = S;
    static constexpr Size EChunk = align_up(Chunk);

    constexpr StackAllocator() : ptr(base()) {}

    Memory alloc(Size capacity) {
        Size ecap = std::max(align_up(capacity), EChunk);
        Memory mem;
        if (LIKELY(ecap <= remaining())) {
            LOG(Memory, "StackAllocator alloc: %lu : %p", ecap, ptr);
            mem = Memory(ptr, ecap);
            ptr += ecap;
        }
        return mem;
    }

    bool dealloc(Memory mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory mem) {
        Byte *mem_ptr = byte_ptr(mem.ptr);
        if (ptr == mem_ptr + align_up(mem.capacity)) {
            LOG(Memory, "StackAllocator free: %lu : %p", mem.capacity, mem_ptr);
            ptr = mem_ptr;
        }
    }

    void free_all() {
        ptr = base();
    }

    bool owns(const Memory &mem) const {
        return mem.ptr >= base() && mem.ptr < base() + Count; 
    }    

private:
    UU_ALWAYS_INLINE 
    Byte *base() const { return reinterpret_cast<Byte *>(const_cast<Byte *>(bytes)); }
    
    UU_ALWAYS_INLINE 
    Size remaining() const { return Count - cast_size(ptr - base()); }

    Byte bytes[Count];
    Byte *ptr;
};

// BlockAllocator =================================================================================

template <Size Count, Size LoFit, Size HiFit = LoFit, bool ChecksFit = false> requires 
    IsMutipleOf64<Count> && IsLessThanOrEqual<LoFit, HiFit>
class BlockAllocator
{
public:
    using Block = MemoryBlock<HiFit, Count>;

    constexpr BlockAllocator() {}

    constexpr bool fits(Size capacity) const { return capacity >= LoFit && capacity <= HiFit; }

    Memory alloc(Size capacity) {
        Size ecap = align_up(capacity);
        Memory mem;
        bool test_fit = true;
        if constexpr (ChecksFit) {
            test_fit = fits(ecap);
        }
        if (test_fit && m_block.not_full()) {
            ASSERT(fits(ecap));
            mem = m_block.take();
            LOG(Memory, "BlockAllocator alloc: %lu : %p", mem.capacity, mem.ptr);
        }
        return mem;
    }

    bool dealloc(Memory mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory mem) {
        LOG(Memory, "BlockAllocator free: %lu : %p", mem.capacity, mem.ptr);
        m_block.put(mem);
    }

    void free_all() {
        m_block.reset();
        m_block.release();
    }

    bool owns(const Memory &mem) const { 
        return m_block.contains(mem);
    }    

    bool is_empty() const {
        return m_block.is_empty();
    }

private:
    Block m_block;
};

// CascadingAllocator =============================================================================

template <typename Alloc, Size MaxCount = 8> requires IsGreaterThanOne<MaxCount>
class CascadingAllocator
{
public:
    constexpr CascadingAllocator() {
        m_allocators.emplace_back();
    }

    Memory alloc(Size capacity) {
        Size ecap = align_up(capacity);
        Memory mem;
        // search from most recent allocator to return a Memory
        for (Size idx = index; idx < m_allocators.size(); idx++) {
            mem = m_allocators[idx].alloc(ecap);
            if (mem.not_empty()) {
                index = idx;
                return mem;
            }
        }
        // search from start
        for (Size idx = 0; idx < index; idx++) {
            mem = m_allocators[idx].alloc(ecap);
            if (mem.not_empty()) {
                index = idx;
                return mem;
            }
        }
        // if (m_allocators.size() < MaxCount) {
            // cascade… add a new allocator
            LOG(Memory, "CascadingAllocator adding allocator: %llu", m_allocators.size());
            Alloc &a = m_allocators.emplace_back();
            index = cast_size(m_allocators.size()) - 1;
            mem = a.alloc(ecap);
        // }
        return mem;
    }

    bool dealloc(Memory mem) {
        for (Size idx = 0; idx < m_allocators.size(); idx++) {
            Alloc &a = m_allocators[idx];
            if (a.dealloc(mem)) {
                if (m_allocators.size() > 1 && a.is_empty()) {
                    // reclaim allocator
                    LOG(Memory, "CascadingAllocator freeing allocator: %llu", idx);
                    a.free_all();
                    m_allocators.erase(m_allocators.begin() + idx);
                }
                return true;
            }
        }
        return false;
    }

    void free(Memory mem) {
        dealloc(mem);
    }

    bool owns(const Memory &mem) const { 
        for (Size idx = 0; idx < m_allocators.size(); idx++) {
            if (m_allocators[idx].owns(mem)) {
                return true;
            }
        }
        return false;
    }    

private:
    std::vector<Alloc> m_allocators;
    Size index = 0;
};

// StatsAllocator =================================================================================

template <typename Alloc>
class StatsAllocator : private Alloc
{
public:
    Memory alloc(Size capacity) {
        Memory mem = Alloc::alloc(capacity);
        allocs++;
        bytes_allocated += mem.capacity;
        bytes_allocated_now += mem.capacity;
        bytes_allocated_highwater = std::max(bytes_allocated_highwater, bytes_allocated_now);
        return mem;
    }

    bool dealloc(Memory mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory mem) {
        if (!owns(mem)) {
            return;
        }
        deallocs++;
        bytes_deallocated += mem.capacity;
        bytes_allocated_now -= mem.capacity;
        Alloc::free(mem);
    }

    bool owns(const Memory &mem) const { return Alloc::owns(mem); }    

    std::string stats() const {
        Size num_digits = number_of_digits(bytes_allocated);
        std::string num_fmt = "{0:" + integer_to_string(num_digits) + "}";
        std::string result;
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
    Memory alloc(Size capacity) {
        Memory mem = Memory(malloc(capacity), capacity);
        LOG(Memory, "Mallocator alloc: %lu : %p", mem.capacity, mem.ptr);
        return mem;
    }

    bool dealloc(Memory mem) {
        free(mem);
        return true;
    }

    void free(Memory mem) {
        LOG(Memory, "Mallocator free: %lu : %p", mem.capacity, mem.ptr);
        ::free(mem.ptr);
    }

    bool owns(const Memory &mem) const { return true; }
};


}  // namespace UU

#endif  // UU_ALLOCATOR_H

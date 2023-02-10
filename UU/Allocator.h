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

#include <array>
#include <atomic>
#include <format>
#include <mutex>
#include <pthread.h>
#include <sstream>
#include <string>

#include <UU/Assertions.h>
#include <UU/BitBlock.h>
#include <UU/MathLike.h>
#include <UU/SmallVector.h>
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

// NullAllocator ==================================================================================

class NullAllocator
{
public:
    Memory alloc(Size capacity) {
        return Memory();
    }

    bool dealloc(Memory &mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory &mem) {
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

    bool dealloc(Memory &mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory &mem) {
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

    bool dealloc(Memory &mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory &mem) {
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

    bool dealloc(Memory &mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory &mem) {
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

    bool dealloc(Memory &mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory &mem) {
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
    Size remaining() const { return Count - (ptr - base()); }

    Byte bytes[Count];
    Byte *ptr;
};

// StatsAllocator =================================================================================

template <typename Alloc>
class StatsAllocator : private Alloc
{
public:
    using Elapsed = std::chrono::duration<double>;

    Memory alloc(Size capacity) {
        auto mark = std::chrono::steady_clock::now();
        Memory mem = Alloc::alloc(capacity);
        auto done = std::chrono::steady_clock::now();
        Elapsed elapsed = done - mark;
        time_in_seconds += elapsed;
        allocs++;
        bytes_allocated += mem.capacity;
        bytes_allocated_now += mem.capacity;
        bytes_allocated_highwater = std::max(bytes_allocated_highwater, bytes_allocated_now);
        return mem;
    }

    bool dealloc(Memory &mem) {
        if (!owns(mem)) {
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory &mem) {
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
        // Size num_digits = number_of_digits(bytes_allocated);
        // std::string num_fmt = "{0:" + integer_to_string(num_digits) + "}";
        // std::string result;
        // std::string allocs_fmt =                    std::format("allocs:                    {0}\n", num_fmt);
        // std::string deallocs_fmt =                  std::format("deallocs:                  {0}\n", num_fmt);
        // std::string bytes_allocated_fmt =           std::format("bytes allocated:           {0}\n", num_fmt);
        // std::string bytes_deallocated_fmt =         std::format("bytes deallocated:         {0}\n", num_fmt);
        // std::string bytes_allocated_now_fmt =       std::format("bytes allocated now:       {0}\n", num_fmt);
        // std::string bytes_allocated_highwater_fmt = std::format("bytes allocated highwater: {0}\n", num_fmt);
        // result += "============================================================\n";
        // result += "Allocator stats\n";
        // result += "------------------------------------------------------------\n";
        // result += std::vformat(allocs_fmt, std::make_format_args(integer_to_string<Size>(allocs)));
        // result += std::vformat(deallocs_fmt, std::make_format_args(integer_to_string<Size>(deallocs)));
        // result += std::vformat(bytes_allocated_fmt, std::make_format_args(integer_to_string<Size>(bytes_allocated)));
        // result += std::vformat(bytes_deallocated_fmt, std::make_format_args(integer_to_string<Size>(bytes_deallocated)));
        // result += std::vformat(bytes_allocated_now_fmt, std::make_format_args(integer_to_string<Size>(bytes_allocated_now)));
        // result += std::vformat(bytes_allocated_highwater_fmt, std::make_format_args(integer_to_string<Size>(bytes_allocated_highwater)));
        // return result;
        std::stringstream result;
        result << "============================================================\n";
        result << "Allocator stats\n";
        result << "------------------------------------------------------------\n";
        result << "time in seconds:           " << time_in_seconds.count() << std::endl;
        result << "allocs:                    " << allocs << std::endl;
        result << "deallocs:                  " << deallocs << std::endl;
        result << "outstanding blocks:        " << (allocs - deallocs) << std::endl;
        result << "bytes allocated:           " << bytes_allocated << std::endl;
        result << "bytes deallocated:         " << bytes_deallocated << std::endl;
        result << "bytes allocated now:       " << bytes_allocated_now << std::endl;
        result << "bytes allocated highwater: " << bytes_allocated_highwater << std::endl;
        return result.str();
    }    

private:
    Elapsed time_in_seconds = Elapsed(0);
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

    bool dealloc(Memory &mem) {
        free(mem);
        return true;
    }

    void free(Memory &mem) {
        LOG(Memory, "Mallocator free: %lu : %p", mem.capacity, mem.ptr);
        ::free(mem.ptr);
    }

    bool owns(const Memory &mem) const { return true; }
};

// MemoryBlock ====================================================================================

struct MemoryBlock {
    static constexpr Size BLOCK_COUNT = 2;

    constexpr MemoryBlock(Size capacity_) : capacity(capacity_) {}
    
    UU_ALWAYS_INLINE constexpr bool is_empty() const { return bits.is_empty(); }
    UU_ALWAYS_INLINE constexpr bool not_empty() const { return !is_empty(); }
    UU_ALWAYS_INLINE constexpr bool is_full() const { return bits.is_full(); }
    UU_ALWAYS_INLINE constexpr bool not_full() const { return !is_full(); }

    constexpr Memory take() {
        if (UNLIKELY(base == nullptr)) {
            base = malloc(capacity * BLOCK_COUNT);
            extent = byte_ptr(base) + (capacity * BLOCK_COUNT);
        }
        ASSERT(not_full());
        UInt32 idx = bits.peek();
        bits.set(idx);
        // LOG(Memory, "MemoryBlock take: %d of %d : %lu", idx, BLOCK_COUNT, capacity);
        return { byte_ptr(base) + (idx * capacity), capacity };
    }

    constexpr void put(const Memory &mem) { 
        Size idx = (byte_ptr(mem.ptr) - byte_ptr(base)) / capacity;
        // LOG(Memory, "MemoryBlock put: %d", idx);
        ASSERT(idx < Count);
        bits.clear(idx);
    }

    constexpr void reset() { 
        bits.reset();
    }

    constexpr void release() { 
        // LOG(Memory, "MemoryBlock free: %p", base);
        free(base);
        base = nullptr;
        extent = nullptr;
    }
    
    bool contains(const Memory &mem) const {
        return mem.ptr >= base && mem.ptr < extent;
    }    

    void *base = nullptr;
    void *extent = nullptr;
    Size capacity = 0;
    BitBlock<BLOCK_COUNT> bits;
};

// BlockAllocator =================================================================================

template <Size Count, Size LoFit, Size HiFit = LoFit, bool ChecksFit = false> requires 
    IsMutipleOf64<Count> && IsLessThanOrEqual<LoFit, HiFit>
class BlockAllocator
{
public:
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
            ASSERT_WITH_MESSAGE(fits(ecap), "must fit in %lu - %lu ; got %lu", LoFit, HiFit, capacity);
            mem = m_block.take();
            LOG(Memory, "BlockAllocator alloc: %lu (%lu - %lu) : %p (%d => %p)", mem.capacity, LoFit, HiFit, mem.ptr, pthread_main_np(), pthread_self());
        }
        return mem;
    }

    bool dealloc(Memory &mem) {
        if (!owns(mem)) {
            LOG(Memory, "BlockAllocator doesn't own: %lu (%lu - %lu) : %p (%d => %p)", mem.capacity, LoFit, HiFit, mem.ptr, pthread_main_np(), pthread_self());
            return false;
        }
        free(mem);
        return true;
    }

    void free(Memory &mem) {
        LOG(Memory, "BlockAllocator free: %lu (%lu - %lu) : %p (%d => %p)", mem.capacity, LoFit, HiFit, mem.ptr, pthread_main_np(), pthread_self());
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
    MemoryBlock m_block = MemoryBlock(HiFit);
};

// CascadingAllocator =============================================================================

template <typename Alloc, Size MaxCount = 8> requires IsGreaterThanOne<MaxCount>
class CascadingAllocator
{
public:
    constexpr CascadingAllocator() {
        for (Size idx = 0; idx < MaxCount; idx++) {
            m_allocators[idx] = Alloc();
        }
    }

    Memory alloc(Size capacity) {
        Size ecap = align_up(capacity);
        Memory mem;
        // search from most recent allocator
        for (Size idx = index; idx < MaxCount; idx++) {
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
        return mem;
    }

    bool dealloc(Memory &mem) {
        // search from most recent allocator
        for (Size idx = index; idx < MaxCount; idx++) {
            Alloc &a = m_allocators[idx];
            if (a.dealloc(mem)) {
                if (a.is_empty()) {
                    LOG(Memory, "CascadingAllocator freeing allocator: %llu", idx);
                    a.free_all();
                }
                return true;
            }
        }
        // search from start
        for (Size idx = 0; idx < index; idx++) {
            Alloc &a = m_allocators[idx];
            if (a.dealloc(mem)) {
                if (a.is_empty()) {
                    LOG(Memory, "CascadingAllocator freeing allocator: %llu", idx);
                    a.free_all();
                }
                return true;
            }
        }
        return false;
    }

    void free(Memory &mem) {
        dealloc(mem);
    }

    bool owns(const Memory &mem) const { 
        for (Size idx = 0; idx < MaxCount; idx++) {
            if (m_allocators[idx].owns(mem)) {
                return true;
            }
        }
        return false;
    }    

private:
    Alloc m_allocators[MaxCount];
    Size index = 0;
};

// GPAllocator =====================================================================================

class GPAllocator
{
    static constexpr Size Size0 =    0;
    static constexpr Size Size1 =   32;
    static constexpr Size Size2 =   64;
    static constexpr Size Size3 =   96;
    static constexpr Size Size4 =  128;
    static constexpr Size Size5 =  256;
    static constexpr Size Size6 =  384;
    static constexpr Size Size7 =  512;
    static constexpr Size Size8 = 1024;

    using Size1Allocator = CascadingAllocator<BlockAllocator<256, Size0, Size1>>;
    using Size2Allocator = CascadingAllocator<BlockAllocator<256, Size1 + 1, Size2>>;
    using Size3Allocator = CascadingAllocator<BlockAllocator<256, Size2 + 1, Size3>>;
    using Size4Allocator = CascadingAllocator<BlockAllocator<256, Size3 + 1, Size4>>;
    using Size5Allocator = CascadingAllocator<BlockAllocator<256, Size4 + 1, Size5>>;
    using Size6Allocator = CascadingAllocator<BlockAllocator<256, Size5 + 1, Size6>>;
    using Size7Allocator = CascadingAllocator<BlockAllocator<256, Size6 + 1, Size7>>;
    using Size8Allocator = CascadingAllocator<BlockAllocator<256, Size7 + 1, Size8>>;

public:
    UU_ALWAYS_INLINE void lock(std::atomic_flag &lock) {
        while (lock.test_and_set(std::memory_order_acquire)) {
            lock.wait(true, std::memory_order_relaxed);
        }
    }

    UU_ALWAYS_INLINE void unlock(std::atomic_flag &lock) {
        lock.clear(std::memory_order_release);
        lock.notify_one();
    }

    Memory alloc(Size capacity) {
        Size ecapacity = align_up(capacity);
        Memory mem;
        if (ecapacity <= Size1) {
            lock(x1);
            mem = a1.alloc(ecapacity);
            unlock(x1);
        }
        else if (ecapacity <= Size2) {
            lock(x2);
            mem = a2.alloc(ecapacity);
            unlock(x2);
        }
        else if (ecapacity <= Size3) {
            lock(x3);
            mem = a3.alloc(ecapacity);
            unlock(x3);
        }
        else if (ecapacity <= Size4) {
            lock(x4);
            mem = a4.alloc(ecapacity);
            unlock(x4);
        }
        else if (ecapacity <= Size5) {
            lock(x5);
            mem = a5.alloc(ecapacity);
            unlock(x5);
        }
        else if (ecapacity <= Size6) {
            lock(x6);
            mem = a6.alloc(ecapacity);
            unlock(x6);
        }
        else if (ecapacity <= Size7) {
            lock(x7);
            mem = a7.alloc(ecapacity);
            unlock(x7);
        }
        else if (ecapacity <= Size8) {
            lock(x8);
            mem = a8.alloc(ecapacity);
            unlock(x8);
        }
        if (mem.is_empty()) {
            lock(xm);
            mem = mallocator.alloc(ecapacity);
            unlock(xm);
        }
        return mem;
    }

    UU_ALWAYS_INLINE void mallocator_dealloc(Memory &mem) {
        lock(xm);
        mallocator.dealloc(mem);
        unlock(xm);
    }

    bool dealloc(Memory &mem) {
        bool b = false;
        if (mem.capacity <= Size1) {
            lock(x1);
            b = a1.dealloc(mem);
            unlock(x1);
        }
        else if (mem.capacity <= Size2) {
            lock(x2);
            b = a2.dealloc(mem);
            unlock(x2);
        }
        else if (mem.capacity <= Size3) {
            lock(x3);
            b = a3.dealloc(mem);
            unlock(x3);
        }
        else if (mem.capacity <= Size4) {
            lock(x4);
            b = a4.dealloc(mem);
            unlock(x4);
        }
        else if (mem.capacity <= Size5) {
            lock(x5);
            b = a5.dealloc(mem);
            unlock(x5);
        }
        else if (mem.capacity <= Size6) {
            lock(x6);
            b = a6.dealloc(mem);
            unlock(x6);
        }
        else if (mem.capacity <= Size7) {
            lock(x7);
            b = a7.dealloc(mem);
            unlock(x7);
        }
        else if (mem.capacity <= Size8) {
            lock(x8);
            b = a8.dealloc(mem);
            unlock(x8);
        }
        else {
            b = true;
            mallocator_dealloc(mem);
        }
        if (!b) {
            mallocator_dealloc(mem);
        }
        return true;
    }

    void free(Memory &mem) {
        dealloc(mem);
    }

    bool owns(const Memory &mem) const { return true; }

private:
    Size1Allocator a1;
    Size2Allocator a2;
    Size3Allocator a3;
    Size4Allocator a4;
    Size5Allocator a5;
    Size6Allocator a6;
    Size7Allocator a7;
    Size8Allocator a8;
    Mallocator mallocator;
    std::atomic_flag x1 = ATOMIC_FLAG_INIT;
    std::atomic_flag x2 = ATOMIC_FLAG_INIT;
    std::atomic_flag x3 = ATOMIC_FLAG_INIT;
    std::atomic_flag x4 = ATOMIC_FLAG_INIT;
    std::atomic_flag x5 = ATOMIC_FLAG_INIT;
    std::atomic_flag x6 = ATOMIC_FLAG_INIT;
    std::atomic_flag x7 = ATOMIC_FLAG_INIT;
    std::atomic_flag x8 = ATOMIC_FLAG_INIT;
    std::atomic_flag xm = ATOMIC_FLAG_INIT;
};


}  // namespace UU

#endif  // UU_ALLOCATOR_H

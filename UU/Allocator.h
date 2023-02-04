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
#include <UU/Types.h>
#include <UU/UUString.h>

namespace UU {

// Memory =========================================================================================

struct Memory {
    constexpr Memory() {}
    constexpr Memory(void *ptr_, Size length_) : ptr(ptr_), length(length_) {}
    void *ptr = nullptr;
    Size length = 0;
};

// helpers ========================================================================================

UU_ALWAYS_INLINE 
Byte *byte_ptr(void *ptr) { return reinterpret_cast<Byte *>(ptr); }

UU_ALWAYS_INLINE 
constexpr Size align_up(Size size) { 
    constexpr Size void_size = std::alignment_of_v<void *>;
    Size rem = size % void_size;
    return rem == 0 ? size : size + (void_size - rem); 
}

// Freelist =======================================================================================

template <typename Allocator, Size Length, 
    Size AllocMargin = 0, Size FreeMargin = 0, Size Capacity = SizeMax>
requires IsGreaterThan<align_up(Length), align_up(AllocMargin), std::alignment_of_v<void *>> &&
         IsGreaterThan<align_up(Length), align_up(FreeMargin), std::alignment_of_v<void *>>
class Freelist
{
public:
    static constexpr Size AllocLength = align_up(Length) - align_up(AllocMargin);
    static constexpr Size FreeLength = align_up(Length) + align_up(FreeMargin);

    Memory alloc(Size length) {
        if (!root) {
            return parent.alloc(length);
        }
        Size elength = align_up(length);
        if (elength < AllocLength || elength > Length) {
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
        if ((elength < Length || elength > FreeLength) || capacity == Capacity) {
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

    bool owns(Memory mem) const { 
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

    bool owns(Memory mem) const { return mem.ptr == nullptr; }    
};

// FallbackAllocator ==============================================================================

template <typename First, typename Second>
class FallbackAllocator : private First, private Second
{
public:
    Memory alloc(Size length) {
        Memory mem = First::alloc(length);
        if (!mem.ptr) {
            mem = Second::alloc(length);
        }
        return mem;
    }

    void dealloc(Memory mem) {
        if (First::owns(mem)) {
            First::dealloc(mem);
        }
        else {
            Second::dealloc(mem);
        }
    }

    bool owns(Memory mem) const { return First::owns(mem) || Second::owns(mem); }
};

// StackAllocator =================================================================================

template <Size S>
class StackAllocator
{
public:
    static constexpr Size Capacity = S;

    constexpr StackAllocator() : ptr(base()) {}

    Memory alloc(Size length) {
        Size elength = align_up(length);
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

    bool owns(Memory mem) const { return mem.ptr >= base() && mem.ptr < base() + Capacity; }    

private:
    UU_ALWAYS_INLINE 
    Byte *base() const { return reinterpret_cast<Byte *>(const_cast<Byte *>(bytes)); }
    
    UU_ALWAYS_INLINE 
    Size remaining() const { return Capacity - (ptr - base()); }

    Byte bytes[Capacity];
    Byte *ptr;
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

    bool owns(Memory mem) const { return Allocator::owns(mem); }    

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
        return Memory(malloc(length), length);
    }

    void dealloc(Memory mem) {
        ::free(mem.ptr);
    }

    bool owns(Memory mem) const { return true; }
};


}  // namespace UU

#endif  // UU_ALLOCATOR_H

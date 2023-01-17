//
// Storage.h
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

#ifndef UU_STORAGE_H
#define UU_STORAGE_H

#include <UU/Assertions.h>
#include <UU/Types.h>

#include <string>

namespace UU {

template <Size S, typename A> requires IsNonZeroSize<S>
class Storage 
{
public:
    static constexpr Size InlineCapacity = S;
    using AlignmentT = A;

    static constexpr UInt32 UsingAllocatedBuffer = 0x01;

    constexpr Storage() : m_ptr(buf_ptr()) {}

    constexpr Byte *data() const { return m_ptr; }
    constexpr Size length() const { return m_length; }
    constexpr Size capacity() const { return m_capacity; }
    constexpr UInt32 flags() const { return m_flags; }

    constexpr void set_allocated_storage(Byte *ptr) { m_ptr = ptr; set_using_allocated_storage(); }
    constexpr void clear_allocated_storage() { m_ptr = buf_ptr(); set_using_inline_storage(); }
    constexpr void set_length(Size length) { m_length = length; }
    constexpr void set_capacity(Size capacity) { m_capacity = capacity; }
    constexpr void set_flags(UInt32 flags) { m_flags = flags; }

    template <bool B = true> constexpr bool is_using_allocated_storage() const { 
        return (m_flags & UsingAllocatedBuffer) == B; 
    }

    template <bool B = true> constexpr void set_using_allocated_storage() {
        if (B) {
            m_flags |= UsingAllocatedBuffer;
        }
        else {
            m_flags &= ~UsingAllocatedBuffer;
        }
    }

    template <bool B = true> constexpr bool is_using_inline_storage() const { 
        return !is_using_allocated_storage() == B; 
    }

    template <bool B = true> constexpr void set_using_inline_storage() {
        set_using_allocated_storage<!B>();
    }

private:
    Storage(const Storage &) = delete;
    Storage(Storage &&) = delete;
    Storage &operator=(const Storage &) = delete;
    Storage &operator=(Storage &&) = delete;

    using InlineStorage = std::aligned_storage_t<InlineCapacity, std::alignment_of_v<AlignmentT>>;
    
    constexpr Byte *buf_ptr() { return reinterpret_cast<Byte *>(m_buf); }

    InlineStorage m_buf[InlineCapacity];
    Byte *m_ptr = nullptr;
    Size m_length = 0;
    Size m_capacity = InlineCapacity;
    UInt32 m_flags = 0;
};

}  // namespace UU

#endif  // UU_STORAGE_H
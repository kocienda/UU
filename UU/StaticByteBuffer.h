//
// StaticByteBuffer.h
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

#ifndef UU_STATIC_BYTE_BUFFER_H
#define UU_STATIC_BYTE_BUFFER_H

#include <initializer_list>

#include <UU/Assertions.h>
#include <UU/Types.h>

namespace UU {

template <Size S>
class StaticByteBuffer {
public:
    static constexpr Size Capacity = S;
    static constexpr const Size npos = SizeMax;
    static constexpr Byte empty_value = 0;

    constexpr StaticByteBuffer() {
        for (Size pos = 0; pos < capacity(); ++pos) {
            m_buf[pos] = empty_value;
        }
    }
    constexpr StaticByteBuffer(Byte *ptr, Size size) {
        for (Size pos = 0; pos < std::min(size, capacity()); ++pos) {
            m_buf[pos] = ptr[pos];
        }
        for (Size pos = std::min(size, capacity()); pos < capacity(); ++pos) {
            m_buf[pos] = empty_value;
        }
    }
    constexpr StaticByteBuffer(const Byte *ptr, Size size) { 
        for (Size pos = 0; pos < std::min(size, capacity()); ++pos) {
            m_buf[pos] = ptr[pos];
        }
        for (Size pos = std::min(size, capacity()); pos < capacity(); ++pos) {
            m_buf[pos] = empty_value;
        }
    }
    constexpr StaticByteBuffer(std::initializer_list<Byte> ilist) { 
        Size idx = 0;
        for (auto it = ilist.begin(); it != ilist.end(); ++it) {
            m_buf[idx] = *it;
            idx++;
            if (idx == capacity()) {
                break;
            }
        }
        for (Size pos = std::min(idx, capacity()); pos < capacity(); ++pos) {
            m_buf[pos] = empty_value;
        }
    }
    constexpr StaticByteBuffer &operator=(const StaticByteBuffer &other) { 
        for (Size pos = 0; pos < capacity(); ++pos) {
            m_buf[pos] = other[pos];
        }
        return *this;
    }

    constexpr Size capacity() const { return Capacity; }

    constexpr Byte &at(Size pos) const {
        if (pos < capacity()) {
            return m_buf[pos];
        }
        else {
            return m_buf[capacity() - 1];
        }
    }
    
    constexpr const Byte &operator[](Size index) const {
        return m_buf[index];
    }
    
    constexpr Byte &operator[](Size index) {
        return m_buf[index];
    }

    constexpr friend bool operator==(const StaticByteBuffer &a, const StaticByteBuffer &b) {
        if (a.capacity() != b.capacity()) {
            return false;
        }
        if (a.capacity() == 0) {
            return true;
        }
        // return memcmp(&a.m_buf, &b.m_buf, a.capacity()) == 0;
        for (Size pos = 0; pos < a.capacity(); pos++) {
            if (a.m_buf[pos] != b.m_buf[pos]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const StaticByteBuffer &a, const StaticByteBuffer &b) {
        return !(a == b);
    }

private:
    Byte m_buf[Capacity] = {};
};

}  // namespace UU

#endif  // UU_STATIC_BYTE_BUFFER_H

//
// ByteBuffer.h
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

#ifndef UU_BYTE_BUFFER_H
#define UU_BYTE_BUFFER_H

#include <UU/Assertions.h>
#include <UU/Types.h>

namespace UU {

template <Size S> requires IsNonZeroSize<S>
class ByteBuffer {
public:
    static constexpr Size Capacity = S;
    static constexpr const Size npos = SizeMax;
    static constexpr Byte empty_value = 0;

    constexpr ByteBuffer() { memset(m_buf, empty_value, Capacity); }
    constexpr ByteBuffer(Byte *ptr, Size size) {}
    constexpr ByteBuffer(const Byte *ptr, Size size) {}

    constexpr Byte *data() const { return m_buf; }
    constexpr Size length() const { return Capacity; }

private:
    constexpr Byte *buf_ptr() { return reinterpret_cast<Byte *>(m_buf); }

    Byte m_buf[Capacity];
};

}  // namespace UU

#endif  // UU_BYTE_BUFFER_H
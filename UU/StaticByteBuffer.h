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

#include <UU/Assertions.h>
#include <UU/ByteWriter.h>
#include <UU/ByteReader.h>
#include <UU/Types.h>

namespace UU {

class StaticByteBuffer : public ByteWriter, public ByteReader
{
public:
    static constexpr const SizeType nidx = -1;
    static constexpr Byte empty_value = 0;

    constexpr StaticByteBuffer() {}
    StaticByteBuffer(Byte *ptr, SizeType size) : m_ptr(ptr), m_size(size), m_writable(true) {}
    StaticByteBuffer(const Byte *ptr, SizeType size) : m_ptr((Byte *)ptr), m_size(size), m_writable(false) {}
    virtual ~StaticByteBuffer() {}

    bool is_writable() const { return m_writable; }

    Byte *bytes() const override { return m_ptr; }
    SizeType size() const override { return m_size; }

    void write(const std::string &str) override { write((const Byte *)str.c_str(), (SizeType)str.length()); }

    void write(const Byte *bytes, SizeType size) override {
        if (can_write(size)) {
            memcpy((void *)(m_ptr + m_write_offset), bytes, size);
            m_write_offset += size;
        }
    }
    
    void write(Byte b) override {
        if (can_write(1)) {
            m_ptr[m_write_offset] = b;
            m_write_offset++;
        }
    }

    Byte at(SizeType index) {
        if (m_size > index) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const Byte &at(SizeType index) const {
        if (m_size > index) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const Byte &operator[](SizeType index) const {
        return m_ptr[index];
    }
    
    friend bool operator==(const StaticByteBuffer &a, const StaticByteBuffer &b) {
        if (a.size() != b.size()) {
            return false;
        }
        if (a.size() == 0) {
            return true;
        }
        return memcmp(a.bytes(), b.bytes(), a.size()) == 0;
    }

    friend bool operator!=(const StaticByteBuffer &a, const StaticByteBuffer &b) {
        return !(a == b);
    }

private:
    bool can_write(SizeType size_to_write) {
        return m_writable && (m_write_offset + size_to_write < m_size);
    }

    Byte *m_ptr = nullptr;
    SizeType m_size = 0;
    SizeType m_write_offset = 0;
    bool m_writable = false;
};

}  // namespace UU

#endif  // UU_STATIC_BYTE_BUFFER_H

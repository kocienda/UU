//
// String.h
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

#ifndef UU_STRING_H
#define UU_STRING_H

#include <UU/Assertions.h>
#include <UU/MathLike.h>
#include <UU/Types.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>

#include <stdlib.h>

namespace UU {

template <typename T> class Span;
class TextRef;

template <SizeType S>
class BasicString
{
public:
    enum { InlineCapacity = S };

    static constexpr const SizeType npos = SizeTypeMax;
    static constexpr Byte empty_value = 0;

    constexpr BasicString() {}
    
    explicit BasicString(SizeType capacity) {
        ensure_capacity(capacity);
    }
    
    BasicString(const Byte *bytes, SizeType length) {
        append(bytes, length);
    }
    
    BasicString(const char *ptr, SizeType length) {
        append((const Byte *)ptr, length);
    }

    BasicString(std::istream &in) {
        char buffer[1024];
        while (in.read(buffer, sizeof(buffer))) {
            append((const Byte *)buffer, sizeof(buffer));
        }
        append((const Byte *)buffer, (SizeType)in.gcount());
    }

    BasicString(const BasicString &other) {
        append(other.bytes(), other.length());
    }

    BasicString(BasicString &&other) {
        if (other.using_allocated_buffer()) {
            m_ptr = other.m_ptr;
            m_length = other.length();
            m_capacity = other.capacity();
        }
        else {
            m_length = other.length();
            ensure_capacity(other.capacity());
            copy_from(other.bytes(), other.length());
        }
    }

    BasicString &operator=(const BasicString &other) {
        if (this == &other) {
            return *this;
        }

        m_length = other.length();
        ensure_capacity(other.capacity());
        if (m_length > 0) {
            copy_from(other.bytes(), other.length());
        }

        return *this;
    }

    BasicString &operator=(const std::string &str) {
        m_length = 0;
        append((const Byte *)str.c_str(), (SizeType)str.length());
        return *this;
    }

    BasicString &operator=(BasicString &&other) {
        if (other.using_allocated_buffer()) {
            if (using_allocated_buffer()) {
                delete m_ptr;
            }
            m_ptr = other.m_ptr;
            m_length = other.length();
            m_capacity = other.capacity();
        }
        else {
            m_length = other.length();
            ensure_capacity(other.capacity());
            copy_from(other.bytes(), other.length());
        }
        return *this;
    }

    ~BasicString() {
        if (using_allocated_buffer()) {
            delete m_ptr;
        }
    }

    Byte *data() const { return m_ptr; }
    SizeType length() const { return m_length; }

    SizeType capacity() const { return m_capacity; }
    void reserve(SizeType length) { ensure_capacity(length); }

    void append(const Byte *bytes, SizeType length) {
        if (LIKELY(length > 0)) {
            ensure_capacity(m_length + length);
            memcpy(m_ptr + m_length, bytes, length);
            m_length += length;
        }
    }
    
    void append(Byte byte) {
        ensure_capacity(m_length + 1);
        m_ptr[m_length] = byte;
        m_length++;
    }

    void append(const std::string &str) {
        append((const Byte *)str.data(), (SizeType)str.length());
    }

    template <typename N>
    void append_as_string(N val) {
        char buf[MaximumInteger64LengthAsString];
        char *ptr = buf;
        integer_to_string(val, ptr);
        SizeType len = strlen(ptr);
        ensure_capacity(m_length + len);
        append((const Byte *)ptr, len);
    }

    void append(const Span<SizeType> &);
    void append(const TextRef &);

    BasicString &operator+=(const std::string &s) {
        append(s.c_str(), s.length());
        return *this;
    }

    BasicString &operator+=(const char *s) {
        append(s, strlen(s));
        return *this;
    }

    BasicString &operator+=(Byte b) {
        append(b);
        return *this;
    }

    void append(Byte b, SizeType length) {
        if (LIKELY(length > 0)) {
            ensure_capacity(m_length + length);
            memset(m_ptr, b, length);
            m_length += length;
        }
    }

    template <bool B = true> bool is_empty() const { return (m_length == 0) == B; }

    bool using_inline_buffer() const { return m_ptr == const_cast<Byte *>(m_buf); }
    bool using_allocated_buffer() const { return !(using_inline_buffer()); }

    void clear() {
        m_length = 0;
    }
    
    Byte at(SizeType index) {
        if (LIKELY(m_length > index)) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const Byte &at(SizeType index) const {
        if (m_length > index) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const Byte &operator[](SizeType index) const {
        return m_ptr[index];
    }
    
    Byte &operator[](SizeType index) {
        return m_ptr[index];
    }

    operator std::string () {
         return std::string((char *)data(), length());
    }

    friend bool operator==(const BasicString &a, const BasicString &b) {
        if (a.length() != b.length()) {
            return false;
        }
        if (a.length() == 0) {
            return true;
        }
        return memcmp(a.bytes(), b.bytes(), a.length()) == 0;
    }

    friend bool operator!=(const BasicString &a, const BasicString &b) {
        return !(a == b);
    }

private:
    void ensure_capacity(SizeType new_capacity) {
        if (new_capacity <= InlineCapacity) {
            m_capacity = InlineCapacity;
            return;
        }
    
        if (new_capacity <= m_capacity) {
            return;
        }
    
        while (m_capacity < new_capacity) {
            m_capacity *= 2;
        }

        SizeType amt = m_capacity * sizeof(Byte);
        if (m_ptr != static_cast<Byte *>(m_buf)) {
            m_ptr = static_cast<Byte *>(realloc(m_ptr, amt));
        }
        else {
            m_ptr = static_cast<Byte *>(malloc(amt));
            copy_from(m_buf, InlineCapacity);
        }
    }

    void copy_from(const Byte *src, SizeType length) {
        memcpy(m_ptr, src, length);
    }

    Byte m_buf[InlineCapacity];
    Byte *m_ptr = m_buf;
    SizeType m_length = 0;
    SizeType m_capacity = InlineCapacity;
};

using String = BasicString<256>;

}  // namespace UU

#endif  // UU_STRING_H

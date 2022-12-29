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
#include <string>
#include <string_view>
#include <utility>

#include <stdlib.h>

namespace UU {

template <typename T> class Span;
class TextRef;

static constexpr SizeType DefaultBasicStringSize = 256;

template <typename CharT, class Traits = std::char_traits<CharT>, SizeType S = DefaultBasicStringSize>
class BasicString
{
public:
    enum { InlineCapacity = S };

    static constexpr const SizeType npos = SizeTypeMax;
    static constexpr CharT empty_value = CharT();

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

    BasicString(const char *ptr) {
        append((const Byte *)ptr, strlen(ptr));
    }

    BasicString(const std::string &str) {
        append((const char *)str.data(), str.length());
    }

    BasicString(SizeType length, CharT c) {
        append(c, length);
    }

    BasicString(std::istream &in) {
        char buffer[1024];
        while (in.read(buffer, sizeof(buffer))) {
            append((const Byte *)buffer, sizeof(buffer));
        }
        append((const Byte *)buffer, (SizeType)in.gcount());
    }

    BasicString(const BasicString &other) {
        append(other.data(), other.length());
    }

    BasicString(BasicString &&other) {
        if (other.using_allocated_buffer()) {
            m_ptr = other.m_ptr;
            m_length = other.length();
            m_capacity = other.capacity();
            other.reset();
        }
        else {
            append(other.data(), other.length());
        }
    }

    BasicString &operator=(const BasicString &other) {
        if (this == &other) {
            return *this;
        }

        m_length = 0;
        append(other.data(), other.length());
        return *this;
    }

    BasicString &operator=(const std::string &str) {
        m_length = 0;
        append((const Byte *)str.data(), (SizeType)str.length());
        return *this;
    }

    BasicString &operator=(BasicString &&other) {
        m_length = 0;
        if (other.using_allocated_buffer()) {
            if (using_allocated_buffer()) {
                delete m_ptr;
            }
            m_ptr = other.m_ptr;
            m_length = other.length();
            m_capacity = other.capacity();
            other.reset();
        }
        else {
            append(other.data(), other.length());
        }
        return *this;
    }

    ~BasicString() {
        if (using_allocated_buffer()) {
            delete m_ptr;
        }
    }

    CharT *data() const { return m_ptr; }
    SizeType length() const { return m_length; }

    SizeType capacity() const { return m_capacity; }
    void reserve(SizeType length) { ensure_capacity(length); }

    void append(const Byte *bytes, SizeType length) {
        if (LIKELY(length > 0)) {
            ensure_capacity(m_length + length);
            for (int idx = 0; idx < length; idx++) {
                m_ptr[m_length + idx] = bytes[idx];
            }
            m_length += length;
        }
    }
    
    void append(const char *bytes, SizeType length) {
        if (LIKELY(length > 0)) {
            ensure_capacity(m_length + length);
            for (int idx = 0; idx < length; idx++) {
                m_ptr[m_length + idx] = bytes[idx];
            }
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

    void append(const Span<int> &);
    void append(const Span<SizeType> &);
    void append(const Span<Int64> &);
    void append(const TextRef &);

    template <typename N>
    void append_as_string(N val) {
        char buf[MaximumInteger64LengthAsString];
        char *ptr = buf;
        integer_to_string(val, ptr);
        SizeType len = strlen(ptr);
        ensure_capacity(m_length + len);
        append((const Byte *)ptr, len);
    }

    BasicString &operator+=(const std::string &s) {
        append(s.data(), s.length());
        return *this;
    }

    BasicString &operator+=(const std::string_view &s) {
        append(s.data(), s.length());
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
            for (int idx = 0; idx < length; idx++) {
                m_ptr[m_length + idx] = b;
            }
            m_length += length;
        }
    }

    void append(CharT c, SizeType length) {
        if (LIKELY(length > 0)) {
            ensure_capacity(m_length + length);
            for (int idx = 0; idx < length; idx++) {
                m_ptr[m_length + idx] = c;
            }
            m_length += length;
        }
    }

    template <bool B = true> bool is_empty() const { return (m_length == 0) == B; }

    bool using_inline_buffer() const { return m_ptr == const_cast<CharT *>(m_buf); }
    bool using_allocated_buffer() const { return !(using_inline_buffer()); }

    void clear() {
        m_length = 0;
    }
    
    CharT at(SizeType index) {
        if (LIKELY(m_length > index)) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const CharT &at(SizeType index) const {
        if (m_length > index) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const CharT &operator[](SizeType index) const {
        return m_ptr[index];
    }
    
    Byte &operator[](SizeType index) {
        return m_ptr[index];
    }

    operator std::basic_string<CharT, std::char_traits<CharT>>() const noexcept {
        return std::basic_string(data(), length());
    }

    operator std::basic_string_view<CharT, std::char_traits<CharT>>() const noexcept {
        return std::basic_string_view(data(), length());
    }

    friend bool operator==(const BasicString &a, const BasicString &b) {
        if (a.length() != b.length()) {
            return false;
        }
        if (a.length() == 0) {
            return true;
        }
        return Traits::compare(a.data(), b.data(), a.length()) == 0;
    }

    friend bool operator!=(const BasicString &a, const BasicString &b) {
        return !(a == b);
    }

    friend bool operator<(const BasicString &a, const BasicString &b) {
        SizeType len = std::min(a.length(), b.length());
        int cmp = Traits::compare(a.data(), b.data(), len);
        return cmp != 0 ? cmp : (a.length() < b.length());
    }

    friend bool operator>(const BasicString &a, const BasicString &b) {
        return !(a < b); 
    }

    class iterator : public std::random_access_iterator_tag {
    public:
        using iterator_topic = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = CharT;
        using pointer = CharT *;
        using reference = CharT &;
        
        constexpr iterator(const BasicString *str, size_t index = 0) : m_str(str), m_index(index) {}
      
        reference operator *() const { return *(m_str->data() + m_index); }
        reference operator[](SizeType idx) { return *(m_str->data() + m_index + idx); }
        reference operator[](SizeType idx) const { return *(m_str->data() + m_index + idx); }

        iterator &operator+=(int offset) {
            if (m_index + offset >= m_str->length()) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index += offset;
            }
            return *this;
        }

        iterator operator+(int offset) {
            iterator it(*this);
            return it += 1;
        }

        iterator &operator-=(int offset) {
            if (m_index < offset || m_str->length() < offset) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index -= offset;
            }
            return *this;
        }

        iterator operator-(int offset) {
            iterator it(*this);
            return it -= 1;
        }

        iterator &operator++() {
            if (m_index + 1 >= m_str->length()) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index++;
            }
            return *this;
        }

        iterator &operator--() {
            if (m_index == 0 || m_str->length() == 0) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index--;
            }
            return *this;
        }

        difference_type operator-(const iterator &rhs) {
            if (m_str != rhs.m_str) {
                return 0;
            }
            difference_type a = m_index;
            difference_type b = rhs.m_index;
            return a - b;
        }

        friend bool operator==(const iterator &lhs, const iterator &rhs) {
            return lhs.m_str == rhs.m_str && lhs.m_index == rhs.m_index;
        }

        friend bool operator!=(const iterator &lhs, const iterator &rhs) {
            return !(lhs==rhs);
        }

        friend bool operator<(const iterator &lhs, const iterator &rhs) {
            if (lhs.m_str == rhs.m_str) {
                return lhs.m_index < rhs.m_index;
            }
            return false;
        }

        friend bool operator<=(const iterator &lhs, const iterator &rhs) {
            return operator==(lhs, rhs) || operator<(lhs, rhs);
        }

        friend bool operator>(const iterator &lhs, const iterator &rhs) {
            if (lhs.m_str == rhs.m_str) {
                return lhs.m_index > rhs.m_index;
            }
            return false;
        }

        friend bool operator>=(const iterator &lhs, const iterator &rhs) {
            return operator==(lhs, rhs) || operator>(lhs, rhs);
        }

    private:
        const BasicString *m_str = nullptr;
        SizeType m_index = 0;
    };

    class reverse_iterator : public std::random_access_iterator_tag {
    public:
        using iterator_topic = std::random_access_iterator_tag;
        using difference_type = off_t;
        using value_type = CharT;
        using pointer = CharT *;
        using reference = CharT &;
        
        constexpr reverse_iterator(const BasicString *str, size_t index = 0) : m_str(str), m_index(index) {}
      
        reference operator *() const { return *(m_str->data() + (m_str->length() - m_index - 1)); }
        reference operator[](SizeType idx) { return *(m_str->data() + (m_str->length() + m_index - idx - 1)); }
        reference operator[](SizeType idx) const { return *(m_str->data() + (m_str->length() + m_index - idx - 1)); }

        reverse_iterator &operator+=(int offset) {
            if (m_index + offset >= m_str->length()) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index += offset;
            }
            return *this;
        }

        reverse_iterator operator+(int offset) {
            reverse_iterator it(*this);
            return it += 1;
        }

        reverse_iterator &operator-=(int offset) {
            if (m_index < offset || m_str->length() < offset) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index -= offset;
            }
            return *this;
        }

        reverse_iterator operator-(int offset) {
            reverse_iterator it(*this);
            return it -= 1;
        }

        reverse_iterator &operator++() {
            if (m_index + 1 >= m_str->length()) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index++;
            }
            return *this;
        }

        reverse_iterator &operator--() {
            if (m_index == 0 || m_str->length() == 0) {
                m_str = nullptr;
                m_index = 0;
            }
            else {
                m_index--;
            }
            return *this;
        }

        difference_type operator-(const reverse_iterator &rhs) {
            if (m_str != rhs.m_str) {
                return 0;
            }
            difference_type a = m_index;
            difference_type b = rhs.m_index;
            return a - b;
        }

        friend bool operator==(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return lhs.m_str == rhs.m_str && lhs.m_index == rhs.m_index;
        }

        friend bool operator!=(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return !(lhs==rhs);
        }

        friend bool operator<(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            if (lhs.m_str == rhs.m_str) {
                return lhs.m_index < rhs.m_index;
            }
            return false;
        }

        friend bool operator<=(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return operator==(lhs, rhs) || operator<(lhs, rhs);
        }

        friend bool operator>(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            if (lhs.m_str == rhs.m_str) {
                return lhs.m_index > rhs.m_index;
            }
            return false;
        }

        friend bool operator>=(const reverse_iterator &lhs, const reverse_iterator &rhs) {
            return operator==(lhs, rhs) || operator>(lhs, rhs);
        }

    private:
        const BasicString *m_str = nullptr;
        SizeType m_index = 0;
    };


    constexpr iterator begin() const {
        return iterator(this);
    }

    constexpr iterator end() const {
        return iterator(nullptr);
    }

    constexpr iterator cbegin() const {
        return iterator(this);
    }

    constexpr iterator cend() const {
        return iterator(nullptr);
    }

    constexpr reverse_iterator rbegin() const {
        return reverse_iterator(this);
    }

    constexpr reverse_iterator rend() const {
        return reverse_iterator(nullptr);
    }

    constexpr reverse_iterator crbegin() const {
        return reverse_iterator(this);
    }

    constexpr reverse_iterator crend() const {
        return reverse_iterator(nullptr);
    }

private:
    void reset() {
        m_ptr = m_buf;
        m_length = 0;
        m_capacity = InlineCapacity;
    }

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

        SizeType amt = m_capacity * sizeof(CharT);
        if (m_ptr != static_cast<CharT *>(m_buf)) {
            m_ptr = static_cast<CharT *>(realloc(m_ptr, amt));
        }
        else {
            m_ptr = static_cast<CharT *>(malloc(amt));
            Traits::copy(m_ptr, m_buf, InlineCapacity);
        }
    }

    CharT m_buf[InlineCapacity];
    CharT *m_ptr = m_buf;
    SizeType m_length = 0;
    SizeType m_capacity = InlineCapacity;
};

template <typename CharT, class Traits, SizeType S>
std::ostream &operator<<(std::ostream &os, const BasicString<CharT, Traits, S> &str)
{
    os.write(str.data(), str.length());
    return os;
}

using String = BasicString<char, std::char_traits<char>, DefaultBasicStringSize>;

}  // namespace UU


namespace std
{
    template <typename CharT, UU::SizeType S>
    struct less<UU::BasicString<CharT, std::char_traits<CharT>, S>>
    {
        using StringT = UU::BasicString<CharT, std::char_traits<CharT>, S>;
        bool operator()(const StringT &lhs, const StringT &rhs) const {
            return lhs < rhs;
        }
    };
}

#endif  // UU_STRING_H

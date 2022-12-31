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

#include "Assertions.h"
#include <UU/Assertions.h>
#include <UU/MathLike.h>
#include <UU/Types.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <stdlib.h>

#define UU_STRING_THROWS_EXCEPTIONS 0

namespace UU {

// forward declarations ===========================================================================

template <typename T> class Span;
class TextRef;

// constants ======================================================================================

static constexpr SizeType BasicStringDefaultSize = 256;

// template metaprogramming =======================================================================

template <typename T>
using IsByteSized_ = std::bool_constant<sizeof(T) == sizeof(char)>;
template <typename T> constexpr bool IsByteSized = IsByteSized_<T>::value;


template <typename T>
struct HasIteratorCategory
{
private:
    template <class U> static std::false_type test(...);
    template <class U> static std::true_type test(typename U::iterator_category* = nullptr);
public:
    static const bool value = decltype(test<T>(nullptr))::value;
};

template <typename T, typename U, bool = HasIteratorCategory<std::iterator_traits<T>>::value>
struct HasIteratorCategoryConvertibleTo : 
    std::is_convertible<typename std::iterator_traits<T>::iterator_category, U>
{};

template <typename T>
struct IsInputIteratorCategory_ : 
    public HasIteratorCategoryConvertibleTo<T, std::input_iterator_tag> {};

template <typename T> constexpr bool IsInputIteratorCategory = IsInputIteratorCategory_<T>::value;

template <typename T, typename CharT, typename Traits>
using IsConvertibleToStringView_ = std::is_convertible<T, std::basic_string_view<CharT, Traits>>;
template <typename T, typename CharT, typename Traits> 
    constexpr bool IsConvertibleToStringView = IsConvertibleToStringView_<T, CharT, Traits>::value;

template <typename T, typename CharT>
using IsConvertibleToConstCharTStar_ = std::is_convertible<T, const CharT *>;
template <typename T, typename CharT> 
    constexpr bool IsConvertibleToConstCharTStar = IsConvertibleToConstCharTStar_<T, CharT>::value;

template <typename T, typename CharT, typename Traits>
constexpr bool IsStringViewLike = IsConvertibleToStringView<T, CharT, Traits> && 
    !IsConvertibleToConstCharTStar<T, CharT>;

// ================================================================================================
// BasicString class
//

template <typename CharT, SizeType S = BasicStringDefaultSize, class Traits = std::char_traits<CharT>>
class BasicString
{
private:
    // internal helpers ===========================================================================

#define UU_STRING_ASSERT_NULL_TERMINATED \
    do { \
        ASSERT_WITH_MESSAGE(m_ptr[m_length] == '\0', "string not null terminated"); \
    } while (0)

    UU_ALWAYS_INLINE constexpr void null_terminate() {
        ensure_capacity(m_length + 1);
        m_ptr[m_length] = '\0';
    }

    UU_ALWAYS_INLINE 
    void ensure_capacity(SizeType new_capacity) {
        if (new_capacity <= InlineCapacity) {
            m_capacity = InlineCapacity;
            return;
        }
    
        if (new_capacity <= m_capacity) {
            return;
        }
        
        grow(new_capacity);
    }

    UU_ALWAYS_INLINE constexpr bool using_inline_buffer() const { 
        return m_ptr == const_cast<CharT *>(m_buf); 
    }

    UU_ALWAYS_INLINE constexpr bool using_allocated_buffer() const { 
        return !(using_inline_buffer()); 
    }

    UU_ALWAYS_INLINE void return_empty_or_throw_out_of_range(SizeType pos) const { 
#if UU_STRING_THROWS_EXCEPTIONS
        BasicString<char, std::char_traits<char>, 64> msg("String access out of range: ");
        msg.append_as_string(pos);
        throw std::out_of_range(msg);
#else
        return empty_value;
#endif
    }

public:
    // constants ==================================================================================

    static constexpr SizeType InlineCapacity = S;
    static constexpr const SizeType npos = SizeTypeMax;
    static constexpr CharT empty_value = CharT();

    // usings =====================================================================================

    using BasicStringView = std::basic_string_view<CharT, std::char_traits<CharT>>;
    using iterator = CharT *;
    using const_iterator = const CharT *;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    // constructors ===============================================================================

    constexpr BasicString() { null_terminate(); }
    
    explicit BasicString(SizeType capacity) {
        ensure_capacity(capacity);
        null_terminate();
    }
    
    BasicString(const char *ptr, SizeType length) {
        append(ptr, length);
    }

    BasicString(const CharT *ptr) {
        append(ptr, Traits::length(ptr));
    }

    BasicString(const std::string &str) {
        append(str.data(), str.length());
    }

    BasicString(SizeType count, CharT c) {
        append(count, c);
    }

    BasicString(std::istream &in) {
        char buffer[1024];
        while (in.read(buffer, sizeof(buffer))) {
            append((const Byte *)buffer, sizeof(buffer));
        }
        append(buffer, in.gcount());
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    BasicString(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            ensure_capacity(m_length + 1);
            m_ptr[m_length] = *it;
            m_length++;
        }
        null_terminate();
    }

    BasicString(std::initializer_list<CharT> ilist) {
        insert(end(), ilist.begin(), ilist.end());
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

    // destructor =================================================================================

    ~BasicString() {
        if (using_allocated_buffer()) {
            delete m_ptr;
        }
    }

    // accessors ==================================================================================

    constexpr CharT *data() const { return m_ptr; }
    constexpr SizeType length() const { return m_length; }
    constexpr SizeType capacity() const { return m_capacity; }
    constexpr const CharT &operator[](SizeType index) const { return m_ptr[index]; }
    constexpr CharT &operator[](SizeType index) { return m_ptr[index]; }
    constexpr CharT& front() { return m_ptr[0]; }
    constexpr CharT& front() const { return m_ptr[0]; }
    constexpr CharT& back() { return m_ptr[m_length - 1]; }
    constexpr CharT& back() const { return m_ptr[m_length - 1]; }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr const char *c_str() const noexcept { return reinterpret_cast<const char *>(data()); }

    template <class CharX = CharT, std::enable_if_t<IsByteSized<CharX>, int> = 0>
    constexpr const char *c_str() const noexcept { return data(); }

    template <bool B = true> constexpr bool is_empty() const { return (m_length == 0) == B; }
    
    constexpr CharT at(SizeType index) {
        if (LIKELY(m_length > index)) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    constexpr const CharT &at(SizeType index) const {
        if (m_length > index) {
            return m_ptr[index];
        }
        else {
            return return_empty_or_throw_out_of_range(index);
        }
    }
    
    // resizing ===================================================================================

    void reserve(SizeType length) { ensure_capacity(length); }
    constexpr void clear() { m_length = 0; }

    // appending ==================================================================================
    // all calls in this section must end with null_terminate() or UU_STRING_ASSERT_NULL_TERMINATED

    BasicString &append(const char *ptr, SizeType length) {
        if (LIKELY(length > 0)) {
            ensure_capacity(m_length + length);
            for (int idx = 0; idx < length; idx++) {
                m_ptr[m_length + idx] = ptr[idx];
            }
            m_length += length;
        }
        null_terminate();
        return *this;
    }
    
    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    BasicString &append(const CharT *ptr, SizeType length) {
        if (LIKELY(length > 0)) {
            ensure_capacity(m_length + length);
            Traits::copy(m_ptr + m_length, ptr, length);
            m_length += length;
        }
        null_terminate();
        return *this;
    }

    BasicString &append(CharT byte) {
        ensure_capacity(m_length + 1);
        m_ptr[m_length] = byte;
        m_length++;
        null_terminate();
        return *this;
    }

    BasicString &append(SizeType count, CharT c) {
        if (LIKELY(count > 0)) {
            ensure_capacity(m_length + count);
            for (int idx = 0; idx < count; idx++) {
                m_ptr[m_length + idx] = c;
            }
            m_length += count;
        }
        null_terminate();
        return *this;
    }

    BasicString &append(const std::string &str) {
        append(str.data(), str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    BasicString &append(InputIt first, InputIt last) {
        insert(end(), first, last);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &append(const StringViewLikeT &t) {
        append(t.data(), t.length());
        return *this;
    }

    BasicString &append(const Span<int> &);
    BasicString &append(const Span<SizeType> &);
    BasicString &append(const Span<Int64> &);
    BasicString &append(const TextRef &);

    template <typename N>
    BasicString &append_as_string(N val) {
        char buf[MaximumInteger64LengthAsString];
        char *ptr = buf;
        integer_to_string(val, ptr);
        SizeType len = strlen(ptr);
        ensure_capacity(m_length + len);
        append(ptr, len);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // inserting ==================================================================================
    // all calls in this section must end with null_terminate() or UU_STRING_ASSERT_NULL_TERMINATED

    constexpr BasicString &insert(SizeType index, SizeType count, CharT c) {
        ensure_capacity(m_length + count);
        const_iterator pos = cbegin() + index;
        iterator dst = iterator(pos) + count;
        Traits::move(dst, pos, end() - pos);
        ptrdiff_t diff = pos - begin();
        for (int idx = 0; idx < count; idx++) {
            m_ptr[diff + idx] = c;
        }
        m_length += count;
        null_terminate();
        return *this;
    }

    constexpr BasicString &insert(SizeType index, const CharT *s) {
        return insert(index, s, strlen(s));
    }

    constexpr BasicString &insert(SizeType index, const CharT *s, SizeType count) {
        ensure_capacity(m_length + count);
        iterator pos = begin() + index;
        iterator dst = pos + count;
        Traits::move(dst, pos, end() - pos);
        Traits::copy(pos, s, count);
        m_length += count;
        null_terminate();
        return *this;
    }

    constexpr BasicString &insert(SizeType index, const BasicString& str) {
        insert(index, str, 0, str.length()); 
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;      
    }

    constexpr BasicString &insert(SizeType index, const BasicString &str, SizeType index_str, SizeType count = npos) {
        SizeType ecount = std::min(count, str.length() - index_str);
        ensure_capacity(m_length + ecount);
        iterator pos = begin() + index;
        iterator dst = pos + ecount;
        Traits::move(dst, pos, end() - pos);
        Traits::copy(pos, str.begin() + index_str, ecount);
        m_length += ecount;
        null_terminate();
        return *this;
    }

    constexpr iterator insert(const_iterator pos, CharT ch) {
        ensure_capacity(m_length + 1);
        iterator dst = iterator(pos);
        Traits::move(dst + 1, pos, end() - pos);
        m_ptr[pos - begin()] = ch;
        m_length++;
        null_terminate();
        return iterator(pos);
    }

    constexpr iterator insert(const_iterator pos, SizeType count, CharT ch) {
        ensure_capacity(m_length + count);
        iterator dst = iterator(pos);
        Traits::move(dst + count, pos, end() - pos);
        ptrdiff_t diff = pos - begin();
        for (int idx = 0; idx < count; idx++) {
            m_ptr[diff + idx] = ch;
        }
        m_length += count;
        null_terminate();
        return iterator(pos);
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        BasicString str(first, last);
        ptrdiff_t diff = pos - begin();
        insert(diff, str, 0, str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return iterator(pos);
    }

    constexpr iterator insert(const_iterator pos, std::initializer_list<CharT> ilist) {
        BasicString str(ilist);
        ptrdiff_t diff = pos - begin();
        insert(diff, str, 0, str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return iterator(pos);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &insert(SizeType index, const StringViewLikeT &t) {
        insert(index, t.data(), t.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &insert(SizeType index, const StringViewLikeT &t, 
        SizeType index_str, SizeType count = npos) {
        insert(index, t.data(), index_str, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // operators ==================================================================================

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
        append(str.data(), str.length());
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

    friend constexpr bool operator==(const BasicString &a, const BasicString &b) {
        if (a.length() != b.length()) {
            return false;
        }
        if (a.length() == 0) {
            return true;
        }
        return Traits::compare(a.data(), b.data(), a.length()) == 0;
    }

    friend constexpr bool operator!=(const BasicString &a, const BasicString &b) {
        return !(a == b);
    }

    friend constexpr bool operator<(const BasicString &a, const BasicString &b) {
        SizeType len = std::min(a.length(), b.length());
        int cmp = Traits::compare(a.data(), b.data(), len);
        return cmp != 0 ? cmp : (a.length() < b.length());
    }

    friend constexpr bool operator<=(const BasicString &a, const BasicString &b) {
        return operator==(a, b) || operator<(a, b); 
    }

    friend constexpr bool operator>(const BasicString &a, const BasicString &b) {
        return !(a < b); 
    }

    friend constexpr bool operator>=(const BasicString &a, const BasicString &b) {
        return operator==(a, b) || operator>(a, b); 
    }

    operator std::basic_string<CharT, std::char_traits<CharT>>() const noexcept {
        return std::basic_string(data(), length());
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    operator std::string() const noexcept {
        return std::basic_string(c_str(), length());
    }

    constexpr operator BasicStringView() const noexcept {
        return BasicStringView(data(), length());
    }

    // substrings =================================================================================

    constexpr BasicString substr(SizeType pos = 0, SizeType count = npos) const {
        ASSERT(pos < m_length);
        return BasicString(data(), pos, count);
    }

    BasicStringView substrview(SizeType pos = 0, SizeType count = npos) const noexcept {
        ASSERT(pos < m_length);
        return BasicStringView(data() + pos, count);
    }

    // iterators ==================================================================================

    constexpr iterator begin() const {
        return m_ptr;
    }

    constexpr iterator end() const {
        return m_ptr + m_length;
    }

    constexpr const_iterator cbegin() const {
        return m_ptr;
    }

    constexpr const_iterator cend() const {
        return m_ptr + m_length;
    }

    constexpr reverse_iterator rbegin() const {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() const {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator crend() const {
        return reverse_iterator(begin());
    }

private:
    void reset() {
        m_ptr = m_buf;
        m_length = 0;
        m_capacity = InlineCapacity;
    }

    void grow(SizeType new_capacity) {
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

template <typename CharT, SizeType S, class Traits>
std::ostream &operator<<(std::ostream &os, const BasicString<CharT, S, Traits> &str)
{
    os.write(str.data(), str.length());
    return os;
}

using String = BasicString<char, BasicStringDefaultSize>;

}  // namespace UU


namespace std
{
    template <typename CharT, UU::SizeType S>
    struct less<UU::BasicString<CharT, S, std::char_traits<CharT>>>
    {
        using StringT = UU::BasicString<CharT, S, std::char_traits<CharT>>;
        bool operator()(const StringT &lhs, const StringT &rhs) const {
            return lhs < rhs;
        }
    };
}

#endif  // UU_STRING_H

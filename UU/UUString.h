//
// UUString.h
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

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <stdlib.h>

#include <UU/Assertions.h>
#include <UU/Compiler.h>
#include <UU/IteratorWrapper.h>
#include <UU/MathLike.h>
#include <UU/Types.h>

#define UU_STRING_THROWS_EXCEPTIONS 0

namespace UU {

// forward declarations ===========================================================================

template <typename T> class Span;
class TextRef;

// string size ====================================================================================

static constexpr Size BasicStringDefaultInlineCapacity = 72;

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

template <typename CharT, Size S = BasicStringDefaultInlineCapacity, 
    typename Traits = std::char_traits<CharT>>
class BasicString
{
public:
    // using ======================================================================================

    using CharType = CharT;
    using TraitsType = Traits;
    using iterator = IteratorWrapper<CharT *>;
    using const_iterator = IteratorWrapper<const CharT *>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using BasicStringView = std::basic_string_view<CharT, std::char_traits<CharT>>;

    // constants ==================================================================================

    static constexpr Size InlineCapacity = S;
    static constexpr const Size npos = SizeMax;
    static constexpr CharT empty_value = 0;

    // public guts inspection =====================================================================

    template <bool B = true> constexpr bool is_using_inline_buffer() const { 
        return (m_ptr == const_cast<CharT *>(m_buf)) == B; 
    }

    template <bool B = true> constexpr bool is_using_allocated_buffer() const { 
        return (!(is_using_inline_buffer())) == B; 
    }

private:
    // internal helpers ===========================================================================

#define UU_STRING_ASSERT_NULL_TERMINATED \
    do { \
        ASSERT_WITH_MESSAGE(m_ptr[m_length] == '\0', "string not null terminated"); \
    } while (0)

    UU_ALWAYS_INLINE CharT *ptr() const { return m_ptr; }

    UU_ALWAYS_INLINE constexpr void null_terminate() {
        ensure_capacity(m_length);
        m_ptr[m_length] = '\0';
    }

    UU_ALWAYS_INLINE 
    constexpr void ensure_capacity(Size new_capacity) {
        new_capacity++; // room for null termination
        if (new_capacity <= InlineCapacity) {
            m_capacity = InlineCapacity;
            return;
        }
    
        if (new_capacity <= m_capacity) {
            return;
        }
        
        grow(new_capacity);
    }

    UU_ALWAYS_INLINE CharT return_empty_or_throw_out_of_range(Size pos) const { 
#if UU_STRING_THROWS_EXCEPTIONS
        BasicString<char, std::char_traits<char>, 64> msg("String access out of range: ");
        msg.append_as_string(pos);
        throw std::out_of_range(msg);
#else
        return empty_value;
#endif
    }

    UU_ALWAYS_INLINE BasicString &return_this_or_throw_out_of_range(Size pos) { 
#if UU_STRING_THROWS_EXCEPTIONS
        BasicString<char, std::char_traits<char>, 64> msg("String access out of range: ");
        msg.append_as_string(pos);
        throw std::out_of_range(msg);
#else
        return *this;
#endif
    }

    UU_ALWAYS_INLINE BasicString return_string_or_throw_out_of_range(Size pos) const { 
#if UU_STRING_THROWS_EXCEPTIONS
        BasicString<char, std::char_traits<char>, 64> msg("String access out of range: ");
        msg.append_as_string(pos);
        throw std::out_of_range(msg);
#else
        return BasicString();
#endif
    }

    UU_ALWAYS_INLINE Size return_zero_or_throw_out_of_range(Size pos) const { 
#if UU_STRING_THROWS_EXCEPTIONS
        BasicString<char, std::char_traits<char>, 64> msg("String access out of range: ");
        msg.append_as_string(pos);
        throw std::out_of_range(msg);
#else
        return 0;
#endif
    }

    template <bool B = true> constexpr bool is_same_string(const BasicString &other) const { 
        return (ptr() == other.ptr()) == B; 
    }

    template <bool B = true> constexpr bool are_same_strings(const BasicString &a, const BasicString &b) const { 
        return a.is_same_string<B>(b); 
    }

    UU_ALWAYS_INLINE iterator unconst_copy(const_iterator it) { 
        return iterator(const_cast<CharT *>(it.base())); 
    }

    UU_ALWAYS_INLINE void reset() {
        m_ptr = m_buf;
        clear();
        m_capacity = InlineCapacity;
    }

public:
    // constructing ===============================================================================

    constexpr BasicString() noexcept { null_terminate(); }
    
    constexpr explicit BasicString(Size capacity) {
        ensure_capacity(capacity);
        null_terminate();
    }
    
    constexpr BasicString(Size count, CharT c) {
        assign(count, c);
    }

    constexpr BasicString(const BasicString &other, Size pos, Size count = npos) {
        assign(other, pos, std::min(count, other.length() - pos));
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString(const char *ptr, Size length) {
        assign(ptr, length);
    }

    constexpr BasicString(const CharT *ptr, Size length) {
        assign(ptr, length);
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString(const char *ptr) {
        assign(ptr, strlen(ptr));
    }

    constexpr BasicString(const CharT *ptr) {
        assign(ptr, Traits::length(ptr));
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr BasicString(InputIt first, InputIt last) {
        assign(first, last);
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString(const std::string &str) {
        assign(str.data(), str.length());
    }

    constexpr BasicString(const std::basic_string<CharT> &str) {
        assign(str.data(), str.length());
    }

    constexpr BasicString(const BasicString &other) {
        assign(other.data(), other.length());
    }

    constexpr BasicString(const std::filesystem::path &path) {
        assign(path.string());
    }

    constexpr BasicString(BasicString &&other) {
        if (other.is_using_allocated_buffer()) {
            m_ptr = other.m_ptr;
            m_length = other.length();
            m_capacity = other.capacity();
            other.reset();
        }
        else {
            assign(other.data(), other.length());
        }
    }

    constexpr BasicString(std::initializer_list<CharT> ilist) {
        assign(ilist);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString(const StringViewLikeT &str) {
        assign(str);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString(const StringViewLikeT &str, Size pos, Size count = npos) {
        assign(str, pos, count);
    }

    // destructor =================================================================================

    constexpr ~BasicString() {
        if (is_using_allocated_buffer()) {
            free(m_ptr);
        }
    }

    // accessors ==================================================================================

    constexpr CharT *data() const { return m_ptr; }
    constexpr CharT *data() { return m_ptr; }
    constexpr Size length() const { return m_length; }
    constexpr Size size() const { return m_length; }
    constexpr Size max_size() const noexcept { return std::distance(begin(), end()); }
    constexpr Size capacity() const { return m_capacity; }
    constexpr CharT& front() { return m_ptr[0]; }
    constexpr CharT& front() const { return m_ptr[0]; }
    constexpr CharT& back() { return m_ptr[m_length - 1]; }
    constexpr CharT& back() const { return m_ptr[m_length - 1]; }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr const char *c_str() const noexcept { return reinterpret_cast<const char *>(data()); }

    template <class CharX = CharT, std::enable_if_t<IsByteSized<CharX>, int> = 0>
    constexpr const char *c_str() const noexcept { return data(); }

    template <bool B = true> constexpr bool is_empty() const { return (m_length == 0) == B; }
    constexpr bool empty() const { return is_empty(); }
    
    constexpr CharT at(Size index) {
        if (LIKELY(m_length > index)) {
            return m_ptr[index];
        }
        else {
            return return_empty_or_throw_out_of_range(index);
        }
    }
    
    constexpr const CharT &at(Size index) const {
        if (LIKELY(m_length > index)) {
            return m_ptr[index];
        }
        else {
            return return_empty_or_throw_out_of_range(index);
        }
    }

    // resizing ===================================================================================

    void reserve(Size length) { ensure_capacity(length); }
    constexpr void clear() { resize(0); }

    constexpr void resize(Size length) { 
        ensure_capacity(length);
        m_length = length; 
        null_terminate();
    }

    constexpr void resize(Size count, CharT c) {
        resize(0);
        append(count, c);
        UU_STRING_ASSERT_NULL_TERMINATED;
    }

    constexpr void shrink_to_fit() {
        if (is_using_inline_buffer()) {
            return;
        }

        if (length() < InlineCapacity) {
            CharT *old_ptr = m_ptr;
            m_ptr = m_buf;
            Traits::copy(m_ptr, old_ptr, length());
            free(old_ptr);
            null_terminate();
            ASSERT(is_using_inline_buffer());
            return;
        }

        Size shrink_length = ceil_to_page_size(length());
        if (shrink_length == ceil_to_page_size(capacity())) {
            return;
        }
        CharT *old_ptr = m_ptr;
        Size amt = (shrink_length * sizeof(CharT)) + 1;
        m_ptr = static_cast<CharT *>(malloc(amt));
        Traits::copy(m_ptr, old_ptr, length());
        free(old_ptr);
        null_terminate();
        ASSERT(is_using_allocated_buffer());
    }

    // push and pop ===============================================================================

    constexpr void push_back(CharT c) {
        append(c);
        UU_STRING_ASSERT_NULL_TERMINATED;
    }

    constexpr void pop_back() {
        erase(end() - 1);
        UU_STRING_ASSERT_NULL_TERMINATED;
    }

    constexpr void push(CharT c) {
        append(c);
        UU_STRING_ASSERT_NULL_TERMINATED;
    }

    constexpr CharT pop() {
        if (UNLIKELY(length() == 0)) {
            return empty_value;
        }
        CharT c = back();
        pop_back();
        UU_STRING_ASSERT_NULL_TERMINATED;
        return c;
    }

    // assigning ==================================================================================
    // all calls in this section must end with null_terminate() or UU_STRING_ASSERT_NULL_TERMINATED

    constexpr BasicString &assign(Size count, CharT c) {
        clear();
        append(count, c);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr BasicString &assign(const BasicString &str) {
        clear();
        append(str);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString &assign(const std::string &str) {
        clear();
        append(str.data(), str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr BasicString &assign(const BasicString &str, Size pos, Size count = npos) {
        clear();
        append(str, pos, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr BasicString &assign(BasicString &&str) noexcept {
        if (str.is_using_allocated_buffer()) {
            m_ptr = str.m_ptr;
            m_length = str.length();
            m_capacity = str.capacity();
        }
        else {
            assign(str.data(), str.length());
        }
        str.reset();
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString &assign(const char *ptr, Size length) {
        clear();
        append(ptr, length);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr BasicString &assign(const CharT *ptr, Size length) {
        clear();
        append(ptr, length);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr BasicString &assign(const CharT *ptr) {
        clear();
        append(ptr);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString &assign(const char *ptr) {
        clear();
        append(ptr);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr BasicString &assign(InputIt first, InputIt last) {
        clear();
        append(first, last);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr BasicString &assign(std::initializer_list<CharT> ilist) {
        clear();
        append(ilist);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &assign(const StringViewLikeT &t) {
        clear();
        append(t);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &assign(const StringViewLikeT &t, Size pos, Size count = npos) {
        clear();
        append(t, pos, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // appending ==================================================================================
    // all calls in this section must end with null_terminate() or UU_STRING_ASSERT_NULL_TERMINATED

    constexpr BasicString &append(Size count, CharT c) {
        ensure_capacity(m_length + count);
        for (Size idx = 0; idx < count; idx++) {
            m_ptr[m_length + idx] = c;
        }
        m_length += count;
        null_terminate();
        return *this;
    }

    constexpr BasicString &append(const BasicString &str) {
        append(str.data(), str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString &append(const std::string &str) {
        append(str.data(), str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr BasicString &append(const BasicString &str, Size pos, Size count = npos) {
        insert(length(), str, pos, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString &append(const char *ptr, Size length) {
        ensure_capacity(m_length + length);
        for (Size idx = 0; idx < length; idx++) {
            m_ptr[m_length + idx] = ptr[idx];
        }
        m_length += length;
        null_terminate();
        return *this;
    }
    
    constexpr BasicString &append(const CharT *ptr, Size length) {
        ensure_capacity(m_length + length);
        Traits::copy(m_ptr + m_length, ptr, length);
        m_length += length;
        null_terminate();
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString &append(const char *ptr) {
        Size length = strlen(ptr);
        ensure_capacity(m_length + length);
        for (Size idx = 0; idx < length; idx++) {
            m_ptr[idx + length] = ptr[idx];
        }
        m_length += length;
        null_terminate();
        return *this;
    }

    constexpr BasicString &append(const CharT *ptr) {
        Size length = Traits::length(ptr);
        ensure_capacity(m_length + length);
        Traits::copy(m_ptr + m_length, ptr, length);
        m_length += length;
        null_terminate();
        return *this;
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr BasicString &append(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            ensure_capacity(m_length);
            m_ptr[m_length] = *it;
            m_length++;
        }
        null_terminate();
        return *this;
    }

    constexpr BasicString &append(std::initializer_list<CharT> ilist) {
        for (auto it = ilist.begin(); it != ilist.end(); ++it) {
            ensure_capacity(m_length);
            m_ptr[m_length] = *it;
            m_length++;
        }
        null_terminate();
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &append(const StringViewLikeT &t) {
        append(t.data(), t.length());
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &append(const StringViewLikeT &t, Size pos, Size count = npos) {
        BasicStringView v(t.substr(pos, count));
        append(v.data(), v.length());
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr BasicString &append(char c) {
        ensure_capacity(m_length);
        m_ptr[m_length] = c;
        m_length++;
        null_terminate();
        return *this;
    }

    constexpr BasicString &append(CharT c) {
        ensure_capacity(m_length);
        m_ptr[m_length] = c;
        m_length++;
        null_terminate();
        return *this;
    }

    BasicString &append(const Span<int> &);
    BasicString &append(const Span<Size> &);
    BasicString &append(const Span<Int64> &);
    BasicString &append(const TextRef &);

    template <typename N>
    BasicString &append_as_string(N val) {
        char buf[MaximumInteger64LengthAsString];
        char *ptr = buf;
        integer_to_string(val, ptr);
        Size len = strlen(ptr);
        ensure_capacity(m_length + len);
        append(ptr, len);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // inserting ==================================================================================
    // all calls in this section must end with null_terminate() or UU_STRING_ASSERT_NULL_TERMINATED

    constexpr BasicString &insert(Size index, Size count, CharT c) {
        ensure_capacity(m_length + count);
        iterator pos = begin() + index;
        iterator dst = pos + count;
        Traits::move(dst, pos, end() - pos);
        ptrdiff_t diff = pos - begin();
        for (Size idx = 0; idx < count; idx++) {
            m_ptr[diff + idx] = c;
        }
        m_length += count;
        null_terminate();
        return *this;
    }

    constexpr BasicString &insert(Size index, const CharT *s) {
        return insert(index, s, strlen(s));
    }

    constexpr BasicString &insert(Size index, const CharT *s, Size count) {
        ensure_capacity(m_length + count);
        iterator pos = begin() + index;
        iterator dst = pos + count;
        Traits::move(dst, pos, end() - pos);
        Traits::copy(pos, s, count);
        m_length += count;
        null_terminate();
        return *this;
    }

    constexpr BasicString &insert(Size index, const BasicString& str) {
        insert(index, str, 0, str.length()); 
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;      
    }

    constexpr BasicString &insert(Size index, const BasicString &str, Size index_str, Size count = npos) {
        Size ecount = std::min(count, str.length() - index_str);
        ensure_capacity(m_length + ecount);
        iterator pos = begin() + index;
        iterator dst = pos + ecount;
        Traits::move(dst, pos, end() - pos);
        Traits::copy(pos, str.begin() + index_str, ecount);
        m_length += ecount;
        null_terminate();
        return *this;
    }

    constexpr iterator insert(const_iterator pos, CharT c) {
        ensure_capacity(m_length);
        iterator dst = unconst_copy(pos);
        Traits::move(dst + 1, pos, end() - pos);
        m_ptr[pos - begin()] = c;
        m_length++;
        null_terminate();
        return dst;
    }

    constexpr iterator insert(const_iterator pos, Size count, CharT c) {
        ensure_capacity(m_length + count);
        iterator dst = unconst_copy(pos);
        Traits::move(dst + count, pos, end() - pos);
        ptrdiff_t diff = pos - begin();
        for (Size idx = 0; idx < count; idx++) {
            m_ptr[diff + idx] = c;
        }
        m_length += count;
        null_terminate();
        return dst;
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        ptrdiff_t count = last - first;
        ensure_capacity(m_length + count);
        iterator dst = unconst_copy(pos);
        Traits::move(dst + count, pos, end() - pos);
        ptrdiff_t offset = pos - begin(); 
        for (Size idx = 0; idx < count; idx++) {
            m_ptr[offset + idx] = *(first + idx);
        }
        m_length += count;
        null_terminate();
        return dst;
    }

    constexpr iterator insert(const_iterator pos, std::initializer_list<CharT> ilist) {
        ptrdiff_t count = ilist.size();
        ensure_capacity(m_length + count);
        iterator dst = unconst_copy(pos);
        Traits::move(dst + count, pos, end() - pos);
        ptrdiff_t offset = pos - begin(); 
        for (Size idx = 0; idx < count; idx++) {
            m_ptr[offset + idx] = *(ilist.begin() + idx);
        }
        m_length += count;
        null_terminate();
        return dst;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &insert(Size index, const StringViewLikeT &t) {
        insert(index, t.data(), t.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &insert(Size index, const StringViewLikeT &t, Size index_str, Size count = npos) {
        insert(index, t.data(), index_str, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // erasing ===================================================================================

    constexpr BasicString &erase(Size index = 0, Size count = npos) {
        if (UNLIKELY(index > length())) {
            return_this_or_throw_out_of_range(index);
        }
        Size amt = std::min(count, length() - index);
        Size rem = length() - amt;
        Traits::move(m_ptr + index, m_ptr + index + amt, rem);
        m_length -= amt;
        null_terminate();
        return *this;
    }

    constexpr iterator erase(const_iterator pos) {
        if (pos == end()) {
            return end();
        }
        Size rem = end() - pos - 1;
        ptrdiff_t diff = pos - begin();
        Traits::move(begin() + diff, begin() + diff + 1, rem);
        m_length -= 1;
        null_terminate();
        return begin() + diff;
    }

    constexpr iterator erase(const_iterator first, const_iterator last) {
        if (first == end()) {
            return end();
        }
        if (first >= last) {
            last = end();
        }
        ptrdiff_t first_idx = first - begin();
        ptrdiff_t last_idx = last - begin();
        Size amt = last_idx - first_idx;
        ptrdiff_t rem = length() - last_idx;
        Traits::move(begin() + first_idx, begin() + last_idx, rem);
        m_length -= amt;
        null_terminate();
        return unconst_copy(first);
    }

    // starts_with ================================================================================

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr bool starts_with(const StringViewLikeT &t) const {
        if (t.length() == 0) {
            return true;
        }
        if (t.length() > length()) {
            return false;
        }
        return Traits::compare(data(), t.data(), t.length()) == 0;
    }

    constexpr bool starts_with(CharT c) const noexcept {
        return length() > 0 && Traits::eq(m_ptr[0], c);
    }

    constexpr bool starts_with(const CharT *s) const {
        Size len = Traits::length(s);
        return length() >= len && Traits::compare(data(), s, len) == 0;
    }

    // ends_with ==================================================================================

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr bool ends_with(const StringViewLikeT &t) const {
        if (t.length() == 0) {
            return true;
        }
        if (t.length() > length()) {
            return false;
        }
        Size pos = length() - t.length();
        return Traits::compare(data() + pos, t.data(), t.length()) == 0;
    }

    constexpr bool ends_with(CharT c) const noexcept {
        return length() > 0 && Traits::eq(m_ptr[length() - 1], c);
    }

    constexpr bool ends_with(const CharT *s) const {
        Size len = Traits::length(s);
        Size pos = length() - len;
        return length() >= len && Traits::compare(data() + pos, s, len) == 0;
    }

    // contains ===================================================================================

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr bool contains(const StringViewLikeT &t) const noexcept {
        return find(t) != npos;
    }

    constexpr bool contains(CharT c) const noexcept {
        return find(c) != npos;
    }

    constexpr bool contains(const CharT *s) const {
        return find(s) != npos;
    }

    // finding ====================================================================================

    constexpr Size find(const BasicString &str, Size pos = 0) const noexcept {
        return find(BasicStringView(str), pos);
    }

    constexpr Size find(const CharT *s, Size pos, Size count) const {
        return find(BasicStringView(s, count), pos);
    }

    constexpr Size find(const CharT *s, Size pos = 0) const {
        return find(BasicStringView(s, Traits::length(s)), pos);
    }

    constexpr Size find(CharT c, Size pos = 0) const noexcept {
        for (Size idx = pos; idx < length(); idx++) {
            if (Traits::eq(m_ptr[idx], c)) {
                return idx;
            }
        }
        return npos;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr Size find(const StringViewLikeT &t, Size pos = 0) const noexcept {
        if (t.length() == 0) {
            return pos;
        }
        if (t.length() > length() || pos > length()) {
            return npos;
        }
        else if (t.length() == 1) {
            return find(t[0], pos);
        }
        else if (t.length() == 2) {
            const CharT a = t[0];
            const CharT b = t[1];
            for (Size idx = pos; idx < length() - 1; idx++) {
                if (Traits::eq(m_ptr[idx], a) && Traits::eq(m_ptr[idx + 1], b)) {
                    return idx;
                }
            }
            return npos;
        }
        else {
            const CharT a = t[0];
            for (Size idx = pos; idx <= length() - t.length(); idx++) {
                if (Traits::eq(m_ptr[idx], a) && Traits::compare(m_ptr + idx, t.data(), t.length()) == 0) {
                    return idx;
                }
            }
            return npos;
        }
    }

    constexpr Size find_boyer_moore(const BasicString &str, Size pos = 0) const {
        return find_boyer_moore(BasicStringView(str), pos);
    }

    constexpr Size find_boyer_moore(const CharT *s, Size pos, Size count) const {
        return find_boyer_moore(BasicStringView(s, count), pos);
    }

    constexpr Size find_boyer_moore(const CharT *s, Size pos = 0) const {
        return find_boyer_moore(BasicStringView(s, Traits::length(s)), pos);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr Size find_boyer_moore(const StringViewLikeT &t, Size pos = 0) const {
        if (t.length() == 0) {
            return pos;
        }
        if (t.length() > length() || pos > length()) {
            return npos;
        }
        else if (t.length() == 1) {
            return find(t[0], pos);
        }
        else if (t.length() == 2) {
            const CharT a = t[0];
            const CharT b = t[1];
            for (Size idx = pos; idx < length() - 1; idx++) {
                if (Traits::eq(m_ptr[idx], a) && Traits::eq(m_ptr[idx + 1], b)) {
                    return idx;
                }
            }
            return npos;
        }
        else {
            const auto searcher = std::boyer_moore_searcher(t.begin(), t.end());
            auto it = std::search(begin() + pos, end(), searcher);
            return it != end() ? it - begin() : npos;
        }
    }

    // rfind =======================================================================================

    constexpr Size rfind( const BasicString &str, Size pos = npos) const noexcept {
        return str.length() == 0 ? std::min(pos, length()) : rfind(BasicStringView(str), pos);
    }

    constexpr Size rfind(const CharT *s, Size pos, Size count) const {
        return count == 0 ? std::min(pos, length()) : rfind(BasicStringView(s, count), pos);
    }

    constexpr Size rfind(const CharT *s, Size pos = npos) const {
        Size len = Traits::length(s);
        return len == 0 ? std::min(pos, length()) : rfind(BasicStringView(s, len), pos);
    }

    constexpr Size rfind(CharT c, Size pos = npos) const noexcept {
        if (length() == 0) {
            return npos;
        }
        Size idx = std::min(pos, length() - 1);
        for (;;) {
            if (Traits::eq(m_ptr[idx], c)) {
                return idx;
            }
            if (idx == 0) {
                break;
            }
            idx--;
        }
        return npos;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr Size rfind(const StringViewLikeT &t, Size pos = npos) const noexcept {
        if (length() == 0) {
            return npos;
        }
        if (t.length() == 0) {
            return std::min(pos, length());
        }
        if (t.length() > length()) {
            return npos;
        }
        else if (t.length() == 1) {
            return rfind(t[0], pos);
        }
        else if (t.length() == 2) {
            const CharT a = t[0];
            const CharT b = t[1];
            Size idx = std::min(pos, length() - t.length());
            for (;;) {
                if (Traits::eq(m_ptr[idx], a) && Traits::eq(m_ptr[idx + 1], b)) {
                    return idx;
                }
                if (idx == 0) {
                    break;
                }
                idx--;
            }
            return npos;
        }
        else {
            const CharT a = t[0];
            Size idx = std::min(pos, length() - t.length());
            for (;;) {
                if (Traits::eq(m_ptr[idx], a) && Traits::compare(m_ptr + idx, t.data(), t.length()) == 0) {
                    return idx;
                }
                if (idx == 0) {
                    break;
                }
                idx--;
            }
            return npos;
        }
    }

    // find_first_of ==============================================================================

    constexpr Size find_first_of(const BasicString &str, Size pos = 0) const noexcept {
        return find_first_of(BasicStringView(str), pos);
    }

    constexpr Size find_first_of(const CharT *s, Size pos, Size count) const {
        return find_first_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_first_of(const CharT *s, Size pos = 0) const {
        return find_first_of(BasicStringView(s, Traits::length(s)), pos);
    }

    constexpr Size find_first_of(CharT c, Size pos = 0) const noexcept {
        return find(c, pos);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr Size find_first_of(const StringViewLikeT &t, Size pos = 0) const noexcept {
        if (pos > length()) {
            return npos;
        }
        for (Size idx = pos; idx < length(); idx++) {
            for (Size cidx = 0; cidx < t.length(); cidx++) {
                if (Traits::eq(m_ptr[idx], t[cidx])) {
                    return idx;
                }
            }
        }
        return npos;
    }

    // find_first_not_of ==========================================================================

    constexpr Size find_first_not_of(const BasicString &str, Size pos = 0) const noexcept {
        return find_first_not_of(BasicStringView(str), pos);
    }

    constexpr Size find_first_not_of(const CharT *s, Size pos, Size count) const {
        return find_first_not_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_first_not_of(const CharT *s, Size pos = 0) const {
        return find_first_not_of(BasicStringView(s, Traits::length(s)), pos);
    }

    constexpr Size find_first_not_of(CharT c, Size pos = 0) const noexcept {
        if (pos > length()) {
            return npos;
        }
        for (Size idx = pos; idx < length(); idx++) {
            if (!Traits::eq(m_ptr[idx], c)) {
                return idx;
            }
        }
        return npos;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr Size find_first_not_of(const StringViewLikeT &t, Size pos = 0) const noexcept {
        if (pos > length()) {
            return npos;
        }
        for (Size idx = pos; idx < length(); idx++) {
            bool match = false;
            for (Size cidx = 0; cidx < t.length(); cidx++) {
                if (Traits::eq(m_ptr[idx], t[cidx])) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                return idx;
            }
        }
        return npos;
    }

    // find_last_of ==============================================================================

    constexpr Size find_last_of(const BasicString &str, Size pos = npos) const noexcept {
        return find_last_of(BasicStringView(str), pos);
    }

    constexpr Size find_last_of(const CharT *s, Size pos, Size count) const {
        return find_last_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_last_of(const CharT *s, Size pos = npos) const {
        return find_last_of(BasicStringView(s, Traits::length(s)), pos);
    }

    constexpr Size find_last_of(CharT c, Size pos = npos) const noexcept {
        return rfind(c, pos);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr Size find_last_of(const StringViewLikeT &t, Size pos = npos) const noexcept {
        if (length() == 0) {
            return npos;
        }
        if (t.length() == 0) {
            return npos;
        }
        Size idx = std::min(pos, length() - 1);
        for (;;) {
            for (Size cidx = 0; cidx < t.length(); cidx++) {
                if (Traits::eq(m_ptr[idx], t[cidx])) {
                    return idx;
                }
            }
            if (idx == 0) {
                break;
            }
            idx--;
        }
        return npos;
    }

    // find_last_not_of ==============================================================================

    constexpr Size find_last_not_of(const BasicString& str, Size pos = npos) const noexcept {
        return find_last_not_of(BasicStringView(str), pos);
    }

    constexpr Size find_last_not_of(const CharT *s, Size pos, Size count) const {
        return find_last_not_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_last_not_of(const CharT *s, Size pos = npos) const {
        return find_last_not_of(BasicStringView(s, Traits::length(s)), pos);
    }

    constexpr Size find_last_not_of(CharT c, Size pos = npos) const noexcept {
        Size idx = std::min(pos, length() - 1);
        for (;;) {
            if (!Traits::eq(m_ptr[idx], c)) {
                return idx;
            }
            if (idx == 0) {
                break;
            }
            idx--;
        }
        return npos;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr Size find_last_not_of(const StringViewLikeT& t, Size pos = npos) const noexcept {
        if (length() == 0) {
            return npos;
        }
        Size idx = std::min(pos, length() - 1);
        if (t.length() == 0) {
            return idx;
        }
        for (;;) {
            bool match = false;
            for (Size cidx = 0; cidx < t.length(); cidx++) {
                if (Traits::eq(m_ptr[idx], t[cidx])) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                return idx;
            }
            if (idx == 0) {
                break;
            }
            idx--;
        }
        return npos;
    }

    // replace ====================================================================================

    constexpr BasicString &replace(Size pos, Size count, const BasicString &str) {
        return replace(pos, count, str, 0, str.length());
    }

    constexpr BasicString &replace(Size pos, Size count, const BasicString &str, Size pos2, Size count2 = npos) {
        return replace(pos, count, BasicStringView(str), pos2, count2);
    }

    constexpr BasicString &replace(const_iterator first, const_iterator last, const BasicString &str) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, str);
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr BasicString &replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, BasicStringView(first2, last2), 0, last2 - first2);
    }

    constexpr BasicString &replace(Size pos, Size count, const CharT *cstr, Size count2) {
        return replace(pos, count, BasicStringView(cstr, count2), 0, count2);
    }

    constexpr BasicString &replace(const_iterator first, const_iterator last, const CharT *cstr, Size count2) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, BasicStringView(cstr, count2), 0, count2);
    }

    constexpr BasicString &replace(Size pos, Size count, const CharT *cstr) {
        Size len = Traits::length(cstr);
        return replace(pos, count, BasicStringView(cstr, len), 0, len);
    }

    constexpr BasicString &replace(const_iterator first, const_iterator last, const CharT *cstr) {
        Size pos = first - begin();
        Size count = last - first;
        Size len = Traits::length(cstr);
        return replace(pos, count, BasicStringView(cstr, len), 0, len);
    }

    constexpr BasicString &replace(Size pos, Size count, Size count2, CharT c) {
        BasicString str(count2, c);
        return replace(pos, count, str);
    }

    constexpr BasicString &replace(const_iterator first, const_iterator last, Size count2, CharT c) {
        Size pos = first - begin();
        Size count = last - first;
        BasicString str(count2, c);
        return replace(pos, count, str);
    }

    constexpr BasicString &replace(const_iterator first, const_iterator last, std::initializer_list<CharT> ilist) {
        Size pos = first - begin();
        Size count = last - first;
        BasicString str(ilist);
        return replace(pos, count, str);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &replace(Size pos, Size count, const StringViewLikeT &t) {
        return replace(pos, count, t, 0, t.length());
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &replace(const_iterator first, const_iterator last, const StringViewLikeT &t) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, t, 0, t.length());
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &replace(Size pos, Size count, 
        const StringViewLikeT &t, Size pos2, Size count2 = npos) {
        StringViewLikeT et = t.substr(pos2, std::min(count2, t.length() - pos2));
        if (pos > length()) {
            return_this_or_throw_out_of_range(pos);
        }
        if (pos2 > et.length()) {
            return_this_or_throw_out_of_range(pos);
        }
        if (count + pos > length()) {
            count = length() - pos;
        }
        if (et.length() > count) {
            insert(pos, et.length() - count, 'x');
        }
        else if (count > et.length()) {
            erase(pos, count - et.length());
        }
        // overwrite
        Traits::copy(begin() + pos, et.data(), et.length());
        null_terminate();
        return *this;
    }

    // copy =======================================================================================

    constexpr Size copy(CharT* dst, Size count, Size pos = 0) const {
        if (UNLIKELY(pos > length())) {
            return_zero_or_throw_out_of_range(pos);
        }
        Size ecount = std::min(count, length() - pos);
        Traits::copy(dst, begin() + pos, ecount);
        return ecount;
    }

    // operator[] =================================================================================

    constexpr CharT operator[](Size index) {
        return m_ptr[index];
    }
    
    constexpr const CharT &operator[](Size index) const {
        return m_ptr[index];
    }

    // operator= ==================================================================================

    constexpr BasicString &operator=(const BasicString &other) {
        if (is_same_string(other)) {
            return *this;
        }
        return assign(other);
    }

    constexpr BasicString &operator=(BasicString &&other) noexcept {
        clear();
        if (other.is_using_allocated_buffer()) {
            if (is_using_allocated_buffer()) {
                free(m_ptr);
            }
            m_ptr = other.m_ptr;
            m_length = other.length();
            m_capacity = other.capacity();
        }
        else {
            assign(other.data(), other.length());
        }
        other.reset();
        return *this;
    }

    constexpr BasicString &operator=(const CharT *s) {
        return assign(s);
    }

    constexpr BasicString &operator=(CharT c) {
        return assign(c);
    }

    constexpr BasicString &operator=(std::initializer_list<CharT> ilist) {
        return assign(ilist);
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &operator=(const StringViewLikeT &t) {
        return assign(t);
    }

    // operator+= =================================================================================

    constexpr BasicString &operator+=(const BasicString &other) {
        return append(other.data(), other.length());
    }

    constexpr BasicString &operator+=(BasicString &&other) noexcept {
        return append(other.data(), other.length());
    }

    constexpr BasicString &operator+=(const CharT *s) {
        return append(s);
    }

    constexpr BasicString &operator+=(CharT c) {
        return append(c);
    }

    constexpr BasicString &operator+=(std::initializer_list<CharT> ilist) {
        return append(ilist);
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &operator+=(const StringViewLikeT &t) {
        return append(t.data(), t.length());
    }

    // comparison operators =======================================================================

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
        return Traits::compare(a.data(), b.data(), std::min(a.length(), b.length())) < 0;
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

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    operator std::string() const noexcept {
        return std::string(data(), length());
    }

    // conversion operators =======================================================================

    operator std::basic_string<CharT>() const noexcept {
        return std::basic_string<CharT>(data(), length());
    }

    constexpr operator BasicStringView() const noexcept {
        return BasicStringView(data(), length());
    }

    operator std::filesystem::path() const {
        return std::filesystem::path(BasicStringView(data(), length()));
    }

    // substrings =================================================================================

    constexpr BasicString substr(Size pos = 0, Size count = npos) const {
        if (UNLIKELY(pos > length())) {
            return_string_or_throw_out_of_range(pos);
        }
        return BasicString(data(), pos, std::min(count, length() - pos));
    }

    constexpr BasicStringView substrview(Size pos = 0, Size count = npos) const noexcept {
        if (UNLIKELY(pos > length())) {
            return_string_or_throw_out_of_range(pos);
        }
        return BasicStringView(data() + pos, std::min(count, length() - pos));
    }

    // iterators ==================================================================================

    constexpr iterator begin() {
        return iterator(m_ptr);
    }

    constexpr const_iterator begin() const {
        return iterator(m_ptr);
    }

    constexpr const_iterator cbegin() const {
        return iterator(m_ptr);
    }

    constexpr iterator end() {
        return iterator(m_ptr) + m_length;
    }

    constexpr const_iterator end() const {
        return iterator(m_ptr) + m_length;
    }

    constexpr const_iterator cend() const {
        return iterator(m_ptr) + m_length;
    }

    constexpr reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator crbegin() const {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crend() const {
        return reverse_iterator(begin());
    }

    // swap ==================================================================================

    constexpr void swap(BasicString &other) noexcept {
        // four cases
        if (is_using_inline_buffer() && other.is_using_inline_buffer()) {
            // m_buf
            CharT tmp_buf[InlineCapacity];
            Traits::copy(tmp_buf, m_buf, InlineCapacity);
            Traits::copy(m_buf, other.m_buf, InlineCapacity);
            Traits::copy(other.m_buf, tmp_buf, InlineCapacity);

            // m_ptr
            // no-op

            // m_length
            Size tmp_length = length();
            m_length = other.length();
            other.m_length = tmp_length;

            // m_capacity
            // no-op
        }
        else if (is_using_inline_buffer() && other.is_using_allocated_buffer()) {
            // m_buf and m_ptr
            CharT *tmp_ptr = other.m_ptr;
            Traits::copy(other.m_buf, m_buf, InlineCapacity);
            other.m_ptr = other.m_buf;
            m_ptr = tmp_ptr;

            // m_length
            Size tmp_length = length();
            m_length = other.length();
            other.m_length = tmp_length;

            // m_capacity
            Size tmp_capacity = capacity();
            m_capacity = other.capacity();
            other.m_capacity = tmp_capacity;
        }
        else if (is_using_allocated_buffer() && other.is_using_inline_buffer()) {
            // m_buf and m_ptr
            CharT *tmp_ptr = m_ptr;
            Traits::copy(m_buf, other.m_buf, InlineCapacity);
            m_ptr = m_buf;
            other.m_ptr = tmp_ptr;

            // m_length
            Size tmp_length = length();
            m_length = other.length();
            other.m_length = tmp_length;

            // m_capacity
            Size tmp_capacity = capacity();
            m_capacity = other.capacity();
            other.m_capacity = tmp_capacity;
        }
        else {
            ASSERT(is_using_allocated_buffer());
            ASSERT(other.is_using_allocated_buffer());
            // m_buf and m_ptr
            CharT *tmp_ptr = m_ptr;
            m_ptr = other.m_ptr;
            other.m_ptr = tmp_ptr;

            // m_length
            Size tmp_length = length();
            m_length = other.length();
            other.m_length = tmp_length;

            // m_capacity
            Size tmp_capacity = capacity();
            m_capacity = other.capacity();
            other.m_capacity = tmp_capacity;
        }
    }

    // extensions =================================================================================

    constexpr BasicString &replace_all(CharT a, CharT b) {
        Size pos = find(a);
        while (pos < length()) {
            m_ptr[pos] = b;
            pos = find(a, pos + 1);
        }
        null_terminate();
        return *this;
    }

    constexpr BasicString &replace_all(const CharT *a, const CharT *b) {
        BasicStringView av = BasicStringView(a, Traits::length(a));
        BasicStringView bv = BasicStringView(b, Traits::length(b));
        return replace_all(av, bv);
    }

    constexpr BasicString &replace_all(BasicString &a, BasicString &b) {
        return replace_all(a, b);
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, Traits>, int> = 0>
    constexpr BasicString &replace_all(StringViewLikeT &a, StringViewLikeT &b) {
        Size len = a.length();
        Size pos = find(a);
        while (pos < length()) {
            replace(pos, len, b);
            pos = find(a, pos + b.length());
        }
        null_terminate();
        return *this;
    }

    constexpr BasicString &chop() {
        if (length() > 0) {
            m_length--;
            null_terminate();
        }
        return *this;
    }

    template <bool B = true, class CharX = CharT, std::enable_if_t<IsByteSized<CharX>, int> = 0>
    static constexpr bool is_whitespace(CharT c) {
        bool result = false;
        switch (c) {
            case ' ':
            case '\f':
            case '\r':
            case '\n':
            case '\t':
            case '\v':
                result = true;
                break;
        }
        return result == B;
    }

    template <bool B = true, class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    static constexpr bool is_whitespace(CharT c) { 
        bool result = false;
        switch (c) {
            case L'\u0009':
            case L'\u000A':
            case L'\u000B':
            case L'\u000C':
            case L'\u000D':
            case L'\u0020':
            case L'\u1680':
            case L'\u180E':
            case L'\u2000':
            case L'\u2001':
            case L'\u2002':
            case L'\u2003':
            case L'\u2004':
            case L'\u2005':
            case L'\u2006':
            case L'\u2008':
            case L'\u2009':
            case L'\u200A':
            case L'\u2028':
            case L'\u2029':
            case L'\u205F':
            case L'\u3000':
                result = true;
                break;
        }
        return result == B;
    }

    constexpr BasicString &chomp() {
        if (length() > 0) {
            if (is_whitespace(back())) {
                chop();
                UU_STRING_ASSERT_NULL_TERMINATED;
            }
        }
        return *this;
    }


private:
    void grow(Size new_capacity) {
        while (m_capacity < new_capacity) {
            m_capacity *= 2;
        }
        Size amt = m_capacity * sizeof(CharT);
        if (is_using_allocated_buffer()) {
            m_ptr = static_cast<CharT *>(realloc(m_ptr, amt));
        }
        else {
            m_ptr = static_cast<CharT *>(malloc(amt));
            Traits::copy(m_ptr, m_buf, length());
        }
        null_terminate();
    }

    CharT m_buf[InlineCapacity] = { '\0' };
    CharT *m_ptr = m_buf;
    Size m_length = 0;
    Size m_capacity = InlineCapacity;
};

template <typename CharT, Size S, typename Traits>
std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const BasicString<CharT, S, Traits> &str)
{
    os.write(str.data(), str.length());
    return os;
}

// operator+ ======================================================================================

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &lhs,
    const BasicString<CharT, S, Traits> &rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &lhs, 
    const CharT* rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &lhs, CharT rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const CharT* lhs, 
    const BasicString<CharT, S, Traits> &rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(CharT lhs, const BasicString<CharT, S, Traits> &rhs) {
        BasicString<CharT, S, Traits> str(1, lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &&lhs,
    const BasicString<CharT, S, Traits> &&rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &&lhs,
    const BasicString<CharT, S, Traits> &rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &&lhs,
    const CharT *rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &&lhs, CharT rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const BasicString<CharT, S, Traits> &lhs,
    const BasicString<CharT, S, Traits> &&rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(const CharT *lhs, 
    const BasicString<CharT, S, Traits> &&rhs) {
        BasicString<CharT, S, Traits> str(lhs);
        str += rhs;
        return str;
}

template <typename CharT, Size S, typename Traits>
BasicString<CharT, S, Traits> operator+(CharT lhs,
    const BasicString<CharT, S, Traits> &&rhs) {
        BasicString<CharT, S, Traits> str(1, lhs);
        str += rhs;
        return str;
}

// String =========================================================================================

using String = BasicString<char, BasicStringDefaultInlineCapacity>;
using StringView = BasicString<char, BasicStringDefaultInlineCapacity>::BasicStringView;

}  // namespace UU


namespace std
{
    // Implement std::swap in terms of BasicString swap
    template <typename CharT, UU::Size S>
    UU_ALWAYS_INLINE void swap(UU::BasicString<CharT, S, std::char_traits<CharT>> &lhs, UU::BasicString<CharT, S, std::char_traits<CharT>> &rhs) {
        lhs.swap(rhs);
    }
    
    template <typename CharT, UU::Size S>
    struct less<UU::BasicString<CharT, S, std::char_traits<CharT>>>
    {
        using StringT = UU::BasicString<CharT, S, std::char_traits<CharT>>;
        bool operator()(const StringT &lhs, const StringT &rhs) const {
            return lhs < rhs;
        }
    };
}

#endif  // UU_STRING_H

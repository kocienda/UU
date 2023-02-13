//
// ProtoIStringForm.h
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

#ifndef UU_ISTRING_H
#define UU_ISTRING_H

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <format>
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
#include <UU/Allocator.h>
#include <UU/Compiler.h>
#include <UU/Context.h>
#include <UU/IteratorWrapper.h>
#include <UU/MathLike.h>
#include <UU/Types.h>

namespace UU {

class ProtoIStringBase
{
public:
    UU_ALWAYS_INLINE Size capacity() const { return m_capacity; }
    UU_ALWAYS_INLINE Size length() const { return m_length; }

    UU_NO_DISCARD bool is_empty() const { return m_length == 0; }
    UU_NO_DISCARD bool not_empty() const { return !is_empty(); }
    
protected:
    ProtoIStringBase() = delete;
    constexpr ProtoIStringBase(void *base, Size capacity) : m_base(base), m_capacity(capacity) {}

    UU_ALWAYS_INLINE void *base() const { return m_base; }
    UU_ALWAYS_INLINE void set_base(void *base) { m_base = base; }
    
    // Set the capacity data member, but does not handle actually changing the capacity.
    void set_capacity(Size capacity) {
        m_capacity = capacity;
    }
    
    // Set the string size, which must be less than or equal to the current capacity
    void set_length(Size length) {
        ASSERT_WITH_MESSAGE(length <= capacity(), 
            "length: %lu ; capacity: %lu", length, capacity());
        m_length = length;
    }

    // data members
    void *m_base;
    Size m_capacity = 0;
    Size m_length = 0;
};

// ================================================================================================
// IStringAlignmentAndSize
//
// Figure out the offset of the first element.
template <class CharT> struct IStringAlignmentAndSize {
    alignas(ProtoIStringBase) char m_align[sizeof(ProtoIStringBase)];
    alignas(CharT) char m_first_char[sizeof(CharT)];
};

// ================================================================================================
// ProtoIStringForm class
//

template <typename CharT, typename TraitsT = std::char_traits<CharT>>
class ProtoIStringForm : public ProtoIStringBase
{
public:
    // using ======================================================================================

    using CharType = CharT;
    using TraitsType = TraitsT;
    using iterator = IteratorWrapper<CharT *>;
    using const_iterator = IteratorWrapper<const CharT *>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using BasicStringView = std::basic_string_view<CharT, std::char_traits<CharT>>;

    // constants ==================================================================================

    static constexpr const Size npos = SizeMax;
    static constexpr CharT empty_value = 0;

    // public guts inspection =====================================================================

    template <bool B = true> constexpr bool is_using_inline_buffer() const { 
        return (data() == first_char_addr()) == B; 
    }

    template <bool B = true> constexpr bool is_using_allocated_buffer() const { 
        return (!(is_using_inline_buffer())) == B; 
    }

    // internal helpers ===========================================================================

    // Find the address of the first element.  For this pointer math to be valid
    // with small-size of 0 for CharT with lots of alignment, it's important that
    // IStringAlignmentAndSize is properly-aligned even for small-size of 0.
    constexpr void *first_char_addr() const {
        auto offset = offsetof(IStringAlignmentAndSize<CharT>, m_first_char);
        auto addr = reinterpret_cast<const char *>(this) + offset;
        return const_cast<void *>(reinterpret_cast<const void *>(addr));
    }
    // Space after 'm_first_char' is clobbered, do not add any instance vars after it.

#define UU_STRING_ASSERT_NULL_TERMINATED \
    do { \
        ASSERT_WITH_MESSAGE(data()[m_length] == '\0', "string not null terminated"); \
    } while (0)

#define UU_STRING_DATA_ASSERT_NULL_TERMINATED(data_ptr, length) \
    do { \
        ASSERT_WITH_MESSAGE(data_ptr[length] == '\0', "string not null terminated"); \
    } while (0)

    UU_ALWAYS_INLINE constexpr void null_terminate() {
        reserve(m_length);
        data()[m_length] = '\0';
    }

    UU_ALWAYS_INLINE CharT return_empty_or_throw_out_of_range(Size pos) const { 
        ASSERT_WITH_MESSAGE(false, "out of range: %lu", pos);
        return empty_value;
    }

    UU_ALWAYS_INLINE ProtoIStringForm &return_this_or_throw_out_of_range(Size pos) { 
        ASSERT_WITH_MESSAGE(false, "out of range: %lu", pos);
        return *this;
    }

    UU_ALWAYS_INLINE ProtoIStringForm return_string_or_throw_out_of_range(Size pos) const { 
        ASSERT_WITH_MESSAGE(false, "out of range: %lu", pos);
        return ProtoIStringForm();
    }

    UU_ALWAYS_INLINE Size return_zero_or_throw_out_of_range(Size pos) const { 
        ASSERT_WITH_MESSAGE(false, "out of range: %lu", pos);
        return 0;
    }

    template <bool B = true> constexpr bool is_same_string(const ProtoIStringForm &other) const { 
        return (data() == other.data()) == B; 
    }

    template <bool B = true> constexpr bool are_same_strings(const ProtoIStringForm &a, const ProtoIStringForm &b) const { 
        return a.is_same_string<B>(b); 
    }

    UU_ALWAYS_INLINE iterator unconst_copy(const_iterator it) { 
        return iterator(const_cast<CharT *>(it.base())); 
    }

    // Reset to inline storage, assuming that any dynamically-allocated memory is managed elsewhere.
    void reset_to_inline_storage() {
        this->set_base(first_char_addr());
        this->set_capacity(0); // FIXME: Setting capacity to 0 is suspect.
        this->set_length(0);
    }

    UU_ALWAYS_INLINE void reset() {
        reset_to_inline_storage();
    }

public:
    // accessors ==================================================================================

    constexpr CharT *data() const { return (CharT *)base(); }
    constexpr CharT *data() { return (CharT *)base(); }
    constexpr Size length() const { return m_length; }
    constexpr Size size() const { return m_length; }
    constexpr Size max_size() const noexcept { return std::distance(begin(), end()); }
    constexpr Size capacity() const { return m_capacity; }
    constexpr CharT& front() { return data()[0]; }
    constexpr CharT& front() const { return data()[0]; }
    constexpr CharT& back() { return data()[m_length - 1]; }
    constexpr CharT& back() const { return data()[m_length - 1]; }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr const char *c_str() const noexcept { return reinterpret_cast<const char *>(data()); }

    template <class CharX = CharT, std::enable_if_t<IsByteSized<CharX>, int> = 0>
    constexpr const char *c_str() const noexcept { return data(); }

    constexpr CharT at(Size index) {
        if (LIKELY(m_length > index)) {
            return data()[index];
        }
        else {
            return return_empty_or_throw_out_of_range(index);
        }
    }
    
    constexpr const CharT &at(Size index) const {
        if (LIKELY(m_length > index)) {
            return data()[index];
        }
        else {
            return return_empty_or_throw_out_of_range(index);
        }
    }

    // resizing ===================================================================================

    constexpr void reserve(Size new_capacity) { 
        new_capacity++; // room for null termination
        if (new_capacity <= m_capacity) {
            return;
        }
        grow(new_capacity);
        UU_STRING_ASSERT_NULL_TERMINATED;
    }

    constexpr void clear() {
        m_length = 0; 
        null_terminate();
    }

    constexpr void resize(Size length) { 
        reserve(length);
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

        Size shrink_length = ceil_to_page_size(length());
        if (shrink_length == ceil_to_page_size(capacity())) {
            return;
        }

        Memory old_mem = { data(), capacity() };
        Allocator &allocator = Context::get().allocator();
        Size amt = (shrink_length * sizeof(CharT)) + 1;
        Memory mem = allocator.alloc(amt);
        set_base(mem.ptr);
        set_capacity(mem.capacity);
        TraitsT::copy(data(), (CharT *)old_mem.ptr, length());
        allocator.dealloc(old_mem);
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

    constexpr ProtoIStringForm &assign(Size count, CharT c) {
        clear();
        append(count, c);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr ProtoIStringForm &assign(const ProtoIStringForm &str) {
        clear();
        append(str);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIStringForm &assign(const std::string &str) {
        clear();
        append(str.data(), str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr ProtoIStringForm &assign(const ProtoIStringForm &str, Size pos, Size count = npos) {
        clear();
        append(str, pos, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr ProtoIStringForm &assign(ProtoIStringForm &&str) noexcept {
        if (str.is_using_allocated_buffer()) {
            data() = str.data();
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
    constexpr ProtoIStringForm &assign(const char *ptr, Size length) {
        clear();
        append(ptr, length);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr ProtoIStringForm &assign(const CharT *ptr, Size length) {
        clear();
        append(ptr, length);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr ProtoIStringForm &assign(const CharT *ptr) {
        clear();
        append(ptr);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIStringForm &assign(const char *ptr) {
        clear();
        append(ptr);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr ProtoIStringForm &assign(InputIt first, InputIt last) {
        clear();
        append(first, last);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr ProtoIStringForm &assign(std::initializer_list<CharT> ilist) {
        clear();
        append(ilist);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &assign(const StringViewLikeT &t) {
        clear();
        append(t);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &assign(const StringViewLikeT &t, Size pos, Size count = npos) {
        clear();
        append(t, pos, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // appending ==================================================================================
    // all calls in this section must end with null_terminate() or UU_STRING_ASSERT_NULL_TERMINATED

    constexpr ProtoIStringForm &append(Size count, CharT c) {
        reserve(m_length + count);
        for (Size idx = 0; idx < count; idx++) {
            data()[m_length + idx] = c;
        }
        m_length += count;
        null_terminate();
        return *this;
    }

    constexpr ProtoIStringForm &append(const ProtoIStringForm &str) {
        append(str.data(), str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIStringForm &append(const std::string &str) {
        append(str.data(), str.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    constexpr ProtoIStringForm &append(const ProtoIStringForm &str, Size pos, Size count = npos) {
        insert(length(), str, pos, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIStringForm &append(const char *ptr, Size length) {
        reserve(m_length + length);
        for (Size idx = 0; idx < length; idx++) {
            data()[m_length + idx] = ptr[idx];
        }
        m_length += length;
        null_terminate();
        return *this;
    }
    
    constexpr ProtoIStringForm &append(const CharT *ptr, Size length) {
        reserve(m_length + length);
        TraitsT::copy(data() + m_length, ptr, length);
        m_length += length;
        null_terminate();
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIStringForm &append(const char *ptr) {
        Size length = strlen(ptr);
        reserve(m_length + length);
        for (Size idx = 0; idx < length; idx++) {
            data()[idx + length] = ptr[idx];
        }
        m_length += length;
        null_terminate();
        return *this;
    }

    constexpr ProtoIStringForm &append(const CharT *ptr) {
        Size length = TraitsT::length(ptr);
        reserve(m_length + length);
        TraitsT::copy(data() + m_length, ptr, length);
        m_length += length;
        null_terminate();
        return *this;
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr ProtoIStringForm &append(InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it) {
            reserve(m_length);
            data()[m_length] = *it;
            m_length++;
        }
        null_terminate();
        return *this;
    }

    constexpr ProtoIStringForm &append(std::initializer_list<CharT> ilist) {
        for (auto it = ilist.begin(); it != ilist.end(); ++it) {
            reserve(m_length);
            data()[m_length] = *it;
            m_length++;
        }
        null_terminate();
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &append(const StringViewLikeT &t) {
        append(t.data(), t.length());
        return *this;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &append(const StringViewLikeT &t, Size pos, Size count = npos) {
        BasicStringView v(t.substr(pos, count));
        append(v.data(), v.length());
        return *this;
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIStringForm &append(char c) {
        reserve(m_length);
        data()[m_length] = c;
        m_length++;
        null_terminate();
        return *this;
    }

    constexpr ProtoIStringForm &append(CharT c) {
        reserve(m_length);
        data()[m_length] = c;
        m_length++;
        null_terminate();
        return *this;
    }

    // ProtoIStringForm &append(const Spread<int> &);
    // ProtoIStringForm &append(const Spread<Size> &);
    // ProtoIStringForm &append(const Spread<Int64> &);
    // ProtoIStringForm &append(const TextRef &);

    template <typename N>
    ProtoIStringForm &append_as_string(N val) {
        char buf[MaximumInteger64LengthAsString];
        char *ptr = buf;
        integer_to_string(val, ptr);
        Size len = strlen(ptr);
        reserve(m_length + len);
        append(ptr, len);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // inserting ==================================================================================
    // all calls in this section must end with null_terminate() or UU_STRING_ASSERT_NULL_TERMINATED

    constexpr ProtoIStringForm &insert(Size index, Size count, CharT c) {
        reserve(m_length + count);
        iterator pos = begin() + index;
        iterator dst = pos + count;
        TraitsT::move(dst, pos, end() - pos);
        ptrdiff_t diff = pos - begin();
        for (Size idx = 0; idx < count; idx++) {
            data()[diff + idx] = c;
        }
        m_length += count;
        null_terminate();
        return *this;
    }

    constexpr ProtoIStringForm &insert(Size index, const CharT *s) {
        return insert(index, s, TraitsT::length(s));
    }

    constexpr ProtoIStringForm &insert(Size index, const ProtoIStringForm& str) {
        insert(index, str, 0, str.length()); 
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;      
    }

    constexpr ProtoIStringForm &insert(Size index, const ProtoIStringForm &str, Size index_str, Size count = npos) {
        Size ecount = std::min(count, str.length() - index_str);
        reserve(m_length + ecount);
        iterator pos = begin() + index;
        iterator dst = pos + ecount;
        TraitsT::move(dst, pos, end() - pos);
        TraitsT::copy(pos, str.begin() + index_str, ecount);
        m_length += ecount;
        null_terminate();
        return *this;
    }

    // constexpr ProtoIStringForm &insert(Size index, const CharT *s, Size count) {
    //     reserve(m_length + count);
    //     iterator pos = begin() + index;
    //     iterator dst = pos + count;
    //     TraitsT::move(dst, pos, end() - pos);
    //     TraitsT::copy(pos, s, count);
    //     m_length += count;
    //     null_terminate();
    //     return *this;
    // }

    constexpr ProtoIStringForm &insert(Size index, const CharT *s, Size index_str, Size count = npos) {
        Size length = TraitsT::length(s);
        Size ecount = std::min(count, length - index_str);
        reserve(m_length + ecount);
        iterator pos = begin() + index;
        iterator dst = pos + ecount;
        TraitsT::move(dst, pos, end() - pos);
        TraitsT::copy(pos, s + index_str, ecount);
        m_length += ecount;
        null_terminate();
        return *this;
    }

    constexpr iterator insert(const_iterator pos, CharT c) {
        reserve(m_length);
        iterator dst = unconst_copy(pos);
        TraitsT::move(dst + 1, pos, end() - pos);
        data()[pos - begin()] = c;
        m_length++;
        null_terminate();
        return dst;
    }

    constexpr iterator insert(const_iterator pos, Size count, CharT c) {
        reserve(m_length + count);
        iterator dst = unconst_copy(pos);
        TraitsT::move(dst + count, pos, end() - pos);
        ptrdiff_t diff = pos - begin();
        for (Size idx = 0; idx < count; idx++) {
            data()[diff + idx] = c;
        }
        m_length += count;
        null_terminate();
        return dst;
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        ptrdiff_t count = last - first;
        reserve(m_length + count);
        iterator dst = unconst_copy(pos);
        TraitsT::move(dst + count, pos, end() - pos);
        ptrdiff_t offset = pos - begin(); 
        for (Size idx = 0; idx < count; idx++) {
            data()[offset + idx] = *(first + idx);
        }
        m_length += count;
        null_terminate();
        return dst;
    }

    constexpr iterator insert(const_iterator pos, std::initializer_list<CharT> ilist) {
        ptrdiff_t count = ilist.size();
        reserve(m_length + count);
        iterator dst = unconst_copy(pos);
        TraitsT::move(dst + count, pos, end() - pos);
        ptrdiff_t offset = pos - begin(); 
        for (Size idx = 0; idx < count; idx++) {
            data()[offset + idx] = *(ilist.begin() + idx);
        }
        m_length += count;
        null_terminate();
        return dst;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &insert(Size index, const StringViewLikeT &t) {
        insert(index, t.data(), t.length());
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &insert(Size index, const StringViewLikeT &t, Size index_str, Size count = npos) {
        insert(index, t.data(), index_str, count);
        UU_STRING_ASSERT_NULL_TERMINATED;
        return *this;
    }

    // erasing ===================================================================================

    constexpr ProtoIStringForm &erase(Size index = 0, Size count = npos) {
        if (UNLIKELY(index > length())) {
            return_this_or_throw_out_of_range(index);
        }
        Size amt = std::min(count, length() - index);
        Size rem = length() - amt;
        TraitsT::move(data() + index, data() + index + amt, rem);
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
        TraitsT::move(begin() + diff, begin() + diff + 1, rem);
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
        TraitsT::move(begin() + first_idx, begin() + last_idx, rem);
        m_length -= amt;
        null_terminate();
        return unconst_copy(first);
    }

    // starts_with ================================================================================

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr bool starts_with(const StringViewLikeT &t) const {
        if (t.length() == 0) {
            return true;
        }
        if (t.length() > length()) {
            return false;
        }
        return TraitsT::compare(data(), t.data(), t.length()) == 0;
    }

    constexpr bool starts_with(CharT c) const noexcept {
        return length() > 0 && TraitsT::eq(data()[0], c);
    }

    constexpr bool starts_with(const CharT *s) const {
        Size len = TraitsT::length(s);
        return length() >= len && TraitsT::compare(data(), s, len) == 0;
    }

    // ends_with ==================================================================================

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr bool ends_with(const StringViewLikeT &t) const {
        if (t.length() == 0) {
            return true;
        }
        if (t.length() > length()) {
            return false;
        }
        Size pos = length() - t.length();
        return TraitsT::compare(data() + pos, t.data(), t.length()) == 0;
    }

    constexpr bool ends_with(CharT c) const noexcept {
        return length() > 0 && TraitsT::eq(data()[length() - 1], c);
    }

    constexpr bool ends_with(const CharT *s) const {
        Size len = TraitsT::length(s);
        Size pos = length() - len;
        return length() >= len && TraitsT::compare(data() + pos, s, len) == 0;
    }

    // contains ===================================================================================

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
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

    constexpr Size find(const ProtoIStringForm &str, Size pos = 0) const noexcept {
        return find(BasicStringView(str), pos);
    }

    constexpr Size find(const CharT *s, Size pos, Size count) const {
        return find(BasicStringView(s, count), pos);
    }

    constexpr Size find(const CharT *s, Size pos = 0) const {
        return find(BasicStringView(s, TraitsT::length(s)), pos);
    }

    constexpr Size find(CharT c, Size pos = 0) const noexcept {
        for (Size idx = pos; idx < length(); idx++) {
            if (TraitsT::eq(data()[idx], c)) {
                return idx;
            }
        }
        return npos;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
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
                if (TraitsT::eq(data()[idx], a) && TraitsT::eq(data()[idx + 1], b)) {
                    return idx;
                }
            }
            return npos;
        }
        else {
            const CharT a = t[0];
            for (Size idx = pos; idx <= length() - t.length(); idx++) {
                if (TraitsT::eq(data()[idx], a) && TraitsT::compare(data() + idx, t.data(), t.length()) == 0) {
                    return idx;
                }
            }
            return npos;
        }
    }

    constexpr Size find_boyer_moore(const ProtoIStringForm &str, Size pos = 0) const {
        return find_boyer_moore(BasicStringView(str), pos);
    }

    constexpr Size find_boyer_moore(const CharT *s, Size pos, Size count) const {
        return find_boyer_moore(BasicStringView(s, count), pos);
    }

    constexpr Size find_boyer_moore(const CharT *s, Size pos = 0) const {
        return find_boyer_moore(BasicStringView(s, TraitsT::length(s)), pos);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
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
                if (TraitsT::eq(data()[idx], a) && TraitsT::eq(data()[idx + 1], b)) {
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

    constexpr Size rfind( const ProtoIStringForm &str, Size pos = npos) const noexcept {
        return str.length() == 0 ? std::min(pos, length()) : rfind(BasicStringView(str), pos);
    }

    constexpr Size rfind(const CharT *s, Size pos, Size count) const {
        return count == 0 ? std::min(pos, length()) : rfind(BasicStringView(s, count), pos);
    }

    constexpr Size rfind(const CharT *s, Size pos = npos) const {
        Size len = TraitsT::length(s);
        return len == 0 ? std::min(pos, length()) : rfind(BasicStringView(s, len), pos);
    }

    constexpr Size rfind(CharT c, Size pos = npos) const noexcept {
        if (length() == 0) {
            return npos;
        }
        Size idx = std::min(pos, length() - 1);
        for (;;) {
            if (TraitsT::eq(data()[idx], c)) {
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
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
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
                if (TraitsT::eq(data()[idx], a) && TraitsT::eq(data()[idx + 1], b)) {
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
                if (TraitsT::eq(data()[idx], a) && TraitsT::compare(data() + idx, t.data(), t.length()) == 0) {
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

    constexpr Size find_first_of(const ProtoIStringForm &str, Size pos = 0) const noexcept {
        return find_first_of(BasicStringView(str), pos);
    }

    constexpr Size find_first_of(const CharT *s, Size pos, Size count) const {
        return find_first_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_first_of(const CharT *s, Size pos = 0) const {
        return find_first_of(BasicStringView(s, TraitsT::length(s)), pos);
    }

    constexpr Size find_first_of(CharT c, Size pos = 0) const noexcept {
        return find(c, pos);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr Size find_first_of(const StringViewLikeT &t, Size pos = 0) const noexcept {
        if (pos > length()) {
            return npos;
        }
        for (Size idx = pos; idx < length(); idx++) {
            for (Size cidx = 0; cidx < t.length(); cidx++) {
                if (TraitsT::eq(data()[idx], t[cidx])) {
                    return idx;
                }
            }
        }
        return npos;
    }

    // find_first_not_of ==========================================================================

    constexpr Size find_first_not_of(const ProtoIStringForm &str, Size pos = 0) const noexcept {
        return find_first_not_of(BasicStringView(str), pos);
    }

    constexpr Size find_first_not_of(const CharT *s, Size pos, Size count) const {
        return find_first_not_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_first_not_of(const CharT *s, Size pos = 0) const {
        return find_first_not_of(BasicStringView(s, TraitsT::length(s)), pos);
    }

    constexpr Size find_first_not_of(CharT c, Size pos = 0) const noexcept {
        if (pos > length()) {
            return npos;
        }
        for (Size idx = pos; idx < length(); idx++) {
            if (!TraitsT::eq(data()[idx], c)) {
                return idx;
            }
        }
        return npos;
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr Size find_first_not_of(const StringViewLikeT &t, Size pos = 0) const noexcept {
        if (pos > length()) {
            return npos;
        }
        for (Size idx = pos; idx < length(); idx++) {
            bool match = false;
            for (Size cidx = 0; cidx < t.length(); cidx++) {
                if (TraitsT::eq(data()[idx], t[cidx])) {
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

    constexpr Size find_last_of(const ProtoIStringForm &str, Size pos = npos) const noexcept {
        return find_last_of(BasicStringView(str), pos);
    }

    constexpr Size find_last_of(const CharT *s, Size pos, Size count) const {
        return find_last_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_last_of(const CharT *s, Size pos = npos) const {
        return find_last_of(BasicStringView(s, TraitsT::length(s)), pos);
    }

    constexpr Size find_last_of(CharT c, Size pos = npos) const noexcept {
        return rfind(c, pos);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
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
                if (TraitsT::eq(data()[idx], t[cidx])) {
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

    constexpr Size find_last_not_of(const ProtoIStringForm& str, Size pos = npos) const noexcept {
        return find_last_not_of(BasicStringView(str), pos);
    }

    constexpr Size find_last_not_of(const CharT *s, Size pos, Size count) const {
        return find_last_not_of(BasicStringView(s, count), pos);
    }

    constexpr Size find_last_not_of(const CharT *s, Size pos = npos) const {
        return find_last_not_of(BasicStringView(s, TraitsT::length(s)), pos);
    }

    constexpr Size find_last_not_of(CharT c, Size pos = npos) const noexcept {
        Size idx = std::min(pos, length() - 1);
        for (;;) {
            if (!TraitsT::eq(data()[idx], c)) {
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
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
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
                if (TraitsT::eq(data()[idx], t[cidx])) {
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

    constexpr ProtoIStringForm &replace(Size pos, Size count, const ProtoIStringForm &str) {
        return replace(pos, count, str, 0, str.length());
    }

    constexpr ProtoIStringForm &replace(Size pos, Size count, const ProtoIStringForm &str, Size pos2, Size count2 = npos) {
        return replace(pos, count, BasicStringView(str), pos2, count2);
    }

    constexpr ProtoIStringForm &replace(const_iterator first, const_iterator last, const ProtoIStringForm &str) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, str);
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr ProtoIStringForm &replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, BasicStringView(first2, last2), 0, last2 - first2);
    }

    constexpr ProtoIStringForm &replace(Size pos, Size count, const CharT *cstr, Size count2) {
        return replace(pos, count, BasicStringView(cstr, count2), 0, count2);
    }

    constexpr ProtoIStringForm &replace(const_iterator first, const_iterator last, const CharT *cstr, Size count2) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, BasicStringView(cstr, count2), 0, count2);
    }

    constexpr ProtoIStringForm &replace(Size pos, Size count, const CharT *cstr) {
        Size len = TraitsT::length(cstr);
        return replace(pos, count, BasicStringView(cstr, len), 0, len);
    }

    constexpr ProtoIStringForm &replace(const_iterator first, const_iterator last, const CharT *cstr) {
        Size pos = first - begin();
        Size count = last - first;
        Size len = TraitsT::length(cstr);
        return replace(pos, count, BasicStringView(cstr, len), 0, len);
    }

    constexpr ProtoIStringForm &replace(Size pos, Size count, Size count2, CharT c) {
        std::basic_string<CharT> str(count2, c);
        return replace(pos, count, str);
    }

    constexpr ProtoIStringForm &replace(const_iterator first, const_iterator last, Size count2, CharT c) {
        Size pos = first - begin();
        Size count = last - first;
        std::basic_string<CharT> str(count2, c);
        return replace(pos, count, str);
    }

    constexpr ProtoIStringForm &replace(const_iterator first, const_iterator last, std::initializer_list<CharT> ilist) {
        Size pos = first - begin();
        Size count = last - first;
        std::basic_string<CharT> str(ilist);
        return replace(pos, count, str);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &replace(Size pos, Size count, const StringViewLikeT &t) {
        return replace(pos, count, t, 0, t.length());
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &replace(const_iterator first, const_iterator last, const StringViewLikeT &t) {
        Size pos = first - begin();
        Size count = last - first;
        return replace(pos, count, t, 0, t.length());
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &replace(Size pos, Size count, 
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
        TraitsT::copy(begin() + pos, et.data(), et.length());
        null_terminate();
        return *this;
    }

    // copy =======================================================================================

    constexpr Size copy(CharT* dst, Size count, Size pos = 0) const {
        if (UNLIKELY(pos > length())) {
            return_zero_or_throw_out_of_range(pos);
        }
        Size ecount = std::min(count, length() - pos);
        TraitsT::copy(dst, begin() + pos, ecount);
        return ecount;
    }

    // operator[] =================================================================================

    constexpr CharT operator[](Size index) {
        return data()[index];
    }
    
    constexpr const CharT &operator[](Size index) const {
        return data()[index];
    }

    // operator+= =================================================================================

    constexpr ProtoIStringForm &operator+=(const ProtoIStringForm &other) {
        return append(other.data(), other.length());
    }

    constexpr ProtoIStringForm &operator+=(ProtoIStringForm &&other) noexcept {
        return append(other.data(), other.length());
    }

    constexpr ProtoIStringForm &operator+=(const CharT *s) {
        return append(s);
    }

    constexpr ProtoIStringForm &operator+=(CharT c) {
        return append(c);
    }

    constexpr ProtoIStringForm &operator+=(std::initializer_list<CharT> ilist) {
        return append(ilist);
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &operator+=(const StringViewLikeT &t) {
        return append(t.data(), t.length());
    }

    // comparison operators =======================================================================

    friend constexpr bool operator==(const ProtoIStringForm &a, const ProtoIStringForm &b) {
        if (a.length() != b.length()) {
            return false;
        }
        if (a.length() == 0) {
            return true;
        }
        return TraitsT::compare(a.data(), b.data(), a.length()) == 0;
    }

    friend constexpr int operator<=>(const ProtoIStringForm &a, const ProtoIStringForm &b) noexcept {
        return TraitsT::compare(a.data(), b.data(), std::min(a.length(), b.length()));
    }

    // conversion operators =======================================================================

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    operator std::string() const noexcept {
        return std::string(data(), length());
    }

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

    constexpr BasicStringView substrview(Size pos = 0, Size count = npos) const noexcept {
        if (UNLIKELY(pos > length())) {
            return_string_or_throw_out_of_range(pos);
        }
        return BasicStringView(data() + pos, std::min(count, length() - pos));
    }

    // iterators ==================================================================================

    constexpr iterator begin() {
        return iterator(data());
    }

    constexpr const_iterator begin() const {
        return iterator(data());
    }

    constexpr const_iterator cbegin() const {
        return iterator(data());
    }

    constexpr iterator end() {
        return iterator(data()) + m_length;
    }

    constexpr const_iterator end() const {
        return iterator(data()) + m_length;
    }

    constexpr const_iterator cend() const {
        return iterator(data()) + m_length;
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

    constexpr void swap(ProtoIStringForm &other) noexcept {
        CharT *char_addr = (CharT *)first_char_addr();        
        CharT *other_char_addr = (CharT *)other.first_char_addr();        

        // four cases
        if (is_using_inline_buffer() && other.is_using_inline_buffer()) {
            // first case has three sub-cases
            if (capacity() == other.capacity()) {
                CharT tmp_buf[capacity()];
                TraitsT::copy(tmp_buf, char_addr, capacity());
                TraitsT::copy(char_addr, other_char_addr, capacity());
                TraitsT::copy(other_char_addr, tmp_buf, capacity());
            }
            else if (capacity() > other.capacity()) {
                CharT tmp_buf[capacity()];
                TraitsT::copy(tmp_buf, char_addr, capacity());
                TraitsT::copy(char_addr, other_char_addr, other.capacity());

                Allocator &allocator = Context::get().allocator();
                Memory mem = allocator.alloc(capacity());
                other.set_base(static_cast<CharT *>(mem.ptr));
                other.set_capacity(mem.capacity);
                TraitsT::copy(other.data(), tmp_buf, length());
            }
            else {
                ASSERT(capacity() < other.capacity());
                CharT tmp_buf[other.capacity()];
                TraitsT::copy(tmp_buf, other_char_addr, other.capacity());
                TraitsT::copy(other_char_addr, char_addr, capacity());

                Allocator &allocator = Context::get().allocator();
                Memory mem = allocator.alloc(other.capacity());
                set_base(static_cast<CharT *>(mem.ptr));
                set_capacity(mem.capacity);
                TraitsT::copy(data(), tmp_buf, length());
            }
        }
        else if (is_using_inline_buffer() && other.is_using_allocated_buffer()) {
            void *tmp_ptr = other.base();
            TraitsT::copy(other_char_addr, char_addr, capacity());
            other.set_base(other.first_char_addr());
            set_base(tmp_ptr);
        }
        else if (is_using_allocated_buffer() && other.is_using_inline_buffer()) {
            void *tmp_ptr = base();
            TraitsT::copy(char_addr, other_char_addr, other.capacity());
            set_base(first_char_addr());
            other.set_base(tmp_ptr);
        }
        else {
            ASSERT(is_using_allocated_buffer());
            ASSERT(other.is_using_allocated_buffer());
            void *tmp_ptr = base();
            set_base(other.base());
            other.set_base(tmp_ptr);
        }

        // capacity
        Size tmp_capacity = capacity();
        set_capacity(other.capacity());
        other.set_capacity(tmp_capacity);

        // length
        Size tmp_length = length();
        set_length(other.length());
        other.set_length(tmp_length);

        null_terminate();
        other.null_terminate();

        UU_STRING_ASSERT_NULL_TERMINATED;
        UU_STRING_DATA_ASSERT_NULL_TERMINATED(other.data(), other.length());
    }

    // extensions =================================================================================

    constexpr ProtoIStringForm &replace_all(CharT a, CharT b) {
        Size pos = find(a);
        while (pos < length()) {
            data()[pos] = b;
            pos = find(a, pos + 1);
        }
        null_terminate();
        return *this;
    }

    constexpr ProtoIStringForm &replace_all(const CharT *a, const CharT *b) {
        BasicStringView av = BasicStringView(a, TraitsT::length(a));
        BasicStringView bv = BasicStringView(b, TraitsT::length(b));
        return replace_all(av, bv);
    }

    constexpr ProtoIStringForm &replace_all(ProtoIStringForm &a, ProtoIStringForm &b) {
        return replace_all(a, b);
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIStringForm &replace_all(StringViewLikeT &a, StringViewLikeT &b) {
        Size len = a.length();
        Size pos = find(a);
        while (pos < length()) {
            replace(pos, len, b);
            pos = find(a, pos + b.length());
        }
        null_terminate();
        return *this;
    }

    constexpr ProtoIStringForm &chop() {
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

    constexpr ProtoIStringForm &chomp() {
        if (length() > 0) {
            if (is_whitespace(back())) {
                chop();
                UU_STRING_ASSERT_NULL_TERMINATED;
            }
        }
        return *this;
    }

protected:
    constexpr explicit ProtoIStringForm(Size size) : ProtoIStringBase(first_char_addr(), size) {}

private:
    void grow(Size new_capacity) {
        Size old_capacity = m_capacity;
        while (m_capacity < new_capacity) {
            m_capacity *= 2;
        }
        Size amt = m_capacity * sizeof(CharT);
        if (is_using_allocated_buffer()) {
            Memory old_mem = { data(), old_capacity };
            Allocator &allocator = Context::get().allocator();
            Memory mem = allocator.alloc(amt);
            TraitsT::copy((CharT *)mem.ptr, data(), length());
            set_base(static_cast<CharT *>(mem.ptr));
            m_capacity = mem.capacity;
            allocator.dealloc(old_mem);
        }
        else {
            Allocator &allocator = Context::get().allocator();
            Memory mem = allocator.alloc(amt);
            set_base(static_cast<CharT *>(mem.ptr));
            m_capacity = mem.capacity;
            TraitsT::copy(data(), (CharT *)first_char_addr(), length());
        }
        null_terminate();
    }
};

// output ======================================================================================---

template <typename CharT, typename TraitsT>
std::basic_ostream<CharT> &operator<<(std::basic_ostream<CharT> &os, const ProtoIStringForm<CharT, TraitsT> &str)
{
    os.write(str.data(), str.length());
    return os;
}

// IStringStrorage ================================================================================

// Storage for the Array elements.  This is specialized for the N=0 case
// to avoid allocating unnecessary storage.
template <typename CharT, Size S>
struct IStringStrorage {
    alignas(CharT) char buf[S * sizeof(CharT)];
};

// Storage must be properly aligned even for small-size of 0 so that the
// pointer math in ProtoIStringForm::first_char_addr() is well-defined.
template <typename CharT> struct alignas(CharT) IStringStrorage<CharT, 0> {};

// ProtoIString =========================================================================================

static constexpr Size ProtoIStringDefaultInlineCapacity = 24;

template <typename CharT, Size S = ProtoIStringDefaultInlineCapacity, 
    typename TraitsT = std::char_traits<CharT>>
class ProtoIString : 
    public ProtoIStringForm<CharT, TraitsT>, IStringStrorage<CharT, S>
{
public:
    using Super = ProtoIStringForm<CharT, TraitsT>;

    // constants ==================================================================================

    static constexpr const Size InlineCapacity = S;
    static constexpr const Size npos = Super::npos;
    static constexpr CharT empty_value = Super::empty_value;

    // constructing ===============================================================================

    constexpr ProtoIString() noexcept : Super(InlineCapacity) {
        this->null_terminate(); 
    }
    
    constexpr explicit ProtoIString(Size capacity) : Super(InlineCapacity) {
        this->reserve(capacity);
        this->null_terminate();
    }
    
    constexpr ProtoIString(Size count, CharT c) : Super(InlineCapacity) {
        this->assign(count, c);
    }

    constexpr ProtoIString(const ProtoIString &other, Size pos, Size count = npos) : 
        Super(InlineCapacity) {
        this->assign(other, pos, std::min(count, other.length() - pos));
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIString(const char *ptr, Size length) : Super(InlineCapacity) {
        this->assign(ptr, length);
    }

    constexpr ProtoIString(const CharT *ptr, Size length) : Super(InlineCapacity) {
        this->assign(ptr, length);
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIString(const char *ptr) : Super(InlineCapacity) {
        this->assign(ptr, strlen(ptr));
    }

    constexpr ProtoIString(const CharT *ptr) : Super(InlineCapacity) {
        this->assign(ptr, TraitsT::length(ptr));
    }

    template <typename InputIt, typename MaybeT = InputIt, 
        std::enable_if_t<IsInputIteratorCategory<MaybeT>, int> = 0>
    constexpr ProtoIString(InputIt first, InputIt last) : Super(InlineCapacity) {
        this->assign(first, last);
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    constexpr ProtoIString(const std::string &str) : Super(InlineCapacity) {
        this->assign(str.data(), str.length());
    }

    constexpr ProtoIString(const std::basic_string<CharT> &str) : Super(InlineCapacity) {
        this->assign(str.data(), str.length());
    }

    constexpr ProtoIString(const ProtoIString &other) : Super(InlineCapacity) {
        this->assign(other.data(), other.length());
    }

    constexpr ProtoIString(const std::filesystem::path &path) : Super(InlineCapacity) {
        this->assign(path.string());
    }

    constexpr ProtoIString(ProtoIString &&other) : Super(InlineCapacity) {
        if (other.is_using_allocated_buffer()) {
            this->set_base(other.base());
            this->set_capacity(other.capacity());
            this->set_length(other.length());
            other.reset();
        }
        else {
            this->assign(other.data(), other.length());
        }
    }

    constexpr ProtoIString(std::initializer_list<CharT> ilist) : Super(InlineCapacity) {
        this->assign(ilist);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIString(const StringViewLikeT &str) : Super(InlineCapacity) {
        this->assign(str);
    }

    template <typename StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIString(const StringViewLikeT &str, Size pos, Size count = npos) : Super(InlineCapacity) {
        this->assign(str, pos, count);
    }

    // destructor =================================================================================

    constexpr ~ProtoIString() {
        if (this->is_using_allocated_buffer()) {
            Memory mem = { this->data(), this->capacity() };
            Allocator &allocator = Context::get().allocator();
            allocator.dealloc(mem);
        }
    }

    // operator= ==================================================================================

    constexpr ProtoIString &operator=(const ProtoIString &other) {
        if (this->is_same_string(other)) {
            return *this;
        }
        this->assign(other);
        return *this;
    }

    constexpr ProtoIString &operator=(ProtoIString &&other) noexcept {
        this->clear();
        if (other.is_using_allocated_buffer()) {
            if (this->is_using_allocated_buffer()) {
                Memory mem = { this->data(), this->capacity() };
                Allocator &allocator = Context::get().allocator();
                allocator.dealloc(mem);
            }
            this->set_base(other.base());
            this->set_capacity(other.capacity());
            this->set_length(other.length());
        }
        else {
            this->assign(other.data(), other.length());
        }
        other.reset();
        return *this;
    }

    constexpr ProtoIString &operator=(const CharT *s) {
        this->assign(s);
        return *this;
    }

    constexpr ProtoIString &operator=(CharT c) {
        this->assign(c);
        return *this;
    }

    constexpr ProtoIString &operator=(std::initializer_list<CharT> ilist) {
        this->assign(ilist);
        return *this;
    }

    template <class StringViewLikeT, typename MaybeT = StringViewLikeT,
        std::enable_if_t<IsStringViewLike<MaybeT, CharT, TraitsT>, int> = 0>
    constexpr ProtoIString &operator=(const StringViewLikeT &t) {
        this->assign(t);
        return *this;
    }


    constexpr ProtoIString substr(Size pos = 0, Size count = npos) const {
        if (UNLIKELY(pos > this->length())) {
            // FIXME
            // return_string_or_throw_out_of_range(pos);
        }
        return ProtoIString(this->data(), pos, std::min(count, this->length() - pos));
    }

    friend constexpr bool operator==(const ProtoIString &a, const ProtoIString &b) {
        if (a.length() != b.length()) {
            return false;
        }
        if (a.length() == 0) {
            return true;
        }
        return TraitsT::compare(a.data(), b.data(), a.length()) == 0;
    }

    friend constexpr int operator<=>(const ProtoIString &a, const ProtoIString &b) noexcept {
        return TraitsT::compare(a.data(), b.data(), std::min(a.length(), b.length()));
    }

    template <class CharX = CharT, std::enable_if_t<!IsByteSized<CharX>, int> = 0>
    operator std::string() const noexcept {
        return std::string(this->data(), this->length());
    }

    operator std::basic_string<CharT>() const noexcept {
        return std::basic_string<CharT>(this->data(), this->length());
    }

};

using ShortIString = ProtoIString<char>;
using MediumIString = ProtoIString<char, 64>;
using LongIString = ProtoIString<char, 96>;
using IString = ShortIString;

// operator+ ======================================================================================

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &lhs,
    const ProtoIStringForm<CharT, TraitsT> &rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &lhs, 
    const CharT* rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &lhs, CharT rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const CharT* lhs, 
    const ProtoIStringForm<CharT, TraitsT> &rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(CharT lhs, const ProtoIStringForm<CharT, TraitsT> &rhs) {
    IString str(1, lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &&lhs,
    const ProtoIStringForm<CharT, TraitsT> &&rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &&lhs,
    const ProtoIStringForm<CharT, TraitsT> &rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &&lhs,
    const CharT *rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &&lhs, CharT rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const ProtoIStringForm<CharT, TraitsT> &lhs,
    const ProtoIStringForm<CharT, TraitsT> &&rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(const CharT *lhs, 
    const ProtoIStringForm<CharT, TraitsT> &&rhs) {
    IString str(lhs);
    str += rhs;
    return str;
}

template <typename CharT, typename TraitsT>
ProtoIStringForm<CharT, TraitsT> operator+(CharT lhs,
    const ProtoIStringForm<CharT, TraitsT> &&rhs) {
    IString str(1, lhs);
    str += rhs;
    return str;
}

}  // namespace UU

namespace std
{
    // Implement std::swap in terms of ProtoIStringForm swap
    template <typename CharT>
    UU_ALWAYS_INLINE void swap(UU::ProtoIStringForm<CharT, std::char_traits<CharT>> &lhs, 
                               UU::ProtoIStringForm<CharT, std::char_traits<CharT>> &rhs) {
        lhs.swap(rhs);
    }
    
    template <typename CharT>
    struct less<UU::ProtoIStringForm<CharT, std::char_traits<CharT>>>
    {
        using StringT = UU::ProtoIStringForm<CharT, std::char_traits<CharT>>;
        bool operator()(const StringT &lhs, const StringT &rhs) const {
            return lhs < rhs;
        }
    };
}  // namespace std

#endif  // UU_ISTRING_H

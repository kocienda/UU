//
// Array.h
//
// Based (mightily) on SmallVector.h. Its license is as follows:
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// Changes made by me are:
//
// MIT License
// Copyright (c) 2022-2023 Ken Kocienda. All rights reserved.
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

#ifndef UU_ARRAY_H
#define UU_ARRAY_H

#include <type_traits>

#include <UU/Assertions.h>
#include <UU/Types.h>

namespace UU {

template <typename IteratorT> class iterator_range;

// Code common to all Arrays.
//
// The template parameter specifies the type which should be used to hold the
// size and capacity of the Array, so it can be adjusted.
// Using 32 bit size is desirable to shrink the size of the Array.
// Using 64 bit size is desirable for cases like Array<char>, where a
// 32 bit size would limit the array to ~4GB. Arrays are used for
// buffering bitcode output - which can exceed 4GB.
//
template <class SizeT> class ArrayBase {
public:
    UU_ALWAYS_INLINE SizeT size() const { return m_size; }
    UU_ALWAYS_INLINE SizeT capacity() const { return m_capacity; }

    UU_NO_DISCARD bool is_empty() const { return m_size == 0; }

protected:
    static constexpr size_t SizeTMax() {
        return std::numeric_limits<SizeT>::max();
    }

    ArrayBase() = delete;
    ArrayBase(void *base, SizeT capacity) : m_base(base), m_capacity(capacity) {}

    UU_ALWAYS_INLINE void *base() const { return m_base; }
    UU_ALWAYS_INLINE void set_base(void *base) { m_base = base; }

    // Set the array size, which must be less than or equal to the current capacity
    // Does not construct or destroy elements.
    void set_size(SizeT size) {
        ASSERT(size <= capacity());
        m_size = size;
    }

    void increment_size() { m_size++; }
    void increase_size(SizeT size) { m_size += size; }

    void decrement_size() { m_size--; }
    void decrease_size(SizeT size) { m_size -= size; }

    // Set the capacity data member, but does not handle actually changing the capacity.
    // Does not construct or destroy elements.
    void set_capacity(SizeT capacity) {
        m_capacity = capacity;
    }

    // A helper for grow(). Defined out of line to reduce code duplication.
    void *allocate_for_grow(size_t min_size, size_t t_size, size_t &new_capacity);

    // An implementation of grow() for POD types.
    // Defined out of line to reduce code duplication.
    void grow_pod(void *first_element, size_t min_size, size_t t_size);

    // data members
    void *m_base;
    SizeT m_size = 0;
    SizeT m_capacity = 0;
};

template <typename T>
using ArraySizeType = 
    typename std::conditional<sizeof(T) < 4 && sizeof(void *) >= 8, uint64_t, uint32_t>::type;

// Figure out the offset of the first element.
template <class T, typename = void> struct ArrayAlignmentAndSize {
    using SizeT = ArraySizeType<T>;
    alignas(ArrayBase<SizeT>) char m_align[sizeof(ArrayBase<SizeT>)];
    alignas(T) char m_first_element[sizeof(T)];
};

// This is the part of ArrayTypedBase which does not depend on whether
// the type T is a POD. The extra dummy template argument is used by ArrayRef
// to avoid unnecessarily requiring T to be complete.
template <typename T, typename = void>
class ArrayCommon : public ArrayBase<ArraySizeType<T>> {
    using ArrayBaseT = ArrayBase<ArraySizeType<T>>;

    // Find the address of the first element.  For this pointer math to be valid
    // with small-size of 0 for T with lots of alignment, it's important that
    // ArrayStorage is properly-aligned even for small-size of 0.
    void *first_element() const {
        auto offset = offsetof(ArrayAlignmentAndSize<T>, m_first_element);
        auto addr = reinterpret_cast<const char *>(this) + offset;
        return const_cast<void *>(reinterpret_cast<const void *>(addr));
    }
    // Space after 'm_first_element' is clobbered, do not add any instance vars after it.

protected:
    ArrayCommon(ArraySizeType<T> size) : ArrayBaseT(first_element(), size) {}

    void grow_pod(size_t MinSize, size_t TSize) {
        ArrayBaseT::grow_pod(first_element(), MinSize, TSize);
    }

    // Return true if the array is using its inline storage and false 
    // if dynamic memory has been allocated to hold more elements
    // than the inline storage can hold.
    bool is_using_inline_storage() const { return this->base() == first_element(); }

    // Reset to inline storage, assuming that any dynamically-allocated memory
    // is managed elsewhere.
    void reset_to_inline_storage() {
        this->set_base(first_element());
        this->set_size(0);
        this->set_capacity(0); // FIXME: Setting capacity to 0 is suspect.
    }

    // Return true if V is an internal reference to the given range.
    bool is_reference_to_range(const void *v, const void *first, const void *last) const {
        // Use std::less to avoid UB.
        std::less<> less;
        return !less(v, first) && less(v, last);
    }

    // Return true if V is an internal reference to this array.
    bool is_reference_to_storage(const void *v) const {
        return is_reference_to_range(v, this->begin(), this->end());
    }

    // Return true if First and Last form a valid (possibly is_empty) range in this
    // array's storage.
    bool is_range_in_storage(const void *first, const void *last) const {
        // Use std::less to avoid UB.
        std::less<> less;
        return !less(first, this->begin()) && !less(last, first) && !less(this->end(), last);
    }

    // Return false if the element would be invalidated by resizing the array to new_size,
    // true otherwise
    bool is_safe_to_reference_after_resize(const void *element, size_t new_size) {
        // Past the end.
        if (LIKELY(!is_reference_to_storage(element))) {
            return true;
        }

        // Return false if element would be destroyed by shrinking.
        if (new_size <= this->size()) {
            return element < this->begin() + new_size;
        }

        // Return false if necessary to grow.
        return new_size <= this->capacity();
    }

    // Check whether element would be invalidated by resizing the array to new_size.
    void assert_safe_to_reference_after_resize(const void *element, size_t new_size) {
        ASSERT_WITH_MESSAGE(is_safe_to_reference_after_resize(element, new_size),
            "Attempt to reference an element of the array in an operation that invalidates it.");
    }

    // Check whether element would be invalidated by increasing the size of the array by
    // the given amount.
    void assert_safe_to_add(const void *element, size_t amount = 1) {
        this->assert_safe_to_reference_after_resize(element, this->size() + amount);
    }

    // Check whether any part of the range would be invalidated by clearing.
    void assert_safe_to_reference_after_clear(const T *from, const T *to) {
        if (from == to) {
            return;
        }
        this->assert_safe_to_reference_after_resize(from, 0);
        this->assert_safe_to_reference_after_resize(to - 1, 0);
    }

    template <class I, std::enable_if_t<!std::is_same<std::remove_const_t<I>, T *>::value, bool> = false>
    void assert_safe_to_reference_after_clear(I, I) {}

    // Check whether any part of the range will be invalidated by growing.
    void assert_safe_to_add_range(const T *from, const T *to) {
        if (from == to) {
            return;
        }
        this->assert_safe_to_add(from, to - from);
        this->assert_safe_to_add(to - 1, to - from);
    }

    template <class I, std::enable_if_t<!std::is_same<std::remove_const_t<I>, T *>::value, bool> = false>
    void assert_safe_to_add_range(I, I) {}

    // Reserve enough space to add one element, and return the updated element
    // pointer in case it was a reference to the storage.
    template <class U>
    static const T *reserve_for_element_and_get_address_impl(U *u, const T &element, size_t size) {
        size_t new_size = u->size() + size;
        if (LIKELY(new_size <= u->capacity()))
            return &element;

        bool references_storage = false;
        int64_t index = -1;
        if (!U::TakesElementsByValue) {
            if (UNLIKELY(u->is_reference_to_storage(&element))) {
                references_storage = true;
                index = &element - u->begin();
            }
        }
        u->grow(new_size);
        return references_storage ? u->begin() + index : &element;
    }

public:
    using size_type = ArraySizeType<T>;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using iterator = T *;
    using const_iterator = const T *;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;

    using ArrayBaseT::capacity;
    using ArrayBaseT::is_empty;
    using ArrayBaseT::size;

    iterator begin() { return (iterator)this->base(); }
    const_iterator begin() const { return (const_iterator)this->base(); }
    iterator end() { return begin() + size(); }
    const_iterator end() const { return begin() + size(); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const{ return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin());}

    size_type size_in_bytes() const { return size() * sizeof(T); }
    size_type max_size() const { return std::min(this->SizeTMax(), size_type(-1) / sizeof(T)); }
    size_t capacity_in_bytes() const { return capacity() * sizeof(T); }

    pointer data() { return pointer(begin()); }
    const_pointer data() const { return const_pointer(begin()); }

    reference operator[](size_type idx) {
        ASSERT(idx < size());
        return begin()[idx];
    }

    const_reference operator[](size_type idx) const {
        ASSERT(idx < size());
        return begin()[idx];
    }

    reference front() {
        ASSERT(!is_empty());
        return begin()[0];
    }

    const_reference front() const {
        ASSERT(!is_empty());
        return begin()[0];
    }

    reference back() {
        ASSERT(!is_empty());
        return end()[-1];
    }

    const_reference back() const {
        ASSERT(!is_empty());
        return end()[-1];
    }
};

// ArrayTypedBase<TriviallyCopyable = false> - This is where we put
// method implementations that are designed to work with non-trivial T's.
//
// Approximate is_trivially_copyable with trivial move/copy construction and
// trivial destruction. While the standard doesn't specify that it's allowed
// copy these types with memcpy, there is no way for the type to observe this.
// This catches the important case of std::pair<POD, POD>, which is not
// trivially assignable.
template <typename T, bool = (std::is_trivially_copy_constructible<T>::value) &&
                             (std::is_trivially_move_constructible<T>::value) &&
                             std::is_trivially_destructible<T>::value>
class ArrayTypedBase : public ArrayCommon<T> 
{
    friend class ArrayCommon<T>;

protected:
    static constexpr bool TakesElementsByValue = false;
    using ElementT = const T &;

    ArrayTypedBase(size_t size) : ArrayCommon<T>(size) {}

    static void call_destructors_on_range(T *start, T *end) {
        while (start != end) {
            --end;
            end->~T();
        }
    }

    // Move the range [it, end) into the uninitialized memory starting with dst,
    // constructing elements as needed.
    template<typename It1, typename It2>
    static void uninitialized_move(It1 it, It1 end, It2 dst) {
        std::uninitialized_copy(std::make_move_iterator(it), std::make_move_iterator(end), dst);
    }

    // Copy the range [it, end) into the uninitialized memory starting with dst,
    // constructing elements as needed.
    template<typename It1, typename It2>
    static void uninitialized_copy(It1 it, It1 end, It2 dst) {
        std::uninitialized_copy(it, end, dst);
    }

    // Grow the allocated memory (without initializing new elements), doubling
    // the size of the allocated memory. Guarantees space for at least one more
    // element, or min_size more elements if specified.
    void grow(size_t min_size = 0);

    // Allocate enough for min_size and pass back its size in new_capacity. 
    // This is the first step of grow().
    T *allocate_for_grow(size_t min_size, size_t &new_capacity) {
        using A = ArrayBase<ArraySizeType<T>>;
        return static_cast<T *>(A::allocate_for_grow(min_size, sizeof(T), new_capacity));
    }

    // Move existing elements over to the newly-allocated elements, the middle step of grow().
    void move_elements_for_grow(T *new_elements);

    // Transfer ownership of the allocation, the last step up for grow().
    void transfer_allocation_for_grow(T *new_elements, size_t new_capacity);

    // Reserve enough space to add one element, and return the updated element
    // pointer in case it was a reference to the storage.
    const T *reserve_for_element_and_get_address(const T &element, size_t size = 1) {
        return this->reserve_for_element_and_get_address_impl(this, element, size);
    }
    T *reserve_for_element_and_get_address(T &element, size_t size = 1) {
        const T *addr = this->reserve_for_element_and_get_address_impl(this, element, size);
        return const_cast<T *>(addr);
    }

    static T &&forward_element(T &&element) { return std::move(element); }
    static const T &forward_element(const T &element) { return element; }

    void grow_and_assign(size_t num_elements, const T &element) {
        // Grow manually in case element is an internal reference.
        size_t new_capacity;
        T *new_elements = allocate_for_grow(num_elements, new_capacity);
        std::uninitialized_fill_n(new_elements, num_elements, element);
        this->call_destructors_on_range(this->begin(), this->end());
        transfer_allocation_for_grow(new_elements, new_capacity);
        this->set_size(num_elements);
    }

    template <typename... ArgTypes> T &grow_and_emplace_back(ArgTypes &&... args) {
        // Grow manually in case one of args is an internal reference.
        size_t new_capacity;
        T *new_elements = allocate_for_grow(0, new_capacity);
        ::new ((void *)(new_elements + this->size())) T(std::forward<ArgTypes>(args)...);
        move_elements_for_grow(new_elements);
        transfer_allocation_for_grow(new_elements, new_capacity);
        this->increment_size();
        return this->back();
    }

public:
    void push_back(const T &element) {
        const T *element_ptr = reserve_for_element_and_get_address(element);
        ::new ((void *)this->end()) T(*element_ptr);
        this->increment_size();
    }

    void push_back(T &&element) {
        T *element_ptr = reserve_for_element_and_get_address(element);
        ::new ((void *)this->end()) T(::std::move(*element_ptr));
        this->increment_size();
    }

    void pop_back() {
        this->decrement_size();
        this->end()->~T();
    }

};

// Define out-of-line to dissuade the C++ compiler from inlining it.
template <typename T, bool TriviallyCopyable>
void ArrayTypedBase<T, TriviallyCopyable>::grow(size_t min_size) {
    size_t new_capacity;
    T *new_elements = allocate_for_grow(min_size, new_capacity);
    move_elements_for_grow(new_elements);
    transfer_allocation_for_grow(new_elements, new_capacity);
}

// Define out-of-line to dissuade the C++ compiler from inlining it.
template <typename T, bool TriviallyCopyable>
void ArrayTypedBase<T, TriviallyCopyable>::move_elements_for_grow(T *new_elements) {
    this->uninitialized_move(this->begin(), this->end(), new_elements);
    call_destructors_on_range(this->begin(), this->end());
}

// Define this out-of-line to dissuade the C++ compiler from inlining it.
template <typename T, bool TriviallyCopyable>
void ArrayTypedBase<T, TriviallyCopyable>::transfer_allocation_for_grow(T *new_elements, size_t new_capacity) {
    // If this wasn't grown from the inline copy, deallocate the old space.
    if (!this->is_using_inline_storage()) {
        free(this->begin());
    }
    this->set_base(new_elements);
    this->set_capacity(new_capacity);
}

// ArrayTypedBase<TriviallyCopyable = true> - These are the method implementations 
// designed to work with trivially copyable T's, allowing use of memcpy instead of 
// calling copy/move constructors and non-trivial destructors.
template <typename T>
class ArrayTypedBase<T, true> : public ArrayCommon<T> {
    friend class ArrayCommon<T>;

protected:
    // True if it's cheap enough to take parameters by value. Doing so avoids
    // overhead related to mitigations for reference invalidation.
    static constexpr bool TakesElementsByValue = sizeof(T) <= 2 * sizeof(void *);

    // Either const T& or T, depending on whether it's cheap enough to take
    // parameters by value.
    using ElementT = typename std::conditional<TakesElementsByValue, T, const T &>::type;

    ArrayTypedBase(ArraySizeType<T> size) : ArrayCommon<T>(size) {}

    // No need to do a destroy loop for trivial elements.
    static void call_destructors_on_range(T *, T *) {}

    // Move the range [it, end) into the uninitialized memory starting with dst,
    // constructing elements as needed.
    template<typename It1, typename It2>
    static void uninitialized_move(It1 it, It1 end, It2 dst) {
        // simple copy.
        uninitialized_copy(it, end, dst);
    }

    // Copy the range [it, end) into the uninitialized memory starting with dst,
    // constructing elements as needed.
    template<typename It1, typename It2>
    static void uninitialized_copy(It1 it, It1 end, It2 dst) {
        std::uninitialized_copy(it, end, dst);
    }

    // Copy the range [it, end) into the uninitialized memory starting with dst,
    // constructing elements as needed.
    template <typename T1, typename T2>
    static void uninitialized_copy(T1 *it, T1 *end, T2 *dst,
    std::enable_if_t<std::is_same<typename std::remove_const<T1>::type,T2>::value> * = nullptr) {
        // Use memcpy for PODs iterated by pointers (which includes Array
        // iterators): std::uninitialized_copy optimizes to memmove, but we can
        // use memcpy here. Note that I and E are iterators and thus might be
        // invalid for memcpy if they are equal.
        if (it != end) {
            memcpy(reinterpret_cast<void *>(dst), it, (end - it) * sizeof(T));
        }
    }

    // Double the size of the allocated memory, guaranteeing space for at
    // least one more element or MinSize if specified.
    void grow(size_t MinSize = 0) { this->grow_pod(MinSize, sizeof(T)); }

    // Reserve enough space to add one element, and return the updated element
    // pointer in case it was a reference to the storage.
    const T *reserve_for_element_and_get_address(const T &element, size_t size = 1) {
        return this->reserve_for_element_and_get_address_impl(this, element, size);
    }
    T *reserve_for_element_and_get_address(T &element, size_t size = 1) {
        const T *addr = this->reserve_for_element_and_get_address_impl(this, element, size);
        return const_cast<T *>(addr);
    }

    // Copy element or return a reference, depending on ElementT.
    static ElementT forward_element(ElementT element) { return element; }

    void grow_and_assign(size_t size, T element) {
        // Elt has been copied if it's an internal reference, side-stepping
        // reference invalidation problems without losing the realloc optimization.
        this->set_size(0);
        this->grow(size);
        std::uninitialized_fill_n(this->begin(), size, element);
        this->set_size(size);
    }

    template <typename... ArgTypes> T &grow_and_emplace_back(ArgTypes &&... args) {
        // Use push_back with a copy in case Args has an internal reference,
        // side-stepping reference invalidation problems without losing the realloc
        // optimization.
        push_back(T(std::forward<ArgTypes>(args)...));
        return this->back();
    }

public:
    void push_back(ElementT Elt) {
        const T *EltPtr = reserve_for_element_and_get_address(Elt);
        memcpy(reinterpret_cast<void *>(this->end()), EltPtr, sizeof(T));
        this->set_size(this->size() + 1);
    }

    void pop_back() { this->decrement_size(); }
};

// Common code factored out of the Array class to reduce code duplication 
// based on the Array 'N' template parameter.
template <typename T>
class ArrayForm : public ArrayTypedBase<T> 
{
    using SuperClass = ArrayTypedBase<T>;

public:
    using iterator = typename SuperClass::iterator;
    using const_iterator = typename SuperClass::const_iterator;
    using reference = typename SuperClass::reference;
    using size_type = typename SuperClass::size_type;

protected:
    using ArrayTypedBase<T>::TakesElementsByValue;
    using ElementT = typename SuperClass::ElementT;

    // Default ctor - Initialize to is_empty.
    explicit ArrayForm(unsigned N) : ArrayTypedBase<T>(N) {}

    void assign_remote(ArrayForm &&a) {
        this->call_destructors_on_range(this->begin(), this->end());
        if (!this->is_using_inline_storage()) {
            free(this->begin());
        }
        this->set_base(a.base());
        this->set_size(a.size);
        this->set_capacity(a.capacity());
        a.reset_to_inline_storage();
    }

public:
    ArrayForm(const ArrayForm &) = delete;

    ~ArrayForm() {
        // Subclass has already destructed this array's elements.
        // If this wasn't grown from the inline copy, deallocate the old space.
        if (!this->is_using_inline_storage()) {
            free(this->begin());
        }
    }

    void clear() {
        this->call_destructors_on_range(this->begin(), this->end());
        this->set_size(0);
    }

private:
    // Make set_size() private to avoid misuse in subclasses.
    using SuperClass::set_size;

    template <bool ForOverwrite> void resize_impl(size_type N) {
        if (N == this->size()) {
            return;
        }

        if (N < this->size()) {
            this->truncate(N);
            return;
        }

        this->reserve(N);
        for (auto I = this->end(), E = this->begin() + N; I != E; ++I) {
            if (ForOverwrite) {
                new (&*I) T;
            }
            else {
                new (&*I) T();
            }
        }
        this->set_size(N);
    }

public:
    void resize(size_type N) { resize_impl<false>(N); }

    // Like resize, but T is POD, the new values won't be initialized.
    void resize_for_overwrite(size_type N) { resize_impl<true>(N); }

    // Like resize, but requires that N is less than size().
    void truncate(size_type size) {
        ASSERT_WITH_MESSAGE(this->size() >= size, "Can't increase size with truncate");
        this->call_destructors_on_range(this->begin() + size, this->end());
        this->set_size(size);
    }

    void resize(size_type size, ElementT new_element) {
        // same size
        if (size == this->size()) {
            return;
        }

        // truncate
        if (size < this->size()) {
            this->truncate(size);
            return;
        }

        // grow: defer to append
        this->append(size - this->size(), new_element);
    }

    void reserve(size_type size) {
        if (this->capacity() < size) {
            this->grow(size);
        }
    }

    void pop_back_n(size_type size) {
        ASSERT(this->size() >= size);
        truncate(this->size() - size);
    }

    UU_NO_DISCARD T pop_back_val() {
        T result = ::std::move(this->back());
        this->pop_back();
        return result;
    }

    void swap(ArrayForm &);

    // Add the specified range to the end of the Array.
    template <typename I, typename = std::enable_if_t<IsInputIteratorCategory<I>>>
    void append(I begin_it, I end_it) {
        using SizeT = ArraySizeType<T>;
        this->assert_safe_to_add_range(begin_it, end_it);
        size_type size = SizeT(std::distance(begin_it, end_it));
        this->reserve(this->size() + size);
        this->uninitialized_copy(begin_it, end_it, this->end());
        this->increase_size(size);
    }

    // Append copies of element to the end.
    void append(size_type size, ElementT element) {
        const T *element_ptr = this->reserve_for_element_and_get_address(element, size);
        std::uninitialized_fill_n(this->end(), size, *element_ptr);
        this->increase_size(size);
    }

    void append(std::initializer_list<T> il) {
        append(il.begin(), il.end());
    }

    void append(const ArrayForm &a) { append(a.begin(), a.end()); }

    void assign(size_type size, ElementT element) {
        // Note that element could be an internal reference.
        if (size > this->capacity()) {
            this->grow_and_assign(size, element);
            return;
        }

        // Assign over existing elements.
        std::fill_n(this->begin(), std::min(size, this->size()), element);
        if (size > this->size()) {
            std::uninitialized_fill_n(this->end(), size - this->size(), element);
        }
        else if (size < this->size()) {
            this->call_destructors_on_range(this->begin() + size, this->end());
        }
        this->set_size(size);
    }

    // FIXME: Consider assigning over existing elements, rather than clearing &
    // re-initializing them - for all assign(...) variants.

    template <typename I, typename = std::enable_if_t<IsInputIteratorCategory<I>>>
    void assign(I begin_it, I end_it) {
        this->assert_safe_to_reference_after_clear(begin_it, end_it);
        clear();
        append(begin_it, end_it);
    }

    void assign(std::initializer_list<T> il) {
        clear();
        append(il);
    }

    void assign(const ArrayForm &a) { assign(a.begin(), a.end()); }

    iterator erase(const_iterator cit) {
        ASSERT_WITH_MESSAGE(this->is_reference_to_storage(cit), 
            "Iterator to erase out of bounds.");

        // Cast away constness because this is a non-const member function.
        iterator it = const_cast<iterator>(cit);

        // Shift all elements down one.
        // Drop the last element.
        // Return an iterator to the position passed in.
        iterator result_it = it;
        std::move(it + 1, this->end(), it);
        this->pop_back();
        return result_it;
    }

    iterator erase(const_iterator cbegin_it, const_iterator cend_it) {
        ASSERT_WITH_MESSAGE(this->is_range_in_storage(cbegin_it, cend_it), 
            "Range to erase out of bounds.");

        // Cast away constness because this is a non-const member function.
        iterator begin_it = const_cast<iterator>(cbegin_it);
        iterator end_it = const_cast<iterator>(cend_it);

        // Shift all elements down one.
        // Drop the elements in the range.
        // Return an iterator to the position passed in.
        iterator result_it = begin_it;
        iterator it = std::move(end_it, this->end(), begin_it);
        this->call_destructors_on_range(it, this->end());
        this->set_size(it - this->begin());
        return result_it;
    }

private:
    template <class ElementArgT> iterator insert_one_impl(iterator it, ElementArgT &&element) {
        // Callers ensure that ElementArgT is derived from T.
        static_assert(std::is_same<std::remove_const_t<std::remove_reference_t<ElementArgT>>, T>::value,
            "Element argument must be derived from T.");

        if (it == this->end()) {  // Important special case for is_empty array.
            this->push_back(::std::forward<ElementArgT>(element));
            return this->end() - 1;
        }

        ASSERT_WITH_MESSAGE(this->is_reference_to_storage(it), "Iterator to erase out of bounds.");

        // Grow if necessary.
        size_t index = it - this->begin();
        std::remove_reference_t<ElementArgT> *element_ptr = 
            this->reserve_for_element_and_get_address(element);
        it = this->begin() + index;

        ::new ((void*) this->end()) T(::std::move(this->back()));
        // Push everything else over.
        std::move_backward(it, this->end()-1, this->end());
        this->increment_size();

        // If we just moved the element we're inserting, 
        // update the reference (never happens if TakesElementsByValue).
        static_assert(!TakesElementsByValue || std::is_same<ElementArgT, T>::value,
            "ElementArgT must be 'T' when taking by value.");
        if (!TakesElementsByValue && this->is_reference_to_range(element_ptr, it, this->end())) {
            ++element_ptr;
        }
        *it = ::std::forward<ElementArgT>(*element_ptr);
        return it;
    }

public:
    iterator insert(iterator it, T &&element) {
        return insert_one_impl(it, this->forward_element(std::move(element)));
    }

    iterator insert(iterator it, const T &element) {
        return insert_one_impl(it, this->forward_element(element));
    }

    iterator insert(iterator it, size_type insert_size, ElementT element) {
        // Convert iterator to an index to avoid invalidating iterator after 
        // calling reserve_for_element_and_get_address()
        size_t insert_index = it - this->begin();

        if (it == this->end()) {  // Important special case for is_empty array.
            append(insert_size, element);
            return this->begin() + insert_index;
        }

        ASSERT_WITH_MESSAGE(this->is_reference_to_storage(it), "Iterator to erase out of bounds.");

        // Ensure there is enough space, and get the (maybe updated) address of element.
        const T *element_ptr = this->reserve_for_element_and_get_address(element, insert_size);

        // Uninvalidate the iterator.
        it = this->begin() + insert_index;

        // If there are more elements between the insertion point and the end of the
        // range than there are being inserted, use a simple approach to insertion.
        // Since space is already reserved, this won't reallocate the array.
        if (size_t(this->end()-it) >= insert_size) {
            T *old_end = this->end();
            append(std::move_iterator<iterator>(this->end() - insert_size),
                std::move_iterator<iterator>(this->end()));

            // Copy the existing elements that get replaced.
            std::move_backward(it, old_end - insert_size, old_end);

            // If the element we're inserting moved, update the reference 
            // (never happens if TakesElementsByValue).
            if (!TakesElementsByValue && it <= element_ptr && element_ptr < this->end()) {
                element_ptr += insert_size;
            }

            std::fill_n(it, insert_size, *element_ptr);
            return it;
        }

        // Otherwise, more elements are being inserted than exist already 
        // and the insertion point isn't at the end.

        // Move over the elements about to be overwritten.
        T *old_end = this->end();
        this->set_size(this->size() + insert_size);
        size_t overwritten_size = old_end - it;
        this->uninitialized_move(it, old_end, this->end() - overwritten_size);

        // If we just moved the element we're inserting, be sure to update
        // the reference (never happens if TakesElementsByValue).
        if (!TakesElementsByValue && it <= element_ptr && element_ptr < this->end()) {
            element_ptr += insert_size;
        }

        // Replace the overwritten part.
        std::fill_n(it, overwritten_size, *element_ptr);

        // Insert the non-overwritten middle part.
        std::uninitialized_fill_n(old_end, insert_size - overwritten_size, *element_ptr);
        return it;
    }

    template <typename I, typename = std::enable_if_t<IsInputIteratorCategory<I>>>
    iterator insert(iterator it, I from, I to) {
        using SizeT = ArraySizeType<T>;

        // Convert iterator to an index to avoid invalidating iterator after calling reserve()
        SizeT insert_index = SizeT(it - this->begin());

        if (it == this->end()) {  // Important special case for is_empty array.
            append(from, to);
            return this->begin() + insert_index;
        }

        ASSERT_WITH_MESSAGE(this->is_reference_to_storage(it), "Iterator to erase out of bounds.");

        // Check that the reserve that follows doesn't invalidate the iterators.
        this->assert_safe_to_add_range(from, to);

        SizeT insert_size = SizeT(std::distance(from, to));

        // Ensure there is enough space.
        reserve(this->size() + insert_size);

        // Uninvalidate the iterator.
        it = this->begin() + insert_index;

        // If there are more elements between the insertion point and the end of the
        // range than there are being inserted, use a simple approach to insertion.
        // Since space is already reserved, this won't reallocate the array.
        if (SizeT(this->end() - it) >= insert_size) {
            T *old_end = this->end();
            append(std::move_iterator<iterator>(this->end() - insert_size),
                std::move_iterator<iterator>(this->end()));

            // Copy the existing elements that get replaced.
            std::move_backward(it, old_end - insert_size, old_end);

            std::copy(from, to, it);
            return it;
        }

        // Otherwise, more elements are being inserted than exist already 
        // and the insertion point isn't at the end.

        // Move over the elements about to be overwritten.
        T *old_end = this->end();
        this->increase_size(insert_size);
        SizeT overwritten_size = SizeT(old_end - it);
        this->uninitialized_move(it, old_end, this->end() - overwritten_size);

        // Replace the overwritten part.
        for (T *replace_it = it; overwritten_size > 0; --overwritten_size) {
            *replace_it = *from;
            ++replace_it;
            ++from;
        }

        // Insert the non-overwritten middle part.
        this->uninitialized_copy(from, to, old_end);
        return it;
    }

    void insert(iterator it, std::initializer_list<T> il) {
        insert(it, il.begin(), il.end());
    }

    template <typename... ArgTypes> reference emplace_back(ArgTypes &&... args) {
        if (UNLIKELY(this->size() >= this->capacity()))
            return this->grow_and_emplace_back(std::forward<ArgTypes>(args)...);

        ::new ((void *)this->end()) T(std::forward<ArgTypes>(args)...);
        this->increment_size();
        return this->back();
    }

    ArrayForm &operator=(const ArrayForm &);
    ArrayForm &operator=(ArrayForm &&);

    bool operator==(const ArrayForm &a) const {
        if (this->size() != a.size()) {
            return false;
        }
        return std::equal(this->begin(), this->end(), a.begin());
    }

    bool operator!=(const ArrayForm &a) const {
        return !(*this == a);
    }

    bool operator<(const ArrayForm &a) const {
        return std::lexicographical_compare(this->begin(), this->end(), a.begin(), a.end());
    }

    bool operator>(const ArrayForm &a) const { return a < *this; }
    bool operator<=(const ArrayForm &a) const { return !(*this > a); }
    bool operator>=(const ArrayForm &a) const { return !(*this < a); }
};

template <typename T>
void ArrayForm<T>::swap(ArrayForm<T> &a) 
{
    using SizeT = ArraySizeType<T>;

    if (this == &a) {
        return;
    }

    // Simple case: both arrays are using inline storage
    if (!this->is_using_inline_storage() && !a.is_using_inline_storage()) {
        void *base_tmp = this->base();
        this->set_base(a.base());
        a.set_base(base_tmp);
        
        SizeT size_tmp = this->size();
        this->set_size(a.size());
        a.set_size(size_tmp);

        SizeT capacity_tmp = this->capacity();
        this->set_capacity(a.capacity());
        a.set_capacity(capacity_tmp);
        return;
    }

    this->reserve(a.size());
    a.reserve(this->size());

    // Swap the shared elements.
    SizeT shared_size = this->size();
    if (shared_size > a.size()) {
        shared_size = a.size();
    }
    for (SizeT i = 0; i != shared_size; ++i) {
        std::swap((*this)[i], a[i]);
    }

    // Copy over the extra elements.
    if (this->size() > a.size()) {
        SizeT diff_size = this->size() - a.size();
        this->uninitialized_copy(this->begin() + shared_size, this->end(), a.end());
        a.set_size(a.size() + diff_size);
        this->call_destructors_on_range(this->begin() + shared_size, this->end());
        this->set_size(shared_size);
    } else if (a.size() > this->size()) {
        SizeT diff_size = a.size() - this->size();
        this->uninitialized_copy(a.begin() + shared_size, a.end(), this->end());
        this->set_size(this->size() + diff_size);
        this->call_destructors_on_range(a.begin() + shared_size, a.end());
        a.set_size(shared_size);
    }
}

template <typename T>
ArrayForm<T> &ArrayForm<T>::operator=(const ArrayForm<T> &a) 
{
    using SizeT = ArraySizeType<T>;

    if (this == &a) {
        return *this;
    }

    // If there's sufficient space, assign the common elements, then destroy any excess.
    SizeT asize = a.size();
    SizeT current_size = this->size();
    if (current_size >= asize) {
        // Assign common elements.
        iterator end_it;
        if (asize) {
            end_it = std::copy(a.begin(), a.begin()+asize, this->begin());
        }
        else {
            end_it = this->begin();
        }

        // Destroy excess elements and trim.
        this->call_destructors_on_range(end_it, this->end());
        this->set_size(asize);

        return *this;
    }

    // If it's necessary to grow to have enough elements, destroy the current elements.
    // This avoids copying them during the grow.
    // FIXME: don't do this if they're efficiently moveable.
    if (this->capacity() < asize) {
        // Destroy current elements.
        this->clear();
        current_size = 0;
        this->grow(asize);
    } 
    else if (current_size) {
        // Otherwise, use assignment for the already-constructed elements.
        std::copy(a.begin(), a.begin() + current_size, this->begin());
    }

    // Copy construct the new elements in place.
    this->uninitialized_copy(a.begin() + current_size, a.end(), this->begin() + current_size);

    // Set end.
    this->set_size(asize);
    return *this;
}

template <typename T>
ArrayForm<T> &ArrayForm<T>::operator=(ArrayForm<T> &&a) 
{
    using SizeT = ArraySizeType<T>;

    if (this == &a) {
        return *this;
    }

    // If passed-in array isn't using inline storage, clear this array
    // and steal the passed-in array's buffer.
    if (!a.is_using_inline_storage()) {
        this->assign_remote(std::move(a));
        return *this;
    }

    // If there's sufficient space, assign the common elements, then destroy any excess.
    SizeT asize = a.size();
    SizeT current_size = this->size();
    if (current_size >= asize) {
        // Assign common elements.
        iterator end_it = this->begin();
        if (asize) {
            end_it = std::move(a.begin(), a.end(), end_it);
        }

        // Destroy excess elements and trim the bounds and clear the passed-in array.
        this->call_destructors_on_range(end_it, this->end());
        this->set_size(asize);
        a.clear();

        return *this;
    }

    // If it's necessary to grow to have enough elements, destroy the current elements.
    // This avoids copying them during the grow.
    // FIXME: don't do this if they're efficiently moveable.
    if (this->capacity() < asize) {
        // Destroy current elements.
        this->clear();
        current_size = 0;
        this->grow(asize);
    } 
    else if (current_size) {
        // Otherwise, use assignment for the already-constructed elements.
        std::move(a.begin(), a.begin() + current_size, this->begin());
    }

    // Move-construct the new elements in place.
    this->uninitialized_move(a.begin() + current_size, a.end(), this->begin() + current_size);

    // Set end.
    this->set_size(asize);

    a.clear();
    return *this;
}

// Storage for the Array elements.  This is specialized for the N=0 case
// to avoid allocating unnecessary storage.
template <typename T, unsigned N>
struct ArrayStorage {
    alignas(T) char inline_elements[N * sizeof(T)];
};

// Storage must be properly aligned even for small-size of 0 so that the
// pointer math in ArrayCommon::first_element() is well-defined.
template <typename T> struct alignas(T) ArrayStorage<T, 0> {};

// Forward declaration of Array so that CalculateArrayDefaultInlinedElements 
// can reference sizeof(Array<T, 0>).
template <typename T, unsigned N> class Array;

// Helper class for calculating the default number of inline elements for Array<T>.
//
// This should be migrated to a constexpr function when our minimum
// compiler support is enough for multi-statement constexpr functions.
template <typename T> struct CalculateArrayDefaultInlinedElements 
{
    // Parameter controlling the default number of inlined elements for Array<T>.
    //
    // The default number of inlined elements ensures that
    // 1. There is at least one inlined element.
    // 2. sizeof(Array<T>) <= ArrayPreferredArraySize unless it contradicts 1.
    static constexpr size_t ArrayPreferredArraySize = 64;

    // static_assert that sizeof(T) is not "too big".
    //
    // Because our policy guarantees at least one inlined element, it is possible
    // for an arbitrarily large inlined element to allocate an arbitrarily large
    // amount of inline storage. We generally consider it an antipattern for a
    // Array to allocate an excessive amount of inline storage, so we want
    // to call attention to these cases and make sure that users are making an
    // intentional decision if they request a lot of inline storage.
    //
    // This assertion should trigger in pathological cases, but otherwise not be
    // too easy to hit. To accomplish that, the cutoff is actually somewhat larger
    // than ArrayPreferredArraySize (otherwise, Array<Array<T>> would be one easy 
    // way to trip it, and that pattern seems useful in practice).
    //
    // One wrinkle is that this assertion is in theory non-portable, since
    // sizeof(T) is in general platform-dependent. However, we don't expect this
    // to be much of an issue, because most modern development happens on 64-bit
    // hosts, and therefore sizeof(T) is expected to *decrease* when compiled for
    // 32-bit hosts, dodging the issue. The reverse situation, where development
    // happens on a 32-bit host and then fails due to sizeof(T) *increasing* on a
    // 64-bit host, is expected to be very rare.
    static_assert(sizeof(T) <= 256,
        "The default number of inlined elements for Array<T> and sizeof(T) are big.");

    // Discount the size of the header itself when calculating the maximum inline
    // bytes.
    static constexpr size_t PreferredInlineBytes = ArrayPreferredArraySize - sizeof(Array<T, 0>);
    static constexpr size_t NumElementsThatFit = PreferredInlineBytes / sizeof(T);
    static constexpr size_t value = NumElementsThatFit == 0 ? 1 : NumElementsThatFit;
};

// This is a variable-sized array optimized for the case when the array is small. 
// It contains some number of elements in-place, which allows it to avoid heap 
// allocation when the actual number of elements is below that threshold.
// This allows normal "small" cases to be fast without losing generality for large inputs.
//
// In the absence of a well-motivated choice for the number of inlined
// elements N, it is recommended to use Array<T> (that is, omitting the N). 
// This will choose a default number of inlined elements reasonable for allocation 
// on the stack (for example, trying to keep sizeof(Array<T>) around 64 bytes).
//
// Not exception safe.
//
// https://llvm.org/docs/ProgrammersManual.html#llvm-adt-smallvector-h
//
template <typename T, unsigned N = CalculateArrayDefaultInlinedElements<T>::value>
class Array : public ArrayForm<T>, ArrayStorage<T, N> 
{
public:
    Array() : ArrayForm<T>(N) {}

    ~Array() {
        this->call_destructors_on_range(this->begin(), this->end());
    }

    explicit Array(size_t size, const T &Value = T()) : ArrayForm<T>(N) {
        this->assign(size, Value);
    }

    template <typename I, typename = std::enable_if_t<IsInputIteratorCategory<I>>>
        Array(I begin_it, I end_it) : ArrayForm<T>(N) {
        this->append(begin_it, end_it);
    }

    template <typename IR>
    explicit Array(const iterator_range<IR> &R) : ArrayForm<T>(N) {
        this->append(R.begin(), R.end());
    }

    Array(std::initializer_list<T> il) : ArrayForm<T>(N) {
        this->assign(il);
    }

    Array(const Array &a) : ArrayForm<T>(N) {
        if (!a.is_empty()) {
            ArrayForm<T>::operator=(a);
        }
    }

    Array &operator=(const Array &a) {
        ArrayForm<T>::operator=(a);
        return *this;
    }

    Array(Array &&a) : ArrayForm<T>(N) {
        if (!a.is_empty()) {
            ArrayForm<T>::operator=(::std::move(a));
        }
    }

    Array(ArrayForm<T> &&a) : ArrayForm<T>(N) {
        if (!a.is_empty()) {
            ArrayForm<T>::operator=(::std::move(a));
        }
    }

    Array &operator=(Array &&a) {
        if (N) {
            ArrayForm<T>::operator=(::std::move(a));
            return *this;
        }
        // ArrayForm<T>::operator= does not leverage N==0. Optimize the case.
        if (this == &a) {
            return *this;
        }
        if (a.is_empty()) {
            this->call_destructors_on_range(this->begin(), this->end());
            this->size = 0;
        } 
        else {
            this->assign_remote(std::move(a));
        }
        return *this;
    }

    Array &operator=(ArrayForm<T> &&a) {
        ArrayForm<T>::operator=(::std::move(a));
        return *this;
    }

    Array &operator=(std::initializer_list<T> il) {
        this->assign(il);
        return *this;
    }
};

template <typename T, unsigned N>
UU_ALWAYS_INLINE size_t capacity_in_bytes(const Array<T, N> &a) {
    return a.capacity_in_bytes();
}

template <typename RangeType>
using ElementTypeFromRangeType = typename std::remove_const<typename std::remove_reference<
    decltype(*std::begin(std::declval<RangeType &>()))>::type>::type;

// Given a range of type R, iterate the entire range and return a
// Array with elements of the array.  This is useful, for example,
// to iterate a range and then sort the results.
template <unsigned S, typename R>
Array<ElementTypeFromRangeType<R>, S> to_vector(R &&Range) {
    return {std::begin(Range), std::end(Range)};
}

template <typename R>
Array<ElementTypeFromRangeType<R>, 
    CalculateArrayDefaultInlinedElements<ElementTypeFromRangeType<R>>::value>
to_array(R &&Range) {
    return {std::begin(Range), std::end(Range)};
}

}  // namespace UU

#endif  // UU_ARRAY_H

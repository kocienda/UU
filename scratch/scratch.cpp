//
// scratch.cpp
//

#include <bit>

#include <atomic>
#include <format>
#include <iostream>
#include <limits>
#include <mutex>
#include <string>
#include <string_view>

#include <UU/UU.h>

using namespace UU;

#if 0
template <typename ArraySizeT>
class ArrayBase 
{
public:
    UU_ALWAYS_INLINE ArraySizeT size() const { return m_size; }
    UU_ALWAYS_INLINE ArraySizeT capacity() const { return m_capacity; }

    UU_NO_DISCARD UU_ALWAYS_INLINE bool is_empty() const { return m_size == 0; }

protected:
    ArrayBase() = delete;
    ArrayBase(void *base, Size capacity) : m_base(base), m_capacity(capacity) {}

    static constexpr ArraySizeT ArraySizeTMax() {
        return std::numeric_limits<ArraySizeT>::max();
    }

    UU_ALWAYS_INLINE void *base() { return m_base; }

    UU_ALWAYS_INLINE void set_size(ArraySizeT size) { m_size = size; }
    UU_ALWAYS_INLINE void set_capacity(ArraySizeT capacity) const { m_capacity = capacity; }
    UU_ALWAYS_INLINE void set_size(Size size) { ASSERT(size <= capacity()); m_size = size; }

    void *malloc_for_grow(Size min_size, Size t_size, Size &new_capacity);
    void grow_pod(void *p, Size min_size, Size t_size);

    void *m_base = nullptr;
    ArraySizeT m_size = 0;
    ArraySizeT m_capacity = 0;
};

template <typename T>
using ArraySizeType =
    typename std::conditional<sizeof(T) < 4 && sizeof(void *) >= 8, uint64_t, uint32_t>::type;

/// Figure out the offset of the initial element.
template <class T, typename = void> struct ArrayAlignmentAndSize {
    alignas(ArrayBase<ArraySizeType<T>>) char base[sizeof(ArrayBase<ArraySizeType<T>>)];
    alignas(T) char initial[sizeof(T)];
};

template <typename T, typename = void>
class ArrayCommon : public ArrayBase<ArraySizeType<T>> {
    using Base = ArrayBase<ArraySizeType<T>>;

    /// Find the address of the first element.  For this pointer math to be valid
    /// with small-size of 0 for T with lots of alignment, it's important that
    /// ArrayStorage is properly-aligned even for small-size of 0.
    void *get_initial() const {
        const char *char_this = reinterpret_cast<const char *>(this);
        const char *offset_char_this = char_this + offsetof(ArrayAlignmentAndSize<T>, initial);
        return const_cast<void *>(reinterpret_cast<const void *>(offset_char_this));
    }
    // Space after 'initial' is clobbered, do not add any instance vars after it.

protected:
    ArrayCommon(Size size) : Base(get_initial(), size) {}

    void grow_pod(size_t min_size, size_t t_size) {
        Base::grow_pod(get_initial(), min_size, t_size);
    }

    bool is_using_inline_storage() const { return this->base() == get_initial(); }

    void reset_to_inline_storage() {
        this->set_base(get_initial());
        this->set_size(0);
        this->set_capacity(0); // FIXME: setting capacity to 0 is suspect.
    }

    // Return true if v is an internal reference to the given range.
    bool is_reference_to_range(const void *v, const void *first, const void *last) const {
        // Use std::less to avoid UB.
        std::less<> less;
        return !less(v, first) && less(v, last);
    }

    // Return true if v is an internal reference to this vector.
    bool is_reference_to_storage(const void *v) const {
        return is_reference_to_range(v, this->begin(), this->end());
    }

    /// Return true if first and last form a valid (possibly empty) range in this
    /// vector's storage.
    bool is_range_in_storage(const void *first, const void *last) const {
        // Use std::less to avoid UB.
        std::less<> less;
        return !less(first, this->begin()) && !less(last, first) && !less(this->end(), last);
    }

    /// Return true unless elt will be invalidated by resizing the array to new_size.
    bool is_safe_to_reference_after_resize(const void *elt, Size new_size) {
        // Past the end.
        if (LIKELY(!is_reference_to_storage(elt))) {
            return true;
        }

        // Return false if elt will be destroyed by shrinking.
        if (new_size <= this->size()) {
            return elt < this->begin() + new_size;
        }

        // Return false if we need to grow.
        return new_size <= this->capacity();
    }

    void assert_safe_to_reference_after_resize(const void *elt, Size new_size) {
        ASSERT_WITH_MESSAGE(is_safe_to_reference_after_resize(elt, new_size),
            "attempting to reference an element of the array in a way that invalidates it");
    }

    // Check whether elt will be invalidated by increasing the size of the array by N.
    void assert_safe_to_add(const void *elt, size_t n = 1) {
        this->assert_safe_to_reference_after_resize(elt, this->size() + n);
    }

    /// Check whether any part of the range will be invalidated by clearing.
    void assert_safe_to_reference_after_clear(const T *from, const T *to) {
        if (from != to) {
            this->assert_safe_to_reference_after_resize(from, 0);
            this->assert_safe_to_reference_after_resize(to - 1, 0);
        }
    }

    template <class I, std::enable_if_t<!std::is_same<std::remove_const_t<I>, T *>::value, bool> = false>
    void assert_safe_to_reference_after_clear(I, I) {}

    /// Check whether any part of the range will be invalidated by growing.
    void assert_safe_to_add_range(const T *from, const T *to) {
        if (from != to) {
            this->assert_safe_to_add(from, to - from);
            this->assert_safe_to_add(to - 1, to - from);
        }
    }

    template <class I, std::enable_if_t<!std::is_same<std::remove_const_t<I>, T *>::value, bool> = false>
    void assert_safe_to_add_range(I, I) {}

    /// Reserve enough space to add one element, and return the updated element
    /// pointer in case it was a reference to the storage.
    template <class U>
    static const T *reserve_for_param_and_get_address_impl(U *u, const T &elt, size_t n) {
        size_t new_size = u->size() + n;
        if (LIKELY(new_size <= u->capacity())) {
            return &elt;
        }

        bool references_storage = false;
        int64_t index = -1;
        if (!U::TakesParamByValue) {
            if (UNLIKELY(u->is_reference_to_storage(&elt))) {
                references_storage = true;
                index = &elt - u->begin();
            }
        }
        u->grow(new_size);
        return references_storage ? u->begin() + index : &elt;
    }

public:
    using size_type = Size;
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

    using Base::capacity;
    using Base::is_empty;
    using Base::size;

    // forward iterator creation methods.
    iterator begin() { return (iterator)this->base(); }
    const_iterator begin() const { return (const_iterator)this->base(); }
    iterator end() { return begin() + size(); }
    const_iterator end() const { return begin() + size(); }

    // reverse iterator creation methods.
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin());}

    size_type size_in_bytes() const { return size() * sizeof(T); }
    size_type max_size() const { 
        return std::min(this->ArraySizeTMax(), size_type(-1) / sizeof(T)); 
    }

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



// template <typename T>
// class ArrayView : public ArrayBase<>
// {
// };

// template <typename T, Size N>
// class Array : ArrayView<T>
// {
// };

#endif

int main(int argc, const char *argv[]) 
{
    LOG_CHANNEL_ON(General);
    LOG_CHANNEL_ON(Memory);

    

    return 0;
} 
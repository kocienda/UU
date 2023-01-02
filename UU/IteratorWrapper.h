//
// IteratorWrapper.h
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

#ifndef UU_ITERATOR_WRAPPER_H
#define UU_ITERATOR_WRAPPER_H

#include <iterator>
#include <type_traits>

#include <UU/Assertions.h>
#include <UU/Types.h>

namespace UU {

template <typename IteratorT>
class IteratorWrapper
{
public:
    using iterator_type = IteratorT;
    using value_type = typename std::iterator_traits<iterator_type>::value_type;
    using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
    using pointer = typename std::iterator_traits<iterator_type>::pointer;
    using reference = typename std::iterator_traits<iterator_type>::reference;
    using iterator_category = typename std::iterator_traits<iterator_type>::iterator_category;

    constexpr IteratorWrapper() : m_it(nullptr) {}
    constexpr IteratorWrapper(iterator_type &it) : m_it(it) {}
    IteratorWrapper(const iterator_type &it) : m_it(it) {}

    IteratorWrapper(const IteratorWrapper &t) : m_it(t.base()) {}
    IteratorWrapper &operator=(const IteratorWrapper &t) { m_it = t.base(); return *this; }

    template <typename T, typename std::enable_if<std::is_convertible<T, iterator_type>::value>::type* = nullptr> 
    IteratorWrapper(const IteratorWrapper<T> &t) : m_it(t.base()) {}

    template <typename T, typename std::enable_if<std::is_convertible<T, iterator_type>::value>::type* = nullptr> 
    IteratorWrapper &operator=(const IteratorWrapper &t) { m_it = t.base(); return *this; }

    operator iterator_type() noexcept { return m_it; }
    reference operator*() const noexcept { return *m_it; }
    pointer operator->() const noexcept { return m_it; }
    IteratorWrapper &operator++() noexcept { ++m_it; return *this; }
    IteratorWrapper operator++(int) noexcept { auto tmp(*this); ++(*this); return tmp; }
    IteratorWrapper &operator--() noexcept { --m_it; return *this; }
    IteratorWrapper operator--(int) noexcept { auto tmp(*this); --(*this); return tmp; }
    IteratorWrapper operator+(difference_type n) noexcept { auto it(*this); it += n; return it; }
    IteratorWrapper operator-(difference_type n) noexcept { auto it(*this); it -= n; return it; }
    IteratorWrapper &operator+=(difference_type n) noexcept { m_it += n; return *this; }
    IteratorWrapper &operator-=(difference_type n) noexcept { m_it -= n; return *this; }
    reference operator[](difference_type n) noexcept { return m_it[n]; }

    iterator_type base() const noexcept { return m_it; }

private:
    iterator_type m_it;
};

template <typename T>
bool operator==(const IteratorWrapper<T> &a, const IteratorWrapper<T> &b) noexcept {
    return a.base() == b.base();
}

template <typename T1, typename T2>
bool operator==(const IteratorWrapper<T1> &a, const IteratorWrapper<T2> &b) noexcept {
    return a.base() == b.base();
}

template <typename T>
bool operator!=(const IteratorWrapper<T> &a, const IteratorWrapper<T> &b) noexcept {
    return !(a == b);
}

template <typename T1, typename T2>
bool operator!=(const IteratorWrapper<T1> &a, const IteratorWrapper<T2> &b) noexcept {
    return !(a == b);
}

template <typename T>
bool operator<(const IteratorWrapper<T> &a, const IteratorWrapper<T> &b) noexcept {
    return a.base() < b.base();
}

template <typename T1, typename T2>
bool operator<(const IteratorWrapper<T1> &a, const IteratorWrapper<T2> &b) noexcept {
    return a.base() < b.base();
}

template <typename T>
bool operator<=(const IteratorWrapper<T> &a, const IteratorWrapper<T> &b) noexcept {
    return a.base() <= b.base();
}

template <typename T1, typename T2>
bool operator<=(const IteratorWrapper<T1> &a, const IteratorWrapper<T2> &b) noexcept {
    return a.base() <= b.base();
}

template <typename T>
bool operator>(const IteratorWrapper<T> &a, const IteratorWrapper<T> &b) noexcept {
    return a.base() > b.base();
}

template <typename T1, typename T2>
bool operator>(const IteratorWrapper<T1> &a, const IteratorWrapper<T2> &b) noexcept {
    return a.base() > b.base();
}

template <typename T>
bool operator>=(const IteratorWrapper<T> &a, const IteratorWrapper<T> &b) noexcept {
    return a.base() >= b.base();
}

template <typename T1, typename T2>
bool operator>=(const IteratorWrapper<T1> &a, const IteratorWrapper<T2> &b) noexcept {
    return a.base() >= b.base();
}

template <typename T>
IteratorWrapper<T> operator+(typename IteratorWrapper<T>::difference_type n, IteratorWrapper<T> it) {
    it += n;
    return it;
}

template <typename T>
IteratorWrapper<T> operator-(typename IteratorWrapper<T>::difference_type n, IteratorWrapper<T> it) {
    it -= n;
    return it;
}

}  // namespace UU

#endif  // UU_ITERATOR_WRAPPER_H

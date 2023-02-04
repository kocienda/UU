//
// Stretch.h
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

#ifndef UU_STRETCH_H
#define UU_STRETCH_H

#include <algorithm>
#include <iostream>
#include <limits>

#include <UU/Assertions.h>
#include <UU/IteratorWrapper.h>

namespace UU {

template <class ValueT> class Stretch
{
public:
    static constexpr ValueT MinValue = std::numeric_limits<ValueT>::min();
    static constexpr ValueT MaxValue = std::numeric_limits<ValueT>::max();

    static constexpr Stretch all() { return Stretch(MinValue, MaxValue); }
    
    constexpr Stretch() {}
    constexpr Stretch(ValueT first, ValueT last) :
        m_first(std::min(first, last)), m_last(std::max(first, last)) {}

    constexpr void set(ValueT first, ValueT last) { 
        set_first(std::min(first, last)); 
        set_last(std::max(first, last)); 
    }

    constexpr ValueT first() const { return m_first; }
    constexpr void set_first(ValueT first) { m_first = first; }
    
    constexpr ValueT last() const { return m_last; }
    constexpr void set_last(ValueT last) { m_last = last; }

    constexpr ValueT length() const { return m_last - m_first; }
    
    template <bool B=true> constexpr bool contains(ValueT t) const { return (t >= first() && t <= last()) == B; }
    template <bool B=true> constexpr bool empty() const { return (first() == last()) == B; }

    friend constexpr bool operator==(const Stretch &a, const Stretch &b) {
        return a.first() == b.first() && a.last() == b.last();
    };
    
    friend constexpr bool operator!=(const Stretch &a, const Stretch &b) {
        return !(a==b);
    };
    
    static constexpr bool compare(const Stretch &a, const Stretch &b) {
        return a.first() < b.first();
    }
    
    static constexpr bool overlap(const Stretch &a, const Stretch &b) {
        return !(a.last() < b.first() || a.first() > b.last());
    }

    template <bool B=true>
    UU_ALWAYS_INLINE static constexpr bool contains(ValueT c, ValueT lo, ValueT hi) noexcept {
        return ((c >= lo) && (c <= hi)) == B;
    }

    class iterator : public std::input_iterator_tag {
    public:
        using iterator_tag = std::input_iterator_tag;
        using iterator_category = std::input_iterator_tag;
        using difference_type = Int64;
        using value_type = ValueT;
        using pointer = ValueT *;
        using reference = ValueT &;
        
        constexpr iterator() : m_stretch(Stretch()), m_val(0), m_valid(false) {}

        constexpr iterator(const Stretch &stretch, bool valid = true) : 
            m_stretch(stretch), m_val(stretch.first()), m_valid(valid) {}
      
        const ValueT &operator *() const { return m_val; }

        const bool valid() const { return m_valid; }

        iterator &operator++() {
            if (m_valid) {
                if (m_val < m_stretch.last()) {
                    m_val++;
                }
                else {
                    m_valid = false;
                }
            }
            return *this;
        }

        iterator &operator--() {
            if (m_valid) {
                if (m_val > m_stretch.first()) {
                    m_val--;
                }
                else {
                    m_valid = false;
                }
            }
            return *this;
        }

        friend constexpr bool operator==(const iterator &lhs, const iterator &rhs) {
            if (!lhs.m_valid && !rhs.m_valid) {
                return true;
            }
            return lhs.m_valid == rhs.m_valid && lhs.m_val == rhs.m_val;
        }
        friend constexpr  bool operator!=(const iterator &lhs, const iterator &rhs) {
            return !(lhs==rhs);
        }

        friend class Stretch;

    private:
        Stretch m_stretch;
        value_type m_val = 0;
        bool m_valid;
    };

    constexpr iterator begin() const {
        return iterator(*this, true);
    }

    constexpr iterator end() const {
        return iterator(Stretch(), false);
    }

private:
    ValueT m_first = 0;
    ValueT m_last = 0;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const Stretch<T> &r)
{
    if (r.first() == r.last()) {
        os << r.first();
    }
    else {
        os << r.first();
        os << "..";
        os << r.last();
    }
    return os;
}

template <class D, class S> Stretch<D> convert(const Stretch<S> &stretch) {
    const D &first = static_cast<D>(stretch.first());
    const D &last = static_cast<D>(stretch.last());
    return Stretch<D>(first, last);
}

}  // namespace UU

#endif  // UU_STRETCH_H

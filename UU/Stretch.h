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

#ifndef UU_SWEEP_H
#define UU_SWEEP_H

#include <algorithm>
#include <iostream>
#include <limits>

#include <UU/Assertions.h>

namespace UU {

template <class ValueT> class Stretch
{
public:
    static constexpr ValueT MinValue = std::numeric_limits<ValueT>::min();
    static constexpr ValueT MaxValue = std::numeric_limits<ValueT>::max();

    static Stretch full() { return Stretch(MinValue, MaxValue); }
    
    constexpr Stretch() {}
    constexpr Stretch(ValueT first, ValueT last) :
        m_first(std::min(first, last)), m_last(std::max(first, last)) {}

    ValueT first() const { return m_first; }
    void set_first(ValueT first) { m_first = first; }
    
    ValueT last() const { return m_last; }
    void set_last(ValueT last) { m_last = last; }

    ValueT length() const { return m_last - m_first; }
    
    template <bool B=true> bool contains(ValueT t) const { return (t >= first() && t <= last()) == B; }
    template <bool B=true> bool empty() const { return (first() == last()) == B; }

    friend bool operator==(const Stretch &a, const Stretch &b) {
        return a.first() == b.first() && a.last() == b.last();
    };
    
    friend bool operator!=(const Stretch &a, const Stretch &b) {
        return !(a==b);
    };
    
    static bool compare(const Stretch &a, const Stretch &b) {
        return a.first() < b.first();
    }
    
    static bool overlap(const Stretch &a, const Stretch &b) {
        return !(a.last() < b.first() || a.first() > b.last());
    }

    template <bool B=true>
    UU_ALWAYS_INLINE static constexpr bool contains(ValueT c, ValueT lo, ValueT hi) noexcept {
        return ((c >= lo) && (c <= hi)) == B;
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

#endif  // UU_SWEEP_H

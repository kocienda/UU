//
// Range.h
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

#ifndef UU_RANGE_H
#define UU_RANGE_H

#include <algorithm>
#include <iostream>
#include <limits>

namespace UU {

template <class ValueT> class Range
{
public:
    static constexpr ValueT MinValue = std::numeric_limits<ValueT>::min();
    static constexpr ValueT MaxValue = std::numeric_limits<ValueT>::max();

    static Range full() { return Range(MinValue, MaxValue); }
    
    constexpr Range() {}
    constexpr Range(ValueT first, ValueT last) :
        m_first(std::min(first, last)), m_last(std::max(first, last)) {}

    ValueT first() const { return m_first; }
    void set_first(ValueT first) { m_first = first; }
    
    ValueT last() const { return m_last; }
    void set_last(ValueT last) { m_last = last; }

    ValueT length() const { return m_last - m_first; }
    
    template <bool B=true> bool contains(ValueT t) const { return (t >= first() && t <= last()) == B; }
    template <bool B=true> bool empty() const { return (first() == last()) == B; }

    friend bool operator==(const Range &a, const Range &b) {
        return a.first() == b.first() && a.last() == b.last();
    };
    
    friend bool operator!=(const Range &a, const Range &b) {
        return !(a==b);
    };
    
    static bool compare(const Range &a, const Range &b) {
        return a.first() < b.first();
    }
    
    static bool overlap(const Range &a, const Range &b) {
        return !(a.last() < b.first() || a.first() > b.last());
    }
    
private:
    ValueT m_first = 0;
    ValueT m_last = 0;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const Range<T> &r)
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

template <class D, class S> Range<D> convert(const Range<S> &range) {
    const D &first = static_cast<D>(range.first());
    const D &last = static_cast<D>(range.last());
    return Range<D>(first, last);
}

}  // namespace UU

#endif  // UU_RANGE_H

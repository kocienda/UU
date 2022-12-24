//
//  Range.h
//

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
        return !(a.last() < b.first() - 1 || a.first() - 1 > b.last());
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

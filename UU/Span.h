//
//  Span.h
//

#ifndef UU_SPAN_H
#define UU_SPAN_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <UU/Range.h>
#include <UU/SmallVector.h>
#include <UU/StringLike.h>

namespace UU {

template <class ValueT> class Span
{
public:
    using StringT = std::basic_string<ValueT>;
    using RangeT = Range<ValueT>;

    using RangeVector = SmallVector<RangeT, 2>;

    static Span all() {
        return Span(RangeT::MinValue, RangeT::MaxValue);
    }
    
    constexpr Span() {}
    explicit Span(ValueT t) { add(t); };
    explicit Span(ValueT first, ValueT last) { add(first, last); };
    explicit Span(const RangeVector &v) : m_ranges(v) {};
    explicit Span(RangeVector &&v) : m_ranges(std::move(v)) {};
    Span(const Span &span) : m_ranges(span.ranges()) {}
    Span(Span &&span) : m_ranges(std::move(span.ranges())) {}

    Span &operator=(const Span &span) { m_ranges = span.ranges(); return *this; }
    Span &operator=(Span &&span) { m_ranges = std::move(span.ranges()); return *this; }

    explicit Span(const std::vector<ValueT> &v) {
        ValueT prev = 0;
        bool new_range = true;
        
        RangeT range;
        
        for (const auto n : v) {
            if (new_range) {
                new_range = false;
                range.set_first(n);
            }
            else if (n != prev + 1) {
                range.set_last(prev);
                m_ranges.push_back(range);
                range.set_first(n);
                range.set_last(n);
            }
            prev = n;
        }
        if (!new_range) {
            range.set_last(prev);
            m_ranges.push_back(range);
        }
    }
    
    size_t size() const {
        size_t result = 0;
        for (const auto &range : m_ranges) {
            result += 1 + (range.last() - range.first());
        }
        return result;
    }
    
    void add(ValueT t) { m_ranges.emplace_back(t, t); }
    void add(ValueT first, ValueT last) {
            m_ranges.emplace_back(std::min(first, last), std::max(first, last));
    }
    void add(const RangeT &range) { m_ranges.push_back(range); }
    void add(RangeT &&range) { m_ranges.emplace_back(std::move(range)); }
    void add(const Span &span) { add_ranges(span.ranges()); }
    void add(const StringT &string) {
        for (const ValueT &t : string) {
            add(t);
        }
    }
    void add(const std::string &str) {
        std::string buf;
        std::stringstream ss(str);
        
        while (ss >> buf) {
            ValueT i1 = UU::parse_uint<ValueT>(buf).first;
            ValueT i2 = i1;
            std::string::size_type pos = buf.find_first_of("..", 0);
            if (std::string::npos != pos) {
                if (pos == buf.length() - 1) {
                    i2 = std::numeric_limits<ValueT>::max();
                }
                else {
                    std::string buf2(buf.substr(pos+2, buf.length()));
                    i2 = UU::parse_uint<ValueT>(buf2).first;
                    if (i2 < i1) {
                        i2 = i1;
                    }
                }
            }
            add(i1, i2);
        }
    }

    ValueT first() const { return m_ranges.size() ? m_ranges.front().first() : std::numeric_limits<ValueT>::max(); }
    ValueT last() const { return m_ranges.size() ? m_ranges.back().last() : std::numeric_limits<ValueT>::max(); }

    const RangeVector &ranges() const { return m_ranges; }
    void set_ranges(const RangeVector &ranges) { m_ranges = ranges; }
    void add_ranges(const RangeVector &ranges) { m_ranges.insert(m_ranges.end(), ranges.begin(), ranges.end()); }
    
    void clear() { m_ranges.clear(); }
    template <bool B=true> bool is_empty() const { return (m_ranges.size() == 0) == B; }

    template <bool B=true> bool contains(ValueT t) const {
        for (const RangeT &range : m_ranges) {
            if (range.contains(t)) {
                return true;
            }
        }
        return false;
    }

    void simplify() {
        if (m_ranges.size() < 2) {
            return;
        }
        std::sort(m_ranges.begin(), m_ranges.end(), RangeT::compare);
        RangeVector simplified;
        simplified.push_back(m_ranges[0]);
        for (size_t idx = 1; idx < m_ranges.size(); idx++) {
            RangeT &r1 = simplified.back();
            const RangeT &r2 = m_ranges[idx];
            if (RangeT::overlap(r1, r2)) {
                // overlap, collapse
                r1.set_first(std::min(r1.first(), r2.first()));
                r1.set_last(std::max(r1.last(), r2.last()));
            }
            else {
                // no overlap
                simplified.push_back(r2);
            }
        }
        m_ranges = simplified;
    }
    
    class iterator : public std::input_iterator_tag {
    public:
        using iterator_topic = std::input_iterator_tag;
        using difference_type = void;
        using value_type = ValueT;
        using pointer = ValueT *;
        using reference = ValueT &;
        
        iterator(const Span *span) :
            m_span(span && span->ranges().size() ? span : nullptr),
            m_range_idx(0), m_range_val(has_ranges() ? current_range_first() : 0) {}
      
        const value_type &operator *() const { return m_range_val; }

        iterator &operator++() {
            if (m_span) {
                if (m_range_val + 1 <= current_range_last()) {
                    m_range_val++;
                }
                else if (m_range_idx + 1 < ValueT(m_span->ranges().size())) {
                    m_range_idx++;
                    m_range_val = current_range_first();
                }
                else {
                    m_span = nullptr;
                }
            }
            return *this;
        }

        friend bool operator==(const iterator &lhs, const iterator &rhs) {
            return lhs.m_span == rhs.m_span;
        }
        friend bool operator!=(const iterator &lhs, const iterator &rhs) {
            return !(lhs==rhs);
        }

    private:
        bool has_ranges() const { return m_span && m_span->ranges().size(); }
        value_type current_range_first() const { return m_span->ranges()[m_range_idx].first(); }
        value_type current_range_last() const { return m_span->ranges()[m_range_idx].last(); }

        const Span *m_span = nullptr;
        value_type m_range_idx = 0;
        value_type m_range_val = 0;
    };

    iterator begin() const {
        return iterator(this);
    }

    iterator end() const {
        return iterator(nullptr);
    }

    iterator cbegin() const {
        return iterator(this);
    }

    iterator cend() const {
        return iterator(nullptr);
    }

    friend bool operator==(const Span &a, const Span &b) {
        return a.ranges() == b.ranges();
    };

    friend bool operator!=(const Span &a, const Span &b) {
        return !(a==b);
    };

private:
    RangeVector m_ranges;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const Span<T> &span)
{
    bool initial = true;
    for (const auto &r : span.ranges()) {
        if (!initial) {
            os << " ; ";
        }
        os << r;
        initial = false;
    }
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const Span<char32_t> &span);

template <class T>
std::string to_string(const Span<T> &c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

template <class D, class S> Span<D> convert(const Span<S> &src) {
    Span<D> dst;
    for (const auto &range : src.ranges()) {
        dst.add(convert<D, S>(range));
    }
    return dst;
}

}  // namespace UU

#endif  // UU_SPAN_H


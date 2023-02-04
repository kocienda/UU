//
//  Spread.h
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

#ifndef UU_SPREAD_H
#define UU_SPREAD_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <UU/Stretch.h>
#include <UU/SmallVector.h>
#include <UU/StringLike.h>

namespace UU {

template <class ValueT> class Spread
{
public:
    using StringT = std::basic_string<ValueT>;
    using StretchT = Stretch<ValueT>;

    using StretchVector = SmallVector<StretchT, 2>;

    static constexpr Spread all() {
        return Spread(StretchT::MinValue, StretchT::MaxValue);
    }
    
    constexpr Spread() {}
    explicit constexpr Spread(ValueT t) { add(t); };
    explicit constexpr Spread(ValueT first, ValueT last) { add(first, last); };
    explicit constexpr Spread(const StretchVector &v) : m_stretches(v) {};
    explicit constexpr Spread(StretchVector &&v) : m_stretches(std::move(v)) {};
    explicit constexpr Spread(const std::string &s) { add(s); };
    constexpr Spread(const Spread &spread) : m_stretches(spread.stretches()) {}
    constexpr Spread(Spread &&spread) : m_stretches(std::move(spread.stretches())) {}

    constexpr Spread &operator=(const Spread &spread) { m_stretches = spread.stretches(); return *this; }
    constexpr Spread &operator=(Spread &&spread) { m_stretches = std::move(spread.stretches()); return *this; }

    explicit constexpr Spread(const std::vector<ValueT> &v) {
        ValueT prev = 0;
        bool new_stretch = true;
        
        StretchT stretch;
        
        for (const auto n : v) {
            if (new_stretch) {
                new_stretch = false;
                stretch.set_first(n);
            }
            else if (n != prev + 1) {
                stretch.set_last(prev);
                m_stretches.push_back(stretch);
                stretch.set_first(n);
                stretch.set_last(n);
            }
            prev = n;
        }
        if (!new_stretch) {
            stretch.set_last(prev);
            m_stretches.push_back(stretch);
        }
    }
    
    constexpr Size size() const {
        Size result = 0;
        for (const auto &stretch : m_stretches) {
            result += 1 + (stretch.last() - stretch.first());
        }
        return result;
    }
    
    constexpr void add(ValueT t) { m_stretches.emplace_back(t, t); }
    constexpr void add(ValueT first, ValueT last) {
            m_stretches.emplace_back(std::min(first, last), std::max(first, last));
    }
    constexpr void add(const StretchT &stretch) { m_stretches.push_back(stretch); }
    constexpr void add(StretchT &&stretch) { m_stretches.emplace_back(std::move(stretch)); }
    constexpr void add(const Spread &spread) { add_stretches(spread.stretches()); }
    constexpr void add(const StringT &string) {
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

    constexpr ValueT first() const { return m_stretches.size() ? m_stretches.front().first() : std::numeric_limits<ValueT>::max(); }
    constexpr ValueT last() const { return m_stretches.size() ? m_stretches.back().last() : std::numeric_limits<ValueT>::max(); }

    constexpr const StretchVector &stretches() const { return m_stretches; }
    constexpr void set_stretches(const StretchVector &stretches) { m_stretches = stretches; }
    constexpr void add_stretches(const StretchVector &stretches) { m_stretches.insert(m_stretches.end(), stretches.begin(), stretches.end()); }
    
    constexpr void clear() { m_stretches.clear(); }
    template <bool B=true> constexpr bool is_empty() const { return (m_stretches.size() == 0) == B; }

    template <bool B=true> constexpr bool contains(ValueT t) const {
        for (const StretchT &stretch : m_stretches) {
            if (stretch.contains(t)) {
                return true;
            }
        }
        return false;
    }

    constexpr void simplify() {
        if (m_stretches.size() < 2) {
            return;
        }
        std::sort(m_stretches.begin(), m_stretches.end(), StretchT::compare);
        StretchVector simplified;
        simplified.reserve(m_stretches.size());
        simplified.push_back(m_stretches[0]);
        for (size_t idx = 1; idx < m_stretches.size(); idx++) {
            StretchT &r1 = simplified.back();
            const StretchT &r2 = m_stretches[idx];
            if (StretchT::overlap(r1, r2)) {
                // overlap, collapse
                r1.set_first(std::min(r1.first(), r2.first()));
                r1.set_last(std::max(r1.last(), r2.last()));
            }
            else {
                // no overlap
                simplified.push_back(r2);
            }
        }
        m_stretches = simplified;
    }
    
    class iterator : public std::input_iterator_tag {
    public:
        using iterator_tag = std::input_iterator_tag;
        using iterator_category = std::input_iterator_tag;
        using difference_type = Int64;
        using value_type = ValueT;
        using pointer = ValueT *;
        using reference = ValueT &;
        
        iterator(const Spread *spread) :
            m_spread(spread && spread->stretches().size() ? spread : nullptr)
        {
            if (spread && spread->stretches().size()) {
                m_it = m_spread->stretches()[m_stretch_idx].begin();
            }
        }
      
        const ValueT &operator *() const { return *m_it; }

        iterator &operator++() {
            if (m_spread) {
                ++m_it;
                if (!m_it.valid()) {
                    if (m_stretch_idx + 1 < ValueT(m_spread->stretches().size())) {
                        m_stretch_idx++;
                        m_it = m_spread->stretches()[m_stretch_idx].begin();
                    }
                    else {
                        m_spread = nullptr;
                    }
                }
            }
            return *this;
        }

        iterator &operator--() {
            if (m_spread) {
                --m_it;
                if (!m_it.valid()) {
                    if (m_stretch_idx - 1 > 0) {
                        m_stretch_idx--;
                        m_it = m_spread->stretches()[m_stretch_idx].end();
                    }
                    else {
                        m_spread = nullptr;
                    }
                }
            }
            return *this;
        }

        friend bool operator==(const iterator &lhs, const iterator &rhs) {
            return lhs.m_spread == rhs.m_spread;
        }
        friend bool operator!=(const iterator &lhs, const iterator &rhs) {
            return !(lhs==rhs);
        }

        friend class Spread;

    private:
        constexpr bool has_stretches() const { return m_spread && m_spread->stretches().size(); }

        const Spread *m_spread = nullptr;
        int m_stretch_idx = 0;
        typename Stretch<ValueT>::iterator m_it;
    };

    constexpr iterator begin() const {
        return iterator(this);
    }

    constexpr iterator end() const {
        return iterator(nullptr);
    }

    friend bool operator==(const Spread &a, const Spread &b) {
        return a.stretches() == b.stretches();
    };

    friend bool operator!=(const Spread &a, const Spread &b) {
        return !(a==b);
    };

private:
    StretchVector m_stretches;
};

template <class T>
std::ostream &operator<<(std::ostream &os, const Spread<T> &spread)
{
    bool initial = true;
    for (const auto &r : spread.stretches()) {
        if (!initial) {
            os << ",";
        }
        os << r;
        initial = false;
    }
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const Spread<char32_t> &spread);

template <class T>
std::string to_string(const Spread<T> &c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

template <class D, class S> Spread<D> convert(const Spread<S> &src) {
    Spread<D> dst;
    for (const auto &stretch : src.stretches()) {
        dst.add(convert<D, S>(stretch));
    }
    return dst;
}

}  // namespace UU

#endif  // UU_SPREAD_H


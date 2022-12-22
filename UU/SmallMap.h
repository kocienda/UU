//
//  SmallMap.h
//

#ifndef UU_SMALLMAP_H
#define UU_SMALLMAP_H

#include <initializer_list>
#include <iterator>

#include <UU/Assertions.h>
#include <UU/SmallVector.h>
#include <UU/Types.h>

namespace UU {

template <class KeyT, class ValueT, unsigned N> class SmallMap {
public:
    using Entry = std::pair<KeyT, ValueT>;
    using EntryVector = SmallVector<Entry, N>;
    using ValueReferenceT = ValueT &;
    using ValueConstReferenceT = const ValueT &;
    using Iterator = typename EntryVector::iterator;
    using ConstIterator = typename EntryVector::const_iterator;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    SmallMap() {}
    
    SmallMap(std::initializer_list<Entry> list) : m_entries(list) {}

    SmallMap(const SmallMap &other) {
        if (!other.empty()) {
            m_entries = other.entries();
        }
    }

    SmallMap(SmallMap &&other) {
        if (!other.empty()) {
            m_entries(std::move(other.entries()));
        }
    }

    const SmallMap &operator=(const SmallMap &other) {
        m_entries = other.entries();
        return *this;
    }

    UU_ALWAYS_INLINE
    Iterator find(const KeyT &key) {
        for (auto it = begin(); it != end(); ++it) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    UU_ALWAYS_INLINE
    ConstIterator find(const KeyT &key) const {
        for (auto it = begin(); it != end(); ++it) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    std::pair<Iterator, bool> insert(const Entry &entry) {
        std::pair<Iterator, bool> result = std::make_pair(end(), false);
        Iterator it = find(entry.first);
        if (it == end()) {
            m_entries.push_back(entry);
            result.first = m_entries.back();
            result.second = true;
        }
        else {
            result.first = it;
        }
        return result;
    }

    std::pair<Iterator, bool> insert(Entry &&entry) {
        std::pair<Iterator, bool> result = std::make_pair(end(), false);
        Iterator it = find(entry.first);
        if (it == end()) {
            m_entries.push_back(entry);
            result.first = m_entries.end() - 1;
            result.second = true;
        }
        else {
            result.first = it;
        }
        return result;
    }

    std::pair<Iterator, bool> insert(const KeyT &k, const ValueT &v) {
        return insert(std::make_pair(k, v));
    }
    
    std::pair<Iterator, bool> insert_or_assign(const KeyT &k, const ValueT &v) {
        std::pair<Iterator, bool> result = std::make_pair(end(), false);
        Iterator it = find(k);
        if (it == end()) {
            m_entries.push_back(std::make_pair(k, v));
            result.first = m_entries.end() - 1;
            result.second = true;
        }
        else {
            it->second = v;
            result.first = it;
        }
        return result;
    }
    
    std::pair<Iterator, bool> insert_or_assign(KeyT &&k, ValueT &&v) {
        std::pair<Iterator, bool> result = std::make_pair(end(), false);
        Entry entry = std::make_pair(std::move(k), std::forward<ValueT>(v));
        Iterator it = find(entry.first);
        if (it == end()) {
            m_entries.push_back(entry);
            result.first = m_entries.back();
            result.second = true;
        }
        else {
            *it = entry.second;
            result.first = it;
        }
        return result;
    }
    
    UU_ALWAYS_INLINE
    std::pair<Iterator, bool> set(const KeyT &k, const ValueT &v) { return insert_or_assign(k, v); }

    UU_ALWAYS_INLINE
    std::pair<Iterator, bool> set(KeyT &&k, ValueT &&v) { return insert_or_assign(k, v); }

    template <class... Args>
    std::pair<Iterator,bool> emplace(Args&&... args) {
        return insert(std::forward<Entry>(args)...);
    }
    
    UU_ALWAYS_INLINE
    Iterator operator[](const KeyT &key) {
        for (auto it = begin(); it != end(); ++it) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }
    
    UU_ALWAYS_INLINE
    ValueConstReferenceT operator[](const KeyT &key) const {
        for (auto it = begin(); it != end(); ++it) {
            if (it->first == key) {
                return it->second;
            }
        }
        return end();
    }
    
    UU_ALWAYS_INLINE
    ValueConstReferenceT lookup(const KeyT &key, ValueConstReferenceT sentinel) {
        ConstIterator it = find(key);
        return it != end() ? it->second : sentinel;
    }

    bool contains(const KeyT &key) const { return find(key) != end(); }
    template <bool B = true> bool contains(const KeyT &key) const { return contains(key) == B; }
    bool empty() const { return m_entries.empty(); }
    Index size() const { return m_entries.size(); }
    void clear() { m_entries.clear(); }
    void swap(SmallMap &other) { m_entries.swap(other.entries()); }

    const EntryVector &entries() const { return m_entries; }

    UU_ALWAYS_INLINE Iterator begin() { return m_entries.begin(); }
    UU_ALWAYS_INLINE Iterator end() { return m_entries.end(); }
    UU_ALWAYS_INLINE ConstIterator begin() const { return (ConstIterator)m_entries.begin(); }
    UU_ALWAYS_INLINE ConstIterator end() const { return (ConstIterator)m_entries.end(); }

    UU_ALWAYS_INLINE ReverseIterator rbegin() { return m_entries.rbegin(); }
    UU_ALWAYS_INLINE ReverseIterator rend() { return m_entries.rend(); }
    UU_ALWAYS_INLINE ConstReverseIterator rbegin() const { return (ConstIterator)m_entries.rbegin(); }
    UU_ALWAYS_INLINE ConstReverseIterator rend() const { return (ConstIterator)m_entries.rend(); }

    friend bool operator==(const SmallMap &a, const SmallMap &b) {
        if (a.size() != b.size()) {
            return false;
        }
        for (const auto &entry : a.entries()) {
            if (b.contains<false>(entry->first)) {
                return false;
            }
        }
        return true;
    }
    
    friend bool operator!=(const SmallMap &a, const SmallMap &b) { return !(a==b); }
    
private:
    EntryVector &entries() { return m_entries; }
    
    EntryVector m_entries;
};

}  // namespace UU

#endif  // UU_SMALLMAP_H

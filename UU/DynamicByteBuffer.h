//
//  DynamicByteBuffer.h
//

#ifndef UU_DYNAMIC_BYTE_BUFFER_H
#define UU_DYNAMIC_BYTE_BUFFER_H

#include <UU/Assertions.h>
#include <UU/ByteReader.h>
#include <UU/ByteWriter.h>
#include <UU/Types.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <utility>

#include <stdlib.h>

namespace UU {

template <SizeType S>
class BasicDynamicByteBuffer : public ByteWriter, public ByteReader
{
public:
    enum { InlineCapacity = S };

    static constexpr const SizeType nidx = -1;
    static constexpr Byte empty_value = 0;

    constexpr BasicDynamicByteBuffer() {}
    
    explicit BasicDynamicByteBuffer(SizeType capacity) {
        ensure_capacity(capacity);
    }
    
    BasicDynamicByteBuffer(const Byte *bytes, SizeType size) {
        write(bytes, size);
    }
    
    BasicDynamicByteBuffer(const char *ptr, SizeType size) {
        write((const Byte *)ptr, size);
    }

    BasicDynamicByteBuffer(std::istream &in) {
        char buffer[1024];
        while (in.read(buffer, sizeof(buffer))) {
            write((const Byte *)buffer, sizeof(buffer));
        }
        write((const Byte *)buffer, (SizeType)in.gcount());
    }

    BasicDynamicByteBuffer(const BasicDynamicByteBuffer &other) {
        write(other.bytes(), other.size());
    }

    BasicDynamicByteBuffer(BasicDynamicByteBuffer &&other) {
        if (other.using_allocated_buffer()) {
            m_ptr = other.m_ptr;
            m_size = other.size();
            m_capacity = other.capacity();
        }
        else {
            m_size = other.size();
            ensure_capacity(other.capacity());
            copy_from(other.bytes(), other.size());
        }
    }

    BasicDynamicByteBuffer &operator=(const BasicDynamicByteBuffer &other) {
        if (this == &other) {
            return *this;
        }

        m_size = other.size();
        ensure_capacity(other.capacity());
        if (m_size > 0) {
            copy_from(other.bytes(), other.size());
        }

        return *this;
    }

    BasicDynamicByteBuffer &operator=(const std::string &str) {
        m_size = 0;
        write((const Byte *)str.c_str(), (SizeType)str.length());
        return *this;
    }

    BasicDynamicByteBuffer &operator=(BasicDynamicByteBuffer &&other) {
        if (other.using_allocated_buffer()) {
            if (using_allocated_buffer()) {
                delete m_ptr;
            }
            m_ptr = other.m_ptr;
            m_size = other.size();
            m_capacity = other.capacity();
        }
        else {
            m_size = other.size();
            ensure_capacity(other.capacity());
            copy_from(other.bytes(), other.size());
        }
        return *this;
    }

    ~BasicDynamicByteBuffer() {
        if (using_allocated_buffer()) {
            delete m_ptr;
        }
    }

    Byte *bytes() const override { return m_ptr; }
    SizeType size() const override { return m_size; }
    SizeType capacity() const { return m_capacity; }

    void reserve(SizeType size) { ensure_capacity(size); }

    void write(const std::string &str) override {
        write((const Byte *)str.c_str(), (SizeType)str.length());
    }

    void write(const Byte *bytes, SizeType size) override {
        if (LIKELY(size > 0)) {
            ensure_capacity(m_size + size);
            memcpy(m_ptr + m_size, bytes, size);
            m_size += size;
        }
    }
    
    void write(Byte byte) override {
        ensure_capacity(m_size + 1);
        m_ptr[m_size] = byte;
        m_size++;
    }

    BasicDynamicByteBuffer &operator+=(const std::string &s) {
        write(s.c_str(), s.length());
        return *this;
    }

    BasicDynamicByteBuffer &operator+=(const char *s) {
        write(s, strlen(s));
        return *this;
    }

    BasicDynamicByteBuffer &operator+=(Byte b) {
        write(b);
        return *this;
    }

    bool empty() const { return m_size == 0; }
    bool not_empty() const { return !(empty()); }

    bool using_inline_buffer() const { return m_ptr == const_cast<Byte *>(m_buffer); }
    bool using_allocated_buffer() const { return !(using_inline_buffer()); }

    void clear() {
        if (!m_ptr) {
            return;
        }
        
        fill(empty_value, m_size);
        m_size = 0;
    }
    
    Byte at(SizeType index) {
        if (LIKELY(m_size > index)) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const Byte &at(SizeType index) const {
        if (m_size > index) {
            return m_ptr[index];
        }
        else {
            return empty_value;
        }
    }
    
    const Byte &operator[](SizeType index) const {
        return m_ptr[index];
    }
    
    Byte &operator[](SizeType index) {
        return m_ptr[index];
    }

    operator std::string () {
         return std::string((char *)bytes(), size());
    }

    friend bool operator==(const BasicDynamicByteBuffer &a, const BasicDynamicByteBuffer &b) {
        if (a.size() != b.size()) {
            return false;
        }
        if (a.size() == 0) {
            return true;
        }
        return memcmp(a.bytes(), b.bytes(), a.size()) == 0;
    }

    friend bool operator!=(const BasicDynamicByteBuffer &a, const BasicDynamicByteBuffer &b) {
        return !(a == b);
    }

private:
    void ensure_capacity(SizeType new_capacity) {
        if (new_capacity <= InlineCapacity) {
            m_capacity = InlineCapacity;
            return;
        }
    
        if (new_capacity <= m_capacity) {
            return;
        }
    
        while (m_capacity < new_capacity) {
            m_capacity *= 2;
        }

        SizeType amt = m_capacity * sizeof(Byte);
        if (m_ptr != static_cast<Byte *>(m_buffer)) {
            m_ptr = static_cast<Byte *>(realloc(m_ptr, amt));
            fill(m_size, m_capacity - m_size);
        }
        else {
            m_ptr = static_cast<Byte *>(malloc(amt));
            copy_from(m_buffer, InlineCapacity);
            fill(InlineCapacity, m_capacity - InlineCapacity);
        }
    }

    void copy_from(const Byte *src, SizeType size) {
        memcpy(m_ptr, src, size);
    }

    void fill(SizeType index, SizeType size) {
        for (SizeType i = index; i < index + size; i++) {
            m_ptr[i] = empty_value;
        }
    }

    Byte m_buffer[InlineCapacity];
    Byte *m_ptr = m_buffer;
    SizeType m_size = 0;
    SizeType m_capacity = InlineCapacity;
};

using DynamicByteBuffer = BasicDynamicByteBuffer<256>;

}  // namespace UU

#endif  // UU_DYNAMIC_BYTE_BUFFER_H

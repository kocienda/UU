//
// TextRef.h
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

#ifndef UU_TEXT_REF_H
#define UU_TEXT_REF_H

#include <filesystem>
#include <string>
#include <vector>

#include <UU/Spread.h>
#include <UU/Types.h>
#include <UU/UUString.h>

namespace UU {

class TextRef
{
public:
    static constexpr int Index =             0x0001;
    static constexpr int Filename =          0x0002;
    static constexpr int Line =              0x0004;
    static constexpr int Column =            0x0008;
    static constexpr int Spread =              0x0010; 
    static constexpr int Extent =            0x0020; 
    static constexpr int Message =           0x0040;
    static constexpr int HighlightFilename = 0x1000;
    static constexpr int HighlightMessage =  0x2000;

    static constexpr int CompactFeatures = Index | Filename | Line | Message;
    static constexpr int StandardFeatures = Index | Filename | Line | Column | Message;
    static constexpr int ExtendedFeatures = Index | Filename | Line | Spread | Message;


    enum class FilenameFormat { RELATIVE, ABSOLUTE, TERSE };

    static constexpr size_t Invalid = UU::SizeMax;

    static TextRef from_rune_string(const RuneString &str);
    static TextRef from_string(const UU::String &str);

    TextRef() {}
    
    TextRef(const String &filename, size_t line = Invalid, size_t column = Invalid, const std::string &message = std::string()) :
        m_index(NotAnIndex), m_filename(filename), m_line(line), m_message(message) {
        if (column != Invalid) {
            m_spread.add(column);
        }
    }
    
    TextRef(size_t index, const String &filename, size_t line = Invalid, size_t column = Invalid, size_t end_column = Invalid,
        const std::string &message = std::string()) :
        m_index(index), m_filename(filename), m_line(line), m_message(message) {
        if (column != Invalid && end_column != Invalid) {
            m_spread.add(column, end_column);
        }
        else if (column != Invalid) {
            m_spread.add(column);
        }
    }

    TextRef(size_t index, const String &filename, size_t line, const UU::Spread<size_t> spread = UU::Spread<size_t>(), 
        const std::string &message = std::string()) :
        m_index(index), m_filename(filename), m_line(line), m_spread(spread), m_message(message) {}

    size_t index() const { return m_index; }
    void set_index(size_t index) { m_index = index; }
    const String &filename() const { return m_filename; }
    size_t line() const { return m_line; }
    size_t column() const { return m_spread.is_empty() ? Invalid : m_spread.first(); }
    const UU::Spread<size_t> &spread() const { return m_spread; }
    void add_spread(const UU::Spread<size_t> &spread) { m_spread.add(spread); }
    void simplify_spread() { m_spread.simplify(); }
    const String &message() const { return m_message; }
    void set_message(const std::string &message) { m_message = message; }

    String to_string(int flags = TextRef::StandardFeatures, FilenameFormat filename_format = FilenameFormat::RELATIVE,
        const std::filesystem::path &reference_path = std::filesystem::path(""), int highlight_color = 0) const;

    void write_to_string(String &output, int flags = TextRef::StandardFeatures, FilenameFormat filename_format = FilenameFormat::RELATIVE,
        const std::filesystem::path &reference_path = std::filesystem::path(""), int highlight_color = 0) const;

    template <bool B = true> bool has_index() const { return (m_index != Invalid) == B; }
    template <bool B = true> bool has_filename() const { return (!m_filename.empty()) == B; }
    template <bool B = true> bool has_line() const { return (m_line != Invalid) == B; }
    template <bool B = true> bool has_spread() const { return (m_spread.is_empty<false>()) == B; }
    template <bool B = true> bool has_message() const { return (m_message.length() > 0) == B; }

private:
    size_t m_index = Invalid;
    UU::String m_filename;
    size_t m_line = Invalid;
    UU::Spread<size_t> m_spread;
    String m_message;
};

std::ostream &operator<<(std::ostream &os, const TextRef &ref);

}  // namespace UU

namespace std
{
    template <>
    struct less<UU::TextRef>
    {
        bool operator()(const UU::TextRef &lhs, const UU::TextRef &rhs) const {
            if (lhs.filename() != rhs.filename()) {
                return lhs.filename() < rhs.filename();
            }
            if (lhs.line() != rhs.line()) {
                return lhs.line() < rhs.line();
            }
            if (lhs.spread() != rhs.spread()) {
                return lhs.spread().first() < rhs.spread().first();
            }
            return lhs.message() < rhs.message();
        }
    };
}

#endif  // UU_TEXT_REF_H

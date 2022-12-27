//
//  TextRef.h
//

#ifndef UU_TEXT_REF_H
#define UU_TEXT_REF_H

#include <filesystem>
#include <string>
#include <vector>

#include <UU/Span.h>
#include <UU/Types.h>

namespace UU {

class TextRef
{
public:
    static constexpr int Index =       0x01;
    static constexpr int Filename =    0x02;
    static constexpr int Line =        0x04;
    static constexpr int Column =      0x08;
    static constexpr int Span =        0x10; 
    static constexpr int Extent =      0x20; 
    static constexpr int Message =     0x40;
    static constexpr int CompactFeatures = Index | Filename | Line | Message;
    static constexpr int StandardFeatures = Index | Filename | Line | Column | Message;
    static constexpr int ExtendedFeatures = Index | Filename | Line | Span | Message;

    enum class FilenameFormat { RELATIVE, ABSOLUTE };

    static constexpr size_t Invalid = std::numeric_limits<size_t>::max();

    static TextRef from_rune_string(const RuneString &str);
    static TextRef from_string(const std::string &str);

    TextRef() : m_index(NotAnIndex), m_line(Invalid) {}
    
    TextRef(const std::filesystem::path &filename, size_t line = Invalid, size_t column = Invalid, const std::string &message = std::string()) :
        m_index(NotAnIndex), m_filename(filename), m_line(line), m_message(message) {
        if (column != Invalid) {
            m_span.add(column);
        }
    }
    
    // TextRef(size_t index, const std::filesystem::path &filename, size_t line = Invalid, const std::string &message = std::string()) :
    //     m_index(index), m_filename(filename), m_line(line), m_message(message) {}

    // TextRef(size_t index, const std::filesystem::path &filename, size_t line = Invalid, size_t column = Invalid, 
    //     const std::string &message = std::string()) :
    //     m_index(index), m_filename(filename), m_line(line), m_message(message) {
    //     if (column != Invalid) {
    //         m_span.add(column);
    //     }
    // }

    TextRef(size_t index, const std::filesystem::path &filename, size_t line = Invalid, size_t column = Invalid, size_t end_column = Invalid,
        const std::string &message = std::string()) :
        m_index(index), m_filename(filename), m_line(line), m_message(message) {
        if (column != Invalid && end_column != Invalid) {
            m_span.add(column, end_column);
        }
        else if (column != Invalid) {
            m_span.add(column);
        }
    }

    TextRef(size_t index, const std::filesystem::path &filename, size_t line = Invalid, const UU::Span<size_t> span = UU::Span<size_t>(), 
        const std::string &message = std::string()) :
        m_index(index), m_filename(filename), m_line(line), m_span(span), m_message(message) {
    }

    size_t index() const { return m_index; }
    void set_index(size_t index) { m_index = index; }
    const std::filesystem::path &filename() const { return m_filename; }
    size_t line() const { return m_line; }
    size_t column() const { return m_span.is_empty() ? Invalid : m_span.first(); }
    const UU::Span<size_t> &span() const { return m_span; }
    void add_span(const UU::Span<size_t> &span) { m_span.add(span); }
    void simplify_span() { m_span.simplify(); }
    const std::string &message() const { return m_message; }

    std::string to_string(int flags = TextRef::StandardFeatures, FilenameFormat filename_format = FilenameFormat::RELATIVE, 
        const std::filesystem::path &reference_path = std::filesystem::path(""), int highlight_color = 0) const;

    template <bool B = true> bool has_index() const { return (m_index != Invalid) == B; }
    template <bool B = true> bool has_filename() const { return (!m_filename.empty()) == B; }
    template <bool B = true> bool has_line() const { return (m_line != Invalid) == B; }
    template <bool B = true> bool has_span() const { return (m_span.is_empty<false>()) == B; }
    template <bool B = true> bool has_message() const { return (m_message.length() > 0) == B; }

private:
    size_t m_index;
    std::filesystem::path m_filename;
    size_t m_line;
    UU::Span<size_t> m_span;
    std::string m_message;
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
            if (lhs.span() != rhs.span()) {
                return lhs.span().first() < rhs.span().first();
            }
            return lhs.message() < rhs.message();
        }
    };
}

#endif  // UU_TEXT_REF_H

//
//  TextRef.h
//

#ifndef UU_TEXT_REF_H
#define UU_TEXT_REF_H

#include <filesystem>
#include <string>
#include <vector>

#include <UU/Types.h>

namespace UU {

class TextRef
{
public:
    static constexpr int Index =       0x01;
    static constexpr int Filename =    0x02;
    static constexpr int Line =        0x04;
    static constexpr int Column =      0x08; 
    static constexpr int Extent =      0x10; 
    static constexpr int Message =     0x20;
    static constexpr int StandardFeatures = Index | Filename | Line | Column | Message;
    static constexpr int AllFeatures = Index | Filename | Line | Column | Extent | Message;

    enum class FilenameFormat { RELATIVE, ABSOLUTE };

    static constexpr size_t Invalid = std::numeric_limits<size_t>::max();

    static TextRef from_rune_string(const RuneString &str);
    static TextRef from_string(const std::string &str);

    TextRef() : m_index(NotAnIndex), m_line(Invalid), m_column(Invalid) {}
    
    TextRef(const std::filesystem::path &filename, size_t line = Invalid, size_t column = Invalid, const std::string &message = std::string()) :
        m_index(NotAnIndex), m_filename(filename), m_line(line), m_column(column), m_extent(Invalid), m_message(message) {}
    
    TextRef(size_t index, const std::filesystem::path &filename, size_t line = Invalid, const std::string &message = std::string()) :
        m_index(index), m_filename(filename), m_line(line), m_column(Invalid), m_extent(Invalid), m_message(message) {}

    TextRef(size_t index, const std::filesystem::path &filename, size_t line = Invalid, size_t column = Invalid, 
        const std::string &message = std::string()) :
        m_index(index), m_filename(filename), m_line(line), m_column(column), m_extent(Invalid), m_message(message) {}

    TextRef(size_t index, const std::filesystem::path &filename, size_t line = Invalid, size_t column = Invalid, size_t extent = Invalid,
        const std::string &message = std::string()) :
        m_index(index), m_filename(filename), m_line(line), m_column(column), m_extent(extent), m_message(message) {}

    size_t index() const { return m_index; }
    void set_index(size_t index) { m_index = index; }
    const std::filesystem::path &filename() const { return m_filename; }
    size_t line() const { return m_line; }
    size_t column() const { return m_column; }
    size_t extent() const { return m_extent; }
    const std::string &message() const { return m_message; }

    std::string to_string(int flags = TextRef::AllFeatures, FilenameFormat filename_format = FilenameFormat::RELATIVE, 
        const std::filesystem::path &reference_path = std::filesystem::path("")) const;

    template <bool B = true> bool has_index() const { return (m_index != Invalid) == B; }
    template <bool B = true> bool has_filename() const { return (!m_filename.empty()) == B; }
    template <bool B = true> bool has_line() const { return (m_line != Invalid) == B; }
    template <bool B = true> bool has_column() const { return (m_column != Invalid) == B; }
    template <bool B = true> bool has_extent() const { return (m_extent != Invalid) == B; }
    template <bool B = true> bool has_message() const { return (m_message.length() > 0) == B; }

private:
    size_t m_index;
    std::filesystem::path m_filename;
    size_t m_line;
    size_t m_column;
    size_t m_extent;
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
            if (lhs.column() != rhs.column()) {
                return lhs.column() < rhs.column();
            }
            return lhs.message() < rhs.message();
        }
    };
}

#endif  // UU_TEXT_REF_H

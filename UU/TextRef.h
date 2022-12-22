//
//  TextRef.h
//

#ifndef UU_TEXT_REF_H
#define UU_TEXT_REF_H

#include <string>
#include <vector>

#include <UU/Types.h>

namespace UU {

class TextRef
{
public:
    static constexpr int Index =       0x01;
    static constexpr int SourceName =  0x02;
    static constexpr int Line =        0x04;
    static constexpr int Column =      0x08; 
    static constexpr int Message =     0x10;
    static constexpr int AllFeatures = Index | SourceName | Line | Column | Message;

    enum { NotAnIndex = -1, NotALine = -1, NotAColumn = -1 };

    static TextRef from_rune_string(const RuneString &str);
    static TextRef from_string(const std::string &str);

    TextRef() : m_index(NotAnIndex), m_line(NotALine), m_column(NotAColumn) {}
    
    TextRef(const RuneString &source_name, size_t line = NotALine, size_t column = NotAColumn, const RuneString &message = RuneString()) :
        m_index(NotAnIndex), m_source_name(source_name), m_line(line), m_column(column), m_message(message) {}
    
    TextRef(size_t index, const RuneString &source_name, size_t line = NotALine, const RuneString &message = RuneString()) :
        m_index(index), m_source_name(source_name), m_line(line), m_column(NotAColumn), m_message(message) {}

    TextRef(size_t index, const RuneString &source_name, size_t line = NotALine, size_t column = NotAColumn, const RuneString &message = RuneString()) :
        m_index(index), m_source_name(source_name), m_line(line), m_column(column), m_message(message) {}

    size_t index() const { return m_index; }
    const RuneString &source_name() const { return m_source_name; }
    size_t line() const { return m_line; }
    size_t column() const { return m_column; }
    const RuneString &message() const { return m_message; }

    std::string to_string(int flags = TextRef::AllFeatures, bool make_source_name_absolute = false) const;

    template <bool B = true> bool has_index() const { return (m_index != NotAnIndex) == B; }
    template <bool B = true> bool has_source_name() const { return (m_source_name.length() > 0) == B; }
    template <bool B = true> bool has_line() const { return (m_line != NotALine) == B; }
    template <bool B = true> bool has_column() const { return (m_column != NotAColumn) == B; }
    template <bool B = true> bool has_message() const { return (m_message.length() > 0) == B; }

private:
    size_t m_index;
    RuneString m_source_name;
    size_t m_line;
    size_t m_column;
    RuneString m_message;
};

std::ostream &operator<<(std::ostream &os, const TextRef &ref);

}  // namespace UU

#endif  // UU_TEXT_REF_H

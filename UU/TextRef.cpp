//
//  TextRef.cpp
//

#include <filesystem>
#include <iostream>
#include <regex>

#include "StringLike.h"
#include "TextRef.h"

namespace UU {

TextRef TextRef::from_rune_string(const RuneString &str)
{
    return from_string(UU::to_string(str));
}

TextRef TextRef::from_string(const std::string &str)
{
    size_t index = NotAnIndex;
    RuneString path;
    size_t line = NotALine;
    size_t column = NotAColumn;
    RuneString message;

    static std::regex rx("([0-9]+[)][ ]+)([^:]+)(:([0-9]+):(([0-9]+):)?(.*))?");        
    std::cmatch ms;
    if (regex_search(str.c_str(), ms, rx)) {
        auto pindex = UU::parse_unsigned_int(ms[1]);
        if (pindex.second) {
            index = pindex.first;
        }
        path = UU::to_rune_string(ms[2]);
        auto pline = UU::parse_unsigned_int(ms[4]);
        if (pline.second) {
            line = pline.first;
        }
        auto pcolumn = UU::parse_unsigned_int(ms[6]);
        if (pcolumn.second) {
            column = pcolumn.first;
        }
        message = UU::to_rune_string(ms[7]);
    }

    return TextRef(index, path, line, column, message);
}

std::string TextRef::to_string(int flags, bool make_source_name_absolute) const
{
    std::stringstream ss;

    if (has_index() && (flags & TextRef::Index)) {
        ss << index() << ") ";
    }
    if (has_source_name() && (flags & TextRef::SourceName)) {
        if (make_source_name_absolute) {
            ss << std::filesystem::absolute(UU::to_string(source_name()));
        }
        else {
            ss << UU::to_string(source_name());
        }
    }
    if (has_line() && (flags & TextRef::Line)) {
        if (ss.tellp()) {
            ss << ":";
        }
        ss << line();
    }
    if (has_column() && (flags & TextRef::Column)) {
        if (ss.tellp()) {
            ss << ":";
        }
        ss << column();
    }
    if (has_message() && (flags & TextRef::Message)) {
        if (ss.tellp()) {
            ss << ":";
        }
        ss << message();
    }

    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const TextRef &ref)
{
    return os << ref.to_string();
}

}  // namespace UU

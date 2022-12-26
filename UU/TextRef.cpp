//
//  TextRef.cpp
//

#include <filesystem>
#include <iostream>
#include <regex>

#include "Assertions.h"
#include "StringLike.h"
#include "TextRef.h"

namespace fs = std::filesystem;

namespace UU {

TextRef TextRef::from_rune_string(const RuneString &str)
{
    return from_string(UU::to_string(str));
}

TextRef TextRef::from_string(const std::string &str)
{
    size_t index = Invalid;
    fs::path path;
    size_t line = Invalid;
    size_t column = Invalid;
    size_t extent = Invalid;
    std::string message;

    static std::regex rx("([0-9]+[)][ ]+)?([^:]+)(:([0-9]+))?(:([0-9]+))?(:([0-9]+))?(:(.+))?");        
    std::cmatch ms;
    if (regex_match(str.c_str(), ms, rx)) {
        auto pindex = UU::parse_uint<UU::UInt32>(ms[1]);
        if (pindex.second) {
            index = pindex.first;
        }
        path = ms[2];
        auto pline = UU::parse_uint<UU::UInt32>(ms[4]);
        if (pline.second) {
            line = pline.first;
        }
        auto pcolumn = UU::parse_uint<UU::UInt32>(ms[6]);
        if (pcolumn.second) {
            column = pcolumn.first;
        }
        auto pextent = UU::parse_uint<UU::UInt32>(ms[8]);
        if (pextent.second) {
            extent = pextent.first;
        }
        message = ms[10];
    }

    return TextRef(index, path, line, column, extent, message);
}

std::string TextRef::to_string(int flags, FilenameFormat filename_format, const fs::path &reference_path, int highlight_color) const
{
    std::stringstream ss;

    if (has_index() && (flags & TextRef::Index)) {
        ss << index() << ") ";
    }
    if (has_filename() && (flags & TextRef::Filename)) {
        if (filename_format == FilenameFormat::ABSOLUTE) {
            ss << fs::absolute(filename()).c_str();
        }
        else if (!reference_path.empty()) {
            fs::path absolute_reference_path = fs::absolute(reference_path);
            fs::path absolute_filename_path = fs::absolute(filename());
            ss << absolute_filename_path.string().substr(absolute_reference_path.string().length() + 1);
        }
        else {
            ss << filename().c_str();
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
    if (has_extent() && (flags & TextRef::Extent)) {
        if (ss.tellp()) {
            ss << ":";
        }
        ss << extent();
    }
    if (has_message() && (flags & TextRef::Message)) {
        if (ss.tellp()) {
            ss << ":";
        }
        if (highlight_color == 0 || !has_column() || !has_extent() || (column() + extent()) >= message().length()) {
            ss << message();
        }
        else {
            std::string m = message();
            ss << m.substr(0, column() - 1);
            ss << "\033[" << highlight_color << "m";
            ss << m.substr(column() - 1, extent());
            ss << "\033[0m";
            ss << m.substr(column() - 1 + extent());
        }
    }

    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const TextRef &ref)
{
    return os << ref.to_string();
}

}  // namespace UU

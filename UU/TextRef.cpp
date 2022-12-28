//
//  TextRef.cpp
//

#include <filesystem>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>

#include "Assertions.h"
#include "StringLike.h"
#include "TextRef.h"
#include "Span.h"
#include "UnixLike.h"

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
    size_t end_column = Invalid;
    UU::Span<size_t> span;
    std::string message;

    std::cmatch match;

    // optional index:filename:line
    static std::regex rx1("([0-9]+[)][ ]+)?([^:]+):([0-9]+)");        
    if (regex_match(str.c_str(), match, rx1)) {
        // std::cout << "*** match rx1: " << str.length() << ":" << match.length() << std::endl;
        auto pindex = UU::parse_uint<UU::UInt32>(match[1]);
        if (pindex.second) {
            index = pindex.first;
        }
        path = match[2];
        auto pline = UU::parse_uint<UU::UInt32>(match[3]);
        if (pline.second) {
            line = pline.first;
        }
        return TextRef(index, path, line, Invalid);
    }

    // optional index:filename:line:span:optional message
    static std::regex rx2("([0-9]+[)][ ]+)?([^:]+):([0-9]+):([0-9]+((\\.{2}|,)[0-9]+)+)(:(.+))?");        
    if (regex_match(str.c_str(), match, rx2)) {
        // std::cout << "*** match rx2" << std::endl;
        auto pindex = UU::parse_uint<UU::UInt32>(match[1]);
        if (pindex.second) {
            index = pindex.first;
        }
        path = match[2];
        auto pline = UU::parse_uint<UU::UInt32>(match[3]);
        if (pline.second) {
            line = pline.first;
        }
        span = UU::Span<size_t>(match[4]);
        message = match[8];
 
        return TextRef(index, path, line, span, message);
    }

    // optional index:filename:optional line:optional column:optional column end:optional message
    static std::regex rx3("([0-9]+[)][ ]+)?([^:]+)(:([0-9]+))?(:([0-9]+))?(:([0-9]+))?(:(.+))?");        
    if (regex_match(str.c_str(), match, rx3)) {
        // std::cout << "*** match rx3" << std::endl;
        auto pindex = UU::parse_uint<UU::UInt32>(match[1]);
        if (pindex.second) {
            index = pindex.first;
        }
        path = match[2];
        auto pline = UU::parse_uint<UU::UInt32>(match[4]);
        if (pline.second) {
            line = pline.first;
        }
        auto pcolumn = UU::parse_uint<UU::UInt32>(match[6]);
        if (pcolumn.second) {
            column = pcolumn.first;
        }
        auto pend_column = UU::parse_uint<UU::UInt32>(match[8]);
        if (pend_column.second) {
            end_column = pend_column.first;
        }
        message = match[10];
        return TextRef(index, path, line, column, end_column, message);
    }

    return TextRef();
}

UU_ALWAYS_INLINE static void zap_gremlins(std::stringstream &ss, std::string_view &sv) {
    if (contains_gremlins<false>(sv)) {
        ss << sv;
    }
    else {
        // zap
        for (int idx = 0; idx < sv.length(); idx++) {
            if (is_gremlin(sv[idx])) {
                ss << '?';
            }
            else {
                ss << sv[idx];
            }
        }
    }
}

std::string TextRef::to_string(int flags, FilenameFormat filename_format, const fs::path &reference_path, int highlight_color) const
{
    std::stringstream ss;

    if (has_index() && (flags & TextRef::Index)) {
        ss << index() << ") ";
    }
    if (has_filename() && (flags & TextRef::Filename)) {
        std::string path = filename();
        if (filename_format == FilenameFormat::ABSOLUTE) {
            path = fs::absolute(filename());
        }
        else if (!reference_path.empty()) {
            fs::path absolute_reference_path = fs::absolute(reference_path);
            fs::path absolute_filename_path = fs::absolute(filename());
            path = absolute_filename_path.string().substr(absolute_reference_path.string().length() + 1);
        }
        ss << shell_escaped_string(path.c_str());
    }
    if (has_line() && (flags & TextRef::Line)) {
        if (ss.tellp()) {
            ss << ":";
        }
        ss << line();
    }
    if (has_span()) {
        if ((flags & TextRef::Column) && (flags & TextRef::Span) == 0) {
            if (ss.tellp()) {
                ss << ":";
            }
            ss << column();
        }
        else if (flags & TextRef::Span) {
            if (ss.tellp()) {
                ss << ":";
            }
            ss << span();
        }
    }
    if (has_message() && (flags & TextRef::Message)) {
        if (ss.tellp()) {
            ss << ":";
        }
        std::string_view msg(message());
        if (highlight_color == 0 || !has_span()) {
            zap_gremlins(ss, msg);
        }
        else {
            const auto &spn = span();
            size_t idx = 0;
            for (const auto &range : spn.ranges()) {
                const auto first = range.first();
                const auto last = range.last();
                if (first - 1 > idx) {
                    std::string_view chunk = msg.substr(idx, first - 1 - idx);
                    zap_gremlins(ss, chunk);
                }
                ss << "\033[" << highlight_color << "m";
                ss << msg.substr(first - 1, last - first);
                ss << "\033[0m";
                idx = last - 1;
            }
            if (spn.last() - 1 < msg.length()) {
                std::string_view chunk = msg.substr(spn.last() - 1);
                zap_gremlins(ss, chunk);
            }
        }
    }

    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const TextRef &ref)
{
    return os << ref.to_string();
}

}  // namespace UU

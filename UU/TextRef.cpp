//
// TextRef.cpp
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

#include <filesystem>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>

#include "Assertions.h"
#include "StringLike.h"
#include "TextRef.h"
#include "Spread.h"
#include "UnixLike.h"

namespace fs = std::filesystem;

namespace UU {

TextRef TextRef::from_rune_string(const RuneString &str)
{
    return from_string(UU::to_string(str));
}

TextRef TextRef::from_string(const String &str)
{
    size_t index = Invalid;
    fs::path path;
    size_t line = Invalid;
    size_t column = Invalid;
    size_t end_column = Invalid;
    UU::Spread<size_t> spread;
    std::string message;

    String estr = str;
    estr.chomp();

    std::cmatch match;

    // optional index:filename:line
    static std::regex rx1("([0-9]+[)][ ]+)?([^:]+):([0-9]+)");        
    if (regex_match(estr.c_str(), match, rx1)) {
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

    // optional index:filename:line:spread:optional message
    static std::regex rx2("([0-9]+[)][ ]+)?([^:]+):([0-9]+):([0-9]+((\\.{2}|,)[0-9]+)+)(:(.+))?");        
    if (regex_match(estr.c_str(), match, rx2)) {
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
        spread = UU::Spread<size_t>(match[4]);
        message = match[8];
 
        return TextRef(index, path, line, spread, message);
    }

    // optional index:filename:optional line:optional column:optional column end:optional message
    static std::regex rx3("([0-9]+[)][ ]+)?([^:]+)(:([0-9]+))?(:([0-9]+))?(:([0-9]+))?(:(.+))?");        
    if (regex_match(estr.c_str(), match, rx3)) {
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

// UU_ALWAYS_INLINE static void zap_gremlins(std::stringstream &ss, std::string_view &sv) {
//     if (contains_gremlins<false>(sv)) {
//         ss << sv;
//     }
//     else {
//         // zap
//         for (int idx = 0; idx < sv.length(); idx++) {
//             if (is_gremlin(sv[idx])) {
//                 ss << '?';
//             }
//             else {
//                 ss << sv[idx];
//             }
//         }
//     }
// }

static void add_highlight(String &output, const String &str, const Spread<size_t> &spread, int highlight_color) 
{
    size_t idx = 0;
    for (const auto &range : spread.ranges()) {
        const auto first = range.first();
        const auto last = range.last();
        if (first - 1 > idx) {
            StringView chunk = str.substrview(idx, first - 1 - idx);
            output += chunk;
        }
        output += "\033[";
        output.append_as_string(highlight_color);
        output += 'm';
        StringView chunk = str.substrview(first - 1, last - first);
        output += chunk;
        output += "\033[0m";
        idx = last - 1;
    }
    if (spread.last() - 1 < str.length()) {
        StringView chunk = str.substrview(spread.last() - 1);
        output += chunk;
    }
}

String TextRef::to_string(int flags, FilenameFormat filename_format, const fs::path &reference_path, int highlight_color) const
{
    String output;
    output.reserve(m_filename.length() + m_message.length() + 32); // estimate

    if (has_index() && (flags & TextRef::Index)) {
        output.append_as_string(index());
        output += ") ";
    }

    if (has_filename() && (flags & TextRef::Filename)) {
        String output_filename = filename();
        switch (filename_format) {
            case FilenameFormat::RELATIVE:
                if (reference_path.empty()) {
                    output_filename = filename();
                }
                else {
                    fs::path absolute_reference_path = fs::absolute(reference_path);
                    fs::path absolute_filename_path = fs::absolute(filename());
                    output_filename = absolute_filename_path.string().substr(absolute_reference_path.string().length() + 1);
                }
                break;
            case FilenameFormat::ABSOLUTE:
                output_filename = fs::absolute(filename()).string();
                break;
            case FilenameFormat::TERSE:
                output_filename = fs::path(output_filename).filename();
                break;
        }
        String escaped_path = shell_escaped_string(output_filename.c_str());
        if (highlight_color == 0 || ((flags & HighlightFilename) == 0) || !has_spread()) {
            output += output_filename;
        }
        else {
            add_highlight(output, output_filename, spread(), highlight_color);
        }
    }
    if (has_line() && (flags & TextRef::Line)) {
        if (output.length()) {
            output += ':';
        }
        output.append_as_string(line());
    }
    if (has_spread()) {
        if ((flags & TextRef::Column) && (flags & TextRef::Spread) == 0) {
            if (output.length()) {
                output += ':';
            }
            output.append_as_string(column());
        }
        else if (flags & TextRef::Spread) {
            if (output.length()) {
                output += ':';
            }
            output.append(spread());
        }
    }
    if (has_message() && (flags & TextRef::Message)) {
        if (output.length()) {
            output += ':';
        }
        std::string_view msg(message());
        if (highlight_color == 0 || ((flags & HighlightMessage) == 0) || !has_spread()) {
            output += msg;
        }
        else {
            add_highlight(output, msg, spread(), highlight_color);
        }
    }

    return output;
}

std::ostream &operator<<(std::ostream &os, const TextRef &ref)
{
    return os << ref.to_string();
}

}  // namespace UU

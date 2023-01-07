//
// StringLike.cpp
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

#include <limits>

#include "Compiler.h"
#include "StringLike.h"
#include "UTF8.h"

namespace UU {

bool is_valid_utf8(const std::string &s)
{
    std::string::size_type idx = 0;
    std::string::size_type len = s.length();
    const char *p = s.data();
    char32_t c = 0;
    while (idx < len) {
        U8_NEXT(p, idx, len, c);
        if (c == 0xfffd) {
            return false;
        }
    }
    return true;
}

std::vector<Size> find_line_end_offsets(const std::string_view &str, Size max_string_index, Size max_line)
{
    max_string_index = std::min(max_string_index, str.length()); 
    std::vector<Size> result;
    bool added_last_line_ending = false;

    // find all line endings in str up to and including the line with the last match
    Size pos = 0;
    for (;;) {
        pos = str.find_first_of("\r\n", pos);
        if (pos == std::string_view::npos) {
            break;
        }
        result.push_back(pos);
        if (result.size() > max_line) {
            added_last_line_ending = true;
            break;
        }
        pos = str.find_first_not_of("\r\n", pos);
        if (pos == std::string_view::npos) {
            break;
        }
    }
    // add the line end after the last match, or if there is none, the last index in the file
    if (!added_last_line_ending) {
        pos = str.find_first_of("\r\n", pos);
        if (pos != std::string_view::npos) {
            result.push_back(pos);
            added_last_line_ending = true;
        }
    }
    if (!added_last_line_ending) {
        result.push_back(str.length()); // one after the end
    }

    return result;
}

std::string_view string_view_for_line(const std::string_view &str, const std::vector<Size> &line_end_offsets, Size line)
{
    std::string_view result;
    if (line == 0 || line > line_end_offsets.size()) {
        return result;
    }
    Size line_start_offset = 0;
    if (line > 1) {
        line_start_offset = line_end_offsets[line - 2];
        line_start_offset = str.find_first_not_of("\r\n", line_start_offset);
        if (line_start_offset == std::string_view::npos) {
            line_start_offset = str.length();
        }
    }
    Size line_end_offset = line_end_offsets[line - 1];
    line_end_offset = std::max(line_start_offset, line_end_offset);
    Size line_length = line_end_offset - line_start_offset;
    return str.substr(line_start_offset, line_length);
}

std::string_view string_view_for_line(const std::string_view &str, Size line)
{
    std::vector<Size> line_end_offsets = find_line_end_offsets(str);
    return string_view_for_line(str, line_end_offsets, line);
}

}  // namespace UU

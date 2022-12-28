//
//  StringLike.cpp
//

#include <limits>

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

std::vector<SizeType> find_line_end_offsets(const std::string_view &str, SizeType max_string_index, SizeType max_line)
{
    max_string_index = std::min(max_string_index, str.length()); 
    std::vector<SizeType> result;
    bool added_last_line_ending = false;

    // find all line endings in str up to and including the line with the last match
    for (SizeType idx = 0; idx < max_string_index; idx++) {
        if (str[idx] == '\n') {
            result.push_back(idx);
            if (result.size() > max_line || idx == max_string_index - 1) {
                added_last_line_ending = true;
                break;
            }
        }
    }

    // add the line end after the last match, or if there is none, the last index in the file
    for (SizeType idx = max_string_index; idx < str.length(); idx++) {
        if (str[idx] == '\n') {
            result.push_back(idx);
            added_last_line_ending = true;
            break;
        }
    }
    if (!added_last_line_ending) {
        result.push_back(str.length()); // one after the end
    }

    return result;
}

std::string_view string_view_for_line(const std::string_view &str, const std::vector<SizeType> &line_end_offsets, SizeType line)
{
    std::string_view result;
    if (line == 0 || line > line_end_offsets.size()) {
        return result;
    }
    SizeType line_start_offset = line > 1 ? (line_end_offsets[line - 2] + 1) : 0;
    SizeType line_end_offset = line_end_offsets[line - 1];
    line_end_offset = std::max(line_start_offset, line_end_offset);
    SizeType line_length = line_end_offset - line_start_offset;
    return str.substr(line_start_offset, line_length);
}

std::string_view string_view_for_line(const std::string_view &str, SizeType line)
{
    std::vector<SizeType> line_end_offsets = find_line_end_offsets(str);
    return string_view_for_line(str, line_end_offsets, line);
}


}  // namespace UU

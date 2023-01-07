//
// StringLike.h
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

#ifndef UU_STRING_LIKE_H
#define UU_STRING_LIKE_H

#include <codecvt>
#include <limits>
#include <locale>
#include <string>
#include <string_view>

#include <UU/Assertions.h>
#include <UU/Compiler.h>
#include <UU/MathLike.h>
#include <UU/Types.h>
#include <UU/UUString.h>

namespace UU {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
STATIC_INLINE std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> &utf8_char32_conv() {
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv;
}
#pragma clang diagnostic pop

UU_ALWAYS_INLINE std::string to_string(const RuneString &str) {
    return utf8_char32_conv().to_bytes(str);
}

UU_ALWAYS_INLINE std::string to_string(const RuneStringView &str) {
    return to_string(RuneString(str));
}

UU_ALWAYS_INLINE RuneString to_rune_string(const std::string &str) {
    return utf8_char32_conv().from_bytes(str);
}

UU_ALWAYS_INLINE RuneString to_rune_string(const std::string_view &str) {
    return utf8_char32_conv().from_bytes(str.data(), str.data() + str.length());
}

bool is_valid_utf8(const std::string &s);

template <typename U> std::pair<U, bool> parse_uint(const std::string &s, std::size_t *pos = nullptr, int base = 10) {
    try {
        U val = U(std::stoll(s, pos, base));
        return std::make_pair(val, true);
    }
    catch (std::invalid_argument const &ex) {
        return std::make_pair(0, false);
    }
    catch (std::out_of_range const& ex) {
        return std::make_pair(0, false);
    }
}

template <typename I> std::pair<int, bool> parse_int(const std::string &s, std::size_t *pos = nullptr, int base = 10) {
    try {
        I val = I(std::stoll(s, pos, base));
        return std::make_pair(val, true);
    }
    catch (std::invalid_argument const &ex) {
        return std::make_pair(0, false);
    }
    catch (std::out_of_range const& ex) {
        return std::make_pair(0, false);
    }
}

std::vector<Size> find_line_end_offsets(const std::string_view &str, Size max_string_index = SizeMax, Size max_line = SizeMax);
std::pair<Size, Size> offsets_for_line(const std::string_view &str, const std::vector<Size> &line_end_offsets, Size line);
std::string_view string_view_for_line(const std::string_view &str, const std::vector<Size> &line_end_offsets, Size line);
std::string_view string_view_for_line(const std::string_view &str, Size line);

template <bool B = true> bool is_gremlin(char c) { return (c < 32) == B; }
template <bool B = true> bool contains_gremlins(const std::string_view &str) { 
    bool b = false;
    for (size_t idx = 0; idx < str.length(); idx++) {
        if (is_gremlin(str[idx])) {
            b = true;
            break;
        }
    }
    return B == b;
}
template <bool B = true> bool contains_gremlins(const std::string &str) { return (contains_gremlins(std::string_view(str))) == B; }


}  // namespace UU

#endif // UU_STRING_LIKE_H

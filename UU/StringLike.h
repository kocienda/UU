//
//  StringLike.h
//

#ifndef UU_STRING_LIKE_H
#define UU_STRING_LIKE_H

#include <codecvt>
#include <limits>
#include <locale>
#include <string>
#include <string_view>

#include <UU/Compiler.h>
#include <UU/Types.h>

namespace UU {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
STATIC_INLINE std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> &utf8_char32_conv() {
    static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    return conv;
}
#pragma clang diagnostic pop

STATIC_INLINE std::string to_string(const RuneString &str) {
    return utf8_char32_conv().to_bytes(str);
}

STATIC_INLINE std::string to_string(const RuneStringView &str) {
    return to_string(RuneString(str));
}
STATIC_INLINE RuneString to_rune_string(const std::string &str) {
    return utf8_char32_conv().from_bytes(str);
}

STATIC_INLINE RuneString to_rune_string(const std::string_view &str) {
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

std::vector<SizeType> find_line_end_offsets(const std::string_view &str, SizeType max_string_index = SizeTypeMax, SizeType max_line = SizeTypeMax);
std::string_view string_view_for_line(const std::string_view &str, const std::vector<SizeType> &line_end_offsets, SizeType line);
std::string_view string_view_for_line(const std::string_view &str, SizeType line);

}  // namespace UU

#endif // UU_STRING_LIKE_H

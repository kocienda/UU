//
//  Stringy.h
//

#ifndef UU_STRINGY_H
#define UU_STRINGY_H

#include <UU/Compiler.h>
#include <UU/Types.h>

#include <codecvt>
#include <limits>
#include <locale>
#include <string>
#include <string_view>

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

STATIC_INLINE std::pair<unsigned int, bool> parse_unsigned_int(const std::string &s, std::size_t *pos = nullptr, int base = 10) {
    try {
        long val = std::stoi(s, pos, base);
        if (val < 0 || val > std::numeric_limits<unsigned int>::max()) {
            return std::make_pair(0, false);
        }
        return std::make_pair(val, true);
    }
    catch (std::invalid_argument const &ex) {
        return std::make_pair(0, false);
    }
    catch (std::out_of_range const& ex) {
        return std::make_pair(0, false);
    }
}

STATIC_INLINE std::pair<int, bool> parse_int(const std::string &s, std::size_t *pos = nullptr, int base = 10) {
    try {
        int val = std::stoi(s, pos, base);
        return std::make_pair(val, true);
    }
    catch (std::invalid_argument const &ex) {
        return std::make_pair(0, false);
    }
    catch (std::out_of_range const& ex) {
        return std::make_pair(0, false);
    }
}

STATIC_INLINE std::pair<long, bool> parse_long(const std::string &s, std::size_t *pos = nullptr, int base = 10) {
    try {
        long val = std::stol(s, pos, base);
        return std::make_pair(val, true);
    }
    catch (std::invalid_argument const &ex) {
        return std::make_pair(0, false);
    }
    catch (std::out_of_range const& ex) {
        return std::make_pair(0, false);
    }
}

STATIC_INLINE std::pair<long long, bool> parse_long_long(const std::string &s, std::size_t *pos = nullptr, int base = 10) {
    try {
        long long val = std::stoll(s, pos, base);
        return std::make_pair(val, true);
    }
    catch (std::invalid_argument const &ex) {
        return std::make_pair(0, false);
    }
    catch (std::out_of_range const& ex) {
        return std::make_pair(0, false);
    }
}

}  // namespace UU

#endif // UU_STRINGY_H
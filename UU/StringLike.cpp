//
//  StringLike.cpp
//

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

}  // namespace UU

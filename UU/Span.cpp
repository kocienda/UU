//
//  Span.cpp
//

#include "Span.h"

namespace UU {

template <>
std::ostream &operator<<(std::ostream &os, const Span<char32_t> &span)
{
    bool first = true;
    for (const auto &r : span.ranges()) {
        if (!first) {
            os << ",";
        }
        if (r.first() == r.last()) {
            if (r.first() < 256) {
                os << '\'' << (unsigned char)r.first() << '\'';
            }
            else {
                os << std::hex << r.first();
            }
        }
        else {
            if (r.first() < 256) {
                os << '\'' << (unsigned char)r.first() << '\'';
            }
            else {
                os << std::hex << r.first();
            }
            os << "..";
            if (r.last() < 256) {
                os << '\'' << (unsigned char)r.last() << '\'';
            }
            else {
                os << std::hex << r.last();
            }
        }
        first = false;
    }
    return os;
}

}  // namespace UU

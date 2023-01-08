//
// Spread.cpp
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

#include "Spread.h"
#include "UUString.h"

namespace UU {

template <>
std::ostream &operator<<(std::ostream &os, const Spread<char32_t> &spread)
{
    bool first = true;
    for (const auto &r : spread.sweeps()) {
        if (!first) {
            os << ",";
        }
        if (r.first() == r.last()) {
            if (r.first() < 256) {
                unsigned char c = r.first() & 0xff;
                os << '\'' << c << '\'';
            }
            else {
                uint32_t c = r.first();
                os << std::hex << c;
            }
        }
        else {
            if (r.first() < 256) {
                unsigned char c = r.first() & 0xff;
                os << '\'' << c << '\'';
            }
            else {
                uint32_t c = r.first();
                os << std::hex << c;
            }
            os << "..";
            if (r.last() < 256) {
                unsigned char c = r.last() & 0xff;
                os << '\'' << c << '\'';
            }
            else {
                uint32_t c = r.last();
                os << std::hex << c;
            }
        }
        first = false;
    }
    return os;
}

}  // namespace UU

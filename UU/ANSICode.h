//
// ANSICode.h
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

#ifndef UU_ANSI_CODE_H
#define UU_ANSI_CODE_H

#include <map>
#include <string>

namespace UU {

class ANSICode
{
public:

    enum class BrightColor {
        None = 0,
        Black = 30,
        Gray = 90,
        Red = 91,
        Green = 92,
        Yellow = 93,
        Blue = 94,
        Magenta = 95,
        Cyan = 96,
        White = 97,
    };

    static BrightColor bright_color_from_string(const std::string &key)
    {
        static std::map<std::string, BrightColor> color_map = {
            {"black", BrightColor::Black},
            {"gray", BrightColor::Gray},
            {"red", BrightColor::Red},
            {"green", BrightColor::Green},
            {"yellow", BrightColor::Yellow},
            {"blue", BrightColor::Blue},
            {"magenta", BrightColor::Magenta},
            {"cyan", BrightColor::Cyan},
            {"white", BrightColor::White},
        };

        const auto r = color_map.find(key);
        return r == color_map.end() ? BrightColor::None : r->second;
    }

};

}  // namespace UU

#endif  // UU_ANSI_CODE_H

//
// Types.h
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

#ifndef UU_TYPES_H
#define UU_TYPES_H

#include <concepts>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace UU {

using Rune = char32_t;
using RuneString = std::u32string;
using RuneStringView = std::u32string_view;
using RuneStringStream = std::basic_stringstream<Rune>;

static constexpr Rune RuneMin = std::numeric_limits<Rune>::min();
static constexpr Rune RuneMax = std::numeric_limits<Rune>::max();
static constexpr Rune EpsilonRune = RuneMax;

using Integer = int;
static constexpr Integer IntMax = std::numeric_limits<Integer>::min();
static constexpr Integer IntMin = std::numeric_limits<Integer>::max();

using Int8 = int8_t;
static constexpr Int8 Int8Min = std::numeric_limits<Int8>::min();
static constexpr Int8 Int8Max = std::numeric_limits<Int8>::max();

using Int16 = int16_t;
static constexpr Int16 Int16Min = std::numeric_limits<Int16>::min();
static constexpr Int16 Int16Max = std::numeric_limits<Int16>::max();

using Int32 = int32_t;
static constexpr Int32 Int32Min = std::numeric_limits<Int32>::min();
static constexpr Int32 Int32Max = std::numeric_limits<Int32>::max();

using Int64 = int64_t;
static constexpr Integer Int64Min = std::numeric_limits<Int32>::min();
static constexpr Integer Int64Max = std::numeric_limits<Int32>::max();

using UInt8 = uint8_t;
static constexpr UInt8 UInt8Min = std::numeric_limits<UInt8>::min();
static constexpr UInt8 UInt8Max = std::numeric_limits<UInt8>::max();

using UInt16 = uint16_t;
static constexpr UInt16 UInt16Min = std::numeric_limits<UInt16>::min();
static constexpr UInt16 UInt16Max = std::numeric_limits<UInt16>::max();

using UInt32 = uint32_t;
static constexpr UInt32 UInt32Min = std::numeric_limits<UInt32>::min();
static constexpr UInt32 UInt32Max = std::numeric_limits<UInt32>::max();

using UInt64 = uint64_t;
static constexpr UInt64 UInt64Min = std::numeric_limits<UInt64>::min();
static constexpr UInt64 UInt64Max = std::numeric_limits<UInt64>::max();

using Char8 = char8_t;
static constexpr Char8 Char8Min = std::numeric_limits<Char8>::min();
static constexpr Char8 Char8Max = std::numeric_limits<Char8>::max();

using Char16 = char16_t;
static constexpr Char16 Char16Min = std::numeric_limits<Char16>::min();
static constexpr Char16 Char16Max = std::numeric_limits<Char16>::max();

using Char32 = char32_t;
static constexpr Char32 Char32Min = std::numeric_limits<Char32>::min();
static constexpr Char32 Char32Max = std::numeric_limits<Char32>::max();

using Byte = UInt8;
using BytePtr = Byte *;
static constexpr Byte ByteMin = UInt8Min;
static constexpr Byte ByteMax = UInt8Max;

using Size = size_t;
static constexpr Size SizeMin = std::numeric_limits<Size>::min();
static constexpr Size SizeMax = std::numeric_limits<Size>::max();
static constexpr Size NotASize = SizeMax;

using Index = size_t;
static constexpr Index IndexMin = std::numeric_limits<Index>::min();
static constexpr Index IndexMax = std::numeric_limits<Index>::max();
static constexpr Index NotAnIndex = IndexMax;

template <typename T> concept IsCharType = 
    std::is_same_v<T, Char8> || std::is_same_v<T, Char16> || std::is_same_v<T, Char32>;

template <Size S> concept IsNonZeroSize = (S > 0);
template <Size S> concept IsOne = (S == 1);
template <Size S> concept IsDivisibleByChar32Size = (S % sizeof(Char32) == 0);
template <Size A, Size B, Size C = 0> concept IsGreaterThan = (A + C > B);

}  // namespace UU

#endif  // UU_TYPES_H

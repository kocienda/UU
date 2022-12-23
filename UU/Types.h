//
//  Types.h
//

#ifndef UU_TYPES_H
#define UU_TYPES_H

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

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

using Index = uint32_t;
static constexpr Index NotAnIndex = std::numeric_limits<Index>::max();

using Byte = UInt8;
using BytePtr = Byte *;
static constexpr Byte ByteMin = UInt8Min;
static constexpr Byte ByteMax = UInt8Max;

using SizeType = size_t;

}  // namespace UU

#endif  // UU_TYPES_H

//
// scratch.cpp
//

#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <UU/UU.h>
#include <UU/UUText.h>
#include <UU/Types.h>

using namespace UU;
using namespace UU::TextEncoding;

template <std::integral T>
static constexpr T byteswap(T value) noexcept {
    static_assert(std::has_unique_object_representations_v<T>,  "T may not have padding bits");
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::ranges::reverse(value_representation);
    return std::bit_cast<T>(value_representation);
}

int main(int argc, const char *argv[]) {

    // std::cout << static_cast<int>(UTF8Traits::FormV) << std::endl;
    // std::cout << sizeof(UTF8Traits::BOM) << std::endl;
    // std::cout << sizeof(UTF8Traits::CharT) << std::endl;
    // std::cout << sizeof(UTF8Traits::CodePointT) << std::endl;

    UInt32 u = 0xA0B0;
    // UInt32 u = 1 << 12;
    // u++;
    std::cout << u << std::endl;

    Byte cp1 = (u & 0xff00) >> 8;
    Byte cp2 = (u & 0xff);

    std::cout << (int)cp1 << std::endl;
    std::cout << (int)cp2 << std::endl;

    // UInt32 t = byteswap(u);
    // std::cout << t << std::endl;

    // Byte *uptr = reinterpret_cast<Byte *>(&u);
    // Byte *tptr = reinterpret_cast<Byte *>(&t);

    // std::cout << (int)uptr[0] << std::endl;
    // std::cout << (int)uptr[1] << std::endl;
    // std::cout << (int)uptr[2] << std::endl;
    // std::cout << (int)uptr[3] << std::endl;
    // std::cout << "==========" << std::endl;
    // std::cout << (int)tptr[0] << std::endl;
    // std::cout << (int)tptr[1] << std::endl;
    // std::cout << (int)tptr[2] << std::endl;
    // std::cout << (int)tptr[3] << std::endl;

    // UInt16 s = 0x80;
    // std::cout << "0b";
    // for (int idx = 0; idx < 16; idx++) {
    //     const char *b = (s & 0x1) ? "1" : "0";
    //     std::cout << b;
    //     s = s >> 1;
    // }
    // std::cout << std::endl;


    return 0;
} 
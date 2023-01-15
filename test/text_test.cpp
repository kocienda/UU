//
// uu_str_test.cpp
//

#include <UU/UU.h>
#include <UU/UUText.h>
#include <UU/Types.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "text storage smoke", "[text]" ) {

    Storage<32, char32_t> s1;
    Storage<8, char32_t> s2;

    REQUIRE(s1.capacity() == 32);
    REQUIRE(s2.capacity() == 8);
}

TEST_CASE( "UTF8TextEncodingTraits::is_single(CharT)", "[text]" ) {
    REQUIRE(UTF8TextEncodingTraits::is_single('c'));
    REQUIRE_FALSE(UTF8TextEncodingTraits::is_single<false>('c'));
    REQUIRE(UTF8TextEncodingTraits::is_single('c'));
}

#define U8_IS_TRAIL(c) ((int8_t)(c)<-0x40)

TEST_CASE( "UTF8TextEncodingTraits::is_trail(CharT)", "[text]" ) {
    REQUIRE(U8_IS_TRAIL(0x80));
    REQUIRE(UTF8TextEncodingTraits::is_trail(0x80));

    REQUIRE_FALSE(U8_IS_TRAIL(0x7f));
    REQUIRE_FALSE(UTF8TextEncodingTraits::is_trail(0x7f));

    REQUIRE_FALSE(U8_IS_TRAIL(0xC0));
    REQUIRE_FALSE(UTF8TextEncodingTraits::is_trail(0xC0));

    REQUIRE(U8_IS_TRAIL(0x88));
    REQUIRE(UTF8TextEncodingTraits::is_trail(0x88));
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) two byte", "[text]" ) {
    // Code Points:  U+0080..U+07FF
    // First Byte:   C2..DF
    // Second Byte:  80..BF
    Char32 offset = 0x80;
    Char32 idx = 0;
    for (Char8 b1 = 0xC2; b1 <= 0xDF; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0xBF; b2++) {
            const Char8 str[2] = { b1, b2 };
            Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
            Char32 e = offset + idx;
            REQUIRE(c == e);
            idx++;
        }
    }
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) three byte 1", "[text]" ) {
    // Code Points:  U+0800..U+0FFF
    // First Byte:   E0
    // Second Byte:  A0..BF
    // Third Byte:   80..BF
    Char32 offset = 0x800;
    Char32 idx = 0;
    Char8 b1 = 0xE0;
    for (Char8 b2 = 0xA0; b2 <= 0xBF; b2++) {
        for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
            const Char8 str[3] = { b1, b2, b3 };
            Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
            Char32 e = offset + idx;
            REQUIRE(c == e);
            idx++;
        }
    }
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) three byte 2", "[text]" ) {
    // Code Points:  U+1000..U+CFFF
    // First Byte:   E1..EC
    // Second Byte:  80..BF
    // Third Byte:   80..BF
    Char32 offset = 0x1000;
    Char32 idx = 0;
    for (Char8 b1 = 0xE1; b1 <= 0xEC; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0xBF; b2++) {
            for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
                const Char8 str[3] = { b1, b2, b3 };
                Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(c == e);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) three byte 3", "[text]" ) {
    // Code Points:  U+D000..U+D7FF
    // First Byte:   ED
    // Second Byte:  80..9F
    // Third Byte:   80..BF
    Char32 offset = 0xD000;
    Char32 idx = 0;
    for (Char8 b1 = 0xED; b1 <= 0xED; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0x9F; b2++) {
            for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
                const Char8 str[3] = { b1, b2, b3 };
                Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(c == e);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) three byte 4", "[text]" ) {
    // Code Points:  U+E000..U+FFFF
    // First Byte:   EE..EF
    // Second Byte:  80..BF
    // Third Byte:   80..BF
    Char32 offset = 0xE000;
    Char32 idx = 0;
    for (Char8 b1 = 0xEE; b1 <= 0xEF; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0xBF; b2++) {
            for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
                const Char8 str[3] = { b1, b2, b3 };
                Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(c == e);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) four byte 1", "[text]" ) {
    // Code Points:  U+10000..U+3FFFF
    // First Byte:   F0
    // Second Byte:  90..BF
    // Third Byte:   80..BF
    // Fourth Byte:  80..BF
    Char32 offset = 0x10000;
    Char32 idx = 0;
    for (Char8 b1 = 0xF0; b1 <= 0xF0; b1++) {
        for (Char8 b2 = 0x90; b2 <= 0xBF; b2++) {
            for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
                for (Char8 b4 = 0x80; b4 <= 0xBF; b4++) {
                    const Char8 str[4] = { b1, b2, b3, b4 };
                    Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(c == e);
                    idx++;
                }
            }
        }
    }
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) four byte 2", "[text]" ) {
    // Code Points:  U+40000..U+FFFFF
    // First Byte:   F1..F3
    // Second Byte:  80..BF
    // Third Byte:   80..BF
    // Fourth Byte:  80..BF
    Char32 offset = 0x40000;
    Char32 idx = 0;
    for (Char8 b1 = 0xF1; b1 <= 0xF3; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0xBF; b2++) {
            for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
                for (Char8 b4 = 0x80; b4 <= 0xBF; b4++) {
                    const Char8 str[4] = { b1, b2, b3, b4 };
                    Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(c == e);
                    idx++;
                }
            }
        }
    }
}

TEST_CASE( "UTF8TextEncodingTraits::get(const CharT *ptr, Size len, Size bpos) four byte 3", "[text]" ) {
    // Code Points:  U+100000..U+10FFFF
    // First Byte:   F4
    // Second Byte:  80..8F
    // Third Byte:   80..BF
    // Fourth Byte:  80..BF
    Char32 offset = 0x100000;
    Char32 idx = 0;
    for (Char8 b1 = 0xF4; b1 <= 0xF4; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0x8F; b2++) {
            for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
                for (Char8 b4 = 0x80; b4 <= 0xBF; b4++) {
                    const Char8 str[4] = { b1, b2, b3, b4 };
                    Char32 c = UTF8TextEncodingTraits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(c == e);
                    idx++;
                }
            }
        }
    }
}



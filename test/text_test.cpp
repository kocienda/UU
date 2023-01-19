//
// uu_str_test.cpp
//

#include <UU/UU.h>
#include <UU/UUText.h>
#include <UU/Types.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;
using namespace UU::TextEncoding;

TEST_CASE( "text storage smoke", "[text]" ) {

    Storage<32, char32_t> s1;
    Storage<8, char32_t> s2;

    REQUIRE(s1.capacity() == 32);
    REQUIRE(s2.capacity() == 8);
}

TEST_CASE( "UTF8Traits::is_single(CharT)", "[text]" ) {
    REQUIRE(UTF8Traits::is_single('c'));
    REQUIRE_FALSE(UTF8Traits::not_single('c'));
    REQUIRE(UTF8Traits::is_single('c'));
}

#define U8_IS_TRAIL(c) ((int8_t)(c)<-0x40)

TEST_CASE( "UTF8Traits::is_trail(CharT)", "[text]" ) {
    REQUIRE(U8_IS_TRAIL(0x80));
    REQUIRE(UTF8Traits::is_trail(0x80));

    REQUIRE_FALSE(U8_IS_TRAIL(0x7f));
    REQUIRE_FALSE(UTF8Traits::is_trail(0x7f));

    REQUIRE_FALSE(U8_IS_TRAIL(0xC0));
    REQUIRE_FALSE(UTF8Traits::is_trail(0xC0));

    REQUIRE(U8_IS_TRAIL(0x88));
    REQUIRE(UTF8Traits::is_trail(0x88));
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) two byte", "[text]" ) {
    // Code Points:  U+0080..U+07FF
    // First Byte:   C2..DF
    // Second Byte:  80..BF
    Char32 offset = 0x80;
    Char32 idx = 0;
    for (Char8 b1 = 0xC2; b1 <= 0xDF; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0xBF; b2++) {
            const Char8 str[2] = { b1, b2 };
            auto r = UTF8Traits::decode(str, sizeof(str), 0);
            Char32 e = offset + idx;
            REQUIRE(r.code_point == e);
            REQUIRE(r.advance == 2);
            idx++;
        }
    }
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) three byte 1", "[text]" ) {
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
            auto r = UTF8Traits::decode(str, sizeof(str), 0);
            Char32 e = offset + idx;
            REQUIRE(r.code_point == e);
            REQUIRE(r.advance == 3);
            idx++;
        }
    }
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) three byte 2", "[text]" ) {
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
                auto r = UTF8Traits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(r.code_point == e);
                REQUIRE(r.advance == 3);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) three byte 3", "[text]" ) {
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
                auto r = UTF8Traits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(r.code_point == e);
                REQUIRE(r.advance == 3);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) three byte 4", "[text]" ) {
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
                    auto r = UTF8Traits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(r.code_point == e);
                    REQUIRE(r.advance == 3);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) four byte 1", "[text]" ) {
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
                    auto r = UTF8Traits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(r.code_point == e);
                    REQUIRE(r.advance == 4);
                    idx++;
                }
            }
        }
    }
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) four byte 2", "[text]" ) {
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
                    auto r = UTF8Traits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(r.code_point == e);
                    REQUIRE(r.advance == 4);
                    idx++;
                }
            }
        }
    }
}

TEST_CASE( "UTF8Traits::decode(const CharT *ptr, Size len, Size bpos) four byte 3", "[text]" ) {
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
                    auto r = UTF8Traits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(r.code_point == e);
                    REQUIRE(r.advance == 4);
                    idx++;
                }
            }
        }
    }
}

TEST_CASE( "UTF8Traits::decode_bom(const CharT *ptr, Size len) 1", "[text]" ) {
    const Char8 str[4] = { 0x65, 0x66, 0x67, 0x68 };
    auto r = UTF8Traits::decode_bom(str, sizeof(str));
    REQUIRE(r.code_point == 0);
    REQUIRE(r.advance == 0);
}

TEST_CASE( "UTF8Traits::decode_bom(const CharT *ptr, Size len) 2", "[text]" ) {
    const Char8 str[4] = { 0xEF, 0xBB, 0xBF, 0x65 };
    auto r = UTF8Traits::decode_bom(str, sizeof(str));
    REQUIRE(r.code_point == 0);
    REQUIRE(r.advance == 3);
}

TEST_CASE( "UTF8Traits::decode_bom(const CharT *ptr, Size len) 3", "[text]" ) {
    const Char8 str[1] = { 0x65 };
    auto r = UTF8Traits::decode_bom(str, sizeof(str));
    REQUIRE(r.code_point == 0);
    REQUIRE(r.advance == 0);
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) ascii with bom", "[text]" ) {
    const Char8 str[] = { 
        0xEF,0xBB,0xBF,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x57,0x65,0x6C,0x6C,
        0x46,0x6F,0x72,0x6D,0x65,0x64,0x52,0x65,0x73,0x75,0x6C,0x74,0x20,0x7B,
        0x0A,0x20,0x20,0x20,0x20,0x55,0x55,0x5F,0x41,0x4C,0x57,0x41,0x59,0x53,
        0x5F,0x49,0x4E,0x4C,0x49,0x4E,0x45,0x20,0x63,0x6F,0x6E,0x73,0x74,0x65,
        0x78,0x70,0x72,0x20,0x62,0x6F,0x6F,0x6C,0x20,0x69,0x73,0x5F,0x6F,0x6B,
        0x28,0x29,0x20,0x6E,0x6F,0x65,0x78,0x63,0x65,0x70,0x74,0x20,0x7B,0x20,
        0x72,0x65,0x74,0x75,0x72,0x6E,0x20,0x63,0x6F,0x75,0x6E,0x74,0x20,0x3D,
        0x3D,0x20,0x62,0x70,0x6F,0x73,0x3B,0x20,0x7D,0x0A,0x20,0x20,0x20,0x20,
        0x53,0x69,0x7A,0x65,0x20,0x63,0x6F,0x75,0x6E,0x74,0x3B,0x0A,0x20,0x20,
        0x20,0x20,0x53,0x69,0x7A,0x65,0x20,0x62,0x70,0x6F,0x73,0x3B,0x0A,0x7D,
        0x3B,0x0A
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE(r.is_ok());
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) ascii no bom", "[text]" ) {
    const Char8 str[] = { 
        0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x57,0x65,0x6C,0x6C,0x46,0x6F,0x72,
        0x6D,0x65,0x64,0x52,0x65,0x73,0x75,0x6C,0x74,0x20,0x7B,0x0A,0x20,0x20,
        0x20,0x20,0x55,0x55,0x5F,0x41,0x4C,0x57,0x41,0x59,0x53,0x5F,0x49,0x4E,
        0x4C,0x49,0x4E,0x45,0x20,0x63,0x6F,0x6E,0x73,0x74,0x65,0x78,0x70,0x72,
        0x20,0x62,0x6F,0x6F,0x6C,0x20,0x69,0x73,0x5F,0x6F,0x6B,0x28,0x29,0x20,
        0x6E,0x6F,0x65,0x78,0x63,0x65,0x70,0x74,0x20,0x7B,0x20,0x72,0x65,0x74,
        0x75,0x72,0x6E,0x20,0x63,0x6F,0x75,0x6E,0x74,0x20,0x3D,0x3D,0x20,0x62,
        0x70,0x6F,0x73,0x3B,0x20,0x7D,0x0A,0x20,0x20,0x20,0x20,0x53,0x69,0x7A,
        0x65,0x20,0x63,0x6F,0x75,0x6E,0x74,0x3B,0x0A,0x20,0x20,0x20,0x20,0x53,
        0x69,0x7A,0x65,0x20,0x62,0x70,0x6F,0x73,0x3B,0x0A,0x7D,0x3B,0x0A
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE(r.is_ok());
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) japanese with bom", "[text]" ) {
    const Char8 str[] = { 
        0xEF,0xBB,0xBF,0xE4,0xBA,0xAC,0xE9,0x83,0xBD,0xE3,0x81,0xAF,0xE7,0xBE,
        0x8E,0xE3,0x81,0x97,0xE3,0x81,0x84,0xE8,0xA1,0x97,0xE3,0x81,0xA7,0xE3,
        0x81,0x99
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE(r.is_ok());
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) japanese no bom", "[text]" ) {
    const Char8 str[] = { 
        0xE4,0xBA,0xAC,0xE9,0x83,0xBD,0xE3,0x81,0xAF,0xE7,0xBE,0x8E,0xE3,0x81,
        0x97,0xE3,0x81,0x84,0xE8,0xA1,0x97,0xE3,0x81,0xA7,0xE3,0x81,0x99
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE(r.is_ok());
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) sanskrit no bom", "[text]" ) {
    const Char8 str[] = { 
        0xE0,0xA4,0xA8,0xE0,0xA4,0xAE,0xE0,0xA4,0xB8,0xE0,0xA5,0x8D,0xE0,0xA4,
        0x95,0xE0,0xA4,0xBE,0xE0,0xA4,0xB0,0x20,0xE0,0xA4,0x9C,0xE0,0xA4,0x97,
        0xE0,0xA4,0xA4,0xE0,0xA5,0x8D
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE(r.is_ok());
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) sanskrit bad", "[text]" ) {
    const Char8 str[] = { 
        0xE0,0xA4,0xA8,0xE0,0xA4,0xAE,0xE0,0xA4,0xB8,0xE0,0xA5,0x8D,0xE0,0xA4,
        0x95,0xE0,0xA4,0xBE,0xE0,0xA4,0xB0,0x20,0xE0,0xA4,0x9C,0xFF,0xA4,0x97,
        //                                       bad byte here ^^^^
        0xE0,0xA4,0xA4,0xE0,0xA5,0x8D
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE_FALSE(r.is_ok());
    REQUIRE(r.bpos == 25);
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) crank latin no bom", "[text]" ) {
    const Char8 str[] = { 
        0x68,0xC3,0xA9,0x6C,0x6C,0xC3,0xB8,0x20,0x77,0xC3,0xB6,0x72,0xC5,0x82,0x64
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE(r.is_ok());
}

TEST_CASE( "UTF8Traits::is_well_formed(const CharT *ptr, Size len) bad 1", "[text]" ) {
    const Char8 str[] = { 
        0xEF,0xBB,0xBF,0x73,0x74,0xC0,0x75,0x63,0x74,0x20,0x57,0x65,0x6C,0x6C,
        //         bad byte here ^^^^
    };
    auto r = UTF8Traits::is_well_formed(str, sizeof(str));
    REQUIRE_FALSE(r.is_ok());
    REQUIRE(r.bpos == 5);
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) two byte 1", "[text]" ) {
    // Code Points:  U+0080..U+07FF
    // First Byte:   C2..DF
    // Second Byte:  80..BF
    Char32 offset = 0x80;
    Char32 idx = 0;
    for (Char8 b1 = 0xC2; b1 <= 0xDF; b1++) {
        for (Char8 b2 = 0x80; b2 <= 0xBF; b2++) {
            Char32 code_point = offset + idx;
            auto r = UTF8Traits::encode(code_point);
            StaticByteBuffer<4> buf = { b1, b2, 0, 0 };
            REQUIRE(r.is_ok());
            REQUIRE(r.bytes == buf);
            idx++;
        }
    }
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) three byte 1", "[text]" ) {
    // Code Points:  U+0800..U+0FFF
    // First Byte:   E0
    // Second Byte:  A0..BF
    // Third Byte:   80..BF
    Char32 offset = 0x800;
    Char32 idx = 0;
    Char8 b1 = 0xE0;
    for (Char8 b2 = 0xA0; b2 <= 0xBF; b2++) {
        for (Char8 b3 = 0x80; b3 <= 0xBF; b3++) {
            Char32 code_point = offset + idx;
            auto r = UTF8Traits::encode(code_point);
            StaticByteBuffer<4> buf = { b1, b2, b3 };
            // std::cout << (int)b1 << " : " << (int)r.bytes[0] << std::endl;
            // std::cout << (int)b2 << " : " << (int)r.bytes[1] << std::endl;
            // std::cout << (int)b3 << " : " << (int)r.bytes[2] << std::endl;
            REQUIRE(r.is_ok());
            REQUIRE(r.bytes == buf);
            idx++;
        }
    }
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) three byte 2", "[text]" ) {
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
                auto r = UTF8Traits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(r.code_point == e);
                REQUIRE(r.advance == 3);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) three byte 3", "[text]" ) {
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
                auto r = UTF8Traits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(r.code_point == e);
                REQUIRE(r.advance == 3);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) three byte 4", "[text]" ) {
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
                auto r = UTF8Traits::decode(str, sizeof(str), 0);
                Char32 e = offset + idx;
                REQUIRE(r.code_point == e);
                REQUIRE(r.advance == 3);
                idx++;
            }
        }
    }
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) four byte 1", "[text]" ) {
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
                    auto r = UTF8Traits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(r.code_point == e);
                    REQUIRE(r.advance == 4);
                    idx++;
                }
            }
        }
    }
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) four byte 2", "[text]" ) {
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
                    auto r = UTF8Traits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(r.code_point == e);
                    REQUIRE(r.advance == 4);
                    idx++;
                }
            }
        }
    }
}

TEST_CASE( "UTF8Traits::encode(CodePointT code_point) four byte 3", "[text]" ) {
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
                    auto r = UTF8Traits::decode(str, sizeof(str), 0);
                    Char32 e = offset + idx;
                    REQUIRE(r.code_point == e);
                    REQUIRE(r.advance == 4);
                    idx++;
                }
            }
        }
    }
}

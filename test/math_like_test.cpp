//
// math_like_test.cpp
//

#include <string>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "integer to string 1", "[string_like]" ) {
    UInt64 n = 1234567890123456789;
    std::string s = integer_to_string(n);
    REQUIRE(s == "1234567890123456789");
}

TEST_CASE( "integer to string 2", "[string_like]" ) {
    int n = -765;
    std::string s = integer_to_string(n);
    REQUIRE(s == "-765");
}

TEST_CASE( "integer to string 3", "[string_like]" ) {
    UInt64 n = 765;
    std::string s = integer_to_string(n);
    REQUIRE(s == "765");
}

TEST_CASE( "integer to string 4", "[string_like]" ) {
    Int64 n = -1234567890123456789;
    std::string s = integer_to_string(n);
    REQUIRE(s == "-1234567890123456789");
}

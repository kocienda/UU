//
// string_test.cpp
//

#include <string>
#include <sstream>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE("String append smoke test 1", "[string]" ) {
    String str;
    str.append("hello");
    str.append('-');
    str.append_as_string(1234567890123456789);
    std::string s = str;
    REQUIRE(s == "hello-1234567890123456789");
}

TEST_CASE("String append span test 1", "[string]" ) {
    String str;
    Span<int> span;
    span.add(1,3);
    span.add(5,7);
    span.add(11);
    str.append(span);
    std::string s = str;
    REQUIRE(s == "1..3,5..7,11");
}

TEST_CASE("String BasicString(SizeType length, CharT c) constructor test", "[string]" ) {
    String str(16, 'g');
    String exp = "gggggggggggggggg";
    REQUIRE(str == exp);
}

TEST_CASE("String convert to std::string with operator std::basic_string", "[string]" ) {
    String str("hello there");
    std::string dat = str;
    REQUIRE(dat == "hello there");
}

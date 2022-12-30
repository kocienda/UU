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
    REQUIRE(strlen(str.c_str()) == 16);
}

TEST_CASE("String convert to std::string with operator std::basic_string", "[string]" ) {
    String str("hello there");
    REQUIRE(strlen(str.c_str()) == 11);
    std::string dat = str;
    REQUIRE(dat == "hello there");
}

TEST_CASE("String iterator test 1", "[string]" ) {
    String str("hello!");
    REQUIRE(strlen(str.c_str()) == 6);
    int index = 0;
    for (auto it = str.begin(); it != str.end(); ++it) {
        index++;
    }
    REQUIRE(index == 6);
}

TEST_CASE("String iterator test 2", "[string]" ) {
    String str1("hello!");
    REQUIRE(strlen(str1.c_str()) == 6);
    String str2;
    for (auto it = str1.begin(); it != str1.end(); ++it) {
        str2.append(*it);
    }
    REQUIRE(str1 == str2);
}

TEST_CASE("String iterator test 2a", "[string]" ) {
    String str1("12345");
    String str2;
    auto it = str1.begin();
    ++it;
    str2 += it[0];
    str2 += it[1];
    REQUIRE(str2 == "23");
    REQUIRE(strlen(str2.c_str()) == 2);
}

TEST_CASE("String iterator test 2b", "[string]" ) {
    std::string str1("12345");
    std::string str2;
    auto it = str1.begin();
    ++it;
    str2 += it[0];
    str2 += it[1];
    REQUIRE(str2 == "23");
    REQUIRE(strlen(str2.c_str()) == 2);
}

TEST_CASE("String iterator test 3", "[string]" ) {
    String str1("1234567890");
    String str2;
    for (auto it = str1.begin(); it != str1.end(); it += 2) {
        str2.append(*it);
    }
    REQUIRE(str2 == "13579");
    REQUIRE(strlen(str2.c_str()) == 5);
}

TEST_CASE("String reverse_iterator append test", "[string]" ) {
    String str1("1234567890");
    String str2;
    for (auto it = str1.rbegin(); it != str1.rend(); ++it) {
        str2.append(*it);
    }
    REQUIRE(str2 == "0987654321");
    REQUIRE(strlen(str2.c_str()) == 10);
}

TEST_CASE("std::string reverse_iterator append test", "[string]" ) {
    std::string str1("1234567890");
    std::string str2;
    for (auto it = str1.rbegin(); it != str1.rend(); ++it) {
        str2 += *it;
    }
    REQUIRE(str2 == "0987654321");
}

TEST_CASE("String iterator test comparators", "[string]" ) {
    String str1("1234567890");
    auto it1 = str1.begin();
    auto it2 = str1.begin();
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    ++it2;
    REQUIRE_FALSE(it1 == it2);
    REQUIRE(it1 != it2);
    REQUIRE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE_FALSE(it2 <= it1);
    REQUIRE_FALSE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 > 0);
    REQUIRE(it1 + 1 == it2);
    REQUIRE(it2 - 1 == it1);
    --it2;
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    REQUIRE(it1 + 1 > it2);
    REQUIRE(it2 - 1 != it1);
}

TEST_CASE("std::string iterator test comparators", "[string]" ) {
    std::string str1("1234567890");
    auto it1 = str1.begin();
    auto it2 = str1.begin();
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    ++it2;
    REQUIRE_FALSE(it1 == it2);
    REQUIRE(it1 != it2);
    REQUIRE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE_FALSE(it2 <= it1);
    REQUIRE_FALSE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 > 0);
    REQUIRE(it1 + 1 == it2);
    REQUIRE(it2 - 1 == it1);
    --it2;
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    REQUIRE(it1 + 1 > it2);
    REQUIRE(it2 - 1 != it1);
}

TEST_CASE("std::string reverse_iterator test comparators", "[string]" ) {
    std::string str1("1234567890");
    auto it1 = str1.begin();
    auto it2 = str1.begin();
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    ++it2;
    REQUIRE_FALSE(it1 == it2);
    REQUIRE(it1 != it2);
    REQUIRE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE_FALSE(it2 <= it1);
    REQUIRE_FALSE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 > 0);
    REQUIRE(it1 + 1 == it2);
    REQUIRE(it2 - 1 == it1);
    --it2;
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    REQUIRE(it1 + 1 > it2);
    REQUIRE(it2 - 1 != it1);
}

TEST_CASE("String reverse_iterator test comparators", "[string]" ) {
    String str1("1234567890");
    auto it1 = str1.begin();
    auto it2 = str1.begin();
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    ++it2;
    REQUIRE_FALSE(it1 == it2);
    REQUIRE(it1 != it2);
    REQUIRE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE_FALSE(it2 <= it1);
    REQUIRE_FALSE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 > 0);
    REQUIRE(it1 + 1 == it2);
    REQUIRE(it2 - 1 == it1);
    --it2;
    REQUIRE(it1 == it2);
    REQUIRE_FALSE(it1 != it2);
    REQUIRE_FALSE(it1 < it2);
    REQUIRE_FALSE(it2 < it1);
    REQUIRE_FALSE(it1 > it2);
    REQUIRE_FALSE(it2 > it1);
    REQUIRE(it1 <= it2);
    REQUIRE(it2 <= it1);
    REQUIRE(it1 >= it2);
    REQUIRE(it2 >= it1);
    REQUIRE(it2 - it1 == 0);
    REQUIRE(it1 + 1 > it2);
    REQUIRE(it2 - 1 != it1);
}

TEST_CASE("String insert test", "[string]" ) {
    String str1("0123456789");
    str1.insert(5, 3, 'a');
    REQUIRE(str1 == "01234aaa");
    REQUIRE(strlen(str1.c_str()) == 8);
}

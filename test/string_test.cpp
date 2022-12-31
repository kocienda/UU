//
// string_test.cpp
//

#include "UU/UUString.h"
#include <string>
#include <sstream>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE("String ctor test: std::initializer_list", "[string]" ) {
    String ustr{ 'a', 'b', 'c' };
    REQUIRE(ustr == "abc");
}

TEST_CASE("String ctor test: BasicString(SizeType length, CharT c)", "[string]" ) {
    String str(16, 'g');
    String exp = "gggggggggggggggg";
    REQUIRE(str == exp);
    REQUIRE(strlen(str.c_str()) == 16);
}

TEST_CASE("BasicString<Char32> override", "[string]" ) {
    BasicString<char32_t> ustr1("");
    ustr1.append("abc", 3);
    ustr1.append(U"abc", 3);
    BasicString<char32_t> ustr2(U"abcabc");
    bool t = ustr1 == ustr2;
    REQUIRE(t);
    REQUIRE(std::string(ustr1) == std::string(ustr2));
    REQUIRE(strcmp(ustr1.c_str(), ustr2.c_str()) == 0);
}

TEST_CASE("String append smoke test 1", "[string]" ) {
    String str;
    str.append("hello");
    str.append('-').append_as_string(1234567890123456789);
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

TEST_CASE("String append test 1: constexpr BasicString &insert(const StringViewLikeT &t)", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    std::string sstr2 = "abc";

    sstr1.append(sstr2);
    ustr1.append(sstr2);
    REQUIRE(sstr1 == "0123456789abc");
    REQUIRE(ustr1 == "0123456789abc");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("String append test 2: constexpr BasicString &append(const StringViewLikeT &t)", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    String sstr2 = "abc";

    sstr1.append(sstr2);
    ustr1.append(sstr2);
    REQUIRE(sstr1 == "0123456789abc");
    REQUIRE(ustr1 == "0123456789abc");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
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
    for (auto it = str1.begin(); it < str1.end(); it += 2) {
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

TEST_CASE("String insert test: SizeType index, SizeType count, CharT c", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");
    
    sstr.insert(5, 3, 'a');
    ustr.insert(5, 3, 'a');
    REQUIRE(sstr == "01234aaa56789");
    REQUIRE(ustr == "01234aaa56789");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);

    sstr.insert(sstr.length(), 3, 'a');
    ustr.insert(ustr.length(), 3, 'a');
    REQUIRE(sstr == "01234aaa56789aaa");
    REQUIRE(ustr == "01234aaa56789aaa");
    REQUIRE(strlen(sstr.c_str()) == 16);
    REQUIRE(strlen(ustr.c_str()) == 16);
}

TEST_CASE("String insert test: SizeType index, const CharT *s", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");
    
    sstr.insert(5, "aaa");
    ustr.insert(5, "aaa");
    REQUIRE(sstr == "01234aaa56789");
    REQUIRE(ustr == "01234aaa56789");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);

    sstr.insert(sstr.length(), "aaa");
    ustr.insert(ustr.length(), "aaa");
    REQUIRE(sstr == "01234aaa56789aaa");
    REQUIRE(ustr == "01234aaa56789aaa");
    REQUIRE(strlen(sstr.c_str()) == 16);
    REQUIRE(strlen(ustr.c_str()) == 16);
}

TEST_CASE("String insert test: SizeType index, const CharT *s, SizeType count", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");
    
    sstr.insert(5, "abcdef", 3);
    ustr.insert(5, "abcdef", 3);
    REQUIRE(sstr == "01234abc56789");
    REQUIRE(ustr == "01234abc56789");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);

    sstr.insert(sstr.length(), "abcdef", 3);
    ustr.insert(ustr.length(), "abcdef", 3);
    REQUIRE(sstr == "01234abc56789abc");
    REQUIRE(ustr == "01234abc56789abc");
    REQUIRE(strlen(sstr.c_str()) == 16);
    REQUIRE(strlen(ustr.c_str()) == 16);
}

TEST_CASE("String insert test: SizeType index, const BasicString &str, SizeType index_str, SizeType count", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    String ustr1("0123456789");
    String ustr2("abcdefghij");
    
    sstr1.insert(5, sstr2, 3, 3);
    ustr1.insert(5, ustr2, 3, 3);
    REQUIRE(sstr1 == "01234def56789");
    REQUIRE(ustr1 == "01234def56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);

    sstr1.insert(sstr1.length(), sstr2, 3, std::string::npos);
    ustr1.insert(ustr1.length(), sstr2, 3, String::npos);
    REQUIRE(sstr1 == "01234def56789defghij");
    REQUIRE(ustr1 == "01234def56789defghij");
    REQUIRE(strlen(sstr1.c_str()) == 20);
    REQUIRE(strlen(ustr1.c_str()) == 20);
}

TEST_CASE("std::string insert test: const_iterator pos, CharT ch", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");
    
    sstr.insert(sstr.begin(), 'a');
    ustr.insert(ustr.begin(), 'a');
    REQUIRE(sstr == "a0123456789");
    REQUIRE(ustr == "a0123456789");
    REQUIRE(strlen(sstr.c_str()) == 11);
    REQUIRE(strlen(ustr.c_str()) == 11);

    sstr.insert(sstr.begin() + 3, 'b');
    ustr.insert(ustr.begin() + 3, 'b');
    REQUIRE(sstr == "a01b23456789");
    REQUIRE(ustr == "a01b23456789");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("String insert test: const_iterator pos, CharT ch", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.insert(sstr.begin(), 'a');
    ustr.insert(ustr.begin(), 'a');
    REQUIRE(sstr == "a0123456789");
    REQUIRE(ustr == "a0123456789");
    REQUIRE(strlen(sstr.c_str()) == 11);
    REQUIRE(strlen(ustr.c_str()) == 11);

    sstr.insert(sstr.begin() + 3, 'b');
    ustr.insert(ustr.begin() + 3, 'b');
    REQUIRE(sstr == "a01b23456789");
    REQUIRE(ustr == "a01b23456789");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("String insert test: const_iterator pos, InputIt first, InputIt last", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    String ustr1("0123456789");
    String ustr2("abcdefghij");

    sstr1.insert(sstr1.begin() + 5, sstr2.begin(), sstr2.end());
    ustr1.insert(ustr1.begin() + 5, ustr2.begin(), ustr2.end());
    REQUIRE(sstr1 == "01234abcdefghij56789");
    REQUIRE(ustr1 == "01234abcdefghij56789");
    REQUIRE(strlen(sstr1.c_str()) == 20);
    REQUIRE(strlen(ustr1.c_str()) == 20);
}

TEST_CASE("String insert test: const_iterator pos, std::initializer_list<CharT> ilist", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    sstr1.insert(sstr1.begin() + 5, { 'a', 'b', 'c' });
    ustr1.insert(ustr1.begin() + 5, { 'a', 'b', 'c' });
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("String insert test 1: constexpr BasicString &insert(SizeType index, const StringViewLikeT &t)", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    std::string sstr2 = "abc";

    sstr1.insert(5, sstr2);
    ustr1.insert(5, sstr2);
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("String insert test 2: constexpr BasicString &insert(SizeType index, const StringViewLikeT &t)", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    String sstr2 = "abc";

    sstr1.insert(5, sstr2);
    ustr1.insert(5, sstr2);
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("String insert test 1: SizeType index, const StringViewLikeT &t, SizeType index_str, SizeType count", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    std::string sstr2 = "abc";

    sstr1.insert(5, sstr2);
    ustr1.insert(5, sstr2);
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("String insert test 2: SizeType index, const StringViewLikeT &t, SizeType index_str, SizeType count", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    String sstr2 = "abcdefghij";

    sstr1.insert(5, sstr2, 3, 4);
    ustr1.insert(5, sstr2, 3, 4);
    REQUIRE(sstr1 == "01234defg56789");
    REQUIRE(ustr1 == "01234defg56789");
    REQUIRE(strlen(sstr1.c_str()) == 14);
    REQUIRE(strlen(ustr1.c_str()) == 14);
}

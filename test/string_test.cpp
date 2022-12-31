//
// string_test.cpp
//

#include "UU/UUString.h"
#include <string>
#include <sstream>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

// constructing ===================================================================================

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
    REQUIRE(ustr1 == ustr2);
    REQUIRE(strcmp(ustr1.c_str(), ustr2.c_str()) == 0);
}

// appending ======================================================================================

TEST_CASE("String::append(SizeType count, CharT c)", "[string]" ) {
    std::string sstr("hello");
    String ustr("hello");

    sstr.append(1, '-');
    sstr.append(5, 'a');

    ustr.append(1, '-');
    ustr.append(5, 'a');

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "hello-aaaaa");
    REQUIRE(ustr == "hello-aaaaa");
    REQUIRE(strlen(sstr.c_str()) == 11);
    REQUIRE(strlen(ustr.c_str()) == 11);
}

TEST_CASE("String::append(const String &str)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    String ustr1("hello ");
    String ustr2("world!");

    sstr1.append(sstr2);
    ustr1.append(ustr2);

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "hello world!");
    REQUIRE(ustr1 == "hello world!");
    REQUIRE(strlen(sstr1.c_str()) == 12);
    REQUIRE(strlen(ustr1.c_str()) == 12);
}

TEST_CASE("BasicString<char32_t>::append(const BasicString<char32_t> &str)", "[string]" ) {
    std::basic_string<char32_t> sstr1(U"hello ");
    std::basic_string<char32_t> sstr2(U"world!");
    BasicString<char32_t> ustr1(U"hello ");
    BasicString<char32_t> ustr2(U"world!");

    sstr1.append(sstr2);
    ustr1.append(ustr2);

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == U"hello world!");
    REQUIRE(ustr1 == U"hello world!");
    REQUIRE(sstr1.length() == 12);
    REQUIRE(ustr1.length() == 12);
}

TEST_CASE("BasicString<char32_t>::append(const std::string &str)", "[string]" ) {
    std::basic_string<char32_t> sstr1(U"hello world!");
    BasicString<char32_t> ustr1(U"hello ");
    std::string sstr2("world!");

    ustr1.append(sstr2);

    REQUIRE(ustr1 == sstr1);
    REQUIRE(ustr1 == U"hello world!");
    REQUIRE(ustr1.length() == 12);
}

TEST_CASE("String::append(const CharT *ptr, SizeType length)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    String ustr("hello ");

    sstr.append(cstr, strlen(cstr));
    ustr.append(cstr, strlen(cstr));

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "hello world!");
    REQUIRE(ustr == "hello world!");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("BasicString<char32_t>::append(const CharT *ptr, SizeType length)", "[string]" ) {
    const char *cstr("world!");
    std::basic_string<char32_t> sstr(U"hello world!");
    BasicString<char32_t> ustr(U"hello ");

    ustr.append(cstr, strlen(cstr));

    REQUIRE(ustr == sstr);
    REQUIRE(ustr == U"hello world!");
    REQUIRE(ustr.length() == 12);
}

TEST_CASE("String::append(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    String ustr("hello ");

    sstr.append(cstr);
    ustr.append(cstr);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "hello world!");
    REQUIRE(ustr == "hello world!");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("String::append(InputIt first, InputIt last)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    String ustr1("0123456789");
    String ustr2("abcdefghij");

    sstr1.append(sstr2.begin() + 3, sstr2.end());
    ustr1.append(ustr2.begin() + 3, ustr2.end());

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "0123456789defghij");
    REQUIRE(ustr1 == "0123456789defghij");
    REQUIRE(strlen(sstr1.c_str()) == 17);
    REQUIRE(strlen(ustr1.c_str()) == 17);
}

TEST_CASE("String::append(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.append({ 'a', 'b', 'c' });
    ustr.append({ 'a', 'b', 'c' });

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("String::append(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.append(vstr_view);
    ustr.append(vstr_view);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("String::append(const StringViewLikeT &t, SizeType pos, SizeType count)", "[string]" ) {
    std::string_view vstr("abcdefghij");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.append(vstr_view, 3, 4);
    ustr.append(vstr_view, 3, 4);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789defg");
    REQUIRE(ustr == "0123456789defg");
    REQUIRE(strlen(sstr.c_str()) == 14);
    REQUIRE(strlen(ustr.c_str()) == 14);
}

TEST_CASE("String::append(CharT c)", "[string]" ) {
    std::string sstr("0123456789a");
    String ustr("0123456789");

    ustr.append('a');

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789a");
    REQUIRE(ustr == "0123456789a");
    REQUIRE(strlen(sstr.c_str()) == 11);
    REQUIRE(strlen(ustr.c_str()) == 11);
}

TEST_CASE("String::append(const Span &)", "[string]" ) {
    String str;
    Span<int> span;
    span.add(1,3);
    span.add(5,7);
    span.add(11);
    str.append(span);
    std::string s = str;
    REQUIRE(s == "1..3,5..7,11");
}

// inserting ======================================================================================

TEST_CASE("String::insert(SizeType index, SizeType count, CharT c)", "[string]" ) {
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

TEST_CASE("String::insert(SizeType index, const CharT *s)", "[string]" ) {
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

TEST_CASE("String::insert(SizeType index, const CharT *s, SizeType count)", "[string]" ) {
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

TEST_CASE("String::insert(SizeType index, const BasicString& str)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    String ustr1("0123456789");
    String ustr2("abcdefghij");
    
    sstr1.insert(5, sstr2);
    ustr1.insert(5, ustr2);
    REQUIRE(sstr1 == "01234abcdefghij56789");
    REQUIRE(ustr1 == "01234abcdefghij56789");
    REQUIRE(strlen(sstr1.c_str()) == 20);
    REQUIRE(strlen(ustr1.c_str()) == 20);
}

TEST_CASE("String::insert(SizeType index, const BasicString &str, SizeType index_str, SizeType count)", "[string]" ) {
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

TEST_CASE("String::insert(const_iterator pos, CharT ch)", "[string]" ) {
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

TEST_CASE("String::insert(const_iterator pos, SizeType count, CharT ch)", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.insert(sstr.begin(), 4, 'a');
    ustr.insert(ustr.begin(), 4, 'a');
    REQUIRE(sstr == "aaaa0123456789");
    REQUIRE(ustr == "aaaa0123456789");
    REQUIRE(strlen(sstr.c_str()) == 14);
    REQUIRE(strlen(ustr.c_str()) == 14);
}

TEST_CASE("String::insert(const_iterator pos, InputIt first, InputIt last)", "[string]" ) {
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

TEST_CASE("String::insert(const_iterator pos, std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    sstr1.insert(sstr1.begin() + 5, { 'a', 'b', 'c' });
    ustr1.insert(ustr1.begin() + 5, { 'a', 'b', 'c' });
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("String::insert(SizeType index, const StringViewLikeT &t)", "[string]" ) {
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

TEST_CASE("String::insert(SizeType index, const StringViewLikeT &t, SizeType index_str, SizeType count = npos)", "[string]" ) {
    std::string sstr1("0123456789");
    String ustr1("0123456789");

    std::string sstr2 = "abc";
    std::string_view vstr(sstr2);

    sstr1.insert(5, vstr);
    ustr1.insert(5, vstr);
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

// iterating ======================================================================================

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

// operators ======================================================================================

TEST_CASE("String convert to std::string with operator std::basic_string", "[string]" ) {
    String str("hello there");
    REQUIRE(strlen(str.c_str()) == 11);
    std::string dat = str;
    REQUIRE(dat == "hello there");
}





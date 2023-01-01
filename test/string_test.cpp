//
// string_test.cpp
//

#include "UU/UUString.h"
#include <cstring>
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

// starts_with ====================================================================================

TEST_CASE("String::starts_with(const StringViewLikeT &t)", "[string]" ) {
    String str1("0123456789abcdefghij");
    
    String str2("0123456789abcdefghij");
    REQUIRE(str1.starts_with(str2));

    str2 = "0123456789";
    REQUIRE(str1.starts_with(str2));

    str2 = "0";
    REQUIRE(str1.starts_with(str2));

    str2 = "";
    REQUIRE(str1.starts_with(str2));

    str2 = "0123456789abcdefghijk";
    REQUIRE_FALSE(str1.starts_with(str2));

    str2 = "abcdef";
    REQUIRE_FALSE(str1.starts_with(str2));
}

TEST_CASE("String::starts_with(CharT c)", "[string]" ) {
    String str1("0123456789");
    
    REQUIRE(str1.starts_with('0'));
    REQUIRE_FALSE(str1.starts_with('1'));
}

TEST_CASE("String::starts_with(const CharT *s)", "[string]" ) {
    String str1("0123456789abcdefghij");
    
    REQUIRE(str1.starts_with("0123456789abcdefghij"));
    REQUIRE(str1.starts_with("0123456789"));
    REQUIRE(str1.starts_with("0"));
    REQUIRE(str1.starts_with(""));
    REQUIRE_FALSE(str1.starts_with("0123456789abcdefghijk"));
    REQUIRE_FALSE(str1.starts_with("abcdef"));
}

// ends_with ======================================================================================

TEST_CASE("String::ends_with(const StringViewLikeT &t)", "[string]" ) {
    String str1("0123456789abcdefghij");
    
    String str2("0123456789abcdefghij");
    REQUIRE(str1.ends_with(str2));

    str2 = "abcdefghij";
    REQUIRE(str1.ends_with(str2));

    str2 = "j";
    REQUIRE(str1.ends_with(str2));

    str2 = "";
    REQUIRE(str1.ends_with(str2));

    str2 = "0123456789abcdefghijk";
    REQUIRE_FALSE(str1.ends_with(str2));

    str2 = "012345";
    REQUIRE_FALSE(str1.ends_with(str2));
}

TEST_CASE("String::ends_with(CharT c)", "[string]" ) {
    String str1("0123456789");
    
    REQUIRE(str1.ends_with('9'));
    REQUIRE_FALSE(str1.ends_with('1'));
}

TEST_CASE("String::ends_with(const CharT *s)", "[string]" ) {
    String str1("0123456789abcdefghij");
    
    REQUIRE(str1.ends_with("0123456789abcdefghij"));
    REQUIRE(str1.ends_with("abcdefghij"));
    REQUIRE(str1.ends_with("j"));
    REQUIRE(str1.ends_with(""));
    REQUIRE_FALSE(str1.ends_with("0123456789abcdefghijk"));
    REQUIRE_FALSE(str1.ends_with("012345"));
}

// contains =======================================================================================

TEST_CASE("String::contains(const StringViewLikeT &t)", "[string]" ) {
    String str1("0123456789abcdefghij");
    
    String str2("0123456789abcdefghij");
    REQUIRE(str1.contains(str2));

    str2 = "abcdefghij";
    REQUIRE(str1.contains(str2));

    str2 = "6789abc";
    REQUIRE(str1.contains(str2));

    str2 = "j";
    REQUIRE(str1.contains(str2));

    str2 = "";
    REQUIRE(str1.contains(str2));

    str2 = "6789abce";
    REQUIRE_FALSE(str1.contains(str2));

    str2 = "0123456789abcdefghijk";
    REQUIRE_FALSE(str1.contains(str2));

    str2 = "0123459";
    REQUIRE_FALSE(str1.contains(str2));
}

TEST_CASE("String::contains(CharT c)", "[string]" ) {
    String str1("0123456789");
    
    REQUIRE(str1.contains('9'));
    REQUIRE(str1.contains('0'));
    REQUIRE(str1.contains('1'));
    REQUIRE(str1.contains('5'));
    REQUIRE_FALSE(str1.contains('a'));
}

TEST_CASE("String::contains(const CharT *s)", "[string]" ) {
    String str1("0123456789abcdefghij");
    
    REQUIRE(str1.contains("0123456789abcdefghij"));
    REQUIRE(str1.contains("abcdefghij"));
    REQUIRE(str1.contains("789abcdef"));
    REQUIRE(str1.contains("01"));
    REQUIRE(str1.contains("12"));
    REQUIRE(str1.contains("567"));
    REQUIRE(str1.contains("0"));
    REQUIRE(str1.contains("ij"));
    REQUIRE(str1.contains("j"));
    REQUIRE(str1.contains(""));
    REQUIRE_FALSE(str1.contains("0123456789abcdefghijk"));
    REQUIRE_FALSE(str1.contains("012345a"));
    REQUIRE_FALSE(str1.contains("k"));
    REQUIRE_FALSE(str1.contains("k"));
}

// copy ==========================================================================================

TEST_CASE("String::copy(CharT* dst, SizeType count, SizeType pos = 0)", "[string]" ) {
    char buf[32];
    String str("0123456789abcdefghij");
    
    SizeType c1 = str.copy(buf, 10);
    REQUIRE(memcmp(buf, "0123456789", 10) == 0);
    REQUIRE(c1 == 10);

    SizeType c2 = str.copy(buf, 10, 5);
    REQUIRE(memcmp(buf, "56789abcde", 10) == 0);
    REQUIRE(c2 == 10);

    SizeType c3 = str.copy(buf, 4, 16);
    REQUIRE(memcmp(buf, "ghij", 4) == 0);
    REQUIRE(c3 == 4);
}

// push and pop ===================================================================================

TEST_CASE("String push and pop", "[string]" ) {
    String ustr("12345");

    ustr.push_back('6');
    REQUIRE(ustr == "123456");
    REQUIRE(ustr.length() == 6);

    ustr.push('7');
    REQUIRE(ustr == "1234567");
    REQUIRE(ustr.length() == 7);

    ustr.pop_back();
    REQUIRE(ustr == "123456");
    REQUIRE(ustr.length() == 6);

    String::CharType c = ustr.pop();
    REQUIRE(ustr == "12345");
    REQUIRE(ustr.length() == 5);
    REQUIRE(c == '6');
}

// assigning ======================================================================================

TEST_CASE("String::assign(SizeType count, CharT c)", "[string]" ) {
    std::string sstr("hello");
    String ustr("hello");

    sstr.assign(5, 'a');
    ustr.assign(5, 'a');

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "aaaaa");
    REQUIRE(ustr == "aaaaa");
    REQUIRE(strlen(sstr.c_str()) == 5);
    REQUIRE(strlen(ustr.c_str()) == 5);
}

TEST_CASE("String::assign(const String &str)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    String ustr1("hello ");
    String ustr2("world!");

    sstr1.assign(sstr2);
    ustr1.assign(ustr2);

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "world!");
    REQUIRE(ustr1 == "world!");
    REQUIRE(strlen(sstr1.c_str()) == 6);
    REQUIRE(strlen(ustr1.c_str()) == 6);
}

TEST_CASE("String::assign(BasicString &&str)", "[string]" ) {
    std::string sstr1("1234567890");
    String ustr1("1234567890");

    sstr1.assign("abcdefghij");
    ustr1.assign("abcdefghij");

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(strlen(sstr1.c_str()) == 10);
    REQUIRE(strlen(ustr1.c_str()) == 10);
}

TEST_CASE("String::assign(const CharT *ptr, SizeType length)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    String ustr("hello ");

    sstr.assign(cstr, strlen(cstr));
    ustr.assign(cstr, strlen(cstr));

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "world!");
    REQUIRE(ustr == "world!");
    REQUIRE(strlen(sstr.c_str()) == 6);
    REQUIRE(strlen(ustr.c_str()) == 6);
}

TEST_CASE("String::assign(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    String ustr("hello ");

    sstr.assign(cstr);
    ustr.assign(cstr);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "world!");
    REQUIRE(ustr == "world!");
    REQUIRE(strlen(sstr.c_str()) == 6);
    REQUIRE(strlen(ustr.c_str()) == 6);
}

TEST_CASE("String::assign(InputIt first, InputIt last)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    String ustr1("0123456789");
    String ustr2("abcdefghij");

    sstr1.assign(sstr2.begin() + 3, sstr2.end());
    ustr1.assign(ustr2.begin() + 3, ustr2.end());

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "defghij");
    REQUIRE(ustr1 == "defghij");
    REQUIRE(strlen(sstr1.c_str()) == 7);
    REQUIRE(strlen(ustr1.c_str()) == 7);
}

TEST_CASE("String::assign(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.assign({ 'a', 'b', 'c' });
    ustr.assign({ 'a', 'b', 'c' });

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("String::assign(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.assign(vstr_view);
    ustr.assign(vstr_view);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("String::assign(const StringViewLikeT &t, SizeType pos, SizeType count)", "[string]" ) {
    std::string_view vstr("abcdefghij");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr.assign(vstr_view, 3, 4);
    ustr.assign(vstr_view, 3, 4);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "defg");
    REQUIRE(ustr == "defg");
    REQUIRE(strlen(sstr.c_str()) == 4);
    REQUIRE(strlen(ustr.c_str()) == 4);
}

// operator= ======================================================================================

TEST_CASE("String::operator=(const String &other)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    String ustr1("hello ");
    String ustr2("world!");

    sstr1 = sstr2;
    ustr1 = ustr2;

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "world!");
    REQUIRE(ustr1 == "world!");
    REQUIRE(strlen(sstr1.c_str()) == 6);
    REQUIRE(strlen(ustr1.c_str()) == 6);
}

TEST_CASE("String::operator=(BasicString &&str)", "[string]" ) {
    std::string sstr1("1234567890");
    String ustr1("1234567890");

    sstr1 = std::string("abcdefghij");
    ustr1 = String("abcdefghij");

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(strlen(sstr1.c_str()) == 10);
    REQUIRE(strlen(ustr1.c_str()) == 10);
}

TEST_CASE("String::operator=(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    String ustr("hello ");

    sstr = cstr;
    ustr = cstr;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "world!");
    REQUIRE(ustr == "world!");
    REQUIRE(strlen(sstr.c_str()) == 6);
    REQUIRE(strlen(ustr.c_str()) == 6);
}

TEST_CASE("String::operator=(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr = { 'a', 'b', 'c' };
    ustr = { 'a', 'b', 'c' };

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("String::operator=(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr = vstr_view;
    ustr = vstr_view;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
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

TEST_CASE("String::append(const BasicString &str, SizeType pos, SizeType count)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    String ustr1("0123456789");
    String ustr2("abcdefghij");

    sstr1.append(sstr2, 5);
    ustr1.append(ustr2, 5);

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "0123456789fghij");
    REQUIRE(ustr1 == "0123456789fghij");
    REQUIRE(strlen(sstr1.c_str()) == 15);
    REQUIRE(strlen(ustr1.c_str()) == 15);
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
    REQUIRE(str == "1..3,5..7,11");
}

// operator+= =====================================================================================

TEST_CASE("String::operator+=(const String &str)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    String ustr1("hello ");
    String ustr2("world!");

    sstr1 += sstr2;
    ustr1 += ustr2;

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "hello world!");
    REQUIRE(ustr1 == "hello world!");
    REQUIRE(strlen(sstr1.c_str()) == 12);
    REQUIRE(strlen(ustr1.c_str()) == 12);
}

TEST_CASE("BasicString<char32_t>::operator+=(const BasicString<char32_t> &str)", "[string]" ) {
    std::basic_string<char32_t> sstr1(U"hello ");
    std::basic_string<char32_t> sstr2(U"world!");
    BasicString<char32_t> ustr1(U"hello ");
    BasicString<char32_t> ustr2(U"world!");

    sstr1 += sstr2;
    ustr1 += ustr2;

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == U"hello world!");
    REQUIRE(ustr1 == U"hello world!");
    REQUIRE(sstr1.length() == 12);
    REQUIRE(ustr1.length() == 12);
}

TEST_CASE("BasicString<char32_t>::operator+=(const std::string &str)", "[string]" ) {
    std::basic_string<char32_t> sstr1(U"hello world!");
    BasicString<char32_t> ustr1(U"hello ");
    std::string sstr2("world!");

    ustr1 += sstr2;

    REQUIRE(ustr1 == sstr1);
    REQUIRE(ustr1 == U"hello world!");
    REQUIRE(ustr1.length() == 12);
}

TEST_CASE("String::operator+=(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    String ustr("hello ");

    sstr += cstr;
    ustr += cstr;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "hello world!");
    REQUIRE(ustr == "hello world!");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("String::operator+=(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr += { 'a', 'b', 'c' };
    ustr += { 'a', 'b', 'c' };

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("String::operator+=(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    String ustr("0123456789");

    sstr += vstr_view;
    ustr += vstr_view;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("String::operator+=(CharT c)", "[string]" ) {
    std::string sstr("0123456789a");
    String ustr("0123456789");

    ustr += 'a';

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789a");
    REQUIRE(ustr == "0123456789a");
    REQUIRE(strlen(sstr.c_str()) == 11);
    REQUIRE(strlen(ustr.c_str()) == 11);
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

// swapping =======================================================================================

TEST_CASE("String::swap 1", "[string]" ) {
    String str1("12345");
    String str2("abcde");
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    REQUIRE(str1 == "abcde");
    REQUIRE(str2 == "12345");
}

TEST_CASE("String::swap 2", "[string]" ) {
    static const char *small_str = "abcde";
    static const char *big_str = 
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef";

    String str1(big_str);
    String str2(small_str);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    REQUIRE(str1 == small_str);
    REQUIRE(str2 == big_str);
}

TEST_CASE("String::swap 3", "[string]" ) {
    static const char *small_str = "abcde";
    static const char *big_str = 
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef";

    String str1(small_str);
    String str2(big_str);
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    REQUIRE(str1 == big_str);
    REQUIRE(str2 == small_str);
}

TEST_CASE("String::swap 4", "[string]" ) {
    static const char *big_str1 = 
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef";

    static const char *big_str2 = 
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "1234567890abcdef"
        "xxx";

    String str1(big_str1);
    String str2(big_str2);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    REQUIRE(str1 == big_str2);
    REQUIRE(str2 == big_str1);
}

// erasing ======================================================================================

TEST_CASE("String::erase(SizeType index, SizeType count)", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    String ustr("0123456789abcdefghij");

    sstr.erase(0, 5);
    ustr.erase(0, 5);
    
    REQUIRE(sstr == "56789abcdefghij");
    REQUIRE(ustr == "56789abcdefghij");
    REQUIRE(sstr.length() == 15);
    REQUIRE(ustr.length() == 15);

    sstr.erase(5, 5);
    ustr.erase(5, 5);

    REQUIRE(sstr == "56789fghij");
    REQUIRE(ustr == "56789fghij");
    REQUIRE(sstr.length() == 10);
    REQUIRE(ustr.length() == 10);

    sstr.erase(5);
    ustr.erase(5);

    REQUIRE(sstr == "56789");
    REQUIRE(ustr == "56789");
    REQUIRE(sstr.length() == 5);
    REQUIRE(ustr.length() == 5);

    sstr.erase();
    ustr.erase();

    REQUIRE(sstr == "");
    REQUIRE(ustr == "");
    REQUIRE(sstr.length() == 0);
    REQUIRE(ustr.length() == 0);
}

TEST_CASE("String::erase(const_iterator pos)", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    String ustr("0123456789abcdefghij");

    auto sit = sstr.erase(sstr.begin() + 5);
    auto uit = ustr.erase(ustr.begin() + 5);
    
    REQUIRE(sstr == "012346789abcdefghij");
    REQUIRE(ustr == "012346789abcdefghij");
    REQUIRE(sstr.length() == 19);
    REQUIRE(ustr.length() == 19);

    sstr.erase(sit);
    ustr.erase(uit);
    
    REQUIRE(sstr == "01234789abcdefghij");
    REQUIRE(ustr == "01234789abcdefghij");
    REQUIRE(sstr.length() == 18);
    REQUIRE(ustr.length() == 18);
}

TEST_CASE("String::erase(const_iterator first, const_iterator last)", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    String ustr("0123456789abcdefghij");

    auto sit = sstr.erase(sstr.begin() + 5, sstr.begin() + 8);
    auto uit = ustr.erase(ustr.begin() + 5, ustr.begin() + 8);
    
    REQUIRE(sstr == "0123489abcdefghij");
    REQUIRE(ustr == "0123489abcdefghij");
    REQUIRE(sstr.length() == 17);
    REQUIRE(ustr.length() == 17);

    sstr.erase(sit, sstr.begin() + 8);
    ustr.erase(uit, ustr.begin() + 8);
    
    REQUIRE(sstr == "01234bcdefghij");
    REQUIRE(ustr == "01234bcdefghij");
    REQUIRE(sstr.length() == 14);
    REQUIRE(ustr.length() == 14);

    sit = sstr.erase(sit, sstr.end());
    uit = ustr.erase(uit, ustr.end());
    
    REQUIRE(sstr == "01234");
    REQUIRE(ustr == "01234");
    REQUIRE(sstr.length() == 5);
    REQUIRE(ustr.length() == 5);
}

TEST_CASE("String::erase(const_iterator first, const_iterator last) check", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    String ustr("0123456789abcdefghij");

    auto sit = sstr.erase(sstr.begin() + 8, sstr.begin() + 5);
    auto uit = ustr.erase(ustr.begin() + 8, ustr.begin() + 5);
    
    REQUIRE(sstr == "01234567");
    REQUIRE(ustr == "01234567");
    REQUIRE(sit == sstr.begin() + 8);
    REQUIRE(uit == ustr.begin() + 8);
    REQUIRE(sstr.length() == 8);
    REQUIRE(ustr.length() == 8);
}

// substrings =====================================================================================

TEST_CASE("String::substr(SizeType pos, SizeType count)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    String ustr1("0123456789abcdefghij");

    std::string sstr2 = sstr1.substr(0, 5);
    String ustr2 = ustr1.substr(0, 5);
    REQUIRE(sstr2 == "01234");
    REQUIRE(ustr2 == "01234");
    REQUIRE(sstr2.length() == 5);
    REQUIRE(ustr2.length() == 5);

    sstr2 = sstr1.substr();
    ustr2 = ustr1.substr();
    REQUIRE(sstr2 == "0123456789abcdefghij");
    REQUIRE(ustr2 == "0123456789abcdefghij");
    REQUIRE(sstr2.length() == 20);
    REQUIRE(ustr2.length() == 20);

    sstr2 = sstr1.substr(10);
    ustr2 = ustr1.substr(10);
    REQUIRE(sstr2 == "abcdefghij");
    REQUIRE(ustr2 == "abcdefghij");
    REQUIRE(sstr2.length() == 10);
    REQUIRE(ustr2.length() == 10);
}

TEST_CASE("String::substrview(SizeType pos, SizeType count)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    String ustr1("0123456789abcdefghij");

    auto sstr2 = sstr1.substr(0, 5);
    auto ustr2 = ustr1.substrview(0, 5);
    REQUIRE(sstr2 == "01234");
    REQUIRE(String(ustr2) == "01234");
    REQUIRE(sstr2.length() == 5);
    REQUIRE(ustr2.length() == 5);

    sstr2 = sstr1.substr();
    ustr2 = ustr1.substrview();
    REQUIRE(sstr2 == "0123456789abcdefghij");
    REQUIRE(String(ustr2) == "0123456789abcdefghij");
    REQUIRE(sstr2.length() == 20);
    REQUIRE(ustr2.length() == 20);

    sstr2 = sstr1.substr(10);
    ustr2 = ustr1.substrview(10);
    REQUIRE(sstr2 == "abcdefghij");
    REQUIRE(String(ustr2) == "abcdefghij");
    REQUIRE(sstr2.length() == 10);
    REQUIRE(ustr2.length() == 10);
}


// shrinking ======================================================================================

TEST_CASE("String::shrink() 1", "[string]" ) {
    String str1("abcdefghij");
    String str2;

    for (int idx = 0; idx < 33; idx++) {
        str2 += str1;
    }
    for (int idx = 0; idx < 32; idx++) {
        str2.erase(0, str1.length());
    }
    str2.shrink_to_fit();
    REQUIRE(str2.length() == str1.length());
    REQUIRE(str2.capacity() == String::InlineCapacity);
}

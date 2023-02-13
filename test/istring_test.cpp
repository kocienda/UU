//
// istring_test.cpp
//

#include "UU/IString.h"
#include <cstring>
#include <string>
#include <sstream>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

// constructing ===================================================================================

TEST_CASE("IString(Size count, CharT c)", "[string]" ) {
    std::string sstr(16, 'g');
    IString ustr(16, 'g');
    REQUIRE(sstr == ustr);
    REQUIRE(ustr == "gggggggggggggggg");
    REQUIRE(strlen(sstr.c_str()) == 16);
    REQUIRE(strlen(ustr.c_str()) == 16);
}

TEST_CASE("IString(const BasicString &other, Size pos, Size count)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");

    auto sstr2 = std::string(sstr1, 5, 5); 
    auto ustr2 = IString(ustr1, 5, 5); 

    REQUIRE(sstr2 == ustr2);
    REQUIRE(ustr2 == "56789");
    REQUIRE(strlen(sstr2.c_str()) == 5);
    REQUIRE(strlen(ustr2.c_str()) == 5);
}

TEST_CASE("IString(const CharT *ptr, Size length)", "[string]" ) {
    std::string sstr("0123456789", 5);
    IString ustr("0123456789", 5);

    REQUIRE(sstr == ustr);
    REQUIRE(ustr == "01234");
    REQUIRE(strlen(sstr.c_str()) == 5);
    REQUIRE(strlen(ustr.c_str()) == 5);
}

TEST_CASE("BasicString<char32_t>(const CharT *ptr, Size length)", "[string]" ) {
    std::basic_string<char32_t> sstr(U"0123456789", 5);
    BasicString<char32_t> ustr(U"0123456789", 5);

    REQUIRE(sstr == ustr);
    REQUIRE(ustr == U"01234");
    REQUIRE(sstr.length() == 5);
    REQUIRE(ustr.length() == 5);
}

TEST_CASE("BasicString<char32_t>(const char *ptr, Size length)", "[string]" ) {
    BasicString<char32_t> ustr("0123456789", 5);

    REQUIRE(ustr == "01234");
    REQUIRE(ustr.length() == 5);
}

TEST_CASE("IString(const CharT *ptr)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");

    REQUIRE(sstr == ustr);
    REQUIRE(ustr == "0123456789");
    REQUIRE(strlen(sstr.c_str()) == 10);
    REQUIRE(strlen(ustr.c_str()) == 10);
}

TEST_CASE("BasicString<char32_t>(const CharT *ptr)", "[string]" ) {
    std::basic_string<char32_t> sstr(U"0123456789");
    BasicString<char32_t> ustr(U"0123456789");

    REQUIRE(sstr == ustr);
    REQUIRE(ustr == U"0123456789");
    REQUIRE(sstr.length() == 10);
    REQUIRE(ustr.length() == 10);
}

TEST_CASE("BasicString<char32_t>(const char *ptr)", "[string]" ) {
    BasicString<char32_t> ustr("0123456789");

    REQUIRE(ustr == "0123456789");
    REQUIRE(ustr.length() == 10);
}

TEST_CASE("IString(InputIt first, InputIt last)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2 = std::string(sstr1.begin() + 3, sstr1.end());
    IString ustr1("0123456789");
    IString ustr2 = IString(ustr1.begin() + 3, ustr1.end());

    REQUIRE(ustr2 == sstr2);
    REQUIRE(sstr2 == "3456789");
    REQUIRE(ustr2 == "3456789");
    REQUIRE(strlen(sstr2.c_str()) == 7);
    REQUIRE(strlen(ustr2.c_str()) == 7);
}

TEST_CASE("IString(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr({ 'a', 'b', 'c' });
    IString ustr({ 'a', 'b', 'c' });

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("IString(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr(vstr_view);
    IString ustr(vstr_view);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("IString(const StringViewLikeT &t, Size pos, Size count)", "[string]" ) {
    std::string_view vstr("abcdefghij");
    std::string_view vstr_view(vstr);
    std::string sstr(vstr_view, 3, 4);
    IString ustr(vstr_view, 3, 4);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "defg");
    REQUIRE(ustr == "defg");
    REQUIRE(strlen(sstr.c_str()) == 4);
    REQUIRE(strlen(ustr.c_str()) == 4);
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

TEST_CASE("IString::starts_with(const StringViewLikeT &t)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
    IString str2("0123456789abcdefghij");
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

TEST_CASE("IString::starts_with(CharT c)", "[string]" ) {
    IString str1("0123456789");
    
    REQUIRE(str1.starts_with('0'));
    REQUIRE_FALSE(str1.starts_with('1'));
}

TEST_CASE("IString::starts_with(const CharT *s)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
    REQUIRE(str1.starts_with("0123456789abcdefghij"));
    REQUIRE(str1.starts_with("0123456789"));
    REQUIRE(str1.starts_with("0"));
    REQUIRE(str1.starts_with(""));
    REQUIRE_FALSE(str1.starts_with("0123456789abcdefghijk"));
    REQUIRE_FALSE(str1.starts_with("abcdef"));
}

// ends_with ======================================================================================

TEST_CASE("IString::ends_with(const StringViewLikeT &t)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
    IString str2("0123456789abcdefghij");
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

TEST_CASE("IString::ends_with(CharT c)", "[string]" ) {
    IString str1("0123456789");
    
    REQUIRE(str1.ends_with('9'));
    REQUIRE_FALSE(str1.ends_with('1'));
}

TEST_CASE("IString::ends_with(const CharT *s)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
    REQUIRE(str1.ends_with("0123456789abcdefghij"));
    REQUIRE(str1.ends_with("abcdefghij"));
    REQUIRE(str1.ends_with("j"));
    REQUIRE(str1.ends_with(""));
    REQUIRE_FALSE(str1.ends_with("0123456789abcdefghijk"));
    REQUIRE_FALSE(str1.ends_with("012345"));
}

// contains =======================================================================================

TEST_CASE("IString::contains(const StringViewLikeT &t)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
    IString str2("0123456789abcdefghij");
    REQUIRE(str1.contains(str2));

    str2 = "abcdefghij";
    REQUIRE(str1.contains(str2));

    str2 = "6789abc";
    REQUIRE(str1.contains(str2));

    str2 = "j";
    REQUIRE(str1.contains(str2));

    str2 = "01";
    REQUIRE(str1.contains(str2));

    str2 = "12";
    REQUIRE(str1.contains(str2));

    str2 = "hi";
    REQUIRE(str1.contains(str2));

    str2 = "ij";
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

TEST_CASE("IString::contains(CharT c)", "[string]" ) {
    IString str1("0123456789");
    
    REQUIRE(str1.contains('9'));
    REQUIRE(str1.contains('0'));
    REQUIRE(str1.contains('1'));
    REQUIRE(str1.contains('5'));
    REQUIRE_FALSE(str1.contains('a'));
}

TEST_CASE("IString::contains(const CharT *s)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
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

// find ===========================================================================================

TEST_CASE("IString::find(const IString &s, Size pos)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find(std::string("0123456789abcdefghij")) == 0);
    REQUIRE(ustr1.find(IString("0123456789abcdefghij")) == 0);
    REQUIRE(sstr1.find(std::string("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(ustr1.find(IString("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(sstr1.find(std::string("abcdefghij")) == 10);
    REQUIRE(ustr1.find(IString("abcdefghij")) == 10);
    REQUIRE(sstr1.find(std::string("abcdefghij"), 3) == 10);
    REQUIRE(ustr1.find(IString("abcdefghij"), 3) == 10);
    REQUIRE(sstr1.find(std::string("789abcdef")) == 7);
    REQUIRE(ustr1.find(IString("789abcdef")) == 7);
    REQUIRE(sstr1.find(std::string("789abcdef"), 2) == 7);
    REQUIRE(ustr1.find(IString("789abcdef"), 2) == 7);
    REQUIRE(sstr1.find(std::string("01")) == 0);
    REQUIRE(ustr1.find(IString("01")) == 0);
    REQUIRE(sstr1.find(std::string("01"), 1) == IString::npos);
    REQUIRE(ustr1.find(IString("01"), 1) == IString::npos);
    REQUIRE(sstr1.find(std::string("12")) == 1);
    REQUIRE(ustr1.find(IString("12")) == 1);
    REQUIRE(sstr1.find(std::string("12"), 1) == 1);
    REQUIRE(ustr1.find(IString("12"), 1) == 1);
    REQUIRE(sstr1.find(std::string("12"), 2) == IString::npos);
    REQUIRE(ustr1.find(IString("12"), 2) == IString::npos);
    REQUIRE(sstr1.find(std::string("567")) == 5);
    REQUIRE(ustr1.find(IString("567")) == 5);
    REQUIRE(sstr1.find(std::string("567"), 2) == 5);
    REQUIRE(ustr1.find(IString("567"), 2) == 5);
    REQUIRE(sstr1.find(std::string("567"), 7) == IString::npos);
    REQUIRE(ustr1.find(IString("567"), 7) == IString::npos);
    REQUIRE(sstr1.find(std::string("0")) == 0);
    REQUIRE(ustr1.find(IString("0")) == 0);
    REQUIRE(sstr1.find(std::string("0"), 1) == IString::npos);
    REQUIRE(ustr1.find(IString("0"), 1) == IString::npos);
    REQUIRE(sstr1.find(std::string("ij")) == 18);
    REQUIRE(ustr1.find(IString("ij")) == 18);
    REQUIRE(sstr1.find(std::string("ij"), 10) == 18);
    REQUIRE(ustr1.find(IString("ij"), 10) == 18);
    REQUIRE(sstr1.find(std::string("ij"), 19) == IString::npos);
    REQUIRE(ustr1.find(IString("ij"), 19) == IString::npos);
    REQUIRE(sstr1.find(std::string("j")) == 19);
    REQUIRE(ustr1.find(IString("j")) == 19);
    REQUIRE(sstr1.find(std::string("j"), 5) == 19);
    REQUIRE(ustr1.find(IString("j"), 5) == 19);
    REQUIRE(sstr1.find(std::string("j"), 19) == 19);
    REQUIRE(ustr1.find(IString("j"), 19) == 19);
    REQUIRE(sstr1.find(std::string("j"), 20) == IString::npos);
    REQUIRE(ustr1.find(IString("j"), 20) == IString::npos);
    REQUIRE(sstr1.find(std::string("")) == 0);
    REQUIRE(ustr1.find(IString("")) == 0);
    REQUIRE(sstr1.find(std::string("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(ustr1.find(IString("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(sstr1.find(std::string("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(ustr1.find(IString("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(sstr1.find(std::string("012345a")) == IString::npos);
    REQUIRE(ustr1.find(IString("012345a")) == IString::npos);
    REQUIRE(sstr1.find(std::string("012345a"), 3) == IString::npos);
    REQUIRE(ustr1.find(IString("012345a"), 3) == IString::npos);
    REQUIRE(sstr1.find(std::string("k")) == IString::npos);
    REQUIRE(ustr1.find(IString("k")) == IString::npos);
    REQUIRE(sstr1.find(std::string("k"), 2) == IString::npos);
    REQUIRE(ustr1.find(IString("k"), 2) == IString::npos);
}

TEST_CASE("IString::find(const StringViewLikeT &t)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
    IString str2("0123456789abcdefghij");
    REQUIRE(str1.find(str2) == 0);

    str2 = "abcdefghij";
    REQUIRE(str1.find(str2) == 10);

    str2 = "6789abc";
    REQUIRE(str1.find(str2) == 6);

    str2 = "j";
    REQUIRE(str1.find(str2) == 19);

    str2 = "01";
    REQUIRE(str1.find(str2) == 0);

    str2 = "12";
    REQUIRE(str1.find(str2) == 1);

    str2 = "hi";
    REQUIRE(str1.find(str2) == 17);

    str2 = "ij";
    REQUIRE(str1.find(str2) == 18);

    str2 = "";
    REQUIRE(str1.find(str2) == 0);

    str2 = "6789abce";
    REQUIRE(str1.find(str2) == IString::npos);

    str2 = "0123456789abcdefghijk";
    REQUIRE(str1.find(str2) == IString::npos);

    str2 = "0123459";
    REQUIRE(str1.find(str2) == IString::npos);
}

TEST_CASE("IString::find(const CharT *s)", "[string]" ) {
    IString str1("0123456789abcdefghij");
    
    REQUIRE(str1.find("0123456789abcdefghij") == 0);
    REQUIRE(str1.find("abcdefghij") == 10);
    REQUIRE(str1.find("789abcdef") == 7);
    REQUIRE(str1.find("01") == 0);
    REQUIRE(str1.find("12") == 1);
    REQUIRE(str1.find("567") == 5);
    REQUIRE(str1.find("0") == 0);
    REQUIRE(str1.find("ij") == 18);
    REQUIRE(str1.find("j") == 19);
    REQUIRE(str1.find("") == 0);
    REQUIRE(str1.find("0123456789abcdefghijk") == IString::npos);
    REQUIRE(str1.find("012345a") == IString::npos);
    REQUIRE(str1.find("k") == IString::npos);
    REQUIRE(str1.find("k") == IString::npos);
}

TEST_CASE("IString::find(const CharT *s, Size pos)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find("0123456789abcdefghij") == 0);
    REQUIRE(ustr1.find("0123456789abcdefghij") == 0);
    REQUIRE(sstr1.find("0123456789abcdefghij", 5) == IString::npos);
    REQUIRE(ustr1.find("0123456789abcdefghij", 5) == IString::npos);
    REQUIRE(sstr1.find("abcdefghij") == 10);
    REQUIRE(ustr1.find("abcdefghij") == 10);
    REQUIRE(sstr1.find("abcdefghij", 3) == 10);
    REQUIRE(ustr1.find("abcdefghij", 3) == 10);
    REQUIRE(sstr1.find("789abcdef") == 7);
    REQUIRE(ustr1.find("789abcdef") == 7);
    REQUIRE(sstr1.find("789abcdef", 2) == 7);
    REQUIRE(ustr1.find("789abcdef", 2) == 7);
    REQUIRE(sstr1.find("01") == 0);
    REQUIRE(ustr1.find("01") == 0);
    REQUIRE(sstr1.find("01", 1) == IString::npos);
    REQUIRE(ustr1.find("01", 1) == IString::npos);
    REQUIRE(sstr1.find("12") == 1);
    REQUIRE(ustr1.find("12") == 1);
    REQUIRE(sstr1.find("12", 1) == 1);
    REQUIRE(ustr1.find("12", 1) == 1);
    REQUIRE(sstr1.find("12", 2) == IString::npos);
    REQUIRE(ustr1.find("12", 2) == IString::npos);
    REQUIRE(sstr1.find("567") == 5);
    REQUIRE(ustr1.find("567") == 5);
    REQUIRE(sstr1.find("567", 2) == 5);
    REQUIRE(ustr1.find("567", 2) == 5);
    REQUIRE(sstr1.find("567", 7) == IString::npos);
    REQUIRE(ustr1.find("567", 7) == IString::npos);
    REQUIRE(sstr1.find("0") == 0);
    REQUIRE(ustr1.find("0") == 0);
    REQUIRE(sstr1.find("0", 1) == IString::npos);
    REQUIRE(ustr1.find("0", 1) == IString::npos);
    REQUIRE(sstr1.find("ij") == 18);
    REQUIRE(ustr1.find("ij") == 18);
    REQUIRE(sstr1.find("ij", 10) == 18);
    REQUIRE(ustr1.find("ij", 10) == 18);
    REQUIRE(sstr1.find("ij", 19) == IString::npos);
    REQUIRE(ustr1.find("ij", 19) == IString::npos);
    REQUIRE(sstr1.find("j") == 19);
    REQUIRE(ustr1.find("j") == 19);
    REQUIRE(sstr1.find("j", 5) == 19);
    REQUIRE(ustr1.find("j", 5) == 19);
    REQUIRE(sstr1.find("j", 19) == 19);
    REQUIRE(ustr1.find("j", 19) == 19);
    REQUIRE(sstr1.find("j", 20) == IString::npos);
    REQUIRE(ustr1.find("j", 20) == IString::npos);
    REQUIRE(sstr1.find("") == 0);
    REQUIRE(ustr1.find("") == 0);
    REQUIRE(sstr1.find("0123456789abcdefghijk") == IString::npos);
    REQUIRE(ustr1.find("0123456789abcdefghijk") == IString::npos);
    REQUIRE(sstr1.find("0123456789abcdefghijk", 5) == IString::npos);
    REQUIRE(ustr1.find("0123456789abcdefghijk", 5) == IString::npos);
    REQUIRE(sstr1.find("012345a") == IString::npos);
    REQUIRE(ustr1.find("012345a") == IString::npos);
    REQUIRE(sstr1.find("012345a", 3) == IString::npos);
    REQUIRE(ustr1.find("012345a", 3) == IString::npos);
    REQUIRE(sstr1.find("k") == IString::npos);
    REQUIRE(ustr1.find("k") == IString::npos);
    REQUIRE(sstr1.find("k", 2) == IString::npos);
    REQUIRE(ustr1.find("k", 2) == IString::npos);
}

TEST_CASE("IString::find(const CharT *s, Size pos, Size count)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find("0123456789abcdefghij", 0, 5) == 0);
    REQUIRE(ustr1.find("0123456789abcdefghij", 0, 5) == 0);
    REQUIRE(sstr1.find("0123456789abcdefghij", 5, 5) == IString::npos);
    REQUIRE(ustr1.find("0123456789abcdefghij", 5, 5) == IString::npos);
    REQUIRE(sstr1.find("abcdefghij", 0, 5) == 10);
    REQUIRE(ustr1.find("abcdefghij", 0, 5) == 10);
    REQUIRE(sstr1.find("abcdefghij", 3, 5) == 10);
    REQUIRE(ustr1.find("abcdefghij", 3, 5) == 10);
    REQUIRE(sstr1.find("789abcdef", 1, 4) == 7);
    REQUIRE(ustr1.find("789abcdef", 1, 4) == 7);
    REQUIRE(sstr1.find("789abcdef", 2, 5) == 7);
    REQUIRE(ustr1.find("789abcdef", 2, 5) == 7);
    REQUIRE(sstr1.find("01", 0, 1) == 0);
    REQUIRE(ustr1.find("01", 0, 1) == 0);
    REQUIRE(sstr1.find("01", 1, 1) == IString::npos);
    REQUIRE(ustr1.find("01", 1, 1) == IString::npos);
    REQUIRE(sstr1.find("12", 0, 1) == 1);
    REQUIRE(ustr1.find("12", 0, 1) == 1);
    REQUIRE(sstr1.find("12", 1, 2) == 1);
    REQUIRE(ustr1.find("12", 1, 2) == 1);
    REQUIRE(sstr1.find("12", 2, 2) == IString::npos);
    REQUIRE(ustr1.find("12", 2, 2) == IString::npos);
    REQUIRE(sstr1.find("567", 0, 2) == 5);
    REQUIRE(ustr1.find("567", 0, 2) == 5);
    REQUIRE(sstr1.find("567", 2, 3) == 5);
    REQUIRE(ustr1.find("567", 2, 3) == 5);
    REQUIRE(sstr1.find("567", 7, 3) == IString::npos);
    REQUIRE(ustr1.find("567", 7, 3) == IString::npos);
    REQUIRE(sstr1.find("0", 0, 1) == 0);
    REQUIRE(ustr1.find("0", 0, 1) == 0);
    REQUIRE(sstr1.find("0", 1, 1) == IString::npos);
    REQUIRE(ustr1.find("0", 1, 1) == IString::npos);
    REQUIRE(sstr1.find("ij", 0, 2) == 18);
    REQUIRE(ustr1.find("ij", 0, 2) == 18);
    REQUIRE(sstr1.find("ij", 10, 1) == 18);
    REQUIRE(ustr1.find("ij", 10, 1) == 18);
    REQUIRE(sstr1.find("ij", 19, 1) == IString::npos);
    REQUIRE(ustr1.find("ij", 19, 1) == IString::npos);
    REQUIRE(sstr1.find("j", 0, 1) == 19);
    REQUIRE(ustr1.find("j", 0, 1) == 19);
    REQUIRE(sstr1.find("j", 5, 1) == 19);
    REQUIRE(ustr1.find("j", 5, 1) == 19);
    REQUIRE(sstr1.find("j", 19, 1) == 19);
    REQUIRE(ustr1.find("j", 19, 1) == 19);
    REQUIRE(sstr1.find("j", 20, 1) == IString::npos);
    REQUIRE(ustr1.find("j", 20, 1) == IString::npos);
    REQUIRE(sstr1.find("", 0, 0) == 0);
    REQUIRE(ustr1.find("", 0, 0) == 0);
    REQUIRE(sstr1.find("0123456789abcdefghijk", 0, 10) == 0);
    REQUIRE(ustr1.find("0123456789abcdefghijk", 0, 10) == 0);
    REQUIRE(sstr1.find("0123456789abcdefghijk", 0, 21) == IString::npos);
    REQUIRE(ustr1.find("0123456789abcdefghijk", 0, 21) == IString::npos);
    REQUIRE(sstr1.find("0123456789abcdefghijk", 5, 10) == IString::npos);
    REQUIRE(ustr1.find("0123456789abcdefghijk", 5, 10) == IString::npos);
    REQUIRE(sstr1.find("012345a", 0, 5) == 0);
    REQUIRE(ustr1.find("012345a", 0, 5) == 0);
    REQUIRE(sstr1.find("012345a", 0, 7) == IString::npos);
    REQUIRE(ustr1.find("012345a", 0, 7) == IString::npos);
    REQUIRE(sstr1.find("012345a", 3, 4) == IString::npos);
    REQUIRE(ustr1.find("012345a", 3, 4) == IString::npos);
    REQUIRE(sstr1.find("012345a", 3, 7) == IString::npos);
    REQUIRE(ustr1.find("012345a", 3, 7) == IString::npos);
    REQUIRE(sstr1.find("k", 0, 1) == IString::npos);
    REQUIRE(ustr1.find("k", 0, 1) == IString::npos);
    REQUIRE(sstr1.find("k", 2, 1) == IString::npos);
    REQUIRE(ustr1.find("k", 2, 1) == IString::npos);
}

TEST_CASE("IString::find(CharT c, Size pos)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");
    
    REQUIRE(sstr1.find('9') == 9);
    REQUIRE(ustr1.find('9') == 9);
    REQUIRE(sstr1.find('9', 3) == 9);
    REQUIRE(ustr1.find('9', 3) == 9);
    REQUIRE(sstr1.find('9', 10) == IString::npos);
    REQUIRE(ustr1.find('9', 10) == IString::npos);
    REQUIRE(sstr1.find('0') == 0);
    REQUIRE(ustr1.find('0') == 0);
    REQUIRE(sstr1.find('0', 1) == IString::npos);
    REQUIRE(ustr1.find('0', 1) == IString::npos);
    REQUIRE(sstr1.find('1') == 1);
    REQUIRE(ustr1.find('1') == 1);
    REQUIRE(sstr1.find('1', 5) == IString::npos);
    REQUIRE(ustr1.find('1', 5) == IString::npos);
    REQUIRE(sstr1.find('5', 2) == 5);
    REQUIRE(ustr1.find('5', 2) == 5);
    REQUIRE(sstr1.find('5', 7) == IString::npos);
    REQUIRE(ustr1.find('5', 7) == IString::npos);
    REQUIRE(sstr1.find('5') == 5);
    REQUIRE(ustr1.find('5') == 5);
    REQUIRE(sstr1.find('a') == IString::npos);
    REQUIRE(ustr1.find('a') == IString::npos);
    REQUIRE(sstr1.find('a', 5) == IString::npos);
    REQUIRE(ustr1.find('a', 5) == IString::npos);
}

TEST_CASE("IString::find_boyer_moore(const IString &s, Size pos)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find(std::string("0123456789abcdefghij")) == 0);
    REQUIRE(ustr1.find_boyer_moore(IString("0123456789abcdefghij")) == 0);
    REQUIRE(sstr1.find(std::string("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(sstr1.find(std::string("abcdefghij")) == 10);
    REQUIRE(ustr1.find_boyer_moore(IString("abcdefghij")) == 10);
    REQUIRE(sstr1.find(std::string("abcdefghij"), 3) == 10);
    REQUIRE(ustr1.find_boyer_moore(IString("abcdefghij"), 3) == 10);
    REQUIRE(sstr1.find(std::string("789abcdef")) == 7);
    REQUIRE(ustr1.find_boyer_moore(IString("789abcdef")) == 7);
    REQUIRE(sstr1.find(std::string("789abcdef"), 2) == 7);
    REQUIRE(ustr1.find_boyer_moore(IString("789abcdef"), 2) == 7);
    REQUIRE(sstr1.find(std::string("01")) == 0);
    REQUIRE(ustr1.find_boyer_moore(IString("01")) == 0);
    REQUIRE(sstr1.find(std::string("01"), 1) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("01"), 1) == IString::npos);
    REQUIRE(sstr1.find(std::string("12")) == 1);
    REQUIRE(ustr1.find_boyer_moore(IString("12")) == 1);
    REQUIRE(sstr1.find(std::string("12"), 1) == 1);
    REQUIRE(ustr1.find_boyer_moore(IString("12"), 1) == 1);
    REQUIRE(sstr1.find(std::string("12"), 2) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("12"), 2) == IString::npos);
    REQUIRE(sstr1.find(std::string("567")) == 5);
    REQUIRE(ustr1.find_boyer_moore(IString("567")) == 5);
    REQUIRE(sstr1.find(std::string("567"), 2) == 5);
    REQUIRE(ustr1.find_boyer_moore(IString("567"), 2) == 5);
    REQUIRE(sstr1.find(std::string("567"), 7) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("567"), 7) == IString::npos);
    REQUIRE(sstr1.find(std::string("0")) == 0);
    REQUIRE(ustr1.find_boyer_moore(IString("0")) == 0);
    REQUIRE(sstr1.find(std::string("0"), 1) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("0"), 1) == IString::npos);
    REQUIRE(sstr1.find(std::string("ij")) == 18);
    REQUIRE(ustr1.find_boyer_moore(IString("ij")) == 18);
    REQUIRE(sstr1.find(std::string("ij"), 10) == 18);
    REQUIRE(ustr1.find_boyer_moore(IString("ij"), 10) == 18);
    REQUIRE(sstr1.find(std::string("ij"), 19) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("ij"), 19) == IString::npos);
    REQUIRE(sstr1.find(std::string("j")) == 19);
    REQUIRE(ustr1.find_boyer_moore(IString("j")) == 19);
    REQUIRE(sstr1.find(std::string("j"), 5) == 19);
    REQUIRE(ustr1.find_boyer_moore(IString("j"), 5) == 19);
    REQUIRE(sstr1.find(std::string("j"), 19) == 19);
    REQUIRE(ustr1.find_boyer_moore(IString("j"), 19) == 19);
    REQUIRE(sstr1.find(std::string("j"), 20) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("j"), 20) == IString::npos);
    REQUIRE(sstr1.find(std::string("")) == 0);
    REQUIRE(ustr1.find_boyer_moore(IString("")) == 0);
    REQUIRE(sstr1.find(std::string("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(sstr1.find(std::string("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(sstr1.find(std::string("012345a")) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("012345a")) == IString::npos);
    REQUIRE(sstr1.find(std::string("012345a"), 3) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("012345a"), 3) == IString::npos);
    REQUIRE(sstr1.find(std::string("k")) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("k")) == IString::npos);
    REQUIRE(sstr1.find(std::string("k"), 2) == IString::npos);
    REQUIRE(ustr1.find_boyer_moore(IString("k"), 2) == IString::npos);
}

// rfind ==========================================================================================

TEST_CASE("IString::rfind(CharT c, Size pos)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");
    
    REQUIRE(sstr1.rfind('9') == 9);
    REQUIRE(ustr1.rfind('9') == 9);
    REQUIRE(sstr1.rfind('9', 3) == IString::npos);
    REQUIRE(ustr1.rfind('9', 3) == IString::npos);
    REQUIRE(sstr1.rfind('9', 10) == 9);
    REQUIRE(ustr1.rfind('9', 10) == 9);
    REQUIRE(sstr1.rfind('0') == 0);
    REQUIRE(ustr1.rfind('0') == 0);
    REQUIRE(sstr1.rfind('0', 1) == 0);
    REQUIRE(ustr1.rfind('0', 1) == 0);
    REQUIRE(sstr1.rfind('1') == 1);
    REQUIRE(ustr1.rfind('1') == 1);
    REQUIRE(sstr1.rfind('1', 5) == 1);
    REQUIRE(ustr1.rfind('1', 5) == 1);
    REQUIRE(sstr1.rfind('1', 0) == IString::npos);
    REQUIRE(ustr1.rfind('1', 0) == IString::npos);
    REQUIRE(sstr1.rfind('5', 2) == IString::npos);
    REQUIRE(ustr1.rfind('5', 2) == IString::npos);
    REQUIRE(sstr1.rfind('5', 7) == 5);
    REQUIRE(ustr1.rfind('5', 7) == 5);
    REQUIRE(sstr1.rfind('5') == 5);
    REQUIRE(ustr1.rfind('5') == 5);
    REQUIRE(sstr1.rfind('a') == IString::npos);
    REQUIRE(ustr1.rfind('a') == IString::npos);
    REQUIRE(sstr1.rfind('a', 5) == IString::npos);
    REQUIRE(ustr1.rfind('a', 5) == IString::npos);
}

TEST_CASE("IString::rfind(const CharT *s, Size pos, Size count)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.rfind("0123456789abcdefghij", 0, 5) == 0);
    REQUIRE(ustr1.rfind("0123456789abcdefghij", 0, 5) == 0);
    REQUIRE(sstr1.rfind("0123456789abcdefghij", 5, 5) == 0);
    REQUIRE(ustr1.rfind("0123456789abcdefghij", 5, 5) == 0);
    REQUIRE(sstr1.rfind("0123456789abcdefghij") == 0);
    REQUIRE(ustr1.rfind("0123456789abcdefghij") == 0);
    REQUIRE(sstr1.rfind("abcdefghij", 0, 5) == IString::npos);
    REQUIRE(ustr1.rfind("abcdefghij", 0, 5) == IString::npos);
    REQUIRE(sstr1.rfind("abcdefghij", 10, 5) == 10);
    REQUIRE(ustr1.rfind("abcdefghij", 10, 5) == 10);
    REQUIRE(sstr1.rfind("abcdefghij", IString::npos, 5) == 10);
    REQUIRE(ustr1.rfind("abcdefghij", IString::npos, 5) == 10);
    REQUIRE(sstr1.rfind("abcdefghij", 3, 5) == IString::npos);
    REQUIRE(ustr1.rfind("abcdefghij", 3, 5) == IString::npos);
    REQUIRE(sstr1.rfind("789abcdef", 1, 4) == IString::npos);
    REQUIRE(ustr1.rfind("789abcdef", 1, 4) == IString::npos);
    REQUIRE(sstr1.rfind("789abcdef", 10, 5) == 7);
    REQUIRE(ustr1.rfind("789abcdef", 10, 5) == 7);
    REQUIRE(sstr1.rfind("01", 0, 1) == 0);
    REQUIRE(ustr1.rfind("01", 0, 1) == 0);
    REQUIRE(sstr1.rfind("01", 1, 1) == 0);
    REQUIRE(ustr1.rfind("01", 1, 1) == 0);
    REQUIRE(sstr1.rfind("12", 0, 1) == IString::npos);
    REQUIRE(ustr1.rfind("12", 0, 1) == IString::npos);
    REQUIRE(sstr1.rfind("12", 1, 2) == 1);
    REQUIRE(ustr1.rfind("12", 1, 2) == 1);
    REQUIRE(sstr1.rfind("12", 2, 2) == 1);
    REQUIRE(ustr1.rfind("12", 2, 2) == 1);
    REQUIRE(sstr1.rfind("12", 0, 2) == IString::npos);
    REQUIRE(ustr1.rfind("12", 0, 2) == IString::npos);
    REQUIRE(sstr1.rfind("567", 0, 2) == IString::npos);
    REQUIRE(ustr1.rfind("567", 0, 2) == IString::npos);
    REQUIRE(sstr1.rfind("567", 2, 3) == IString::npos);
    REQUIRE(ustr1.rfind("567", 2, 3) == IString::npos);
    REQUIRE(sstr1.rfind("567", 7, 3) == 5);
    REQUIRE(ustr1.rfind("567", 7, 3) == 5);
    REQUIRE(sstr1.rfind("0", 0, 1) == 0);
    REQUIRE(ustr1.rfind("0", 0, 1) == 0);
    REQUIRE(sstr1.rfind("0", 1, 1) == 0);
    REQUIRE(ustr1.rfind("0", 1, 1) == 0);
    REQUIRE(sstr1.rfind("ij", 0, 2) == IString::npos);
    REQUIRE(ustr1.rfind("ij", 0, 2) == IString::npos);
    REQUIRE(sstr1.rfind("ij", 10, 1) == IString::npos);
    REQUIRE(ustr1.rfind("ij", 10, 1) == IString::npos);
    REQUIRE(sstr1.rfind("ij", 19, 1) == 18);
    REQUIRE(ustr1.rfind("ij", 19, 1) == 18);
    REQUIRE(sstr1.rfind("j", 0, 1) == IString::npos);
    REQUIRE(ustr1.rfind("j", 0, 1) == IString::npos);
    REQUIRE(sstr1.rfind("j", 5, 1) == IString::npos);
    REQUIRE(ustr1.rfind("j", 5, 1) == IString::npos);
    REQUIRE(sstr1.rfind("j", 19, 1) == 19);
    REQUIRE(ustr1.rfind("j", 19, 1) == 19);
    REQUIRE(sstr1.rfind("j", 20, 1) == 19);
    REQUIRE(ustr1.rfind("j", 20, 1) == 19);
    REQUIRE(sstr1.rfind("j", IString::npos, 1) == 19);
    REQUIRE(ustr1.rfind("j", IString::npos, 1) == 19);
    REQUIRE(sstr1.rfind("", 0, 0) == 0);
    REQUIRE(ustr1.rfind("", 0, 0) == 0);
    REQUIRE(sstr1.rfind("0123456789abcdefghijk", 0, 10) == 0);
    REQUIRE(ustr1.rfind("0123456789abcdefghijk", 0, 10) == 0);
    REQUIRE(sstr1.rfind("0123456789abcdefghijk", 0, 21) == IString::npos);
    REQUIRE(ustr1.rfind("0123456789abcdefghijk", 0, 21) == IString::npos);
    REQUIRE(sstr1.rfind("0123456789abcdefghijk", 5, 10) == 0);
    REQUIRE(ustr1.rfind("0123456789abcdefghijk", 5, 10) == 0);
    REQUIRE(sstr1.rfind("012345a", 0, 5) == 0);
    REQUIRE(ustr1.rfind("012345a", 0, 5) == 0);
    REQUIRE(sstr1.rfind("012345a", 0, 7) == IString::npos);
    REQUIRE(ustr1.rfind("012345a", 0, 7) == IString::npos);
    REQUIRE(sstr1.rfind("012345a", 3, 4) == 0);
    REQUIRE(ustr1.rfind("012345a", 3, 4) == 0);
    REQUIRE(sstr1.rfind("012345a", 3, 7) == IString::npos);
    REQUIRE(ustr1.rfind("012345a", 3, 7) == IString::npos);
    REQUIRE(sstr1.rfind("k", 0, 1) == IString::npos);
    REQUIRE(ustr1.rfind("k", 0, 1) == IString::npos);
    REQUIRE(sstr1.rfind("k", 2, 1) == IString::npos);
    REQUIRE(ustr1.rfind("k", 2, 1) == IString::npos);
    REQUIRE(sstr1.rfind("", IString::npos) == sstr1.length());
    REQUIRE(ustr1.rfind("", IString::npos) == ustr1.length());

    std::string sstr2;
    IString ustr2;
    REQUIRE(sstr2.rfind("", IString::npos) == 0);
    REQUIRE(ustr2.rfind("", IString::npos) == 0);
}

// find_first_of ==================================================================================

TEST_CASE("IString::find_first_of(CharT c, Size pos)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");
    
    REQUIRE(sstr1.find_first_of("9") == 9);
    REQUIRE(ustr1.find_first_of("9") == 9);
    REQUIRE(sstr1.find_first_of("9", 3) == 9);
    REQUIRE(ustr1.find_first_of("9", 3) == 9);
    REQUIRE(sstr1.find_first_of("9", 10) == IString::npos);
    REQUIRE(ustr1.find_first_of("9", 10) == IString::npos);
    REQUIRE(sstr1.find_first_of("0") == 0);
    REQUIRE(ustr1.find_first_of("0") == 0);
    REQUIRE(sstr1.find_first_of("0", 1) == IString::npos);
    REQUIRE(ustr1.find_first_of("0", 1) == IString::npos);
    REQUIRE(sstr1.find_first_of("1") == 1);
    REQUIRE(ustr1.find_first_of("1") == 1);
    REQUIRE(sstr1.find_first_of("1", 5) == IString::npos);
    REQUIRE(ustr1.find_first_of("1", 5) == IString::npos);
    REQUIRE(sstr1.find_first_of("5", 2) == 5);
    REQUIRE(ustr1.find_first_of("5", 2) == 5);
    REQUIRE(sstr1.find_first_of("5", 7) == IString::npos);
    REQUIRE(ustr1.find_first_of("5", 7) == IString::npos);
    REQUIRE(sstr1.find_first_of("5") == 5);
    REQUIRE(ustr1.find_first_of("5") == 5);
    REQUIRE(sstr1.find_first_of("a") == IString::npos);
    REQUIRE(ustr1.find_first_of("a") == IString::npos);
    REQUIRE(sstr1.find_first_of("a", 5) == IString::npos);
    REQUIRE(ustr1.find_first_of("a", 5) == IString::npos);
}

TEST_CASE("IString::find_first_of(const IString &s, Size pos)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find_first_of(std::string("0123456789abcdefghij")) == 0);
    REQUIRE(ustr1.find_first_of(IString("0123456789abcdefghij")) == 0);
    REQUIRE(sstr1.find_first_of(std::string("0123456789abcdefghij"), 5) == 5);
    REQUIRE(ustr1.find_first_of(IString("0123456789abcdefghij"), 5) == 5);
    REQUIRE(sstr1.find_first_of(std::string("abcdefghij")) == 10);
    REQUIRE(ustr1.find_first_of(IString("abcdefghij")) == 10);
    REQUIRE(sstr1.find_first_of(std::string("abcdefghij"), 3) == 10);
    REQUIRE(ustr1.find_first_of(IString("abcdefghij"), 3) == 10);
    REQUIRE(sstr1.find_first_of(std::string("789abcdef")) == 7);
    REQUIRE(ustr1.find_first_of(IString("789abcdef")) == 7);
    REQUIRE(sstr1.find_first_of(std::string("789abcdef"), 2) == 7);
    REQUIRE(ustr1.find_first_of(IString("789abcdef"), 2) == 7);
    REQUIRE(sstr1.find_first_of(std::string("abcdef789abcdef"), 2) == 7);
    REQUIRE(ustr1.find_first_of(IString("abcdef789abcdef"), 2) == 7);
    REQUIRE(sstr1.find_first_of(std::string("01")) == 0);
    REQUIRE(ustr1.find_first_of(IString("01")) == 0);
    REQUIRE(sstr1.find_first_of(std::string("01"), 1) == 1);
    REQUIRE(ustr1.find_first_of(IString("01"), 1) == 1);
    REQUIRE(sstr1.find_first_of(std::string("12")) == 1);
    REQUIRE(ustr1.find_first_of(IString("12")) == 1);
    REQUIRE(sstr1.find_first_of(std::string("12"), 1) == 1);
    REQUIRE(ustr1.find_first_of(IString("12"), 1) == 1);
    REQUIRE(sstr1.find_first_of(std::string("12"), 2) == 2);
    REQUIRE(ustr1.find_first_of(IString("12"), 2) == 2);
    REQUIRE(sstr1.find_first_of(std::string("12"), 3) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("12"), 3) == IString::npos);
    REQUIRE(sstr1.find_first_of(std::string("567")) == 5);
    REQUIRE(ustr1.find_first_of(IString("567")) == 5);
    REQUIRE(sstr1.find_first_of(std::string("567"), 2) == 5);
    REQUIRE(ustr1.find_first_of(IString("567"), 2) == 5);
    REQUIRE(sstr1.find_first_of(std::string("567"), 7) == 7);
    REQUIRE(ustr1.find_first_of(IString("567"), 7) == 7);
    REQUIRE(sstr1.find_first_of(std::string("567"), 9) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("567"), 9) == IString::npos);
    REQUIRE(sstr1.find_first_of(std::string("0")) == 0);
    REQUIRE(ustr1.find_first_of(IString("0")) == 0);
    REQUIRE(sstr1.find_first_of(std::string("0"), 1) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("0"), 1) == IString::npos);
    REQUIRE(sstr1.find_first_of(std::string("ij")) == 18);
    REQUIRE(ustr1.find_first_of(IString("ij")) == 18);
    REQUIRE(sstr1.find_first_of(std::string("ij"), 10) == 18);
    REQUIRE(ustr1.find_first_of(IString("ij"), 10) == 18);
    REQUIRE(sstr1.find_first_of(std::string("ij"), 19) == 19);
    REQUIRE(ustr1.find_first_of(IString("ij"), 19) == 19);
    REQUIRE(sstr1.find_first_of(std::string("ij"), 20) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("ij"), 20) == IString::npos);
    REQUIRE(sstr1.find_first_of(std::string("j")) == 19);
    REQUIRE(ustr1.find_first_of(IString("j")) == 19);
    REQUIRE(sstr1.find_first_of(std::string("j"), 5) == 19);
    REQUIRE(ustr1.find_first_of(IString("j"), 5) == 19);
    REQUIRE(sstr1.find_first_of(std::string("j"), 19) == 19);
    REQUIRE(ustr1.find_first_of(IString("j"), 19) == 19);
    REQUIRE(sstr1.find_first_of(std::string("j"), 20) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("j"), 20) == IString::npos);
    REQUIRE(sstr1.find_first_of(std::string("")) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("")) == IString::npos);
    REQUIRE(sstr1.find_first_of(std::string("0123456789abcdefghijk")) == 0);
    REQUIRE(ustr1.find_first_of(IString("0123456789abcdefghijk")) == 0);
    REQUIRE(sstr1.find_first_of(std::string("0123456789abcdefghijk"), 5) == 5);
    REQUIRE(ustr1.find_first_of(IString("0123456789abcdefghijk"), 5) == 5);
    REQUIRE(sstr1.find_first_of(std::string("abcdef012345")) == 0);
    REQUIRE(ustr1.find_first_of(IString("abcdef012345")) == 0);
    REQUIRE(sstr1.find_first_of(std::string("012345a")) == 0);
    REQUIRE(ustr1.find_first_of(IString("012345a")) == 0);
    REQUIRE(sstr1.find_first_of(std::string("012345a"), 3) == 3);
    REQUIRE(ustr1.find_first_of(IString("012345a"), 3) == 3);
    REQUIRE(sstr1.find_first_of(std::string("k")) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("k")) == IString::npos);
    REQUIRE(sstr1.find_first_of(std::string("k"), 2) == IString::npos);
    REQUIRE(ustr1.find_first_of(IString("k"), 2) == IString::npos);
}

// find_first_not_of ==============================================================================

TEST_CASE("IString::find_first_not_of(CharT c, Size pos)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");
    
    REQUIRE(sstr1.find_first_not_of("9") == 0);
    REQUIRE(ustr1.find_first_not_of("9") == 0);
    REQUIRE(sstr1.find_first_not_of("9", 3) == 3);
    REQUIRE(ustr1.find_first_not_of("9", 3) == 3);
    REQUIRE(sstr1.find_first_not_of("9", 10) == IString::npos);
    REQUIRE(ustr1.find_first_not_of("9", 10) == IString::npos);
    REQUIRE(sstr1.find_first_not_of("0") == 1);
    REQUIRE(ustr1.find_first_not_of("0") == 1);
    REQUIRE(sstr1.find_first_not_of("0", 1) == 1);
    REQUIRE(ustr1.find_first_not_of("0", 1) == 1);
    REQUIRE(sstr1.find_first_not_of("1") == 0);
    REQUIRE(ustr1.find_first_not_of("1") == 0);
    REQUIRE(sstr1.find_first_not_of("1", 5) == 5);
    REQUIRE(ustr1.find_first_not_of("1", 5) == 5);
    REQUIRE(sstr1.find_first_not_of("5", 2) == 2);
    REQUIRE(ustr1.find_first_not_of("5", 2) == 2);
    REQUIRE(sstr1.find_first_not_of("5", 7) == 7);
    REQUIRE(ustr1.find_first_not_of("5", 7) == 7);
    REQUIRE(sstr1.find_first_not_of("5") == 0);
    REQUIRE(ustr1.find_first_not_of("5") == 0);
    REQUIRE(sstr1.find_first_not_of("a") == 0);
    REQUIRE(ustr1.find_first_not_of("a") == 0);
    REQUIRE(sstr1.find_first_not_of("a", 5) == 5);
    REQUIRE(ustr1.find_first_not_of("a", 5) == 5);
}

TEST_CASE("IString::find_first_not_of(const IString &s, Size pos)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find_first_not_of(std::string("0123456789abcdefghij")) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("0123456789abcdefghij")) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("abcdefghij")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("abcdefghij")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("abcdefghij"), 3) == 3);
    REQUIRE(ustr1.find_first_not_of(IString("abcdefghij"), 3) == 3);
    REQUIRE(sstr1.find_first_not_of(std::string("789abcdef")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("789abcdef")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("789abcdef"), 2) == 2);
    REQUIRE(ustr1.find_first_not_of(IString("789abcdef"), 2) == 2);
    REQUIRE(sstr1.find_first_not_of(std::string("abcdef789abcdef"), 2) == 2);
    REQUIRE(ustr1.find_first_not_of(IString("abcdef789abcdef"), 2) == 2);
    REQUIRE(sstr1.find_first_not_of(std::string("01")) == 2);
    REQUIRE(ustr1.find_first_not_of(IString("01")) == 2);
    REQUIRE(sstr1.find_first_not_of(std::string("01"), 1) == 2);
    REQUIRE(ustr1.find_first_not_of(IString("01"), 1) == 2);
    REQUIRE(sstr1.find_first_not_of(std::string("12")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("12")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("12"), 1) == 3);
    REQUIRE(ustr1.find_first_not_of(IString("12"), 1) == 3);
    REQUIRE(sstr1.find_first_not_of(std::string("12"), 2) == 3);
    REQUIRE(ustr1.find_first_not_of(IString("12"), 2) == 3);
    REQUIRE(sstr1.find_first_not_of(std::string("12"), 3) == 3);
    REQUIRE(ustr1.find_first_not_of(IString("12"), 3) == 3);
    REQUIRE(sstr1.find_first_not_of(std::string("567")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("567")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("567"), 2) == 2);
    REQUIRE(ustr1.find_first_not_of(IString("567"), 2) == 2);
    REQUIRE(sstr1.find_first_not_of(std::string("567"), 7) == 8);
    REQUIRE(ustr1.find_first_not_of(IString("567"), 7) == 8);
    REQUIRE(sstr1.find_first_not_of(std::string("567"), 9) == 9);
    REQUIRE(ustr1.find_first_not_of(IString("567"), 9) == 9);
    REQUIRE(sstr1.find_first_not_of(std::string("0")) == 1);
    REQUIRE(ustr1.find_first_not_of(IString("0")) == 1);
    REQUIRE(sstr1.find_first_not_of(std::string("0"), 1) == 1);
    REQUIRE(ustr1.find_first_not_of(IString("0"), 1) == 1);
    REQUIRE(sstr1.find_first_not_of(std::string("ij")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("ij")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("ij"), 10) == 10);
    REQUIRE(ustr1.find_first_not_of(IString("ij"), 10) == 10);
    REQUIRE(sstr1.find_first_not_of(std::string("ij"), 19) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("ij"), 19) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("ij"), 20) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("ij"), 20) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("j")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("j")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("j"), 5) == 5);
    REQUIRE(ustr1.find_first_not_of(IString("j"), 5) == 5);
    REQUIRE(sstr1.find_first_not_of(std::string("j"), 19) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("j"), 19) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("j"), 20) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("j"), 20) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(ustr1.find_first_not_of(IString("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(sstr1.find_first_not_of(std::string("abcdef012345")) == 6);
    REQUIRE(ustr1.find_first_not_of(IString("abcdef012345")) == 6);
    REQUIRE(sstr1.find_first_not_of(std::string("012345a")) == 6);
    REQUIRE(ustr1.find_first_not_of(IString("012345a")) == 6);
    REQUIRE(sstr1.find_first_not_of(std::string("012345a"), 3) == 6);
    REQUIRE(ustr1.find_first_not_of(IString("012345a"), 3) == 6);
    REQUIRE(sstr1.find_first_not_of(std::string("k")) == 0);
    REQUIRE(ustr1.find_first_not_of(IString("k")) == 0);
    REQUIRE(sstr1.find_first_not_of(std::string("k"), 2) == 2);
    REQUIRE(ustr1.find_first_not_of(IString("k"), 2) == 2);
}

// find_last_of ==================================================================================

TEST_CASE("IString::find_last_of(CharT c, Size pos)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");
    
    REQUIRE(sstr1.find_last_of('9') == 9);
    REQUIRE(ustr1.find_last_of('9') == 9);
    REQUIRE(sstr1.find_last_of('9', 3) == IString::npos);
    REQUIRE(ustr1.find_last_of('9', 3) == IString::npos);
    REQUIRE(sstr1.find_last_of('9', 10) == 9);
    REQUIRE(ustr1.find_last_of('9', 10) == 9);
    REQUIRE(sstr1.find_last_of('0') == 0);
    REQUIRE(ustr1.find_last_of('0') == 0);
    REQUIRE(sstr1.find_last_of('0', 1) == 0);
    REQUIRE(ustr1.find_last_of('0', 1) == 0);
    REQUIRE(sstr1.find_last_of('1') == 1);
    REQUIRE(ustr1.find_last_of('1') == 1);
    REQUIRE(sstr1.find_last_of('1', 5) == 1);
    REQUIRE(ustr1.find_last_of('1', 5) == 1);
    REQUIRE(sstr1.find_last_of('1', 0) == IString::npos);
    REQUIRE(ustr1.find_last_of('1', 0) == IString::npos);
    REQUIRE(sstr1.find_last_of('5', 2) == IString::npos);
    REQUIRE(ustr1.find_last_of('5', 2) == IString::npos);
    REQUIRE(sstr1.find_last_of('5', 7) == 5);
    REQUIRE(ustr1.find_last_of('5', 7) == 5);
    REQUIRE(sstr1.find_last_of('5') == 5);
    REQUIRE(ustr1.find_last_of('5') == 5);
    REQUIRE(sstr1.find_last_of('a') == IString::npos);
    REQUIRE(ustr1.find_last_of('a') == IString::npos);
    REQUIRE(sstr1.find_last_of('a', 5) == IString::npos);
    REQUIRE(ustr1.find_last_of('a', 5) == IString::npos);
}

TEST_CASE("IString::find_last_of(const IString &s, Size pos)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find_last_of(std::string("0123456789abcdefghij")) == 19);
    REQUIRE(ustr1.find_last_of(IString("0123456789abcdefghij")) == 19);
    REQUIRE(sstr1.find_last_of(std::string("0123456789abcdefghij"), 5) == 5);
    REQUIRE(ustr1.find_last_of(IString("0123456789abcdefghij"), 5) == 5);
    REQUIRE(sstr1.find_last_of(std::string("abcdefghij")) == 19);
    REQUIRE(ustr1.find_last_of(IString("abcdefghij")) == 19);
    REQUIRE(sstr1.find_last_of(std::string("abcdefghij"), 3) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("abcdefghij"), 3) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("789abcdef")) == 15);
    REQUIRE(ustr1.find_last_of(IString("789abcdef")) == 15);
    REQUIRE(sstr1.find_last_of(std::string("789abcdef"), 2) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("789abcdef"), 2) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("abcdef789abcdef"), 2) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("abcdef789abcdef"), 2) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("abcdef789abcdef")) == 15);
    REQUIRE(ustr1.find_last_of(IString("abcdef789abcdef")) == 15);
    REQUIRE(sstr1.find_last_of(std::string("01")) == 1);
    REQUIRE(ustr1.find_last_of(IString("01")) == 1);
    REQUIRE(sstr1.find_last_of(std::string("01"), 1) == 1);
    REQUIRE(ustr1.find_last_of(IString("01"), 1) == 1);
    REQUIRE(sstr1.find_last_of(std::string("12")) == 2);
    REQUIRE(ustr1.find_last_of(IString("12")) == 2);
    REQUIRE(sstr1.find_last_of(std::string("12"), 1) == 1);
    REQUIRE(ustr1.find_last_of(IString("12"), 1) == 1);
    REQUIRE(sstr1.find_last_of(std::string("12"), 2) == 2);
    REQUIRE(ustr1.find_last_of(IString("12"), 2) == 2);
    REQUIRE(sstr1.find_last_of(std::string("12"), 3) == 2);
    REQUIRE(ustr1.find_last_of(IString("12"), 3) == 2);
    REQUIRE(sstr1.find_last_of(std::string("567")) == 7);
    REQUIRE(ustr1.find_last_of(IString("567")) == 7);
    REQUIRE(sstr1.find_last_of(std::string("567"), 2) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("567"), 2) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("567"), 7) == 7);
    REQUIRE(ustr1.find_last_of(IString("567"), 7) == 7);
    REQUIRE(sstr1.find_last_of(std::string("567"), 9) == 7);
    REQUIRE(ustr1.find_last_of(IString("567"), 9) == 7);
    REQUIRE(sstr1.find_last_of(std::string("0")) == 0);
    REQUIRE(ustr1.find_last_of(IString("0")) == 0);
    REQUIRE(sstr1.find_last_of(std::string("0"), 1) == 0);
    REQUIRE(ustr1.find_last_of(IString("0"), 1) == 0);
    REQUIRE(sstr1.find_last_of(std::string("ij")) == 19);
    REQUIRE(ustr1.find_last_of(IString("ij")) == 19);
    REQUIRE(sstr1.find_last_of(std::string("ij"), 10) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("ij"), 10) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("ij"), 19) == 19);
    REQUIRE(ustr1.find_last_of(IString("ij"), 19) == 19);
    REQUIRE(sstr1.find_last_of(std::string("ij"), 20) == 19);
    REQUIRE(ustr1.find_last_of(IString("ij"), 20) == 19);
    REQUIRE(sstr1.find_last_of(std::string("j")) == 19);
    REQUIRE(ustr1.find_last_of(IString("j")) == 19);
    REQUIRE(sstr1.find_last_of(std::string("j"), 5) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("j"), 5) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("j"), 19) == 19);
    REQUIRE(ustr1.find_last_of(IString("j"), 19) == 19);
    REQUIRE(sstr1.find_last_of(std::string("j"), 20) == 19);
    REQUIRE(ustr1.find_last_of(IString("j"), 20) == 19);
    REQUIRE(sstr1.find_last_of(std::string("")) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("")) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("0123456789abcdefghijk")) == 19);
    REQUIRE(ustr1.find_last_of(IString("0123456789abcdefghijk")) == 19);
    REQUIRE(sstr1.find_last_of(std::string("0123456789abcdefghijk"), 5) == 5);
    REQUIRE(ustr1.find_last_of(IString("0123456789abcdefghijk"), 5) == 5);
    REQUIRE(sstr1.find_last_of(std::string("abcdef012345")) == 15);
    REQUIRE(ustr1.find_last_of(IString("abcdef012345")) == 15);
    REQUIRE(sstr1.find_last_of(std::string("012345a")) == 10);
    REQUIRE(ustr1.find_last_of(IString("012345a")) == 10);
    REQUIRE(sstr1.find_last_of(std::string("012345a"), 3) == 3);
    REQUIRE(ustr1.find_last_of(IString("012345a"), 3) == 3);
    REQUIRE(sstr1.find_last_of(std::string("k")) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("k")) == IString::npos);
    REQUIRE(sstr1.find_last_of(std::string("k"), 2) == IString::npos);
    REQUIRE(ustr1.find_last_of(IString("k"), 2) == IString::npos);
}

// find_last_not_of ==================================================================================

TEST_CASE("IString::find_last_not_of(CharT c, Size pos)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");
    
    REQUIRE(sstr1.find_last_not_of('9') == 8);
    REQUIRE(ustr1.find_last_not_of('9') == 8);
    REQUIRE(sstr1.find_last_not_of('9', 3) == 3);
    REQUIRE(ustr1.find_last_not_of('9', 3) == 3);
    REQUIRE(sstr1.find_last_not_of('9', 10) == 8);
    REQUIRE(ustr1.find_last_not_of('9', 10) == 8);
    REQUIRE(sstr1.find_last_not_of('0') == 9);
    REQUIRE(ustr1.find_last_not_of('0') == 9);
    REQUIRE(sstr1.find_last_not_of('0', 1) == 1);
    REQUIRE(ustr1.find_last_not_of('0', 1) == 1);
    REQUIRE(sstr1.find_last_not_of('0', 0) == IString::npos);
    REQUIRE(ustr1.find_last_not_of('0', 0) == IString::npos);
    REQUIRE(sstr1.find_last_not_of('1') == 9);
    REQUIRE(ustr1.find_last_not_of('1') == 9);
    REQUIRE(sstr1.find_last_not_of('1', 5) == 5);
    REQUIRE(ustr1.find_last_not_of('1', 5) == 5);
    REQUIRE(sstr1.find_last_not_of('1', 0) == 0);
    REQUIRE(ustr1.find_last_not_of('1', 0) == 0);
    REQUIRE(sstr1.find_last_not_of('5', 2) == 2);
    REQUIRE(ustr1.find_last_not_of('5', 2) == 2);
    REQUIRE(sstr1.find_last_not_of('5', 7) == 7);
    REQUIRE(ustr1.find_last_not_of('5', 7) == 7);
    REQUIRE(sstr1.find_last_not_of('5', 5) == 4);
    REQUIRE(ustr1.find_last_not_of('5', 5) == 4);
    REQUIRE(sstr1.find_last_not_of('5') == 9);
    REQUIRE(ustr1.find_last_not_of('5') == 9);
    REQUIRE(sstr1.find_last_not_of('a') == 9);
    REQUIRE(ustr1.find_last_not_of('a') == 9);
    REQUIRE(sstr1.find_last_not_of('a', 5) == 5);
    REQUIRE(ustr1.find_last_not_of('a', 5) == 5);
}

TEST_CASE("IString::find_last_not_of(const IString &s, Size pos)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");
    
    REQUIRE(sstr1.find_last_not_of(std::string("0123456789abcdefghij")) == IString::npos);
    REQUIRE(ustr1.find_last_not_of(IString("0123456789abcdefghij")) == IString::npos);
    REQUIRE(sstr1.find_last_not_of(std::string("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(ustr1.find_last_not_of(IString("0123456789abcdefghij"), 5) == IString::npos);
    REQUIRE(sstr1.find_last_not_of(std::string("abcdefghij")) == 9);
    REQUIRE(ustr1.find_last_not_of(IString("abcdefghij")) == 9);
    REQUIRE(sstr1.find_last_not_of(std::string("abcdefghij"), 3) == 3);
    REQUIRE(ustr1.find_last_not_of(IString("abcdefghij"), 3) == 3);
    REQUIRE(sstr1.find_last_not_of(std::string("789abcdef")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("789abcdef")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("789abcdef"), 2) == 2);
    REQUIRE(ustr1.find_last_not_of(IString("789abcdef"), 2) == 2);
    REQUIRE(sstr1.find_last_not_of(std::string("abcdef789abcdef"), 2) == 2);
    REQUIRE(ustr1.find_last_not_of(IString("abcdef789abcdef"), 2) == 2);
    REQUIRE(sstr1.find_last_not_of(std::string("abcdef789abcdef")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("abcdef789abcdef")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("01")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("01")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("01"), 1) == IString::npos);
    REQUIRE(ustr1.find_last_not_of(IString("01"), 1) == IString::npos);
    REQUIRE(sstr1.find_last_not_of(std::string("12")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("12")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("12"), 1) == 0);
    REQUIRE(ustr1.find_last_not_of(IString("12"), 1) == 0);
    REQUIRE(sstr1.find_last_not_of(std::string("12"), 2) == 0);
    REQUIRE(ustr1.find_last_not_of(IString("12"), 2) == 0);
    REQUIRE(sstr1.find_last_not_of(std::string("12"), 3) == 3);
    REQUIRE(ustr1.find_last_not_of(IString("12"), 3) == 3);
    REQUIRE(sstr1.find_last_not_of(std::string("567")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("567")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("567"), 2) == 2);
    REQUIRE(ustr1.find_last_not_of(IString("567"), 2) == 2);
    REQUIRE(sstr1.find_last_not_of(std::string("567"), 7) == 4);
    REQUIRE(ustr1.find_last_not_of(IString("567"), 7) == 4);
    REQUIRE(sstr1.find_last_not_of(std::string("567"), 9) == 9);
    REQUIRE(ustr1.find_last_not_of(IString("567"), 9) == 9);
    REQUIRE(sstr1.find_last_not_of(std::string("0")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("0")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("0"), 1) == 1);
    REQUIRE(ustr1.find_last_not_of(IString("0"), 1) == 1);
    REQUIRE(sstr1.find_last_not_of(std::string("0"), 0) == IString::npos);
    REQUIRE(ustr1.find_last_not_of(IString("0"), 0) == IString::npos);
    REQUIRE(sstr1.find_last_not_of(std::string("ij")) == 17);
    REQUIRE(ustr1.find_last_not_of(IString("ij")) == 17);
    REQUIRE(sstr1.find_last_not_of(std::string("ij"), 10) == 10);
    REQUIRE(ustr1.find_last_not_of(IString("ij"), 10) == 10);
    REQUIRE(sstr1.find_last_not_of(std::string("ij"), 19) == 17);
    REQUIRE(ustr1.find_last_not_of(IString("ij"), 19) == 17);
    REQUIRE(sstr1.find_last_not_of(std::string("ij"), 20) == 17);
    REQUIRE(ustr1.find_last_not_of(IString("ij"), 20) == 17);
    REQUIRE(sstr1.find_last_not_of(std::string("j")) == 18);
    REQUIRE(ustr1.find_last_not_of(IString("j")) == 18);
    REQUIRE(sstr1.find_last_not_of(std::string("j"), 5) == 5);
    REQUIRE(ustr1.find_last_not_of(IString("j"), 5) == 5);
    REQUIRE(sstr1.find_last_not_of(std::string("j"), 19) == 18);
    REQUIRE(ustr1.find_last_not_of(IString("j"), 19) == 18);
    REQUIRE(sstr1.find_last_not_of(std::string("j"), 20) == 18);
    REQUIRE(ustr1.find_last_not_of(IString("j"), 20) == 18);
    REQUIRE(sstr1.find_last_not_of(std::string("")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(ustr1.find_last_not_of(IString("0123456789abcdefghijk")) == IString::npos);
    REQUIRE(sstr1.find_last_not_of(std::string("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(ustr1.find_last_not_of(IString("0123456789abcdefghijk"), 5) == IString::npos);
    REQUIRE(sstr1.find_last_not_of(std::string("abcdef012345")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("abcdef012345")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("012345a")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("012345a")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("012345a"), 3) == IString::npos);
    REQUIRE(ustr1.find_last_not_of(IString("012345a"), 3) == IString::npos);
    REQUIRE(sstr1.find_last_not_of(std::string("k")) == 19);
    REQUIRE(ustr1.find_last_not_of(IString("k")) == 19);
    REQUIRE(sstr1.find_last_not_of(std::string("k"), 2) == 2);
    REQUIRE(ustr1.find_last_not_of(IString("k"), 2) == 2);
}

// replace ===========================================================================================

TEST_CASE("IString::replace(Size pos, Size count, const CharT *cstr)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(0, 0, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij0123456789");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(0, 0, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij0123456789");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(0, 5, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij56789");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(0, 5, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij56789");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(1, 5, "abcdefghij");
    REQUIRE(sstr1 == "0abcdefghij6789");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(1, 5, "abcdefghij");
    REQUIRE(ustr1 == "0abcdefghij6789");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(10, 0, "abcdefghij");
    REQUIRE(sstr1 == "0123456789abcdefghij");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(10, 0, "abcdefghij");
    REQUIRE(ustr1 == "0123456789abcdefghij");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(5, 0, "abcdefghij");
    REQUIRE(sstr1 == "01234abcdefghij56789");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(5, 0, "abcdefghij");
    REQUIRE(ustr1 == "01234abcdefghij56789");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(0, 10, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(0, 10, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(ustr1.length() == 10);

    sstr1 = sstr0;
    sstr1.replace(0, IString::npos, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(0, IString::npos, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(ustr1.length() == 10);

    sstr1 = sstr0;
    sstr1.replace(3, 10, "abcdefghij");
    REQUIRE(sstr1 == "012abcdefghij");
    REQUIRE(sstr1.length() == 13);

    ustr1 = ustr0;
    ustr1.replace(3, 10, "abcdefghij");
    REQUIRE(ustr1 == "012abcdefghij");
    REQUIRE(ustr1.length() == 13);

    sstr1 = sstr0;
    sstr1.replace(3, 5, "abcdefghij");
    REQUIRE(sstr1 == "012abcdefghij89");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(3, 5, "abcdefghij");
    REQUIRE(ustr1 == "012abcdefghij89");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(3, 10, "abc");
    REQUIRE(sstr1 == "012abc");
    REQUIRE(sstr1.length() == 6);

    ustr1 = ustr0;
    ustr1.replace(3, 10, "abc");
    REQUIRE(ustr1 == "012abc");
    REQUIRE(ustr1.length() == 6);
}

TEST_CASE("IString::replace(Size pos, Size count, BasicString &str)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(0, 0, std::string("abcdefghij"));
    REQUIRE(sstr1 == "abcdefghij0123456789");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(0, 0, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "abcdefghij0123456789");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(0, 5, std::string("abcdefghij"));
    REQUIRE(sstr1 == "abcdefghij56789");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(0, 5, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "abcdefghij56789");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(1, 5, std::string("abcdefghij"));
    REQUIRE(sstr1 == "0abcdefghij6789");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(1, 5, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "0abcdefghij6789");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(10, 0, std::string("abcdefghij"));
    REQUIRE(sstr1 == "0123456789abcdefghij");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(10, 0, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "0123456789abcdefghij");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(5, 0, std::string("abcdefghij"));
    REQUIRE(sstr1 == "01234abcdefghij56789");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(5, 0, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "01234abcdefghij56789");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(0, 10, std::string("abcdefghij"));
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(0, 10, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(ustr1.length() == 10);

    sstr1 = sstr0;
    sstr1.replace(0, IString::npos, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(0, IString::npos, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(ustr1.length() == 10);

    sstr1 = sstr0;
    sstr1.replace(3, 10, std::string("abcdefghij"));
    REQUIRE(sstr1 == "012abcdefghij");
    REQUIRE(sstr1.length() == 13);

    ustr1 = ustr0;
    ustr1.replace(3, 10, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "012abcdefghij");
    REQUIRE(ustr1.length() == 13);

    sstr1 = sstr0;
    sstr1.replace(3, 5, std::string("abcdefghij"));
    REQUIRE(sstr1 == "012abcdefghij89");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(3, 5, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "012abcdefghij89");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(3, 10, std::string("abc"));
    REQUIRE(sstr1 == "012abc");
    REQUIRE(sstr1.length() == 6);

    ustr1 = ustr0;
    ustr1.replace(3, 10, BasicString("abc"));
    REQUIRE(ustr1 == "012abc");
    REQUIRE(ustr1.length() == 6);
}

TEST_CASE("IString::replace(const_iterator first, const_iterator last, BasicString &str)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 0, sstr1.begin() + 0, std::string("abcdefghij"));
    REQUIRE(sstr1 == "abcdefghij0123456789");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 0, ustr1.begin() + 0, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "abcdefghij0123456789");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 0, sstr1.begin() + 5, std::string("abcdefghij"));
    REQUIRE(sstr1 == "abcdefghij56789");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 0, ustr1.begin() + 5, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "abcdefghij56789");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 1, sstr1.begin() + 5, std::string("abcdefghij"));
    REQUIRE(sstr1 == "0abcdefghij56789");
    REQUIRE(sstr1.length() == 16);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 1, ustr1.begin() + 5, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "0abcdefghij56789");
    REQUIRE(ustr1.length() == 16);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 0, sstr1.begin() + 10, std::string("abcdefghij"));
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 0, ustr1.begin() + 10, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(ustr1.length() == 10);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 3, sstr1.begin() + 10, std::string("abcdefghij"));
    REQUIRE(sstr1 == "012abcdefghij");
    REQUIRE(sstr1.length() == 13);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 3, ustr1.begin() + 10, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "012abcdefghij");
    REQUIRE(ustr1.length() == 13);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 3, sstr1.begin() + 5, std::string("abcdefghij"));
    REQUIRE(sstr1 == "012abcdefghij56789");
    REQUIRE(sstr1.length() == 18);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 3, ustr1.begin() + 5, BasicString("abcdefghij"));
    REQUIRE(ustr1 == "012abcdefghij56789");
    REQUIRE(ustr1.length() == 18);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 3, sstr1.begin() + 10, std::string("abc"));
    REQUIRE(sstr1 == "012abc");
    REQUIRE(sstr1.length() == 6);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 3, ustr1.begin() + 10, BasicString("abc"));
    REQUIRE(ustr1 == "012abc");
    REQUIRE(ustr1.length() == 6);
}

TEST_CASE("IString::replace(const_iterator first, const_iterator last, const CharT *cstr)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 0, sstr1.begin() + 0, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij0123456789");
    REQUIRE(sstr1.length() == 20);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 0, ustr1.begin() + 0, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij0123456789");
    REQUIRE(ustr1.length() == 20);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 0, sstr1.begin() + 5, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij56789");
    REQUIRE(sstr1.length() == 15);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 0, ustr1.begin() + 5, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij56789");
    REQUIRE(ustr1.length() == 15);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 1, sstr1.begin() + 5, "abcdefghij");
    REQUIRE(sstr1 == "0abcdefghij56789");
    REQUIRE(sstr1.length() == 16);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 1, ustr1.begin() + 5, "abcdefghij");
    REQUIRE(ustr1 == "0abcdefghij56789");
    REQUIRE(ustr1.length() == 16);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 0, sstr1.begin() + 10, "abcdefghij");
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 0, ustr1.begin() + 10, "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(ustr1.length() == 10);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 3, sstr1.begin() + 10, "abcdefghij");
    REQUIRE(sstr1 == "012abcdefghij");
    REQUIRE(sstr1.length() == 13);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 3, ustr1.begin() + 10, "abcdefghij");
    REQUIRE(ustr1 == "012abcdefghij");
    REQUIRE(ustr1.length() == 13);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 3, sstr1.begin() + 5, "abcdefghij");
    REQUIRE(sstr1 == "012abcdefghij56789");
    REQUIRE(sstr1.length() == 18);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 3, ustr1.begin() + 5, "abcdefghij");
    REQUIRE(ustr1 == "012abcdefghij56789");
    REQUIRE(ustr1.length() == 18);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin() + 3, sstr1.begin() + 10, "abc");
    REQUIRE(sstr1 == "012abc");
    REQUIRE(sstr1.length() == 6);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin() + 3, ustr1.begin() + 10, "abc");
    REQUIRE(ustr1 == "012abc");
    REQUIRE(ustr1.length() == 6);
}

TEST_CASE("IString::replace(Size pos, Size count, const BasicString &str, Size pos2, Size count2)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(0, 0, std::string("abcdefghij"), 0, 3);
    REQUIRE(sstr1 == "abc0123456789");
    REQUIRE(sstr1.length() == 13);

    ustr1 = ustr0;
    ustr1.replace(0, 0, BasicString("abcdefghij"), 0, 3);
    REQUIRE(ustr1 == "abc0123456789");
    REQUIRE(ustr1.length() == 13);

    sstr1 = sstr0;
    sstr1.replace(0, 5, std::string("abcdefghij"), 1, 4);
    REQUIRE(sstr1 == "bcde56789");
    REQUIRE(sstr1.length() == 9);

    ustr1 = ustr0;
    ustr1.replace(0, 5, BasicString("abcdefghij"), 1, 4);
    REQUIRE(ustr1 == "bcde56789");
    REQUIRE(ustr1.length() == 9);

    sstr1 = sstr0;
    sstr1.replace(1, 5, std::string("abcdefghij"), 6, 2);
    REQUIRE(sstr1 == "0gh6789");
    REQUIRE(sstr1.length() == 7);

    ustr1 = ustr0;
    ustr1.replace(1, 5, BasicString("abcdefghij"), 6, 2);
    REQUIRE(ustr1 == "0gh6789");
    REQUIRE(ustr1.length() == 7);
}

TEST_CASE("IString::replace(Size pos, Size count, Size count2, CharT c)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(0, 0, 3, 'x');
    REQUIRE(sstr1 == "xxx0123456789");
    REQUIRE(sstr1.length() == 13);

    ustr1 = ustr0;
    ustr1.replace(0, 0, 3, 'x');
    REQUIRE(ustr1 == "xxx0123456789");
    REQUIRE(ustr1.length() == 13);
}

TEST_CASE("IString::replace(const_iterator first, const_iterator last, Size count2, CharT c)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin(), sstr1.begin() + 3, 3, 'x');
    REQUIRE(sstr1 == "xxx3456789");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin(), ustr1.begin() + 3, 3, 'x');
    REQUIRE(ustr1 == "xxx3456789");
    REQUIRE(ustr1.length() == 10);
}

TEST_CASE("IString::replace(const_iterator first, const_iterator last, const_iterator first, const_iterator last, std::initializer_list<CharT> ilist)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;

    IString ustr0("0123456789");
    IString ustr1;

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin(), sstr1.begin() + 3, { 'a', 'b', 'c' });
    REQUIRE(sstr1 == "abc3456789");
    REQUIRE(sstr1.length() == 10);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin(), ustr1.begin() + 3, { 'a', 'b', 'c' });
    REQUIRE(ustr1 == "abc3456789");
    REQUIRE(ustr1.length() == 10);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin(), sstr1.begin() + 3, { 'a', 'b', 'c', 'd' });
    REQUIRE(sstr1 == "abcd3456789");
    REQUIRE(sstr1.length() == 11);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin(), ustr1.begin() + 3, { 'a', 'b', 'c', 'd' });
    REQUIRE(ustr1 == "abcd3456789");
    REQUIRE(ustr1.length() == 11);

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin(), sstr1.begin() + 3, { 'a', 'b' });
    REQUIRE(sstr1 == "ab3456789");
    REQUIRE(sstr1.length() == 9);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin(), ustr1.begin() + 3, { 'a', 'b' });
    REQUIRE(ustr1 == "ab3456789");
    REQUIRE(ustr1.length() == 9);
}

TEST_CASE("IString::replace(Size pos, Size count, const StringViewLikeT &t)", "[string::replace]" ) {
    std::string sstr0("0123456789");
    std::string sstr1;
    std::string sstr2("abcdefghij");
    std::string_view sstr2_view = sstr2;

    IString ustr0("0123456789");
    IString ustr1;
    IString ustr2("abcdefghij");
    StringView ustr2_view = ustr2;

    sstr1 = sstr0;
    sstr1.replace(sstr1.begin(), sstr1.begin() + 3, sstr2_view);
    REQUIRE(sstr1 == "abcdefghij3456789");
    REQUIRE(sstr1.length() == 17);

    ustr1 = ustr0;
    ustr1.replace(ustr1.begin(), ustr1.begin() + 3, ustr2_view);
    REQUIRE(ustr1 == "abcdefghij3456789");
    REQUIRE(ustr1.length() == 17);
}

// copy ===========================================================================================

TEST_CASE("IString::copy(CharT* dst, Size count, Size pos)", "[string]" ) {
    char buf[32];
    IString str("0123456789abcdefghij");
    
    Size c1 = str.copy(buf, 10);
    REQUIRE(memcmp(buf, "0123456789", 10) == 0);
    REQUIRE(c1 == 10);

    Size c2 = str.copy(buf, 10, 5);
    REQUIRE(memcmp(buf, "56789abcde", 10) == 0);
    REQUIRE(c2 == 10);

    Size c3 = str.copy(buf, 4, 16);
    REQUIRE(memcmp(buf, "ghij", 4) == 0);
    REQUIRE(c3 == 4);
}

// push and pop ===================================================================================

TEST_CASE("IString push and pop", "[string]" ) {
    IString ustr("12345");

    ustr.push_back('6');
    REQUIRE(ustr == "123456");
    REQUIRE(ustr.length() == 6);

    ustr.push('7');
    REQUIRE(ustr == "1234567");
    REQUIRE(ustr.length() == 7);

    ustr.pop_back();
    REQUIRE(ustr == "123456");
    REQUIRE(ustr.length() == 6);

    IString::CharType c = ustr.pop();
    REQUIRE(ustr == "12345");
    REQUIRE(ustr.length() == 5);
    REQUIRE(c == '6');
}

// assigning ======================================================================================

TEST_CASE("IString::assign(Size count, CharT c)", "[string]" ) {
    std::string sstr("hello");
    IString ustr("hello");

    sstr.assign(5, 'a');
    ustr.assign(5, 'a');

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "aaaaa");
    REQUIRE(ustr == "aaaaa");
    REQUIRE(strlen(sstr.c_str()) == 5);
    REQUIRE(strlen(ustr.c_str()) == 5);
}

TEST_CASE("IString::assign(const IString &str)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    IString ustr1("hello ");
    IString ustr2("world!");

    sstr1.assign(sstr2);
    ustr1.assign(ustr2);

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "world!");
    REQUIRE(ustr1 == "world!");
    REQUIRE(strlen(sstr1.c_str()) == 6);
    REQUIRE(strlen(ustr1.c_str()) == 6);
}

TEST_CASE("IString::assign(BasicString &&str)", "[string]" ) {
    std::string sstr1("1234567890");
    IString ustr1("1234567890");

    sstr1.assign("abcdefghij");
    ustr1.assign("abcdefghij");

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(strlen(sstr1.c_str()) == 10);
    REQUIRE(strlen(ustr1.c_str()) == 10);
}

TEST_CASE("IString::assign(const CharT *ptr, Size length)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    IString ustr("hello ");

    sstr.assign(cstr, strlen(cstr));
    ustr.assign(cstr, strlen(cstr));

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "world!");
    REQUIRE(ustr == "world!");
    REQUIRE(strlen(sstr.c_str()) == 6);
    REQUIRE(strlen(ustr.c_str()) == 6);
}

TEST_CASE("IString::assign(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    IString ustr("hello ");

    sstr.assign(cstr);
    ustr.assign(cstr);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "world!");
    REQUIRE(ustr == "world!");
    REQUIRE(strlen(sstr.c_str()) == 6);
    REQUIRE(strlen(ustr.c_str()) == 6);
}

TEST_CASE("IString::assign(InputIt first, InputIt last)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    IString ustr1("0123456789");
    IString ustr2("abcdefghij");

    sstr1.assign(sstr2.begin() + 3, sstr2.end());
    ustr1.assign(ustr2.begin() + 3, ustr2.end());

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "defghij");
    REQUIRE(ustr1 == "defghij");
    REQUIRE(strlen(sstr1.c_str()) == 7);
    REQUIRE(strlen(ustr1.c_str()) == 7);
}

TEST_CASE("IString::assign(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr.assign({ 'a', 'b', 'c' });
    ustr.assign({ 'a', 'b', 'c' });

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("IString::assign(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr.assign(vstr_view);
    ustr.assign(vstr_view);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("IString::assign(const StringViewLikeT &t, Size pos, Size count)", "[string]" ) {
    std::string_view vstr("abcdefghij");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr.assign(vstr_view, 3, 4);
    ustr.assign(vstr_view, 3, 4);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "defg");
    REQUIRE(ustr == "defg");
    REQUIRE(strlen(sstr.c_str()) == 4);
    REQUIRE(strlen(ustr.c_str()) == 4);
}

// operator= ======================================================================================

TEST_CASE("IString::operator=(const IString &other)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    IString ustr1("hello ");
    IString ustr2("world!");

    sstr1 = sstr2;
    ustr1 = ustr2;

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "world!");
    REQUIRE(ustr1 == "world!");
    REQUIRE(strlen(sstr1.c_str()) == 6);
    REQUIRE(strlen(ustr1.c_str()) == 6);
}

TEST_CASE("IString::operator=(BasicString &&str)", "[string]" ) {
    std::string sstr1("1234567890");
    IString ustr1("1234567890");

    sstr1 = std::string("abcdefghij");
    ustr1 = IString("abcdefghij");

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "abcdefghij");
    REQUIRE(ustr1 == "abcdefghij");
    REQUIRE(strlen(sstr1.c_str()) == 10);
    REQUIRE(strlen(ustr1.c_str()) == 10);
}

TEST_CASE("IString::operator=(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    IString ustr("hello ");

    sstr = cstr;
    ustr = cstr;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "world!");
    REQUIRE(ustr == "world!");
    REQUIRE(strlen(sstr.c_str()) == 6);
    REQUIRE(strlen(ustr.c_str()) == 6);
}

TEST_CASE("IString::operator=(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr = { 'a', 'b', 'c' };
    ustr = { 'a', 'b', 'c' };

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

TEST_CASE("IString::operator=(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr = vstr_view;
    ustr = vstr_view;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "abc");
    REQUIRE(ustr == "abc");
    REQUIRE(strlen(sstr.c_str()) == 3);
    REQUIRE(strlen(ustr.c_str()) == 3);
}

// appending ======================================================================================

TEST_CASE("IString::append(Size count, CharT c)", "[string]" ) {
    std::string sstr("hello");
    IString ustr("hello");

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

TEST_CASE("IString::append(const IString &str)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    IString ustr1("hello ");
    IString ustr2("world!");

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

TEST_CASE("IString::append(const BasicString &str, Size pos, Size count)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    IString ustr1("0123456789");
    IString ustr2("abcdefghij");

    sstr1.append(sstr2, 5);
    ustr1.append(ustr2, 5);

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "0123456789fghij");
    REQUIRE(ustr1 == "0123456789fghij");
    REQUIRE(strlen(sstr1.c_str()) == 15);
    REQUIRE(strlen(ustr1.c_str()) == 15);
}

TEST_CASE("IString::append(const CharT *ptr, Size length)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    IString ustr("hello ");

    sstr.append(cstr, strlen(cstr));
    ustr.append(cstr, strlen(cstr));

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "hello world!");
    REQUIRE(ustr == "hello world!");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("BasicString<char32_t>::append(const CharT *ptr, Size length)", "[string]" ) {
    const char *cstr("world!");
    std::basic_string<char32_t> sstr(U"hello world!");
    BasicString<char32_t> ustr(U"hello ");

    ustr.append(cstr, strlen(cstr));

    REQUIRE(ustr == sstr);
    REQUIRE(ustr == U"hello world!");
    REQUIRE(ustr.length() == 12);
}

TEST_CASE("IString::append(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    IString ustr("hello ");

    sstr.append(cstr);
    ustr.append(cstr);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "hello world!");
    REQUIRE(ustr == "hello world!");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("IString::append(InputIt first, InputIt last)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    IString ustr1("0123456789");
    IString ustr2("abcdefghij");

    sstr1.append(sstr2.begin() + 3, sstr2.end());
    ustr1.append(ustr2.begin() + 3, ustr2.end());

    REQUIRE(ustr1 == sstr1);
    REQUIRE(sstr1 == "0123456789defghij");
    REQUIRE(ustr1 == "0123456789defghij");
    REQUIRE(strlen(sstr1.c_str()) == 17);
    REQUIRE(strlen(ustr1.c_str()) == 17);
}

TEST_CASE("IString::append(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr.append({ 'a', 'b', 'c' });
    ustr.append({ 'a', 'b', 'c' });

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("IString::append(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr.append(vstr_view);
    ustr.append(vstr_view);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("IString::append(const StringViewLikeT &t, Size pos, Size count)", "[string]" ) {
    std::string_view vstr("abcdefghij");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr.append(vstr_view, 3, 4);
    ustr.append(vstr_view, 3, 4);

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789defg");
    REQUIRE(ustr == "0123456789defg");
    REQUIRE(strlen(sstr.c_str()) == 14);
    REQUIRE(strlen(ustr.c_str()) == 14);
}

TEST_CASE("IString::append(CharT c)", "[string]" ) {
    std::string sstr("0123456789a");
    IString ustr("0123456789");

    ustr.append('a');

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789a");
    REQUIRE(ustr == "0123456789a");
    REQUIRE(strlen(sstr.c_str()) == 11);
    REQUIRE(strlen(ustr.c_str()) == 11);
}

// FIXME
// TEST_CASE("IString::append(const Spread &)", "[string]" ) {
//     IString str;
//     Spread<int> spread;
//     spread.add(1,3);
//     spread.add(5,7);
//     spread.add(11);
//     str.append(spread);
//     REQUIRE(str == "1..3,5..7,11");
// }

// operator+= =====================================================================================

TEST_CASE("IString::operator+=(const IString &str)", "[string]" ) {
    std::string sstr1("hello ");
    std::string sstr2("world!");
    IString ustr1("hello ");
    IString ustr2("world!");

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

TEST_CASE("IString::operator+=(const CharT *ptr)", "[string]" ) {
    const char *cstr("world!");
    std::string sstr("hello ");
    IString ustr("hello ");

    sstr += cstr;
    ustr += cstr;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "hello world!");
    REQUIRE(ustr == "hello world!");
    REQUIRE(strlen(sstr.c_str()) == 12);
    REQUIRE(strlen(ustr.c_str()) == 12);
}

TEST_CASE("IString::operator+=(std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr += { 'a', 'b', 'c' };
    ustr += { 'a', 'b', 'c' };

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("IString::operator+=(const StringViewLikeT &t)", "[string]" ) {
    std::string_view vstr("abc");
    std::string_view vstr_view(vstr);
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr += vstr_view;
    ustr += vstr_view;

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789abc");
    REQUIRE(ustr == "0123456789abc");
    REQUIRE(strlen(sstr.c_str()) == 13);
    REQUIRE(strlen(ustr.c_str()) == 13);
}

TEST_CASE("IString::operator+=(CharT c)", "[string]" ) {
    std::string sstr("0123456789a");
    IString ustr("0123456789");

    ustr += 'a';

    REQUIRE(ustr == sstr);
    REQUIRE(sstr == "0123456789a");
    REQUIRE(ustr == "0123456789a");
    REQUIRE(strlen(sstr.c_str()) == 11);
    REQUIRE(strlen(ustr.c_str()) == 11);
}

// operator+= =====================================================================================

TEST_CASE("IString::operator+(const IString &a, const IString &b)", "[string]" ) {
    std::string sstr1("hello ");
    IString ustr1("hello ");
    std::string sstr1a("world!");
    IString ustr1a("world!");

    std::string sstr2 = sstr1 + sstr1a;
    IString ustr2 = ustr1 + ustr1a;

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello world!");
    REQUIRE(ustr2 == "hello world!");
    REQUIRE(sstr2.length() == 12);
    REQUIRE(ustr2.length() == 12);
}

TEST_CASE("IString::operator+(const IString &a, CharT b)", "[string]" ) {
    std::string sstr1("hello");
    IString ustr1("hello");

    std::string sstr2 = sstr1 + '!';
    IString ustr2 = ustr1 + '!';

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello!");
    REQUIRE(ustr2 == "hello!");
    REQUIRE(sstr2.length() == 6);
    REQUIRE(ustr2.length() == 6);
}

TEST_CASE("IString::operator+(const CharT *a, const IString &b)", "[string]" ) {
    std::string sstr1("world!");
    IString ustr1("world!");

    std::string sstr2 = "hello " + sstr1;
    IString ustr2 = "hello " + ustr1;

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello world!");
    REQUIRE(ustr2 == "hello world!");
    REQUIRE(sstr2.length() == 12);
    REQUIRE(ustr2.length() == 12);
}

TEST_CASE("IString::operator+(CharT a, const IString &b)", "[string]" ) {
    std::string sstr1("hello");
    IString ustr1("hello");

    std::string sstr2 = '!' + sstr1;
    IString ustr2 = '!' + ustr1;

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "!hello");
    REQUIRE(ustr2 == "!hello");
    REQUIRE(sstr2.length() == 6);
    REQUIRE(ustr2.length() == 6);
}

TEST_CASE("IString::operator+(const IString &&a, const IString &&b)", "[string]" ) {
    std::string sstr2 = std::string("hello ") + std::string("world!");
    IString ustr2 = IString("hello ") + IString("world!");

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello world!");
    REQUIRE(ustr2 == "hello world!");
    REQUIRE(sstr2.length() == 12);
    REQUIRE(ustr2.length() == 12);
}

TEST_CASE("IString::operator+(const IString &&a, const IString &b)", "[string]" ) {
    std::string sstr1("world!");
    IString ustr1("world!");

    std::string sstr2 = std::string("hello ") + sstr1;
    IString ustr2 = IString("hello ") + ustr1;

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello world!");
    REQUIRE(ustr2 == "hello world!");
    REQUIRE(sstr2.length() == 12);
    REQUIRE(ustr2.length() == 12);
}

TEST_CASE("IString::operator+(const IString &&a, const CharT *b)", "[string]" ) {
    std::string sstr2 = std::string("hello ") + "world!";
    IString ustr2 = IString("hello ") + "world!";

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello world!");
    REQUIRE(ustr2 == "hello world!");
    REQUIRE(sstr2.length() == 12);
    REQUIRE(ustr2.length() == 12);
}

TEST_CASE("IString::operator+(const IString &&a, CharT b)", "[string]" ) {
    std::string sstr2 = std::string("hello") + '!';
    IString ustr2 = IString("hello") + '!';

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello!");
    REQUIRE(ustr2 == "hello!");
    REQUIRE(sstr2.length() == 6);
    REQUIRE(ustr2.length() == 6);
}

TEST_CASE("IString::operator+(const IString &a, const IString &&b)", "[string]" ) {
    std::string sstr1("hello ");
    IString ustr1("hello ");

    std::string sstr2 = sstr1 + std::string("world!");
    IString ustr2 = ustr1 + IString("world!");

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello world!");
    REQUIRE(ustr2 == "hello world!");
    REQUIRE(sstr2.length() == 12);
    REQUIRE(ustr2.length() == 12);
}

TEST_CASE("IString::operator+(const CharT *a, const IString &&b)", "[string]" ) {
    std::string sstr2 = "hello " + std::string("world!");
    IString ustr2 = "hello " + IString("world!");

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "hello world!");
    REQUIRE(ustr2 == "hello world!");
    REQUIRE(sstr2.length() == 12);
    REQUIRE(ustr2.length() == 12);
}

TEST_CASE("IString::operator+(CharT a, const IString &&b)", "[string]" ) {
    std::string sstr2 = '!' + std::string("hello");
    IString ustr2 = '!' + IString("hello");

    REQUIRE(sstr2 == ustr2);
    REQUIRE(sstr2 == "!hello");
    REQUIRE(ustr2 == "!hello");
    REQUIRE(sstr2.length() == 6);
    REQUIRE(ustr2.length() == 6);
}

// inserting ======================================================================================

TEST_CASE("IString::insert(Size index, Size count, CharT c)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");
    
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

TEST_CASE("IString::insert(Size index, const CharT *s)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");
    
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

TEST_CASE("IString::insert(Size index, const CharT *s, Size count)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");
    
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

TEST_CASE("IString::insert(Size index, const BasicString& str)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    IString ustr1("0123456789");
    IString ustr2("abcdefghij");
    
    sstr1.insert(5, sstr2);
    ustr1.insert(5, ustr2);
    REQUIRE(sstr1 == "01234abcdefghij56789");
    REQUIRE(ustr1 == "01234abcdefghij56789");
    REQUIRE(strlen(sstr1.c_str()) == 20);
    REQUIRE(strlen(ustr1.c_str()) == 20);
}

TEST_CASE("IString::insert(Size index, const BasicString &str, Size index_str, Size count)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    IString ustr1("0123456789");
    IString ustr2("abcdefghij");
    
    sstr1.insert(5, sstr2, 3, 3);
    ustr1.insert(5, ustr2, 3, 3);
    REQUIRE(sstr1 == "01234def56789");
    REQUIRE(ustr1 == "01234def56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);

    sstr1.insert(sstr1.length(), sstr2, 3, std::string::npos);
    ustr1.insert(ustr1.length(), sstr2, 3, IString::npos);
    REQUIRE(sstr1 == "01234def56789defghij");
    REQUIRE(ustr1 == "01234def56789defghij");
    REQUIRE(strlen(sstr1.c_str()) == 20);
    REQUIRE(strlen(ustr1.c_str()) == 20);
}

TEST_CASE("IString::insert(const_iterator pos, CharT ch)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");

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

TEST_CASE("IString::insert(const_iterator pos, Size count, CharT ch)", "[string]" ) {
    std::string sstr("0123456789");
    IString ustr("0123456789");

    sstr.insert(sstr.begin(), 4, 'a');
    ustr.insert(ustr.begin(), 4, 'a');
    REQUIRE(sstr == "aaaa0123456789");
    REQUIRE(ustr == "aaaa0123456789");
    REQUIRE(strlen(sstr.c_str()) == 14);
    REQUIRE(strlen(ustr.c_str()) == 14);
}

TEST_CASE("IString::insert(const_iterator pos, InputIt first, InputIt last)", "[string]" ) {
    std::string sstr1("0123456789");
    std::string sstr2("abcdefghij");
    IString ustr1("0123456789");
    IString ustr2("abcdefghij");

    sstr1.insert(sstr1.begin() + 5, sstr2.begin(), sstr2.end());
    ustr1.insert(ustr1.begin() + 5, ustr2.begin(), ustr2.end());
    REQUIRE(sstr1 == "01234abcdefghij56789");
    REQUIRE(ustr1 == "01234abcdefghij56789");
    REQUIRE(strlen(sstr1.c_str()) == 20);
    REQUIRE(strlen(ustr1.c_str()) == 20);
}

TEST_CASE("IString::insert(const_iterator pos, std::initializer_list<CharT> ilist)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");

    sstr1.insert(sstr1.begin() + 5, { 'a', 'b', 'c' });
    ustr1.insert(ustr1.begin() + 5, { 'a', 'b', 'c' });
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("IString::insert(Size index, const StringViewLikeT &t)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");

    std::string sstr2 = "abc";

    sstr1.insert(5, sstr2);
    ustr1.insert(5, sstr2);
    REQUIRE(sstr1 == "01234abc56789");
    REQUIRE(ustr1 == "01234abc56789");
    REQUIRE(strlen(sstr1.c_str()) == 13);
    REQUIRE(strlen(ustr1.c_str()) == 13);
}

TEST_CASE("IString::insert(Size index, const StringViewLikeT &t, Size index_str, Size count)", "[string]" ) {
    std::string sstr1("0123456789");
    IString ustr1("0123456789");

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

TEST_CASE("IString iterator test 1", "[string]" ) {
    IString str("hello!");
    REQUIRE(strlen(str.c_str()) == 6);
    int index = 0;
    for (auto it = str.begin(); it != str.end(); ++it) {
        index++;
    }
    REQUIRE(index == 6);
}

TEST_CASE("IString iterator test 2", "[string]" ) {
    IString str1("hello!");
    REQUIRE(strlen(str1.c_str()) == 6);
    IString str2;
    for (auto it = str1.begin(); it != str1.end(); ++it) {
        str2.append(*it);
    }
    REQUIRE(str1 == str2);
}

TEST_CASE("IString iterator test 2a", "[string]" ) {
    IString str1("12345");
    IString str2;
    auto it = str1.begin();
    ++it;
    str2 += it[0];
    str2 += it[1];
    REQUIRE(str2 == "23");
    REQUIRE(strlen(str2.c_str()) == 2);
}

TEST_CASE("IString iterator test 2b", "[string]" ) {
    std::string str1("12345");
    std::string str2;
    auto it = str1.begin();
    ++it;
    str2 += it[0];
    str2 += it[1];
    REQUIRE(str2 == "23");
    REQUIRE(strlen(str2.c_str()) == 2);
}

TEST_CASE("IString iterator test 3", "[string]" ) {
    IString str1("1234567890");
    IString str2;
    for (auto it = str1.begin(); it < str1.end(); it += 2) {
        str2.append(*it);
    }
    REQUIRE(str2 == "13579");
    REQUIRE(strlen(str2.c_str()) == 5);
}

TEST_CASE("IString reverse_iterator append test", "[string]" ) {
    IString str1("1234567890");
    IString str2;
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

TEST_CASE("IString iterator test comparators", "[string]" ) {
    IString str1("1234567890");
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

TEST_CASE("IString reverse_iterator test comparators", "[string]" ) {
    IString str1("1234567890");
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

TEST_CASE("IString convert to std::string with operator std::basic_string", "[string]" ) {
    IString str("hello there");
    REQUIRE(strlen(str.c_str()) == 11);
    std::string dat = str;
    REQUIRE(dat == "hello there");
}

// swapping =======================================================================================

TEST_CASE("IString::swap 1", "[string]" ) {
    IString str1("12345");
    IString str2("abcde");
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    REQUIRE(str1 == "abcde");
    REQUIRE(str2 == "12345");
}

TEST_CASE("IString::swap 2", "[string]" ) {
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

    IString str1(big_str);
    IString str2(small_str);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    REQUIRE(str1 == small_str);
    REQUIRE(str2 == big_str);
}

TEST_CASE("IString::swap 3", "[string]" ) {
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

    IString str1(small_str);
    IString str2(big_str);
    REQUIRE(str1.is_using_inline_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_inline_buffer());
    REQUIRE(str1 == big_str);
    REQUIRE(str2 == small_str);
}

TEST_CASE("IString::swap 4", "[string]" ) {
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

    IString str1(big_str1);
    IString str2(big_str2);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    std::swap(str1, str2);
    REQUIRE(str1.is_using_allocated_buffer());
    REQUIRE(str2.is_using_allocated_buffer());
    REQUIRE(str1 == big_str2);
    REQUIRE(str2 == big_str1);
}

// erasing ======================================================================================

TEST_CASE("IString::erase(Size index, Size count)", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    IString ustr("0123456789abcdefghij");

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

TEST_CASE("IString::erase(const_iterator pos)", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    IString ustr("0123456789abcdefghij");

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

TEST_CASE("IString::erase(const_iterator first, const_iterator last)", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    IString ustr("0123456789abcdefghij");

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

TEST_CASE("IString::erase(const_iterator first, const_iterator last) check", "[string]" ) {
    std::string sstr("0123456789abcdefghij");
    IString ustr("0123456789abcdefghij");

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

TEST_CASE("IString::substr(Size pos, Size count)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");

    std::string sstr2 = sstr1.substr(0, 5);
    IString ustr2 = ustr1.substr(0, 5);
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

TEST_CASE("IString::substrview(Size pos, Size count)", "[string]" ) {
    std::string sstr1("0123456789abcdefghij");
    IString ustr1("0123456789abcdefghij");

    auto sstr2 = sstr1.substr(0, 5);
    auto ustr2 = ustr1.substrview(0, 5);
    REQUIRE(sstr2 == "01234");
    REQUIRE(IString(ustr2) == "01234");
    REQUIRE(sstr2.length() == 5);
    REQUIRE(ustr2.length() == 5);

    sstr2 = sstr1.substr();
    ustr2 = ustr1.substrview();
    REQUIRE(sstr2 == "0123456789abcdefghij");
    REQUIRE(IString(ustr2) == "0123456789abcdefghij");
    REQUIRE(sstr2.length() == 20);
    REQUIRE(ustr2.length() == 20);

    sstr2 = sstr1.substr(10);
    ustr2 = ustr1.substrview(10);
    REQUIRE(sstr2 == "abcdefghij");
    REQUIRE(IString(ustr2) == "abcdefghij");
    REQUIRE(sstr2.length() == 10);
    REQUIRE(ustr2.length() == 10);
}


// shrinking ======================================================================================

TEST_CASE("IString::shrink() 1", "[string]" ) {
    IString str1("abcdefghij");
    IString str2;

    for (int idx = 0; idx < 33; idx++) {
        str2 += str1;
    }
    for (int idx = 0; idx < 32; idx++) {
        str2.erase(0, str1.length());
    }
    str2.shrink_to_fit();
    REQUIRE(str2.length() == str1.length());
    REQUIRE(str2.capacity() == IString::InlineCapacity);
}

// replace_all =====================================================================================

TEST_CASE("IString::replace_all(CharT a, CharT b)", "[string]" ) {
    IString ustr1("01123456789abcdefghij");

    ustr1.replace_all('a', 'z');
    REQUIRE(ustr1 == "01123456789zbcdefghij");
    REQUIRE(ustr1.length() == 21);

    ustr1.replace_all('0', 'y');
    REQUIRE(ustr1 == "y1123456789zbcdefghij");
    REQUIRE(ustr1.length() == 21);

    ustr1.replace_all('j', 'x');
    REQUIRE(ustr1 == "y1123456789zbcdefghix");
    REQUIRE(ustr1.length() == 21);

    ustr1.replace_all('1', 'w');
    REQUIRE(ustr1 == "yww23456789zbcdefghix");
    REQUIRE(ustr1.length() == 21);
}

TEST_CASE("IString::replace_all(StringViewLikeT &a, StringViewLikeT &b)", "[string]" ) {
    IString ustr1("0123456789");

    ustr1.replace_all("23", "AB");
    REQUIRE(ustr1 == "01AB456789");
    REQUIRE(ustr1.length() == 10);

    ustr1.replace_all("7", "CDE");
    REQUIRE(ustr1 == "01AB456CDE89");
    REQUIRE(ustr1.length() == 12);

    ustr1.replace_all("6CDE8", "");
    REQUIRE(ustr1 == "01AB459");
    REQUIRE(ustr1.length() == 7);

    ustr1 = "01234567890123456789";
    ustr1.replace_all("9", "Z");
    REQUIRE(ustr1 == "012345678Z012345678Z");
    REQUIRE(ustr1.length() == 20);
}

// chomp & chop ===================================================================================

TEST_CASE("IString::chomp(", "[string]" ) {
    IString ustr1("0123456789\n");

    ustr1.chomp();
    REQUIRE(ustr1 == "0123456789");
    REQUIRE(ustr1.length() == 10);
}

// constexpr ======================================================================================

TEST_CASE("IString constexpr", "[string]" ) {
    static constexpr IString ustr("0123456789");
    REQUIRE(ustr == "0123456789");
    REQUIRE(ustr.length() == 10);
}

// whitespace =====================================================================================

TEST_CASE("IString::is_whitespace 1", "[string]" ) {
    static constexpr IString ustr("0123456789 ");
    static constexpr bool n1 = IString::is_whitespace(ustr[0]);
    static constexpr bool y1 = IString::is_whitespace<false>(ustr[0]);
    static constexpr bool n2 = IString::is_whitespace<false>(ustr[10]);
    static constexpr bool y2 = IString::is_whitespace(ustr[10]);
    REQUIRE_FALSE(n1);
    REQUIRE_FALSE(n2);
    REQUIRE(y1);
    REQUIRE(y2);
    REQUIRE(ustr.length() == 11);
}

TEST_CASE("IString::is_whitespace 2", "[string]" ) {
    static constexpr BasicString<char32_t> ustr(U"0123456789 ");
    static constexpr bool n1 = IString::is_whitespace(ustr[0]);
    static constexpr bool y1 = IString::is_whitespace<false>(ustr[0]);
    static constexpr bool n2 = IString::is_whitespace<false>(ustr[10]);
    static constexpr bool y2 = IString::is_whitespace(ustr[10]);
    REQUIRE_FALSE(n1);
    REQUIRE_FALSE(n2);
    REQUIRE(y1);
    REQUIRE(y2);
    REQUIRE(ustr.length() == 11);
}

// comparisons ====================================================================================

TEST_CASE("IString comparisons" ) {
    std::string sstr1("abcdefghij");
    std::string sstr2("bcdefghij");
    std::string sstr3 = sstr1;

    IString ustr1("abcdefghij");
    IString ustr2("bcdefghij");
    IString ustr3 = ustr1;

    REQUIRE(sstr1 < sstr2);
    REQUIRE(ustr1 < ustr2);

    REQUIRE(sstr1 <= sstr2);
    REQUIRE(ustr1 <= ustr2);

    REQUIRE(sstr2 > sstr1);
    REQUIRE(ustr2 > ustr1);

    REQUIRE(sstr2 >= sstr1);
    REQUIRE(ustr2 >= ustr1);

    REQUIRE(sstr1 == sstr3);
    REQUIRE(ustr1 == ustr3);

    REQUIRE(sstr1 == sstr1);
    REQUIRE(ustr1 == ustr1);

    REQUIRE(sstr1 != sstr2);
    REQUIRE(ustr1 != ustr2);
}


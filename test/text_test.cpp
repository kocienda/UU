//
// uu_str_test.cpp
//

#include <UU/UU.h>
#include <UU/UUText.h>
#include <UU/Types.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "text storage smoke", "[file_like]" ) {

    BasicTextStorage<32> s1;
    BasicTextStorage<8> s2;

    REQUIRE(s1.capacity() == 32);
    REQUIRE(s2.capacity() == 8);
}

TEST_CASE( "text smoke", "[file_like]" ) {
    Text8 t1("hello!");
    REQUIRE(t1.length() == 6);
}

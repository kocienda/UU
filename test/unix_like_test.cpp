//
// unix_like_test.cpp
//
#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "shell_escaped_string function", "[unix_like]" ) {
    REQUIRE(shell_escaped_string("foo") == "foo");
    REQUIRE(shell_escaped_string("foo bar") == "foo\\ bar");
    REQUIRE(shell_escaped_string("foo bar's") == "foo\\ bar\\'s");
    REQUIRE_FALSE(shell_escaped_string("foo bar") == "foo bar");
    REQUIRE_FALSE(shell_escaped_string("foo bar's") == "foo bar's");
}

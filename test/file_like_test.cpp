//
// file_like_test.cpp
//

#include <filesystem>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

namespace fs = std::filesystem;

using namespace UU;

TEST_CASE( "filename_match function", "[file_like]" ) {
    REQUIRE(filename_match("foo", fs::path("foobar")));
    REQUIRE(filename_match("foo", fs::path("foobar"), FilenameMatchWildcard));
    REQUIRE(filename_match(".foo", fs::path(".foobar")));
    REQUIRE(filename_match("foo", fs::path(".foobar")));
    REQUIRE_FALSE(filename_match("foo", fs::path("barbaz")));
    REQUIRE_FALSE(filename_match("foo", fs::path("foobar"), FilenameMatchExact));
}

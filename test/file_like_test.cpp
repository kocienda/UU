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
    REQUIRE(is_searchable(UU::searchable_paths(), "foo.txt"));
    REQUIRE(is_searchable(UU::searchable_paths(), "foo.cpp"));
    REQUIRE(is_searchable(UU::searchable_paths(), "foo.cpp"));
    REQUIRE_FALSE(is_searchable(UU::searchable_paths(), "lldb/test/API/functionalities/postmortem/elf-core/linux-s390x.core"));
    REQUIRE_FALSE(is_searchable(UU::searchable_paths(), "foo.core"));
    REQUIRE_FALSE(is_searchable(UU::searchable_paths(), "core"));
    REQUIRE_FALSE(filename_match("foo", fs::path("barbaz")));
    REQUIRE_FALSE(filename_match("foo", fs::path("foobar"), FilenameMatchExact));
}

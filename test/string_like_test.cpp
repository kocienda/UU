//
// string_like_test.cpp
//

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "line end offsets 1", "[string_like]" ) {
    std::string string("foo\nbar\nbaz\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 2));
    REQUIRE(line == "bar");
}

TEST_CASE( "line end offsets 2", "[string_like]" ) {
    std::string string("foo\nbar\nbaz\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 5));
    REQUIRE(line == "the end");
}

TEST_CASE( "line end offsets 3", "[string_like]" ) {
    std::string string("foo\nbar\nbaz\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 1));
    REQUIRE(line == "foo");
}

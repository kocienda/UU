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

TEST_CASE( "line end offsets 1LF", "[string_like]" ) {
    std::string string("foo\nbar\nbaz\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 2));
    REQUIRE(line == "bar");
}

TEST_CASE( "line end offsets 1CR", "[string_like]" ) {
    std::string string("foo\rbar\rbaz\ra longer line\rthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 2));
    REQUIRE(line == "bar");
}

TEST_CASE( "line end offsets 1CRLF", "[string_like]" ) {
    std::string string("foo\r\nbar\r\nbaz\r\na longer line\r\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 2));
    REQUIRE(line == "bar");
}

TEST_CASE( "line end offsets 1MIX", "[string_like]" ) {
    std::string string("foo\nbar\rbaz\r\na longer line\rthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 2));
    REQUIRE(line == "bar");
}

TEST_CASE( "line end offsets 2LF", "[string_like]" ) {
    std::string string("foo\nbar\nbaz\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 5));
    REQUIRE(line == "the end");
}

TEST_CASE( "line end offsets 2CR", "[string_like]" ) {
    std::string string("foo\rbar\rbaz\ra longer line\rthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 5));
    REQUIRE(line == "the end");
}

TEST_CASE( "line end offsets 2CRLF", "[string_like]" ) {
    std::string string("foo\r\nbar\r\nbaz\r\na longer line\r\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 5));
    REQUIRE(line == "the end");
}

TEST_CASE( "line end offsets 2MIX", "[string_like]" ) {
    std::string string("foo\rbar\nbaz\r\na longer line\r\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 5));
    REQUIRE(line == "the end");
}

TEST_CASE( "line end offsets 3LF", "[string_like]" ) {
    std::string string("foo\nbar\nbaz\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 1));
    REQUIRE(line == "foo");
}

TEST_CASE( "line end offsets 3CR", "[string_like]" ) {
    std::string string("foo\rbar\rbaz\ra longer line\rthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 1));
    REQUIRE(line == "foo");
}

TEST_CASE( "line end offsets 3CRLF", "[string_like]" ) {
    std::string string("foo\r\nbar\r\nbaz\r\na longer line\r\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 1));
    REQUIRE(line == "foo");
}

TEST_CASE( "line end offsets 3MIX", "[string_like]" ) {
    std::string string("foo\nbar\rbaz\r\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 1));
    REQUIRE(line == "foo");
}

TEST_CASE( "line end offsets 4LF", "[string_like]" ) {
    std::string string("foo\nbar\nbaz\n\n\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 6));
    REQUIRE(line == "a longer line");
}

TEST_CASE( "line end offsets 4CR", "[string_like]" ) {
    std::string string("foo\rbar\rbaz\r\r\ra longer line\rthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 6));
    REQUIRE(line == "a longer line");
}

TEST_CASE( "line end offsets 4CRLF", "[string_like]" ) {
    std::string string("foo\r\nbar\r\nbaz\r\n\r\n\r\na longer line\r\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 6));
    REQUIRE(line == "a longer line");
}

TEST_CASE( "line end offsets 4MIX", "[string_like]" ) {
    std::string string("foo\nbar\rbaz\r\n\r\n\r\na longer line\nthe end");
    std::vector<Size> line_end_offsets = find_line_end_offsets(string);
    std::string line = std::string(string_view_for_line(string, line_end_offsets, 6));
    REQUIRE(line == "a longer line");
}

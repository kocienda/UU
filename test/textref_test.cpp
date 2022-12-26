//
// textref_test.cpp
//
#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "TextRef parsing function 1", "[textref]" ) {
    TextRef t = TextRef::from_string("/u/src/iota/search-tool.cpp:285:5:8:std::string");
    REQUIRE(t.index() == TextRef::Invalid);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == 5);
    REQUIRE(t.extent() == 8);
    REQUIRE(t.message() == "std::string");
}

TEST_CASE( "TextRef parsing function 2", "[textref]" ) {
    TextRef t = TextRef::from_string("7) /u/src/iota/search-tool.cpp:285:5:8:std::string");
    REQUIRE(t.index() == 7);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == 5);
    REQUIRE(t.extent() == 8);
    REQUIRE(t.message() == "std::string");
}

TEST_CASE( "TextRef parsing function 3", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:5:8");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == 5);
    REQUIRE(t.extent() == 8);
    REQUIRE(t.message() == "");
}

TEST_CASE( "TextRef parsing function 4", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == TextRef::Invalid);
    REQUIRE(t.extent() == TextRef::Invalid);
    REQUIRE(t.message() == "std::string");
}

TEST_CASE( "TextRef parsing function 5", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:33 std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == TextRef::Invalid);
    REQUIRE(t.extent() == TextRef::Invalid);
    REQUIRE(t.message() == "33 std::string");
}

TEST_CASE( "TextRef parsing function 6", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:10:33 std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == 10);
    REQUIRE(t.extent() == TextRef::Invalid);
    REQUIRE(t.message() == "33 std::string");
}

TEST_CASE( "TextRef parsing function 7", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:10:4:33 std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == 10);
    REQUIRE(t.extent() == 4);
    REQUIRE(t.message() == "33 std::string");
}

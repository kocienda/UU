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
    REQUIRE(t.span() == Span<size_t>(5,8));
    REQUIRE(t.message() == "std::string");
}

TEST_CASE( "TextRef parsing function 2", "[textref]" ) {
    TextRef t = TextRef::from_string("7) /u/src/iota/search-tool.cpp:285:5:8:std::string");
    REQUIRE(t.index() == 7);
    REQUIRE(t.line() == 285);
    REQUIRE(t.span() == Span<size_t>(5,8));
    REQUIRE(t.message() == "std::string");
}

TEST_CASE( "TextRef parsing function 3", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:5:8");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.span() == Span<size_t>(5,8));
    REQUIRE(t.message() == "");
}

TEST_CASE( "TextRef parsing function 4", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == TextRef::Invalid);
    REQUIRE(t.span().is_empty());
    REQUIRE(t.message() == "std::string");
}

TEST_CASE( "TextRef parsing function 5", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:33 std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == TextRef::Invalid);
    REQUIRE(t.span().is_empty());
    REQUIRE(t.message() == "33 std::string");
}

TEST_CASE( "TextRef parsing function 6", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:10:33 std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == 10);
    REQUIRE(t.span() == Span<size_t>(10));
    REQUIRE(t.message() == "33 std::string");
}

TEST_CASE( "TextRef parsing function 7", "[textref]" ) {
    TextRef t = TextRef::from_string("17) /u/src/iota/search-tool.cpp:285:10:14:33 std::string");
    REQUIRE(t.index() == 17);
    REQUIRE(t.line() == 285);
    REQUIRE(t.column() == 10);
    REQUIRE(t.span() == Span<size_t>(10, 14));
    REQUIRE(t.message() == "33 std::string");
}

TEST_CASE( "TextRef parsing function 8", "[textref]" ) {
    TextRef t = TextRef::from_string("81) search-tool.cpp:291:42..48:                std::string str = ref.to_string()");
    REQUIRE(t.index() == 81);
    REQUIRE(t.filename() == std::string("search-tool.cpp"));
    REQUIRE(t.line() == 291);
    REQUIRE(t.column() == 42);
    REQUIRE(t.span() == Span<size_t>(42, 48));
    REQUIRE(t.message() == "                std::string str = ref.to_string()");
}

TEST_CASE( "TextRef parsing function 9", "[textref]" ) {
    TextRef t = TextRef::from_string("81) search-tool.cpp:291:42..48,56:                std::string str = ref.to_string()");
    REQUIRE(t.index() == 81);
    REQUIRE(t.filename() == std::string("search-tool.cpp"));
    REQUIRE(t.line() == 291);
    REQUIRE(t.column() == 42);
    REQUIRE(t.span() == Span<size_t>("42..48,56"));
    REQUIRE(t.message() == "                std::string str = ref.to_string()");
}

TEST_CASE( "TextRef parsing function 10", "[textref]" ) {
    TextRef t = TextRef::from_string("81) search-tool.cpp:291");
    REQUIRE(t.index() == 81);
    REQUIRE(t.filename() == std::string("search-tool.cpp"));
    REQUIRE(t.line() == 291);
}

TEST_CASE( "TextRef parsing function 11", "[textref]" ) {
    TextRef t = TextRef::from_string("2) /System/Volumes/Data/Projects/work/Project 4/control.c:309:18:* Function Name: ProcessAll");
    REQUIRE(t.index() == 2);
    REQUIRE(t.filename() == std::string("/System/Volumes/Data/Projects/work/Project 4/control.c"));
    REQUIRE(t.line() == 309);
}


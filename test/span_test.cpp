//
// span_test.cpp
//
#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "Span test", "[span]" ) {
    Span<long> span;
    REQUIRE(span.is_empty());
    span.add(1);
    REQUIRE(span.contains(1));
    REQUIRE_FALSE(span.contains(0));
    span.add("2..5");
    REQUIRE(span.contains(1));
    REQUIRE(span.contains(3));
    REQUIRE(span.contains(5));
    REQUIRE_FALSE(span.contains(0));
    span.add(0, 10);
    REQUIRE(span.contains(5));
    REQUIRE(span.contains(9));
    REQUIRE(span.contains(10));
    REQUIRE_FALSE(span.contains(-1));
}

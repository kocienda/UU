//
// spread_test.cpp
//
#include <UU/UU.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "Spread test", "[spread]" ) {
    Spread<long> spread;
    REQUIRE(spread.is_empty());
    spread.add(1);
    REQUIRE(spread.contains(1));
    REQUIRE_FALSE(spread.contains(0));
    spread.add("2..5");
    REQUIRE(spread.contains(1));
    REQUIRE(spread.contains(3));
    REQUIRE(spread.contains(5));
    REQUIRE_FALSE(spread.contains(0));
    spread.add(0, 10);
    REQUIRE(spread.contains(5));
    REQUIRE(spread.contains(9));
    REQUIRE(spread.contains(10));
    REQUIRE_FALSE(spread.contains(-1));
}

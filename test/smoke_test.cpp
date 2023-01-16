//
// smoke_test.cpp
//

#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <UU/UU.h>
#include <UU/Types.h>

#include <catch2/catch_test_macros.hpp>

using namespace UU;

TEST_CASE( "smoke test", "[smoke]" ) {
    CHECK(true);
}

TEST_CASE( "smoke test fail", "[smoke]" ) {
    CHECK_FALSE(false);
}

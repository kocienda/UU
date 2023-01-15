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
    String str = "& world!!";
    Size s = EOF;
    std::cout << "val: " << s << std::endl;
    std::cout << "eq: " << ((s == SizeMax) ? "Y" : "N") << std::endl;

     std::cout << std::format("Hello {0} : {1}", s, str);

    CHECK(false);
}

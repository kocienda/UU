//
// scratch.cpp
//

#include "UU/UUText.h"
#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <UU/UU.h>
#include <UU/Types.h>

using namespace UU;

int main(int argc, const char *argv[]) {

    std::cout << static_cast<int>(UTF8TextEncodingTraits::TextEncodingV) << std::endl;
    std::cout << sizeof(UTF8TextEncodingTraits::BOM) << std::endl;
    std::cout << sizeof(UTF8TextEncodingTraits::CharT) << std::endl;
    std::cout << sizeof(UTF8TextEncodingTraits::CodePointT) << std::endl;

    return 0;
} 
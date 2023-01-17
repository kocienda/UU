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
using namespace UU::TextEncoding;

int main(int argc, const char *argv[]) {

    std::cout << static_cast<int>(UTF8Traits::FormV) << std::endl;
    std::cout << sizeof(UTF8Traits::BOM) << std::endl;
    std::cout << sizeof(UTF8Traits::CharT) << std::endl;
    std::cout << sizeof(UTF8Traits::CodePointT) << std::endl;

    return 0;
} 
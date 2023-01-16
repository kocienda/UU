//
// scratch.cpp
//

#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <UU/UU.h>
#include <UU/Types.h>

using namespace UU;

static Size test(Size len, Size bpos) noexcept { 
    Size bmax = len - bpos - 1;
    if (LIKELY(bmax < len)) {
        return std::min(bmax + 1, 4UL);
    }
    return SizeMax;
}

int main(int argc, const char *argv[]) {
    std::cout << test(10, 1) << std::endl;
    return 0;
} 
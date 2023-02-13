//
// scratch.cpp
//

#include <bit>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

#include <UU/UU.h>

#include <UU/IString.h>

using namespace UU;

int main(int argc, const char *argv[]) 
{
    LOG_CHANNEL_ON(General);
    LOG_CHANNEL_ON(Memory);

    IString s1;
    s1.append(std::string("1234567890123456789012345678901234567890"));
    std::cout << "s1: " << s1 << std::endl;

    IString s2;
    std::swap(s1, s2);
    std::cout << "s2: " << s2 << std::endl;


    return 0;
} 
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

    ProtoIString<char> s;
    s.append(std::string("foo"));
    std::cout << "size: " << sizeof(s) << std::endl;
    std::cout << "string: " << s << std::endl;

    return 0;
} 
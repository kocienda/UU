//
// scratch.cpp
//

#include <bit>
#include <iostream>
#include <limits>
#include <type_traits>

#include <UU/UU.h>

using namespace UU;

struct Trivial {
    int foo;
    double bar;
};

struct NonTrivial {
    NonTrivial() {}
    ~NonTrivial() {}
    void *baz;
};

template <typename T>
static void assign_array(ArrayForm<T> &dst, const ArrayForm<T> &src) {
    dst = src;
}

int main(int argc, const char *argv[]) 
{
    LOG_CHANNEL_ON(General);
    LOG_CHANNEL_ON(Memory);

    std::cout << std::boolalpha;
    std::cout << std::is_trivial<Trivial>::value << '\n';
    std::cout << std::is_trivial<NonTrivial>::value << '\n';

    Array<int, 4> a1;
    Array<int, 4> a1a;
    Array<int, 6> a1b;

    a1b.push_back(200);
    a1b.push_back(201);

    std::cout << "==================================================" << std::endl;
    std::cout << "a1 elements" << std::endl;

    for (int i = 0; i < 10; i++) {
        a1.push_back(i);
    }
    for (int i = 10; i < 20; i++) {
        a1.emplace_back(i);
    }

    for (const auto it : a1) {
        std::cout << it << std::endl;
    }
    std::cout << "a1a size: " << a1a.size() << std::endl;

    std::cout << "==================================================" << std::endl;
    std::cout << "a1a swapped elements" << std::endl;

    a1a.swap(a1);
    for (const auto it : a1a) {
        std::cout << it << std::endl;
    }
    std::cout << "a1 size: " << a1.size() << std::endl;

    std::cout << "==================================================" << std::endl;
    std::cout << "a1 reassigned elements" << std::endl;

    a1 = a1a;
    a1a.clear();
    for (const auto it : a1) {
        std::cout << it << std::endl;
    }
    std::cout << "a1a size: " << a1a.size() << std::endl;

    std::cout << "==================================================" << std::endl;
    std::cout << "a1 inserted from a1b" << std::endl;

    a1.insert(a1.end(), a1b.begin(), a1b.end());

    for (const auto it : a1) {
        std::cout << it << std::endl;
    }

    std::cout << "==================================================" << std::endl;
    std::cout << "a1b assign to a1" << std::endl;

    assign_array(a1, a1b);

    for (const auto it : a1) {
        std::cout << it << std::endl;
    }

    std::cout << "==================================================" << std::endl;
    std::cout << "a2 elements" << std::endl;

    Array<char, 4> a2;
    for (int i = 0; i < 10; i++) {
        a2.push_back('0' + i);
    }

    for (const auto it : a2) {
        std::cout << it << std::endl;
    }

    std::cout << "ArraySizeType<int>: " << sizeof(ArraySizeType<int>{}) << std::endl;
    std::cout << "ArraySizeType<char>: " << sizeof(ArraySizeType<char>{}) << std::endl;


    return 0;
} 
//
// scratch.cpp
//

#include <pthread.h>

#include <cstdio>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <UU/UU.h>
#include <UU/UUText.h>
#include <UU/Types.h>

using namespace UU;
using namespace UU::TextEncoding;

template <std::integral T>
static constexpr T byteswap(T value) noexcept {
    static_assert(std::has_unique_object_representations_v<T>,  "T may not have padding bits");
    auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    std::ranges::reverse(value_representation);
    return std::bit_cast<T>(value_representation);
}

int main(int argc, const char *argv[]) {

    LOG_CHANNEL_ON(General);
    LOG_CHANNEL_ON(Memory);

    // BitBlock b;
    // b.set(0);
    // b.set(1);
    // b.set(3);
    // b.reset();
    // std::cout << "get[0]: " << b.get(0) << std::endl;
    // std::cout << "get[1]: " << b.get(1) << std::endl;

    // auto t = b.take();
    // if (t.ok) {
    //     std::cout << "take: " << t.idx << std::endl;
    // }

    // // b.set_all();
    // std::cout << "get[2]: " << b.get(2) << std::endl;
    // t = b.take();
    // if (t.ok) {
    //     std::cout << "take: " << t.idx << std::endl;
    // }
    // else {
    //     std::cout << "!!!full: " << std::endl;
    // }

    // Stretch<int> s(1, 10);
    // s.add(1, 10);
    // s.add(12, 20);
    // s.simplify();
    // auto it = s.begin();
    // for (auto it = s.begin(); it != s.end(); ++it) {
    //     std::cout << "v: " << *it << std::endl;
    // }

    // for (auto it = s.rbegin(); it != s.rend(); ++it) {
    //     std::cout << "v: " << *it << std::endl;
    // }

    Spread<int> p(1, 10);
    p.add(12, 20);
    for (auto it = p.begin(); it != p.end(); ++it) {
        std::cout << "p: " << *it << std::endl;
    }
    // std::cout << "---" << std::endl;
    // for (auto it = p.rbegin(); it != p.rend(); ++it) {
    //     std::cout << "p: " << *it << std::endl;
    // }

    // 
    // std::cout << "contains 1: " << s.contains(1) << std::endl;
    // std::cout << "contains 10: " << s.contains(10) << std::endl;
    // std::cout << "contains 11: " << s.contains(11) << std::endl;

    // pthread_attr_t attr;
    // Size stack_size;
    // int rc = pthread_attr_getstacksize(&attr, &stack_size);
    // std::cout << "stack_size: " << rc << ": " << stack_size << std::endl;

    // Mallocator mallocator;
    // Memory mem = mallocator.alloc(32);
    // mallocator.dealloc(mem);

    // using FreelistAllocator = Freelist<StackAllocator<16384>, 256, 128, 128, 64>;
    // using Allocator = StatsAllocator<FallbackAllocator<FreelistAllocator, Mallocator>>;
    // Allocator allocator;

    // Memory mem0 = allocator.alloc(384);
    // allocator.dealloc(mem0);
    // Memory mem1 = allocator.alloc(128);
    // Memory mem2 = allocator.alloc(256);
    // allocator.dealloc(mem1);
    // allocator.dealloc(mem2);
    // Memory mem3 = allocator.alloc(256);
    // // allocator.dealloc(mem2);
    // allocator.dealloc(mem3);

    // std::cout << allocator.stats() << std::endl;

    // std::cout << static_cast<int>(UTF8Traits::FormV) << std::endl;
    // std::cout << sizeof(UTF8Traits::BOM) << std::endl;
    // std::cout << sizeof(UTF8Traits::CharT) << std::endl;
    // std::cout << sizeof(UTF8Traits::CodePointT) << std::endl;

    // UInt32 u = 0xA0B0;
    // UInt32 u = 1 << 12;
    // u++;
    // std::cout << u << std::endl;

    // Byte cp1 = (u & 0xff00) >> 8;
    // Byte cp2 = (u & 0xff);

    // std::cout << (int)cp1 << std::endl;
    // std::cout << (int)cp2 << std::endl;

    // UInt32 t = byteswap(u);
    // std::cout << t << std::endl;

    // Byte *uptr = reinterpret_cast<Byte *>(&u);
    // Byte *tptr = reinterpret_cast<Byte *>(&t);

    // std::cout << (int)uptr[0] << std::endl;
    // std::cout << (int)uptr[1] << std::endl;
    // std::cout << (int)uptr[2] << std::endl;
    // std::cout << (int)uptr[3] << std::endl;
    // std::cout << "==========" << std::endl;
    // std::cout << (int)tptr[0] << std::endl;
    // std::cout << (int)tptr[1] << std::endl;
    // std::cout << (int)tptr[2] << std::endl;
    // std::cout << (int)tptr[3] << std::endl;

    // UInt16 s = 0x80;
    // std::cout << "0b";
    // for (int idx = 0; idx < 16; idx++) {
    //     const char *b = (s & 0x1) ? "1" : "0";
    //     std::cout << b;
    //     s = s >> 1;
    // }
    // std::cout << std::endl;


    return 0;
} 
//
// scratch.cpp
//

#include <bit>

#include <atomic>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <UU/UU.h>

using namespace UU;
// using namespace UU::TextEncoding;

int main(int argc, const char *argv[]) {

    LOG_CHANNEL_ON(General);
    LOG_CHANNEL_ON(Memory);

    using TAllocator = Mallocator;
    // using TAllocator = FallbackAllocator<CascadingAllocator<BlockAllocator<256, 65, 128>>, Mallocator>;

    // TAllocator tallocator;
    GPAllocator tallocator;

    constexpr int LOOPS = 100000;
    constexpr int COUNT = 1000;
    constexpr Size SIZE = 32;
    Memory mem[COUNT];
    for (int loops = 0; loops < LOOPS; loops++) {
        time_check_mark(0);
        for (int c = 0; c < COUNT; c++) {
            mem[c] = tallocator.alloc(SIZE);
        }
        time_check_done(0);
        time_check_mark(1);
        for (int c = 0; c < COUNT; c++) {
            tallocator.dealloc(mem[c]);
        }
        time_check_done(1);
    }
    printf("alloc:   %0.9f\n", time_check_elapsed_seconds(0));
    printf("dealloc: %0.9f\n", time_check_elapsed_seconds(1));

    // UInt32 u1 = 72; //0b00000000000000000000000000000000;
    
    // for (UInt32 i = 0; i < 520; i += 8) {
    //     int z = std::min(8, 32 - std::countl_zero(i >> 5));
    //     std::cout << "bucket: " << i << " => " << z << std::endl;
    // }
    
    // for (UInt32 i = 0; i < 12; i ++) {
    //     int z = i & 0b111;
    //     std::cout << "mask: " << i << " => " << z << std::endl;
    // }



    // UInt64 u1 = UInt64Max - 1;
    // UInt32 u1 = UInt32Max;
    // int z = UU::countr_one(u1);
    // int p = UU::popcount(u1);
    // std::cout << "countr_one: " << u1 << " => " << z << std::endl;
    // std::cout << "popcount: " << u1 << " => " << p << std::endl;
    
    // BitBlock<4> b;
    // constexpr Size count = 200;
    // for (int i = 0; i < count; i++) {
    //     UInt32 t = b.take();
    //     std::cout << "t: " << t << std::endl;
    // }

    // b.set(0);
    // b.set(71);
    // b.set(3);
    // // b.reset();
    // std::cout << "test[0]: " << b.test(0) << std::endl;
    // std::cout << "test[71]: " << b.test(71) << std::endl;
    // std::cout << "test[72]: " << b.test(72) << std::endl;
    // std::cout << "take: " << b.take() << std::endl;
    // std::cout << "take: " << b.take() << std::endl;
    // std::cout << "take: " << b.take() << std::endl;

    // b.set_all();
    // std::cout << "get[0]: " << b.get(0) << std::endl;

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

    // Spread<int> p(1, 10);
    // p.add(12, 20);
    // for (auto it = p.begin(); it != p.end(); ++it) {
    //     std::cout << "p: " << *it << std::endl;
    // }
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

    // using FreelistAllocator = Freelist<StackAllocator<16384>, 128, 128>;
    // using Allocator = StatsAllocator<
    //     FallbackAllocator<FreelistAllocator, Mallocator>>
    // ;
    // Allocator allocator;

    // CascadingAllocator<BlockAllocator<64, 65, 128>> callocator;
    // constexpr Size count = 200;
    // Memory mem[count];
    // for (int i = 0; i < count; i++) {
    //     mem[i] = callocator.alloc(72);
    // }
    // for (int i = count - 1; i >= 0; i--) {
    //     callocator.dealloc(mem[i]);
    // }
    

    // using Size1Allocator = FallbackAllocator<FallbackAllocator<Freelist<StackAllocator<16384>, 64, 256>, BlockAllocator<2048, 0, 64>>, Mallocator>;
    // using Size2Allocator = FallbackAllocator<CascadingAllocator<BlockAllocator<256, 65, 128>>, Mallocator>;
    // using Size3Allocator = FallbackAllocator<CascadingAllocator<BlockAllocator<256, 129, 256>>, Mallocator>;
    // using Size4Allocator = FallbackAllocator<CascadingAllocator<BlockAllocator<256, 257, 384>>, Mallocator>;
    // using Size5Allocator = FallbackAllocator<CascadingAllocator<BlockAllocator<256, 385, 512>>, Mallocator>;
    // using Size6Allocator = FallbackAllocator<CascadingAllocator<BlockAllocator<256, 513, 1024>>, Mallocator>;
    // using Allocator = StatsAllocator<
    //     Segregator<64, Size1Allocator, 
    //     Segregator<128, Size2Allocator, 
    //     Segregator<256, Size3Allocator, 
    //     Segregator<384, Size4Allocator, 
    //     Segregator<512, Size4Allocator, 
    //     Segregator<1024, Size5Allocator, 
    //     Mallocator>>>>>>>;
    
    // Allocator allocator;

    // Memory mem0 = allocator.alloc(32);
    // allocator.dealloc(mem0);
    // Memory mem1 = allocator.alloc(24);
    // Memory mem2 = allocator.alloc(256);
    // allocator.dealloc(mem1);
    // Memory mem3 = allocator.alloc(256);
    // allocator.dealloc(mem2);
    // allocator.dealloc(mem3);

    // using Allocator = StatsAllocator<Mallocator>;

    // constexpr Size count = 384;
    // Memory mem4[count];
    // for (int i = 0; i < count; i++) {
    //     Allocator &allocator = Context::get().allocator();
    //     mem4[i] = allocator.alloc(256);
    // }
    // for (int i = 0; i < count; i++) {
    //     Allocator &allocator = Context::get().allocator();
    //     allocator.dealloc(mem4[i]);
    // }
    // for (int i = 0; i < 32; i++) {
    //     mem4[i] = allocator.alloc(256);
    // }
    // allocator.alloc(50000);

    // Allocator &allocator = Context::get().allocator();
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
//
// Context.h
//
// MIT License
// Copyright (c) 2022-2023 Ken Kocienda. All rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef UU_CONTEXT_H
#define UU_CONTEXT_H

#include <vector>

#include <UU/Allocator.h>
#include <UU/SmallVector.h>

namespace UU {

using Allocator = StatsAllocator<Mallocator>;
// using Allocator = Mallocator;

// using Size1Allocator = FallbackAllocator<FallbackAllocator<Freelist<StackAllocator<16384>, 64, 256>, BlockAllocator<2048, 0, 64>>, Mallocator>;
// using Size1Allocator = FallbackAllocator<CascadingAllocator<BlockAllocator<2048, 0, 64>>, Mallocator>;
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

class Context
{
public:
    static void init();
    static Context &get();

    Context() {}
    Allocator &allocator() { return m_allocator; }

private:
    Allocator m_allocator;
};



}  // namespace UU

#endif  // UU_CONTEXT_H

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

using Allocator = GPAllocator;
// using Allocator = Mallocator;
// using Allocator = StatsAllocator<Mallocator>;

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

//
// Array.cpp
//
// Based (mightily) on SmallVector.cpp. Its license is as follows:
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//
// Changes made by me are:
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

#include "Assertions.h"
#include "Array.h"

namespace UU {

// Check that no bytes are wasted and everything is well-aligned.
namespace {
struct Struct16B { alignas(16) void *X; };
struct Struct32B { alignas(32) void *X; };
}  // namespace

static_assert(sizeof(Array<void *, 0>) == sizeof(unsigned) * 2 + sizeof(void *), "wasted space in Array size 0");
static_assert(alignof(Array<Struct16B, 0>) >= alignof(Struct16B), "wrong alignment for 16-byte aligned T");
static_assert(alignof(Array<Struct32B, 0>) >= alignof(Struct32B), "wrong alignment for 32-byte aligned T");
static_assert(sizeof(Array<Struct16B, 0>) >= alignof(Struct16B), "missing padding for 16-byte aligned T");
static_assert(sizeof(Array<Struct32B, 0>) >= alignof(Struct32B), "missing padding for 32-byte aligned T");
static_assert(sizeof(Array<void *, 1>) == sizeof(unsigned) * 2 + sizeof(void *) * 2, "wasted space in Array size 1");
static_assert(sizeof(Array<char, 0>) == sizeof(void *) * 2 + sizeof(void *), "1 byte elements have word-sized type for size and capacity");

// Report that min_size doesn't fit into the size type for this array.
[[noreturn]] static void report_size_overflow(size_t min_size, size_t max_size);
static void report_size_overflow(size_t min_size, size_t max_size) 
{
    std::string reason = "Array unable to grow. Requested capacity (" + 
        std::to_string(min_size) + 
        ") is larger than maximum value for size type (" +
        std::to_string(max_size) + 
        ")";
    ASSERT_WITH_MESSAGE(false, "%s", reason.c_str());
    CRASH();
}

// Report that the array is at maximum capacity.
[[noreturn]] static void report_at_maximum_capacity(size_t max_size);
static void report_at_maximum_capacity(size_t max_size) 
{
    ASSERT_WITH_MESSAGE(false, "Array at maximum capacity: %lu", max_size);
    CRASH();
}

// Note: Moving this function into the header may cause performance regression.
template <class SizeT>
static SizeT calculate_new_capacity(SizeT min_size, SizeT t_size, SizeT old_capacity) 
{
    constexpr SizeT max_size = std::numeric_limits<SizeT>::max();

    // Ensure we can fit the new capacity.
    // This is only going to be applicable when the capacity is 32 bit.
    if (min_size > max_size) {
        report_size_overflow(min_size, max_size);
    }

    // Ensure we can meet the guarantee of space for at least one more element.
    // The above check alone will not catch the case where grow is called with a
    // default min_size of 0, but the current capacity cannot be increased.
    // This is only going to be applicable when the capacity is 32 bit.
    if (old_capacity == max_size) {
        report_at_maximum_capacity(max_size);
    }

    // In theory 2*old_capacity can overflow if the capacity is 64 bit, but the
    // old capacity would never be large enough for this to be a problem.
    SizeT new_capacity = (2 * old_capacity) + 1; // Always grow.
    return std::min(std::max(new_capacity, min_size), max_size);
}

// Note: Moving this function into the header may cause performance regression.
template <class SizeT>
void *ArrayBase<SizeT>::allocate_for_grow(SizeT min_size, SizeT t_size, SizeT &new_capacity) 
{
    new_capacity = calculate_new_capacity(min_size, t_size, this->capacity());
    return malloc(new_capacity * t_size);
}

// Note: Moving this function into the header may cause performance regression.
template <class ArraySizeType>
void ArrayBase<ArraySizeType>::grow_pod(void *first_element, ArraySizeType min_size, ArraySizeType t_size) 
{
    ArraySizeType new_capacity = calculate_new_capacity(min_size, t_size, this->capacity());
    void *base;

    if (this->base() == first_element) {
        base = malloc(new_capacity * t_size);

        // Copy the elements over.  No need to run dtors on PODs.
        memcpy(base, this->base(), size() * t_size);
    } 
    else {
        // If this wasn't grown from the inline copy, grow the allocated space.
        base = realloc(this->base(), new_capacity * t_size);
    }

    set_base(base);
    set_capacity(new_capacity);
}

template class UU::ArrayBase<uint32_t>;

// Disable the uint64_t instantiation for 32-bit builds.
// Both uint32_t and uint64_t instantiations are needed for 64-bit builds.
// This instantiation will never be used in 32-bit builds, and will cause
// warnings when sizeof(SizeT) > sizeof(size_t).
#if SIZE_MAX > UINT32_MAX
template class UU::ArrayBase<uint64_t>;
// Assertions to ensure this #if stays in sync with ArraySizeType.
static constexpr size_t ArrayCharSizeType = sizeof(ArraySizeType<char>);
static_assert(ArrayCharSizeType == sizeof(uint64_t), "ArrayBase<uint64_t> should be in use.");
#else
static_assert(ArrayCharSizeType == sizeof(uint32_t), "ArrayBase<uint32_t> should be in use.");
#endif

}  // namespace UU
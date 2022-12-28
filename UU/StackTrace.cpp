//
// StackTrace.cpp
//
// Based (substantially) on StackTrace.cpp from WebKit.
// Copyright (c) 2022 Ken Kocienda. All rights reserved.
//
/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
 * Copyright (C) 2017 Yusuke Suzuki <utatane.tea@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>

#include "Assertions.h"
#include "MathLike.h"
#include "StackTrace.h"

void UUGetBacktrace(void** stack, int* size)
{
    *size = backtrace(stack, *size);
}

namespace UU {

size_t StackTrace::instanceSize(int capacity)
{
    ASSERT(capacity >= 1);
    return sizeof(StackTrace) + (capacity - 1) * sizeof(void*);
}

std::unique_ptr<StackTrace> StackTrace::captureStackTrace(int maxFrames, int framesToSkip)
{
    maxFrames = UUMaxT(int, 1, maxFrames);
    size_t sizeToAllocate = instanceSize(maxFrames);
    std::unique_ptr<StackTrace> trace(new (malloc(sizeToAllocate)) StackTrace());

    // Skip 2 additional frames i.e. StackTrace::captureStackTrace and UUGetBacktrace.
    framesToSkip += 0;
    int numberOfFrames = maxFrames + framesToSkip;

    UUGetBacktrace(&trace->m_skippedFrame0, &numberOfFrames);
    if (numberOfFrames) {
        trace->m_size = numberOfFrames - framesToSkip;
    }
    else {
        trace->m_size = 0;
    }

    trace->m_capacity = maxFrames;

    return trace;
}

StackTrace::DemangleEntry StackTrace::demangle(void *pc)
{
    const char* mangledName = nullptr;
    const char* cxaDemangled = nullptr;
    Dl_info info;
    if (dladdr(pc, &info) && info.dli_sname) {
        mangledName = info.dli_sname;
    }
    if (mangledName) {
        int status = 0;
        cxaDemangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
    }
    if (mangledName || cxaDemangled) {
        return DemangleEntry { mangledName, cxaDemangled };
    }
    return DemangleEntry { mangledName, nullptr };
}

void StackTrace::dump(const char *indent_string) const
{
    const auto *stack = this->stack();
    char **symbols = backtrace_symbols(stack, m_size);
    if (!symbols) {
        return;
    }

    if (!indent_string) {
        indent_string = "";
    }
    for (int i = 0; i < m_size; ++i) {
        const char* mangledName = nullptr;
        const char* cxaDemangled = nullptr;
        auto demangled = demangle(stack[i]);
        mangledName = demangled.mangledName();
        cxaDemangled = demangled.demangledName();
        const int frameNumber = i + 1;
        if (mangledName || cxaDemangled) {
            fprintf(stderr, "%s%-3d %p %s\n", indent_string, frameNumber, stack[i], cxaDemangled ? cxaDemangled : mangledName);
        }
        else {
            fprintf(stderr, "%s%-3d %p\n", indent_string, frameNumber, stack[i]);
        }
    }
}

}  // namespace UU

//
//  Assertions.cpp
//
//  Based (substantially) on Assertions.cpp from WebKit.
//
/*
 * Copyright (C) 2003-2019 Apple Inc.  All rights reserved.
 * Copyright (C) 2007-2009 Torch Mobile, Inc.
 * Copyright (C) 2011 University of Szeged. All rights reserved.
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

#include <stdio.h>

#include <vector>
#include <string>

#include "Assertions.h"
#include "StackTrace.h"
#include "Stringy.h"

namespace UU {

static std::string create_with_format_and_arguments(const char *format, va_list args)
{
    std::string result;

    va_list argsCopy;
    va_copy(argsCopy, args);

    char ch;
    int rc = vsnprintf(&ch, 1, format, args);

    if (!rc) {
        va_end(argsCopy);
        return std::string();
    }
    if (rc < 0) {
        va_end(argsCopy);
        return std::string();
    }

    std::vector<char> buffer;
    unsigned length = rc;
    buffer.reserve(length + 1);

    // Now do the formatting again, guaranteed to fit.
    vsnprintf(buffer.data(), buffer.size(), format, argsCopy);
    va_end(argsCopy);

    result = std::string(buffer.data(), length);
    return result;
}

}  // namespace UU

extern "C" {

static void log_to_stderr(const char *buffer)
{
    fputs(buffer, stderr);
}

static void vprintf_stderr_common(const char *format, va_list args)
{
    if (strstr(format, "%@")) {
        std::string str = UU::create_with_format_and_arguments(format, args);
        log_to_stderr(str.c_str());
        return;
    }
    vfprintf(stderr, format, args);
}

static void vprintf_stderr_with_prefix(const char *prefix, const char *format, va_list args)
{
    size_t prefix_length = strlen(prefix);
    size_t format_length = strlen(format);
    std::vector<char> format_with_prefix;
    format_with_prefix.reserve(prefix_length + format_length + 1);
    memcpy(format_with_prefix.data(), prefix, prefix_length);
    memcpy(format_with_prefix.data() + prefix_length, format, format_length);
    format_with_prefix[prefix_length + format_length] = 0;
    vprintf_stderr_common(format_with_prefix.data(), args);
}

static void vprintf_stderr_with_trailing_newline(const char *format, va_list args)
{
    size_t format_length = strlen(format);
    if (format_length && format[format_length - 1] == '\n') {
        vprintf_stderr_common(format, args);
        return;
    }
    std::vector<char> format_with_newline;
    format_with_newline.reserve(format_length + 2);
    memcpy(format_with_newline.data(), format, format_length);
    format_with_newline[format_length] = '\n';
    format_with_newline[format_length + 1] = 0;
    vprintf_stderr_common(format_with_newline.data(), args);
}

static void printf_stderr_common(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf_stderr_common(format, args);
    va_end(args);
}

static void print_call_site(const char *file, int line, const char *function)
{
    printf_stderr_common("%s:%d:%s\n", file, line, function);
}

void UULogEnable(UULogChannel *channel)
{
    channel->state = UULogChannelOn;
}

void UULogDisable(UULogChannel *channel)
{
    channel->state = UULogChannelOff;
}

void UUReportNotImplementedYet(const char *file, int line, const char *function)
{
    printf_stderr_common("NOT IMPLEMENTED YET\n");
    print_call_site(file, line, function);
}

void UUReportAssertionFailure(const char *file, int line, const char *function, const char *assertion)
{
    if (assertion) {
        printf_stderr_common("ASSERTION FAILED: %s\n", assertion);
    }
    else {
        printf_stderr_common("SHOULD NEVER BE REACHED\n");
    }
    print_call_site(file, line, function);
}

void UUReportAssertionFailureWithMessage(const char *file, int line, const char *function, const char *assertion, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf_stderr_with_prefix("ASSERTION FAILED: ", format, args);
    va_end(args);
    printf_stderr_common("\n%s\n", assertion);
    print_call_site(file, line, function);
}

void UUReportArgumentAssertionFailure(const char *file, int line, const char *function, const char *argName, const char *assertion)
{
    printf_stderr_common("ARGUMENT BAD: %s, %s\n", argName, assertion);
    print_call_site(file, line, function);
}

void UUCrash()
{
    UUReportBacktrace();
    *(int *)(uintptr_t)0xbbadbeef = 0;
    // More reliable, but doesn't say BBADBEEF.
    __builtin_trap();
}

void UUReportBacktrace()
{
    static constexpr int framesToShow = 31;
    static constexpr int framesToSkip = 4;
    void* samples[framesToShow + framesToSkip];
    int frames = framesToShow + framesToSkip;

    UUGetBacktrace(samples, &frames);
    UUPrintBacktrace(samples + framesToSkip, frames - framesToSkip);
}

void UUPrintBacktrace(void** stack, int size)
{
    UU::StackTrace stackTrace(stack, size);
    stackTrace.dump();
}

void UUReportFatalError(const char *file, int line, const char *function, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf_stderr_with_prefix("FATAL ERROR: ", format, args);
    va_end(args);
    printf_stderr_common("\n");
    print_call_site(file, line, function);
}

void UUReportError(const char *file, int line, const char *function, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf_stderr_with_prefix("ERROR: ", format, args);
    va_end(args);
    printf_stderr_common("\n");
    print_call_site(file, line, function);
}

static void UULogVaList(UULogChannel *channel, const char *format, va_list args)
{
    if (channel->state == UULogChannelOff) {
        return;
    }
    vprintf_stderr_with_trailing_newline(format, args);
}

void UULog(UULogChannel *channel, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    UULogVaList(channel, format, args);
    va_end(args);
}

void UULogVerbose(const char *file, int line, const char *function, UULogChannel *channel, const char *format, ...)
{
    if (channel->state != UULogChannelOn) {
        return;
    }
    
    va_list args;
    va_start(args, format);
    UULogVaList(channel, format, args);
    va_end(args);

    print_call_site(file, line, function);
}

void UULogAlwaysV(const char *format, va_list args)
{
    vprintf_stderr_with_trailing_newline(format, args);
}

void UULogAlways(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    UULogAlwaysV(format, args);
    va_end(args);
}

void UULogAlwaysAndCrash(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    UULogAlwaysV(format, args);
    va_end(args);
    CRASH();
}

}  // extern "C"

#if LOG_DISABLED
#else
UU_LOG_CHANNELS(DEFINE_LOG_CHANNEL)
#endif

//
// Assertions.h
//
// Based (substantially) on Assertions.h from WebKit.
// Copyright (c) 2022 Ken Kocienda. All rights reserved.
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

#ifndef UU_ASSERTIONS_H
#define UU_ASSERTIONS_H

#include <stdarg.h>
#include <stdlib.h>

// =========================================================================================================================================

#ifdef NDEBUG
#define ASSERTIONS_DISABLED_DEFAULT 1
#else
#define ASSERTIONS_DISABLED_DEFAULT 0
#endif

#ifndef ASSERT_DISABLED
#define ASSERT_DISABLED ASSERTIONS_DISABLED_DEFAULT
#endif

#ifndef ASSERT_MSG_DISABLED
#define ASSERT_MSG_DISABLED ASSERTIONS_DISABLED_DEFAULT
#endif

#ifndef ASSERT_ARG_DISABLED
#define ASSERT_ARG_DISABLED ASSERTIONS_DISABLED_DEFAULT
#endif

#ifndef FATAL_DISABLED
#define FATAL_DISABLED ASSERTIONS_DISABLED_DEFAULT
#endif

#ifndef ERROR_DISABLED
#define ERROR_DISABLED ASSERTIONS_DISABLED_DEFAULT
#endif

#ifndef LOG_DISABLED
#define LOG_DISABLED ASSERTIONS_DISABLED_DEFAULT
#endif

// =========================================================================================================================================

#if !defined(NO_RETURN)
#define NO_RETURN __attribute((__noreturn__))
#endif

#define NO_RETURN_DUE_TO_CRASH NO_RETURN

// =========================================================================================================================================

#ifndef UU_ALWAYS_INLINE
#ifdef NDEBUG
#define UU_ALWAYS_INLINE inline __attribute__ ((__visibility__("hidden"), __always_inline__))
#else
#define UU_ALWAYS_INLINE inline
#endif
#endif

#ifndef UU_NEVER_INLINE
#ifdef NDEBUG
#define UU_NEVER_INLINE __attribute__ ((__visibility__("default"), noinline))
#else
#define UU_NEVER_INLINE
#endif
#endif

// =========================================================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { UULogChannelOff, UULogChannelOn } UULogChannelState;

typedef struct {
    UULogChannelState state;
    const char *name;
} UULogChannel;

#ifndef LOG_CHANNEL_PREFIX
#define LOG_CHANNEL_PREFIX Log
#endif

#define LOG_CHANNEL(name) JOIN_LOG_CHANNEL_WITH_PREFIX(LOG_CHANNEL_PREFIX, name)
#define LOG_CHANNEL_ADDRESS(name) &LOG_CHANNEL(name),
#define JOIN_LOG_CHANNEL_WITH_PREFIX(prefix, channel) JOIN_LOG_CHANNEL_WITH_PREFIX_LEVEL_2(prefix, channel)
#define JOIN_LOG_CHANNEL_WITH_PREFIX_LEVEL_2(prefix, channel) prefix ## channel

#define DECLARE_LOG_CHANNEL(name) \
    extern UULogChannel LOG_CHANNEL(name);

#if !defined(DEFINE_LOG_CHANNEL)
#define DEFINE_LOG_CHANNEL(name) \
    UULogChannel LOG_CHANNEL(name) = { UULogChannelOff, #name };
#endif

void UULogEnable(UULogChannel *);
void UULogDisable(UULogChannel *);
void UUReportNotImplementedYet(const char *file, int line, const char *function);
void UUReportAssertionFailure(const char *file, int line, const char *function, const char *assertion);
void UUReportAssertionFailureWithMessage(const char *file, int line, const char *function, const char *assertion, const char *format, ...);
void UUReportArgumentAssertionFailure(const char *file, int line, const char *function, const char *argName, const char *assertion);
void UUReportFatalError(const char *file, int line, const char *function, const char *format, ...);
void UUReportError(const char *file, int line, const char *function, const char *format, ...);
void UULogVerbose(const char *file, int line, const char *function, UULogChannel *channel, const char *format, ...);
void UULogAlwaysV(const char *format, va_list args);
void UULogAlways(const char *format, ...);
void UULogAlwaysAndCrash(const char *format, ...);
void UULog(UULogChannel *, const char *format, ...);
void UUGetBacktrace(void **stack, int *size);
void UUReportBacktrace(void);
void UUPrintBacktrace(void** stack, int size);

#if LOG_DISABLED
#define LOG_CHANNEL_ON(channel) ((void)0)
#else
#define LOG_CHANNEL_ON(channel) UULogEnable(&LOG_CHANNEL(channel))
#endif

#if LOG_DISABLED
#define LOG_CHANNEL_OFF(channel) ((void)0)
#else
#define LOG_CHANNEL_OFF(channel) UULogDisable(&LOG_CHANNEL(channel))
#endif

#include <UU/Platform.h>

#if CPU(X86_64) || CPU(X86)
#define UUBreakpointTrap()  asm volatile ("int3")
#elif CPU(ARM_THUMB2)
#define UUBreakpointTrap()  asm volatile ("bkpt #0")
#elif CPU(ARM64)
#define UUBreakpointTrap()  asm volatile ("brk #0")
#else
#define UUBreakpointTrap() UUCrash() // Not implemented.
#endif

#ifndef CRASH

#if defined(NDEBUG)
#define CRASH() do { \
    UUBreakpointTrap(); \
    __builtin_unreachable(); \
} while (0)
#else
#define CRASH() UUCrash()
#endif

#endif // !defined(CRASH)

NO_RETURN_DUE_TO_CRASH void UUCrash(void);

#if LOG_DISABLED
#else
#define UU_LOG_CHANNELS(M) \
M(General) \
M(Error) \
M(Leak) \
M(Memory) \

UU_LOG_CHANNELS(DECLARE_LOG_CHANNEL)
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#define PRETTY_FUNCTION __PRETTY_FUNCTION__

#if LOG_DISABLED
#define LOG(channel, ...) ((void)0)
#define LOG_DEBUG(...) ((void)0)
#else
#define LOG(channel, ...) UULog(&LOG_CHANNEL(channel), __VA_ARGS__)
#define LOG_DEBUG(...) UUDebug(__VA_ARGS__)
#endif

// =========================================================================================================================================

#if ASSERT_DISABLED

#define ASSERT(assertion) ((void)0)
#define ASSERT_AT(assertion, file, line, function) ((void)0)
#define ASSERT_NOT_REACHED() ((void)0)
#define NO_RETURN_DUE_TO_ASSERT

#else  // ASSERT_DISABLED

#define ASSERT(assertion) do { \
    if (!(assertion)) { \
        UUReportAssertionFailure(__FILE__, __LINE__, PRETTY_FUNCTION, #assertion); \
        CRASH(); \
    } \
} while (0)

#define ASSERT_AT(assertion, file, line, function) do { \
    if (!(assertion)) { \
        UUReportAssertionFailure(file, line, function, #assertion); \
        CRASH(); \
    } \
} while (0)

#define ASSERT_NOT_REACHED() do { \
    UUReportAssertionFailure(__FILE__, __LINE__, PRETTY_FUNCTION, 0); \
    CRASH(); \
} while (0)

#define NO_RETURN_DUE_TO_ASSERT NO_RETURN_DUE_TO_CRASH

#endif  // ASSERT_DISABLED

/* ASSERT_WITH_MESSAGE */

#if ASSERT_MSG_DISABLED
#define ASSERT_WITH_MESSAGE(assertion, ...) ((void)0)
#else
#define ASSERT_WITH_MESSAGE(assertion, ...) do { \
    if (!(assertion)) { \
        UUReportAssertionFailureWithMessage(__FILE__, __LINE__, PRETTY_FUNCTION, #assertion, __VA_ARGS__); \
        CRASH(); \
    } \
} while (0)
#endif

// FATAL

#if FATAL_DISABLED
#define FATAL(...) ((void)0)
#else
#define FATAL(...) do { \
    UUReportFatalError(__FILE__, __LINE__, PRETTY_FUNCTION, __VA_ARGS__); \
    CRASH(); \
} while (0)
#endif

// LOG_ERROR

#if ERROR_DISABLED
#define LOG_ERROR(...) ((void)0)
#else
#define LOG_ERROR(...) UUReportError(__FILE__, __LINE__, PRETTY_FUNCTION, __VA_ARGS__)
#endif

// LOG

#if LOG_DISABLED
#define LOG_NORMAL(channel, ...) ((void)0)
#else
#define LOG_NORMAL(channel, ...) UULog(&LOG_CHANNEL(channel), __VA_ARGS__)
#endif

// LOG_VERBOSE

#if LOG_DISABLED
#define LOG_VERBOSE(channel, ...) ((void)0)
#else
#define LOG_VERBOSE(channel, ...) UULogVerbose(__FILE__, __LINE__, PRETTY_FUNCTION, &LOG_CHANNEL(channel), __VA_ARGS__)
#endif

// =========================================================================================================================================

#endif // UU_ASSERTIONS_H
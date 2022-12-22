//
//  Platform.h
//
//  Based on Platform.h from WebKit.
//  Provides support for building a single codebase on different platforms and CPUs.
//
/*
 * Copyright (C) 2006-2017 Apple Inc. All rights reserved.
 * Copyright (C) 2007-2009 Torch Mobile, Inc.
 * Copyright (C) 2010, 2011 Research In Motion Limited. All rights reserved.
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

#ifndef UU_PLATFORM_H
#define UU_PLATFORM_H

/* Include compiler specific macros */
#include <UU/Compiler.h>

/* ==== PLATFORM handles OS, operating environment, graphics API, and
   CPU. This macro will be phased out in favor of platform adaptation
   macros, policy decision macros, and top-level port definitions. ==== */
#define PLATFORM(UU_FEATURE) (defined UU_PLATFORM_##UU_FEATURE  && UU_PLATFORM_##UU_FEATURE)


/* ==== Platform adaptation macros: these describe properties of the target environment. ==== */

/* CPU() - the target CPU architecture */
#define CPU(UU_FEATURE) (defined UU_CPU_##UU_FEATURE  && UU_CPU_##UU_FEATURE)
/* HAVE() - specific system features (headers, functions or similar) that are present or not */
#define HAVE(UU_FEATURE) (defined HAVE_##UU_FEATURE  && HAVE_##UU_FEATURE)
/* OS() - underlying operating system; only to be used for mandated low-level services like 
   virtual memory, not to choose a GUI toolkit */
#define OS(UU_FEATURE) (defined UU_OS_##UU_FEATURE  && UU_OS_##UU_FEATURE)


/* ==== Policy decision macros: these define policy choices for a particular port. ==== */

/* USE() - use a particular third-party library or optional OS service */
#define USE(UU_FEATURE) (defined USE_##UU_FEATURE  && USE_##UU_FEATURE)
/* ENABLE() - turn on a specific feature of WebKit */
#define ENABLE(UU_FEATURE) (defined ENABLE_##UU_FEATURE  && ENABLE_##UU_FEATURE)


/* CPU() - the target CPU architecture */
#define CPU(UU_FEATURE) (defined UU_CPU_##UU_FEATURE  && UU_CPU_##UU_FEATURE)

/* ==== CPU() - the target CPU architecture ==== */
/* CPU(KNOWN) becomes true if we explicitly support a target CPU. */

/* CPU(MIPS) - MIPS 32-bit and 64-bit */
#if (defined(mips) || defined(__mips__) || defined(MIPS) || defined(_MIPS_) || defined(__mips64))
#if defined(_ABI64) && (_MIPS_SIM == _ABI64)
#define UU_CPU_MIPS64 1
#define UU_MIPS_ARCH __mips64
#else
#define UU_CPU_MIPS 1
#define UU_MIPS_ARCH __mips
#endif
#define UU_CPU_KNOWN 1
#define UU_MIPS_PIC (defined __PIC__)
#define UU_MIPS_ISA(v) (defined UU_MIPS_ARCH && UU_MIPS_ARCH == v)
#define UU_MIPS_ISA_AT_LEAST(v) (defined UU_MIPS_ARCH && UU_MIPS_ARCH >= v)
#define UU_MIPS_ARCH_REV __mips_isa_rev
#define UU_MIPS_ISA_REV(v) (defined UU_MIPS_ARCH_REV && UU_MIPS_ARCH_REV == v)
#define UU_MIPS_ISA_REV_AT_LEAST(v) (defined UU_MIPS_ARCH_REV && UU_MIPS_ARCH_REV >= v)
#define UU_MIPS_DOUBLE_FLOAT (defined __mips_hard_float && !defined __mips_single_float)
#define UU_MIPS_FP64 (defined __mips_fpr && __mips_fpr == 64)
#endif

/* CPU(LOONGARCH64) - LOONGARCH64 */
#if defined(__loongarch64)
#define UU_CPU_LOONGARCH64 1
#define UU_CPU_KNOWN 1
#endif

/* CPU(PPC64) - PowerPC 64-bit Big Endian */
#if (  defined(__ppc64__)      \
    || defined(__PPC64__))     \
    && defined(__BYTE_ORDER__) \
    && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define UU_CPU_PPC64 1
#define UU_CPU_KNOWN 1
#endif

/* CPU(PPC64LE) - PowerPC 64-bit Little Endian */
#if (   defined(__ppc64__)     \
    || defined(__PPC64__)      \
    || defined(__ppc64le__)    \
    || defined(__PPC64LE__))   \
    && defined(__BYTE_ORDER__) \
    && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define UU_CPU_PPC64LE 1
#define UU_CPU_KNOWN 1
#endif

/* CPU(PPC) - PowerPC 32-bit */
#if (  defined(__ppc__)        \
    || defined(__PPC__)        \
    || defined(__powerpc__)    \
    || defined(__powerpc)      \
    || defined(__POWERPC__)    \
    || defined(_M_PPC)         \
    || defined(__PPC))         \
    && !CPU(PPC64)             \
    && CPU(BIG_ENDIAN)
#define UU_CPU_PPC 1
#define UU_CPU_KNOWN 1
#endif

/* CPU(X86) - i386 / x86 32-bit */
#if   defined(__i386__) \
    || defined(i386)     \
    || defined(_M_IX86)  \
    || defined(_X86_)    \
    || defined(__THW_INTEL)
#define UU_CPU_X86 1
#define UU_CPU_KNOWN 1

#if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#define UU_CPU_X86_SSE2 1
#endif

#endif

/* CPU(X86_64) - AMD64 / Intel64 / x86_64 64-bit */
#if   defined(__x86_64__) \
    || defined(_M_X64)
#define UU_CPU_X86_64 1
#define UU_CPU_X86_SSE2 1
#define UU_CPU_KNOWN 1
#endif

/* CPU(ARM64) */
#if defined(__arm64__) || defined(__aarch64__)
#define UU_CPU_ARM64 1
#define UU_CPU_KNOWN 1

#if defined(__arm64e__)
#define UU_CPU_ARM64E 1
#endif
#endif

/* CPU(ARM) - ARM, any version*/
#define UU_ARM_ARCH_AT_LEAST(N) (CPU(ARM) && UU_ARM_ARCH_VERSION >= N)

#if   defined(arm) \
    || defined(__arm__) \
    || defined(ARM) \
    || defined(_ARM_)
#define UU_CPU_ARM 1
#define UU_CPU_KNOWN 1

#if defined(__ARM_PCS_VFP)
#define UU_CPU_ARM_HARDFP 1
#endif

/* Set UU_ARM_ARCH_VERSION */
#if   defined(__ARM_ARCH_4__) \
    || defined(__ARM_ARCH_4T__) \
    || defined(__MARM_ARMV4__)
#define UU_ARM_ARCH_VERSION 4

#elif defined(__ARM_ARCH_5__) \
    || defined(__ARM_ARCH_5T__) \
    || defined(__MARM_ARMV5__)
#define UU_ARM_ARCH_VERSION 5

#elif defined(__ARM_ARCH_5E__) \
    || defined(__ARM_ARCH_5TE__) \
    || defined(__ARM_ARCH_5TEJ__)
#define UU_ARM_ARCH_VERSION 5

#elif defined(__ARM_ARCH_6__) \
    || defined(__ARM_ARCH_6J__) \
    || defined(__ARM_ARCH_6K__) \
    || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6ZK__) \
    || defined(__ARM_ARCH_6T2__) \
    || defined(__ARMV6__)
#define UU_ARM_ARCH_VERSION 6

#elif defined(__ARM_ARCH_7A__) \
    || defined(__ARM_ARCH_7K__) \
    || defined(__ARM_ARCH_7R__) \
    || defined(__ARM_ARCH_7S__)
#define UU_ARM_ARCH_VERSION 7

#elif defined(__ARM_ARCH_8__) \
    || defined(__ARM_ARCH_8A__)
#define UU_ARM_ARCH_VERSION 8

/* MSVC sets _M_ARM */
#elif defined(_M_ARM)
#define UU_ARM_ARCH_VERSION _M_ARM

/* RVCT sets _TARGET_ARCH_ARM */
#elif defined(__TARGET_ARCH_ARM)
#define UU_ARM_ARCH_VERSION __TARGET_ARCH_ARM

#else
#define UU_ARM_ARCH_VERSION 0

#endif

/* FIXME: UU_THUMB_ARCH_VERSION seems unused. Remove. */
/* Set UU_THUMB_ARCH_VERSION */
#if   defined(__ARM_ARCH_4T__)
#define UU_THUMB_ARCH_VERSION 1

#elif defined(__ARM_ARCH_5T__) \
    || defined(__ARM_ARCH_5TE__) \
    || defined(__ARM_ARCH_5TEJ__)
#define UU_THUMB_ARCH_VERSION 2

#elif defined(__ARM_ARCH_6J__) \
    || defined(__ARM_ARCH_6K__) \
    || defined(__ARM_ARCH_6Z__) \
    || defined(__ARM_ARCH_6ZK__) \
    || defined(__ARM_ARCH_6M__)
#define UU_THUMB_ARCH_VERSION 3

#elif defined(__ARM_ARCH_6T2__) \
    || defined(__ARM_ARCH_7__) \
    || defined(__ARM_ARCH_7A__) \
    || defined(__ARM_ARCH_7K__) \
    || defined(__ARM_ARCH_7M__) \
    || defined(__ARM_ARCH_7R__) \
    || defined(__ARM_ARCH_7S__)
#define UU_THUMB_ARCH_VERSION 4

/* RVCT sets __TARGET_ARCH_THUMB */
#elif defined(__TARGET_ARCH_THUMB)
#define UU_THUMB_ARCH_VERSION __TARGET_ARCH_THUMB

#else
#define UU_THUMB_ARCH_VERSION 0
#endif


/* FIXME: CPU(ARMV5_OR_LOWER) seems unused. Remove. */
/* CPU(ARMV5_OR_LOWER) - ARM instruction set v5 or earlier */
/* On ARMv5 and below the natural alignment is required. 
   And there are some other differences for v5 or earlier. */
#if !defined(ARMV5_OR_LOWER) && !UU_ARM_ARCH_AT_LEAST(6)
#define UU_CPU_ARMV5_OR_LOWER 1
#endif


/* CPU(ARM_TRADITIONAL) - Thumb2 is not available, only traditional ARM (v4 or greater) */
/* CPU(ARM_THUMB2) - Thumb2 instruction set is available */
/* Only one of these will be defined. */
#if !defined(UU_CPU_ARM_TRADITIONAL) && !defined(UU_CPU_ARM_THUMB2)
#  if defined(thumb2) || defined(__thumb2__) \
    || ((defined(__thumb) || defined(__thumb__)) && UU_THUMB_ARCH_VERSION == 4)
#    define UU_CPU_ARM_TRADITIONAL 0
#    define UU_CPU_ARM_THUMB2 1
#  elif UU_ARM_ARCH_AT_LEAST(4)
#    define UU_CPU_ARM_TRADITIONAL 1
#    define UU_CPU_ARM_THUMB2 0
#  else
#    error "Not supported ARM architecture"
#  endif
#elif CPU(ARM_TRADITIONAL) && CPU(ARM_THUMB2) /* Sanity Check */
#  error "Cannot use both of UU_CPU_ARM_TRADITIONAL and UU_CPU_ARM_THUMB2 platforms"
#endif /* !defined(UU_CPU_ARM_TRADITIONAL) && !defined(UU_CPU_ARM_THUMB2) */

#if defined(__ARM_NEON__) && !defined(UU_CPU_ARM_NEON)
#define UU_CPU_ARM_NEON 1
#endif

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
#define UU_CPU_ARM_VFP 1
#endif

/* If CPU(ARM_NEON) is not enabled, we'll conservatively assume only VFP2 or VFPv3D16
   support is available. Hence, only the first 16 64-bit floating point registers
   are available. See:
   NEON registers: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473c/CJACABEJ.html
   VFP2 and VFP3 registers: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473c/CIHDIBDG.html
   NEON to VFP register mapping: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473c/CJAIJHFC.html
*/
#if CPU(ARM_NEON)
#define UU_CPU_ARM_VFP_V3_D32 1
#else
#define UU_CPU_ARM_VFP_V2 1
#endif

#if defined(__ARM_ARCH_7K__)
#define UU_CPU_APPLE_ARMV7K 1
#endif

#if defined(__ARM_ARCH_7S__)
#define UU_CPU_APPLE_ARMV7S 1
#endif

#endif /* ARM */

/* CPU(RISCV64) - RISC-V 64-bit */
#if    defined(__riscv) \
    && defined(__riscv_xlen) \
    && (__riscv_xlen == 64)
#define UU_CPU_RISCV64 1
#define UU_CPU_KNOWN 1
#endif

#if !CPU(KNOWN)
#define UU_CPU_UNKNOWN 1
#endif

#if CPU(ARM) || CPU(MIPS) || CPU(RISCV64) || CPU(UNKNOWN)
#define UU_CPU_NEEDS_ALIGNED_ACCESS 1
#endif

#if COMPILER(GCC_COMPATIBLE)
/* __LP64__ is not defined on 64bit Windows since it uses LLP64. Using __SIZEOF_POINTER__ is simpler. */
#if __SIZEOF_POINTER__ == 8
#define UU_CPU_ADDRESS64 1
#elif __SIZEOF_POINTER__ == 4
#define UU_CPU_ADDRESS32 1
#else
#error "Unsupported pointer width"
#endif
#elif COMPILER(MSVC)
#if defined(_WIN64)
#define UU_CPU_ADDRESS64 1
#else
#define UU_CPU_ADDRESS32 1
#endif
#else
/* This is the most generic way. But in OS(DARWIN), Platform.h can be included by sandbox definition file (.sb).
 * At that time, we cannot include "stdint.h" header. So in the case of known compilers, we use predefined constants instead. */
#include <stdint.h>
#if UINTPTR_MAX > UINT32_MAX
#define UU_CPU_ADDRESS64 1
#else
#define UU_CPU_ADDRESS32 1
#endif
#endif

/* CPU general purpose register width. */
#if !defined(UU_CPU_REGISTER64) && !defined(UU_CPU_REGISTER32)
#if CPU(ADDRESS64) || CPU(ARM64)
#define UU_CPU_REGISTER64 1
#else
#define UU_CPU_REGISTER32 1
#endif
#endif

/* CPU(BIG_ENDIAN) or CPU(MIDDLE_ENDIAN) or neither, as appropriate. */

#if COMPILER(GCC_COMPATIBLE)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define UU_CPU_BIG_ENDIAN 1
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define UU_CPU_LITTLE_ENDIAN 1
#elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#define UU_CPU_MIDDLE_ENDIAN 1
#else
#error "Unknown endian"
#endif
#else
#if defined(WIN32) || defined(_WIN32)
/* Windows only have little endian architecture. */
#define UU_CPU_LITTLE_ENDIAN 1
#else
#include <sys/types.h>
#if __has_include(<endian.h>)
#include <endian.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#define UU_CPU_BIG_ENDIAN 1
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#define UU_CPU_LITTLE_ENDIAN 1
#elif __BYTE_ORDER == __PDP_ENDIAN
#define UU_CPU_MIDDLE_ENDIAN 1
#else
#error "Unknown endian"
#endif
#else
#if __has_include(<machine/endian.h>)
#include <machine/endian.h>
#else
#include <sys/endian.h>
#endif
#if BYTE_ORDER == BIG_ENDIAN
#define UU_CPU_BIG_ENDIAN 1
#elif BYTE_ORDER == LITTLE_ENDIAN
#define UU_CPU_LITTLE_ENDIAN 1
#elif BYTE_ORDER == PDP_ENDIAN
#define UU_CPU_MIDDLE_ENDIAN 1
#else
#error "Unknown endian"
#endif
#endif
#endif
#endif

#if !CPU(LITTLE_ENDIAN) && !CPU(BIG_ENDIAN)
#error "Unsupported endian"
#endif

/* ==== OS() - underlying operating system; only to be used for mandated low-level services like 
   virtual memory, not to choose a GUI toolkit ==== */

/* OS(AIX) - AIX */
#ifdef _AIX
#define UU_OS_AIX 1
#endif

/* OS(DARWIN) - Any Darwin-based OS, including Mac OS X and iPhone OS */
#ifdef __APPLE__
#define UU_OS_DARWIN 1

#include <Availability.h>
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#endif

/* OS(IOS) - iOS */
/* OS(MAC_OS_X) - Mac OS X (not including iOS) */
#if OS(DARWIN) && ((defined(TARGET_OS_EMBEDDED) && TARGET_OS_EMBEDDED) \
    || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)                 \
    || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR))
#define UU_OS_IOS 1
#elif OS(DARWIN) && defined(TARGET_OS_MAC) && TARGET_OS_MAC
#define UU_OS_MAC_OS_X 1
#endif

/* OS(FREEBSD) - FreeBSD */
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)
#define UU_OS_FREEBSD 1
#endif

/* OS(HURD) - GNU/Hurd */
#ifdef __GNU__
#define UU_OS_HURD 1
#endif

/* OS(LINUX) - Linux */
#ifdef __linux__
#define UU_OS_LINUX 1
#endif

/* OS(NETBSD) - NetBSD */
#if defined(__NetBSD__)
#define UU_OS_NETBSD 1
#endif

/* OS(OPENBSD) - OpenBSD */
#ifdef __OpenBSD__
#define UU_OS_OPENBSD 1
#endif

/* OS(SOLARIS) - Solaris */
#if defined(sun) || defined(__sun)
#define UU_OS_SOLARIS 1
#endif

/* OS(WINDOWS) - Any version of Windows */
#if defined(WIN32) || defined(_WIN32)
#define UU_OS_WINDOWS 1
#endif

#define UU_OS_WIN ERROR "USE WINDOWS WITH OS NOT WIN"
#define UU_OS_MAC ERROR "USE MAC_OS_X WITH OS NOT MAC"

/* OS(UNIX) - Any Unix-like system */
#if    OS(AIX)              \
    || OS(DARWIN)           \
    || OS(FREEBSD)          \
    || OS(HURD)             \
    || OS(LINUX)            \
    || OS(NETBSD)           \
    || OS(OPENBSD)          \
    || OS(SOLARIS)          \
    || defined(unix)        \
    || defined(__unix)      \
    || defined(__unix__)
#define UU_OS_UNIX 1
#endif

/* Operating environments */

/* Export macro support. Detects the attributes available for shared library symbol export
   decorations. */
#if OS(WINDOWS) || (COMPILER_HAS_CLANG_DECLSPEC(dllimport) && COMPILER_HAS_CLANG_DECLSPEC(dllexport))
#define USE_DECLSPEC_ATTRIBUTE 1
#define USE_VISIBILITY_ATTRIBUTE 0
#elif defined(__GNUC__)
#define USE_DECLSPEC_ATTRIBUTE 0
#define USE_VISIBILITY_ATTRIBUTE 1
#else
#define USE_DECLSPEC_ATTRIBUTE 0
#define USE_VISIBILITY_ATTRIBUTE 0
#endif

/* Standard libraries */
#if defined(HAVE_FEATURES_H) && HAVE_FEATURES_H
/* If the included features.h is glibc's one, __GLIBC__ is defined. */
#include <features.h>
#endif

/* FIXME: these are all mixes of OS, operating environment and policy choices. */
/* PLATFORM(GTK) */
/* PLATFORM(MAC) */
/* PLATFORM(IOS) */
/* PLATFORM(IOS_SIMULATOR) */
/* PLATFORM(WIN) */
/* PLATFORM(TEENSY) */
#if defined(BUILDING_GTK__)
#define UU_PLATFORM_GTK 1
#elif defined(BUILDING_WPE__)
#define UU_PLATFORM_WPE 1
#elif defined(BUILDING_JSCONLY__)
/* JSCOnly does not provide PLATFORM() macro */
#elif OS(MAC_OS_X)
#define UU_PLATFORM_MAC 1
#elif OS(IOS)
#define UU_PLATFORM_IOS 1
#if defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR
#define UU_PLATFORM_IOS_SIMULATOR 1
#endif
#elif OS(WINDOWS)
#define UU_PLATFORM_WIN 1
#elif defined(TEENSYDUINO)
#define UU_PLATFORM_TEENSY 1
#endif

#if defined(SDK_OS_FREE_RTOS)
#define UU_PLATFORM_FREE_RTOS 1
#endif

/* PLATFORM(COCOA) */
#if PLATFORM(MAC) || PLATFORM(IOS)
#define UU_PLATFORM_COCOA 1
#endif

#if PLATFORM(MAC)

/* OS X defines a series of platform macros for debugging. */
/* Some of them are really annoying because they use common names (e.g. check()). */
/* Disable those macros so that we are not limited in how we name methods and functions. */
#undef __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0

#endif /* PLATFORM(MAC) */

/* FIXME: Remove after CMake build enabled on Darwin */
#if OS(DARWIN)
#define HAVE_ERRNO_H 1
#define HAVE_LANGINFO_H 1
#define HAVE_LOCALTIME_R 1
#define HAVE_MMAP 1
#define HAVE_REGEX_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_STAT_BIRTHTIME 1
#define HAVE_STRINGS_H 1
#define HAVE_STRNSTR 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_TIME_H 1 
#define HAVE_TM_GMTOFF 1
#define HAVE_TM_ZONE 1
#define HAVE_TIMEGM 1

#if CPU(X86_64) || CPU(ARM64)
#define HAVE_INT128_T 1
#endif
#endif /* OS(DARWIN) */

#if OS(UNIX)
#define USE_PTHREADS 1
#endif /* OS(UNIX) */

#if OS(DARWIN)
#define HAVE_DISPATCH_H 1
#define HAVE_MADV_FREE 1
#define HAVE_MADV_FREE_REUSE 1
#define HAVE_MADV_DONTNEED 1
#define HAVE_MERGESORT 1
#define HAVE_PTHREAD_SETMONIKER_NP 1
#define HAVE_READLINE 1
#define HAVE_SYS_TIMEB_H 1

#endif /* OS(DARWIN) */

#if OS(DARWIN) || ((OS(FREEBSD) || defined(__GLIBC__)) && (CPU(X86) || CPU(X86_64) || CPU(ARM) || CPU(ARM64) || CPU(MIPS)))
#define HAVE_MACHINE_CONTEXT 1
#endif

#if OS(DARWIN) || (OS(LINUX) && defined(__GLIBC__) && !defined(__UCLIBC__))
#define HAVE_BACKTRACE 1
#endif

#if OS(DARWIN) || OS(LINUX)
#if PLATFORM(GTK)
#if defined(__GLIBC__) && !defined(__UCLIBC__)
#define HAVE_BACKTRACE_SYMBOLS 1
#endif
#endif /* PLATFORM(GTK) */
#define HAVE_DLADDR 1
#endif /* OS(DARWIN) || OS(LINUX) */

#if USE(EXPORT_MACROS)

#if !PLATFORM(WIN)
#define UU_EXPORT UU_EXPORT
#define UU_TESTSUPPORT_EXPORT UU_EXPORT
#else
// Windows must set this per-project
#endif

#else // !USE(EXPORT_MACROS)
#define UU_EXPORT
#define UU_TESTSUPPORT_EXPORT

#endif // USE(EXPORT_MACROS)

#if !defined(USE_UU_VALUE_64) && !defined(USE_UU_VALUE_32_64)
#if (CPU(X86_64) && !defined(__ILP32__) && (OS(UNIX) || OS(WINDOWS))) \
    || (CPU(IA64) && !CPU(IA64_32)) \
    || CPU(ALPHA) \
    || CPU(ARM64) \
    || CPU(S390X) \
    || CPU(MIPS64) \
    || CPU(PPC64) \
    || CPU(PPC64LE)
#define USE_UU_VALUE_64 1
#else
#define USE_UU_VALUE_32_64 1
#endif
#endif /* !defined(USE_UU_VALUE_64) && !defined(USE_UU_VALUE_32_64) */

#if !defined(USE_UU_64_BIT_FLOATS) && !defined(USE_UU_32_BIT_FLOATS)
#if (CPU(X86_64) && !defined(__ILP32__) && (OS(UNIX) || OS(WINDOWS))) \
    || (CPU(IA64) && !CPU(IA64_32)) \
    || CPU(ALPHA) \
    || CPU(ARM64) \
    || CPU(S390X) \
    || CPU(MIPS64) \
    || CPU(PPC64) \
    || CPU(PPC64LE)
#define USE_UU_64_BIT_FLOATS 1
#else
#define USE_UU_32_BIT_FLOATS 1
#endif
#endif /* !defined(USE_UU_64_BIT_FLOATS) && !defined(USE_UU_32_BIT_FLOATS) */

#endif // UU_PLATFORM_H

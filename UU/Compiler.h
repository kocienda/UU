//
// Compiler.h
//
// Based (substantially) on Compiler.h from WebKit.
// Support for building a single codebase with different compilers.
// Copyright (c) 2022 Ken Kocienda. All rights reserved.
//
/*
 * Copyright (C) 2011, 2012, 2014 Apple Inc. All rights reserved.
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

#ifndef UU_COMPILER_H
#define UU_COMPILER_H

/* COMPILER() - the compiler being used to build the project */
#define COMPILER(UU_FEATURE) (defined UU_COMPILER_##UU_FEATURE  && UU_COMPILER_##UU_FEATURE)

/* COMPILER_SNUBPORTS() - whether the compiler being used to build the project supports the given feature. */
#define COMPILER_SNUBPORTS(UU_COMPILER_FEATURE) (defined UU_COMPILER_SNUBPORTS_##UU_COMPILER_FEATURE  && UU_COMPILER_SNUBPORTS_##UU_COMPILER_FEATURE)

/* COMPILER_QUIRK() - whether the compiler being used to build the project requires a given quirk. */
#define COMPILER_QUIRK(UU_COMPILER_QUIRK) (defined UU_COMPILER_QUIRK_##UU_COMPILER_QUIRK  && UU_COMPILER_QUIRK_##UU_COMPILER_QUIRK)

/* COMPILER_HAS_CLANG_BUILTIN() - whether the compiler supports a particular clang builtin. */
#ifdef __has_builtin
#define COMPILER_HAS_CLANG_BUILTIN(x) __has_builtin(x)
#else
#define COMPILER_HAS_CLANG_BUILTIN(x) 0
#endif

/* COMPILER_HAS_CLANG_HEATURE() - whether the compiler supports a particular language or library feature. */
/* http://clang.llvm.org/docs/LanguageExtensions.html#has-feature-and-has-extension */
#ifdef __has_feature
#define COMPILER_HAS_CLANG_FEATURE(x) __has_feature(x)
#else
#define COMPILER_HAS_CLANG_FEATURE(x) 0
#endif

/* COMPILER_HAS_CLANG_DECLSPEC() - whether the compiler supports a Microsoft style __declspec attribute. */
/* https://clang.llvm.org/docs/LanguageExtensions.html#has-declspec-attribute */
#ifdef __has_declspec_attribute
#define COMPILER_HAS_CLANG_DECLSPEC(x) __has_declspec_attribute(x)
#else
#define COMPILER_HAS_CLANG_DECLSPEC(x) 0
#endif

/* ==== COMPILER() - primary detection of the compiler being used to build the project, in alphabetical order ==== */

/* COMPILER(CLANG) - Clang  */

#if defined(__clang__)
#define UU_COMPILER_CLANG 1
#define UU_COMPILER_SNUBPORTS_BLOCKS COMPILER_HAS_CLANG_FEATURE(blocks)
#define UU_COMPILER_SNUBPORTS_C_STATIC_ASSERT COMPILER_HAS_CLANG_FEATURE(c_static_assert)
#define UU_COMPILER_SNUBPORTS_CXX_REFERENCE_QUALIFIED_FUNCTIONS COMPILER_HAS_CLANG_FEATURE(cxx_reference_qualified_functions)
#define UU_COMPILER_SNUBPORTS_FALLTHROUGH_WARNINGS COMPILER_HAS_CLANG_FEATURE(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define UU_COMPILER_SNUBPORTS_CXX_EXCEPTIONS COMPILER_HAS_CLANG_FEATURE(cxx_exceptions)
#define UU_COMPILER_SNUBPORTS_BUILTIN_IS_TRIVIALLY_COPYABLE COMPILER_HAS_CLANG_FEATURE(is_trivially_copyable)

#endif // defined(__clang__)

/* COMPILER(GCC_OR_CLANG) - GNU Compiler Collection or Clang */
#if defined(__GNUC__)
#define UU_COMPILER_GCC_OR_CLANG 1
#endif

/* COMPILER(GCC) - GNU Compiler Collection */
/* Note: This section must come after the Clang section since we check !COMPILER(CLANG) here. */
#if COMPILER(GCC_OR_CLANG) && !COMPILER(CLANG)
#define UU_COMPILER_GCC 1
#define UU_COMPILER_SNUBPORTS_CXX_REFERENCE_QUALIFIED_FUNCTIONS 1

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define GCC_VERSION_AT_LEAST(major, minor, patch) (GCC_VERSION >= (major * 10000 + minor * 100 + patch))

#if !GCC_VERSION_AT_LEAST(4, 9, 0)
// test for a compiler of at least a certain version
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define UU_COMPILER_SNUBPORTS_C_STATIC_ASSERT 1
#endif

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#define __nodiscard__

#endif /* COMPILER(GCC) */

#ifndef __nodiscard__
#define __nodiscard__ [[nodiscard]]
#endif

#ifdef __cplusplus
#if __cplusplus <= 201103L
#define UU_CPP_STD_VER 11
#elif __cplusplus <= 201402L
#define UU_CPP_STD_VER 14
#elif __cplusplus <= 201703L
#define UU_CPP_STD_VER 17
#endif
#endif

#ifdef __cpp_exceptions
#define USE_EXCEPTIONS 1
#else
#define USE_EXCEPTIONS 0
#endif

#ifdef __cpp_rtti
#define USE_TYPEINFO 1
#else
#define USE_TYPEINFO 0
#endif

/* ==== Compiler-independent macros for various compiler features, in alphabetical order ==== */

/* ALIGN */

#if !defined(ALIGN)
#define ALIGN(N) __attribute__ ((aligned(N)))
#endif

/* ALLOW_UNUSED */

#if !defined(ALLOW_UNUSED)
#define ALLOW_UNUSED(x) (void)(x)
#endif

/* UU_ALWAYS_INLINE */

#if !defined(UU_ALWAYS_INLINE) && COMPILER(GCC_OR_CLANG) && defined(NDEBUG)
#define UU_ALWAYS_INLINE inline __attribute__((__always_inline__))
#endif

#if !defined(UU_ALWAYS_INLINE) && defined(NDEBUG)
#define UU_ALWAYS_INLINE __forceinline
#endif

#if !defined(UU_ALWAYS_INLINE)
#define UU_ALWAYS_INLINE inline
#endif

/* FALLTHROUGH */

#if !defined(FALLTHROUGH) && COMPILER_SNUBPORTS(FALLTHROUGH_WARNINGS) && COMPILER(CLANG)
#define FALLTHROUGH [[clang::fallthrough]]
#endif

#if !defined(FALLTHROUGH)
#define FALLTHROUGH
#endif

/* LIKELY */

#if !defined(LIKELY) && COMPILER(GCC_OR_CLANG)
#define LIKELY(x) __builtin_expect(!!(x), 1)
#endif

#if !defined(LIKELY)
#define LIKELY(x) (x)
#endif

/* UU_NEVER_INLINE */

#if !defined(UU_NEVER_INLINE) && COMPILER(GCC_OR_CLANG)
#define UU_NEVER_INLINE __attribute__((__noinline__))
#endif

#if !defined(UU_NEVER_INLINE) && COMPILER(MSVC)
#define UU_NEVER_INLINE __declspec(noinline)
#endif

#if !defined(UU_NEVER_INLINE)
#define UU_NEVER_INLINE
#endif

/* NO_RETURN */

#if !defined(NO_RETURN) && COMPILER(GCC_OR_CLANG)
#define NO_RETURN __attribute((__noreturn__))
#endif

#if !defined(NO_RETURN) && COMPILER(MSVC)
#define NO_RETURN __declspec(noreturn)
#endif

#if !defined(NO_RETURN)
#define NO_RETURN
#endif

/* RETURNS_NONNULL */
#if !defined(RETURNS_NONNULL) && COMPILER(GCC_OR_CLANG)
#define RETURNS_NONNULL __attribute__((returns_nonnull))
#endif

#if !defined(RETURNS_NONNULL)
#define RETURNS_NONNULL
#endif

/* NO_RETURN_WITH_VALUE */

#if !defined(NO_RETURN_WITH_VALUE) && !COMPILER(MSVC)
#define NO_RETURN_WITH_VALUE NO_RETURN
#endif

#if !defined(NO_RETURN_WITH_VALUE)
#define NO_RETURN_WITH_VALUE
#endif

/* PURE_FUNCTION */

#if !defined(PURE_FUNCTION) && COMPILER(GCC_OR_CLANG)
#define PURE_FUNCTION __attribute__((__pure__))
#endif

#if !defined(PURE_FUNCTION)
#define PURE_FUNCTION
#endif

/* UNUSED_FUNCTION */

#if !defined(UNUSED_FUNCTION) && COMPILER(GCC_OR_CLANG)
#define UNUSED_FUNCTION __attribute__((unused))
#endif

#if !defined(UNUSED_FUNCTION)
#define UNUSED_FUNCTION
#endif

/* REFERENCED_FROM_ASM */

#if !defined(REFERENCED_FROM_ASM) && COMPILER(GCC_OR_CLANG)
#define REFERENCED_FROM_ASM __attribute__((__used__))
#endif

#if !defined(REFERENCED_FROM_ASM)
#define REFERENCED_FROM_ASM
#endif

/* STATIC CONST, CONSTEXPR, INLINE */

#if !defined(STATIC_CONST_EXPLICIT) && COMPILER(GCC_OR_CLANG)
#define STATIC_CONST_EXPLICIT static const
#endif

#if !defined(STATIC_CONSTEXPR_EXPLICIT) && COMPILER(GCC_OR_CLANG)
#define STATIC_CONSTEXPR_EXPLICIT static constexpr
#endif

#if !defined(STATIC_CONSTEXPR) && COMPILER(GCC_OR_CLANG)
#define STATIC_CONSTEXPR STATIC_CONSTEXPR_EXPLICIT
#endif

#if !defined(STATIC_INLINE_CONSTEXPR_EXPLICIT) && COMPILER(GCC_OR_CLANG)
#define STATIC_INLINE_CONSTEXPR_EXPLICIT static inline constexpr
#endif

#if !defined(STATIC_INLINE_EXPLICIT) && COMPILER(GCC_OR_CLANG)
#define STATIC_INLINE_EXPLICIT static inline
#endif

#if !defined(STATIC_INLINE) && COMPILER(GCC_OR_CLANG)
#define STATIC_INLINE STATIC_INLINE_EXPLICIT
#endif

#if !defined(STATIC_CONST) && COMPILER(GCC_OR_CLANG)
#if __cplusplus
#define STATIC_CONST STATIC_CONSTEXPR_EXPLICIT
#else
#define STATIC_CONST STATIC_CONST_EXPLICIT
#endif  // __cplusplus
#endif

#if !defined(STATIC_INLINE_CONST) && COMPILER(GCC_OR_CLANG)
#if __cplusplus
#define STATIC_INLINE_CONST STATIC_INLINE_CONSTEXPR_EXPLICIT
#else
#define STATIC_INLINE_CONST STATIC_INLINE_EXPLICIT
#endif  // __cplusplus
#endif

/* UNLIKELY */

#if !defined(UNLIKELY) && COMPILER(GCC_OR_CLANG)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#endif

#if !defined(UNLIKELY)
#define UNLIKELY(x) (x)
#endif

/* UNUSED_LABEL */

/* Keep the compiler from complaining for a local label that is defined but not referenced. */
/* Helpful when mixing hand-written and autogenerated code. */

#if !defined(UNUSED_LABEL) && COMPILER(MSVC)
#define UNUSED_LABEL(label) if (false) goto label
#endif

#if !defined(UNUSED_LABEL)
#define UNUSED_LABEL(label) UNUSED_PARAM(&& label)
#endif

/* UNUSED_PARAM */

#if !defined(UNUSED_PARAM) && COMPILER(MSVC)
#define UNUSED_PARAM(variable) (void)&variable
#endif

#if !defined(UNUSED_PARAM)
#define UNUSED_PARAM(variable) (void)variable
#endif

/* WARN_UNUSED_RETURN */

#if !defined(WARN_UNUSED_RETURN) && COMPILER(GCC_OR_CLANG)
#define WARN_UNUSED_RETURN __attribute__((__warn_unused_result__))
#endif

#if !defined(WARN_UNUSED_RETURN)
#define WARN_UNUSED_RETURN
#endif

#if !defined(__has_include) && COMPILER(MSVC)
#define __has_include(path) 0
#endif

/* LANG() - language being compiled */
#define LANG(UU_FEATURE) (defined UU_LANG_##UU_FEATURE  && UU_LANG_##UU_FEATURE)

#if defined(__cplusplus)
#define UU_LANG_CPP 1
#endif

/* weak references */
#define UU_weak(_x) ({ \
__weak typeof(_x) _w = _x; \
_w; \
})

#endif // UU_COMPILER_H
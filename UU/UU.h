//
// UU.h
//
// MIT License
// Copyright (c) 2022 Ken Kocienda. All rights reserved.
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

#ifndef UU_H
#define UU_H

static constexpr int UU_MAJOR_VERSION = 0;
static constexpr int UU_MINOR_VERSION = 1;
static constexpr int UU_FIXES_VERSION = 0;

#include <UU/Assertions.h>

#include <UU/AcquireReleaseGuard.h>
#include <UU/ANSICode.h>
#include <UU/Any.h>
#include <UU/CloseGuard.h>
#include <UU/Compiler.h>
#include <UU/FileLike.h>
#include <UU/IteratorWrapper.h>
#include <UU/MappedFile.h>
#include <UU/MathLike.h>
#include <UU/Platform.h>
#include <UU/SmallVector.h>
#include <UU/Spread.h>
#include <UU/Spread.h>
#include <UU/StackTrace.h>
#include <UU/StaticByteBuffer.h>
#include <UU/Storage.h>
#include <UU/Stretch.h>
#include <UU/StringLike.h>
#include <UU/TextRef.h>
#include <UU/Types.h>
#include <UU/UTF8.h>
#include <UU/UnixLike.h>
#include <UU/UUString.h>

#endif // UU_H

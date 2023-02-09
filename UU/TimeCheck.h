//
// TimeCheck.h
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

#ifndef UU_TIME_CHECK_H
#define UU_TIME_CHECK_H

#include <stdio.h>

#include <chrono>
#include <string>

#include <UU/Assertions.h>

namespace UU {

using TimeMark = std::chrono::time_point<std::chrono::steady_clock>;
using TimeElapsed = std::chrono::duration<double>;

struct TimeCheck {
    TimeMark mark;
    TimeElapsed elapsed = TimeElapsed(0);
};



UU_ALWAYS_INLINE TimeMark time_check_now() {
    return std::chrono::steady_clock::now();
}

UU_ALWAYS_INLINE TimeCheck &time_check_get(int idx) {
    ASSERT(idx < TIME_CHECK_COUNT);
    static constexpr int TIME_CHECK_COUNT = 8;
    static TimeCheck time_checks[TIME_CHECK_COUNT];
    return time_checks[idx];
}

UU_ALWAYS_INLINE void time_check_mark(int idx) {
    ASSERT(idx < TIME_CHECK_COUNT);
    TimeCheck &time_check = time_check_get(idx);
    time_check.mark = time_check_now();
}

UU_ALWAYS_INLINE void time_check_done(int idx) {
    ASSERT(idx < TIME_CHECK_COUNT);
    TimeCheck &time_check = time_check_get(idx);
    TimeMark done = time_check_now();
    TimeElapsed elapsed = done - time_check.mark;
    time_check.elapsed += elapsed;
}

UU_ALWAYS_INLINE TimeElapsed time_check_elapsed(int idx) {
    ASSERT(idx < TIME_CHECK_COUNT);
    TimeCheck &time_check = time_check_get(idx);
    return time_check.elapsed;
}

UU_ALWAYS_INLINE double time_check_elapsed_seconds(int idx) {
    return time_check_elapsed(idx).count();
}

UU_ALWAYS_INLINE void time_check_reset(int idx) {
    ASSERT(idx < TIME_CHECK_COUNT);
    TimeCheck &time_check = time_check_get(idx);
    time_check.elapsed = TimeElapsed(0);
}

}  // namespace UU

#endif  // UU_TIME_CHECK_H

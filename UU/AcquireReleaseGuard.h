//
// AcquireReleaseGuard.h
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

#ifndef UU_ACQUIRE_RELEASE_GUARD_H
#define UU_ACQUIRE_RELEASE_GUARD_H

namespace UU {

template <class Resource>
class AcquireReleaseGuard
{
public:
    AcquireReleaseGuard(Resource &rsrc) : m_rsrc(rsrc) { m_rsrc.acquire(); }
    ~AcquireReleaseGuard() { m_rsrc.release(); }
private:
    AcquireReleaseGuard(const AcquireReleaseGuard &) = delete;
    AcquireReleaseGuard &operator=(const AcquireReleaseGuard &) = delete;
    Resource &m_rsrc;
};

}  // namespace UU

#endif  // UU_ACQUIRE_RELEASE_GUARD_H

//
// SmallVector.cpp
//
// Based on SmallVector.cpp from LLVM.
// Copyright (c) 2022 Ken Kocienda. All rights reserved.
//

/*

Copyright (c) 2003-2017 University of Illinois at Urbana-Champaign.
All rights reserved.

The LLVM Compiler Infrastructure

This file is distributed under the University of Illinois Open Source License. 
See below or LICENSE-LLVM.txt for details.


==============================================================================
LLVM Release License
==============================================================================
University of Illinois/NCSA
Open Source License

Copyright (c) 2003-2017 University of Illinois at Urbana-Champaign.
All rights reserved.

Developed by:

    LLVM Team

    University of Illinois at Urbana-Champaign

    http://llvm.org

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal with
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimers.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimers in the
      documentation and/or other materials provided with the distribution.

    * Neither the names of the LLVM Team, University of Illinois at
      Urbana-Champaign, nor the names of its contributors may be used to
      endorse or promote products derived from this Software without specific
      prior written permission.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
SOFTWARE.

*/

#include <UU/SmallVector.h>

namespace UU {

// grow_pod - This is an implementation of the grow() method which only works
// on POD-like datatypes and is out of line to reduce code duplication.
void SmallVectorBase::grow_pod(void *m_first_element, size_t MinSizeInBytes, size_t TSize) 
{
    size_t CurSizeBytes = size_in_bytes();
    size_t NewCapacityInBytes = 2 * capacity_in_bytes() + TSize; // Always grow.
    if (NewCapacityInBytes < MinSizeInBytes) {
        NewCapacityInBytes = MinSizeInBytes;
    }

    void *NewElts;
    if (BeginX == m_first_element) {
        NewElts = malloc(NewCapacityInBytes);
        if (NewElts == nullptr) {
            ASSERT_WITH_MESSAGE(false, "Allocation of SmallVector element failed.");
        }

        // Copy the elements over.  No need to run dtors on PODs.
        memcpy(NewElts, this->BeginX, CurSizeBytes);
    } 
    else {
        // If this wasn't grown from the inline copy, grow the allocated space.
        NewElts = realloc(this->BeginX, NewCapacityInBytes);
        if (NewElts == nullptr) {
            ASSERT_WITH_MESSAGE(false, "Allocation of SmallVector element failed.");
        }
    }

    this->EndX = (char*)NewElts + CurSizeBytes;
    this->BeginX = NewElts;
    this->CapacityX = (char*)this->BeginX + NewCapacityInBytes;
}

} // namespace UU

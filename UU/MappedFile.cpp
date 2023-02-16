//
// MappedFile.cpp
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

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "Assertions.h"
#include "MappedFile.h"
#include "MathLike.h"

namespace fs = std::filesystem;

namespace UU {

MappedFile::MappedFile(const fs::path &path) : m_path(path)
{
    m_fd = open(m_path.c_str(), O_RDONLY | O_NONBLOCK, static_cast<int>(fs::perms::owner_read));
    if (m_fd == -1) {
        m_sys_errno = errno;
        LOG(Error, "MappedFile: cannot open file: %s: %s", m_path.c_str(), strerror(m_sys_errno));
        close();
        return;
    }

    struct stat sb;
    int rc = stat(path.c_str(), &sb);
    if (rc == -1) {
        m_sys_errno = errno;
        LOG(Error, "MappedFile: stat error: %s: %s", m_path.c_str(), strerror(m_sys_errno));
        close();
        return;
    }
    
    m_file_length = sb.st_size;
    if (m_file_length == 0) {
        m_sys_errno = ENOTSUP;
        LOG(Error, "MappedFile: attempt to map zero-length file: %s", m_path.c_str());
        close();
        return;
    }

    m_map_length = ceil_to_page_size(m_file_length);
    m_base = mmap(0, m_map_length, PROT_READ, MAP_FILE | MAP_SHARED, m_fd, 0);
    if (m_base == MAP_FAILED) {
        m_sys_errno = errno;
        LOG(Error, "MappedFile: mmap error: %s: %s", m_path.c_str(), strerror(m_sys_errno));
        close();
        return;
    }

    m_valid = true;
}

MappedFile::~MappedFile()
{
    close();
}

void MappedFile::close()
{
    if (m_base) {
        munmap(m_base, m_map_length);
        m_base = nullptr;
    }
    if (m_fd != NotAnFD) {
        ::close(m_fd);
        m_fd = NotAnFD;
    }
    m_valid = false;
}

}  // namespace UU

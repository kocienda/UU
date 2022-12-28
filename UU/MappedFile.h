//
// MappedFile.h
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

#include <filesystem>

namespace UU {

class MappedFile
{
public:
    enum { NotAnFD = -1 };

    MappedFile() {}
    MappedFile(const std::filesystem::path &path); 
    ~MappedFile();

    const std::filesystem::path &path() const { return m_path; }
    int fd() const { return m_fd; }
    int sys_errno() const { return m_sys_errno; }
    size_t file_length() const { return m_file_length; }
    size_t map_length() const { return m_map_length; }
    void *base() const { return m_base; }
    void close();

    template <bool B = true> bool is_valid() const { return m_valid == B; };

private:
    void map(void *addr = 0);

    std::filesystem::path m_path;
    int m_fd = NotAnFD;
    int m_sys_errno = 0;

    size_t m_file_length = 0;
    size_t m_map_length = 0;
    void *m_base = nullptr;
    bool m_valid = false;
};

}  // namespace UU

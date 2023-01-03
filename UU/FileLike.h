//
// FileLike.h
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

#ifndef UU_FILE_LIKE_H
#define UU_FILE_LIKE_H

#include <filesystem>
#include <string>
#include <vector>

#include <fnmatch.h>

#include <UU/UUString.h>


namespace UU {

static constexpr int FilenameMatchWildcard = 0;
static constexpr int FilenameMatchCaseFold = 0;
static constexpr int FilenameMatchExact = 0x1;
static constexpr int FilenameMatchCaseSensitive = 0x2;

std::vector<std::filesystem::path> skippable_paths();
bool is_skippable(const std::vector<std::filesystem::path> skippables, const std::filesystem::path &path, int flags=0);

std::vector<std::filesystem::path> searchable_paths();
bool is_searchable(const std::vector<std::filesystem::path> searchables, const std::filesystem::path &path, int flags=0);

std::string get_file_contents_as_string(const std::filesystem::path &path);
bool write_file(const std::filesystem::path &path, const std::string &string);

bool filename_match(const UU::String &pattern, const std::filesystem::path &path, int flags=0);

std::filesystem::path absolute_path_relative_to(const std::filesystem::path &path, const std::filesystem::path &reference_path);

}  // namespace UU

#endif // UU_FILE_LIKE_H

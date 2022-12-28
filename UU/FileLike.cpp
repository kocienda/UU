//
// FileLike.cpp
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

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>

#include "FileLike.h"

namespace fs = std::filesystem;

namespace UU {

std::vector<fs::path> skippable_paths()
{
    static std::vector<fs::path> result;
    static std::once_flag flag;
    std::call_once(flag, []() { 
        char *env_path = getenv("SKIPPABLES_PATH");
        fs::path skippables_path;
        if (env_path) {
            skippables_path = env_path;
        }
        else {
            skippables_path = "~/.skippables";
        }
        skippables_path = fs::absolute(skippables_path);

        std::string skippables_contents = get_file_contents_as_string(skippables_path);
        std::stringstream skippables_contents_stream(skippables_contents);
        std::string line;
        while (getline(skippables_contents_stream, line, '\n')) {
            result.push_back(line);
        }
    });
    return result;
}

bool is_skippable(const std::vector<fs::path> skippables, const fs::path &path, int flags)
{
    for (const auto &skippable : skippables) {
        if (filename_match(skippable, path.filename(), flags)) {
            return true;
        }
    }
    return false;
}

std::vector<std::filesystem::path> searchable_paths()
{
    static std::vector<fs::path> result;
    static std::once_flag flag;
    std::call_once(flag, []() { 
        char *env_path = getenv("SEARCHABLES_PATH");
        fs::path searchables_path;
        if (env_path) {
            searchables_path = env_path;
        }
        else {
            searchables_path = "~/.searchables";
        }
        searchables_path = fs::absolute(searchables_path);

        std::string searchables_contents = get_file_contents_as_string(searchables_path);
        std::stringstream searchables_contents_stream(searchables_contents);
        std::string line;
        while (getline(searchables_contents_stream, line, '\n')) {
            result.push_back(line);
        }
    });
    return result;
}

bool is_searchable(const std::vector<std::filesystem::path> searchables, const std::filesystem::path &path, int flags)
{
    for (const auto &searchable : searchables) {
        if (path.extension() == searchable) {
            return true;
        }
    }
    return false;
}

std::string get_file_contents_as_string(const fs::path &path)
{
    std::string result;

    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (f) {
        f.seekg(0, std::ios::end);
        size_t size = f.tellg();
        result.resize(size);
        f.seekg(0, std::ios::beg);
        f.read(&result[0], size);
        f.close();
    }

    return result;
}

bool write_file(const std::filesystem::path &path, const std::string &string)
{
    std::ofstream os(path, std::ios::trunc);
    if (os.fail()) {
        return false;
    }
    os << string;
    os.close();
    return true;
}

bool filename_match(const std::string &pattern, const fs::path &path, int flags)
{
    std::string filename = path.filename().c_str();
    std::string effective_pattern = (flags & FilenameMatchExact) ? pattern : ("*" + pattern + "*");
    int fnmatch_flags = (flags & FilenameMatchCaseSensitive) ? 0 : FNM_CASEFOLD;
    return fnmatch(effective_pattern.c_str(), filename.c_str(), fnmatch_flags) == 0;
}

fs::path absolute_path_relative_to(const fs::path &path, const fs::path &reference_path)
{
    fs::path absolute_path = fs::absolute(path);
    fs::path absolute_reference_path = fs::absolute(reference_path);
    return fs::path(absolute_path.string().substr(absolute_reference_path.string().length() + 1));
}


}  // namespace UU
//
//  FileLike.cpp
//

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

}  // namespace UU
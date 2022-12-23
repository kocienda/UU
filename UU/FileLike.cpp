//
//  FileLike.cpp
//

#include <fstream>
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

        std::stringstream contents(get_file_contents_as_string(skippables_path));
        std::string line;
        while (getline(contents, line, '\n')) {
            result.push_back(line);
        }
    });

    return result;
}

bool is_skippable(const std::vector<fs::path> skippables, const fs::path &path, int flags)
{
    for (const auto &skippable : skippables) {
        if (fnmatch(skippable.c_str(), path.filename().c_str(), flags) == 0) {
            return true;
        }
    }
    return false;
}

std::string get_file_contents_as_string(const fs::path &path)
{
    std::string result;

    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f) {
        return result;
    }

    const auto size = fs::file_size(path);
    result.reserve(size);
    f.read(result.data(), size);

    return result;
}

}  // namespace UU
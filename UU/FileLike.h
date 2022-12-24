//
//  FileLike.h
//

#ifndef UU_FILE_LIKE_H
#define UU_FILE_LIKE_H

#include <filesystem>
#include <string>
#include <vector>

#include <fnmatch.h>

namespace UU {

std::vector<std::filesystem::path> skippable_paths();
bool is_skippable(const std::vector<std::filesystem::path> skippables, const std::filesystem::path &path, int flags=FNM_CASEFOLD|FNM_PERIOD);

std::string get_file_contents_as_string(const std::filesystem::path &path);

enum class FilenameMatchFlag { EXACT, WILDCARD };
bool filename_match(const std::string &pattern, const std::filesystem::path &path, FilenameMatchFlag flag=FilenameMatchFlag::WILDCARD);

}  // namespace UU

#endif // UU_FILE_LIKE_H

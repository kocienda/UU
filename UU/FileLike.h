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

static constexpr int FilenameMatchWildcard = 0;
static constexpr int FilenameMatchCaseFold = 0;
static constexpr int FilenameMatchExact = 0x1;
static constexpr int FilenameMatchCaseSensitive = 0x2;

std::vector<std::filesystem::path> skippable_paths();
bool is_skippable(const std::vector<std::filesystem::path> skippables, const std::filesystem::path &path, int flags=0);

std::vector<std::filesystem::path> searchable_paths();
bool is_searchable(const std::vector<std::filesystem::path> searchables, const std::filesystem::path &path, int flags=0);

std::string get_file_contents_as_string(const std::filesystem::path &path);

bool filename_match(const std::string &pattern, const std::filesystem::path &path, int flags=0);

}  // namespace UU

#endif // UU_FILE_LIKE_H

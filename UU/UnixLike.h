//
//  UnixLike.h
//

#ifndef UU_UNIX_LIKE_H
#define UU_UNIX_LIKE_H

#include <filesystem>
#include <string>
#include <vector>

namespace UU {

int launch(const std::filesystem::path &program, const std::vector<std::string> &args);
std::string shell_escaped_string(const std::string &str);

}  // namespace UU

#endif // UU_UNIX_LIKE_H

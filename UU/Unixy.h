//
//  Unixy.h
//

#ifndef UU_UNIXY_H
#define UU_UNIXY_H

#include <filesystem>
#include <string>
#include <vector>

namespace UU {

int launch(const std::filesystem::path &program, const std::vector<std::string> &args);
std::string shell_escaped_string(const std::string &str);

}  // namespace UU

#endif // UU_UNIXY_H

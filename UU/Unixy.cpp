//
//  Unixy.cpp
//

#include <unistd.h>

#include "DynamicByteBuffer.h"
#include "Unixy.h"

namespace fs = std::filesystem;

namespace UU {

int launch(const fs::path &program, const std::vector<std::string> &args)
{
        size_t size = args.size() + 2;
        char *exec_args[size];
        exec_args[0] = strdup(program.c_str());
        exec_args[size - 1] = NULL;
        int idx = 1;
        for (const auto &arg : args) {
            exec_args[idx] = strdup(arg.c_str());
            idx++;
        }
        int rc = execvp(program.c_str(), exec_args);
        // free memory on execvp failure
        for (int idx = 0; idx < size - 1; idx++) {
            free(exec_args[idx]);
        }
        return rc;
}

std::string shell_escaped_string(const std::string &str)
{
    std::string result;
    if (str.find(' ') == std::string::npos && str.find('\'') == std::string::npos) {
        result = str;
    }
    else {
        DynamicByteBuffer buf;
        buf.reserve(str.size());
        for (size_t idx = 0; idx < str.size(); idx++) {
            if (str[idx] == ' ' || str[idx] == '\'') {
                buf += '\\';
            }
            buf += str[idx];
        }
        result = buf;
    }
    return result;
}

}  // namespace UU
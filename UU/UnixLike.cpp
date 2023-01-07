//
// UnixLike.cpp
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

#include <sys/types.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include "Assertions.h"
#include "UnixLike.h"

namespace fs = std::filesystem;

namespace UU {

int launch(const fs::path &program, const std::vector<String> &args)
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

String shell_escaped_string(const String &str)
{
    String result;
    if (str.find(' ') == String::npos && str.find('\'') == String::npos) {
        result = str;
    }
    else {
        String buf;
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

int get_sysctl_logicalcpu()
{
    int num = 0;
    size_t len = sizeof(num);
    int rc = sysctlbyname("hw.logicalcpu", &num, &len, NULL, 0);
    if (rc == -1) {
        LOG(Error, "get_sysctl_logicalcpu: failed: %s", strerror(errno));
        return 0;
    }
    return num;
}

int get_good_concurrency_count()
{
    int c = get_sysctl_logicalcpu();
    return c > 0 ? c : 8;
}


}  // namespace UU

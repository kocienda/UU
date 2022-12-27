//
//  MappedFile.h
//

#include <filesystem>

namespace UU {

class MappedFile
{
public:
    enum { NotAnFD = -1 };

    MappedFile() {}
    MappedFile(const std::filesystem::path &path); 
    ~MappedFile();

    const std::filesystem::path &path() const { return m_path; }
    int fd() const { return m_fd; }
    int sys_errno() const { return m_sys_errno; }
    size_t file_length() const { return m_file_length; }
    size_t map_length() const { return m_map_length; }
    void *base() const { return m_base; }
    void close();

    template <bool B = true> bool is_valid() const { return m_valid == B; };

private:
    void map(void *addr = 0);

    std::filesystem::path m_path;
    int m_fd = NotAnFD;
    int m_sys_errno = 0;

    size_t m_file_length = 0;
    size_t m_map_length = 0;
    void *m_base = nullptr;
    bool m_valid = false;
};

}  // namespace UU

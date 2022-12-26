//
//  MappedFile.h
//

#include <filesystem>

#include <fcntl.h>
#include <sys/mman.h>

namespace UU {

class MappedFile
{
public:

    enum { NotAnFD = -1 };

    MappedFile() {}
    MappedFile(const std::filesystem::path &path, int flags = O_RDONLY, 
        std::filesystem::perms perms = std::filesystem::perms::owner_read, int prot = PROT_READ); 
    ~MappedFile();

    const std::filesystem::path &path() const { return m_path; }
    int fd() const { return m_fd; }
    int flags() const { return m_flags; }
    std::filesystem::perms perms() const { return m_perms; }
    template <bool B = true> bool is_valid() const { return m_valid == B; };

    template <bool B = true> bool is_dirty() const { return m_dirty == B; }
    void set_dirty(bool flag = true) { m_dirty = flag; }

    size_t file_length() const { return m_file_length; }
    size_t map_length() const { return m_map_length; }
    int prot() const { return m_prot; }
    void *base() const { return m_base; }
    void sync();
    bool resize(int32_t);
    void close();

private:
    void set_valid(bool flag = true) { m_valid = flag; }
    void invalidate();
    void map(void *addr = 0);

    std::filesystem::path m_path;
    int m_fd = NotAnFD;
    int m_flags = O_RDONLY;
    std::filesystem::perms m_perms = std::filesystem::perms::owner_read;
    bool m_valid = false;
    int m_errno = 0;

    size_t m_file_length = 0;
    size_t m_map_length = 0;
    int m_prot = PROT_NONE;
    bool m_dirty = false;
    void *m_base = nullptr;
};

}  // namespace UU

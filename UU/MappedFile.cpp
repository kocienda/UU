//
//  MappedFile.cpp
//

#include <sys/stat.h>
#include <unistd.h>

#include "MappedFile.h"

namespace UU {

static size_t compute_map_length(size_t length)
{
    size_t page_size = getpagesize();
    bool even = length % page_size == 0;
    return even ? length : ((length / page_size) + 1) * page_size;
}

MappedFile::MappedFile(const std::filesystem::path &path, int flags, std::filesystem::perms perms, int prot) 
    : m_path(path), m_flags(flags), m_perms(perms), m_prot(prot)
{
    m_fd = open(m_path.c_str(), m_flags, static_cast<int>(m_perms));
    m_errno = errno;
    if (m_fd >= 0) {
        m_valid = true;
    }

    if (is_valid<false>()) {
        return;
    }
    
    if (m_prot == PROT_NONE) {
        fprintf(stderr, "MappedFile: illegal protection mode: %d\n", m_prot);
        set_valid(false);
        return;
    }

    struct stat sb;
    int rc = stat(path.c_str(), &sb);
    if (rc) {
        fprintf(stderr, "MappedFile: stat error: %s\n", strerror(errno));
        set_valid(false);
        return;
    }
    
    m_file_length = sb.st_size;
    size_t provisional_map_length = 0;
    if (m_file_length == 0) {
        if ((m_prot & PROT_WRITE) == 0) {
            set_valid(false);
            close();
            return;
        }
        provisional_map_length = compute_map_length(1);
    }
    else {
        provisional_map_length = compute_map_length(m_file_length);
    }

    if (provisional_map_length != m_file_length && (m_prot & PROT_WRITE)) {
        resize((int32_t)(provisional_map_length / getpagesize()));
    }
    else {
        m_map_length = provisional_map_length;
        map();
    }
}

MappedFile::~MappedFile()
{
    close();
}

void MappedFile::map(void *addr)
{
    m_base = mmap(addr, m_map_length, m_prot, MAP_FILE | MAP_SHARED, fd(), 0);
    if (m_base == MAP_FAILED) {
        fprintf(stderr, "MappedFile: mmap error: %s\n", strerror(errno));
        set_valid(false);
    }
}

void MappedFile::sync()
{
    if (is_valid() && is_dirty() && (prot() & PROT_WRITE)) {
        msync(base(), map_length(), MS_SYNC);
        set_dirty(false);
    }
}

bool MappedFile::resize(int32_t pages)
{
    if (pages == 0) {
        return true;
    }

    sync();
    
    void *saved_base = m_base;
    if (m_base) {
        munmap(m_base, m_map_length);
        m_base = 0;
    }
    
    int32_t new_length = pages * getpagesize();
    if (new_length < 0) {
        new_length = 0;
    }

    int rc = ftruncate(fd(), new_length);
    if (rc < 0) {
        fprintf(stderr, "MappedFile: error in resize: %s\n", strerror(errno));
        set_valid(false);
        return false;
    }
    
    m_file_length = m_map_length = new_length;
    map(saved_base);
 
    return is_valid();
}

void MappedFile::close()
{
    if (is_valid<false>()) {
        return;
    }
    sync();
    munmap(m_base, m_map_length);
    ::close(m_fd);
    m_fd = NotAnFD;
    m_valid = false;
}

}  // namespace UU

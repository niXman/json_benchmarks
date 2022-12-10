
#ifndef __JSON_BENCHMARKS__MMFILE_HPP
#define __JSON_BENCHMARKS__MMFILE_HPP

#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>

namespace json_benchmarks {

/*************************************************************************************************/

struct mmsource {
    mmsource(const char *fname)
        :m_fd{::open(fname, O_RDONLY)}
    { assert(open()); }

    ~mmsource() {
        if ( m_fd == -1 ) {
            return;
        }

        ::munmap(m_addr, m_size);
        ::close(m_fd);
    }

    bool open() {
        if ( m_fd == -1 ) {
            return false;
        }

        struct stat st;
        ::fstat(m_fd, &st);
        m_size = st.st_size;

        m_addr = ::mmap(nullptr, m_size, PROT_READ, MAP_SHARED, m_fd, 0);
        assert(m_addr != MAP_FAILED);

        return ::posix_madvise(m_addr, m_size, POSIX_MADV_SEQUENTIAL) == 0;
    }

    const char* begin() const { return static_cast<const char*>(m_addr); }
    const char* end()   const { return static_cast<const char*>(m_addr) + m_size; }

    const char* data() const { return static_cast<const char*>(m_addr); }
    char*       data()       { return static_cast<char*>(m_addr); }
    size_t      size() const { return m_size; }

private:
    int m_fd;
    size_t m_size;
    void *m_addr;
};

/*************************************************************************************************/

struct mmsink {
    mmsink(const char *fname, size_t max_size)
        :ofname{fname}
        ,m_fd{-1}
        ,m_size{max_size}
    { assert(open()); }

    ~mmsink() {
        if ( m_fd == -1 ) {
            return;
        }

        ::munmap(m_addr, m_size);
        ::close(m_fd);
    }

    bool resize(size_t new_size) {
        bool ok = ::ftruncate(m_fd, new_size) == 0;
        assert(ok);

        void *p = ::mremap(m_addr, m_size, new_size, MREMAP_MAYMOVE);
        assert(p != MAP_FAILED);

        m_addr = p;
        m_size = new_size;

        return ::posix_madvise(m_addr, m_size, POSIX_MADV_SEQUENTIAL) == 0;
    }

    bool open() {
        m_fd = ::open(ofname.c_str(), O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
        if ( m_fd == -1 ) {
            return false;
        }

        assert(::ftruncate(m_fd, m_size) == 0);

        m_addr = ::mmap(nullptr, m_size, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0);
        assert(m_addr != MAP_FAILED);

        return ::posix_madvise(m_addr, m_size, POSIX_MADV_SEQUENTIAL) == 0;
    }

    char* begin() { return static_cast<char *>(m_addr); }
    char* end()   { return static_cast<char *>(m_addr) + m_size; }

    char*  data() const { return static_cast<char*>(m_addr); }
    size_t size() const { return m_size; }

private:
    std::string ofname;
    int m_fd;
    size_t m_size;
    void *m_addr;
};

/*************************************************************************************************/

} // json_benchmarks

#endif // __JSON_BENCHMARKS__MMFILE_HPP

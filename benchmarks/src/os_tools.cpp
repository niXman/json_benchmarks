// Copyright 2016 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://sourceforge.net/projects/jsoncons/files/ for latest version
// See https://sourceforge.net/p/jsoncons/wiki/Home/ for documentation.

#include <fstream>
#include <cassert>

#ifdef WIN32
#   include "windows.h"
#   include "psapi.h"
#elif defined(__linux__)
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <unistd.h>
#   include <sys/resource.h>
#else
#   error "unknown OS"
#endif

#include "os_tools.hpp"
#include "stringize.hpp"

namespace json_benchmarks {

std::string get_os_type() {
#ifdef WIN32
    return "Windows";
#elif defined(__linux__)
    return "Linux";
#else
#   error "unknown OS"
#endif
}

std::string get_os() {
#ifdef WIN32
    return "UNIMPLEMENTED";
#elif defined(__linux__)
    std::ifstream is{"/etc/os-release"};
    if ( !is ) {
        return "UNKNOWN";
    }

    static const char strid[] = "PRETTY_NAME=";
    std::string line;
    while ( std::getline(is, line) ) {
        if ( line.find(strid) != std::string::npos ) {
            break;
        }
    }

    line = line.substr(sizeof(strid)-1);
    if ( line.front() == '"' ) {
        line = line.substr(1);
    }
    if ( line.back() == '"' ) {
        line.pop_back();
    }

    if ( sizeof(void *) == sizeof(std::uint64_t) ) {
        line += ", x86_64";
    } else {
        line += ", x86_32";
    }

    return line;
#else
#   error "unknown OS"
#endif
}

std::string get_compiler() {
#ifdef _MSC_VER
    return "unknown MSVC";
#elif defined(__clang__)
    return "clang++-" __STRINGIZE_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    return "g++-" __STRINGIZE_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
#   error "unknown compiler"
#endif
}

std::string get_motherboard() {
#ifdef WIN32
    return "UNIMPLEMENTED";
#elif defined(__linux__)
    std::ifstream is{"/sys/class/dmi/id/board_vendor"};
    if ( !is ) {
        return "UNKNOWN";
    }

    std::string vendor;
    std::getline(is, vendor);
    if ( vendor.empty() ) {
        return "UNKNOWN";
    }
    if ( vendor.back() == '.' ) {
        vendor.pop_back();
    }

    std::ifstream is2{"/sys/class/dmi/id/board_name"};
    if ( !is2 ) {
        vendor += ", UNKNOWN";
        return  vendor;
    }

    std::string name;
    std::getline(is2, name);
    if ( name.empty() ) {
        vendor += ", UNKNOWN";
        return  vendor;
    }

    vendor += "-";
    vendor += name;

    return vendor;
#else
#   error "unknown OS"
#endif
}

std::string get_cpu_type() {
#ifdef WIN32
    return "UNIMPLEMENTED";
#elif defined(__linux__)
    std::ifstream is{"/proc/cpuinfo"};
    if ( !is ) {
        return "UNKNOWN";
    }
    auto pos = 0ul;
    std::string line;
    while ( std::getline(is, line) ) {
        pos = line.find("vendor_id");
        if ( pos != std::string::npos ) {
            break;
        }
    }

    pos = line.find(':');
    if ( pos == std::string::npos ) {
        return "UNKNOWN";
    }
    line = line.substr(pos + 1);

    for ( ; !line.empty() && line.front() == ' '; line = line.substr(1) )
    {}

    if ( line == "AuthenticAMD" ) {
        return "AMD";
    } else if ( line == "GenuineIntel" ) {
        return "Intel";
    }

    return line;
#else
#   error "unknown OS"
#endif
}

std::string get_cpu() {
#ifdef WIN32
    return "UNIMPLEMENTED";
#elif defined(__linux__)
    std::ifstream is{"/proc/cpuinfo"};
    if ( !is ) {
        return "UNKNOWN";
    }
    auto pos = 0ul;
    std::string line;
    while ( std::getline(is, line) ) {
        pos = line.find("model name");
        if ( pos != std::string::npos ) {
            break;
        }
    }

    pos = line.find(':');
    if ( pos == std::string::npos ) {
        return "UNKNOWN";
    }
    line = line.substr(pos + 1);

    for ( ; !line.empty() && line.front() == ' '; line = line.substr(1) )
    {}

    return line;
#else
#   error "unknown OS"
#endif
}

std::string get_ram() {
#ifdef WIN32
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    ::GlobalMemoryStatusEx(&status);
    auto s = std::to_string(status.ullTotalPhys);
    s += " GB";
    return s;
#elif defined(__linux__)
    long pages = ::sysconf(_SC_PHYS_PAGES);
    long page_size = ::sysconf(_SC_PAGE_SIZE);

    auto total = pages * page_size;
    total /= 1024ul*1024ul*1024ul;
    auto s = std::to_string(total);
    s += " GB";
    return s;
#else
#   error "unknown OS"
#endif
}

std::size_t file_size(const char *fname) {
    struct stat st;
    assert(::stat(fname, &st) == 0);

    return st.st_size;
}

std::size_t file_size(int fd) {
    struct stat st;
    assert(::fstat(fd, &st) == 0);

    return st.st_size;
}

} // ns json_benchmarks

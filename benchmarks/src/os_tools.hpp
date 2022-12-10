// Copyright 2016 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://sourceforge.net/projects/jsoncons/files/ for latest version
// See https://sourceforge.net/p/jsoncons/wiki/Home/ for documentation.

#ifndef JSON_BENCHMARKS_MEASURER_HPP
#define JSON_BENCHMARKS_MEASURER_HPP

#include <string>
#include <cstdint>

namespace json_benchmarks {

std::size_t get_process_memory();
std::string get_os_type();
std::string get_os();
std::string get_compiler();
std::string get_motherboard();
std::string get_cpu_type();
std::string get_cpu();
std::string get_ram();

std::size_t file_size(const char *fname);
std::size_t file_size(int fd);

} // ns json_benchmarks

#endif


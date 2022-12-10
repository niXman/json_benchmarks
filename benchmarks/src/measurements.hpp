// Copyright 2016 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://sourceforge.net/projects/jsoncons/files/ for latest version
// See https://sourceforge.net/p/jsoncons/wiki/Home/ for documentation.

#ifndef JSON_BENCHMARKS_MEASUREMENTS_HPP
#define JSON_BENCHMARKS_MEASUREMENTS_HPP

#include <string>
#include <vector>
#include <iosfwd>
#include <filesystem>

#include <malloc-stat/api.h>

namespace json_benchmarks {

namespace fs = std::filesystem;

/*************************************************************************************************/

template <class F>
void json_file_finder(const char* dir_name, F f)
{
    fs::path p{ dir_name };

    if (exists(p) && is_directory(p))
    {
        fs::directory_iterator end_iter;
        for (fs::directory_iterator dir_itr(p);
            dir_itr != end_iter;
            ++dir_itr)
        {
            if (fs::is_regular_file(dir_itr->status()))
            {
                if (dir_itr->path().extension() == ".json")
                {
                    f(dir_itr->path());
                }
            }
        }
    }
}

struct measurements {
    std::string name;
    std::string errmsg;
    size_t prepare_allocated;
    size_t prepare_allocations;
    size_t prepare_deallocations;
    size_t time_to_prepare;
    size_t parse_allocated;
    size_t parse_allocations;
    size_t parse_deallocations;
    size_t time_to_parse;
    size_t print_allocated;
    size_t print_allocations;
    size_t print_deallocations;
    size_t time_to_print;
    size_t free_deallocated;
    size_t free_deallocations;
    size_t free_leaked_bytes;
    size_t free_leaked_allocations;
    size_t time_to_free;

    measurements()
        :name{}
        ,errmsg{}
        ,parse_allocated{}
        ,parse_allocations{}
        ,parse_deallocations{}
        ,time_to_parse{}
        ,print_allocated{}
        ,print_allocations{}
        ,print_deallocations{}
        ,time_to_print{}
        ,free_deallocated{}
        ,free_deallocations{}
        ,free_leaked_bytes{}
        ,free_leaked_allocations{}
        ,time_to_free{}
    {}

    static std::string humanSize(std::uint64_t bytes) {
        static const char *suffix[] = {"B", "KB", "MB", "GB", "TB"};
        static const int length = sizeof(suffix) / sizeof(suffix[0]);

        int i = 0;
        double dblBytes = bytes;

        if ( bytes > 1024 ) {
            for ( i = 0; (bytes / 1024) > 0 && i<length-1; i++, bytes /= 1024 ) {
                dblBytes = bytes / 1024.0;
            }
        }

        char output[200];
        std::snprintf(output, sizeof(output), "%.02lf %s", dblBytes, suffix[i]);

        return output;
    }

    friend std::ostream& operator<< (std::ostream &os, const measurements &m) {
        os
            << "    errmsg: " << (m.errmsg.empty() ? "nope" : m.errmsg.c_str()) << std::endl
            << "    prepare time: " << m.time_to_prepare/1000.0 << ", allocated : " << humanSize(m.prepare_allocated) << ", allocs: " << m.prepare_allocations << ", deallocs: " << m.prepare_deallocations << std::endl
            << "    parse   time: " << m.time_to_parse/1000.0 << ", allocated : " << humanSize(m.parse_allocated) << ", allocs: " << m.parse_allocations << ", deallocs: " << m.parse_deallocations << std::endl
            << "    print   time: " << m.time_to_print/1000.0 << ", allocated : " << humanSize(m.print_allocated) << ", allocs: " << m.print_allocations << ", deallocs: " << m.print_deallocations << std::endl
            << "    free    time: " << m.time_to_free/1000.0 << ", deallocated: " << humanSize(m.free_deallocated) << ", deallocs: " << m.free_deallocations << std::endl
            << "    leaked bytes: " << m.free_leaked_bytes << ", leaked allocs: " << m.free_leaked_allocations << std::flush;
        ;

        return os;
    }
};

enum class result_code
{
    expected_result,
    expected_success_parsing_failed,
    expected_failure_parsing_succeeded,
    result_undefined_parsing_succeeded,
    result_undefined_parsing_failed,
    process_stopped
};

struct result_code_info
{
    result_code code;
    std::string description;
    std::string color;
};

enum class expected_result : uint8_t
{
    expect_success = 0,
    expect_failure = 1,
    result_undefined = 2
};

struct test_suite_file
{
    fs::path path;
    expected_result type;
    std::string text;
};

using test_suite_files = std::vector<test_suite_file>;

struct test_suite_result
{
    result_code result;

    test_suite_result(result_code result) :
        result(result)
    {
    }
};

using test_suite_results = std::vector<test_suite_result>;

inline
size_t count_results(const std::vector<test_suite_result>& results,
                     result_code outcome)
{
    size_t count = 0;
    for (auto result : results)
    {
        if (result.result == outcome)
        {
            ++count;
        }
    }
    return count;
}

} // namespace json_benchmarks

#endif


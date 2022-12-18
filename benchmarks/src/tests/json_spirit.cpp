#include <chrono>
#include <fstream>

#include "../measurements.hpp"
#include "../os_tools.hpp"
#include "json_benchmarks.hpp"

#ifndef JSON_SPIRIT_VALUE_ENABLED
#define JSON_SPIRIT_VALUE_ENABLED
#endif

#include <json_spirit/json_spirit_reader_template.h>
#include <json_spirit/json_spirit_writer_template.h>

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;
using namespace json_benchmarks;
using namespace json_spirit;

namespace json_benchmarks {

const std::string& json_spirit_benchmarks::name() const
{
    static const std::string s = "json_spirit";

    return s;
}

const std::string& json_spirit_benchmarks::url() const
{
    static const std::string s = "https://github.com/png85/json_spirit";

    return s;
}

const std::string& json_spirit_benchmarks::version() const
{
    static const std::string s = "4.0.8";

    return s;
}

const std::string& json_spirit_benchmarks::notes() const
{
    static const std::string s = "Uses Boost spirit parsing, expect slower parsing.";

    return s;
}

measurements json_spirit_benchmarks::measure_small(const std::string& input, std::string& output)
{
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;

    {
        start_memory_used =  get_process_memory();

        Value root;
        {
            {
                auto start = high_resolution_clock::now();
                try
                {
                    const char *begin = input.c_str();
                    read_range(begin, begin + input.size(), root);
                    auto end = high_resolution_clock::now();
                    time_to_read = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                }
                catch (const std::exception & e)
                {
                    std::cout << e.what() << std::endl;
                    exit(1);
                }
            }
        }
        end_memory_used =  get_process_memory();
        {
            auto start = high_resolution_clock::now();
            std::ostringstream os;
            write_stream(root,os);
            output = os.str();
            auto end = high_resolution_clock::now();
            time_to_write = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
    }
    size_t final_memory_used = get_process_memory();

    measurements results;
    results.library_name = name();
    results.memory_used = end_memory_used - start_memory_used;
    results.time_to_read = time_to_read;
    results.time_to_write = time_to_write;
    return results;
}

measurements json_spirit_benchmarks::measure_big(const char *input_filename, const char *output_filename)
{
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;

    {
        start_memory_used =  get_process_memory();

        Value root;
        {
            {
                auto start = high_resolution_clock::now();
                try
                {
                    std::ifstream is(input_filename);
                    read_stream(is, root);
                    auto end = high_resolution_clock::now();
                    time_to_read = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                }
                catch (const std::exception & e)
                {
                    std::cout << e.what() << std::endl;
                    exit(1);
                }
            }
        }
        end_memory_used =  get_process_memory();
        {
            std::ofstream os;
            os.open(output_filename, std::ios_base::out | std::ios_base::binary);
            auto start = high_resolution_clock::now();
            write_stream(root,os);
            auto end = high_resolution_clock::now();
            time_to_write = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        }
    }
    size_t final_memory_used = get_process_memory();

    measurements results;
    results.library_name = name();
    results.memory_used = (end_memory_used - start_memory_used)/1000000;
    results.time_to_read = time_to_read;
    results.time_to_write = time_to_write;
    return results;
}

std::vector<test_suite_result> json_spirit_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
{
    std::vector<test_suite_result> results;
    for (auto& file : pathnames)
    {
#ifdef WIN32
        std::string command = "x64\\Release\\json_spirit_parser.exe ";
#elif defined(__linux__)
        std::string command = "./parsers/json_spirit/build/json_spirit_parser ";
#else
#   error "unknown OS"
#endif
        command += file.path.string();
        int result = std::system(command.c_str());

        if (file.type == expected_result::expect_success)
        {
            if (result == 0)
            {
                results.emplace_back(result_code::expected_result);
            }
            else if (result == 1)
            {
                results.emplace_back(result_code::expected_success_parsing_failed);
            }
            else
            {
                results.emplace_back(result_code::process_stopped);
            }
        }
        else if (file.type == expected_result::expect_failure)
        {
            if (result == 0)
            {
                results.emplace_back(result_code::expected_failure_parsing_succeeded);
            }
            else if (result == 1)
            {
                results.emplace_back(result_code::expected_result);
            }
            else
            {
                results.emplace_back(result_code::process_stopped);
            }
        }
        else if (file.type == expected_result::result_undefined)
        {
            if (result == 0)
            {
                results.emplace_back(result_code::result_undefined_parsing_succeeded);
            }
            else if (result == 1)
            {
                results.emplace_back(result_code::result_undefined_parsing_failed);
            }
            else
            {
                results.emplace_back(result_code::process_stopped);
            }
        }
    }

    return results;
}

}

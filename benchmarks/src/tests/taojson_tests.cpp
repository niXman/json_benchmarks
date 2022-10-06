#include <fstream>
#include <chrono>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "../measurements.hpp"
#include "../os_tools.hpp"
#include "json_benchmarks.hpp"

#include <tao/json.hpp>

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;
using namespace json_benchmarks;

namespace json_benchmarks {

const std::string& taojson_benchmarks::name() const
{
    static const std::string s = "taojson";

    return s;
}

const std::string& taojson_benchmarks::url() const
{
    static const std::string s = "https://github.com/taocpp/json";

    return s;
}

const std::string& taojson_benchmarks::version() const
{
    static const std::string s = "1.0.0-beta.13";

    return s;
}

const std::string& taojson_benchmarks::notes() const
{
    static const std::string s = "Uses modified [google/double conversion](https://github.com/google/double-conversion) routines for parsing doubles. Uses modified [jeaiii/itoa](https://github.com/jeaiii/itoa) routines for outputting integers. Uses slightly modified [Grisu2 implementation by Florian Loitsch](https://florian.loitsch.com/publications) for printing doubles, expect faster serializing.";

    return s;
}

measurements taojson_benchmarks::measure_small(const std::string& input, std::string& output)
{
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;
    std::string buffer;

    {
        start_memory_used =  get_process_memory();

        tao::json::value val;
        {
            auto start = high_resolution_clock::now();
            val = tao::json::from_string(input);

            auto end = high_resolution_clock::now();
            time_to_read = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
        end_memory_used =  get_process_memory();
        {
            auto start = high_resolution_clock::now();

            output = tao::json::to_string(val);

            auto end = high_resolution_clock::now();
            time_to_write = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
    }
    size_t final_memory_used = get_process_memory();
    
    measurements results;
    results.library_name = name();
    results.memory_used = (end_memory_used - start_memory_used);
    results.time_to_read = time_to_read;
    results.time_to_write = time_to_write;
    return results;
}

measurements taojson_benchmarks::measure_big(const char *input_filename, const char* output_filename)
{
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;
    std::string buffer;

    {
        start_memory_used =  get_process_memory();

        tao::json::value val;
        {
            auto start = high_resolution_clock::now();
            try
            {
                std::fstream is(input_filename);
                val = tao::json::from_stream(is, input_filename);
                auto end = high_resolution_clock::now();
                time_to_read = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            }
            catch (const std::exception& e)
            {
                std::cout << "[taojson_measure_big1] " << e.what() << std::endl;
            }
        }
        end_memory_used =  get_process_memory();
        {
            auto start = high_resolution_clock::now();

            try
            {
                std::ofstream os;
                os.open(output_filename, std::ios_base::out | std::ios_base::binary);
                os << val << std::endl;
                os.close();
            }
            catch (const std::exception& e)
            {
                std::cout << "[taojson_measure_big2] " << e.what() << std::endl;
            }

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

std::vector<test_suite_result> taojson_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
{
    std::vector<test_suite_result> results;
    for (auto& file : pathnames)
    {
#ifdef WIN32
        std::string command = "x64\\Release\\taojson_parser.exe ";
#elif defined(__linux__)
        std::string command = "./parsers/taojson_parser/build/taojson_parser ";
#else
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




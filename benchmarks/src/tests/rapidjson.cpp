#include <cstdio>
#include <chrono>
#include <fstream>
#include <iostream>
#include <cassert>
#include <vector>

#include "../measurements.hpp"
#include "../os_tools.hpp"
#include "json_benchmarks.hpp"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/writer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;

using namespace json_benchmarks;
using namespace rapidjson;

namespace json_benchmarks {

const std::string& rapidjson_benchmarks::name() const
{
    static const std::string s = "rapidjson";

    return s;
}

const std::string& rapidjson_benchmarks::url() const
{
    static const std::string s = "https://github.com/miloyip/rapidjson";

    return s;
}

const std::string& rapidjson_benchmarks::version() const
{
    static const std::string s = __STRINGIZE_VERSION(RAPIDJSON_MAJOR_VERSION, RAPIDJSON_MINOR_VERSION, RAPIDJSON_PATCH_VERSION);

    return s;
}

const std::string& rapidjson_benchmarks::notes() const
{
    static const std::string s = "Uses custom floating point parsing, expect faster parsing. Uses girsu3 for printing doubles, expect faster serializing. Uses custom allocation and flat map for objects, expect smaller memory footprint.";

    return s;
}

measurements rapidjson_benchmarks::measure_small(const std::string& input, std::string& output)
{
    size_t start_memory_used = 0;
    size_t end_memory_used = 0;
    size_t time_to_read = 0;
    size_t time_to_write = 0;

    start_memory_used =  get_process_memory();
    {
        Document d;
        try
        {
            auto start = high_resolution_clock::now();
            d.Parse(input.c_str());
            auto end = high_resolution_clock::now();
            time_to_read = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
            exit(1);
        }
        end_memory_used =  get_process_memory();
        {
            try
            {
                auto start = high_resolution_clock::now();
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                d.Accept(writer);
                output = buffer.GetString();
                auto end = high_resolution_clock::now();
                time_to_write = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
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

measurements rapidjson_benchmarks::measure_big(const char *input_filename, const char* output_filename)
{
    //std::cout << "rapidjson output_filename: " << output_filename << "\n";
    size_t start_memory_used = 0;
    size_t end_memory_used = 0;
    size_t time_to_read = 0;
    size_t time_to_write = 0;

    start_memory_used =  get_process_memory();
    {
        Document d;
        try
        {
            auto start = high_resolution_clock::now();
            FILE* fp = fopen(input_filename, "rb"); // non-Windows use "r"
            assert(fp);
            std::vector<char> readBuffer; 
            readBuffer.resize(65536);
            FileReadStream is(fp, readBuffer.data(), readBuffer.size());
            if (d.ParseStream(is).HasParseError()) 
            {
                fprintf(stderr, "\nError(offset %u): %s\n", 
                    (unsigned)d.GetErrorOffset(),
                    GetParseError_En(d.GetParseError()));
                exit(1);
            }
            auto end = high_resolution_clock::now();
            time_to_read = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            fclose(fp);
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
            exit(1);
        }
        end_memory_used =  get_process_memory();
        {
            try
            {
                FILE* fp = fopen(output_filename, "wb");
                if( !fp )
                {
                    printf( "The file '%s' was not opened\n", output_filename);
                    exit(1);
                }

                std::vector<char> writeBuffer;
                writeBuffer.resize(65536);
                FileWriteStream os(fp, writeBuffer.data(), writeBuffer.size());

                Writer<FileWriteStream> writer(os);
                auto start = high_resolution_clock::now();
                d.Accept(writer);
                auto end = high_resolution_clock::now();
                time_to_write = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                fclose(fp);        
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
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

void print(FILE* fp, const Value& val)
{
    char buffer[1000];
    FileWriteStream fws(fp, buffer, sizeof(buffer));
    Writer<FileWriteStream> writer(fws);
    val.Accept(writer);
    fws.Flush();
}

std::vector<test_suite_result> rapidjson_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
{
    std::vector<test_suite_result> results;
    for (auto& file : pathnames)
    {
        Document d;
#ifdef WIN32
        std::string command = "x64\\Release\\rapidjson_parser.exe ";
#elif defined(__linux__)
        std::string command = "./parsers/rapidjson/build/rapidjson_parser ";
#else
#   error "unknown OS"
#endif
        command = command + file.path.string();
        int result = std::system(command.c_str());
        if (file.type == expected_result::expect_success)
        {
            if (result == 0)
            {
                results.emplace_back(
                    test_suite_result{result_code::expected_result}
                );
            }
            else if (result == 1)
            {
                results.emplace_back(
                    test_suite_result{result_code::expected_success_parsing_failed}
                );
            }
            else
            {
                results.emplace_back(
                    test_suite_result{result_code::process_stopped}
                );
            }
        }
        else if (file.type == expected_result::expect_failure)
        {
            if (result == 0)
            {
                results.emplace_back(
                    test_suite_result{result_code::expected_failure_parsing_succeeded}
                );
            }
            else if (result == 1)
            {
                results.emplace_back(
                    test_suite_result{result_code::expected_result}
                );
            }
            else
            {
                results.emplace_back(
                    test_suite_result{result_code::process_stopped}
                );
            }
        }
        else if (file.type == expected_result::result_undefined)
        {
            if (result == 0)
            {
                results.emplace_back(
                    test_suite_result{result_code::result_undefined_parsing_succeeded}
                );
            }
            else if (result == 1)
            {
                results.emplace_back(
                    test_suite_result{result_code::result_undefined_parsing_failed}
                );
            }
            else
            {
                results.emplace_back(
                    test_suite_result{result_code::process_stopped}
                );
            }
        }
    }

    return results;
}
}

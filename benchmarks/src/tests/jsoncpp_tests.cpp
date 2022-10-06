#include <fstream>
#include <chrono>
#include <iostream>
#include <sstream>

#include "../measurements.hpp"
#include "../os_tools.hpp"
#include "json_benchmarks.hpp"

#include <json/json.h>

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;
using namespace json_benchmarks;
using namespace Json;

namespace json_benchmarks {

const std::string& jsoncpp_benchmarks::name() const
{
    static const std::string s = "jsoncpp";

    return s;
}

const std::string& jsoncpp_benchmarks::url() const
{
    static const std::string s = "https://github.com/open-source-parsers/jsoncpp";

    return s;
}

const std::string& jsoncpp_benchmarks::version() const
{
    static const std::string s = __STRINGIZE_VERSION(JSONCPP_VERSION_MAJOR, JSONCPP_VERSION_MINOR, JSONCPP_VERSION_PATCH);

    return s;
}

const std::string& jsoncpp_benchmarks::notes() const
{
    static const std::string s = "Uses std::map for both arrays and objects, expect larger memory footprint.";

    return s;
}

measurements jsoncpp_benchmarks::measure_small(const std::string& input, std::string& output)
{
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;

    {
        start_memory_used =  get_process_memory();

        Value root;
        {
            try
            {
                auto start = high_resolution_clock::now();
                Reader reader;
                if (!reader.parse(input, root))
                {
                    std::cerr << "jsoncpp failed." << std::endl;
                }
                auto end = high_resolution_clock::now();
                time_to_read = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
                exit(1);
            }
        }
        end_memory_used =  get_process_memory();
        {
            std::stringstream os;
            auto start = high_resolution_clock::now();


            Json::StreamWriterBuilder builder;
            builder.settings_["indentation"] = "";
            std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
            writer->write(root, &os);

            output = os.str();

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

measurements jsoncpp_benchmarks::measure_big(const char *input_filename, const char* output_filename)
{
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;

    {
        start_memory_used =  get_process_memory();

        Value root;
        {
            try
            {
                auto start = high_resolution_clock::now();
                std::ifstream is(input_filename);
                is >> root;
                //Reader reader;
                //if (!reader.parse(input_filename, root))
                //{
                //    std::cerr << "jsoncpp failed." << std::endl;
                //}
                auto end = high_resolution_clock::now();
                time_to_read = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
                exit(1);
            }
        }
        end_memory_used =  get_process_memory();
        {
            std::ofstream os;
            os.open(output_filename, std::ios_base::out | std::ios_base::binary);
            auto start = high_resolution_clock::now();

            Json::StreamWriterBuilder builder;
            builder.settings_["indentation"] = "";
            std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
            writer->write(root, &os);

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

std::vector<test_suite_result> jsoncpp_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
{
    std::vector<test_suite_result> results;
    for (auto& file : pathnames)
    {
        if (file.type == expected_result::expect_success)
        {
            try
            {
                Value val;
                std::istringstream is(file.text);
                is >> val;
                results.emplace_back(result_code::expected_result);
            }
            catch (const std::exception&)
            {
                results.emplace_back(result_code::expected_success_parsing_failed);
            }
        }
        else if (file.type == expected_result::expect_failure)
        {
            try
            {
                Value val;
                std::istringstream is(file.text);
                is >> val;
                results.emplace_back(result_code::expected_failure_parsing_succeeded);
            }
            catch (const std::exception&)
            {
                results.emplace_back(result_code::expected_result);
            }
        }
        else if (file.type == expected_result::result_undefined)
        {
            try
            {
                Value val;
                std::istringstream is(file.text);
                is >> val;
                results.emplace_back(result_code::result_undefined_parsing_succeeded);
            }
            catch (const std::exception&)
            {
                results.emplace_back(result_code::result_undefined_parsing_failed);
            }
        }
    }

    return results;
}
}




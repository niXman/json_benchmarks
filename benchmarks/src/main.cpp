#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstring>

#include "measurements.hpp"
#include "tests/json_benchmarks.hpp"
#include "json_parsing_test_reporter.hpp"
#include "data_generator.hpp"
#include "os_tools.hpp"

namespace fs = std::filesystem;

using namespace json_benchmarks;

void benchmarks_small_file(std::vector<benchmarks_ptr>& implementations)
{
    try
    {
        const char *filename = "data/input/small_file/small_file_text_array.json";

        size_t file_size = fs::file_size(filename);
        std::string input;
        input.resize(file_size);
        std::string output;
        {
            std::ifstream in(filename, std::ifstream::binary);
            in.read(&input[0], file_size);
        }
        //std::cout << input << std::endl;
        output.reserve(input.size()*2);

        std::ofstream os("report/performance_small_file_text_array.md");
        os << std::endl;
        os << "## Read and Write Time Comparison" << std::endl << std::endl;
        os << std::endl;
        os << "Input filename|Size (bytes)|Content" << std::endl;
        os << "---|---|---" << std::endl;
        os << filename << "|" << file_size << "|" << "Text,integers" << std::endl;
        os << std::endl;
        os << "Environment"
           << "|" << get_os_type() << ", " << get_cpu_type() << std::endl;
        os << "---|---" << std::endl;
        os << "Computer"
           << "|" << get_motherboard() << ", " << get_cpu() << ", " << get_ram() << std::endl;
        os << "Operating system"
           << "|" << get_os() << std::endl;
        os << "Compiler"
           << "|" << get_compiler() << std::endl;

        os << std::endl;

        os << "Library|Version" << std::endl;
        os << "---|---" << std::endl;
        for (const auto& val : implementations)
        {
            os << "[" << val->name() << "](" << val->url() << ")" << "|" << val->version() << std::endl;
        }
        os << std::endl;

        os << "Library|Time to read (milliseconds)|Time to write (milliseconds)|Memory footprint of json value (bytes)|Remarks" << std::endl;
        os << "---|---|---|---|---" << std::endl;

        std::vector<measurements> v(implementations.size());

        size_t number_times = 50000;
        for (size_t i = 0; i < number_times; ++i)
        {
            for (size_t j = 0; j < implementations.size(); ++j)
            {
                auto results = implementations[j]->measure_small(input,output);
                v[j].time_to_read += results.time_to_read;
                v[j].time_to_write += results.time_to_write;
                v[j].memory_used += results.memory_used;
            }
            output.clear();
        }
        for (size_t j = 0; j < implementations.size(); ++j)
        {
            os << v[j].library_name
               << "|" << v[j].time_to_read/(number_times)
               << "|" << v[j].time_to_write/(number_times)
               << "|" << v[j].memory_used/number_times
               << "|" << implementations[j]->notes()
               << std::endl; 
        }
        os << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void benchmarks_int(std::vector<benchmarks_ptr>& implementations)
{
    try
    {
        const char *filename = "data/output/persons.json";
        make_big_file(filename, 50000, 5000, 0, 0);

        size_t file_size = fs::file_size(filename);

        std::ofstream os("report/performance.md");
        os << std::endl;
        os << "## Read and Write Time Comparison" << std::endl << std::endl;
        os << std::endl;
        os << "Input filename|Size (MB)|Content" << std::endl;
        os << "---|---|---" << std::endl;
        os << filename << "|" << (file_size/1000000.0) << "|" << "Text,integers" << std::endl;
        os << std::endl;
        os << "Environment"
           << "|" << get_os_type() << ", " << get_cpu_type() << std::endl;
        os << "---|---" << std::endl;
        os << "Computer"
           << "|" << get_motherboard() << ", " << get_cpu() << ", " << get_ram() << std::endl;
        os << "Operating system"
           << "|" << get_os() << std::endl;
        os << "Compiler"
           << "|" << get_compiler() << std::endl;

        os << std::endl;

        os << "Library|Version" << std::endl;
        os << "---|---" << std::endl;

        for (const auto& val : implementations)
        {
            os << "[" << val->name() << "](" << val->url() << ")" << "|" << val->version() << std::endl;
        }
        os << std::endl;

        os << "Library|Time to read (s)|Time to write (s)|Memory footprint of json value (MB)|Remarks" << std::endl;
        os << "---|---|---|---|---" << std::endl;

        for ( const auto &impl: implementations )
        {
            std::string output_path = "data/output/persons_" + impl->name() + ".json";
            auto results = impl->measure_big("data/output/persons.json",output_path.c_str());
            os << "[" << impl->name() << "](" << impl->url() << ")"
               << "|" << (results.time_to_read/1000.0) 
               << "|" << (results.time_to_write/1000.0) 
               << "|" << (results.memory_used)
               << "|" << impl->notes()
               << std::endl; 
        }

        os << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void benchmarks_fp(std::vector<benchmarks_ptr>& implementations)
{
    try
    {
        const char *filename = "data/output/persons_fp.json";
        make_big_file(filename, 50000, 0, 2500, 0);

        size_t file_size = fs::file_size(filename);

        std::ofstream os("report/performance_fp.md");
        os << std::endl;
        os << "## Read and Write Time Comparison" << std::endl << std::endl;
        os << std::endl;
        os << "Input filename|Size (MB)|Content" << std::endl;
        os << "---|---|---" << std::endl;
        os << filename << "|" << (file_size/1000000.0) << "|" << "Text,doubles" << std::endl;
        os << std::endl;
        os << "Environment"
           << "|" << get_os_type() << ", " << get_cpu_type() << std::endl;
        os << "---|---" << std::endl;
        os << "Computer"
           << "|" << get_motherboard() << ", " << get_cpu() << ", " << get_ram() << std::endl;
        os << "Operating system"
           << "|" << get_os() << std::endl;
        os << "Compiler"
           << "|" << get_compiler() << std::endl;

        os << std::endl;

        os << "Library|Version" << std::endl;
        os << "---|---" << std::endl;
        for (const auto& val : implementations)
        {
            os << "[" << val->name() << "](" << val->url() << ")" << "|" << val->version() << std::endl;
        }
        os << std::endl;

        os << "Library|Time to read (s)|Time to write (s)|Memory footprint of json value (MB)|Remarks" << std::endl;
        os << "---|---|---|---|---" << std::endl;

        for ( const auto &impl: implementations )
        {
            std::string output_path = "data/output_fp/persons_" + impl->name() + ".json";
            auto results = impl->measure_big("data/output/persons_fp.json",output_path.c_str());
            os << "[" << impl->name() << "](" << impl->url() << ")"
               << "|" << (results.time_to_read/1000.0) 
               << "|" << (results.time_to_write/1000.0) 
               << "|" << (results.memory_used)
               << "|" << impl->notes()
               << std::endl; 
        }

        os << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void json_test_suite_parsing_tests(const std::vector<benchmarks_ptr>& implementations,
                                  json_parsing_test_visitor& visitor)
{
    try
    {
        std::vector<test_suite_file> pathnames;

        json_file_finder
        (
            "data/input/JSONTestSuite/test_parsing",
            [&](const fs::path& path) 
            {
                std::string buffer;
                {
                    size_t size = fs::file_size(path);
                    buffer.resize(size);
                    std::ifstream fs(path.string(), std::ios::in|std::ios::binary);
                    if (fs.is_open())
                    {
                        fs.read(&buffer[0], size);
                    }
                }
                char type = path.filename().string().c_str()[0];
                switch (type)
                {
                case 'y':
                    pathnames.push_back(test_suite_file{path,expected_result::expect_success,buffer});
                    break;
                case 'n':
                    pathnames.push_back(test_suite_file{path,expected_result::expect_failure,buffer});
                    break;
                case 'i':
                    pathnames.push_back(test_suite_file{path,expected_result::result_undefined,buffer});
                    break;
                }
            }
        );

        std::stable_sort(pathnames.begin(),pathnames.end(),
                         [](const test_suite_file& a, const test_suite_file& b) -> bool
        {
            return a.type < b.type; 
        }
        );

        std::vector<std::vector<test_suite_result>> results;
        for (auto& impl : implementations)
        {
            results.emplace_back(impl->run_test_suite(pathnames));
        }

        visitor.visit(pathnames,results);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void json_checker_parsing_tests(const std::vector<benchmarks_ptr>& implementations,
                                json_parsing_test_visitor& visitor)
{
    try
    {
        std::vector<test_suite_file> pathnames;

        json_file_finder
        (
            "data/input/JSON_checker",
            [&](const fs::path& path) 
            {
                std::string buffer;
                {
                    size_t size = fs::file_size(path);
                    buffer.resize(size);
                    std::ifstream fs(path.string(), std::ios::in|std::ios::binary);
                    if (fs.is_open())
                    {
                        fs.read(&buffer[0], size);
                    }
                }
                char type = path.filename().string().c_str()[0];
                switch (type)
                {
                case 'p':
                    pathnames.push_back(test_suite_file{path,expected_result::expect_success,buffer});
                    break;
                case 'f':
                    pathnames.push_back(test_suite_file{path,expected_result::expect_failure,buffer});
                    break;
                case 'i':
                    pathnames.push_back(test_suite_file{path,expected_result::result_undefined,buffer});
                    break;
                }
            }
        );

        std::stable_sort(pathnames.begin(),pathnames.end(),
                         [](const test_suite_file& a, const test_suite_file& b)
        {
            return a.type < b.type; 
        }
        );

        std::vector<std::vector<test_suite_result>> results;
        for (auto& impl : implementations)
        {
            results.emplace_back(impl->run_test_suite(pathnames));
        }

        visitor.visit(pathnames,results);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void usage(const char *argv0) {
    const char *p = std::strrchr(argv0, fs::path::preferred_separator);
    p = (p ? p+1: argv0);

    std::cout
    << p << " [int, float, smallfile]" << std::endl
    << "  int       - use integers on generated BIG test data's" << std::endl
    << "  float     - use floats on generated BIG test data's" << std::endl
    << "  smallfile - test using small test data's" << std::endl
    << "--- can be used together ---" << std::endl
    << std::endl;
}

int main(int argc, char **argv)
{
    if ( argc < 2 ) {
        usage(argv[0]);

        return EXIT_FAILURE;
    }

    bool test_int = false;
    bool test_float = false;
    bool smallfile = false;
    for ( int i = 1; i < argc; ++i ) {
        if ( std::strcmp(argv[i], "int") == 0 ) {
            test_int = true;
        } else if ( std::strcmp(argv[i], "float") == 0 ) {
            test_float = true;
        } else if ( std::strcmp(argv[i], "smallfile") == 0 ) {
            smallfile = true;
        } else {
            std::cout << "wrong command line: \"" << argv[i] << "\"" << std::endl;
            usage(argv[0]);

            return EXIT_FAILURE;
        }
    }

    std::vector<benchmarks_ptr> implementations{
         std::make_shared<jsoncons_benchmarks>()
        ,std::make_shared<nlohmann_benchmarks>()
        ,std::make_shared<cjson_benchmarks>()
        ,std::make_shared<json11_benchmarks>()
        ,std::make_shared<rapidjson_benchmarks>()
        ,std::make_shared<jsoncpp_benchmarks>()
        ,std::make_shared<json_spirit_benchmarks>()
        ,std::make_shared<taojson_benchmarks>()
    };

    if ( test_int ) {
        benchmarks_int(implementations);
    }
    if ( test_float ) {
        benchmarks_fp(implementations);
    }
    if ( smallfile ) {
        benchmarks_small_file(implementations);
    }

#if 0
    std::vector<result_code_info> result_code_infos;
    result_code_infos.push_back(result_code_info{result_code::expected_result,"Expected result","#008000"});
    result_code_infos.push_back(result_code_info{result_code::expected_success_parsing_failed,"Expected success, parsing failed","#d19b73"});
    result_code_infos.push_back(result_code_info{result_code::expected_failure_parsing_succeeded,"Expected failure, parsing succeeded","#001a75"});
    result_code_infos.push_back(result_code_info{result_code::result_undefined_parsing_succeeded,"Result undefined, parsing succeeded","#f7a8ff"});
    result_code_infos.push_back(result_code_info{result_code::result_undefined_parsing_failed,"Result undefined, parsing failed","#050f07"});
    result_code_infos.push_back(result_code_info{result_code::process_stopped,"Process stopped","#e00053"});

    std::ofstream fs("docs/index.html");
    json_parsing_test_reporter reporter("Parser Comparisons", implementations, result_code_infos, fs);
    reporter.register_test("JSON Test Suite",json_test_suite_parsing_tests);
    reporter.register_test("JSON Checker",json_checker_parsing_tests);
    reporter.run_tests();
#endif // 0

    return 0;
}


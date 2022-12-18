
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstring>
#include <cassert>

#include "measurements.hpp"
#include "benchmarks.hpp"
//#include "json_parsing_test_reporter.hpp"
#include "data_generator.hpp"
#include "os_tools.hpp"
#include "io_device.hpp"

#include <malloc-stat/api.h>
#include <cmdargs/cmdargs.hpp>

namespace fs = std::filesystem;

using namespace json_benchmarks;

malloc_stat_get_stat_fnptr get_alloc_stat = nullptr;

/*************************************************************************************************/

#ifdef SMALL_FILE_TEST_ENABLED
bool benchmarks_small_file(
     std::size_t iter_times
    ,const benchmarks_list &implementations
    ,const std::string &input_fname
    ,const std::string &output_dir)
{
    try {
        std::ofstream os("report/performance_small_file_text_array.md");
        os << std::endl;
        os << "## Read and Write Time Comparison" << std::endl << std::endl;
        os << std::endl;
        os << "Input filename|Size (bytes)|Content" << std::endl;
        os << "---|---|---" << std::endl;
        os << input_fname << "|" << file_size(input_fname.c_str()) << "|" << "Text,integers" << std::endl;
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

        os << "Library|Time to read ms|Time to write ms|Memory footprint on read|Memory footprint on write|Allocations on read|Allocations on write|Remarks" << std::endl;
        os << "---|---|---|---|---|---|---|---" << std::endl;

        std::vector<measurements> measurements_res(implementations.size());

        for (size_t i = 0; i < iter_times; ++i) {
            for (size_t j = 0; j < implementations.size(); ++j) {
                auto &impl = implementations[j];
                auto output_fname = output_dir;
                output_fname += "/";
                output_fname += output_fname_prefix;
                output_fname += impl->name();
                output_fname += ".json";

                auto input_io = create_input(impl->input_io_type(), input_fname);
                auto output_io = create_output(impl->output_io_type(), output_fname);

                auto results = impl->measure(*input_io, *output_io);
                if ( !results.errmsg.empty() ) {
                    std::cerr << "the benchmark for \"" << results.name << "\" was failed with error: "
                              << results.errmsg << std::endl
                    ;

                    return false;
                }

                measurements_res[j].read_memory_used += results.read_memory_used;
                measurements_res[j].read_allocations += results.read_allocations;
                measurements_res[j].write_memory_used += results.write_memory_used;
                measurements_res[j].write_allocations += results.write_allocations;
                measurements_res[j].time_to_read += results.time_to_read;
                measurements_res[j].time_to_write += results.time_to_write;
            }
        }

        for (size_t j = 0; j < measurements_res.size(); ++j) {
            auto &it = measurements_res[j];
            os << it.name
            << "|" << it.time_to_read/(iter_times)
            << "|" << it.time_to_write/(iter_times)
            << "|" << it.read_memory_used/iter_times
            << "|" << it.write_memory_used/iter_times
            << "|" << it.read_allocations/iter_times
            << "|" << it.write_allocations/iter_times
            << "|" << implementations[j]->notes()
            << std::endl;
        }
        os << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "benchmarks_small_file: what: " << e.what() << std::endl;

        return false;
    }

    return true;
}
#endif // SMALL_FILE_TEST_ENABLED

/*************************************************************************************************/

bool benchmark(
     const benchmarks_list &implementations
    ,const std::string &report_fname
    ,const std::string &input_fname
    ,const std::string &output_dir
    ,std::size_t json_flags)
{
    try {
        auto fsize = file_size(input_fname.c_str());

        std::ofstream os{report_fname};
        os << std::endl;
        os << "## Read and Write Time Comparison" << std::endl << std::endl;
        os << std::endl;
        os << "Input filename|Size (MB)|Content" << std::endl;
        os << "---|---|---" << std::endl;
        os << input_fname << "|" << (fsize/1000000.0) << "|" << "Text,doubles" << std::endl;
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
        for ( const auto& val : implementations ) {
            os << "[" << val->name() << "](" << val->url() << ")" << "|" << val->version() << std::endl;
        }
        os << std::endl;

        os << "Library|Time to read s|Time to write s|Memory footprint on read MB|Memory footprint on write MB|Allocations on read|Allocations on write|Remarks" << std::endl;
        os << "---|---|---|---|---|---|---|---" << std::endl;

        for ( const auto &impl: implementations ) {
            std::cout << "  name: " << impl->name() << std::endl;

            measurements stat;
            stat.name = impl->name();

            auto [input_io, output_io] = impl->create_io(input_fname);
            output_io->reserve(input_io->size() * 2);

            ///////////////////////////////////////////////////////// prepare
            std::cout << "    prepairing... " << std::flush;

            auto prepare_start = impl->start_time();
            MALLOC_STAT_RESET_STAT(get_alloc_stat);

            impl->prepare(input_io.get(), json_flags);

            malloc_stat_vars prepare_stat = MALLOC_STAT_GET_STAT(get_alloc_stat);
            auto prepare_time = impl->duration(prepare_start);

            std::cout << "done" << std::endl;
            ///////////////////////////////////////////////////////// parse
            std::cout << "    parsing... " << std::flush;

            auto parse_start = impl->start_time();
            MALLOC_STAT_RESET_STAT(get_alloc_stat);

            auto [parse_ok, parse_err] = impl->parse(input_io.get(), json_flags);
            if ( !parse_ok ) {
                stat.errmsg = parse_err;

                std::cerr
                    << std::endl
                    << "the PARSE benchmark for \"" << impl->name() << "\" finished with error: "
                    << parse_err << std::endl
                ;

                return false;
            }

            malloc_stat_vars parse_stat = MALLOC_STAT_GET_STAT(get_alloc_stat);
            auto parse_time = impl->duration(parse_start);

            std::cout << "done" << std::endl;
            ///////////////////////////////////////////////////////// print
            std::cout << "    printing... " << std::flush;

            auto print_start = impl->start_time();
            MALLOC_STAT_RESET_STAT(get_alloc_stat);

            if ( parse_ok ) {
                auto [print_ok, print_err] = impl->print(output_io.get(), json_flags);
                if ( !print_ok ) {
                    stat.errmsg = print_err;

                    std::cerr
                        << std::endl
                        << "the PRINT benchmark for \"" << impl->name() << "\" finished with error: "
                        << print_err << std::endl
                    ;

                    return false;
                }
            }

            malloc_stat_vars print_stat = MALLOC_STAT_GET_STAT(get_alloc_stat);
            auto print_time = impl->duration(print_start);

            std::cout << "done" << std::endl;
            ///////////////////////////////////////////////////////// free
            std::cout << "    free... " << std::flush;

            auto free_start = impl->start_time();
            MALLOC_STAT_RESET_STAT(get_alloc_stat);

            impl->finish();

            malloc_stat_vars free_stat = MALLOC_STAT_GET_STAT(get_alloc_stat);
            auto free_time = impl->duration(free_start);

            std::cout << "done" << std::endl;
            ///////////////////////////////////////////////////////// check
            std::cout << "    comparing... " << std::flush;
            auto check_start = impl->start_time();

            //auto check_res = impl->check(input_io.get(), output_io.get(), json_flags);

            auto check_time = impl->duration(check_start);

            std::cout << "done, took " << check_time/1000.0 << " s" << std::endl;
            ///////////////////////////////////////////////////////// end

            stat.time_to_prepare = prepare_time;
            stat.prepare_allocated = prepare_stat.allocated;
            stat.prepare_allocations = prepare_stat.allocations;
            stat.prepare_deallocations = prepare_stat.deallocations;
            stat.time_to_parse = parse_time;
            stat.parse_allocated = parse_stat.allocated;
            stat.parse_allocations = parse_stat.allocations;
            stat.parse_deallocations = parse_stat.deallocations;
            stat.time_to_print = print_time;
            stat.print_allocated = print_stat.allocated;
            stat.print_allocations = print_stat.allocations;
            stat.print_deallocations = print_stat.deallocations;
            stat.time_to_free = free_time;

            stat.free_deallocated = free_stat.deallocated;
            stat.free_deallocations = free_stat.deallocations;

            auto summ_of_allocs = prepare_stat.allocations + parse_stat.allocations + print_stat.allocations + free_stat.allocations;
            auto summ_of_allocated = prepare_stat.allocated + parse_stat.allocated + print_stat.allocated + free_stat.allocated;
            auto summ_of_deallocs = prepare_stat.deallocations + parse_stat.deallocations + print_stat.deallocations + free_stat.deallocations;
            auto summ_of_deallocated = prepare_stat.deallocated + parse_stat.deallocated + print_stat.deallocated + free_stat.deallocated;
            stat.free_leaked_bytes = summ_of_allocated - summ_of_deallocated;
            stat.free_leaked_allocations = summ_of_allocs - summ_of_deallocs;

            std::cout << stat;

            auto allowed_leaks = impl->allowed_leaks();
            if ( allowed_leaks.first != stat.free_leaked_bytes || allowed_leaks.second != stat.free_leaked_allocations ) {
                std::cerr
                    << "  WARN: leaked memory suspicion ("
                    << stat.free_leaked_bytes << " bytes, "
                    << stat.free_leaked_allocations << " in blocks)"
                << std::endl;
            } else {
                if ( stat.free_leaked_bytes ) {
                    std::cerr << " (as expected!)" << std::endl;
                } else {
                    std::cerr << std::endl;
                }
            }

            // when we are faced with JSON test mismatch - there is no reason
            // for inclusion that test results into the report...
            if ( 0 /*!check_res.first*/ ) {
                std::cerr
                    << "  WARN: the comparison test for the SOURCE and GENERATED JSON was failed with error: " << "check_res.second" << "\"\n"
                    << "  the results of that test will not be included into the report!"
                << std::endl;
            } else {
    //            os << "Library|Time to read s|Time to write s|Memory footprint on read MB|Memory footprint on write MB|Allocations on read|Allocations on write|Remarks" << std::endl;
    //            os << "---|---|---|---|---|---|---|---" << std::endl;

    //            os
    //                << "[" << impl->name() << "](" << impl->url() << ")"
    //                << "|" << stat.time_to_parse
    //                << "|" << stat.time_to_print
    //                << "|" << stat.parse_memory_used
    //                << "|" << stat.print_memory_used
    //                << "|" << stat.parse_allocations
    //                << "|" << stat.print_allocations
    //                << "|" << impl->notes()
    //                << std::endl
    //            ;
            }
        }
        os << std::endl;
    } catch (const std::exception &e) {
        std::cout << "benchmarks error: " << e.what() << std::endl;

        return false;
    }

    return true;
}

/*************************************************************************************************/

#if 0
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
#endif

/*************************************************************************************************/

void usage(const char *argv0) {
    const char *p = std::strrchr(argv0, '/');
    p = (p ? p+1: argv0);

    std::cout
        << p << " [ints, floats, strings, mixed, smallfile]" << std::endl
        << "  ints      - use integers for generate test data" << std::endl
        << "  floats    - use floats for generate test data" << std::endl
        << "  strings   - use strings for generate test data" << std::endl
        << "  keywords  - use JSON keywords for generate test data" << std::endl
        << "  mixed     - use mixed mode for generate test data" << std::endl
        << "  smallfile - test using small test data" << std::endl
        << "  despaced  - generated test data will not contain any spaces" << std::endl
        << "--- can be used together ---" << std::endl
        << std::endl
    ;
}

int main(int argc, char **argv) {
    get_alloc_stat = MALLOC_STAT_GET_STAT_FNPTR();
    assert(get_alloc_stat);
    assert(MALLOC_STAT_CHECK_VERSION());

    if ( argc < 2 ) {
        usage(argv[0]);

        return EXIT_FAILURE;
    }

    struct kwords: cmdargs::kwords_group {
        CMDARGS_OPTION_ADD(mode, e_data_generator_mode::k_e, "test mode selector"
            ,validator_([](const char *str, std::size_t len){
                for ( const auto &it: s_data_generator_mode ) {
                    if ( std::strncmp(it, str, len) == 0 ) {
                        return true;
                    }
                }

                return false;
            })
            ,converter_([](void *dstptr, const char *str, std::size_t len){
                auto &dst = *static_cast<e_data_generator_mode::k_e *>(dstptr);
                std::string s{str, len};
                dst = s == s_data_generator_mode[0]
                    ? e_data_generator_mode::ints
                    : s == s_data_generator_mode[1]
                        ? e_data_generator_mode::floats
                        : s == s_data_generator_mode[2]
                            ? e_data_generator_mode::strings
                            : s == s_data_generator_mode[3]
                                ? e_data_generator_mode::keywords
                                : s == s_data_generator_mode[4]
                                    ? e_data_generator_mode::mixed
                                    : e_data_generator_mode::smallfile
                ;

                return true;
            })
        );
        CMDARGS_OPTION_ADD(despaced, bool, "test data will be generated as compacted JSON", optional);
        CMDARGS_OPTION_ADD(num_ints, std::size_t, "number of integers in generated JSON", optional);
        CMDARGS_OPTION_ADD(num_floats, std::size_t, "number of floats in generated JSON", optional);
        CMDARGS_OPTION_ADD(num_strings, std::size_t, "number of strings in generated JSON", optional);
        CMDARGS_OPTION_ADD(num_keywords, std::size_t, "number of keywords in generated JSON", optional);
        CMDARGS_OPTION_ADD(num_repeats, std::size_t, "number of strings in generated JSON", optional);

        CMDARGS_OPTION_ADD_HELP();
        CMDARGS_OPTION_ADD_VERSION();
    } static const kwords;

    std::string emsg;
    auto args = cmdargs::parse_args(&emsg, argc, argv, kwords);
    if ( !emsg.empty() ) {
        std::cout << "cmdline parse error: " << emsg << std::endl;

        return EXIT_FAILURE;
    }
    if ( args.is_set(kwords.help) ) {
        cmdargs::show_help(std::cout, argv[0], args);

        return EXIT_SUCCESS;
    }
    if ( args.is_set(kwords.version) ) {
        std::cout << "version: " << std::endl;

        return EXIT_SUCCESS;
    }

    const auto mode        = args.get(kwords.mode);
    const auto despaced    = args.get(kwords.despaced, false);
    const auto num_ints    = args.get(kwords.num_ints, 5000);
    const auto num_floats  = args.get(kwords.num_floats, 5000);
    const auto num_strings = args.get(kwords.num_strings, 5000);
    const auto num_keywords= args.get(kwords.num_keywords, 5000);
    const auto num_repeats = args.get(kwords.num_repeats, 5000);
    std::cout
        << kwords.mode.name() << ": " << mode << ", "
        << kwords.despaced.name() << ": " << despaced << ", "
        << kwords.num_ints.name() << ": " << num_ints << ", "
        << kwords.num_floats.name() << ": " << num_floats << ", "
        << kwords.num_strings.name() << ": " << num_strings << ", "
        << kwords.num_keywords.name() << ": " << num_keywords << ", "
        << kwords.num_repeats.name() << ": " << num_repeats << std::endl
    ;

    static const std::string test_file_fname = "data/output/testdata.json";
    std::string output_dir = fs::path{test_file_fname}.parent_path();
    if ( !fs::exists(output_dir) ) {
        fs::create_directories(output_dir);
    }

    std::cout << "test file (" << test_file_fname << ") generation..." << std::flush;
    std::size_t flags = mode | (despaced ? e_data_generator_mode::compacted : 0u);
    auto time_to_write = make_test_file(
         test_file_fname
        ,num_repeats
        ,num_ints
        ,num_floats
        ,num_strings
        ,num_keywords
        ,flags
    );
    std::cout << "took " << (time_to_write/1000.0) << " seconds, "
              << human_size(fs::file_size(test_file_fname)) << " bytes" << std::endl;

    std::string report_fname;
    switch ( mode ) {
        case e_data_generator_mode::ints: {
            report_fname = "reports/ints.md";
            break;
        }
        case e_data_generator_mode::floats: {
            report_fname = "reports/floats.md";
            break;
        }
        case e_data_generator_mode::strings: {
            report_fname = "reports/strings.md";
            break;
        }
        case e_data_generator_mode::keywords: {
            report_fname = "reports/keywords.md";
            break;
        }
        case e_data_generator_mode::mixed: {
            report_fname = "reports/mixed.md";
            break;
        }
        case e_data_generator_mode::smallfile: {
            report_fname = "reports/smallfile.md";
            break;
        }
        default: assert("wrong mode" == nullptr);
    }

    std::cout << "ints test started..." << std::endl;
    std::size_t json_flags = 0;
    json_flags = despaced ? (json_flags | e_json_flags::despaced) : 0u;

    auto benchmarks = create_benchmarks();
    if ( !benchmark(
         benchmarks
        ,report_fname
        ,test_file_fname
        ,output_dir
        ,json_flags)
    ) {
        return EXIT_FAILURE;
    }

#ifdef SMALL_FILE_TEST_ENABLED
    if ( small_file ) {
        const std::string input_fname = "data/input/small_file/small_file_text_array.json";
        const std::string output_dir = "data/output/small_file";
        const std::string output_fname_prefix = "small_file_text_array_";
        size_t number_times = 50000;

        if ( !fs::exists(output_dir) ) {
            fs::create_directories(output_dir);
        }

        std::cout << "small file test started, " << number_times << " times..." << std::endl;

        if ( !benchmarks_small_file(
                  number_times
                 ,implementations
                 ,input_fname
                 ,output_dir
                 ,output_fname_prefix)
        ) {
            return EXIT_FAILURE;
        }
    }
#endif // SMALL_FILE_TEST_ENABLED

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

    return EXIT_SUCCESS;
}

/*************************************************************************************************/

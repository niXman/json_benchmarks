
#include "jsoncons.hpp"
#include "../stringize.hpp"

#include <jsoncons/json.hpp>
#include <jsoncons/json_reader.hpp>

namespace json_benchmarks {

io_type jsoncons_benchmarks::input_io_type() const { return io_type::mmap_streams; }
io_type jsoncons_benchmarks::output_io_type() const { return io_type::string_buffer; }

const char* jsoncons_benchmarks::name() const { return "jsoncons"; }

const char* jsoncons_benchmarks::url() const { return "https://github.com/danielaparker/jsoncons"; }

const char* jsoncons_benchmarks::version() const {
    return __STRINGIZE_VERSION(JSONCONS_VERSION_MAJOR, JSONCONS_VERSION_MINOR, JSONCONS_VERSION_PATCH);
}

const char* jsoncons_benchmarks::notes() const {
    return "Uses sorted `std::vector` of key/value pairs for objects, expect smaller memory footprint."
           "Uses slightly modified [grisu3_59_56 implementation by Florian Loitsch](https://florian.loitsch.com/publications) "
           "plus fallback for printing doubles, expect faster serializing."
    ;
}

static jsoncons::json *local_obj = nullptr;

void jsoncons_benchmarks::prepare(io_device *in, std::size_t flags) const {
    local_obj = new jsoncons::json;
}

std::pair<bool, std::string>
jsoncons_benchmarks::parse(io_device *in, std::size_t flags) {
    auto *input  = in->input_io<io_type::mmap_streams>();
    auto pair = input->stream();

    std::string err;
    try {
        *local_obj = jsoncons::json::parse(pair.first, pair.second);
    } catch (const std::exception &ex) {
        err = ex.what();
    }

    if ( !err.empty() ) {
        return {false, err};
    }

    return {true, err};
}

std::pair<bool, std::string>
jsoncons_benchmarks::print(io_device *out, std::size_t flags) {
    auto *output = out->output_io<io_type::string_buffer>();
    auto &string = output->stream();

    std::string err;
    try {
        local_obj->dump(string);
    } catch (const std::exception &ex) {
        err = ex.what();
    }

    return {err.empty(), std::move(err)};
}

void jsoncons_benchmarks::finish() const {
    delete local_obj;
    local_obj = nullptr;
}

#if 0
const std::string& jsoncons_benchmarks::name() const
{
    static const std::string s = "jsoncons";

    return s;
}

const std::string& jsoncons_benchmarks::url() const
{
    static const std::string s = "https://github.com/danielaparker/jsoncons";

    return s;
}

const std::string& jsoncons_benchmarks::version() const
{
    static const std::string s = __STRINGIZE_VERSION(JSONCONS_VERSION_MAJOR, JSONCONS_VERSION_MINOR, JSONCONS_VERSION_PATCH);

    return s;
}

const std::string& jsoncons_benchmarks::notes() const
{
    static const std::string s = "Uses sorted `std::vector` of key/value pairs for objects, expect smaller memory footprint.Uses slightly modified [grisu3_59_56 implementation by Florian Loitsch](https://florian.loitsch.com/publications) plus fallback for printing doubles, expect faster serializing.";

    return s;
}

measurements jsoncons_benchmarks::measure_small(const std::string& input, std::string& output)
{
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;

    {
        start_memory_used =  get_process_memory();
        {
            jsoncons::json root;
            {
                auto start = high_resolution_clock::now();
                try
                {
                    root = jsoncons::json::parse(input.data(),input.length());
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
                auto start = high_resolution_clock::now();
                root.dump(output);
                auto end = high_resolution_clock::now();
                time_to_write = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            }
        }
    }
    size_t final_memory_used = get_process_memory();
    
    measurements results;
    results.library_name = name();
    results.memory_used = end_memory_used > start_memory_used ? end_memory_used - start_memory_used : 0;
    results.time_to_read = time_to_read;
    results.time_to_write = time_to_write;
    return results;
}

measurements jsoncons_benchmarks::measure_big(const char *input_filename, const char* output_filename)
{
    //std::cout << "jsoncons output_filename: " << output_filename << "\n";
    size_t start_memory_used;
    size_t end_memory_used;
    size_t time_to_read;
    size_t time_to_write;

    {
        start_memory_used =  get_process_memory();
        {
            jsoncons::json root;
            {
                auto start = high_resolution_clock::now();
                try
                {
                    std::ifstream is(input_filename);
                    root = jsoncons::json::parse(is);
                }
                catch (const std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                    exit(1);
                }
                time_to_read = std::chrono::duration_cast<std::chrono::milliseconds>(
                    high_resolution_clock::now() - start
                ).count();
            }
            end_memory_used =  get_process_memory();
            {
                auto start = high_resolution_clock::now();

                std::ofstream os;
                os.open(output_filename, std::ios_base::out | std::ios_base::binary);
                root.dump(os);

                time_to_write = std::chrono::duration_cast<std::chrono::milliseconds>(
                    high_resolution_clock::now() - start
                ).count();
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

std::vector<test_suite_result> jsoncons_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
{
    std::vector<test_suite_result> results;
    for (auto& file : pathnames)
    {
        strict_json_parsing err_handler;
        if (file.type == expected_result::expect_success)
        {
            if (file.path.filename().string().find("utf16") != std::string::npos)
            {
                try
                {
                    std::wifstream fin(file.path.string().c_str(), std::ios::binary);
                    // apply BOM-sensitive UTF-16 facet
                    fin.imbue(std::locale(fin.getloc(),
                                          new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
                    wjson document;
                    fin >> document;
                    results.emplace_back(result_code::expected_result);
                }
                catch (const std::exception&)
                {
                    results.emplace_back(result_code::expected_success_parsing_failed);
                }
            }
            else
            {
                try
                {
                    std::istringstream is(file.text);
                    json val = json::parse(is,err_handler);
                    results.emplace_back(result_code::expected_result);
                }
                catch (const std::exception&)
                {
                    results.emplace_back(result_code::expected_success_parsing_failed);
                }
            }
        }
        else if (file.type == expected_result::expect_failure)
        {
            try
            {
                std::istringstream is(file.text);
                json val = json::parse(is,err_handler);
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
                std::istringstream is(file.text);
                json val = json::parse(is,err_handler);
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
#endif

}

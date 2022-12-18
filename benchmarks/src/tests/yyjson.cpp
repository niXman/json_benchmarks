
#include "yyjson.hpp"
#include "../stringize.hpp"

#include <yyjson.h>

namespace json_benchmarks {

io_type yyjson_benchmarks::input_io_type() const { return io_type::mmap_streams; }
io_type yyjson_benchmarks::output_io_type() const { return io_type::string_buffer; }

const char* yyjson_benchmarks::name() const { return "yyjson"; }

const char* yyjson_benchmarks::url() const { return "https://github.com/ibireme/yyjson"; }

const char* yyjson_benchmarks::version() const { return YYJSON_VERSION_STRING; }

const char* yyjson_benchmarks::notes() const {
    return
        "very fast non-simd implementation"
    ;
}

static yyjson_doc *local_obj = nullptr;

void yyjson_benchmarks::prepare(io_device */*in*/, std::size_t /*flags*/) const {
}

std::pair<bool, std::string>
yyjson_benchmarks::parse(io_device *in, std::size_t flags) {
    auto *istream  = in->input_io<io_type::mmap_streams>();
    auto pair = istream->stream();

    yyjson_read_err errv;
    local_obj = yyjson_read_opts(pair.first, pair.second, 0, nullptr, &errv);

    std::string err;
    if ( errv.code != YYJSON_READ_SUCCESS ) {
        err = errv.msg;

        return {false, err};
    }

    return {true, err};
}

std::pair<bool, std::string>
yyjson_benchmarks::print(io_device *out, std::size_t flags) {
    auto *ostream = out->output_io<io_type::string_buffer>();
    auto &string = ostream->stream();

    std::size_t written;
    char *ptr = yyjson_write(local_obj, 0, &written);

    std::string err;
    if ( !ptr ) {
        err = "write error";

        return {false, err};
    }

    string = ptr;
    free(ptr);

    return {true, err};
}

void yyjson_benchmarks::finish() const {
    yyjson_doc_free(local_obj);
}

#if 0
const std::string& yyjson_benchmarks::name() const
{
    static const std::string s = "jsoncons";

    return s;
}

const std::string& yyjson_benchmarks::url() const
{
    static const std::string s = "https://github.com/danielaparker/jsoncons";

    return s;
}

const std::string& yyjson_benchmarks::version() const
{
    static const std::string s = __STRINGIZE_VERSION(JSONCONS_VERSION_MAJOR, JSONCONS_VERSION_MINOR, JSONCONS_VERSION_PATCH);

    return s;
}

const std::string& yyjson_benchmarks::notes() const
{
    static const std::string s = "Uses sorted `std::vector` of key/value pairs for objects, expect smaller memory footprint.Uses slightly modified [grisu3_59_56 implementation by Florian Loitsch](https://florian.loitsch.com/publications) plus fallback for printing doubles, expect faster serializing.";

    return s;
}

measurements yyjson_benchmarks::measure_small(const std::string& input, std::string& output)
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

measurements yyjson_benchmarks::measure_big(const char *input_filename, const char* output_filename)
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

std::vector<test_suite_result> yyjson_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
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

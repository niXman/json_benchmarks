
#include "flatjson.hpp"
#include "../stringize.hpp"

#include <flatjson/flatjson.hpp>

namespace json_benchmarks {

io_type flatjson_benchmarks::input_io_type() const { return io_type::mmap_streams; }
io_type flatjson_benchmarks::output_io_type() const { return io_type::string_buffer; }

const char* flatjson_benchmarks::name() const { return "flatjson"; }

const char* flatjson_benchmarks::url() const { return "https://github.com/niXman/flatjson"; }

const char* flatjson_benchmarks::version() const { return FJ_VERSION_STRING; }

const char* flatjson_benchmarks::notes() const {
    return
        "it can work without any allocation when the number of tokens known in advance. "
        "in the usual case the parser will count the number of tokens first and then will allocate the required number of tokens at once. "
        "the downside here may seem to be the size of the memory required for the token."
    ;
}

static flatjson::parser *local_obj = nullptr;

void flatjson_benchmarks::prepare(io_device *in, std::size_t flags) const {
    auto *input  = in->input_io<io_type::mmap_streams>();
    auto pair = input->stream();

    bool despaced = (flags & e_json_flags::despaced) != 0;
    local_obj = flatjson::alloc_parser(pair.first, pair.first + pair.second, despaced);
}

std::pair<bool, std::string>
flatjson_benchmarks::parse(io_device *in, std::size_t flags) {
    bool despaced = (flags & e_json_flags::despaced) != 0;
    flatjson::parse(local_obj, despaced);

    std::string err;
    if ( !flatjson::is_valid(local_obj) ) {
        err = flatjson::get_error_message(local_obj);

        return {false, err};
    }

#ifdef __FJ__ANALYZE_PARSER
    flatjson::dump_parser_stat(local_obj);
#endif

    return {true, err};
}

std::pair<bool, std::string>
flatjson_benchmarks::print(io_device *out, std::size_t flags) {
    auto *output = out->output_io<io_type::string_buffer>();
    auto &string = output->stream();
    auto reserved = string.capacity();
    string.resize(reserved);

    std::string err;
    auto beg = flatjson::iter_begin(local_obj);
    auto end = flatjson::iter_end(local_obj);
    auto wr = flatjson::serialize(beg, end, string.data(), string.capacity());
    string.resize(wr);

    return {err.empty(), err};
}

void flatjson_benchmarks::finish() const {
    flatjson::free_parser(local_obj);
    local_obj = nullptr;
}

#if 0
const std::string& flatjson_benchmarks::name() const
{
    static const std::string s = "jsoncons";

    return s;
}

const std::string& flatjson_benchmarks::url() const
{
    static const std::string s = "https://github.com/danielaparker/jsoncons";

    return s;
}

const std::string& flatjson_benchmarks::version() const
{
    static const std::string s = __STRINGIZE_VERSION(JSONCONS_VERSION_MAJOR, JSONCONS_VERSION_MINOR, JSONCONS_VERSION_PATCH);

    return s;
}

const std::string& flatjson_benchmarks::notes() const
{
    static const std::string s = "Uses sorted `std::vector` of key/value pairs for objects, expect smaller memory footprint.Uses slightly modified [grisu3_59_56 implementation by Florian Loitsch](https://florian.loitsch.com/publications) plus fallback for printing doubles, expect faster serializing.";

    return s;
}

measurements flatjson_benchmarks::measure_small(const std::string& input, std::string& output)
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

measurements flatjson_benchmarks::measure_big(const char *input_filename, const char* output_filename)
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

std::vector<test_suite_result> flatjson_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
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

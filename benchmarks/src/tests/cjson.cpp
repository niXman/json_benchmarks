
#include "cjson.hpp"
#include "../stringize.hpp"

#include <cstring>
#include <cJSON.h>
#include <unistd.h>

namespace json_benchmarks {

io_type cjson_benchmarks::input_io_type() const { return io_type::mmap_streams; }
io_type cjson_benchmarks::output_io_type() const { return io_type::string_buffer; }

const char* cjson_benchmarks::name() const { return "cJSON"; }

const char* cjson_benchmarks::url() const { return "https://github.com/DaveGamble/cJSON"; }

const char* cjson_benchmarks::version() const {
    return __STRINGIZE_VERSION(CJSON_VERSION_MAJOR, CJSON_VERSION_MINOR, CJSON_VERSION_PATCH);
}

const char* cjson_benchmarks::notes() const {
    return "Inefficient storage (items do not share the same space), expect larger memory footprint. "
           "Uses sprintf and sscanf to support locale-independent round-trip."
           "Can read using 'begin' and 'length' args, so memory mapping is used."
    ;
}

static cJSON *local_obj = nullptr;

void cjson_benchmarks::prepare(io_device */*in*/, std::size_t /*flags*/) const {

}

std::pair<bool, std::string>
cjson_benchmarks::parse(io_device *in, std::size_t flags) {
    auto *istream  = in->input_io<io_type::mmap_streams>();
    auto pair = istream->stream();

    local_obj = cJSON_ParseWithLength(pair.first, pair.second);

    std::string err;
    if ( !local_obj ) {
        err = cJSON_GetErrorPtr();

        return {false, err};
    }

    return {true, err};
}

std::pair<bool, std::string>
cjson_benchmarks::print(io_device *out, std::size_t flags) {
    auto *ostream = out->output_io<io_type::string_buffer>();
    auto &string = ostream->stream();

    auto reserved = string.capacity();
    string.resize(reserved);
    bool ok = cJSON_PrintPreallocated(local_obj, string.data(), string.size(), 0);
    std::string err;
    if ( !ok ) {
        err = cJSON_GetErrorPtr();

        return {false, std::move(err)};
    }

    auto len = std::strlen(string.data());
    ostream->resize(len);

    return {true, std::move(err)};
}

void cjson_benchmarks::finish() const {
    cJSON_Delete(local_obj);
    local_obj = nullptr;
}

//std::vector<test_suite_result> cjson_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
//{
//    std::vector<test_suite_result> results;
//    for (auto& file : pathnames)
//    {
//        std::string text(file.text);
//        if (file.type == expected_result::expect_success)
//        {
//            cJSON* j = cJSON_Parse(text.c_str());
//            if (j != nullptr)
//            {
//                cJSON_Delete(j);
//                results.emplace_back(result_code::expected_result);
//            }
//            else
//            {
//                results.emplace_back(result_code::expected_success_parsing_failed);
//            }
//        }
//        else if (file.type == expected_result::expect_failure)
//        {
//            cJSON* j = cJSON_Parse(text.c_str());
//            if (j != nullptr)
//            {
//                cJSON_Delete(j);
//                results.emplace_back(result_code::expected_failure_parsing_succeeded);
//            }
//            else
//            {
//                results.emplace_back(result_code::expected_result);
//            }

//        }
//        else if (file.type == expected_result::result_undefined)
//        {
//            cJSON* j = cJSON_Parse(text.c_str());
//            if (j != nullptr)
//            {
//                cJSON_Delete(j);
//                results.emplace_back(result_code::result_undefined_parsing_succeeded);
//            }
//            else
//            {
//                results.emplace_back(result_code::result_undefined_parsing_failed);
//            }
//        }
//    }

//    return results;
//}

} // namespace json_benchmarks

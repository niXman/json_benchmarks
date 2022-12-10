
#include <cstring>

#include "cjson_tests.hpp"
#include "../stringize.hpp"

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

void* cjson_benchmarks::alloc_json_obj(io_device */*in*/) const {
    return nullptr;
}

std::pair<bool, std::string>
cjson_benchmarks::parse(void **json_obj_ptr, io_device *in) {
    auto *istream  = in->input_io<io_type::mmap_streams>();
    auto pair = istream->stream();

    *((cJSON**)json_obj_ptr) = cJSON_ParseWithLength(pair.first, pair.second);

    std::string err;
    if ( ! *json_obj_ptr ) {
        err = cJSON_GetErrorPtr();

        return {false, err};
    }

    return {true, err};
}

std::pair<bool, std::string>
cjson_benchmarks::print(void *json_obj_ptr, io_device *out) {
    auto *json = static_cast<cJSON *>(json_obj_ptr);
    auto *ostream = out->output_io<io_type::string_buffer>();
    auto &string = ostream->stream();

    auto reserved = string.capacity();
    string.resize(reserved);
    bool ok = cJSON_PrintPreallocated(json, string.data(), string.size(), 0);
    std::string err;
    if ( !ok ) {
        err = cJSON_GetErrorPtr();

        return {false, std::move(err)};
    }

    auto len = std::strlen(string.data());
    ostream->resize(len);

    return {true, std::move(err)};
}

void cjson_benchmarks::free_json_obj(void *json_obj_ptr) const {
    auto *json = static_cast<cJSON *>(json_obj_ptr);
    cJSON_Delete(json);
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

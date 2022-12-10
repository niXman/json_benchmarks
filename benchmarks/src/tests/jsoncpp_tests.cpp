
#include <chrono>
#include <iostream>

#include "jsoncpp_tests.hpp"
#include "../stringize.hpp"

#include <json/json.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration;

namespace json_benchmarks {

io_type jsoncpp_benchmarks::input_io_type() const { return io_type::string_buffer; }
io_type jsoncpp_benchmarks::output_io_type() const { return io_type::std_strstreams; }

const char* jsoncpp_benchmarks::name() const { return "jsoncpp"; }

const char* jsoncpp_benchmarks::url() const { return "https://github.com/open-source-parsers/jsoncpp"; }

const char* jsoncpp_benchmarks::version() const {
    return __STRINGIZE_VERSION(JSONCPP_VERSION_MAJOR, JSONCPP_VERSION_MINOR, JSONCPP_VERSION_PATCH);
}

const char* jsoncpp_benchmarks::notes() const {
    return "Uses std::map for both arrays and objects, expect larger memory footprint. "
           "Can read using 'begin' and 'end' pointers, then will use string as source. "
           "But can't write to buffer, then will write into stringstream."
    ;
}

void *jsoncpp_benchmarks::alloc_json_obj(io_device *in) const {
    return new Json::Value;
}

std::pair<bool, std::string>
jsoncpp_benchmarks::parse(void **json_obj_ptr, io_device *in) {
    auto &root = *static_cast<Json::Value *>(*json_obj_ptr);;
    auto *input  = in->input_io<io_type::string_buffer>();
    auto &string = input->stream();

    Json::Reader reader;
    auto pair = input->stream();
    if ( !reader.parse(string.data(), string.data() + string.length(), root) ) {
        auto err = reader.getFormattedErrorMessages();

        return {false, err};
    }

    return {true, std::string{}};
}

std::pair<bool, std::string>
jsoncpp_benchmarks::print(void *json_obj_ptr, io_device *out) {
    auto &json = *static_cast<Json::Value *>(json_obj_ptr);;
    auto *stream = out->output_io<io_type::std_strstreams>();
    auto &ostream = stream->stream();

    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    std::string err;
    try {
        writer->write(json, &ostream);
    } catch (const std::exception &ex) {
        err = ex.what();
    }

    return {err.empty(), std::move(err)};
}

void jsoncpp_benchmarks::free_json_obj(void *json_obj_ptr) const {
    auto *json = static_cast<Json::Value *>(json_obj_ptr);
    delete json;
}

//std::vector<test_suite_result> jsoncpp_benchmarks::run_test_suite(std::vector<test_suite_file>& pathnames)
//{
//    std::vector<test_suite_result> results;
//    for (auto& file : pathnames)
//    {
//        if (file.type == expected_result::expect_success)
//        {
//            try
//            {
//                Value val;
//                std::istringstream is(file.text);
//                is >> val;
//                results.emplace_back(result_code::expected_result);
//            }
//            catch (const std::exception&)
//            {
//                results.emplace_back(result_code::expected_success_parsing_failed);
//            }
//        }
//        else if (file.type == expected_result::expect_failure)
//        {
//            try
//            {
//                Value val;
//                std::istringstream is(file.text);
//                is >> val;
//                results.emplace_back(result_code::expected_failure_parsing_succeeded);
//            }
//            catch (const std::exception&)
//            {
//                results.emplace_back(result_code::expected_result);
//            }
//        }
//        else if (file.type == expected_result::result_undefined)
//        {
//            try
//            {
//                Value val;
//                std::istringstream is(file.text);
//                is >> val;
//                results.emplace_back(result_code::result_undefined_parsing_succeeded);
//            }
//            catch (const std::exception&)
//            {
//                results.emplace_back(result_code::result_undefined_parsing_failed);
//            }
//        }
//    }

//    return results;
//}

} // namespace json_benchmarks

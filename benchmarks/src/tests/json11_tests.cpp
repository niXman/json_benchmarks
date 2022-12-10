
#include "json11_tests.hpp"

#include <fstream>
#include <iostream> // TODO:

#include <json11.hpp>

namespace json_benchmarks {

io_type json11_benchmarks::input_io_type() const { return io_type::string_buffer; }
io_type json11_benchmarks::output_io_type() const { return io_type::string_buffer; }

const char* json11_benchmarks::name() const { return "json11"; }

const char* json11_benchmarks::url() const { return "https://github.com/dropbox/json11"; }

const char* json11_benchmarks::version() const { return "master (2df9473)"; }

const char* json11_benchmarks::notes() const {
    return "Uses pimpl idiom, implementation uses virtual inheritance, expect larger memory footprint."
            "can read and write from/to std::string only, so the data should be loaded into a buffer first."
    ;
}

void* json11_benchmarks::alloc_json_obj(io_device *in) const {
    return new json11::Json;
}

std::pair<bool, std::string>
json11_benchmarks::parse(void **json_obj_ptr, io_device *in) {
    auto *input  = in->input_io<io_type::string_buffer>();
    auto *json = static_cast<json11::Json *>(*json_obj_ptr);

    std::string err;
    *json = json11::Json::parse(input->stream(), err);
    if (err.length() > 0) {
        return {false, std::move(err)};
    }

    return {true, std::move(err)};
}

std::pair<bool, std::string>
json11_benchmarks::print(void *json_obj_ptr, io_device *out) {
    auto *output  = out->input_io<io_type::string_buffer>();
    auto *json = static_cast<json11::Json *>(json_obj_ptr);
    auto &string = output->stream();

    json->dump(string);

    return {true, std::string{}};
}

void json11_benchmarks::free_json_obj(void *json_obj_ptr) const {
    auto *json = static_cast<json11::Json *>(json_obj_ptr);

    delete json;
}

std::pair<std::size_t, std::size_t>
json11_benchmarks::allowed_leaks() const { return {120, 3}; }

//test_suite_result json11_benchmarks::run_test_suite(const test_suite_files &pathnames) {
//    std::vector<test_suite_result> results;
//    for (auto& file : pathnames) {
//        if (file.type == expected_result::expect_success) {
//            std::string err;
//            json11::Json val = json11::Json::parse(file.text, err);
//            if (err.length() == 0) {
//                results.emplace_back(result_code::expected_result);
//            } else {
//                results.emplace_back(result_code::expected_success_parsing_failed);
//            }
//        } else if (file.type == expected_result::expect_failure) {
//            std::string err;
//            json11::Json val = json11::Json::parse(file.text,err);
//            if (err.length() == 0) {
//                results.emplace_back(result_code::expected_failure_parsing_succeeded);
//            } else {
//                results.emplace_back(result_code::expected_result);
//            }
//        } else if (file.type == expected_result::result_undefined) {
//            std::string err;
//            json11::Json val = json11::Json::parse(file.text,err);
//            if (err.length() == 0) {
//                results.emplace_back(result_code::result_undefined_parsing_succeeded);
//            } else {
//                results.emplace_back(result_code::result_undefined_parsing_failed);
//            }
//        }
//    }

//    return results;
//}

} // json_benchmarks

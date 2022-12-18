
#include "json11.hpp"

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

static json11::Json *local_obj = nullptr;;

void json11_benchmarks::prepare(io_device *in, std::size_t flags) const {
    local_obj = new json11::Json;
}

std::pair<bool, std::string>
json11_benchmarks::parse(io_device *in, std::size_t flags) {
    auto *input  = in->input_io<io_type::string_buffer>();

    std::string err;
    *local_obj = json11::Json::parse(input->stream(), err);
    if (err.length() > 0) {
        return {false, std::move(err)};
    }

    return {true, std::move(err)};
}

std::pair<bool, std::string>
json11_benchmarks::print(io_device *out, std::size_t flags) {
    auto *output  = out->input_io<io_type::string_buffer>();
    auto &string = output->stream();

    local_obj->dump(string);

    return {true, std::string{}};
}

void json11_benchmarks::finish() const {
    delete local_obj;
    local_obj = nullptr;
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

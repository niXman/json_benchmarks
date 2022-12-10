
#pragma once

#include "../benchmarks.hpp"

namespace json_benchmarks {

/*************************************************************************************************/

struct flatjson_benchmarks: benchmarks {
    virtual ~flatjson_benchmarks() = default;

    void*alloc_json_obj(io_device *in) const override;
    std::pair<bool, std::string> parse(void **json_obj_ptr, io_device *in) override;
    std::pair<bool, std::string> print(void *json_obj_ptr, io_device *out) override;
    void free_json_obj(void *json_obj_ptr) const override;

//    test_suite_results run_test_suite(const test_suite_files &pathnames) override;

    io_type input_io_type() const override;
    io_type output_io_type() const override;
    const char* name() const override;
    const char* url() const override;
    const char* version() const override;
    const char* notes() const override;
};

/*************************************************************************************************/

} // namespace json_benchmarks

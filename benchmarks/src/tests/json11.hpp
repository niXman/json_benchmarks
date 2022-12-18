
#pragma once

#include "../benchmarks.hpp"

namespace json_benchmarks {

/*************************************************************************************************/

struct json11_benchmarks: benchmarks {
    virtual ~json11_benchmarks() = default;

    void prepare(io_device *in, std::size_t flags) const override;
    std::pair<bool, std::string> parse(io_device *in, std::size_t flags) override;
    std::pair<bool, std::string> print(io_device *out, std::size_t flags) override;
    void finish() const override;

    std::pair<std::size_t, std::size_t> allowed_leaks() const override;

//    test_suite_results run_test_suite(const test_suite_files &pathnames) override;

    io_type input_io_type() const override;
    io_type output_io_type() const override;
    const char* name() const override;
    const char* url() const override;
    const char* version() const override;
    const char* notes() const override;
};

/*************************************************************************************************/

} // ns json_benchmarks

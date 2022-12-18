
#ifndef JSON_BENCHMARKS_HPP
#define JSON_BENCHMARKS_HPP

#include <vector>
#include <memory>

#include "io_device.hpp"

namespace json_benchmarks {

/*************************************************************************************************/

struct e_json_flags {
    enum {
        despaced = 1u << 0
    };
};

/*************************************************************************************************/

struct benchmarks {
    virtual ~benchmarks() = default;
    // used to allocate the root object, to warmup input data, count a tokens, etc...
    // pointer/object ot that type should be accessed inside the CPP file only.
    virtual void prepare(io_device *in, std::size_t flags) const = 0;
    virtual std::pair<bool, std::string> parse(io_device *in, std::size_t flags) = 0;
    virtual std::pair<bool, std::string> print(io_device *out, std::size_t flags) = 0;
    virtual void  finish() const = 0;

    // just compare the source and the generated json for structure equality
    std::pair<bool, std::string> check(io_device *in, io_device *out, std::size_t flags) const;

    // this was introduced because of 'json11' because it uses a singleton
    // contains static vars witch is initialized on 'Json' ctor, but freed at the end of the program.
    virtual std::pair<
         std::size_t // in bytes
        ,std::size_t // in allocations
    > allowed_leaks() const;

//    virtual test_suite_results run_test_suite(const test_suite_files &pathnames) = 0;

    virtual io_type input_io_type() const = 0;
    virtual io_type output_io_type() const = 0;

    virtual const char* name() const = 0;
    virtual const char* url() const = 0;
    virtual const char* version() const = 0;
    virtual const char* notes() const = 0;

    std::pair<std::unique_ptr<io_device>, std::unique_ptr<io_device>>
    create_io(const std::string &input_fname) const;

    std::size_t start_time();
    std::size_t duration(std::size_t start);
};

using benchmarks_ptr  = std::unique_ptr<benchmarks>;
using benchmarks_list = std::vector<benchmarks_ptr>;

benchmarks_list create_benchmarks();

/*************************************************************************************************/
#if 0
struct json_spirit_benchmarks: benchmarks {
    measurements measure(io_device *id, io_device *od) override;
//    test_suite_results run_test_suite(const test_suite_files &pathnames) override;

    io_type input_io_type() const override;
    io_type output_io_type() const override;
    const char* name() const override;
    const char* url() const override;
    const char* version() const override;
    const char* notes() const override;
};

/*************************************************************************************************/

struct jsoncons_benchmarks: benchmarks {
    measurements measure(io_device *id, io_device *od) override;
//    test_suite_results run_test_suite(const test_suite_files &pathnames) override;

    io_type input_io_type() const override;
    io_type output_io_type() const override;
    const char* name() const override;
    const char* url() const override;
    const char* version() const override;
    const char* notes() const override;
};

/*************************************************************************************************/

struct nlohmann_benchmarks: benchmarks {
    measurements measure(io_device *id, io_device *od) override;
//    test_suite_results run_test_suite(const test_suite_files &pathnames) override;

    io_type input_io_type() const override;
    io_type output_io_type() const override;
    const char* name() const override;
    const char* url() const override;
    const char* version() const override;
    const char* notes() const override;
};

/*************************************************************************************************/

struct rapidjson_benchmarks: benchmarks {
    measurements measure(io_device *id, io_device *od) override;
//    test_suite_results run_test_suite(const test_suite_files &pathnames) override;

    io_type input_io_type() const override;
    io_type output_io_type() const override;
    const char* name() const override;
    const char* url() const override;
    const char* version() const override;
    const char* notes() const override;
};

/*************************************************************************************************/

struct taojson_benchmarks: benchmarks {
    measurements measure(io_device *id, io_device *od) override;
//    test_suite_results run_test_suite(const test_suite_files &pathnames) override;

    io_type input_io_type() const override;
    io_type output_io_type() const override;
    const char* name() const override;
    const char* url() const override;
    const char* version() const override;
    const char* notes() const override;
};

/*************************************************************************************************/
#endif
} // json_benchmarks

#endif



#include "benchmarks.hpp"

#include <chrono>
#include <sstream>

#include "tests/jsoncons_tests.hpp"
#include "tests/cjson_tests.hpp"
#include "tests/json11_tests.hpp"
#include "tests/jsoncpp_tests.hpp"
#include "tests/flatjson_tests.hpp"

#include <flatjson/flatjson.hpp>

namespace json_benchmarks {

using std::chrono::high_resolution_clock;
using std::chrono::duration;

/*************************************************************************************************/

std::pair<bool, std::string>
benchmarks::check(io_device *in, io_device *out) const {
    assert(in->type() == io_type::string_buffer || in->type() == io_type::mmap_streams);
    assert(out->type() == io_type::string_buffer || out->type() == io_type::std_strstreams);

    const char *srcptr = nullptr;
    std::size_t srcsize= 0;
    if ( in->type() == io_type::string_buffer ) {
        const auto &string = static_cast<input_string_buffer_io *>(in)->stream();
        srcptr = string.data();
        srcsize= string.length();
    } else {
        const auto pair = static_cast<input_mmap_stream_io *>(in)->stream();
        srcptr = pair.first;
        srcsize= pair.second;
    }

    std::string dststring;
    if ( out->type() == io_type::string_buffer ) {
        auto &string = static_cast<output_string_buffer_io *>(out)->stream();
        dststring = std::move(string);
    } else {
        auto &stringstream = static_cast<output_std_strstream_io *>(out)->stream();
        auto wr = stringstream.tellp();
        dststring = std::move(stringstream.str());
        dststring.resize(wr);
    }

    auto *os = std::fopen("test-output.json", "wb");
    assert(os);
    std::fwrite(dststring.data(), 1, dststring.size(), os);
    std::fclose(os);

    auto srcp = flatjson::make_parser(srcptr, srcptr + srcsize);
    flatjson::parse(&srcp);
    assert(flatjson::is_valid(&srcp));

    auto dstp = flatjson::make_parser(dststring.data(), dststring.data() + dststring.length());
    flatjson::parse(&dstp);
    assert(flatjson::is_valid(&dstp));

    flatjson::iterator ldiff, rdiff;
    auto cmpres = flatjson::compare(&ldiff, &rdiff, &srcp, &dstp, flatjson::compare_mode::markup_only);

    std::string err;
    if ( cmpres != flatjson::compare_result::equal ) {
        err = flatjson::compare_result_string(cmpres);
    }

    flatjson::free_parser(&srcp);
    flatjson::free_parser(&dstp);

    return {err.empty(), std::move(err)};
}

/*************************************************************************************************/

std::pair<std::size_t, std::size_t>
benchmarks::allowed_leaks() const { return {0, 0};}

/*************************************************************************************************/

std::pair<std::unique_ptr<io_device>, std::unique_ptr<io_device>>
benchmarks::create_io(const std::string &input_fname, const std::string &output_fname) const {
    return {
         json_benchmarks::create_io(io_direction::input, input_io_type(), input_fname)
        ,json_benchmarks::create_io(io_direction::output, output_io_type(), output_fname)
    };
}

/*************************************************************************************************/

std::size_t benchmarks::start_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        high_resolution_clock::now().time_since_epoch()
    ).count();
}

std::size_t benchmarks::duration(std::size_t start) {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        high_resolution_clock::now().time_since_epoch()
    ).count();
    return now - start;
}

/*************************************************************************************************/

benchmarks_list create_benchmarks() {
    benchmarks_list list;

    list.emplace_back(std::make_unique<jsoncons_benchmarks>());
    list.emplace_back(std::make_unique<flatjson_benchmarks>());
    list.emplace_back(std::make_unique<cjson_benchmarks>());
    list.emplace_back(std::make_unique<jsoncpp_benchmarks>());
    list.emplace_back(std::make_unique<json11_benchmarks>());

    return list;
}

/*************************************************************************************************/

} // json_benchmarks


#include "data_generator.hpp"

#include <chrono>
#include <random>
#include <sstream>
#include <fstream>
#include "jsoncons/json_encoder.hpp"

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;

/*************************************************************************************************/

std::size_t make_test_file(
     const std::string &filename
    ,std::size_t repeats
    ,std::size_t numInts
    ,std::size_t numFloats
    ,std::size_t numStrings
    ,std::size_t numKeywords
    ,std::size_t flags)
{
    std::ofstream os(filename, std::ofstream::binary|std::ios_base::out|std::ios_base::trunc);
    if ( !os ) {
        std::ostringstream ec;
        ec << "Cannot create file " << filename;
        throw std::runtime_error(ec.str());
    }

    std::random_device rd;
    std::mt19937_64 rng(rd());

    jsoncons::json_options options;
    options.escape_all_non_ascii(true);
    jsoncons::json_stream_encoder pretyfied_handler(os, options);
    jsoncons::compact_json_stream_encoder compacted_handler(os, options);
    auto &handler = (flags & static_cast<std::size_t>(e_data_generator_mode::compacted))
        ? static_cast<jsoncons::basic_json_visitor<char> &>(compacted_handler)
        : static_cast<jsoncons::basic_json_visitor<char> &>(pretyfied_handler)
    ;

    auto local_flags = (flags & static_cast<std::size_t>(e_data_generator_mode::mixed))
        ? (static_cast<std::size_t>(e_data_generator_mode::ints)
            | static_cast<std::size_t>(e_data_generator_mode::floats)
                | static_cast<std::size_t>(e_data_generator_mode::strings)
                    | static_cast<std::size_t>(e_data_generator_mode::keywords))
        : flags
    ;

    auto start = high_resolution_clock::now();

    std::vector<std::string> string_values;
    if ( local_flags & static_cast<std::size_t>(e_data_generator_mode::strings) ) {
        for ( auto i = 0u; i < numStrings; ++i ) {
            string_values.push_back("All cats like mice, \"\\uD800\\uDC00\"");
        }
    }

    std::vector<double> double_values;
    if ( local_flags & static_cast<std::size_t>(e_data_generator_mode::floats) ) {
        double_values.reserve(numFloats);

        std::uniform_real_distribution<double> real_dist{0, 10};
        for ( auto i = 0u; i < numFloats; ++i ) {
            double_values.push_back(real_dist(rng));
        }
    }

    std::vector<uint64_t> integer_values;
    if ( local_flags & static_cast<std::size_t>(e_data_generator_mode::ints) ) {
        integer_values.reserve(numInts);

        std::uniform_int_distribution<std::uint64_t> int_dist
            {0, std::numeric_limits<std::uint64_t>::max()};
        for ( auto i = 0u; i < numInts; ++i ) {
            integer_values.push_back(int_dist(rng));
        }
    }

    static const char keywords[] = {0, 1, 2}; // 0 - true, 1 = false, 2 = null
    std::vector<char> keywords_values;
    if ( local_flags & static_cast<std::size_t>(e_data_generator_mode::keywords) ) {
        keywords_values.reserve(numKeywords);

        std::uniform_int_distribution<std::uint64_t> int_dist{0, 2};
        for ( auto i = 0u; i < numKeywords; ++i ) {
            keywords_values.push_back(keywords[int_dist(rng)]);
        }
    }

    handler.begin_array();
    for ( auto i = 0u; i < repeats; i += 2 ) {
        handler.begin_object();
            handler.key("person");
            handler.begin_object();
                handler.key("first_name");
                handler.string_value("John");
                handler.key("last_name"   );
                handler.string_value("Doe");
                handler.key("birthdate");
                handler.string_value("1998-05-13");
                handler.key("sex");
                handler.string_value("m");
                handler.key("salary");
                handler.uint64_value(70000);
                handler.key("married");
                handler.bool_value(false);
                handler.key("interests");
                handler.begin_array();
                handler.string_value("Reading");
                handler.string_value("Mountain biking");
                handler.string_value("Hacking");
                handler.end_array();
                handler.key("favorites");
                handler.begin_object();
                    handler.key("color");
                    handler.string_value("blue");
                    handler.key("sport");
                    handler.string_value("soccer");
                    handler.key("food");
                    handler.string_value("spaghetti");
                    handler.key("big_text");
                        handler.begin_array();
                        for ( const auto &x: string_values ) {
                            handler.string_value(x);
                        }
                        handler.end_array();
                    handler.key("integer_values");
                        handler.begin_array();
                        for ( auto x : integer_values ) {
                            handler.int64_value(x);
                        }
                        handler.end_array();
                    handler.key("double_values");
                        handler.begin_array();
                        for (auto x : double_values) {
                            handler.double_value(x);
                        }
                        handler.end_array();
                    handler.key("keywords_values");
                        handler.begin_array();
                        for ( auto x : keywords_values ) {
                            if ( x == 0 ) { handler.bool_value(true); }
                            if ( x == 1 ) { handler.bool_value(false); }
                            if ( x == 2 ) { handler.null_value(); }
                        }
                        handler.end_array();
                handler.end_object();
            handler.end_object();
        handler.end_object();
    }

    for ( auto i = 0u; i < repeats; i += 2 ) {
        handler.begin_object();
            handler.key("person");
            handler.begin_object();
                handler.key("first_name");
                handler.string_value("jane");
                handler.key("last_name"   );
                handler.string_value("doe");
                handler.key("birthdate");
                handler.string_value("1998-05-13");
                handler.key("sex");
                handler.string_value("f");
                handler.key("salary");
                handler.uint64_value(80000);
                handler.key("married");
                handler.bool_value(true);
                handler.key("pets");
                handler.null_value();
                handler.key("interests");
                handler.begin_array();
                handler.string_value("Skiing");
                handler.string_value("Hiking");
                handler.string_value("Camoing");
                handler.end_array();
                handler.key("favorites");
                handler.begin_object();
                    handler.key("color");
                    handler.string_value("Red");
                    handler.key("sport");
                    handler.string_value("skiing");
                    handler.key("food");
                    handler.string_value("risotto");
                handler.end_object();
            handler.end_object();
        handler.end_object();
    }
    handler.end_array();
    handler.flush();

    auto end = high_resolution_clock::now();
    auto time_to_write = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return time_to_write;
}

/*************************************************************************************************/

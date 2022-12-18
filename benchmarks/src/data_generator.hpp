
#ifndef DATA_GENERATOR_HPP
#define DATA_GENERATOR_HPP

#include <string>
#include <ostream>

/*************************************************************************************************/

// !!! DO NOT REORDER !!!
struct e_data_generator_mode {
    enum k_e {
         ints      = 1u << 0
        ,floats    = 1u << 1
        ,strings   = 1u << 2
        ,keywords  = 1u << 3
        ,mixed     = 1u << 4
        ,smallfile = 1u << 5
        ,compacted = 1u << 6 // OR`ed
    };
};

// !!! DO NOT REORDER !!!
static constexpr const char *s_data_generator_mode[] = {
     "ints"
    ,"floats"
    ,"strings"
    ,"keywords"
    ,"mixed"
    ,"smallfile"
    ,"compacted"
};

inline std::ostream& operator<< (std::ostream &os, e_data_generator_mode::k_e v) {
    switch ( v ) {
        case e_data_generator_mode::ints: return os << s_data_generator_mode[0];
        case e_data_generator_mode::floats: return os << s_data_generator_mode[1];
        case e_data_generator_mode::strings: return os << s_data_generator_mode[2];
        case e_data_generator_mode::keywords: return os << s_data_generator_mode[3];
        case e_data_generator_mode::mixed: return os << s_data_generator_mode[4];
        case e_data_generator_mode::smallfile: return os << s_data_generator_mode[5];
        case e_data_generator_mode::compacted: return os << s_data_generator_mode[6];
    }

    return os;
}

/*************************************************************************************************/

std::size_t make_test_file(
    const std::string &filename
    ,std::size_t repeats
    ,std::size_t numInts
    ,std::size_t numFloats
    ,std::size_t numStrings
    ,std::size_t numKeywords
    ,std::size_t flags
);

/*************************************************************************************************/

#endif

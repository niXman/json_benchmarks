cmake_minimum_required(VERSION 3.1)
project(json-benchmarks)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Werror=return-type -Wno-unused-parameter -Wno-unused-variable \
        -Wno-deprecated-declarations"
)

add_definitions(
    -UNDEBUG
    -DFJ_DONT_CHECK_OVERFLOW
    #-D__FJ__ANALYZE_PARSER
    #-DFJ_DONT_USE_SIMD
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer \
        -fsanitize=address -fsanitize=leak -static-libasan")
    add_definitions(-D_GLIBCXX_DEBUG)
endif()

include_directories(
    src
    ../thirdparty/malloc-stat/include
    ../thirdparty/justargs/include
    # tests
    ../thirdparty/jsoncons/include
    ../thirdparty/flatjson/include
    ../thirdparty/taojson/include
    ../thirdparty/taojson/external/PEGTL/include
    ../thirdparty/cjson
    ../thirdparty/json11
    ../thirdparty/jsoncpp/include
    ../thirdparty/boost
    ../thirdparty/json_spirit
    ../thirdparty/nlohmann/include
    ../thirdparty/rapidjson/include
)

set(HEADERS
    src/tests/cjson_tests.hpp
    src/tests/json11_tests.hpp
    src/tests/jsoncons_tests.hpp
    src/tests/jsoncpp_tests.hpp
    src/tests/json_spirit_tests.hpp
    src/tests/nlohmann_tests.hpp
    src/tests/rapidjson_tests.hpp
    src/tests/taojson_tests.hpp
    src/tests/flatjson_tests.hpp
    #
    src/benchmarks.hpp
    src/io_device.hpp
    src/measurements.hpp
    src/stringize.hpp
    src/mmfile.hpp
)

set(SOURCES
    src/main.cpp
    src/benchmarks.cpp
    src/data_generator.cpp
    src/io_device.cpp
    src/os_tools.cpp
    #
    src/tests/cjson_tests.cpp
    src/tests/json11_tests.cpp
    src/tests/jsoncons_tests.cpp
    src/tests/jsoncpp_tests.cpp
    src/tests/flatjson_tests.cpp
#    src/tests/json_spirit_tests.cpp
#    src/tests/nlohmann_tests.cpp
#    src/tests/rapidjson_tests.cpp
#    src/tests/taojson_tests.cpp
    # thirdparty sources
    ../thirdparty/cjson/cJSON.c
    ../thirdparty/json11/json11.cpp
    ../thirdparty/jsoncpp/src/lib_json/json_value.cpp
    ../thirdparty/jsoncpp/src/lib_json/json_reader.cpp
    ../thirdparty/jsoncpp/src/lib_json/json_writer.cpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../thirdparty/malloc-stat/src
    COMMAND $(MAKE)
    COMMAND cp ${CMAKE_SOURCE_DIR}/../thirdparty/malloc-stat/src/malloc-stat.so ${CMAKE_SOURCE_DIR}/../
    DEPENDS malloc-stat.c ../include/malloc-stat/api.h
)

target_link_libraries(
    ${PROJECT_NAME}
    pthread
)

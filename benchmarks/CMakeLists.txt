cmake_minimum_required(VERSION 3.1)
project(json-benchmarks)

set (CMAKE_CXX_STANDARD 17)

#set(CC clang)
#set(CXX clang++)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Werror=return-type -Wno-unused-parameter -Wno-unused-variable \
        -Wno-deprecated-declarations"
)

add_definitions(
    -UNDEBUG
    -DFJ_DONT_CHECK_OVERFLOW
    #-D__FJ__ANALYZE_PARSER
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer \
        -fsanitize=address -fsanitize=leak -static-libasan")
    add_definitions(-D_GLIBCXX_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

include_directories(
    src
    ../thirdparty/malloc-stat/include
    ../thirdparty/cmdargs/include
    # tests
    ../thirdparty/jsoncons/include
    ../thirdparty/flatjson/include
    ../thirdparty/yyjson/src
    ../thirdparty/simdjson/singleheader
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
    src/tests/cjson.hpp
    src/tests/json11.hpp
    src/tests/jsoncons.hpp
    src/tests/jsoncpp.hpp
    src/tests/json_spirit.hpp
    src/tests/nlohmann.hpp
    src/tests/rapidjson.hpp
    src/tests/taojson.hpp
    src/tests/flatjson.hpp
    src/tests/yyjson.hpp
    src/tests/simdjson.hpp
    #
    src/benchmarks.hpp
    src/io_device.hpp
    src/measurements.hpp
    src/stringize.hpp
    src/mmfile.hpp
    src/data_generator.hpp
)

set(SOURCES
    src/main.cpp
    src/benchmarks.cpp
    src/data_generator.cpp
    src/io_device.cpp
    src/os_tools.cpp
    #
    src/tests/cjson.cpp
    src/tests/json11.cpp
    src/tests/jsoncons.cpp
    src/tests/jsoncpp.cpp
    src/tests/flatjson.cpp
    src/tests/yyjson.cpp
    src/tests/simdjson.cpp
    src/tests/taojson.cpp
#    src/tests/json_spirit.cpp
#    src/tests/nlohmann.cpp
#    src/tests/rapidjson.cpp

    # yyjson
    ../thirdparty/yyjson/src/yyjson.c
    # simdjson
    ../thirdparty/simdjson/singleheader/simdjson.cpp
    # cjson
    ../thirdparty/cjson/cJSON.c
    # json11
    ../thirdparty/json11/json11.cpp
    # jsoncpp
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
    COMMAND $(MAKE) malloc-stat.so
    COMMAND cp ${CMAKE_SOURCE_DIR}/../thirdparty/malloc-stat/src/malloc-stat.so ${CMAKE_SOURCE_DIR}/../
    DEPENDS malloc-stat.c ../include/malloc-stat/api.h
)

target_link_libraries(
    ${PROJECT_NAME}
    pthread
)

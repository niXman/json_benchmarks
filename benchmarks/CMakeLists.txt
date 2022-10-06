cmake_minimum_required(VERSION 3.1)
project(json_benchmarks)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "\
    ${CMAKE_CXX_FLAGS} \
    -Wno-unused-parameter -Wno-unused-variable -Wno-deprecated-declarations"
)

include_directories(
    src
    ../thirdparty/jsoncons/include
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

set(SOURCES
    src/main.cpp
    src/data_generator.cpp
    src/os_tools.cpp
    src/tests/cjson_tests.cpp
    src/tests/json11_tests.cpp
    src/tests/json_benchmarks.hpp
    src/tests/jsoncons_tests.cpp
    src/tests/jsoncpp_tests.cpp
    src/tests/json_spirit_tests.cpp
    src/tests/nlohmann_tests.cpp
    src/tests/rapidjson_tests.cpp
    src/tests/taojson_tests.cpp
    # thirdparty sources
    ../thirdparty/cjson/cJSON.c
    ../thirdparty/json11/json11.cpp
    ../thirdparty/jsoncpp/src/lib_json/json_value.cpp
    ../thirdparty/jsoncpp/src/lib_json/json_reader.cpp
    ../thirdparty/jsoncpp/src/lib_json/json_writer.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    pthread
)

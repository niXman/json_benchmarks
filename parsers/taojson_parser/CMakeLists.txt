cmake_minimum_required(VERSION 3.1)
project(taojson_parser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")

include_directories(
    ../../thirdparty/taojson/include
    ../../thirdparty/taojson/external/PEGTL/include
)

set(SOURCES
    taojson_parser.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    pthread
)

cmake_minimum_required(VERSION 3.1)
project(boost_json_parser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

include_directories(
    ../../thirdparty/boost
)

set(SOURCES
    boost_json_parser.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    pthread
)

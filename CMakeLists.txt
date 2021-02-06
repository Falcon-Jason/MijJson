cmake_minimum_required(VERSION 3.19)
project(MijJson)

set(CMAKE_CXX_STANDARD 14)
add_library(MijJson mijjson_parser.cpp mijjson_value.cpp)

add_executable(MijJson_Test test.cpp mijjson_value.cpp mijjson_parser.cpp)
target_link_libraries(MijJson_Test gtest)
cmake_minimum_required(VERSION 3.19)
project(MijJson)

set(CMAKE_CXX_STANDARD 14)

add_executable(MijJson test.cpp mijjson.cpp mijjson.h mijjson_parse.cpp)
target_link_libraries(MijJson gtest)
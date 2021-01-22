cmake_minimum_required(VERSION 3.19)
project(MijJson)

set(CMAKE_CXX_STANDARD 14)

add_executable(MijJson test.cpp MijJson.cpp MijJson.h MijJson_Parsing.cpp)
target_link_libraries(MijJson gtest)
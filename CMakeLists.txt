cmake_minimum_required(VERSION 3.19)
project(MijJson)

set(CMAKE_CXX_STANDARD 14)

add_executable(MijJson main.cpp JsonDocument.cpp JsonDocument.h)
target_link_libraries(MijJson gtest)
cmake_minimum_required(VERSION 3.0)
project(test_fb)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/)

add_subdirectory(src)
add_subdirectory(test)
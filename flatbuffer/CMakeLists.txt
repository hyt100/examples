cmake_minimum_required(VERSION 3.0)
project(test_fb)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(flatbuffer)

COMPILE_FLATBUFFERS_SCHEMA_TO_CPP(TARGET_FLATC ${CMAKE_CURRENT_SOURCE_DIR}/stu.fbs)

include_directories(src/ ${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(src/ SRC)
add_executable(${PROJECT_NAME} ${SRC})
add_dependencies(${PROJECT_NAME} TARGET_FLATC)
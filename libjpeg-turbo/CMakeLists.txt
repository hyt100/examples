cmake_minimum_required(VERSION 3.0)
project(my)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/ ${CMAKE_CURRENT_SOURCE_DIR} /opt/libjpeg-turbo/include)
link_directories(/opt/libjpeg-turbo/lib64)
add_definitions(-g)

aux_source_directory(src/ SRC)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} jpeg)
cmake_minimum_required(VERSION 3.0)
project(run)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(thirdparty/glm-0.9.9.8/)

aux_source_directory(src/ SRC)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME})
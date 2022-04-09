cmake_minimum_required(VERSION 3.0)
project(run)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(thirdparty/eigen-3.3.9/Eigen)

# aux_source_directory(src/ SRC)

add_executable(run_alphashape src/alpha_shape.cc)
target_link_libraries(run_alphashape gmp)
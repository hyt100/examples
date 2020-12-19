project(test_run)

aux_source_directory(./ SRC)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} gtest func pthread)
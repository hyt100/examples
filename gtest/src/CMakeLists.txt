project(func)

aux_source_directory(./ SRC)
add_library(${PROJECT_NAME} SHARED ${SRC})

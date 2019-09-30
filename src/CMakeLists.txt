cmake_minimum_required(VERSION 3.0)

file(GLOB SRC_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_source_files(${SRC_SOURCES})

add_subdirectory("tasks")
add_subdirectory("drivers")
add_subdirectory("mac")
add_subdirectory("radio")
add_subdirectory("utility")

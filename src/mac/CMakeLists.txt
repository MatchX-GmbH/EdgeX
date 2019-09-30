cmake_minimum_required(VERSION 3.6)

file(GLOB MAC_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/region/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/soft-se/*.c"
)

add_source_files(${MAC_SOURCES})

cmake_minimum_required(VERSION 3.6)

file(GLOB RADIO_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sx126x/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/board/*.c"
)

add_source_files(${RADIO_SOURCES})

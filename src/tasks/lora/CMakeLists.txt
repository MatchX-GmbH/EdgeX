cmake_minimum_required(VERSION 3.0)

file(GLOB LORA_APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/LmHandler/*.c"
)

add_source_files(${LORA_APP_SOURCES})

cmake_minimum_required(VERSION 3.0)

add_source_files(common.c)

if(APP STREQUAL "parking")
  add_subdirectory(parking)
elseif(APP STREQUAL "person")
  add_subdirectory(person)
endif()

cmake_minimum_required(VERSION 3.10)

project(stmepic_base LANGUAGES C CXX)

set(UPPER_PROJECT_NAME ${CMAKE_PROJECT_NAME})

add_library(stmepic_base INTERFACE)

add_subdirectory(src/Timing)
add_subdirectory(src/Inc)
add_subdirectory(src/Device)
add_subdirectory(src/Algorithm)
add_subdirectory(src/Hardware)
add_subdirectory(src/Sensors)
add_subdirectory(src/Memory)
add_subdirectory(src/Controllers)
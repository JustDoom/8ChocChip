cmake_minimum_required(VERSION 3.20)
project(8ChocChip
LANGUAGES CXX
VERSION 0.1)

add_subdirectory(dependencies)
add_subdirectory(src)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

install(DIRECTORY assets DESTINATION ${CMAKE_INSTALL_PREFIX})
cmake_minimum_required(VERSION 3.20)
project(8ChocChip
LANGUAGES CXX
VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
#
#install(DIRECTORY assets DESTINATION ${CMAKE_INSTALL_PREFIX})

file(GLOB ASSETS "${CMAKE_SOURCE_DIR}/assets/*")

# Copy assets to build directory
foreach(ASSET ${ASSETS})
    configure_file(${ASSET} ${CMAKE_BINARY_DIR}/assets COPYONLY)
endforeach()

add_subdirectory(dependencies)
add_subdirectory(src)
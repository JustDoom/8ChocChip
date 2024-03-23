cmake_minimum_required(VERSION 3.20)
project(8ChocChip
LANGUAGES CXX
VERSION 0.1)

# Set the path to the vcpkg toolchain file for Windows
if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

add_subdirectory(dependencies)
add_subdirectory(src)

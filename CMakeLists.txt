cmake_minimum_required(VERSION 3.20)
project(8ChocChip
LANGUAGES CXX
VERSION 0.1)

option(BUILD_SHARED_LIBS "Build a shared library instead of static" OFF)
option(SDL_VENDORED "Use vendored libraries" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
    set(SDL_GAMEINPUT OFF CACHE BOOL "Disable GameInput for XP compatibility" FORCE)
    add_executable(8ChocChip WIN32)
    set_target_properties(8ChocChip PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
    add_definitions(-D_WIN32_WINNT=0x0501 -DWINVER=0x0501)
else()
    set(libname "config")
    add_executable(8ChocChip)
endif()

add_subdirectory(dependencies)
add_subdirectory(src)

target_link_libraries(8ChocChip PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf ${libname}++)
target_compile_definitions(8ChocChip PUBLIC SDL_MAIN_USE_CALLBACKS)

set_target_properties(8ChocChip PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_RPATH "$ORIGIN"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

cmake_minimum_required(VERSION 3.20)
project(8ChocChip
LANGUAGES CXX
VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -finline-functions -fomit-frame-pointer -fstrict-aliasing -ffast-math")

set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_GAMECONTROLLER OFF CACHE BOOL "" FORCE)

set(SDL3TTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDL3TTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL3TTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)

set(SDLIMAGE_VENDORED ON)

set(SDL3IMAGE_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL3IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDL3IMAGE_TESTS OFF CACHE BOOL "" FORCE)

# Disable all image types except for PNG
option(SDLIMAGE_AVIF OFF)
option(SDLIMAGE_BMP OFF)
option(SDLIMAGE_GIF OFF)
option(SDLIMAGE_JPG OFF)
option(SDLIMAGE_JXL OFF)
option(SDLIMAGE_LBM OFF)
option(SDLIMAGE_PCX OFF)
option(SDLIMAGE_PNM OFF)
option(SDLIMAGE_QOI OFF)
option(SDLIMAGE_SVG OFF)
option(SDLIMAGE_TGA OFF)
option(SDLIMAGE_TIF OFF)
option(SDLIMAGE_WEBP OFF)
option(SDLIMAGE_XCF OFF)
option(SDLIMAGE_XPM OFF)
option(SDLIMAGE_XV OFF)

option(BUILD_SHARED_LIBS "Build a shared library instead of static" OFF)
option(SDL_VENDORED "Use vendored libraries" ON)

add_executable(8ChocChip)

add_subdirectory(dependencies)
add_subdirectory(src)

target_link_libraries(8ChocChip PRIVATE nlohmann_json::nlohmann_json SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image SDL3::SDL3)
if (WIN32)
    target_link_libraries(8ChocChip PRIVATE bcrypt)
else ()
    find_package(OpenSSL REQUIRED)
    target_link_libraries(8ChocChip PRIVATE OpenSSL::Crypto)
endif()

if (MINGW)
    target_link_options(8ChocChip PRIVATE -static -pthread -static-libgcc -static-libstdc++)
endif()

set_target_properties(8ChocChip PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_RPATH "$ORIGIN"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")
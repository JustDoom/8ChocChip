cmake_minimum_required(VERSION 3.20)
project(8ChocChip
LANGUAGES CXX
VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(SDL_VENDORED "Use vendored libraries" ON)

add_executable(8ChocChip)

add_subdirectory(dependencies)
add_subdirectory(src)

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

target_link_libraries(8ChocChip PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf ${libname}++)

set_target_properties(8ChocChip PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_RPATH "$ORIGIN"
)

# Copy nfd and config++ libraries to the executable directory
if (UNIX AND NOT APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:config++>"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ln -sf "libnfd.so.1.2.1" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libnfd.so.1"
            COMMAND ln -sf "libconfig++.so.1.7.4" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libconfig++.so.1"
    )
endif()


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

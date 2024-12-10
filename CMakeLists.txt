cmake_minimum_required(VERSION 3.20)
project(8ChocChip
LANGUAGES CXX
VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(8ChocChip)

file(GLOB ASSETS "${CMAKE_SOURCE_DIR}/assets/*")

foreach(ASSET ${ASSETS})
    configure_file(${ASSET} ${CMAKE_BINARY_DIR}/assets COPYONLY)
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_subdirectory(dependencies)
add_subdirectory(src)

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

target_link_libraries(8ChocChip PRIVATE sfml-graphics sfml-audio sfml-window sfml-system nfd ${libname}++)

# Set the RPATH of the executable to include the directory where SFML libraries are located
set_target_properties(8ChocChip PROPERTIES
        INSTALL_RPATH "$ORIGIN"
)

# Copy DLLs needed for runtime on Windows
if(WIN32)
    add_custom_command(
            TARGET 8ChocChip
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:8ChocChip>
            VERBATIM)

    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET 8ChocChip POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:sfml-graphics>
                $<TARGET_FILE:sfml-window>
                $<TARGET_FILE:sfml-system>
                $<TARGET_FILE:sfml-audio>
                $<TARGET_FILE_DIR:8ChocChip>)
    endif()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

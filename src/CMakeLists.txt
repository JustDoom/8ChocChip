add_executable(8ChocChip)

add_subdirectory(sfml)
add_subdirectory(util)
add_subdirectory(emulator)

target_sources(8ChocChip PUBLIC main.cpp)

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

target_link_libraries(8ChocChip PRIVATE sfml-graphics sfml-audio sfml-window sfml-system nativefiledialog ${libname}++)

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
                $<TARGET_FILE_DIR:8ChocChip>)
    endif()
endif()

add_executable(8ChocChip main.cpp
        Keyboard.cpp
        Keyboard.h
        Speaker.cpp
        Speaker.h
        Renderer.cpp
        Renderer.h
        Cpu.cpp
        Cpu.h)
target_link_libraries(8ChocChip PRIVATE sfml-graphics sfml-audio)

if (WIN32)
    foreach (SFML_LIB ${SFML_LIBRARIES})
        get_filename_component(SFML_LIB_DIR ${SFML_LIB} DIRECTORY)
        file(GLOB SFML_DLLS "${SFML_LIB_DIR}/*.dll")
        foreach (SFML_DLL ${SFML_DLLS})
            add_custom_command(TARGET 8ChocChip POST_BUILD COMMAND
                    ${CMAKE_COMMAND} -E copy_if_different ${SFML_DLL} $<TARGET_FILE_DIR:8ChocChip>)
        endforeach()
    endforeach()
endif()

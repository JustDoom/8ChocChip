add_executable(8ChocChip main.cpp
        Keyboard.cpp
        Keyboard.h
        Speaker.cpp
        Speaker.h
        Renderer.cpp
        Renderer.h
        Cpu.cpp
        Cpu.h)
target_link_libraries(8ChocChip PRIVATE sfml-graphics sfml-audio sfml-window sfml-system)

# Copy DLLs needed for runtime on Windows
if(WIN32)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET 8ChocChip POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:sfml-graphics>
                $<TARGET_FILE:sfml-window>
                $<TARGET_FILE:sfml-system>
                $<TARGET_FILE_DIR:8ChocChip>)
    endif()
endif()

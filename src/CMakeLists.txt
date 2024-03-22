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
target_sources(8ChocChip PRIVATE
        MainMenu.cpp
        MainMenu.h
        Emulator.cpp
        Emulator.h
        InputHandler.cpp
        InputHandler.h)

add_subdirectory(ui)
add_subdirectory(emulator)
add_subdirectory(sdl)
add_subdirectory(util)
add_subdirectory(emulator)

target_sources(${EXECUTABLE_NAME} PRIVATE
        main.cpp
        ClaySDL3Renderer.cpp
        ClaySDL3Renderer.h
        EmulatorMain.cpp
        EmulatorMain.h)
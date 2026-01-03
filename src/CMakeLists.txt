add_subdirectory(sdl)
add_subdirectory(util)
add_subdirectory(emulator)

target_sources(8ChocChip PUBLIC main.cpp ClaySDL3Renderer.cpp ClaySDL3Renderer.h EmulatorMain.cpp EmulatorMain.h)
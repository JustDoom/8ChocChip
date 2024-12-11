add_subdirectory(sdl)
add_subdirectory(util)
add_subdirectory(emulator)

target_sources(8ChocChip PUBLIC main.cpp)

if(CMAKE_HOST_WIN32)
    set(libname "libconfig")
else()
    set(libname "config")
endif()

target_link_libraries(8ChocChip PRIVATE nfd ${libname}++ SDL2)
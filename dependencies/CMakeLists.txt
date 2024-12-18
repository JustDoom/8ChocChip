set(BUILD_EXAMPLES OFF CACHE BOOL "Disable examples for libconfig" FORCE)
set(BUILD_TESTS ON CACHE BOOL "Disable tests for libconfig" FORCE)
add_subdirectory(libconfig EXCLUDE_FROM_ALL)

if(SDL_VENDORED)
    add_subdirectory(sdl EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

add_subdirectory(sdl_ttf)
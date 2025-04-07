set(CLAY_INCLUDE_ALL_EXAMPLES OFF CACHE BOOL "Build all examples" FORCE)
add_subdirectory(clay)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

if(SDL_VENDORED)
    add_subdirectory(sdl EXCLUDE_FROM_ALL)
    add_subdirectory(sdl_image EXCLUDE_FROM_ALL)
    add_subdirectory(sdl_ttf EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()
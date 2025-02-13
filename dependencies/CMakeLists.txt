set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

if(SDL_VENDORED)
    add_subdirectory(sdl EXCLUDE_FROM_ALL)
else()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

add_subdirectory(sdl_ttf)
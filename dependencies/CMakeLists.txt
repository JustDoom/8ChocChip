if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sfml/CMakeLists.txt")
    message("${CMAKE_CURRENT_SOURCE_DIR}/sfml/CMakeLists.txt")
    message(FATAL_ERROR "Please initialize submodules using:\n  git submodule update --init --recursive")
endif()

add_subdirectory(sfml)
add_subdirectory(nativefiledialog)
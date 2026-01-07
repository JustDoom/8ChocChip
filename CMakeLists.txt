# Based on https://github.com/Ravbug/sdl3-sample

cmake_minimum_required(VERSION 3.20)

# Output dynamic libraries to the right place
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

project(8ChocChip LANGUAGES CXX VERSION 0.1)

if ((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "") # Disable shared builds on platforms where it does not make sense to use them
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP) # Parallelize each target, unless Ninja is the generator
    endif()
endif()

set(EXECUTABLE_NAME ${PROJECT_NAME})

if (ANDROID)
    # The SDL java code is hardcoded to load libmain.so on android, so we need to change EXECUTABLE_NAME
    set(EXECUTABLE_NAME main)
    add_library(${EXECUTABLE_NAME} SHARED)
else()
    add_executable(${EXECUTABLE_NAME})
endif()

add_subdirectory(src)

# To get an app icon on Apple platforms, add it to your executable. Afterward, the image file in Info.plist.in.
if(APPLE)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "assets/icon.png")
endif()

target_compile_features(${EXECUTABLE_NAME} PUBLIC cxx_std_20)

# on Web targets, we need CMake to generate a HTML webpage.
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
endif()

set(CLAY_INCLUDE_ALL_EXAMPLES OFF CACHE BOOL "Build all examples" FORCE)
add_subdirectory(dependencies/clay)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
set(JSON_MultipleHeaders OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/json EXCLUDE_FROM_ALL)

set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_GAMECONTROLLER OFF CACHE BOOL "" FORCE)
# Configure SDL by calling its CMake file. We use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory(dependencies/sdl EXCLUDE_FROM_ALL)

set(SDLTTF_VENDORED ON) # Tell SDL_ttf to build its own dependencies
set(SDL3TTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDL3TTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL3TTF_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/sdl_ttf EXCLUDE_FROM_ALL)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC
        nlohmann_json::nlohmann_json
        SDL3_ttf::SDL3_ttf
        SDL3::SDL3
)

# SDL_Image bug: https://github.com/libsdl-org/SDL_image/issues/506
if (APPLE AND NOT BUILD_SHARED_LIBS)
    find_library(IO_LIB ImageIO REQUIRED)
    find_library(CS_LIB CoreServices REQUIRED)
    find_library(CT_LIB CoreText REQUIRED)
    find_library(CG_LIB CoreGraphics REQUIRED)
    find_library(CF_LIB CoreFoundation REQUIRED)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CF_LIB} ${CT_LIB} ${IO_LIB} ${CS_LIB} ${CG_LIB})
endif()

if(EMSCRIPTEN)
    target_link_options(${EXECUTABLE_NAME} PRIVATE
            -sEXPORTED_FUNCTIONS=_main,_free,_malloc
            -sEXPORTED_RUNTIME_METHODS=ccall,cwrap
            -sDISABLE_EXCEPTION_CATCHING=0
    )
endif()

# Dealing with assets
# We have some non-code resources that our application needs in order to work. How we deal with those differs per platform.
if (APPLE)
    # On Apple targets, the application bundle has a "resources" subfolder where we can place our assets.
    # SDL_GetBasePath() gives us easy access to that location.
    set(input_root "${CMAKE_CURRENT_LIST_DIR}/assets")
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_root}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(${EXECUTABLE_NAME} PRIVATE ${FILE})
        set_property(SOURCE ${FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()
    add_resource("${CMAKE_CURRENT_LIST_DIR}/assets/font.ttf")
    add_resource("${CMAKE_CURRENT_LIST_DIR}/assets/icon.bmp")
    add_resource("${CMAKE_CURRENT_LIST_DIR}/assets/icon.png")
elseif(EMSCRIPTEN)
    # On the web, we have to put the files inside of the webassembly somewhat unintuitively, this is done via a linker argument.
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
            "--preload-file=${CMAKE_CURRENT_LIST_DIR}/assets@/assets"
    )
else()
    if (ANDROID)
        if (NOT MOBILE_ASSETS_DIR)
            message(FATAL_ERROR "Building for Android, but MOBILE_ASSETS_DIR is not set")
        endif()
        file(MAKE_DIRECTORY "${MOBILE_ASSETS_DIR}") # We need to create the project Assets dir otherwise CMake won't copy our assets.
    endif()


    macro(copy_helper filename)
        if (ANDROID)
            # MOBILE_ASSETS_DIR is set in the gradle file via the cmake command line and points to the Android Studio Assets folder.
            # when we copy assets there, the Android build pipeline knows to add them to the apk.
            set(outname "${MOBILE_ASSETS_DIR}/assets/${filename}")
        else()
            # For platforms that do not have a good packaging format, all we can do is copy the assets to the process working directory.
            set(outname "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/assets/${filename}")
        endif()
        add_custom_command(POST_BUILD
                TARGET "${EXECUTABLE_NAME}"
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/assets/${filename}" "${outname}"
        )
    endmacro()
    copy_helper("font.ttf")
    copy_helper("icon.bmp")
    copy_helper("icon.png")
endif()

# Set some extra configs for each platform
set(RESOURCE_FILES "assets/icon.png")
if(IOS)
    set(RESOURCE_FILES "src/iosLaunchScreen.storyboard;assets/icon.png") # make the splash screen show up on iOS
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        # On macOS, make a proper .app bundle instead of a bare executable
        MACOSX_BUNDLE TRUE
        # Set the Info.plist file for Apple Mobile platforms. Without this file, your app will not launch.
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist.in"

        # In Xcode, create a Scheme in the schemes dropdown for the app.
        XCODE_GENERATE_SCHEME TRUE
        # Identification for Xcode
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.imjustdoom.8chocchip"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.imjustdoom.8chocchip"
        XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
        RESOURCE "${RESOURCE_FILES}"
)

# On Visual Studio, set our app as the default project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXECUTABLE_NAME}")

# On macOS Platforms, ensure that the bundle is valid for distribution by calling fixup_bundle.
# note that fixup_bundle does not work on iOS, so you will want to use static libraries
# or manually copy dylibs and set rpaths
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    # tell Install about the target, otherwise fixup won't know about the transitive dependencies
    install(TARGETS ${EXECUTABLE_NAME}
            BUNDLE DESTINATION ./install COMPONENT Runtime
            RUNTIME DESTINATION ./install/bin COMPONENT Runtime
    )

    set(DEP_DIR "${CMAKE_BINARY_DIR}") # Where to look for dependencies when fixing up
    INSTALL(CODE
            "include(BundleUtilities)
        fixup_bundle(\"$<TARGET_BUNDLE_DIR:${EXECUTABLE_NAME}>\" \"\" \"${DEP_DIR}\")
        "
    )
    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
endif()
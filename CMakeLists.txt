cmake_minimum_required(VERSION 3.25)
project(pacman VERSION 1.0.0 LANGUAGES CXX)

# Modern C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type defaults to Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies via FetchContent
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

# SDL3 Core
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# SDL3_image for sprite loading
FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# SDL3_ttf for text rendering
FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# SDL3_mixer for audio
FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# Fetch all SDL3 dependencies
FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_ttf SDL3_mixer)

# Collect source files
file(GLOB_RECURSE PACMAN_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE PACMAN_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

# Create executable
# WIN32 - Windows GUI application (no console window)
# MACOSX_BUNDLE - macOS application bundle
add_executable(pacman WIN32 MACOSX_BUNDLE
    ${PACMAN_SOURCES}
    ${PACMAN_HEADERS}
)

# Include directories
target_include_directories(pacman PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link SDL3 libraries
target_link_libraries(pacman PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
)

# Copy assets to build directory
add_custom_command(TARGET pacman POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:pacman>/assets"
    COMMENT "Copying assets to build directory"
)

# macOS bundle configuration
if(APPLE)
    set_target_properties(pacman PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Pacman"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.game.pacman"
    )

    # Ad-hoc codesign the app bundle to allow execution
    add_custom_command(TARGET pacman POST_BUILD
        COMMAND codesign --force --deep --sign - "$<TARGET_BUNDLE_DIR:pacman>"
        COMMENT "Ad-hoc signing pacman.app"
    )
endif()

# Installation rules
install(TARGETS pacman
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY assets/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/assets
)

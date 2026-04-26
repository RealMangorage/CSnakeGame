cmake_minimum_required(VERSION 3.31)
project(CSnakeGame C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

# --- 1. SET GLOBAL FLAGS FIRST ---
# This tells both SDL3 and SDL3_mixer: "Do not build DLLs!"
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# SDL3 Specifics
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)

# Mixer Specifics
set(SDL3MIXER_VENDORED ON CACHE BOOL "" FORCE)
set(SDL3MIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDL3MIXER_VORBIS OFF CACHE BOOL "" FORCE)

# --- 2. FETCH CONTENT ---
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
)

FetchContent_Declare(
        SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG main
)

# This command actually downloads and adds the sub-projects
FetchContent_MakeAvailable(SDL3 SDL3_mixer)

# --- 3. YOUR EXECUTABLE ---
add_executable(CSnakeGame
        main.c
        snake.c
        snake.h
        SoundPlayer.h
        SoundPlayer.c
)

# --- 4. LINKING ---
# Since we are using FetchContent, we link to the static targets directly.
target_link_libraries(CSnakeGame PRIVATE SDL3-static SDL3_mixer-static)

if(WIN32)
    set_target_properties(CSnakeGame PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
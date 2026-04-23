cmake_minimum_required(VERSION 3.31)
project(CSnakeGame C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

# Portable!
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(SDL3)

add_executable(CSnakeGame main.c snake.c snake.h)

# Link against SDL3
target_link_libraries(CSnakeGame PRIVATE SDL3::SDL3)

# Fix for "unresolved external symbol printf" on Windows MSVC
if(MSVC)
    target_link_libraries(CSnakeGame PRIVATE ucrt.lib)
endif()

# Windows-specific: Copy the SDL3 DLL to the output folder for local testing
if(WIN32)
    add_custom_command(TARGET CSnakeGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3>"
            $<TARGET_FILE_DIR:CSnakeGame>
            VERBATIM
    )
endif()
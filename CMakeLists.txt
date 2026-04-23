cmake_minimum_required(VERSION 3.31)

# 1. Define the project
project(CSnakeGame C)

# 2. Standard C configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 3. Use FetchContent to download SDL3 automatically
include(FetchContent)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        main  # You can replace 'main' with a specific tag like 'preview-3.2.0'
)

# This downloads and adds SDL3 to your project sub-build
FetchContent_MakeAvailable(SDL3)

# 4. Define your executable
# Listing headers (snake.h) is optional but helps in IDEs like CLion
add_executable(
        CSnakeGame
        main.c
        snake.c
        snake.h
)

# 5. Link against SDL3
# FetchContent provides the 'SDL3::SDL3' target automatically
target_link_libraries(CSnakeGame PRIVATE SDL3::SDL3)

# 6. Windows-specific: Copy the SDL3 DLL to the output folder
# Without this, the .exe will fail to start because it won't find SDL3.dll
if(WIN32)
    add_custom_command(TARGET CSnakeGame POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3>"
            $<TARGET_FILE_DIR:CSnakeGame>
            VERBATIM
    )
endif()
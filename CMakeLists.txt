cmake_minimum_required(VERSION 3.31)
project(CSnakeGame C)

set(CMAKE_C_STANDARD 11)

if(MINGW)
    add_link_options("-municode")
    add_compile_options("-municode")
endif()

add_executable(
        CSnakeGame
        WIN32
        main.c
        snake.c
        snake.h
)
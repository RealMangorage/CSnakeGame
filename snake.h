#ifndef SNAKE_H
#define SNAKE_H

#include "SDL3/SDL_render.h"

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;


void Snake_SetDimensions(int width, int height);
void Snake_SetDirection(Direction newDirection);
void Snake_SetSize(int size);

void Snake_Grow(); // Cheat/Debug

void Snake_Init();
void Snake_Dispose();

void Snake_Main_Update(SDL_Window* window);
void Snake_Render(SDL_Renderer* renderer);

#endif
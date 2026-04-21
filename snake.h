#ifndef SNAKE_H
#define SNAKE_H

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;


void Snake_SetDimensions(int width, int height);
void Snake_SetSize(int size);
void Snake_Init();
void Snake_Dispose();

void Snake_SetDirection(Direction newDirection);

void Snake_Reset();

void Snake_Main_Update(HWND hwnd);
void Snake_Render(HDC hdc);

#endif
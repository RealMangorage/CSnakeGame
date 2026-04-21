#ifndef SNAKE_H
#define SNAKE_H

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;


void setDimensions(int width, int height);
void setSize(int size);
void init();
void dispose();

void setDirection(Direction direction);

void resetSnake();

void update(HWND hwnd);
void render(HDC hdc);

#endif
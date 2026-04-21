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

void setDirection(Direction direction);

void resetSnake();
void grow(); // Debug

void update();
void render(HDC hdc);

#endif
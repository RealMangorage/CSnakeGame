#include <windows.h>
#include <stdio.h>
#include "snake.h"

// ----------------------- STRUCTS START ----------------------- //
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point* segments;
    int currentLength;
    int maxLength;
} Snake;

// ----------------------- STRUCTS END ----------------------- //


// ----------------------- DEBUG STATES START ----------------------- //
static int skipBorderCollisionChecks = 0;
static int skipSnakeCollisionChecks  = 0;
static int alwaysUpdateTitle         = 0;
// ----------------------- DEBUG STATES END ----------------------- //


// ----------------------- RENDER STATE START ----------------------- //
HBRUSH headBrush;
HBRUSH segmentBrush;
HBRUSH appleBrush;
// ----------------------- RENDER STATE END ----------------------- //

// ----------------------- STATE DATA START ----------------------- //
int ticks = 0;
int size  = 0;
int width, height;

Direction direction;

Snake gameSnake;
Point gameApple;
RECT drawingRect;

Point start[] = {
        {11, 10},
        {12, 10},
        {13, 10},
        {14, 10}
};

// ----------------------- STATE DATA END ----------------------- //


// ----------------------- PRIVATE FUNCS START ----------------------- //

// Make Compiler stop complaining about unchanged values, these are for debug/dev
void Snake_SetDebugOption(int option, int value) {
    if (option == 1)
        skipBorderCollisionChecks = value;
    if (option == 2)
        skipSnakeCollisionChecks = value;
    if (option == 3)
        alwaysUpdateTitle = value;
}

RECT Snake_NewGridRect(int gx, int gy, int tSize)
{
    return (RECT) {
            gx * tSize,
            gy * tSize,
            (gx + 1) * tSize,
            (gy + 1) * tSize
    };
}

void Snake_UpdateGridRect(RECT* rect, int gx, int gy, int tSize) {
    rect->left = gx * tSize;
    rect->top = gy * tSize;
    rect->right = (gx + 1) * tSize;
    rect->bottom = (gy + 1) * tSize;
}

int Snake_isOutOfBounds() {
    // Calculate the number of cells available in the grid
    int maxGridX = width / size;
    int maxGridY = height / size;

    if (gameSnake.segments[0].x < 0 || gameSnake.segments[0].x >= maxGridX || gameSnake.segments[0].y < 0 || gameSnake.segments[0].y >= maxGridY) {
        return 1;
    }

    return 0;
}

int Snake_isPointsEqual(Point p1, Point p2) {
    return (p1.x == p2.x && p1.y == p2.y);
}

int Snake_isIntersecting(Snake* snake) {
    if (!snake || snake->currentLength < 2) return 0;

    Point head = snake->segments[0];
    for (int i = 1; i < snake->currentLength; i++) {
        if (Snake_isPointsEqual(head, snake->segments[i]))
            return 1;
    }

    return 0;
}

void Snake_Push(Snake* s, Point newHead, int grow)
{
    // ensure capacity
    if (s->currentLength >= s->maxLength)
    {
        int newCap = s->maxLength + 10;

        Point* temp = realloc(s->segments, sizeof(Point) * newCap);
        if (!temp) return;

        s->segments = temp;
        s->maxLength = newCap;
    }

    // shift body right (makes space for head)
    memmove(
            s->segments + 1,
            s->segments,
            sizeof(Point) * s->currentLength
    );

    // insert new head
    s->segments[0] = newHead;

    // snake always grows by 1 temporarily
    s->currentLength++;

    if (!grow)
    {
        // remove tail logically
        s->currentLength--;
    }
}

void Snake_Update(Snake* s, Direction dir, int grow) {
    Point newHead = {s->segments[0].x, s->segments[0].y};

    if (dir == UP)    newHead.y--;
    if (dir == DOWN)  newHead.y++;
    if (dir == LEFT)  newHead.x--;
    if (dir == RIGHT) newHead.x++;

    Snake_Push(s, newHead, grow);
}

void Snake_Grow() {
    Snake_Update(&gameSnake, direction, 1);
}

void Snake_GenerateApple() {
    int maxGridX = (width / size) - 1;
    int maxGridY = (height / size) - 1;
    int overlapping;

    do {
        overlapping = 0;
        // 1. Pick a random spot
        gameApple.x = rand() % maxGridX;
        gameApple.y = rand() % maxGridY;

        // 2. Make sure that spot isn't currently occupied by the snake
        for (int i = 0; i < gameSnake.currentLength; i++) {
            if (gameSnake.segments[i].x == gameApple.x && gameSnake.segments[i].y == gameApple.y) {
                overlapping = 1;
                break;
            }
        }
    } while (overlapping); // Keep trying until we find an empty spot
}

void Snake_UpdateTitle(HWND hwnd) {
    char title[64];
    int score = (gameSnake.currentLength - 4) * 10;
    sprintf(title, "Snake Game - Score: %d", score);
    SetWindowTextA(hwnd, title);
}

// ----------------------- PRIVATE FUNCS END ----------------------- //


// ----------------------- API FUNCS START ----------------------- //
void Snake_Reset()
{
    gameSnake.currentLength = 0;

    for (int i = 0; i < 4; i++)
    {
        Snake_Push(&gameSnake, start[i], 1);
    }

    Snake_GenerateApple();
    direction = RIGHT;
}

void Snake_SetDimensions(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

void Snake_SetSize(int newSize) {
    size = newSize;
}

void Snake_Init() {
    headBrush = CreateSolidBrush(RGB(255, 0, 0));
    segmentBrush = CreateSolidBrush(RGB(0, 255, 0));
    appleBrush = CreateSolidBrush(RGB(0,0, 255));
    drawingRect = Snake_NewGridRect(0, 0, size);

    gameSnake.segments = malloc(sizeof(Point) * 10);
    gameSnake.currentLength = 0;
    gameSnake.maxLength = 1;

    Snake_Reset();
}

void Snake_SetDirection(Direction newDirection) {
    if (direction == LEFT && newDirection == RIGHT) return;
    if (direction == RIGHT && newDirection == LEFT) return;
    if (direction == UP && newDirection == DOWN) return;
    if (direction == DOWN && newDirection == UP) return;
    direction = newDirection;
}

void Snake_Main_Update(HWND hwnd) {
    ticks++;

    if (ticks % 4 == 0) {
        if (Snake_isOutOfBounds() && !skipBorderCollisionChecks) {
            printf("Game Over! Hit a wall at %d, %d\n", gameSnake.segments[0].x, gameSnake.segments[0].y);
            Snake_Reset();
            Snake_UpdateTitle(hwnd);
            return;
        }

        if (Snake_isIntersecting(&gameSnake) && !skipSnakeCollisionChecks) {
            printf("Game Over! Ran into self at %d, %d\n", gameSnake.segments[0].x, gameSnake.segments[0].y);
            Snake_Reset();
            Snake_UpdateTitle(hwnd);
            return;
        }

        if (Snake_isPointsEqual(gameSnake.segments[0], gameApple)) {
            Snake_GenerateApple();
            Snake_Update(&gameSnake, direction, 1);
            Snake_UpdateTitle(hwnd);
        } else {
            Snake_Update(&gameSnake, direction, 0);
        }


        if (alwaysUpdateTitle) {
            Snake_UpdateTitle(hwnd);
        }
    }

}

void Snake_Render(HDC hdc) {
    int head = 0;

    for (int i = 0; i < gameSnake.currentLength; i++) {

        Snake_UpdateGridRect(&drawingRect, gameSnake.segments[i].x, gameSnake.segments[i].y, size);
        FillRect(hdc, &drawingRect, head ? headBrush : segmentBrush);

        if (!head)
            head = 1;
    }

    Snake_UpdateGridRect(&drawingRect,gameApple.x, gameApple.y, size);
    FillRect(hdc, &drawingRect, appleBrush);
}

void Snake_Free(Snake* s) {
    if (!s) return;

    if (s->segments != NULL) {
        free(s->segments);
        s->segments = NULL;
    }

    s->currentLength = 0;
    s->maxLength = 0;
}

void Snake_Dispose() {
    // Free snake heap memory
    Snake_Free(&gameSnake);

    // Free WinAPI GDI objects
    if (headBrush) {
        DeleteObject(headBrush);
        headBrush = NULL;
    }

    if (segmentBrush) {
        DeleteObject(segmentBrush);
        segmentBrush = NULL;
    }

    if (appleBrush) {
        DeleteObject(appleBrush);
        appleBrush = NULL;
    }
}

// ----------------------- API FUNCS END ----------------------- //
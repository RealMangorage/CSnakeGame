#include <windows.h>
#include <stdio.h>
#include "snake.h"

// ----------------------- STRUCTS START ----------------------- //
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point* segments;
    int length;
} Snake;

// ----------------------- STRUCTS END ----------------------- //


// ----------------------- RENDER STATE START ----------------------- //
HBRUSH headBrush;
HBRUSH segmentBrush;
HBRUSH appleBrush;
// ----------------------- RENDER STATE END ----------------------- //

// ----------------------- STATE DATA START ----------------------- //
int ticks = 0;
int size = 0;
int width, height;

Direction direction = RIGHT;

Snake gameSnake;
Point gameApple;

// ----------------------- STATE DATA END ----------------------- //



// ----------------------- PRIVATE FUNCS START ----------------------- //
RECT GridRect(int gx, int gy, int tSize)
{
    return (RECT) {
            gx * tSize,
            gy * tSize,
            (gx + 1) * tSize,
            (gy + 1) * tSize
    };
}

int isOutOfBounds() {
    // Calculate the number of cells available in the grid
    int maxGridX = width / size;
    int maxGridY = height / size;

    if (gameSnake.segments[0].x < 0 || gameSnake.segments[0].x >= maxGridX || gameSnake.segments[0].y < 0 || gameSnake.segments[0].y >= maxGridY) {
        return 1;
    }

    return 0;
}

int isEqualPoints(Point p1, Point p2) {
    return (p1.x == p2.x && p1.y == p2.y);
}

int isIntersecting(Snake* snake) {
    if (!snake || snake->length < 2) return 0;

    Point head = snake->segments[0];
    for (int i = 1; i < snake->length; i++) {
        if (isEqualPoints(head, snake->segments[i]))
            return 1;
    }

    return 0;
}

void Snake_PushFront(Snake* s, Point p) {
    s->length++;

    // Reallocating the actual pointer
    Point* temp = realloc(s->segments, sizeof(Point) * s->length);
    if (temp == NULL) return; // Always good to check for safety
    s->segments = temp;

    if (s->length > 1) {
        memmove(&s->segments[1], &s->segments[0], sizeof(Point) * (s->length - 1));
    }

    s->segments[0] = p;
}

void Snake_PopBack(Snake* s) {
    if (s->length > 0) {
        s->length--;
    }
}

void Snake_Update(Snake* s, Direction dir, int grow) {
    Point newHead = s->segments[0];

    if (dir == UP)    newHead.y--;
    if (dir == DOWN)  newHead.y++;
    if (dir == LEFT)  newHead.x--;
    if (dir == RIGHT) newHead.x++;

    Snake_PushFront(s, newHead);

    if (!grow) {
        Snake_PopBack(s);
    }

}

Point GenerateApple() {
    Point apple;
    int maxGridX = (width / size) - 1;
    int maxGridY = (height / size) - 1;
    int overlapping;

    do {
        overlapping = 0;
        // 1. Pick a random spot
        apple.x = rand() % maxGridX;
        apple.y = rand() % maxGridY;

        // 2. Make sure that spot isn't currently occupied by the snake
        for (int i = 0; i < gameSnake.length; i++) {
            if (gameSnake.segments[i].x == apple.x && gameSnake.segments[i].y == apple.y) {
                overlapping = 1;
                break;
            }
        }
    } while (overlapping); // Keep trying until we find an empty spot

    return apple;
}

void updateTitle(HWND hwnd) {
    char title[64];
    int score = (gameSnake.length - 4) * 10;
    sprintf(title, "Snake Game - Score: %d", score);
    SetWindowTextA(hwnd, title);
}

// ----------------------- PRIVATE FUNCS END ----------------------- //


// ----------------------- API FUNCS START ----------------------- //
void resetSnake() {
    if (gameSnake.segments != NULL) {
        free(gameSnake.segments);
    }

    gameSnake.length = 4;
    gameSnake.segments = malloc(sizeof(Point) * gameSnake.length);

    gameSnake.segments[0] = (Point){13, 10};
    gameSnake.segments[1] = (Point){12, 10};
    gameSnake.segments[2] = (Point){11, 10};
    gameSnake.segments[3] = (Point){10, 10};
    direction = RIGHT;
    gameApple = GenerateApple();
}

void setDimensions(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

void setSize(int newSize) {
    size = newSize;
}

void init() {
    headBrush = CreateSolidBrush(RGB(255, 0, 0));
    segmentBrush = CreateSolidBrush(RGB(0, 255, 0));
    appleBrush = CreateSolidBrush(RGB(0,0, 255));
}

void setDirection(Direction newDirection) {
    if (direction == LEFT && newDirection == RIGHT) return;
    if (direction == RIGHT && newDirection == LEFT) return;
    if (direction == UP && newDirection == DOWN) return;
    if (direction == DOWN && newDirection == UP) return;
    direction = newDirection;
}

void update(HWND hwnd) {
    ticks++;
    if (ticks % 5 == 0) {

        if (isOutOfBounds()) {
            printf("Game Over! Hit a wall at %d, %d\n", gameSnake.segments[0].x, gameSnake.segments[0].y);
            resetSnake();
            updateTitle(hwnd);
            return;
        }

        if (isIntersecting(&gameSnake)) {
            printf("Game Over! Ran into self at %d, %d\n", gameSnake.segments[0].x, gameSnake.segments[0].y);
            resetSnake();
            updateTitle(hwnd);
            return;
        }

        if (isEqualPoints(gameSnake.segments[0], gameApple)) {
            gameApple = GenerateApple();
            Snake_Update(&gameSnake, direction, 1);
            updateTitle(hwnd);
        } else {
            Snake_Update(&gameSnake, direction, 0);
        }

    }
}

void render(HDC hdc) {
    boolean head = FALSE;

    for (int i = 0; i < gameSnake.length; i++) {
        RECT segmentRect = GridRect(gameSnake.segments[i].x, gameSnake.segments[i].y, size);
        FillRect(hdc, &segmentRect, head ? headBrush : segmentBrush);

        if (head == FALSE)
            head = TRUE;
    }

    RECT appleRect =  GridRect(gameApple.x, gameApple.y, size);
    FillRect(hdc, &appleRect, appleBrush);
}

void dispose() {
    DeleteObject(headBrush);
    DeleteObject(segmentBrush);
    DeleteObject(appleBrush);
}

// ----------------------- API FUNCS END ----------------------- //
#include "snake.h"
#include "food.h"
#include "move.h"
#include "gameover.h"
#include "music.h"
#include "SoundPlayer.h"


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


// ----------------------- STATE DATA START ----------------------- //
int ticks = 0;
int size  = 0;
int width, height;

Direction direction;
SDL_FRect segmentRect;

Snake gameSnake;
Point gameApple;

Point start[] = {
        {11, 10},
        {12, 10},
        {13, 10},
        {14, 10}
};

SoundAsset* soundFood;
SoundAsset* soundMove;
SoundAsset* soundGameOver;
SoundAsset* soundMusic;

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

void Snake_UpdateGridRect(SDL_FRect* rect, int gx, int gy, int tSize) {
    // SDL_FRect uses (x, y, width, height)
    rect->x = (float)(gx * tSize);
    rect->y = (float)(gy * tSize);
    rect->w = (float)tSize;
    rect->h = (float)tSize;
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

        Point* temp = SDL_realloc(s->segments, sizeof(Point) * newCap);
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
        gameApple.x = SDL_rand(32) % maxGridX;
        gameApple.y = SDL_rand(32) % maxGridY;

        // 2. Make sure that spot isn't currently occupied by the snake
        for (int i = 0; i < gameSnake.currentLength; i++) {
            if (gameSnake.segments[i].x == gameApple.x && gameSnake.segments[i].y == gameApple.y) {
                overlapping = 1;
                break;
            }
        }
    } while (overlapping); // Keep trying until we find an empty spot
}

void Snake_UpdateTitle(SDL_Window* window) {
    char title[64];
    int score = (gameSnake.currentLength - 4) * 10;
    SDL_snprintf(title, sizeof(title), "Snake Game - Score: %d", score);
    SDL_SetWindowTitle(window, title);
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
    gameSnake.segments = SDL_malloc(sizeof(Point) * 10);
    gameSnake.currentLength = 0;
    gameSnake.maxLength = 1;

    SoundPlayer_Init();

    soundFood = SoundPlayer_LoadFromMem(food_mp3, food_mp3_len);
    soundGameOver = SoundPlayer_LoadFromMem(gameover_mp3, gameover_mp3_len);
    soundMove = SoundPlayer_LoadFromMem(gameover_mp3, gameover_mp3_len);
    soundMusic = SoundPlayer_LoadFromMem(music_mp3, music_mp3_len);

//    SoundPlayer_PlayLoop(soundMusic);

    Snake_Reset();
}

void Snake_SetDirection(Direction newDirection) {
    if (direction == LEFT && newDirection == RIGHT) return;
    if (direction == RIGHT && newDirection == LEFT) return;
    if (direction == UP && newDirection == DOWN) return;
    if (direction == DOWN && newDirection == UP) return;
    direction = newDirection;
}

void Snake_Main_Update(SDL_Window* window) {
    ticks++;


    if (SoundPlayer_IsPlaying(soundGameOver)) {
        SDL_Log("Playing!");
    }

    if (ticks % 4 == 0) {
        if (Snake_isOutOfBounds() && !skipBorderCollisionChecks) {
            SoundPlayer_Play(soundGameOver);
            Snake_Reset();
            Snake_UpdateTitle(window);
            return;
        }

        if (Snake_isIntersecting(&gameSnake) && !skipSnakeCollisionChecks) {
            SoundPlayer_Play(soundGameOver);
            Snake_Reset();
            Snake_UpdateTitle(window);
            return;
        }

        if (Snake_isPointsEqual(gameSnake.segments[0], gameApple)) {
            Snake_GenerateApple();
            SoundPlayer_Play(soundFood);
            Snake_Update(&gameSnake, direction, 1);
            Snake_UpdateTitle(window);
        } else {
            Snake_Update(&gameSnake, direction, 0);
        }


        if (alwaysUpdateTitle) {
            Snake_UpdateTitle(window);
        }
    }

}

void Snake_Render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    int head = 0;

    for (int i = 0; i < gameSnake.currentLength; i++) {
        Snake_UpdateGridRect(&segmentRect, gameSnake.segments[i].x, gameSnake.segments[i].y, size);

        SDL_RenderFillRect(renderer, &segmentRect);

        if (!head) {
            head = 1;
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    Snake_UpdateGridRect(&segmentRect, gameApple.x, gameApple.y, size);
    SDL_RenderFillRect(renderer, &segmentRect);
}

void Snake_Free(Snake* s) {
    if (!s) return;

    if (s->segments != NULL) {
        SDL_free(s->segments);
        s->segments = NULL;
    }

    s->currentLength = 0;
    s->maxLength = 0;
}

void Snake_Dispose() {
    // Free snake heap memory
    Snake_Free(&gameSnake);
    SoundPlayer_FreeAsset(soundMusic);
    SoundPlayer_FreeAsset(soundMove);
    SoundPlayer_FreeAsset(soundGameOver);
    SoundPlayer_FreeAsset(soundFood);
    SoundPlayer_Quit();
}

// ----------------------- API FUNCS END ----------------------- //
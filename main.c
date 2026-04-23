#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "snake.h"

#define TILE_SIZE 20
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char* argv[]) {
    // 1. Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("Snake Game -> Score: 0",
                                     SCREEN_WIDTH, SCREEN_HEIGHT,
                                     0, &window, &renderer
                                     )) {
        SDL_Log("Window/Renderer error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Snake_SetSize(TILE_SIZE);
    Snake_SetDimensions(SCREEN_WIDTH, SCREEN_HEIGHT);
    Snake_Init();

    int keep_running = 1;
    SDL_Event event;

    while (keep_running) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                keep_running = 0;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_UP:    Snake_SetDirection(UP);    break;
                    case SDLK_DOWN:  Snake_SetDirection(DOWN);  break;
                    case SDLK_LEFT:  Snake_SetDirection(LEFT);  break;
                    case SDLK_RIGHT: Snake_SetDirection(RIGHT); break;
                    case SDLK_ESCAPE: keep_running = 0;          break;
                }
            }
        }

        Snake_Main_Update(window);

        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderClear(renderer);

        Snake_Render(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    Snake_Dispose();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
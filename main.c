#include "Chip8VM.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_NAME "Chippy8"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320



struct sdl_app {
    SDL_Window* window;
    SDL_Surface* screenSurface;
    SDL_Event event;
};



void init_sdl_app(struct sdl_app* app) {
    // Init SDL's video and audio components
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Unable to initialise SDL. Error: %s\n", SDL_GetError());
        exit(1);
    }

    app->window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (app->window == NULL) {
        printf("Unable to initialise SDL window. Error: %s\n", SDL_GetError());
        exit(1);
    }

    app->screenSurface = SDL_GetWindowSurface(app->window);
    if (app->screenSurface == NULL) {
        printf("Unable to get screen surface. Error: %s\n", SDL_GetError());
        exit(1);
    }
}

void destroy_sdl_app(struct sdl_app* app) {
    SDL_DestroyWindow(app->window);
    SDL_Quit();
}
int main(int argc, const char** argv) {
    struct sdl_app app = {0};
    init_sdl_app(&app);

    SDL_FillRect(app.screenSurface, NULL, SDL_MapRGB(app.screenSurface->format, 255, 255, 255));

    SDL_UpdateWindowSurface(app.window);

    bool quit = false;

    struct chip8_vm vm = {0};
    printf("%s\n", argv[1]);
    loadROM(&vm, argv[1]);
    
    while (!quit) {
        while (SDL_PollEvent(&app.event)) {
            if (app.event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    destroy_sdl_app(&app);
    return 0;
}
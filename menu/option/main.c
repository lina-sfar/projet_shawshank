#include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

int main() {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_JPG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window *win = SDL_CreateWindow("Options",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_SHOWN);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED);

    Menu menu;
    menu_init(ren, &menu);

    int running = 1;
    SDL_Event e;

    while (running) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;

            menu_update(&menu, &e, win);
        }

        SDL_RenderClear(ren);
        menu_render(ren, &menu);
        SDL_RenderPresent(ren);
    }

    menu_destroy(&menu);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

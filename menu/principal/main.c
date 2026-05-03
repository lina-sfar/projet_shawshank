#include "header.h"

/* State (only what we need) */
#define MENU_PRINCIPAL 0
#define MENU_OPTIONS 1

int main(int argc, char* argv[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event e;

    int running = 1;

    /* SDL INIT */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG Error: %s\n", IMG_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Menu",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Window Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer Error: %s\n", SDL_GetError());
        return 1;
    }

    Menu menu;
    if (!initMenu(&menu, renderer)) {
        printf("Menu init failed\n");
        return 1;
    }

    /* MAIN LOOP */
    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = 0;

            /* 🔥 FIX: handleMenu MUST be inside event loop */
            int action = handleMenu(&menu, &e);

            if (action == -1)   /* quit */
            {
                running = 0;
            }
            else if (action == MENU_OPTIONS)   /* OPEN OPTIONS */
            {
                run_options(renderer);
            }
        }

        SDL_RenderClear(renderer);
        renderMenu(&menu, renderer);
        SDL_RenderPresent(renderer);
    }

    /* CLEANUP */
    destroyMenu(&menu);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

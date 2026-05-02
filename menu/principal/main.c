   #include "header.h"

int main(int argc, char* argv[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event e;

    int running = 1;

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // Init SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG Error: %s\n", IMG_GetError());
        return 1;
    }

    // Init SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    // Create window
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

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer Error: %s\n", SDL_GetError());
        return 1;
    }

    // Init menu
    Menu menu;
    if (!initMenu(&menu, renderer)) {
        printf("Menu init failed\n");
        return 1;
    }

    // Main loop
    while (running) {

        while (SDL_PollEvent(&e)) {

            if (e.type == SDL_QUIT)
                running = 0;

            // Gestion du menu (Quit button déjà géré ici)
            handleMenu(&menu, &e, &running);
        }

        SDL_RenderClear(renderer);

        renderMenu(&menu, renderer);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    destroyMenu(&menu);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

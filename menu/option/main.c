#include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

void run_options(SDL_Renderer *ren)
{
    Menu menu;
    menu_init(ren, &menu);

    int running = 1;
    SDL_Event e;

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                return;

            // IMPORTANT: remove window dependency
            menu_update(&menu, &e, NULL);

            // optional: ESC to go back
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                running = 0;
        }

        SDL_RenderClear(ren);
        menu_render(ren, &menu);
        SDL_RenderPresent(ren);
    }

    menu_destroy(&menu);
}

#include "puzzle.h"
#include <stdio.h>

void run_lot6(SDL_Renderer* renderer)
{
    TTF_Font *font  = TTF_OpenFont("arial.ttf", 28);
    TTF_Font *small = TTF_OpenFont("arial.ttf", 18);

    if (!font || !small)
    {
        printf("Font error: %s\n", TTF_GetError());
        return;
    }

    Game g;
    init_game(renderer, &g);

    int running = 1;
    SDL_Event e;

    while (running)
    {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = 0;

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_x && g.solved)
                    running = 0;

                if (e.key.keysym.sym == SDLK_t && g.time_up)
                    new_round(renderer, &g);

                if (e.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
                handle_mousedown(&g, e.button.x, e.button.y);

            if (e.type == SDL_MOUSEBUTTONUP)
                handle_mouseup(&g, e.button.x, e.button.y);

            if (e.type == SDL_MOUSEMOTION)
                handle_mousemove(&g, e.motion.x, e.motion.y);
        }

        SDL_SetRenderDrawColor(renderer, 18, 20, 30, 255);
        SDL_RenderClear(renderer);

        draw_game(renderer, font, small, &g, mx, my);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    free_game(&g);

    TTF_CloseFont(font);
    TTF_CloseFont(small);
}

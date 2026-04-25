#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "puzzle.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *win = SDL_CreateWindow(
        "Puzzle Quiz",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, 0);

    SDL_Renderer *r = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font  = TTF_OpenFont("arial.ttf", 28);
    TTF_Font *small = TTF_OpenFont("arial.ttf", 18);
    if (!font || !small) {
        printf("Font error: %s\n", TTF_GetError());
        return 1;
    }

    Game g;
    init_game(r, &g);

    int running = 1;
    SDL_Event e;

    while (running) {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = 0;

            if (e.type == SDL_KEYDOWN) {
                /* X to close when solved */
                if (e.key.keysym.sym == SDLK_x && g.solved)
                    running = 0;

                /* T to try again when time is up */
                if (e.key.keysym.sym == SDLK_t && g.time_up)
                    new_round(r, &g);

                /* ESC always quits */
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                handle_mousedown(&g, e.button.x, e.button.y);

            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
                handle_mouseup(&g, e.button.x, e.button.y);

            if (e.type == SDL_MOUSEMOTION)
                handle_mousemove(&g, e.motion.x, e.motion.y);
        }

        SDL_SetRenderDrawColor(r, 18, 20, 30, 255);
        SDL_RenderClear(r);

        draw_game(r, font, small, &g, mx, my);

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }

    free_game(&g);
    TTF_CloseFont(font);
    TTF_CloseFont(small);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

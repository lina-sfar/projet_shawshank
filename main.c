#include "lot1.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window* win = SDL_CreateWindow("LOT1 - Deux joueurs", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    run_lot1(ren);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

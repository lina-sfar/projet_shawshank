#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "lot1.h"
#include "lot2.h"
#include "lot3.h"
#include "lot4.h"
#include "lot5.h"
#include "lot6.h"

int main() {
    // Init SDL systems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return 1;
    }

    if (TTF_Init() == -1) {
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window + renderer
    SDL_Window* win = SDL_CreateWindow(
        "Game Project",
        100, 100,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (!win) {
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    if (!ren) {
        SDL_DestroyWindow(win);
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // =========================
    // GAME FLOW (LOTS)
    // =========================

    run_lot1(ren);
    run_lot2(ren);
    run_lot3(ren);
    run_lot4(ren);
    run_lot5(ren);
    run_lot6(ren);

    // =========================
    // CLEANUP
    // =========================

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);

    Mix_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

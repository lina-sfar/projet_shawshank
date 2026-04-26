#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "../lot1/lot1.h"
#include "../lot2/lot2.h"
#include "../lot3/lot3.h"
#include "../lot4/lot4.h"
#include "../lot5/lot5.h"
#include "../lot6/lot6.h"

int main() {
    printf("=== Projet Shawshank ===\n");
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window* win = SDL_CreateWindow("Shawshank Redemption",
                                       100, 100, 800, 600,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // Lancer le lot1 pour l'instant (tu pourras faire un menu plus tard)
    run_lot1(ren);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

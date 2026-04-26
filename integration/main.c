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

void afficherMenu() {
    printf("\n=== SHAWSHANK REDEMPTION ===\n");
    printf("1. Lancer le LOT 1 (Joueur)\n");
    printf("2. Lancer le LOT 2 (Background)\n");
    printf("3. Lancer le LOT 3 (Ennemis)\n");
    printf("4. Lancer le LOT 4 (Mini map)\n");
    printf("5. Lancer le LOT 5 (Quiz)\n");
    printf("6. Lancer le LOT 6 (Puzzle)\n");
    printf("0. Quitter\n");
    printf("Votre choix : ");
}

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_Window* win = SDL_CreateWindow("Shawshank Redemption",
                                       100, 100, 800, 600,
                                       SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    int choix;
    do {
        afficherMenu();
        scanf("%d", &choix);
        switch(choix) {
            case 1: run_lot1(ren); break;
            case 2: run_lot2(ren); break;
            case 3: run_lot3(ren); break;
            case 4: run_lot4(ren); break;
            case 5: run_lot5(ren); break;
            case 6: run_lot6(ren); break;
            case 0: printf("Au revoir !\n"); break;
            default: printf("Choix invalide.\n");
        }
    } while(choix != 0);

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

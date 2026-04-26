#ifndef ENIGME_H
#define ENIGME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define MAX_QUESTION 256
#define MAX_REPONSE 128
#define MAX_ENIGMES 50

typedef struct {

    char question[MAX_QUESTION];
    char reponse1[MAX_REPONSE];
    char reponse2[MAX_REPONSE];
    char reponse3[MAX_REPONSE];

    int bonne_reponse;

    int deja_vu;

    int etat;

    SDL_Texture* texture_question;
    SDL_Texture* texture_rep1;
    SDL_Texture* texture_rep2;
    SDL_Texture* texture_rep3;

    SDL_Rect pos_question;
    SDL_Rect pos_rep1;
    SDL_Rect pos_rep2;
    SDL_Rect pos_rep3;

} Enigme;

// MAIN ENTRY POINT FOR LOT5
void run_lot5(SDL_Renderer* renderer);

#endif

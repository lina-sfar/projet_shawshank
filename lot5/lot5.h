#ifndef ENIGME_H
#define ENIGME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MAX_QUESTION 256
#define MAX_REPONSE 128
#define MAX_ENIGMES 50

typedef struct {
    // Les questions et réponses
    char question[MAX_QUESTION];
    char reponse1[MAX_REPONSE];
    char reponse2[MAX_REPONSE];
    char reponse3[MAX_REPONSE];
    int bonne_reponse;  // 1, 2 ou 3
    
    // Pour la génération aléatoire (champ deja vu)
    int deja_vu;  // 0 = pas encore vue, 1 = déjà utilisée
    
    // État de l'énigme (-1: echec, 0: non résolu, 1: succès)
    int etat;
    
    // Textures SDL
    SDL_Texture* texture_question;
    SDL_Texture* texture_rep1;
    SDL_Texture* texture_rep2;
    SDL_Texture* texture_rep3;
    
    // Positions à l'écran
    SDL_Rect pos_question;
    SDL_Rect pos_rep1;
    SDL_Rect pos_rep2;
    SDL_Rect pos_rep3;
} Enigme;
void lot5
#endif

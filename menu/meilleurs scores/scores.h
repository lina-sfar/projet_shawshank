#ifndef SCORES_H
#define SCORES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// affiche le sous-menu scores
// retourne 1 pour revenir au menu principal, 0 pour quitter
int showScoresMenu(SDL_Renderer *renderer, TTF_Font *font);

// sauvegarde un score dans le fichier
void saveScore(const char *name, int score);

// charge les meilleurs scores
void loadTopScores(char names[3][50], int scores[3]);

// initialise le système de scores
void initScoresSystem();

#endif

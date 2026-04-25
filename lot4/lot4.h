#ifndef ENTETE_H_INCLUDED
#define ENTETE_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// ================= STRUCTS =================

typedef struct
{
    SDL_Texture *map;
    SDL_Texture *map1;
    SDL_Texture *joueur;
    SDL_Rect posmap;
    SDL_Rect posjoueur;
} minimap;

typedef struct
{
    SDL_Rect posob;
} obstacle;

// ================= FUNCTIONS =================

// main entry of lot4
void run_lot4(SDL_Renderer* renderer);

void initmap(minimap *m, SDL_Renderer *renderer);
void MAJMinimap(minimap *m, SDL_Rect posPerso);
void afficherminimap(minimap m, SDL_Renderer *renderer, int currentLevel);
void freeMinimap(minimap *m);

int collisionmap(SDL_Surface *masque, SDL_Rect posPerso, int direction);
int collisionBB(SDL_Rect perso, SDL_Rect obstacle);

void animation(minimap *m, int *currentLevel);

#endif

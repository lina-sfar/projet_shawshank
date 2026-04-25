#ifndef LOT3_H
#define LOT3_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_ENNEMIS 10
#define MAX_ES 5
#define MAX_WALK_FRAMES 6
#define MAX_ATTACK_FRAMES 4

// ================= ENNEMY =================

typedef struct {
    int x, y, w, h;
    int pv, pv_max;
    int degats;
    int score_valeur;
    int type;
    int trajectoire;
    float vitesse;
    int direction;
    float startX, startY;
    float angle;
    bool actif;
    int timer_attaque;
    int etat_sante;

    SDL_Texture* walk_frames[MAX_WALK_FRAMES];
    SDL_Texture* attack_frames[MAX_ATTACK_FRAMES];
    SDL_Texture* current_texture;
    int current_frame;
    int max_frames;
    int frame_timer;
    int animation_speed;
} Ennemi;

// ================= ITEMS =================

typedef struct {
    int x, y, w, h;
    int type;
    int valeur;
    bool actif;
    SDL_Texture* texture;
} ElementSpecial;

// ================= LEVEL =================

typedef struct {
    int niveau;
    int nb_ennemis;
    Ennemi ennemis[MAX_ENNEMIS];
    int nb_es;
    ElementSpecial es[MAX_ES];
    SDL_Texture* background;
} Niveau;

// ================= FUNCTIONS =================

// MAIN ENTRY POINT OF LOT3
void run_lot3(SDL_Renderer* renderer);

void initialiser_npc(Niveau* niveau, int level, SDL_Renderer* renderer);
void afficher_npc(SDL_Renderer* renderer, Niveau* niveau);
void deplacement_aleatoire(Ennemi* e);
int collision_bb(SDL_Rect a, SDL_Rect b);
void gerer_sante(Ennemi* e, int degats);
void ia_deplacement(Ennemi* e, int joueurX, int joueurY);
void update_npc(Niveau* niveau, int joueurX, int joueurY);
void collecter_es(Niveau* niveau, SDL_Rect joueurRect, int* score, int* vies, int* pv);
void liberer_textures_npc(Niveau* niveau);
void afficher_background(SDL_Renderer* renderer, Niveau* niveau);

#endif

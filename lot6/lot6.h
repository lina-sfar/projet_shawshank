#ifndef PUZZLE_H
#define PUZZLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define PIECE_W   150
#define PIECE_H   150
#define GRID_X    40
#define GRID_Y    80
#define CHOICE_X  530
#define WINDOW_W  800
#define WINDOW_H  560

#define TIMER_SECONDS 10   /* countdown duration */

typedef struct {
    SDL_Texture *texture;
    int piece_index;
    SDL_Rect rect;        /* current drawn position */
    SDL_Rect home;        /* original resting position on the right */
    int is_correct;
    int wrong_flash;      /* ticks when wrong drop happened */
    int visible;          /* 0 = being dragged (don't draw in list) */
} Choice;

typedef struct {
    SDL_Texture *full;
    SDL_Texture *pieces[9];
    int missing;
    Choice choices[3];
    int solved;           /* 1 = correct piece placed */
    int img_index;

    /* drag state */
    int dragging;         /* index into choices[], -1 = none */
    int drag_ox, drag_oy; /* offset from piece origin to mouse */

    /* timer */
    Uint32 start_ticks;   /* when the round started */
    int time_up;          /* 1 = time ran out */
} Game;
void lot6();
int  init_game(SDL_Renderer *r, Game *g);
void new_round(SDL_Renderer *r, Game *g);
void free_game(Game *g);
void draw_game(SDL_Renderer *r, TTF_Font *font, TTF_Font *small, Game *g, int mx, int my);
void handle_mousedown(Game *g, int mx, int my);
void handle_mouseup(Game *g, int mx, int my);
void handle_mousemove(Game *g, int mx, int my);

#endif

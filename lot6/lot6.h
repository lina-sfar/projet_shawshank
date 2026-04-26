#ifndef PUZZLE_H
#define PUZZLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#define PIECE_W   150
#define PIECE_H   150
#define GRID_X    40
#define GRID_Y    80
#define CHOICE_X  530
#define WINDOW_W  800
#define WINDOW_H  560

#define TIMER_SECONDS 10

// ================= STRUCTS =================

typedef struct {
    SDL_Texture *texture;
    int piece_index;
    SDL_Rect rect;
    SDL_Rect home;
    int is_correct;
    int wrong_flash;
    int visible;
} Choice;

typedef struct {
    SDL_Texture *full;
    SDL_Texture *pieces[9];
    int missing;
    Choice choices[3];
    int solved;
    int img_index;

    int dragging;
    int drag_ox;
    int drag_oy;

    Uint32 start_ticks;
    int time_up;
} Game;

// ================= FUNCTIONS =================

void run_lot6(SDL_Renderer* renderer);

int  init_game(SDL_Renderer *renderer, Game *g);
void new_round(SDL_Renderer *renderer, Game *g);
void free_game(Game *g);

void draw_game(SDL_Renderer *renderer, TTF_Font *font, TTF_Font *small,
               Game *g, int mx, int my);

void handle_mousedown(Game *g, int mx, int my);
void handle_mouseup(Game *g, int mx, int my);
void handle_mousemove(Game *g, int mx, int my);

#endif

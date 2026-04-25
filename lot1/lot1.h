#ifndef LOT1_H
#define LOT1_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef enum {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_ATTACKING
} PlayerState;

typedef struct {
    float x, y;
    int w, h;
    int health;
    float velX, velY;
    bool onGround;
    int direction;
    PlayerState state;
    SDL_Texture* texture;
} Player;

// ONLY ONE ENTRY POINT PER LOT
void run_lot1(SDL_Renderer* renderer);

// gameplay functions
void init_player(Player* p);
void update_player(Player* p);
void render_player(SDL_Renderer* renderer, Player* p);

#endif

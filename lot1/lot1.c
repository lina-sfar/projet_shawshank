#include "lot1.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

static Player players[MAX_PLAYERS];
static int playerCount = 2;

static TTF_Font* font = NULL;
static Mix_Chunk* jumpSound = NULL;
static Mix_Chunk* attackSound = NULL;
static Mix_Chunk* hurtSound = NULL;

bool init_lot1(SDL_Renderer* renderer) {
    printf("LOT1 : Initialisation\n");

    font = TTF_OpenFont("assets/fonts/arial.ttf", 18);
    if (!font)
        font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf", 18);

    jumpSound = Mix_LoadWAV("assets/sounds/jump.wav");
    attackSound = Mix_LoadWAV("assets/sounds/attack.wav");
    hurtSound = Mix_LoadWAV("assets/sounds/hurt.wav");

    init_player(&players[0], 0, "P1", 150, 400);
    init_player(&players[1], 1, "P2", 550, 400);

    for (int i = 0; i < playerCount; i++) {
        load_player_animations(renderer, &players[i]);
    }

    return true;
}

void cleanup_lot1(void) {
    for (int i = 0; i < playerCount; i++) {

        for (int f = 0; f < 2; f++) {
            if (players[i].walk_left[f]) SDL_DestroyTexture(players[i].walk_left[f]);
            if (players[i].walk_right[f]) SDL_DestroyTexture(players[i].walk_right[f]);
        }

        for (int f = 0; f < 3; f++) {
            if (players[i].attack_left[f]) SDL_DestroyTexture(players[i].attack_left[f]);
            if (players[i].attack_right[f]) SDL_DestroyTexture(players[i].attack_right[f]);
        }

        if (players[i].jump_left) SDL_DestroyTexture(players[i].jump_left);
        if (players[i].jump_right) SDL_DestroyTexture(players[i].jump_right);
    }

    if (font) TTF_CloseFont(font);
    if (jumpSound) Mix_FreeChunk(jumpSound);
    if (attackSound) Mix_FreeChunk(attackSound);
    if (hurtSound) Mix_FreeChunk(hurtSound);

    printf("LOT1 nettoyé\n");
}

void init_player(Player* p, int id, const char* name, int startX, int startY) {
    p->playerId = id;
    snprintf(p->name, sizeof(p->name), "%s", name);

    p->x = startX;
    p->y = startY;

    p->w = PLAYER_WIDTH;
    p->h = PLAYER_HEIGHT;

    p->health = 100;
    p->maxHealth = 100;
    p->lives = 3;
    p->score = 0;

    p->velX = 0;
    p->velY = 0;

    p->onGround = false;
    p->direction = 1;

    p->state = STATE_IDLE;

    p->attackCooldown = 0;
    p->isInvincible = false;
    p->invincibilityFrames = 0;

    p->anim_frame = 0;
    p->anim_timer = 0;
    p->attack_frame = 0;
    p->attack_timer = 0;

    for (int i = 0; i < 2; i++) {
        p->walk_left[i] = NULL;
        p->walk_right[i] = NULL;
    }

    for (int i = 0; i < 3; i++) {
        p->attack_left[i] = NULL;
        p->attack_right[i] = NULL;
    }

    p->jump_left = NULL;
    p->jump_right = NULL;

    if (id == 0) {
        p->keyLeft = SDLK_LEFT;
        p->keyRight = SDLK_RIGHT;
        p->keyJump = SDLK_UP;
        p->keyAttack = SDLK_SPACE;
        p->keyRun = SDLK_RSHIFT;
    } else {
        p->keyLeft = SDLK_q;
        p->keyRight = SDLK_d;
        p->keyJump = SDLK_z;
        p->keyAttack = SDLK_a;
        p->keyRun = SDLK_LSHIFT;
    }
}

void load_player_animations(SDL_Renderer* renderer, Player* p) {
    p->walk_left[0] = IMG_LoadTexture(renderer, "assets/left1.png");
    p->walk_left[1] = IMG_LoadTexture(renderer, "assets/left2.png");

    p->walk_right[0] = IMG_LoadTexture(renderer, "assets/right1.png");
    p->walk_right[1] = IMG_LoadTexture(renderer, "assets/right2.png");

    p->jump_left = IMG_LoadTexture(renderer, "assets/jumpleft.png");
    p->jump_right = IMG_LoadTexture(renderer, "assets/jumpright.png");

    p->attack_left[0] = IMG_LoadTexture(renderer, "assets/al1.png");
    p->attack_left[1] = IMG_LoadTexture(renderer, "assets/al2.png");
    p->attack_left[2] = IMG_LoadTexture(renderer, "assets/al3.png");

    p->attack_right[0] = IMG_LoadTexture(renderer, "assets/ar1.png");
    p->attack_right[1] = IMG_LoadTexture(renderer, "assets/ar2.png");
    p->attack_right[2] = IMG_LoadTexture(renderer, "assets/ar3.png");
}

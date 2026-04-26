#ifndef LOT1_H
#define LOT1_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAX_ANIM_FRAMES 3

typedef enum {
    STATE_IDLE,
    STATE_WALKING,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_ATTACKING
} PlayerState;

typedef struct {
    // identity
    int playerId;
    char name[50];

    // position / size
    float x, y;
    int w, h;

    // physics
    float velX, velY;
    bool onGround;
    int direction;

    // gameplay stats
    int health;
    int maxHealth;
    int lives;
    int score;

    // state
    PlayerState state;

    // combat
    int attackCooldown;

    // invincibility
    bool isInvincible;
    int invincibilityFrames;

    // animation
    int anim_frame;
    int anim_timer;
    int attack_frame;
    int attack_timer;

    // textures
    SDL_Texture* walk_left[2];
    SDL_Texture* walk_right[2];

    SDL_Texture* attack_left[MAX_ANIM_FRAMES];
    SDL_Texture* attack_right[MAX_ANIM_FRAMES];

    SDL_Texture* jump_left;
    SDL_Texture* jump_right;

    SDL_Texture* idle_texture;

    // controls
    int keyLeft;
    int keyRight;
    int keyJump;
    int keyAttack;
    int keyRun;

} Player;

// entry point
void run_lot1(SDL_Renderer* renderer);

// core functions
void init_player(Player* p, int id, const char* name, int startX, int startY);
void update_player(Player* p);
void render_player(SDL_Renderer* renderer, Player* p);

// optional gameplay helpers (used in your .c)
void load_player_animations(SDL_Renderer* renderer, Player* p);
void update_player_animation(Player* p);
void add_score(Player* p, int points);
void take_damage(Player* p, int dmg);

#endif

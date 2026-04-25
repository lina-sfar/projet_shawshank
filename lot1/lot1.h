#ifndef LOT1_H
#define LOT1_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define MAX_PLAYERS   2
#define GRAVITY       0.5
#define JUMP_FORCE    -10
#define WALK_SPEED    3
#define RUN_SPEED     6

// Taille des personnages (agrandie)
#define PLAYER_WIDTH  70
#define PLAYER_HEIGHT 100

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
    int score;
    int lives;
    int health;
    int maxHealth;
    float velX, velY;
    bool onGround;
    int direction;          // 1 = droite, -1 = gauche
    PlayerState state;
    int playerId;
    char name[20];
    int attackCooldown;
    bool isInvincible;
    int invincibilityFrames;

    // Textures d'animation
    SDL_Texture* walk_left[2];
    SDL_Texture* walk_right[2];
    SDL_Texture* jump_left;
    SDL_Texture* jump_right;
    SDL_Texture* attack_left[3];
    SDL_Texture* attack_right[3];

    // Animation
    int anim_frame;
    int anim_timer;
    int attack_frame;
    int attack_timer;

    // Touches personnalisées
    SDL_Keycode keyLeft, keyRight, keyJump, keyAttack, keyRun;
} Player;

// Variables globales pour le game over
extern bool game_over;
extern char winner_name[50];

// Fonctions principales
bool init_lot1(SDL_Renderer* renderer);
void cleanup_lot1(void);
void update_lot1(void);
void render_lot1(SDL_Renderer* renderer);
void handle_events_lot1(SDL_Event* event, bool* running);
void run_lot1(SDL_Renderer* renderer);

// Gestion joueur
void init_player(Player* p, int id, const char* name, int startX, int startY);
void load_player_animations(SDL_Renderer* renderer, Player* p);
void render_player(SDL_Renderer* renderer, Player* p);
void update_player_animation(Player* p);
void add_score(Player* p, int points);
void take_damage(Player* p, int damage);

// Mouvements
void move_player(Player* p, float dx);
void jump_player(Player* p);
void attack_player(Player* p);
void apply_physics(Player* p);
void check_player_collision(Player* p1, Player* p2);

#endif

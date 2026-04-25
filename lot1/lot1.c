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
bool game_over = false;
char winner_name[50] = "";
bool init_lot1(SDL_Renderer* renderer) {
    printf("LOT1 : Initialisation\n");

    font = TTF_OpenFont("assets/fonts/arial.ttf", 18);
    if (!font) font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf", 18);

    jumpSound = Mix_LoadWAV("assets/sounds/jump.wav");
    attackSound = Mix_LoadWAV("assets/sounds/attack.wav");
    hurtSound = Mix_LoadWAV("assets/sounds/hurt.wav");

    init_player(&players[0], 0, "P1", 150, 400);
    init_player(&players[1], 1, "P2", 550, 400);

    for (int i = 0; i < playerCount; i++)
        load_player_animations(renderer, &players[i]);

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
    p->score = 0;
    p->lives = 3;
    p->health = 100;
    p->maxHealth = 100;
    p->velX = p->velY = 0;
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
    // Marche
    p->walk_left[0] = IMG_LoadTexture(renderer, "assets/left1.png");
    p->walk_left[1] = IMG_LoadTexture(renderer, "assets/left2.png");
    p->walk_right[0] = IMG_LoadTexture(renderer, "assets/right1.png");
    p->walk_right[1] = IMG_LoadTexture(renderer, "assets/right2.png");

    // Saut (minuscules)
    p->jump_left = IMG_LoadTexture(renderer, "assets/jumpleft.png");
    p->jump_right = IMG_LoadTexture(renderer, "assets/jumpright.png");

    // Attaque
    p->attack_left[0] = IMG_LoadTexture(renderer, "assets/al1.png");
    p->attack_left[1] = IMG_LoadTexture(renderer, "assets/al2.png");
    p->attack_left[2] = IMG_LoadTexture(renderer, "assets/al3.png");
    p->attack_right[0] = IMG_LoadTexture(renderer, "assets/ar1.png");
    p->attack_right[1] = IMG_LoadTexture(renderer, "assets/ar2.png");
    p->attack_right[2] = IMG_LoadTexture(renderer, "assets/ar3.png");
}

void update_player_animation(Player* p) {
    if (p->state == STATE_ATTACKING) {
        p->attack_timer++;
        if (p->attack_timer >= 5) {
            p->attack_timer = 0;
            p->attack_frame++;
            if (p->attack_frame >= 3) {
                p->attack_frame = 0;
                p->state = STATE_IDLE;
            }
        }
    } else if (p->state == STATE_WALKING || p->state == STATE_RUNNING) {
        p->anim_timer++;
        if (p->anim_timer >= 8) {
            p->anim_timer = 0;
            p->anim_frame = (p->anim_frame + 1) % 2;
        }
    } else {
        p->anim_frame = 0;
        p->anim_timer = 0;
        if (p->state != STATE_ATTACKING)
            p->attack_frame = 0;
    }
}

void add_score(Player* p, int points) {
    p->score += points;
    printf("%s : score +%d (total %d)\n", p->name, points, p->score);
}

void take_damage(Player* p, int damage) {
    if (p->isInvincible) return;
    p->health -= damage;
    p->invincibilityFrames = 60;
    p->isInvincible = true;
    if (hurtSound) Mix_PlayChannel(-1, hurtSound, 0);
    if (p->health <= 0) {
        p->lives--;
        p->health = p->maxHealth;
        if (p->lives < 0) p->lives = 0;
        printf("%s : perdu une vie, reste %d vies\n", p->name, p->lives);
        // Si le joueur meurt, on le replace en position de départ
        if (p->lives > 0) {
            p->x = (p->playerId == 0) ? 150 : 550;
            p->y = 400;
            p->velX = 0;
            p->velY = 0;
            p->state = STATE_IDLE;
        }
    }
}

void move_player(Player* p, float dx) {
    if (p->lives <= 0) return; // mort, ne peut pas bouger
    p->velX = dx;
    if (dx != 0) {
        p->direction = (dx > 0) ? 1 : -1;
        if (p->onGround && p->state != STATE_ATTACKING && p->state != STATE_JUMPING) {
            p->state = (fabs(dx) > WALK_SPEED) ? STATE_RUNNING : STATE_WALKING;
        }
    } else {
        if (p->onGround && p->state != STATE_ATTACKING && p->state != STATE_JUMPING)
            p->state = STATE_IDLE;
    }
}

void jump_player(Player* p) {
    if (p->lives <= 0) return;
    if (p->onGround && p->state != STATE_ATTACKING) {
        p->velY = JUMP_FORCE;
        p->onGround = false;
        p->state = STATE_JUMPING;
        if (jumpSound) Mix_PlayChannel(-1, jumpSound, 0);
    }
}

void attack_player(Player* p) {
    if (p->lives <= 0) return;
    if (p->attackCooldown <= 0 && p->state != STATE_JUMPING) {
        p->attackCooldown = 30;
        p->state = STATE_ATTACKING;
        p->attack_frame = 0;
        p->attack_timer = 0;
        if (attackSound) Mix_PlayChannel(-1, attackSound, 0);
        printf("%s : Attaque !\n", p->name);
    }
}

void apply_physics(Player* p) {
    if (p->lives <= 0) return; // mort, pas de physique
    if (!p->onGround) p->velY += GRAVITY;
    p->x += p->velX;
    p->y += p->velY;

    // Sol
    if (p->y > SCREEN_HEIGHT - 150) {
        p->y = SCREEN_HEIGHT - 150;
        p->velY = 0;
        p->onGround = true;
        if (p->state == STATE_JUMPING && p->velX == 0)
            p->state = STATE_IDLE;
    } else {
        p->onGround = false;
    }

    // Bords
    if (p->x < 0) p->x = 0;
    if (p->x > SCREEN_WIDTH - p->w) p->x = SCREEN_WIDTH - p->w;

    // Invincibilité
    if (p->invincibilityFrames > 0) {
        p->invincibilityFrames--;
        p->isInvincible = (p->invincibilityFrames > 0);
    }
    if (p->attackCooldown > 0) p->attackCooldown--;

    update_player_animation(p);
}

void check_player_collision(Player* p1, Player* p2) {
    SDL_Rect r1 = {(int)p1->x, (int)p1->y, p1->w, p1->h};
    SDL_Rect r2 = {(int)p2->x, (int)p2->y, p2->w, p2->h};
    if (SDL_HasIntersection(&r1, &r2)) {
        // P1 attaque et touche P2
        if (p1->state == STATE_ATTACKING && p1->attackCooldown > 25 && p2->lives > 0) {
            take_damage(p2, 20);
            add_score(p1, 10);
        }
        // P2 attaque et touche P1
        if (p2->state == STATE_ATTACKING && p2->attackCooldown > 25 && p1->lives > 0) {
            take_damage(p1, 20);
            add_score(p2, 10);
        }
    }
}

void render_player(SDL_Renderer* renderer, Player* p) {
    if (p->isInvincible && (SDL_GetTicks() / 100) % 2 == 0) return;

    SDL_Rect rect = {(int)p->x, (int)p->y, p->w, p->h};
    SDL_Texture* tex = NULL;

    if (p->state == STATE_ATTACKING) {
        if (p->direction == 1)
            tex = p->attack_right[p->attack_frame];
        else
            tex = p->attack_left[p->attack_frame];
    } else if (p->state == STATE_JUMPING) {
        if (p->direction == 1)
            tex = p->jump_right;
        else
            tex = p->jump_left;
    } else if (p->state == STATE_WALKING || p->state == STATE_RUNNING) {
        if (p->direction == 1)
            tex = p->walk_right[p->anim_frame];
        else
            tex = p->walk_left[p->anim_frame];
    } else { // IDLE
        if (p->direction == 1)
            tex = p->walk_right[0];
        else
            tex = p->walk_left[0];
    }

    if (tex) {
        SDL_RenderCopy(renderer, tex, NULL, &rect);
    } else {
        // Fallback : rectangle orange pour le saut si texture manquante
        if (p->state == STATE_JUMPING) {
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
        }
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderDrawRect(renderer, &rect);
    }

    // Barre de vie
    SDL_Rect healthBg = {(int)p->x, (int)p->y - 10, p->w, 5};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, &healthBg);
    SDL_Rect health = {(int)p->x, (int)p->y - 10,
                       (int)((float)p->health / p->maxHealth * p->w), 5};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &health);
}

void render_lot1(SDL_Renderer* renderer) {
    // Fond
    SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
    SDL_RenderClear(renderer);
    SDL_Rect ground = {0, SCREEN_HEIGHT - 100, SCREEN_WIDTH, 100};
    SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
    SDL_RenderFillRect(renderer, &ground);

    // Scores et vies en haut
    if (font) {
        char info[200];
        sprintf(info, "P1: %d pts  %d vies     P2: %d pts  %d vies",
                players[0].score, players[0].lives,
                players[1].score, players[1].lives);
        SDL_Surface* surf = TTF_RenderText_Solid(font, info, (SDL_Color){255,255,255,255});
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dest = {20, 20, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, NULL, &dest);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }

    // Game over message
    if (players[0].lives <= 0) {
        const char* msg = "Game Over ! Joueur 2 gagne !";
        SDL_Surface* surf = TTF_RenderText_Solid(font, msg, (SDL_Color){255,0,0,255});
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dest = {SCREEN_WIDTH/2 - surf->w/2, SCREEN_HEIGHT/2 - 50, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, NULL, &dest);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    } else if (players[1].lives <= 0) {
        const char* msg = "Game Over ! Joueur 1 gagne !";
        SDL_Surface* surf = TTF_RenderText_Solid(font, msg, (SDL_Color){255,0,0,255});
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dest = {SCREEN_WIDTH/2 - surf->w/2, SCREEN_HEIGHT/2 - 50, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, NULL, &dest);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }

    // Joueurs
    for (int i = 0; i < playerCount; i++)
        render_player(renderer, &players[i]);
}

void update_lot1(void) {
    for (int i = 0; i < playerCount; i++) {
        if (players[i].lives > 0)
            apply_physics(&players[i]);
    }
    check_player_collision(&players[0], &players[1]);
}

void handle_events_lot1(SDL_Event* event, bool* running) {
    if (event->type == SDL_QUIT) {
        *running = false;
        return;
    }

    for (int i = 0; i < playerCount; i++) {
        Player* p = &players[i];
        if (p->lives <= 0) continue; // joueur mort, ignore ses touches
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == p->keyRight)
                move_player(p, WALK_SPEED);
            if (event->key.keysym.sym == p->keyLeft)
                move_player(p, -WALK_SPEED);
            if (event->key.keysym.sym == p->keyJump)
                jump_player(p);
            if (event->key.keysym.sym == p->keyAttack)
                attack_player(p);
            if (event->key.keysym.sym == p->keyRun) {
                float speed = (p->direction == 1) ? RUN_SPEED : -RUN_SPEED;
                move_player(p, speed);
            }
            if (event->key.keysym.sym == SDLK_ESCAPE)
                *running = false;
        }
        if (event->type == SDL_KEYUP) {
            if (event->key.keysym.sym == p->keyRight || event->key.keysym.sym == p->keyLeft)
                move_player(p, 0);
            if (event->key.keysym.sym == p->keyRun && p->state != STATE_ATTACKING && p->state != STATE_JUMPING) {
                if (p->velX != 0)
                    move_player(p, (p->velX > 0) ? WALK_SPEED : -WALK_SPEED);
            }
        }
    }
}

void run_lot1(SDL_Renderer* renderer) {
    if (!init_lot1(renderer)) return;
    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) handle_events_lot1(&e, &running);
        update_lot1();
        render_lot1(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    cleanup_lot1();
}

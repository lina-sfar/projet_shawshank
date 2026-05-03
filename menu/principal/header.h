#ifndef HEADER_H
#define HEADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

#define NB_BUTTONS 6

typedef struct {
    SDL_Texture* normal;
    SDL_Texture* hover;
    SDL_Rect rect;
    int isHovered;
} Button;

typedef struct {
    SDL_Texture* background;
    Button buttons[NB_BUTTONS + 1];
    Mix_Chunk* clickSound;
    Mix_Music* bgMusic;
} Menu;

/* BUTTON */
int initButton(Button* btn, SDL_Renderer* renderer,
               const char* normalImg,
               const char* hoverImg,
               int x, int y,
               int w, int h);

void handleButton(Button* btn, SDL_Event* e);
int isClicked(Button* btn, SDL_Event* e);
void renderButton(Button* btn, SDL_Renderer* renderer);
void destroyButton(Button* btn);

/* MENU */
int initMenu(Menu* menu, SDL_Renderer* renderer);
int handleMenu(Menu* menu, SDL_Event* e);
void renderMenu(Menu* menu, SDL_Renderer* renderer);
void destroyMenu(Menu* menu);

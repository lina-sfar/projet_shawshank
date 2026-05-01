#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
    SDL_Texture *tex;
    SDL_Texture *tex_hover;
    SDL_Rect rect;
    int is_hover;
} Button;

typedef struct {

    SDL_Texture *background;
    SDL_Texture *borders;

    Button volume;      // does NOT change
    Button increase;
    Button decrease;

    Button display;     // does NOT change
    Button normal;
    Button fullscreen;

    Button options;
    Button returnBtn;

    Mix_Music *music;
    Mix_Chunk *click;

    int volume_level;

} Menu;

int menu_init(SDL_Renderer *ren, Menu *menu);
void menu_update(Menu *menu, SDL_Event *e, SDL_Window *win);
void menu_render(SDL_Renderer *ren, Menu *menu);
void menu_destroy(Menu *menu);

#endif

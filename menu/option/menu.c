#include "menu.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

static SDL_Texture* loadTexture(SDL_Renderer *ren, const char *path) {
    SDL_Texture *tex = IMG_LoadTexture(ren, path);
    if (!tex)
        printf("Failed to load %s : %s\n", path, IMG_GetError());
    return tex;
}

int menu_init(SDL_Renderer *ren, Menu *menu) {

    menu->background = loadTexture(ren, "background.jpeg");
    menu->borders = loadTexture(ren, "borders.jpeg");

    int centerX = 1280 / 2;
    int firstRowY = 250;
    int secondRowY = 370;

    int buttonW = 180;
    int buttonH = 60;
    int spacing = 90;

    int totalWidth = (buttonW * 3) + (spacing * 2);
    int startX = centerX - totalWidth / 2;

    // ---- FIRST ROW ----
    menu->volume.tex = loadTexture(ren, "volume.jpeg");
    menu->volume.tex_hover = menu->volume.tex; // DOES NOT CHANGE
    menu->volume.rect = (SDL_Rect){startX, firstRowY, buttonW, buttonH};

    menu->increase.tex = loadTexture(ren, "increase.jpeg");
    menu->increase.tex_hover = loadTexture(ren, "increase2.jpeg");
    menu->increase.rect = (SDL_Rect){startX + buttonW + spacing, firstRowY, buttonW, buttonH};

    menu->decrease.tex = loadTexture(ren, "decrease.jpeg");
    menu->decrease.tex_hover = loadTexture(ren, "decrease2.jpeg");
    menu->decrease.rect = (SDL_Rect){startX + (buttonW + spacing) * 2, firstRowY, buttonW, buttonH};

    // ---- SECOND ROW ----
    menu->display.tex = loadTexture(ren, "display_mode.jpeg");
    menu->display.tex_hover = menu->display.tex; // DOES NOT CHANGE
    menu->display.rect = (SDL_Rect){startX, secondRowY, buttonW, buttonH};

    menu->normal.tex = loadTexture(ren, "normal.jpeg");
    menu->normal.tex_hover = loadTexture(ren, "normal2.jpeg");
    menu->normal.rect = (SDL_Rect){startX + buttonW + spacing, secondRowY, buttonW, buttonH};

    menu->fullscreen.tex = loadTexture(ren, "full_screen.jpeg");
    menu->fullscreen.tex_hover = loadTexture(ren, "full_screen2.jpeg");
    menu->fullscreen.rect = (SDL_Rect){startX + (buttonW + spacing) * 2, secondRowY, buttonW, buttonH};

    // ---- TITLE ----
    menu->options.tex = loadTexture(ren, "options.jpeg");
    menu->options.tex_hover = menu->options.tex;
    menu->options.rect = (SDL_Rect){centerX - 200, 80, 400, 80};

    // ---- RETURN bottom right ----
    menu->returnBtn.tex = loadTexture(ren, "return.jpeg");
    menu->returnBtn.tex_hover = loadTexture(ren, "return2.jpeg");
    menu->returnBtn.rect = (SDL_Rect){1050, 600, 180, 60};

    // ---- AUDIO ----
    menu->music = Mix_LoadMUS("audio.mp3");
    menu->click = Mix_LoadWAV("click.mp3");

    menu->volume_level = MIX_MAX_VOLUME / 2;
    Mix_VolumeMusic(menu->volume_level);
    Mix_PlayMusic(menu->music, -1);

    return 0;
}

void menu_update(Menu *menu, SDL_Event *e, SDL_Window *win) {

    int mx, my;
    SDL_GetMouseState(&mx, &my);

    Button *hoverButtons[] = {
        &menu->increase,
        &menu->decrease,
        &menu->normal,
        &menu->fullscreen,
        &menu->returnBtn
    };

    for (int i = 0; i < 5; i++) {
        hoverButtons[i]->is_hover =
            mx >= hoverButtons[i]->rect.x &&
            mx <= hoverButtons[i]->rect.x + hoverButtons[i]->rect.w &&
            my >= hoverButtons[i]->rect.y &&
            my <= hoverButtons[i]->rect.y + hoverButtons[i]->rect.h;
    }

    if (e->type == SDL_MOUSEBUTTONDOWN) {

        if (menu->increase.is_hover) {
            Mix_PlayChannel(-1, menu->click, 0);
            menu->volume_level += 8;
            if (menu->volume_level > MIX_MAX_VOLUME)
                menu->volume_level = MIX_MAX_VOLUME;
            Mix_VolumeMusic(menu->volume_level);
        }

        if (menu->decrease.is_hover) {
            Mix_PlayChannel(-1, menu->click, 0);
            menu->volume_level -= 8;
            if (menu->volume_level < 0)
                menu->volume_level = 0;
            Mix_VolumeMusic(menu->volume_level);
        }

        if (menu->fullscreen.is_hover) {
            Mix_PlayChannel(-1, menu->click, 0);
            SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }

        if (menu->normal.is_hover) {
            Mix_PlayChannel(-1, menu->click, 0);
            SDL_SetWindowFullscreen(win, 0);
        }

        if (menu->returnBtn.is_hover) {
            Mix_PlayChannel(-1, menu->click, 0);
            SDL_Event quit;
            quit.type = SDL_QUIT;
            SDL_PushEvent(&quit);
        }
    }
}

void menu_render(SDL_Renderer *ren, Menu *menu) {

    SDL_RenderCopy(ren, menu->background, NULL, NULL);
    SDL_RenderCopy(ren, menu->borders, NULL, NULL);

    SDL_RenderCopy(ren, menu->options.tex, NULL, &menu->options.rect);
    SDL_RenderCopy(ren, menu->display.tex, NULL, &menu->display.rect);
    SDL_RenderCopy(ren, menu->volume.tex, NULL, &menu->volume.rect);

    Button *buttons[] = {
        &menu->increase,
        &menu->decrease,
        &menu->normal,
        &menu->fullscreen,
        &menu->returnBtn
    };

    for (int i = 0; i < 5; i++) {
        SDL_Texture *tex = buttons[i]->is_hover ?
            buttons[i]->tex_hover : buttons[i]->tex;
        SDL_RenderCopy(ren, tex, NULL, &buttons[i]->rect);
    }
}

void menu_destroy(Menu *menu) {
    Mix_FreeMusic(menu->music);
    Mix_FreeChunk(menu->click);
}

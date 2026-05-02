#include "header.h"

/* ================= BUTTON ================= */

int initButton(Button* btn, SDL_Renderer* renderer,
               const char* normalImg,
               const char* hoverImg,
               int x, int y,
               int w, int h)
{
    SDL_Surface* surface;

    surface = IMG_Load(normalImg);
    if (!surface) {
        printf("Error loading: %s\n", normalImg);
        return 0;
    }
    btn->normal = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load(hoverImg);
    if (!surface) {
        printf("Error loading: %s\n", hoverImg);
        return 0;
    }
    btn->hover = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    btn->rect = (SDL_Rect){x, y, w, h};
    btn->isHovered = 0;

    return 1;
}

void handleButton(Button* btn, SDL_Event* e)
{
    if (e->type == SDL_MOUSEMOTION)
    {
        int x = e->motion.x;
        int y = e->motion.y;

        btn->isHovered =
            (x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
             y >= btn->rect.y && y <= btn->rect.y + btn->rect.h);
    }
}

int isClicked(Button* btn, SDL_Event* e)
{
    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_LEFT)
    {
        int x = e->button.x;
        int y = e->button.y;

        return (x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
                y >= btn->rect.y && y <= btn->rect.y + btn->rect.h);
    }
    return 0;
}

void renderButton(Button* btn, SDL_Renderer* renderer)
{
    SDL_RenderCopy(renderer,
                   btn->isHovered ? btn->hover : btn->normal,
                   NULL,
                   &btn->rect);
}

void destroyButton(Button* btn)
{
    if (btn->normal) SDL_DestroyTexture(btn->normal);
    if (btn->hover) SDL_DestroyTexture(btn->hover);
}

/* ================= MENU ================= */

int initMenu(Menu* menu, SDL_Renderer* renderer)
{
    SDL_Surface* surface;

    surface = IMG_Load("full_menu0.png");
    if (!surface) {
        printf("Background load error\n");
        return 0;
    }
    menu->background = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    menu->clickSound = Mix_LoadWAV("sound.mp3");
    menu->bgMusic = Mix_LoadMUS("nyan_cat.mp3");

    if (menu->bgMusic)
        Mix_PlayMusic(menu->bgMusic, -1);

    int buttonW = SCREEN_WIDTH * 0.12;
    int buttonH = SCREEN_HEIGHT * 0.09;

    int marginX = SCREEN_WIDTH * 0.1;
    int marginY = SCREEN_HEIGHT * 0.15;

    int colX[3] = {
        marginX,
        (SCREEN_WIDTH - buttonW) / 2,
        SCREEN_WIDTH - marginX - buttonW
    };

    int rowY[3] = {
        marginY,
        SCREEN_HEIGHT * 0.4,
        SCREEN_HEIGHT * 0.65
    };

    initButton(&menu->buttons[0], renderer, "save.png", "save2.png", colX[0], rowY[0], buttonW, buttonH);
    initButton(&menu->buttons[1], renderer, "start.png", "start2.png", colX[1], rowY[0], buttonW, buttonH);
    initButton(&menu->buttons[2], renderer, "best_score.png", "best_score2.png", colX[2], rowY[0], buttonW, buttonH);

    initButton(&menu->buttons[3], renderer, "options.png", "options2.png", colX[0], rowY[1], buttonW, buttonH);
    initButton(&menu->buttons[4], renderer, "quit.png", "quit2.png", colX[1], rowY[1], buttonW, buttonH);
    initButton(&menu->buttons[5], renderer, "player.png", "player2.png", colX[2], rowY[1], buttonW, buttonH);

    initButton(&menu->buttons[6], renderer, "quiz.png", "quiz2.png",
               colX[1], rowY[2], buttonW, buttonH);

    return 1;
}

/* ================= FIXED MENU HANDLER ================= */

int handleMenu(Menu* menu, SDL_Event* e)
{
    for (int i = 0; i < NB_BUTTONS + 1; i++)
    {
        handleButton(&menu->buttons[i], e);

        if (isClicked(&menu->buttons[i], e))
        {
            if (menu->clickSound)
                Mix_PlayChannel(-1, menu->clickSound, 0);

            if (i == 4)
                return -1;  // QUIT

            if (i == 3)
                return 1;   // OPTIONS

            if (i == 1 || i == 6)
                return 2;   // START / QUIZ

            if (i == 2)
                return 3;   // BEST SCORE

            return 0;       // MAIN MENU
        }
    }

    return 0;
}

/* ================= RENDER ================= */

void renderMenu(Menu* menu, SDL_Renderer* renderer)
{
    SDL_Rect full = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_RenderCopy(renderer, menu->background, NULL, &full);

    for (int i = 0; i < NB_BUTTONS + 1; i++)
        renderButton(&menu->buttons[i], renderer);
}

/* ================= CLEAN ================= */

void destroyMenu(Menu* menu)
{
    SDL_DestroyTexture(menu->background);

    for (int i = 0; i < NB_BUTTONS + 1; i++)
        destroyButton(&menu->buttons[i]);

    if (menu->clickSound)
        Mix_FreeChunk(menu->clickSound);

    if (menu->bgMusic)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(menu->bgMusic);
    }
}

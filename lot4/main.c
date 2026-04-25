#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "minimap.h"

static SDL_Texture *load_texture(SDL_Renderer *renderer, const char *filename)
{
    SDL_Surface *surface = IMG_Load(filename);
    SDL_Texture *texture;

    if (surface == NULL)
    {
        printf("Erreur chargement %s : %s\n", filename, IMG_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
    {
        printf("Erreur texture %s : %s\n", filename, SDL_GetError());
    }

    SDL_FreeSurface(surface);
    return texture;
}

void lot4(int argc, char *argv[])
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    SDL_Texture *bg;
    SDL_Texture *bg1;
    SDL_Texture *perso;
    SDL_Texture *rock;
    SDL_Texture *bigrock;
    SDL_Surface *masque;
    SDL_Rect rock_positions[4];
    SDL_Rect rock_rects[4];
    SDL_Rect posPerso;
    minimap m;
    int currentLevel;
    int game;
    int direction;

    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Erreur SDL_Init : %s\n", SDL_GetError());
        return 1;
    }

    if ((IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG)) == 0)
    {
        printf("Erreur IMG_Init : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    window = SDL_CreateWindow("Mini map SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, 0);
    if (window == NULL)
    {
        printf("Erreur fenetre : %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        renderer = SDL_CreateRenderer(window, -1, 0);
    }
    if (renderer == NULL)
    {
        printf("Erreur renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    bg = load_texture(renderer, "bg7.png");
    bg1 = load_texture(renderer, "bg8.png");
    perso = load_texture(renderer, "perso.png");
    rock = load_texture(renderer, "rocks.png");
    bigrock = load_texture(renderer, "rocks2.png");
    masque = IMG_Load("mask6.png");

    if (bg == NULL || bg1 == NULL || perso == NULL || rock == NULL || bigrock == NULL || masque == NULL)
    {
        printf("Erreur chargement ressources\n");
        if (masque)
            SDL_FreeSurface(masque);
        if (bg)
            SDL_DestroyTexture(bg);
        if (bg1)
            SDL_DestroyTexture(bg1);
        if (perso)
            SDL_DestroyTexture(perso);
        if (rock)
            SDL_DestroyTexture(rock);
        if (bigrock)
            SDL_DestroyTexture(bigrock);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    rock_positions[0].x = 100; rock_positions[0].y = 500; rock_positions[0].w = 60; rock_positions[0].h = 60;
    rock_positions[1].x = 350; rock_positions[1].y = 500; rock_positions[1].w = 90; rock_positions[1].h = 90;
    rock_positions[2].x = 520; rock_positions[2].y = 500; rock_positions[2].w = 60; rock_positions[2].h = 60;
    rock_positions[3].x = 750; rock_positions[3].y = 530; rock_positions[3].w = 90; rock_positions[3].h = 90;

    rock_rects[0] = rock_positions[0];
    rock_rects[1] = rock_positions[1];
    rock_rects[2] = rock_positions[2];
    rock_rects[3] = rock_positions[3];

    posPerso.x = 200;
    posPerso.y = 150;
    posPerso.w = 48;
    posPerso.h = 48;

    currentLevel = 1;
    game = 1;
    direction = -1;

    initmap(&m, renderer);

    while (game)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                game = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_e:
                case SDLK_ESCAPE:
                    game = 0;
                    break;
                case SDLK_RIGHT:
                    direction = 1;
                    break;
                case SDLK_LEFT:
                    direction = 0;
                    break;
                case SDLK_UP:
                    direction = 2;
                    break;
                case SDLK_DOWN:
                    direction = 3;
                    break;
                case SDLK_SPACE:
                    if (currentLevel == 1)
                        currentLevel = 2;
                    else
                        currentLevel = 1;
                    break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_RIGHT:
                case SDLK_LEFT:
                case SDLK_UP:
                case SDLK_DOWN:
                    direction = -1;
                    break;
                }
            }
        }

        if (direction != -1 && collisionmap(masque, posPerso, direction) == 0)
        {
            SDL_Rect nextPos = posPerso;
            if (direction == 1)
                nextPos.x += 10;
            if (direction == 0)
                nextPos.x -= 10;
            if (direction == 2)
                nextPos.y -= 10;
            if (direction == 3)
                nextPos.y += 10;

            if (!collisionBB(nextPos, rock_rects[0]) &&
                !collisionBB(nextPos, rock_rects[1]) &&
                !collisionBB(nextPos, rock_rects[2]) &&
                !collisionBB(nextPos, rock_rects[3]))
            {
                posPerso = nextPos;
                animation(&m, &currentLevel);
            }
        }

        MAJMinimap(&m, posPerso);

        SDL_RenderClear(renderer);
        if (currentLevel == 1)
            SDL_RenderCopy(renderer, bg, NULL, NULL);
        else
            SDL_RenderCopy(renderer, bg1, NULL, NULL);

        SDL_RenderCopy(renderer, rock, NULL, &rock_positions[0]);
        SDL_RenderCopy(renderer, bigrock, NULL, &rock_positions[1]);
        SDL_RenderCopy(renderer, rock, NULL, &rock_positions[2]);
        SDL_RenderCopy(renderer, bigrock, NULL, &rock_positions[3]);
        SDL_RenderCopy(renderer, perso, NULL, &posPerso);
        afficherminimap(m, renderer, currentLevel);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    freeMinimap(&m);
    SDL_FreeSurface(masque);
    SDL_DestroyTexture(bg);
    SDL_DestroyTexture(bg1);
    SDL_DestroyTexture(perso);
    SDL_DestroyTexture(rock);
    SDL_DestroyTexture(bigrock);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

#include <stdio.h>
#include <string.h>
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

void initmap(minimap *m, SDL_Renderer *renderer)
{
    m->map = load_texture(renderer, "mini.png");
    m->map1 = load_texture(renderer, "mini1.png");
    m->joueur = load_texture(renderer, "point.png");
    m->posmap.x = 700;
    m->posmap.y = 20;
    m->posmap.w = 180;
    m->posmap.h = 120;
    m->posjoueur.x = 700;
    m->posjoueur.y = 20;
    m->posjoueur.w = 10;
    m->posjoueur.h = 10;
}

void MAJMinimap(minimap *m, SDL_Rect posPerso)
{
    m->posjoueur.x = m->posmap.x + (int)(posPerso.x * 0.2f);
    m->posjoueur.y = m->posmap.y + (int)(posPerso.y * 0.2f);
}

void afficherminimap(minimap m, SDL_Renderer *renderer, int currentLevel)
{
    if (currentLevel == 1)
    {
        SDL_RenderCopy(renderer, m.map, NULL, &m.posmap);
    }
    else
    {
        SDL_RenderCopy(renderer, m.map1, NULL, &m.posmap);
    }

    SDL_RenderCopy(renderer, m.joueur, NULL, &m.posjoueur);
}

void freeMinimap(minimap *m)
{
    if (m->map)
        SDL_DestroyTexture(m->map);
    if (m->map1)
        SDL_DestroyTexture(m->map1);
    if (m->joueur)
        SDL_DestroyTexture(m->joueur);
}

static SDL_Color GetPixel(SDL_Surface *pSurface, int x, int y)
{
    SDL_Color color;
    Uint32 col = 0;
    char *pPosition;

    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x >= pSurface->w)
        x = pSurface->w - 1;
    if (y >= pSurface->h)
        y = pSurface->h - 1;

    pPosition = (char *)pSurface->pixels;
    pPosition += (pSurface->pitch * y);
    pPosition += (pSurface->format->BytesPerPixel * x);
    memcpy(&col, pPosition, pSurface->format->BytesPerPixel);
    SDL_GetRGB(col, pSurface->format, &color.r, &color.g, &color.b);
    return color;
}

int collisionmap(SDL_Surface *masque, SDL_Rect posPerso, int direction)
{
    SDL_Color col;
    SDL_Color col2;
    SDL_Color col3;

    if (direction == 1)
    {
        col = GetPixel(masque, posPerso.x + posPerso.w - 1, posPerso.y + posPerso.h - 1);
        col2 = GetPixel(masque, posPerso.x + posPerso.w - 1, posPerso.y + posPerso.h / 2);
        col3 = GetPixel(masque, posPerso.x + posPerso.w - 1, posPerso.y);
    }
    else if (direction == 0)
    {
        col = GetPixel(masque, posPerso.x, posPerso.y + posPerso.h - 1);
        col2 = GetPixel(masque, posPerso.x, posPerso.y + posPerso.h / 2);
        col3 = GetPixel(masque, posPerso.x, posPerso.y);
    }
    else if (direction == 2)
    {
        col = GetPixel(masque, posPerso.x + posPerso.w - 1, posPerso.y);
        col2 = GetPixel(masque, posPerso.x + posPerso.w / 2, posPerso.y);
        col3 = GetPixel(masque, posPerso.x, posPerso.y);
    }
    else if (direction == 3)
    {
        col = GetPixel(masque, posPerso.x + posPerso.w - 1, posPerso.y + posPerso.h - 1);
        col2 = GetPixel(masque, posPerso.x + posPerso.w / 2, posPerso.y + posPerso.h - 1);
        col3 = GetPixel(masque, posPerso.x, posPerso.y + posPerso.h - 1);
    }
    else
    {
        return 0;
    }

    if ((col.r == 0 && col.g == 0 && col.b == 0) ||
        (col2.r == 0 && col2.g == 0 && col2.b == 0) ||
        (col3.r == 0 && col3.g == 0 && col3.b == 0))
    {
        return 1;
    }

    return 0;
}

int collisionBB(SDL_Rect perso, SDL_Rect obstacle)
{
    if (obstacle.y > perso.y + perso.h)
        return 0;
    if (obstacle.x > perso.x + perso.w)
        return 0;
    if (obstacle.y + obstacle.h < perso.y)
        return 0;
    if (obstacle.x + obstacle.w < perso.x)
        return 0;
    return 1;
}

void animation(minimap *m, int *currentLevel)
{
    static int direction = 1;

    m->posmap.x += direction;
    if (m->posmap.x > 710)
        direction = -1;
    if (m->posmap.x < 660)
        direction = 1;

    (void)currentLevel;
}

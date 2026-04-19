#include "lot2.h"
#include <stdio.h>
#include <math.h>

void initialiser_background(Background* bg, int level) {
    bg->nbPlateformes = 0;
    bg->nbGuides = 0;
    bg->mode_ecran = 1; 

    bg->camera.x = 0;
    bg->camera.y = 0;
    bg->camera.levelWidth = LEVEL_WIDTH;
    bg->camera.levelHeight = LEVEL_HEIGHT;
    
    bg->timer.startTime = SDL_GetTicks();
    bg->timer.currentTime = 0;
    bg->timer.secondes = 0;
    bg->timer.minutes = 0;
    
    printf("[BACKGROUND] Initialisation niveau %d\n", level);
    
    Plateforme sol = {{0, 550, 2000, 50}, PLATFORM_FIXE, 1, -1, 0, 0, 0, 0, 0};
    bg->plateformes[bg->nbPlateformes++] = sol;
    
    if (level == 1) {

        Plateforme p1 = {{300, 450, 100, 20}, PLATFORM_FIXE, 1, -1, 0, 0, 0, 0, 0};
        bg->plateformes[bg->nbPlateformes++] = p1;
        
        Plateforme p2 = {{500, 350, 80, 20}, PLATFORM_MOBILE_H, 1, -1, 500, 350, 150, 2.0f, 0};
        bg->plateformes[bg->nbPlateformes++] = p2;
        
        Plateforme p3 = {{700, 400, 80, 20}, PLATFORM_MOBILE_V, 1, -1, 700, 400, 100, 1.5f, 0};
        bg->plateformes[bg->nbPlateformes++] = p3;
        
        Plateforme p4 = {{900, 450, 80, 20}, PLATFORM_DESTRUCTIBLE, 1, 3, 0, 0, 0, 0, 0};
        bg->plateformes[bg->nbPlateformes++] = p4;
        
        printf("  -> %d plateformes (Level 1)\n", bg->nbPlateformes);
        
    } else if (level == 2) {
     
        for (int i = 0; i < 6; i++) {
            int type = (i % 3 == 0) ? PLATFORM_MOBILE_H :
                       (i % 3 == 1) ? PLATFORM_MOBILE_V : PLATFORM_FIXE;

            Plateforme p = {{200 + i*150, 400 - (i%3)*50, 80, 20}, 
                           type, 1, (type == PLATFORM_DESTRUCTIBLE) ? 3 : -1, 
                           200 + i*150, 400 - (i%3)*50, 120, 2.0f, 0};

            bg->plateformes[bg->nbPlateformes++] = p;
        }

        printf("  -> %d plateformes (Level 2)\n", bg->nbPlateformes);
    }
}

void afficher_plateformes(SDL_Renderer* renderer, Background* bg) {
    for (int i = 0; i < bg->nbPlateformes; i++) {
        Plateforme* p = &bg->plateformes[i];

        if (p->active) {

            SDL_Rect screenRect = {
                p->rect.x - bg->camera.x,
                p->rect.y - bg->camera.y,
                p->rect.w,
                p->rect.h
            };

            if (!(screenRect.x + screenRect.w < 0 || screenRect.x > SCREEN_WIDTH ||
                  screenRect.y + screenRect.h < 0 || screenRect.y > SCREEN_HEIGHT)) {

                switch(p->type) {
                    case PLATFORM_FIXE:
                        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                        break;

                    case PLATFORM_MOBILE_H:
                    case PLATFORM_MOBILE_V:
                        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
                        break;

                    case PLATFORM_DESTRUCTIBLE:
                        if(p->health == 3)
                            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
                        else if(p->health == 2)
                            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
                        else
                            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                        break;
                }

                SDL_RenderFillRect(renderer, &screenRect);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &screenRect);
            }
        }
    }
}

void scrolling(Background* bg, int dx, int dy) {
    bg->camera.x += dx;
    bg->camera.y += dy;
    
    if (bg->camera.x < 0) bg->camera.x = 0;
    if (bg->camera.y < 0) bg->camera.y = 0;

    if (bg->camera.x > bg->camera.levelWidth - SCREEN_WIDTH)
        bg->camera.x = bg->camera.levelWidth - SCREEN_WIDTH;

    if (bg->camera.y > bg->camera.levelHeight - SCREEN_HEIGHT)
        bg->camera.y = bg->camera.levelHeight - SCREEN_HEIGHT;
}

void partage_ecran(Background* bg, int mode) {
    bg->mode_ecran = mode;
    printf("[SPLIT] Mode: %s\n", mode == 1 ? "MONO" : "SPLIT");
}

void afficher_temps(SDL_Renderer* renderer, TTF_Font* font, GameTimer* timer, int x, int y) {
    timer->currentTime = SDL_GetTicks() - timer->startTime;
    timer->secondes = (timer->currentTime / 1000) % 60;
    timer->minutes = (timer->currentTime / 60000);
    
    char temps[20];
    sprintf(temps, "%02d:%02d", timer->minutes, timer->secondes);
    
    if (font) {
        SDL_Color blanc = {255, 255, 255, 255};
        SDL_Surface* surf = TTF_RenderText_Solid(font, temps, blanc);

        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_RenderCopy(renderer, tex, NULL, &(SDL_Rect){x, y, surf->w, surf->h});
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }
}

void init_guide(GuideWindow* guide, int x, int y, int w, int h, const char* titre, const char* contenu) {
    guide->rect.x = x;
    guide->rect.y = y;
    guide->rect.w = w;
    guide->rect.h = h;
    strcpy(guide->titre, titre);
    strcpy(guide->contenu, contenu);
    guide->visible = true;
    guide->texture = NULL;
}

void afficher_guide(SDL_Renderer* renderer, GuideWindow* guide) {
    if (guide->visible) {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 220);
        SDL_RenderFillRect(renderer, &guide->rect);

        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawRect(renderer, &guide->rect);

        printf("[GUIDE] %s\n", guide->titre);
    }
}

void mettre_a_jour_plateformes(Background* bg, float dt) {
    static float time = 0;
    time += dt;

    for (int i = 0; i < bg->nbPlateformes; i++) {
        Plateforme* p = &bg->plateformes[i];

        if (p->active) {

            switch(p->type) {
                case PLATFORM_MOBILE_H:
                    p->rect.x = p->startX + sin(time * p->speed) * p->moveRange;
                    break;

                case PLATFORM_MOBILE_V:
                    p->rect.y = p->startY + sin(time * p->speed) * p->moveRange;
                    break;

                default:
                    break;
            }
        }
    }
}

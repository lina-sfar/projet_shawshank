#include "lot2.h"
#include <SDL2/SDL.h>

int main() {
    printf("\n=== TEST LOT 2 - BACKGROUND ===\n");
    printf("Commandes:\n");
    printf("  Fleches → Scrolling\n");
    printf("  S → Split screen (mono/multi)\n");
    printf("  ECHAP → Quitter\n\n");
    
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    SDL_Window* window = SDL_CreateWindow("LOT 2 - Background",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf", 16);
    
    Background bg;
    initialiser_background(&bg, 2); 
    
    init_guide(&bg.guides[0], 50, 50, 200, 100, "Bienvenue", "Bienvenue dans Shawshank!");
    init_guide(&bg.guides[1], 550, 50, 200, 100, "Plateformes", "Les plateformes mobiles bougent!");
    bg.nbGuides = 2;
    
    int running = 1;
    SDL_Event event;
    float lastTime = SDL_GetTicks();
    
    while (running) {
        float currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_UP: scrolling(&bg, 0, -20); break;
                    case SDLK_DOWN: scrolling(&bg, 0, 20); break;
                    case SDLK_LEFT: scrolling(&bg, -20, 0); break;
                    case SDLK_RIGHT: scrolling(&bg, 20, 0); break;
                    case SDLK_s: 
                        partage_ecran(&bg, bg.mode_ecran == 1 ? 2 : 1); 
                        break;
                }
            }
        }
        
        mettre_a_jour_plateformes(&bg, dt);
        
    
        SDL_SetRenderDrawColor(renderer, 50, 100, 150, 255);
        SDL_RenderClear(renderer);
        
       
        if (bg.mode_ecran == 2) {
     
            SDL_Rect viewport1 = {0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT};
            SDL_RenderSetViewport(renderer, &viewport1);
            afficher_plateformes(renderer, &bg);
            
       
            SDL_Rect viewport2 = {SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT};
            SDL_RenderSetViewport(renderer, &viewport2);
            afficher_plateformes(renderer, &bg);
            
        
            SDL_RenderSetViewport(renderer, NULL);
          
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);
        } else {

            afficher_plateformes(renderer, &bg);
        }
        
        for (int i = 0; i < bg.nbGuides; i++) {
            afficher_guide(renderer, &bg.guides[i]);
        }
        
      
        afficher_temps(renderer, font, &bg.timer, 10, 10);
      
        if (font) {
            char mode[30];
            sprintf(mode, "Mode: %s (S pour changer)", bg.mode_ecran == 1 ? "MONO" : "SPLIT");
            SDL_Color blanc = {255,255,255,255};
            SDL_Surface* surf = TTF_RenderText_Solid(font, mode, blanc);
            if (surf) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                SDL_RenderCopy(renderer, tex, NULL, &(SDL_Rect){10, 50, surf->w, surf->h});
                SDL_FreeSurface(surf);
                SDL_DestroyTexture(tex);
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}


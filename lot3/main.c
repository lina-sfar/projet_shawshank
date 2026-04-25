#include "lot3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

void lot3() {
    printf("\n========== JEU AVEC BACKGROUNDS ==========\n");
    printf("  ← →  : Deplacer le personnage\n");
    printf("  ↑    : SAUTER\n");
    printf("  ESPACE : Attaquer\n");
    printf("  1 : Niveau 1 (Goblins)\n");
    printf("  2 : Niveau 2 (Ogres & Demons)\n");
    printf("  ECHAP : Quitter\n\n");
    
    printf("DOSSIER 'sprites/' REQUIS:\n");
    printf("  backgrounds/: bg_level1.png, bg_level2.png\n");
    printf("  enemies/: goblin_walk_1-6.png, goblin_attack_1-4.png\n");
    printf("  enemies/: ogre_walk_1-6.png, ogre_attack_1-4.png\n");
    printf("  enemies/: demon_walk_1-6.png, demon_attack_1-4.png\n");
    printf("  items/: score_orb.png, heart.png, potion.png\n\n");
    
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur d'initialisation de SDL_image : %s\n", IMG_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("RPG Game - Backgrounds", 
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED, 
                                          800, 600, 
                                          SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf", 36);
    if (!font) {
        printf("Police non trouvee, texte limite\n");
    }
    
    Niveau niveau;
    initialiser_npc(&niveau, 1, renderer);
    
    SDL_Rect joueur = {400, 450, 60, 90};
    int score = 0, vies = 3, pv = 100;
    int attaque = 0, att_timer = 0;
    float velY = 0; 
    int en_lair = 0;
    int invincible = 0;
    int game_over = 0;
    int running = 1;
    SDL_Event event;
    
    int gauche = 0, droite = 0, saut = 0;
    
    Uint32 frame_time;
    const int FPS_DELAY = 16;
    
    while (running) {
        frame_time = SDL_GetTicks();
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            
            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_1: 
                        if(!game_over) {
                            liberer_textures_npc(&niveau);
                            initialiser_npc(&niveau, 1, renderer);
                            score = 0; pv = 100; vies = 3;
                            printf("\n*** NIVEAU 1 COMMENCE ***\n\n");
                        }
                        break;
                    case SDLK_2: 
                        if(!game_over) {
                            liberer_textures_npc(&niveau);
                            initialiser_npc(&niveau, 2, renderer);
                            score = 0; pv = 100; vies = 3;
                            printf("\n*** NIVEAU 2 COMMENCE ***\n\n");
                        }
                        break;
                    case SDLK_LEFT: gauche = 1; break;
                    case SDLK_RIGHT: droite = 1; break;
                    case SDLK_UP: saut = 1; break;
                    case SDLK_SPACE: if(!game_over && att_timer == 0) attaque = 1; break;
                }
            }
            if (event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT: gauche = 0; break;
                    case SDLK_RIGHT: droite = 0; break;
                    case SDLK_UP: saut = 0; break;
                }
            }
        }
        
        if (!game_over) {
            int vitesse = 6;
            if (gauche) joueur.x -= vitesse;
            if (droite) joueur.x += vitesse;
            if (joueur.x < 0) joueur.x = 0;
            if (joueur.x > 740) joueur.x = 740;
            
            if (saut && !en_lair) {
                velY = -12;
                en_lair = 1;
            }
            if (en_lair) {
                velY += 0.6;
                joueur.y += velY;
            }
            if (joueur.y >= 450) {
                joueur.y = 450;
                velY = 0;
                en_lair = 0;
            }
            
            if (invincible > 0) invincible--;
            
            if (attaque && att_timer == 0) {
                att_timer = 25;
                printf("\n*** ATTAQUE ! ***\n");
                
                SDL_Rect attaque_rect = {joueur.x + (gauche ? -30 : joueur.w + 10), 
                                        joueur.y, 40, joueur.h};
                
                for (int i = 0; i < niveau.nb_ennemis; i++) {
                    Ennemi* e = &niveau.ennemis[i];
                    if (!e->actif) continue;
                    
                    SDL_Rect er = {e->x, e->y, e->w, e->h};
                    if (collision_bb(attaque_rect, er)) {
                        gerer_sante(e, 20);
                        if (e->etat_sante == 3) {
                            score += e->score_valeur;
                            printf("  Score: %d\n", score);
                        }
                    }
                }
                attaque = 0;
            }
            if (att_timer > 0) att_timer--;
            
            update_npc(&niveau, joueur.x + joueur.w/2, joueur.y + joueur.h/2);
            collecter_es(&niveau, joueur, &score, &vies, &pv);
            
            for (int i = 0; i < niveau.nb_ennemis; i++) {
                Ennemi* e = &niveau.ennemis[i];
                if (!e->actif) continue;
                
                SDL_Rect er = {e->x, e->y, e->w, e->h};
                if (collision_bb(joueur, er) && e->timer_attaque > 15 && invincible == 0) {
                    pv -= e->degats;
                    invincible = 40;
                    printf("[JOUEUR] Subit %d degats ! PV: %d/100\n", e->degats, pv);
                    
                    if (pv <= 0) {
                        vies--;
                        pv = 100;
                        printf("[JOUEUR] Vie perdue ! Vies restantes: %d\n", vies);
                        
                        if (vies <= 0) {
                            game_over = 1;
                            printf("\n*** GAME OVER ***\n");
                        }
                    }
                }
            }
            
            int ennemis_restants = 0;
            for (int i = 0; i < niveau.nb_ennemis; i++) {
                if (niveau.ennemis[i].actif) ennemis_restants++;
            }
            if (ennemis_restants == 0 && niveau.nb_ennemis > 0) {
                printf("\n*** VICTOIRE ! ***\n");
                printf("Appuyez sur 1 ou 2 pour le prochain niveau\n\n");
                niveau.nb_ennemis = 0;
            }
        }
        
        // AFFICHAGE
        afficher_background(renderer, &niveau);
        
        if (!game_over) {
            // Sol
            SDL_Rect sol = {0, 520, 800, 80};
            SDL_SetRenderDrawColor(renderer, 34, 139, 34, 200);
            SDL_RenderFillRect(renderer, &sol);
            
            // Herbe
            SDL_SetRenderDrawColor(renderer, 50, 205, 50, 255);
            for(int i = 0; i < 800; i += 20) {
                SDL_Rect herbe = {i, 515, 10, 10};
                SDL_RenderFillRect(renderer, &herbe);
            }
            
            // Joueur
            if (invincible > 0 && (SDL_GetTicks() / 100) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else if (en_lair) {
                SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
            } else if (att_timer > 0) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 70, 130, 200, 255);
            }
            SDL_RenderFillRect(renderer, &joueur);
            
            // Yeux
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect oeil_g = {joueur.x + 15, joueur.y + 25, 10, 10};
            SDL_Rect oeil_d = {joueur.x + 35, joueur.y + 25, 10, 10};
            SDL_RenderFillRect(renderer, &oeil_g);
            SDL_RenderFillRect(renderer, &oeil_d);
            
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect pupille_g = {joueur.x + 18, joueur.y + 28, 5, 5};
            SDL_Rect pupille_d = {joueur.x + 38, joueur.y + 28, 5, 5};
            SDL_RenderFillRect(renderer, &pupille_g);
            SDL_RenderFillRect(renderer, &pupille_d);
            
            afficher_npc(renderer, &niveau);
            
            // Interface
            if (font) {
                SDL_Rect pv_fond = {0, 0, 0, 0};
                SDL_SetRenderDrawColor(renderer, 100, 0, 0, 200);
                SDL_RenderFillRect(renderer, &pv_fond);
                SDL_Rect pv_barre = {10, 50, (int)(200 * pv / 100.0f), 25};
                SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
                SDL_RenderFillRect(renderer, &pv_barre);
                
                char pv_texte[20];
                sprintf(pv_texte, "PV: %d/100", pv);
                SDL_Color blanc = {255, 255, 255, 255};
                SDL_Surface* surface = TTF_RenderText_Solid(font, pv_texte, blanc);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect dest = {15, 52, surface->w, surface->h};
                    SDL_RenderCopy(renderer, texture, NULL, &dest);
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
                }
                
                char info[100];
                sprintf(info, "Score: %d  Vies: %d", score, vies);
                surface = TTF_RenderText_Solid(font, info, blanc);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect dest = {10, 10, surface->w, surface->h};
                    SDL_RenderCopy(renderer, texture, NULL, &dest);
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
                }
            }
        } else {
            if (font) {
                SDL_Color rouge = {255, 0, 0, 255};
                SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME OVER", rouge);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect dest = {300, 200, surface->w, surface->h};
                    SDL_RenderCopy(renderer, texture, NULL, &dest);
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
                }
                
                char score_texte[50];
                sprintf(score_texte, "Score Final: %d", score);
                SDL_Color blanc = {255, 255, 255, 255};
                surface = TTF_RenderText_Solid(font, score_texte, blanc);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect dest = {310, 280, surface->w, surface->h};
                    SDL_RenderCopy(renderer, texture, NULL, &dest);
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
                }
            }
        }
        
        SDL_RenderPresent(renderer);
        
        int frame_delay = FPS_DELAY - (SDL_GetTicks() - frame_time);
        if (frame_delay > 0) {
            SDL_Delay(frame_delay);
        }
    }
    
    liberer_textures_npc(&niveau);
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

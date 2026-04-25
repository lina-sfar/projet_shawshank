#include "lot3.h"
#include <stdio.h>

void run_lot3(SDL_Renderer* renderer) {

    printf("\n========== JEU AVEC BACKGROUNDS ==========\n");

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

            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_KEYDOWN) {

                switch(event.key.keysym.sym) {

                    case SDLK_ESCAPE:
                        running = 0;
                        break;

                    case SDLK_1:
                        if (!game_over) {
                            liberer_textures_npc(&niveau);
                            initialiser_npc(&niveau, 1, renderer);
                            score = 0; pv = 100; vies = 3;
                        }
                        break;

                    case SDLK_2:
                        if (!game_over) {
                            liberer_textures_npc(&niveau);
                            initialiser_npc(&niveau, 2, renderer);
                            score = 0; pv = 100; vies = 3;
                        }
                        break;

                    case SDLK_LEFT: gauche = 1; break;
                    case SDLK_RIGHT: droite = 1; break;
                    case SDLK_UP: saut = 1; break;
                    case SDLK_SPACE:
                        if (!game_over && att_timer == 0)
                            attaque = 1;
                        break;
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

        // ================= GAME LOGIC =================

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
                attaque = 0;
            }

            if (att_timer > 0)
                att_timer--;

            update_npc(&niveau, joueur.x + joueur.w/2, joueur.y + joueur.h/2);
            collecter_es(&niveau, joueur, &score, &vies, &pv);

            int ennemis_restants = 0;
            for (int i = 0; i < niveau.nb_ennemis; i++) {
                if (niveau.ennemis[i].actif)
                    ennemis_restants++;
            }

            if (ennemis_restants == 0 && niveau.nb_ennemis > 0) {
                niveau.nb_ennemis = 0;
            }
        }

        // ================= RENDER =================

        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderClear(renderer);

        SDL_Rect sol = {0, 520, 800, 80};
        SDL_RenderFillRect(renderer, &sol);

        SDL_SetRenderDrawColor(renderer, 70, 130, 200, 255);
        SDL_RenderFillRect(renderer, &joueur);

        afficher_npc(renderer, &niveau);

        SDL_RenderPresent(renderer);

        int frame_delay = FPS_DELAY - (SDL_GetTicks() - frame_time);
        if (frame_delay > 0)
            SDL_Delay(frame_delay);
    }

    liberer_textures_npc(&niveau);
}

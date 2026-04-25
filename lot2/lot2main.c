#include "lot2.h"
#include <stdio.h>

void run_lot2(SDL_Renderer* renderer) {

    printf("\n=== TEST LOT 2 - BACKGROUND ===\n");

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

            if (event.type == SDL_QUIT)
                running = 0;

            if (event.type == SDL_KEYDOWN) {

                switch (event.key.keysym.sym) {

                    case SDLK_ESCAPE:
                        running = 0;
                        break;

                    case SDLK_UP:
                        scrolling(&bg, 0, -20);
                        break;

                    case SDLK_DOWN:
                        scrolling(&bg, 0, 20);
                        break;

                    case SDLK_LEFT:
                        scrolling(&bg, -20, 0);
                        break;

                    case SDLK_RIGHT:
                        scrolling(&bg, 20, 0);
                        break;

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

            SDL_Rect v1 = {0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT};
            SDL_RenderSetViewport(renderer, &v1);
            afficher_plateformes(renderer, &bg);

            SDL_Rect v2 = {SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT};
            SDL_RenderSetViewport(renderer, &v2);
            afficher_plateformes(renderer, &bg);

            SDL_RenderSetViewport(renderer, NULL);

        } else {
            afficher_plateformes(renderer, &bg);
        }

        for (int i = 0; i < bg.nbGuides; i++) {
            afficher_guide(renderer, &bg.guides[i]);
        }

        afficher_temps(renderer, NULL, &bg.timer, 10, 10);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

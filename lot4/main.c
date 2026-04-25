#include "lot4.h"
#include <stdio.h>

void run_lot4(SDL_Renderer* renderer)
{
    SDL_Event event;

    SDL_Texture *bg, *bg1, *perso, *rock, *bigrock;
    SDL_Surface *masque;

    SDL_Rect rock_positions[4];
    SDL_Rect rock_rects[4];
    SDL_Rect posPerso;

    minimap m;

    int currentLevel = 1;
    int game = 1;
    int direction = -1;

    // Load textures (NO SDL init here)
    bg = load_texture(renderer, "bg7.png");
    bg1 = load_texture(renderer, "bg8.png");
    perso = load_texture(renderer, "perso.png");
    rock = load_texture(renderer, "rocks.png");
    bigrock = load_texture(renderer, "rocks2.png");
    masque = IMG_Load("mask6.png");

    if (!bg || !bg1 || !perso || !rock || !bigrock || !masque)
    {
        printf("Erreur chargement ressources LOT4\n");
        return;
    }

    // Rocks setup
    rock_positions[0] = (SDL_Rect){100, 500, 60, 60};
    rock_positions[1] = (SDL_Rect){350, 500, 90, 90};
    rock_positions[2] = (SDL_Rect){520, 500, 60, 60};
    rock_positions[3] = (SDL_Rect){750, 530, 90, 90};

    for (int i = 0; i < 4; i++)
        rock_rects[i] = rock_positions[i];

    posPerso = (SDL_Rect){200, 150, 48, 48};

    initmap(&m, renderer);

    while (game)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                game = 0;

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        game = 0;
                        break;

                    case SDLK_RIGHT: direction = 1; break;
                    case SDLK_LEFT: direction = 0; break;
                    case SDLK_UP: direction = 2; break;
                    case SDLK_DOWN: direction = 3; break;

                    case SDLK_SPACE:
                        currentLevel = (currentLevel == 1) ? 2 : 1;
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                direction = -1;
            }
        }

        if (direction != -1 && collisionmap(masque, posPerso, direction) == 0)
        {
            SDL_Rect nextPos = posPerso;

            if (direction == 1) nextPos.x += 10;
            if (direction == 0) nextPos.x -= 10;
            if (direction == 2) nextPos.y -= 10;
            if (direction == 3) nextPos.y += 10;

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

        // ================= RENDER =================
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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
}

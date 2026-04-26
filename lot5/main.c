#include "enigme.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LARGEUR_ECRAN 800
#define HAUTEUR_ECRAN 600
#define TEMPS_MAX_ENIGME 30

// helper
int souris_sur_reponse(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void run_lot5(SDL_Renderer* renderer)
{
    srand(time(NULL));

    TTF_Font* police = TTF_OpenFont("arial.ttf", 24);

    Enigme toutes_les_enigmes[MAX_ENIGMES];
    int nb_enigmes = charger_enigmes(toutes_les_enigmes, "enigmes.txt");

    if (nb_enigmes == 0)
    {
        printf("Erreur: aucune enigme chargee\n");
        return;
    }

    int score = 0;
    int vies = 3;
    int niveau = 1;

    int jeu_termine = 0;
    int mode_enigme = 0;
    int afficher_message = 0;

    Enigme enigme_actuelle;
    int temps_restant = TEMPS_MAX_ENIGME;
    int temps_depart = 0;
    int choix_joueur = 0;

    SDL_Event event;

    while (!jeu_termine && vies > 0)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                jeu_termine = 1;

            if (event.type == SDL_MOUSEBUTTONDOWN && mode_enigme && !afficher_message)
            {
                int x = event.button.x;
                int y = event.button.y;

                if (souris_sur_reponse(x, y, enigme_actuelle.pos_rep1)) choix_joueur = 1;
                else if (souris_sur_reponse(x, y, enigme_actuelle.pos_rep2)) choix_joueur = 2;
                else if (souris_sur_reponse(x, y, enigme_actuelle.pos_rep3)) choix_joueur = 3;
            }
        }

        const Uint8* clavier = SDL_GetKeyboardState(NULL);

        if (clavier[SDL_SCANCODE_SPACE] && !mode_enigme && !afficher_message)
        {
            mode_enigme = 1;
            enigme_actuelle = generer_enigme_aleatoire(toutes_les_enigmes, nb_enigmes);
            creer_textures_enigme(&enigme_actuelle, renderer, police);

            temps_restant = TEMPS_MAX_ENIGME;
            temps_depart = SDL_GetTicks() / 1000;
            choix_joueur = 0;
        }

        if (mode_enigme && !afficher_message)
        {
            int ecoule = (SDL_GetTicks() / 1000) - temps_depart;
            temps_restant = TEMPS_MAX_ENIGME - ecoule;

            if (temps_restant <= 0)
            {
                vies--;
                afficher_message = 1;
                mode_enigme = 0;
            }
        }

        if (choix_joueur && mode_enigme)
        {
            verifier_et_mettre_a_jour(&enigme_actuelle, choix_joueur, &score, &vies, &niveau);
            afficher_message = 1;
            mode_enigme = 0;
        }

        SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255);
        SDL_RenderClear(renderer);

        if (mode_enigme)
        {
            afficher_enigme(renderer, &enigme_actuelle);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (police)
        TTF_CloseFont(police);
}

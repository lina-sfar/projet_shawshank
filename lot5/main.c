#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "enigme.h"

#define LARGEUR_ECRAN 800
#define HAUTEUR_ECRAN 600
#define TEMPS_MAX_ENIGME 30

// ===== FUNCTION PROTOTYPES (DECLARATIONS) =====
int compter_lignes(char* nom_fichier);
int charger_enigmes(Enigme* tableau, char* nom_fichier);
void reinitialiser_enigmes(Enigme* tableau, int nb_enigmes);
Enigme generer_enigme_aleatoire(Enigme* tableau, int nb_enigmes);
void creer_textures_enigme(Enigme* e, SDL_Renderer* renderer, TTF_Font* police);
void afficher_enigme(SDL_Renderer* renderer, Enigme* e);
void liberer_textures_enigme(Enigme* e);
int verifier_et_mettre_a_jour(Enigme* e,char choix_joueur, int* score, int* vies, int* niveau);
void afficher_chronometre_animation(SDL_Renderer* renderer, int temps_restant, int temps_max, int x, int y, int largeur, int hauteur);
int souris_sur_reponse(int x, int y, SDL_Rect rect);
// ============================================

int souris_sur_reponse(int x, int y, SDL_Rect rect) {
    return (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    srand(time(NULL));
    
    SDL_Window* fenetre = SDL_CreateWindow("Jeu d'Enigmes",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LARGEUR_ECRAN, HAUTEUR_ECRAN, SDL_WINDOW_SHOWN);
    
    SDL_Renderer* renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    
    TTF_Font* police = TTF_OpenFont("arial.ttf", 24);
    if (police == NULL) {
        printf("Police non trouvée, essai avec une autre police...\n");
        police = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
        if (police == NULL) {
            printf("Aucune police trouvée, les textes ne s'afficheront pas\n");
        }
    }
    
    Enigme toutes_les_enigmes[MAX_ENIGMES];
    int nb_enigmes = charger_enigmes(toutes_les_enigmes, "enigmes.txt");
    printf("%d enigmes chargees avec succes !\n", nb_enigmes);
    
    if (nb_enigmes == 0) {
        printf("Erreur: Aucune enigme trouvee dans le fichier !\n");
        return 1;
    }
    
    int score = 0;
    int vies = 3;
    int niveau = 1;
    int jeu_termine = 0;
    int mode_enigme = 0;
    int afficher_message = 0;
    int temps_message = 0;
    
    Enigme enigme_actuelle;
    int temps_restant = TEMPS_MAX_ENIGME;
    int temps_depart = 0;
    int choix_joueur;
    
    int barre_x = 150;
    int barre_y = 50;
    int barre_largeur = 500;
    int barre_hauteur = 30;
    
    while (!jeu_termine && vies > 0) {
        SDL_Event evenement;
        while (SDL_PollEvent(&evenement)) {
            if (evenement.type == SDL_QUIT) {
                jeu_termine = 1;
            }
            
            if (evenement.type == SDL_MOUSEBUTTONDOWN && mode_enigme == 1 && afficher_message == 0) {
                int x = evenement.button.x;
                int y = evenement.button.y;
                
                if (souris_sur_reponse(x, y, enigme_actuelle.pos_rep1)) {
                    choix_joueur = 1;
                }
                else if (souris_sur_reponse(x, y, enigme_actuelle.pos_rep2)) {
                    choix_joueur = 2;
                }
                else if (souris_sur_reponse(x, y, enigme_actuelle.pos_rep3)) {
                    choix_joueur = 3;
                }
            }
        }
        
        const Uint8* clavier = SDL_GetKeyboardState(NULL);
        if (clavier[SDL_SCANCODE_SPACE] && mode_enigme == 0 && afficher_message == 0) {
            mode_enigme = 1;
            enigme_actuelle = generer_enigme_aleatoire(toutes_les_enigmes, nb_enigmes);
            creer_textures_enigme(&enigme_actuelle, renderer, police);
            temps_restant = TEMPS_MAX_ENIGME;
            temps_depart = SDL_GetTicks() / 1000;
            choix_joueur = 0;
        }
        
        if (mode_enigme == 1 && afficher_message == 0) {
            int temps_ecoule = (SDL_GetTicks() / 1000) - temps_depart;
            temps_restant = TEMPS_MAX_ENIGME - temps_ecoule;
            
            if (temps_restant <= 0) {
                vies--;
                printf("Temps ecoule ! -1 vie. Vies restantes: %d\n", vies);
                afficher_message = 1;
                temps_message = SDL_GetTicks() / 1000;
                mode_enigme = 0;
            }
        }
        
        if (choix_joueur != 0 && mode_enigme == 1 && afficher_message == 0) {
            verifier_et_mettre_a_jour(&enigme_actuelle, choix_joueur, &score, &vies, &niveau);
            afficher_message = 1;
            temps_message = SDL_GetTicks() / 1000;
            mode_enigme = 0;
        }
        
        if (afficher_message == 1) {
            if ((SDL_GetTicks() / 1000) - temps_message >= 2) {
                afficher_message = 0;
                liberer_textures_enigme(&enigme_actuelle);
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 50, 50, 150, 255);
        SDL_RenderClear(renderer);
        
        if (police != NULL) {
            char infos[100];
            SDL_Color blanc = {255, 255, 255, 255};
            SDL_Surface* surface;
            SDL_Texture* texture;
            
            sprintf(infos, "Score: %d   Vies: %d   Niveau: %d", score, vies, niveau);
            surface = TTF_RenderUTF8_Blended(police, infos, blanc);
            if (surface != NULL) {
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = {20, 20, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
        
        if (mode_enigme == 1) {
            afficher_enigme(renderer, &enigme_actuelle);
            afficher_chronometre_animation(renderer, temps_restant, TEMPS_MAX_ENIGME,
                                           barre_x, barre_y, barre_largeur, barre_hauteur);
        } 
        else if (afficher_message == 1 && police != NULL) {
            char* msg = (enigme_actuelle.etat == 1) ? "Bonne reponse !" : "Mauvaise reponse !";
            SDL_Color blanc = {255, 255, 255, 255};
            SDL_Surface* surface = TTF_RenderUTF8_Blended(police, msg, blanc);
            if (surface != NULL) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = {LARGEUR_ECRAN/2 - surface->w/2, HAUTEUR_ECRAN/2 - surface->h/2, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        } 
        else if (police != NULL) {
            char instruction[100];
            SDL_Color blanc = {255, 255, 255, 255};
            sprintf(instruction, "Appuyez sur ESPACE pour afficher une enigme !");
            SDL_Surface* surface = TTF_RenderUTF8_Blended(police, instruction, blanc);
            if (surface != NULL) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect rect = {LARGEUR_ECRAN/2 - surface->w/2, HAUTEUR_ECRAN/2, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    if (police != NULL) {
        char fin[100];
        SDL_Color blanc = {255, 255, 255, 255};
        sprintf(fin, "GAME OVER ! Score final: %d", score);
        SDL_Surface* surface = TTF_RenderUTF8_Blended(police, fin, blanc);
        if (surface != NULL) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {LARGEUR_ECRAN/2 - surface->w/2, HAUTEUR_ECRAN/2, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
    
    if (police != NULL) TTF_CloseFont(police);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}

#include "enigme.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Compter le nombre de lignes dans le fichier
int compter_lignes(char* nom_fichier) {
    FILE* f = fopen(nom_fichier, "r");
    if (f == NULL) return 0;
    
    int lignes = 0;
    char ligne[500];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        lignes++;
    }
    fclose(f);
    return lignes;
}

// Charger toutes les énigmes depuis le fichier texte
int charger_enigmes(Enigme* tableau, char* nom_fichier) {
    FILE* f = fopen(nom_fichier, "r");
    if (f == NULL) {
        printf("Erreur: Impossible d'ouvrir %s\n", nom_fichier);
        return 0;
    }
    
    char ligne[500];
    int i = 0;
    
    while (fgets(ligne, sizeof(ligne), f) != NULL && i < MAX_ENIGMES) {
        ligne[strlen(ligne) - 1] = '\0';
        
        char* morceau = strtok(ligne, "|");
        if (morceau != NULL) strcpy(tableau[i].question, morceau);
        
        morceau = strtok(NULL, "|");
        if (morceau != NULL) strcpy(tableau[i].reponse1, morceau);
        
        morceau = strtok(NULL, "|");
        if (morceau != NULL) strcpy(tableau[i].reponse2, morceau);
        
        morceau = strtok(NULL, "|");
        if (morceau != NULL) strcpy(tableau[i].reponse3, morceau);
        
        morceau = strtok(NULL, "|");
        if (morceau != NULL) tableau[i].bonne_reponse = atoi(morceau);
        
        tableau[i].deja_vu = 0;
        tableau[i].etat = 0;
        tableau[i].texture_question = NULL;
        tableau[i].texture_rep1 = NULL;
        tableau[i].texture_rep2 = NULL;
        tableau[i].texture_rep3 = NULL;
        
        tableau[i].pos_question.x = 100;
        tableau[i].pos_question.y = 100;
        tableau[i].pos_question.w = 600;
        tableau[i].pos_question.h = 50;
        
        tableau[i].pos_rep1.x = 150;
        tableau[i].pos_rep1.y = 250;
        tableau[i].pos_rep1.w = 500;
        tableau[i].pos_rep1.h = 40;
        
        tableau[i].pos_rep2.x = 150;
        tableau[i].pos_rep2.y = 320;
        tableau[i].pos_rep2.w = 500;
        tableau[i].pos_rep2.h = 40;
        
        tableau[i].pos_rep3.x = 150;
        tableau[i].pos_rep3.y = 390;
        tableau[i].pos_rep3.w = 500;
        tableau[i].pos_rep3.h = 40;
        
        i++;
    }
    
    fclose(f);
    return i;
}

// Créer les textures à partir du texte
void creer_textures_enigme(Enigme* e, SDL_Renderer* renderer, TTF_Font* police) {
    if (police == NULL) return;
    
    SDL_Surface* surface;
    SDL_Color blanc = {255, 255, 255, 255};
    SDL_Color gris = {200, 200, 200, 255};
    
    surface = TTF_RenderUTF8_Blended(police, e->question, blanc);
    if (surface != NULL) {
        e->texture_question = SDL_CreateTextureFromSurface(renderer, surface);
        e->pos_question.w = surface->w;
        e->pos_question.h = surface->h;
        SDL_FreeSurface(surface);
    }
    
    char texte1[200];
    sprintf(texte1, "a. %s", e->reponse1);
    surface = TTF_RenderUTF8_Blended(police, texte1, gris);
    if (surface != NULL) {
        e->texture_rep1 = SDL_CreateTextureFromSurface(renderer, surface);
        e->pos_rep1.w = surface->w;
        e->pos_rep1.h = surface->h;
        SDL_FreeSurface(surface);
    }
    
    char texte2[200];
    sprintf(texte2, "b. %s", e->reponse2);
    surface = TTF_RenderUTF8_Blended(police, texte2, gris);
    if (surface != NULL) {
        e->texture_rep2 = SDL_CreateTextureFromSurface(renderer, surface);
        e->pos_rep2.w = surface->w;
        e->pos_rep2.h = surface->h;
        SDL_FreeSurface(surface);
    }
    
    char texte3[200];
    sprintf(texte3, "c. %s", e->reponse3);
    surface = TTF_RenderUTF8_Blended(police, texte3, gris);
    if (surface != NULL) {
        e->texture_rep3 = SDL_CreateTextureFromSurface(renderer, surface);
        e->pos_rep3.w = surface->w;
        e->pos_rep3.h = surface->h;
        SDL_FreeSurface(surface);
    }
}

// Afficher l'énigme à l'écran
void afficher_enigme(SDL_Renderer* renderer, Enigme* e) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect ecran = {0, 0, 800, 600};
    SDL_RenderFillRect(renderer, &ecran);
    
    if (e->texture_question != NULL) {
        SDL_RenderCopy(renderer, e->texture_question, NULL, &e->pos_question);
    }
    if (e->texture_rep1 != NULL) {
        SDL_RenderCopy(renderer, e->texture_rep1, NULL, &e->pos_rep1);
    }
    if (e->texture_rep2 != NULL) {
        SDL_RenderCopy(renderer, e->texture_rep2, NULL, &e->pos_rep2);
    }
    if (e->texture_rep3 != NULL) {
        SDL_RenderCopy(renderer, e->texture_rep3, NULL, &e->pos_rep3);
    }
}

// Libérer les textures
void liberer_textures_enigme(Enigme* e) {
    if (e->texture_question != NULL) SDL_DestroyTexture(e->texture_question);
    if (e->texture_rep1 != NULL) SDL_DestroyTexture(e->texture_rep1);
    if (e->texture_rep2 != NULL) SDL_DestroyTexture(e->texture_rep2);
    if (e->texture_rep3 != NULL) SDL_DestroyTexture(e->texture_rep3);
    
    e->texture_question = NULL;
    e->texture_rep1 = NULL;
    e->texture_rep2 = NULL;
    e->texture_rep3 = NULL;
}

// Réinitialiser toutes les énigmes
void reinitialiser_enigmes(Enigme* tableau, int nb_enigmes) {
    for (int i = 0; i < nb_enigmes; i++) {
        tableau[i].deja_vu = 0;
    }
    printf("Toutes les enigmes ont ete reinitialisees !\n");
}

// Générer une énigme aléatoire sans répéter
Enigme generer_enigme_aleatoire(Enigme* tableau, int nb_enigmes) {
    int toutes_vues = 1;
    for (int i = 0; i < nb_enigmes; i++) {
        if (tableau[i].deja_vu == 0) {
            toutes_vues = 0;
            break;
        }
    }
    
    if (toutes_vues == 1) {
        reinitialiser_enigmes(tableau, nb_enigmes);
    }
    
    int choix;
    do {
        choix = rand() % nb_enigmes;
    } while (tableau[choix].deja_vu == 1);
    
    tableau[choix].deja_vu = 1;
    tableau[choix].etat = 0;
    
    return tableau[choix];
}

// Vérifier la réponse et gérer score/vies/niveau
int verifier_et_mettre_a_jour(Enigme* e, int choix_joueur, int* score, int* vies, int* niveau) {
    if (choix_joueur == e->bonne_reponse) {
        e->etat = 1;
        *score = *score + 10;
        printf("Bonne reponse ! Score: %d\n", *score);
        
        if (*score >= *niveau * 50) {
            *niveau = *niveau + 1;
            printf("Niveau %d atteint !\n", *niveau);
        }
        return 1;
    } else {
        e->etat = -1;
        *vies = *vies - 1;
        if (*score >= 5) {
            *score = *score - 5;
        }
        printf("Mauvaise reponse ! Vies: %d\n", *vies);
        return 0;
    }
}

// Afficher le chronomètre avec animation
void afficher_chronometre_animation(SDL_Renderer* renderer, int temps_restant, int temps_max,
                                     int x, int y, int largeur, int hauteur) {
    float progression = (float)temps_restant / temps_max;
    int largeur_barre = largeur * progression;
    
    if (progression > 0.66) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    } else if (progression > 0.33) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }
    
    SDL_Rect barre = {x, y, largeur_barre, hauteur};
    SDL_RenderFillRect(renderer, &barre);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect contour = {x, y, largeur, hauteur};
    SDL_RenderDrawRect(renderer, &contour);
}

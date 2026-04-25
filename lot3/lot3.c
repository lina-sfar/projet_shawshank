#include "lot3.h"
#include <stdio.h>

// Fonction utilitaire pour charger une texture
SDL_Texture* charger_texture(const char* chemin, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(chemin);
    if (!surface) {
        printf("Erreur de chargement de l'image %s : %s\n", chemin, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Erreur de création de la texture pour %s : %s\n", chemin, SDL_GetError());
    }
    return texture;
}

// Fonction pour afficher le background
void afficher_background(SDL_Renderer* renderer, Niveau* niveau) {
    if (niveau->background != NULL) {
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, niveau->background, NULL, &destRect);
    } else {
        // Fallback: dégradé de couleur
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            int r = 50 + (i * 50 / SCREEN_HEIGHT);
            int g = 100 + (i * 80 / SCREEN_HEIGHT);
            int b = 150 + (i * 100 / SCREEN_HEIGHT);
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_Rect line = {0, i, SCREEN_WIDTH, 1};
            SDL_RenderFillRect(renderer, &line);
        }
    }
}

// Fonction pour charger le background selon le niveau
SDL_Texture* charger_background(int level, SDL_Renderer* renderer) {
    char chemin[256];
    switch(level) {
        case 1:
            sprintf(chemin, "sprites/back.png");
            break;
        case 2:
            sprintf(chemin, "sprites/back.png");
            break;
     
    }
    return charger_texture(chemin, renderer);
}

// Fonction pour charger toutes les frames d'animation
void charger_frames_animation(Ennemi* e, SDL_Renderer* renderer, const char* base_name) {
    char chemin[256];
    
    for (int i = 0; i < MAX_WALK_FRAMES; i++) {
        sprintf(chemin, "sprites/walk%d.png", i + 1);
        e->walk_frames[i] = charger_texture(chemin, renderer);
        if (!e->walk_frames[i]) {
            printf("  Attention: %s non trouvé\n", chemin);
        }
    }
    
    for (int i = 0; i < MAX_ATTACK_FRAMES; i++) {
        sprintf(chemin, "sprites/attack%d.png", i + 1);
        e->attack_frames[i] = charger_texture(chemin, renderer);
        if (!e->attack_frames[i]) {
            printf("  Attention: %s non trouvé\n", chemin);
        }
    }
}

// Charger la texture pour un élément spécial
SDL_Texture* charger_texture_es(int type, SDL_Renderer* renderer) {
    char chemin[256];
    switch(type) {
        case 1:
            sprintf(chemin, "sprites/score.png");
            break;
        case 2:
            sprintf(chemin, "sprites/life.png");
            break;
        case 3:
            sprintf(chemin, "sprites/portion.png");
            break;
        default:
            return NULL;
    }
    return charger_texture(chemin, renderer);
}

void initialiser_npc(Niveau* niveau, int level, SDL_Renderer* renderer) {
    niveau->niveau = level;
    niveau->nb_ennemis = 0;
    niveau->nb_es = 0;
    
    niveau->background = charger_background(level, renderer);
    
    printf("\n========== NIVEAU %d ==========\n", level);
    
    if (level == 1) {
        for (int i = 0; i < 3; i++) {
            Ennemi e = {
                .x = 120 + i * 220, .y = 380, .w = 120, .h = 120,
                .pv = 50, .pv_max = 50, .degats = 10, .score_valeur = 100,
                .type = 0, .trajectoire = 0, .vitesse = 0.3f,
                .direction = (i % 2 == 0) ? 1 : -1,
                .startX = 120 + i * 220, .startY = 450,
                .angle = 0, .actif = true, .timer_attaque = 0, .etat_sante = 0,
                .current_frame = 0,
                .max_frames = MAX_WALK_FRAMES,
                .frame_timer = 0,
                .animation_speed = 8
            };
            
            charger_frames_animation(&e, renderer, "goblin");
            e.current_texture = e.walk_frames[0];
            niveau->ennemis[niveau->nb_ennemis++] = e;
        }
        printf("  %d ennemis GOBLINS\n", niveau->nb_ennemis);
        
        niveau->es[niveau->nb_es++] = (ElementSpecial){200, 350, 40, 40, 1, 50, true, NULL};
        niveau->es[niveau->nb_es++] = (ElementSpecial){400, 300, 40, 40, 2, 1, true, NULL};
        niveau->es[niveau->nb_es++] = (ElementSpecial){600, 350, 40, 40, 3, 30, true, NULL};
        
        for (int i = 0; i < niveau->nb_es; i++) {
            niveau->es[i].texture = charger_texture_es(niveau->es[i].type, renderer);
        }
        printf("  %d elements speciaux\n", niveau->nb_es);
        
    } else if (level == 2) {
        for (int i = 0; i < 5; i++) {
            int est_volant = (i % 2 == 0) ? 0 : 1;
            Ennemi e = {
                .x = 50 + i * 150,
                .y = (est_volant) ? 320 : 380,
                .w = (est_volant) ? 100 : 130,
                .h = (est_volant) ? 100 : 130,
                .pv = (est_volant) ? 30 : 60,
                .pv_max = (est_volant) ? 30 : 60,
                .degats = (est_volant) ? 8 : 12,
                .score_valeur = 150,
                .type = est_volant,
                .trajectoire = 1,
                .vitesse = (est_volant) ? 0.4f : 0.3f,
                .direction = (i % 2 == 0) ? 1 : -1,
                .startX = 50 + i * 150,
                .startY = (est_volant) ? 320 : 380,
                .angle = i * 0.8f,
                .actif = true,
                .timer_attaque = 0,
                .etat_sante = 0,
                .current_frame = 0,
                .max_frames = MAX_WALK_FRAMES,
                .frame_timer = 0,
                .animation_speed = 8
            };
            
            if (est_volant) {
                charger_frames_animation(&e, renderer, "demon");
            } else {
                charger_frames_animation(&e, renderer, "ogre");
            }
            
            e.current_texture = e.walk_frames[0];
            niveau->ennemis[niveau->nb_ennemis++] = e;
        }
        printf("  %d ennemis (Ogres & Demons)\n", niveau->nb_ennemis);
        
        niveau->es[niveau->nb_es++] = (ElementSpecial){150, 350, 45, 45, 1, 100, true, NULL};
        niveau->es[niveau->nb_es++] = (ElementSpecial){350, 280, 45, 45, 2, 1, true, NULL};
        niveau->es[niveau->nb_es++] = (ElementSpecial){550, 320, 45, 45, 3, 50, true, NULL};
        niveau->es[niveau->nb_es++] = (ElementSpecial){700, 380, 45, 45, 1, 150, true, NULL};
        
        for (int i = 0; i < niveau->nb_es; i++) {
            niveau->es[i].texture = charger_texture_es(niveau->es[i].type, renderer);
        }
        printf("  %d elements speciaux\n", niveau->nb_es);
    }
}

SDL_Texture* get_current_texture(Ennemi* e) {
    if (e->max_frames == MAX_WALK_FRAMES) {
        if (e->current_frame < MAX_WALK_FRAMES && e->walk_frames[e->current_frame] != NULL) {
            return e->walk_frames[e->current_frame];
        }
    } else if (e->max_frames == MAX_ATTACK_FRAMES) {
        if (e->current_frame < MAX_ATTACK_FRAMES && e->attack_frames[e->current_frame] != NULL) {
            return e->attack_frames[e->current_frame];
        }
    }
    
    if (e->max_frames == MAX_WALK_FRAMES) {
        for (int i = 0; i < MAX_WALK_FRAMES; i++) {
            if (e->walk_frames[i] != NULL) return e->walk_frames[i];
        }
    } else {
        for (int i = 0; i < MAX_ATTACK_FRAMES; i++) {
            if (e->attack_frames[i] != NULL) return e->attack_frames[i];
        }
    }
    
    return NULL;
}

void afficher_npc(SDL_Renderer* renderer, Niveau* niveau) {
    for (int i = 0; i < niveau->nb_ennemis; i++) {
        Ennemi* e = &niveau->ennemis[i];
        if (!e->actif) continue;
        
        SDL_Texture* current_tex = get_current_texture(e);
        
        if (current_tex) {
            SDL_Rect destRect = {e->x, e->y, e->w, e->h};
            SDL_RendererFlip flip = (e->direction == 1) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            
            if (e->etat_sante == 2) SDL_SetTextureColorMod(current_tex, 0, 0, 0);
            else if (e->etat_sante == 1) SDL_SetTextureColorMod(current_tex, 255, 100, 100);
            else SDL_SetTextureColorMod(current_tex, 255, 255, 255);
            
            SDL_RenderCopyEx(renderer, current_tex, NULL, &destRect, 0, NULL, flip);
        } else {
            SDL_SetRenderDrawColor(renderer, 150, 0, 150, 255);
            SDL_Rect rect = {e->x, e->y, e->w, e->h};
            SDL_RenderFillRect(renderer, &rect);
        }
        
        e->frame_timer++;
        if (e->frame_timer >= e->animation_speed) {
            e->current_frame = (e->current_frame + 1) % e->max_frames;
            e->frame_timer = 0;
        }

        float ratio = (float)e->pv / e->pv_max;
        int barre_y = e->y + e->h + 5;
        SDL_Rect fond = {e->x, barre_y, e->w, 8};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(renderer, &fond);
        SDL_Rect vie = {e->x, barre_y, (int)(e->w * ratio), 8};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(renderer, &vie);
    }
    
    for (int i = 0; i < niveau->nb_es; i++) {
        ElementSpecial* es = &niveau->es[i];
        if (!es->actif) continue;
        
        if (es->texture != NULL) {
            static Uint32 last_time = 0;
            Uint32 current_time = SDL_GetTicks();
            float scale = 1.0f + sin(current_time * 0.005) * 0.1f;
            
            int w = (int)(es->w * scale);
            int h = (int)(es->h * scale);
            int x = es->x + (es->w - w) / 2;
            int y = es->y + (es->h - h) / 2;
            
            SDL_Rect destRect = {x, y, w, h};
            SDL_RenderCopy(renderer, es->texture, NULL, &destRect);
        } else {
            SDL_Rect rect = {es->x, es->y, es->w, es->h};
            if (es->type == 1) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            else if (es->type == 2) SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            else SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void deplacement_aleatoire(Ennemi* e) {
    static float temps = 0;
    temps += 0.016f;
    
    if (e->trajectoire == 0) {
        e->x += e->vitesse * e->direction;
        if (e->x > e->startX + 150 || e->x < e->startX - 150) {
            e->direction *= -1;
        }
    } else {
        e->x = e->startX + sin(temps * e->vitesse + e->angle) * 150;
        if (e->type == 1) {
            e->y = e->startY + cos(temps * 1.8f + e->angle) * 50;
        }
    }
}

int collision_bb(SDL_Rect a, SDL_Rect b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x && 
            a.y < b.y + b.h && a.y + a.h > b.y);
}

void gerer_sante(Ennemi* e, int degats) {
    e->pv -= degats;
    if (e->pv <= 0) { 
        e->etat_sante = 3; 
        e->actif = false; 
        printf("  [ENNEMI] MORT ! +%d points\n", e->score_valeur); 
    }
    else if (e->pv <= e->pv_max / 3) { 
        e->etat_sante = 2; 
        printf("  [ENNEMI] NEUTRALISE (PV: %d/%d)\n", e->pv, e->pv_max); 
    }
    else if (e->pv <= e->pv_max * 2 / 3) { 
        e->etat_sante = 1; 
        printf("  [ENNEMI] BLESSE (PV: %d/%d)\n", e->pv, e->pv_max); 
    }
    else { 
        e->etat_sante = 0; 
    }
}

void ia_deplacement(Ennemi* e, int joueurX, int joueurY) {
    deplacement_aleatoire(e);
    int distance = abs((e->x + e->w/2) - joueurX);
    
    if (distance < 80) {
        if (e->max_frames != MAX_ATTACK_FRAMES) {
            e->max_frames = MAX_ATTACK_FRAMES;
            e->current_frame = 0;
            e->frame_timer = 0;
            e->animation_speed = 6;
        }
        
        if (e->timer_attaque == 0) {
            e->timer_attaque = 30;
            printf("  [IA] Ennemi ATTAQUE !\n");
        }
    } else {
        if (e->max_frames != MAX_WALK_FRAMES) {
            e->max_frames = MAX_WALK_FRAMES;
            e->current_frame = 0;
            e->frame_timer = 0;
            e->animation_speed = 8;
        }
    }
    
    if (e->timer_attaque > 0) e->timer_attaque--;
}

void update_npc(Niveau* niveau, int joueurX, int joueurY) {
    for (int i = 0; i < niveau->nb_ennemis; i++) {
        Ennemi* e = &niveau->ennemis[i];
        if (e->actif && e->etat_sante != 3) {
            ia_deplacement(e, joueurX, joueurY);
        }
    }
}

void collecter_es(Niveau* niveau, SDL_Rect joueurRect, int* score, int* vies, int* pv) {
    for (int i = 0; i < niveau->nb_es; i++) {
        ElementSpecial* es = &niveau->es[i];
        if (!es->actif) continue;
        
        SDL_Rect esRect = {es->x, es->y, es->w, es->h};
        if (collision_bb(joueurRect, esRect)) {
            es->actif = false;
            
            if (es->type == 1) { 
                *score += es->valeur; 
                printf("[ES] +%d points ! (Score total: %d)\n", es->valeur, *score); 
            }
            else if (es->type == 2) { 
                *vies += 1; 
                printf("[ES] +1 vie ! (Vies: %d)\n", *vies); 
            }
            else if (es->type == 3) { 
                *pv += es->valeur; 
                if (*pv > 100) *pv = 100; 
                printf("[ES] +%d PV ! (PV: %d/100)\n", es->valeur, *pv); 
            }
        }
    }
}

void liberer_textures_npc(Niveau* niveau) {
    if (niveau->background) {
        SDL_DestroyTexture(niveau->background);
        niveau->background = NULL;
    }
    
    for (int i = 0; i < niveau->nb_ennemis; i++) {
        Ennemi* e = &niveau->ennemis[i];
        
        for (int j = 0; j < MAX_WALK_FRAMES; j++) {
            if (e->walk_frames[j]) {
                SDL_DestroyTexture(e->walk_frames[j]);
                e->walk_frames[j] = NULL;
            }
        }
        
        for (int j = 0; j < MAX_ATTACK_FRAMES; j++) {
            if (e->attack_frames[j]) {
                SDL_DestroyTexture(e->attack_frames[j]);
                e->attack_frames[j] = NULL;
            }
        }
    }
    
    for (int i = 0; i < niveau->nb_es; i++) {
        if (niveau->es[i].texture) {
            SDL_DestroyTexture(niveau->es[i].texture);
            niveau->es[i].texture = NULL;
        }
    }
}

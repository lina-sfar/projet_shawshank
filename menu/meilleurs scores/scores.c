#include "scores.h"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern void drawGradientBackground(SDL_Renderer *renderer);

static Mix_Music *victoryMusic = NULL;
static Mix_Chunk *hoverSound = NULL;
static Mix_Chunk *clickSound = NULL;
static SDL_Texture *background3 = NULL;

typedef struct {
    char name[50];
    int score;
} ScoreEntry;

static ScoreEntry topScores[3];
static int scoreCount = 0;

void initScoresSystem() {
    hoverSound = Mix_LoadWAV("assets/hover.wav");
    if(!hoverSound) hoverSound = Mix_LoadWAV("assets/click.wav");
    clickSound = Mix_LoadWAV("assets/click.wav");
    victoryMusic = Mix_LoadMUS("assets/victory.mp3");
    if(!victoryMusic) victoryMusic = Mix_LoadMUS("assets/music.mp3");
}

void loadTopScores(char names[3][50], int scores[3]) {
    FILE *f = fopen("scores.dat", "rb");
    if(f) {
        scoreCount = fread(topScores, sizeof(ScoreEntry), 3, f);
        fclose(f);
    }
    
    if(scoreCount == 0) {
        strcpy(topScores[0].name, "ANDY");
        topScores[0].score = 1847;
        strcpy(topScores[1].name, "RED");
        topScores[1].score = 1243;
        strcpy(topScores[2].name, "BROOKS");
        topScores[2].score = 876;
        scoreCount = 3;
    }
    
    for(int i = 0; i < 3; i++) {
        strcpy(names[i], topScores[i].name);
        scores[i] = topScores[i].score;
    }
}

void saveScore(const char *name, int score) {
    ScoreEntry newEntry;
    strncpy(newEntry.name, name, 49);
    newEntry.name[49] = '\0';
    newEntry.score = score;
    
    ScoreEntry all[4];
    for(int i = 0; i < scoreCount && i < 3; i++) all[i] = topScores[i];
    all[scoreCount] = newEntry;
    
    for(int i = 0; i < scoreCount + 1; i++) {
        for(int j = i + 1; j < scoreCount + 1; j++) {
            if(all[i].score < all[j].score) {
                ScoreEntry temp = all[i];
                all[i] = all[j];
                all[j] = temp;
            }
        }
    }
    
    for(int i = 0; i < 3 && i < scoreCount + 1; i++) topScores[i] = all[i];
    if(scoreCount < 3) scoreCount++;
    
    FILE *f = fopen("scores.dat", "wb");
    if(f) {
        fwrite(topScores, sizeof(ScoreEntry), 3, f);
        fclose(f);
    }
}

void drawCreativeText(SDL_Renderer *renderer, TTF_Font *font, const char *text, 
                      int x, int y, SDL_Color color, int shadow) {
    if(shadow) {
        SDL_Color shadowColor = {0, 0, 0, 200};
        SDL_Surface *shadowSurf = TTF_RenderText_Blended(font, text, shadowColor);
        SDL_Texture *shadowTex = SDL_CreateTextureFromSurface(renderer, shadowSurf);
        SDL_Rect shadowRect = {x + 2, y + 2, shadowSurf->w, shadowSurf->h};
        SDL_RenderCopy(renderer, shadowTex, NULL, &shadowRect);
        SDL_FreeSurface(shadowSurf);
        SDL_DestroyTexture(shadowTex);
    }
    
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect dest = {x, y, surf->w, surf->h};
    SDL_RenderCopy(renderer, tex, NULL, &dest);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void drawGoldBorder(SDL_Renderer *renderer, SDL_Rect *rect) {
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, rect);
    SDL_Rect inner = {rect->x + 2, rect->y + 2, rect->w - 4, rect->h - 4};
    SDL_SetRenderDrawColor(renderer, 255, 200, 50, 150);
    SDL_RenderDrawRect(renderer, &inner);
}

int showScoresMenu(SDL_Renderer *renderer, TTF_Font *font) {
    if(!background3) {
        SDL_Surface *bg = IMG_Load("assets/bg3.png");
        if(!bg) bg = SDL_LoadBMP("assets/bg3.bmp");
        if(bg) {
            background3 = SDL_CreateTextureFromSurface(renderer, bg);
            SDL_FreeSurface(bg);
        }
    }
    
    char playerName[50] = "";
    int nameLen = 0;
    int inputActive = 1;
    int showScoresList = 0;
    int finalScore = 1000;
    
    char topNames[3][50];
    int topScoresList[3];
    loadTopScores(topNames, topScoresList);
    
    SDL_Rect validateRect = {300, 350, 200, 50};
    SDL_Rect returnRect = {300, 500, 200, 50};
    int hoverValidate = 0, hoverReturn = 0;
    SDL_Event event;
    int running = 1, result = 1, victoryPlayed = 0;
    
    // Activer la saisie clavier
    SDL_StartTextInput();
    
    while(running) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) { 
                running = 0; 
                result = 0; 
            }
            
            if(event.type == SDL_MOUSEMOTION) {
                int x = event.motion.x, y = event.motion.y;
                int oldHoverV = hoverValidate, oldHoverR = hoverReturn;
                hoverValidate = (x >= validateRect.x && x <= validateRect.x + validateRect.w &&
                                y >= validateRect.y && y <= validateRect.y + validateRect.h);
                hoverReturn = (x >= returnRect.x && x <= returnRect.x + returnRect.w &&
                              y >= returnRect.y && y <= returnRect.y + returnRect.h);
                if((hoverValidate && !oldHoverV) || (hoverReturn && !oldHoverR)) {
                    if(hoverSound) Mix_PlayChannel(-1, hoverSound, 0);
                }
            }
            
            // Gestion du texte (meilleure méthode)
            if(event.type == SDL_TEXTINPUT && inputActive && !showScoresList) {
                const char *text = event.text.text;
                int len = strlen(text);
                if(nameLen + len < 49) {
                    strcat(playerName, text);
                    nameLen += len;
                }
                printf("Texte saisi: %s, Nom: %s\n", text, playerName);
            }
            
            // Gestion des touches spéciales
            if(event.type == SDL_KEYDOWN && inputActive && !showScoresList) {
                int key = event.key.keysym.sym;
                
                if(key == SDLK_RETURN && nameLen > 0) {
                    saveScore(playerName, finalScore);
                    loadTopScores(topNames, topScoresList);
                    showScoresList = 1;
                    inputActive = 0;
                    if(victoryMusic && !victoryPlayed) { 
                        Mix_PlayMusic(victoryMusic, 1); 
                        victoryPlayed = 1; 
                    }
                }
                else if(key == SDLK_BACKSPACE && nameLen > 0) {
                    nameLen--;
                    playerName[nameLen] = '\0';
                    printf("Backspace, Nom: %s\n", playerName);
                }
                else if(key == SDLK_ESCAPE) { 
                    running = 0; 
                    result = 0; 
                }
                else if(key == SDLK_e) { 
                    running = 0; 
                    result = 2; 
                }
            }
            
            if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x, y = event.button.y;
                
                if(x >= validateRect.x && x <= validateRect.x + validateRect.w &&
                   y >= validateRect.y && y <= validateRect.y + validateRect.h) {
                    if(inputActive && !showScoresList && nameLen > 0) {
                        saveScore(playerName, finalScore);
                        loadTopScores(topNames, topScoresList);
                        showScoresList = 1;
                        inputActive = 0;
                        if(victoryMusic && !victoryPlayed) { 
                            Mix_PlayMusic(victoryMusic, 1); 
                            victoryPlayed = 1; 
                        }
                        if(clickSound) Mix_PlayChannel(-1, clickSound, 0);
                    }
                }
                else if(x >= returnRect.x && x <= returnRect.x + returnRect.w &&
                        y >= returnRect.y && y <= returnRect.y + returnRect.h) {
                    if(clickSound) Mix_PlayChannel(-1, clickSound, 0);
                    if(Mix_PlayingMusic()) Mix_HaltMusic();
                    running = 0; 
                    result = 1;
                }
            }
        }
        
        // RENDU
        if(background3) {
            SDL_RenderCopy(renderer, background3, NULL, NULL);
        } else {
            drawGradientBackground(renderer);
        }
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 120);
        SDL_Rect fullRect = {0, 0, 800, 600};
        SDL_RenderFillRect(renderer, &fullRect);
        
        SDL_Color goldColor = {255, 215, 0, 255};
        SDL_Color whiteColor = {255, 255, 255, 255};
        SDL_Color cyanColor = {0, 255, 255, 255};
        
        drawCreativeText(renderer, font, "SHAWSHANK SCORES", 250, 40, goldColor, 1);
        drawCreativeText(renderer, font, "Entrez votre pseudo:", 280, 100, cyanColor, 0);
        
        if(!showScoresList) {
            // Zone d'affichage du pseudo
            SDL_Rect nameBgRect = {250, 180, 300, 50};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_RenderFillRect(renderer, &nameBgRect);
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
            SDL_RenderDrawRect(renderer, &nameBgRect);
            
            // Afficher le pseudo ou "_____" si vide
            char displayName[60];
            if(nameLen == 0) {
                strcpy(displayName, "_____");
            } else {
                strcpy(displayName, playerName);
            }
            
            // Curseur clignotant
            static int frame = 0;
            frame++;
            if(inputActive && (frame / 30) % 2 == 0 && nameLen < 49) {
                char temp[60];
                strcpy(temp, displayName);
                strcat(temp, "_");
                drawCreativeText(renderer, font, temp, 270, 190, cyanColor, 0);
            } else {
                drawCreativeText(renderer, font, displayName, 270, 190, cyanColor, 0);
            }
            if(frame > 60) frame = 0;
            
            // Afficher le score
            char scoreText[50];
            snprintf(scoreText, sizeof(scoreText), "Votre score: %d points", finalScore);
            drawCreativeText(renderer, font, scoreText, 280, 270, goldColor, 1);
            
            // Bouton Valider
            SDL_Color btnColor = hoverValidate ? goldColor : whiteColor;
            if(hoverValidate) {
                SDL_SetRenderDrawColor(renderer, 80, 60, 20, 200);
                SDL_RenderFillRect(renderer, &validateRect);
            } else {
                SDL_SetRenderDrawColor(renderer, 40, 30, 10, 200);
                SDL_RenderFillRect(renderer, &validateRect);
            }
            drawGoldBorder(renderer, &validateRect);
            drawCreativeText(renderer, font, "VALIDER", 360, 362, btnColor, 0);
        }
        
        if(showScoresList) {
            drawCreativeText(renderer, font, "=== TOP 3 DES EVADES ===", 250, 170, goldColor, 1);
            int yPos = 230;
            for(int i = 0; i < 3; i++) {
                char entry[100];
                SDL_Color rankColor;
                if(i == 0) rankColor = goldColor;
                else if(i == 1) rankColor = (SDL_Color){192, 192, 192, 255};
                else rankColor = (SDL_Color){205, 127, 50, 255};
                snprintf(entry, sizeof(entry), "%d.  %s  -  %d points", i+1, topNames[i], topScoresList[i]);
                drawCreativeText(renderer, font, entry, 250, yPos, rankColor, 1);
                yPos += 50;
            }
        }
        
        // Bouton Retour
        SDL_Color returnColor = hoverReturn ? goldColor : whiteColor;
        if(hoverReturn) {
            SDL_SetRenderDrawColor(renderer, 60, 20, 20, 200);
            SDL_RenderFillRect(renderer, &returnRect);
        } else {
            SDL_SetRenderDrawColor(renderer, 30, 10, 10, 200);
            SDL_RenderFillRect(renderer, &returnRect);
        }
        drawGoldBorder(renderer, &returnRect);
        drawCreativeText(renderer, font, "RETOUR", 365, 512, returnColor, 0);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_StopTextInput();
    return result;
}

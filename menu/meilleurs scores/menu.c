#include "menu.h"
#include "scores.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <string.h>

typedef struct {
    SDL_Rect rect;
    char text[50];
} Button;

static TTF_Font *font = NULL;
static Mix_Music *music = NULL;
static Mix_Chunk *click = NULL;
static Mix_Chunk *hover = NULL;
static SDL_Texture *backgroundTex = NULL;

void drawGradientBackground(SDL_Renderer *renderer) {
    for(int i = 0; i < 600; i++) {
        int r = 30 + (i * 20 / 600);
        int g = 20 + (i * 15 / 600);
        int b = 15 + (i * 10 / 600);
        if(r > 60) r = 60;
        if(g > 50) g = 50;
        if(b > 40) b = 40;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, i, 800, i);
    }
}

void drawText(SDL_Renderer *renderer, TTF_Font *font,
              const char *text, int x, int y, SDL_Color color, int shadow)
{
    if(shadow) {
        SDL_Color shadowColor = {0, 0, 0, 180};
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

int showMainMenu(SDL_Renderer *renderer)
{
    font = TTF_OpenFont("assets/font.ttf", 28);  // Police plus petite
    if(!font) font = TTF_OpenFont("assets/arial.ttf", 28);
    if(!font) font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 28);
    
    music = Mix_LoadMUS("assets/music.mp3");
    click = Mix_LoadWAV("assets/click.wav");
    hover = Mix_LoadWAV("assets/hover.wav");
    if(!hover) hover = Mix_LoadWAV("assets/click.wav");
    
    SDL_Surface *bg = IMG_Load("assets/bg_menu.png");
    if(!bg) bg = SDL_LoadBMP("assets/bg_menu.bmp");
    if(bg) {
        backgroundTex = SDL_CreateTextureFromSurface(renderer, bg);
        SDL_FreeSurface(bg);
    }
    
    Mix_PlayMusic(music, -1);
    
    // Boutons plus larges pour "Meilleurs Scores"
    Button play = {{280, 200, 240, 55}, "Jouer"};
    Button scores = {{250, 275, 300, 55}, "Meilleurs Scores"};  // Plus large
    Button quit = {{280, 350, 240, 55}, "Quitter"};
    
    SDL_Event event;
    int running = 1;
    int result = 0;
    int hoverPlay = 0, hoverScores = 0, hoverQuit = 0;
    
    initScoresSystem();
    
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                running = 0;
                result = 0;
            }
            
            if (event.type == SDL_MOUSEMOTION) {
                int x = event.motion.x, y = event.motion.y;
                int oldHoverPlay = hoverPlay;
                int oldHoverScores = hoverScores;
                int oldHoverQuit = hoverQuit;
                
                hoverPlay = (x >= play.rect.x && x <= play.rect.x + play.rect.w &&
                            y >= play.rect.y && y <= play.rect.y + play.rect.h);
                hoverScores = (x >= scores.rect.x && x <= scores.rect.x + scores.rect.w &&
                              y >= scores.rect.y && y <= scores.rect.y + scores.rect.h);
                hoverQuit = (x >= quit.rect.x && x <= quit.rect.x + quit.rect.w &&
                            y >= quit.rect.y && y <= quit.rect.y + quit.rect.h);
                
                if((hoverPlay && !oldHoverPlay) || (hoverScores && !oldHoverScores) || (hoverQuit && !oldHoverQuit)) {
                    if(hover) Mix_PlayChannel(-1, hover, 0);
                }
            }
            
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = event.button.x;
                int y = event.button.y;
                
                if (x >= play.rect.x && x <= play.rect.x + play.rect.w &&
                    y >= play.rect.y && y <= play.rect.y + play.rect.h)
                {
                    if(click) Mix_PlayChannel(-1, click, 0);
                    running = 0;
                    result = 1;
                }
                
                if (x >= scores.rect.x && x <= scores.rect.x + scores.rect.w &&
                    y >= scores.rect.y && y <= scores.rect.y + scores.rect.h)
                {
                    if(click) Mix_PlayChannel(-1, click, 0);
                    int scoresResult = showScoresMenu(renderer, font);
                    if(scoresResult == 0) {
                        running = 0;
                        result = 0;
                    }
                }
                
                if (x >= quit.rect.x && x <= quit.rect.x + quit.rect.w &&
                    y >= quit.rect.y && y <= quit.rect.y + quit.rect.h)
                {
                    if(click) Mix_PlayChannel(-1, click, 0);
                    running = 0;
                    result = 0;
                }
            }
            
            if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
                result = 0;
            }
        }
        
        if(backgroundTex) {
            SDL_RenderCopy(renderer, backgroundTex, NULL, NULL);
        } else {
            drawGradientBackground(renderer);
        }
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
        SDL_Rect fullRect = {0, 0, 800, 600};
        SDL_RenderFillRect(renderer, &fullRect);
        
        SDL_Color goldColor = {255, 215, 0, 255};
        SDL_Color whiteColor = {255, 255, 255, 255};
        SDL_Color hoverColor = {255, 200, 50, 255};
        
        drawText(renderer, font, "SHAWSHANK", 340, 80, goldColor, 1);
        drawText(renderer, font, "GAME", 380, 125, goldColor, 1);
        
        SDL_Color playColor = hoverPlay ? hoverColor : whiteColor;
        if(hoverPlay) {
            SDL_SetRenderDrawColor(renderer, 80, 60, 20, 200);
            SDL_RenderFillRect(renderer, &play.rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 30, 10, 180);
            SDL_RenderFillRect(renderer, &play.rect);
        }
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawRect(renderer, &play.rect);
        drawText(renderer, font, play.text, 375, 212, playColor, 0);
        
        SDL_Color scoresColor = hoverScores ? hoverColor : whiteColor;
        if(hoverScores) {
            SDL_SetRenderDrawColor(renderer, 80, 60, 20, 200);
            SDL_RenderFillRect(renderer, &scores.rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 30, 10, 180);
            SDL_RenderFillRect(renderer, &scores.rect);
        }
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawRect(renderer, &scores.rect);
        drawText(renderer, font, scores.text, 325, 287, scoresColor, 0);
        
        SDL_Color quitColor = hoverQuit ? hoverColor : whiteColor;
        if(hoverQuit) {
            SDL_SetRenderDrawColor(renderer, 80, 60, 20, 200);
            SDL_RenderFillRect(renderer, &quit.rect);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 30, 10, 180);
            SDL_RenderFillRect(renderer, &quit.rect);
        }
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawRect(renderer, &quit.rect);
        drawText(renderer, font, quit.text, 375, 362, quitColor, 0);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    if(font) TTF_CloseFont(font);
    if(music) Mix_FreeMusic(music);
    if(click) Mix_FreeChunk(click);
    if(hover) Mix_FreeChunk(hover);
    if(backgroundTex) SDL_DestroyTexture(backgroundTex);
    
    return result;
}

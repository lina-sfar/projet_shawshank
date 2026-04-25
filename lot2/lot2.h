#ifndef LOT2_H
#define LOT2_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define LEVEL_WIDTH 2000
#define LEVEL_HEIGHT 2000


typedef enum {
    PLATFORM_FIXE,
    PLATFORM_MOBILE_H,
    PLATFORM_MOBILE_V,
    PLATFORM_DESTRUCTIBLE
} PlatformType;

typedef struct {
    SDL_Rect rect;
    PlatformType type;
    bool active;
    int health;
    float startX, startY;
    float moveRange;
    float speed;
    float offset;
} Plateforme;

typedef struct {
    int x, y;
    int levelWidth, levelHeight;
} Camera;


typedef struct {
    Uint32 startTime;
    Uint32 currentTime;
    int secondes;
    int minutes;
} GameTimer;

typedef struct {
    SDL_Rect rect;
    char titre[50];
    char contenu[200];
    bool visible;
    SDL_Texture* texture;
} GuideWindow;


typedef struct {
    Plateforme plateformes[50];
    int nbPlateformes;
    Camera camera;
    GameTimer timer;
    GuideWindow guides[5];
    int nbGuides;
    int mode_ecran; 
} Background;

void lot2();
void initialiser_background(Background* bg, int level);
void afficher_plateformes(SDL_Renderer* renderer, Background* bg);
void scrolling(Background* bg, int dx, int dy);
void partage_ecran(Background* bg, int mode);
void afficher_temps(SDL_Renderer* renderer, TTF_Font* font, GameTimer* timer, int x, int y);
void init_guide(GuideWindow* guide, int x, int y, int w, int h, const char* titre, const char* contenu);
void afficher_guide(SDL_Renderer* renderer, GuideWindow* guide);

void mettre_a_jour_plateformes(Background* bg, float dt);
void update_timer(GameTimer* timer);

#endif


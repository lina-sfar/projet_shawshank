#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Button positions
#define QUIZ_BTN_X 300
#define QUIZ_BTN_Y 250
#define PUZZLE_BTN_X 300
#define PUZZLE_BTN_Y 320
#define ANSWER_A_X 250
#define ANSWER_B_X 375
#define ANSWER_C_X 500
#define ANSWER_Y 400
#define QUESTION_X 250
#define QUESTION_Y 300
#define QUESTION_WIDTH 300
#define QUESTION_HEIGHT 60

// Game states
typedef enum {
    STATE_SUBMENU,
    STATE_QUIZ,
    STATE_PUZZLE
} GameState;

// Button structure - INCREASED buffer size from 50 to 100
typedef struct {
    int x, y, w, h;
    int selected;
    char text[100];  // Increased from 50 to 100
    int visible;
} Button;

// Global variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
GameState gameState = STATE_SUBMENU;
Button quizButton, puzzleButton;
Button answerA, answerB, answerC;
SDL_Texture* background = NULL;
int mouseX, mouseY;

// Sound variables
Mix_Music* suspenseMusic = NULL;
Mix_Chunk* hoverSound = NULL;
int isMusicPlaying = 0;

// Quiz variables - INCREASED buffer sizes
char currentQuestion[200] = "What is the capital of France?";  // Increased from 100 to 200
char answerTextA[100] = "London";  // Increased from 50 to 100
char answerTextB[100] = "Paris";   // Increased from 50 to 100
char answerTextC[100] = "Berlin";  // Increased from 50 to 100

// Function prototypes
int initSDL();
void initButtons();
SDL_Texture* loadTexture(const char* path);
SDL_Texture* renderText(const char* text, SDL_Color color);
void handleEvents(int* running);
void update();
void render();
void cleanup();
void drawButton(Button* btn);
void drawText(const char* text, int x, int y, SDL_Color color);
int isMouseOverButton(Button* btn, int x, int y);
void startQuiz();
void returnToMainMenu();

int main(int argc, char* argv[]) {
    if (!initSDL()) {
        printf("Failed to initialize SDL\n");
        return 1;
    }
    
    initButtons();
    
    int running = 1;
    while (running) {
        handleEvents(&running);
        update();
        render();
        SDL_Delay(16);
    }
    
    cleanup();
    return 0;
}

int initSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    // Create window
    window = SDL_CreateWindow("Game Menu", 
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, 
                              SCREEN_HEIGHT, 
                              SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return 0;
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 0;
    }
    
    // Load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-Regular.ttf", 24);
    if (!font) {
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
        if (!font) {
            printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
            // Continue without font - we'll use a fallback
        }
    }
    
    // Load background
    background = loadTexture("background.jpg");
    if (!background) {
        printf("Using default background color\n");
    }
    
    // Load sounds
    suspenseMusic = Mix_LoadMUS("suspense.wav");
    if (!suspenseMusic) {
        printf("Warning: Could not load suspense.wav\n");
    }
    
    hoverSound = Mix_LoadWAV("hover.wav");
    if (!hoverSound) {
        printf("Warning: Could not load hover.wav\n");
    }
    
    return 1;
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);
    
    if (loadedSurface) {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }
    
    return newTexture;
}

SDL_Texture* renderText(const char* text, SDL_Color color) {
    if (!font || !text) return NULL;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return NULL;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

void initButtons() {
    // Quiz button
    quizButton.x = QUIZ_BTN_X;
    quizButton.y = QUIZ_BTN_Y;
    quizButton.w = 150;
    quizButton.h = 50;
    quizButton.selected = 0;
    quizButton.visible = 1;
    strcpy(quizButton.text, "QUIZ");
    
    // Puzzle button
    puzzleButton.x = PUZZLE_BTN_X;
    puzzleButton.y = PUZZLE_BTN_Y;
    puzzleButton.w = 150;
    puzzleButton.h = 50;
    puzzleButton.selected = 0;
    puzzleButton.visible = 1;
    strcpy(puzzleButton.text, "PUZZLE");
    
    // Answer buttons
    answerA.x = ANSWER_A_X;
    answerA.y = ANSWER_Y;
    answerA.w = 100;
    answerA.h = 40;
    answerA.selected = 0;
    answerA.visible = 0;
    strcpy(answerA.text, "A");
    
    answerB.x = ANSWER_B_X;
    answerB.y = ANSWER_Y;
    answerB.w = 100;
    answerB.h = 40;
    answerB.selected = 0;
    answerB.visible = 0;
    strcpy(answerB.text, "B");
    
    answerC.x = ANSWER_C_X;
    answerC.y = ANSWER_Y;
    answerC.w = 100;
    answerC.h = 40;
    answerC.selected = 0;
    answerC.visible = 0;
    strcpy(answerC.text, "C");
}

int isMouseOverButton(Button* btn, int x, int y) {
    if (!btn->visible) return 0;
    return (x >= btn->x && x <= btn->x + btn->w &&
            y >= btn->y && y <= btn->y + btn->h);
}

void handleEvents(int* running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *running = 0;
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    if (gameState == STATE_QUIZ || gameState == STATE_PUZZLE) {
                        returnToMainMenu();
                    } else {
                        *running = 0;
                    }
                }
                break;
                
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (gameState == STATE_SUBMENU) {
                        if (quizButton.selected) {
                            startQuiz();
                        } else if (puzzleButton.selected) {
                            gameState = STATE_PUZZLE;
                            quizButton.visible = 0;
                            puzzleButton.visible = 0;
                        }
                    } else if (gameState == STATE_QUIZ) {
                        if (answerA.selected) {
                            printf("Selected answer A: %s\n", answerTextA);
                        } else if (answerB.selected) {
                            printf("Selected answer B: %s (Correct!)\n", answerTextB);
                        } else if (answerC.selected) {
                            printf("Selected answer C: %s\n", answerTextC);
                        }
                    }
                }
                break;
        }
    }
}

void update() {
    // Update button selection states
    int oldSelected;
    
    if (gameState == STATE_SUBMENU) {
        oldSelected = quizButton.selected;
        quizButton.selected = isMouseOverButton(&quizButton, mouseX, mouseY);
        if (quizButton.selected != oldSelected && quizButton.selected && hoverSound) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }
        
        oldSelected = puzzleButton.selected;
        puzzleButton.selected = isMouseOverButton(&puzzleButton, mouseX, mouseY);
        if (puzzleButton.selected != oldSelected && puzzleButton.selected && hoverSound) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }
    }
    
    if (gameState == STATE_QUIZ) {
        oldSelected = answerA.selected;
        answerA.selected = isMouseOverButton(&answerA, mouseX, mouseY);
        if (answerA.selected != oldSelected && answerA.selected && hoverSound) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }
        
        oldSelected = answerB.selected;
        answerB.selected = isMouseOverButton(&answerB, mouseX, mouseY);
        if (answerB.selected != oldSelected && answerB.selected && hoverSound) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }
        
        oldSelected = answerC.selected;
        answerC.selected = isMouseOverButton(&answerC, mouseX, mouseY);
        if (answerC.selected != oldSelected && answerC.selected && hoverSound) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }
    }
}

void drawButton(Button* btn) {
    if (!btn->visible) return;
    
    SDL_Rect rect = {btn->x, btn->y, btn->w, btn->h};
    
    // Draw button background
    if (btn->selected) {
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255); // Green for selected
    } else {
        SDL_SetRenderDrawColor(renderer, 70, 70, 180, 255); // Blue for normal
    }
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw border
    if (btn->selected) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow border for selected
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    }
    SDL_RenderDrawRect(renderer, &rect);
    
    // Draw text
    SDL_Color white = {255, 255, 255, 255};
    SDL_Texture* textTexture = renderText(btn->text, white);
    if (textTexture) {
        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {
            btn->x + (btn->w - textW) / 2,
            btn->y + (btn->h - textH) / 2,
            textW, textH
        };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);
    } else {
        // Fallback if font fails
        SDL_Color color = {255, 255, 255, 255};
        // Simple text rendering fallback
        printf("Text rendering failed for: %s\n", btn->text);
    }
}

void drawText(const char* text, int x, int y, SDL_Color color) {
    if (!text) return;
    
    SDL_Texture* textTexture = renderText(text, color);
    if (textTexture) {
        int textW, textH;
        SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);
        SDL_Rect textRect = {x, y, textW, textH};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);
    }
}

void drawQuestionLabel() {
    // Draw label background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 230);
    SDL_Rect labelRect = {QUESTION_X, QUESTION_Y, QUESTION_WIDTH, QUESTION_HEIGHT};
    SDL_RenderFillRect(renderer, &labelRect);
    
    // Draw border
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &labelRect);
    
    // Draw question text
    SDL_Color white = {255, 255, 255, 255};
    drawText(currentQuestion, QUESTION_X + 10, QUESTION_Y + 20, white);
}

void render() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderClear(renderer);
    
    // Draw background if available
    if (background) {
        SDL_RenderCopy(renderer, background, NULL, NULL);
    }
    
    // Draw based on game state
    switch (gameState) {
        case STATE_SUBMENU:
            drawButton(&quizButton);
            drawButton(&puzzleButton);
            drawText("ENIGMA MENU", 350, 150, (SDL_Color){255, 255, 255, 255});
            break;
            
        case STATE_QUIZ:
            drawQuestionLabel();
            drawButton(&answerA);
            drawButton(&answerB);
            drawButton(&answerC);
            drawText("QUIZ", 370, 200, (SDL_Color){255, 255, 255, 255});
            break;
            
        case STATE_PUZZLE:
            drawText("PUZZLE MODE", 350, 300, (SDL_Color){255, 255, 255, 255});
            drawText("Press ESC to return", 330, 350, (SDL_Color){255, 255, 255, 255});
            break;
    }
    
    SDL_RenderPresent(renderer);
}

void startQuiz() {
    gameState = STATE_QUIZ;
    quizButton.visible = 0;
    puzzleButton.visible = 0;
    answerA.visible = 1;
    answerB.visible = 1;
    answerC.visible = 1;
    
    // Update answer texts - now safe with larger buffers
    snprintf(answerA.text, sizeof(answerA.text), "A: %s", answerTextA);
    snprintf(answerB.text, sizeof(answerB.text), "B: %s", answerTextB);
    snprintf(answerC.text, sizeof(answerC.text), "C: %s", answerTextC);
    
    // Play music
    if (suspenseMusic && !isMusicPlaying) {
        Mix_PlayMusic(suspenseMusic, -1);
        isMusicPlaying = 1;
    }
}

void returnToMainMenu() {
    gameState = STATE_SUBMENU;
    quizButton.visible = 1;
    puzzleButton.visible = 1;
    answerA.visible = 0;
    answerB.visible = 0;
    answerC.visible = 0;
    
    // Restore original button texts
    strcpy(answerA.text, "A");
    strcpy(answerB.text, "B");
    strcpy(answerC.text, "C");
    
    // Stop music
    if (isMusicPlaying) {
        Mix_HaltMusic();
        isMusicPlaying = 0;
    }
}

void cleanup() {
    if (background) SDL_DestroyTexture(background);
    if (suspenseMusic) Mix_FreeMusic(suspenseMusic);
    if (hoverSound) Mix_FreeChunk(hoverSound);
    if (font) TTF_CloseFont(font);
    
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

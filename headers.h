#ifndef HEADERS_H
#define HEADERS_H

#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC     2
#define PLATFORM_UNIX    3

#if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_ttf.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define DEFAULT_TRANSITION_DURATION 15

// Estrutura que carrega os elementos globais do programa
typedef struct GameInfo {
    // Janela global
    SDL_Window* window;
    // Superfície global
    SDL_Surface* screenSurface;
    // Renderizador global
    SDL_Renderer* renderer;
    // Fonte principal
    TTF_Font* mainFont;
} GameInfo;

typedef struct SceneManager SceneManager;

extern GameInfo gInfo;
extern SceneManager sMng;

#endif
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
    #include <SDL_mixer.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_TRANSITION_DURATION 10

// Estrutura que carrega os elementos globais do programa
typedef struct GameInfo {
    // Janela global
    SDL_Window* window;
    // Superfície global
    SDL_Surface* screenSurface;
    // Renderizador global
    SDL_Renderer* renderer;
    // Comprimento da tela
    int screenWidth;
    // Altura da tela
    int screenHeight;
    // Controle da resolução e escalonamento
    double screenMulti;
    // Fonte principal
    TTF_Font* mainFont;
    // Fonte MainMenu
    TTF_Font* menuFont;
    // Fonte de entrada de texto
    TTF_Font* inputFont;
} GameInfo;

typedef struct SceneManager SceneManager;

extern GameInfo gInfo;
extern SceneManager sMng;

#endif
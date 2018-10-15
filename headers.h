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
#include <math.h>

// Comprimento de referência da tela
#define REFERENCE_WIDTH 1440
// Altura de referência da tela
#define REFERENCE_HEIGHT 1080

// Duração padrao da transiçao (em frames)
#define DEFAULT_TRANSITION_DURATION 10
// Número de camadas do mapa
#define MAP_LAYERS 2
// Tamanho (em pixels) de cada tile (quadrado)
#define TILE_SIZE 64
// Comprimento da imagem do Tileset
#define TILESET_WIDTH 256
// Tile que representa a parede
#define WALL_TILE 1
// Número de jogadores padrão
#define MAX_PLAYERS 4
// Quantidade de partes que o os tiles serão divididos para movimentação dos personagens
#define MOVEMENT_PARTS 4
// Porta padrão do servidor
#define SERVER_DEFAULT_PORT 7567
// Tickrate do servidor (por segundo)
#define SERVER_TICKRATE 120
// Número máximo de bombas por jogador
#define MAX_BOMBS_PER_PLAYER 5

typedef struct Address Address;
typedef struct Map Map;

// Estrutura que carrega os elementos globais do programa
typedef struct GameInfo {
    // Janela global
    SDL_Window* window;
    // Superfície global
    SDL_Surface* screenSurface;
    // Renderizador global
    SDL_Renderer* renderer;
    // Textura da tela
    SDL_Texture* screenTexture;
    // Comprimento da tela
    int screenWidth;
    // Altura da tela
    int screenHeight;
    // Taxa de atualização da tela
    int screenFreq;
    // Controle da resolução e escalonamento
    double screenMulti;
    // Fonte principal
    TTF_Font* mainFont;
    // Fonte telaLogin
    TTF_Font* telaLogin;
    // Fonte de entrada de texto
    TTF_Font* inputFont;
    // Fonte mainmenu
    TTF_Font* mainMenu;
    // Fonte mainmenu - botoes
    TTF_Font* mainMenu_botoes;
    // Fontes servers
    TTF_Font* serversFonte;
    TTF_Font* serversName;
    TTF_Font* serversFontd;
    // Modo debug
    bool debug;
    char nome[50];
    char loginID[50];
    int rankPos;

    TTF_Font* rank;

    Map* map;
} GameInfo;

typedef struct Server Server;

typedef struct NetworkS {
    int sockFd;
    bool connectedToServer;
    int clientId;
    bool serverHost;
    Address* serverAddress;
    Server* server;
    SDL_Thread* serverThread;
    uint64_t lastMovementId;
} NetworkS;

typedef struct SceneManagerS SceneManagerS;

extern GameInfo Game;
extern SceneManagerS SceneManager;
extern NetworkS Network;

#endif
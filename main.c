#include "headers.h"
#include "scene_base.h"

// Tentar inicializar a biblioteca SDL e suas funcionalidades
bool initialize(GameInfo* g) {
    bool success = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Falha ao inicializar o SDL! Erro: %s\n", SDL_GetError());
        success = false;
    } else {
        g->window = SDL_CreateWindow("Projeto UEL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(g->window == NULL) {
            printf("Falha ao criar a janela! Erro: %s\n", SDL_GetError());
            success = false;
        } else {
            //SDL_SetWindowFullscreen(g->window, SDL_WINDOW_FULLSCREEN);
            g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(g->renderer == NULL) {
                printf("Falha ao criar o renderer! Erro: %s\n", SDL_GetError());
                success = false;
            } else {
                SDL_SetRenderDrawColor(g->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags)) {
                    printf("Falha ao inicializar o SDL_image! Erro: %s\n", IMG_GetError());
                    success = false;
                }
                if(TTF_Init() == -1) {
                    printf("Falha ao inicializar o SDL_ttf! Erro: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }
    return success;
}

// Destruir os elementos do SDL
void destroy(GameInfo* g) {
    SDL_DestroyRenderer(g->renderer);
    g->renderer = NULL;
    SDL_DestroyWindow(g->window);
    g->window = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main() {
    GameInfo g;
    SceneManager s;
    if(!initialize(&g)) {
        printf("Falha ao inicializar!\n");
    } else {
        bool quit = false;
        g.mainFont = TTF_OpenFont("content/Fipps-Regular.ttf", 18);
        s.currentScene = SCENE_MAINMENU;
        s.sMainMenu = SceneMainMenu_new(&g);
        SDL_Event e;
        while(!quit) {
            while(SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_QUIT) {
                    quit = true;
                } else if(e.type == SDL_KEYDOWN) {
                    if(e.key.keysym.sym == SDLK_RETURN) {
                        SceneManager_performTransition(&s, 30, SCENE_MAINMENU);
                    }
                }
            }
            SDL_SetRenderDrawColor(g.renderer, 0x12, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(g.renderer);
            SceneManager_updateScene(&s, &g);
            SDL_RenderPresent(g.renderer);
        }
        TTF_CloseFont(g.mainFont);
        g.mainFont = NULL;
    }
    destroy(&g);
    return 0;
}
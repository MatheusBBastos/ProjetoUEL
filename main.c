#include "headers.h"

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
    if(!initialize(&g)) {
        printf("Falha ao inicializar!\n");
    } else {
        bool quit = false;
        SDL_Event e;
        TTF_Font* font = TTF_OpenFont("content/Fipps-Regular.ttf", 18);
        SDL_Color color = {0, 0, 255};
        SDL_Color color2 = {255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderUTF8_Shaded(font, "eae peçual", color, color2);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(g.renderer, textSurface);
        int w = textSurface->w, h = textSurface->h;
        SDL_FreeSurface(textSurface);
        while(!quit) {
            while(SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_QUIT) {
                    quit = true;
                }
            }
            SDL_SetRenderDrawColor(g.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(g.renderer);
            SDL_Rect renderQuad = {(SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2, w, h};
            SDL_RenderCopy(g.renderer, texture, NULL, &renderQuad);
            SDL_RenderPresent(g.renderer);
        }
        TTF_CloseFont(font);
        font = NULL;
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    destroy(&g);
    return 0;
}
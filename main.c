#include "headers.h"
#include "scene_base.h"
#include "network.h"
#include <locale.h>

GameInfo Game;
SceneManagerS SceneManager;
NetworkS Network;

// Tentar inicializar a biblioteca SDL e suas funcionalidades
bool initialize() {
    bool success = true;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Falha ao inicializar o SDL! Erro: %s\n", SDL_GetError());
        success = false;
    } else {
        Game.debug = false;
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
        SDL_DisplayMode current;
        setlocale(LC_ALL, "Portuguese");
        SDL_GetCurrentDisplayMode(0, &current);
        Game.screenFreq = current.refresh_rate;
        if (current.w >= 1920) {
            Game.screenMulti = 0.8;
        }
        else {
            Game.screenMulti = 0.5;
        }
        Game.screenWidth = REFERENCE_WIDTH * Game.screenMulti;
        Game.screenHeight = REFERENCE_HEIGHT * Game.screenMulti;
        Game.window = SDL_CreateWindow("Projeto UEL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Game.screenWidth, Game.screenHeight, SDL_WINDOW_SHOWN);
        SDL_Surface *icon = IMG_Load("content/icon.png");
        SDL_SetWindowIcon(Game.window, icon);
        if(Game.window == NULL) {
            printf("Falha ao criar a janela! Erro: %s\n", SDL_GetError());
            success = false;
        } else {
            //SDL_SetWindowFullscreen(Game.window, SDL_WINDOW_FULLSCREEN);
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
            Game.renderer = SDL_CreateRenderer(Game.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(Game.renderer == NULL) {
                printf("Falha ao criar o renderer! Erro: %s\n", SDL_GetError());
                success = false;
            } else {
                SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                Game.screenTexture = SDL_CreateTexture(Game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, REFERENCE_WIDTH, REFERENCE_HEIGHT);
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags)) {
                    printf("Falha ao inicializar o SDL_image! Erro: %s\n", IMG_GetError());
                    success = false;
                }
                if(TTF_Init() == -1) {
                    printf("Falha ao inicializar o SDL_ttf! Erro: %s\n", TTF_GetError());
                    success = false;
                }
                if(!Network_InitSockets()) {
                    printf("Falha ao inicializar os módulos de conexão!\n");
                    success = false;
                }
            }
        }
    }
    return success;
}

// Destruir os elementos do SDL
void destroy() {
    if(Network.connectedToServer) {
        char data[] = "DCS";
        Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
        Network.connectedToServer = false;
    }
    if(Network.serverHost)
        Server_Close(Network.server);
    if(Network.sockFd != 0)
        Socket_Close(Network.sockFd);
    SDL_DestroyTexture(Game.screenTexture);
    SDL_DestroyRenderer(Game.renderer);
    Game.renderer = NULL;
    SDL_DestroyWindow(Game.window);
    Game.window = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    Network_ShutdownSockets();
}

int main(int argc, char* argv[]) {
    if(!initialize()) {
        printf("Falha ao inicializar!\n");
    } else {
        SceneManager.quit = false;
        // Fontes
        Game.telaLogin = TTF_OpenFont("content/kozuki.ttf", 48);
        Game.inputFont = TTF_OpenFont("content/kozuki.ttf", 40);
        Game.rankMini = TTF_OpenFont("content/Minecraft.ttf", 44);
        Game.mainMenu = TTF_OpenFont("content/Minecraft.ttf", 68);
        Game.mainMenu_botoes = TTF_OpenFont("content/kozuki.ttf", 62);
        Game.Unisans = TTF_OpenFont("content/uni.ttf", 60);
        Game.roboto = TTF_OpenFont("content/roboto.ttf", 60);
        Game.serversFontd = TTF_OpenFont("content/Minecraft.ttf", 56);
        Game.serversFonte = TTF_OpenFont("content/Minecraft.ttf", 48);
        Game.startFont = TTF_OpenFont("content/kozuki.ttf", 25); 
        Game.serversName = TTF_OpenFont("content/kozuki.ttf", 80);
        Game.tip = TTF_OpenFont("content/roboto.ttf", 48);
        Game.win = TTF_OpenFont("content/roboto.ttf", 100);

        Game.enter = Mix_LoadWAV("content/enter.ogg");
        Game.change = Mix_LoadWAV("content/change.ogg");


        SDL_RenderPresent(Game.renderer);
        SceneManager.currentScene = SCENE_LOGIN;
        SceneManager.sLogin = SceneLogin_new();
        SDL_Event e;

        Game.mainMusic = Mix_LoadMUS("content/hang.mp3");
        Game.type = Mix_LoadWAV("content/type.wav");
        Mix_PlayMusic(Game.mainMusic, -1);

        while(!SceneManager.quit) {
            while(SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_QUIT) {
                    SceneManager.quit = true;
                } else {
                    SceneManager_handleEvent(&e);
                }
            }
            SDL_SetRenderTarget(Game.renderer, Game.screenTexture);
            SceneManager_updateScene();
            SDL_SetRenderTarget(Game.renderer, NULL);
            SDL_Rect dest = {0, 0, Game.screenWidth, Game.screenHeight};
            SDL_RenderCopy(Game.renderer, Game.screenTexture, NULL, &dest);
            SDL_RenderPresent(Game.renderer);
        }
        SceneManager_changeScene(SCENE_UNDEFINED);
    }
    destroy();
    return 0;
}
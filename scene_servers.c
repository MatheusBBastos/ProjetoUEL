#include "scene_servers.h"


Scene_Servers* SceneServers_new() {
    if(Network.sockFd == 0) {
        Network.sockFd = Socket_Open(0);
    }

    Scene_Servers* newScene = malloc(sizeof(Scene_Servers));

    newScene->receiveSock = Socket_Open(0);
    int broadcast = 1;
    setsockopt(newScene->receiveSock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    newScene->nome = WD_CreateTexture();
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->mutiplayer = WD_CreateTexture();
    newScene->server = WD_CreateTexture();
    newScene->entrar = WD_CreateTexture();
    newScene->servir = WD_CreateTexture();
    newScene->server1 = WD_CreateTexture();
    newScene->server2 = WD_CreateTexture();
    newScene->server3 = WD_CreateTexture();
    newScene->nomeServer1 = WD_CreateTexture();
    newScene->nomeServer2 = WD_CreateTexture();
    newScene->nomeServer3 = WD_CreateTexture();
    newScene->voltar = WD_CreateTexture();
    newScene->loading = WD_CreateTexture();
    newScene->indexe = 0;
    newScene-> indexd = 0;
    newScene-> posTela = 0;
    newScene->page = newScene->indexd/3;
    newScene->esquerda = true;
    newScene->numServers = 10;
    newScene->servers = malloc(newScene->numServers * sizeof(ServerInfo));
    for(int i = 0; i < newScene->numServers; i++) {
        newScene->servers[i].text[0] = '\0';
    }
    char sendData[] = "INF";
    Address* broad = NewAddress(255, 255, 255, 255, SERVER_DEFAULT_PORT);
    Socket_Send(newScene->receiveSock, broad, sendData, sizeof(sendData));
    DestroyAddress(broad);
    newScene->receivingInfo = true;
    newScene->receivingTimeout = 5 * Game.screenFreq;
    newScene->waitingConnection = false;

    SDL_Color Cname = {204, 204, 204};
    SDL_Color Cmult = {0, 132, 255};
    SDL_Color Cwhite = {255, 255, 255};

    newScene->boxIp = WD_CreateTextBox(75, 750, 380, 52, 16, Game.serversFonte, Cwhite, false);

    WD_TextureLoadFromText(newScene->nome, "Basto Forte", Game.serversName, Cname);
    WD_TextureLoadFromText(newScene->mutiplayer, "MULTIPLAYER", Game.serversFonte, Cmult);
    WD_TextureLoadFromText(newScene->server, "SERVER: 2", Game.serversFonte, Cwhite);
    WD_TextureLoadFromText(newScene->entrar, "Entrar", Game.serversFonte, Cwhite);
    WD_TextureLoadFromText(newScene->servir, "Servir", Game.serversFonte, Cwhite);
    WD_TextureLoadFromText(newScene->server1, "Server #1", Game.serversFontd, Cwhite);
    WD_TextureLoadFromText(newScene->server2, "Server #2", Game.serversFontd, Cwhite);
    WD_TextureLoadFromText(newScene->server3, "Server #3", Game.serversFontd, Cwhite);
    WD_TextureLoadFromText(newScene->nomeServer1, "Basto forte", Game.serversFontd, Cwhite);
    WD_TextureLoadFromText(newScene->nomeServer2, "Melvi forte", Game.serversFontd, Cwhite);
    WD_TextureLoadFromText(newScene->nomeServer3, "Tampy fraco ;(", Game.serversFontd, Cwhite);
    WD_TextureLoadFromText(newScene->voltar, "Voltar", Game.serversFonte, Cwhite);
    WD_TextureLoadFromFile(newScene->loading, "content/loading.png");

    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_mainMenu.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w;
    newScene->renderQuad.h = h;
    newScene->frame = 0;
    SDL_StartTextInput();

    return newScene;
}


void SceneServers_update(Scene_Servers* s) {
    if(s->waitingConnection && !SceneManager.inTransition) {
        Address sender;
        char data[32];
        int bytes = Socket_Receive(Network.sockFd, &sender, data, sizeof(data));
        if(bytes > 0 && sender.address == Network.serverAddress->address && sender.port == Network.serverAddress->port) {
            if(strncmp("CON", data, 3) == 0) {
                sscanf(data + 4, "%d", &Network.clientId);
                Network.connectedToServer = true;
                printf("[Client] Connection to server estabilished\n");
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOBBY);
            } else if(strncmp("FLL", data, 3) == 0) {
                printf("[Client] Server full\n");
                s->waitingConnection = false;
            } else if(strncmp("ING", data, 3) == 0) {
                printf("[Client] Server in-game\n");
                s->waitingConnection = false;
            }
        }
        s->connectionTimeout++;
        if(!Network.connectedToServer && s->connectionTimeout >= 3 * Game.screenFreq) {
            s->waitingConnection = false;
            printf("timeout\n");
        }
    }
    if(s->receivingInfo) {
        Address sender;
        char data[64];
        int bytes = Socket_Receive(s->receiveSock, &sender, data, sizeof(data));
        if(bytes > 0) {
            printf("Received info: %s\n", data);
            if(strncmp("INF", data, 3) == 0) {
                int min, max;
                char serverName[32];
                sscanf(data + 4, "%d %d %31[^\n]", &min, &max, serverName);
                for(int i = 0; i < s->numServers; i++) {
                    if(s->servers[i].text[0] == '\0') {
                        s->servers[i].addr.address = sender.address;
                        s->servers[i].addr.port = sender.port;
                        sprintf(s->servers[i].text, "%s - %d/%d", serverName, min, max);
                        SDL_Color color = {255, 255, 255};
                        WD_TextureLoadFromText(s->nomeServer1, s->servers[i].text, Game.serversFontd, color);
                    } 
                }
            }
        }
        s->receivingTimeout--;
        if(s->receivingTimeout <= 0) {
            s->receivingInfo = false;
        }
    }
    SDL_RenderClear(Game.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    WD_TextureRender(s->nome, 75, 515);
    WD_TextureRender(s->mutiplayer, 75, 585);
    WD_TextureRender(s->server, 120, 655);
    
    if (s->waitingConnection) {
        double angle = Game.screenFreq / 60.0 * 6 * s->frame;
        int loadingX = 470, loadingY = 750;
        SDL_Rect c = {loadingX, loadingY, 60, 60};
        SDL_RenderCopyEx(Game.renderer, s->loading->mTexture, NULL, &c, angle, NULL, SDL_FLIP_NONE);
    }

    //box
    SDL_SetRenderDrawBlendMode(Game.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 100);
    SDL_Rect rect = { 75, 750, 380, 52 };
    SDL_RenderFillRect(Game.renderer, &rect);
    WD_TextBoxRender(s->boxIp, s->frame);
    s->frame++;
    if(s->frame >= Game.screenFreq) {
        s->frame = 0;
    }

    SDL_SetTextureColorMod(s->entrar->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->servir->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->voltar->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->server1->mTexture, 0, 132, 255);
    SDL_SetTextureColorMod(s->server2->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->server3->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer1->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer2->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer3->mTexture, 255, 255, 255);
    

    if(s->esquerda) {
        if(s->indexe == 0)
            s->boxIp->active = true;
        else 
            s->boxIp->active = false;
        if(s->indexe == 1)
            SDL_SetTextureColorMod(s->entrar->mTexture, 255, 66, 0);
        else if(s->indexe == 2)
            SDL_SetTextureColorMod(s->servir->mTexture, 255, 66, 0);
        else if (s->indexe == 3)
            SDL_SetTextureColorMod(s->voltar->mTexture, 255, 66, 0);
    } else {
        s->boxIp->active = false;
        if(s->posTela == 0)
            SDL_SetTextureColorMod(s->nomeServer1->mTexture, 255, 66, 0);
        else if(s->posTela == 1)
            SDL_SetTextureColorMod(s->nomeServer2->mTexture, 255, 66, 0);
        else if (s->posTela == 2)
            SDL_SetTextureColorMod(s->nomeServer3->mTexture, 255, 66, 0);
    }

    WD_TextureRender(s->entrar, 160, 825);
    WD_TextureRender(s->servir, 160, 895);
    WD_TextureRender(s->voltar, 160, 965);
    WD_TextureRender(s->server1, 700, 515);
    WD_TextureRender(s->nomeServer1, 800, 585);
    WD_TextureRender(s->server2, 700, 655);
    WD_TextureRender(s->nomeServer2, 800, 725);
    WD_TextureRender(s->server3, 700, 795);
    WD_TextureRender(s->nomeServer3, 800, 865);
}

void SceneServers_destroy(Scene_Servers* s) {
    Socket_Close(s->receiveSock);
    WD_TextureDestroy(s->nome);
    WD_TextureDestroy(s->mutiplayer);
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->server);
    WD_TextureDestroy(s->entrar);
    WD_TextureDestroy(s->servir);
    WD_TextureDestroy(s->server1);
    WD_TextureDestroy(s->server2);
    WD_TextureDestroy(s->server3);
    WD_TextureDestroy(s->nomeServer1);
    WD_TextureDestroy(s->nomeServer2);
    WD_TextureDestroy(s->nomeServer3);
    WD_TextureDestroy(s->voltar);
    WD_TextureDestroy(s->loading);
    free(s);
}

void SceneServers_handleEvent(Scene_Servers* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {   
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            if(s->posTela < 2 && !s->esquerda)
                s->posTela++;

            if(s->indexe < 3 && s->esquerda)
                s->indexe++;
            else if(s->indexd < s->numServers-1 && !s->esquerda) //colocar variavel do nmr de servidores
                s->indexd++;

            if(s->posTela == 2 && !s->esquerda) {
                SDL_Color color = {255, 255, 255};
                //s->page = s->index/3;
                sprintf(s->string1, "Server #%d", s->indexd-1);
                sprintf(s->string2, "Server #%d", s->indexd);   
                sprintf(s->string3, "Server #%d", s->indexd+1);
                WD_TextureLoadFromText(s->server1, s->string1, Game.serversFontd, color);
                WD_TextureLoadFromText(s->server2, s->string2, Game.serversFontd, color);
                WD_TextureLoadFromText(s->server3, s->string3, Game.serversFontd, color);
            }

        } else if(e->key.keysym.sym == SDLK_UP) {
            if(s->posTela > 0 && !s->esquerda)
                s->posTela--;

            if(s->esquerda && s->indexe > 0)
                s->indexe--;
            else if(!s->esquerda && s->indexd > 0)
                s->indexd--;
            
            if(!s->esquerda && s->posTela == 0) {
                SDL_Color color = {255, 255, 255};
                //s->page = s->index/3;
                sprintf(s->string1, "Server #%d", s->indexd+1);
                sprintf(s->string2, "Server #%d", s->indexd+2);
                sprintf(s->string3, "Server #%d", s->indexd+3);
                WD_TextureLoadFromText(s->server1, s->string1, Game.serversFontd, color);
                WD_TextureLoadFromText(s->server2, s->string2, Game.serversFontd, color);
                WD_TextureLoadFromText(s->server3, s->string3, Game.serversFontd, color);
            }
            
            
        } else if(e->key.keysym.sym == SDLK_RIGHT) {
            s->esquerda = false;
        } else if(e->key.keysym.sym == SDLK_LEFT) {
            s->esquerda = true; 
        } else if(e->key.keysym.sym == SDLK_RETURN) {
            if(s->esquerda == false) {
                if(Network.serverAddress != NULL)
                    DestroyAddress(Network.serverAddress);
                Network.serverAddress = malloc(sizeof(Address));
                Network.serverAddress->address = s->servers[0].addr.address;
                Network.serverAddress->port = s->servers[0].addr.port;
                char data[32];
                sprintf(data, "CON 1 0 %s", Game.nome);
                Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
                s->waitingConnection = true;
                s->connectionTimeout = 0;
            }
            if(s->esquerda && s->indexe == 1) {
                unsigned int ip1, ip2, ip3, ip4;
                sscanf(s->boxIp->text, "%u.%u.%u.%u", &ip1, &ip2, &ip3, &ip4);
                if(Network.serverAddress != NULL)
                    DestroyAddress(Network.serverAddress);
                Network.serverAddress = NewAddress(ip1, ip2, ip3, ip4, SERVER_DEFAULT_PORT);
                char data[32];
                sprintf(data, "CON 1 0 %s", Game.nome);
                Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
                s->waitingConnection = true;
                s->connectionTimeout = 0;
            } else if(s->esquerda && s->indexe == 2) {
                Network.serverHost = true;
                Network.server = Server_Open(SERVER_DEFAULT_PORT);
                if(Network.server != NULL) {
                    printf("Server open\n");
                    Network.serverThread = SDL_CreateThread(Server_InitLoop, "ServerLoop", Network.server);
                    if(Network.serverAddress != NULL)
                        DestroyAddress(Network.serverAddress);
                    Network.serverAddress = NewAddress(127, 0, 0, 1, SERVER_DEFAULT_PORT);
                    char data[32];
                    sprintf(data, "CON 1 1 %s", Game.nome);
                    Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
                    s->waitingConnection = true;
                    s->connectionTimeout = 0;
                }
            }
            else if(s->esquerda && s->indexe == 3)
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);

        } 

    }

    WD_TextBoxHandleEvent(s->boxIp, e);

}
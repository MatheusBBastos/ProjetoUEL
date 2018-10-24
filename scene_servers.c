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
    newScene->servir[0] = WD_CreateTexture();
    newScene->servir[1] = WD_CreateTexture();
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
    SDL_Color blue = { 106,122,244 };
    SDL_Color orange = { 255,172,65 };

    newScene->boxNome = WD_CreateTextBox(135, 770, 380, 50, 16, Game.serversFonte, Cwhite, false);
    newScene->boxIp = WD_CreateTextBox(135, 860, 380, 50, 16, Game.serversFonte, Cwhite, false);
    WD_TextureLoadFromText(newScene->entrar, "Entrar", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromText(newScene->servir[0], "criar ", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromText(newScene->servir[1], "nova sala", Game.mainMenu_botoes, orange);
    for (int i = 0; i < 4; i++) {
        newScene->serverName[i] = WD_CreateTexture();
        newScene->serverSlot[i] = WD_CreateTexture();
        WD_TextureLoadFromText(newScene->serverName[i], "mlv", Game.serversName, blue);
        WD_TextureLoadFromText(newScene->serverSlot[i], "03/04", Game.telaLogin, Cwhite);
    }
    WD_TextureLoadFromText(newScene->voltar, "Voltar", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromFile(newScene->loading, "content/loading.png");

    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/bgserver.png");
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
                      //  WD_TextureLoadFromText(s->nomeServer1, s->servers[i].text, Game.serversFontd, color);
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

    
    if (s->waitingConnection) {
        double angle = Game.screenFreq / 60.0 * 6 * s->frame;
        int loadingX = 470, loadingY = 750;
        SDL_Rect c = {loadingX, loadingY, 60, 60};
        SDL_RenderCopyEx(Game.renderer, s->loading->mTexture, NULL, &c, angle, NULL, SDL_FLIP_NONE);
    }



    //box
    SDL_SetRenderDrawBlendMode(Game.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 100);
    if (s->boxIp->active) {
        SDL_Rect rect = { 135, 860, 380, 50 };
        SDL_RenderFillRect(Game.renderer, &rect);
        WD_TextBoxRender(s->boxIp, s->frame);
    }

    if (s->boxNome->active) {
        SDL_Rect rect = { 135, 770, 380, 50 };
        SDL_RenderFillRect(Game.renderer, &rect);
        WD_TextBoxRender(s->boxNome, s->frame);
    }


    s->frame++;
    if(s->frame >= Game.screenFreq) {
        s->frame = 0;
    }

    /*SDL_SetTextureColorMod(s->entrar->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->servir[0]->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->voltar->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->server1->mTexture, 0, 132, 255);
    SDL_SetTextureColorMod(s->server2->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->server3->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer1->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer2->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer3->mTexture, 255, 255, 255);*/
    
    SDL_Rect indexEntrar = { 135, 860 + s->entrar->h , s->entrar->w, 5 };
    SDL_Rect indexServir = { 135, 770 + s->servir[1]->h , s->servir[1]->w, 5 };
    SDL_Rect indexVoltar = { 135, 945 + s->voltar->h , s->voltar->w, 5 };

    SDL_SetRenderDrawBlendMode(Game.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 100);

    if (s->boxIp->active) {
        WD_TextureRender(s->entrar, 135, 780);
        WD_TextureRender(s->servir[0], 135, 640);
        WD_TextureRender(s->servir[1], 135, 690);
    }
    else if (s->boxNome->active) {
        WD_TextureRender(s->entrar, 135, 860);
        WD_TextureRender(s->servir[0], 135, 640);
        WD_TextureRender(s->servir[1], 135, 690);
    }
    else {
        WD_TextureRender(s->entrar, 135, 860);
        WD_TextureRender(s->servir[0], 135, 720);
        WD_TextureRender(s->servir[1], 135, 770);
    }




    if(s->esquerda) {
        if (s->indexe == 0 && !s->boxNome->active) {
            SDL_RenderFillRect(Game.renderer, &indexServir);
        }
        else if (s->indexe == 1 && !s->boxIp->active) {
            SDL_RenderFillRect(Game.renderer, &indexEntrar);
        }
        else if (s->indexe == 2) {
            SDL_RenderFillRect(Game.renderer, &indexVoltar);
        }
    } else {
        s->boxIp->active = false;
       /* if(s->posTela == 0)
            SDL_SetTextureColorMod(s->nomeServer1->mTexture, 255, 66, 0);
        else if(s->posTela == 1)
            SDL_SetTextureColorMod(s->nomeServer2->mTexture, 255, 66, 0);
        else if (s->posTela == 2)
            SDL_SetTextureColorMod(s->nomeServer3->mTexture, 255, 66, 0);*/
    }


    int pos = 3 * 1440 / 4;
    for (int i = 0; i < 4; i++) {
        WD_TextureRender(s->serverName[i], pos - (s->serverName[i]->w / 2), 365 + ((i)*175));
        WD_TextureRender(s->serverSlot [i], pos - (s->serverSlot[i]->w / 2), 445 + ((i) * 175));
    }

    WD_TextureRender(s->voltar, 135, 945);
    /*
    WD_TextureRender(s->server1, 700, 515);
    WD_TextureRender(s->nomeServer1, 800, 585);
    WD_TextureRender(s->server2, 700, 655);
    WD_TextureRender(s->nomeServer2, 800, 725);
    WD_TextureRender(s->server3, 700, 795);
    WD_TextureRender(s->nomeServer3, 800, 865);*/
}

void SceneServers_destroy(Scene_Servers* s) {
    Socket_Close(s->receiveSock);
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->entrar);
    WD_TextureDestroy(s->servir[0]);
    WD_TextureDestroy(s->servir[1]);
    for (int i = 0; i < 4; i++) {
        WD_TextureDestroy(s->serverSlot[i]);
        WD_TextureDestroy(s->serverName[i]);
    }
    WD_TextureDestroy(s->voltar);
    WD_TextureDestroy(s->loading);
    free(s);
}

void SceneServers_handleEvent(Scene_Servers* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {   
        if(e->key.keysym.sym == SDLK_TAB) {
            if (s->boxIp->active || s->boxNome->active) {
                s->boxIp->active = false;
                s->boxNome->active = false;
            }
            else {
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
            }
        } else if(e->key.keysym.sym == SDLK_DOWN && !s->boxIp->active && !s->boxNome->active) {
            if(s->posTela < 2 && !s->esquerda)
                s->posTela++;

            if(s->indexe < 2 && s->esquerda)
                s->indexe++;
            else if(s->indexd < s->numServers-1 && !s->esquerda) //colocar variavel do nmr de servidores
                s->indexd++;

            if(s->posTela == 2 && !s->esquerda) {
                SDL_Color color = {255, 255, 255};
                //s->page = s->index/3;
                sprintf(s->string1, "Server #%d", s->indexd-1);
                sprintf(s->string2, "Server #%d", s->indexd);   
                sprintf(s->string3, "Server #%d", s->indexd+1);
             //   WD_TextureLoadFromText(s->server1, s->string1, Game.serversFontd, color);
           //     WD_TextureLoadFromText(s->server2, s->string2, Game.serversFontd, color);
            //    WD_TextureLoadFromText(s->server3, s->string3, Game.serversFontd, color);
            }

        } else if(e->key.keysym.sym == SDLK_UP && !s->boxIp->active && !s->boxNome->active) {
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
            //    WD_TextureLoadFromText(s->server1, s->string1, Game.serversFontd, color);
            //    WD_TextureLoadFromText(s->server2, s->string2, Game.serversFontd, color);
            //    WD_TextureLoadFromText(s->server3, s->string3, Game.serversFontd, color);
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
                if (s->boxIp->active == true) {
                    unsigned int ip1, ip2, ip3, ip4;
                    sscanf(s->boxIp->text, "%u.%u.%u.%u", &ip1, &ip2, &ip3, &ip4);
                    if (Network.serverAddress != NULL)
                        DestroyAddress(Network.serverAddress);
                    Network.serverAddress = NewAddress(ip1, ip2, ip3, ip4, SERVER_DEFAULT_PORT);
                    char data[32];
                    sprintf(data, "CON 1 0 %s", Game.nome);
                    Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
                    s->waitingConnection = true;
                    s->connectionTimeout = 0;
                }
                else {
                    s->boxIp->active = true;
                }
            } else if(s->esquerda && s->indexe == 0) {
                if (s->boxNome->active == true) {
                    Network.serverHost = true;
                    Network.server = Server_Open(SERVER_DEFAULT_PORT, s->boxNome->text);
                    if (Network.server != NULL) {
                        printf("Server open\n");
                        Network.serverThread = SDL_CreateThread(Server_InitLoop, "ServerLoop", Network.server);
                        if (Network.serverAddress != NULL)
                            DestroyAddress(Network.serverAddress);
                        Network.serverAddress = NewAddress(127, 0, 0, 1, SERVER_DEFAULT_PORT);
                        char data[32];
                        sprintf(data, "CON 1 1 %s", Game.nome);
                        Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
                        s->waitingConnection = true;
                        s->connectionTimeout = 0;
                    }
                }
                else {
                    s->boxNome->active = true;
                }
                
            }
            else if(s->esquerda && s->indexe == 2)
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);

        } 

    }

    WD_TextBoxHandleEvent(s->boxIp, e);
    WD_TextBoxHandleEvent(s->boxNome, e);

}

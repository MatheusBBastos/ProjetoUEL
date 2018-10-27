#include "scene_servers.h"


Scene_Servers* SceneServers_new() {
    if(Network.sockFd == 0) {
        Network.sockFd = Socket_Open(0, false);
    }

    Scene_Servers* newScene = malloc(sizeof(Scene_Servers));

    newScene->receiveSock = Socket_Open(0, false);
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
    newScene->indexShow = 0;
    newScene->indexd = 0;
    newScene->posTela = 0;
    newScene->inGame = false;
    newScene->esquerda = true;
    newScene->numServers = 0;
    newScene->maxServers = 10;
    newScene->servers = malloc(newScene->maxServers * sizeof(ServerInfo));
    newScene->waitingConnection = false;

    newScene->needServersRefresh = false;

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
        //WD_TextureLoadFromText(newScene->serverName[i], "mlv", Game.serversName, blue);
    //    WD_TextureLoadFromText(newScene->serverSlot[i], "03/04", Game.telaLogin, Cwhite);
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

    SceneServers_RefreshList(newScene);

    SDL_StartTextInput();

    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        Mix_PlayMusic(Game.mainMusic, -1);
    }

    return newScene;
}

void SceneServers_RefreshList(Scene_Servers* s) {
    for(int i = 0; i < s->maxServers; i++) {
        s->servers[i].text[0] = '\0';
    }
    s->numServers = 0;
    s->indexShow = 0;
    s->indexd = 0;
    s->posTela = 0;
    s->esquerda = true;
    s->inGame = false;

    char sendData[] = "INF";
    Address* broad = NewAddress(255, 255, 255, 255, BROADCAST_PORT);
    Socket_Send(s->receiveSock, broad, sendData, sizeof(sendData));
    DestroyAddress(broad);
    SDL_Color Cwhite = {255, 255, 255};
    s->receivingInfo = true;
    s->receivingTimeout = 5 * Game.screenFreq;
    for(int i = 0; i < 3; i++) {
        WD_TextureLoadFromText(s->serverName[i], " ", Game.serversFontd, Cwhite);
    }
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
                Network.singleplayer = false;
                printf("[Client] Connection to server estabilished\n");
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOBBY);
            } else if(strncmp("FLL", data, 3) == 0) {
                printf("[Client] Server full\n");
                s->waitingConnection = false;
            } else if(strncmp("ING", data, 3) == 0) {
                printf("[Client] Server in-game\n");
                s->waitingConnection = false;
                s->inGame = true;
            }
        }
        s->connectionTimeout++;
        if(!Network.connectedToServer && s->connectionTimeout >= 3 * Game.screenFreq) {
            s->waitingConnection = false;
        }
    }
    if(s->receivingInfo) {
        Address sender;
        char data[64];
        int bytes = Socket_Receive(s->receiveSock, &sender, data, sizeof(data));
        if(bytes > 0) {
            if(strncmp("INF", data, 3) == 0) {
                int min, max;
                char serverName[32];
                sscanf(data + 4, "%d %d %31[^\n]", &min, &max, serverName);
                for(int i = 0; i < s->maxServers; i++) {
                    if(s->servers[i].text[0] == '\0') {
                        s->numServers++;
                        s->servers[i].addr.address = sender.address;
                        s->servers[i].addr.port = sender.port;
                        sprintf(s->servers[i].text, "%s", serverName);
                        sprintf(s->servers[i].num, "%d/%d", min, max);
                        s->needServersRefresh = true;
                        break;   
                    } else if(s->servers[i].addr.address == sender.address && s->servers[i].addr.port == sender.port) {
                        sprintf(s->servers[i].text, "%s", serverName);
                        sprintf(s->servers[i].num, "%d/%d", min, max);
                        s->needServersRefresh = true;
                        break;
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

    if(s->needServersRefresh) {
        SDL_Color white = {255, 255, 255};
        SDL_Color green = {153, 204, 50};
        SDL_Color red = {255, 0, 0};
        SDL_Color orange = {228, 120, 51};
        for(int i = 0; i <= 3; i++) {
            if(s->servers[s->indexShow + i].text[0] != '\0') {
                WD_TextureLoadFromText(s->serverName[i], s->servers[s->indexShow + i].text, Game.mainMenu_botoes, white);

                int min, max;
                sscanf(s->servers[i].num, "%d/%d", &min, &max);
                if(s->inGame)    
                    WD_TextureLoadFromText(s->serverSlot[i], "EM JOGO", Game.serversFontd, orange);
                else if(min == 4)
                    WD_TextureLoadFromText(s->serverSlot[i], "LOTADO", Game.serversFontd, red);
                else if(min == 3)
                    WD_TextureLoadFromText(s->serverSlot[i], s->servers[s->indexShow + i].num, Game.serversFontd, orange);
                else
                    WD_TextureLoadFromText(s->serverSlot[i], s->servers[s->indexShow + i].num, Game.serversFontd, green);

                
                
                
            } else {
                WD_TextureLoadFromText(s->serverName[i], " ", Game.serversFontd, white);
                WD_TextureLoadFromText(s->serverSlot[i], " ", Game.serversFontd, white);
            }
        }
    }
    
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
    SDL_SetTextureColorMod(s->nomeServer[0]->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer[1]->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer[2]->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer1->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer2->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer3->mTexture, 255, 255, 255);*/
    
    SDL_Rect indexEntrar = { 135, 860 + s->entrar->h , s->entrar->w, 5 };
    SDL_Rect indexServir = { 135, 770 + s->servir[1]->h , s->servir[1]->w, 5 };
    SDL_Rect indexVoltar = { 135, 945 + s->voltar->h , s->voltar->w, 5 };

    int pos = 3 * 1440 / 4;
    SDL_Rect indexPos0 = { pos - (s->serverName[0]->w / 2), 365 + s->serverName[0]->h, s->serverName[0]->w, 5};
    SDL_Rect indexPos1 = { pos - (s->serverName[1]->w / 2), 365 + ((1)*175) + s->serverName[1]->h, s->serverName[1]->w, 5};
    SDL_Rect indexPos2 = { pos - (s->serverName[2]->w / 2), 365 + ((2)*175) + s->serverName[2]->h, s->serverName[2]->w, 5};
    SDL_Rect indexPos3 = { pos - (s->serverName[3]->w / 2), 365 + ((3)*175) + s->serverName[3]->h, s->serverName[3]->w, 5};


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
        if(s->posTela == 0)
            SDL_RenderFillRect(Game.renderer, &indexPos0);
        else if(s->posTela == 1)
            SDL_RenderFillRect(Game.renderer, &indexPos1);
        else if(s->posTela == 2)
            SDL_RenderFillRect(Game.renderer, &indexPos2);
        else if(s->posTela == 3)
            SDL_RenderFillRect(Game.renderer, &indexPos3);

    }


    //int pos = 3 * 1440 / 4;
    for (int i = 0; i < 4; i++) {
        WD_TextureRender(s->serverName[i], pos - (s->serverName[i]->w / 2), 365 + ((i)*175));
        WD_TextureRender(s->serverSlot[i], pos - (s->serverSlot[i]->w / 2), 445 + ((i) * 175));
    }

    WD_TextureRender(s->voltar, 135, 945);
    /*
    WD_TextureRender(s->server1, 700, 515);
    WD_TextureRender(s->nomeServer[0], 800, 585);
    WD_TextureRender(s->server2, 700, 655);
    WD_TextureRender(s->nomeServer[1], 800, 725);
    WD_TextureRender(s->server3, 700, 795);
    WD_TextureRender(s->nomeServer[2], 800, 865);
    WD_TextureRender(s->nomeServer3, 800, 865);*/
}

void SceneServers_destroy(Scene_Servers* s) {
    Socket_Close(s->receiveSock);
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->entrar);
    WD_TextureDestroy(s->server);
    WD_TextureDestroy(s->mutiplayer);
    WD_TextBoxDestroy(s->boxIp);
    WD_TextBoxDestroy(s->boxNome);
    WD_TextureDestroy(s->servir[0]);
    WD_TextureDestroy(s->servir[1]);
    for (int i = 0; i < 4; i++) {
        WD_TextureDestroy(s->serverSlot[i]);
        WD_TextureDestroy(s->serverName[i]);
    }
    WD_TextureDestroy(s->voltar);
    WD_TextureDestroy(s->loading);
    SDL_StopTextInput();
    free(s);
}

void SceneServers_handleEvent(Scene_Servers* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;

    if ((e->type == SDL_TEXTINPUT || e->type == SDL_TEXTEDITING) && (s->boxIp->active || s->boxNome->active)) {
        Mix_PlayChannel(-1, Game.type, 0);
    }

    if(e->type == SDL_KEYDOWN) {   
        if(e->key.keysym.sym == SDLK_ESCAPE) {
            if (s->boxIp->active || s->boxNome->active) {
                s->boxIp->active = false;
                s->boxNome->active = false;
            }
            else {
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
            }
        } else if(e->key.keysym.sym == SDLK_DOWN && !s->boxIp->active && !s->boxNome->active) {
            if (s->posTela < 3 && !s->esquerda && s->posTela < (s->numServers - 1)) {// pra ir só até o num servers
                s->posTela++;
                Mix_PlayChannel(-1, Game.change, 0);
            }
            if (s->indexe < 2 && s->esquerda) {
                s->indexe++;
                Mix_PlayChannel(-1, Game.change, 0);
            }
            else if (s->indexd < (s->numServers - 1) && !s->esquerda) {
                s->indexd++;
                Mix_PlayChannel(-1, Game.change, 0);
            }

            if(s->posTela == 3 && !s->esquerda) {
                SDL_Color color = {255, 255, 255};
                s->indexShow = s->indexd-3;
                s->needServersRefresh = true;
            }

        } else if(e->key.keysym.sym == SDLK_UP && !s->boxIp->active && !s->boxNome->active) {
            if (s->posTela > 0 && !s->esquerda) {
                s->posTela--;
                Mix_PlayChannel(-1, Game.change, 0);
            }

            if (s->esquerda && s->indexe > 0) {
                s->indexe--;
                Mix_PlayChannel(-1, Game.change, 0);
            }
            else if (!s->esquerda && s->indexd > 0) {
                s->indexd--;
                Mix_PlayChannel(-1, Game.change, 0);
            }
            
            if(!s->esquerda && s->posTela == 0) {
                SDL_Color color = {255, 255, 255};
                s->indexShow = s->indexd;
            }
            
            
        } else if(e->key.keysym.sym == SDLK_RIGHT && s->numServers != 0) {
            s->esquerda = false;
            Mix_PlayChannel(-1,Game.change,0);
        } else if(e->key.keysym.sym == SDLK_LEFT) {
            s->esquerda = true; 
            Mix_PlayChannel(-1, Game.change, 0);
        } else if(e->key.keysym.sym == SDLK_RETURN) {
            Mix_PlayChannel(-1, Game.enter, 0);
            if(s->esquerda == false) {
                if(s->servers[s->indexd].text[0] != '\0') {
                    if(Network.serverAddress != NULL)
                        DestroyAddress(Network.serverAddress);
                    Network.serverAddress = malloc(sizeof(Address));
                    Network.serverAddress->address = s->servers[s->indexd].addr.address;
                    Network.serverAddress->port = s->servers[s->indexd].addr.port;
                    char data[32];
                    sprintf(data, "CON 1 0 %s", Game.nome);
                    Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
                    s->waitingConnection = true;
                    s->connectionTimeout = 0;
                }
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
                if (s->boxNome->active == true && strcmp(s->boxNome->text, "") != 0) {
                    Network.serverHost = true;
                    Network.server = Server_Open(SERVER_DEFAULT_PORT, s->boxNome->text, false);
                    if (Network.server != NULL) {
                        Network.serverThread = SDL_CreateThread(Server_InitLoop, "ServerLoop", Network.server);
                        if (Network.serverAddress != NULL)
                            DestroyAddress(Network.serverAddress);
                        struct sockaddr_in sin;
                        int len = sizeof(sin);
                        unsigned short port;
                        getsockname(Network.server->sockfd, (struct sockaddr *)&sin, &len);
                        port = ntohs(sin.sin_port);
                        Network.serverAddress = NewAddress(127, 0, 0, 1, port);
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

        } else if(e->key.keysym.sym == SDLK_F5) {
            SceneServers_RefreshList(s);
        } 

    }

    WD_TextBoxHandleEvent(s->boxIp, e);
    WD_TextBoxHandleEvent(s->boxNome, e);

}

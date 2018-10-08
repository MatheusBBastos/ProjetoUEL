#include "scene_map.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->keyLeft = false;
    newScene->keyRight = false;
    newScene->keyDown = false;
    newScene->keyUp = false;
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/001-Grassland01.png");
    Map_RenderFull(Game.map, newScene->tileMap);
    newScene->screenX = 0;
    newScene->screenY = 0;
    newScene->player = Game.map->characters[Network.clientId];
    newScene->renderCharacters = malloc(Game.map->charNumber * sizeof(int));
    newScene->waitingConnection = false;
    return newScene;
}

int compareCharacters(const void * a, const void * b) {
    Character** chars = Game.map->characters;
    int i = *(int*)a;
    int j = *(int*)b;
    if(i == -1 || chars[i] == NULL) {
        return -1;
    }
    if(j == -1 || chars[j] == NULL) {
        return 1;
    }
    return ( chars[i]->renderY - chars[j]->renderY );
}

void SceneMap_update(Scene_Map* s) {
    if(s->waitingConnection) {
        Address sender;
        char data[512];
        int bytes = Socket_Receive(Network.sockFd, &sender, data, sizeof(data));
        if(bytes > 0 && sender.address == Network.serverAddress->address && sender.port == Network.serverAddress->port) {
            if(strncmp("CON", data, 3) == 0) {
                Network.connectedToServer = true;
                printf("[Client] Connection to server estabilished\n");
                s->waitingConnection = false;
            }
        }
    }
    if(Network.connectedToServer) {
        char sendData[] = "PNG";
        Socket_Send(Network.sockFd, Network.serverAddress, sendData, sizeof(sendData));
        Address sender;
        char data[512];
        while(Socket_Receive(Network.sockFd, &sender, data, sizeof(data)) > 0) {
            if(sender.address == Network.serverAddress->address && sender.port == Network.serverAddress->port) {
                printf("[Client] Received from server: %s\n", data);
                if(strncmp("KCK", data, 3) == 0) {
                    Network.connectedToServer = false;
                    Socket_Close(Network.sockFd);
                } else if(strncmp("FIX", data, 3) == 0) {
                    uint64_t mId;
                    int x, y;
                    sscanf(data + 4, "%llu %d %d", &mId, &x, &y);
                    if(s->player != NULL) {
                        s->player->x = x;
                        s->player->y = y;
                    }
                } else if(strncmp("POS", data, 3) == 0) {
                    int id, x, y, dir;
                    sscanf(data + 4, "%d %d %d %d", &id, &x, &y, &dir);
                    if(Game.map->characters != NULL && Game.map->characters[id] != NULL) {
                        Game.map->characters[id]->x = x;
                        Game.map->characters[id]->y = y;
                        Game.map->characters[id]->direction = dir;
                    }
                } else if(strncmp("PDC", data, 3) == 0) {
                    int id;
                    sscanf(data + 4, "%d", &id);
                    if(Game.map->characters[id] != NULL) {
                        Character_Destroy(Game.map->characters[id]);
                        Game.map->characters[id] = NULL;
                    }
                } else if(strncmp("SSD", data, 3) == 0) {
                    Network.connectedToServer = false;
                    SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SERVERS);
                }
            }
        }
    }
    // Centralizar a câmera no jogador
    if(s->player != NULL) {
        s->screenX = ((int) s->player->renderX + s->player->sprite->w / 6) - (Game.screenWidth) / 2;
        s->screenY = ((int) s->player->renderY + s->player->sprite->h / 8) - (Game.screenHeight) / 2;
    }
    if(s->screenX > Game.map->width * TILE_SIZE - Game.screenWidth) {
        s->screenX = Game.map->width * TILE_SIZE - Game.screenWidth;
    }
    if(s->screenY > Game.map->height * TILE_SIZE - Game.screenHeight) {
        s->screenY = Game.map->height * TILE_SIZE - Game.screenHeight;
    }
    if(s->screenX < 0)
        s->screenX = 0;
    if(s->screenY < 0)
        s->screenY = 0;
    SDL_SetRenderDrawColor(Game.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(Game.renderer);
    SDL_Rect renderQuad = {s->screenX, s->screenY, Game.screenWidth, Game.screenHeight};
    int dstWidth, dstHeight;
    if(Game.map->width * TILE_SIZE < Game.screenWidth)
        dstWidth = Game.map->width * TILE_SIZE;
    else
        dstWidth = Game.screenWidth;
    if(Game.map->height * TILE_SIZE < Game.screenHeight)
        dstHeight = Game.map->width * TILE_SIZE;
    else
        dstHeight = Game.screenHeight;
    SDL_Rect dstRect = {0, 0, dstWidth, dstHeight};
    // Renderizar as camadas do mapa
    SDL_RenderCopy(Game.renderer, Game.map->layers[0], &renderQuad, &dstRect);
    SDL_RenderCopy(Game.renderer, Game.map->layers[1], &renderQuad, &dstRect);

    // MUDAR, TÁ MUITO RUIM (realmente) 
    /*
    if(s->player != NULL && s->player->x == s->player->renderX && s->player->y == s->player->renderY) {
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP]) {
            Character_TryToMove(s->player, 3, Game.map);
        } else if(state[SDL_SCANCODE_DOWN]) {
            Character_TryToMove(s->player, 0, Game.map);
        } else if(state[SDL_SCANCODE_LEFT]) {
            Character_TryToMove(s->player, 1, Game.map);
        } else if(state[SDL_SCANCODE_RIGHT]) {
            Character_TryToMove(s->player, 2, Game.map);
        }
    }*/


    if (s->player != NULL && s->player->x == s->player->renderX && s->player->y == s->player->renderY) {
        if(s->player->moving) {
            switch (s->lastMov) {
            case 'U':
                Character_TryToMove(s->player, 3, Game.map);
                break;
            case 'D':
                Character_TryToMove(s->player, 0, Game.map);
                break;
            case 'L':
                Character_TryToMove(s->player, 1, Game.map);
                break;
            case 'R':
                Character_TryToMove(s->player, 2, Game.map);
                break;
            }

        }
    }
    // ------------------------------------ //

    // Atualização dos personagens
    for(int i = 0; i < Game.map->charNumber; i++) {
        if(Game.map->characters[i] != NULL) {
            Character_Update(Game.map->characters[i], Game.map);
            s->renderCharacters[i] = i;
        } else {
            s->renderCharacters[i] = -1;
        }
    }
    
    // Ordenar a lista de renderização dos personagens com base em suas alturas
    qsort(s->renderCharacters, Game.map->charNumber, sizeof(int), compareCharacters);
    
    // Renderizar os personagens
    for(int i = 0; i < Game.map->charNumber; i++) {
        if(s->renderCharacters[i] != -1 && Game.map->characters[s->renderCharacters[i]] != NULL) {
            Character_Render(Game.map->characters[s->renderCharacters[i]], s->screenX, s->screenY);
        }
    }
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if(e->type == SDL_KEYDOWN) {
        //Atualiza estado da tecla se o evento dela for mais recente
        if(s->player != NULL) {
                switch (e->key.keysym.scancode) {
                case SDL_SCANCODE_DOWN:
                    s->keyDown = true;
                    s->lastMov = 'D';
                    if (!s->player->moving) {
                        s->player->moving = true;
                    }
                    break;
                case SDL_SCANCODE_UP:
                    s->keyUp = true;
                    s->lastMov = 'U';
                    if (!s->player->moving) {
                        s->player->moving = true;
                    }
                    break;
                case SDL_SCANCODE_LEFT:
                    s->keyLeft = true;
                    s->lastMov = 'L';
                    if (!s->player->moving) {
                        s->player->moving = true;
                    }
                    break;
                case SDL_SCANCODE_RIGHT:
                    s->lastMov = 'R';
                    s->keyRight = true;
                    if (!s->player->moving) {
                        s->player->moving = true;
                    }
                    break;
                }
                
            
        }



        if(e->key.keysym.sym == SDLK_o) {
            s->player->moveSpeed = 4;
        }

        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if(e->key.keysym.sym == SDLK_F3) {
            Game.debug = !Game.debug;
        } else if(e->key.keysym.sym == SDLK_F4) {
            Network.server = Server_Open(3000);
            if(Network.server != NULL) {
                printf("Server open\n");
                Network.serverThread = SDL_CreateThread(Server_InitLoop, "ServerLoop", Network.server);
            }
        } else if(e->key.keysym.sym == SDLK_F5) {
            if(Network.connectedToServer == false)
                Network.sockFd = Socket_Open(0);
            if(Network.serverAddress == NULL) {
                DestroyAddress(Network.serverAddress);
                Network.serverAddress = NewAddress(127, 0, 0, 1, 3000);
            }
            char data[] = "CON 1 1 basto";
            Socket_Send(Network.sockFd, Network.serverAddress, data, sizeof(data));
            s->waitingConnection = true;
        } else if(e->key.keysym.sym == SDLK_F6) {
            if(Network.server != NULL && Network.server->running) {
                Network.server->running = false;
                int returnValue;
                SDL_WaitThread(Network.serverThread, &returnValue);
                printf("Thread closed with return value %d\n", returnValue);
                Server_Destroy(Network.server);
                Network.server = NULL;
            }
        }
    }
    else if (e->type == SDL_KEYUP) {
        if(s->player != NULL) {
            //Atualiza estado da tecla
            bool keyChanged = false;
            switch (e->key.keysym.scancode) {
            case SDL_SCANCODE_DOWN:
                s->keyDown = false;
                keyChanged = true;
                break;
            case SDL_SCANCODE_UP:
                s->keyUp = false;
                keyChanged = true;
                break;
            case SDL_SCANCODE_LEFT:
                s->keyLeft = false;
                keyChanged = true;
                break;
            case SDL_SCANCODE_RIGHT:
                s->keyRight = false;
                keyChanged = true;
                break;
            }

            if (keyChanged) {
                if (s->keyDown) {
                    s->lastMov = 'D';
                    s->player->moving = true;
                }
                else if (s->keyUp) {
                    s->lastMov = 'U';
                    s->player->moving = true;
                }
                else if (s->keyLeft) {
                    s->lastMov = 'L';
                    s->player->moving = true;
                }
                else if (s->keyRight) {
                    s->lastMov = 'R';
                    s->player->moving = true;
                }
           
            
            }
        }
    }
    if(s->player != NULL) {
        if (!s->keyDown && !s->keyLeft && !s->keyRight && !s->keyUp) {
            s->player->moving = false;
            s->lastMov = 'N';
        }
    }


}


void SceneMap_destroy(Scene_Map* s) {
    if(Network.connectedToServer) {
        char data[] = "DCS";
        Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
        Network.connectedToServer = false;
    }
    if(Network.serverHost)
            Server_Close(Network.server);
    Map_Destroy(Game.map);
    Game.map = NULL;
    WD_TextureDestroy(s->tileMap);
    if(s->renderCharacters != NULL) {
        free(s->renderCharacters);
    }
    free(s);
}
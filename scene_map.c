#include "scene_map.h"
#include "stdio.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->lastTimeStamp = 0;
    newScene->keyLeft = false;
    newScene->keyRight = false;
    newScene->keyDown = false;
    newScene->keyUp = false;
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/001-Grassland01.png");
    newScene->map = Map_Create();
    Map_Load(newScene->map, "map.txt", true);
    Map_RenderFull(newScene->map, newScene->tileMap);
    newScene->screenX = 0;
    newScene->screenY = 0;
    newScene->player = NULL;
    newScene->charNumber = 0;
    newScene->characters = NULL;
    newScene->renderCharacters = NULL;
    newScene->waitingConnection = false;
    newScene->testServer = NULL;
    return newScene;
}

int compareCharacters(const void * a, const void * b) {
    Character** chars = sMng.sMap->characters;
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
        int bytes = Socket_Receive(gInfo.sockFd, &sender, data, sizeof(data));
        if(bytes > 0 && sender.address == gInfo.serverAddress->address && sender.port == gInfo.serverAddress->port) {
            if(strncmp("CON", data, 3) == 0) {
                gInfo.connectedToServer = true;
                printf("[Client] Connection to server estabilished\n");
                s->waitingConnection = false;
            }
        }
    }
    if(gInfo.connectedToServer) {
        char sendData[] = "PNG";
        Socket_Send(gInfo.sockFd, gInfo.serverAddress, sendData, sizeof(sendData));
        Address sender;
        char data[512];
        while(Socket_Receive(gInfo.sockFd, &sender, data, sizeof(data)) > 0) {
            if(sender.address == gInfo.serverAddress->address && sender.port == gInfo.serverAddress->port) {
                printf("[Client] Received from server: %s\n", data);
                if(strncmp("KCK", data, 3) == 0) {
                    gInfo.connectedToServer = false;
                    Socket_Close(gInfo.sockFd);
                } else if(strncmp("CNM", data, 3) == 0) {
                    if(s->characters != NULL) {
                        for(int i = 0; i < s->charNumber; i++) {
                            if(s->characters[i] != NULL)
                                Character_Destroy(s->characters[i]);
                        }
                        free(s->characters);
                        free(s->renderCharacters);
                    }
                    sscanf(data + 4, "%d", &s->charNumber);
                    s->characters = malloc(s->charNumber * sizeof(Character*));
                    s->renderCharacters = malloc(s->charNumber * sizeof(int));
                    for(int i = 0; i < s->charNumber; i++) {
                        s->characters[i] = NULL;
                    }
                } else if(strncmp("CHR", data, 3) == 0) {
                    int id;
                    int x, y, dir;
                    char file[64];
                    sscanf(data + 4, "%d %d %d %d %s", &x, &y, &dir, &id, file);
                    if(s->characters[id] != NULL) {
                        Character_Destroy(s->characters[id]);
                    }
                    s->characters[id] = Character_Create(file, id, false);
                    s->characters[id]->x = x;
                    s->characters[id]->renderX = x;
                    s->characters[id]->x4 = x * 4;
                    s->characters[id]->y = y;
                    s->characters[id]->renderY = y;
                    s->characters[id]->y4 = y * 4;
                    s->characters[id]->direction = dir;
                } else if(strncmp("POS", data, 3) == 0) {
                    int id, x, y, dir;
                    sscanf(data + 4, "%d %d %d %d", &id, &x, &y, &dir);
                    if(s->characters != NULL && s->characters[id] != NULL) {
                        s->characters[id]->x = x;
                        s->characters[id]->y = y;
                        s->characters[id]->direction = dir;
                    }
                } else if(strncmp("PLY", data, 3) == 0) {
                    int id;
                    sscanf(data + 4, "%d", &id);
                    if(s->characters[id] != NULL) {
                        s->player = s->characters[id];
                    }
                } else if(strncmp("PDC", data, 3) == 0) {
                    int id;
                    sscanf(data + 4, "%d", &id);
                    if(s->characters[id] != NULL) {
                        Character_Destroy(s->characters[id]);
                        s->characters[id] = NULL;
                    }
                } else if(strncmp("SSD", data, 3) == 0) {
                    for(int i = 0; i < s->charNumber; i++) {
                        if(s->characters[i] != NULL)
                            Character_Destroy(s->characters[i]);
                    }
                    if(s->characters != NULL) {
                        free(s->characters);
                        free(s->renderCharacters);
                        s->characters = NULL;
                        s->renderCharacters = NULL;
                    }
                    s->charNumber = 0;
                    s->player = NULL;
                    gInfo.connectedToServer = false;
                    Socket_Close(gInfo.sockFd);
                }
            }
        }
    }
    if(s->player != NULL) {
        s->screenX = ((int) s->player->renderX + s->player->sprite->w / 6) - (gInfo.screenWidth) / 2;
        s->screenY = ((int) s->player->renderY + s->player->sprite->h / 8) - (gInfo.screenHeight) / 2;
    }
    if(s->screenX > s->map->width * TILE_SIZE - gInfo.screenWidth) {
        s->screenX = s->map->width * TILE_SIZE - gInfo.screenWidth;
    }
    if(s->screenY > s->map->height * TILE_SIZE - gInfo.screenHeight) {
        s->screenY = s->map->height * TILE_SIZE - gInfo.screenHeight;
    }
    if(s->screenX < 0)
        s->screenX = 0;
    if(s->screenY < 0)
        s->screenY = 0;
    SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    SDL_Rect renderQuad = {s->screenX, s->screenY, gInfo.screenWidth, gInfo.screenHeight};
    int dstWidth, dstHeight;
    if(s->map->width * TILE_SIZE < gInfo.screenWidth)
        dstWidth = s->map->width * TILE_SIZE;
    else
        dstWidth = gInfo.screenWidth;
    if(s->map->height * TILE_SIZE < gInfo.screenHeight)
        dstHeight = s->map->width * TILE_SIZE;
    else
        dstHeight = gInfo.screenHeight;
    SDL_Rect dstRect = {0, 0, dstWidth, dstHeight};
    SDL_RenderCopy(gInfo.renderer, s->map->layers[0], &renderQuad, &dstRect);
    SDL_RenderCopy(gInfo.renderer, s->map->layers[1], &renderQuad, &dstRect);
    //Map_Render(s->map, s->tileMap, s->screenX, s->screenY);

    // MUDAR, TÁ MUITO RUIM (realmente)
    if(s->player != NULL) {
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP]) {
            if(s->player->x == s->player->renderX && s->player->y == s->player->renderY) {
                Character_TryToMove(s->player, 3, s->map, s->characters, s->charNumber);
            }
        } else if(state[SDL_SCANCODE_DOWN]) {
            if(s->player->x == s->player->renderX && s->player->y == s->player->renderY) {
                Character_TryToMove(s->player, 0, s->map, s->characters, s->charNumber);
            }
        } else if(state[SDL_SCANCODE_LEFT]) {
            if(s->player->x == s->player->renderX && s->player->y == s->player->renderY) {
                Character_TryToMove(s->player, 1, s->map, s->characters, s->charNumber);
            }
        } else if(state[SDL_SCANCODE_RIGHT]) {
            if(s->player->x == s->player->renderX && s->player->y == s->player->renderY) {
                Character_TryToMove(s->player, 2, s->map, s->characters, s->charNumber);
            }
        }
    }
    




    // ------------------------------------ //

    //Character_Update(s->player, s->map, s->characters, s->charNumber);
    for(int i = 0; i < s->charNumber; i++) {
        if(s->characters[i] != NULL) {
            Character_Update(s->characters[i], s->map, s->characters, s->charNumber);
            s->renderCharacters[i] = i;
        } else {
            s->renderCharacters[i] = -1;
        }
    }
    qsort(s->renderCharacters, s->charNumber, sizeof(int), compareCharacters);
    //Character_Render(s->player, s->screenX, s->screenY);
    for(int i = 0; i < s->charNumber; i++) {
        if(s->renderCharacters[i] != -1 && s->characters[s->renderCharacters[i]] != NULL) {
            Character_Render(s->characters[s->renderCharacters[i]], s->screenX, s->screenY);
        }
    }
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if(e->type == SDL_KEYDOWN) {
        //Atualiza estado da tecla se o evento dela for mais recente
        /*if(s->player != NULL) {
            if (e->key.timestamp > s->lastTimeStamp) {
                switch (e->key.keysym.scancode) {
                case SDL_SCANCODE_DOWN:
                    s->keyDown = true;
                    if (!s->player->moving) {
                        Character_TryToMove(s->player, 0, s->map, s->characters, s->charNumber);
                    }
                    break;
                case SDL_SCANCODE_UP:
                    s->keyUp = true;
                    if (!s->player->moving) {
                        Character_TryToMove(s->player, 3, s->map, s->characters, s->charNumber);
                    }
                    break;
                case SDL_SCANCODE_LEFT:
                    s->keyLeft = true;
                    if (!s->player->moving) {
                        Character_TryToMove(s->player, 1, s->map, s->characters, s->charNumber);
                    }
                    break;
                case SDL_SCANCODE_RIGHT:
                    s->keyRight = true;
                    if (!s->player->moving) {
                        Character_TryToMove(s->player, 2, s->map, s->characters, s->charNumber);
                    }
                    break;
                default:
                    //s->player->moving = false;
                    break;
                }
                
            }
        }*/





        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if(e->key.keysym.sym == SDLK_F3) {
            gInfo.debug = !gInfo.debug;
        } else if(e->key.keysym.sym == SDLK_F4) {
            s->testServer = Server_Open(3000);
            if(s->testServer != NULL) {
                printf("Server open\n");
                s->serverThread = SDL_CreateThread(Server_InitLoop, "ServerLoop", s->testServer);
            }
        } else if(e->key.keysym.sym == SDLK_F5) {
            if(gInfo.connectedToServer == false)
                gInfo.sockFd = Socket_Open(0);
            if(gInfo.serverAddress == NULL) {
                DestroyAddress(gInfo.serverAddress);
                gInfo.serverAddress = NewAddress(127, 0, 0, 1, 3000);
            }
            char data[] = "CON 1";
            Socket_Send(gInfo.sockFd, gInfo.serverAddress, data, sizeof(data));
            s->waitingConnection = true;
        } else if(e->key.keysym.sym == SDLK_F6) {
            if(s->testServer != NULL && s->testServer->running) {
                s->testServer->running = false;
                int returnValue;
                SDL_WaitThread(s->serverThread, &returnValue);
                printf("Thread closed with return value %d\n", returnValue);
                Server_Destroy(s->testServer);
                s->testServer = NULL;
            }
        }
    }
    /*else if (e->type == SDL_KEYUP) {
        if(s->player != NULL) {
            //Atualiza estado da tecla
            bool acotezeo = false;
            switch (e->key.keysym.scancode) {
            case SDL_SCANCODE_DOWN:
                s->keyDown = false;
                acotezeo = true;
                break;
            case SDL_SCANCODE_UP:
                s->keyUp = false;
                acotezeo = true;
                break;
            case SDL_SCANCODE_LEFT:
                s->keyLeft = false;
                acotezeo = true;
                break;
            case SDL_SCANCODE_RIGHT:
                s->keyRight = false;
                acotezeo = true;
                break;
            }

            if (acotezeo && !s->player->moving) {
                if (s->keyDown) {
                    Character_TryToMove(s->player, 0, s->map, s->characters, s->charNumber);
                }
                else if (s->keyUp) {
                    Character_TryToMove(s->player, 3, s->map, s->characters, s->charNumber);
                }
                else if (s->keyLeft) {
                    Character_TryToMove(s->player, 1, s->map, s->characters, s->charNumber);
                }
                else if (s->keyRight) {
                    Character_TryToMove(s->player, 2, s->map, s->characters, s->charNumber);
                }
            
            }
        }
    }
    if(s->player != NULL) {
        if (!s->keyDown && !s->keyLeft && !s->keyRight && !s->keyUp) {
            //s->player->moving = false;
        }
    }*/


}


void SceneMap_destroy(Scene_Map* s) {
    if(gInfo.connectedToServer) {
        char sendData[] = "DCS";
        Socket_Send(gInfo.sockFd, gInfo.serverAddress, sendData, 4);
        gInfo.connectedToServer = false;
    }
    Socket_Close(gInfo.sockFd);
    WD_TextureDestroy(s->tileMap);
    Map_Destroy(s->map);
    for(int i = 0; i < s->charNumber; i++) {
        if(s->characters[i] != NULL)
            Character_Destroy(s->characters[i]);
    }
    if(s->characters != NULL) {
        free(s->characters);
        free(s->renderCharacters);
    }
    free(s);
}
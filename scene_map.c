#include "scene_map.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->keyLeft = false;
    newScene->keyRight = false;
    newScene->keyDown = false;
    newScene->keyUp = false;
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/tilemaster.png");
    Map_RenderFull(Game.map, newScene->tileMap);
    newScene->bombSprite = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->bombSprite, "content/bomb.png");
    newScene->explosionSprite = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->explosionSprite, "content/explosion.png");
    newScene->wallTexture = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->wallTexture, "content/wall.png");
    newScene->screenX = 0;
    newScene->screenY = 0;
    newScene->player = Game.map->characters[Network.clientId];
    newScene->renderCharacters = malloc(Game.map->charNumber * sizeof(int));
    newScene->waitingConnection = false;
    for(int i = 0; i < 20; i++) {
        newScene->bombs[i].active = false;
        newScene->explosions[i].active = false;
    }
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

Bomb_Render(Bomb* b, int screenX, int screenY, WTexture* bombSprite) {
    if(b->active) {
        WD_TextureRender(bombSprite, (b->x * TILE_SIZE - screenX), (b->y * TILE_SIZE - screenY));
    }
}

Explosion_Render(Explosion* e, int screenX, int screenY, WTexture* explosionSprite) {
    if(e->active && e->explosionCount > 0) {
        e->explosionCount--;
        for(int x = e->xMin; x <= e->xMax; x++) {
            SDL_RendererFlip flip;
            SDL_Rect clip = {0, 0, explosionSprite->w / 3, explosionSprite->h};
            if(x == e->x) {
                flip = SDL_FLIP_NONE;
                clip.x = explosionSprite->w / 3 * 2;
            } else if(x == e->xMin) {
                flip = SDL_FLIP_NONE;
            } else if(x == e->xMax) {
                flip = SDL_FLIP_HORIZONTAL;
            } else {
                flip = SDL_FLIP_NONE;
                clip.x = explosionSprite->w / 3;
            }
            WD_TextureRenderExCustom(explosionSprite, (x * TILE_SIZE - screenX), (e->y * TILE_SIZE - screenY), &clip, 0.0, NULL, flip, TILE_SIZE, TILE_SIZE);
        }
        for(int y = e->yMin; y <= e->yMax; y++) {
            SDL_RendererFlip flip;
            SDL_Rect clip = {0, 0, explosionSprite->w / 3, explosionSprite->h};
            double angle = 90.0;
            if(y == e->y) {
                flip = SDL_FLIP_NONE;
                clip.x = explosionSprite->w / 3 * 2;
            } else if(y == e->yMin) {
                flip = SDL_FLIP_NONE;
            } else if(y == e->yMax) {
                angle = 270.0;
                flip = SDL_FLIP_NONE;
            } else {
                flip = SDL_FLIP_NONE;
                clip.x = explosionSprite->w / 3;
            }
            WD_TextureRenderExCustom(explosionSprite, (e->x * TILE_SIZE - screenX), (y * TILE_SIZE - screenY), &clip, angle, NULL, flip, TILE_SIZE, TILE_SIZE);
        }
    }
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
                } else if(strncmp("BMB", data, 3) == 0) {
                    int id, x, y;
                    sscanf(data + 4, "%d %d %d", &id, &x, &y);
                    s->bombs[id].active = true;
                    s->bombs[id].x = x;
                    s->bombs[id].y = y;
                    Game.map->objects[y][x].exists = true;
                    Game.map->objects[y][x].isWall = false;
                    Game.map->objects[y][x].objId = id;
                    if(s->player != NULL && !s->player->dead) {
                        SDL_Rect bombCollision = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                        SDL_Rect playerCollision;
                        Character_GetCollisionBox(s->player, &playerCollision, 0, 0);
                        if(CheckIntersection(&bombCollision, &playerCollision)) {
                            s->player->bombPassId = id;
                        }
                    }
                } else if(strncmp("EXP", data, 3) == 0) {
                    int id, xMin, xMax, yMin, yMax;
                    sscanf(data + 4, "%d %d %d %d %d", &id, &xMin, &xMax, &yMin, &yMax);
                    s->bombs[id].active = false;
                    Game.map->objects[s->bombs[id].y][s->bombs[id].x].exists = false;
                    s->explosions[id].active = true;
                    s->explosions[id].x = s->bombs[id].x;
                    s->explosions[id].y = s->bombs[id].y;
                    s->explosions[id].xMin = xMin;
                    s->explosions[id].xMax = xMax;
                    s->explosions[id].yMin = yMin;
                    s->explosions[id].yMax = yMax;
                    s->explosions[id].explosionCount = Game.screenFreq * 0.5;
                } else if(strncmp("WDS", data, 3) == 0) {
                    int id;
                    sscanf(data + 4, "%d", &id);
                    Game.map->walls[id].exists = false;
                    int x = Game.map->walls[id].x, y = Game.map->walls[id].y;
                    Game.map->objects[y][x].exists = false;
                } else if(strncmp("DEA", data, 3) == 0) {
                    int id;
                    sscanf(data + 4, "%d", &id);
                    if(Game.map->characters[id] != NULL)
                        Game.map->characters[id]->dead = true;
                }
            }
        }
    }
    // Centralizar a câmera no jogador
    if(s->player != NULL) {
        s->screenX = ((int) s->player->renderX + s->player->sprite->w / 6) - (REFERENCE_WIDTH) / 2;
        s->screenY = ((int) s->player->renderY + s->player->sprite->h / 8) - (REFERENCE_HEIGHT) / 2;
    }
    if(s->screenX > Game.map->width * TILE_SIZE - REFERENCE_WIDTH) {
        s->screenX = Game.map->width * TILE_SIZE - REFERENCE_WIDTH;
    }
    if(s->screenY > Game.map->height * TILE_SIZE - REFERENCE_HEIGHT) {
        s->screenY = Game.map->height * TILE_SIZE - REFERENCE_HEIGHT;
    }
    if(s->screenX < 0)
        s->screenX = 0;
    if(s->screenY < 0)
        s->screenY = 0;
    SDL_SetRenderDrawColor(Game.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(Game.renderer);
    SDL_Rect renderQuad = {s->screenX, s->screenY, REFERENCE_WIDTH, REFERENCE_HEIGHT};
    int dstWidth, dstHeight;
    if(Game.map->width * TILE_SIZE < REFERENCE_WIDTH)
        dstWidth = Game.map->width * TILE_SIZE;
    else
        dstWidth = REFERENCE_WIDTH;
    if(Game.map->height * TILE_SIZE < REFERENCE_HEIGHT)
        dstHeight = Game.map->width * TILE_SIZE;
    else
        dstHeight = REFERENCE_HEIGHT;
    SDL_Rect dstRect = {0, 0, dstWidth, dstHeight};
    // Renderizar as camadas do mapa
    SDL_RenderCopy(Game.renderer, Game.map->layers[0], &renderQuad, &dstRect);
    SDL_RenderCopy(Game.renderer, Game.map->layers[1], &renderQuad, &dstRect);

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

    Map_RenderWalls(Game.map, s->wallTexture, s->screenX, s->screenY);
    
    for(int i = 0; i < 20; i++) {
        Bomb_Render(&s->bombs[i], s->screenX, s->screenY, s->bombSprite);
    }

    for(int i = 0; i < 20; i++) {
        Explosion_Render(&s->explosions[i], s->screenX, s->screenY, s->explosionSprite);
    }

    // Ordenar a lista de renderização dos personagens com base em suas alturas
    qsort(s->renderCharacters, Game.map->charNumber, sizeof(int), compareCharacters);
    
    // Renderizar os personagens
    for(int i = 0; i < Game.map->charNumber; i++) {
        if(s->renderCharacters[i] != -1 && Game.map->characters[s->renderCharacters[i]] != NULL) {
            Character_Render(Game.map->characters[s->renderCharacters[i]], s->screenX, s->screenY);
        }
    }
    SDL_RenderCopy(Game.renderer, Game.map->layers[2], &renderQuad, &dstRect);
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_ESCAPE) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SERVERS);
        } else if(e->key.keysym.sym == SDLK_SPACE) {
            Socket_Send(Network.sockFd, Network.serverAddress, "BMB", 4);
        }
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

        if(e->key.keysym.sym == SDLK_F3) {
            Game.debug = !Game.debug;
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
    WD_TextureDestroy(s->bombSprite);
    WD_TextureDestroy(s->explosionSprite);
    WD_TextureDestroy(s->wallTexture);
    free(s);
}
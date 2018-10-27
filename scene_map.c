#include "scene_map.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));

    // Texturas
    newScene->bg = WD_CreateTexture();
    newScene->bombSprite = WD_CreateTexture();
    newScene->explosionSprite = WD_CreateTexture();
    newScene->wallTexture = WD_CreateTexture();
    newScene->puTexture = WD_CreateTexture();
    newScene->winChar = WD_CreateTexture();
    newScene->loseChar = WD_CreateTexture();
    newScene->winText = WD_CreateTexture();
    newScene->tileMap = WD_CreateTexture();
    newScene->animatedBomb = WD_CreateTexture();
    // Sons
    newScene->bombexp = Mix_LoadWAV("content/bexp.mp3");
    newScene->pickup = Mix_LoadWAV("content/pickup.mp3");
    newScene->winSound = Mix_LoadWAV("content/win.mp3");
    newScene->bombload = Mix_LoadWAV("content/bload.mp3");
    newScene->ded = Mix_LoadWAV("content/dedp.mp3");
    newScene->backgroundMusic = Mix_LoadMUS("content/train.mp3");
    newScene->keyLeft = false;
    newScene->keyRight = false;
    newScene->keyDown = false;
    newScene->keyUp = false;
    newScene->connected = false;
    newScene->ended = false;
    newScene->frozen = false;
    newScene->waitingConnection = false;
    newScene->currentFrame = 0;
    newScene->pingCount = 0;
    newScene->myScore = 0;
    newScene->endOpacity = 0;
    newScene->socketFd = 0;
    newScene->screenX = 0;
    newScene->screenY = 0;
    newScene->player = Game.map->characters[Network.clientId];
    newScene->renderCharacters = malloc(Game.map->charNumber * sizeof(int));
    
    Map_RenderFull(Game.map, newScene->tileMap);

    // Carregar texturas
    WD_TextureLoadFromFile(newScene->bg, "content/bgingame.png");
    WD_TextureLoadFromFile(newScene->animatedBomb, "content/FSD.png");
    WD_TextureLoadFromFile(newScene->tileMap, "content/tilemaster.png");
    WD_TextureLoadFromFile(newScene->bombSprite, "content/bomb.png");
    WD_TextureLoadFromFile(newScene->explosionSprite, "content/explosion.png");
    WD_TextureLoadFromFile(newScene->wallTexture, "content/wall.png");
    WD_TextureLoadFromFile(newScene->puTexture, "content/powerup.png");
    WD_TextureLoadFromFile(newScene->winChar, "content/win.png");
    WD_TextureLoadFromFile(newScene->loseChar, "content/lose.png");

    for (int i = 0; i < 4; i++) {
        newScene->status[i] = WD_CreateTexture();
        if (Game.map->characters[i] != NULL) {
            WD_TextureLoadFromText(newScene->status[i], "Vivo", Game.roboto, (SDL_Color) { 255, 255, 255 });
        } else {
            WD_TextureLoadFromText(newScene->status[i], " ", Game.rankMini, (SDL_Color) { 255, 255, 255 });
        }
    }

    for (int i = 0; i < 20; i++) {
        newScene->bombs[i].active = false;
        newScene->explosions[i].active = false;
    }
    for (int i = 0; i < 4; i++) {
        newScene->placement[i] = WD_CreateTexture();
        newScene->playerNames[i] = WD_CreateTexture();
    }
    Mix_PlayMusic(newScene->backgroundMusic, -1);
    Mix_VolumeMusic(80);
    if (Mix_PausedMusic())
        Mix_ResumeMusic();

    return newScene;
}


int compareCharacters(const void * a, const void * b) {
    Character** chars = Game.map->characters;
    int i = *(int*)a;
    int j = *(int*)b;
    if (i == -1 || chars[i] == NULL) {
        return -1;
    }
    if (j == -1 || chars[j] == NULL) {
        return 1;
    }

    return ( chars[i]->renderY - chars[j]->renderY );
}


Bomb_Render(Bomb* b, int screenX, int screenY, WTexture* bombSprite, int currentFrame) {
    if (b->active) {
        if (currentFrame % 20 == 0) {
            b->frame++;
        }
        SDL_Rect clip = { 0, 64 * b->frame, 64, 64 };
        WD_TextureRenderExCustom(bombSprite, (b->x * TILE_SIZE - screenX), (b->y * TILE_SIZE - screenY), &clip, 0.0, NULL, SDL_FLIP_NONE, 64, 64);
    }
}


Explosion_Render(Explosion* e, int screenX, int screenY, WTexture* explosionSprite) {
    if (e->active && e->explosionCount > 0) {
        e->explosionCount--;
        for (int x = e->xMin; x <= e->xMax; x++) {
            SDL_RendererFlip flip;
            SDL_Rect clip = {0, 0, explosionSprite->w / 3, explosionSprite->h};
            if (x == e->x) {
                flip = SDL_FLIP_NONE;
                clip.x = explosionSprite->w / 3 * 2;
            } else if (x == e->xMin) {
                flip = SDL_FLIP_NONE;
            } else if (x == e->xMax) {
                flip = SDL_FLIP_HORIZONTAL;
            } else {
                flip = SDL_FLIP_NONE;
                clip.x = explosionSprite->w / 3;
            }
            WD_TextureRenderExCustom(explosionSprite, (x * TILE_SIZE - screenX), (e->y * TILE_SIZE - screenY), &clip, 0.0, NULL, flip, TILE_SIZE, TILE_SIZE);
        }
        for (int y = e->yMin; y <= e->yMax; y++) {
            SDL_RendererFlip flip;
            SDL_Rect clip = {0, 0, explosionSprite->w / 3, explosionSprite->h};
            double angle = 90.0;
            if (y == e->y) {
                flip = SDL_FLIP_NONE;
                clip.x = explosionSprite->w / 3 * 2;
            } else if (y == e->yMin) {
                flip = SDL_FLIP_NONE;
            } else if (y == e->yMax) {
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

void SceneMap_Receive(Scene_Map* s) {
    Address sender;
    char data[512];
    while (Socket_Receive(Network.sockFd, &sender, data, sizeof(data)) > 0) {
        Network.lastReceivedCount = 0;
        if (sender.address == Network.serverAddress->address && sender.port == Network.serverAddress->port) {
            if (strcmp("PNG", data) != 0)
                printf("[Client] Received from server: %s\n", data);
            if (strncmp("PSE", data, 3) == 0) {
                s->frozen = !s->frozen;
            // Kick
            } else if (strncmp("KCK", data, 3) == 0) {
                Network.connectedToServer = false;
                Mix_PauseMusic();
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, Network.singleplayer ? SCENE_SINGLEPLAYER : SCENE_SERVERS);
            // Correção de posição
            } else if (strncmp("FIX", data, 3) == 0) {
                uint64_t mId;
                int x, y;
                sscanf(data + 4, "%llu %d %d", &mId, &x, &y);
                if(s->player != NULL) {
                    s->player->x = x;
                    s->player->y = y;
                }
            // Atualização de posição
            } else if (strncmp("POS", data, 3) == 0) {
                int id, x, y, dir;
                sscanf(data + 4, "%d %d %d %d", &id, &x, &y, &dir);
                if(Game.map->characters != NULL && Game.map->characters[id] != NULL) {
                    Game.map->characters[id]->renderX = Game.map->characters[id]->x;
                    Game.map->characters[id]->renderY = Game.map->characters[id]->y;
                    Game.map->characters[id]->x4 = Game.map->characters[id]->x * 4;
                    Game.map->characters[id]->y4 = Game.map->characters[id]->y * 4;
                    Game.map->characters[id]->x = x;
                    Game.map->characters[id]->y = y;
                    Game.map->characters[id]->direction = dir;
                }
            // Desconexão de um jogador
            } else if (strncmp("PDC", data, 3) == 0) {
                int id;
                sscanf(data + 4, "%d", &id);
                if(Game.map->characters[id] != NULL) {
                    Character_Destroy(Game.map->characters[id]);
                    Game.map->characters[id] = NULL;
                }
            // Desligamento do servidor
            } else if (strncmp("SSD", data, 3) == 0) {
                Network.connectedToServer = false;
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, Network.singleplayer ? SCENE_SINGLEPLAYER : SCENE_SERVERS);
            // Colocação de uma bomba
            } else if (strncmp("BMB", data, 3) == 0) {
                int id, x, y;
                sscanf(data + 4, "%d %d %d", &id, &x, &y);
                s->bombs[id].active = true;
                s->bombs[id].frame = 0;
                s->bombs[id].x = x;
                s->bombs[id].y = y;
                Game.map->objects[y][x].exists = true;
                Game.map->objects[y][x].type = OBJ_BOMB;
                Game.map->objects[y][x].objId = id;
                if (s->player != NULL && !s->player->dead) {
                    SDL_Rect bombCollision = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_Rect playerCollision;
                    Character_GetCollisionBox(s->player, &playerCollision, 0, 0);
                    if (CheckIntersection(&bombCollision, &playerCollision)) {
                        s->player->bombPassId = id;
                    }
                }
                Mix_PlayChannel(id, s->bombload, 0);
            // Explosão de uma bomba
            } else if (strncmp("EXP", data, 3) == 0) {
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
                Mix_HaltChannel(id);
                Mix_PlayChannel(id, s->bombexp, 0);
            // Destruição de uma parede
            } else if (strncmp("WDS", data, 3) == 0) {
                int id;
                sscanf(data + 4, "%d", &id);
                Game.map->walls[id].exists = false;
                int x = Game.map->walls[id].x, y = Game.map->walls[id].y;
                Game.map->objects[y][x].exists = false;
            // Aparição de um powerup
            } else if (strncmp("PWU", data, 3) == 0) {
                int id, x, y, type;
                sscanf(data + 4, "%d %d %d %d", &id, &x, &y, &type);
                Game.map->powerups[id].exists = true;
                Game.map->powerups[id].x = x;
                Game.map->powerups[id].y = y;
                Game.map->powerups[id].type = type;
                Game.map->objects[y][x].exists = true;
                Game.map->objects[y][x].type = OBJ_POWERUP;
                Game.map->objects[y][x].objId = id;
            // Escudo em um personagem
            } else if (strncmp("SHI", data, 3) == 0) {
                int id;
                sscanf(data + 4, "%d", &id);
                if(Game.map->characters[id] != NULL) {
                    Game.map->characters[id]->shieldDuration = 2 * Game.screenFreq;
                }
            // Desaparecimento de um powerup
            } else if (strncmp("PWD", data, 3) == 0) {
                int id;
                sscanf(data + 4, "%d", &id);
                Game.map->powerups[id].exists = false;
                Mix_PlayChannel(id*2, s->pickup, 0);
            // Morte de um jogador
            } else if (strncmp("DEA", data, 3) == 0) {
                int id;
                sscanf(data + 4, "%d", &id);
                if (Game.map->characters[id] != NULL)
                    Game.map->characters[id]->dead = true;
                WD_TextureLoadFromText(s->status[id], "Morto", Game.roboto, (SDL_Color) { 255, 255, 255 });
                if (s->player->dead) {
                    Mix_PlayChannel(id*3, s->ded, 0);
                }
            // Número de kills
            } else if (strncmp("KIL", data, 3) == 0) {
                int totaln = 0;
                for (int i = 0; i < 4; i++) {
                    int kills, n;
                    sscanf(data + 4 + totaln, "%d%n", &kills, &n);
                    s->kills[i] = kills;
                    totaln += n;
                }

            // Fim de jogo
            } else if (strncmp("END", data, 3) == 0) {
                int type;
                sscanf(data + 4, "%d", &type);
                // EMPATE: type = 0; VITORIA: type = 1
                int placement = 0;
                WD_TextureLoadFromText(s->winText, "Derrota", Game.win, (SDL_Color) { 255, 172, 65 });
                int totaln = 0, n;
                for (int i = 0; i < Game.map->charNumber; i++) {
                    int id;
                    sscanf(data + 6 + totaln, "%d%n ", &id, &n);
                    totaln += n;
                    if (id != -1) {
                        char ganhador[32];
                        sprintf(ganhador, "#%d: %s", placement + 1, Network.playerNames[id]);
                        WD_TextureLoadFromText(s->playerNames[placement], Network.playerNames[id], Game.roboto, (SDL_Color) { 255, 255, 255 });
                        WD_TextureLoadFromText(s->placement[placement], ganhador, Game.inputFont, (SDL_Color) {255, 255, 255});
                        if (placement == 0 && id == Network.clientId && type == 1) {
                            WD_TextureLoadFromText(s->winText, "Vitória", Game.win, (SDL_Color) { 255, 172, 65 });
                            Mix_PauseMusic();
                            Mix_PlayChannel(-1, s->winSound, 0);
                        }
                        if (type == 0) {
                            WD_TextureLoadFromText(s->winText, "Empate", Game.win, (SDL_Color) { 255, 172, 65 });
                        }
                        if (id == Network.clientId) {
                            switch (placement){
                                case 0:
                                    s->myScore = 8 + (8 * s->kills[id]);
                                    break;
                                case 1:
                                    s->myScore = 4 + (4 * s->kills[id]);
                                    break;
                                case 2:
                                    s->myScore = 2 + (2 * s->kills[id]);
                                    break;
                                case 3:
                                    s->myScore = 1 + s->kills[id];
                                    break;
                            }
                        }
                        s->finalRanking[placement] = id;
                        placement++;
                    }
                }
                for (; placement < Game.map->charNumber; placement++) {
                    WD_TextureLoadFromText(s->placement[placement], " ", Game.inputFont, (SDL_Color) {255, 255, 255});
                    s->finalRanking[placement] = -1;
                }
                s->realPlayer = 0;
                for (int i = 0; i < 4; i++) {
                    if (s->finalRanking[i] != -1) {
                        s->realPlayer++;
                    }
                }
                s->frozen = true;
                s->ended = true;

                if (Network.singleplayer) {
                    // Salvar pontuação no singleplayer
                    FILE* arq;
                    arq = fopen("save.dat", "rb");
                    if (arq == NULL) {
                        arq = fopen("save.dat", "wb");
                        fwrite(&s->myScore, sizeof(int), 1, arq);
                        fclose(arq);
                    } else {
                        int pontuacaoAtual;
                        fread(&pontuacaoAtual, sizeof(int), 1, arq);
                        pontuacaoAtual+=s->myScore;
                        fclose(arq);

                        arq = fopen("save.dat", "wb");
                        if (Game.reset)
                            fwrite(&s->myScore, sizeof(int), 1, arq);
                        else
                            fwrite(&pontuacaoAtual, sizeof(int), 1, arq);
                        fclose(arq);
                    }
                } else {
                    s->connected = false;
                    s->socketFd = TCPSocket_Open();
                    if (s->socketFd != 0)
                        TCPSocket_Connect(s->socketFd, "35.198.20.77", 3122);
                }


            }
        }
    }
    s->pingCount++;
    if (s->pingCount == Game.screenFreq) {
        s->pingCount = 0;
        Socket_Send(Network.sockFd, Network.serverAddress, "PNG", 4);
    }
    Network.lastReceivedCount++;
    if (Network.lastReceivedCount > Game.screenFreq * 5) {
        Mix_PauseMusic();
        SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, Network.singleplayer ? SCENE_SINGLEPLAYER : SCENE_SERVERS);
    }
}


void SceneMap_update(Scene_Map* s) {
    if (!SceneManager.inTransition && Network.connectedToServer)
        SceneMap_Receive(s);
    // Centralizar a câmera no jogador
    if (s->player != NULL) {
        s->screenX = ((int) s->player->renderX + s->player->sprite->w / 6) - (REFERENCE_WIDTH) / 2;
        s->screenY = ((int) s->player->renderY + s->player->sprite->h / 8) - (REFERENCE_HEIGHT) / 2;
    }
    // Verificação se passou do limite do mapa
    if (s->screenX > Game.map->width * TILE_SIZE - REFERENCE_WIDTH) {
        s->screenX = Game.map->width * TILE_SIZE - REFERENCE_WIDTH;
    }
    if (s->screenY > Game.map->height * TILE_SIZE - REFERENCE_HEIGHT) {
        s->screenY = Game.map->height * TILE_SIZE - REFERENCE_HEIGHT;
    }
    if (s->screenX < 0)
        s->screenX = 0;
    if (s->screenY < 0)
        s->screenY = 0;
    SDL_SetRenderDrawColor(Game.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(Game.renderer);
    WD_TextureRender(s->bg, 0, 0);
    SDL_Rect renderQuad = {s->screenX, s->screenY, REFERENCE_WIDTH, REFERENCE_HEIGHT};
    s->screenX -= (REFERENCE_WIDTH - Game.map->width * TILE_SIZE) / 2;
    int dstWidth, dstHeight;
    if (Game.map->width * TILE_SIZE < REFERENCE_WIDTH)
        dstWidth = Game.map->width * TILE_SIZE;
    else
        dstWidth = REFERENCE_WIDTH;
    if (Game.map->height * TILE_SIZE < REFERENCE_HEIGHT)
        dstHeight = Game.map->width * TILE_SIZE;
    else
        dstHeight = REFERENCE_HEIGHT;
    SDL_Rect dstRect = {(REFERENCE_WIDTH - Game.map->width * TILE_SIZE) /2, 0, dstWidth, dstHeight};
    // Renderizar as camadas do mapa
    SDL_RenderCopy(Game.renderer, Game.map->layers[0], &renderQuad, &dstRect);
    SDL_RenderCopy(Game.renderer, Game.map->layers[1], &renderQuad, &dstRect);
    for (int i = 0; i < 4; i++) {
        WD_TextureRender(s->status[i], 1300, 125 + (i * 125));
    }

    // Movimentação do jogador
    if (!s->frozen && s->player != NULL && s->player->x == s->player->renderX && s->player->y == s->player->renderY) {
        if (s->player->moving) {
            bool moved = false;
            switch (s->lastMov) {
                case 'U':
                    moved = Character_TryToMove(s->player, DIR_UP, Game.map);
                    break;
                case 'D':
                    moved = Character_TryToMove(s->player, DIR_DOWN, Game.map);
                    break;
                case 'L':
                    moved = Character_TryToMove(s->player, DIR_LEFT, Game.map);
                    break;
                case 'R':
                    moved = Character_TryToMove(s->player, DIR_RIGHT, Game.map);
                    break;
            }
            if (moved) {
                s->player->lastMovementId++;
                char sendData[32];
                sprintf(sendData, "POS %llu %d %d %d", s->player->lastMovementId, s->player->x, s->player->y, s->player->direction);
                Socket_Send(Network.sockFd, Network.serverAddress, sendData, strlen(sendData) + 1);
            }
        }
    }
    // ------------------------------------ //
    // Atualização dos personagens
    for (int i = 0; i < Game.map->charNumber; i++) {
        if (Game.map->characters[i] != NULL) {
            Character_Update(Game.map->characters[i], Game.map);
            s->renderCharacters[i] = i;
        } else {
            s->renderCharacters[i] = -1;
        }
    }


    // Renderizar paredes
    Map_RenderWalls(Game.map, s->wallTexture, s->screenX, s->screenY);
    // Renderizar PowerUps
    Map_RenderPowerUps(Game.map, s->puTexture, s->screenX, s->screenY, s->currentFrame);
    // Renderizar bombas
    for (int i = 0; i < 20; i++) {
        Bomb_Render(&s->bombs[i], s->screenX, s->screenY, s->animatedBomb, s->currentFrame);
    }
    // Renderizar explosões
    for (int i = 0; i < 20; i++) {
        Explosion_Render(&s->explosions[i], s->screenX, s->screenY, s->explosionSprite);
    }

    // Ordenar a lista de renderização dos personagens com base em suas alturas
    qsort(s->renderCharacters, Game.map->charNumber, sizeof(int), compareCharacters);
    
    // Renderizar os personagens
    for (int i = 0; i < Game.map->charNumber; i++) {
        if (s->renderCharacters[i] != -1 && Game.map->characters[s->renderCharacters[i]] != NULL) {
            Character_Render(Game.map->characters[s->renderCharacters[i]], s->screenX, s->screenY);
        }
    }

    // Renderizar terceira camada
    SDL_RenderCopy(Game.renderer, Game.map->layers[2], &renderQuad, &dstRect);

    // Tela final
    if (s->ended) {
        if (s->endOpacity < 150) {
            s->endOpacity += 150 / Game.screenFreq * 3;
            if (s->endOpacity > 150) {
                s->endOpacity = 150;
            }
        }
        SDL_Rect fillRect = {0, 0, REFERENCE_WIDTH, REFERENCE_HEIGHT};
        SDL_SetRenderDrawColor(Game.renderer, 0, 0, 0, s->endOpacity);
        SDL_RenderFillRect(Game.renderer, &fillRect);

        WD_TextureRender(s->winText, 1440 / 2 - s->winText->w / 2, 500);
        
        for (int i = 0; i < 4; i++) {
            if (i == 0) {
                SDL_Rect winPos = { 0 + (128 * s->finalRanking[i]),0, 128, 128 };
                WD_TextureRenderExCustom(s->winChar, 1440 / 2 - 320 / 2, 200, &winPos, 0.0, NULL, SDL_FLIP_NONE, 320, 320);
                WD_TextureRender(s->playerNames[i], 1440 / 2 - s->playerNames[i]->w/2, 100);
            } else if (s->finalRanking[i]!=-1){
                SDL_Rect losPos = { 0 + (128 * s->finalRanking[i]),0, 128, 128 };
                WD_TextureRenderExCustom(s->loseChar, (1440/s->realPlayer) * (i) - 256/2, 600, &losPos, 0.0, NULL, SDL_FLIP_NONE, 256, 256);
                WD_TextureRender(s->playerNames[i], (1440 / s->realPlayer) * (i) - s->playerNames[i]->w/2, 900);
            }
        }
        for (int i = 0; i < Game.map->charNumber; i++) {
            WD_TextureRender(s->placement[i], 15, 15 + 45 * i);
        }
    }

    if (!s->frozen) {
        s->currentFrame++;
        if (s->currentFrame >= Game.screenFreq) {
            s->currentFrame = 0;
        }
    }
    if (s->socketFd != 0 && !s->connected && Game.logado) {
        int c = TCPSocket_CheckConnectionStatus(s->socketFd);
        if (c == 1) {
            s->connected = true;
            char message[120];
            printf("SCORE: %d", s->myScore);
            sprintf(message, "{\"cmd\":\"submitRank\",\"var\":{\"playerNick\":\"%s\",\"playerScore\":%d, \"login\":\"%s\"}}\n", Game.nome, s->myScore, Game.loginID);
            TCPSocket_Send(s->socketFd, message, strlen(message));
        } else if (c == -1) {
            Socket_Close(s->socketFd);
            s->socketFd = 0;
        }
    }
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_ESCAPE) {
            Mix_PauseMusic();
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, Network.singleplayer ? SCENE_SINGLEPLAYER : SCENE_SERVERS);
        } else if (e->key.keysym.sym == SDLK_SPACE && s->player != NULL && !s->player->dead) {
            Socket_Send(Network.sockFd, Network.serverAddress, "BMB", 4);
        } else if (e->key.keysym.sym == SDLK_p) {
            Socket_Send(Network.sockFd, Network.serverAddress, "PSE", 4);
        }
        //Atualiza estado da tecla se o evento dela for mais recente
        if (s->player != NULL) {
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

        if (e->key.keysym.sym == SDLK_F3) {
            Game.debug = !Game.debug;
        }
    }
    else if (e->type == SDL_KEYUP) {
        if (s->player != NULL) {
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
                } else if (s->keyUp) {
                    s->lastMov = 'U';
                    s->player->moving = true;
                } else if (s->keyLeft) {
                    s->lastMov = 'L';
                    s->player->moving = true;
                } else if (s->keyRight) {
                    s->lastMov = 'R';
                    s->player->moving = true;
                }
            }
        }
    }
    if (s->player != NULL) {
        if (!s->keyDown && !s->keyLeft && !s->keyRight && !s->keyUp) {
            s->player->moving = false;
            s->lastMov = 'N';
        }
    }
}


void SceneMap_destroy(Scene_Map* s) {
    if (Network.connectedToServer) {
        char data[] = "DCS";
        Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
        Network.connectedToServer = false;
    }
    if (Network.serverHost)
        Server_Close(Network.server);
    Map_Destroy(Game.map);
    Game.map = NULL;
    WD_TextureDestroy(s->tileMap);
    if (s->renderCharacters != NULL) {
        free(s->renderCharacters);
    }
    for (int i = 0; i < 4; i++) {
        WD_TextureDestroy(s->placement[i]);
        WD_TextureDestroy(s->playerNames[i]);
        WD_TextureDestroy(s->status[i]);
    }
    Mix_FreeChunk(s->bombexp);
    Mix_FreeChunk(s->bombload);
    Mix_FreeMusic(s->backgroundMusic);
    Mix_FreeChunk(s->winSound);
    Mix_FreeChunk(s->pickup);
    WD_TextureDestroy(s->bg);
    WD_TextureDestroy(s->bombSprite);
    WD_TextureDestroy(s->explosionSprite);
    WD_TextureDestroy(s->winText);
    WD_TextureDestroy(s->winChar);
    WD_TextureDestroy(s->loseChar);
    WD_TextureDestroy(s->wallTexture);
    WD_TextureDestroy(s->animatedBomb);
    WD_TextureDestroy(s->puTexture);
    free(s);
}
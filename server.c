#include "server.h"
#include "noise.h"

Client* Client_New(Address* addr, int id, bool bot) {
    Client* newClient = malloc(sizeof(Client));
    if(bot) {
        newClient->addr = NULL;
    } else {
        newClient->addr = malloc(sizeof(Address));
        newClient->addr->address = addr->address;
        newClient->addr->port = addr->port;
    }
    newClient->bot = bot;
    newClient->id = id;
    newClient->lastMessage = SDL_GetTicks();
    newClient->bombsPlaced = 0;
    newClient->maxBombs = 2;
    newClient->bombRadius = 1;
    newClient->kills = 0;
    return newClient;
}

void Client_Destroy(Client* c, Server* s) {
    if(c->addr != NULL)
        free(c->addr);
    free(c);
}

Server* Server_Open(unsigned short port, char nm[32], bool singleplayer) {
    int sockFd = Socket_Open(port, false);
    if(sockFd != 0) {
        Server* newServer = malloc(sizeof(Server));
        strcpy(newServer->name, nm);
        if(!singleplayer) {
            newServer->listenSockFd = Socket_Open(BROADCAST_PORT, true);
        }
        newServer->singleplayer = singleplayer;
        newServer->port = port;
        newServer->sockfd = sockFd;
        newServer->running = false;
        newServer->delayMs = round(1.0 / SERVER_TICKRATE * 1000);
        newServer->maxClients = MAX_PLAYERS;
        newServer->connectedClients = 0;
        newServer->placements = malloc(newServer->maxClients * sizeof(int));
        newServer->clients = malloc(newServer->maxClients * sizeof(Client*));
        for(int i = 0; i < newServer->maxClients; i++) {
            newServer->clients[i] = NULL;
            newServer->placements[i] = -1;
        }
        newServer->currentPlacement = newServer->maxClients - 1;

        newServer->hostId = -1;
        newServer->inGame = false;
        newServer->gameEnded = false;
        newServer->paused = false;
        newServer->map = Map_Create();
        newServer->bombNumber = MAX_BOMBS_PER_PLAYER * newServer->maxClients;
        newServer->bombs = malloc(newServer->bombNumber * sizeof(ServerBomb));
        for(int i = 0; i < newServer->bombNumber; i++) {
            newServer->bombs[i].active = false;
        }
        newServer->powerupNumber = newServer->bombNumber;
        newServer->powerups = malloc(newServer->powerupNumber * sizeof(ServerPowerUp));
        for(int i = 0; i < newServer->powerupNumber; i++) {
            newServer->powerups[i].exists = false;
        }
        return newServer;
    } else {
        return NULL;
    }
}

void Server_Destroy(Server* s) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL)
            Client_Destroy(s->clients[i], s);
    }
    Map_Destroy(s->map);
    free(s->bombs);
    free(s->powerups);
    free(s->clients);
    free(s->placements);
    free(s);
}

void Server_Close(Server* s) {
    if(s != NULL && s->running) {
        s->running = false;
        int returnValue;
        SDL_WaitThread(Network.serverThread, &returnValue);
        printf("Thread closed with return value %d\n", returnValue);
        Server_Destroy(s);
        Network.server = NULL;
        Network.serverHost = false;
    }
}

void Server_Shutdown(Server* s) {
    char data[] = "SSD";
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL && !s->clients[i]->bot) {
            Socket_Send(s->sockfd, s->clients[i]->addr, data, sizeof(data));
        }
    }
}

int Server_FindClient(Server* s, Address* addr) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL) {
            if(s->clients[i]->addr->address == addr->address && s->clients[i]->addr->port == addr->port) {
                return i;
            }
        }
    }
    return -1;
}

int Server_FindEmptySlot(Server* s) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] == NULL) {
            return i;
        }
    }
    return -1;
}

void Server_PlayerDisconnect(Server* s, int clientId) {
    if(s->clients[clientId] != NULL) {
        char data[16];
        sprintf(data, "PDC %d", clientId);
        Server_SendToAll(s, data, clientId);
        Client_Destroy(s->clients[clientId], s);
        s->clients[clientId] = NULL;
        s->connectedClients--;
    }
}

void Server_KickPlayer(Server* s, int clientId) {
    if(s->clients[clientId] != NULL) {
        char data[] = "KCK";
        Socket_Send(s->sockfd, s->clients[clientId]->addr, data, sizeof(data));
        Server_PlayerDisconnect(s, clientId);
    }
}

void Server_CheckInactiveClients(Server* s) {
    Uint32 now = SDL_GetTicks();
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL && !s->clients[i]->bot) {
            if(now - s->clients[i]->lastMessage > 5000) {
                printf("[Server] 5s passed since last received message from client id %d, kicking\n", i);
                Server_KickPlayer(s, i);
            } else {
                Socket_Send(s->sockfd, s->clients[i]->addr, "PNG", 4);
                if(s->inGame && !s->paused && s->map->characters[i] != NULL) {
                    Server_CharacterUpdate(s, s->map->characters[i], s->map);
                    Server_UpdateCharMovement(s, s->map->characters[i]);
                }
            }
        }
    }
}

void Server_SendToAll(Server* s, char* data, int id) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL && i != id && !s->clients[i]->bot) {
            Socket_Send(s->sockfd, s->clients[i]->addr, data, strlen(data) + 1);
        }
    }
}

void Server_SendCharacters(Server* s, Address* addr, int id) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL && i != id) {
            char sendData[80];
            Character* c = s->map->characters[i];
            sprintf(sendData, "CHR %d %d %d %d %s", c->x, c->y, c->direction, i, c->spriteFile);
            Socket_Send(s->sockfd, addr, sendData, sizeof(sendData));
        }
    }
}

void Server_SendInfos(Server* s, Address* addr) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL) {
            char sendData[40];
            sprintf(sendData, "PNM %d %s", i, s->clients[i]->username);
            Socket_Send(s->sockfd, addr, sendData, sizeof(sendData));
        }
    }
}

void Server_GenerateMap(Server* s) {
    srand(time(NULL));
    int seed = rand() % 100;
    s->map->objects = malloc(s->map->height * sizeof(TemporaryObject*));
    int wallNumber = 0;
    for(int y = 0; y < s->map->height; y++) {
        s->map->objects[y] = malloc(s->map->width * sizeof(TemporaryObject));
        for(int x = 0; x < s->map->width; x++) {
            bool possibleWall = true;
            for(int i = 0; i < s->maxClients; i++) {
                if(s->clients[i] == NULL || s->map->characters[i] == NULL) {
                    continue;
                }
                if(abs(s->map->characters[i]->x / TILE_SIZE - x) + abs(s->map->characters[i]->y / TILE_SIZE - y) < 2) {
                    possibleWall = false;
                    break;
                }
            }
            if(possibleWall && Map_Get(s->map, x, y, 1) != WALL_TILE) {
                float n = perlin2d(x, y, PERLIN_FREQUENCY, PERLIN_DEPTH, seed);
                if(n <= PERLIN_DELIMITER) {
                    s->map->objects[y][x].exists = true;
                    s->map->objects[y][x].type = OBJ_WALL;
                    s->map->objects[y][x].objId = wallNumber;
                    wallNumber++;
                } else {
                    s->map->objects[y][x].exists = false;
                }
            } else {
                s->map->objects[y][x].exists = false;
            }
        }
    }
    char sendData[16];
    sprintf(sendData, "GEN %d %d", seed, wallNumber);
    Server_SendToAll(s, sendData, -1);
}

void Server_CharacterUpdate(Server* s, Character* c, Map* m) {
    if(c->shieldDuration > 0) {
        c->shieldDuration--;
    }
    if(c->x != c->renderX || c->y != c->renderY) {
        c->moving = true;
        int x4 = c->x * 4;
        int y4 = c->y * 4;
        int distance = (1 << c->moveSpeed) * 60.0 / SERVER_TICKRATE;
        if(c->renderX < c->x) {
            if(c->x4 + distance > x4) {
                c->x4 = x4;
            } else {
                c->x4 += distance;
            }
        } else {
            if(c->x4 - distance < x4) {
                c->x4 = x4;
            } else {
                c->x4 -= distance;
            }
        }
        if(c->renderY < c->y) {
            if(c->y4 + distance > y4) {
                c->y4 = y4;
            } else {
                c->y4 += distance;
            }
        } else {
            if(c->y4 - distance < y4) {
                c->y4 = y4;
            } else {
                c->y4 -= distance;
            }
        }
        c->renderX = c->x4 / 4;
        c->renderY = c->y4 / 4;
    }
}

bool Server_CheckMovement(Server* s, int id, int x, int y) {
    if(s->clients[id] == NULL) {
        return false;
    }
    Character* c = s->map->characters[id];
    
    if(c->forcingMovement)
        return false;
    int maximumDistance = (1 << (c->moveSpeed)) * 2 ;
    if(abs(x - c->renderX) > maximumDistance || abs(y - c->renderY) > maximumDistance) {
        return false;
    }

    SDL_Rect collisionBox;
    Character_GetCollisionBox(c, &collisionBox, x - c->x, y - c->y);
    if(Map_Passable(s->map, &collisionBox, c)) {
        bool noCollision = true;
        /*for(int i = 0; i < s->maxClients; i++) {
            if(s->clients[i] == NULL || s->map->characters[i]->dead || i == id)
                continue;
            SDL_Rect otherCollisionBox;
            Character_GetCollisionBox(s->map->characters[i], &otherCollisionBox, 0, 0); 
            if(collisionBox.x < otherCollisionBox.x + otherCollisionBox.w &&
                    collisionBox.x + collisionBox.w > otherCollisionBox.x &&
                    collisionBox.y < otherCollisionBox.y + otherCollisionBox.h &&
                    collisionBox.y + collisionBox.h > otherCollisionBox.y) {
                noCollision = false;
                break;
            }
        }*/
        return noCollision;
    } else {
        return false;
    }
}

void Server_CheckName(Server* s, int cId, int number) {
    bool found = false;
    char compstr[32];
    if(number != 0) {
        sprintf(compstr, "%s %d", s->clients[cId]->username, number);
    } else {
        strcpy(compstr, s->clients[cId]->username);
    }
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL && i != cId) {
            if(strcmp(s->clients[i]->username, compstr) == 0) {
                number++;
                Server_CheckName(s, cId, number);
                found = true;
                break;
            }
        }
    }
    if(!found && number > 0) {
        strcpy(s->clients[cId]->username, compstr);
    }
}

void Server_CreateCharacters(Server* s) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL) {
            if(i == 0) {
                s->map->characters[i] = Character_Create("content/azul.png", i, true);
                Character_Place(s->map->characters[i], 1, 1);
            } else if(i == 1) {
                s->map->characters[i] = Character_Create("content/vermelho.png", i, true);
                Character_Place(s->map->characters[i], 15, 15);
            } else if(i == 2) {
                s->map->characters[i] = Character_Create("content/amarelo.png", i, true);
                Character_Place(s->map->characters[i], 15, 1);
            } else if(i == 3) {
                s->map->characters[i] = Character_Create("content/roxo.png", i, true);
                Character_Place(s->map->characters[i], 1, 15);
            }
            char sendData[80];
            Character* c = s->map->characters[i];
            sprintf(sendData, "CHR %d %d %d %d %s", c->x, c->y, c->direction, i, c->spriteFile);
            Server_SendToAll(s, sendData, -1);
        }
    }
}

void Server_PlaceBomb(Server* s, int clientId) {
    if(s->clients[clientId]->bombsPlaced >= s->clients[clientId]->maxBombs || s->map->characters[clientId]->dead)
        return;
    SDL_Rect box;
    Character_GetCollisionBox(s->map->characters[clientId], &box, 0, 0);
    int bombX = (box.x + box.w / 2) / TILE_SIZE;
    int bombY = (box.y + box.h / 2) / TILE_SIZE;
    if(s->map->objects[bombY][bombX].exists)
        return;
    for(int i = 0; i < s->bombNumber; i++) {
        if(!s->bombs[i].active) {
            s->bombs[i].active = true;
            s->map->objects[bombY][bombX].exists = true;
            s->map->objects[bombY][bombX].type = OBJ_BOMB;
            s->map->objects[bombY][bombX].objId = i;
            s->bombs[i].x = bombX;
            s->bombs[i].y = bombY;
            s->bombs[i].clientId = clientId;
            s->bombs[i].count = 0;
            SDL_Rect bombCollision = {bombX * TILE_SIZE, bombY * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            for(int j = 0; j < s->maxClients; j++) {
                if(s->clients[j] != NULL && !s->map->characters[j]->dead) {
                    SDL_Rect playerCollision;
                    Character_GetCollisionBox(s->map->characters[j], &playerCollision, 0, 0);
                    if(CheckIntersection(&bombCollision, &playerCollision)) {
                        s->map->characters[j]->bombPassId = i;
                    }
                }
            }
            char sendData[32];
            sprintf(sendData, "BMB %d %d %d", i, bombX, bombY);
            Server_SendToAll(s, sendData, -1);
            s->clients[clientId]->bombsPlaced++;
            break;
        }
    }
}

void Server_PlacePowerUp(Server* s, int x, int y) {
    if(s->map->objects[y][x].exists)
        return;
    for(int i = 0; i < s->powerupNumber; i++) {
        if(!s->powerups[i].exists) {
            int type = rand() % TOTAL_POWERUPS;
            s->powerups[i].exists = true;
            s->powerups[i].x = x;
            s->powerups[i].y = y;
            s->powerups[i].type = type;
            s->map->objects[y][x].exists = true;
            s->map->objects[y][x].type = OBJ_POWERUP;
            s->map->objects[y][x].objId = i;
            char sendData[32];
            sprintf(sendData, "PWU %d %d %d %d", i, x, y, type);
            Server_SendToAll(s, sendData, -1);
            break;
        }
    }
}

void Server_DestroyWall(Server* s, int x, int y) {
    char sendData[16];
    sprintf(sendData, "WDS %d", s->map->objects[y][x].objId);
    s->map->objects[y][x].exists = false;
#if PLATFORM == PLATFORM_WINDOWS
    int r = rand() % 4;
#else
    int r = random() % 4;
#endif
    if(r == 0) {
        Server_PlacePowerUp(s, x, y);
    }
    Server_SendToAll(s, sendData, -1);
}

void Server_ExplodeBomb(Server* s, int bId) {
    if(s->bombs[bId].active) {
        ServerBomb* b = &s->bombs[bId];
        Client* c = s->clients[b->clientId];
        c->bombsPlaced--;
        b->active = false;
        int xMax = b->x;
        int xMin = b->x;
        int yMax = b->y;
        int yMin = b->y;
        for(int x = 1; x <= c->bombRadius; x++) {
            TemporaryObject* o = &s->map->objects[b->y][b->x + x];
            if(o->exists) {
                if(o->type == OBJ_WALL)
                    Server_DestroyWall(s, b->x + x, b->y);
                else  
                    Server_ExplodeBomb(s, o->objId);
                xMax++;
                if(o->type != OBJ_POWERUP)
                    break;
            } else if(Map_Get(s->map, b->x + x, b->y, 1) == WALL_TILE) {
                break;
            } else {
                xMax++;
            }
        }
        for(int x = 1; x <= c->bombRadius; x++) {
            TemporaryObject* o = &s->map->objects[b->y][b->x - x];
            if(o->exists) {
                if(o->type == OBJ_WALL)
                    Server_DestroyWall(s, b->x - x, b->y);
                else if(o->type == OBJ_BOMB)
                    Server_ExplodeBomb(s, o->objId);
                xMin--;
                if(o->type != OBJ_POWERUP)
                    break;
            } else if(Map_Get(s->map, b->x - x, b->y, 1) == WALL_TILE) {
                break;
            } else {
                xMin--;
            }
        }
        for(int y = 1; y <= c->bombRadius; y++) {
            TemporaryObject* o = &s->map->objects[b->y + y][b->x];
            if(o->exists) {
                if(o->type == OBJ_WALL)
                    Server_DestroyWall(s, b->x, b->y + y);
                else if(o->type == OBJ_BOMB)
                    Server_ExplodeBomb(s, o->objId);
                yMax++;
                if(o->type != OBJ_POWERUP)
                    break;
            } else if(Map_Get(s->map, b->x, b->y + y, 1) == WALL_TILE) {
                break;
            } else {
                yMax++;
            }
        }
        for(int y = 1; y <= c->bombRadius; y++) {
            TemporaryObject* o = &s->map->objects[b->y - y][b->x];
            if(o->exists) {
                if(o->type == OBJ_WALL)
                    Server_DestroyWall(s, b->x, b->y - y);
                else if(o->type == OBJ_BOMB)
                    Server_ExplodeBomb(s, o->objId);
                yMin--;
                if(o->type != OBJ_POWERUP)
                    break;
            } else if(Map_Get(s->map, b->x, b->y - y, 1) == WALL_TILE) {
                break;
            } else {
                yMin--;
            }
        }
        for(int cId = 0; cId < s->maxClients; cId++) {
            // checar colisão da explosão com o personagem
            if(s->clients[cId] != NULL && !s->map->characters[cId]->dead && s->map->characters[cId]->shieldDuration == 0) {
                SDL_Rect charRect;
                Character_GetCollisionBox(s->map->characters[cId], &charRect, 0, 0);
                SDL_Rect expRect1 = {xMin * TILE_SIZE, b->y * TILE_SIZE, (xMax - xMin + 1) * TILE_SIZE, TILE_SIZE};
                SDL_Rect expRect2 = {b->x * TILE_SIZE, yMin * TILE_SIZE, TILE_SIZE, (yMax - yMin + 1) * TILE_SIZE};
                if(CheckIntersection(&charRect, &expRect1) || CheckIntersection(&charRect, &expRect2)) {
                    s->map->characters[cId]->dead = true;
                    char sendData[16];
                    sprintf(sendData, "DEA %d", cId);
                    Server_SendToAll(s, sendData, -1);
                    s->placements[s->currentPlacement] = cId;
                    s->currentPlacement--;
                    if(cId != c->id)
                        c->kills++;
                }
            }
        }
        s->map->objects[b->y][b->x].exists = false;
        char sendData[64];
        sprintf(sendData, "EXP %d %d %d %d %d", bId, xMin, xMax, yMin, yMax);
        Server_SendToAll(s, sendData, -1);
    }
}

void Server_UpdateBombs(Server* s) {
    for(int i = 0; i < s->bombNumber; i++) {
        if(s->bombs[i].active) {
            ServerBomb* b = &s->bombs[i];
            b->count++;
            if(b->count == SERVER_TICKRATE * 3) {
                Server_ExplodeBomb(s, i);
            }
        }
    }
}

void Server_CheckEnd(Server* s) {
    if(s->currentPlacement <= s->maxClients - s->connectedClients) {
        int alive = 0;
        for(int i = 0; i < s->maxClients; i++) {
            if(s->clients[i] != NULL && !s->map->characters[i]->dead) {
                alive++;
                s->placements[s->currentPlacement] = i;
            }
        }
        s->gameEnded = true;
        s->inGame = false;
        char sendData[32];
        sprintf(sendData, "END %d", alive);
        char sendData2[32] = "KIL";
        for(int i = 0; i < s->maxClients; i++) {
            char addData[4];
            sprintf(addData, " %d", s->placements[i]);
            strcat(sendData, addData);
            int kills = -1;
            if(s->clients[i] != NULL)
                kills = s->clients[i]->kills;
            sprintf(addData, " %d", kills);
            strcat(sendData2, addData);
        }
        Server_SendToAll(s, sendData2, -1);
        Server_SendToAll(s, sendData, -1);
    }
}

void Server_HandleMessage(Server* s, Address* sender, char* buffer) {
    int cId = Server_FindClient(s, sender);
    if(cId != -1) {
        s->clients[cId]->lastMessage = SDL_GetTicks();
        if(s->paused) {
            // Despause
            if(strncmp("PSE", buffer, 3) == 0 && cId == s->hostId && s->inGame) {
                s->paused = false;
                Server_SendToAll(s, "PSE", -1);
            }
        } else {
            // Pause
            if(strncmp("PSE", buffer, 3) == 0 && cId == s->hostId && s->inGame) {
                s->paused = true;
                Server_SendToAll(s, "PSE", -1);
            // Movimentação
            } else if(strncmp("POS", buffer, 3) == 0 && s->inGame) {
                int newX, newY, dir;
                uint64_t movementId;
                sscanf(buffer + 4, "%llu %d %d %d", &movementId, &newX, &newY, &dir);
                if(movementId > s->map->characters[cId]->lastMovementId) {
                    Character* chr = s->map->characters[cId];
                    char sendData[32];
                    if(Server_CheckMovement(s, cId, newX, newY)) {
                        chr->renderX = chr->x;
                        chr->renderY = chr->renderY;
                        chr->x4 = chr->x * 4;
                        chr->y4 = chr->y4 = chr->y * 4;
                        chr->x = newX;
                        chr->y = newY;
                        chr->direction = dir;
                        sprintf(sendData, "POS %d %d %d %d", cId, newX, newY, dir);
                        Server_SendToAll(s, sendData, cId);
                    } else {
                        sprintf(sendData, "FIX %llu %d %d %d", movementId, chr->x, chr->y, chr->bombPassId);
                        Socket_Send(s->sockfd, sender, sendData, strlen(sendData) + 1);
                    }
                    chr->lastMovementId = movementId;
                }
            // Desconexão
            } else if(strncmp("DCS", buffer, 3) == 0) {
                Server_PlayerDisconnect(s, cId);
            // Adição de bot
            } else if(strncmp("BOT", buffer, 3) == 0) {
                if(s->hostId == cId && s->connectedClients < s->maxClients && !s->inGame && !s->gameEnded) {
                    int difficulty;
                    sscanf(buffer + 4, "%d", &difficulty);
                    int id = Server_FindEmptySlot(s);
                    s->clients[id] = Client_New(sender, id, false);
                    s->clients[id]->bot = true;
                    s->clients[id]->b.difficulty = difficulty;
                    strcpy(s->clients[id]->username, "BOT");
                    Server_CheckName(s, id, 0);
                    s->connectedClients++;
                    printf("[Server] Bot added (%d)\n", difficulty);
                    char sendData3[40];
                    sprintf(sendData3, "PNM %d %s", id, s->clients[id]->username);
                    Server_SendToAll(s, sendData3, id);
                }
            // Começar jogo
            } else if(strncmp("STA", buffer, 3) == 0) {
                if(s->hostId == cId && !s->inGame && !s->gameEnded) {
                    s->inGame = true;
                    Server_CreateCharacters(s);
                    Map_Load(s->map, "map.txt", false);
                    char sendData[64];
                    sprintf(sendData, "MAP %s", "map.txt");
                    Server_SendToAll(s, sendData, strlen(sendData) + 1);
                    Server_GenerateMap(s);
                }
            // Colocar bomba
            } else if(strncmp("BMB", buffer, 3) == 0) {
                if(s->inGame) {
                    Server_PlaceBomb(s, cId);
                }
            }
        }
    } else {
        // Solicitar informações do servidor
        if(strncmp("INF", buffer, 3) == 0) {
            if(!s->inGame && !s->gameEnded && !s->singleplayer) {
                char sendData[64];
                sprintf(sendData, "INF %d %d %s", s->connectedClients, s->maxClients, s->name);
                Socket_Send(s->sockfd, sender, sendData, strlen(sendData) + 1);
            }
        // Conexão
        } else if(strncmp("CON", buffer, 3) == 0) {
            if(s->singleplayer && s->connectedClients > 0)
                return;
            int version;
            char username[32];
            int host;
            sscanf(buffer + 4, "%3d %1d %31[^\n]", &version, &host, username);
            printf("[Server] Client trying to connect with version %d\n", version);
            if(s->connectedClients < s->maxClients) {
                if(s->inGame || s->gameEnded) {
                    printf("[Server] Game in Progress!\n");
                    char sendData[] = "ING";
                    Socket_Send(s->sockfd, sender, sendData, sizeof(sendData));
                } else {
                    cId = Server_FindEmptySlot(s);
                    if(host == 1 && s->connectedClients == 0 && sender->address == 2130706433) {
                        s->hostId = cId;
                    }
                    s->clients[cId] = Client_New(sender, cId, false);
                    strcpy(s->clients[cId]->username, username);
                    Server_CheckName(s, cId, 0);
                    s->connectedClients++;
                    printf("[Server] New client connected\n");
                    char sendData[16];
                    sprintf(sendData, "CON %d", cId);
                    Socket_Send(s->sockfd, sender, sendData, sizeof(sendData));
                    char sendData2[8];
                    // Enviar número máximo de players
                    sprintf(sendData2, "CNM %d", s->maxClients);
                    Socket_Send(s->sockfd, sender, sendData2, sizeof(sendData2));
                    Server_SendInfos(s, sender);
                    char sendData3[40];
                    sprintf(sendData3, "PNM %d %s", cId, s->clients[cId]->username);
                    Server_SendToAll(s, sendData3, cId);
                }
            } else {
                printf("[Server] Server is full!\n");
                char sendData[] = "FLL";
                Socket_Send(s->sockfd, sender, sendData, sizeof(sendData));
            }
        }
    }
}

int Server_InitLoop(Server* s) {
    s->running = true;
    Address sender;
    char buffer[512];
    int count = 0;
    while(s->running) {
        uint64_t last = SDL_GetPerformanceCounter();
        int bytes;
        while((bytes = Socket_Receive(s->sockfd, &sender, buffer, sizeof(buffer))) > 0) {
            buffer[bytes] = '\0';
            if(strncmp("PNG", buffer, 3) != 0 && Game.debug)
                printf("[Server] Received from %s (Port: %hu): %s\n", sender.addrString, sender.port, buffer);
            Server_HandleMessage(s, &sender, buffer);
        }
        Server_CheckInactiveClients(s);
        if(s->inGame && !s->paused) {
            Server_CheckEnd(s);
            Server_UpdateBombs(s);
            for(int i = 0; i < s->maxClients; i++) {
                if(s->clients[i] != NULL && s->clients[i]->bot) {
                    Server_UpdateBot(s, i);
                    Server_UpdateCharMovement(s, s->map->characters[i]);
                    Server_CharacterUpdate(s, s->map->characters[i], s->map);
                }
            }
        }
        if(s->listenSockFd != 0) {
            while((bytes = Socket_Receive(s->listenSockFd, &sender, buffer, sizeof(buffer))) > 0) {
                buffer[bytes] = '\0';
                if(strncmp("INF", buffer, 3) == 0) {
                    printf("[Server] Received from %s (Port: %hu): %s\n", sender.addrString, sender.port, buffer);
                    char sendData[64];
                    sprintf(sendData, "INF %d %d %s", s->connectedClients, s->maxClients, s->name);
                    Socket_Send(s->sockfd, &sender, sendData, strlen(sendData) + 1);
                }
            }
        }
        count++;
        if(count == SERVER_TICKRATE) {
            if(Game.debug)
                printf("[Server] Connected clients: %d\n", s->connectedClients);
            count = 0;
        }
        uint64_t now = SDL_GetPerformanceCounter();
        double delay = (double) (now - last) / SDL_GetPerformanceFrequency() * 1000;
        if(delay > s->delayMs) {
            delay = s->delayMs;
        }
        SDL_Delay(s->delayMs - delay);
    }
    Server_Shutdown(s);
    printf("Closing server...\n");
    Socket_Close(s->sockfd);
    if(s->listenSockFd != 0)
        Socket_Close(s->listenSockFd);
    return 0;
}
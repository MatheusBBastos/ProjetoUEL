#include "server.h"
#include "noise.h"

Client* Client_New(Address* addr, int id) {
    Client* newClient = malloc(sizeof(Client));
    newClient->addr = malloc(sizeof(Address));
    newClient->addr->address = addr->address;
    newClient->addr->port = addr->port;
    newClient->id = id;
    newClient->lastMessage = SDL_GetTicks();
    newClient->character = NULL;
    newClient->bombsPlaced = 0;
    newClient->maxBombs = 5;
    newClient->bombRadius = 10;
    return newClient;
}

void Client_Destroy(Client* c) {
    if(c->character != NULL)
        Character_Destroy(c->character);
    free(c->addr);
    free(c);
}

Server* Server_Open(unsigned short port) {
    int sockFd = Socket_Open(port);
    if(sockFd != 0) {
        Server* newServer = malloc(sizeof(Server));
        newServer->port = port;
        newServer->sockfd = sockFd;
        newServer->running = false;
        newServer->delayMs = round(1.0 / SERVER_TICKRATE * 1000);
        newServer->maxClients = MAX_PLAYERS;
        newServer->connectedClients = 0;
        newServer->clients = malloc(newServer->maxClients * sizeof(Client*));
        for(int i = 0; i < newServer->maxClients; i++) {
            newServer->clients[i] = NULL;
        }
        newServer->spawnCoords = malloc(newServer->maxClients * sizeof(Position));
        newServer->spawnCoords[0].x = 1;
        newServer->spawnCoords[0].y = 1;

        newServer->spawnCoords[1].x = 17;
        newServer->spawnCoords[1].y = 1;

        newServer->spawnCoords[2].x = 1;
        newServer->spawnCoords[2].y = 17;

        newServer->spawnCoords[3].x = 17;
        newServer->spawnCoords[3].x = 17;

        newServer->hostId = -1;
        newServer->inGame = false;
        newServer->map = Map_Create();
        strcpy(newServer->name, "SERVER FORTE");
        newServer->bombNumber = MAX_BOMBS_PER_PLAYER * newServer->maxClients;
        newServer->bombs = malloc(newServer->bombNumber * sizeof(ServerBomb));
        for(int i = 0; i < newServer->bombNumber; i++) {
            newServer->bombs[i].active = false;
        }
        newServer->objects = NULL;
        return newServer;
    } else {
        return NULL;
    }
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
        Client_Destroy(s->clients[clientId]);
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
        if(s->clients[i] != NULL) {
            if(now - s->clients[i]->lastMessage > 5000) {
                printf("[Server] 5s passed since last received message from client id %d, kicking\n", i);
                Server_KickPlayer(s, i);
            } else {
                if(s->inGame && s->clients[i]->character != NULL)
                    Character_Update(s->clients[i]->character, s->map);
            }
        }
    }
}

void Server_SendToAll(Server* s, char* data, int id) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL && i != id) {
            Socket_Send(s->sockfd, s->clients[i]->addr, data, strlen(data) + 1);
        }
    }
}

void Server_Shutdown(Server* s) {
    char data[] = "SSD";
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL) {
            Socket_Send(s->sockfd, s->clients[i]->addr, data, sizeof(data));
        }
    }
}

void Server_SendCharacters(Server* s, Address* addr, int id) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL && i != id) {
            char sendData[80];
            Character* c = s->clients[i]->character;
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
    s->objects = malloc(s->map->height * sizeof(TemporaryObject*));
    int wallNumber = 0;
    for(int y = 0; y < s->map->height; y++) {
        s->objects[y] = malloc(s->map->width * sizeof(TemporaryObject));
        for(int x = 0; x < s->map->width; x++) {
            bool possibleWall = true;
            for(int i = 0; i < s->maxClients; i++) {
                if(s->clients[i] == NULL || s->clients[i]->character == NULL) {
                    continue;
                }
                if(abs(s->clients[i]->character->x / TILE_SIZE - x) + abs(s->clients[i]->character->y / TILE_SIZE - y) < 2) {
                    possibleWall = false;
                    break;
                }
            }
            if(possibleWall && Map_Get(s->map, x, y, 1) != 4) {
                float n = perlin2d(x, y, 0.5, 1, seed);
                if(n <= 0.7) {
                    s->objects[y][x].exists = true;
                    s->objects[y][x].isWall = true;
                    s->objects[y][x].objId = wallNumber;
                    wallNumber++;
                } else {
                    s->objects[y][x].exists = false;
                }
            } else {
                s->objects[y][x].exists = false;
            }
        }
    }
    char sendData[16];
    sprintf(sendData, "GEN %d %d", seed, wallNumber);
    Server_SendToAll(s, sendData, -1);
}

bool Server_CheckMovement(Server* s, int id, int x, int y) {
    if(s->clients[id] == NULL) {
        return false;
    }
    Character* c = s->clients[id]->character;
    int maximumDistance = (1 << (c->moveSpeed)) * 2 ;
    printf("%d %d\n", c->renderX, c->renderY);
    if(abs(x - c->renderX) > maximumDistance || abs(y - c->renderY) > maximumDistance) {
        return false;
    }

    SDL_Rect collisionBox;
    Character_GetCollisionBox(c, &collisionBox, x - c->x, y - c->y);
    if(Map_Passable(s->map, &collisionBox)) {
        bool noCollision = true;
        for(int i = 0; i < s->maxClients; i++) {
            if(s->clients[i] == NULL || i == id)
                continue;
            SDL_Rect otherCollisionBox;
            Character_GetCollisionBox(s->clients[i]->character, &otherCollisionBox, 0, 0); 
            if(collisionBox.x < otherCollisionBox.x + otherCollisionBox.w &&
                    collisionBox.x + collisionBox.w > otherCollisionBox.x &&
                    collisionBox.y < otherCollisionBox.y + otherCollisionBox.h &&
                    collisionBox.y + collisionBox.h > otherCollisionBox.y) {
                noCollision = false;
                break;
            }
        }
        return noCollision;
    } else {
        return false;
    }
}

void Server_CheckName(Server* s, int cId, int number) {
    bool found = false;
    char compstr[32];
    if(number != 0) {
        sprintf(compstr, "%s(%d)", s->clients[cId]->username, number);
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
                s->clients[i]->character = Character_Create("content/testcharacter.png", i, false);
                Character_Place(s->clients[i]->character, 1, 1);
            } else if(i == 1) {
                s->clients[i]->character = Character_Create("content/testcharacter.png", i, false);
                Character_Place(s->clients[i]->character, 17, 1);
            } else if(i == 2) {
                s->clients[i]->character = Character_Create("content/testcharacter.png", i, false);
                Character_Place(s->clients[i]->character, 1, 17);
            } else if(i == 3) {
                s->clients[i]->character = Character_Create("content/testcharacter.png", i, false);
                Character_Place(s->clients[i]->character, 17, 17);
            }
            char sendData[80];
            Character* c = s->clients[i]->character;
            sprintf(sendData, "CHR %d %d %d %d %s", c->x, c->y, c->direction, i, c->spriteFile);
            Server_SendToAll(s, sendData, -1);
        }
    }
}

void Server_PlaceBomb(Server* s, int clientId) {
    if(s->clients[clientId]->bombsPlaced >= s->clients[clientId]->maxBombs)
        return;
    SDL_Rect box;
    Character_GetCollisionBox(s->clients[clientId]->character, &box, 0, 0);
    int bombX = (box.x + box.w / 2) / TILE_SIZE;
    int bombY = (box.y + box.h / 2) / TILE_SIZE;
    if(s->objects[bombY][bombX].exists)
        return;
    for(int i = 0; i < s->bombNumber; i++) {
        if(!s->bombs[i].active) {
            s->bombs[i].active = true;
            s->objects[bombY][bombX].exists = true;
            s->objects[bombY][bombX].isWall = false;
            s->objects[bombY][bombX].objId = i;
            s->bombs[i].x = bombX;
            s->bombs[i].y = bombY;
            s->bombs[i].clientId = clientId;
            s->bombs[i].count = 0;
            char sendData[32];
            sprintf(sendData, "BMB %d %d %d", i, bombX, bombY);
            Server_SendToAll(s, sendData, -1);
            s->clients[clientId]->bombsPlaced++;
            break;
        }
    }
}

void Server_DestroyWall(Server* s, int x, int y) {
    char sendData[16];
    sprintf(sendData, "WDS %d", s->objects[y][x].objId);
    s->objects[y][x].exists = false;
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
            TemporaryObject* o = &s->objects[b->y][b->x + x];
            if(o->exists) {
                if(o->isWall)
                    Server_DestroyWall(s, b->x + x, b->y);
                else
                    Server_ExplodeBomb(s, o->objId);
                xMax++;
                break;
            } else if(Map_Get(s->map, b->x + x, b->y, 1) == 4) {
                break;
            } else {
                xMax++;
            }
        }
        for(int x = 1; x <= c->bombRadius; x++) {
            TemporaryObject* o = &s->objects[b->y][b->x - x];
            if(o->exists) {
                if(o->isWall)
                    Server_DestroyWall(s, b->x - x, b->y);
                else
                    Server_ExplodeBomb(s, o->objId);
                xMin--;
                break;
            } else if(Map_Get(s->map, b->x - x, b->y, 1) == 4) {
                break;
            } else {
                xMin--;
            }
        }
        for(int y = 1; y <= c->bombRadius; y++) {
            TemporaryObject* o = &s->objects[b->y + y][b->x];
            if(o->exists) {
                if(o->isWall)
                    Server_DestroyWall(s, b->x, b->y + y);
                else
                    Server_ExplodeBomb(s, o->objId);
                yMax++;
                break;
            } else if(Map_Get(s->map, b->x, b->y + y, 1) == 4) {
                break;
            } else {
                yMax++;
            }
        }
        for(int y = 1; y <= c->bombRadius; y++) {
            TemporaryObject* o = &s->objects[b->y - y][b->x];
            if(o->exists) {
                if(o->isWall)
                    Server_DestroyWall(s, b->x, b->y - y);
                else
                    Server_ExplodeBomb(s, o->objId);
                yMin--;
                break;
            } else if(Map_Get(s->map, b->x, b->y - y, 1) == 4) {
                break;
            } else {
                yMin--;
            }
        }
        for(int cId = 0; cId < s->maxClients; cId++) {
            if(s->clients[cId] != NULL) {
                // checar colisão da explosão com o personagem
            }
        }
        s->objects[b->y][b->x].exists = false;
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

void Server_HandleMessage(Server* s, Address* sender, char* buffer) {
    int cId = Server_FindClient(s, sender);
    if(cId != -1) {
        s->clients[cId]->lastMessage = SDL_GetTicks();
        if(strncmp("POS", buffer, 3) == 0) {
            int newX, newY, dir;
            uint64_t movementId;
            sscanf(buffer + 4, "%llu %d %d %d", &movementId, &newX, &newY, &dir);
            if(movementId > s->clients[cId]->character->lastMovementId) {
                Character* chr = s->clients[cId]->character;
                char sendData[32];
                if(Server_CheckMovement(s, cId, newX, newY)) {
                    chr->x = newX;
                    chr->y = newY;
                    chr->direction = dir;
                    sprintf(sendData, "POS %d %d %d %d", cId, newX, newY, dir);
                    Server_SendToAll(s, sendData, cId);
                } else {
                    sprintf(sendData, "FIX %llu %d %d", movementId, chr->x, chr->y);
                    Socket_Send(s->sockfd, sender, sendData, strlen(sendData) + 1);
                }
                chr->lastMovementId = movementId;
            }
        } else if(strncmp("DCS", buffer, 3) == 0) {
            Server_PlayerDisconnect(s, cId);
        } else if(strncmp("STA", buffer, 3) == 0) {
            if(s->hostId == cId && !s->inGame) {
                s->inGame = true;
                Server_CreateCharacters(s);
                Map_Load(s->map, "map.txt", false);
                char sendData[64];
                sprintf(sendData, "MAP %s", "map.txt");
                Server_SendToAll(s, sendData, strlen(sendData) + 1);
                Server_GenerateMap(s);
            }
        } else if(strncmp("BMB", buffer, 3) == 0) {
            if(s->inGame)
                Server_PlaceBomb(s, cId);
        }
    } else {
        if(strncmp("INF", buffer, 3) == 0) {
            char sendData[64];
            sprintf(sendData, "INF %d %d %s", s->connectedClients, s->maxClients, s->name);
            Socket_Send(s->sockfd, sender, sendData, strlen(sendData) + 1);
        } else if(strncmp("CON", buffer, 3) == 0) {
            int version;
            char username[32];
            int host;
            sscanf(buffer + 4, "%3d %1d %31[^\n]", &version, &host, username);
            printf("[Server] Client trying to connect with version %d\n", version);
            if(s->connectedClients < s->maxClients) {
                if(s->inGame) {
                    printf("[Server] Game in Progress!\n");
                    char sendData[] = "ING";
                    Socket_Send(s->sockfd, sender, sendData, sizeof(sendData));
                } else {
                    cId = Server_FindEmptySlot(s);
                    if(host == 1 && s->connectedClients == 0 && sender->address == 2130706433) {
                        s->hostId = cId;
                        printf("carai eh o xoris\n");
                    }
                    s->clients[cId] = Client_New(sender, cId);
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
            if(strncmp("PNG", buffer, 3) != 0)
                printf("[Server] Received from %s (Port: %hu): %s\n", sender.addrString, sender.port, buffer);
            Server_HandleMessage(s, &sender, buffer);
        }
        Server_CheckInactiveClients(s);
        Server_UpdateBombs(s);
        count++;
        if(count == SERVER_TICKRATE) {
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
    return 0;
}

void Server_Destroy(Server* s) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL)
            Client_Destroy(s->clients[i]);
    }
    if(s->objects != NULL) {
        for(int i = 0; i < s->map->height; i++) {
            free(s->objects[i]);
        }
        free(s->objects);
    }
    Map_Destroy(s->map);
    free(s->clients);
    free(s->spawnCoords);
    free(s);
}
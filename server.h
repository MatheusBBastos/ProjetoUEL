#ifndef SERVER_H
#define SERVER_H

#include "headers.h"
#include "network.h"
#include "character.h"
#include "map.h"

typedef struct Client {
    Address* addr;
    int id;
    Uint32 lastMessage;
    Character* character;
    uint64_t lastMovementId;
    char username[32];
    int bombsPlaced;
    int maxBombs;
    int bombRadius;
} Client;

Client* Client_New(Address* addr, int id);

void Client_Destroy(Client* c);

typedef struct ServerBomb {
    bool active;
    int count;
    int clientId;
    int x, y;
} ServerBomb;

typedef struct Position {
    int x, y;
} Position;

typedef struct Server {
    unsigned short port;
    int sockfd;
    bool running;
    int delayMs;
    Client** clients;
    int connectedClients, maxClients;
    Map* map;
    int hostId;
    bool inGame;
    char name[32];
    ServerBomb *bombs;
    TemporaryObject **objects;
    Position *spawnCoords;
    int bombNumber;
} Server;

Server* Server_Open(unsigned short port);

void Server_Close(Server* s);

int Server_FindClient(Server* s, Address* addr);

int Server_FindEmptySlot(Server* s);

void Server_PlayerDisconnect(Server* s, int clientId);

void Server_KickPlayer(Server* s, int clientId);

void Server_CheckInactiveClients(Server* s);

void Server_SendToAll(Server* s, char* data, int id);

void Server_Shutdown(Server* s);

void Server_SendCharacters(Server* s, Address* addr, int id);

bool Server_CheckMovement(Server* s, int id, int x, int y);

void Server_CreateCharacters(Server* s);

void Server_HandleMessage(Server* s, Address* sender, char* buffer);

int Server_InitLoop(Server* s);

void Server_Destroy(Server* s);

#endif
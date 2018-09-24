#ifndef SERVER_H
#define SERVER_H

#include "headers.h"
#include "network.h"

typedef struct Client {
    Address* addr;
    int id;
    Uint32 lastMessage;
} Client;

Client* Client_New(Address* addr, int id);

void Client_Destroy(Client* c);

typedef struct Server {
    unsigned short port;
    int sockfd;
    bool running;
    int delayMs;
    Client** clients;
    int connectedClients, maxClients;
} Server;

Server* Server_Open(unsigned short port);

int Server_FindClient(Server* s, Address* addr);

int Server_FindEmptySlot(Server* s);

void Server_KickPlayer(Server* s, int clientId);

void Server_CheckInactiveClients(Server* s);

void Server_SendToAll(Server* s, char* data, int id);

void Server_Shutdown(Server* s);

void Server_HandleMessage(Server* s, Address* sender, char* buffer);

int Server_InitLoop(Server* s);

void Server_Destroy(Server* s);

#endif
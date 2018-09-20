#ifndef SERVER_H
#define SERVER_H

#include "headers.h"
#include "network.h"

typedef struct Server {
    unsigned short port;
    int sockfd;
    bool running;
} Server;

Server* Server_Open(unsigned short port);

void Server_InitLoop(Server* s);

void Server_Destroy(Server* s);

#endif
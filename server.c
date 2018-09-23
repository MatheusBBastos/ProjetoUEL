#include "server.h"

Client* Client_New(Address* addr, int id) {
    Client* newClient = malloc(sizeof(Client));
    newClient->addr = malloc(sizeof(Address));
    newClient->addr->address = addr->address;
    newClient->addr->port = addr->port;
    newClient->id = id;
    newClient->lastMessage = SDL_GetTicks();
    return newClient;
}

void Client_Destroy(Client* c) {
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
        newServer->maxClients = 4;
        newServer->connectedClients = 0;
        newServer->clients = malloc(newServer->maxClients * sizeof(Client*));
        for(int i = 0; i < newServer->maxClients; i++) {
            newServer->clients[i] = NULL;
        }
        return newServer;
    } else {
        return NULL;
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

void Server_KickPlayer(Server* s, int clientId) {
    if(s->clients[clientId] != NULL) {
        char data[] = "KCK";
        Socket_Send(s->sockfd, s->clients[clientId]->addr, data, sizeof(data));
        Client_Destroy(s->clients[clientId]);
        s->clients[clientId] = NULL;
        s->connectedClients--;
    }
}

void Server_CheckInactiveClients(Server* s) {
    Uint32 now = SDL_GetTicks();
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL) {
            if(now - s->clients[i]->lastMessage > 5000) {
                printf("5s passed since last received message from client id %d, kicking\n", i);
                Server_KickPlayer(s, i);
            }
        }
    }
}

void Server_HandleMessage(Server* s, Address* sender, char* buffer) {
    int cId = Server_FindClient(s, sender);
    if(cId != -1) {
        s->clients[cId]->lastMessage = SDL_GetTicks();
    } else {
        if(strncmp("CON", buffer, 3) == 0) {
            int version;
            sscanf(buffer + 4, "%3d", &version);
            printf("Client trying to connect with version %d\n", version);
            if(s->connectedClients < s->maxClients) {
                cId = Server_FindEmptySlot(s);
                s->clients[cId] = Client_New(sender, cId);
                s->connectedClients++;
                printf("New client connected\n");
                char sendData[] = "CON";
                Socket_Send(s->sockfd, sender, sendData, sizeof(sendData));
            } else {
                printf("Server is full!\n");
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
        while(Socket_Receive(s->sockfd, &sender, buffer, sizeof(buffer)) > 0) {
            if(strncmp("PNG", buffer, 3) != 0)
                printf("[Server] Received from %s (Port: %hu): %s\n", sender.addrString, sender.port, buffer);
            Server_HandleMessage(s, &sender, buffer);
        }
        Server_CheckInactiveClients(s);
        count++;
        if(count == SERVER_TICKRATE) {
            printf("Connected clients: %d\n", s->connectedClients);
            count = 0;
        }
        uint64_t now = SDL_GetPerformanceCounter();
        double delay = (double) (now - last) / SDL_GetPerformanceFrequency() * 1000;
        if(delay > s->delayMs) {
            delay = s->delayMs;
        }
        SDL_Delay(s->delayMs - delay);
    }
    printf("Closing server...\n");
    Socket_Close(s->sockfd);
    return 0;
}

void Server_Destroy(Server* s) {
    for(int i = 0; i < s->maxClients; i++) {
        if(s->clients[i] != NULL)
            Client_Destroy(s->clients[i]);
    }
    free(s->clients);
    free(s);
}
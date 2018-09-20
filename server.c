#include "server.h"

Server* Server_Open(unsigned short port) {
    int sockFd = Socket_Open(port);
    if(sockFd != 0) {
        Server* newServer = malloc(sizeof(Server));
        newServer->port = port;
        newServer->sockfd = sockFd;
        newServer->running = false;
        return newServer;
    } else {
        return NULL;
    }
}

void Server_InitLoop(Server* s) {
    s->running = true;
    Address sender;
    char buffer[512];
    int count = 0;
    while(s->running) {
        uint64_t last = SDL_GetPerformanceCounter();
        while(Socket_Receive(s->sockfd, &sender, buffer, sizeof(buffer)) > 0) {
            printf("[Server] Received from %u (Port: %hu): %s\n", sender.address, sender.port, buffer);
            //Server_HandleMessage(&sender, buffer);
        }
        uint64_t now = SDL_GetPerformanceCounter();
        count++;
        if(count == 60) {
            printf(".\n");
            count = 0;
        }
        double delay = (double) (now - last) / SDL_GetPerformanceFrequency() * 1000;
        if(delay > 17) {
            delay = 17;
        }
        SDL_Delay(17 - delay);
    }
    printf("Closing server...\n");
    Socket_Close(s->sockfd);
}

void Server_Destroy(Server* s) {
    free(s);
}
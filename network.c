#include "network.h"

bool Network_InitSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
    #else
    return true;
    #endif
}

void Network_ShutdownSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
    #endif
}

Address* NewAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port) {
    Address* newAddr = malloc(sizeof(Address));
    newAddr->address = (a << 24) | (b << 16) | (c << 8) | d;
    newAddr->port = port;
    sprintf(newAddr->addrString, "%u.%u.%u.%u", a, b, c, d);
    return newAddr;
}

void DestroyAddress(Address* addr) {
    free(addr);
}

int Socket_Open(unsigned short port) {
    int socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socketFd <= 0) {
        printf("Falha ao criar socket!\n");
        return 0;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short) port);
    if(bind(socketFd, (const struct sockaddr*) &address, sizeof(struct sockaddr_in)) < 0) {
        printf("Falha ao dar bind na socket!\n");
        return 0;
    }
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        int a = 1;
        if(fcntl(socketFd, F_SETFL, O_NONBLOCK, a) == -1) {
            printf("Falha na criação da socket\n");
            return 0;
        }
    #elif PLATFORM == PLATFORM_WINDOWS
        DWORD a = 1;
        if(ioctlsocket(socketFd, FIONBIO, &a) != 0) {
            printf("Falha na criação da socket\n");
            return 0;
        }
    #endif
    return socketFd;
}

bool Socket_Send(int socketFd, Address* destination, void* data, int size) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(destination->address);
    addr.sin_port = htons(destination->port);
    int sent = sendto(socketFd, (const char*) data, size, 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if(sent != size) {
        printf("Falha ao enviar pacote\n");
        return false;
    }
}

int Socket_Receive(int socketFd, Address* sender, char* data, int size) {
    struct sockaddr_in from;
    #if PLATFORM == PLATFORM_WINDOWS
    int fromLength = sizeof(from);
    #else
    socklen_t fromLength = sizeof(from);
    #endif
    int bytes = recvfrom(socketFd, data, size, 0, (struct sockaddr*) &from, &fromLength);
    if(bytes > 0) {
        sender->address = ntohl(from.sin_addr.s_addr);
        sender->port = ntohs(from.sin_port);
        unsigned char ipParts[4];
        ipParts[0] = sender->address & 0xFF;
        ipParts[1] = (sender->address >> 8) & 0xFF;
        ipParts[2] = (sender->address >> 16) & 0xFF;
        ipParts[3] = (sender->address >> 24) & 0xFF;
        sprintf(sender->addrString, "%d.%d.%d.%d", ipParts[3], ipParts[2], ipParts[1], ipParts[0]);
    }
    return bytes;
}

void Socket_Close(int socketFd) {
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        close(socketFd);
    #elif PLATFORM == PLATFORM_WINDOWS
        closesocket(socketFd);
    #endif
}
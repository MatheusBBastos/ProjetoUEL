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

int TCPSocket_Open() {
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFd <= 0) {
        printf("Falha ao criar socket!\n");
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

void TCPSocket_Connect(int socketFd, char* addr, unsigned short port) {
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    connect(socketFd, (struct sockaddr*) &server, sizeof(server));
}

int TCPSocket_CheckConnectionStatus(int socketFd) {
    fd_set writeSet;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO(&writeSet);
    FD_SET(socketFd, &writeSet);
    int r = select(socketFd + 1, NULL, &writeSet, NULL, &timeout);
    if(r < 0) {
        printf("Socket select error\n");
        return -1;
    } else if(r == 0) {
        return 0;
    } else {
        struct sockaddr_in junk;
        int length = sizeof(junk);
        memset(&junk, 0, sizeof(junk));
        if(getpeername(socketFd, (struct sockaddr *)&junk, &length) == 0) {
            return 1;
        } else {
            return -1;
        }
    }
}

int TCPSocket_Send(int socketFd, void* data, int size) {
    int sent = send(socketFd, data, size, 0);
    if(sent != size) {
        return -1;
    } else {
        return 0;
    }
}

int TCPSocket_Receive(int socketFd, char* data, int size) {
    fd_set readSet;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO(&readSet);
    FD_SET(socketFd, &readSet);
    int r = select(socketFd + 1, &readSet, NULL, NULL, &timeout);
    if(r < 0) {
        printf("Socket select error\n");
        return -1;
    } else if(r == 0) {
        return 0;
    } else {
        int bytes = recv(socketFd, data, size, 0);
        if(bytes == 0) {
            return -1;
        } else {
            return bytes;
        }
    }
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
    return true;
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
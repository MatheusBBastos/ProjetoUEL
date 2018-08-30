#ifndef NETWORK_H
#define NETWORK_H

#include "headers.h"

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
    #pragma comment(lib, "wsock32.lib")
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <fcntl.h>
#endif

bool Network_InitSockets();

void Network_ShutdownSockets();

typedef struct Address {
    unsigned int address;
    unsigned short port;
} Address;

Address* NewAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
Address* NewAddressJoined(unsigned int address, unsigned short port);
void DestroyAddress(Address* addr);

int Socket_Open(unsigned short port);
void Socket_Close(int socketFd);
bool Socket_Send(int socketFd, Address* destination, void* data, int size);
int Socket_Receive(int socketFd, Address* sender, char* data, int size);

#endif
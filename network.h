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
    #include <sys/select.h>
#endif

// Inicia o WSA no Windows, não faz nada em outros sistemas
bool Network_InitSockets();
// Desliga o WSA no Windows, não faz nada em outros sistemas
void Network_ShutdownSockets();

// Estrutura que carrega um endereço IP e uma porta no formato aceito pelo sistema
typedef struct Address {
    unsigned int address;
    unsigned short port;
    char addrString[20];
} Address;

Address* NewAddress(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
Address* NewAddressJoined(unsigned int address, unsigned short port);
void DestroyAddress(Address* addr);

// Cria uma socket TCP no modo non-blocking e retorna seu descriptor; retorna 0 se houve falha
int TCPSocket_Open();
// Tenta se conectar ao endereço IP (em forma de string) e porta fornecidos
void TCPSocket_Connect(int socketFd, char* addr, unsigned short port);
// Checa o estado da conexão de uma socket TCP; retorna -1 se falhou, 0 se ainda não houve resposta e 1 se houve sucesso
int TCPSocket_CheckConnectionStatus(int socketFd);
// Envia dados através de uma socket TCP conectada; retorna -1 se falhou, e 0 se houve sucesso
int TCPSocket_Send(int socketFd, void* data, int size);
// Recebe dados de uma socket TCP; retorna -1 se a conexão foi fechada, 0 se não recebeu dados e o número de bytes se recebeu
int TCPSocket_Receive(int socketFd, char* data, int size);

// Cria uma socket UDP no modo non-blocking na porta fornecida, retornando seu descriptor; retorna 0 se houve falha
int Socket_Open(unsigned short port, bool reuse);
// Fecha uma socket (UDP ou TCP)
void Socket_Close(int socketFd);
// Envia dados através de uma socket UDP para um destinatário fornecido
bool Socket_Send(int socketFd, Address* destination, void* data, int size);
// Recebe dados em uma socket UDP, retornando os bytes recebidos e armazenando o endereço de quem os enviou no ponteiro 'sender' fornecido
int Socket_Receive(int socketFd, Address* sender, char* data, int size);

#endif
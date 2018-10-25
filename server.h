#ifndef SERVER_H
#define SERVER_H

#include "headers.h"
#include "network.h"
#include "character.h"
#include "map.h"

enum Difficulties {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
};

typedef struct BotInfo {
    uint8_t difficulty;
} BotInfo;

typedef struct Client {
    Address* addr;
    int id;
    bool bot;
    Uint32 lastMessage;
    uint64_t lastMovementId;
    char username[32];
    int bombsPlaced;
    int maxBombs;
    int bombRadius;
    int kills;
    BotInfo b;
} Client;

Client* Client_New(Address* addr, int id, bool bot);

void Client_Destroy(Client* c, Server* s);

typedef struct ServerBomb {
    bool active;
    int count;
    int clientId;
    int x, y;
} ServerBomb;

typedef struct Position {
    int x, y;
} Position;

enum PowerUps {
    PU_BLAST_RADIUS,
    PU_PLUS_BOMB,
    PU_SHIELD,
    PU_SPEED,
    TOTAL_POWERUPS
};

typedef struct ServerPowerUp {
    int x, y, type;
    bool exists;
} ServerPowerUp;

typedef struct Server {
    unsigned short port;
    int sockfd;
    int listenSockFd;
    bool running;
    int delayMs;
    Client** clients;
    int connectedClients, maxClients;
    Map* map;
    int hostId;
    bool inGame;
    char name[32];
    ServerBomb *bombs;
    int bombNumber;
    ServerPowerUp *powerups;
    int powerupNumber;
} Server;

// Abrir servidor na porta informada
Server* Server_Open(unsigned short port, char nm[32]);
// Loop principal do servidor
int Server_InitLoop(Server* s);
// Desalocar servidor
void Server_Destroy(Server* s);
// Fechar servidor
void Server_Close(Server* s);
// Enviar para os clientes que o servidor desligou
void Server_Shutdown(Server* s);

// Achar o cliente correspondente ao endereço informado
int Server_FindClient(Server* s, Address* addr);
// Procurar por um espaço vazio
int Server_FindEmptySlot(Server* s);
// Desconexão de um jogador
void Server_PlayerDisconnect(Server* s, int clientId);
// Expulsar jogador
void Server_KickPlayer(Server* s, int clientId);
// Checa o nome de um jogador
void Server_CheckName(Server* s, int cId, int number);

// Checagem de clientes inativos
void Server_CheckInactiveClients(Server* s);
// Enviar mensagem para todos os clientes, com exceção do informado em cId
void Server_SendToAll(Server* s, char* data, int id);

// Enviar informações dos personagens para um cliente
void Server_SendCharacters(Server* s, Address* addr, int id);
// Checa a movimentação feita por um jogador, retornando se foi ilegal ou não
bool Server_CheckMovement(Server* s, int id, int x, int y);
// Lida com uma mensagem enviada para o servidor
void Server_HandleMessage(Server* s, Address* sender, char* buffer);

// Cria os personagens
void Server_CreateCharacters(Server* s);
// Gera as paredes destrutíveis do mapa
void Server_GenerateMap(Server* s);

// Atualiza as bombas
void Server_UpdateBombs(Server* s);
// Explode uma bomba
void Server_ExplodeBomb(Server* s, int bId);
// Destrói uma parede
void Server_DestroyWall(Server* s, int x, int y);
// Posiciona uma bomba
void Server_PlaceBomb(Server* s, int clientId);
// Posiciona um PowerUp
void Server_PlacePowerUp(Server* s, int x, int y);

/* ---- FUNÇÕES DOS BOTS ---- */
// Atualização da rota de movimento do personagem
void Server_UpdateCharMovement(Server* s, Character* c);
// Atualiza um bot
void Server_UpdateBot(Server* s, int id);

#endif
#ifndef SCENE_SERVERS_H
#define SCENE_SERVERS_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"
#include "network.h"

typedef struct ServerInfo {
    char text[32], num[10];
    Address addr;
} ServerInfo;

typedef struct Scene_Servers {
    WTexture* loading;
    WTexture* nome;
    WTexture* mutiplayer;
    WTexture* backgroundTexture;
    WTexture* server;
    WTexture* entrar;
    WTexture* servir[2];
    WTexture* serverName[4];
    WTexture* serverSlot[4];
    WTexture* voltar;
    TextBox* boxIp;
    TextBox* boxNome;
    bool waitingConnection;
    bool receivingInfo;
    bool esquerda;
    bool needServersRefresh;
    bool inGame;
    bool connected_Online, dataReceived_Online;
    short int maxServers;
    short int indexd, indexe, posTela, indexShow;
    int numServers;
    int receiveSock;
    int receivingTimeout;
    int connectionTimeout;
    int frame;
    int socketFd_Online;
    char string1[20], string2[20], string3[20];
    SDL_Rect renderQuad;
    ServerInfo* servers;

} Scene_Servers;

Scene_Servers* SceneServers_new();

void SceneServers_update(Scene_Servers* s);

void SceneServers_handleEvent(Scene_Servers* s, SDL_Event* e);

void SceneServers_RefreshList(Scene_Servers* s);

void SceneServers_destroy(Scene_Servers* s);

#endif
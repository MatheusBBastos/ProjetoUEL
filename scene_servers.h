#ifndef SCENE_SERVERS_H
#define SCENE_SERVERS_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"
#include "network.h"

typedef struct ServerInfo {
    char text[32];
    Address addr;
} ServerInfo;

typedef struct Scene_Servers {
    int numServers; //tirar dps
    bool waitingConnection;
    int receiveSock;
    bool receivingInfo;
    int receivingTimeout;
    int connectionTimeout;
    int frame;
    short int indexd, indexe, page, posTela, indexShow;
    bool esquerda;
    bool needServersRefresh;
    ServerInfo* servers;
    char string1[20], string2[20], string3[20];
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
    SDL_Rect renderQuad;
} Scene_Servers;

Scene_Servers* SceneServers_new();

void SceneServers_update(Scene_Servers* s);

void SceneServers_handleEvent(Scene_Servers* s, SDL_Event* e);

void SceneServers_destroy(Scene_Servers* s);

#endif
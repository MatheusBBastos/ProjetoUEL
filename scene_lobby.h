#ifndef SCENE_LOBBY_H
#define SCENE_LOBBY_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Lobby {
    int frame;
    short int index, page;
    bool esquerda;
    char string1[20], string2[20], string3[20];
    WTexture* nome;
    WTexture* mutiplayer;
    WTexture* backgroundTexture;
    WTexture* server;
    WTexture* entrar;
    WTexture* servir;
    WTexture* server1;
    WTexture* server2;
    WTexture* server3;
    WTexture* nomeServer1;
    WTexture* nomeServer2;
    WTexture* nomeServer3;
    TextBox* boxIp;
    SDL_Rect renderQuad;
} Scene_Lobby;

Scene_Lobby* SceneLobby_new();

void SceneLobby_update(Scene_Lobby* s);

void SceneLobby_handleEvent(Scene_Lobby* s, SDL_Event* e);

void SceneLobby_destroy(Scene_Lobby* s);

#endif
#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_MainMenu {
    WTexture* backgroundTexture;
    SDL_Rect renderQuad;
    short int index;
    int socketFd;
    bool connected, dataReceived;
    bool btnJogar;
    bool mult;
    int frame;
    bool animation;
    WTexture* seta;
    WTexture* bemvindo;
    WTexture* nome;
    WTexture* jogar;  
    WTexture* tutorial;
    WTexture* logout;
    WTexture* rankName[6];
    WTexture* scores[6];
    WTexture* posMark[6];
    WTexture* multiplayer;
    WTexture* animatedChar;
    WTexture* singleplayer;
    WTexture* rankPOS; 
    char ranking[100];
} Scene_MainMenu;

Scene_MainMenu * SceneMainMenu_new();

void SceneMainMenu_update(Scene_MainMenu* s);

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e);

void SceneMainMenu_destroy(Scene_MainMenu* s);

int getRank(char s[2][6][30], char* data);

#endif
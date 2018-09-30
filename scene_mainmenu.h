#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_MainMenu {
    WTexture* backgroundTexture;
    SDL_Rect renderQuad;
    short int index;
    int socketFd;
    bool connected, dataReceived;
    WTexture* seta;//
    WTexture* bemvindo  ;
    WTexture* nome;
    WTexture* jogar;  
    WTexture* tutorial;
    WTexture* logout;
    WTexture* jogarOff;
    WTexture* tutorialOff;
    WTexture* logoutOff;
    WTexture* rank[6];
} Scene_MainMenu;

Scene_MainMenu * SceneMainMenu_new();

void SceneMainMenu_update(Scene_MainMenu* s);

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e);

void SceneMainMenu_destroy(Scene_MainMenu* s);

int getRank(char s[6][20], char* data);

#endif
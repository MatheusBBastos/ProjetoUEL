#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_MainMenu {
    int enteringFrame, frame;
    WTexture* backgroundTexture;
    WTexture* textLogar;
    WTexture* textModoOff;
    SDL_Rect renderQuad;
    WTexture* textLogarOff;
    WTexture* textModoOffOff;
    WTexture* seta;
    TextBox* login;
    TextBox* senha;
    SDL_AudioDeviceID* deviceId;
    Uint8* wavBuffer;
    bool modoOff;
    short int index;
} Scene_MainMenu;

Scene_MainMenu* SceneMainMenu_new();

void SceneMainMenu_update(Scene_MainMenu* s);

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e);

void SceneMainMenu_destroy(Scene_MainMenu* s);

#endif
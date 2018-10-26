#ifndef SCENE_LOGIN_H
#define SCENE_LOGIN_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Login {
    int socketFd;
    bool connected, dataReceived, loginPressed, connectionNotStarted;
    int enteringFrame, frame;
    WTexture* loading;
    WTexture* backgroundTexture;
    WTexture* textLogar;
    WTexture* textModoOff;
    WTexture* logo[2];
    WTexture* textVoltar;
    WTexture* textCaps;
    WTexture* textErrorBrute;
    TextBox* login;
    TextBox* senha;
    WTexture* textError;
    bool modoOff;
    bool acessonegado;
    bool brute;
    short int index;
    int positionAnimado;
    SDL_Rect renderQuad;
} Scene_Login;

Scene_Login* SceneLogin_new();

void SceneLogin_update(Scene_Login* s);

void SceneLogin_handleEvent(Scene_Login* s, SDL_Event* e);

void SceneLogin_destroy(Scene_Login* s);

#endif

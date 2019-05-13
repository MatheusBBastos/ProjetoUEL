#ifndef SCENE_LOGIN_H
#define SCENE_LOGIN_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Login {
    WTexture* loading;
    WTexture* backgroundTexture[2];
    WTexture* textLogar;
    WTexture* textModoOff;
    WTexture* logo[2];
    WTexture* textCaps;
    WTexture* textErrorBrute;
    WTexture* textError;
    WTexture* textSair;
    TextBox* login;
    TextBox* senha;
    bool connected, dataReceived, loginPressed, connectionNotStarted;
    bool acessonegado;
    bool brute;
    short int index;
    int socketFd;
    int maxFrame, plusOne;
    int enteringFrame, frame;
    int positionAnimado;
    SDL_Rect renderQuad;
} Scene_Login;

Scene_Login* SceneLogin_new();

void SceneLogin_update(Scene_Login* s);

void SceneLogin_handleEvent(Scene_Login* s, SDL_Event* e);

void SceneLogin_destroy(Scene_Login* s);

#endif

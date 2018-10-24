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
    SDL_Rect renderQuad;
    WTexture* textLogarOff;
    WTexture* textModoOffOff;
    WTexture* seta;
    WTexture* textVoltar;
    WTexture* textVoltarOff;
    WTexture* textCaps;
    WTexture* textErrorBrute;
    TextBox* login;
    TextBox* senha;
    WTexture* textError;
    Mix_Chunk* type;
    Mix_Music* music;
    WTexture* logo[2];
    bool modoOff;
    bool acessonegado;
    bool brute;
    short int index;
    int positionAnimado;
} Scene_Login;

Scene_Login* SceneLogin_new();

void SceneLogin_update(Scene_Login* s);

void SceneLogin_handleEvent(Scene_Login* s, SDL_Event* e);

void SceneLogin_destroy(Scene_Login* s);

#endif
#ifndef SCENE_LOGIN_H
#define SCENE_LOGIN_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Login {
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
    WTexture* textError;
    Mix_Music* music;
    bool modoOff;
    bool acessonegado;
    short int index;
} Scene_Login;

Scene_Login* SceneLogin_new();

void SceneLogin_update(Scene_Login* s);

void SceneLogin_handleEvent(Scene_Login* s, SDL_Event* e);

void SceneLogin_destroy(Scene_Login* s);

#endif
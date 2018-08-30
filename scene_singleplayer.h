#ifndef SCENE_SINGLEPLAYER_H
#define SCENE_SINGLEPLAYER_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Singleplayer {
    WTexture* textTexture;
    WTexture* testTexture;
    SDL_Rect renderQuad;
    int cont;
    int socketFd, s2;
} Scene_Singleplayer;

Scene_Singleplayer* SceneSingleplayer_new();

void SceneSingleplayer_update(Scene_Singleplayer* s);

void SceneSingleplayer_handleEvent(Scene_Singleplayer* s, SDL_Event* e);

void SceneSingleplayer_destroy(Scene_Singleplayer* s);

#endif
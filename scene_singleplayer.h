#ifndef SCENE_SINGLEPLAYER_H
#define SCENE_SINGLEPLAYER_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"

typedef struct Scene_Singleplayer {
    SDL_Texture* textTexture;
    SDL_Rect renderQuad;
} Scene_Singleplayer;

Scene_Singleplayer* SceneSingleplayer_new();

void SceneSingleplayer_update();

void SceneSingleplayer_destroy();

#endif
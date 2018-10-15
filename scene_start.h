#ifndef SCENE_START_H
#define SCENE_START_H

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Start {
    WTexture* background;
    WTexture* start;
    WTexture* options;
    WTexture* quit;
    WTexture* seta;
    SDL_Rect renderQuad;
    short int index;
} Scene_Start;

Scene_Start * SceneStart_new();

void SceneStart_update(Scene_Start* s);

void SceneStart_handleEvent(Scene_Start* s, SDL_Event* e);

void SceneStart_destroy(Scene_Start* s);

#endif
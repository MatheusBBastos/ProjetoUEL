#ifndef SCENE_TUTORIAL_H
#define SCENE_TUTORIAL_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Tutorial {
    WTexture* bg;
    SDL_Rect renderQuad;
} Scene_Tutorial;

Scene_Tutorial* SceneTutorial_new();

void SceneTutorial_update(Scene_Tutorial* s);

void SceneTutorial_handleEvent(Scene_Tutorial* s, SDL_Event* e);

void SceneTutorial_destroy(Scene_Tutorial* s);

#endif
#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"

typedef struct Scene_MainMenu {
    SDL_Texture* textTexture;
    SDL_Rect renderQuad;
} Scene_MainMenu;

Scene_MainMenu* SceneMainMenu_new();

void SceneMainMenu_update();

void SceneMainMenu_destroy();

#endif
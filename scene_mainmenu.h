#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"

typedef struct Scene_MainMenu {
    SDL_Texture* textTexture;
    SDL_Rect renderQuad;
} Scene_MainMenu;

Scene_MainMenu* SceneMainMenu_new(GameInfo* g);

void SceneMainMenu_update(SceneManager* s, GameInfo* g);

void SceneMainMenu_destroy(SceneManager* s);

#endif
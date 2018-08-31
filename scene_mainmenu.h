#ifndef SCENE_MAINMENU_H
#define SCENE_MAINMENU_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"

typedef struct Scene_MainMenu {
    SDL_Texture* textTexture;
    SDL_Texture* textLogar; //PARA QUANDO LOGAR TIVER ATIVO
    SDL_Texture* textModoOff; //PARA QUANDO MODO OFFLINE TIVER ATIVO
    SDL_Texture* textLogarOff; //PARA QUANDO LOGAR TIVER INATIVO
	SDL_Texture* textModoOffOff; //PARA QUANDO MODOOFFLINE TIVER INATIVO
    SDL_Texture* seta;
    SDL_Rect renderQuad;
    bool modoOff;
} Scene_MainMenu;

Scene_MainMenu* SceneMainMenu_new();

void SceneMainMenu_update(Scene_MainMenu* s);

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e);

void SceneMainMenu_destroy(Scene_MainMenu* s);

#endif
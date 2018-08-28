// Esse header contém as bases para o controle das cenas (Scenes).

#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "headers.h"
#include "scene_mainmenu.h"

typedef struct Scene_MainMenu Scene_MainMenu;

// Enumerador de tipos de cenas do jogo
enum SceneTypes {
    SCENE_MAINMENU,
    SCENE_SINGLEPLAYER,
    SCENE_MULTIPLAYER,
    SCENE_CREATE_LOBBY,
    SCENE_LOBBY,
    SCENE_GAME
};

// Estrutura SceneManager: carrega dados para controlar as cenas
typedef struct SceneManager {
    // Identificador da cena atual
    int currentScene;
    // No meio de uma transição entre cenas
    bool inTransition;
    // Duração da transição
    int transitionDuration;
    // Na transição, já trocou as cenas
    bool transitionChangedScene;
    // Cena que substituirá a atual após a transição
    int transitionNextScene;
    // Frame da transição
    unsigned transitionFrame;
    // Estrutura da cena do menu principal
    Scene_MainMenu* sMainMenu;
} SceneManager;

// Fazer transição entre cenas
void SceneManager_performTransition(SceneManager* s, int duration, int newScene);
// Mudar a cena atual
void SceneManager_changeScene(SceneManager* s, GameInfo* g, int newScene);
// Atualizar a cena atual
void SceneManager_updateScene(SceneManager* s, GameInfo* g);
// Lidar com um evento na cena atual
void SceneManager_handleEvent(SceneManager* s, SDL_Event* e);

#endif
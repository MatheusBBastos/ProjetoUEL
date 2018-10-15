// Esse header contém as bases para o controle das cenas (Scenes).

#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "headers.h"
#include "scene_login.h"
#include "scene_singleplayer.h"
#include "scene_mainmenu.h"
#include "scene_map.h"
#include "scene_servers.h"
#include "scene_tutorial.h"
#include "scene_lobby.h"
#include "scene_start.h"

typedef struct Scene_Login Scene_Login;
typedef struct Scene_Singleplayer Scene_Singleplayer;
typedef struct Scene_MainMenu Scene_MainMenu;
typedef struct Scene_Map Scene_Map;
typedef struct Scene_Servers Scene_Servers;
typedef struct Scene_Tutorial Scene_Tutorial;
typedef struct Scene_Lobby Scene_Lobby;
typedef struct Scene_Start Scene_Start;

// Enumerador de tipos de cenas do jogo
enum SceneTypes {
    SCENE_START,
    SCENE_LOGIN,
    SCENE_MAINMENU,
    SCENE_SINGLEPLAYER,
    SCENE_MULTIPLAYER,
    SCENE_CREATE_LOBBY,
    SCENE_LOBBY,
    SCENE_MAP,
    SCENE_TUTORIAL,
    SCENE_SERVERS,
    SCENE_UNDEFINED
};

// Estrutura SceneManagerS: carrega dados para controlar as cenas
typedef struct SceneManagerS {
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
    Scene_Start* sStart;
    Scene_Login* sLogin;
    Scene_Singleplayer* sSingleplayer;
    Scene_MainMenu* sMainMenu;
    Scene_Map* sMap;
    Scene_Servers* sServers;
    Scene_Tutorial* sTutorial;
    Scene_Lobby* sLobby;
    bool quit;
} SceneManagerS;

// Fazer transição entre cenas
void SceneManager_performTransition(int duration, int newScene);
// Mudar a cena atual
void SceneManager_changeScene(int newScene);
// Atualizar a cena atual
void SceneManager_updateScene();
// Lidar com um evento na cena atual
void SceneManager_handleEvent(SDL_Event* e);

#endif
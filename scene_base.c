#include "scene_base.h"

// Começa uma transiçao entre cenas
void SceneManager_performTransition(int duration, int newScene) {
    SceneManager.inTransition = true;
    SceneManager.transitionFrame = 0;
    SceneManager.transitionChangedScene = false;
    SceneManager.transitionNextScene = newScene;
    SceneManager.transitionDuration = duration;
}

// Troca a cena, destruindo a anterior e criando a nova
void SceneManager_changeScene(int newScene) {
    switch(SceneManager.currentScene) {
        case SCENE_LOGIN:
            SceneLogin_destroy(SceneManager.sLogin);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_destroy(SceneManager.sSingleplayer);
            break;
        case SCENE_MAINMENU:
            SceneMainMenu_destroy(SceneManager.sMainMenu);
            break;
        case SCENE_MAP:
            SceneMap_destroy(SceneManager.sMap);
            break;
        case SCENE_SERVERS:
            SceneServers_destroy(SceneManager.sServers);
            break;
        case SCENE_TUTORIAL:
            SceneTutorial_destroy(SceneManager.sTutorial);
            break;
        case SCENE_LOBBY:
            SceneLobby_destroy(SceneManager.sLobby);
            break;
    }
    SceneManager.currentScene = newScene;
    switch(newScene) {
        case SCENE_LOGIN:
            SceneManager.sLogin = SceneLogin_new();
            break;
        case SCENE_SINGLEPLAYER:
            SceneManager.sSingleplayer = SceneSingleplayer_new();
            break;
        case SCENE_MAINMENU:
            SceneManager.sMainMenu = SceneMainMenu_new();
            break;
        case SCENE_MAP:
            SceneManager.sMap = SceneMap_new();
            break;
        case SCENE_SERVERS:
            SceneManager.sServers = SceneServers_new();
            break;
        case SCENE_TUTORIAL:
            SceneManager.sTutorial = SceneTutorial_new();
            break;
        case SCENE_LOBBY:
            SceneManager.sLobby = SceneLobby_new();
            break;
    }
}

// Atualiza e renderiza a cena atual, além de mostrar os efeitos de uma transição se for o caso
void SceneManager_updateScene() {
    switch(SceneManager.currentScene) {
        case SCENE_LOGIN:
            SceneLogin_update(SceneManager.sLogin);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_update(SceneManager.sSingleplayer);
            break;
        case SCENE_MAINMENU:
            SceneMainMenu_update(SceneManager.sMainMenu);
            break;
        case SCENE_MAP:
            SceneMap_update(SceneManager.sMap);
            break;
        case SCENE_SERVERS:
            SceneServers_update(SceneManager.sServers);
            break;
        case SCENE_TUTORIAL:
            SceneTutorial_update(SceneManager.sTutorial);
            break;
        case SCENE_LOBBY:
            SceneLobby_update(SceneManager.sLobby);
            break;
    }
    if(SceneManager.inTransition) {
        // Se, na transição, a cena ainda nao foi trocada (primeira fase)
        if(!SceneManager.transitionChangedScene) {
            // Renderizar na cor preta
            SDL_SetRenderDrawColor(Game.renderer, 0x00, 0x00, 0x00, 0xFF);
            // Retângulo que varia de tamanho conforme o tempo
            SDL_Rect fillRect = {0, 0, (SceneManager.transitionFrame + 2) * Game.screenWidth / SceneManager.transitionDuration, Game.screenHeight};
            SDL_RenderFillRect(Game.renderer, &fillRect);
            SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SceneManager.transitionFrame++;
            // Já preencheu a tela inteira
            if(SceneManager.transitionFrame >= SceneManager.transitionDuration) {
                // Mudar a cena e passar para a próxima fase
                SceneManager_changeScene(SceneManager.transitionNextScene);
                SceneManager.transitionChangedScene = true;
                // Recomeçar a contagem de quadros
                SceneManager.transitionFrame = 0;
            }
        } else {
            // Segunda fase de transição (retângulo preto diminui o tamanho conforme o tempo)
            SDL_SetRenderDrawColor(Game.renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_Rect fillRect = {SceneManager.transitionFrame * Game.screenWidth / SceneManager.transitionDuration, 0, Game.screenWidth, Game.screenHeight};
            SDL_RenderFillRect(Game.renderer, &fillRect);
            SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SceneManager.transitionFrame++;
            // Fim da transição
            if(SceneManager.transitionFrame >= SceneManager.transitionDuration) {
                SceneManager.inTransition = false;
            }
        }
    }
}

void SceneManager_handleEvent(SDL_Event* e) {
    switch(SceneManager.currentScene) {
        case SCENE_LOGIN:
            SceneLogin_handleEvent(SceneManager.sLogin, e);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_handleEvent(SceneManager.sSingleplayer, e);
            break;
        case SCENE_MAINMENU:
            SceneMainMenu_handleEvent(SceneManager.sMainMenu, e);
            break;
        case SCENE_MAP:
            SceneMap_handleEvent(SceneManager.sMap, e);
            break;
        case SCENE_SERVERS:
            SceneServers_handleEvent(SceneManager.sServers, e);
            break;
        case SCENE_TUTORIAL:
            SceneTutorial_handleEvent(SceneManager.sTutorial, e);
            break;
        case SCENE_LOBBY:
            SceneLobby_handleEvent(SceneManager.sLobby, e);
            break;
    }
}
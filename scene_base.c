#include "scene_base.h"

// Começa uma transiçao entre cenas
void SceneManager_performTransition(int duration, int newScene) {
    sMng.inTransition = true;
    sMng.transitionFrame = 0;
    sMng.transitionChangedScene = false;
    sMng.transitionNextScene = newScene;
    sMng.transitionDuration = duration;
}

// Troca a cena, destruindo a anterior e criando a nova
void SceneManager_changeScene(int newScene) {
    switch(sMng.currentScene) {
        case SCENE_LOGIN:
            SceneLogin_destroy(sMng.sLogin);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_destroy(sMng.sSingleplayer);
            break;
        case SCENE_MAINMENU:
            SceneMainMenu_destroy(sMng.sMainMenu);
            break;
        case SCENE_MAP:
            SceneMap_destroy(sMng.sMap);
            break;
        case SCENE_LOBBY:
            SceneLobby_destroy(sMng.sLobby);
            break;
        case SCENE_TUTORIAL:
            SceneTutorial_destroy(sMng.sTutorial);
    }
    sMng.currentScene = newScene;
    switch(newScene) {
        case SCENE_LOGIN:
            sMng.sLogin = SceneLogin_new();
            break;
        case SCENE_SINGLEPLAYER:
            sMng.sSingleplayer = SceneSingleplayer_new();
            break;
        case SCENE_MAINMENU:
            sMng.sMainMenu = SceneMainMenu_new();
            break;
        case SCENE_MAP:
            sMng.sMap = SceneMap_new();
            break;
        case SCENE_LOBBY:
            sMng.sLobby = SceneLobby_new();
            break;
        case SCENE_TUTORIAL:
            sMng.sTutorial = SceneTutorial_new();
            break;
    }
}

// Atualiza e renderiza a cena atual, além de mostrar os efeitos de uma transição se for o caso
void SceneManager_updateScene() {
    switch(sMng.currentScene) {
        case SCENE_LOGIN:
            SceneLogin_update(sMng.sLogin);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_update(sMng.sSingleplayer);
            break;
        case SCENE_MAINMENU:
            SceneMainMenu_update(sMng.sMainMenu);
            break;
        case SCENE_MAP:
            SceneMap_update(sMng.sMap);
            break;
        case SCENE_LOBBY:
            SceneLobby_update(sMng.sLobby);
            break;
        case SCENE_TUTORIAL:
            SceneTutorial_update(sMng.sTutorial);
            break;
    }
    if(sMng.inTransition) {
        // Se, na transição, a cena ainda nao foi trocada (primeira fase)
        if(!sMng.transitionChangedScene) {
            // Renderizar na cor preta
            SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0xFF);
            // Retângulo que varia de tamanho conforme o tempo
            SDL_Rect fillRect = {0, 0, sMng.transitionFrame * gInfo.screenWidth / sMng.transitionDuration, gInfo.screenHeight};
            SDL_RenderFillRect(gInfo.renderer, &fillRect);
            SDL_SetRenderDrawColor(gInfo.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            sMng.transitionFrame++;
            // Já preencheu a tela inteira
            if(sMng.transitionFrame >= sMng.transitionDuration) {
                // Mudar a cena e passar para a próxima fase
                SceneManager_changeScene(sMng.transitionNextScene);
                sMng.transitionChangedScene = true;
                // Recomeçar a contagem de quadros
                sMng.transitionFrame = 0;
            }
        } else {
            // Segunda fase de transição (retângulo preto diminui o tamanho conforme o tempo)
            SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_Rect fillRect = {sMng.transitionFrame * gInfo.screenWidth / sMng.transitionDuration, 0, gInfo.screenWidth, gInfo.screenHeight};
            SDL_RenderFillRect(gInfo.renderer, &fillRect);
            SDL_SetRenderDrawColor(gInfo.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            sMng.transitionFrame++;
            // Fim da transição
            if(sMng.transitionFrame >= sMng.transitionDuration) {
                sMng.inTransition = false;
            }
        }
    }
}

void SceneManager_handleEvent(SDL_Event* e) {
    switch(sMng.currentScene) {
        case SCENE_LOGIN:
            SceneLogin_handleEvent(sMng.sLogin, e);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_handleEvent(sMng.sSingleplayer, e);
            break;
        case SCENE_MAINMENU:
            SceneMainMenu_handleEvent(sMng.sMainMenu, e);
            break;
        case SCENE_MAP:
            SceneMap_handleEvent(sMng.sMap, e);
            break;
        case SCENE_LOBBY:
            SceneLobby_handleEvent(sMng.sLobby, e);
            break;
        case SCENE_TUTORIAL:
            SceneTutorial_handleEvent(sMng.sTutorial, e);
            break;
    }
}
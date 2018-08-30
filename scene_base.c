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
        case SCENE_MAINMENU:
            SceneMainMenu_destroy(sMng.sMainMenu);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_destroy(sMng.sSingleplayer);
            break;
    }
    sMng.currentScene = newScene;
    switch(newScene) {
        case SCENE_MAINMENU:
            sMng.sMainMenu = SceneMainMenu_new();
            break;
        case SCENE_SINGLEPLAYER:
            sMng.sSingleplayer = SceneSingleplayer_new();
            break;
    }
}

// Atualiza e renderiza a cena atual, além de mostrar os efeitos de uma transição se for o caso
void SceneManager_updateScene() {
    switch(sMng.currentScene) {
        case SCENE_MAINMENU:
            SceneMainMenu_update(sMng.sMainMenu);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_update(sMng.sSingleplayer);
            break;
    }
    if(sMng.inTransition) {
        // Se, na transição, a cena ainda nao foi trocada (primeira fase)
        if(!sMng.transitionChangedScene) {
            // Renderizar na cor preta
            SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0xFF);
            // Retângulo que varia de tamanho conforme o tempo
            SDL_Rect fillRect = {0, 0, sMng.transitionFrame * SCREEN_WIDTH / sMng.transitionDuration, SCREEN_HEIGHT};
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
            SDL_Rect fillRect = {sMng.transitionFrame * SCREEN_WIDTH / sMng.transitionDuration, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
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
        case SCENE_MAINMENU:
            SceneMainMenu_handleEvent(e);
            break;
        case SCENE_SINGLEPLAYER:
            SceneSingleplayer_handleEvent(e);
            break;
    }
}
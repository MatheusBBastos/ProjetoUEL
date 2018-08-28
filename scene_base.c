#include "scene_base.h"

void SceneManager_performTransition(SceneManager* s, int duration, int newScene) {
    s->inTransition = true;
    s->transitionFrame = 0;
    s->transitionChangedScene = false;
    s->transitionNextScene = newScene;
    s->transitionDuration = duration;
}

void SceneManager_changeScene(SceneManager* s, GameInfo* g, int newScene) {
    switch(s->currentScene) {
        case SCENE_MAINMENU:
            SceneMainMenu_destroy(s);
            break;
        case SCENE_SINGLEPLAYER:
            //SceneSingleplayer_destroy(s);
            break;
    }
    s->currentScene = newScene;
    switch(newScene) {
        case SCENE_MAINMENU:
            s->sMainMenu = SceneMainMenu_new(g);
            break;
        case SCENE_SINGLEPLAYER:
            //s->sSingleplayer = SceneSingleplayer_new();
            break;
    }
}

void SceneManager_updateScene(SceneManager* s, GameInfo* g) {
    switch(s->currentScene) {
        case SCENE_MAINMENU:
            SceneMainMenu_update(s, g);
            break;
        case SCENE_SINGLEPLAYER:
            //SceneSingleplayer_update(s, e);
            break;
    }
    if(s->inTransition) {
        if(!s->transitionChangedScene) {
            SDL_SetRenderDrawColor(g->renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_Rect fillRect = {0, 0, s->transitionFrame * SCREEN_WIDTH / s->transitionDuration, SCREEN_HEIGHT};
            SDL_RenderFillRect(g->renderer, &fillRect);
            SDL_SetRenderDrawColor(g->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            s->transitionFrame++;
            if(s->transitionFrame >= s->transitionDuration) {
                SceneManager_changeScene(s, g, s->transitionNextScene);
                s->transitionChangedScene = true;
                s->transitionFrame = 0;
            }
        } else {
            SDL_SetRenderDrawColor(g->renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_Rect fillRect = {s->transitionFrame * SCREEN_WIDTH / s->transitionDuration, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(g->renderer, &fillRect);
            SDL_SetRenderDrawColor(g->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            s->transitionFrame++;
            if(s->transitionFrame >= s->transitionDuration) {
                s->inTransition = false;
            }
        }
    }
}

void SceneManager_handleEvent(SceneManager* s, SDL_Event* e) {
    switch(s->currentScene) {
        case SCENE_MAINMENU:
            //SceneMainMenu_handleEvent(s, e);
            break;
        case SCENE_SINGLEPLAYER:
            //SceneSingleplayer_handleEvent(s, e);
            break;
    }
}
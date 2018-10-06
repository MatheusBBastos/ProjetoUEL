#include "scene_tutorial.h"

Scene_Tutorial* SceneTutorial_new() {
    Scene_Tutorial* newScene = malloc(sizeof(Scene_Tutorial));

    newScene->msg = WD_CreateTexture();
    SDL_Color Cmsg = {255, 255, 255};
    WD_TextureLoadFromText(newScene->msg, "Tutorial", Game.mainFont, Cmsg);


    return newScene;
}

void SceneTutorial_update(Scene_Tutorial* s) {
    WD_TextureRender(s->msg, 500 * Game.screenMulti, 500 * Game.screenMulti);
}

void SceneTutorial_destroy(Scene_Tutorial* s) {
    WD_TextureDestroy(s->msg);
}

void SceneTutorial_handleEvent(Scene_Tutorial* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        }
    }
}

#include "scene_tutorial.h"

Scene_Tutorial* SceneTutorial_new() {
    Scene_Tutorial* newScene = malloc(sizeof(Scene_Tutorial));

    // Textura
    newScene->bg = WD_CreateTexture();

    // Cor
    SDL_Color Cmsg = {255, 255, 255};

    // Carregar textura
    WD_TextureLoadFromFile(newScene->bg, "content/bgtutorial.png");

    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = REFERENCE_WIDTH;
    newScene->renderQuad.h = REFERENCE_HEIGHT;

    return newScene;
}


void SceneTutorial_update(Scene_Tutorial* s) {
    SDL_RenderClear(Game.renderer);
    WD_TextureRenderDest(s->bg, &s->renderQuad);
}


void SceneTutorial_destroy(Scene_Tutorial* s) {
    WD_TextureDestroy(s->bg);
}


void SceneTutorial_handleEvent(Scene_Tutorial* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_ESCAPE) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        }
    }
}

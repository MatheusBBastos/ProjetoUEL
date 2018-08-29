#include "scene_singleplayer.h"

Scene_Singleplayer* SceneSingleplayer_new() {
    Scene_Singleplayer* newScene = malloc(sizeof(Scene_Singleplayer));
    SDL_Color color = {0, 255, 0};
    newScene->text = WD_CreateText();
    WD_TextLoad(newScene->text, "Uhulll", gInfo.mainFont, color);
    newScene->renderQuad.x = (SCREEN_WIDTH - newScene->text->w) / 2;
    newScene->renderQuad.y = (SCREEN_HEIGHT - newScene->text->h) / 2;
    return newScene;
}

void SceneSingleplayer_update(Scene_Singleplayer* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x34, 0x92, 0x22, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    WD_TextRender(s->text, s->renderQuad.x, s->renderQuad.y);
}

void SceneSingleplayer_destroy(Scene_Singleplayer* s) {
    WD_TextDestroy(s->text);
    free(s);
}

void SceneSingleplayer_handleEvent(SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        }
    }
}
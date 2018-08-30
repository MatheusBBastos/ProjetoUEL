#include "scene_mainmenu.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    SDL_Color color = {255, 0, 0};
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(gInfo.mainFont, "Testesao", color);
    newScene->textTexture = SDL_CreateTextureFromSurface(gInfo.renderer, textSurface);
    int w = textSurface->w, h = textSurface->h;
    newScene->renderQuad.x = (SCREEN_WIDTH - w) / 2;
    newScene->renderQuad.y = (SCREEN_HEIGHT - h) / 2;
    newScene->renderQuad.w = w;
    newScene->renderQuad.h = h;
    return newScene;
}

void SceneMainMenu_update(Scene_MainMenu* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    SDL_RenderCopy(gInfo.renderer, s->textTexture, NULL, &s->renderQuad);
}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    SDL_DestroyTexture(s->textTexture);
    free(s);
}

void SceneMainMenu_handleEvent(SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SINGLEPLAYER);
        }
    }
}
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

void SceneMainMenu_update() {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    SDL_RenderCopy(gInfo.renderer, sMng.sMainMenu->textTexture, NULL, &sMng.sMainMenu->renderQuad);
}

void SceneMainMenu_destroy() {
    SDL_DestroyTexture(sMng.sMainMenu->textTexture);
    free(sMng.sMainMenu);
    sMng.sMainMenu = NULL;
}
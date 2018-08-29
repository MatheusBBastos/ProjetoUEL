#include "scene_singleplayer.h"

Scene_Singleplayer* SceneSingleplayer_new() {
    Scene_Singleplayer* newScene = malloc(sizeof(Scene_Singleplayer));
    SDL_Color color = {0, 255, 0};
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(gInfo.mainFont, "Uhulll", color);
    newScene->textTexture = SDL_CreateTextureFromSurface(gInfo.renderer, textSurface);
    int w = textSurface->w, h = textSurface->h;
    newScene->renderQuad.x = (SCREEN_WIDTH - w) / 2;
    newScene->renderQuad.y = (SCREEN_HEIGHT - h) / 2;
    newScene->renderQuad.w = w;
    newScene->renderQuad.h = h;
    return newScene;
}

void SceneSingleplayer_update() {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x34, 0x92, 0x22, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    SDL_RenderCopy(gInfo.renderer, sMng.sSingleplayer->textTexture, NULL, &sMng.sSingleplayer->renderQuad);
}

void SceneSingleplayer_destroy() {
    SDL_DestroyTexture(sMng.sSingleplayer->textTexture);
    free(sMng.sSingleplayer);
    sMng.sSingleplayer = NULL;
}
#include "scene_mainmenu.h"
#include "scene_base.h"

Scene_MainMenu* SceneMainMenu_new(GameInfo* g) {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    SDL_Color color = {255, 0, 0};
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(g->mainFont, "Testesao", color);
    newScene->textTexture = SDL_CreateTextureFromSurface(g->renderer, textSurface);
    int w = textSurface->w, h = textSurface->h;
    newScene->renderQuad.x = (SCREEN_WIDTH - w) / 2;
    newScene->renderQuad.y = (SCREEN_HEIGHT - h) / 2;
    newScene->renderQuad.w = w;
    newScene->renderQuad.h = h;
    return newScene;
}

void SceneMainMenu_update(SceneManager* s, GameInfo* g) {
    SDL_RenderCopy(g->renderer, s->sMainMenu->textTexture, NULL, &s->sMainMenu->renderQuad);
}

void SceneMainMenu_destroy(SceneManager* s) {
    SDL_DestroyTexture(s->sMainMenu);
    free(s->sMainMenu);
    s->sMainMenu = NULL;
}
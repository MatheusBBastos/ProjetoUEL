#include "scene_mainmenu.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));

    SDL_Color colorSelected = {255, 156, 0}; // Cores dos botões quando selecionados
    SDL_Color colorNotSelected = {255,255,255}; // Cores dos botões quando não selecionados

    newScene->backgroundTexture = WD_CreateTexture();
    newScene->textLogar = WD_CreateTexture();
    newScene->textModoOff = WD_CreateTexture();
    newScene->textLogarOff = WD_CreateTexture();
    newScene->textModoOffOff = WD_CreateTexture();
    newScene->modoOff = false;
    newScene->seta = WD_CreateTexture();

    WD_TextureLoadFromFile(newScene->seta, "seta.png");
    WD_TextureLoadFromText(newScene->textLogar, "Logar" , gInfo.menuFont, colorSelected);
    WD_TextureLoadFromText(newScene->textLogarOff, "Logar", gInfo.menuFont, colorNotSelected);
    WD_TextureLoadFromText(newScene->textModoOff, "Modo offline", gInfo.menuFont, colorSelected);
    WD_TextureLoadFromText(newScene->textModoOffOff, "Modo offline", gInfo.menuFont, colorNotSelected);

    newScene->seta->h *= 0.5;
    newScene->seta->w *= 0.5;

    WD_TextureLoadFromFile(newScene->backgroundTexture, "BG_Login.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w/2;
    newScene->renderQuad.h = h/2;
    return newScene;
}

void SceneMainMenu_update(Scene_MainMenu* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0x00);
    TTF_SetFontOutline(gInfo.menuFont, 3);
    if (s->modoOff) {
        WD_TextureRender(s->textLogarOff, 325 / 2, 800 / 2);
        WD_TextureRender(s->textModoOff, 800 / 2, 800 / 2);
        WD_TextureRender(s->seta, 720 / 2, 806 / 2);
    }
    else {
        WD_TextureRender(s->textLogar, 325 / 2, 800 / 2);
        WD_TextureRender(s->textModoOffOff, 800 / 2, 800 / 2);
        WD_TextureRender(s->seta, 245 / 2, 806 / 2);
    }
    SDL_SetRenderDrawBlendMode(gInfo.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gInfo.renderer, 0xFF, 0xFF, 0xFF, 100);
    SDL_Rect rect = {240, 290, 260, 26};
    SDL_RenderFillRect(gInfo.renderer, &rect);
    rect.y += 45;
    SDL_RenderFillRect(gInfo.renderer, &rect);
}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->textLogar);
    WD_TextureDestroy(s->textLogarOff);
    WD_TextureDestroy(s->textModoOff);
    WD_TextureDestroy(s->textModoOffOff);
    free(s);
}

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN && s->modoOff) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SINGLEPLAYER);
        }if(e->key.keysym.sym == SDLK_RIGHT || e->key.keysym.sym == SDLK_LEFT){
            s->modoOff = !s->modoOff;
        }
    }
}
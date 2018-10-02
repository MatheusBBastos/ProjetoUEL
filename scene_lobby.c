#include "scene_lobby.h"

Scene_Lobby* SceneLobby_new() {
    Scene_Lobby* newScene = malloc(sizeof(Scene_Lobby));

    newScene->nome = WD_CreateTexture();
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->mutiplayer = WD_CreateTexture();
    newScene->server = WD_CreateTexture();

    SDL_Color Cname = {204, 204, 204};
    SDL_Color Cmult = {0, 132, 255};
    SDL_Color Cwhite = {255, 255, 255};


    newScene->boxIp = WD_CreateTextBox(75 * gInfo.screenMulti, 750 * gInfo.screenMulti, 380 * gInfo.screenMulti, 52 * gInfo.screenMulti, 16, gInfo.lobbyFonte, Cwhite, false);

    WD_TextureLoadFromText(newScene->nome, "Basto Forte", gInfo.lobbyName, Cname);
    WD_TextureLoadFromText(newScene->mutiplayer, "MULTIPLAYER", gInfo.lobbyFonte, Cmult);
    WD_TextureLoadFromText(newScene->server, "SERVER: ", gInfo.lobbyFonte, Cwhite);


    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_mainMenu.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w*gInfo.screenMulti;
    newScene->renderQuad.h = h*gInfo.screenMulti;

    SDL_StartTextInput();

    return newScene;
}


void SceneLobby_update(Scene_Lobby* s) {
    SDL_RenderClear(gInfo.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    WD_TextureRender(s->nome, 75 * gInfo.screenMulti, 515 * gInfo.screenMulti);
    WD_TextureRender(s->mutiplayer, 75 * gInfo.screenMulti, 585 * gInfo.screenMulti);
    WD_TextureRender(s->server, 120 * gInfo.screenMulti, 655 * gInfo.screenMulti);
    //box
    SDL_SetRenderDrawBlendMode(gInfo.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gInfo.renderer, 0xFF, 0xFF, 0xFF, 100);
    SDL_Rect rect = { 75 * gInfo.screenMulti, 750 * gInfo.screenMulti, 380 * gInfo.screenMulti, 52 * gInfo.screenMulti };
    SDL_RenderFillRect(gInfo.renderer, &rect);
    WD_TextBoxRender(s->boxIp, s->frame);
    s->boxIp->active = true;
}

void SceneLobby_destroy(Scene_Lobby* s) {
    WD_TextureDestroy(s->nome);
    WD_TextureDestroy(s->mutiplayer);
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->server);
}

void SceneLobby_handleEvent(Scene_Lobby* s, SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        }
    }

    WD_TextBoxHandleEvent(s->boxIp, e);

}

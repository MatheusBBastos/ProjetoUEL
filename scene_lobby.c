#include "scene_lobby.h"

Scene_Lobby* SceneLobby_new() {
    Scene_Lobby* newScene = malloc(sizeof(Scene_Lobby));
    newScene->msg = WD_CreateTexture();
    SDL_Color Cmsg = {255, 255, 255};

    WD_TextureLoadFromText(newScene->msg, "LOBBY", gInfo.mainFont, Cmsg);


    return newScene;
}


void SceneLobby_update(Scene_Lobby* s) {
    WD_TextureRender(s->msg, 500 * gInfo.screenMulti, 500 * gInfo.screenMulti);
}

void SceneLobby_destroy(Scene_Lobby* s) {
    WD_TextureDestroy(s->msg);
}

void SceneLobby_handleEvent(Scene_Lobby* s, SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        }
    }
}

#include "scene_mainmenu.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    
    return newScene;
}

void SceneMainMenu_update(Scene_MainMenu* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    
}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    
    free(s);
}

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e) {
    /*
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN && s->modoOff && s->index == 2 ) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SINGLEPLAYER);
        }else if(e->key.keysym.sym == SDLK_RIGHT && s->index == 2 || e->key.keysym.sym == SDLK_LEFT && s->index == 2 ){
            s->modoOff = !s->modoOff;
        }else if((e->key.keysym.sym == SDLK_DOWN || e->key.keysym.sym == SDLK_TAB) && s->index < 2){
            s->index++;
        }else if(e->key.keysym.sym == SDLK_UP && s->index > 0){
            s->index--;
        }else if(e->key.keysym.sym == SDLK_1) {
            if (s->acessonegado) {
                s->acessonegado = false;
            }
            else {
                s->acessonegado = true;
            }
        }
    }
    */

}
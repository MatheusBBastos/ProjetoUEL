#include "scene_mainmenu.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->bemvindo = WD_CreateTexture();
    newScene->nome = WD_CreateTexture();

    SDL_Color colorBemvindo = {141,38,38}; 
    SDL_Color colorNome = {255, 255, 255};
    WD_TextureLoadFromText(newScene->bemvindo,"Bem Vindo", gInfo.mainMenu, colorBemvindo);
    WD_TextureLoadFromText(newScene->nome, "Basto Forte", gInfo.mainMenu, colorNome);


    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_mainMenu.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w*gInfo.screenMulti;
    newScene->renderQuad.h = h*gInfo.screenMulti;

    return newScene;
}



void SceneMainMenu_update(Scene_MainMenu* s) {
    //SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    WD_TextureRender(s->bemvindo, 84 * gInfo.screenMulti, 566 * gInfo.screenMulti);
    WD_TextureRender(s->nome, 84 * gInfo.screenMulti, 634 * gInfo.screenMulti);
    
}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->bemvindo);
    WD_TextureDestroy(s->nome);
    free(s);
    
}

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e) {

    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        }
        /*
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
        */
    }
    

}
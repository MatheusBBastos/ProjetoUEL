#include "scene_mainmenu.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    //SDL_Surface* textSurface = TTF_RenderUTF8_Blended(gInfo.mainFont, "Testesao", color);
    
    SDL_Surface * textSurface = IMG_Load("BG_Login.png");//BACKGROUND LOGIN

    SDL_Color colorSelect = {255, 156, 0};//CORES INICIAIS DOS BOTÕES
    SDL_Color colorNotSelect = {255,255,255};

    newScene->textTexture = WD_CreateTexture();
    newScene->textLogar = WD_CreateTexture();
    newScene->textModoOff = WD_CreateTexture();

    WD_TextureLoadFromText(newScene->textLogar, "Logar" , gInfo.menuFont, colorSelect);
    WD_TextureLoadFromText(newScene->textModoOff, "Modo Offline", gInfo.menuFont, colorNotSelect);

    newScene->textTexture = SDL_CreateTextureFromSurface(gInfo.renderer, textSurface);
    int w = textSurface->w, h = textSurface->h;
    newScene->renderQuad.x =0;// (SCREEN_WIDTH - w) / 2;
    newScene->renderQuad.y =0;// (SCREEN_HEIGHT - h) / 2;
    newScene->renderQuad.w = w/2;
    newScene->renderQuad.h = h/2;
    return newScene;
}

void SceneMainMenu_update(Scene_MainMenu* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    SDL_RenderCopy(gInfo.renderer, s->textTexture, NULL, &s->renderQuad);
    WD_TextureRender(s->textLogar, 325/2, 800/2);
    WD_TextureRender(s->textModoOff, 800/2, 800/2);

}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    SDL_DestroyTexture(s->textTexture);
    SDL_DestroyTexture(s->textLogar);
    SDL_DestroyTexture(s->textModoOff);
    free(s);
}

void SceneMainMenu_handleEvent(SDL_Event* e) {
    bool modoOff;
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN && modoOff) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SINGLEPLAYER);
        }if(e->key.keysym.sym == SDLK_RIGHT){
            //WD_TextureLoadFromText(newScene->textLogar, "Logar" , gInfo.menuFont, );
            //WD_TextureLoadFromText(newScene->textModoOff, "Modo Offline", gInfo.menuFont, colorOff);
            modoOff=true;
        }if(e->key.keysym.sym == SDLK_LEFT){
                //WD_TextureLoadFromText(newScene->textLogar, "Logar" , gInfo.menuFont, colorLogar);
                //WD_TextureLoadFromText(newScene->textModoOff, "Modo Offline", gInfo.menuFont, colorOff);
            modoOff=false;
        }
    }
}
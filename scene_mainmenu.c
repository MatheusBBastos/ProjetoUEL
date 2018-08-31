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
    newScene->textLogarOff = WD_CreateTexture();
	newScene->textModoOffOff = WD_CreateTexture();
	newScene->modoOff = false; //SETANDO O INICIO COM O BOTAO "LOGAR" ATIVADO
    newScene->seta = WD_CreateTexture();

    WD_TextureLoadFromText(newScene->textLogar, "Logar" , gInfo.menuFont, colorSelect);
	WD_TextureLoadFromText(newScene->textLogarOff, "Logar", gInfo.menuFont, colorNotSelect);
    WD_TextureLoadFromText(newScene->textModoOff, "Modo offline", gInfo.menuFont, colorSelect);
    WD_TextureLoadFromText(newScene->textModoOffOff, "Modo offline", gInfo.menuFont, colorNotSelect);
    WD_TextureLoadFromFile(newScene->seta, "seta.png");

    
    newScene->textTexture = SDL_CreateTextureFromSurface(gInfo.renderer, textSurface);
    int w = textSurface->w, h = textSurface->h;
    newScene->renderQuad.x =0;// INICIA NO PONTO X=0
    newScene->renderQuad.y =0;// INICIA NO PONTO Y=0
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
    if (s->modoOff) { //VERIFICAR SE MODOOFFLINE TA ATIVO OU INATIVO E RENDERIZAR COM ELE NA COR DESEJADA
		WD_TextureRender(s->textLogarOff, 325 / 2, 800 / 2);
		WD_TextureRender(s->textModoOff, 800 / 2, 800 / 2);
        WD_TextureRender(s->seta, 740/2, 806/2);
	}
	else {
		WD_TextureRender(s->textLogar, 325 / 2, 800 / 2);
		WD_TextureRender(s->textModoOffOff, 800 / 2, 800 / 2);
        WD_TextureRender(s->seta, 238/2, 806/2);
	}

}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    SDL_DestroyTexture(s->textTexture);
    SDL_DestroyTexture(s->textLogar);
    SDL_DestroyTexture(s->textModoOff);
    free(s);
}

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN && s->modoOff) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SINGLEPLAYER);
        }if(e->key.keysym.sym == SDLK_RIGHT){
            s->modoOff=true; //BOTAO MODOOFFLINE ATIVO >>>>>
        }if(e->key.keysym.sym == SDLK_LEFT){
            s->modoOff=false; //BOTAO MODOOFFLINE INATIVO <<<<
        }
    }
}
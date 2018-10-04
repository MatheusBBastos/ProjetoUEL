#include "scene_lobby.h"

Scene_Lobby* SceneLobby_new() {
    Scene_Lobby* newScene = malloc(sizeof(Scene_Lobby));

    newScene->nome = WD_CreateTexture();
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->mutiplayer = WD_CreateTexture();
    newScene->server = WD_CreateTexture();
    newScene->entrar = WD_CreateTexture();
    newScene->servir = WD_CreateTexture();
    newScene->server1 = WD_CreateTexture();
    newScene->server2 = WD_CreateTexture();
    newScene->server3 = WD_CreateTexture();
    newScene->nomeServer1 = WD_CreateTexture();
    newScene->nomeServer2 = WD_CreateTexture();
    newScene->nomeServer3 = WD_CreateTexture();
    newScene->index = 0;
    newScene->page = newScene->index/3;
    newScene->esquerda = true;

    SDL_Color Cname = {204, 204, 204};
    SDL_Color Cmult = {0, 132, 255};
    SDL_Color Cwhite = {255, 255, 255};

    newScene->boxIp = WD_CreateTextBox(75 * gInfo.screenMulti, 750 * gInfo.screenMulti, 380 * gInfo.screenMulti, 52 * gInfo.screenMulti, 16, gInfo.lobbyFonte, Cwhite, false);

    WD_TextureLoadFromText(newScene->nome, "Basto Forte", gInfo.lobbyName, Cname);
    WD_TextureLoadFromText(newScene->mutiplayer, "MULTIPLAYER", gInfo.lobbyFonte, Cmult);
    WD_TextureLoadFromText(newScene->server, "SERVER: 2", gInfo.lobbyFonte, Cwhite);
    WD_TextureLoadFromText(newScene->entrar, "Entrar", gInfo.lobbyFonte, Cwhite);
    WD_TextureLoadFromText(newScene->servir, "Servir", gInfo.lobbyFonte, Cwhite);
    WD_TextureLoadFromText(newScene->server1, "Server #1", gInfo.lobbyFontd, Cwhite);
    WD_TextureLoadFromText(newScene->server2, "Server #2", gInfo.lobbyFontd, Cwhite);
    WD_TextureLoadFromText(newScene->server3, "Server #3", gInfo.lobbyFontd, Cwhite);
    WD_TextureLoadFromText(newScene->nomeServer1, "Basto forte", gInfo.lobbyFontd, Cwhite);
    WD_TextureLoadFromText(newScene->nomeServer2, "Melvi forte", gInfo.lobbyFontd, Cwhite);
    WD_TextureLoadFromText(newScene->nomeServer3, "Tampy fraco ;(", gInfo.lobbyFontd, Cwhite);

    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_mainMenu.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w*gInfo.screenMulti;
    newScene->renderQuad.h = h*gInfo.screenMulti;
    newScene->frame = 0;
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
    s->frame++;
    if(s->frame >= gInfo.screenFreq) {
        s->frame = 0;
    }

    SDL_SetTextureColorMod(s->entrar->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->servir->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->server1->mTexture, 0, 132, 255);
    SDL_SetTextureColorMod(s->server2->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->server3->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer1->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer2->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->nomeServer3->mTexture, 255, 255, 255);

    if(s->index == 0) {
        if(s->esquerda) {
            s->boxIp->active = true;
        } else {
            SDL_SetTextureColorMod(s->nomeServer1->mTexture, 255, 66, 0);
        }
    } else {
        s->boxIp->active = false;
    }

    if(s->index == 1) {
        if(s->esquerda) {
            SDL_SetTextureColorMod(s->entrar->mTexture, 255, 66, 0);
        } else {
            SDL_SetTextureColorMod(s->nomeServer2->mTexture, 255, 66, 0);
        }
    } else if(s->index >= 2) {
        if(s->esquerda) {
            SDL_SetTextureColorMod(s->servir->mTexture, 255, 66, 0);
        } else {
            SDL_SetTextureColorMod(s->nomeServer3->mTexture, 255, 66, 0);
        }
    }

    WD_TextureRender(s->entrar, 160 * gInfo.screenMulti, 825 * gInfo.screenMulti);
    WD_TextureRender(s->servir, 160 * gInfo.screenMulti, 895 * gInfo.screenMulti);
    WD_TextureRender(s->server1, 700 * gInfo.screenMulti, 515 * gInfo.screenMulti);
    WD_TextureRender(s->nomeServer1, 800 * gInfo.screenMulti, 585 * gInfo.screenMulti);
    WD_TextureRender(s->server2, 700 * gInfo.screenMulti, 655 * gInfo.screenMulti);
    WD_TextureRender(s->nomeServer2, 800 * gInfo.screenMulti, 725 * gInfo.screenMulti);
    WD_TextureRender(s->server3, 700 * gInfo.screenMulti, 795 * gInfo.screenMulti);
    WD_TextureRender(s->nomeServer3, 800 * gInfo.screenMulti, 865 * gInfo.screenMulti);
}

void SceneLobby_destroy(Scene_Lobby* s) {
    WD_TextureDestroy(s->nome);
    WD_TextureDestroy(s->mutiplayer);
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->server);
    WD_TextureDestroy(s->entrar);
    WD_TextureDestroy(s->servir);
    WD_TextureDestroy(s->server1);
    WD_TextureDestroy(s->server2);
    WD_TextureDestroy(s->server3);
    WD_TextureDestroy(s->nomeServer1);
    WD_TextureDestroy(s->nomeServer2);
    WD_TextureDestroy(s->nomeServer3);
    free(s);
}

void SceneLobby_handleEvent(Scene_Lobby* s, SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {   
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            if(s->index < 2 && s->esquerda)
                s->index++;
            else if(s->index < 9 && !s->esquerda) //colocar variavel do nmr de servidores
                s->index++;

            if(s->index > 2 && !s->esquerda) {
                SDL_Color color = {255, 255, 255};
                //s->page = s->index/3;
                sprintf(s->string1, "Server #%d", s->index-1);
                sprintf(s->string2, "Server #%d", s->index);
                sprintf(s->string3, "Server #%d", s->index+1);
                WD_TextureLoadFromText(s->server1, s->string1, gInfo.lobbyFontd, color);
                WD_TextureLoadFromText(s->server2, s->string2, gInfo.lobbyFontd, color);
                WD_TextureLoadFromText(s->server3, s->string3, gInfo.lobbyFontd, color);
            }

        } else if(e->key.keysym.sym == SDLK_UP) {
            if(s->index > 0)
                s->index--;
            
            if(s->index > 1 && !s->esquerda) {
                SDL_Color color = {255, 255, 255};
                //s->page = s->index/3;
                sprintf(s->string1, "Server #%d", s->index-1);
                sprintf(s->string2, "Server #%d", s->index);
                sprintf(s->string3, "Server #%d", s->index+1);
                WD_TextureLoadFromText(s->server1, s->string1, gInfo.lobbyFontd, color);
                WD_TextureLoadFromText(s->server2, s->string2, gInfo.lobbyFontd, color);
                WD_TextureLoadFromText(s->server3, s->string3, gInfo.lobbyFontd, color);
            }
            
            
        } else if(e->key.keysym.sym == SDLK_RIGHT) {
            s->esquerda = false;
        } else if(e->key.keysym.sym == SDLK_LEFT) {
            if(!s->esquerda && s->index > 2)
                s->index = 2;
            s->esquerda = true; 
        }
        printf("%d\n", s->index);   

    }

    WD_TextBoxHandleEvent(s->boxIp, e);

}

#include "scene_login.h"

    int posSetaX[2] = { 720,245 };
    int posSetaY = 806;

    int posLogarX = 325;
    int posLogarY = 800;

    int posModoX = 800;
    int posModoY = 800;

    int posLoginX = 480;
    int posLoginY = 580;
    int sizeLogin[2] = { 520,52 };

    int posSenhaX = 480;
    int posSenhaY = 670;
    int sizeSenha[2] = { 520,52 };

    int posErroX = 438;
    int posErroY = 960;

Scene_Login* SceneLogin_new() {
    Scene_Login* newScene = malloc(sizeof(Scene_Login));
    newScene->enteringFrame = 0;
    newScene->frame = 0;

    newScene->music = Mix_LoadMUS("content/teste.ogg");
    Mix_PlayMusic(newScene->music, -1);
    Mix_PauseMusic();

    SDL_Color colorSelected = {255, 156, 0}; // Cores dos botões quando selecionados
    SDL_Color colorNotSelected = {255,255,255}; // Cores dos botões quando não selecionados
    SDL_Color fullRed = { 255,0,0 };

    newScene->textError = WD_CreateTexture();
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->textLogar = WD_CreateTexture();
    newScene->textModoOff = WD_CreateTexture();
    newScene->textLogarOff = WD_CreateTexture();
    newScene->textModoOffOff = WD_CreateTexture();
    newScene->seta = WD_CreateTexture();
    newScene->modoOff = false;
    newScene->index=0; // Começar no login

    WD_TextureLoadFromFile(newScene->seta, "content/seta.png");
    WD_TextureLoadFromText(newScene->textLogar, "Logar" , gInfo.menuFont, colorSelected);
    WD_TextureLoadFromText(newScene->textLogarOff, "Logar", gInfo.menuFont, colorNotSelected);
    WD_TextureLoadFromText(newScene->textModoOff, "Modo offline", gInfo.menuFont, colorSelected);
    WD_TextureLoadFromText(newScene->textModoOffOff, "Modo offline", gInfo.menuFont, colorNotSelected);
    WD_TextureLoadFromText(newScene->textError, "ACESSO NEGADO", gInfo.menuFont, fullRed);

    newScene->seta->h *= gInfo.screenMulti;
    newScene->seta->w *= gInfo.screenMulti;

    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_Login.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w * gInfo.screenMulti;
    newScene->renderQuad.h = h * gInfo.screenMulti;
    SDL_Color textColor = {50, 50, 50, 255};
    newScene->login = WD_CreateTextBox(posLoginX * gInfo.screenMulti, posLoginY * gInfo.screenMulti, sizeLogin[0] * gInfo.screenMulti, sizeLogin[1] * gInfo.screenMulti, 30, gInfo.inputFont, textColor, false);
    newScene->senha = WD_CreateTextBox(posSenhaX* gInfo.screenMulti, posSenhaY* gInfo.screenMulti, sizeSenha[0] * gInfo.screenMulti, sizeSenha[1] * gInfo.screenMulti, 30, gInfo.inputFont, textColor, true);

    newScene->acessonegado = false;
    SDL_StartTextInput();
    
    return newScene;
}

void SceneLogin_update(Scene_Login* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0x00);

    WD_TextureRender(s->textLogarOff, posLogarX * gInfo.screenMulti, posLogarY * gInfo.screenMulti); //Começa com os dois botoes brancos
    WD_TextureRender(s->textModoOffOff, posModoX * gInfo.screenMulti, posModoY * gInfo.screenMulti);

    if (s->frame % 5 == 0) {
        if (s->acessonegado) {
            s->acessonegado = false;
        }
        else {
            s->acessonegado = true;
        }
    }

    if (s->acessonegado) {
        WD_TextureRender(s->textError, posErroX * gInfo.screenMulti, posErroY * gInfo.screenMulti);
    }

    if (s->modoOff && s->index==2) {
        WD_TextureRender(s->textLogarOff, posLogarX * gInfo.screenMulti, posLogarY * gInfo.screenMulti);
        WD_TextureRender(s->textModoOff, posModoX * gInfo.screenMulti, posModoY * gInfo.screenMulti);
        WD_TextureRender(s->seta, posSetaX[0] * gInfo.screenMulti, posSetaY * gInfo.screenMulti);
    }
    else if(!s->modoOff && s->index==2) {
        WD_TextureRender(s->textLogar, posLogarX * gInfo.screenMulti, posLogarY * gInfo.screenMulti);
        WD_TextureRender(s->textModoOffOff, posModoX * gInfo.screenMulti, posModoY * gInfo.screenMulti);
        WD_TextureRender(s->seta, posSetaX[1] * gInfo.screenMulti, posSetaY * gInfo.screenMulti);
    }
    SDL_SetRenderDrawBlendMode(gInfo.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gInfo.renderer, 0xFF, 0xFF, 0xFF, 100);
    SDL_Rect rect = {480 * gInfo.screenMulti, 580 * gInfo.screenMulti, 520 * gInfo.screenMulti, 52 * gInfo.screenMulti };
    SDL_RenderFillRect(gInfo.renderer, &rect);
    rect.y += 90 * gInfo.screenMulti;
    SDL_RenderFillRect(gInfo.renderer, &rect);
    if(s->index == 0) {
        s->login->active = true;
    } else {
        s->login->active = false;
    }
    if(s->index == 1) {
        s->senha->active = true;
    } else {
        s->senha->active = false;
    }
    WD_TextBoxRender(s->login, s->frame);
    WD_TextBoxRender(s->senha, s->frame);
    if(s->enteringFrame < 100) {
        SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 255 - 2.5 * s->enteringFrame);
        SDL_Rect fillRect = {0, 0, gInfo.screenWidth, gInfo.screenHeight};
        SDL_RenderFillRect(gInfo.renderer, &fillRect);
        s->enteringFrame++;
    }
    s->frame++;
    if(s->frame >= gInfo.screenFreq) {
        s->frame = 0;
    }
}

void SceneLogin_destroy(Scene_Login* s) {
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->textLogar);
    WD_TextureDestroy(s->textLogarOff);
    WD_TextureDestroy(s->textModoOff);
    WD_TextureDestroy(s->textModoOffOff);
    WD_TextBoxDestroy(s->login);
    WD_TextBoxDestroy(s->senha);
    Mix_FreeMusic(s->music);
    SDL_StopTextInput();
    free(s);
}

void SceneLogin_handleEvent(Scene_Login* s, SDL_Event* e) {
    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAP);
        } else if(e->key.keysym.sym == SDLK_RETURN && s->modoOff && s->index == 2 ) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
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
        else if (e->key.keysym.sym == SDLK_F2) {
            if (Mix_PausedMusic()) {
                Mix_ResumeMusic();
            }
            else {
                Mix_PauseMusic();
            }
        }
    }
    WD_TextBoxHandleEvent(s->login, e);
    WD_TextBoxHandleEvent(s->senha, e);
}
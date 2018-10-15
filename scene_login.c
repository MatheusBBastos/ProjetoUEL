#include "scene_login.h"
#include "jsmn.h"
#include "rsa.h"

int posSetaX[2] = { 720,245 };

Scene_Login* SceneLogin_new() {
    Scene_Login* newScene = malloc(sizeof(Scene_Login));
    newScene->enteringFrame = 0;
    newScene->frame = 0;

    newScene->music = Mix_LoadMUS("content/teste.ogg");
    Mix_PlayMusic(newScene->music, -1);
    Mix_PauseMusic();

    SDL_Color colorWhite = { 255,255,255 };
    SDL_Color fullRed = { 255,0,0 };

    newScene->loginPressed = false;
    newScene->connectionNotStarted = true;

    newScene->loading = WD_CreateTexture();
    newScene->logo[0] = WD_CreateTexture();
    newScene->logo[1] = WD_CreateTexture();
    newScene->textError = WD_CreateTexture();
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->textLogar = WD_CreateTexture();
    newScene->textModoOff = WD_CreateTexture();
    newScene->seta = WD_CreateTexture();
    newScene->modoOff = false;
    newScene->index = 0; // Começar no login

    WD_TextureLoadFromFile(newScene->loading, "content/loading.png");
    WD_TextureLoadFromFile(newScene->logo[0], "content/dalhebomba.png");
    WD_TextureLoadFromFile(newScene->logo[1], "content/logo.png");
    WD_TextureLoadFromFile(newScene->seta, "content/seta.png");
    WD_TextureLoadFromText(newScene->textLogar, "Logar", Game.telaLogin, colorWhite);
    WD_TextureLoadFromText(newScene->textModoOff, "Modo offline", Game.telaLogin, colorWhite);
    WD_TextureLoadFromText(newScene->textError, "ACESSO NEGADO", Game.telaLogin, fullRed);

    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_Login.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w;
    newScene->renderQuad.h = h;
    SDL_Color textColor = { 50, 50, 50, 255 };
    newScene->login = WD_CreateTextBox(480, 580, 520, 52, 30, Game.inputFont, textColor, false);
    newScene->senha = WD_CreateTextBox(480, 670, 520, 52, 30, Game.inputFont, textColor, true);

    newScene->acessonegado = false;
    SDL_StartTextInput();
    newScene->positionAnimado = 0;

    return newScene;
}

void SceneLogin_update(Scene_Login* s) {
    if (s->loginPressed) {

        if (s->socketFd != 0 && s->connectionNotStarted) {
            s->dataReceived = false;
            s->connected = false;
            s->socketFd = TCPSocket_Open();
            TCPSocket_Connect(s->socketFd, "35.198.20.77", 3122);
            s->connectionNotStarted = false;
        }

        if (s->socketFd != 0 && !s->dataReceived) {
            if (!s->connected) {
                int c = TCPSocket_CheckConnectionStatus(s->socketFd);
                if (c == 1) {
                    s->connected = true;
                    char message[1500], resp[1000];
                    encryptRSA(s->senha->text, resp);
                    sprintf(message, "{\"cmd\":\"login\",\"var\":{\"login\":\"%s\",\"senha\":\"%s\"}}\n", s->login->text, resp);
                    TCPSocket_Send(s->socketFd, message, strlen(message));
                }
                else if (c == -1) {
                    Socket_Close(s->socketFd);
                    s->socketFd = 0;
                }
            }
            else {
                char data[2000];
                int c = TCPSocket_Receive(s->socketFd, data, 2000);
                if (c > 0) {
                    data[c] = '\0';
                    int r;
                    jsmn_parser p;
                    jsmntok_t t[128];
                    jsmn_init(&p);
                    r = jsmn_parse(&p, data, strlen(data), t, sizeof(t) / sizeof(t[0]));
                    if (r < 0) {
                        printf("Failed to parse JSON: %d\n", r);
                    }

                    char resposta[50]; bool logado = false;
                    for (int i = 0; i < r; i++) {
                        if (jsoneq(data, &t[i], "playerName") == 0) {
                            sprintf(resposta, "%.*s", t[i + 1].end - t[i + 1].start, data + t[i + 1].start);
                            logado = true;
                        }
                    }

                    if (logado) {
                        puts(resposta);
                        strcpy(Game.nome, resposta);
                        strcpy(Game.loginID, s->login->text);
                        SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
                    }
                    else {
                        s->acessonegado = true;
                    }

                    s->dataReceived = true;
                    Socket_Close(s->socketFd);
                    s->loginPressed = false;
                    s->connectionNotStarted = true;
                }
                else if (c == -1) {
                    Socket_Close(s->socketFd);
                    s->loginPressed = false;
                    s->connectionNotStarted = true;
                }
            }
        }
    }
    SDL_SetRenderDrawColor(Game.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(Game.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    SDL_SetRenderDrawColor(Game.renderer, 0x00, 0x00, 0x00, 0x00);

    if (s->loginPressed && s->socketFd != 0 && !s->dataReceived) {
        double angle = Game.screenFreq / 60.0 * 6 * s->frame;
        int loadingX = (REFERENCE_WIDTH - 100) / 2, loadingY = REFERENCE_HEIGHT - 100 - 20;
        SDL_Rect c = {loadingX, loadingY, 100, 100};
        SDL_RenderCopyEx(Game.renderer, s->loading->mTexture, NULL, &c, angle, NULL, SDL_FLIP_NONE);
    }
    WD_TextureRender(s->textLogar, 325, 800); //Começa com os dois botoes brancos
    WD_TextureRender(s->textModoOff, 800, 800);
    WD_TextureRender(s->logo[1], 215, 200);

    SDL_SetTextureColorMod(s->textLogar->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->textModoOff->mTexture, 255, 255, 255);

    if (s->acessonegado) {
        WD_TextureRender(s->textError, 438, 960);
    }

    if (s->modoOff && s->index == 2) {
        SDL_SetTextureColorMod(s->textModoOff->mTexture, 247, 159, 55);
        WD_RenderArrow(s->seta, 800, 800, s->textModoOff->w);
    }
    else if (!s->modoOff && s->index == 2) {
        SDL_SetTextureColorMod(s->textLogar->mTexture, 247, 159, 55);
        WD_RenderArrow(s->seta, 325, 800, s->textLogar->w);
    }
    SDL_SetRenderDrawBlendMode(Game.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game.renderer, 0xFF, 0xFF, 0xFF, 100);
    SDL_Rect rect = { 480, 580, 520, 52 };
    SDL_RenderFillRect(Game.renderer, &rect);
    rect.y += 90;
    SDL_RenderFillRect(Game.renderer, &rect);
    if (s->index == 0) {
        s->login->active = true;
    }
    else {
        s->login->active = false;
    }
    if (s->index == 1) {
        s->senha->active = true;
    }
    else {
        s->senha->active = false;
    }
    WD_TextBoxRender(s->login, s->frame);
    WD_TextBoxRender(s->senha, s->frame);
    if (s->enteringFrame < 100) {
        SDL_SetRenderDrawColor(Game.renderer, 0x00, 0x00, 0x00, 255 - 2.5 * s->enteringFrame);
        SDL_Rect fillRect = { 0, 0, REFERENCE_WIDTH, REFERENCE_HEIGHT };
        SDL_RenderFillRect(Game.renderer, &fillRect);
        s->enteringFrame++;
    }
    s->frame++;
    if(s->frame >= Game.screenFreq) {
        s->frame = 0;
    }

    if (s->frame % 6 == 0) {
        s->positionAnimado++;
    }



    if (s->positionAnimado == 16)
        s->positionAnimado = 0;

    SDL_Rect clip = { 0, 320 * s->positionAnimado, 320, 320 };
    WD_TextureRenderExCustom(s->logo[0], 900, 100, &clip, 0.0, NULL, SDL_FLIP_NONE, 100, 100);

}


void SceneLogin_destroy(Scene_Login* s) {
    WD_TextureDestroy(s->loading);
    WD_TextureDestroy(s->logo[0]);
    WD_TextureDestroy(s->logo[1]);
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->textLogar);
    WD_TextureDestroy(s->textModoOff);
    WD_TextBoxDestroy(s->login);
    WD_TextBoxDestroy(s->senha);
    WD_TextureDestroy(s->seta);
    Mix_FreeMusic(s->music);
    SDL_StopTextInput();
    free(s);
}

void SceneLogin_handleEvent(Scene_Login* s, SDL_Event* e) {
    if (SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN && s->modoOff && s->index == 2 ) {
            strcpy(Game.nome, "User Teste");
            strcpy(Game.loginID, "NULLRANK");
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAINMENU);
        }
        else if (e->key.keysym.sym == SDLK_RETURN && !s->modoOff && s->index == 2)
        {
            s->loginPressed = true;
        }
        else if (e->key.keysym.sym == SDLK_RIGHT && s->index == 2 || e->key.keysym.sym == SDLK_LEFT && s->index == 2) {
            s->modoOff = !s->modoOff;
        }
        else if ((e->key.keysym.sym == SDLK_DOWN || e->key.keysym.sym == SDLK_TAB) && s->index < 2) {
            s->index++;
        }
        else if (e->key.keysym.sym == SDLK_UP && s->index > 0) {
            s->index--;
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
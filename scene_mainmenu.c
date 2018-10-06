#include "scene_mainmenu.h"
#include "network.h"
#include "jsmn.h"

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->bemvindo = WD_CreateTexture();
    newScene->nome = WD_CreateTexture();
    newScene->jogar = WD_CreateTexture();
    newScene->tutorial = WD_CreateTexture();
    newScene->logout = WD_CreateTexture();
    newScene->seta =  WD_CreateTexture();
    newScene->multiplayer = WD_CreateTexture();
    newScene->singleplayer = WD_CreateTexture();
    newScene->index = 0;
    newScene->btnJogar=false;
    newScene->mult=true;

    SDL_Color colorBemvindo = {141,38,38}; 
    SDL_Color colorWhite = {255, 255, 255};

    newScene->connected = false;
    newScene->dataReceived = false;
    newScene->socketFd = TCPSocket_Open();
    if(newScene->socketFd != 0)
        TCPSocket_Connect(newScene->socketFd, "35.198.20.77", 3122);
    char anw[6][20];
    for (int i = 0; i < 5; i++) {
        strcpy(anw[i], "Carregando...");
    }

    for (int i = 0; i < 5; i++) {
        newScene->rank[i] = WD_CreateTexture();
        WD_TextureLoadFromText(newScene->rank[i], anw[i], Game.rank, colorWhite);
    }


    WD_TextureLoadFromText(newScene->bemvindo,"Bem Vindo", Game.mainMenu, colorBemvindo);
    WD_TextureLoadFromText(newScene->nome, Game.nome, Game.mainMenu, colorWhite);

    WD_TextureLoadFromText(newScene->jogar, "Jogar", Game.mainMenu_botoes, colorWhite);
    WD_TextureLoadFromText(newScene->tutorial, "Tutorial", Game.mainMenu_botoes, colorWhite);
    WD_TextureLoadFromText(newScene->logout, "Logout", Game.mainMenu_botoes, colorWhite);

    WD_TextureLoadFromText(newScene->multiplayer, "Multiplayer", Game.mainMenu_botoes, colorWhite);
    WD_TextureLoadFromText(newScene->singleplayer, "Singleplayer", Game.mainMenu_botoes, colorWhite);

    WD_TextureLoadFromFile(newScene->seta, "content/seta.png");
    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_mainMenu.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w*Game.screenMulti;
    newScene->renderQuad.h = h*Game.screenMulti;


    newScene->seta->h *= Game.screenMulti;
    newScene->seta->w *= Game.screenMulti;

    return newScene;
}


int getRank(char res[6][20], char* data) {
    int r;
    jsmn_parser p;
    jsmntok_t t[128];

    jsmn_init(&p);
    r = jsmn_parse(&p, data, strlen(data), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }

    char nomes[5][50];
    char scores[5][50];
    sprintf(res[5], "Seu rank #%.*s",t[31 + 1].end - t[31 + 1].start,
        data + t[31 + 1].start);

    for (int i1 = 3, i2 = 5, i3 = 0; i3 < 5; i1 += 6, i2 += 6, i3++) {
        sprintf(nomes[i3], "%.*s", t[i2 + 1].end - t[i2 + 1].start,
            data + t[i2 + 1].start);
        sprintf(scores[i3], "%.*s", t[i1 + 1].end - t[i1 + 1].start,
            data + t[i1 + 1].start);
    }

    for (int i = 0; i < 5; i++) {
        sprintf(res[i],"#%d %s   %s", i+1, nomes[i], scores[i]);
    }
    return 0;
}



void SceneMainMenu_update(Scene_MainMenu* s) {
    if(s->socketFd != 0 && !s->dataReceived) {
        if(!s->connected) {
            int c = TCPSocket_CheckConnectionStatus(s->socketFd);
            if(c == 1) {
                s->connected = true;
                char message[] = "{\"cmd\":\"getRank\",\"var\":{\"login\":\"201800560244\"}}\n";
                TCPSocket_Send(s->socketFd, message, strlen(message));
            } else if(c == -1) {
                Socket_Close(s->socketFd);
                s->socketFd = 0;
            }
        } else {
            char data[2000];
            int c = TCPSocket_Receive(s->socketFd, data, 2000);
            if(c > 0) {
                char anw[6][20];
                data[c] = '\0';
                puts(data);
                if(getRank(anw, data) == 0) {
                    SDL_Color colorNotSelected = {255, 255, 255};
                    for (int i = 0; i < 5; i++) {
                        WD_TextureLoadFromText(s->rank[i], anw[i], Game.rank, colorNotSelected);
                    }
                    printf("%s",(anw[5]));
                }
                s->dataReceived = true;
                Socket_Close(s->socketFd);
            } else if(c == -1) {
                Socket_Close(s->socketFd);
            }
        }
    }

    SDL_RenderClear(Game.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    WD_TextureRender(s->bemvindo, 84 * Game.screenMulti, 566 * Game.screenMulti);
    WD_TextureRender(s->nome, 84 * Game.screenMulti, 634 * Game.screenMulti);

    for (int i = 0; i < 5; i++) {
        WD_TextureRender(s->rank[i], 900 * Game.screenMulti, (590 + ((i)*70))* Game.screenMulti);
    }

    if(!s->btnJogar) {
        if(s->index == 0) {
            SDL_SetTextureColorMod(s->jogar->mTexture, 255, 66, 0);
            SDL_SetTextureColorMod(s->tutorial->mTexture, 255, 255, 255);
            SDL_SetTextureColorMod(s->logout->mTexture, 255, 255, 255);
            WD_TextureRender(s->seta, 76 * Game.screenMulti, 740 * Game.screenMulti);
        } else if(s->index == 1) {
            SDL_SetTextureColorMod(s->jogar->mTexture, 255, 255, 255);
            SDL_SetTextureColorMod(s->tutorial->mTexture, 255, 66, 0);
            SDL_SetTextureColorMod(s->logout->mTexture, 255, 255, 255);
            WD_TextureRender(s->seta, 76 * Game.screenMulti, 824 * Game.screenMulti);
        } else if(s->index == 2){
            SDL_SetTextureColorMod(s->jogar->mTexture, 255, 255, 255);
            SDL_SetTextureColorMod(s->tutorial->mTexture, 255, 255, 255);
            SDL_SetTextureColorMod(s->logout->mTexture, 255, 66, 0);
            WD_TextureRender(s->seta, 76 * Game.screenMulti, 912 * Game.screenMulti);
        }

        WD_TextureRender(s->jogar, 156 * Game.screenMulti, 720 *   Game.screenMulti);
        WD_TextureRender(s->tutorial, 156 * Game.screenMulti, 804 * Game.screenMulti);
        WD_TextureRender(s->logout, 156 * Game.screenMulti, 892 * Game.screenMulti);
    } else {
        WD_TextureRender(s->jogar, 156 * Game.screenMulti, 720 * Game.screenMulti);
        if(s->mult) {
                WD_TextureRender(s->seta, 160 * Game.screenMulti, 824 * Game.screenMulti);
                SDL_SetTextureColorMod(s->multiplayer->mTexture, 255, 66, 0);
                SDL_SetTextureColorMod(s->singleplayer->mTexture, 255, 255, 255);
        } else {
                WD_TextureRender(s->seta, 160 * Game.screenMulti, 912 * Game.screenMulti);
                SDL_SetTextureColorMod(s->multiplayer->mTexture, 255, 255, 255);
                SDL_SetTextureColorMod(s->singleplayer->mTexture, 255, 66, 0);
        }
        WD_TextureRender(s->multiplayer, 240 * Game.screenMulti, 804 * Game.screenMulti);
        WD_TextureRender(s->singleplayer, 240 * Game.screenMulti, 892 * Game.screenMulti);
    }

}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->bemvindo);
    WD_TextureDestroy(s->nome);
    WD_TextureDestroy(s->jogar);
    WD_TextureDestroy(s->tutorial);
    WD_TextureDestroy(s->logout);
    WD_TextureDestroy(s->seta);
    WD_TextureDestroy(s->multiplayer);
    WD_TextureDestroy(s->singleplayer);
    free(s);
    
}

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e) {

    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            if(s->btnJogar)
                s->mult=false;
            else if(s->index<2)
                s->index++;
        } else if(e->key.keysym.sym == SDLK_UP) {
            if(s->btnJogar)
                s->mult=true;
            else if(s->index>0)
                s->index--;
        }  else if (e->key.keysym.sym == SDLK_RETURN && s-> index == 0) {
            if(s->btnJogar == true)
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SERVERS);
            else
                s->btnJogar = true;
        }  else if (e->key.keysym.sym == SDLK_RETURN && s->index == 1) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_TUTORIAL);
        }  else if (e->key.keysym.sym == SDLK_RETURN && s->index == 2) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        }  else if (e->key.keysym.sym == SDLK_ESCAPE && s->btnJogar) {
            s->btnJogar = false;
            s->mult = true;
        }
    }
    

}
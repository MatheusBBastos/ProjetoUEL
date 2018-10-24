#include "scene_mainmenu.h"
#include "network.h"
#include "jsmn.h"
#include "time.h"

SDL_Color blueMenu = { 91,116,212 };

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
    newScene->rankPOS = WD_CreateTexture();
    newScene->index = 0;
    newScene->btnJogar=false;
    newScene->mult=true;
    newScene->animatedChar = WD_CreateTexture();
    newScene->frame = 0;
    newScene->animation = false;

    SDL_Color colorBemvindo = {141,38,38}; 
    SDL_Color colorWhite = {255, 255, 255};
    SDL_Color orange = { 255,172,65 };

    newScene->connected = false;
    newScene->dataReceived = false;
    newScene->socketFd = TCPSocket_Open();
    if(newScene->socketFd != 0)
        TCPSocket_Connect(newScene->socketFd, "35.198.20.77", 3122);
    char anw[6][20];
    for (int i = 0; i < 5; i++) {
        strcpy(anw[i], "OFFLINE");
    }

    for (int i = 0; i < 5; i++) {
        newScene->rankName[i] = WD_CreateTexture();
        newScene->scores[i] = WD_CreateTexture();
        newScene->posMark[i] = WD_CreateTexture();
        WD_TextureLoadFromText(newScene->rankName[i], anw[i], Game.Unisans, colorWhite);
    }

    WD_TextureLoadFromText(newScene->bemvindo, "BEM VINDO", Game.mainMenu, colorWhite);
    WD_TextureLoadFromText(newScene->nome, Game.nome, Game.mainMenu, blueMenu);

    WD_TextureLoadFromText(newScene->jogar, "Jogar", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromText(newScene->tutorial, "Tutorial", Game.mainMenu_botoes, orange);
    WD_TextureLoadFromText(newScene->logout, "Logout", Game.mainMenu_botoes, orange);

    WD_TextureLoadFromText(newScene->multiplayer, "Multiplayer", Game.mainMenu_botoes, colorWhite);
    WD_TextureLoadFromText(newScene->singleplayer, "Singleplayer", Game.mainMenu_botoes, colorWhite);

    WD_TextureLoadFromFile(newScene->seta, "content/seta.png");

    char cor[30];
    srand(time(NULL));   // Initialization, should only be called once.
    int r = rand() % 4;
    switch (r) {
    case 0:
        sprintf(cor, "content/azul.png");
        break;
    case 1:
        sprintf(cor, "content/vermelho.png");
        break;
    case 2:
        sprintf(cor, "content/roxo.png");
        break;
    case 3:
        sprintf(cor, "content/amarelo.png");
        break;
    default:
        sprintf(cor, "content/azul.png");
        break;
    }

    WD_TextureLoadFromFile(newScene->animatedChar,cor);
    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/bgrank.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w;
    newScene->renderQuad.h = h;


    //newScene->seta->h *= Game.screenMulti;
    //newScene->seta->w *= Game.screenMulti;

    return newScene;
}


int getRank(char res[2][6][30], char* data) {
    int r;
    jsmn_parser p;
    jsmntok_t t[128];

    jsmn_init(&p);
    r = jsmn_parse(&p, data, strlen(data), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }

    char rankdoplayer[50];
    char nomes[5][50];
    char scores[5][50];
    sprintf(rankdoplayer, "%.*s",t[31 + 1].end - t[31 + 1].start,
        data + t[31 + 1].start);
    Game.rankPos = atoi(rankdoplayer);

    for (int i1 = 3, i2 = 5, i3 = 0; i3 < 5; i1 += 6, i2 += 6, i3++) {
        sprintf(nomes[i3], "%.*s", t[i2 + 1].end - t[i2 + 1].start,
            data + t[i2 + 1].start);
        sprintf(scores[i3], "%.*s", t[i1 + 1].end - t[i1 + 1].start,
            data + t[i1 + 1].start);
    }

    for (int i = 0; i < 5; i++) {
       // sprintf(res[i],"#%d %s   %s", i+1, nomes[i], scores[i]);
        sprintf(res[0][i], "%s", nomes[i]);
        char temp[5];
        sprintf(temp, "%s", scores[i]);
        int sc = atoi(temp);
        if (sc < 100) {
            sprintf(res[1][i], "0%d", atoi(temp));
            if (sc < 10) {
                sprintf(res[1][i], "00%d", atoi(temp));
            }
        }
        else {
            sprintf(res[1][i], "%d", atoi(temp));
        }
    }
    return 0;
}



void SceneMainMenu_update(Scene_MainMenu* s) {
    if(s->socketFd != 0 && !s->dataReceived) {
        if(!s->connected) {
            int c = TCPSocket_CheckConnectionStatus(s->socketFd);
            if(c == 1) {
                s->connected = true;
                char message[120];
                sprintf(message, "{\"cmd\":\"getRank\",\"var\":{\"login\":\"%s\"}}\n", Game.loginID);
                TCPSocket_Send(s->socketFd, message, strlen(message));
            } else if(c == -1) {
                Socket_Close(s->socketFd);
                s->socketFd = 0;
            }
        } else {
            char data[2000];
            int c = TCPSocket_Receive(s->socketFd, data, 2000);
            if(c > 0) {
                char anw[2][6][30];
                data[c] = '\0';
                if(getRank(anw, data) == 0) {
                    SDL_Color colorNotSelected = {255, 255, 255};
                    for (int i = 0; i < 5; i++) {
                        WD_TextureLoadFromText(s->rankName[i], anw[0][i], Game.Unisans, colorNotSelected);
                        WD_TextureLoadFromText(s->scores[i], anw[1][i], Game.roboto, colorNotSelected);
                        char ex[5];
                        sprintf(ex, "%d.", i + 1);
                        WD_TextureLoadFromText(s->posMark[i], ex, Game.Unisans, colorNotSelected);
                    }
                    char var[20];
                    if (Game.rankPos != -1) {
                        sprintf(var, "#%d", Game.rankPos);
                        WD_TextureLoadFromText(s->rankPOS, var, Game.rankMini, blueMenu);
                    }
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
    WD_TextureRender(s->bemvindo, 270, 835);
    WD_TextureRender(s->nome, 270, 910);

    SDL_Rect indexJogar = { 135, 570 + s->jogar->h , s->jogar->w, 5 };
    SDL_Rect indexTutorial = { 135, 645 + s->tutorial->h , s->tutorial->w, 5 };
    SDL_Rect indexLogout = { 135, 720 + s->logout->h , s->logout->w, 5 };

    SDL_Rect indexMulti = { 200, 490 + s->multiplayer->h , s->multiplayer->w, 5 };
    SDL_Rect indexSingle = { 200, 560 + s->singleplayer->h , s->singleplayer->w, 5 };



    SDL_SetRenderDrawBlendMode(Game.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Game.renderer, 0, 125, 0xFF, 100);



    for (int i = 0; i < 5; i++) {
        WD_TextureRender(s->rankName[i], 1170, (505 + ((i)*70)));
        WD_TextureRender(s->posMark[i], 1100, (505 + ((i) * 70)));
        WD_TextureRender(s->scores[i], 1330, (495 + ((i) * 70)));
    }

    WD_TextureRender(s->rankPOS, 270, 990);

    if(!s->btnJogar) {
        WD_TextureRender(s->jogar, 135, 570);
        switch (s->index) {
        case 0:
            SDL_RenderFillRect(Game.renderer, &indexJogar);
            break;
        case 1:
            SDL_RenderFillRect(Game.renderer, &indexTutorial);
            break;
        case 2:
            SDL_RenderFillRect(Game.renderer, &indexLogout);
            break;
        }
    } else {
        WD_TextureRender(s->jogar, 135, 420);
        if(s->mult) {
            SDL_RenderFillRect(Game.renderer, &indexMulti);
        } else {
            SDL_RenderFillRect(Game.renderer, &indexSingle);
        }
        WD_TextureRender(s->multiplayer, 200, 490);
        WD_TextureRender(s->singleplayer, 200, 560);
    }

    WD_TextureRender(s->tutorial, 135, 645);
    WD_TextureRender(s->logout, 135, 720);
    s->frame++;
    if (s->frame >= Game.screenFreq) {
        s->frame = 0;
    }

    int current = 0;

    if (s->frame % 15 == 0) {
        s->animation = !s->animation;
    }

    if (s->animation) {
        current = 2;
    }
    else {
        current = 0;
    }


    SDL_Rect clip = { 64 * current, 0, 64, 64 };
    WD_TextureRenderExCustom(s->animatedChar, 125, 830, &clip, 0.0, NULL, SDL_FLIP_NONE, 128, 128);

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
            if (s->btnJogar) {
                s->btnJogar = false;
                s->mult = true;
            }
            else {
                Mix_PlayChannel(-1, Game.enter, 0);
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
            }
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            if (s->btnJogar) {
                s->mult = false;
                Mix_PlayChannel(-1, Game.change, 0);
            }
            else if (s->index < 2) {
                Mix_PlayChannel(-1, Game.change, 0);
                s->index++;
            }
        } else if(e->key.keysym.sym == SDLK_UP) {
            if (s->btnJogar) {
                s->mult = true;
                Mix_PlayChannel(-1, Game.change, 0);
            }
            else if (s->index > 0) {
                Mix_PlayChannel(-1, Game.change, 0);
                s->index--;
            }
        }  else if (e->key.keysym.sym == SDLK_RETURN && s-> index == 0) {
            if (s->btnJogar == true) {
                Mix_PlayChannel(-1, Game.enter, 0);
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SERVERS);
            }
            else
                s->btnJogar = true;
        }  else if (e->key.keysym.sym == SDLK_RETURN && s->index == 1) {
            Mix_PlayChannel(-1, Game.enter, 0);
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_TUTORIAL);
        }  else if (e->key.keysym.sym == SDLK_RETURN && s->index == 2) {
            Mix_PlayChannel(-1, Game.enter, 0);
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        }  else if (e->key.keysym.sym == SDLK_ESCAPE && s->btnJogar) {
            s->btnJogar = false;
            s->mult = true;
        }
    }
    

}
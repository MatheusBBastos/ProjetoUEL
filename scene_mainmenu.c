#include "scene_mainmenu.h"
#include "network.h"
#include "jsmn.h"
#define BUFFER_SIZE 5000
#define MAX_TOKEN_COUNT 128

Scene_MainMenu* SceneMainMenu_new() {
    Scene_MainMenu* newScene = malloc(sizeof(Scene_MainMenu));
    newScene->backgroundTexture = WD_CreateTexture();
    newScene->bemvindo = WD_CreateTexture();
    newScene->nome = WD_CreateTexture();
    newScene->jogar = WD_CreateTexture();
    newScene->tutorial = WD_CreateTexture();
    newScene->logout = WD_CreateTexture();
    newScene->jogarOff = WD_CreateTexture();
    newScene->tutorialOff = WD_CreateTexture();
    newScene->logoutOff = WD_CreateTexture();
    newScene->seta =  WD_CreateTexture();//
    newScene->index = 0;


    SDL_Color colorBemvindo = {141,38,38}; 
    SDL_Color colorNome = {255, 255, 255};
    SDL_Color colorSelected = {255, 66, 0};
    SDL_Color colorNotSelected = {255, 255, 255};

    char anw[5][20];
    if (getRank(anw)) {
        for (int i = 0; i < 5; i++) {
            strcpy(anw[i], "NULL");
        }
    }
    for (int i = 0; i < 5; i++) {
        newScene->rank[i] = WD_CreateTexture();
        WD_TextureLoadFromText(newScene->rank[i], anw[i], gInfo.rank, colorNotSelected);
    }


    WD_TextureLoadFromText(newScene->bemvindo,"Bem Vindo", gInfo.mainMenu, colorBemvindo);
    WD_TextureLoadFromText(newScene->nome, "Basto Forte", gInfo.mainMenu, colorNome);

    WD_TextureLoadFromText(newScene->jogar, "Jogar", gInfo.mainMenu_botoes, colorSelected);
    WD_TextureLoadFromText(newScene->tutorial, "Tutorial", gInfo.mainMenu_botoes, colorSelected);
    WD_TextureLoadFromText(newScene->logout, "Logout", gInfo.mainMenu_botoes, colorSelected);
    WD_TextureLoadFromText(newScene->jogarOff, "Jogar", gInfo.mainMenu_botoes, colorNotSelected);
    WD_TextureLoadFromText(newScene->tutorialOff, "Tutorial", gInfo.mainMenu_botoes, colorNotSelected);
    WD_TextureLoadFromText(newScene->logoutOff, "Logout", gInfo.mainMenu_botoes, colorNotSelected);

    WD_TextureLoadFromFile(newScene->seta, "content/seta.png");
    WD_TextureLoadFromFile(newScene->backgroundTexture, "content/BG_mainMenu.png");
    int w = newScene->backgroundTexture->w, h = newScene->backgroundTexture->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = w*gInfo.screenMulti;
    newScene->renderQuad.h = h*gInfo.screenMulti;


    newScene->seta->h *= gInfo.screenMulti;
    newScene->seta->w *= gInfo.screenMulti;

    return newScene;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int getRank(char res[5][20]) {
    int s;
    struct sockaddr_in server;
    char *message, server_reply[2000];
    int recv_size;



    //Create a socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }


    server.sin_addr.s_addr = inet_addr("35.198.20.77");
    server.sin_family = AF_INET;
    server.sin_port = htons(3122);

    //Connect to remote server
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }


    //Send some data
    message = "{\"cmd\":\"getRank\"}\n";
    if (send(s, message, strlen(message), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    //Receive a reply from the server
    if ((recv_size = recv(s, server_reply, 2000, 0)) == -1)
    {
        puts("recv failed");
        return 1;
    }


    //Add a NULL terminating character to make it a proper string before printing
    server_reply[recv_size] = '\0';
    puts(server_reply);


    ///////////////START THE JSON PARSING

    int r;
    jsmn_parser p;
    jsmntok_t t[128];

    jsmn_init(&p);
    r = jsmn_parse(&p, server_reply, strlen(server_reply), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }

    char nomes[5][50];
    char scores[5][50];

    for (int i1 = 3, i2 = 5, i3 = 0; i3 < 5; i1 += 6, i2 += 6, i3++) {
        sprintf(nomes[i3], "%.*s", t[i2 + 1].end - t[i2 + 1].start,
            server_reply + t[i2 + 1].start);
        sprintf(scores[i3], "%.*s", t[i1 + 1].end - t[i1 + 1].start,
            server_reply + t[i1 + 1].start);
    }

    for (int i = 0; i < 5; i++) {
        sprintf(res[i],"#%d %s   %s", i+1, nomes[i], scores[i]);
    }
    return 0;
    
}



void SceneMainMenu_update(Scene_MainMenu* s) {
    //SDL_SetRenderDrawColor(gInfo.renderer, 0x12, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    WD_TextureRenderDest(s->backgroundTexture, &s->renderQuad);
    WD_TextureRender(s->bemvindo, 84 * gInfo.screenMulti, 566 * gInfo.screenMulti);
    WD_TextureRender(s->nome, 84 * gInfo.screenMulti, 634 * gInfo.screenMulti);
    //WD_TextureRender(s->jogar, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
    //WD_TextureRender(s->tutorial, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
    //WD_TextureRender(s->logout, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);

    for (int i = 0; i < 5; i++) {
        WD_TextureRender(s->rank[i], 900 * gInfo.screenMulti, (590 + ((i)*70))* gInfo.screenMulti);
    }

    if(s->index == 0) {
        WD_TextureRender(s->seta, 76 * gInfo.screenMulti, 750 * gInfo.screenMulti);
        WD_TextureRender(s->jogar, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
        WD_TextureRender(s->tutorialOff, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
        WD_TextureRender(s->logoutOff, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);
    } else if(s->index == 1) {
        WD_TextureRender(s->seta, 76 * gInfo.screenMulti, 834 * gInfo.screenMulti);
        WD_TextureRender(s->jogarOff, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
        WD_TextureRender(s->tutorial, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
        WD_TextureRender(s->logoutOff, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);
    } else {
        WD_TextureRender(s->seta, 76 * gInfo.screenMulti, 922 * gInfo.screenMulti);
        WD_TextureRender(s->jogarOff, 156 * gInfo.screenMulti, 720 * gInfo.screenMulti);
        WD_TextureRender(s->tutorialOff, 156 * gInfo.screenMulti, 804 * gInfo.screenMulti);
        WD_TextureRender(s->logout, 156 * gInfo.screenMulti, 892 * gInfo.screenMulti);
    }

}

void SceneMainMenu_destroy(Scene_MainMenu* s) {
    WD_TextureDestroy(s->backgroundTexture);
    WD_TextureDestroy(s->bemvindo);
    WD_TextureDestroy(s->nome);
    WD_TextureDestroy(s->jogar);
    WD_TextureDestroy(s->tutorial);
    WD_TextureDestroy(s->logout);
    WD_TextureDestroy(s->jogarOff);
    WD_TextureDestroy(s->tutorialOff);
    WD_TextureDestroy(s->logoutOff);
    WD_TextureDestroy(s->seta);
    free(s);
    
}

void SceneMainMenu_handleEvent(Scene_MainMenu* s, SDL_Event* e) {

    if(sMng.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if((e->key.keysym.sym == SDLK_DOWN ) && s->index < 2) {
            s->index++;
        } else if(e->key.keysym.sym == SDLK_UP && s->index > 0) {
            s->index--;
        }  else if (e->key.keysym.sym == SDLK_RETURN && s-> index == 0) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOBBY);
        }  else if (e->key.keysym.sym == SDLK_RETURN && s->index == 1) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_TUTORIAL);
        }  else if (e->key.keysym.sym == SDLK_RETURN && s->index == 2) {
            sMng.quit = true;
        } 
    }
    

}
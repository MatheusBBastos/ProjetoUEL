#include "scene_lobby.h"

Scene_Lobby* SceneLobby_new() {
    Scene_Lobby* newScene = malloc(sizeof(Scene_Lobby));
    for(int i = 0; i < 4; i++) {
        sprintf(newScene->playerNames[i], "Slot #%d", i + 1);
    }
    newScene->player1 = WD_CreateTexture();
    newScene->player2 = WD_CreateTexture();
    newScene->player3 = WD_CreateTexture();
    newScene->player4 = WD_CreateTexture();
    newScene->iniciar = WD_CreateTexture();
    newScene->sair = WD_CreateTexture();

    Game.map = Map_Create();
    
    SceneLobby_Receive(newScene);

    SDL_Color Cmsg = {255, 255, 255};

    WD_TextureLoadFromText(newScene->player1, newScene->playerNames[0], Game.inputFont, Cmsg);
    WD_TextureLoadFromText(newScene->player2, newScene->playerNames[1], Game.inputFont, Cmsg);
    WD_TextureLoadFromText(newScene->player3, newScene->playerNames[2], Game.inputFont, Cmsg);
    WD_TextureLoadFromText(newScene->player4, newScene->playerNames[3], Game.inputFont, Cmsg);
    WD_TextureLoadFromText(newScene->iniciar, "Iniciar", Game.inputFont, Cmsg);
    WD_TextureLoadFromText(newScene->sair, "Sair", Game.inputFont, Cmsg);

    return newScene;
}

void SceneLobby_Receive(Scene_Lobby* s) {
    Address sender;
    char data[256];
    while(Socket_Receive(Network.sockFd, &sender, data, 256) > 0) {
        if(sender.address == Network.serverAddress->address && sender.port == Network.serverAddress->port) {
            printf("[Client] Received from server: %s\n", data);
            if(strncmp("KCK", data, 3) == 0) {
                Network.connectedToServer = false;
                Socket_Close(Network.sockFd);
            } else if(strncmp("PNM", data, 3) == 0) {
                int id;
                char name[32];
                sscanf(data + 4, "%d %s", &id, name);
                strcpy(s->playerNames[id], name);
            }
        }
    }
}

void SceneLobby_update(Scene_Lobby* s) {
    WD_TextureRender(s->player1, 300 * Game.screenMulti, 300 * Game.screenMulti);
    WD_TextureRender(s->player2, 1000 * Game.screenMulti, 300 * Game.screenMulti);
    WD_TextureRender(s->player3, 300 * Game.screenMulti, 750 * Game.screenMulti);
    WD_TextureRender(s->player4, 1000 * Game.screenMulti, 750 * Game.screenMulti);
}

void SceneLobby_destroy(Scene_Lobby* s) {
    Map_Destroy(Game.map);
    Game.map = NULL;
    WD_TextureDestroy(s->player1);
    WD_TextureDestroy(s->player2);
    WD_TextureDestroy(s->player3);
    WD_TextureDestroy(s->player4);
    WD_TextureDestroy(s->iniciar);
    WD_TextureDestroy(s->sair);
    free(s);
}   

void SceneLobby_handleEvent(Scene_Lobby* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SERVERS);
        }
    }
}
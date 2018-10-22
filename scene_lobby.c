#include "scene_lobby.h"

Scene_Lobby* SceneLobby_new() {
    Scene_Lobby* newScene = malloc(sizeof(Scene_Lobby));
     newScene->sound = Mix_LoadMUS("content/lobby.mp3");
   Mix_PlayMusic(newScene->sound, -1);
    for(int i = 0; i < 4; i++) {
        sprintf(newScene->playerNames[i], "Slot");
    }
    newScene->players = malloc(MAX_PLAYERS * sizeof(WTexture*));
    for(int i = 0; i < MAX_PLAYERS; i++) {
        newScene->players[i] = WD_CreateTexture();
    }
    newScene->iniciar = WD_CreateTexture();
    newScene->sair = WD_CreateTexture();

    newScene->pingCount = 0;

    Game.map = Map_Create();
    
    SceneLobby_Receive(newScene);

    SDL_Color Cmsg = {255, 255, 255};

    for(int i = 0; i < MAX_PLAYERS; i++) {
        WD_TextureLoadFromText(newScene->players[i], newScene->playerNames[i], Game.inputFont, Cmsg);
    }
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
            if(strncmp("KCK", data, 3) == 0 || strncmp("SSD", data, 3) == 0) {
                Network.connectedToServer = false;
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SERVERS);
            } else if(strncmp("PNM", data, 3) == 0) {
                int id;
                char name[32];
                sscanf(data + 4, "%d %31[^\n]", &id, name);
                strcpy(s->playerNames[id], name);
                SDL_Color color = {255, 255, 255};
                WD_TextureLoadFromText(s->players[id], s->playerNames[id], Game.inputFont, color);
            } else if(strncmp("CNM", data, 3) == 0) {
                int nmb;
                sscanf(data + 4, "%d", &nmb);
                if(nmb != Game.map->charNumber) {
                    Game.map->charNumber = nmb;
                    Game.map->characters = realloc(Game.map->characters, nmb * sizeof(Character*));
                }
            } else if(strncmp("PDC", data, 3) == 0) {
                int id;
                sscanf(data + 4, "%d", &id);
                strcpy(s->playerNames[id], "Slot");
                SDL_Color color = {255, 255, 255};
                WD_TextureLoadFromText(s->players[id], s->playerNames[id], Game.inputFont, color);
            } else if(strncmp("CHR", data, 3) == 0) {
                int id;
                int x, y, dir;
                char file[64];
                sscanf(data + 4, "%d %d %d %d %s", &x, &y, &dir, &id, file);
                if(Game.map->characters[id] != NULL) {
                    Character_Destroy(Game.map->characters[id]);
                }
                Game.map->characters[id] = Character_Create(file, id, false);
                Game.map->characters[id]->x = x;
                Game.map->characters[id]->renderX = x;
                Game.map->characters[id]->x4 = x * 4;
                Game.map->characters[id]->y = y;
                Game.map->characters[id]->renderY = y;
                Game.map->characters[id]->y4 = y * 4;
                Game.map->characters[id]->direction = dir;
            } else if(strncmp("MAP", data, 3) == 0) {
                char mapPath[64];
                sscanf(data + 4, "%s", mapPath);
                Map_Load(Game.map, mapPath, true);
            } else if(strncmp("GEN", data, 3) == 0) {
                int seed, wallNumber;
                sscanf(data + 4, "%d %d", &seed, &wallNumber);
                Game.map->wallNumber = wallNumber;
                Game.map->walls = malloc(wallNumber * sizeof(Wall));
                Map_GenerateWalls(Game.map, seed);
                if(Game.map->loaded) {
                    SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAP);
                }
            }
        }
    }
}

void SceneLobby_update(Scene_Lobby* s) {
    s->pingCount++;
    if(s->pingCount == Game.screenFreq) {
        s->pingCount = 0;
        char data[] = "PNG";
        Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
    }
    SceneLobby_Receive(s);
    SDL_SetRenderDrawColor(Game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(Game.renderer);
    WD_TextureRender(s->players[0], 300, 300);
    WD_TextureRender(s->players[1], 1000, 300);
    WD_TextureRender(s->players[2], 300, 750);
    WD_TextureRender(s->players[3], 1000, 750);
}

void SceneLobby_destroy(Scene_Lobby* s) {
    for(int i = 0; i < MAX_PLAYERS; i++) {
        WD_TextureDestroy(s->players[i]);
    }
    free(s->players);
    WD_TextureDestroy(s->iniciar);
    WD_TextureDestroy(s->sair);
    Mix_FreeMusic(s->sound);
    free(s);
}   

void SceneLobby_handleEvent(Scene_Lobby* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_ESCAPE) {
            if(Network.connectedToServer) {
                char data[] = "DCS";
                Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
                Network.connectedToServer = false;
            }
            if(Network.serverHost)
                Server_Close(Network.server);
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_SERVERS);
            Map_Destroy(Game.map);
            Game.map = NULL;
        } else if(e->key.keysym.sym == SDLK_SPACE) {
            //if(Network.serverHost) {
                char data[] = "STA";
                Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
            //}
        }
    }
}
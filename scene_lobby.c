#include "scene_lobby.h"

Scene_Lobby* SceneLobby_new() {
    Scene_Lobby* newScene = malloc(sizeof(Scene_Lobby));

    newScene->sound = Mix_LoadMUS("content/lobby.mp3");
    SDL_Color Cmsg = { 255, 255, 255 };
    Mix_PlayMusic(newScene->sound, -1);
    for (int i = 0; i < 4; i++) {
        sprintf(newScene->playerNames[i], "Slot");
    }
    newScene->players = malloc(MAX_PLAYERS * sizeof(WTexture*));
    for (int i = 0; i < MAX_PLAYERS; i++) {
        newScene->players[i] = WD_CreateTexture();
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        WD_TextureLoadFromText(newScene->players[i], newScene->playerNames[i], Game.inputFont, Cmsg);
        newScene->started[i] = false;
        newScene->curPos[i] = 0;
        newScene->conected[i] = false;
        newScene->animfinished[i] = false;
        newScene->playersSprite[i] = WD_CreateTexture();
    }

    newScene->bg = WD_CreateTexture();
    newScene->spawn = Mix_LoadWAV("content/feffect.mp3");
    newScene->pingCount = 0;
    newScene->frame = 0;
    newScene->helptext[0] = WD_CreateTexture();
    newScene->helptext[1] = WD_CreateTexture();
    newScene->animation = WD_CreateTexture();

    WD_TextureLoadFromFile(newScene->animation, "content/ASK.png");

    Game.map = Map_Create();
    SceneLobby_Receive(newScene);

    WD_TextureLoadFromFile(newScene->bg, "content/bglobby.png");

    WD_TextureLoadFromFile(newScene->playersSprite[0], "content/azul.png");
    WD_TextureLoadFromFile(newScene->playersSprite[1], "content/amarelo.png");
    WD_TextureLoadFromFile(newScene->playersSprite[2], "content/vermelho.png");
    WD_TextureLoadFromFile(newScene->playersSprite[3], "content/roxo.png");

    WD_TextureLoadFromText(newScene->helptext[0], "Pressione espaço para começar", Game.tip, (SDL_Color) { 255, 255, 255 });
    WD_TextureLoadFromText(newScene->helptext[1], "Adicionar bot: (Z) Fácil, (X) Médio, (C) Difícil", Game.tip, (SDL_Color) { 255, 255, 255 });
    
    return newScene;
}


void SceneLobby_Receive(Scene_Lobby* s) {
    Address sender;
    char data[256];
    while (Socket_Receive(Network.sockFd, &sender, data, 256) > 0) {
        if (sender.address == Network.serverAddress->address && sender.port == Network.serverAddress->port) {
            if (strcmp("PNG", data) != 0)
                printf("[Client] Received from server: %s\n", data);
            Network.lastReceivedCount = 0;
            if (strncmp("KCK", data, 3) == 0 || strncmp("SSD", data, 3) == 0) {
                Network.connectedToServer = false;
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, Network.singleplayer ? SCENE_SINGLEPLAYER : SCENE_SERVERS);
            } else if (strncmp("PNM", data, 3) == 0) {
                int id;
                char name[32];
                sscanf(data + 4, "%d %31[^\n]", &id, name);
                strcpy(Network.playerNames[id], name);
                SDL_Color color = {255, 255, 255};
                WD_TextureLoadFromText(s->players[id], Network.playerNames[id], Game.inputFont, color);
                s->started[id] = true;
                s->conected[id] = true;
            } else if (strncmp("CNM", data, 3) == 0) {
                int nmb;
                sscanf(data + 4, "%d", &nmb);
                if (nmb != Game.map->charNumber) {
                    Game.map->charNumber = nmb;
                    Game.map->characters = realloc(Game.map->characters, nmb * sizeof(Character*));
                }
            } else if (strncmp("PDC", data, 3) == 0) {
                int id;
                sscanf(data + 4, "%d", &id);
                strcpy(Network.playerNames[id], "Slot");
                SDL_Color color = {255, 255, 255};
                WD_TextureLoadFromText(s->players[id], Network.playerNames[id], Game.inputFont, color);
                s->started[id] = false;
                s->conected[id] = false;
            } else if (strncmp("CHR", data, 3) == 0) {
                int id;
                int x, y, dir;
                char file[64];
                sscanf(data + 4, "%d %d %d %d %s", &x, &y, &dir, &id, file);
                if (Game.map->characters[id] != NULL) {
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
            } else if (strncmp("MAP", data, 3) == 0) {
                char mapPath[64];
                sscanf(data + 4, "%s", mapPath);
                Map_Load(Game.map, mapPath, true);
            } else if (strncmp("GEN", data, 3) == 0) {
                int seed, wallNumber;
                sscanf(data + 4, "%d %d", &seed, &wallNumber);
                Game.map->wallNumber = wallNumber;
                Game.map->walls = malloc(wallNumber * sizeof(Wall));
                Map_GenerateWalls(Game.map, seed);
                if (Game.map->loaded) {
                    SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_MAP);
                    return;
                }
            }
        }
    }
    Network.lastReceivedCount++;
    if (Network.lastReceivedCount > Game.screenFreq * 5) {
        SceneLobby_Disconnect(s);
    }
}


void SceneLobby_Disconnect(Scene_Lobby* s) {
    if (Network.connectedToServer) {
        char data[] = "DCS";
        Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
        Network.connectedToServer = false;
    }
    if (Network.serverHost)
        Server_Close(Network.server);
    SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, Network.singleplayer ? SCENE_SINGLEPLAYER : SCENE_SERVERS);
    Map_Destroy(Game.map);
    Game.map = NULL;
}


void SceneLobby_update(Scene_Lobby* s) {
    s->pingCount++;
    if (s->pingCount == Game.screenFreq) {
        s->pingCount = 0;
        Socket_Send(Network.sockFd, Network.serverAddress, "PNG", 4);
    }
    if (!SceneManager.inTransition)
        SceneLobby_Receive(s);
    SDL_SetRenderDrawColor(Game.renderer, 0, 0, 0, 255);
    SDL_RenderClear(Game.renderer);
    WD_TextureRender(s->bg, 0, 0);

    if (Network.serverHost) {
        WD_TextureRender(s->helptext[0], 1440/2 - s->helptext[0]->w/2, 900);
        WD_TextureRender(s->helptext[1], 1440 / 2 - s->helptext[1]->w / 2, 1000);
    }

    SDL_Rect defaultPos = { 64,0, 64, 64 };
    for (int i = 0; i < 4; i++) {
        if (s->conected[i] && s->animfinished[i]) {
            int posX = (288 * (i + 1));
            switch (i) {
                case 0:
                    WD_TextureRenderExCustom(s->playersSprite[i], posX - 192 / 2, 500, &defaultPos, 0.0, NULL, SDL_FLIP_NONE, 192, 192);
                    break;
                case 1:
                    WD_TextureRenderExCustom(s->playersSprite[i], posX - 192 / 2, 500, &defaultPos, 0.0, NULL, SDL_FLIP_NONE, 192, 192);
                    break;
                case 2:
                    WD_TextureRenderExCustom(s->playersSprite[i], posX - 192 / 2, 500, &defaultPos, 0.0, NULL, SDL_FLIP_NONE, 192, 192);
                    break;
                case 3:
                    WD_TextureRenderExCustom(s->playersSprite[i], posX - 192 / 2, 500, &defaultPos, 0.0, NULL, SDL_FLIP_NONE, 192, 192);
                    break;
            }
            WD_TextureRender(s->players[i], posX - s->players[i]->w / 2, 700);
        }
    }

    for (int i = 0; i < 4; i++) {
        if (s->started[i]) {
            int posX = (288 * (i + 1));
            SDL_Rect clip = { 0, 192 * s->curPos[i], 192, 192 };
            WD_TextureRenderExCustom(s->animation, posX - 448 / 2, 350, &clip, 0.0, NULL, SDL_FLIP_NONE, 448, 448);

            if (s->curPos[i] == 0) {
                Mix_PlayChannel(-1, s->spawn, 0);
            }
            if (s->frame % 4 == 0) {
                s->curPos[i]++;
            }
            if (s->curPos[i] == 5) {
                s->animfinished[i] = true;
            }
            if (s->curPos[i] > 11) {
                s->started[i] = false;
            }
        }
    }

    s->frame++;
    if (s->frame >= 60) {
        s->frame = 0;
    }

}


void SceneLobby_destroy(Scene_Lobby* s) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        WD_TextureDestroy(s->players[i]);
    }
    free(s->players);
    Mix_FreeMusic(s->sound);
    Mix_FreeChunk(s->spawn);
    free(s);
}   


void SceneLobby_handleEvent(Scene_Lobby* s, SDL_Event* e) {
    if (SceneManager.inTransition)
        return;
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_ESCAPE) {
            if (Network.connectedToServer) {
                char data[] = "DCS";
                Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
                Network.connectedToServer = false;
            }
            if (Network.serverHost)
                Server_Close(Network.server);
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, Network.singleplayer ? SCENE_SINGLEPLAYER : SCENE_SERVERS);
            Map_Destroy(Game.map);
            Game.map = NULL;
        } else if (e->key.keysym.sym == SDLK_SPACE) {
            if (Network.serverHost) {
                char data[] = "STA";
                Socket_Send(Network.sockFd, Network.serverAddress, data, 4);
            }
        } else if (e->key.keysym.sym == SDLK_z) {
            if (Network.serverHost) {
                Socket_Send(Network.sockFd, Network.serverAddress, "BOT 0", 6);
            }
        } else if (e->key.keysym.sym == SDLK_x) {
            if (Network.serverHost) {
                Socket_Send(Network.sockFd, Network.serverAddress, "BOT 1", 6);
            }
        } else if (e->key.keysym.sym == SDLK_c) {
            if (Network.serverHost) {
                Socket_Send(Network.sockFd, Network.serverAddress, "BOT 2", 6);
            }
        }
    }
}
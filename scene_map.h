#ifndef SCENE_MAP_H
#define SCENE_MAP_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"
#include "map.h"
#include "character.h"
#include "server.h"

typedef struct Bomb {
    int x, y;
    bool active;
    int frame;
} Bomb;

typedef struct Explosion {
    int x, y, xMin, xMax, yMin, yMax;
    bool active;
    int explosionCount;
    short int explosionFrame;
    bool expanding;
} Explosion;

Bomb_Render(Bomb* b, int screenX, int screenY, WTexture* bombSprite, int frame);

Explosion_Render(Explosion* e, int screenX, int screenY, WTexture* explosionSprite);

typedef struct Scene_Map {
    WTexture* tileMap;
    WTexture* animatedBomb;
    WTexture *explosionSprite;
    WTexture* wallTexture;
    WTexture* puTexture;
    WTexture* winChar;
    WTexture* loseChar;
    WTexture* winText;
    WTexture* playerNames[4];
    WTexture* placement[4];
    WTexture* bg;
    WTexture* status[4];
    bool keyUp, keyDown, keyRight, keyLeft;
    bool waitingConnection;
    bool frozen, ended;
    bool connected;
    int realPlayer;
    int finalRanking[4];
    WTexture* deadTexture;
    int tileWidth, tileHeight, mapWidth, mapHeight;
    int screenX, screenY;
    int* renderCharacters;
    int charNumber;
    int sockFd;
    int currentFrame;
    int pingCount;
    int endOpacity;
    int kills[4];
    int socketFd;
    int myScore;
    char lastMov;
    Bomb bombs[20];
    Character* player;
    Character** characters;
    Explosion explosions[20];
    Map* map;
    Mix_Chunk* bombexp;
    Mix_Chunk* bombload;
    Mix_Chunk* ded;
    Mix_Chunk* winSound;
    Mix_Chunk* pickup;
    Mix_Music* backgroundMusic;
    SDL_Thread* serverThread;
    Server* testServer;
} Scene_Map;

Scene_Map* SceneMap_new();

void SceneMap_Receive(Scene_Map* s);

void SceneMap_update(Scene_Map* s);

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e);

void SceneMap_destroy(Scene_Map* s);

#endif
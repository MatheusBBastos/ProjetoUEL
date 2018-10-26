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
} Explosion;

Bomb_Render(Bomb* b, int screenX, int screenY, WTexture* bombSprite, int frame);

Explosion_Render(Explosion* e, int screenX, int screenY, WTexture* explosionSprite);

typedef struct Scene_Map {
    WTexture* tileMap;
    WTexture* animatedBomb;
    WTexture *bombSprite, *explosionSprite;
    WTexture* wallTexture;
    WTexture* puTexture;
    Map* map;
    Mix_Chunk* bombexp;
    Mix_Chunk* bombload;
    Mix_Chunk* ded;
    Mix_Music* backgroundMusic;
    int tileWidth, tileHeight, mapWidth, mapHeight;
    int screenX, screenY;
    Character* player;
    Character** characters;
    int* renderCharacters;
    int charNumber;
    Server* testServer;
    SDL_Thread* serverThread;
    bool waitingConnection;
    int sockFd;
    char lastMov;
    bool keyUp, keyDown, keyRight, keyLeft;
    Bomb bombs[20];
    Explosion explosions[20];
    int currentFrame;
    int pingCount;
    bool frozen, ended;
    int endOpacity;
    WTexture* placement[4];
    int kills[4];
} Scene_Map;

Scene_Map* SceneMap_new();

void SceneMap_Receive(Scene_Map* s);

void SceneMap_update(Scene_Map* s);

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e);

void SceneMap_destroy(Scene_Map* s);

#endif
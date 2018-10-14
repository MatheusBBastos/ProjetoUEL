#ifndef MAP_H
#define MAP_H

#include "headers.h"
#include "widgets.h"
#include "character.h"

typedef struct Character Character;

typedef struct TemporaryObject {
    bool exists;
    bool isWall;
    int objId;
} TemporaryObject;

typedef struct Wall {
    bool exists;
    int x, y;
} Wall;

typedef struct Map {
    int* data;
    bool loaded;
    int width, height;
    SDL_Texture** layers;
    Character** characters;
    TemporaryObject** objects;
    Wall* walls;
    int wallNumber;
    int charNumber;
} Map;

Map* Map_Create();

void Map_Load(Map* map, char* path, bool loadTexture);

int Map_Get(Map* m, int x, int y, int z);

void Map_Set(Map* m, int x, int y, int z, int value);

void Map_GenerateWalls(Map* m, int seed);

void Map_DestroyCharacters(Map* m);

void Map_RenderFull(Map* m, WTexture* tileMap);

void Map_RenderWalls(Map* m, WTexture* wallTexture, int screenX, int screenY);

void Map_Render(Map* m, WTexture* tileMap, int screenX, int screenY);

bool Map_Passable(Map* m, SDL_Rect* box);

void Map_Destroy(Map* m);

#endif
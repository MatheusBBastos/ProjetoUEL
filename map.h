#ifndef MAP_H
#define MAP_H

#include "headers.h"
#include "widgets.h"
#include "character.h"

typedef struct Character Character;

typedef enum ObjectType {
    OBJ_WALL,
    OBJ_BOMB,
    OBJ_POWERUP
} ObjType;

typedef struct TemporaryObject {
    bool exists;
    ObjType type;
    int objId;
} TemporaryObject;

typedef struct Wall {
    bool exists;
    int x, y;
} Wall;

typedef struct PowerUp {
    bool exists;
    int x, y, type;
} PowerUp;

typedef struct Map {
    int* data;
    bool loaded;
    int width, height;
    SDL_Texture** layers;
    Character** characters;
    TemporaryObject** objects;
    Wall* walls;
    PowerUp* powerups;
    int wallNumber;
    int powerupNumber;
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

void Map_RenderPowerUps(Map* m, WTexture* puTexture, int screenX, int screenY, int currentFrame);

void Map_Render(Map* m, WTexture* tileMap, int screenX, int screenY);

bool Map_Passable(Map* m, SDL_Rect* box, Character* c);

bool Map_CheckSafeSpot(Map* m, int x, int y, int checkRange);

void Map_Destroy(Map* m);

#endif
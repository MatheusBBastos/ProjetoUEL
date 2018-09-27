#ifndef CHARACTER_H
#define CHARACTER_H

#include "headers.h"
#include "widgets.h"
#include "map.h"
#include "network.h"

typedef struct Character {
    int id;
    bool player;
    WTexture* sprite;
    short int direction;
    bool moving;
    int x, y;
    int renderX, renderY;
    int x4, y4;
    int moveSpeed;
    int animationIndex;
    int animationCount;
    bool animPart;
    char spriteFile[32];
} Character;

Character* Character_Create(char* spritePath, int id, bool noTexture);

void Character_GetCollisionBox(Character* c, SDL_Rect* box, int offsetX, int offsetY);

void Character_TryToMove(Character* c, int dir, Map* m, Character** characters, int charNumber);

void Character_Update(Character* c, Map* m, Character** characters, int charNumber);

void Character_Render(Character* c, int screenX, int screenY);

void Character_Destroy(Character* c);

#endif
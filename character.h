#ifndef CHARACTER_H
#define CHARACTER_H

#include "headers.h"
#include "widgets.h"
#include "map.h"

typedef struct Character {
    int id;
    bool player;
    WTexture* sprite;
    short int direction;
    bool moving;
    int x, y;
    int animationIndex;
    int animationCount;
} Character;

Character* Character_Create(char* spritePath, int id);

void Character_GetCollisionBox(Character* c, SDL_Rect* box, int offsetX, int offsetY);

void Character_Update(Character* c, Map* m, Character** characters, int charNumber);

void Character_Render(Character* c, int screenX, int screenY);

void Character_Destroy(Character* c);

#endif
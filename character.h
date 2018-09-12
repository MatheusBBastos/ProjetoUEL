#ifndef CHARACTER_H
#define CHARACTER_H

#include "headers.h"
#include "widgets.h"

typedef struct Character {
    bool player;
    WTexture* sprite;
    short int direction;
    bool moving;
    int x, y;
    int animationIndex;
    int animationCount;
} Character;

Character* Character_Create(char* spritePath);

void Character_Update(Character* c);

void Character_Render(Character* c, int screenX, int screenY);

void Character_Destroy(Character* c);

#endif
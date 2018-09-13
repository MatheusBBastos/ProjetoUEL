#include "character.h"

Character* Character_Create(char* spritePath) {
    Character* newCharacter = malloc(sizeof(Character));
    newCharacter->sprite = WD_CreateTexture();
    WD_TextureLoadFromFile(newCharacter->sprite, spritePath);
    newCharacter->direction = 0;
    newCharacter->moving = false;
    newCharacter->x = 0;
    newCharacter->y = 0;
    newCharacter->animationIndex = 0;
    newCharacter->animationCount = 0;
    return newCharacter;
}

void Character_Update(Character* c, Map* m) {
    if(c->moving) {
        c->animationCount++;
        if(c->animationCount == 8) {
            c->animationCount = 0;
            c->animationIndex = (c->animationIndex + 1) % 3;
        }
        int collisionWidth = c->sprite->w / 6;
        int collisionHeight = c->sprite->h / 8;

        if(c->direction == 0) {
            int newY = c->y + 1;
            if(Map_Passable(m, c->x + (c->sprite->w / 3 - collisionWidth) / 2, newY + (c->sprite->h / 4 - collisionHeight), collisionWidth, collisionHeight)) {
                c->y = newY;
            }
            //c->y += 2;
        } else if(c->direction == 1) {
            int newX = c->x - 1;
            if(Map_Passable(m, newX + (c->sprite->w / 3 - collisionWidth) / 2, c->y + (c->sprite->h / 4 - collisionHeight), collisionWidth, collisionHeight)) {
                c->x = newX;
            }
        } else if(c->direction == 2) {
            int newX = c->x + 1;
            if(Map_Passable(m, newX + (c->sprite->w / 3 - collisionWidth) / 2, c->y + (c->sprite->h / 4 - collisionHeight), collisionWidth, collisionHeight)) {
                c->x = newX;
            }
        } else if(c->direction == 3) {
            int newY = c->y - 1;
            if(Map_Passable(m, c->x + (c->sprite->w / 3 - collisionWidth) / 2, newY + (c->sprite->h / 4 - collisionHeight), collisionWidth, collisionHeight)) {
                c->y = newY;
            }
        }
    } else {
        c->animationCount = 0;
        c->animationIndex = 1;
    }
}

void Character_Render(Character* c, int screenX, int screenY) {
    int realX = c->x - screenX;
    int realY = c->y - screenY;
    if(realX + c->sprite->w >= 0 && realY + c->sprite->h >= 0 && realX < gInfo.screenWidth && realY < gInfo.screenHeight) {
        SDL_Rect clip = {c->sprite->w / 3 * c->animationIndex, c->sprite->h / 4 * c->direction, c->sprite->w / 3, c->sprite->h / 4};
        WD_TextureRenderEx(c->sprite, realX, realY, &clip, 0.0, NULL, SDL_FLIP_NONE);
    }
}

void Character_Destroy(Character* c) {
    WD_TextureDestroy(c->sprite);
    free(c);
}
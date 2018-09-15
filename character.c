#include "character.h"

Character* Character_Create(char* spritePath, int id) {
    Character* newCharacter = malloc(sizeof(Character));
    newCharacter->sprite = WD_CreateTexture();
    WD_TextureLoadFromFile(newCharacter->sprite, spritePath);
    newCharacter->id = id;
    newCharacter->direction = 0;
    newCharacter->moving = false;
    newCharacter->x = 0;
    newCharacter->y = 0;
    newCharacter->animationIndex = 0;
    newCharacter->animationCount = 0;
    return newCharacter;
}

void Character_GetCollisionBox(Character* c, SDL_Rect* box, int offsetX, int offsetY) {
    box->w = c->sprite->w / 6;
    box->h = c->sprite->h / 8;
    box->x = c->x + offsetX + (c->sprite->w / 3 - box->w) / 2;
    box->y = c->y + offsetY + (c->sprite->h / 4 - box->h);
}

void Character_Update(Character* c, Map* m, Character** characters, int charNumber) {
    if(c->moving) {
        c->animationCount++;
        if(c->animationCount == 8) {
            c->animationCount = 0;
            c->animationIndex = (c->animationIndex + 1) % 3;
        }
        SDL_Rect collisionBox;
        int newX = c->x, newY = c->y;
        if(c->direction == 0) {
            Character_GetCollisionBox(c, &collisionBox, 0, 1);
            newY += 1;
        } else if(c->direction == 1) {
            Character_GetCollisionBox(c, &collisionBox, -1, 0);
            newX -= 1;
        } else if(c->direction == 2) {
            Character_GetCollisionBox(c, &collisionBox, 1, 0);
            newX += 1;
        } else if(c->direction == 3) {
            Character_GetCollisionBox(c, &collisionBox, 0, -1);
            newY -= 1;
        }
        if(Map_Passable(m, &collisionBox)) {
            bool noCollision = true;
            for(int i = 0; i < charNumber; i++) {
                if(characters[i]->id == c->id)
                    continue;
                SDL_Rect otherCollisionBox;
                Character_GetCollisionBox(characters[i], &otherCollisionBox, 0, 0);
                //printf("TESTANDO TESTANDO TESTANDO\n"); 
                if(collisionBox.x < otherCollisionBox.x + otherCollisionBox.w &&
                      collisionBox.x + collisionBox.w > otherCollisionBox.x &&
                      collisionBox.y < otherCollisionBox.y + otherCollisionBox.h &&
                      collisionBox.y + collisionBox.h > otherCollisionBox.y) {
                    noCollision = false;
                    break;
                }
            }
            if(noCollision) {
                c->x = newX;
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
    if(gInfo.debug) {
        SDL_Rect box;
        Character_GetCollisionBox(c, &box, -screenX, -screenY);
        SDL_SetRenderDrawColor(gInfo.renderer, 255, 0, 0, 150);
        SDL_RenderFillRect(gInfo.renderer, &box);
    }
}

void Character_Destroy(Character* c) {
    WD_TextureDestroy(c->sprite);
    free(c);
}
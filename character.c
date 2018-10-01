#include "character.h"

Character* Character_Create(char* spritePath, int id, bool noTexture) {
    Character* newCharacter = malloc(sizeof(Character));
    strcpy(newCharacter->spriteFile, spritePath);
    if(!noTexture) {
        newCharacter->sprite = WD_CreateTexture();
        WD_TextureLoadFromFile(newCharacter->sprite, spritePath);
    } else {
        newCharacter->sprite = NULL;
    }
    newCharacter->id = id;
    newCharacter->direction = 0;
    newCharacter->moving = false;
    newCharacter->x = 0;
    newCharacter->y = 0;
    newCharacter->renderX = 0;
    newCharacter->renderY = 0;
    newCharacter->x4 = 0;
    newCharacter->y4 = 0;
    newCharacter->moveSpeed = 2;
    newCharacter->animationIndex = 0;
    newCharacter->animationCount = 0;
    newCharacter->animPart = false;
    return newCharacter;
}

void Character_GetCollisionBox(Character* c, SDL_Rect* box, int offsetX, int offsetY) {
    box->w = c->sprite->w / 6;
    box->h = c->sprite->h / 8;
    box->x = c->x + offsetX + (c->sprite->w / 3 - box->w) / 2;
    box->y = c->y + offsetY + (c->sprite->h / 4 - box->h);
}

void Character_TryToMove(Character* c, int dir, Map* m, Character** characters, int charNumber) {
    c->direction = dir;
    SDL_Rect collisionBox;
    int newX = c->x, newY = c->y;
    int distance = TILE_SIZE / MOVEMENT_PARTS;
    if(c->direction == 0) {
        Character_GetCollisionBox(c, &collisionBox, 0, distance);
        newY += distance;
    } else if(c->direction == 1) {
        Character_GetCollisionBox(c, &collisionBox, -distance, 0);
        newX -= distance;
    } else if(c->direction == 2) {
        Character_GetCollisionBox(c, &collisionBox, distance, 0);
        newX += distance;
    } else if(c->direction == 3) {
        Character_GetCollisionBox(c, &collisionBox, 0, -distance);
        newY -= distance;
    }
    if(Map_Passable(m, &collisionBox)) {
        bool noCollision = true;
        for(int i = 0; i < charNumber; i++) {
            if(characters[i] == NULL || characters[i]->id == c->id)
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
            char sendData[32];
            sprintf(sendData, "POS %d %d %d", newX, newY, c->direction);
            //printf("%s\n", sendData);
            //printf("%d\n", gInfo.sockFd);
            Socket_Send(Network.sockFd, Network.serverAddress, sendData, strlen(sendData) + 1);
            c->x = newX;
            c->y = newY;
        }
    }
}

void Character_Update(Character* c, Map* m, Character** characters, int charNumber) {
    if(c->x != c->renderX || c->y != c->renderY) {
        c->moving = true;
        int x4 = c->x * 4;
        int y4 = c->y * 4;
        int distance = (1 << c->moveSpeed);
        if(c->renderX < c->x) {
            if(c->x4 + distance > x4) {
                c->x4 = x4;
            } else {
                c->x4 += distance;
            }
        } else {
            if(c->x4 - distance < x4) {
                c->x4 = x4;
            } else {
                c->x4 -= distance;
            }
        }
        if(c->renderY < c->y) {
            if(c->y4 + distance > y4) {
                c->y4 = y4;
            } else {
                c->y4 += distance;
            }
        } else {
            if(c->y4 - distance < y4) {
                c->y4 = y4;
            } else {
                c->y4 -= distance;
            }
        }
        c->renderX = c->x4 / 4;
        c->renderY = c->y4 / 4;
    } else {
        c->moving = false;
        c->animationIndex = 1;
        c->animationCount = 7;
        c->animPart = !c->animPart;
    }
    if(c->moving) {
        c->animationCount++;
        if(c->animationCount == 8) {
            c->animationCount = 0;
            if(c->animPart) {
                c->animationIndex -= 1;
                if(c->animationIndex < 0) {
                    c->animationIndex = 1;
                    c->animPart = false;
                }
            } else {
                c->animationIndex += 1;
                if(c->animationIndex == 3) {
                    c->animationIndex = 1;
                    c->animPart = true;
                }
            }
        }
    }
}

void Character_Render(Character* c, int screenX, int screenY) {
    int realX = round(c->renderX) - screenX;
    int realY = round(c->renderY) - screenY;
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
    if(c->sprite != NULL)
        WD_TextureDestroy(c->sprite);
    free(c);
}
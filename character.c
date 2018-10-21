#include "character.h"

Character* Character_Create(char* spritePath, int id, bool noTexture) {
    Character* newCharacter = malloc(sizeof(Character));
    strcpy(newCharacter->spriteFile, spritePath);
    newCharacter->sprite = WD_CreateTexture();
    if(!noTexture) {
        WD_TextureLoadFromFile(newCharacter->sprite, spritePath);
    } else {
        newCharacter->sprite->w = TILE_SIZE * 3;
        newCharacter->sprite->h = TILE_SIZE * 4;
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
    newCharacter->moveSpeed = 4;
    newCharacter->animationIndex = 0;
    newCharacter->animationCount = 0;
    newCharacter->animPart = false;
    newCharacter->lastMovementId = 0;
    newCharacter->dead = false;
    newCharacter->opacity = 255;
    newCharacter->bombPassId = -1;
    newCharacter->forcingMovement = false;
    newCharacter->moveRoute = NULL;
    return newCharacter;
}

bool CheckIntersection(SDL_Rect* r1, SDL_Rect* r2) {
    return (r1->x < r2->x + r2->w &&
            r1->x + r1->w > r2->x &&
            r1->y < r2->y + r2->h &&
            r1->y + r1->h > r2->y);
}

void Character_GetCollisionBox(Character* c, SDL_Rect* box, int offsetX, int offsetY) {
    box->w = c->sprite->w / 6;
    box->h = c->sprite->h / 8;
    box->x = c->x + offsetX + (c->sprite->w / 3 - box->w) / 2;
    box->y = c->y + offsetY + (c->sprite->h / 4 - box->h);
}

void Character_GetTilePosition(Character* c, int* x, int* y) {
    SDL_Rect box;
    Character_GetCollisionBox(c, &box, 0, 0);
    *x = (box.x + box.w / 2) / TILE_SIZE;
    *y = (box.y + box.h / 2) / TILE_SIZE;
}

bool Character_Passable(Character* c, Map* m, int x, int y) {
    SDL_Rect collisionBox;
    Character_GetCollisionBox(c, &collisionBox, x * TILE_SIZE - c->x, y * TILE_SIZE - c->y);
    if(Map_Passable(m, &collisionBox, c)) {
        bool noCollision = true;
        for(int i = 0; i < m->charNumber; i++) {
            if(m->characters[i] == NULL || m->characters[i]->dead || m->characters[i]->id == c->id)
                continue;
            SDL_Rect otherCollisionBox;
            Character_GetCollisionBox(m->characters[i], &otherCollisionBox, 0, 0); 
            if(CheckIntersection(&collisionBox, &otherCollisionBox)) {
                noCollision = false;
                break;
            }
        }
        return noCollision;
    } else {
        return false;
    }
}

bool Character_TryToMove(Character* c, int dir, Map* m) {
    if(c->dead)
        return;
    c->direction = dir;
    SDL_Rect collisionBox;
    int newX = c->x, newY = c->y;
    int distance = TILE_SIZE / MOVEMENT_PARTS;
    if(c->direction == DIR_DOWN) {
        Character_GetCollisionBox(c, &collisionBox, 0, distance);
        newY += distance;
    } else if(c->direction == DIR_LEFT) {
        Character_GetCollisionBox(c, &collisionBox, -distance, 0);
        newX -= distance;
    } else if(c->direction == DIR_RIGHT) {
        Character_GetCollisionBox(c, &collisionBox, distance, 0);
        newX += distance;
    } else if(c->direction == DIR_UP) {
        Character_GetCollisionBox(c, &collisionBox, 0, -distance);
        newY -= distance;
    }
    if(Map_Passable(m, &collisionBox, c)) {
        bool noCollision = true;
        for(int i = 0; i < m->charNumber; i++) {
            if(m->characters[i] == NULL || m->characters[i]->dead || m->characters[i]->id == c->id)
                continue;
            SDL_Rect otherCollisionBox;
            Character_GetCollisionBox(m->characters[i], &otherCollisionBox, 0, 0);
            //printf("TESTANDO TESTANDO TESTANDO\n"); 
            if(CheckIntersection(&collisionBox, &otherCollisionBox)) {
                noCollision = false;
                break;
            }
        }
        if(noCollision) {
            c->x = newX;
            c->y = newY;
            return true;
        }
    }
}

void Character_Place(Character* c, int x, int y) {
    c->x = x * TILE_SIZE + (TILE_SIZE - c->sprite->w / 3) / 2;
    c->y = y * TILE_SIZE + (TILE_SIZE - c->sprite->h / 4) / 2;
    c->renderX = c->x;
    c->renderY = c->y;
    c->x4 = c->x * 4;
    c->y4 = c->y * 4;
}

void Character_Update(Character* c, Map* m) {
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
    if(c->dead && c->opacity > 0) {
        uint8_t minus = 255 / Game.screenFreq * 3;
        if(minus > c->opacity) {
            c->opacity = 0;
        } else {
            c->opacity -= minus;
        }
    }
    SDL_SetTextureAlphaMod(c->sprite->mTexture, c->opacity);
    int realX = c->renderX - screenX;
    int realY = c->renderY - screenY;
    if(realX + c->sprite->w >= 0 && realY + c->sprite->h >= 0 && realX < REFERENCE_WIDTH && realY < REFERENCE_HEIGHT) {
        SDL_Rect clip = {c->sprite->w / 3 * c->animationIndex, c->sprite->h / 4 * c->direction, c->sprite->w / 3, c->sprite->h / 4};
        SDL_Rect dest = {realX, realY, c->sprite->w / 3, c->sprite->h / 4};
        SDL_RenderCopy(Game.renderer, c->sprite->mTexture, &clip, &dest);
    }
    if(Game.debug) {
        SDL_Rect box;
        Character_GetCollisionBox(c, &box, -screenX, -screenY);
        SDL_SetRenderDrawColor(Game.renderer, 255, 0, 0, 150);
        SDL_RenderFillRect(Game.renderer, &box);
    }
}

void Character_Destroy(Character* c) {
    if(c->sprite != NULL)
        WD_TextureDestroy(c->sprite);
    free(c);
}
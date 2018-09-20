#include "scene_map.h"
#include "stdio.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->lastTimeStamp = 0;
    newScene->keyLeft = false;
    newScene->keyRight = false;
    newScene->keyDown = false;
    newScene->keyUp = false;
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/001-Grassland01.png");
    newScene->map = Map_Create();
    Map_Load(newScene->map, "map.txt");
    Map_RenderFull(newScene->map, newScene->tileMap);
    newScene->screenX = 0;
    newScene->screenY = 0;
    newScene->charNumber = 4;
    newScene->characters = malloc(newScene->charNumber * sizeof(Character*));
    for(int i = 0; i < newScene->charNumber; i++) {
        if(i == 0) {
            newScene->characters[i] = Character_Create("content/dragon.png", i);
        } else {
            newScene->characters[i] = Character_Create("content/testcharacter.png", i);
            newScene->characters[i]->x += 10 + i * 30;
            newScene->characters[i]->moving = true;
        }
    }
    newScene->player = newScene->characters[0];
    return newScene;
}

void SceneMap_update(Scene_Map* s) {
    s->screenX = (s->player->x + s->player->sprite->w / 6) - (gInfo.screenWidth) / 2;
    s->screenY = (s->player->y + s->player->sprite->h / 8) - (gInfo.screenHeight) / 2;
    if(s->screenX > s->map->width * TILE_SIZE - gInfo.screenWidth) {
        s->screenX = s->map->width * TILE_SIZE - gInfo.screenWidth;
    }
    if(s->screenY > s->map->height * TILE_SIZE - gInfo.screenHeight) {
        s->screenY = s->map->height * TILE_SIZE - gInfo.screenHeight;
    }
    if(s->screenX < 0)
        s->screenX = 0;
    if(s->screenY < 0)
        s->screenY = 0;
    SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    SDL_Rect renderQuad = {s->screenX, s->screenY, gInfo.screenWidth, gInfo.screenHeight};
    int dstWidth, dstHeight;
    if(s->map->width * TILE_SIZE < gInfo.screenWidth)
        dstWidth = s->map->width * TILE_SIZE;
    else
        dstWidth = gInfo.screenWidth;
    if(s->map->height * TILE_SIZE < gInfo.screenHeight)
        dstHeight = s->map->width * TILE_SIZE;
    else
        dstHeight = gInfo.screenHeight;
    SDL_Rect dstRect = {0, 0, dstWidth, dstHeight};
    SDL_RenderCopy(gInfo.renderer, s->map->layers[0], &renderQuad, &dstRect);
    SDL_RenderCopy(gInfo.renderer, s->map->layers[1], &renderQuad, &dstRect);
    //Map_Render(s->map, s->tileMap, s->screenX, s->screenY);

    // MUDAR, TÁ MUITO RUIM (realmente)
    /*
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_UP]) {
        s->player->direction = 3;
        if(!s->player->moving) {
            s->player->moving = true;
            s->player->animationIndex = 0;
        }
    } else if(state[SDL_SCANCODE_DOWN]) {
        s->player->direction = 0;
        if(!s->player->moving) {
            s->player->moving = true;
            s->player->animationIndex = 0;
        }
    } else if(state[SDL_SCANCODE_LEFT]) {
        s->player->direction = 1;
        if(!s->player->moving) {
            s->player->moving = true;
            s->player->animationIndex = 0;
        }
    } else if(state[SDL_SCANCODE_RIGHT]) {
        s->player->direction = 2;
        if(!s->player->moving) {
            s->player->moving = true;
            s->player->animationIndex = 0;
        }
    } else {
        s->player->moving = false;
    }
    */




    // ------------------------------------ //

    //Character_Update(s->player, s->map, s->characters, s->charNumber);
    for(int i = 0; i < s->charNumber; i++) {
        Character_Update(s->characters[i], s->map, s->characters, s->charNumber);
    }
    //Character_Render(s->player, s->screenX, s->screenY);
    for(int i = 0; i < s->charNumber; i++) {
        Character_Render(s->characters[i], s->screenX, s->screenY);
    }
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if(e->type == SDL_KEYDOWN) {
        //Atualiza estado da tecla se o evento dela for mais recente
        if (e->key.timestamp > s->lastTimeStamp) {
            switch (e->key.keysym.scancode) {
            case SDL_SCANCODE_DOWN:
                s->player->direction = 0;
                s->keyDown = true;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
                break;
            case SDL_SCANCODE_UP:
                s->player->direction = 3;
                s->keyUp = true;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
                break;
            case SDL_SCANCODE_LEFT:
                s->player->direction = 1;
                s->keyLeft = true;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
                break;
            case SDL_SCANCODE_RIGHT:
                s->keyRight = true;
                s->player->direction = 2;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
                break;
            default:
                s->player->moving = false;
                break;
            }
            
        }





        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if(e->key.keysym.sym == SDLK_F3) {
            gInfo.debug = !gInfo.debug;
        }
    }
    else if (e->type == SDL_KEYUP) {
        //Atualiza estado da tecla
        bool acotezeo = false;
        switch (e->key.keysym.scancode) {
        case SDL_SCANCODE_DOWN:
            s->keyDown = false;
            acotezeo = true;
            break;
        case SDL_SCANCODE_UP:
            s->keyUp = false;
            acotezeo = true;
            break;
        case SDL_SCANCODE_LEFT:
            s->keyLeft = false;
            acotezeo = true;
            break;
        case SDL_SCANCODE_RIGHT:
            s->keyRight = false;
            acotezeo = true;
            break;
        }

        if (acotezeo) {
            if (s->keyDown) {
                s->player->direction = 0;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
            }
            else if (s->keyUp) {
                s->player->direction = 3;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
            }
            else if (s->keyLeft) {
                s->player->direction = 1;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
            }
            else if (s->keyRight) {
                s->player->direction = 2;
                if (!s->player->moving) {
                    s->player->moving = true;
                    s->player->animationIndex = 0;
                }
            }
           
        }
    }

        if (!s->keyDown && !s->keyLeft && !s->keyRight && !s->keyUp) {
            s->player->moving = false;
        }


}


void SceneMap_destroy(Scene_Map* s) {
    WD_TextureDestroy(s->tileMap);
    Map_Destroy(s->map);
    for(int i = 0; i < s->charNumber; i++) {
        Character_Destroy(s->characters[i]);
    }
    free(s);
}
#include "scene_map.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/tilemap.png");
    newScene->map = Map_Create();
    Map_Load(newScene->map, "map.txt");
    newScene->screenX = 0;
    newScene->screenY = 0;
    newScene->player = Character_Create("content/testcharacter.png");
    return newScene;
}

void SceneMap_update(Scene_Map* s) {
    if(s->screenX < 0)
        s->screenX = 0;
    if(s->screenY < 0)
        s->screenY = 0;
    if(s->screenX > s->map->width * TILE_SIZE - gInfo.screenWidth) {
        s->screenX = s->map->width * TILE_SIZE - gInfo.screenWidth;
    }
    if(s->screenY > s->map->height * TILE_SIZE - gInfo.screenHeight) {
        s->screenY = s->map->height * TILE_SIZE - gInfo.screenHeight;
    }
    SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    Map_Render(s->map, s->tileMap, s->screenX, s->screenY);
    Character_Update(s->player);
    Character_Render(s->player, s->screenX, s->screenY);
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if(e->key.keysym.sym == SDLK_RIGHT){
            // GAMBIARRA SÓ PRA TESTAR
            s->player->direction = 2;
            if(!s->player->moving) {
                s->player->moving = true;
                s->player->animationIndex = 0;
            }
        } else if(e->key.keysym.sym == SDLK_LEFT) {
            s->player->direction = 1;
            if(!s->player->moving) {
                s->player->moving = true;
                s->player->animationIndex = 0;
            }
        } else if(e->key.keysym.sym == SDLK_UP) {
            s->player->direction = 3;
            if(!s->player->moving) {
                s->player->moving = true;
                s->player->animationIndex = 0;
            }
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            s->player->direction = 0;
            if(!s->player->moving) {
                s->player->moving = true;
                s->player->animationIndex = 0;
            }
        }
    } else if(e->type == SDL_KEYUP) {
        // GAMBIARRA SÓ PRA TESTAR
        if(e->key.keysym.sym == SDLK_RIGHT){
            s->player->moving = false;
        } else if(e->key.keysym.sym == SDLK_LEFT) {
            s->player->moving = false;
        } else if(e->key.keysym.sym == SDLK_UP) {
            s->player->moving = false;
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            s->player->moving = false;
        }
    }
}

void SceneMap_destroy(Scene_Map* s) {
    WD_TextureDestroy(s->tileMap);
    Map_Destroy(s->map);
    Character_Destroy(s->player);
    free(s);
}
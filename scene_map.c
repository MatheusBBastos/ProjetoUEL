#include "scene_map.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/tilemap.png");
    newScene->map = Map_Create();
    Map_Load(newScene->map, "map.txt");
    newScene->screenX = 0;
    newScene->screenY = 0;
    return newScene;
}

void SceneMap_update(Scene_Map* s) {
    SDL_SetRenderDrawColor(gInfo.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gInfo.renderer);
    int offsetX = s->screenX % TILE_SIZE;
    int offsetY = s->screenY % TILE_SIZE;
    int startX = (int) (s->screenX / TILE_SIZE);
    int startY = (int) (s->screenY / TILE_SIZE);
    int endX = ((s->screenX + gInfo.screenWidth) / TILE_SIZE + 1);
    int endY = ((s->screenY + gInfo.screenHeight) / TILE_SIZE + 1);
    int x, y, z;
    for(z = 0; z < MAP_LAYERS; z++) {
        for(y = startY; y < endY; y ++) {
            for(x = startX; x < endX; x ++) {
                int tile = Map_Get(s->map, x, y, z);
                if(tile != -1) {
                    int realX = x - s->screenX / TILE_SIZE;
                    int realY = y - s->screenY / TILE_SIZE;
                    SDL_Rect c = {tile * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
                    WD_TextureRenderEx(s->tileMap, realX * TILE_SIZE - offsetX, realY * TILE_SIZE - offsetY, &c, 0.0, NULL, SDL_FLIP_NONE);
                }
            }
        }
    }
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        } else if(e->key.keysym.sym == SDLK_RIGHT){
            s->screenX += 3;
        } else if(e->key.keysym.sym == SDLK_LEFT) {
            s->screenX -= 3;
        } else if(e->key.keysym.sym == SDLK_UP) {
            s->screenY -= 3;
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            s->screenY += 3;
        }
    }  
}

void SceneMap_destroy(Scene_Map* s) {
    WD_TextureDestroy(s->tileMap);
    Map_Destroy(s->map);
    free(s);
}
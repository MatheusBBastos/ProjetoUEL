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
    s->screenX = (s->player->x + s->player->sprite->w / 6) - (gInfo.screenWidth) / 2;
    s->screenY = (s->player->y + s->player->sprite->h / 8) - (gInfo.screenHeight) / 2;
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

    // MUDAR, TÁ MUITO RUIM
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
    // ------------------------------------ //

    Character_Update(s->player, s->map);
    Character_Render(s->player, s->screenX, s->screenY);
}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_TAB) {
            SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
        }
    }
}

void SceneMap_destroy(Scene_Map* s) {
    WD_TextureDestroy(s->tileMap);
    Map_Destroy(s->map);
    Character_Destroy(s->player);
    free(s);
}
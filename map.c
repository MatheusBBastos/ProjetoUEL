#include "map.h"
#include "scene_base.h"

Map* Map_Create() {
    Map* newMap = malloc(sizeof(Map));
    newMap->loaded = false;
    newMap->layers = malloc(MAP_LAYERS * sizeof(SDL_Texture*));
    for(int i = 0; i < MAP_LAYERS; i++) {
        newMap->layers[i] = NULL;
    }
    newMap->characters = NULL;
    newMap->charNumber = 0;
    return newMap;
}

void Map_Load(Map* map, char* path, bool loadTexture) {
    if(map->loaded) {
        free(map->data);
        for(int i = 0; i < MAP_LAYERS; i++) {
            SDL_DestroyTexture(map->layers[i]);
            map->layers[i] = NULL;
        }
    }
    FILE* mapFile = fopen(path, "r");
    if(mapFile == NULL) {
        printf("Falha ao carregar mapa %s!\n", path);
        return;
    }
    fscanf(mapFile, "%d %d", &map->width, &map->height);
    if(loadTexture) {
        for(int i = 0; i < MAP_LAYERS; i++) {
            map->layers[i] = SDL_CreateTexture(Game.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, map->width * TILE_SIZE, map->height * TILE_SIZE);
        }
    }
    map->data = malloc(MAP_LAYERS * map->width * map->height * sizeof(int));
    int x, y, z;
    for(z = 0; z < MAP_LAYERS; z++) {
        for(y = 0; y < map->height; y++) {
            for(x = 0; x < map->width; x++) {
                fscanf(mapFile, "%d", &map->data[map->height * map->width * z + y * map->width + x]);
            }
        }
    }
    fclose(mapFile);
    map->loaded = true;
}

int Map_Get(Map* m, int x, int y, int z) {
    if(m->loaded && x >= 0 && y >= 0 && x < m->width && y < m->height && z < MAP_LAYERS) {
        return m->data[m->height * m->width * z + y * m->width + x];
    } else {
        return -1;
    }
}

void Map_Set(Map* m, int x, int y, int z, int value) {
    if(m->loaded) {
        m->data[MAP_LAYERS * m->height * z + y * m->height + x] = value;
    }
}

void Map_RenderFull(Map* m, WTexture* tileMap) {
    int x, y, z;
    for(z = 0; z < MAP_LAYERS; z++) {
        SDL_SetRenderTarget(Game.renderer, m->layers[z]);
        SDL_SetTextureBlendMode(m->layers[z], SDL_BLENDMODE_BLEND);
        for(y = 0; y < m->height; y ++) {
            for(x = 0; x < m->width; x ++) {
                int tile = Map_Get(m, x, y, z);
                if(tile != -1) {
                    SDL_Rect c = {(tile * TILE_SIZE) % TILESET_WIDTH, (int) tile / TILESET_WIDTH, TILE_SIZE, TILE_SIZE};
                    WD_TextureRenderEx(tileMap, x * TILE_SIZE, y * TILE_SIZE, &c, 0.0, NULL, SDL_FLIP_NONE);
                }
            }
        }
    }
    SDL_SetRenderTarget(Game.renderer, NULL);
}

void Map_Render(Map* m, WTexture* tileMap, int screenX, int screenY) {
    int offsetX = screenX % TILE_SIZE;
    int offsetY = screenY % TILE_SIZE;
    int startX = (int) (screenX / TILE_SIZE);
    int startY = (int) (screenY / TILE_SIZE);
    int endX = ((screenX + Game.screenWidth) / TILE_SIZE + 1);
    int endY = ((screenY + Game.screenHeight) / TILE_SIZE + 1);
    int x, y, z;
    for(z = 0; z < MAP_LAYERS; z++) {
        for(y = startY; y < endY; y ++) {
            for(x = startX; x < endX; x ++) {
                int tile = Map_Get(m, x, y, z);
                if(tile != -1) {
                    int realX = x - screenX / TILE_SIZE;
                    int realY = y - screenY / TILE_SIZE;
                    SDL_Rect c = {(tile * TILE_SIZE) % TILESET_WIDTH, (int) tile / TILESET_WIDTH, TILE_SIZE, TILE_SIZE};
                    WD_TextureRenderEx(tileMap, realX * TILE_SIZE - offsetX, realY * TILE_SIZE - offsetY, &c, 0.0, NULL, SDL_FLIP_NONE);
                }
            }
        }
    }
}

bool Map_Passable(Map* m, SDL_Rect* box) {
    if(box->x < 0 || box->y < 0)
        return false;
    int firstTileX = box->x;
    int firstTileY = box->y;
    int lastTileX = (firstTileX + box->w - 1) / TILE_SIZE;
    int lastTileY = (firstTileY + box->h - 1) / TILE_SIZE;
    firstTileX /= TILE_SIZE;
    firstTileY /= TILE_SIZE;
    for(int z = 0; z < MAP_LAYERS; z++) {
        for(int y = firstTileY; y <= lastTileY; y++) {
            for(int x = firstTileX; x <= lastTileX; x++) {
                if(Game.debug) {
                    int offX = SceneManager.sMap->screenX;
                    int offY = SceneManager.sMap->screenY;
                    SDL_Rect r = {x * TILE_SIZE - offX, y * TILE_SIZE - offY, TILE_SIZE, TILE_SIZE};
                    SDL_SetRenderDrawColor(Game.renderer, 0, 255, 0, 60);
                    SDL_RenderFillRect(Game.renderer, &r);
                }
                int tile = Map_Get(m, x, y, z);
                if(tile == 4 || tile == -1) {
                    return false;
                }
            }
        }
    }
    return true;
}

void Map_Destroy(Map* m) {
    if(m->loaded)
        free(m->data);
    for(int i = 0; i < MAP_LAYERS; i++) {
        SDL_DestroyTexture(m->layers[i]);
    }
    if(m->characters != NULL) {
        for(int i = 0; i < m->charNumber; i++) {
            if(m->characters[i] != NULL)
                Character_Destroy(m->characters[i]);
        }
        free(m->characters);
    }
    free(m->layers);
    free(m);
}
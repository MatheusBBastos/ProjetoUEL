#include "map.h"

Map* Map_Create() {
    Map* newMap = malloc(sizeof(Map));
    newMap->loaded = false;
    return newMap;
}

void Map_Load(Map* map, char* path) {
    if(map->loaded) {
        free(map->data);
    }
    FILE* mapFile = fopen(path, "r");
    fscanf(mapFile, "%d %d", &map->width, &map->height);
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
    if(m->loaded && x < m->width && y < m->height && z < MAP_LAYERS) {
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

void Map_Render(Map* m, WTexture* tileMap, int screenX, int screenY) {
    int offsetX = screenX % TILE_SIZE;
    int offsetY = screenY % TILE_SIZE;
    int startX = (int) (screenX / TILE_SIZE);
    int startY = (int) (screenY / TILE_SIZE);
    int endX = ((screenX + gInfo.screenWidth) / TILE_SIZE + 1);
    int endY = ((screenY + gInfo.screenHeight) / TILE_SIZE + 1);
    int x, y, z;
    for(z = 0; z < MAP_LAYERS; z++) {
        for(y = startY; y < endY; y ++) {
            for(x = startX; x < endX; x ++) {
                int tile = Map_Get(m, x, y, z);
                if(tile != -1) {
                    int realX = x - screenX / TILE_SIZE;
                    int realY = y - screenY / TILE_SIZE;
                    SDL_Rect c = {tile * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
                    WD_TextureRenderEx(tileMap, realX * TILE_SIZE - offsetX, realY * TILE_SIZE - offsetY, &c, 0.0, NULL, SDL_FLIP_NONE);
                }
            }
        }
    }
}

void Map_Destroy(Map* m) {
    if(m->loaded)
        free(m->data);
    free(m);
}
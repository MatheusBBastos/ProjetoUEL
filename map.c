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

void Map_Destroy(Map* m) {
    if(m->loaded)
        free(m->data);
    free(m);
}
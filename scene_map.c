#include "scene_map.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/tilemap.png");
    newScene->tileWidth = newScene->tileMap->w / 3;
    newScene->tileHeight = newScene->tileMap->h / 1;
    FILE* mapFile = fopen("map.txt", "r");
    /*fscanf(mapFile, "%d %d", &newScene->mapWidth, &newScene->mapHeight);
    newScene->map = (int**) malloc(newScene->mapHeight * sizeof(int));
    for(int y = 0; y < newScene->mapHeight; y++) {
        newScene->map[y] = (int*) malloc(newScene->mapWidth * sizeof(int));
        for(int x = 0; x < newScene->mapWidth; x++) {
            fscanf(mapFile, "%d", &newScene->map[y][x]);
        }
    }*/
    fclose(mapFile);
    return newScene;
}

void SceneMap_update(Scene_Map* s);

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e);

void SceneMap_destroy(Scene_Map* s) {
    WD_TextureDestroy(s->tileMap);
}
#include "scene_map.h"

Scene_Map* SceneMap_new() {
    Scene_Map* newScene = malloc(sizeof(Scene_Map));
    newScene->tileMap = WD_CreateTexture();
    WD_TextureLoadFromFile(newScene->tileMap, "content/tilemap.png");
    newScene->map = Map_Create();
    Map_Load(newScene->map, "map.txt");
    return newScene;
}

void SceneMap_update(Scene_Map* s) {

}

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e) {
    
}

void SceneMap_destroy(Scene_Map* s) {
    WD_TextureDestroy(s->tileMap);
    Map_Destroy(s->map);
}
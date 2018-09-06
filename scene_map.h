#ifndef SCENE_MAP_H
#define SCENE_MAP_H

typedef struct SceneManager SceneManager;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"
#include "map.h"

typedef struct Scene_Map {
    WTexture* tileMap;
    Map* map;
    int tileWidth, tileHeight, mapWidth, mapHeight;
} Scene_Map;

Scene_Map* SceneMap_new();

void SceneMap_update(Scene_Map* s);

void SceneMap_handleEvent(Scene_Map* s, SDL_Event* e);

void SceneMap_destroy(Scene_Map* s);

#endif
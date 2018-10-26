#include "map.h"
#include "scene_base.h"
#include "noise.h"

Map* Map_Create() {
    Map* newMap = malloc(sizeof(Map));
    newMap->loaded = false;
    newMap->layers = malloc(MAP_LAYERS * sizeof(SDL_Texture*));
    for(int i = 0; i < MAP_LAYERS; i++) {
        newMap->layers[i] = NULL;
    }
    newMap->characters = malloc(MAX_PLAYERS * sizeof(Character*));
    for(int i = 0; i < MAX_PLAYERS; i++) {
        newMap->characters[i] = NULL;
    }
    newMap->charNumber = MAX_PLAYERS;
    newMap->objects = NULL;
    newMap->walls = NULL;
    newMap->powerups = NULL;
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

void Map_GenerateWalls(Map* m, int seed) {
    m->objects = malloc(m->height * sizeof(TemporaryObject*));
    int wallNumber = 0;
    for(int y = 0; y < m->height; y++) {
        m->objects[y] = malloc(m->width * sizeof(TemporaryObject));
        for(int x = 0; x < m->width; x++) {
            bool possibleWall = true;
            for(int i = 0; i < m->charNumber; i++) {
                if(m->characters[i] == NULL) {
                    continue;
                }
                if(abs(m->characters[i]->x / TILE_SIZE - x) + abs(m->characters[i]->y / TILE_SIZE - y) < 2) {
                    possibleWall = false;
                    break;
                }
            }
            if(possibleWall && Map_Get(m, x, y, 1) != WALL_TILE) {
                float n = perlin2d(x, y, PERLIN_FREQUENCY, PERLIN_DEPTH, seed);
                if(n <= PERLIN_DELIMITER) {
                    m->objects[y][x].exists = true;
                    m->objects[y][x].type = OBJ_WALL;
                    m->objects[y][x].objId = wallNumber;
                    m->walls[wallNumber].exists = true;
                    m->walls[wallNumber].x = x;
                    m->walls[wallNumber].y = y;
                    wallNumber++;
                } else {
                    m->objects[y][x].exists = false;
                }
            } else {
                m->objects[y][x].exists = false;
            }
        }
    }
    m->powerupNumber = 20;
    m->powerups = malloc(m->powerupNumber * sizeof(PowerUp));
    for(int i = 0; i < m->powerupNumber; i++) {
        m->powerups[i].exists = false;
    }
}

void Map_DestroyCharacters(Map* m) {
    if(m->characters != NULL) {
        for(int i = 0; i < m->charNumber; i++) {
            if(m->characters[i] != NULL) {
                Character_Destroy(m->characters[i]);
            }
        }
        free(m->characters);
        m->characters = NULL;
    }
}

void Map_RenderWalls(Map* m, WTexture* wallTexture, int screenX, int screenY) {
    for(int i = 0; i < m->wallNumber; i++) {
        if(m->walls[i].exists) {
            SDL_Rect src = {0, 0, wallTexture->w, wallTexture->h};
            SDL_Rect dst = {TILE_SIZE * m->walls[i].x - screenX, TILE_SIZE * m->walls[i].y - screenY, wallTexture->w, wallTexture->h};
            SDL_RenderCopy(Game.renderer, wallTexture->mTexture, &src, &dst);
        }
    }
}

void Map_RenderPowerUps(Map* m, WTexture* puTexture, int screenX, int screenY, int currentFrame) {
    int offY = currentFrame >= Game.screenFreq / 2 ? -2 : 0;
    for(int i = 0; i < m->powerupNumber; i++) {
        if(m->powerups[i].exists) {
            SDL_Rect clip = {TILE_SIZE * m->powerups[i].type, 0, TILE_SIZE, TILE_SIZE};
            WD_TextureRenderEx(puTexture, TILE_SIZE * m->powerups[i].x - screenX, TILE_SIZE * m->powerups[i].y - screenY + offY, &clip, 0.0, NULL, SDL_FLIP_NONE);
        }
    }
}

void Map_RenderFull(Map* m, WTexture* tileMap) {
    int x, y, z;
    for(z = 0; z < MAP_LAYERS; z++) {
        SDL_SetRenderTarget(Game.renderer, m->layers[z]);
        SDL_SetRenderDrawColor(Game.renderer, 255, 255, 255, 0);
        SDL_RenderClear(Game.renderer);
        SDL_SetTextureBlendMode(m->layers[z], SDL_BLENDMODE_BLEND);
        for(y = 0; y < m->height; y ++) {
            for(x = 0; x < m->width; x ++) {
                int tile = Map_Get(m, x, y, z);
                if(tile != -1) {
                    SDL_Rect c = {(tile * TILE_SIZE) % TILESET_WIDTH, 0, TILE_SIZE, TILE_SIZE};
                    SDL_Rect d = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                    SDL_RenderCopy(Game.renderer, tileMap->mTexture, &c, &d);
                }
            }
        }
    }
    SDL_SetRenderTarget(Game.renderer, Game.screenTexture);
}

void Map_Render(Map* m, WTexture* tileMap, int screenX, int screenY) {
    int offsetX = screenX % TILE_SIZE;
    int offsetY = screenY % TILE_SIZE;
    int startX = (int) (screenX / TILE_SIZE);
    int startY = (int) (screenY / TILE_SIZE);
    int endX = ((screenX + REFERENCE_WIDTH) / TILE_SIZE + 1);
    int endY = ((screenY + REFERENCE_HEIGHT) / TILE_SIZE + 1);
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

bool Map_Passable(Map* m, SDL_Rect* box, Character* c) {
    if(box->x < 0 || box->y < 0)
        return false;
    int firstTileX = box->x;
    int firstTileY = box->y;
    int lastTileX = (firstTileX + box->w - 1) / TILE_SIZE;
    int lastTileY = (firstTileY + box->h - 1) / TILE_SIZE;
    if(lastTileX >= m->width || lastTileY >= m->height)
        return false;
    firstTileX /= TILE_SIZE;
    firstTileY /= TILE_SIZE;
    bool usedPass = false;
    for(int z = 0; z < MAP_LAYERS; z++) {
        for(int y = firstTileY; y <= lastTileY; y++) {
            for(int x = firstTileX; x <= lastTileX; x++) {
                if(m->objects != NULL && m->objects[y][x].exists) {
                    ObjType type = m->objects[y][x].type;
                    if(type == OBJ_WALL || c == NULL || type == OBJ_BOMB && c->bombPassId != m->objects[y][x].objId) {
                        return false;
                    } else {
                        usedPass = true;
                    }
                }
                int tile = Map_Get(m, x, y, z);
                if(tile == WALL_TILE || tile == -1) {
                    return false;
                }
            }
        }
    }
    if(!usedPass && c != NULL && c->bombPassId != -1) {
        //c->bombPassId = -1;
    }
    return true;
}

bool Map_CheckSafeSpot(Map* m, int x, int y, int checkRange) {
    if(m->objects[y][x].exists && m->objects[y][x].type == OBJ_BOMB) {
        return false;
    }
    for(int x1 = 1; x1 <= checkRange; x1++) {
        if(x + x1 >= m->width) {
            break;
        }
        if(m->objects[y][x + x1].exists) {
            int t = m->objects[y][x + x1].type;
            if(t == OBJ_BOMB) {
                return false;
            } else if(t == OBJ_WALL) {
                break;
            }
        }
    }

    for(int x1 = 1; x1 <= checkRange; x1++) {
        if(x - x1 < 0) {
            break;
        }
        if(m->objects[y][x - x1].exists) {
            int t = m->objects[y][x - x1].type;
            if(t == OBJ_BOMB) {
                return false;
            } else if(t == OBJ_WALL) {
                break;
            }
        }
    }

    for(int y1 = 1; y1 <= checkRange; y1++) {
        if(y + y1 >= m->height) {
            break;
        }
        if(m->objects[y + y1][x].exists) {
            int t = m->objects[y + y1][x].type;
            if(t == OBJ_BOMB) {
                return false;
            } else if(t == OBJ_WALL) {
                break;
            }
        }
    }

    for(int y1 = 1; y1 <= 5; y1++) {
        if(y - y1 < 0) {
            break;
        }
        if(m->objects[y - y1][x].exists) {
            int t = m->objects[y - y1][x].type;
            if(t == OBJ_BOMB) {
                return false;
            } else if(t == OBJ_WALL) {
                break;
            }
        }
    }

    return true;
}

void Map_Destroy(Map* m) {
    if(m->loaded)
        free(m->data);
    for(int i = 0; i < MAP_LAYERS; i++) {
        if(m->layers[i] != NULL)
            SDL_DestroyTexture(m->layers[i]);
    }
    if(m->characters != NULL) {
        for(int i = 0; i < m->charNumber; i++) {
            if(m->characters[i] != NULL)
                Character_Destroy(m->characters[i]);
        }
        free(m->characters);
    }
    if(m->objects != NULL) {
        for(int i = 0; i < m->height; i++) {
            free(m->objects[i]);
        }
        free(m->objects);
    }
    if(m->walls != NULL) {
        free(m->walls);
    }
    if(m->powerups != NULL) {
        free(m->powerups);
    }
    free(m->layers);
    free(m);
}
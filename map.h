#include "headers.h"

typedef struct Map {
    int* data;
    bool loaded;
    int width, height;
} Map;

Map* Map_Create();

void Map_Load(Map* map, char* path);

int Map_Get(Map* m, int x, int y, int z);

void Map_Set(Map* m, int x, int y, int z, int value);

void Map_Destroy(Map* m);
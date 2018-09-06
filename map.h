typedef struct Map {
    int* data;
    int width, height;
} Map;

Map* Map_Create();

void Map_Load(Map* map, char* path);

int Map_Get(Map* m, int x, int y);

void Map_Set(Map* m, int x, int y, int value);

void Map_Destroy(Map* m);
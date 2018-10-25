#ifndef PATHFIND_H
#define PATHFIND_H

#include "character.h"

typedef struct Node {
    int x, y, g, h;
    bool closed;
    struct Node* parent;
} Node;

typedef struct NodeGrid {
    int w, h;
    Node* data;
} NodeGrid;

typedef struct NodeHeap {
    Node** data;
    int size;
} NodeHeap;

typedef struct PFInstance {
    bool safeSpot;
    int minRange;
    NodeHeap heap;
    NodeGrid* grid;
    Node* targetNode;
    Character* character;
    Map* map;
} PFInstance;

bool PF_Find(Map* map, Character* c, int tx, int ty, int minRange, bool findSafeSpot);

#endif
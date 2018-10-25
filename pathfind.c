#include "pathfind.h"

void Heap_Insert(NodeHeap* heap, Node* node) {
    heap->data[heap->size] = node;
    int i = heap->size;
    Node* temp;
    while(i != 0 && heap->data[i]->h + heap->data[i]->g < heap->data[(i-1)/2]->h + heap->data[(i-1)/2]->g) {
        temp = heap->data[i/2];
        heap->data[(i-1)/2] = heap->data[i];
        heap->data[i] = temp;
        i = (i-1)/2;
    }
    heap->size++;
}

int Heap_MinChild(NodeHeap* heap, int i) {
    if(i * 2 + 2 > heap->size) {
        return i * 2 + 1;
    } else {
        if(heap->data[i * 2 + 1]->g + heap->data[i * 2 + 1]->h < heap->data[i * 2 + 2]->g + heap->data[i * 2 + 2]->h) {
            return i * 2 + 1;
        } else {
            return i * 2 + 2;
        }
    }
}

Node* Heap_Pop(NodeHeap* heap) {
    if(heap->size == 0) {
        return NULL;
    }
    Node *node = heap->data[0], *temp;
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;
    int i = 0, mc;
    while(i * 2 + 1 < heap->size) {
        mc = Heap_MinChild(heap, i);
        if(heap->data[i]->g + heap->data[i]->h > heap->data[mc]->g + heap->data[mc]->h) {
            temp = heap->data[i];
            heap->data[i] = heap->data[mc];
            heap->data[mc] = temp;
        }
        i = mc;
    }
    return node;
}

NodeGrid* PF_CreateNodeGrid(int w, int h) {
    NodeGrid* nodeGrid = malloc(sizeof(NodeGrid));
    nodeGrid->data = malloc(w * h * sizeof(Node));
    memset(nodeGrid->data, 0, w * h * sizeof(Node));
    nodeGrid->w = w;
    nodeGrid->h = h;
    return nodeGrid;
}

void PF_DestroyNodeGrid(NodeGrid* ng) {
    free(ng->data);
    free(ng);
}

Node* PF_NodeGet(NodeGrid* ng, int x, int y) {
    int index = y * ng->w + x;
    if(ng->data[index].g == 0) {
        ng->data[index].x = x;
        ng->data[index].y = y;
    }
    return &ng->data[index];
}

int PF_ManhattanDistance(PFInstance* instance, Node* node) {
    return abs(instance->targetNode->x - node->x) + abs(instance->targetNode->y - node->y);
}

void PF_NodeSetNeighbors(PFInstance* instance, Node* node) {
    int newx, newy;
    for(int y1 = -1; y1 <= 1; y1++) {
        for(int x1 = -1; x1 <= 1; x1++) {
            newx = node->x + x1;
            newy = node->y + y1;
            if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0 || !Character_Passable(instance->character, instance->map, newx, newy)) {
                continue;
            }
            Node* newNode = PF_NodeGet(instance->grid, newx, newy);
            if(newNode->g != 0 || newNode->closed) {
                if(newNode->g > node->g + 1) {
                    newNode->parent = node;
                    newNode->g = node->g + 1;
                }
            } else {
                newNode->parent = node;
                newNode->g = node->g + 1;
                newNode->h = instance->safeSpot ? 0 : PF_ManhattanDistance(instance, newNode);
                Heap_Insert(&instance->heap, newNode);
            }
        }
    }
}

bool PF_ConditionSatisfied(PFInstance* instance, Node* node) {
    if(instance->safeSpot) {
        return Map_CheckSafeSpot(instance->map, node->x, node->y, instance->minRange);
    } else {
        return (PF_ManhattanDistance(instance, node) <= instance->minRange);
    }
}

bool PF_Find(Map* map, Character* c, int tx, int ty, int minRange, bool findSafeSpot) {
    PFInstance instance;
    instance.safeSpot = findSafeSpot;
    instance.minRange = minRange;
    instance.map = map;
    instance.character = c;
    instance.grid = PF_CreateNodeGrid(map->width, map->height);
    int sx, sy;
    Character_GetTilePosition(c, &sx, &sy);
    int hScore = findSafeSpot ? 0 : abs(tx - sx) + abs(ty - sy);
    Node* currentNode = PF_NodeGet(instance.grid, sx, sy);
    currentNode->h = hScore;
    instance.targetNode = PF_NodeGet(instance.grid, tx, ty);

    instance.heap.size = 0;
    instance.heap.data = malloc(map->width * map->height * sizeof(Node*));

    currentNode->closed = true;
    while(!PF_ConditionSatisfied(&instance, currentNode)) {
        PF_NodeSetNeighbors(&instance, currentNode);
        currentNode = Heap_Pop(&instance.heap);
        if(currentNode == NULL) {
            return false;
        }
        currentNode->closed = true;
    }

    int foundX = currentNode->x, foundY = currentNode->y;

    while(currentNode != NULL && currentNode->parent != NULL) {
        uint8_t dir;
        if(currentNode->y - currentNode->parent->y > 0) {
            dir = DIR_DOWN;
        } else if(currentNode->x - currentNode->parent->x < 0) {
            dir = DIR_LEFT;
        } else if(currentNode->x - currentNode->parent->x > 0) {
            dir = DIR_RIGHT;
        } else {
            dir = DIR_UP;
        }
        for(int i = 0; i < MOVEMENT_PARTS; i++) {
            instance.character->movementStackTop++;
            instance.character->movementStack[instance.character->movementStackTop] = dir;
        }
        currentNode = currentNode->parent;
    }

    if(instance.character->movementStackTop > -1) {
        instance.character->forcingMovement = true;
        instance.character->targetX = foundX;
        instance.character->targetY = foundY;
    }

    free(instance.heap.data);
    PF_DestroyNodeGrid(instance.grid);

    return true;
}
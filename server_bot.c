#include "server.h"
#include "pathfind.h"

void Server_UpdateCharMovement(Server* s, Character* c) {
    if(c->forcingMovement && c->x == c->renderX && c->y == c->renderY) {
        Character_TryToMove(c, c->moveRoute->dir, s->map);
        Movement* next = c->moveRoute->next;
        free(c->moveRoute);
        if(next == NULL) {
            c->forcingMovement = false;
        }
        c->moveRoute = next;
        char sendData[32];
        sprintf(sendData, "POS %d %d %d %d", c->id, c->x, c->y, c->direction);
        Server_SendToAll(s, sendData, -1);
    }
}

void Server_UpdateBot(Server* s, int id) {
    // Character: s->map->characters[id]
    // Client: s->clients[id]
    // PF_Find(s->map, s->map->characters[id], x, y)
    // -> retorna true se achou um caminho, false é impossível

    // ISSO AQUI TA UMA BOSTA, POR FAVOR NAO USA NADA
    Character* c = s->map->characters[id];
    if(!c->dead && !c->forcingMovement) {
        int x, y, bombX, bombY;
        bool actionMade = false;
        Character_GetTilePosition(c, &x, &y);
        // Verificar se tem parede em volta
        for(int x1 = -1; x1 <= 1; x1++) {
            for(int y1 = -1; y1 <= 1; y1++) {
                if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                    continue;
                }
                TemporaryObject o = s->map->objects[y + y1][x + x1];
                if(o.exists && o.type == OBJ_WALL) {
                    // se tiver parede em volta colocar uma bomba
                    Server_PlaceBomb(s, id);
                    actionMade = true;
                    bombX = x + x1;
                    bombY = y + y1;
                    break;
                }
            }
            if(actionMade) {
                break;
            }
        }
        // colocou uma bomba, entao correr
        if(actionMade) {
            actionMade = false;
            // achar um lugar perto pra correr
            for(int x1 = -3; x1 <= 3; x1++) {
                for(int y1 = -3; y1 <= 3; y1++) {
                    if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
                        continue;
                    }
                    if(x + x1 == bombX || y + y1 == bombY) {
                        continue;
                    }
                    
                    if(PF_Find(s->map, c, x + x1, y + y1)) {
                        actionMade = true;
                        break;
                    }
                }
                if(actionMade) {
                    break;
                }
            }
        } else {
            // não colocou a bomba, então andar pra um lugar aí
            for(int x1 = -3; x1 <= 3; x1++) {
                for(int y1 = -3; y1 <= 3; y1++) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
                        continue;
                    }
                    if(PF_Find(s->map, c, x + x1, y + y1)) {
                        actionMade = true;
                        break;
                    }
                }
                if(actionMade) {
                    break;
                }
            }
        }

    }
}
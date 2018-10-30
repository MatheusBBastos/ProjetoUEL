#include "server.h"
#include "pathfind.h"

void Server_UpdateCharMovement(Server* s, Character* c) {
    if(c->dead)
        return;
    if(c->forcingMovement && c->x == c->renderX && c->y == c->renderY) {
        if(Character_TryToMove(c, c->movementStack[c->movementStackTop], s->map)) {
            c->movementStackTop--;
            if(c->movementStackTop == -1) {
                c->forcingMovement = false;
            }
        } else {
            c->forcingMovement = false;
            c->movementStackTop = -1;
        }
        char sendData[32];
        sprintf(sendData, "POS %d %d %d %d", c->id, c->x, c->y, c->direction);
        Server_SendToAll(s, sendData, -1);
    } else {
        int x, y;
        Character_GetTilePosition(c, &x, &y);
        TemporaryObject *o = &s->map->objects[y][x];
        if(o->exists && o->type == OBJ_POWERUP) {
            char sendData[16];
            switch(s->powerups[o->objId].type) {
                case PU_BLAST_RADIUS:
                    s->clients[c->id]->bombRadius++;
                    break;
                case PU_PLUS_BOMB:
                    s->clients[c->id]->maxBombs++;
                    if(s->clients[c->id]->maxBombs > MAX_BOMBS_PER_PLAYER) {
                        s->clients[c->id]->maxBombs = MAX_BOMBS_PER_PLAYER;
                    }
                    break;
                case PU_SHIELD:
                    c->shieldDuration = 2 * SERVER_TICKRATE;
                    sprintf(sendData, "SHI %d", c->id);
                    Server_SendToAll(s, sendData, -1);
                    break;
            }
            s->powerups[o->objId].exists = false;
            o->exists = false;
            sprintf(sendData, "PWD %d", o->objId);
            Server_SendToAll(s, sendData, -1);
        }
    }
}

int distance(int xBot, int yBot, int xOthers, int yOthers) {
    return (abs(xBot - xOthers) + abs(yBot - yOthers));
}

bool Bot_CheckKill(Map* map, int radius, int bx, int by, int tx, int ty) {
    if(bx != tx && by != ty)
        return false;
    if(bx == tx && by == ty)
        return true;
    if(bx == tx) {
        if(by < ty) {
            for(int i = 1; i <= radius; i++) {
                if(by + i == ty) {
                    return true;
                } else if(map->objects[by + i][bx].exists && map->objects[by + i][bx].type == OBJ_WALL) {
                    return false;
                } else if(Map_Get(map, bx, by + i, 1) == WALL_TILE) {
                    return false;
                }
            }
            return false;
        } else if(by > ty) {
            for(int i = 1; i <= radius; i++) {
                if(by - i == ty) {
                    return true;
                } else if(map->objects[by - i][bx].exists && map->objects[by - i][bx].type == OBJ_WALL) {
                    return false;
                } else if(Map_Get(map, bx, by - i, 1) == WALL_TILE) {
                    return false;
                }
            }
            return false;
        } else {
            return true;
        }
    } else {
        if(bx < tx) {
            for(int i = 1; i <= radius; i++) {
                if(bx + i == tx) {
                    return true;
                } else if(map->objects[by][bx + i].exists && map->objects[by][bx + i].type == OBJ_WALL) {
                    return false;
                } else if(Map_Get(map, bx + i, by, 1) == WALL_TILE) {
                    return false;
                }
            }
            return false;
        } else {
            for(int i = 1; i <= radius; i++) {
                if(bx - i == tx) {
                    return true;
                } else if(map->objects[by][bx - i].exists && map->objects[by][bx - i].type == OBJ_WALL) {
                    return false;
                } else if(Map_Get(map, bx - i, by, 1) == WALL_TILE) {
                    return false;
                }
            }
            return false;
        }
    }
}

bool checarEIr(Server* s, int id, int x, int y, int x1, int y1) {
    if(x1 == 0 && y1 == 0 || !Character_Passable(s->map->characters[id], s->map, x + x1, y + y1)) {
        return false;
    }
    if(s->clients[id]->b.difficulty == DIFFICULTY_HARD && !Map_CheckSafeSpot(s->map, x+x1, y+y1, 5)) {
        return false;
    }
    return (PF_Find(s->map, s->map->characters[id], x + x1, y + y1, 0, false));
}

void Server_UpdateBot(Server* s, int id) {
    // Character: s->map->characters[id]
    // Client: s->clients[id]
    // PF_Find(s->map, s->map->characters[id], x, y)
    // -> retorna true se achou um caminho, false é impossível
    // s->clients[id]->b.difficultyy

    Character* c = s->map->characters[id];
    if(c->dead)
        return;
    int x, y, bombX, bombY;
    Character_GetTilePosition(c, &x, &y);
    if(s->clients[id]->bombsPlaced == 0) {
        bool actionMade = false;

        // Pegando o boneco mais proximo
        int menorDist = 40;
        int distX, distY;
        int posX_prox = -1, posY_prox = -1;
        for(int i=0; i < s->maxClients; i++) {
            if(s->clients[i] != NULL && i != id && !s->map->characters[i]->dead) {
                int x2 = s->map->characters[i]->x;
                int y2 = s->map->characters[i]->y;
                Character_GetTilePosition(s->map->characters[i], &x2, &y2);
                if(distance(x, y, x2, y2) < menorDist && (s->clients[id]->b.difficulty != DIFFICULTY_EASY || distance(x, y, x2, y2) > 0)) {
                    menorDist = distance(x, y, x2, y2);
                    distX = x - x2;
                    distY = y - y2;
                    posX_prox = x2;
                    posY_prox = y2;
                }
            }
        }

        if(posX_prox == -1 && posY_prox == -1)
            return;

        if(menorDist == 0 && c->forcingMovement && s->clients[id]->b.difficulty > DIFFICULTY_EASY) {
            Server_PlaceBomb(s, id);
        }
        
        if(c->forcingMovement)
            return;

        if(menorDist < (s->clients[id]->bombRadius + 2) && s->clients[id]->b.difficulty > DIFFICULTY_EASY) {
            if(s->clients[id]->b.difficulty == DIFFICULTY_MEDIUM || Bot_CheckKill(s->map, s->clients[id]->bombRadius, x, y, posX_prox, posY_prox)) {
                Server_PlaceBomb(s, id);
                bombX = x;
                bombY = y;
                actionMade = true;
            }
        } else {
            for(int x1 = -1; x1 <= 1; x1++) {
                for(int y1 = -1; y1 <= 1; y1++) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }

                    // Se tiver parede em volta colocar uma bomba
                    TemporaryObject o = s->map->objects[y + y1][x + x1];
                    if(o.exists && o.type == OBJ_WALL) {
                        Server_PlaceBomb(s, id);
                        actionMade = true;
                        bombX = x;
                        bombY = y;
                        break;
                    }
                }
                if(actionMade)
                    break;
            }

        }  
        // Se fez alguma ação,
        // Achar um lugar perto para correr
        if(actionMade) {
            actionMade = false;
            // Se for bot dificil, verificar se tem power up em alguma posição primeiro
            if(s->clients[id]->b.difficulty == DIFFICULTY_HARD) {
                for(int x1 = -3; x1 <= 3; x1++) {
                    for(int y1 = -3; y1 <= 3; y1++) {
                        if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
                            continue;
                        }
                        if(x + x1 == bombX || y + y1 == bombY) {
                            continue;
                        }
                        TemporaryObject o = s->map->objects[y + y1][x + x1];
                        if(o.exists && o.type == OBJ_POWERUP && Map_CheckSafeSpot(s->map, x+x1, y+y1, 5)) {
                            PF_Find(s->map, c, x+x1, y+y1, 0, false);
                            actionMade = true;
                            break;
                        }
                        
                    if(actionMade)
                        break;
                    }
                }
            }        

            // Se nao for bot dificil
            if(!actionMade) {
                PF_Find(s->map, c, 0, 0, 1 + s->clients[id]->b.difficulty * 2, true);
            }
        } else {
            // Se possivel, ir na direção do player
            if(PF_Find(s->map, c, posX_prox, posY_prox, 1, false)) {
                actionMade = true;
            } else {
                // Caso contrario, achar alguma posição que seja possivel andar
                if(distY > 0 && distX > 0 || distY > 0 && distX == 0) {
                    for(int x1 = -1; x1 <= 1; x1++) {
                        for(int y1 = -1; y1 <= 1; y1++) {
                            if(checarEIr(s, id, x, y, x1, y1)) {
                                actionMade = true;
                                break;
                            }
                        }
                        if(actionMade) {
                            break;
                        }
                    }
                } else if(distY > 0 && distX < 0 || distY == 0 && distX > 0) {
                    for(int x1 = 1; x1 >= -1; x1--) {
                        for(int y1 = -1; y1 <= 1; y1++) {
                            if(checarEIr(s, id, x, y, x1, y1)) {
                                actionMade = true;
                                break;
                            }
                        }
                        if(actionMade) {
                            break;
                        }
                    }

                } else if(distY < 0 && distX < 0 || distY == 0 && distX < 0) {
                    
                    for(int x1 = 1; x1 >= -1; x1--) {
                        for(int y1 = 1; y1 >= -1; y1--) {
                            if(checarEIr(s, id, x, y, x1, y1)) {
                                actionMade = true;
                                break;
                            }
                        }
                        if(actionMade) {
                            break;
                        }
                    }

                } else if(distY < 0 && distX > 0 || distY < 0 && distX == 0) {
                    
                    for(int x1 = -1; x1 <= 1; x1++) {
                        for(int y1 = 1; y1 >= -1; y1--) {
                            if(checarEIr(s, id, x, y, x1, y1)) {
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
    } else if(!c->forcingMovement && s->clients[id]->b.difficulty > DIFFICULTY_EASY) {
        if(!Map_CheckSafeSpot(s->map, x, y, 1 + s->clients[id]->b.difficulty * 2)) {
            c->movementStackTop = -1;
            PF_Find(s->map, c, 0, 0, 1 + s->clients[id]->b.difficulty * 2, true);
        }
    }
}
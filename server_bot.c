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
            switch(s->powerups[o->objId].type) {
                case PU_BLAST_RADIUS:
                    printf("b\n");
                    s->clients[c->id]->bombRadius++;
                    break;
                case PU_PLUS_BOMB:
                    printf("a\n");
                    s->clients[c->id]->maxBombs++;
                    if(s->clients[c->id]->maxBombs > MAX_BOMBS_PER_PLAYER) {
                        s->clients[c->id]->maxBombs = MAX_BOMBS_PER_PLAYER;
                    }
                    break;
            }
            s->powerups[o->objId].exists = false;
            o->exists = false;
            char sendData[16];
            sprintf(sendData, "PWD %d", o->objId);
            Server_SendToAll(s, sendData, -1);
        }
    }
}

int distance(int xBot, int yBot, int xOthers, int yOthers) {
    return (abs(xBot - xOthers) + abs(yBot - yOthers));
}

void Server_UpdateBot(Server* s, int id) {
    // Character: s->map->characters[id]
    // Client: s->clients[id]
    // PF_Find(s->map, s->map->characters[id], x, y)
    // -> retorna true se achou um caminho, false é impossível
    // s->clients[id]->b.difficultyy

    Character* c = s->map->characters[id];
    if(!c->dead && !c->forcingMovement && s->clients[id]->bombsPlaced == 0) {
        int x, y, bombX, bombY;
        bool actionMade = false;
        Character_GetTilePosition(c, &x, &y);

        // Pegando o boneco mais proximo
        int menorDist = 40;
        int distX, distY;
        int posX_prox = -1, posY_prox = -1;
        for(int i=0; i < s->maxClients; i++) {
            if(s->clients[i] != NULL && i != id && !s->map->characters[i]->dead) {
                int x2 = s->map->characters[i]->x;
                int y2 = s->map->characters[i]->y;
                Character_GetTilePosition(s->map->characters[i], &x2, &y2);
                if(distance(x, y, x2, y2) < menorDist && distance(x, y, x2, y2) > 0) {
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

        if(menorDist < (s->clients[id]->bombRadius + 2) && s->clients[id]->b.difficulty > DIFFICULTY_EASY) {
            if(s->clients[id]->b.difficulty == DIFFICULTY_MEDIUM || (x == posX_prox || y == posY_prox)) {
                Server_PlaceBomb(s, id);
                bombX = x;
                bombY = y;
                actionMade = true;
            }
        } else if(distY > 0 && distX > 0 || distY > 0 && distX == 0) {// Cima esquerda
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

        }  else if(distY > 0 && distX < 0 || distY == 0 && distX > 0) {// Cima direita
            for(int x1 = 1; x1 >= -1; x1--) {
                for(int y1 = -1; y1 <= 1; y1++) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }

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

        } else if(distY < 0 && distX < 0 || distY == 0 && distX < 0) { // Baixo direita
            for(int x1 = 1; x1 >= -1; x1--) {
                for(int y1 = 1; y1 >= -1; y1--) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }

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

        } else if(distY < 0 && distX > 0 || distY < 0 && distX == 0) { // Baixo esquerda
            for(int x1 = -1; x1 <= 1; x1++) {
                for(int y1 = 1; y1 >= -1; y1--) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }

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
                    for(int x1 = -2; x1 <= 2; x1++) {
                        for(int y1 = -2; y1 <= 2; y1++) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
                                continue;
                            }
                            if(PF_Find(s->map, c, x + x1, y + y1, 0, false)) {
                                actionMade = true;
                                break;
                            }
                        }
                        if(actionMade) {
                            break;
                        }
                    }
                } else if(distY > 0 && distX < 0 || distY == 0 && distX > 0) {
                    for(int x1 = 2; x1 >= -2; x1--) {
                        for(int y1 = -2; y1 <= 2; y1++) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
                                continue;
                            }
                            if(PF_Find(s->map, c, x + x1, y + y1, 0, false)) {
                                actionMade = true;
                                break;
                            }
                        }
                        if(actionMade) {
                            break;
                        }
                    }

                } else if(distY < 0 && distX < 0 || distY == 0 && distX < 0) {
                    
                    for(int x1 = 2; x1 >= -2; x1--) {
                        for(int y1 = 2; y1 >= -2; y1--) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
                                continue;
                            }
                            if(PF_Find(s->map, c, x + x1, y + y1, 0, false)) {
                                actionMade = true;
                                break;
                            }
                        }
                        if(actionMade) {
                            break;
                        }
                    }

                } else if(distY < 0 && distX > 0 || distY < 0 && distX == 0) {
                    
                    for(int x1 = -2; x1 <= 2; x1++) {
                        for(int y1 = 2; y1 >= -2; y1--) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
                                continue;
                            }
                            if(PF_Find(s->map, c, x + x1, y + y1, 0, false)) {
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
    }
}
#include "server.h"
#include "pathfind.h"

void Server_UpdateCharMovement(Server* s, Character* c) {
    if(c->forcingMovement && c->x == c->renderX && c->y == c->renderY) {
        if(Character_TryToMove(c, c->movementStack[c->movementStackTop], s->map)) {
            c->movementStackTop--;
            if(c->movementStackTop == -1) {
                c->forcingMovement = false;
            }
        } else {
            c->forcingMovement = false;
            c->movementStackTop = -1;
            PF_Find(s->map, c, c->targetX, c->targetY);
        }
        char sendData[32];
        sprintf(sendData, "POS %d %d %d %d", c->id, c->x, c->y, c->direction);
        Server_SendToAll(s, sendData, -1);
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
    // s->clients[id]->b.difficulty
    // ISSO AQUI TA UMA BOSTA, POR FAVOR NAO USA NADA


    Character* c = s->map->characters[id];
    if(!c->dead && !c->forcingMovement && s->clients[id]->bombsPlaced == 0) {
        int x, y, bombX, bombY;
        bool actionMade = false;
        Character_GetTilePosition(c, &x, &y);

        //pegando o boneco mais proximo
        int menorDist = 40;
        int distX, distY;
        int posX_prox = -1, posY_prox = -1;
        for(int i=0; i < s->maxClients; i++) {
            if(s->clients[i] != NULL && i != id && !s->map->characters[i]->dead) {
                int x2 = s->map->characters[i]->x;
                int y2 = s->map->characters[i]->y;
                Character_GetTilePosition(s->map->characters[i], &x2, &y2);
                //printf("AAAAAAAAAAAAAAAAAAAAA %d %d %d %d DISTANCE %d\n", x,y,x2,y2, distance(x,y,x2,y2) );
                if(distance(x, y, x2, y2) < menorDist && distance(x, y, x2, y2) > 0) {
                    menorDist = distance(x, y, x2, y2);
                    distX = x - x2;
                    distY = y - y2;
                    //printf("XXXXXXXXXXXXXXXXXXXXx %d %d\n",a,b);
                    posX_prox = x2;
                    posY_prox = y2;
                }
            }
        }

        //printf("XXXXXXXXXXXXX %d %d\n", a, b);
        if(posX_prox == -1 && posY_prox == -1)
            return;

        if(menorDist < (s->clients[id]->bombRadius + 2) && s->clients[id]->b.difficulty > DIFFICULTY_EASY) {
            if(s->clients[id]->b.difficulty == DIFFICULTY_MEDIUM || (x == posX_prox || y == posY_prox)) {
                Server_PlaceBomb(s, id);
                actionMade = true;
            }
        } else if(distY > 0 && distX > 0 || distY > 0 && distX == 0) {// cima esquerda
            printf("CIMA ESQUERDA\n");
            for(int x1 = -1; x1 <= 1; x1++) {
                for(int y1 = -1; y1 <= 1; y1++) {
                    printf("%d %d\n", x1, y1);
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }
                    TemporaryObject o = s->map->objects[y + y1][x + x1];
                    if(o.exists && o.type == OBJ_WALL) {
                        // se tiver parede em volta colocar uma bomba
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

        }  else if(distY > 0 && distX < 0 || distY == 0 && distX > 0) {// cima direita
            printf("CIMA DIREITA\n");
            for(int x1 = 1; x1 >= -1; x1--) {
                for(int y1 = -1; y1 <= 1; y1++) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }
                    TemporaryObject o = s->map->objects[y + y1][x + x1];
                    if(o.exists && o.type == OBJ_WALL) {
                        // se tiver parede em volta colocar uma bomba
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

        } else if(distY < 0 && distX < 0 || distY == 0 && distX < 0) { // baixo direita
            printf("BAIXO DIREITA\n");
            for(int x1 = 1; x1 >= -1; x1--) {
                for(int y1 = 1; y1 >= -1; y1--) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }
                    TemporaryObject o = s->map->objects[y + y1][x + x1];
                    if(o.exists && o.type == OBJ_WALL) {
                        // se tiver parede em volta colocar uma bomba
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

        } else if(distY < 0 && distX > 0 || distY < 0 && distX == 0) { // baixo esquerda
            printf("BAIXO ESQUERDA\n");
            for(int x1 = -1; x1 <= 1; x1++) {
                for(int y1 = 1; y1 >= -1; y1--) {
                    if(x1 != 0 && y1 != 0 || x1 == 0 && y1 == 0) {
                        continue;
                    }
                    TemporaryObject o = s->map->objects[y + y1][x + x1];
                    if(o.exists && o.type == OBJ_WALL) {
                        // se tiver parede em volta colocar uma bomba
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
        if(actionMade) {
            actionMade = false;
            // achar um lugar perto pra correr
            for(int x1 = -2; x1 <= 2; x1++) {
                for(int y1 = -2; y1 <= 2; y1++) {
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
                //AQIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
            if(PF_Find(s->map, c, posX_prox, posY_prox)) {
                actionMade = true;
            } else {
                if(distY > 0 && distX > 0 || distY > 0 && distX == 0) {
                    for(int x1 = -2; x1 <= 2; x1++) {
                        for(int y1 = -2; y1 <= 2; y1++) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
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
                } else if(distY > 0 && distX < 0 || distY == 0 && distX > 0) {
                    for(int x1 = 2; x1 >= -2; x1--) {
                        for(int y1 = -2; y1 <= 2; y1++) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
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

                } else if(distY < 0 && distX < 0 || distY == 0 && distX < 0) {
                    
                    for(int x1 = 2; x1 >= -2; x1--) {
                        for(int y1 = 2; y1 >= -2; y1--) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
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

                } else if(distY < 0 && distX > 0 || distY < 0 && distX == 0) {
                    
                    for(int x1 = -2; x1 <= 2; x1++) {
                        for(int y1 = 2; y1 >= -2; y1--) {
                            if(x1 == 0 && y1 == 0 || !Character_Passable(c, s->map, x + x1, y + y1)) {
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
    }
}










    /*
    Character* c = s->map->characters[id];
    if(!c->dead && !c->forcingMovement && s->clients[id]->bombsPlaced == 0) {
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
                    bombX = x;
                    bombY = y;
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
            for(int x1 = -2; x1 <= 2; x1++) {
                for(int y1 = -2; y1 <= 2; y1++) {
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
            for(int x1 = -2; x1 <= 2; x1++) {
                for(int y1 = -2; y1 <= 2; y1++) {
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
*/
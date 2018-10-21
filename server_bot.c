#include "server.h"

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

void Server_UpdateBot(Server* s, Character* c) {
    
}
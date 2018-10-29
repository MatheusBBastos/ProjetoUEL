#ifndef SCENE_SINGLEPLAYER_H
#define SCENE_SINGLEPLAYER_H

typedef struct SceneManagerS SceneManagerS;

#include "headers.h"
#include "scene_base.h"
#include "widgets.h"

typedef struct Scene_Singleplayer {
    WTexture* iniciar;
    WTexture* continuar;
    WTexture* voltar;
    WTexture* background;
    WTexture* mensagem;
    WTexture* mensagem1;
    WTexture* sim;
    WTexture* nao;
    WTexture* bemVindo;
    WTexture* nome;
    WTexture* pontu;
    WTexture* nivel;
    WTexture* boxbox;
    WTexture* imgBoneco;
    SDL_Rect renderQuad;
    SDL_Rect posBoneco;
    bool primeiraTela;
    bool temArquivo;
    short int index;
    int pontuacao;
    bool waitingConnection;
} Scene_Singleplayer;

Scene_Singleplayer* SceneSingleplayer_new();

void SceneSingleplayer_update(Scene_Singleplayer* s);

void SceneSingleplayer_handleEvent(Scene_Singleplayer* s, SDL_Event* e);

void SceneSingleplayer_destroy(Scene_Singleplayer* s);

#endif
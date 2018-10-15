#include "scene_start.h"

Scene_Start* SceneStart_new() {
    Scene_Start* newScene = malloc(sizeof(Scene_Start));

    newScene->background = WD_CreateTexture();
    newScene->start = WD_CreateTexture();
    newScene->options = WD_CreateTexture();
    newScene->quit = WD_CreateTexture();
    newScene->seta = WD_CreateTexture();
    newScene->index = 0;

    SDL_Color white = {255, 255, 255};

    WD_TextureLoadFromFile(newScene->background, "content/BG_Start.png");
    WD_TextureLoadFromFile(newScene->seta, "content/seta.png");
    WD_TextureLoadFromText(newScene->start, "JOGAR", Game.startFont, white);
    WD_TextureLoadFromText(newScene->options, "OPCOES", Game.startFont, white);
    WD_TextureLoadFromText(newScene->quit, "SAIR", Game.startFont, white);

    //int w = newScene->background->w, h = newScene->background->h;
    newScene->renderQuad.x = 0;
    newScene->renderQuad.y = 0;
    newScene->renderQuad.w = REFERENCE_WIDTH;
    newScene->renderQuad.h = REFERENCE_HEIGHT;

    return newScene;
}

void SceneStart_update(Scene_Start* s) {
    int startX = (REFERENCE_WIDTH - s->start->w) / 2;
    int optionsX = (REFERENCE_WIDTH - s->options->w) / 2;
    int quitX = (REFERENCE_WIDTH - s->quit->w) / 2;
    WD_TextureRenderDest(s->background, &s->renderQuad);
    WD_TextureRender(s->start, startX, 701);
    WD_TextureRender(s->options, optionsX, 793);
    WD_TextureRender(s->quit, quitX, 885);

    SDL_SetTextureColorMod(s->start->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->options->mTexture, 255, 255, 255);
    SDL_SetTextureColorMod(s->quit->mTexture, 255, 255, 255);
    if(s->index == 0) {
        SDL_SetTextureColorMod(s->start->mTexture, 247, 159, 55);
        WD_RenderArrow(s->seta, startX, 701, s->start->w);
    } else if(s->index == 1) {
        SDL_SetTextureColorMod(s->options->mTexture, 247, 159, 55);
        WD_RenderArrow(s->seta, optionsX, 793, s->options->w);
    } else if(s->index == 2) {
        SDL_SetTextureColorMod(s->quit->mTexture, 247, 159, 55);
        WD_RenderArrow(s->seta, quitX, 885, s->quit->w);
    }

}

void SceneStart_destroy(Scene_Start* s) {
    WD_TextureDestroy(s->background);
    WD_TextureDestroy(s->start);
    WD_TextureDestroy(s->options);
    WD_TextureDestroy(s->quit);
    WD_TextureDestroy(s->seta);
    free(s);
}

void SceneStart_handleEvent(Scene_Start* s, SDL_Event* e) {
    if(SceneManager.inTransition)
        return;
    if(e->type == SDL_KEYDOWN) {
        if(e->key.keysym.sym == SDLK_RETURN) {
            if(s->index == 0)
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_LOGIN);
            else if(s->index == 1)
                SceneManager_performTransition(DEFAULT_TRANSITION_DURATION, SCENE_TUTORIAL);
            else if(s->index == 2) {
                SceneManager.quit = true;
            }
        } else if(e->key.keysym.sym == SDLK_DOWN) {
            if(s->index < 2)
                s->index++;
        } else if(e->key.keysym.sym == SDLK_UP) {
            if(s->index > 0)
                s->index--;
        }
    }
}



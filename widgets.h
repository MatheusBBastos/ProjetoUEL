// widgets.h - Esta parte do código facilita a criação de widgets na tela.

#ifndef WIDGETS_H
#define WIDGETS_H

#include "headers.h"

// WTexture - estrutura que carrega informações de uma textura
typedef struct WTexture {
    SDL_Texture* mTexture;
    int w, h;
} WTexture;

WTexture* WD_CreateTexture();

void WD_TextureDestroy(WTexture* wtexture);

void WD_TextureLoadFromText(WTexture* wtexture, char* newTextStr, TTF_Font* font, SDL_Color color);

void WD_TextureLoadFromFile(WTexture* wtexture, char* path);

void WD_TextureRender(WTexture* wtexture, int x, int y);

void WD_TextureRenderDest(WTexture* wtexture, SDL_Rect* renderQuad);

void WD_TextureRenderEx(WTexture* wtexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip);

// Button - estrutura que carrega informações necessárias para renderizar um botão
typedef struct Button {
    WTexture* textW;
    SDL_Rect buttonRect;
    SDL_Color buttonColor;
    bool clicking;
} Button;

Button* WD_CreateButton(char* text, int x, int y, TTF_Font* font, SDL_Color textColor, SDL_Color buttonColor);

void WD_ButtonRender();

#endif
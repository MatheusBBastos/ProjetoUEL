// widgets.h - Esta parte do código facilita a criação de widgets na tela.

#ifndef WIDGETS_H
#define WIDGETS_H

#include "headers.h"

// WTexture - estrutura que carrega informações de uma textura
typedef struct WTexture {
    SDL_Texture* mTexture;
    int w, h;
    bool text;
} WTexture;

WTexture* WD_CreateTexture();

void WD_TextureDestroy(WTexture* wtexture);

void WD_TextureLoadFromText(WTexture* wtexture, char* newTextStr, TTF_Font* font, SDL_Color color);

void WD_TextureLoadFromFile(WTexture* wtexture, char* path);

void WD_TextureRender(WTexture* wtexture, int x, int y);

void WD_TextureRenderDest(WTexture* wtexture, SDL_Rect* renderQuad);

void WD_TextureRenderEx(WTexture* wtexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip);

void WD_TextureRenderExCustom(WTexture* wtexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, int w, int h);

void WD_RenderArrow(WTexture* s, int posX, int posY, int width, int height); 

// Button - estrutura que carrega informações necessárias para renderizar um botão
typedef struct Button {
    WTexture* textW;
    SDL_Rect buttonRect;
    SDL_Color buttonColor;
    bool clicking;
} Button;

Button* WD_CreateButton(char* text, int x, int y, TTF_Font* font, SDL_Color textColor, SDL_Color buttonColor);

void WD_ButtonRender();

// TextBox - estrutura que carrega informações de uma caixa de texto
typedef struct TextBox {
    int x, y, width, height, cursorX, cursorY, maxSize;
    TTF_Font* font;
    char* text;
    char* displayText;
    SDL_Color textColor;
    bool needRefresh, active, password;
    WTexture* textTexture;
    SDL_Rect textClip;
} TextBox;

TextBox* WD_CreateTextBox(int x, int y, int width, int height, int maxSize, TTF_Font* font, SDL_Color textColor, bool password);

bool WD_TextBoxHandleEvent(TextBox* t, SDL_Event* e);

void WD_TextBoxRender(TextBox* t, unsigned frameCount);

void WD_TextBoxDestroy(TextBox* t);

#endif
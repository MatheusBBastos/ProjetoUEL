#ifndef WIDGETS_H
#define WIDGETS_H

#include "headers.h"

typedef struct Text {
    SDL_Texture* mTexture;
    int w, h;
} Text;

Text* WD_CreateText();

void WD_TextDestroy(Text* text);

void WD_TextLoad(Text* text, char* newTextStr, TTF_Font* font, SDL_Color color);

void WD_TextRender(Text* text, int x, int y);

typedef struct Button {
    Text* textW;
    SDL_Rect buttonRect;
    SDL_Color buttonColor;
    bool clicking;
} Button;

Button* WD_CreateButton(char* text, int x, int y, TTF_Font* font, SDL_Color textColor, SDL_Color buttonColor);

void WD_ButtonRender();

#endif
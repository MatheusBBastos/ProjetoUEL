#include "widgets.h"

Text* WD_CreateText() {
    Text* newText = malloc(sizeof(Text));
    newText->mTexture = NULL;
    newText->w = 0;
    newText->h = 0;
    return newText;
}

void WD_TextDestroy(Text* text) {
    if(text->mTexture != NULL) {
        SDL_DestroyTexture(text->mTexture);
        text->mTexture = NULL;
    }
    free(text);
}

void WD_TextLoad(Text* text, char* newTextStr, TTF_Font* font, SDL_Color color) {
    if(text->mTexture != NULL) {
        SDL_DestroyTexture(text->mTexture);
    }
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, newTextStr, color);
    text->mTexture = SDL_CreateTextureFromSurface(gInfo.renderer, textSurface);
    text->w = textSurface->w;
    text->h = textSurface->h;
    SDL_FreeSurface(textSurface);
}

void WD_TextRender(Text* text, int x, int y) {
    SDL_Rect textRect = {x, y, text->w, text->h};
    SDL_RenderCopy(gInfo.renderer, text->mTexture, NULL, &textRect);
}

Button* WD_CreateButton(char* text, int x, int y, TTF_Font* font, SDL_Color textColor, SDL_Color buttonColor) {
    Button* newButton = malloc(sizeof(Button));
    newButton->textW = WD_CreateText();
    WD_TextLoad(newButton->textW, text, font, textColor);
    newButton->buttonRect.w = newButton->textW->w + 12;
    newButton->buttonRect.h = newButton->textW->h + 6;
    newButton->buttonRect.x = x;
    newButton->buttonRect.y = y;
    newButton->buttonColor = buttonColor;
}

void WD_ButtonRender(Button* button) {
    SDL_SetRenderDrawColor(gInfo.renderer, button->buttonColor.r, button->buttonColor.g, button->buttonColor.b, button->buttonColor.a);
    SDL_RenderFillRect(gInfo.renderer, &button->buttonRect);
    WD_TextRender(button->textW, button->buttonRect.x + 6, button->buttonRect.y + (button->clicking ? 4 : 3));
}

void WD_ButtonDestroy(Button* button) {
    WD_TextDestroy(button->textW);
    free(button);
}
#include "widgets.h"

// Cria uma textura e retorna seu ponteiro
WTexture* WD_CreateTexture() {
    WTexture* newWTexture = malloc(sizeof(WTexture));
    newWTexture->mTexture = NULL;
    newWTexture->w = 0;
    newWTexture->h = 0;
    return newWTexture;
}

// Destrói uma textura alocada anteriormente
void WD_TextureDestroy(WTexture* wtexture) {
    if(wtexture->mTexture != NULL) {
        SDL_DestroyTexture(wtexture->mTexture);
        wtexture->mTexture = NULL;
    }
    free(wtexture);
}

// Carrega uma textura a partir de uma string, fonte e cor
void WD_TextureLoadFromText(WTexture* wtexture, char* newTextStr, TTF_Font* font, SDL_Color color) {
    if(wtexture->mTexture != NULL) {
        SDL_DestroyTexture(wtexture->mTexture);
    }
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, newTextStr, color);
    wtexture->mTexture = SDL_CreateTextureFromSurface(gInfo.renderer, textSurface);
    wtexture->w = textSurface->w;
    wtexture->h = textSurface->h;
    SDL_FreeSurface(textSurface);
}

// Carrega uma textura a partir de um arquivo de imagem PNG
void WD_TextureLoadFromFile(WTexture* wtexture, char* path) {
    if(wtexture->mTexture != NULL) {
        SDL_DestroyTexture(wtexture->mTexture);
    }
    SDL_Surface* imgSurface = IMG_Load(path);
    if(imgSurface == NULL) {
        printf("Falha ao carregar imagem! Erro: %s\n", IMG_GetError());
    } else {
        SDL_SetColorKey(imgSurface, SDL_TRUE, SDL_MapRGB(imgSurface->format, 0, 255, 255));
        wtexture->mTexture = SDL_CreateTextureFromSurface(gInfo.renderer, imgSurface);
        wtexture->w = imgSurface->w;
        wtexture->h = imgSurface->h;
        SDL_FreeSurface(imgSurface);
    }
}

// Renderiza uma textura (necessário que ela já tenha sido alocada e carregada)
void WD_TextureRender(WTexture* wtexture, int x, int y) {
    SDL_Rect textRect = {x, y, wtexture->w, wtexture->h};
    SDL_RenderCopy(gInfo.renderer, wtexture->mTexture, NULL, &textRect);
}

// Cria um novo botão a partir das informações fornecidas e retorna seu ponteiro
Button* WD_CreateButton(char* text, int x, int y, TTF_Font* font, SDL_Color textColor, SDL_Color buttonColor) {
    Button* newButton = malloc(sizeof(Button));
    newButton->textW = WD_CreateTexture();
    WD_TextureLoadFromText(newButton->textW, text, font, textColor);
    newButton->buttonRect.w = newButton->textW->w + 12;
    newButton->buttonRect.h = newButton->textW->h + 6;
    newButton->buttonRect.x = x;
    newButton->buttonRect.y = y;
    newButton->buttonColor = buttonColor;
    return newButton;
}

// Renderiza um botão
void WD_ButtonRender(Button* button) {
    SDL_SetRenderDrawColor(gInfo.renderer, button->buttonColor.r, button->buttonColor.g, button->buttonColor.b, button->buttonColor.a);
    SDL_RenderFillRect(gInfo.renderer, &button->buttonRect);
    WD_TextureRender(button->textW, button->buttonRect.x + 6, button->buttonRect.y + (button->clicking ? 4 : 3));
}

// Destrói um botão alocado anteriormente
void WD_ButtonDestroy(Button* button) {
    WD_TextureDestroy(button->textW);
    free(button);
}
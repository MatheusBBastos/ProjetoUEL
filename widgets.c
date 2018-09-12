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
    wtexture->mTexture = IMG_LoadTexture(gInfo.renderer, path);
    if(wtexture->mTexture == NULL) {
        printf("Falha ao carregar imagem! Erro: %s\n", IMG_GetError());
    } else {
        SDL_QueryTexture(wtexture->mTexture, NULL, NULL, &wtexture->w, &wtexture->h);
    }
}

// Renderiza uma textura (necessário que ela já tenha sido alocada e carregada)
void WD_TextureRender(WTexture* wtexture, int x, int y) {
    SDL_Rect textureRect = {x, y, wtexture->w, wtexture->h};
    SDL_RenderCopy(gInfo.renderer, wtexture->mTexture, NULL, &textureRect);
}

// Renderiza a textura em um retângulo
void WD_TextureRenderDest(WTexture* wtexture, SDL_Rect* renderQuad) {
    SDL_RenderCopy(gInfo.renderer, wtexture->mTexture, NULL, renderQuad);
}

// Renderiza uma textura com mais opções
// clip: região da textura que será renderizada
// angle: ângulo (em graus) que indica a rotação em sentido horário aplicada na imagem
// center: ponto indicando o centro em torno do qual a imagem será rotacionada
// flip: valor indicando o espelhamento da imagem; Valores possíves: SDL_FLIP_HORIZONTAL, SDL_FLIP_VERITICAL, SDL_FLIP_NONE
void WD_TextureRenderEx(WTexture* wtexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect textureRect = {x, y, wtexture->w, wtexture->h};
    if(clip != NULL) {
        textureRect.w = clip->w;
        textureRect.h = clip->h;
    }
    SDL_RenderCopyEx(gInfo.renderer, wtexture->mTexture, clip, &textureRect, angle, center, flip);
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

// Cria uma nova caixa de texto a partir das informações fornecidas
// maxSize - tamanho máximo do texto
// password - o texto contido não será mostrado, no lugar dele ficarão asteriscos
TextBox* WD_CreateTextBox(int x, int y, int width, int height, int maxSize, TTF_Font* font, SDL_Color textColor, bool password) {
    TextBox* newTextBox = malloc(sizeof(TextBox));
    newTextBox->x = x;
    newTextBox->y = y;
    newTextBox->width = width;
    newTextBox->height = height;
    newTextBox->maxSize = maxSize;
    newTextBox->font = font;
    newTextBox->textColor = textColor;
    newTextBox->password = password;
    newTextBox->textTexture = WD_CreateTexture();
    newTextBox->text = malloc(maxSize * sizeof(char));
    newTextBox->displayText = malloc(maxSize * sizeof(char));
    newTextBox->text[0] = '\0';
    newTextBox->displayText[0] = '\0';
    newTextBox->needRefresh = true;
    newTextBox->active = false;
    return newTextBox;
}

// Renderiza uma caixa de texto
void WD_TextBoxRender(TextBox* t, unsigned frameCount) {
    // Só carregar novas texturas caso necessário
    if(t->needRefresh) {
        if(t->password) {
            // Preencher o texto com asteriscos caso a caixa esteja no modo senha
            int i, len = strlen(t->text);
            for(i = 0; i < len; i++) {
                t->displayText[i] = '*';
            }
            t->displayText[i] = '\0';
            // Carregar textura do novo texto
            if(i == 0)
                WD_TextureLoadFromText(t->textTexture, " ", t->font, t->textColor);
            else
                WD_TextureLoadFromText(t->textTexture, t->displayText, t->font, t->textColor);
        } else {
            // Carregar textura do novo texto
            if(strlen(t->text) == 0)
                WD_TextureLoadFromText(t->textTexture, " ", t->font, t->textColor);
            else
                WD_TextureLoadFromText(t->textTexture, t->text, t->font, t->textColor);
        }
        // Coordenadas iniciais do cursor
        t->cursorX = t->x + 4;
        int xClip, wClip;
        // Se o texto está maior que a caixa de texto
        if(t->textTexture->w > t->width - 12) {
            // Cursor na posição máxima
            t->cursorX += t->width - 12;
            // Parte do texto renderizada será a mais da direita
            xClip = t->textTexture->w - t->width + 12;
            wClip = t->width - 12;
        } else {
            if(strlen(t->text) > 0) {
                // Posicionar o cursor à direita do texto
                t->cursorX += t->textTexture->w;
            }
            xClip = 0;
            wClip = t->textTexture->w;
        }
        t->cursorY = t->y;
        t->textClip.x = xClip;
        t->textClip.y = 0;
        t->textClip.w = wClip;
        t->textClip.h = t->height - 3;
        t->needRefresh = false;
    }
    // Renderizar o texto
    WD_TextureRenderEx(t->textTexture, t->x + 4, t->y + 4, &t->textClip, 0.0, NULL, SDL_FLIP_NONE);
    if(t->active && frameCount >= gInfo.screenFreq / 2) {
        SDL_Rect cursorRect = {t->cursorX, t->y + 2, 1, t->height - 4};
        SDL_SetRenderDrawColor(gInfo.renderer, t->textColor.r, t->textColor.g, t->textColor.b, t->textColor.a);
        // Renderizar o cursor
        SDL_RenderFillRect(gInfo.renderer, &cursorRect);
    }
}

// Lida com um evento na caixa de texto
bool WD_TextBoxHandleEvent(TextBox* t, SDL_Event* e) {
    if(t->active) {
        if(e->type == SDL_TEXTINPUT) {
            // Adicionar texto digitado se ele couber
            if(t->maxSize >= strlen(t->text) + strlen(e->text.text)) {
                strcat(t->text, e->text.text);
                t->needRefresh = true;
            }
            return true;
        } else if(e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_BACKSPACE) {
            // Backspace para uma string em UTF-8
            int textlen = SDL_strlen(t->text);
            if(textlen > 0) {
                while(true) {
                    if(textlen == 0) {
                        break;
                    }
                    if((t->text[textlen-1] & 0x80) == 0x00) {
                        t->text[textlen-1] = 0x00;
                        break;
                    }
                    if ((t->text[textlen-1] & 0xC0) == 0x80) {
                        t->text[textlen-1] = 0x00;
                        textlen--;
                    }
                    if ((t->text[textlen-1] & 0xC0) == 0xC0) {
                        t->text[textlen-1] = 0x00;
                        break;
                    }
                }
                t->needRefresh = true;
            }
            return true;
        }
    }
    return false;
}

// Destrói uma caixa de texto
void WD_TextBoxDestroy(TextBox* t) {
    WD_TextureDestroy(t->textTexture);
    free(t->text);
    free(t->displayText);
    free(t);
}
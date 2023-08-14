#ifndef SRC_UTILITIES_H_
#define SRC_UTILITIES_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <bits/stdc++.h>
using namespace std;

void renderTexture(SDL_Texture *tex,
        SDL_Renderer *ren, SDL_Rect dst, SDL_Rect *src = nullptr);

void renderTexture(SDL_Texture *tex,
        SDL_Renderer *ren, double x, double y, SDL_Rect *src = nullptr);

SDL_Texture *loadTextureFromIMG(const std::string &path, SDL_Renderer *renderer);

SDL_Texture* renderText(const string &message,
        const string &fontFile, SDL_Color color,
        int fontSize, SDL_Renderer *renderer);

#endif  // SRC_UTILITIES_H_

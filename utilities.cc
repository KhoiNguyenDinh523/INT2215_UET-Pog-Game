#include "utilities.h"

using namespace std;

void renderTexture(SDL_Texture* tex,
        SDL_Renderer* ren, double x, double y, SDL_Rect* src) {
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    if (src != nullptr) {
        dst.w = src->w;
        dst.h = src->h;
    } else {
        SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
    }
    SDL_RenderCopy(ren, tex, src, &dst);
}

SDL_Texture *loadTextureFromIMG(const std::string &path, SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(("assets/" + path).c_str());
    if (!surface) {
        std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture* renderText(const string &message,
    const string &fontFile, SDL_Color color,
    int fontSize, SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (!font) std::cout << "Failed to load font. TTF_Error: " << TTF_GetError() << std::endl;

    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_FreeSurface(surf);
    TTF_CloseFont(font);
    return texture;
}

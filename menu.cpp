#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <bits/stdc++.h>
using namespace std;

// Screen dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Menu positioning
const int MENU_START_X = 320;
const int MENU_START_Y = 200;
const int MENU_OPTION_SPACING = 50;


// Initialize SDL and its subsystems
bool initSDL(SDL_Window *&window, SDL_Renderer *&renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL initialization failed. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return 1;

    window = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window creation failed. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cout << "Renderer creation failed. SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// Load a texture from file using SDL_image
SDL_Texture *loadTexture(const std::string &path, SDL_Renderer *renderer) {
    SDL_Surface *surface = IMG_Load(("assets/" + path).c_str());
    if (!surface) {
        std::cout << "Failed to load image: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Render text using SDL_ttf
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

//SDL_Texture *renderText(const std::string &text, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
//    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
//    if (!surface) {
//        std::cout << "Failed to render text: " << TTF_GetError() << std::endl;
//        return nullptr;
//    }
//
//    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
//    SDL_FreeSurface(surface);
//    return texture;
// }

//void renderTexture(SDL_Texture* tex,
//        SDL_Renderer* ren, double x, double y, SDL_Rect* src) {
//    SDL_Rect dst;
//    dst.x = x;
//    dst.y = y;
//    if (src != nullptr) {
//        dst.w = src->w;
//        dst.h = src->h;
//    } else {
//        SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
//    }
//    renderTexture(tex, ren, dst, src);
//}


int main(int argv, char** args) {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!initSDL(window, renderer)) {
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        std::cout << "SDL_ttf initialization failed. TTF_Error: " << TTF_GetError() << std::endl;
        return 1;
    }

    // Load fonts and textures
    SDL_Color textColor = {255,255,255,255};
    string font = "assets/fonts/Peepo.ttf";
    const int optionSize = 36;

//    TTF_Font *font = TTF_OpenFont("assets/fonts/Peepo.ttf", 36);
    SDL_Texture *bg = loadTexture("bg.jpg", renderer);

    SDL_Texture *p1 = renderText("1 Player", font, textColor, optionSize, renderer);
    SDL_Texture *p2 = renderText("2 Player", font, textColor, optionSize, renderer);
    SDL_Texture *settings = renderText("Settings", font, textColor, optionSize, renderer);
    SDL_Texture *exit = renderText("Exit", font, textColor, optionSize, renderer);
    SDL_Texture *menuOptionTextures[4] = {p1, p2, settings, exit};


    SDL_Texture *arrowTexture = loadTexture("select.jpg", renderer);
    if (!arrowTexture) return 1;

    // Main loop
    bool quit = false, inMenu = true;
    int selectedOption = 0; // 0: 1 player, 1: 2 players, 2: Settings, 3: Exit
    SDL_Event e;

    while (!quit) {
//        while (inMenu) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                        case SDLK_UP:
                            selectedOption = (selectedOption - 1 + 4) % 4; // To ensure positive index
                            break;
                        case SDLK_DOWN:
                            selectedOption = (selectedOption + 1) % 4;
                            break;
                        case SDLK_RETURN:
                            if (selectedOption == 3) {
                                quit = true; // Exit
                            } else {
                                inMenu = false;
                                // Handle the chosen option and proceed to the game
                                // For now, we'll just print the chosen option
                                std::cout << "Chosen option: " << selectedOption << std::endl;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            // Clear the screen
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, bg, nullptr, nullptr);

            // Render your menu options and arrow here using SDL functions and textures
            for (int i = 0; i < 4; ++i) {
                int optionX = MENU_START_X;
                int optionY = MENU_START_Y + i * MENU_OPTION_SPACING;
                int optionW, optionH;
                SDL_QueryTexture(menuOptionTextures[i], NULL, NULL, &optionW, &optionH);
                SDL_Rect optionRect = {optionX, optionY, optionW, optionH};
                SDL_RenderCopy(renderer, menuOptionTextures[i], nullptr, &optionRect);
            }

            // Render arrow
            int arrowX = MENU_START_X - 75; // Adjust the X-coordinate based on your design
            int arrowY = MENU_START_Y + selectedOption * MENU_OPTION_SPACING;
            SDL_Rect arrowRect = { arrowX, arrowY, 70, 50};
            SDL_RenderCopy(renderer, arrowTexture, nullptr, &arrowRect);

            // Update the screen
            SDL_RenderPresent(renderer);
//        }
//        switch(selectedOption) {
//            case 0:
//
//                break;
//            case 1:
//            case 2:
//        }
    }

    // Cleanup and shutdown
    for (int i = 0; i < 4; ++i) {
        SDL_DestroyTexture(menuOptionTextures[i]);
    }
    SDL_DestroyTexture(arrowTexture);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

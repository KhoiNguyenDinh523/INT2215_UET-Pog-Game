#include <SDL.h>
#include <SDL_image.h>
#include <bits/stdc++.h>
#include <SDL_ttf.h>
using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Rect player; // Represents the player's movable rectangle

enum PowerUpType {
    PADDLE_SIZE_INCREASE,
    BALL_SPEED_UP,
    OPPONENT_PADDLE_SLOWDOWN,
    NUM_POWER_UP_TYPES
};

class PowerUp {
public:
    SDL_Texture* texture;
    PowerUpType type;
    bool isActive;
    SDL_Rect position;
    int timeToLive;

    PowerUp(SDL_Texture* _texture, PowerUpType _type) {
        texture = _texture;
        type = _type;
        isActive = false;
        position = { 0, 0, 40, 40 };
        timeToLive = 300; // 300 frames = 5 seconds at 60 FPS
    }

    void Spawn() {
        isActive = true;
        position.x = rand() % (SCREEN_WIDTH - position.w);
        position.y = rand() % (SCREEN_HEIGHT - position.h);
    }

    void Update() {
        if (isActive) {
            timeToLive--;
            if (timeToLive <= 0) {
                isActive = false;
            }
        }
    }

    void Render() {
        if (isActive) {
            SDL_RenderCopy(renderer, texture, nullptr, &position);
        }
    }
};

bool CheckCollision(const SDL_Rect& rect1, const SDL_Rect& rect2) {
    return (rect1.x < rect2.x + rect2.w && rect1.x + rect1.w > rect2.x &&
            rect1.y < rect2.y + rect2.h && rect1.y + rect1.h > rect2.y);
}


//UTILS
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

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow("Power-Up Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //Setup fonts
    const string font = "assets/fonts/peepo.ttf";
    SDL_Color textColor = { 255, 0, 0 }; // White color

    // Load power-up textures
    SDL_Texture* paddleSizeIncreaseTexture = IMG_LoadTexture(renderer, "assets/paddle_increase.png");
    SDL_Texture* ballSpeedUpTexture = IMG_LoadTexture(renderer, "assets/speedball.png");
    SDL_Texture* opponentPaddleSlowdownTexture = IMG_LoadTexture(renderer, "assets/opponent_slowdown.png");

    std::vector<PowerUp> powerUps = {
        PowerUp(paddleSizeIncreaseTexture, PADDLE_SIZE_INCREASE),
        PowerUp(ballSpeedUpTexture, BALL_SPEED_UP),
        PowerUp(opponentPaddleSlowdownTexture, OPPONENT_PADDLE_SLOWDOWN)
    };

    player = { 100, 100, 40, 40 }; // Initial player rectangle

    srand(time(0));

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    bool quit = false;

    SDL_Texture* timerTexture;
    stringstream timeText;
    Uint32 lastspawnTime = 0;
    Uint32 spawnInterval = 7000;
    int countdownTime = 5;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        const Uint8* keystates = SDL_GetKeyboardState(nullptr);
        if (keystates[SDL_SCANCODE_LEFT]) {
            player.x -= 3;
        }
        if (keystates[SDL_SCANCODE_RIGHT]) {
            player.x += 3;
        }
        if (keystates[SDL_SCANCODE_UP]) {
            player.y -= 3;
        }
        if (keystates[SDL_SCANCODE_DOWN]) {
            player.y += 3;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &player);

        Uint32 currentTime = SDL_GetTicks() / 1000;

        //Warning powerup spawn

        // Update and render power-ups
//        if (currentTime - lastSpawnTime >= spawnInterval) {
        int powIdx = rand()%3;
        PowerUp* currPU = &powerUps[powIdx];
//            for (PowerUp& powerUp : powerUps) {
                if (!currPU->isActive) {
//                    if (currentTime != lastspawnTime) {
//                        previousTime = currentTime;
//                        countdownTime--;
//                    }
                    int countdownTime = 5;
                    while (countdownTime > 0) {
                        timeText.str("");
                        timeText << "Power-up spawn in: " << countdownTime << " seconds";
                        timerTexture = renderText(timeText.str().c_str(),font,textColor, 30, renderer);
                        int timeW, timeH;
                        SDL_QueryTexture(timerTexture, nullptr, nullptr, &timeW, &timeH);
                        renderTexture(timerTexture, renderer, 20, SCREEN_HEIGHT - timeH - 10, nullptr); // timerPosition is the SDL_Rect for the timer's position
                        countdownTime--;
                    }

                    currPU->Spawn();
//                    lastSpawnTime = currentTime;
                    countdownTime = 5;
//                    break; // Only spawn one power-up per interval
                }
                currPU->Update();
                currPU->Render();

                // Check collision with player
                if (CheckCollision(player, currPU->position)) {
                    if (currPU->isActive) {
                        std::string powerUpType;
                        switch (currPU->type) {
                            case PADDLE_SIZE_INCREASE:
                                powerUpType = "Paddle Size Increase";
                                break;
                            case BALL_SPEED_UP:
                                powerUpType = "Ball Speed Up";
                                break;
                            case OPPONENT_PADDLE_SLOWDOWN:
                                powerUpType = "Opponent Paddle Slowdown";
                                break;
                        }
                        std::cout << "Collide with " << powerUpType << " orb" << std::endl;
                        currPU->isActive = false;
                    }
                }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Cap frame rate to approximately 60 FPS

    }

    // Cleanup and exit
    for (PowerUp& powerUp : powerUps) {
        SDL_DestroyTexture(powerUp.texture);
    }
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
//    SDL_RemoveTimer(timerID);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

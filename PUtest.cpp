#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <ctime>

SDL_Window* window;
SDL_Renderer* renderer;

const int windowWidth = 800;
const int windowHeight = 600;

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
        position.x = rand() % (windowWidth - position.w);
        position.y = rand() % (windowHeight - position.h);
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


Uint32 SpawnPowerUpCallback(Uint32 interval, void* param) {
    std::vector<PowerUp>* powerUps = static_cast<std::vector<PowerUp>*>(param);

    int idx = rand()%3;
    powerUps[idx].spawn();
    return interval; // Return the same interval to keep the timer repeating
}



bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        std::cout << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() != 0) {
        std::cout << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        return false;
    }



    window = SDL_CreateWindow("Power-Up Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    if (!window) {
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    init();
    // Load power-up textures
    SDL_Texture* paddleSizeIncreaseTexture = IMG_LoadTexture(renderer, "assets/paddle_increase.png");
    SDL_Texture* ballSpeedUpTexture = IMG_LoadTexture(renderer, "assets/speedball.png");
    SDL_Texture* opponentPaddleSlowdownTexture = IMG_LoadTexture(renderer, "assets/opponent_slowdown.png");

    // Load TTF font
    TTF_Font* font = TTF_OpenFont("assets/fonts/peepo.ttf", 24); // Replace with your font file and size

    std::vector<PowerUp> powerUps = {
        PowerUp(paddleSizeIncreaseTexture, PADDLE_SIZE_INCREASE),
        PowerUp(ballSpeedUpTexture, BALL_SPEED_UP),
        PowerUp(opponentPaddleSlowdownTexture, OPPONENT_PADDLE_SLOWDOWN)
    };
    player = { 100, 100, 40, 40 }; // Initial player rectangle

    Uint32 spawnInterval = 3000; // 7 seconds in milliseconds
    Uint32 lastSpawnTime = SDL_GetTicks();

    int countdownSeconds = 5;
    Uint32 countdownStartTime = 0;

    srand(time(0));

    bool quit = false;
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

        SDL_TimerID powerUpTimer = SDL_AddTimer(spawnInterval, SpawnPowerUpCallback, &powerUps);

        SDL_RemoveTimer(powerUpTimer);

//        Uint32 currentTime = SDL_GetTicks();
//        if (currentTime - lastSpawnTime >= spawnInterval) {
//            for (PowerUp& powerUp : powerUps) {
//                if (!powerUp.isActive) {
//                    powerUp.Spawn();
//                    lastSpawnTime = currentTime;
//                     Start the countdown timer
//                    countdownStartTime = currentTime;
//                    break; // Only spawn one power-up per interval
//                }
//            }
//        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set the background color to black
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0,255); // Set the player rectangle color to red
        SDL_RenderFillRect(renderer, &player);

        // Update and render power-ups
        for (PowerUp& powerUp : powerUps) {
            if (countdownStartTime != 0) {
                int countdownRemaining = countdownSeconds - (currentTime - countdownStartTime) / 1000;

                // Create and render the countdown timer text
                std::string countdownText = "Power-up spawns in " + std::to_string(countdownRemaining) + " seconds";
                SDL_Color textColor = { 255, 255, 255, 255 }; // White color
                SDL_Surface* countdownSurface = TTF_RenderText_Solid(font, countdownText.c_str(), textColor);
                SDL_Texture* countdownTexture = SDL_CreateTextureFromSurface(renderer, countdownSurface);

                SDL_Rect countdownRect = { (windowWidth - countdownSurface->w) / 2, windowHeight - countdownSurface->h, countdownSurface->w, countdownSurface->h };
                SDL_RenderCopy(renderer, countdownTexture, nullptr, &countdownRect);

                if (countdownRemaining <= 0) {
                    countdownStartTime = 0;

                    // Clean up countdown texture and surface
                    SDL_DestroyTexture(countdownTexture);
                    SDL_FreeSurface(countdownSurface);
                }
            }

            if (!powerUp.isActive && rand() % 200 == 0) {
                powerUp.Spawn();
            }
            powerUp.Update();
            powerUp.Render();

            // Check collision with player
            if (CheckCollision(player, powerUp.position)) {
                if (powerUp.isActive) {
                    std::string powerUpType;
                    switch (powerUp.type) {
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
                    powerUp.isActive = false;
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Cap frame rate to approximately 60 FPS
    }

    // Cleanup and exit
    for (PowerUp& powerUp : powerUps) {
        SDL_DestroyTexture(powerUp.texture);
    }

    // Cleanup TTF font
    TTF_CloseFont(font);

    // Cleanup countdown texture and surface
//    if (countdownTexture) {
//        SDL_DestroyTexture(countdownTexture);
//    }
//    if (countdownSurface) {
//        SDL_FreeSurface(countdownSurface);
//    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

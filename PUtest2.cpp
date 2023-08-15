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
        timeToLive = 180; // 300 frames = 5 seconds at 60 FPS
    }

    void spawn() {
        isActive = true;
        position.x = rand() % (windowWidth - position.w);
        position.y = rand() % (windowHeight - position.h);
    }

    bool checkCollision(const SDL_Rect& rect1) const{
        SDL_Rect rect2 = position;
        return (rect1.x < rect2.x + rect2.w && rect1.x + rect1.w > rect2.x &&
                rect1.y < rect2.y + rect2.h && rect1.y + rect1.h > rect2.y);
    }

    void update() {
        if (isActive) {
            timeToLive--;
            if (timeToLive <= 0) {
                isActive = false;
            }
        }
    }

    void render() {
        if (isActive) {
            SDL_RenderCopy(renderer, texture, nullptr, &position);
        }
    }
};

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow("Power-Up Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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

    PowerUp* currPU = &powerUps[2];
    Uint32 spawnInterval = 7000; // 7 seconds in milliseconds
    Uint32 lastSpawnTime = 0;

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set the background color to black
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Set the player rectangle color to red
        SDL_RenderFillRect(renderer, &player);

        // Render countdown
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastSpawnTime;
        if (deltaTime < spawnInterval) {
//            if (deltaTime + 5000 == spawnInterval) countdownStartTime = currentTime;
//            int countdownRemaining = 5 - (currentTime - countdownStartTime) / 1000;
            int countdownRemaining = (spawnInterval - deltaTime) / 1000;

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
                if(countdownTexture) SDL_DestroyTexture(countdownTexture);
                if(countdownSurface) SDL_FreeSurface(countdownSurface);
            }
        }

        //
        if (deltaTime >= spawnInterval) {
//            if(!currPU->isActive) {
                int powIdx = rand()%3;
                currPU = &powerUps[powIdx];
                currPU->spawn();
                currPU->update();
                lastSpawnTime = currentTime;
                countdownStartTime = currentTime;
//            }
        }



        if (currPU->isActive && currPU->checkCollision(player)) {
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

        currPU->render();

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

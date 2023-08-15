#include "ball.h"
#include "pong.h"
#include "paddle.h"
#include "power_ups.h"


#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <ctime>

void PowerUp::spawn() {
    isActive = true;
    // Define the boundaries of the spawn area (adjust these values as needed)
    int minX = Paddle::WIDTH + 300;
    int maxX = Pong::SCREEN_WIDTH - Paddle::WIDTH - 300;
    int minY = 250;
    int maxY = Pong::SCREEN_HEIGHT - 250;

    // Generate random coordinates within the defined boundaries
    position.x = minX + rand() % (maxX - minX + 1);
    position.y = minY + rand() % (maxY - minY + 1);

    position.x = rand() % (Pong::SCREEN_WIDTH - position.w);
    position.y = rand() % (Pong::SCREEN_HEIGHT - position.h);
}

bool PowerUp::checkCollision(const Ball *ball) const{
    SDL_Rect rect2 = position;
    return (ball->x < rect2.x + rect2.w && ball->x + Ball::LENGTH > rect2.x &&
            ball->y < rect2.y + rect2.h && ball->y + Ball::LENGTH > rect2.y);
}

void PowerUp::update() {
    if (isActive) {
        timeToLive--;
        if (timeToLive <= 0) {
            isActive = false;
        }
    }
}

void PowerUp::render(SDL_Renderer* ren) {
    if (isActive) {
        SDL_RenderCopy(ren, texture, nullptr, &position);
    }
}

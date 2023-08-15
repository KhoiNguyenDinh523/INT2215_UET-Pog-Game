#include "paddle.h"
#include "ball.h"
#include "pong.h"

const int Paddle::HEIGHT = 130;
const int Paddle::WIDTH = 24;
const int Paddle::VEL = 30;
Paddle::Paddle(double new_x, double new_y) {
    x = new_x;
    y = new_y;
    h = Paddle::HEIGHT;
}

int Paddle::get_x() const {
    return x;
}

int Paddle::get_y() const {
    return y;
}

int Paddle::get_h() const {
    return h;
}

void Paddle::sizeIncrease(){
    h += 20;
}

void Paddle::set_y(int new_y) {
    y = new_y;

    // paddles not allowed to go above or below the screen.
    if (y < 0)
        y = 0;
    else if (y + HEIGHT > Pong::SCREEN_HEIGHT)
        y = Pong::SCREEN_HEIGHT - HEIGHT;
}

void Paddle::add_to_y(int new_y) {
    y += new_y;
    if(y < 0) y = 0;
    else if (y + HEIGHT > Pong::SCREEN_HEIGHT) y = Pong::SCREEN_HEIGHT - HEIGHT;
}

//player 2
void Paddle::Player2() //SDL_Event& e
{
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_UP]) {
        add_to_y(-Paddle::VEL);
    }
    if (currentKeyStates[SDL_SCANCODE_DOWN]) {
        add_to_y(Paddle::VEL);
    }
}

// Imprecise prediction of ball position on the y-axis.
int Paddle::predict(Ball *ball) {
    // Find slope.
    float slope = (ball->dy) /(ball->dx);

    // Distance between ball and paddle.
    int paddle_distance = ball->x - x;

    int predicted_y = abs(ball->y - slope * (paddle_distance));

    // Calculate number of reflexions.
    int number_of_reflexions = predicted_y / Pong::SCREEN_HEIGHT;

    // Predictions taking into consideration upper and bottom
    // wall collisions.
    if (number_of_reflexions % 2 == 0)
        predicted_y = predicted_y % Pong::SCREEN_HEIGHT;
    else
        predicted_y = Pong::SCREEN_HEIGHT - (predicted_y % Pong::SCREEN_HEIGHT);

    return predicted_y;
}

// Basic AI movement.
void Paddle::AI(Ball *ball) {
    // Ball on the left 3/5th side of the screen and going left.
    if (ball->x < Pong::SCREEN_WIDTH*3/5 && ball->dx < 0) {
        // Follow the ball.
        if (y + (HEIGHT - ball->LENGTH)/2 < ball->predicted_y-2)
            add_to_y(ball->speed/8 * 5);
        else if (y + (HEIGHT - ball->LENGTH)/2 > ball->predicted_y+2)
            add_to_y(-(ball->speed/8 * 5));

    // Ball is anywhere on the screen but going right.
    } else if (ball->dx >= 0) {
        // Left paddle slowly moves to the center.
        if (y + HEIGHT / 2 < Pong::SCREEN_HEIGHT/2)
            add_to_y(5);
        else if (y + HEIGHT / 2 > Pong::SCREEN_HEIGHT/2)
            add_to_y(-5);
    }
}

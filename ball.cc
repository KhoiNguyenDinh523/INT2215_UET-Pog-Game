#include <cmath>
#include <random>

#include "ball.h"
#include "pong.h"
#include "paddle.h"

namespace {
    std::random_device rd;
    std::mt19937 gen(rd()); // mersenne_twister_engine use to generate random numbers.
}

// Ball dimensions (a square).
const int Ball::LENGTH = 20;

Ball::Ball(double x, double y) {
    // Ball status.
    status = READY;

    // Ball position.
    this->x = x;
    this->y = y;

    // Ball movement.
    dx = 0;
    dy = 0;
    speed = 15;
    angle = 0;
    hits = 0;
    predicted_y = 0;
}

void Ball::launch_ball() {
    //Launch the ball in a random direction
    std::uniform_int_distribution<int> dir(0, 1);
    int direction = 1 + (-2) * (dir(gen)%2);  // Either 1 or -1.

    std::uniform_int_distribution<int> ang(-60, 60);
    angle = ang(gen);  // Between -60 and 60.

    dx = direction * speed * cos(angle * M_PI/180);  // Speed on x-axis (with angle converted to radian).
    dy = speed * sin(angle * M_PI/180);  // Speed on y-axis.

    status = LAUNCHED;
}

void Ball::bounces_off(Paddle *paddle) {
    if (paddle == nullptr) return;

    hits++;

    int sign = (paddle->get_x() < Pong::SCREEN_WIDTH/2) ? 1 : -1; //1 is left, -1 is right paddle.

    double relative_y = (y - paddle->get_y() + LENGTH); //screen coords where the ball hit the paddle vertically.

    // Calculate the normalized relative position within the paddle (0 to 1)
    double normalized_relative_y = relative_y / Paddle::HEIGHT;

    // Calculate the angle based on the normalized relative position
    // You can experiment with the angle calculation formula
    angle = (-normalized_relative_y * 120) + 60; // Adjust the constants as needed

    dx = sign * speed * cos(angle * M_PI/180);
    dy = speed * sin(angle * M_PI/180);
}

void Ball::update_speed() {
    // Increment ball speed for every 5 hit.
    if (hits == 4) {
        speed+=3;
        hits = 0;
    }
}

bool Ball::wall_collision() {
    return (y + dy < 0) || (y + LENGTH + dy >= Pong::SCREEN_HEIGHT);
}

bool Ball::pad_collides(Paddle *paddle) {
    if (paddle->get_x() < Pong::SCREEN_WIDTH/2) {
        // Check if collision with left paddle occurs.
        if (x > paddle->get_x() + Paddle::WIDTH ||
            x < paddle->get_x() ||
            !(y + LENGTH > paddle->get_y() &&
              y < paddle->get_y() + Paddle::HEIGHT))
            return false;
        else
            return true;
    } else {
        // Check if collision with right paddle occurs.
        if (x + LENGTH < paddle->get_x() ||
            x >= paddle->get_x() + Paddle::WIDTH ||
            !(y + LENGTH > paddle->get_y() &&
              y < paddle->get_y() + Paddle::HEIGHT))
            return false;
        else
            return true;
    }
}

    // Reset ball to initial state.
void Ball::reset() {
    x = Pong::SCREEN_WIDTH/2 - LENGTH/2;
    y = Pong::SCREEN_HEIGHT/2;

    // Ball is fixed.
    dx = 0;
    dy = 0;
    status = READY;

    // Speed and hit counter are reset to their initial positions.
    speed = 15;
    hits = 0;
}

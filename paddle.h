#ifndef SRC_PADDLE_H_
#define SRC_PADDLE_H_

#include <SDL.h>

class Ball;

class Paddle {
 private:
    // Paddle position
    double x;
    double y;
    double h;

 public:
    Paddle(double x, double y);

    // Paddle dimensions
    static const int HEIGHT;
    static const int WIDTH;
    static const int VEL;

    int get_x() const;
    int get_y() const;
    int get_h() const;

    void set_y(int new_y);
    void add_to_y(int new_y);
    int predict(Ball *ball);
    void AI(Ball *ball);
//    void Player2( SDL_Event& e );
    void Player2();

    void sizeIncrease();
};

#endif  // SRC_PADDLE_H_

#ifndef SRC_BALL_H_
#define SRC_BALL_H_

#include "paddle.h"

class Ball {
 public:
        Ball(double x, double y);
        ~Ball() {}

        // Ball status.
        enum Status {READY, LAUNCH, LAUNCHED};
        Status status;

        // Ball size.
        static const int LENGTH;

        // Ball position.
        double x;
        double y;

        // Ball movement(speed along axis x and y).
        double dx;
        double dy;

        double speed;  // ball speed = √(dx²+dy²).
        double angle;  // Angle after collision with paddle.
        int hits;  // Counts the number of hits of the ball with
                   // both paddles, increase speed after every 4 hit.
        double predicted_y;  // Predicted ball position on y-axis after
                          // right paddle collision (used for paddle AI).

        void launch_ball();
        void update_speed();
        bool wall_collision();

        //Interact with paddles
        bool pad_collides(Paddle *paddle);
        void bounces_off(Paddle *paddle);
        void reset();

        //Interact with power orbs
};

#endif  // SRC_BALL_H_

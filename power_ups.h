#ifndef POWER_UPS_H_INCLUDED
#define POWER_UPS_H_INCLUDED

class Powup {
private:
    //positions
    double x, y;
    enum PUType {
        PADDLE_UP,
        BALL_SPEED_UP,
        DOUBALL,
        // Add more power-up types as needed
    };
    PUType type;
   Powup(double x, double y);
   ~Powup() {}


};

#endif // POWER_UPS_H_INCLUDED

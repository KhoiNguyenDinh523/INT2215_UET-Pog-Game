#ifndef POWER_UPS_H_INCLUDED
#define POWER_UPS_H_INCLUDED

enum PowerUpType {
    PADDLE_SIZE_INCREASE,
    BALL_SPEED_UP,
    SHIELD,
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
        position = { 0, 0, 70, 70 };
        timeToLive = 180;
    }

    void spawn();
    bool checkCollision(const Ball *ball) const;
    void update();
    void render(SDL_Renderer* ren);

};

#endif // POWER_UPS_H_INCLUDED

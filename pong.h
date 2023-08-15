#ifndef SRC_PONG_H_
#define SRC_PONG_H_

#include <SDL.h>                       // SDL library.
#include <SDL_ttf.h>                   // SDL font library.
#include <SDL_mixer.h>                 // SDL sound library.
#include <string>
#include <bits/stdc++.h>

class Ball;
class Paddle;
class PowerUp;

class Pong {
 private:
    // Window and renderer.
    SDL_Window* window;  // Holds window properties.
    SDL_Renderer* renderer;  // Holds rendering surface properties.

    // Game objects.
    Ball* ball;
    Paddle* left_paddle;
    Paddle* right_paddle;

    //Sounds.
    Mix_Chunk* paddle_sound;
    Mix_Chunk* wall_sound;
    Mix_Chunk* score_sound;
    Mix_Music* bg_music;

    // Mouse coordinates.
    int mouse_x, mouse_y;

    // Fonts.
    std::string font_name;

    SDL_Color font_color;

    //Text textures
    SDL_Texture* font_image_title;
    SDL_Texture* font_image_launch; // Holds first part of text suggesting to launch the ball.
    SDL_Texture* font_image_left_score;
    SDL_Texture* font_image_right_score;
    SDL_Texture* font_image_winner;
    SDL_Texture* font_image_restart;

    // Scores.
    int left_score;
    int right_score;

    // Indicates when rendering new score is necessary.
    bool left_score_changed;
    bool right_score_changed;

    //Power-up textures
    SDL_Texture* paddleSizeIncreaseTexture;
    SDL_Texture* ballSpeedUpTexture;
    SDL_Texture* shieldTexture;

    std::vector<PowerUp> powerUps;

    //Timing of power ups
    PowerUp* currPU;
    Uint32 spawnInterval; // 7 seconds in milliseconds
    Uint32 lastSpawnTime;

    // Game status.
    bool exit;  // True when player wants to exit game.
    bool inMenu;
    bool isPaused;

 public:
    // Screen resolution.
    static const int SCREEN_WIDTH;
    static const int SCREEN_HEIGHT;

    Pong(int argc, char *argv[]);
    ~Pong();
    void execute();
    void menu();
    void input();
    void PU(); //power-ups
    void update();
    void render();
};

#endif  // SRC_PONG_H_

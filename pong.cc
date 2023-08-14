#include <iostream>
#include "pong.h"
#include "ball.h"
#include "paddle.h"
#include "utilities.h"

// Screen resolution.
const int Pong::SCREEN_WIDTH = 640; //640
const int Pong::SCREEN_HEIGHT = 480; //480

//Game mode
enum difLevel {ez, med, hard};
difLevel dif = ez;
int selectedOption = 0; // 0: 1 player, 1: 2 players, 2: Settings, 3: Exit

//Game state
//bool isPaused = false;

//Game functions
Pong::Pong(int argc, char *argv[]) {
    // Intialize SDL.
    SDL_Init(SDL_INIT_EVERYTHING);

    // Don't show cursor.
//    SDL_ShowCursor(0);

    // Create window and renderer.
    window = SDL_CreateWindow("Pog",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,  // Centered window.
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC); // Uses hardware acceleration with present synchronized with refresh rate --> Smooth animation.

    // Instantiate game objects.
    ball = new Ball(SCREEN_WIDTH/2 - ball->LENGTH/2,
            SCREEN_HEIGHT/2 - ball->LENGTH/2);
    left_paddle = new Paddle(40, SCREEN_HEIGHT/2-Paddle::HEIGHT/2);
    right_paddle = new Paddle(SCREEN_WIDTH-(40+Paddle::WIDTH),
            SCREEN_HEIGHT/2-Paddle::HEIGHT/2);

    // Initialize SDL_mixer.
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

    // Load sounds.
    bg_music = Mix_LoadMUS("assets/sounds/bg music.mp3");
    if( bg_music == NULL )
	{
		printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
	}
    paddle_sound = Mix_LoadWAV("assets/sounds/paddle_hit.wav");
    wall_sound = Mix_LoadWAV("assets/sounds/wall_hit.wav");
    score_sound = Mix_LoadWAV("assets/sounds/score_update.wav");

    // Fonts.
    if (TTF_Init() < 0) {
        std::cout << "SDL_ttf initialization failed. TTF_Error: " << TTF_GetError() << std::endl;
    }
    // Initialize font.
    font_color = {255, 255, 255, 255};
    font_name = "assets/fonts/Peepo.ttf";
    font_image_title = renderText("POG THE GAME 2K23",
            font_name, font_color, 60, renderer);
    font_image_launch = renderText("Press SPACE to launch",
            font_name, font_color, 30, renderer);
    // Scores.
    left_score = 0;
    right_score = 0;

    // Indicates when rendering new score is necessary.
    left_score_changed = true;
    right_score_changed = true;

    // Game status.
    exit = false;
    inMenu = true;
}

Pong::~Pong() {
    // Destroy textures.
    SDL_DestroyTexture(font_image_left_score);
    SDL_DestroyTexture(font_image_right_score);
    SDL_DestroyTexture(font_image_winner);
    SDL_DestroyTexture(font_image_restart);
    SDL_DestroyTexture(font_image_launch);

    // Free sound effects.
    Mix_FreeChunk(paddle_sound);
    Mix_FreeChunk(wall_sound);
    Mix_FreeChunk(score_sound);
    Mix_FreeMusic(bg_music);

    // Quit SDL_mixer.
    Mix_CloseAudio();

    // Destroy renderer and window.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Shuts down SDL.
    TTF_Quit();
    Mix_Quit();
	IMG_Quit();
    SDL_Quit();
}

void Pong::menu() {
    const int MENU_START_X = 320;
    const int MENU_START_Y = 200;
    const int MENU_OPTION_SPACING = 50;
    const int optionSize = 36;

    //Selected option FX
    bool optionSelected = false;
//    Uint32 blinkStartTime = 0;
//    const Uint32 blinkInterval = 500;

    //Load media
    if(Mix_PlayingMusic() == 0) Mix_PlayMusic( bg_music, -1 );

    SDL_Texture *bg = loadTextureFromIMG("bg.jpg", renderer);
    SDL_Texture *p1 = renderText("1 Player", font_name, font_color, optionSize, renderer);
    SDL_Texture *p2 = renderText("2 Player", font_name, font_color, optionSize, renderer);
    SDL_Texture *settings = renderText("Settings", font_name, font_color, optionSize, renderer);
    SDL_Texture *esc = renderText("Exit", font_name, font_color, optionSize, renderer);
    SDL_Texture *menuOptionTextures[4] = {p1, p2, settings, esc};
    SDL_Texture *arrowTexture = loadTextureFromIMG("indicator.png", renderer);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit = true;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 4) % 4; // To ensure positive index
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 4;
                    break;
                case SDLK_RETURN:

                    if (selectedOption == 3) {
                        exit = true; // Exit

                    } else {
                        optionSelected = true;
//                        blinkStartTime = SDL_GetTicks();
                        // Handle the chosen option and proceed to the game
                        // For now, we'll just print the chosen option
                        std::cout << "Chosen option: " << selectedOption << std::endl;

//                                if (ball->status == ball->READY) {
//                                    ball->status = ball->LAUNCH;
//                                }
                    }
                    inMenu = false;
                    break;
                default:
                    break;
            }
        }
    }
    // Clear the screen
    SDL_RenderClear(renderer);


    //Background
    SDL_RenderCopy(renderer, bg, nullptr, nullptr);

    //Title
    renderTexture(font_image_title,
        renderer, SCREEN_WIDTH / 4 - 140, SCREEN_HEIGHT/4 - 150);


    // Render your menu options and arrow here using SDL functions and textures
    for (int i = 0; i < 4; ++i) {
        double optionX = MENU_START_X;
        double optionY = MENU_START_Y + i * MENU_OPTION_SPACING;
        double optionW, optionH;
//        if (optionSelected && i == selectedOption) {
//            Uint32 currentTime = SDL_GetTicks();
//            if ((currentTime - blinkStartTime) % (2 * blinkInterval) < blinkInterval) {
//                continue; // Skip rendering this option for half the interval
//            }
//            SDL_Delay(1000);
//        }
//                SDL_QueryTexture(menuOptionTextures[i], NULL, NULL, &optionW, &optionH);
//                SDL_Rect optionRect = {optionX, optionY, optionW, optionH};
//                SDL_RenderCopy(renderer, menuOptionTextures[i], nullptr, &optionRect);
        renderTexture(menuOptionTextures[i], renderer, optionX, optionY);
    }

    // Render arrow
    int arrowX = MENU_START_X - 75; // Adjust the X-coordinate based on your design
    int arrowY = MENU_START_Y + selectedOption * MENU_OPTION_SPACING;
    SDL_Rect arrowRect = { arrowX, arrowY, 70, 50};
    SDL_RenderCopy(renderer, arrowTexture, nullptr, &arrowRect);

    // Update the screen
    SDL_RenderPresent(renderer);

    // Cleanup and shutdown
    if(!inMenu) {
        for (int i = 0; i < 4; ++i)
            SDL_DestroyTexture(menuOptionTextures[i]);
        SDL_DestroyTexture(arrowTexture);
        SDL_DestroyTexture(bg);
    }
}


void Pong::execute() {
    while (!exit) {
        while(inMenu) menu();
        if(exit) break;
        input();
        update();
        render();
        SDL_Delay(20);
    }
}

void Pong::input() {
    // Handle events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        // Track mouse movement.
        if (event.type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
        }

        // Clicking 'x' or pressing F4.
        if (event.type == SDL_QUIT) {
            exit = true;
        }

        // Pressing a key.
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {

                // Pressing ESC exits from the game.
                case SDLK_ESCAPE: {
                    exit = true;
                    break;
                }
                // Pressing space will launch the ball if it isn't
                // already launched.
                case SDLK_SPACE: {
                    if (ball->status == ball->READY) {
                        ball->status = ball->LAUNCH;
                    }
                    break;
                }
                // Pressing F11 to toggle fullscreen.
                case SDLK_F11: {
                    int flags = SDL_GetWindowFlags(window);
                    if (flags & SDL_WINDOW_FULLSCREEN) {
                        SDL_SetWindowFullscreen(window, 0);
                    } else {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                    }
                    break;
                }
                case SDLK_b: {
                    inMenu = true;
                    break;
                }
                case SDLK_p: {
                    //If there is no music playing
                    if( Mix_PlayingMusic() == 0 )
                    {
                        //Play the music
                        Mix_PlayMusic( bg_music, -1 );
                    }
                    //If music is being played
                    else
                    {
                        //If the music is paused
                        if( Mix_PausedMusic() == 1 )
                        {
                            //Resume the music
                            Mix_ResumeMusic();
                        }
                        //If the music is playing
                        else
                        {
                            //Pause the music
                            Mix_PauseMusic();
                        }
                    }
                    break;
                }
            }
        }
    }
}

// Update game values.
void Pong::update() {
    // Paddle movement.
    right_paddle->set_y(mouse_y);

    if(selectedOption == 1) left_paddle->Player2();
    else left_paddle->AI(ball); // AI paddle movement.

    // Launch ball.
    if (ball->status == ball->READY) {
        return;
    } else if (ball->status == ball->LAUNCH) {
        ball->launch_ball();
        ball->predicted_y = left_paddle->predict(ball);
    }

    // Update ball speed.
    ball->update_speed();


    // Collision.
    if (ball->pad_collides(left_paddle)) {
        ball->bounces_off(left_paddle);
        Mix_PlayChannel(-1, paddle_sound, 0);  // Play collision sound.
    } else if (ball->pad_collides(right_paddle)) {
        ball->bounces_off(right_paddle);
        // Predict ball position on the y-axis.
        ball->predicted_y = left_paddle->predict(ball);
        Mix_PlayChannel(-1, paddle_sound, 0);
    }

    // Upper and bottom walls collision.
    if (ball->wall_collision()) {
        ball->dy *= -1;  // Reverse ball direction on y-axis.
        Mix_PlayChannel(-1, wall_sound, 0);  // Play collision sound.
    }

    // Update ball coordinates.
    ball->x += ball->dx;
    ball->y += ball->dy;

    // Ball goes out / Goal.
    if (ball->x > SCREEN_WIDTH || ball->x < 0) {
        // Change score.
        if (ball->x > SCREEN_WIDTH) {
            left_score++;
            left_score_changed = true;
        } else {
            right_score++;
            right_score_changed = true;
        }
        Mix_PlayChannel(-1, score_sound, 0);
        ball->reset();
    }
}

// Render objects on screen.
void Pong::render() {
    // Clear screen (background color).
    SDL_SetRenderDrawColor(renderer, 26, 51, 122, 255);
    SDL_RenderClear(renderer);

    // Color right background with white.
    SDL_SetRenderDrawColor( renderer, 187, 191, 194, 255 );
    SDL_Rect right_background = { SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT };
    SDL_RenderFillRect( renderer, &right_background );

    // Render filled paddle.
    SDL_Rect lpad = { left_paddle->get_x(),
        left_paddle->get_y(),
        Paddle::WIDTH, Paddle::HEIGHT };
    SDL_RenderFillRect(renderer, &lpad);

    // Render filled paddle.
    SDL_SetRenderDrawColor(renderer, 26, 51, 122, 255);
    SDL_Rect rpad = { right_paddle->get_x(),
        right_paddle->get_y(),
        Paddle::WIDTH, Paddle::HEIGHT };
    SDL_RenderFillRect(renderer, &rpad);

    // Render ball.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect pong_ball = { ball->x, ball->y, ball->LENGTH, ball->LENGTH };
    SDL_RenderFillRect(renderer, &pong_ball);

    // Render scores.
    if (left_score_changed) {
        font_image_left_score = renderText(std::to_string(left_score),
                font_name, font_color, 45, renderer);
        left_score_changed = false;
        if (left_score == 7) {
            font_image_winner = renderText("Player 1 won!",
                    font_name, font_color, 30, renderer);
            font_image_restart = renderText("Press SPACE to restart",
                    font_name, font_color, 20, renderer);
        }
    }
    renderTexture(font_image_left_score,
            renderer, SCREEN_WIDTH * 4 / 10, SCREEN_HEIGHT / 12);


    int score_font_size = 30;
    if (right_score_changed) {
        font_image_right_score = renderText(std::to_string(right_score),
                font_name, font_color, 45, renderer);
        right_score_changed = false;
        if (right_score == 7) {
            font_image_winner = renderText("Player 2 won!",
                    font_name, font_color, 30, renderer);
            font_image_restart = renderText("PRESS SPACE TO RESTART",
                    font_name, font_color, 20, renderer);
        }
    }
    renderTexture(font_image_right_score,
            renderer,
            SCREEN_WIDTH * 6 / 10 - score_font_size/2, SCREEN_HEIGHT/ 12);


    if (left_score == 7) {
       // Align with score.
        renderTexture(font_image_winner,
                renderer, SCREEN_WIDTH * 1 / 10 + 3, SCREEN_HEIGHT / 4);
        renderTexture(font_image_restart,
                renderer, SCREEN_WIDTH * 1 / 10 + 3, SCREEN_HEIGHT / 3);
        if (ball->status == ball->LAUNCHED) {
            left_score = 0;
            right_score = 0;
            left_score_changed = true;
            right_score_changed = true;
        }
    } else if (right_score == 7) {
       // Align with score.
        renderTexture(font_image_winner,
                renderer,
                SCREEN_WIDTH * 6 / 10 - score_font_size/2, SCREEN_HEIGHT / 4);
        renderTexture(font_image_restart,
                renderer,
                SCREEN_WIDTH * 6 / 10 - score_font_size/2, SCREEN_HEIGHT / 3);
        if (ball->status == ball->LAUNCHED) {
            left_score = 0;
            right_score = 0;
            left_score_changed = true;
            right_score_changed = true;
        }
    } else if (ball->status == ball->READY) {
        // Draw "Press SPACE to start".
        renderTexture(font_image_launch,
                renderer, SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT - 150);
    }

    // Swap buffers.
    SDL_RenderPresent(renderer);
}


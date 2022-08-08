#ifndef CONFIG_H
#define CONFIG_H

#include <SDL2/SDL.h>

/* clang-format off */
/* Physics engine */
static const int GROUND_WIDTH			= 432;
static const int GROUND_HALF_WIDTH		= GROUND_WIDTH / 2;
static const int PLAYER_LENGTH			= 64;
static const int PLAYER_HALF_LENGTH		= PLAYER_LENGTH / 2;
static const int PLAYER_TOUCHING_GROUND_Y_COORD = 244;
static const int BALL_RADIUS			= 20;
static const int BALL_TOUCHING_GROUND_Y_COORD	= 252;
static const int NET_PILLAR_HALF_WIDTH		= 25;
static const int NET_PILLAR_TOP_TOP_Y_COORD	= 176;
static const int NET_PILLAR_TOP_BOTTOM_Y_COORD	= 192;

static const int INFINITE_LOOP_LIMIT		= 100;

static const int PLAYER_INITIAL_OFFSET		= 16;
static const int BALL_INITIAL_OFFSET		= 56;
static const int PLAYER_NORMAL_VELOCITY		= 6;
static const int PLAYER_DIVING_VELOCITY		= 6;
static const int PLAYER_JUMP_VELOCITY		= 16;
static const int PLAYER_LYING_DOWN_DURATION	= 3;

/* Render constants */
static const int FPS                      = 30;
static const int SLOW_MOTION_FPS          = 5;
static const int SLOW_MOTION_FRAME_NUMBER = 6;
static const int DEFAULT_WINNING_SCORE    = 15;

static const int DEFAULT_SCREEN_WIDTH  = 432;
static const int DEFAULT_SCREEN_HEIGHT = 304;

static const SDL_Keycode PLAYER1_KEY_LEFT     = SDLK_d;
static const SDL_Keycode PLAYER1_KEY_RIGHT    = SDLK_g;
static const SDL_Keycode PLAYER1_KEY_UP       = SDLK_r;
static const SDL_Keycode PLAYER1_KEY_DOWN     = SDLK_f;
static const SDL_Keycode PLAYER1_KEY_POWERHIT = SDLK_z;

static const SDL_Keycode PLAYER2_KEY_LEFT     = SDLK_l;
static const SDL_Keycode PLAYER2_KEY_RIGHT    = SDLK_QUOTE;
static const SDL_Keycode PLAYER2_KEY_UP       = SDLK_p;
static const SDL_Keycode PLAYER2_KEY_DOWN     = SDLK_SEMICOLON;
static const SDL_Keycode PLAYER2_KEY_POWERHIT = SDLK_m;

static const int FRAME_TOTAL_INTRO                     = 165;
static const int FRAME_TOTAL_NO_INPUT                  = 255;
static const int FRAME_TOTAL_AFTER_MENU_SELECTION      = 15;
static const int FRAME_TOTAL_BEFORE_START_OF_NEW_GAME  = 15;
static const int FRAME_TOTAL_START_OF_NEW_GAME         = 71;
static const int FRAME_TOTAL_AFTER_END_OF_ROUND        = 5;
static const int FRAME_TOTAL_BEFORE_START_OF_NEXT_ROUND = 30;
static const int FRAME_TOTAL_GAME_END                  = 211;

static const int NUM_OF_CLOUD = 10;

/* clang-format on */

#endif /* CONFIG_H */

#ifndef VIEW_H
#define VIEW_H

#include "cloud.h"
#include "wave.h"
#include "pikachu.h"
#include "pokeball.h"

#include <SDL2/SDL.h>

struct PikaView {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;

	struct PikaCloud **clouds;
	struct PikaWave *wave;

	int screenWidth;
	int screenHeight;
	int canvasWidth;
	int canvasHeight;

	int xOffset;
	int yOffset;
	float xFactor;
	float yFactor;

	int frameCounter;
	int paused;
};

struct PikaView *NewPikaView(void);
void DestroyPikaView(struct PikaView *view);

void DrawIntro(struct PikaView *view);
void DrawMenu(struct PikaView *view, int selectedWithWho);
void DrawBlackBoarder(struct PikaView *view);
void DrawBackground(struct PikaView *v);
void DrawPikachu(struct PikaView *v, struct Pikachu *p);
void DrawBall(struct PikaView *v, struct Pokeball *b);
void DrawScore(struct PikaView *v, int p1Score, int p2Score);
void DrawGameStartMessage(struct PikaView *v);
void DrawReadyMessage(struct PikaView *v);
void DrawGameEndMessage(struct PikaView *v);
void DrawBlackCover(struct PikaView *v, int alpha);

#endif /* VIEW_H */

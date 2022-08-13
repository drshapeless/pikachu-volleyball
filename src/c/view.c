#include "view.h"
#include "config.h"
#include "sprite.h"
#include "cloud.h"
#include "wave.h"
#include "pikachu.h"
#include "pokeball.h"

#include <stdlib.h>
#include <stdio.h>

void initializeWindowParameters(struct PikaView *view)
{
	view->screenWidth = DEFAULT_SCREEN_WIDTH;
	view->screenHeight = DEFAULT_SCREEN_HEIGHT;
	view->canvasWidth = DEFAULT_SCREEN_WIDTH;
	view->canvasHeight = DEFAULT_SCREEN_HEIGHT;

	view->xOffset = 0;
	view->yOffset = 0;
	view->xFactor = 1;
	view->yFactor = 1;

	view->frameCounter = 0;
	view->paused = 0;
}

struct PikaView *NewPikaView()
{
	SDL_Window *window =
		SDL_CreateWindow("Pikachu Volleyball", SDL_WINDOWPOS_UNDEFINED,
				 SDL_WINDOWPOS_UNDEFINED, DEFAULT_SCREEN_WIDTH,
				 DEFAULT_SCREEN_HEIGHT,
				 SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n",
		       SDL_GetError());
		return NULL;
	}

	SDL_Renderer *renderer =
		SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		printf("Renderer could not be created! SDL_Error: %s\n",
		       SDL_GetError());
		return NULL;
	}

	SDL_Texture *texture = NewPikaSpriteSheetTexture(renderer);
	if (texture == NULL) {
		/* The error message is done in sprite. */
		return NULL;
	}

	/* Clouds and wave should always be initialized successfully.
	 * If not, crash the game. */
	struct PikaCloud **clouds =
		malloc(sizeof(struct PikaCloud *) * NUM_OF_CLOUD);
	for (int i = 0; i < NUM_OF_CLOUD; i++) {
		clouds[i] = NewPikaCloud();
	}

	struct PikaWave *wave = NewPikaWave();

	struct PikaView *view = malloc(sizeof(struct PikaView));
	view->window = window;
	view->renderer = renderer;
	view->texture = texture;
	view->clouds = clouds;
	view->wave = wave;

	initializeWindowParameters(view);
	return view;
}

void DestroyPikaView(struct PikaView *view)
{
	SDL_DestroyTexture(view->texture);
	SDL_DestroyRenderer(view->renderer);
	SDL_DestroyWindow(view->window);
	free(view);
}

void scaleRect(struct PikaView *v, SDL_Rect *r)
{
	r->w *= v->xFactor;
	r->h *= v->yFactor;
	r->x = r->x * v->xFactor + v->xOffset;
	r->y = r->y * v->yFactor + v->yOffset;
}

void centerRect(SDL_Rect *r)
{
	/* Make the anchor at the center. */
	r->x -= r->w / 2;
	r->y -= r->h / 2;
}

void flexDraw(struct PikaView *v, SDL_Rect src, SDL_Rect dest)
{
	scaleRect(v, &dest);
	SDL_RenderCopy(v->renderer, v->texture, &src, &dest);
}

void flexDrawFlip(struct PikaView *v, SDL_Rect src, SDL_Rect dest,
		  SDL_RendererFlip flip)
{
	scaleRect(v, &dest);
	SDL_RenderCopyEx(v->renderer, v->texture, &src, &dest, 0, NULL, flip);
}

void plainDraw(struct PikaView *v, SDL_Rect src, SDL_Rect dest)
{
	SDL_RenderCopy(v->renderer, v->texture, &src, &dest);
}

void setAlpha(struct PikaView *v, int alpha)
{
	SDL_SetTextureAlphaMod(v->texture, alpha);
}

void resetAlpha(struct PikaView *v)
{
	SDL_SetTextureAlphaMod(v->texture, 255);
}

void DrawIntro(struct PikaView *v)
{
	int alpha = 0;
	if (v->frameCounter < 128) {
		alpha = v->frameCounter * 2;
	} else {
		int temp = 255 - (v->frameCounter - 128) * 5;
		alpha = temp > 0 ? temp : 0;
	}

	SDL_Rect src = PikaTextureRect[ja_mark];
	SDL_Rect des = src;

	des.x = DEFAULT_SCREEN_WIDTH / 2;
	des.y = DEFAULT_SCREEN_WIDTH / 2;

	centerRect(&des);
	setAlpha(v, alpha);
	flexDraw(v, src, des);
	resetAlpha(v);
}

void drawFightMessage(struct PikaView *v)
{
	const int sizeArray[] = { 20, 22, 25, 27, 30, 27, 25, 22, 20 };

	SDL_Rect src = PikaTextureRect[ja_fight];
	int w = src.w;
	int h = src.h;

	SDL_Rect des = src;
	des.x = 100;
	des.y = 70;

	if (v->frameCounter < 30) {
		int halfWidth = v->frameCounter * w / 30 / 2;
		int halfHeight = v->frameCounter * h / 30 / 2;

		des.w = halfWidth * 2;
		des.h = halfHeight * 2;
	} else {
		int index = (v->frameCounter + 1) % 9;

		int halfWidth = sizeArray[index] * w / 30 / 2;
		int halfHeight = sizeArray[index] * h / 30 / 2;
		des.w = halfWidth * 2;
		des.h = halfHeight * 2;
	}

	centerRect(&des);
	flexDraw(v, src, des);
}

void drawSachisoft(struct PikaView *v)
{
	if (v->frameCounter < 70) {
		return;
	}

	SDL_Rect src = PikaTextureRect[common_sachisoft];
	SDL_Rect des = src;
	des.x = 216 - src.w / 2;
	des.y = 264;

	flexDraw(v, src, des);
}

void drawSittingPikachuTiles(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[sitting_pikachu];
	SDL_Rect des = src;

	int displacement = v->frameCounter * 2 % src.h;
	int x = -displacement;
	int y = -displacement;

	for (int j = 0; j < DEFAULT_SCREEN_HEIGHT / src.h + 2; j++) {
		for (int i = 0; i < DEFAULT_SCREEN_WIDTH / src.w + 2; i++) {
			des.x = x + i * src.w;
			des.y = y + j * src.h;
			flexDraw(v, src, des);
		}
	}
}

void drawPikachuVolleyballMessage(struct PikaView *v)
{
	int f = v->frameCounter;
	if (f <= 30)
		return;

	SDL_Rect src = PikaTextureRect[ja_pikachu_volleyball];
	SDL_Rect des = src;

	des.x = 140;
	des.y = 80;

	if (f <= 44) {
		des.x += 195 - 15 * (f - 30);
	} else if (f <= 55) {
		/* No need to test for f > 44 in an else statement. */
		des.w = 200 - 15 * (f - 44);
	} else if (f <= 71) {
		des.w = 40 + 15 * (f - 55);
	}

	flexDraw(v, src, des);
}

void drawPokemonMessage(struct PikaView *v)
{
	if (v->frameCounter <= 71) {
		return;
	}

	SDL_Rect src = PikaTextureRect[ja_pokemon];
	SDL_Rect des = src;
	des.x = 170;
	des.y = 40;

	flexDraw(v, src, des);
}

void drawWithComputerMessage(struct PikaView *v, int zoom)
{
	/* Disable fancy enlargement effect. */
	SDL_Rect src = PikaTextureRect[ja_with_computer];
	SDL_Rect des = src;
	des.x = 216 - src.w / 2;
	des.y = 184;

	if (zoom) {
		des.x -= 12;
		des.y -= 10;
		des.w += 24;
		des.h += 20;
	}
	flexDraw(v, src, des);
}

void drawWithFriendMessage(struct PikaView *v, int zoom)
{
	SDL_Rect src = PikaTextureRect[ja_with_friend];
	SDL_Rect des = src;
	des.x = 216 - src.w / 2;
	des.y = 214;

	if (zoom) {
		des.x -= 12;
		des.y -= 10;
		des.w += 24;
		des.h += 20;
	}
	flexDraw(v, src, des);
}

void drawWithWhoMessage(struct PikaView *v, int selectedWithWho)
{
	if (v->frameCounter <= 70)
		return;

	switch (selectedWithWho) {
	case -1:
		drawWithComputerMessage(v, 0);
		drawWithFriendMessage(v, 0);
		break;
	case 0:
		drawWithComputerMessage(v, 1);
		drawWithFriendMessage(v, 0);
		break;
	case 1:
		drawWithComputerMessage(v, 0);
		drawWithFriendMessage(v, 1);
		break;
	}
}

void DrawMenu(struct PikaView *v, int selectedWithWho)
{
	drawSittingPikachuTiles(v);
	drawSachisoft(v);
	drawFightMessage(v);
	drawPikachuVolleyballMessage(v);
	drawPokemonMessage(v);
	drawWithWhoMessage(v, selectedWithWho);
}

void DrawBlackBoarder(struct PikaView *v)
{
	SDL_SetRenderDrawColor(v->renderer, 0, 0, 0, 255);
	if (v->screenHeight > v->canvasHeight) {
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = v->canvasWidth;
		r.h = (v->screenHeight - v->canvasHeight) / 2;
		SDL_RenderFillRect(v->renderer, &r);

		r.y = v->yOffset + v->canvasHeight;
		SDL_RenderFillRect(v->renderer, &r);
	}

	if (v->screenWidth > v->canvasWidth) {
		SDL_Rect r;
		r.x = 0;
		r.y = 0;
		r.w = (v->screenWidth - v->canvasWidth) / 2;
		r.h = v->screenHeight;
		SDL_RenderFillRect(v->renderer, &r);

		r.x = v->xOffset + v->canvasWidth;
		SDL_RenderFillRect(v->renderer, &r);
	}
}

void drawSky(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[sky_blue];
	SDL_Rect des = src;
	for (int j = 0; j < 12; j++) {
		for (int i = 0; i < 432 / 16; i++) {
			des.x = 16 * i;
			des.y = 16 * j;
			flexDraw(v, src, des);
		}
	}
}

void drawMountain(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[mountain];
	SDL_Rect des = src;
	des.x = 0;
	des.y = 188;
	flexDraw(v, src, des);
}

void drawGroundRed(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[ground_red];
	SDL_Rect des = src;
	des.y = 248;
	for (int i = 0; i < 432 / 16; i++) {
		des.x = 16 * i;
		flexDraw(v, src, des);
	}
}

void drawGroundLine(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[ground_line];
	SDL_Rect des = src;
	des.y = 264;
	for (int i = 0; i < 432 / 16; i++) {
		des.x = 16 * i;
		flexDraw(v, src, des);
	}

	src = PikaTextureRect[ground_line_leftmost];
	des = src;
	des.x = 0;
	des.y = 264;
	flexDraw(v, src, des);

	src = PikaTextureRect[ground_line_rightmost];
	des = src;
	des.x = 432 - 16;
	des.y = 264;
	flexDraw(v, src, des);
}

void drawGroundYellow(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[ground_yellow];
	SDL_Rect des = src;
	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 432 / 16; i++) {
			des.x = 16 * i;
			des.y = 280 + 16 * j;
			flexDraw(v, src, des);
		}
	}
}

void drawNetPillar(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[net_pillar_top];
	SDL_Rect des = src;
	des.x = 213;
	des.y = 176;
	flexDraw(v, src, des);

	src = PikaTextureRect[net_pillar];
	des = src;
	des.x = 213;
	for (int j = 0; j < 12; j++) {
		des.y = 184 + 8 * j;
		flexDraw(v, src, des);
	}
}

void drawCloud(struct PikaView *v)
{
	CloudEngine(v->clouds, NUM_OF_CLOUD);

	SDL_Rect src = PikaTextureRect[cloud];
	SDL_Rect des = src;

	for (int i = 0; i < NUM_OF_CLOUD; i++) {
		struct PikaCloud *cloud = v->clouds[i];
		des.x = PikaCloudSpriteTopLeftPointX(cloud);
		des.y = PikaCloudSpriteTopLeftPointY(cloud);
		des.w = PikaCloudSpriteWidth(cloud);
		des.h = PikaCloudSpriteHeight(cloud);
		flexDraw(v, src, des);
	}
}

void drawWave(struct PikaView *v)
{
	WaveEngine(v->wave);
	SDL_Rect src = PikaTextureRect[wave];
	SDL_Rect des = src;

	for (int i = 0; i < 432 / 16; i++) {
		des.x = 16 * i;
		des.y = v->wave->yCoords[i];
		flexDraw(v, src, des);
	}
}

void DrawBackground(struct PikaView *v)
{
	drawSky(v);
	drawMountain(v);
	drawGroundRed(v);
	drawGroundLine(v);
	drawGroundYellow(v);
	drawNetPillar(v);
	drawCloud(v);
	drawWave(v);
}

void DrawGameStartMessage(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[ja_game_start];
	SDL_Rect des = src;
	int halfWidth = src.w * v->frameCounter / 50;
	int halfHeight = src.h * v->frameCounter / 50;
	des.x = 216 - halfWidth;
	des.y = 50 + 2 * halfHeight;
	des.w = 2 * halfWidth;
	des.h = 2 * halfHeight;

	flexDraw(v, src, des);
}

int getFrameNumberForPlayerAnimatedSprite(int state, int frameNumber)
{
	/* I have not looked into this function seriously. */
	if (state < 4) {
		return 5 * state + frameNumber;
	} else if (state == 4) {
		return 17 + frameNumber;

	} else if (state > 4) {
		return 18 + 5 * (state - 5) + frameNumber;
	}

	return 0;
}

void DrawPikachu(struct PikaView *v, struct Pikachu *p)
{
	const SDL_Rect *pikachuRect = PikachuRect(p->state, p->frameNumber);
	if (pikachuRect == NULL) {
		/* In some weird case, this function will call a
		 * non-existing state and frame number.
		 *
		 * Resulting in segmentation fault.
		 *
		 * For now, just skip the frame. */

		/* TODO: Fix. */
		return;
	}
	SDL_Rect src = *pikachuRect;
	SDL_Rect des = src;
	des.x = p->x - src.w / 2;
	des.y = p->y - src.h / 2;

	/* No flip for p1, flip for p2. */
	SDL_RendererFlip flip = p->isPlayer2;
	if (p->state == 3 || p->state == 4) {
		flip = p->divingDirection == -1 ? SDL_FLIP_HORIZONTAL : 0;
	}
	flexDrawFlip(v, src, des, flip);

	src = PikaTextureRect[shadow];
	des = src;
	des.y = SHADOW_Y_COORD;
	des.x = p->x - src.w / 2;
	flexDraw(v, src, des);
}

void DrawBall(struct PikaView *v, struct Pokeball *b)
{
	const SDL_Rect *ballRect = BallRect(b->rotation);
	SDL_Rect src = *ballRect;
	SDL_Rect des = src;

	des.x = b->x;
	des.y = b->y;
	centerRect(&des);
	flexDraw(v, src, des);

	/* Shadow */
	src = PikaTextureRect[shadow];
	des = src;
	des.y = SHADOW_Y_COORD;
	des.x = b->x;
	flexDraw(v, src, des);

	if (b->punchEffectRadius > 0) {
		src = PikaTextureRect[ball_punch];
		b->punchEffectRadius -= 2;
		des.w = 2 * b->punchEffectRadius;
		des.h = 2 * b->punchEffectRadius;
		des.x = b->punchEffectX;
		des.y = b->punchEffectY;
		centerRect(&des);
		flexDraw(v, src, des);
	}

	if (b->isPowerHit) {
		/* Hyper ball */
		src = PikaTextureRect[ball_hyper];
		des = src;
		des.x = b->previousX;
		des.y = b->previousY;
		centerRect(&des);
		flexDraw(v, src, des);

		/* Trailing effect */
		src = PikaTextureRect[ball_trail];
		des = src;
		des.x = b->previousPreviousX;
		des.y = b->previousPreviousY;
		centerRect(&des);
		flexDraw(v, src, des);
	}
}

void DrawScore(struct PikaView *v, int p1Score, int p2Score)
{
	SDL_Rect src;
	SDL_Rect des;

	/* p1 score */
	int xoff = 14;
	int yoff = 10;
	if (p1Score >= 10) {
		/* Although the ten should never exceed 1. */
		src = *NumberRect(p1Score / 10);
		des = src;
		des.x = xoff;
		des.y = yoff;
		flexDraw(v, src, des);
	}

	src = *NumberRect(p1Score % 10);
	des = src;
	des.x = xoff + 32;
	des.y = yoff;
	flexDraw(v, src, des);

	/* p2 score */
	xoff = 432 - 32 - 32 - 14;
	yoff = 10;
	if (p2Score >= 10) {
		src = *NumberRect(p2Score / 10);
		des = src;
		des.x = xoff;
		des.y = yoff;
		flexDraw(v, src, des);
	}
	src = *NumberRect(p2Score % 10);
	des = src;
	des.x = xoff + 32;
	des.y = yoff;
	flexDraw(v, src, des);
}

void DrawReadyMessage(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[common_ready];
	SDL_Rect des = src;
	des.x = 176;
	des.y = 38;

	flexDraw(v, src, des);
}

void DrawGameEndMessage(struct PikaView *v)
{
	SDL_Rect src = PikaTextureRect[common_game_end];
	SDL_Rect des = src;

	if (v->frameCounter < 50) {
		int halfWidthIncrement =
			2 * (50 - v->frameCounter) * des.w / 50;
		int halfHeightIncrement =
			2 * (50 - v->frameCounter) * des.h / 50;

		des.x = 216 - des.w / 2 - halfWidthIncrement;
		des.y = 50 - halfHeightIncrement;
		des.w += 2 * halfWidthIncrement;
		des.h += 2 * halfHeightIncrement;
	} else {
		des.x = 216 - des.w / 2;
		des.y = 50;
	}

	flexDraw(v, src, des);
}

void DrawBlackCover(struct PikaView *v, int alpha)
{
	setAlpha(v, alpha);
	SDL_Rect src = PikaTextureRect[black];
	SDL_Rect des = src;
	des.x = v->xOffset;
	des.y = v->yOffset;
	des.w = v->canvasWidth;
	des.h = v->canvasHeight;
	SDL_RenderCopy(v->renderer, v->texture, &src, &des);
	resetAlpha(v);
}

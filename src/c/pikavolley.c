#include "pikavolley.h"
#include "view.h"
#include "config.h"
#include "sound.h"
#include "keyboard.h"
#include "userinput.h"

#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

void windowResizeCallback(struct PikaVolley *pikaVolley);
void loop(struct PikaVolley *v, int *running);
void processEvent(struct PikaVolley *v, int *running);
void playSoundEffects(struct PikaVolley *v);

int initializeView()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n",
		       SDL_GetError());
		return -1;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not initialize! SDL_image Error: %s\n",
		       IMG_GetError());
		return -1;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -1;
	}

	return 0;
}

struct PikaVolley *NewPikaVolley(void)
{
	if (initializeView() < 0) {
		return NULL;
	}

	struct PikaView *view = NewPikaView();
	if (view == NULL) {
		printf("Failed to create PikaView\n");
		return NULL;
	}

	struct PikaAudio *audio = NewPikaAudio();
	if (audio == NULL) {
		printf("Failed to create PikaAudio\n");
		return NULL;
	}

	struct PikaGame *game = NewPikaGame(1, 1);
	if (game == NULL) {
		printf("Failed to create PikaGame\n");
		return NULL;
	}

	struct PikaKeyboard **keyboards =
		malloc(sizeof(struct PikaKeyboard *) * 2);
	keyboards[0] = NewPikaKeyboard();
	keyboards[1] = NewPikaKeyboard();

	struct PikaUserInput **inputs =
		malloc(sizeof(struct PikaUserInput *) * 2);
	inputs[0] = NewPikaUserInput();
	inputs[1] = NewPikaUserInput();

	struct PikaVolley *v = malloc(sizeof(struct PikaVolley));
	v->view = view;
	v->audio = audio;
	v->game = game;
	v->keyboards = keyboards;
	v->inputs = inputs;

	v->scene = 0;
	v->gameEnded = 0;
	v->roundEnded = 0;
	v->isPlayer2Serve = 0;
	v->p1Score = 0;
	v->p2Score = 0;
	v->selectedWithWho = 0;
	v->noInputFrameCounter = 0;

	return v;
}

void DestroyPikaVolley(struct PikaVolley *volley)
{
	DestroyPikaView(volley->view);
	DestroyPikaAudio(volley->audio);
	DestroyPikaGame(volley->game);
	DestroyPikaKeyboard(volley->keyboards[0]);
	DestroyPikaKeyboard(volley->keyboards[1]);
	DestroyPikaUserInput(volley->inputs[0]);
	DestroyPikaUserInput(volley->inputs[1]);
	free(volley->keyboards);
	free(volley->inputs);

	free(volley);
}

void StartPikaVolley(struct PikaVolley *v)
{
	int running = 1;
	int lastTick = 0;
	while (running) {
		int current = SDL_GetTicks();
		if (current - lastTick < 1000 / FPS) {
			continue;
		}
		lastTick = current;
		loop(v, &running);
	}
}

void runIntro(struct PikaVolley *v)
{
	DrawIntro(v->view);
	v->view->frameCounter++;

	if (v->view->frameCounter >= FRAME_TOTAL_INTRO) {
		v->scene = MENU_SCENE;
		v->view->frameCounter = 0.;

	} else if (v->keyboards[0]->powerHitKey ||
		   v->keyboards[1]->powerHitKey) {
		v->scene = MENU_SCENE;
		v->view->frameCounter = 0;
	}
}

void runMenu(struct PikaVolley *v)
{
	DrawMenu(v->view, v->selectedWithWho);
	v->view->frameCounter++;

	if (v->view->frameCounter < 71) {
		if (v->keyboards[0]->powerHitKey ||
		    v->keyboards[1]->powerHitKey) {
			v->view->frameCounter = 71;
		}
		return;
	}

	if (v->noInputFrameCounter >= FRAME_TOTAL_NO_INPUT) {
		v->game->p1->isComputer = 1;
		v->game->p2->isComputer = 1;
		v->view->frameCounter = 0;
		v->noInputFrameCounter = 0;
		v->scene = AFTER_MENU_SELECTION_SCENE;
	}

	if (v->keyboards[0]->downKey || v->keyboards[1]->downKey) {
		v->selectedWithWho = 1;
		v->noInputFrameCounter = 0;
	} else if (v->keyboards[0]->upKey || v->keyboards[1]->upKey) {
		v->selectedWithWho = 0;
		v->noInputFrameCounter = 0;
	} else if (v->keyboards[0]->powerHitKey ||
		   v->keyboards[1]->powerHitKey) {
		if (v->selectedWithWho == 1) {
			v->game->p1->isComputer = 0;
			v->game->p2->isComputer = 0;
		} else if (v->selectedWithWho == 0) {
			if (v->keyboards[0]->powerHitKey) {
				v->game->p1->isComputer = 0;
				v->game->p2->isComputer = 1;
			} else {
				v->game->p1->isComputer = 1;
				v->game->p2->isComputer = 0;
			}
		}
		v->scene = AFTER_MENU_SELECTION_SCENE;
		v->view->frameCounter = 0;
	} else {
		v->noInputFrameCounter++;
	}
}

void runAfterMenuSelection(struct PikaVolley *v)
{
	if (v->view->frameCounter >= FRAME_TOTAL_AFTER_MENU_SELECTION) {
		v->scene = BEFORE_START_OF_NEW_GAME_SCENE;
		v->view->frameCounter = 0;
		return;
	} else {
		DrawMenu(v->view, v->selectedWithWho);
		int alpha = 255 * ((float)v->view->frameCounter /
				   FRAME_TOTAL_AFTER_MENU_SELECTION);
		/* TODO: Draw fade in fade out effect. */
	}
	v->view->frameCounter++;
}

void runBeforeStartOfNewGame(struct PikaVolley *v)
{
	DrawBackground(v->view);
	DrawPikachu(v->view, v->game->p1);
	DrawPikachu(v->view, v->game->p2);
	DrawBall(v->view, v->game->ball);
	int alpha = 255 - (float)255 * ((float)v->view->frameCounter /
					FRAME_TOTAL_BEFORE_START_OF_NEW_GAME);
	DrawBlackCover(v->view, alpha);

	if (v->view->frameCounter >= FRAME_TOTAL_BEFORE_START_OF_NEW_GAME) {
		v->scene = START_OF_NEW_GAME_SCENE;
		v->view->frameCounter = 0;
		return;
	}

	v->view->frameCounter++;
}

void runStartOfNewGame(struct PikaVolley *v)
{
	if (v->view->frameCounter == 0) {
		v->gameEnded = 0;
		v->roundEnded = 0;
		v->isPlayer2Serve = 0;
		v->game->p1->gameEnded = 0;
		v->game->p1->isWinner = 0;
		v->game->p2->gameEnded = 0;
		v->game->p2->isWinner = 0;
		v->p1Score = 0;
		v->p2Score = 0;

		InitializePikachu(v->game->p1);
		InitializePikachu(v->game->p2);
		InitializePokeball(v->game->ball, v->isPlayer2Serve);

		if (Mix_PlayingMusic() == 0) {
			Mix_PlayMusic(v->audio->bgm, -1);
		}
	}

	DrawBackground(v->view);
	DrawPikachu(v->view, v->game->p1);
	DrawPikachu(v->view, v->game->p2);
	DrawBall(v->view, v->game->ball);
	DrawScore(v->view, v->p1Score, v->p2Score);

	if (v->view->frameCounter >= FRAME_TOTAL_START_OF_NEW_GAME) {
		v->view->frameCounter = 0;
		v->scene = ROUND_SCENE;
		return;
	}
	v->view->frameCounter++;
}

void runRound(struct PikaVolley *v)
{
	int pressedPowerHit = v->keyboards[0]->powerHitKey ||
			      v->keyboards[1]->powerHitKey;

	if (v->game->p1->isComputer && v->game->p2->isComputer &&
	    pressedPowerHit) {
		v->scene = INTRO_SCENE;
		v->view->frameCounter = 0;
		return;
	}

	KeyboardToUserInput(v->keyboards[0], v->inputs[0]);
	KeyboardToUserInput(v->keyboards[1], v->inputs[1]);

	int isBallTouchingGround =
		PikaGameTick(v->game, v->inputs[0], v->inputs[1]);

	playSoundEffects(v);
	DrawBackground(v->view);
	DrawPikachu(v->view, v->game->p1);
	DrawPikachu(v->view, v->game->p2);
	DrawBall(v->view, v->game->ball);
	DrawScore(v->view, v->p1Score, v->p2Score);

	if (isBallTouchingGround && v->roundEnded == 0 && v->gameEnded == 0) {
		if (v->game->ball->punchEffectX < GROUND_HALF_WIDTH) {
			v->isPlayer2Serve = 1;
			v->p2Score += 1;
			if (v->p2Score >= DEFAULT_WINNING_SCORE) {
				v->gameEnded = 1;
				v->game->p1->isWinner = 0;
				v->game->p2->isWinner = 1;
				v->game->p1->gameEnded = 1;
				v->game->p2->gameEnded = 1;
			}
		} else {
			v->isPlayer2Serve = 0;
			v->p1Score += 1;
			if (v->p1Score >= DEFAULT_WINNING_SCORE) {
				v->gameEnded = 1;
				v->game->p1->isWinner = 1;
				v->game->p2->isWinner = 0;
				v->game->p1->gameEnded = 1;
				v->game->p2->gameEnded = 1;
			}
		}
		v->roundEnded = 1;
	}

	if (v->gameEnded) {
		DrawGameEndMessage(v->view);
		v->view->frameCounter++;
		if (v->view->frameCounter >= FRAME_TOTAL_GAME_END ||
		    (v->view->frameCounter >= 70 && pressedPowerHit)) {
			v->view->frameCounter = 0;
			v->scene = INTRO_SCENE;
		}
		return;
	}

	if (v->roundEnded) {
		v->scene = AFTER_END_OF_ROUND_SCENE;
		/* Reset frameCounter here? */
		v->view->frameCounter = 0;
		return;
	}

	v->view->frameCounter++;
}

void runAfterEndOfRound(struct PikaVolley *v)
{
	DrawBackground(v->view);
	DrawPikachu(v->view, v->game->p1);
	DrawPikachu(v->view, v->game->p2);
	DrawBall(v->view, v->game->ball);
	DrawScore(v->view, v->p1Score, v->p2Score);

	int alpha = (float)255 * ((float)v->view->frameCounter /
				  FRAME_TOTAL_AFTER_END_OF_ROUND);
	DrawBlackCover(v->view, alpha);

	if (v->view->frameCounter >= FRAME_TOTAL_AFTER_END_OF_ROUND) {
		v->view->frameCounter = 0;
		v->scene = BEFORE_START_OF_NEXT_ROUND_SCENE;
		return;
	}

	v->view->frameCounter++;
}

void runBeforeStartOfNextRound(struct PikaVolley *v)
{
	if (v->view->frameCounter == 0) {
		DrawBackground(v->view);
		DrawPikachu(v->view, v->game->p1);
		DrawPikachu(v->view, v->game->p2);
		DrawBall(v->view, v->game->ball);
		DrawReadyMessage(v->view);
		InitializePikachu(v->game->p1);
		InitializePikachu(v->game->p2);
		InitializePokeball(v->game->ball, v->isPlayer2Serve);
	}

	DrawBackground(v->view);
	DrawPikachu(v->view, v->game->p1);
	DrawPikachu(v->view, v->game->p2);
	DrawBall(v->view, v->game->ball);
	int alpha = 255 - (float)255 * ((float)v->view->frameCounter /
					FRAME_TOTAL_BEFORE_START_OF_NEXT_ROUND);
	DrawBlackCover(v->view, alpha);

	if (v->view->frameCounter >= FRAME_TOTAL_BEFORE_START_OF_NEXT_ROUND) {
		v->view->frameCounter = 0;
		v->roundEnded = 0;
		v->scene = ROUND_SCENE;
		return;
	}

	v->view->frameCounter++;
}

void runScenes(struct PikaVolley *v)
{
	SDL_RenderClear(v->view->renderer);

	switch (v->scene) {
	case INTRO_SCENE:
		runIntro(v);
		break;
	case MENU_SCENE:
		runMenu(v);
		break;
	case AFTER_MENU_SELECTION_SCENE:
		runAfterMenuSelection(v);
		break;
	case BEFORE_START_OF_NEW_GAME_SCENE:
		runBeforeStartOfNewGame(v);
		break;
	case START_OF_NEW_GAME_SCENE:
		runStartOfNewGame(v);
		break;
	case ROUND_SCENE:
		runRound(v);
		break;
	case AFTER_END_OF_ROUND_SCENE:
		runAfterEndOfRound(v);
		break;
	case BEFORE_START_OF_NEXT_ROUND_SCENE:
		runBeforeStartOfNextRound(v);
		break;
	}

	DrawBlackBoarder(v->view);

	SDL_RenderPresent(v->view->renderer);
}

void loop(struct PikaVolley *v, int *running)
{
	processEvent(v, running);
	runScenes(v);
}

void processKeys(SDL_Keycode k, struct PikaKeyboard **keyboards, int isDown)
{
	switch (k) {
	case PLAYER1_KEY_LEFT:
		keyboards[0]->leftKey = isDown;
		break;
	case PLAYER1_KEY_RIGHT:
		keyboards[0]->rightKey = isDown;
		break;
	case PLAYER1_KEY_UP:
		keyboards[0]->upKey = isDown;
		break;
	case PLAYER1_KEY_DOWN:
		keyboards[0]->downKey = isDown;
		break;
	case PLAYER1_KEY_POWERHIT:
		keyboards[0]->powerHitKey = isDown;
		break;
	case PLAYER2_KEY_LEFT:
		keyboards[1]->leftKey = isDown;
		break;
	case PLAYER2_KEY_RIGHT:
		keyboards[1]->rightKey = isDown;
		break;
	case PLAYER2_KEY_UP:
		keyboards[1]->upKey = isDown;
		break;
	case PLAYER2_KEY_DOWN:
		keyboards[1]->downKey = isDown;
		break;
	case PLAYER2_KEY_POWERHIT:
		keyboards[1]->powerHitKey = isDown;
		break;
	}
}

void processEvent(struct PikaVolley *v, int *running)
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			*running = 0;
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				windowResizeCallback(v);
			}
			break;
		case SDL_KEYDOWN:
			processKeys(e.key.keysym.sym, v->keyboards, 1);
			break;
		case SDL_KEYUP:
			processKeys(e.key.keysym.sym, v->keyboards, 0);
			break;
		}
	}
}

void windowResizeCallback(struct PikaVolley *v)
{
	SDL_GetWindowSize(v->view->window, &v->view->screenWidth,
			  &v->view->screenHeight);
	float originalScale =
		(float)DEFAULT_SCREEN_WIDTH / DEFAULT_SCREEN_HEIGHT;
	float scale = (float)v->view->screenWidth / v->view->screenHeight;
	if (scale > originalScale) {
		/* Wide screen */
		v->view->canvasWidth = v->view->screenHeight * originalScale;
		v->view->canvasHeight = v->view->screenHeight;
		v->view->xOffset =
			(v->view->screenWidth - v->view->canvasWidth) / 2;
		v->view->yOffset = 0;
	} else {
		/* A tall screen */
		v->view->canvasWidth = v->view->screenWidth;
		v->view->canvasHeight = v->view->screenWidth / originalScale;
		v->view->xOffset = 0;
		v->view->yOffset =
			(v->view->screenHeight - v->view->canvasHeight) / 2;
	}

	v->view->xFactor = (float)v->view->canvasWidth / DEFAULT_SCREEN_WIDTH;
	v->view->yFactor = (float)v->view->canvasHeight / DEFAULT_SCREEN_HEIGHT;
}

void tick(struct PikaVolley *v)
{
}

void playSound(Mix_Chunk *chunk, int leftCenterRight)
{
	/* -1: both channels. */
	/*  0: left */
	/*  1: right */
	if (leftCenterRight == -1) {
		Mix_PlayChannel(0, chunk, 0);
		Mix_PlayChannel(1, chunk, 0);
		return;
	} else {
		Mix_PlayChannel(leftCenterRight, chunk, 0);
		Mix_PlayChannel(leftCenterRight, chunk, 0);
	}
}

void playPikachuSound(struct PikaAudio *a, struct Pikachu *p)
{
	int leftOrRight = p->isPlayer2;
	if (p->soundPipikachu) {
		playSound(a->pipikachu, leftOrRight);
		p->soundPipikachu = 0;
	}

	if (p->soundPika) {
		playSound(a->pika, leftOrRight);
		p->soundPika = 0;
	}

	if (p->soundChu) {
		playSound(a->chu, leftOrRight);
		p->soundChu = 0;
	}
}

void playBallSound(struct PikaAudio *a, struct Pokeball *b)
{
	int leftOfRight = -1;
	if (b->punchEffectX < GROUND_HALF_WIDTH) {
		leftOfRight = 0;
	} else {
		leftOfRight = 1;
	}

	if (b->soundPowerHit) {
		playSound(a->powerhit, leftOfRight);
		b->soundPowerHit = 0;
	}

	if (b->soundBallTouchesGround) {
		playSound(a->ballTouchesGround, leftOfRight);
		b->soundBallTouchesGround = 0;
	}
}

void playSoundEffects(struct PikaVolley *v)
{
	playPikachuSound(v->audio, v->game->p1);
	playPikachuSound(v->audio, v->game->p2);
	playBallSound(v->audio, v->game->ball);
}

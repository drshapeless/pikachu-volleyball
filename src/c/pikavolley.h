#ifndef PIKAVOLLEY_H
#define PIKAVOLLEY_H

#include "view.h"
#include "sound.h"
#include "game.h"
#include "userinput.h"
#include "keyboard.h"

enum scene_name {
	INTRO_SCENE,
	MENU_SCENE,
	AFTER_MENU_SELECTION_SCENE,
	BEFORE_START_OF_NEW_GAME_SCENE,
	START_OF_NEW_GAME_SCENE,
	ROUND_SCENE,
	AFTER_END_OF_ROUND_SCENE,
	BEFORE_START_OF_NEXT_ROUND_SCENE,
};

/* This is where everything comes together. */
struct PikaVolley {
	struct PikaView *view;
	struct PikaAudio *audio;
	struct PikaGame *game;

	/* There are only two of these. */
	struct PikaKeyboard **keyboards;
	struct PikaUserInput **inputs;

	/* Refers to scene_name enum */
	int scene;
	int gameEnded;
	int roundEnded;
	int isPlayer2Serve;
	int p1Score;
	int p2Score;
	int selectedWithWho;
	int noInputFrameCounter;
};

struct PikaVolley *NewPikaVolley(void);
void DestroyPikaVolley(struct PikaVolley *volley);
void StartPikaVolley(struct PikaVolley *volley);

void loop(struct PikaVolley *v, int *running);

#endif /* PIKAVOLLEY_H */

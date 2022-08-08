#include "keyboard.h"

#include <stdlib.h>

struct PikaKeyboard *NewPikaKeyboard(void)
{
	struct PikaKeyboard *k = malloc(sizeof(struct PikaKeyboard));
	k->powerHitKeyWasDown = 0;
	k->leftKey = 0;
	k->rightKey = 0;
	k->upKey = 0;
	k->downKey = 0;
	k->powerHitKey = 0;

	return k;
}

void DestroyPikaKeyboard(struct PikaKeyboard *keyboard)
{
	free(keyboard);
}

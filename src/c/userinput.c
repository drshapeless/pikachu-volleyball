#include "userinput.h"

#include <stdlib.h>

struct PikaUserInput *NewPikaUserInput()
{
	struct PikaUserInput *i = malloc(sizeof(struct PikaUserInput));
	i->xDirection = 0;
	i->yDirection = 0;
	i->powerHit = 0;
	return i;
}

void DestroyPikaUserInput(struct PikaUserInput *input)
{
	free(input);
}

void KeyboardToUserInput(struct PikaKeyboard *k, struct PikaUserInput *i)
{
	if (k->leftKey) {
		i->xDirection = -1;
	} else if (k->rightKey) {
		i->xDirection = 1;
	} else {
		i->xDirection = 0;
	}

	if (k->upKey) {
		i->yDirection = -1;
	} else if (k->downKey) {
		i->yDirection = 1;
	} else {
		i->yDirection = 0;
	}

	if (!k->powerHitKeyWasDown && k->powerHitKey) {
		i->powerHit = 1;
		k->powerHitKeyWasDown = 1;
	} else {
		i->powerHit = 0;
		k->powerHitKeyWasDown = 0;
	}
}

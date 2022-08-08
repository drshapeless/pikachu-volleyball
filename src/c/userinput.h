#ifndef USERINPUT_H
#define USERINPUT_H

#include "keyboard.h"

struct PikaUserInput {
	int xDirection;
	int yDirection;
	int powerHit;
};

struct PikaUserInput *NewPikaUserInput();
void DestroyPikaUserInput(struct PikaUserInput *input);

void KeyboardToUserInput(struct PikaKeyboard *keyboard,
			 struct PikaUserInput *input);

#endif /* USERINPUT_H */

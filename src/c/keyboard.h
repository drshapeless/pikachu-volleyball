#ifndef KEYBOARD_H
#define KEYBOARD_H

struct PikaKeyboard {
	int powerHitKeyWasDown;
	int leftKey;
	int rightKey;
	int upKey;
	int downKey;
	int powerHitKey;
};

struct PikaKeyboard *NewPikaKeyboard(void);
void DestroyPikaKeyboard(struct PikaKeyboard *keyboard);

#endif /* KEYBOARD_H */

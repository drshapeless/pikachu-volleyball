#ifndef GAME_H
#define GAME_H

#include "pikachu.h"
#include "pokeball.h"
#include "userinput.h"

struct PikaGame {
	struct Pikachu *p1;
	struct Pikachu *p2;
	struct Pokeball *ball;
};

struct PikaGame *NewPikaGame(int isP1Computer, int isP2Computer);
void DestroyPikaGame(struct PikaGame *game);

int PikaGameTick(struct PikaGame *game, struct PikaUserInput *p1Input,
		 struct PikaUserInput *p2Input);

#endif /* GAME_H */

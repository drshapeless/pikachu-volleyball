#include "pikachu.h"
#include "config.h"

#include <stdlib.h>

void InitializePikachu(struct Pikachu *p)
{
	if (p->isPlayer2) {
		p->x = GROUND_WIDTH - PLAYER_HALF_LENGTH;
	} else {
		p->x = PLAYER_HALF_LENGTH;
	}

	p->y = PLAYER_TOUCHING_GROUND_Y_COORD;
	p->yVelocity = 0;
	p->isCollisionWithBallHappened = 0;

	p->state = 0;
	p->frameNumber = 0;
	p->normalStatusArmSwingDirection = 1;
	p->delayBeforeNextFrame = 0;

	p->divingDirection = 0;
	p->lyingDownDurationLeft = -1;
	p->computerWhereToStandBy = 0;

	p->computerBoldness = rand() % 5;

	p->soundPipikachu = 0;
	p->soundPika = 0;
	p->soundChu = 0;
}

struct Pikachu *NewPikachu(int isPlayer2, int isComputer)
{
	struct Pikachu *p = malloc(sizeof(struct Pikachu));
	p->isPlayer2 = isPlayer2;
	p->isComputer = isComputer;
	InitializePikachu(p);

	p->isWinner = 0;
	p->gameEnded = 0;

	return p;
}

void DestroyPikachu(struct Pikachu *p)
{
	free(p);
}

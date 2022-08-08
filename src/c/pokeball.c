#include "pokeball.h"
#include "config.h"

#include <stdlib.h>

void InitializePokeball(struct Pokeball *b, int isPlayer2Serve)
{
	if (isPlayer2Serve) {
		b->x = GROUND_WIDTH - BALL_INITIAL_OFFSET;
	} else {
		b->x = BALL_INITIAL_OFFSET;
	}

	b->y = BALL_RADIUS;
	b->xVelocity = 0;
	b->yVelocity = 1;
	b->punchEffectRadius = 0;
	b->isPowerHit = 0;

	b->expectedLandingPointX = 0;
	b->rotation = 0;
	b->fineRotation = 0;
	b->punchEffectX = 0;
	b->punchEffectY = 0;

	b->previousX = 0;
	b->previousPreviousX = 0;
	b->previousY = 0;
	b->previousPreviousY = 0;

	b->soundPowerHit = 0;
	b->soundBallTouchesGround = 0;
}

struct Pokeball *NewPokeball(int isPlayer2Serve)
{
	struct Pokeball *b = malloc(sizeof(struct Pokeball));

	InitializePokeball(b, isPlayer2Serve);

	return b;
}

void DestroyPokeball(struct Pokeball *b)
{
	free(b);
}

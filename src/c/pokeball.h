#ifndef POKEBALL_H
#define POKEBALL_H

struct Pokeball {
	int expectedLandingPointX;
	/**
	 * ball rotation frame number selector
	 * During the period where it continues to be 5, hyper ball glitch occur.
	 * */
	int rotation;
	int fineRotation;
	int punchEffectX;
	int punchEffectY;

	/* Trailing effect */
	int previousX;
	int previousPreviousX;
	int previousY;
	int previousPreviousY;

	int soundPowerHit;
	int soundBallTouchesGround;

	int x;
	int y;
	int xVelocity;
	int yVelocity;
	int punchEffectRadius;
	int isPowerHit;
};

void InitializePokeball(struct Pokeball *ball, int isPlayer2Serve);
struct Pokeball *NewPokeball(int isPlayer2Serve);
void DestroyPokeball(struct Pokeball *ball);

#endif /* POKEBALL_H */

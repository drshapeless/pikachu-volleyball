#include "game.h"
#include "pikachu.h"
#include "pokeball.h"
#include "config.h"

#include <stdlib.h>

int processBall(struct Pokeball *ball);
void setBallPreviousPosition(struct Pokeball *ball);
void setBallNextPosition(struct Pokeball *ball);
void setBallRotation(struct Pokeball *ball);
int touchGround(struct Pokeball *ball);
int touchCeiling(struct Pokeball *ball);
int touchWall(struct Pokeball *ball);
int touchNet(struct Pokeball *ball);
void movePlayer(struct Pikachu *p, struct PikaUserInput *i);
int isBallTouchPlayer(struct Pokeball *ball, int playerX, int playerY);
void ballPlayerCollision(struct Pokeball *b, int playerX,
			 struct PikaUserInput *i, int playerState);

struct PikaGame *NewPikaGame(int isP1Computer, int isP2Computer)
{
	struct PikaGame *g = malloc(sizeof(struct PikaGame));
	g->p1 = NewPikachu(0, isP1Computer);
	g->p2 = NewPikachu(1, isP2Computer);
	/* p1 always serve first. */
	g->ball = NewPokeball(0);

	return g;
}

void DestroyPikaGame(struct PikaGame *g)
{
	DestroyPikachu(g->p1);
	DestroyPikachu(g->p2);
	DestroyPokeball(g->ball);
	free(g);
}

int PikaGameTick(struct PikaGame *game, struct PikaUserInput *p1Input,
		 struct PikaUserInput *p2Input)
{
	/* Returns whether the ball touches the ground. */
	if (processBall(game->ball)) {
		return 1;
	}

	movePlayer(game->p1, p1Input);
	movePlayer(game->p2, p2Input);
	game->p1->isCollisionWithBallHappened = 0;
	game->p2->isCollisionWithBallHappened = 0;
	if (isBallTouchPlayer(game->ball, game->p1->x, game->p1->y)) {
		ballPlayerCollision(game->ball, game->p1->x, p1Input,
				    game->p1->state);
		game->p1->isCollisionWithBallHappened = 1;
	} else if (isBallTouchPlayer(game->ball, game->p2->x, game->p2->y)) {
		ballPlayerCollision(game->ball, game->p2->x, p2Input,
				    game->p2->state);
		game->p2->isCollisionWithBallHappened = 1;
	}

	return 0;
}

int processBall(struct Pokeball *b)
{
	setBallPreviousPosition(b);
	setBallRotation(b);
	if (touchGround(b)) {
		/* If the ball touches the ground, ignore everything
		 * else. */
		return 1;
	} else if (!(touchCeiling(b) || touchWall(b) || touchNet(b))) {
		setBallNextPosition(b);
	}

	/* Gravity */
	b->yVelocity += 1;

	return 0;
}

void setBallPreviousPosition(struct Pokeball *b)
{
	b->previousPreviousX = b->previousX;
	b->previousPreviousY = b->previousY;
	b->previousX = b->x;
	b->previousY = b->y;
}

void setBallRotation(struct Pokeball *b)
{
	/* This is all about rendering. */
	int futureFineRotation = b->fineRotation + (b->xVelocity / 2);
	if (futureFineRotation < 0) {
		futureFineRotation += 50;
	} else if (futureFineRotation > 50) {
		futureFineRotation += -50;
	}
	b->fineRotation = futureFineRotation;
	b->rotation = b->fineRotation / 10;
}

void setBallNextPosition(struct Pokeball *b)
{
	b->x += b->xVelocity;
	b->y += b->yVelocity;
}

int touchGround(struct Pokeball *b)
{
	/* If the ball touches the ground, return 1 and
	 * set the ball to the next position. */

	if (b->y + b->yVelocity > BALL_TOUCHING_GROUND_Y_COORD) {
		b->x += b->xVelocity;
		b->y = BALL_TOUCHING_GROUND_Y_COORD;
		b->yVelocity = -b->yVelocity;

		b->soundBallTouchesGround = 1;
		b->punchEffectX = b->x;
		b->punchEffectRadius = BALL_RADIUS;
		b->punchEffectY = BALL_TOUCHING_GROUND_Y_COORD + BALL_RADIUS;

		return 1;
	}
	return 0;
}

int touchCeiling(struct Pokeball *b)
{
	/* If the ball touches the ceiling, return 1 and
	 * set the ball to the next position. */

	if (b->y + b->yVelocity < BALL_RADIUS) {
		b->x += b->xVelocity;
		b->y = BALL_RADIUS;
		b->yVelocity = 1;
		return 1;
	}

	return 0;
}

int touchWall(struct Pokeball *b)
{
	int futureX = b->x + b->xVelocity;
	if (futureX < BALL_RADIUS) {
		b->x = BALL_RADIUS;
		b->y += b->yVelocity;
		b->xVelocity = -b->xVelocity;
		return 1;
	} else if (futureX > GROUND_WIDTH - BALL_RADIUS) {
		b->x = GROUND_WIDTH - BALL_RADIUS;
		b->y += b->yVelocity;
		b->xVelocity = -b->xVelocity;
		return 1;
	}
	return 0;
}

int touchNet(struct Pokeball *b)
{
	int futureX = b->x + b->xVelocity;
	int futureY = b->y + b->yVelocity;

	if (futureY > NET_PILLAR_TOP_TOP_Y_COORD &&
	    futureX > GROUND_HALF_WIDTH - NET_PILLAR_HALF_WIDTH &&
	    futureX < GROUND_HALF_WIDTH + NET_PILLAR_HALF_WIDTH) {
		if (b->y < NET_PILLAR_TOP_TOP_Y_COORD) {
			/* Hit from top side. */
			b->y = NET_PILLAR_TOP_TOP_Y_COORD;
			b->x += b->xVelocity;
			b->yVelocity = -b->yVelocity;
			return 1;
		} else if (b->x < GROUND_HALF_WIDTH - NET_PILLAR_HALF_WIDTH) {
			/* Hit from left side. */
			b->x = GROUND_HALF_WIDTH - NET_PILLAR_HALF_WIDTH;
			b->y += b->yVelocity;
			b->xVelocity = -b->xVelocity;
			return 1;
		} else if (b->x > GROUND_HALF_WIDTH + NET_PILLAR_HALF_WIDTH) {
			/* Hit from the right side. */
			b->x = GROUND_HALF_WIDTH + NET_PILLAR_HALF_WIDTH;
			b->y += b->yVelocity;
			b->xVelocity = -b->xVelocity;
			return 1;
		}
	}

	return 0;
}

void processGameEndFrameFor(struct Pikachu *p)
{
	if (p->gameEnded && p->frameNumber < 4) {
		p->delayBeforeNextFrame += 1;
		if (p->delayBeforeNextFrame > 4) {
			p->delayBeforeNextFrame = 0;
			p->frameNumber += 1;
		}
	}
}

void movePlayer(struct Pikachu *p, struct PikaUserInput *i)
{
	/* If pikachu is lying down, return. */
	if (p->state == LYING_DOWN_PIKACHU) {
		p->lyingDownDurationLeft -= 1;
		if (p->lyingDownDurationLeft < -1) {
			p->state = NORMAL_PIKACHU;
		}
		return;
	}

	/* Velocity X */
	int vx = 0;
	if (p->state < DIVING_PIKACHU) {
		vx = i->xDirection * PLAYER_NORMAL_VELOCITY;
	} else {
		vx = p->divingDirection * PLAYER_DIVING_VELOCITY;
	}

	/* Future player x position. */
	int fpx = p->x + vx;
	p->x = fpx;
	if (p->isPlayer2) {
		if (fpx < GROUND_HALF_WIDTH + PLAYER_HALF_LENGTH) {
			p->x = GROUND_HALF_WIDTH + PLAYER_HALF_LENGTH;
		} else if (fpx >= GROUND_WIDTH - PLAYER_HALF_LENGTH) {
			p->x = GROUND_WIDTH - PLAYER_HALF_LENGTH;
		}
	} else {
		if (fpx < PLAYER_HALF_LENGTH) {
			p->x = PLAYER_HALF_LENGTH;
		} else if (fpx >= GROUND_HALF_WIDTH - PLAYER_HALF_LENGTH) {
			p->x = GROUND_HALF_WIDTH - PLAYER_HALF_LENGTH;
		}
	}

	/* Jump */
	if (p->state < DIVING_PIKACHU && i->yDirection == -1 &&
	    p->y == PLAYER_TOUCHING_GROUND_Y_COORD) {
		p->yVelocity = -PLAYER_JUMP_VELOCITY;
		p->state = JUMPING_PIKACHU;
		p->frameNumber = 0;
		p->soundChu = 1;
	}

	/* Gravity */
	int fpy = p->y + p->yVelocity;
	p->y = fpy;
	if (fpy < PLAYER_TOUCHING_GROUND_Y_COORD) {
		p->yVelocity += 1;
	} else if (fpy > PLAYER_TOUCHING_GROUND_Y_COORD) {
		p->yVelocity = 0;
		p->y = PLAYER_TOUCHING_GROUND_Y_COORD;
		p->frameNumber = 0;
		if (p->state == DIVING_PIKACHU) {
			p->state = LYING_DOWN_PIKACHU;
			p->frameNumber = 0;
			p->lyingDownDurationLeft = PLAYER_LYING_DOWN_DURATION;
		} else {
			p->state = NORMAL_PIKACHU;
		}
	}

	/* Diving or powerhit */
	if (i->powerHit == 1) {
		if (p->state == JUMPING_PIKACHU) {
			p->delayBeforeNextFrame = 5;
			p->frameNumber = 0;
			p->state = JUMPING_AND_POWERHITTING_PIKACHU;
			p->soundPika = 1;
		} else if (p->state == NORMAL_PIKACHU && i->xDirection != 0) {
			p->state = DIVING_PIKACHU;
			p->frameNumber = 0;
			p->divingDirection = i->xDirection;
			p->yVelocity = -PLAYER_DIVING_UPWARD_VELOCITY;
			p->soundChu = 1;
		}
	}

	if (p->state == JUMPING_PIKACHU) {
		p->frameNumber = (p->frameNumber + 1) % 3;

	} else if (p->state == JUMPING_AND_POWERHITTING_PIKACHU) {
		if (p->delayBeforeNextFrame < 1) {
			p->frameNumber += 1;
			if (p->frameNumber > 4) {
				p->frameNumber = 0;
				p->state = JUMPING_PIKACHU;
			}
		} else {
			p->delayBeforeNextFrame -= 1;
		}
	} else if (p->state == NORMAL_PIKACHU) {
		p->delayBeforeNextFrame += 1;
		if (p->delayBeforeNextFrame > 3) {
			p->delayBeforeNextFrame = 0;
			int futureFrameNumber =
				p->frameNumber +
				p->normalStatusArmSwingDirection;
			if (futureFrameNumber < 0 || futureFrameNumber > 4) {
				p->normalStatusArmSwingDirection =
					-p->normalStatusArmSwingDirection;
			}
			p->frameNumber += p->normalStatusArmSwingDirection;
		}
	}

	if (p->gameEnded) {
		if (p->state == NORMAL_PIKACHU) {
			if (p->isWinner) {
				p->state = WIN_PIKACHU;
				p->soundPipikachu = 1;
			} else {
				p->state = LOSE_PIKACHU;
			}
			p->delayBeforeNextFrame = 0;
			p->frameNumber = 0;
		}
		processGameEndFrameFor(p);
	}
}

int isBallTouchPlayer(struct Pokeball *b, int playerX, int playerY)
{
	int diff = b->x - playerX;
	if (abs(diff) <= PLAYER_HALF_LENGTH) {
		diff = b->y - playerY;
		if (abs(diff) <= PLAYER_HALF_LENGTH) {
			return 1;
		}
	}

	return 0;
}

void ballPlayerCollision(struct Pokeball *b, int playerX,
			 struct PikaUserInput *i, int playerState)
{
	/* playerX is pikachu's x position */
	/* if collision occur, */
	/* greater the x position difference between pika and ball, */
	/* greater the x velocity of the ball. */
	if (b->x < playerX) {
		b->xVelocity = -(abs(b->x - playerX) / 3);
	} else if (b->x > playerX) {
		b->xVelocity = abs(b->x - playerX) / 3;
	}

	/* If ball velocity x is 0, randomly choose one of -1, 0, 1. */
	if (b->xVelocity == 0) {
		b->xVelocity = (rand() % 3) - 1;
	}

	int ballAbsYVelocity = abs(b->yVelocity);
	b->yVelocity = -ballAbsYVelocity;

	if (ballAbsYVelocity < 15) {
		b->yVelocity = -15;
	}

	/* If player is jumping and power hitting */
	if (playerState == JUMPING_AND_POWERHITTING_PIKACHU) {
		if (b->x < GROUND_HALF_WIDTH) {
			b->xVelocity = (abs(i->xDirection) + 1) * 10;
		} else {
			b->xVelocity = -(abs(i->xDirection) + 1) * 10;
		}
		b->punchEffectX = b->x;
		b->punchEffectY = b->y;

		b->yVelocity = abs(b->yVelocity) * i->yDirection * 2;
		b->punchEffectRadius = BALL_RADIUS;
		b->soundPowerHit = 1;
		b->isPowerHit = 1;
	} else {
		b->isPowerHit = 0;
	}
}

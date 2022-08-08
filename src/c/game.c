#include "game.h"
#include "pikachu.h"
#include "pokeball.h"
#include "config.h"

#include <stdlib.h>

int processCollisionBetweenBallAndWorldAndSetBallPosition(struct Pokeball *ball);
int isCollisionBetweenBallAndPlayerHappened(struct Pokeball *b, int playerX,
					    int playerY);
void calculateExpectedLandingPointXFor(struct Pokeball *ball);
void processPlayerMovementAndSetPlayerPosition(struct Pikachu *player,
					       struct PikaUserInput *userInput,
					       struct Pokeball *ball);
int decideWhetherInputPowerHit(struct Pikachu *player, struct Pokeball *ball,
			       struct Pikachu *otherPlayer,
			       struct PikaUserInput *userInput);
int expectedLandingPointXWhenPowerHit(int userInputXDirection,
				      int userInputYDirection,
				      struct Pokeball *ball);
void processCollisionBetweenBallAndPlayer(struct Pokeball *b, int playerX,
					  struct PikaUserInput *i,
					  int playerState);
void letComputerDecideUserInput(struct Pikachu *player, struct Pokeball *ball,
				struct Pikachu *otherPlayer,
				struct PikaUserInput *userInput);

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

int PikaGameTick(struct PikaGame *g, struct PikaUserInput *i1,
		 struct PikaUserInput *i2)
{
	int isBallTouchingGround =
		processCollisionBetweenBallAndWorldAndSetBallPosition(g->ball);
	if (isBallTouchingGround) {
		return 1;
	}

	if (g->p1->isComputer) {
		letComputerDecideUserInput(g->p1, g->ball, g->p2, i1);
	}
	if (g->p2->isComputer) {
		letComputerDecideUserInput(g->p2, g->ball, g->p1, i2);
	}
	processPlayerMovementAndSetPlayerPosition(g->p1, i1, g->ball);
	processPlayerMovementAndSetPlayerPosition(g->p2, i2, g->ball);

	if (isCollisionBetweenBallAndPlayerHappened(g->ball, g->p1->x,
						    g->p1->y)) {
		if (g->p1->isCollisionWithBallHappened == 0) {
			processCollisionBetweenBallAndPlayer(g->ball, g->p1->x,
							     i1, g->p1->state);
			g->p1->isCollisionWithBallHappened = 1;
		}
	} else {
		g->p1->isCollisionWithBallHappened = 0;
	}

	if (isCollisionBetweenBallAndPlayerHappened(g->ball, g->p2->x,
						    g->p2->y)) {
		if (g->p2->isCollisionWithBallHappened == 0) {
			processCollisionBetweenBallAndPlayer(g->ball, g->p2->x,
							     i2, g->p2->state);
			g->p2->isCollisionWithBallHappened = 1;
		}
	} else {
		g->p2->isCollisionWithBallHappened = 0;
	}

	return isBallTouchingGround;
}

int isCollisionBetweenBallAndPlayerHappened(struct Pokeball *b, int playerX,
					    int playerY)
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

void processCollisionBetweenBallAndWall(struct Pokeball *b, int futureX)
{
	/* The original author make a weird mistake.
	 * (futureBallX > GROUND_WIDTH) instead of below.

	 * gorisanson said editing it will introduce an infinite loop
	 * issue.
	 *
	 * Infinite loop is real. */

	/* If the ball hits the wall, reverse the x-velocity. */
	if (futureX < BALL_RADIUS || futureX > GROUND_WIDTH - BALL_RADIUS) {
		b->xVelocity = -b->xVelocity;
	}
}

void processCollisionBetweenBallAndCeiling(struct Pokeball *b, int futureY)
{
	/* If the ball hits the ceiling, let gravity do the work. */
	if (futureY < BALL_RADIUS) {
		b->yVelocity = 1;
	}
}

void processCollisionBetweenBallAndNet(struct Pokeball *b, int futureX)
{
	/* If the ball hit the net. */
	if (abs(futureX - GROUND_HALF_WIDTH) < NET_PILLAR_HALF_WIDTH &&
	    b->y > NET_PILLAR_TOP_TOP_Y_COORD) {
		if (b->y <= NET_PILLAR_TOP_BOTTOM_Y_COORD && b->yVelocity > 0) {
			/* If the ball hit the top part,
			 * and is falling,
			 * bounce up. */
			b->yVelocity = -b->yVelocity;
		} else {
			/* If the ball hit the other part of the
			 * net, bounce back. */
			b->xVelocity = -b->xVelocity;
		}
	}
}

int processCollisionBetweenBallAndGround(struct Pokeball *b, int futureY)
{
	/* If the ball hit the ground, bounce up. */
	if (futureY > BALL_TOUCHING_GROUND_Y_COORD) {
		b->yVelocity = -b->yVelocity;
		return 1;
	}
	return 0;
}

int processCollisionBetweenBallAndWorldAndSetBallPosition(struct Pokeball *b)
{
	b->previousPreviousX = b->previousX;
	b->previousPreviousY = b->previousY;
	b->previousX = b->x;
	b->previousY = b->y;

	/* I don't fucking know what a fine rotation means. */
	int futureFineRotation = b->fineRotation + (b->xVelocity / 2);
	if (futureFineRotation < 0) {
		futureFineRotation += 50;
	} else if (futureFineRotation > 50) {
		futureFineRotation += -50;
	}
	b->fineRotation = futureFineRotation;
	b->rotation = b->fineRotation / 10;

	int futureBallX = b->x + b->xVelocity;
	int futureBallY = b->y + b->yVelocity;

	processCollisionBetweenBallAndWall(b, futureBallX);

	processCollisionBetweenBallAndCeiling(b, futureBallY);

	processCollisionBetweenBallAndNet(b, futureBallX);

	if (processCollisionBetweenBallAndGround(b, futureBallY)) {
		b->soundBallTouchesGround = 1;
		b->punchEffectX = b->x;
		b->y = BALL_TOUCHING_GROUND_Y_COORD;
		b->punchEffectRadius = BALL_RADIUS;
		b->punchEffectY = BALL_TOUCHING_GROUND_Y_COORD + BALL_RADIUS;
		return 1;
	}

	b->x = futureBallX;
	b->y = futureBallY;
	/* Gravity acceleration. */
	b->yVelocity += 1;
	return 0;
}

void calculateExpectedLandingPointXFor(struct Pokeball *ball)
{
	struct Pokeball b = *ball;

	/* Infinite loop is real, fuck that. */
	int loopCounter = 0;
	while (1) {
		loopCounter++;
		int futureX = b.xVelocity + b.x;
		int futureY = b.yVelocity + b.y;
		processCollisionBetweenBallAndWall(&b, futureX);

		processCollisionBetweenBallAndCeiling(&b, futureY);

		processCollisionBetweenBallAndNet(&b, futureX);

		if (processCollisionBetweenBallAndGround(&b, futureY) ||
		    loopCounter >= INFINITE_LOOP_LIMIT) {
			break;
		}
		b.x = futureX;
		b.y = futureY;
	}
	ball->expectedLandingPointX = b.x;
}

void setPlayerJump(struct Pikachu *p, struct PikaUserInput *i)
{
	if (i->yDirection == -1 && p->y == PLAYER_TOUCHING_GROUND_Y_COORD) {
		p->yVelocity = -PLAYER_JUMP_VELOCITY;
		p->state = 1;
		p->frameNumber = 0;
		p->soundChu = 1;
	}
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

void processPlayerGravity(struct Pikachu *p)
{
	/* Gravity */
	int futurePlayerY = p->y + p->yVelocity;
	p->y = futurePlayerY;
	/* If futurePlayerY == PLAYER_TOUCHING_GROUND_Y_COORD,
		 * do nothing. */
	if (futurePlayerY < PLAYER_TOUCHING_GROUND_Y_COORD) {
		/* Player is falling */
		p->yVelocity += 1;

	} else if (futurePlayerY > PLAYER_TOUCHING_GROUND_Y_COORD) {
		/* Player is landing */
		p->yVelocity = 0;
		p->y = PLAYER_TOUCHING_GROUND_Y_COORD;
		p->frameNumber = 0;

		if (p->state == 3) {
			p->state = 4;
			p->frameNumber = 0;
			p->lyingDownDurationLeft = PLAYER_LYING_DOWN_DURATION;
		} else {
			p->state = 0;
		}
	}
}

/* void processPlayerMovementAndSetPlayerPosition(struct Pikachu *p, */
/*					       struct PikaUserInput *i, */
/*					       struct Pokeball *b) */
/* { */
/*	/\* This function has been completely refactored. */
/*	 * The original JavaScript version is an if-else hell. */
/*	 * The same condition is tested multiple times */
/*	 * unnecessarily. *\/ */
/*	int playerVelocityX = 0; */
/*	switch (p->state) { */
/*	case 0: /\* normal *\/ */
/*		playerVelocityX = i->xDirection * PLAYER_NORMAL_VELOCITY; */
/*		/\* Player can only jump at normal state. *\/ */
/*		setPlayerJump(p, i); */
/*		/\* This can be written as i->xDirection, but compare */
/*		 * it for clarity because xDirection can be -1, 0, or */
/*		 * 1, not a boolean value. *\/ */
/*		if (i->powerHit && i->xDirection != 0) { */
/*			/\* Diving *\/ */
/*			p->state = 3; */
/*			p->frameNumber = 0; */
/*			p->divingDirection = i->xDirection; */
/*			p->yVelocity = -5; */
/*			p->soundChu = 1; */
/*			/\* If don't return here, Pikachu will move up */
/*			 * after diving. */
/*			 * */
/*			 * I have no idea why diving have a -5 */
/*			 * increase in yVelocity. *\/ */
/*		} */

/*		p->delayBeforeNextFrame += 1; */
/*		if (p->delayBeforeNextFrame > 3) { */
/*			p->delayBeforeNextFrame = 0; */
/*			int futureFrameNumber = */
/*				p->frameNumber + */
/*				p->normalStatusArmSwingDirection; */
/*			if (futureFrameNumber < 0 || futureFrameNumber > 4) { */
/*				p->normalStatusArmSwingDirection = */
/*					-p->normalStatusArmSwingDirection; */
/*			} */
/*			p->frameNumber += p->normalStatusArmSwingDirection; */
/*		} */

/*		break; */
/*	case 1: /\* jumping *\/ */
/*		playerVelocityX = i->xDirection * PLAYER_NORMAL_VELOCITY; */
/*		if (i->powerHit) { */
/*			/\* Jumping and powerhit *\/ */
/*			p->delayBeforeNextFrame = 5; */
/*			p->frameNumber = 0; */
/*			p->state = 2; */
/*			p->soundPika = 1; */
/*		} */

/*		p->frameNumber = (p->frameNumber + 1) % 3; */

/*		break; */
/*	case 2: /\* jumping and power hitting *\/ */
/*		playerVelocityX = i->xDirection * PLAYER_NORMAL_VELOCITY; */
/*		if (p->delayBeforeNextFrame < 1) { */
/*			p->frameNumber += 1; */
/*			if (p->frameNumber > 4) { */
/*				p->frameNumber = 0; */
/*				p->state = 1; */
/*			} */
/*		} else { */
/*			p->delayBeforeNextFrame -= 1; */
/*		} */
/*		break; */
/*	case 3: /\* diving *\/ */
/*		playerVelocityX = p->divingDirection * PLAYER_DIVING_VELOCITY; */
/*		p->state = 4; */
/*		p->frameNumber = 0; */
/*		p->lyingDownDurationLeft = PLAYER_LYING_DOWN_DURATION; */
/*		break; */
/*	case 4: /\* lying down after diving *\/ */
/*		p->lyingDownDurationLeft -= 1; */
/*		if (p->lyingDownDurationLeft <= 0) { */
/*			p->state = 0; */
/*		} */
/*		return; */
/*		break; */
/*	} */

/*	/\* Player's x-direction world boundary *\/ */
/*	int futurePlayerX = p->x + playerVelocityX; */
/*	p->x = futurePlayerX; */
/*	if (p->isPlayer2 == 0) { */
/*		/\* player1 *\/ */
/*		if (futurePlayerX < PLAYER_HALF_LENGTH) { */
/*			p->x = PLAYER_HALF_LENGTH; */
/*		} else if (futurePlayerX > */
/*			   GROUND_HALF_WIDTH - PLAYER_HALF_LENGTH) { */
/*			p->x = GROUND_HALF_WIDTH - PLAYER_HALF_LENGTH; */
/*		} */
/*	} else { */
/*		/\* player2 *\/ */
/*		if (futurePlayerX < GROUND_HALF_WIDTH + PLAYER_HALF_LENGTH) { */
/*			p->x = GROUND_HALF_WIDTH + PLAYER_HALF_LENGTH; */
/*		} else if (futurePlayerX > GROUND_WIDTH - PLAYER_HALF_LENGTH) { */
/*			p->x = GROUND_WIDTH - PLAYER_HALF_LENGTH; */
/*		} */
/*	} */

/*	processPlayerGravity(p); */

/*	/\* Game end *\/ */
/*	if (p->gameEnded) { */
/*		if (p->state == 0) { */
/*			if (p->isWinner) { */
/*				p->state = 5; */
/*				p->soundPipikachu = 1; */
/*			} else { */
/*				p->state = 6; */
/*			} */
/*			p->delayBeforeNextFrame = 0; */
/*			p->frameNumber = 0; */
/*		} */
/*		processGameEndFrameFor(p); */
/*	} */
/* } */

void processPlayerMovementAndSetPlayerPosition(struct Pikachu *p,
					       struct PikaUserInput *i,
					       struct Pokeball *b)
{
	/* If pikachu is lying down, return. */
	if (p->state == 4) {
		p->lyingDownDurationLeft -= 1;
		if (p->lyingDownDurationLeft < -1) {
			p->state = 0;
		}
		return;
	}

	int playerVelocityX = 0;
	if (p->state < 5) {
		if (p->state < 3) {
			playerVelocityX =
				i->xDirection * PLAYER_NORMAL_VELOCITY;
		} else {
			playerVelocityX =
				p->divingDirection * PLAYER_DIVING_VELOCITY;
		}
	}

	/* x-direction */
	int futurePlayerX = p->x + playerVelocityX;
	p->x = futurePlayerX;
	if (p->isPlayer2) {
		if (futurePlayerX < GROUND_HALF_WIDTH + PLAYER_HALF_LENGTH) {
			p->x = GROUND_HALF_WIDTH + PLAYER_HALF_LENGTH;
		} else if (futurePlayerX >= GROUND_WIDTH - PLAYER_HALF_LENGTH) {
			p->x = GROUND_WIDTH - PLAYER_HALF_LENGTH;
		}
	} else {
		if (futurePlayerX < PLAYER_HALF_LENGTH) {
			p->x = PLAYER_HALF_LENGTH;
		} else if (futurePlayerX >=
			   GROUND_HALF_WIDTH - PLAYER_HALF_LENGTH) {
			p->x = GROUND_HALF_WIDTH - PLAYER_HALF_LENGTH;
		}
	}

	/* Jump */
	if (p->state < 3 && i->yDirection == -1 &&
	    p->y == PLAYER_TOUCHING_GROUND_Y_COORD) {
		p->yVelocity = -PLAYER_JUMP_VELOCITY;
		p->state = 1;
		p->frameNumber = 0;
		p->soundChu = 1;
	}

	/* Gravity */
	int futurePlayerY = p->y + p->yVelocity;
	p->y = futurePlayerY;
	if (futurePlayerY < PLAYER_TOUCHING_GROUND_Y_COORD) {
		p->yVelocity += 1;
	} else if (futurePlayerY > PLAYER_TOUCHING_GROUND_Y_COORD) {
		p->yVelocity = 0;
		p->y = PLAYER_TOUCHING_GROUND_Y_COORD;
		p->frameNumber = 0;
		if (p->state == 3) {
			p->state = 4;
			p->frameNumber = 0;
			p->lyingDownDurationLeft = PLAYER_LYING_DOWN_DURATION;
		} else {
			p->state = 0;
		}
	}

	/* Diving or powerhit */
	if (i->powerHit == 1) {
		if (p->state == 1) {
			p->delayBeforeNextFrame = 5;
			p->frameNumber = 0;
			p->state = 2;
			p->soundPika = 1;
		} else if (p->state == 0 && i->xDirection != 0) {
			p->state = 3;
			p->frameNumber = 0;
			p->divingDirection = i->xDirection;
			p->yVelocity = -5;
			p->soundChu = 1;
		}
	}

	if (p->state == 1) {
		p->frameNumber = (p->frameNumber + 1) % 3;

	} else if (p->state == 2) {
		if (p->delayBeforeNextFrame < 1) {
			p->frameNumber += 1;
			if (p->frameNumber > 4) {
				p->frameNumber = 0;
				p->state = 1;
			}
		} else {
			p->delayBeforeNextFrame -= 1;
		}
	} else if (p->state == 0) {
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
		if (p->state == 0) {
			if (p->isWinner) {
				p->state = 5;
				p->soundPipikachu = 1;
			} else {
				p->state = 6;
			}
			p->delayBeforeNextFrame = 0;
			p->frameNumber = 0;
		}
		processGameEndFrameFor(p);
	}
}

void processCollisionBetweenBallAndPlayer(struct Pokeball *b, int playerX,
					  struct PikaUserInput *i,
					  int playerState)
{
	/* This function is directly copied. */

	// playerX is pikachu's x position
	// if collision occur,
	// greater the x position difference between pika and ball,
	// greater the x velocity of the ball.
	if (b->x < playerX) {
		b->xVelocity = -(abs(b->x - playerX) / 3);
	} else if (b->x > playerX) {
		b->xVelocity = abs(b->x - playerX) / 3;
	}

	// If ball velocity x is 0, randomly choose one of -1, 0, 1.
	if (b->xVelocity == 0) {
		b->xVelocity = (rand() % 3) - 1;
	}

	int ballAbsYVelocity = abs(b->yVelocity);
	b->yVelocity = -ballAbsYVelocity;

	if (ballAbsYVelocity < 15) {
		b->yVelocity = -15;
	}

	// If player is jumping and power hitting
	if (playerState == 2) {
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

	/* Omit calculate landing point. */
}

/* Computer related functions. */
/* TODO: Write a better computer. */
void letComputerDecideUserInput(struct Pikachu *player, struct Pokeball *ball,
				struct Pikachu *otherPlayer,
				struct PikaUserInput *userInput)
{
	/* Resetting user input */
	userInput->xDirection = 0;
	userInput->yDirection = 0;
	userInput->powerHit = 0;

	/* Calculate here */
	calculateExpectedLandingPointXFor(ball);

	int virtualExpectedLandingPointX = ball->expectedLandingPointX;
	if (abs(ball->x - player->x) > 100 &&
	    abs(ball->xVelocity) < player->computerBoldness + 5) {
		int leftBoundary = player->isPlayer2 * GROUND_HALF_WIDTH;
		if ((ball->expectedLandingPointX <= leftBoundary ||
		     ball->expectedLandingPointX >=
			     player->isPlayer2 * GROUND_WIDTH +
				     GROUND_HALF_WIDTH) &&
		    player->computerWhereToStandBy == 0) {
			// If conditions above met, the computer
			// estimates the proper location to stay as
			// the middle point of their side
			virtualExpectedLandingPointX =
				leftBoundary + (GROUND_HALF_WIDTH / 2);
		}
	}
	if (abs(virtualExpectedLandingPointX - player->x) >
	    player->computerBoldness + 8) {
		if (player->x < virtualExpectedLandingPointX) {
			userInput->xDirection = 1;
		} else {
			userInput->xDirection = -1;
		}
	} else if (rand() % 20 == 0) {
		player->computerWhereToStandBy = rand() % 2;
	}

	if (player->state == 0) {
		if (abs(ball->xVelocity) < player->computerBoldness + 3 &&
		    abs(ball->x - player->x) < PLAYER_HALF_LENGTH &&
		    ball->y > -36 &&
		    ball->y < 10 * player->computerBoldness + 84 &&
		    ball->yVelocity > 0) {
			userInput->yDirection = -1;
		}

		int leftBoundary = player->isPlayer2 * GROUND_HALF_WIDTH;
		int rightBoundary = (player->isPlayer2 + 1) * GROUND_HALF_WIDTH;
		if (ball->expectedLandingPointX > leftBoundary &&
		    ball->expectedLandingPointX < rightBoundary &&
		    abs(ball->x - player->x) >
			    player->computerBoldness * 5 + PLAYER_LENGTH &&
		    ball->x > leftBoundary && ball->x < rightBoundary &&
		    ball->y > 174) {
			// If conditions above met, the computer decides to dive!
			userInput->powerHit = 1;
			if (player->x < ball->x) {
				userInput->xDirection = 1;
			} else {
				userInput->xDirection = -1;
			}
		}
	} else if (player->state == 1 || player->state == 2) {
		if (abs(ball->x - player->x) > 8) {
			if (player->x < ball->x) {
				userInput->xDirection = 1;
			} else {
				userInput->xDirection = -1;
			}
		}
		if (abs(ball->x - player->x) < 48 &&
		    abs(ball->y - player->y) < 48) {
			int willInputPowerHit = decideWhetherInputPowerHit(
				player, ball, otherPlayer, userInput);
			if (willInputPowerHit) {
				userInput->powerHit = 1;
				if (abs(otherPlayer->x - player->x) < 80 &&
				    userInput->yDirection != -1) {
					userInput->yDirection = -1;
				}
			}
		}
	}
}

int decideWhetherInputPowerHit(struct Pikachu *player, struct Pokeball *ball,
			       struct Pikachu *otherPlayer,
			       struct PikaUserInput *userInput)
{
	if (rand() % 2 == 0) {
		for (int xDirection = 1; xDirection > -1; xDirection--) {
			for (int yDirection = -1; yDirection < 2;
			     yDirection++) {
				int expectedLandingPointX =
					expectedLandingPointXWhenPowerHit(
						xDirection, yDirection, ball);
				if ((expectedLandingPointX <=
					     player->isPlayer2 *
						     GROUND_HALF_WIDTH ||
				     expectedLandingPointX >=
					     player->isPlayer2 * GROUND_WIDTH +
						     GROUND_HALF_WIDTH) &&
				    abs(expectedLandingPointX -
					otherPlayer->x) > PLAYER_LENGTH) {
					userInput->xDirection = xDirection;
					userInput->yDirection = yDirection;
					return 1;
				}
			}
		}
	} else {
		for (int xDirection = 1; xDirection > -1; xDirection--) {
			for (int yDirection = 1; yDirection > -2;
			     yDirection--) {
				int expectedLandingPointX =
					expectedLandingPointXWhenPowerHit(
						xDirection, yDirection, ball);
				if ((expectedLandingPointX <=
					     player->isPlayer2 *
						     GROUND_HALF_WIDTH ||
				     expectedLandingPointX >=
					     player->isPlayer2 * GROUND_WIDTH +
						     GROUND_HALF_WIDTH) &&
				    abs(expectedLandingPointX -
					otherPlayer->x) > PLAYER_LENGTH) {
					userInput->xDirection = xDirection;
					userInput->yDirection = yDirection;
					return 1;
				}
			}
		}
	}
	return 0;
}

int expectedLandingPointXWhenPowerHit(int userInputXDirection,
				      int userInputYDirection,
				      struct Pokeball *ball)
{
	struct Pokeball copyBall = *ball;

	if (copyBall.x < GROUND_HALF_WIDTH) {
		copyBall.xVelocity = (abs(userInputXDirection) + 1) * 10;
	} else {
		copyBall.xVelocity = -(abs(userInputXDirection) + 1) * 10;
	}
	copyBall.yVelocity = abs(copyBall.yVelocity) * userInputYDirection * 2;

	int loopCounter = 0;
	while (1) {
		loopCounter++;

		int futureCopyBallX = copyBall.x + copyBall.xVelocity;
		if (futureCopyBallX < BALL_RADIUS ||
		    futureCopyBallX > GROUND_WIDTH) {
			copyBall.xVelocity = -copyBall.xVelocity;
		}
		if (copyBall.y + copyBall.yVelocity < 0) {
			copyBall.yVelocity = 1;
		}
		if (abs(copyBall.x - GROUND_HALF_WIDTH) <
			    NET_PILLAR_HALF_WIDTH &&
		    copyBall.y > NET_PILLAR_TOP_TOP_Y_COORD) {
			if (copyBall.y <= NET_PILLAR_TOP_BOTTOM_Y_COORD) {
				if (copyBall.yVelocity > 0) {
					copyBall.yVelocity =
						-copyBall.yVelocity;
				}
			} else {
				if (copyBall.x < GROUND_HALF_WIDTH) {
					copyBall.xVelocity =
						-abs(copyBall.xVelocity);
				} else {
					copyBall.xVelocity =
						abs(copyBall.xVelocity);
				}
			}
		}

		copyBall.y = copyBall.y + copyBall.yVelocity;
		if (copyBall.y > BALL_TOUCHING_GROUND_Y_COORD ||
		    loopCounter >= INFINITE_LOOP_LIMIT) {
			return copyBall.x;
		}
		copyBall.x += copyBall.xVelocity;
		copyBall.yVelocity += 1;
	}
}

#include "wave.h"

#include <stdlib.h>

struct PikaWave *NewPikaWave()
{
	struct PikaWave *pw = malloc(sizeof(struct PikaWave));

	pw->verticalCoord = 0;
	pw->verticalCoordVelocity = 2;
	pw->yCoords = malloc(sizeof(int) * 432 / 16);
	for (int i = 0; i < 432 / 16; i++) {
		pw->yCoords[i] = 314;
	}

	return pw;
}

void DestroyPikaWave(struct PikaWave *pikaWave)
{
	free(pikaWave->yCoords);
	free(pikaWave);
}

void WaveEngine(struct PikaWave *pw)
{
	pw->verticalCoord += pw->verticalCoordVelocity;
	if (pw->verticalCoord > 32) {
		pw->verticalCoord = 32;
		pw->verticalCoordVelocity = -1;
	} else if (pw->verticalCoord < 0 && pw->verticalCoordVelocity < 0) {
		pw->verticalCoordVelocity = 2;
		pw->verticalCoord = -(rand() % 40);
	}

	for (int i = 0; i < 432 / 16; i++) {
		pw->yCoords[i] = 314 - pw->verticalCoord + (rand() % 3);
	}
}

#ifndef WAVE_H
#define WAVE_H

struct PikaWave {
	int verticalCoord;
	int verticalCoordVelocity;
	int *yCoords;
};

struct PikaWave *NewPikaWave();
void DestroyPikaWave(struct PikaWave *pikaWave);

void WaveEngine(struct PikaWave *pikaWave);

#endif /* WAVE_H */

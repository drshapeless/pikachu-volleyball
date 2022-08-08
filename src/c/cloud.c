#include "cloud.h"

#include <stdlib.h>

struct PikaCloud *NewPikaCloud()
{
	struct PikaCloud *pc = malloc(sizeof(struct PikaCloud));
	pc->topLeftPointX = -68 + (rand() % (432 + 68));
	pc->topLeftPointY = rand() % 152;
	pc->topLeftPointXVelocity = 1 + (rand() % 2);
	pc->sizeDiffTurnNumber = rand() % 11;

	return pc;
}

void DestoryPikaCloud(struct PikaCloud *pikaCloud)
{
	free(pikaCloud);
}

int PikaCloudSizeDiff(struct PikaCloud *pikaCloud)
{
	return 5 - abs(pikaCloud->sizeDiffTurnNumber - 5);
}

int PikaCloudSpriteTopLeftPointX(struct PikaCloud *pikaCloud)
{
	return pikaCloud->topLeftPointX - PikaCloudSizeDiff(pikaCloud);
}

int PikaCloudSpriteTopLeftPointY(struct PikaCloud *pikaCloud)
{
	return pikaCloud->topLeftPointY - PikaCloudSizeDiff(pikaCloud);
}

int PikaCloudSpriteWidth(struct PikaCloud *pikaCloud)
{
	return 48 + 2 * PikaCloudSizeDiff(pikaCloud);
}

int PikaCloudSpriteHeight(struct PikaCloud *pikaCloud)
{
	return 24 + 2 * PikaCloudSizeDiff(pikaCloud);
}

void CloudEngine(struct PikaCloud **arr, int cnt)
{
	for (int i = 0; i < cnt; i++) {
		arr[i]->topLeftPointX += arr[i]->topLeftPointXVelocity;
		if (arr[i]->topLeftPointX > 432) {
			arr[i]->topLeftPointX = -68;
			arr[i]->topLeftPointY = rand() % 152;
			arr[i]->topLeftPointXVelocity = 1 + (rand() % 2);
		}
		arr[i]->sizeDiffTurnNumber =
			(arr[i]->sizeDiffTurnNumber + 1) % 11;
	}
}

#ifndef CLOUD_H
#define CLOUD_H

struct PikaCloud {
	int topLeftPointX;
	int topLeftPointY;
	int topLeftPointXVelocity;
	int sizeDiffTurnNumber;
};

struct PikaCloud *NewPikaCloud();
void DestoryPikaCloud(struct PikaCloud *pikaCloud);

int PikaCloudSizeDiff(struct PikaCloud *pikaCloud);
int PikaCloudSpriteTopLeftPointX(struct PikaCloud *pikaCloud);
int PikaCloudSpriteTopLeftPointY(struct PikaCloud *pikaCloud);
int PikaCloudSpriteWidth(struct PikaCloud *pikaCloud);
int PikaCloudSpriteHeight(struct PikaCloud *pikaCloud);

void CloudEngine(struct PikaCloud **pikaCloudArray, int count);

#endif /* CLOUD_H */

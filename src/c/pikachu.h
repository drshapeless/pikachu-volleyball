#ifndef PIKACHU_H
#define PIKACHU_H

enum pikachu_state {
        NORMAL_PIKACHU,
        JUMPING_PIKACHU,
        JUMPING_AND_POWERHITTING_PIKACHU,
        DIVING_PIKACHU,
        LYING_DOWN_PIKACHU,
        WIN_PIKACHU,
        LOSE_PIKACHU,
};

struct Pikachu {
        int x;
        int y;
        int yVelocity;

        int isPlayer2;
        int isComputer;

        /* -1: left, 0: no diving, 1: right */
        int divingDirection;
        int lyingDownDurationLeft;
        int isWinner;
        int gameEnded;

        int computerWhereToStandBy;

        int isCollisionWithBallHappened;
        /**
         * Refer to pikachu_state
         * 0: normal
         * 1: jumping
         * 2: jumping_and_power_hitting
         * 3: diving
         * 4: lying_down_after_diving
         * 5: win!
         * 6: lost..
         */
        int state;
        int frameNumber;
        int normalStatusArmSwingDirection;
        int delayBeforeNextFrame;
        int computerBoldness;

        /* Sound */
        int soundPipikachu;
        int soundPika;
        int soundChu;
};

void InitializePikachu(struct Pikachu *p);
struct Pikachu *NewPikachu(int isPlayer2, int isComputer);
void DestroyPikachu(struct Pikachu *p);

#endif /* PIKACHU_H */

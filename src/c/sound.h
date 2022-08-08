#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>

struct PikaAudio {
	Mix_Music *bgm;
	Mix_Chunk *pipikachu;
	Mix_Chunk *pika;
	Mix_Chunk *chu;
	Mix_Chunk *pi;
	Mix_Chunk *pikachu;
	Mix_Chunk *powerhit;
	Mix_Chunk *ballTouchesGround;
};

struct PikaAudio *NewPikaAudio(void);
void DestroyPikaAudio(struct PikaAudio *audio);

#endif /* SOUND_H */
